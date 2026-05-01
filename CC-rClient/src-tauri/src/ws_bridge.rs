//! WebSocket bridge module for CC-rClient
//!
//! This module implements WebSocket connection to CC-Aggregator
//! and bridges between the Tauri frontend and MQTT message broker.

use anyhow::{Context, Result};
use futures_util::{SinkExt, StreamExt};
use serde::{Deserialize, Serialize};
use std::sync::Arc;
use std::time::Duration;
use tauri::{AppHandle, Emitter};
use tokio::sync::{RwLock, mpsc};
use tokio_tungstenite::{connect_async, tungstenite::Message};
use tracing::{debug, error, info, warn};

/// WebSocket message types for client -> aggregator communication
#[derive(Debug, Clone, Serialize, Deserialize)]
#[serde(tag = "type")]
pub enum WsClientMessage {
    #[serde(rename = "subscribe")]
    Subscribe { stations: Vec<String> },

    #[serde(rename = "unsubscribe")]
    Unsubscribe { stations: Vec<String> },

    #[serde(rename = "getStations")]
    GetStations,
}

/// WebSocket message types from aggregator
#[derive(Debug, Clone, Serialize, Deserialize)]
#[serde(tag = "type")]
pub enum WsServerMessage {
    #[serde(rename = "telemetry")]
    Telemetry {
        station_id: String,
        data: TelemetryBundle,
    },

    #[serde(rename = "status")]
    Status {
        station_id: String,
        status: StationStatus,
    },

    #[serde(rename = "stationList")]
    StationList { stations: Vec<String> },

    #[serde(rename = "error")]
    Error { message: String },
}

/// Telemetry bundle from CC-Aggregator
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct TelemetryBundle {
    pub ts: i64,
    pub station_id: String,
    pub interval_ms: u32,
    pub values: Vec<TelemetryValue>,
}

/// Telemetry value
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct TelemetryValue {
    pub key: String,
    pub v: f64,
}

/// Station status
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct StationStatus {
    pub station_id: String,
    pub online: bool,
    pub last_seen: i64,
    pub version: Option<String>,
    pub alert: Option<String>,
}

/// WebSocket connection state
#[derive(Debug, Clone, PartialEq)]
pub enum ConnectionState {
    Disconnected,
    Connecting,
    Connected,
    Reconnecting,
}

/// Event payload for telemetry
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct TelemetryEventPayload {
    pub station_id: String,
    pub data: TelemetryBundle,
}

/// Event payload for status
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct StatusEventPayload {
    pub station_id: String,
    pub status: StationStatus,
}

/// Commands that can be sent to the bridge
#[derive(Debug)]
pub enum BridgeCommand {
    Subscribe(Vec<String>),
    Unsubscribe(Vec<String>),
    GetStations,
    Disconnect,
}

const RECONNECT_DELAY: Duration = Duration::from_secs(2);

/// MqttWsBridge - WebSocket bridge for MQTT telemetry
pub struct MqttWsBridge {
    aggregator_url: String,
    app_handle: AppHandle,
    state: Arc<RwLock<ConnectionState>>,
    subscribed_stations: Arc<RwLock<Vec<String>>>,
    command_tx: Option<mpsc::Sender<BridgeCommand>>,
}

impl MqttWsBridge {
    /// Create a new WebSocket bridge
    pub fn new(app_handle: AppHandle, aggregator_url: &str) -> Self {
        Self {
            aggregator_url: aggregator_url.to_string(),
            app_handle,
            state: Arc::new(RwLock::new(ConnectionState::Disconnected)),
            subscribed_stations: Arc::new(RwLock::new(Vec::new())),
            command_tx: None,
        }
    }

    async fn send_command(&self, command: BridgeCommand) -> Result<()> {
        if let Some(tx) = &self.command_tx {
            tx.send(command).await?;
        }

        Ok(())
    }

    /// Get current connection state
    pub async fn get_state(&self) -> ConnectionState {
        self.state.read().await.clone()
    }

    /// Start the bridge - connects to aggregator and handles messages
    pub async fn start(&mut self) -> Result<()> {
        {
            // Check if already connected or connecting
            let current_state = self.state.read().await;
            if *current_state == ConnectionState::Connected
                || *current_state == ConnectionState::Connecting
                || *current_state == ConnectionState::Reconnecting
            {
                info!("Already connected or connecting");
                return Ok(());
            }
        }

        let mut first_attempt = true;

        loop {
            {
                let mut state = self.state.write().await;
                *state = if first_attempt {
                    ConnectionState::Connecting
                } else {
                    ConnectionState::Reconnecting
                };
            }

            info!("Connecting to CC-Aggregator at: {}", self.aggregator_url);

            let url = self.aggregator_url.clone();
            let app_handle = self.app_handle.clone();
            let state = self.state.clone();

            // Create a fresh command channel for each connection attempt.
            let (tx, mut rx) = mpsc::channel::<BridgeCommand>(100);
            self.command_tx = Some(tx);

            match connect_async(&url).await {
                Ok((ws_stream, _)) => {
                    info!("WebSocket connected to aggregator");
                    {
                        let mut s = state.write().await;
                        *s = ConnectionState::Connected;
                    }

                    let (ws_write, mut read) = ws_stream.split();
                    let subscribed_stations = self.subscribed_stations.read().await.clone();
                    let initial_subscription = if subscribed_stations.is_empty() {
                        BridgeCommand::Subscribe(Vec::new())
                    } else {
                        BridgeCommand::Subscribe(subscribed_stations)
                    };
                    let _ = self.send_command(initial_subscription).await;
                    let _ = self.send_command(BridgeCommand::GetStations).await;

                    // Spawn task to handle commands - takes ownership of ws_write
                    let command_handle = tokio::spawn(async move {
                        let mut write = ws_write;
                        while let Some(cmd) = rx.recv().await {
                            match cmd {
                                BridgeCommand::Subscribe(stations) => {
                                    let msg = WsClientMessage::Subscribe { stations };
                                    if let Ok(json) = serde_json::to_string(&msg) {
                                        if let Err(e) = write.send(Message::Text(json.into())).await {
                                            warn!("Failed to send subscribe message: {:?}", e);
                                            break;
                                        }
                                    }
                                }
                                BridgeCommand::Unsubscribe(stations) => {
                                    let msg = WsClientMessage::Unsubscribe { stations };
                                    if let Ok(json) = serde_json::to_string(&msg) {
                                        if let Err(e) = write.send(Message::Text(json.into())).await {
                                            warn!("Failed to send unsubscribe message: {:?}", e);
                                            break;
                                        }
                                    }
                                }
                                BridgeCommand::GetStations => {
                                    let msg = WsClientMessage::GetStations;
                                    if let Ok(json) = serde_json::to_string(&msg) {
                                        if let Err(e) = write.send(Message::Text(json.into())).await {
                                            warn!("Failed to send getStations message: {:?}", e);
                                            break;
                                        }
                                    }
                                }
                                BridgeCommand::Disconnect => {
                                    info!("Disconnect requested");
                                    let _ = write.close().await;
                                    break;
                                }
                            }
                        }
                    });

                    while let Some(msg_result) = read.next().await {
                        match msg_result {
                            Ok(Message::Text(text)) => {
                                Self::handle_message(&text, &app_handle);
                            }
                            Ok(Message::Binary(data)) => {
                                if let Ok(text) = String::from_utf8(data.to_vec()) {
                                    Self::handle_message(&text, &app_handle);
                                }
                            }
                            Ok(Message::Close(_)) => {
                                info!("WebSocket connection closed by server");
                                break;
                            }
                            Ok(Message::Ping(data)) => {
                                debug!("Received ping (auto-handled)");
                                let _ = data;
                            }
                            Ok(Message::Pong(_)) => {
                                debug!("Received pong");
                            }
                            Err(e) => {
                                error!("WebSocket read error: {:?}", e);
                                break;
                            }
                            _ => {}
                        }
                    }

                    command_handle.abort();
                    {
                        let mut s = state.write().await;
                        *s = ConnectionState::Disconnected;
                    }
                    self.command_tx = None;

                    info!("WebSocket disconnected");
                    let _ = app_handle.emit("ws-disconnected", ());
                }
                Err(e) => {
                    error!("Failed to connect to aggregator: {:?}", e);
                    {
                        let mut s = state.write().await;
                        *s = ConnectionState::Disconnected;
                    }
                    self.command_tx = None;
                    let _ = app_handle.emit("ws-error", e.to_string());
                    warn!("{}", Err::<(), _>(e).context("WebSocket connection failed").unwrap_err());
                }
            }

            first_attempt = false;
            tokio::time::sleep(RECONNECT_DELAY).await;
        }
    }

    /// Handle incoming WebSocket message
    fn handle_message(text: &str, app_handle: &AppHandle) {
        debug!("Received WebSocket message: {}", text);

        if let Ok(msg) = serde_json::from_str::<WsServerMessage>(text) {
            match msg {
                WsServerMessage::Telemetry { station_id, data } => {
                    info!(
                        "Received telemetry for station {}: {} values",
                        station_id,
                        data.values.len()
                    );
                    let payload = TelemetryEventPayload { station_id, data };
                    if let Err(e) = app_handle.emit("telemetry", payload) {
                        warn!("Failed to emit telemetry event: {:?}", e);
                    }
                }
                WsServerMessage::Status { station_id, status } => {
                    info!(
                        "Received status for station {}: online={}",
                        station_id, status.online
                    );
                    let payload = StatusEventPayload { station_id, status };
                    if let Err(e) = app_handle.emit("station-status", payload) {
                        warn!("Failed to emit status event: {:?}", e);
                    }
                }
                WsServerMessage::StationList { stations } => {
                    info!("Received station list: {} stations", stations.len());
                    if let Err(e) = app_handle.emit("station-list", stations) {
                        warn!("Failed to emit station list event: {:?}", e);
                    }
                }
                WsServerMessage::Error { message } => {
                    error!("Received error from aggregator: {}", message);
                    if let Err(e) = app_handle.emit("ws-error", message) {
                        warn!("Failed to emit error event: {:?}", e);
                    }
                }
            }
        } else {
            warn!("Failed to parse WebSocket message: {}", text);
        }
    }

    /// Subscribe to station updates
    pub async fn subscribe_stations(&self, station_ids: Vec<String>) -> Result<()> {
        {
            let mut subscribed = self.subscribed_stations.write().await;
            for id in &station_ids {
                if !subscribed.contains(id) {
                    subscribed.push(id.clone());
                }
            }
        }

        info!("Subscribing to stations: {:?}", station_ids);

        self.send_command(BridgeCommand::Subscribe(station_ids)).await?;

        Ok(())
    }

    /// Unsubscribe from station updates
    pub async fn unsubscribe_stations(&self, station_ids: Vec<String>) -> Result<()> {
        {
            let mut subscribed = self.subscribed_stations.write().await;
            subscribed.retain(|id| !station_ids.contains(id));
        }

        info!("Unsubscribing from stations: {:?}", station_ids);

        self.send_command(BridgeCommand::Unsubscribe(station_ids)).await?;

        Ok(())
    }

    /// Request station list
    pub async fn request_stations(&self) -> Result<()> {
        info!("Requesting station list");

        self.send_command(BridgeCommand::GetStations).await?;

        Ok(())
    }

    /// Disconnect from the aggregator
    pub async fn disconnect(&self) -> Result<()> {
        info!("Disconnecting from aggregator");

        let _ = self.send_command(BridgeCommand::Disconnect).await;

        {
            let mut state = self.state.write().await;
            *state = ConnectionState::Disconnected;
        }

        Ok(())
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_serialize_subscribe_message() {
        let msg = WsClientMessage::Subscribe {
            stations: vec!["ws-001".to_string(), "ws-002".to_string()],
        };
        let json = serde_json::to_string(&msg).unwrap();
        assert!(json.contains("\"type\":\"subscribe\""));
        assert!(json.contains("ws-001"));
        assert!(json.contains("ws-002"));
    }

    #[test]
    fn test_deserialize_telemetry_message() {
        let json = r#"{"type":"telemetry","station_id":"ws-001","data":{"ts":1234567890,"station_id":"ws-001","interval_ms":1000,"values":[{"key":"cpu","v":45.5}]}}"#;
        let msg: WsServerMessage = serde_json::from_str(json).unwrap();
        match msg {
            WsServerMessage::Telemetry { station_id, data } => {
                assert_eq!(station_id, "ws-001");
                assert_eq!(data.values.len(), 1);
                assert_eq!(data.values[0].key, "cpu");
                assert_eq!(data.values[0].v, 45.5);
            }
            _ => panic!("Expected Telemetry message"),
        }
    }

    #[test]
    fn test_deserialize_status_message() {
        let json = r#"{"type":"status","station_id":"ws-001","status":{"station_id":"ws-001","online":true,"last_seen":1234567890,"version":"1.0.0","alert":null}}"#;
        let msg: WsServerMessage = serde_json::from_str(json).unwrap();
        match msg {
            WsServerMessage::Status { station_id, status } => {
                assert_eq!(station_id, "ws-001");
                assert!(status.online);
                assert_eq!(status.version, Some("1.0.0".to_string()));
            }
            _ => panic!("Expected Status message"),
        }
    }
}
