//! MQTT client module for CC-Aggregator
//! 
//! This module implements the MQTT client functionality using rumqttc.
//! It handles connection to the message broker and manages topic subscriptions/publishing.

use anyhow::{Context, Result};
use rumqttc::{AsyncClient, Event, MqttOptions, Packet, QoS};
use serde::{Deserialize, Serialize};
use std::time::Duration;
use tokio::sync::mpsc;
use tracing::{debug, error, info, warn};

use crate::telemetry::TelemetryBundle;

/// MQTT topic constants
pub const TOPIC_TELEMETRY: &str = "cc/{station_id}/telemetry";
pub const TOPIC_STATUS: &str = "cc/{station_id}/status";
pub const TOPIC_DESCRIPTOR: &str = "cc/{station_id}/descriptor/announce";
pub const TOPIC_COMMAND: &str = "cc/{station_id}/command";
pub const TOPIC_COMMAND_ACK: &str = "cc/{station_id}/command/ack";
pub const TOPIC_CONFIG_SET: &str = "cc/{station_id}/config/set";

/// MQTT client wrapper for the aggregator
pub struct MqttClient {
    client: AsyncClient,
    eventloop: rumqttc::EventLoop,
}

impl MqttClient {
    /// Create a new MQTT client connection
    pub fn new(broker_host: &str, broker_port: u16, client_id: &str) -> Result<Self> {
        let mut mqttoptions = MqttOptions::new(client_id, broker_host, broker_port);
        mqttoptions.set_keep_alive(Duration::from_secs(30));
        mqttoptions.set_clean_session(true);

        let (client, eventloop) = AsyncClient::new(mqttoptions, 100);

        info!("MQTT client created: broker={}:{}, client_id={}", 
              broker_host, broker_port, client_id);

        Ok(Self {
            client,
            eventloop,
        })
    }

    /// Subscribe to a topic pattern
    pub async fn subscribe(&self, topic: &str, qos: QoS) -> Result<()> {
        self.client
            .subscribe(topic, qos)
            .await
            .context(format!("Failed to subscribe to topic: {}", topic))?;
        
        info!("Subscribed to topic: {}", topic);
        Ok(())
    }

    /// Subscribe to all station topics using wildcard
    pub async fn subscribe_all(&self) -> Result<()> {
        // Subscribe to command topics for all stations
        self.subscribe("cc/+/command", QoS::AtLeastOnce).await?;
        self.subscribe("cc/+/config/set", QoS::AtLeastOnce).await?;
        
        // Subscribe to telemetry and status for all stations
        self.subscribe("cc/+/telemetry", QoS::AtLeastOnce).await?;
        self.subscribe("cc/+/status", QoS::AtLeastOnce).await?;
        self.subscribe("cc/+/descriptor/announce", QoS::AtLeastOnce).await?;
        
        info!("Subscribed to all CC topics");
        Ok(())
    }

    /// Publish telemetry data
    pub async fn publish_telemetry(&self, station_id: &str, telemetry: &TelemetryBundle) -> Result<()> {
        let topic = format!("cc/{}/telemetry", station_id);
        let payload = serde_json::to_vec(telemetry)?;
        
        self.client
            .publish(&topic, QoS::AtLeastOnce, false, payload)
            .await
            .context("Failed to publish telemetry")?;
        
        debug!("Published telemetry for station: {}", station_id);
        Ok(())
    }

    /// Publish station status
    pub async fn publish_status(&self, station_id: &str, status: &StationStatus) -> Result<()> {
        let topic = format!("cc/{}/status", station_id);
        let payload = serde_json::to_vec(status)?;
        
        self.client
            .publish(&topic, QoS::AtLeastOnce, true, payload)
            .await
            .context("Failed to publish status")?;
        
        debug!("Published status for station: {}", station_id);
        Ok(())
    }

    /// Publish command acknowledgment
    pub async fn publish_command_ack(&self, station_id: &str, ack: &CommandAck) -> Result<()> {
        let topic = format!("cc/{}/command/ack", station_id);
        let payload = serde_json::to_vec(ack)?;
        
        self.client
            .publish(&topic, QoS::AtLeastOnce, false, payload)
            .await
            .context("Failed to publish command ack")?;
        
        Ok(())
    }

    /// Get mutable reference to event loop
    pub fn get_eventloop_mut(&mut self) -> &mut rumqttc::EventLoop {
        &mut self.eventloop
    }

    /// Consume self and return the event loop
    pub fn into_eventloop(self) -> rumqttc::EventLoop {
        self.eventloop
    }
}

/// Station status information
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

    pub fn offline(station_id: String) -> Self {
        Self {
            station_id,
            online: false,
            last_seen: chrono::Utc::now().timestamp_millis(),
            version: None,
            alert: None,
        }
    }
}

/// Command acknowledgment
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct CommandAck {
    pub command_id: String,
    pub station_id: String,
    pub success: bool,
    pub message: Option<String>,
    pub timestamp: i64,
}

impl CommandAck {
    pub fn success(command_id: String, station_id: String) -> Self {
        Self {
            command_id,
            station_id,
            success: true,
            message: None,
            timestamp: chrono::Utc::now().timestamp_millis(),
        }
    }

    pub fn failure(command_id: String, station_id: String, message: String) -> Self {
        Self {
            command_id,
            station_id,
            success: false,
            message: Some(message),
            timestamp: chrono::Utc::now().timestamp_millis(),
        }
    }
}

/// Incoming MQTT message wrapper
#[derive(Debug, Clone)]
pub struct MqttMessage {
    pub topic: String,
    pub payload: Vec<u8>,
    pub qos: QoS,
}

impl MqttMessage {
    /// Parse the topic to extract station_id and message type
    pub fn parse_topic(&self) -> Option<(String, String)> {
        let parts: Vec<&str> = self.topic.split('/').collect();
        if parts.len() >= 3 && parts[0] == "cc" {
            Some((parts[1].to_string(), parts[2].to_string()))
        } else {
            None
        }
    }

    /// Parse payload as JSON
    pub fn parse_json<T: serde::de::DeserializeOwned>(&self) -> Option<T> {
        serde_json::from_slice(&self.payload).ok()
    }
}

/// Event loop handler for processing MQTT events
pub async fn handle_mqtt_events(
    eventloop: &mut rumqttc::EventLoop,
    message_sender: &mpsc::Sender<MqttMessage>,
) -> Result<()> {
    loop {
        match eventloop.poll().await {
            Ok(Event::Incoming(Packet::Publish(publish))) => {
                let message = MqttMessage {
                    topic: publish.topic.clone(),
                    payload: publish.payload.to_vec(),
                    qos: publish.qos,
                };
                
                if let Err(e) = message_sender.send(message).await {
                    warn!("Failed to send MQTT message to channel: {:?}", e);
                    break;
                }
            }
            Ok(Event::Incoming(Packet::ConnAck(_connack))) => {
                info!("MQTT connected");
            }
            Ok(Event::Incoming(Packet::PingResp)) => {
                debug!("MQTT ping response received");
            }
            Ok(Event::Outgoing(_outgoing)) => {
                // Outgoing events - debug log if needed
            }
            Ok(_) => {
                // Other events we don't care about
            }
            Err(e) => {
                error!("MQTT error: {:?}", e);
                // Reconnect logic could be added here
                tokio::time::sleep(Duration::from_secs(1)).await;
            }
        }
    }
    
    Ok(())
}
