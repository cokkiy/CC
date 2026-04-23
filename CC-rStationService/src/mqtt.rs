//! MQTT client module for CC-rStationService
//! 
//! This module implements MQTT telemetry publishing for the station service.
//! It publishes telemetry data to the CC-Aggregator via MQTT and
//! subscribes to command messages from the CC-Aggregator.

use anyhow::{Context, Result};
use rumqttc::{AsyncClient, Event, MqttOptions, Packet, QoS};
use serde::{Deserialize, Serialize};
use std::collections::HashMap;
use std::sync::Arc;
use std::time::Duration;
use tokio::sync::{mpsc, Mutex};
use tracing::{debug, error, info, warn};

/// Command received from the Aggregator via MQTT
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct Command {
    pub command_id: String,
    pub command: String,
    pub params: serde_json::Value,
    pub timestamp: i64,
}

/// Acknowledgment response for a command
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct CommandAck {
    pub command_id: String,
    pub success: bool,
    pub message: String,
    pub timestamp: i64,
}

/// Internal message types for the MQTT worker
enum MqttWorkerMsg {
    /// Request to subscribe to a topic, with a channel to send the command receiver
    Subscribe {
        topic: String,
        response_tx: mpsc::Sender<Result<mpsc::Receiver<Command>, anyhow::Error>>,
    },
}

/// MQTT client wrapper for the station service
pub struct MqttClient {
    client: AsyncClient,
    station_id: String,
    worker_tx: mpsc::Sender<MqttWorkerMsg>,
}

impl Clone for MqttClient {
    fn clone(&self) -> Self {
        Self {
            client: self.client.clone(),
            station_id: self.station_id.clone(),
            worker_tx: self.worker_tx.clone(),
        }
    }
}

impl MqttClient {
    /// Create a new MQTT client connection
    pub fn new(broker_host: &str, broker_port: u16, station_id: &str) -> Result<Self> {
        let client_id = format!("cc-station-{}", station_id);
        let mut mqttoptions = MqttOptions::new(client_id.clone(), broker_host, broker_port);
        mqttoptions.set_keep_alive(Duration::from_secs(30));
        mqttoptions.set_clean_session(true);

        let (client, eventloop) = AsyncClient::new(mqttoptions, 100);
        let (worker_tx, mut worker_rx) = mpsc::channel::<MqttWorkerMsg>(100);
        
        let client_for_worker = client.clone();

        // Spawn background task to handle event loop and subscriptions
        tokio::spawn(async move {
            let mut eventloop = eventloop;
            // Map of topic -> command sender
            let handlers: Arc<Mutex<HashMap<String, mpsc::Sender<Command>>>> = Arc::new(Mutex::new(HashMap::new()));
            
            loop {
                tokio::select! {
                    // Handle messages from the main client (subscription requests)
                    msg = worker_rx.recv() => {
                        match msg {
                            Some(MqttWorkerMsg::Subscribe { topic, response_tx }) => {
                                // Subscribe to the topic
                                match client_for_worker.subscribe(&topic, QoS::AtLeastOnce).await {
                                    Ok(()) => {
                                        debug!("Subscribed to topic: {}", topic);
                                        
                                        // Create a channel for this subscription
                                        let (tx, rx) = mpsc::channel::<Command>(100);
                                        
                                        // Store the handler
                                        let mut handlers_lock = handlers.lock().await;
                                        handlers_lock.insert(topic.clone(), tx);
                                        
                                        let _ = response_tx.send(Ok(rx)).await;
                                    }
                                    Err(e) => {
                                        error!("Failed to subscribe to {}: {:?}", topic, e);
                                        let _ = response_tx.send(Err(anyhow::anyhow!("Subscribe failed: {:?}", e))).await;
                                    }
                                }
                            }
                            None => {
                                debug!("Worker channel closed, stopping worker");
                                break;
                            }
                        }
                    }
                    // Poll the event loop for incoming messages
                    notification = eventloop.poll() => {
                        match notification {
                            Ok(event) => {
                                if let Event::Incoming(Packet::Publish(publish)) = event {
                                    let topic = publish.topic.clone();
                                    
                                    // Look up handler for this topic
                                    let handlers_lock = handlers.lock().await;
                                    if let Some(tx) = handlers_lock.get(&topic) {
                                        if let Ok(cmd) = serde_json::from_slice::<Command>(&publish.payload) {
                                            debug!("Received command for topic {}: {:?}", topic, cmd);
                                            if tx.send(cmd).await.is_err() {
                                                warn!("Handler for topic {} dropped", topic);
                                            }
                                        }
                                    }
                                }
                            }
                            Err(e) => {
                                error!("MQTT event loop error: {:?}", e);
                                tokio::time::sleep(Duration::from_secs(1)).await;
                            }
                        }
                    }
                }
            }
            
            info!("MQTT worker task ending");
        });

        info!("MQTT client created: broker={}:{}, station_id={}", 
              broker_host, broker_port, station_id);

        Ok(Self {
            client,
            station_id: station_id.to_string(),
            worker_tx,
        })
    }

    /// Subscribe to command topic and return a receiver for command messages.
    pub async fn subscribe_commands(&self) -> Result<mpsc::Receiver<Command>> {
        let command_topic = format!("cc/{}/command", self.station_id);
        
        let (response_tx, mut response_rx) = mpsc::channel::<Result<mpsc::Receiver<Command>, anyhow::Error>>(1);
        
        self.worker_tx
            .send(MqttWorkerMsg::Subscribe {
                topic: command_topic,
                response_tx,
            })
            .await
            .context("Failed to send subscribe request to worker")?;
        
        let result = response_rx
            .recv()
            .await
            .context("Worker dropped response channel")??;
        
        Ok(result)
    }

    /// Publish a command acknowledgment to the aggregator
    pub async fn publish_command_ack(&self, ack: &CommandAck) -> Result<()> {
        let topic = format!("cc/{}/command/ack", self.station_id);
        let payload = serde_json::to_vec(ack)?;
        
        self.client
            .publish(&topic, QoS::AtLeastOnce, false, payload)
            .await
            .context("Failed to publish command ack")?;
        
        debug!("Published command ack for command_id: {}", ack.command_id);
        Ok(())
    }

    /// Get the station ID
    pub fn station_id(&self) -> &str {
        &self.station_id
    }

    /// Publish telemetry data
    pub async fn publish_telemetry(&self, telemetry: &TelemetryBundle) -> Result<()> {
        let topic = format!("cc/{}/telemetry", self.station_id);
        let payload = serde_json::to_vec(telemetry)?;
        
        self.client
            .publish(&topic, QoS::AtLeastOnce, false, payload)
            .await
            .context("Failed to publish telemetry")?;
        
        debug!("Published telemetry for station: {}", self.station_id);
        Ok(())
    }

    /// Publish station status
    pub async fn publish_status(&self, status: &StationStatus) -> Result<()> {
        let topic = format!("cc/{}/status", self.station_id);
        let payload = serde_json::to_vec(status)?;
        
        self.client
            .publish(&topic, QoS::AtLeastOnce, true, payload)
            .await
            .context("Failed to publish status")?;
        
        debug!("Published status for station: {}", self.station_id);
        Ok(())
    }

    /// Publish station descriptor
    pub async fn publish_descriptor(&self, descriptor: &StationDescriptor) -> Result<()> {
        let topic = format!("cc/{}/descriptor/announce", self.station_id);
        let payload = serde_json::to_vec(descriptor)?;
        
        self.client
            .publish(&topic, QoS::AtLeastOnce, true, payload)
            .await
            .context("Failed to publish descriptor")?;
        
        info!("Published station descriptor for: {}", self.station_id);
        Ok(())
    }
}

/// Telemetry bundle for MQTT publishing
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct TelemetryBundle {
    pub ts: i64,
    pub station_id: String,
    pub interval_ms: u32,
    pub values: Vec<TelemetryValue>,
}

impl TelemetryBundle {
    pub fn new(station_id: String, interval_ms: u32) -> Self {
        Self {
            ts: chrono::Utc::now().timestamp_millis(),
            station_id,
            interval_ms,
            values: Vec::new(),
        }
    }

    pub fn add_value(&mut self, key: &str, value: f64) {
        self.values.push(TelemetryValue {
            key: key.to_string(),
            v: value,
        });
    }

    /// Convert from StationRunningState to TelemetryBundle
    pub fn from_station_state(state: &crate::grpc::cc::StationRunningState, interval_ms: u32) -> Self {
        let mut bundle = TelemetryBundle::new(state.station_id.clone(), interval_ms);
        
        // Add CPU usage
        bundle.add_value("cpu_usage_percent", state.cpu as f64);
        
        // Add memory usage
        bundle.add_value("memory_used_mb", (state.current_memory as f64) / 1024.0 / 1024.0);
        
        // Add process count
        bundle.add_value("process_count", state.proc_count as f64);
        
        bundle
    }
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

impl StationStatus {
    pub fn online(station_id: String) -> Self {
        Self {
            station_id,
            online: true,
            last_seen: chrono::Utc::now().timestamp_millis(),
            version: None,
            alert: None,
        }
    }
}

/// Station descriptor
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct StationDescriptor {
    pub station_id: String,
    pub descriptors: Vec<TelemetryDescriptor>,
}

impl StationDescriptor {
    pub fn new(station_id: String) -> Self {
        Self {
            station_id,
            descriptors: Vec::new(),
        }
    }

    pub fn add_descriptor(&mut self, descriptor: TelemetryDescriptor) {
        self.descriptors.push(descriptor);
    }
}

/// Telemetry descriptor
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct TelemetryDescriptor {
    pub key: String,
    pub name: String,
    pub name_en: String,
    pub description: String,
    pub value_type: String,
    pub unit: String,
    pub range: TelemetryRange,
    pub update_interval_ms: u32,
    pub aggregation: Vec<String>,
    pub alert: Option<AlertThreshold>,
}

/// Telemetry range
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct TelemetryRange {
    pub min: f64,
    pub max: f64,
}

/// Alert threshold
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct AlertThreshold {
    pub warning: f64,
    pub critical: f64,
}
