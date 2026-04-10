use std::io::Cursor;
use std::net::SocketAddr;
use std::pin::Pin;
#[cfg(target_os = "linux")]
use std::process::Command;
use std::sync::{Arc, Mutex};

use anyhow::{Context, Result, anyhow};
use async_stream::try_stream;
use futures_util::Stream;
use screenshots::Screen;
use screenshots::image::codecs::png::PngEncoder;
use screenshots::image::{self, ColorType, ImageEncoder, RgbaImage};
use tokio::sync::watch;
use tonic::transport::Server;
use tonic::{Request, Response, Status};
use tracing::{info, warn};

use crate::config::AppConfig;
use crate::grpc::agent::{
    CaptureChunk, CaptureRequest, Empty, PingReply, desktop_agent_server::DesktopAgent,
    desktop_agent_server::DesktopAgentServer,
};

pub async fn run(
    config_path: Option<std::path::PathBuf>,
    mut shutdown: watch::Receiver<bool>,
) -> Result<()> {
    let config = AppConfig::load(config_path.as_deref())?;
    let listen_addr: SocketAddr = config.agent.listen_addr.parse().with_context(|| {
        format!(
            "parse desktop agent listen addr {}",
            config.agent.listen_addr
        )
    })?;
    if !listen_addr.ip().is_loopback() {
        anyhow::bail!(
            "desktop agent must bind to loopback, got {}",
            listen_addr.ip()
        );
    }
    let service_name = config.service.service_name.clone();
    let auth_token = config.agent.auth_token.clone();
    let preferred_display_index = config.agent.preferred_display_index;
    let displays = describe_displays();

    info!(
        listen_addr = %listen_addr,
        preferred_display_index,
        display_count = displays.len(),
        "starting desktop agent"
    );
    for display_description in displays {
        info!(display_description = %display_description, "desktop agent display");
    }

    Server::builder()
        .add_service(DesktopAgentServer::new(DesktopAgentService::new(
            auth_token,
            preferred_display_index,
        )))
        .serve_with_shutdown(listen_addr, async move {
            let _ = shutdown.changed().await;
        })
        .await
        .with_context(|| format!("run desktop agent for {service_name}"))
}

#[derive(Clone)]
struct DesktopAgentService {
    auth_token: Arc<str>,
    preferred_display_index: u32,
    cache: Arc<Mutex<Option<Vec<u8>>>>,
}

impl DesktopAgentService {
    fn new(auth_token: String, preferred_display_index: u32) -> Self {
        Self {
            auth_token: Arc::from(auth_token),
            preferred_display_index,
            cache: Arc::new(Mutex::new(None)),
        }
    }

    fn authorize(&self, request: &Request<CaptureRequest>) -> Result<(), Status> {
        let Some(token) = request.metadata().get("x-cc-agent-token") else {
            return Err(Status::unauthenticated("missing desktop agent token"));
        };
        let token = token
            .to_str()
            .map_err(|_| Status::unauthenticated("invalid desktop agent token encoding"))?;

        if token != self.auth_token.as_ref() {
            return Err(Status::permission_denied("desktop agent token mismatch"));
        }

        Ok(())
    }
}

#[tonic::async_trait]
impl DesktopAgent for DesktopAgentService {
    type CaptureScreenStream = Pin<Box<dyn Stream<Item = Result<CaptureChunk, Status>> + Send>>;

    async fn capture_screen(
        &self,
        request: Request<CaptureRequest>,
    ) -> Result<Response<Self::CaptureScreenStream>, Status> {
        self.authorize(&request)?;
        let request = request.into_inner();
        let start_position = request.start_position.max(0) as usize;
        let force_refresh = request.force_refresh;
        let display_index = if request.display_index >= 0 {
            request.display_index as u32
        } else {
            self.preferred_display_index
        };
        let cache = Arc::clone(&self.cache);

        let bytes = {
            let mut guard = cache
                .lock()
                .map_err(|_| Status::internal("desktop capture cache is poisoned"))?;

            let refresh = force_refresh || start_position == 0 || guard.is_none();
            if refresh {
                *guard = Some(capture_png(display_index).map_err(status_from_error)?);
            }

            guard.clone().unwrap_or_default()
        };

        if start_position > bytes.len() {
            return Err(Status::invalid_argument(
                "start_position beyond screenshot size",
            ));
        }

        let output = try_stream! {
            let chunk_size = 64 * 1024;
            let mut position = start_position;

            while position < bytes.len() {
                let end = (position + chunk_size).min(bytes.len());
                let data = bytes[position..end].to_vec();
                let completed = end >= bytes.len();

                yield CaptureChunk {
                    position: position as i64,
                    length: data.len() as i32,
                    data,
                    completed,
                    mime_type: "image/png".to_string(),
                };

                position = end;
            }

            if bytes.is_empty() {
                yield CaptureChunk {
                    position: 0,
                    length: 0,
                    data: Vec::new(),
                    completed: true,
                    mime_type: "image/png".to_string(),
                };
            }
        };

        Ok(Response::new(Box::pin(output)))
    }

    async fn ping(&self, _request: Request<Empty>) -> Result<Response<PingReply>, Status> {
        Ok(Response::new(PingReply {
            message: format!(
                "desktop agent ready; preferred display index {}",
                self.preferred_display_index
            ),
        }))
    }
}

fn capture_png(display_index: u32) -> Result<Vec<u8>> {
    let screens = Screen::all().map_err(|error| anyhow!("enumerate screens: {error}"))?;
    let screen = select_screen(&screens, display_index)?;

    let image = screen
        .capture()
        .or_else(|error| capture_png_fallback(display_index, &screens, error))?;
    let (width, height) = (image.width(), image.height());
    let raw = image.into_raw();

    let mut png = Vec::new();
    let encoder = PngEncoder::new(Cursor::new(&mut png));
    encoder
        .write_image(&raw, width, height, ColorType::Rgba8.into())
        .context("encode screenshot to PNG")?;

    Ok(png)
}

fn select_screen(screens: &[Screen], display_index: u32) -> Result<Screen> {
    if let Some(screen) = screens.get(display_index as usize) {
        return Ok(*screen);
    }

    if let Some(screen) = screens
        .iter()
        .copied()
        .find(|screen| screen.display_info.is_primary)
    {
        warn!(
            requested_display_index = display_index,
            fallback_display_id = screen.display_info.id,
            "requested display index missing; using primary display"
        );
        return Ok(screen);
    }

    screens
        .first()
        .copied()
        .ok_or_else(|| anyhow!("no displays available for capture"))
}

fn describe_displays() -> Vec<String> {
    match Screen::all() {
        Ok(screens) => screens
            .iter()
            .enumerate()
            .map(|(index, screen)| {
                let info = screen.display_info;
                format!(
                    "#{index}: id={} primary={} pos=({}, {}) size={}x{} scale={} rate={}",
                    info.id,
                    info.is_primary,
                    info.x,
                    info.y,
                    info.width,
                    info.height,
                    info.scale_factor,
                    info.frequency
                )
            })
            .collect(),
        Err(error) => vec![format!("display enumeration failed: {error}")],
    }
}

fn status_from_error(error: anyhow::Error) -> Status {
    Status::internal(error.to_string())
}

#[cfg(target_os = "linux")]
fn capture_png_fallback(
    display_index: u32,
    _screens: &[Screen],
    original_error: anyhow::Error,
) -> Result<RgbaImage> {
    let session_type = std::env::var("XDG_SESSION_TYPE").unwrap_or_default();
    let wayland_display = std::env::var("WAYLAND_DISPLAY").unwrap_or_default();
    let is_wayland = session_type.eq_ignore_ascii_case("wayland")
        || wayland_display.to_ascii_lowercase().contains("wayland");

    if !is_wayland {
        return Err(original_error);
    }

    let output = Command::new("grim")
        .arg("-")
        .output()
        .map_err(|grim_error| {
            anyhow!(
                "capture failed on display index {} and grim fallback unavailable: {}; grim error: {}",
                display_index,
                original_error,
                grim_error
            )
        })?;

    if !output.status.success() {
        return Err(anyhow!(
            "capture failed on display index {} and grim fallback failed: {}; grim stderr: {}",
            display_index,
            original_error,
            String::from_utf8_lossy(&output.stderr)
        ));
    }

    Ok(image::load_from_memory(&output.stdout)
        .context("decode grim PNG output")?
        .into_rgba8())
}

#[cfg(not(target_os = "linux"))]
fn capture_png_fallback(
    _display_index: u32,
    _screens: &[Screen],
    original_error: anyhow::Error,
) -> Result<RgbaImage> {
    Err(original_error)
}
