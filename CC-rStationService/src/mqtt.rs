//! MQTT client module for CC-rStationService
//! 
//! This module implements MQTT telemetry publishing for the station service.
//! It publishes telemetry data to the CC-Aggregator via MQTT.

use anyhow::{Context, Result};
use rumqttc::{AsyncClient, MqttOptions, QoS};
use serde::{Deserialize, Serialize};
use std::time::Duration;
use tracing::{debug, error, info};

/// MQTT client wrapper for the station service
#[derive(Clone)]
pub struct MqttClient {
    client: AsyncClient,
    station_id: String,
}

impl MqttClient {
    /// Create a new MQTT client connection
    pub fn new(broker_host: &str, broker_port: u16, station_id: &str) -> Result<Self> {
        let client_id = format!("cc-station-{}", station_id);
        let mut mqttoptions = MqttOptions::new(client_id, broker_host, broker_port);
        mqttoptions.set_keep_alive(Duration::from_secs(30));
        mqttoptions.set_clean_session(true);

        let (client, mut eventloop) = AsyncClient::new(mqttoptions, 100);

        // Start event loop in background
        tokio::spawn(async move {
            loop {
                match eventloop.poll().await {
                    Ok(_) => {}
                    Err(e) => {
                        error!("MQTT event loop error: {:?}", e);
                        tokio::time::sleep(Duration::from_secs(1)).await;
                    }
                }
            }
        });

        info!("MQTT client created: broker={}:{}, station_id={}", 
              broker_host, broker_port, station_id);

        Ok(Self {
            client,
            station_id: station_id.to_string(),
        })
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
