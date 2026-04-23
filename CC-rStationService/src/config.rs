use std::path::{Path, PathBuf};

use anyhow::{Context, Result};
use serde::Deserialize;
use uuid::Uuid;

#[derive(Debug, Clone, Deserialize, Default)]
#[serde(default)]
pub struct AppConfig {
    pub service: ServiceConfig,
    pub control: ControlConfig,
    pub agent: AgentConfig,
}

#[derive(Debug, Clone, Deserialize)]
#[serde(default)]
pub struct ServiceConfig {
    pub service_name: String,
    pub station_id: String,
    pub state_interval_seconds: u64,
    pub watched_processes: Vec<String>,
    pub udp_display_target: String,
    pub launcher_proxy_path: String,
}

#[derive(Debug, Clone, Deserialize)]
#[serde(default)]
pub struct ControlConfig {
    pub listen_addr: String,
}

#[derive(Debug, Clone, Deserialize)]
#[serde(default)]
pub struct AgentConfig {
    pub listen_addr: String,
    pub auth_token: String,
    pub preferred_display_index: u32,
}

impl Default for ServiceConfig {
    fn default() -> Self {
        Self {
            service_name: "CC-rStationService".to_string(),
            station_id: String::new(),
            state_interval_seconds: 5,
            watched_processes: Vec::new(),
            udp_display_target: "127.0.0.1:9008".to_string(),
            launcher_proxy_path: String::new(),
        }
    }
}

impl Default for ControlConfig {
    fn default() -> Self {
        Self {
            listen_addr: "0.0.0.0:50051".to_string(),
        }
    }
}

impl Default for AgentConfig {
    fn default() -> Self {
        Self {
            listen_addr: "127.0.0.1:50052".to_string(),
            auth_token: "local-change-me".to_string(),
            preferred_display_index: 0,
        }
    }
}

impl AppConfig {
    pub fn load(path: Option<&Path>) -> Result<Self> {
        let path = path
            .map(Path::to_path_buf)
            .unwrap_or_else(default_config_path);

        if !path.exists() {
            return Ok(Self::default());
        }

        let content = std::fs::read_to_string(&path)
            .with_context(|| format!("read config {}", path.display()))?;
        let config = toml::from_str::<Self>(&content)
            .with_context(|| format!("parse config {}", path.display()))?;
        Ok(config)
    }

    pub fn resolved_station_id(&self) -> String {
        if !self.service.station_id.trim().is_empty() {
            return self.service.station_id.trim().to_string();
        }

        let host = hostname::get()
            .ok()
            .and_then(|value| value.into_string().ok())
            .filter(|value| !value.trim().is_empty())
            .unwrap_or_else(|| "station".to_string());

        format!("{host}-{}", Uuid::new_v4())
    }
}

pub fn default_config_path() -> PathBuf {
    std::env::current_exe()
        .ok()
        .and_then(|path| path.parent().map(Path::to_path_buf))
        .unwrap_or_else(|| PathBuf::from("."))
        .join("CC-rStationService.toml")
}
