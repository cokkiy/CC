//! WebSocket server module for CC-Aggregator
//! 
//! This module implements the WebSocket server that bridges
//! between browser clients and the MQTT message broker.

use anyhow::{Context, Result};
use futures_util::StreamExt;
use parking_lot::RwLock;
use std::collections::HashMap;
use std::net::SocketAddr;
use std::sync::Arc;
use tokio::net::{TcpListener, TcpStream};
use tokio::sync::broadcast;
use tokio_tungstenite::{accept_async, tungstenite::Message};
use tracing::{debug, error, info, warn};

use crate::mqtt::{MqttMessage, StationStatus};
use crate::telemetry::TelemetryBundle;

/// WebSocket client session
#[derive(Debug, Clone)]
pub struct WsSession {
    pub id: String,
    pub station_filter: Vec<String>,  // Station IDs this client is interested in
}

impl WsSession {
    pub fn new(id: String) -> Self {
        Self {
            id,
            station_filter: Vec::new(),
        }
    }

    pub fn subscribe_stations(&mut self, station_ids: Vec<String>) {
        self.station_filter = station_ids;
    }
}

/// WebSocket message types
#[derive(Debug, Clone)]
pub enum WsMessage {
    /// Client subscribes to station updates
    Subscribe(Vec<String>),
    /// Client unsubscribes from station updates
    Unsubscribe(Vec<String>),
    /// Client requests station list
    GetStations,
    /// Server pushes telemetry to client
    Telemetry { station_id: String, data: TelemetryBundle },
    /// Server pushes status update to client
    StatusUpdate { station_id: String, status: StationStatus },
    /// Server responds with station list
    StationList(Vec<String>),
    /// Error message
    Error(String),
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
        let sessions = self.sessions.read();
        
        for (session_id, session) in sessions.iter() {
            // Check if session is subscribed to this station
            if session.station_filter.is_empty() || session.station_filter.contains(&station_id.to_string()) {
                debug!("Would broadcast telemetry to session {}", session_id);
            }
        }
    }

    /// Broadcast status update to subscribed clients
    pub async fn broadcast_status(&self, station_id: &str, status: &StationStatus) {
        let sessions = self.sessions.read();
        
        for (session_id, session) in sessions.iter() {
            if session.station_filter.is_empty() || session.station_filter.contains(&station_id.to_string()) {
                debug!("Would broadcast status to session {}", session_id);
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

    // Register session
    {
        let mut sessions = sessions.write();
        sessions.insert(session_id.clone(), WsSession::new(session_id.clone()));
    }

    let (mut write, mut read) = ws_stream.split();
    
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
