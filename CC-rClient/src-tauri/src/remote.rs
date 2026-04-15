use std::path::Path;
use std::time::{Duration, Instant};

use base64::Engine;
use serde::Serialize;
use tokio::fs::{self, File, OpenOptions};
use tokio::io::{AsyncReadExt, AsyncSeekExt, AsyncWriteExt};
use tokio::sync::mpsc;
use tokio::time::timeout;
use tokio_stream::wrappers::ReceiverStream;
use tonic::Request;

use crate::control::{station_endpoints, station_label};
use crate::grpc::cc::{
    self, file_transfer_client::FileTransferClient, station_control_client::StationControlClient,
    telemetry_client::TelemetryClient, telemetry_client_message, telemetry_server_message,
    CaptureScreenRequest, DownloadRequest, Empty, ExecuteCommandRequest, ExecuteCommandResponse,
    PathRef, RenameFileRequest, SetStateGatheringIntervalRequest, SetWatchingAppRequest,
    TelemetryClientMessage, UploadChunk,
};
use crate::models::Station;
use tracing::{debug, error, info, warn};

const FILE_CHUNK_SIZE: usize = 64 * 1024;

#[derive(Debug, Clone, Serialize)]
#[serde(rename_all = "camelCase")]
pub struct StationRuntimeSnapshot {
    pub endpoint: String,
    pub station_id: String,
    pub computer_name: String,
    pub os_name: String,
    pub os_version: String,
    pub total_memory: i64,
    pub current_memory: i64,
    pub cpu: f32,
    pub proc_count: i32,
    pub service_version: String,
    pub app_launcher_version: String,
    pub service_path: String,
    pub app_launcher_path: String,
    pub app_states: Vec<RemoteAppState>,
    pub network_stats: Vec<RemoteInterfaceStat>,
    pub message: String,
}

#[derive(Debug, Clone, Serialize)]
#[serde(rename_all = "camelCase")]
pub struct RemoteAppState {
    pub monitor_name: String,
    pub process_name: String,
    pub process_id: i32,
    pub is_running: bool,
    pub cpu: f32,
    pub proc_count: i32,
    pub thread_count: i32,
    pub current_memory: i64,
    pub app_version: String,
    pub start_time: i64,
}

#[derive(Debug, Clone, Serialize)]
#[serde(rename_all = "camelCase")]
pub struct RemoteInterfaceStat {
    pub if_name: String,
    pub bytes_received_per_sec: f64,
    pub bytes_sented_per_sec: f64,
    pub total_bytes_per_sec: f64,
    pub bytes_received: i64,
    pub bytes_sented: i64,
    pub bytes_total: i64,
    pub unicast_packet_received: i64,
    pub unicast_packet_sented: i64,
    pub multicast_packet_received: i64,
    pub multicast_packet_sented: i64,
}

#[derive(Debug, Clone, Serialize)]
#[serde(rename_all = "camelCase")]
pub struct RemoteFileBrowserResult {
    pub endpoint: String,
    pub requested_path: String,
    pub items: Vec<RemoteFileEntry>,
}

#[derive(Debug, Clone, Serialize)]
#[serde(rename_all = "camelCase")]
pub struct RemoteFileEntry {
    pub parent: String,
    pub path: String,
    pub is_directory: bool,
    pub creation_time: String,
    pub last_modify: String,
}

#[derive(Debug, Clone, Serialize)]
#[serde(rename_all = "camelCase")]
pub struct StationScreenCapture {
    pub endpoint: String,
    pub byte_len: usize,
    pub data_url: String,
}

pub async fn fetch_station_runtime(
    station: &Station,
    interval_seconds: i32,
) -> Result<StationRuntimeSnapshot, String> {
    let monitor_processes = station
        .monitor_processes
        .iter()
        .map(|value| value.trim())
        .filter(|value| !value.is_empty())
        .map(ToOwned::to_owned)
        .collect::<Vec<_>>();
    let wait_window = Duration::from_secs(interval_seconds.max(1) as u64 * 2 + 15);

    with_station_endpoint(station, move |endpoint| {
        let monitor_processes = monitor_processes.clone();
        async move {
            let mut control = StationControlClient::connect(endpoint.clone())
                .await
                .map_err(|error| format!("connect station control via {endpoint}: {error}"))?;

            if !monitor_processes.is_empty() {
                control
                    .set_watching_app(SetWatchingAppRequest {
                        process_names: monitor_processes,
                    })
                    .await
                    .map_err(|error| format!("set watching apps via {endpoint}: {error}"))?;
            }

            if interval_seconds > 0 {
                control
                    .set_state_gathering_interval(SetStateGatheringIntervalRequest {
                        interval_seconds,
                    })
                    .await
                    .map_err(|error| format!("set telemetry interval via {endpoint}: {error}"))?;
            }

            let system_state = control
                .get_system_state(Empty {})
                .await
                .map_err(|error| format!("get system state via {endpoint}: {error}"))?
                .into_inner();
            let server_version = control
                .get_server_version(Empty {})
                .await
                .map_err(|error| format!("get server version via {endpoint}: {error}"))?
                .into_inner();
            let service_path = control
                .get_service_path(Empty {})
                .await
                .map_err(|error| format!("get service path via {endpoint}: {error}"))?
                .into_inner()
                .path;
            let app_launcher_path = control
                .get_app_launcher_path(Empty {})
                .await
                .map_err(|error| format!("get app launcher path via {endpoint}: {error}"))?
                .into_inner()
                .path;

            let mut snapshot = StationRuntimeSnapshot {
                endpoint: endpoint.clone(),
                station_id: system_state.station_id.clone(),
                computer_name: system_state.computer_name,
                os_name: system_state.os_name,
                os_version: system_state.os_version,
                total_memory: system_state.total_memory,
                current_memory: 0,
                cpu: 0.0,
                proc_count: 0,
                service_version: version_text(server_version.services_version),
                app_launcher_version: version_text(server_version.app_launcher_version),
                service_path,
                app_launcher_path,
                app_states: Vec::new(),
                network_stats: Vec::new(),
                message: format!("Connected to {}", station_label(station)),
            };

            let mut telemetry = TelemetryClient::connect(endpoint.clone())
                .await
                .map_err(|error| format!("connect telemetry via {endpoint}: {error}"))?;
            let (tx, rx) = mpsc::channel(1);
            tx.send(TelemetryClientMessage {
                body: Some(telemetry_client_message::Body::Connect(
                    cc::ConnectRequest {
                        controller_id: "CC-rClient".to_string(),
                        requested_station_id: String::new(),
                    },
                )),
            })
            .await
            .map_err(|error| format!("send telemetry connect request via {endpoint}: {error}"))?;
            // Keep tx alive until after we collect all telemetry data. Dropping it here would
            // close the inbound stream on the service side, causing the select loop to break
            // before the sleep interval fires — meaning cpu/memory/proc_count are never sent.

            let response = telemetry
                .stream_telemetry(Request::new(ReceiverStream::new(rx)))
                .await
                .map_err(|error| format!("open telemetry stream via {endpoint}: {error}"))?;
            let mut inbound = response.into_inner();

            let started = Instant::now();
            let mut have_running_state = false;
            let mut have_apps_state = false;
            let mut have_net_stats = false;

            while started.elapsed() < wait_window
                && !(have_running_state && have_apps_state && have_net_stats)
            {
                let remaining = wait_window
                    .checked_sub(started.elapsed())
                    .unwrap_or_else(|| Duration::from_secs(0));
                let next = match timeout(remaining, inbound.message()).await {
                    Err(_) => break, // wait_window elapsed — return whatever partial data we have
                    Ok(Err(error)) => {
                        // Stream closed with error. If we already received the running state
                        // return the partial snapshot rather than failing entirely.
                        if have_running_state {
                            eprintln!("telemetry stream error after running state received (returning partial data): {error}");
                            break;
                        }
                        return Err(format!(
                            "read telemetry message via {endpoint}: {error}"
                        ));
                    }
                    Ok(Ok(msg)) => msg,
                };
                let Some(message) = next else {
                    break;
                };

                match message.body {
                    Some(telemetry_server_message::Body::Accepted(ack)) => {
                        if !ack.ok {
                            return Err(format!(
                                "telemetry rejected via {endpoint}: {}",
                                ack.message
                            ));
                        }
                        snapshot.message = ack.message;
                    }
                    Some(telemetry_server_message::Body::StationRunningState(state)) => {
                        //println!("UI telemetry: cpu={:.2} mem={} proc={}", state.cpu, state.current_memory, state.proc_count);
                        snapshot.current_memory = state.current_memory;
                        snapshot.cpu = state.cpu;
                        snapshot.proc_count = state.proc_count;
                        have_running_state = true;
                    }
                    Some(telemetry_server_message::Body::AppsRunningState(state)) => {
                        snapshot.app_states = state
                            .items
                            .into_iter()
                            .map(|item| RemoteAppState {
                                monitor_name: item
                                    .process
                                    .as_ref()
                                    .map(|process| process.process_monitor_name.clone())
                                    .unwrap_or_default(),
                                process_name: item
                                    .process
                                    .as_ref()
                                    .map(|process| process.process_name.clone())
                                    .unwrap_or_default(),
                                process_id: item
                                    .process
                                    .map(|process| process.id)
                                    .unwrap_or_default(),
                                is_running: item.is_running,
                                cpu: item.cpu,
                                proc_count: item.proc_count,
                                thread_count: item.thread_count,
                                current_memory: item.current_memory,
                                app_version: item.app_version,
                                start_time: item.start_time,
                            })
                            .collect();
                        have_apps_state = true;
                    }
                    Some(telemetry_server_message::Body::NetStatistics(stats)) => {
                        debug!("Received network statistics: {} interfaces", stats.interface_statistics.len());
                        snapshot.network_stats = stats
                            .interface_statistics
                            .into_iter()
                            .map(|item| RemoteInterfaceStat {
                                if_name: item.if_name,
                                bytes_received_per_sec: item.bytes_received_per_sec as f64,
                                bytes_sented_per_sec: item.bytes_sented_per_sec as f64,
                                total_bytes_per_sec: item.total_bytes_per_sec as f64,
                                bytes_received: item.bytes_received,
                                bytes_sented: item.bytes_sented,
                                bytes_total: item.bytes_total,
                                unicast_packet_received: item.unicast_packet_received,
                                unicast_packet_sented: item.unicast_packet_sented,
                                multicast_packet_received: item.multicast_packet_received,
                                multicast_packet_sented: item.multicast_packet_sented,
                            })
                            .collect();
                        debug!("Network statistics collection completed");
                        have_net_stats = true;
                    }
                    Some(telemetry_server_message::Body::ServerVersion(version)) => {
                        snapshot.service_version = version_text(version.services_version);
                        snapshot.app_launcher_version = version_text(version.app_launcher_version);
                    }
                    Some(telemetry_server_message::Body::SystemState(_)) | None => {}
                }
            }

            Ok(snapshot)
        }
    })
    .await
}

pub async fn browse_station_files(
    station: &Station,
    path: &str,
) -> Result<RemoteFileBrowserResult, String> {
    let requested_path = path.to_string();
    with_station_endpoint(station, move |endpoint| {
        let requested_path = requested_path.clone();
        async move {
            let mut client = StationControlClient::connect(endpoint.clone())
                .await
                .map_err(|error| format!("connect station control via {endpoint}: {error}"))?;
            let response = client
                .get_file_info(PathRef {
                    name: requested_path.clone(),
                })
                .await
                .map_err(|error| format!("browse files via {endpoint}: {error}"))?
                .into_inner();

            Ok(RemoteFileBrowserResult {
                endpoint,
                requested_path,
                items: response
                    .items
                    .into_iter()
                    .map(|item| RemoteFileEntry {
                        parent: item.parent,
                        path: item.path,
                        is_directory: item.is_directory,
                        creation_time: item.creation_time,
                        last_modify: item.last_modify,
                    })
                    .collect(),
            })
        }
    })
    .await
}

pub async fn rename_station_file(
    station: &Station,
    old_path: &str,
    new_path: &str,
) -> Result<String, String> {
    let old_name = old_path.to_string();
    let new_name = new_path.to_string();
    with_station_endpoint(station, move |endpoint| {
        let old_name = old_name.clone();
        let new_name = new_name.clone();
        async move {
            let mut client = StationControlClient::connect(endpoint.clone())
                .await
                .map_err(|error| format!("connect station control via {endpoint}: {error}"))?;
            let response = client
                .rename_file(RenameFileRequest {
                    old_name: old_name.clone(),
                    new_name: new_name.clone(),
                })
                .await
                .map_err(|error| format!("rename file via {endpoint}: {error}"))?
                .into_inner();

            if response.ok {
                Ok(format!("Renamed {old_name} -> {new_name} via {endpoint}."))
            } else {
                Err(format!(
                    "The station service rejected the rename {old_name} -> {new_name} via {endpoint}."
                ))
            }
        }
    })
    .await
}

pub async fn download_station_file(
    station: &Station,
    remote_path: &str,
    local_path: &str,
) -> Result<String, String> {
    let remote_path = remote_path.to_string();
    let local_path = local_path.to_string();
    with_station_endpoint(station, move |endpoint| {
        let remote_path = remote_path.clone();
        let local_path = local_path.clone();
        async move {
            let mut client = FileTransferClient::connect(endpoint.clone())
                .await
                .map_err(|error| format!("connect file transfer via {endpoint}: {error}"))?;
            let response = client
                .download(DownloadRequest {
                    file_name: remote_path.clone(),
                    start_position: 0,
                })
                .await
                .map_err(|error| format!("download {remote_path} via {endpoint}: {error}"))?;
            let mut stream = response.into_inner();

            ensure_local_parent(&local_path).await?;
            let mut file = OpenOptions::new()
                .create(true)
                .write(true)
                .truncate(true)
                .open(&local_path)
                .await
                .map_err(|error| format!("open local download target {local_path}: {error}"))?;

            while let Some(chunk) = stream
                .message()
                .await
                .map_err(|error| format!("read download stream via {endpoint}: {error}"))?
            {
                file.seek(std::io::SeekFrom::Start(chunk.position as u64))
                    .await
                    .map_err(|error| format!("seek local file {local_path}: {error}"))?;
                if !chunk.data.is_empty() {
                    file.write_all(&chunk.data)
                        .await
                        .map_err(|error| format!("write local file {local_path}: {error}"))?;
                }
                if chunk.completed {
                    break;
                }
            }

            file.flush()
                .await
                .map_err(|error| format!("flush local file {local_path}: {error}"))?;

            Ok(format!(
                "Downloaded {remote_path} from {endpoint} to {local_path}."
            ))
        }
    })
    .await
}

pub async fn upload_station_file(
    station: &Station,
    local_path: &str,
    remote_path: &str,
) -> Result<String, String> {
    let local_path = local_path.to_string();
    let remote_path = remote_path.to_string();
    with_station_endpoint(station, move |endpoint| {
        let local_path = local_path.clone();
        let remote_path = remote_path.clone();
        async move {
            let mut client = FileTransferClient::connect(endpoint.clone())
                .await
                .map_err(|error| format!("connect file transfer via {endpoint}: {error}"))?;

            let mut file = File::open(&local_path)
                .await
                .map_err(|error| format!("open local file {local_path}: {error}"))?;
            let (tx, rx) = mpsc::channel::<UploadChunk>(4);
            let remote_file_name = remote_path.clone();

            tokio::spawn(async move {
                let mut position = 0i64;
                let mut buffer = vec![0u8; FILE_CHUNK_SIZE];

                loop {
                    let read = match file.read(&mut buffer).await {
                        Ok(read) => read,
                        Err(_) => return,
                    };

                    if read == 0 {
                        let _ = tx
                            .send(UploadChunk {
                                file_name: remote_file_name.clone(),
                                position,
                                data: Vec::new(),
                                close_after_write: true,
                            })
                            .await;
                        return;
                    }

                    let close_after_write = read < FILE_CHUNK_SIZE;
                    let chunk = UploadChunk {
                        file_name: remote_file_name.clone(),
                        position,
                        data: buffer[..read].to_vec(),
                        close_after_write,
                    };
                    if tx.send(chunk).await.is_err() {
                        return;
                    }
                    position += read as i64;
                    if close_after_write {
                        return;
                    }
                }
            });

            let response = client
                .upload(Request::new(ReceiverStream::new(rx)))
                .await
                .map_err(|error| format!("upload {local_path} via {endpoint}: {error}"))?
                .into_inner();

            if response.ok {
                Ok(format!("Uploaded {local_path} to {remote_path} via {endpoint}."))
            } else {
                Err(format!(
                    "The station service reported upload failure for {remote_path} via {endpoint}: {}",
                    response.message
                ))
            }
        }
    })
    .await
}

pub async fn capture_station_screen(station: &Station) -> Result<StationScreenCapture, String> {
    with_station_endpoint(station, move |endpoint| async move {
        let mut client = StationControlClient::connect(endpoint.clone())
            .await
            .map_err(|error| format!("connect station control via {endpoint}: {error}"))?;
        let response = client
            .capture_screen(CaptureScreenRequest { start_position: 0 })
            .await
            .map_err(|error| format!("capture screen via {endpoint}: {error}"))?;
        let mut stream = response.into_inner();
        let mut bytes = Vec::new();

        while let Some(chunk) = stream
            .message()
            .await
            .map_err(|error| format!("read capture stream via {endpoint}: {error}"))?
        {
            if !chunk.data.is_empty() {
                bytes.extend_from_slice(&chunk.data);
            }
            if chunk.completed {
                break;
            }
        }

        if bytes.is_empty() {
            return Err(format!("No screen data returned via {endpoint}."));
        }

        Ok(StationScreenCapture {
            endpoint,
            byte_len: bytes.len(),
            data_url: format!(
                "data:image/png;base64,{}",
                base64::engine::general_purpose::STANDARD.encode(bytes)
            ),
        })
    })
    .await
}

async fn with_station_endpoint<T, F, Fut>(station: &Station, mut f: F) -> Result<T, String>
where
    F: FnMut(String) -> Fut,
    Fut: std::future::Future<Output = Result<T, String>>,
{
    let endpoints = station_endpoints(station);
    if endpoints.is_empty() {
        return Err(format!(
            "No usable IP address is configured for {}.",
            station_label(station)
        ));
    }

    let mut last_error = String::new();
    for endpoint in endpoints {
        match f(endpoint.clone()).await {
            Ok(value) => return Ok(value),
            Err(error) => last_error = error,
        }
    }

    Err(last_error)
}

async fn ensure_local_parent(path: &str) -> Result<(), String> {
    let Some(parent) = Path::new(path).parent() else {
        return Ok(());
    };
    fs::create_dir_all(parent)
        .await
        .map_err(|error| format!("create directory {}: {error}", parent.display()))
}

fn version_text(version: Option<cc::VersionInfo>) -> String {
    version
        .map(|value| value.product_version)
        .filter(|value| !value.trim().is_empty())
        .unwrap_or_default()
}

#[derive(Debug, Clone, Serialize)]
#[serde(rename_all = "camelCase")]
pub struct CommandExecutionResult {
    pub exit_code: i32,
    pub stdout: String,
    pub stderr: String,
}

pub async fn execute_station_command(
    station: &Station,
    command: &str,
    timeout_seconds: i32,
) -> Result<CommandExecutionResult, String> {
    with_station_endpoint(station, move |endpoint| {
        let command = command.to_string();
        async move {
            let mut client = StationControlClient::connect(endpoint.clone())
                .await
                .map_err(|error| format!("connect station control via {endpoint}: {error}"))?;
            let response = client
                .execute_command(ExecuteCommandRequest {
                    command,
                    timeout_seconds,
                })
                .await
                .map_err(|error| format!("execute_command RPC via {endpoint}: {error}"))?
                .into_inner();

            Ok(CommandExecutionResult {
                exit_code: response.exit_code,
                stdout: response.stdout,
                stderr: response.stderr,
            })
        }
    })
    .await
}
