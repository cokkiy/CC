use std::io::SeekFrom;
use std::path::{Path, PathBuf};
use std::pin::Pin;
use std::sync::Arc;
use std::time::Duration;

use anyhow::{Context, Result};
use async_stream::try_stream;
use futures_util::Stream;
use tokio::fs::{self, File, OpenOptions};
use tokio::io::{AsyncReadExt, AsyncSeekExt, AsyncWriteExt};
use tokio::sync::watch;
use tonic::metadata::MetadataValue;
use tonic::transport::Server;
use tonic::{Request, Response, Status, Streaming};
use tracing::{info, warn};

use crate::config::AppConfig;
use crate::grpc::agent::desktop_agent_client::DesktopAgentClient;
use crate::grpc::cc::{
    Ack, AppControlResult, AppStartParameter, AppStartingResult, CaptureScreenChunk,
    CaptureScreenRequest, CloseAppRequest, CloseAppResponse, DownloadChunk, DownloadRequest, Empty,
    GetAllProcessInfoResponse, GetAppLauncherPathResponse, GetConnectionInformationsResponse,
    GetFileInfoResponse, GetNetworkInterfacesResponse, GetServicePathResponse,
    GetTcpListenerInfosResponse, GetUdpListenerInfosResponse, OpenSpecPageRequest, PathRef,
    RebootRequest, RenameFileRequest, RenameFileResponse, RestartAppRequest, RestartAppResponse,
    SendRemoteCtrlCommandRequest, ServerVersionInfo, SetStateGatheringIntervalRequest,
    SetWatchingAppRequest, SetWeatherPictureDownloadOptionRequest, ShutdownRequest,
    StartAppRequest, StartAppResponse, SwitchDisplayPageAndModeRequest, TelemetryClientMessage,
    TelemetryServerMessage, UploadChunk, UploadResult, file_transfer_server::FileTransfer,
    file_transfer_server::FileTransferServer, station_control_server::StationControl,
    station_control_server::StationControlServer, telemetry_client_message,
    telemetry_server::Telemetry, telemetry_server::TelemetryServer, telemetry_server_message,
};
use crate::platform;
use crate::state::{AppState, find_process_ids_by_name, terminate_process};

pub async fn run(
    config_path: Option<PathBuf>,
    mut shutdown: watch::Receiver<bool>,
    console_telemetry: bool,
) -> Result<()> {
    let config = AppConfig::load(config_path.as_deref())?;
    let service_path = std::env::current_exe().context("resolve current executable")?;
    let state = Arc::new(AppState::new(config, service_path)?);
    let listen_addr = state.listen_addr()?;
    if !state.agent_target().ip().is_loopback() {
        anyhow::bail!(
            "desktop agent target must stay on loopback, got {}",
            state.agent_target()
        );
    }

    info!(
        station_id = state.station_id(),
        listen_addr = %listen_addr,
        watched_processes = state.watched_processes().len(),
        console_telemetry,
        "starting CC-rStationService"
    );

    if console_telemetry {
        tokio::spawn(console_telemetry_task(Arc::clone(&state)));
    }

    Server::builder()
        .add_service(StationControlServer::new(StationControlService {
            state: Arc::clone(&state),
        }))
        .add_service(FileTransferServer::new(FileTransferService {
            _state: Arc::clone(&state),
        }))
        .add_service(TelemetryServer::new(TelemetryRpc {
            state: Arc::clone(&state),
        }))
        .serve_with_shutdown(listen_addr, async move {
            let _ = shutdown.changed().await;
        })
        .await
        .context("run gRPC server")
}

#[derive(Clone)]
struct StationControlService {
    state: Arc<AppState>,
}

#[tonic::async_trait]
impl StationControl for StationControlService {
    type CaptureScreenStream =
        Pin<Box<dyn Stream<Item = Result<CaptureScreenChunk, Status>> + Send>>;

    async fn start_app(
        &self,
        request: Request<StartAppRequest>,
    ) -> Result<Response<StartAppResponse>, Status> {
        let mut results = Vec::new();
        for app in request.into_inner().apps {
            results.push(start_one_app(app).await);
        }

        Ok(Response::new(StartAppResponse { results }))
    }

    async fn close_app(
        &self,
        request: Request<CloseAppRequest>,
    ) -> Result<Response<CloseAppResponse>, Status> {
        let mut results = Vec::new();
        for pid in request.into_inner().process_ids {
            let result = match terminate_process(pid).map_err(status_from_error)? {
                true => AppControlResult::Closed,
                false => AppControlResult::NotRunning,
            };
            results.push(result as i32);
        }

        Ok(Response::new(CloseAppResponse { results }))
    }

    async fn restart_app(
        &self,
        request: Request<RestartAppRequest>,
    ) -> Result<Response<RestartAppResponse>, Status> {
        let mut results = Vec::new();
        for app in request.into_inner().apps {
            for pid in find_process_ids_by_name(&effective_process_name(&app)) {
                if let Err(error) = terminate_process(pid) {
                    warn!(pid, error = %error, "failed to terminate process during restart");
                }
            }

            results.push(start_one_app(app).await);
        }

        Ok(Response::new(RestartAppResponse { results }))
    }

    async fn reboot(&self, request: Request<RebootRequest>) -> Result<Response<Empty>, Status> {
        platform::reboot(request.into_inner().force).map_err(status_from_error)?;
        Ok(Response::new(Empty {}))
    }

    async fn shutdown(
        &self,
        _request: Request<ShutdownRequest>,
    ) -> Result<Response<Empty>, Status> {
        platform::shutdown().map_err(status_from_error)?;
        Ok(Response::new(Empty {}))
    }

    async fn get_system_state(
        &self,
        _request: Request<Empty>,
    ) -> Result<Response<crate::grpc::cc::StationSystemState>, Status> {
        Ok(Response::new(self.state.system_state()))
    }

    async fn set_watching_app(
        &self,
        request: Request<SetWatchingAppRequest>,
    ) -> Result<Response<Empty>, Status> {
        self.state
            .set_watched_processes(request.into_inner().process_names);
        Ok(Response::new(Empty {}))
    }

    async fn set_state_gathering_interval(
        &self,
        request: Request<SetStateGatheringIntervalRequest>,
    ) -> Result<Response<Empty>, Status> {
        let interval = request.into_inner().interval_seconds;
        if interval <= 0 {
            return Err(Status::invalid_argument(
                "interval_seconds must be positive",
            ));
        }

        self.state.set_interval_seconds(interval as u64);
        Ok(Response::new(Empty {}))
    }

    async fn set_weather_picture_download_option(
        &self,
        _request: Request<SetWeatherPictureDownloadOptionRequest>,
    ) -> Result<Response<Empty>, Status> {
        Ok(Response::new(Empty {}))
    }

    async fn capture_screen(
        &self,
        request: Request<CaptureScreenRequest>,
    ) -> Result<Response<Self::CaptureScreenStream>, Status> {
        let start_position = request.into_inner().start_position;
        let endpoint = format!("http://{}", self.state.agent_target());
        let auth_token = self.state.agent_auth_token().to_string();
        let display_index = self.state.preferred_display_index() as i32;

        let output = try_stream! {
            let mut client = DesktopAgentClient::connect(endpoint.clone())
                .await
                .map_err(|error| Status::unavailable(format!("connect desktop agent at {endpoint}: {error}")))?;
            let mut request = Request::new(crate::grpc::agent::CaptureRequest {
                start_position,
                display_index,
                force_refresh: start_position == 0,
            });
            let token_metadata = MetadataValue::try_from(auth_token.as_str())
                .map_err(|_| Status::internal("invalid desktop agent auth token"))?;
            request.metadata_mut().insert("x-cc-agent-token", token_metadata);
            let response = client
                .capture_screen(request)
                .await
                .map_err(|error| Status::unavailable(format!("capture screen via desktop agent: {error}")))?;
            let mut stream = response.into_inner();

            while let Some(chunk) = stream.message().await? {
                yield CaptureScreenChunk {
                    position: chunk.position,
                    length: chunk.length,
                    data: chunk.data,
                    completed: chunk.completed,
                };
            }
        };

        Ok(Response::new(Box::pin(output)))
    }

    async fn get_file_info(
        &self,
        request: Request<PathRef>,
    ) -> Result<Response<GetFileInfoResponse>, Status> {
        let response = self
            .state
            .file_info(&request.into_inner().name)
            .map_err(status_from_error)?;
        Ok(Response::new(response))
    }

    async fn get_all_process_info(
        &self,
        _request: Request<Empty>,
    ) -> Result<Response<GetAllProcessInfoResponse>, Status> {
        Ok(Response::new(self.state.all_process_info()))
    }

    async fn get_server_version(
        &self,
        _request: Request<Empty>,
    ) -> Result<Response<ServerVersionInfo>, Status> {
        Ok(Response::new(self.state.server_version()))
    }

    async fn switch_display_page_and_mode(
        &self,
        request: Request<SwitchDisplayPageAndModeRequest>,
    ) -> Result<Response<Empty>, Status> {
        let command = request.into_inner().command;
        platform::send_udp_command(self.state.udp_target(), command, &[])
            .map_err(status_from_error)?;
        Ok(Response::new(Empty {}))
    }

    async fn open_spec_page(
        &self,
        request: Request<OpenSpecPageRequest>,
    ) -> Result<Response<Empty>, Status> {
        let page_num = request.into_inner().page_num;
        platform::send_udp_command(self.state.udp_target(), 10, &page_num.to_le_bytes())
            .map_err(status_from_error)?;
        Ok(Response::new(Empty {}))
    }

    async fn send_remote_ctrl_command(
        &self,
        request: Request<SendRemoteCtrlCommandRequest>,
    ) -> Result<Response<Empty>, Status> {
        let request = request.into_inner();
        platform::send_udp_command(
            self.state.udp_target(),
            request.command_code,
            &request.control_params,
        )
        .map_err(status_from_error)?;
        Ok(Response::new(Empty {}))
    }

    async fn get_service_path(
        &self,
        _request: Request<Empty>,
    ) -> Result<Response<GetServicePathResponse>, Status> {
        Ok(Response::new(GetServicePathResponse {
            path: self.state.service_path().display().to_string(),
        }))
    }

    async fn get_app_launcher_path(
        &self,
        _request: Request<Empty>,
    ) -> Result<Response<GetAppLauncherPathResponse>, Status> {
        Ok(Response::new(GetAppLauncherPathResponse {
            path: self.state.launcher_proxy_path().to_string(),
        }))
    }

    async fn rename_file(
        &self,
        request: Request<RenameFileRequest>,
    ) -> Result<Response<RenameFileResponse>, Status> {
        let request = request.into_inner();
        let old_path = PathBuf::from(&request.old_name);
        let new_path = PathBuf::from(&request.new_name);

        if !old_path.exists() {
            return Ok(Response::new(RenameFileResponse { ok: false }));
        }

        if new_path.exists() {
            remove_existing_path(&new_path)
                .await
                .map_err(status_from_error)?;
        }

        fs::rename(&old_path, &new_path)
            .await
            .with_context(|| format!("rename {} -> {}", old_path.display(), new_path.display()))
            .map_err(status_from_error)?;

        Ok(Response::new(RenameFileResponse { ok: true }))
    }

    async fn get_network_interfaces(
        &self,
        _request: Request<Empty>,
    ) -> Result<Response<GetNetworkInterfacesResponse>, Status> {
        Ok(Response::new(self.state.network_interfaces()))
    }

    async fn get_connection_informations(
        &self,
        _request: Request<Empty>,
    ) -> Result<Response<GetConnectionInformationsResponse>, Status> {
        let response = self.state.connection_infos().map_err(status_from_error)?;
        Ok(Response::new(response))
    }

    async fn get_tcp_listener_infos(
        &self,
        _request: Request<Empty>,
    ) -> Result<Response<GetTcpListenerInfosResponse>, Status> {
        let response = self.state.tcp_listener_infos().map_err(status_from_error)?;
        Ok(Response::new(response))
    }

    async fn get_udp_listener_infos(
        &self,
        _request: Request<Empty>,
    ) -> Result<Response<GetUdpListenerInfosResponse>, Status> {
        let response = self.state.udp_listener_infos().map_err(status_from_error)?;
        Ok(Response::new(response))
    }
}

#[derive(Clone)]
struct FileTransferService {
    _state: Arc<AppState>,
}

#[tonic::async_trait]
impl FileTransfer for FileTransferService {
    type DownloadStream = Pin<Box<dyn Stream<Item = Result<DownloadChunk, Status>> + Send>>;

    async fn upload(
        &self,
        request: Request<Streaming<UploadChunk>>,
    ) -> Result<Response<UploadResult>, Status> {
        let mut stream = request.into_inner();
        let mut current_path: Option<PathBuf> = None;
        let mut current_file: Option<File> = None;

        while let Some(chunk) = stream.message().await? {
            if chunk.file_name.trim().is_empty() {
                return Err(Status::invalid_argument("file_name is required"));
            }

            let path = PathBuf::from(&chunk.file_name);
            if current_path.as_ref() != Some(&path) || current_file.is_none() {
                if let Some(parent) = path.parent() {
                    fs::create_dir_all(parent)
                        .await
                        .with_context(|| format!("create directory {}", parent.display()))
                        .map_err(status_from_error)?;
                }

                current_file = Some(
                    OpenOptions::new()
                        .create(true)
                        .read(true)
                        .write(true)
                        .open(&path)
                        .await
                        .with_context(|| format!("open upload file {}", path.display()))
                        .map_err(status_from_error)?,
                );
                current_path = Some(path.clone());
            }

            let Some(file) = current_file.as_mut() else {
                return Err(Status::internal("upload stream lost its active file"));
            };

            file.seek(SeekFrom::Start(chunk.position as u64))
                .await
                .with_context(|| format!("seek upload file {}", path.display()))
                .map_err(status_from_error)?;
            file.write_all(&chunk.data)
                .await
                .with_context(|| format!("write upload file {}", path.display()))
                .map_err(status_from_error)?;

            if chunk.close_after_write {
                finalize_upload(current_path.take(), current_file.take()).await?;
            }
        }

        finalize_upload(current_path, current_file).await?;

        Ok(Response::new(UploadResult {
            ok: true,
            message: "upload completed".to_string(),
        }))
    }

    async fn download(
        &self,
        request: Request<DownloadRequest>,
    ) -> Result<Response<Self::DownloadStream>, Status> {
        let request = request.into_inner();
        let path = PathBuf::from(&request.file_name);
        let metadata = fs::metadata(&path)
            .await
            .with_context(|| format!("read metadata {}", path.display()))
            .map_err(status_from_error)?;
        let total_length = metadata.len();
        let start_position = request.start_position.max(0) as u64;

        let output = try_stream! {
            let mut file = File::open(&path)
                .await
                .with_context(|| format!("open download file {}", path.display()))
                .map_err(status_from_error)?;
            file.seek(SeekFrom::Start(start_position))
                .await
                .with_context(|| format!("seek download file {}", path.display()))
                .map_err(status_from_error)?;

            let mut position = start_position;
            loop {
                let mut buffer = vec![0u8; 64 * 1024];
                let read = file.read(&mut buffer)
                    .await
                    .with_context(|| format!("read download file {}", path.display()))
                    .map_err(status_from_error)?;

                if read == 0 {
                    yield DownloadChunk {
                        position: position as i64,
                        data: Vec::new(),
                        length: 0,
                        completed: true,
                    };
                    break;
                }

                buffer.truncate(read);
                position += read as u64;
                let completed = position >= total_length;

                yield DownloadChunk {
                    position: (position - read as u64) as i64,
                    data: buffer,
                    length: read as i32,
                    completed,
                };

                if completed {
                    break;
                }
            }
        };

        Ok(Response::new(Box::pin(output)))
    }
}

#[derive(Clone)]
struct TelemetryRpc {
    state: Arc<AppState>,
}

#[tonic::async_trait]
impl Telemetry for TelemetryRpc {
    type StreamTelemetryStream =
        Pin<Box<dyn Stream<Item = Result<TelemetryServerMessage, Status>> + Send>>;

    async fn stream_telemetry(
        &self,
        request: Request<Streaming<TelemetryClientMessage>>,
    ) -> Result<Response<Self::StreamTelemetryStream>, Status> {
        let mut inbound = request.into_inner();
        let state = Arc::clone(&self.state);

        let output = try_stream! {
            let mut controller_id = "unknown".to_string();

            if let Some(message) = inbound.message().await? {
                if let Some(body) = message.body {
                    match body {
                        telemetry_client_message::Body::Connect(connect) => {
                            if !connect.requested_station_id.is_empty()
                                && connect.requested_station_id != state.station_id()
                            {
                                yield telemetry_message(telemetry_server_message::Body::Accepted(Ack {
                                    ok: false,
                                    message: format!(
                                        "station mismatch: requested {}, current {}",
                                        connect.requested_station_id,
                                        state.station_id()
                                    ),
                                }));
                                return;
                            }

                            controller_id = connect.controller_id;
                        }
                        telemetry_client_message::Body::Ping(_) | telemetry_client_message::Body::Ack(_) => {}
                    }
                }
            }

            yield telemetry_message(telemetry_server_message::Body::Accepted(Ack {
                ok: true,
                message: format!("connected to station {} for controller {}", state.station_id(), controller_id),
            }));
            yield telemetry_message(telemetry_server_message::Body::SystemState(state.system_state()));
            yield telemetry_message(telemetry_server_message::Body::ServerVersion(state.server_version()));

            loop {
                let sleep = tokio::time::sleep(Duration::from_secs(state.interval_seconds()));
                tokio::pin!(sleep);

                tokio::select! {
                    inbound_message = inbound.message() => {
                        match inbound_message {
                            Ok(Some(message)) => {
                                if let Some(body) = message.body {
                                    match body {
                                        telemetry_client_message::Body::Connect(connect) => {
                                            if !connect.requested_station_id.is_empty()
                                                && connect.requested_station_id != state.station_id()
                                            {
                                                yield telemetry_message(telemetry_server_message::Body::Accepted(Ack {
                                                    ok: false,
                                                    message: format!(
                                                        "station mismatch: requested {}, current {}",
                                                        connect.requested_station_id,
                                                        state.station_id()
                                                    ),
                                                }));
                                                break;
                                            }

                                            controller_id = connect.controller_id;
                                            yield telemetry_message(telemetry_server_message::Body::Accepted(Ack {
                                                ok: true,
                                                message: format!("controller switched to {}", controller_id),
                                            }));
                                        }
                                        telemetry_client_message::Body::Ping(ping) => {
                                            yield telemetry_message(telemetry_server_message::Body::Accepted(Ack {
                                                ok: true,
                                                message: format!("pong {}", ping.unix_millis),
                                            }));
                                        }
                                        telemetry_client_message::Body::Ack(_) => {}
                                    }
                                }
                            }
                            Ok(None) => break,
                            Err(error) => {
                                warn!(error = %error, "telemetry client stream closed with error");
                                break;
                            }
                        }
                    }
                    _ = &mut sleep => {
                        let (running, apps) = state.running_and_apps_state().await;
                        yield telemetry_message(telemetry_server_message::Body::StationRunningState(running));
                        yield telemetry_message(telemetry_server_message::Body::AppsRunningState(apps));
                        yield telemetry_message(telemetry_server_message::Body::NetStatistics(state.network_statistics()));
                    }
                }
            }
        };

        Ok(Response::new(Box::pin(output)))
    }
}

async fn start_one_app(app: AppStartParameter) -> AppStartingResult {
    let process_name = effective_process_name(&app);
    if process_name.is_empty() || app.app_path.trim().is_empty() {
        return AppStartingResult {
            param_id: app.param_id,
            process_id: 0,
            process_name,
            control_result: AppControlResult::FailToStart as i32,
            result: "app_path is required".to_string(),
        };
    }

    if !app.allow_multi_instance {
        if let Some(existing) = find_process_ids_by_name(&process_name).into_iter().next() {
            return AppStartingResult {
                param_id: app.param_id,
                process_id: existing,
                process_name,
                control_result: AppControlResult::AlreadyRunning as i32,
                result: "process already running".to_string(),
            };
        }
    }

    let mut command = tokio::process::Command::new(&app.app_path);
    if let Some(parent) = Path::new(&app.app_path).parent() {
        command.current_dir(parent);
    }

    if !app.arguments.trim().is_empty() {
        let Some(arguments) = shlex::split(&app.arguments) else {
            return AppStartingResult {
                param_id: app.param_id,
                process_id: 0,
                process_name,
                control_result: AppControlResult::FailToStart as i32,
                result: "failed to parse arguments".to_string(),
            };
        };
        command.args(arguments);
    }

    match command.spawn() {
        Ok(child) => AppStartingResult {
            param_id: app.param_id,
            process_id: child.id().unwrap_or_default() as i32,
            process_name,
            control_result: AppControlResult::Started as i32,
            result: "started".to_string(),
        },
        Err(error) => AppStartingResult {
            param_id: app.param_id,
            process_id: 0,
            process_name,
            control_result: AppControlResult::FailToStart as i32,
            result: error.to_string(),
        },
    }
}

fn effective_process_name(app: &AppStartParameter) -> String {
    if !app.process_name.trim().is_empty() {
        return app.process_name.trim().to_string();
    }

    Path::new(&app.app_path)
        .file_stem()
        .and_then(|value| value.to_str())
        .unwrap_or_default()
        .to_string()
}

async fn finalize_upload(path: Option<PathBuf>, file: Option<File>) -> Result<(), Status> {
    if let Some(mut file) = file {
        file.flush()
            .await
            .context("flush upload file")
            .map_err(status_from_error)?;
        file.sync_all()
            .await
            .context("sync upload file")
            .map_err(status_from_error)?;
    }

    if let Some(path) = path {
        #[cfg(unix)]
        {
            use std::os::unix::fs::PermissionsExt;

            let mode = if looks_executable(&path).await.unwrap_or(false) {
                0o777
            } else {
                0o666
            };

            fs::set_permissions(&path, std::fs::Permissions::from_mode(mode))
                .await
                .with_context(|| format!("set permissions {}", path.display()))
                .map_err(status_from_error)?;
        }
    }

    Ok(())
}

async fn looks_executable(path: &Path) -> Result<bool> {
    let extension = path
        .extension()
        .and_then(|value| value.to_str())
        .unwrap_or_default();
    if matches!(extension, "sh" | "py" | "pl" | "run") {
        return Ok(true);
    }

    let mut file = File::open(path)
        .await
        .with_context(|| format!("open file for executable check {}", path.display()))?;
    let mut buffer = [0u8; 4];
    let read = file
        .read(&mut buffer)
        .await
        .with_context(|| format!("read file for executable check {}", path.display()))?;

    Ok((read >= 2 && &buffer[..2] == b"#!")
        || (read >= 4 && buffer == [0x7f, b'E', b'L', b'F'])
        || (read >= 2 && &buffer[..2] == b"MZ"))
}

async fn remove_existing_path(path: &Path) -> Result<()> {
    let metadata = fs::metadata(path)
        .await
        .with_context(|| format!("read metadata {}", path.display()))?;

    if metadata.is_dir() {
        fs::remove_dir_all(path)
            .await
            .with_context(|| format!("remove directory {}", path.display()))?;
    } else {
        fs::remove_file(path)
            .await
            .with_context(|| format!("remove file {}", path.display()))?;
    }

    Ok(())
}

fn telemetry_message(body: telemetry_server_message::Body) -> TelemetryServerMessage {
    TelemetryServerMessage {
        message_id: uuid::Uuid::new_v4().to_string(),
        body: Some(body),
    }
}

async fn console_telemetry_task(state: Arc<AppState>) {
    let mut ticker = tokio::time::interval(Duration::from_secs(state.interval_seconds()));
    ticker.set_missed_tick_behavior(tokio::time::MissedTickBehavior::Skip);

    loop {
        ticker.tick().await;

        let (running, apps) = state.running_and_apps_state().await;
        let network = state.network_statistics();

        let watched = apps
            .items
            .iter()
            .map(|item| {
                let process_name = item
                    .process
                    .as_ref()
                    .map(|process| process.process_monitor_name.as_str())
                    .unwrap_or("<unknown>");
                format!(
                    "{}:running={},proc_count={},cpu={:.2},memory={}",
                    process_name, item.is_running, item.proc_count, item.cpu, item.current_memory
                )
            })
            .collect::<Vec<_>>()
            .join(" | ");

        let network_summary = network
            .interface_statistics
            .iter()
            .map(|item| {
                format!(
                    "{} rx/s={:.0} tx/s={:.0}",
                    item.if_name, item.bytes_received_per_sec, item.bytes_sented_per_sec
                )
            })
            .collect::<Vec<_>>()
            .join(" | ");

        println!(
            "[telemetry] station={} cpu={:.2}% memory={} proc_count={} tcp_connections={} udp_listeners={} watched=[{}] net=[{}]",
            running.station_id,
            running.cpu,
            running.current_memory,
            running.proc_count,
            network.current_connections,
            network.udp_listeners,
            watched,
            network_summary
        );

        let next_seconds = state.interval_seconds().max(1);
        ticker = tokio::time::interval(Duration::from_secs(next_seconds));
        ticker.set_missed_tick_behavior(tokio::time::MissedTickBehavior::Skip);
    }
}

fn status_from_error(error: anyhow::Error) -> Status {
    Status::internal(error.to_string())
}
