use std::path::PathBuf;

use anyhow::{Result, anyhow};
use cc_rstationservice::{app, platform};
use tokio::runtime::Builder;
use tokio::sync::watch;
#[cfg(windows)]
use tracing::{debug, error};
use tracing_subscriber::EnvFilter;

fn main() -> Result<()> {
    init_tracing();
    let cli = Cli::parse()?;

    #[cfg(windows)]
    if matches!(cli.mode, RunMode::Auto) {
        if try_run_windows_service()? {
            return Ok(());
        }
    }

    #[cfg(unix)]
    if matches!(cli.mode, RunMode::Auto | RunMode::Daemon) {
        platform::daemonize()?;
    }

    run_foreground(cli.config_path)
}

fn init_tracing() {
    let _ = tracing_subscriber::fmt()
        .with_env_filter(EnvFilter::from_default_env())
        .try_init();
}

fn run_foreground(config_path: Option<PathBuf>) -> Result<()> {
    let runtime = Builder::new_multi_thread()
        .enable_all()
        .build()
        .map_err(|error| anyhow!("create tokio runtime: {error}"))?;

    runtime.block_on(async move {
        let (shutdown_tx, shutdown_rx) = watch::channel(false);
        let signal_task = tokio::spawn(async move {
            wait_for_shutdown_signal().await;
            let _ = shutdown_tx.send(true);
        });

        let result = app::run(config_path, shutdown_rx).await;
        signal_task.abort();
        result
    })
}

async fn wait_for_shutdown_signal() {
    #[cfg(unix)]
    {
        use tokio::signal::unix::{SignalKind, signal};

        let mut terminate = signal(SignalKind::terminate())
            .unwrap_or_else(|_| panic!("failed to register SIGTERM handler"));
        tokio::select! {
            _ = tokio::signal::ctrl_c() => {}
            _ = terminate.recv() => {}
        }
    }

    #[cfg(not(unix))]
    {
        let _ = tokio::signal::ctrl_c().await;
    }
}

#[derive(Clone, Copy, Debug, Eq, PartialEq)]
enum RunMode {
    Auto,
    Foreground,
    Daemon,
}

#[derive(Debug)]
struct Cli {
    mode: RunMode,
    config_path: Option<PathBuf>,
}

impl Cli {
    fn parse() -> Result<Self> {
        let mut args = std::env::args().skip(1);
        let mut mode = RunMode::Auto;
        let mut config_path = None;

        while let Some(arg) = args.next() {
            match arg.as_str() {
                "foreground" | "--foreground" => mode = RunMode::Foreground,
                "daemon" | "--daemon" => mode = RunMode::Daemon,
                "--config" => {
                    let Some(path) = args.next() else {
                        return Err(anyhow!("--config requires a path"));
                    };
                    config_path = Some(PathBuf::from(path));
                }
                other => return Err(anyhow!("unknown argument: {other}")),
            }
        }

        Ok(Self { mode, config_path })
    }
}

#[cfg(windows)]
const WINDOWS_SERVICE_NAME: &str = "CC-rStationService";

#[cfg(windows)]
windows_service::define_windows_service!(ffi_service_main, windows_service_main);

#[cfg(windows)]
fn try_run_windows_service() -> Result<bool> {
    use windows_service::service_dispatcher;

    match service_dispatcher::start(WINDOWS_SERVICE_NAME, ffi_service_main) {
        Ok(()) => Ok(true),
        Err(error) => {
            debug!(error = %error, "service dispatcher not available, falling back to foreground mode");
            Ok(false)
        }
    }
}

#[cfg(windows)]
fn windows_service_main(_arguments: Vec<std::ffi::OsString>) {
    if let Err(error) = run_windows_service() {
        error!(error = %error, "windows service terminated with error");
    }
}

#[cfg(windows)]
fn run_windows_service() -> Result<()> {
    use windows_service::service::{
        ServiceControl, ServiceControlAccept, ServiceExitCode, ServiceState, ServiceStatus,
        ServiceType,
    };
    use windows_service::service_control_handler::{
        self, ServiceControlHandlerResult, ServiceStatusHandle,
    };

    let (service_stop_tx, service_stop_rx) = std::sync::mpsc::channel::<()>();
    let status_handle =
        service_control_handler::register(WINDOWS_SERVICE_NAME, move |control_event| {
            match control_event {
                ServiceControl::Stop | ServiceControl::Shutdown => {
                    let _ = service_stop_tx.send(());
                    ServiceControlHandlerResult::NoError
                }
                ServiceControl::Interrogate => ServiceControlHandlerResult::NoError,
                _ => ServiceControlHandlerResult::NotImplemented,
            }
        })?;

    set_service_status(&status_handle, ServiceState::StartPending)?;

    let runtime = Builder::new_multi_thread()
        .enable_all()
        .build()
        .map_err(|error| anyhow!("create tokio runtime for service: {error}"))?;
    let (shutdown_tx, shutdown_rx) = watch::channel(false);

    std::thread::spawn(move || {
        let _ = service_stop_rx.recv();
        let _ = shutdown_tx.send(true);
    });

    set_service_status(&status_handle, ServiceState::Running)?;
    let result = runtime.block_on(app::run(None, shutdown_rx));
    set_service_status(&status_handle, ServiceState::Stopped)?;
    result
}

#[cfg(windows)]
fn set_service_status(
    status_handle: &windows_service::service_control_handler::ServiceStatusHandle,
    current_state: windows_service::service::ServiceState,
) -> windows_service::Result<()> {
    use windows_service::service::{
        ServiceControlAccept, ServiceExitCode, ServiceState, ServiceStatus, ServiceType,
    };

    let controls_accepted = if current_state == ServiceState::Running {
        ServiceControlAccept::STOP | ServiceControlAccept::SHUTDOWN
    } else {
        ServiceControlAccept::empty()
    };

    status_handle.set_service_status(ServiceStatus {
        service_type: ServiceType::OWN_PROCESS,
        current_state,
        controls_accepted,
        exit_code: ServiceExitCode::Win32(0),
        checkpoint: 0,
        wait_hint: std::time::Duration::default(),
        process_id: None,
    })
}
