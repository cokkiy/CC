use std::path::Path;

use base64::Engine;
use serde::Serialize;
use tokio::fs::{self, File, OpenOptions};
use tokio::io::{AsyncReadExt, AsyncSeekExt, AsyncWriteExt};
use tokio::sync::mpsc;
use tokio_stream::wrappers::ReceiverStream;
use tonic::Request;

use crate::control::{station_endpoints, station_label};
use crate::grpc::cc::{
    CaptureScreenRequest, DownloadRequest, Empty, ExecuteCommandRequest, PathRef,
    RenameFileRequest, SetWatchingAppRequest, UploadChunk,
    file_transfer_client::FileTransferClient, station_control_client::StationControlClient,
};
use crate::models::Station;

const FILE_CHUNK_SIZE: usize = 64 * 1024;

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

#[derive(Debug, Clone, Serialize)]
#[serde(rename_all = "camelCase")]
pub struct RemoteStationIdentity {
    pub endpoint: String,
    pub station_id: String,
    pub computer_name: String,
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

pub async fn probe_station_identity(station: &Station) -> Result<RemoteStationIdentity, String> {
    with_station_endpoint(station, move |endpoint| async move {
        let mut client = StationControlClient::connect(endpoint.clone())
            .await
            .map_err(|error| format!("connect station control via {endpoint}: {error}"))?;
        let response = client
            .get_system_state(Empty {})
            .await
            .map_err(|error| format!("get_system_state via {endpoint}: {error}"))?
            .into_inner();

        Ok(RemoteStationIdentity {
            endpoint,
            station_id: response.station_id,
            computer_name: response.computer_name,
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

pub async fn set_station_watching_apps(
    station: &Station,
    process_names: Vec<String>,
) -> Result<String, String> {
    with_station_endpoint(station, move |endpoint| {
        let process_names = process_names.clone();
        async move {
            let mut client = StationControlClient::connect(endpoint.clone())
                .await
                .map_err(|error| format!("connect station control via {endpoint}: {error}"))?;
            client
                .set_watching_app(SetWatchingAppRequest { process_names })
                .await
                .map_err(|error| format!("set watching apps via {endpoint}: {error}"))?;
            Ok(format!("Updated watched processes via {endpoint}."))
        }
    })
    .await
}
