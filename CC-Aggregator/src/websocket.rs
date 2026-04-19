//! WebSocket server module for CC-Aggregator
//! 
//! This module implements the WebSocket server that bridges
//! between browser clients and the MQTT message broker.

use anyhow::{Context, Result};
use futures_util::{SinkExt, StreamExt};
use parking_lot::RwLock;
use serde::{Deserialize, Serialize};
use std::collections::HashMap;
use std::net::SocketAddr;
use std::sync::Arc;
use tokio::net::{TcpListener, TcpStream};
use tokio::sync::{broadcast, mpsc};
use tokio_tungstenite::{accept_async, tungstenite::Message};
use tracing::{debug, error, info, warn};

use crate::mqtt::{MqttMessage, StationStatus};
use crate::telemetry::TelemetryBundle;

/// WebSocket client session with message sender for pushing data
#[derive(Debug, Clone)]
pub struct WsSession {
    pub id: String,
    pub station_filter: Vec<String>,  // Station IDs this client is interested in
    tx: mpsc::Sender<String>,         // Sender for pushing messages to this session
}

impl WsSession {
    pub fn new(id: String, tx: mpsc::Sender<String>) -> Self {
        Self {
            id,
            station_filter: Vec::new(),
            tx,
        }
    }

    pub fn subscribe_stations(&mut self, station_ids: Vec<String>) {
        self.station_filter = station_ids;
    }

    /// Get a clone of the message sender for this session
    pub fn get_sender(&self) -> mpsc::Sender<String> {
        self.tx.clone()
    }
}

/// Outbound message types for WebSocket push
#[derive(Debug, Clone)]
pub enum WsOutboundMessage {
    /// Telemetry data for a station
    Telemetry { station_id: String, data: TelemetryBundle },
    /// Status update for a station
    StatusUpdate { station_id: String, status: StationStatus },
    /// Station list response
    StationList(Vec<String>),
    /// Error message
    Error { message: String },
}

impl WsOutboundMessage {
    /// Serialize to JSON string
    pub fn to_json_string(&self) -> Result<String> {
        match self {
            WsOutboundMessage::Telemetry { station_id, data } => {
                serde_json::to_string(&TelemetryWsMessage {
                    msg_type: "telemetry".to_string(),
                    station_id: station_id.clone(),
                    data: Some(data.clone()),
                    status: None,
                    stations: None,
                    message: None,
                }).context("Failed to serialize telemetry message")
            }
            WsOutboundMessage::StatusUpdate { station_id, status } => {
                serde_json::to_string(&TelemetryWsMessage {
                    msg_type: "status".to_string(),
                    station_id: station_id.clone(),
                    data: None,
                    status: Some(status.clone()),
                    stations: None,
                    message: None,
                }).context("Failed to serialize status message")
            }
            WsOutboundMessage::StationList(stations) => {
                serde_json::to_string(&TelemetryWsMessage {
                    msg_type: "stationList".to_string(),
                    station_id: String::new(),
                    data: None,
                    status: None,
                    stations: Some(stations.clone()),
                    message: None,
                }).context("Failed to serialize station list message")
            }
            WsOutboundMessage::Error { message } => {
                serde_json::to_string(&TelemetryWsMessage {
                    msg_type: "error".to_string(),
                    station_id: String::new(),
                    data: None,
                    status: None,
                    stations: None,
                    message: Some(message.clone()),
                }).context("Failed to serialize error message")
            }
        }
    }
}

/// JSON message format for WebSocket communication
#[derive(Debug, Clone, Serialize, Deserialize)]
struct TelemetryWsMessage {
    #[serde(rename = "type")]
    msg_type: String,
    station_id: String,
    data: Option<TelemetryBundle>,
    status: Option<StationStatus>,
    stations: Option<Vec<String>>,
    message: Option<String>,
}

/// WebSocket server state
pub struct WsServer {
    sessions: Arc<RwLock<HashMap<String, WsSession>>>,
    mqtt_tx: broadcast::Sender<MqttMessage>,
}

impl WsServer {
    pub fn new(mqtt_broadcast_tx: broadcast::Sender<MqttMessage>) -> Self {
        Self {
            sessions: Arc::new(RwLock::new(HashMap::new())),
            mqtt_tx: mqtt_broadcast_tx,
        }
    }

    /// Subscribe to incoming MQTT messages
    pub fn subscribe_to_messages(&self) -> broadcast::Receiver<MqttMessage> {
        self.mqtt_tx.subscribe()
    }

    /// Start the WebSocket server
    pub async fn start(&self, listen_addr: &str) -> Result<()> {
        let addr: SocketAddr = listen_addr.parse()
            .context("Invalid WebSocket listen address")?;
        
        let listener = TcpListener::bind(&addr).await
            .context("Failed to bind WebSocket address")?;
        
        info!("WebSocket server listening on {}", addr);

        loop {
            match listener.accept().await {
                Ok((stream, peer_addr)) => {
                    let sessions = self.sessions.clone();
                    let mqtt_tx = self.mqtt_tx.clone();
                    
                    tokio::spawn(async move {
                        if let Err(e) = handle_connection(stream, peer_addr, sessions, mqtt_tx).await {
                            error!("WebSocket connection error: {:?}", e);
                        }
                    });
                }
                Err(e) => {
                    error!("Failed to accept WebSocket connection: {:?}", e);
                }
            }
        }
    }

    /// Broadcast telemetry to subscribed clients
    pub async fn broadcast_telemetry(&self, station_id: &str, data: &TelemetryBundle) {
        let msg = match (WsOutboundMessage::Telemetry {
            station_id: station_id.to_string(),
            data: data.clone(),
        }).to_json_string() {
            Ok(j) => j,
            Err(e) => {
                error!("Failed to serialize telemetry message: {:?}", e);
                return;
            }
        };
        
        // Collect session IDs and senders while holding the lock
        let targets: Vec<(String, mpsc::Sender<String>)> = {
            let sessions = self.sessions.read();
            sessions.iter()
                .filter(|(_, session)| {
                    session.station_filter.is_empty() || 
                    session.station_filter.contains(&station_id.to_string())
                })
                .map(|(id, session)| (id.clone(), session.get_sender()))
                .collect()
        };
        
        // Now release the lock and send messages
        for (session_id, tx) in targets {
            if let Err(e) = tx.send(msg.clone()).await {
                warn!("Failed to send telemetry to session {}: {:?}", session_id, e);
            }
        }
    }

    /// Broadcast status update to subscribed clients
    pub async fn broadcast_status(&self, station_id: &str, status: &StationStatus) {
        let msg = match (WsOutboundMessage::StatusUpdate {
            station_id: station_id.to_string(),
            status: status.clone(),
        }).to_json_string() {
            Ok(j) => j,
            Err(e) => {
                error!("Failed to serialize status message: {:?}", e);
                return;
            }
        };
        
        // Collect session IDs and senders while holding the lock
        let targets: Vec<(String, mpsc::Sender<String>)> = {
            let sessions = self.sessions.read();
            sessions.iter()
                .filter(|(_, session)| {
                    session.station_filter.is_empty() || 
                    session.station_filter.contains(&station_id.to_string())
                })
                .map(|(id, session)| (id.clone(), session.get_sender()))
                .collect()
        };
        
        // Now release the lock and send messages
        for (session_id, tx) in targets {
            if let Err(e) = tx.send(msg.clone()).await {
                warn!("Failed to send status to session {}: {:?}", session_id, e);
            }
        }
    }
}

/// Handle a single WebSocket connection
async fn handle_connection(
    stream: TcpStream,
    peer_addr: SocketAddr,
    sessions: Arc<RwLock<HashMap<String, WsSession>>>,
    _mqtt_tx: broadcast::Sender<MqttMessage>,
) -> Result<()> {
    let ws_stream = accept_async(stream)
        .await
        .context("WebSocket handshake failed")?;
    
    let session_id = uuid::Uuid::new_v4().to_string();
    info!("New WebSocket connection: session_id={}, peer={}", session_id, peer_addr);

    // Create mpsc channel for this session's outbound messages
    let (tx, mut rx) = mpsc::channel::<String>(100);
    
    // Register session with the message sender
    {
        let mut sessions = sessions.write();
        sessions.insert(session_id.clone(), WsSession::new(session_id.clone(), tx));
    }

    let (mut write, mut read) = ws_stream.split();
    
    // Spawn task to forward messages from the channel to the WebSocket
    let write_session_id = session_id.clone();
    let write_task = tokio::spawn(async move {
        while let Some(msg) = rx.recv().await {
            if let Err(e) = write.send(Message::Text(msg.into())).await {
                warn!("Failed to send WebSocket message to session {}: {:?}", write_session_id, e);
                break;
            }
        }
        info!("WebSocket write task ended for session {}", write_session_id);
    });
    
    // Handle incoming messages
    while let Some(msg_result) = read.next().await {
        match msg_result {
            Ok(Message::Text(text)) => {
                if let Err(e) = handle_text_message(&text, &session_id, &sessions).await {
                    warn!("Failed to handle message: {:?}", e);
                }
            }
            Ok(Message::Binary(data)) => {
                // Handle binary messages (JSON)
                if let Ok(text) = String::from_utf8(data.to_vec()) {
                    if let Err(e) = handle_text_message(&text, &session_id, &sessions).await {
                        warn!("Failed to handle binary message: {:?}", e);
                    }
                }
            }
            Ok(Message::Close(_)) => {
                info!("WebSocket connection closed: session_id={}", session_id);
                break;
            }
            Err(e) => {
                error!("WebSocket read error: {:?}", e);
                break;
            }
            _ => {}
        }
    }

    // Abort the write task since we're done
    write_task.abort();
    
    // Cleanup session
    {
        let mut sessions = sessions.write();
        sessions.remove(&session_id);
    }

    Ok(())
}

/// Handle incoming text message
async fn handle_text_message(
    text: &str,
    session_id: &str,
    sessions: &Arc<RwLock<HashMap<String, WsSession>>>,
) -> Result<()> {
    debug!("Received message from {}: {}", session_id, text);

    // Parse message as JSON
    if let Ok(msg) = serde_json::from_str::<serde_json::Value>(text) {
        if let Some(msg_type) = msg.get("type").and_then(|v| v.as_str()) {
            match msg_type {
                "subscribe" => {
                    if let Some(stations) = msg.get("stations").and_then(|v| v.as_array()) {
                        let station_ids: Vec<String> = stations
                            .iter()
                            .filter_map(|v| v.as_str().map(String::from))
                            .collect();
                        
                        let mut sessions = sessions.write();
                        if let Some(session) = sessions.get_mut(session_id) {
                            session.subscribe_stations(station_ids.clone());
                        }
                        
                        info!("Session {} subscribed to {} stations", session_id, station_ids.len());
                    }
                }
                "unsubscribe" => {
                    if let Some(stations) = msg.get("stations").and_then(|v| v.as_array()) {
                        let station_ids: Vec<String> = stations
                            .iter()
                            .filter_map(|v| v.as_str().map(String::from))
                            .collect();
                        
                        let mut sessions = sessions.write();
                        if let Some(session) = sessions.get_mut(session_id) {
                            // Remove stations from filter
                            session.station_filter.retain(|id| !station_ids.contains(id));
                        }
                        
                        info!("Session {} unsubscribed from {} stations", session_id, station_ids.len());
                    }
                }
                "getStations" => {
                    // Client requests station list
                    // Response would be sent via WebSocket
                }
                _ => {
                    warn!("Unknown message type: {}", msg_type);
                }
            }
        }
    }

    Ok(())
}
