//! Configuration module for CC-Aggregator

use anyhow::{Context, Result};
use serde::{Deserialize, Serialize};
use std::path::Path;

/// Aggregator configuration
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct AggregatorConfig {
    /// Aggregator server ID
    pub server_id: String,
    /// MQTT broker configuration
    pub mqtt: MqttConfig,
    /// WebSocket server configuration
    pub websocket: WebSocketConfig,
    /// Logging configuration
    pub logging: LoggingConfig,
}

impl Default for AggregatorConfig {
    fn default() -> Self {
        Self {
            server_id: uuid::Uuid::new_v4().to_string(),
            mqtt: MqttConfig::default(),
            websocket: WebSocketConfig::default(),
            logging: LoggingConfig::default(),
        }
    }
}

impl AggregatorConfig {
    /// Load configuration from file
    pub fn load<P: AsRef<Path>>(path: P) -> Result<Self> {
        let content = std::fs::read_to_string(path.as_ref())
            .context("Failed to read configuration file")?;
        
        let config: AggregatorConfig = toml::from_str(&content)
            .map_err(|e| anyhow::anyhow!("TOML parse error: {}", e))?;
        
        Ok(config)
    }

    /// Save configuration to file
    pub fn save<P: AsRef<Path>>(&self, path: P) -> Result<()> {
        let content = toml::to_string_pretty(self)
            .context("Failed to serialize configuration")?;
        
        std::fs::write(path.as_ref(), content)
            .context("Failed to write configuration file")
    }
}

/// MQTT broker configuration
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct MqttConfig {
    /// Broker host
    pub host: String,
    /// Broker port
    pub port: u16,
    /// MQTT client ID
    pub client_id: String,
    /// Keep-alive interval in seconds
    pub keepalive_secs: u64,
    /// QoS level for telemetry
    pub telemetry_qos: u8,
    /// QoS level for commands
    pub command_qos: u8,
}

impl Default for MqttConfig {
    fn default() -> Self {
        Self {
            host: "localhost".to_string(),
            port: 4222,
            client_id: "cc-aggregator".to_string(),
            keepalive_secs: 30,
            telemetry_qos: 1,
            command_qos: 1,
        }
    }
}

/// WebSocket server configuration
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct WebSocketConfig {
    /// Listen address
    pub listen_addr: String,
    /// Maximum connections
    pub max_connections: usize,
}

impl Default for WebSocketConfig {
    fn default() -> Self {
        Self {
            listen_addr: "127.0.0.1:8080".to_string(),
            max_connections: 1000,
        }
    }
}

/// Logging configuration
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct LoggingConfig {
    /// Log level (trace, debug, info, warn, error)
    pub level: String,
    /// Log file path (optional)
    pub file: Option<String>,
}

impl Default for LoggingConfig {
    fn default() -> Self {
        Self {
            level: "info".to_string(),
            file: None,
        }
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_config_default() {
        let config = AggregatorConfig::default();
        assert_eq!(config.mqtt.host, "localhost");
        assert_eq!(config.mqtt.port, 4222);
        assert_eq!(config.websocket.listen_addr, "127.0.0.1:8080");
    }

    #[test]
    fn test_config_serialization() {
        let config = AggregatorConfig::default();
        let toml = toml::to_string(&config).unwrap();
        let parsed: AggregatorConfig = toml::from_str(&toml).unwrap();
        
        assert_eq!(parsed.server_id, config.server_id);
    }
}
