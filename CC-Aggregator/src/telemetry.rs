//! Telemetry data structures for CC-Aggregator
//! 
//! This module defines the self-describing telemetry format using JSON.
//! Unlike Protobuf, this supports runtime registration of new telemetry items.

use serde::{Deserialize, Serialize};

/// Telemetry value with dynamic key
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct TelemetryValue {
    /// Telemetry key (e.g., "cpu_usage_percent")
    pub key: String,
    /// Telemetry value (stored as f64 for numeric values)
    pub v: f64,
}

/// Telemetry bundle sent by stations
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct TelemetryBundle {
    /// Timestamp in milliseconds
    pub ts: i64,
    /// Station identifier
    pub station_id: String,
    /// Collection interval in milliseconds
    pub interval_ms: u32,
    /// Telemetry values
    pub values: Vec<TelemetryValue>,
}

impl TelemetryBundle {
    /// Create a new telemetry bundle
    pub fn new(station_id: String, interval_ms: u32) -> Self {
        Self {
            ts: chrono::Utc::now().timestamp_millis(),
            station_id,
            interval_ms,
            values: Vec::new(),
        }
    }

    /// Add a telemetry value
    pub fn add_value(&mut self, key: &str, value: f64) {
        self.values.push(TelemetryValue {
            key: key.to_string(),
            v: value,
        });
    }

    /// Get a value by key
    pub fn get(&self, key: &str) -> Option<f64> {
        self.values.iter().find(|v| v.key == key).map(|v| v.v)
    }
}

/// Telemetry descriptor - describes a single telemetry metric
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct TelemetryDescriptor {
    /// Unique key for this metric (e.g., "cpu_usage_percent")
    pub key: String,
    /// Display name in Chinese
    pub name: String,
    /// Display name in English
    pub name_en: String,
    /// Human-readable description
    pub description: String,
    /// Value type (float, int, string, json)
    pub value_type: String,
    /// Unit of measurement
    pub unit: String,
    /// Valid range
    pub range: TelemetryRange,
    /// Update interval in milliseconds
    pub update_interval_ms: u32,
    /// Supported aggregation methods
    pub aggregation: Vec<String>,
    /// Alert thresholds
    pub alert: Option<AlertThreshold>,
}

/// Telemetry value range
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct TelemetryRange {
    pub min: f64,
    pub max: f64,
}

impl Default for TelemetryRange {
    fn default() -> Self {
        Self { min: 0.0, max: 100.0 }
    }
}

/// Alert thresholds
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct AlertThreshold {
    pub warning: f64,
    pub critical: f64,
}

/// Station descriptor - all telemetry descriptors for a station
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct StationDescriptor {
    pub station_id: String,
    pub descriptors: Vec<TelemetryDescriptor>,
}

impl StationDescriptor {
    /// Create a new station descriptor
    pub fn new(station_id: String) -> Self {
        Self {
            station_id,
            descriptors: Vec::new(),
        }
    }

    /// Add a telemetry descriptor
    pub fn add_descriptor(&mut self, descriptor: TelemetryDescriptor) {
        self.descriptors.push(descriptor);
    }

    /// Get a descriptor by key
    pub fn get(&self, key: &str) -> Option<&TelemetryDescriptor> {
        self.descriptors.iter().find(|d| d.key == key)
    }
}

/// Default telemetry descriptors for standard station metrics
pub fn default_descriptors() -> Vec<TelemetryDescriptor> {
    vec![
        TelemetryDescriptor {
            key: "cpu_usage_percent".to_string(),
            name: "CPU使用率".to_string(),
            name_en: "CPU Usage".to_string(),
            description: "最近采样周期的CPU占用百分比".to_string(),
            value_type: "float".to_string(),
            unit: "%".to_string(),
            range: TelemetryRange { min: 0.0, max: 100.0 },
            update_interval_ms: 1000,
            aggregation: vec!["avg".to_string(), "max".to_string(), "min".to_string()],
            alert: Some(AlertThreshold { warning: 80.0, critical: 95.0 }),
        },
        TelemetryDescriptor {
            key: "memory_used_mb".to_string(),
            name: "已用内存".to_string(),
            name_en: "Memory Used".to_string(),
            description: "当前已使用的物理内存".to_string(),
            value_type: "float".to_string(),
            unit: "MB".to_string(),
            range: TelemetryRange { min: 0.0, max: 100000.0 },
            update_interval_ms: 1000,
            aggregation: vec!["avg".to_string(), "max".to_string()],
            alert: Some(AlertThreshold { warning: 80.0, critical: 95.0 }),
        },
        TelemetryDescriptor {
            key: "memory_available_mb".to_string(),
            name: "可用内存".to_string(),
            name_en: "Memory Available".to_string(),
            description: "当前可用的物理内存".to_string(),
            value_type: "float".to_string(),
            unit: "MB".to_string(),
            range: TelemetryRange { min: 0.0, max: 100000.0 },
            update_interval_ms: 1000,
            aggregation: vec!["min".to_string()],
            alert: None,
        },
        TelemetryDescriptor {
            key: "network_rx_bytes_per_sec".to_string(),
            name: "网络接收速率".to_string(),
            name_en: "Network RX Rate".to_string(),
            description: "网络接口每秒接收字节数".to_string(),
            value_type: "float".to_string(),
            unit: "B/s".to_string(),
            range: TelemetryRange { min: 0.0, max: 1e10 },
            update_interval_ms: 1000,
            aggregation: vec!["avg".to_string(), "max".to_string()],
            alert: None,
        },
        TelemetryDescriptor {
            key: "network_tx_bytes_per_sec".to_string(),
            name: "网络发送速率".to_string(),
            name_en: "Network TX Rate".to_string(),
            description: "网络接口每秒发送字节数".to_string(),
            value_type: "float".to_string(),
            unit: "B/s".to_string(),
            range: TelemetryRange { min: 0.0, max: 1e10 },
            update_interval_ms: 1000,
            aggregation: vec!["avg".to_string(), "max".to_string()],
            alert: None,
        },
        TelemetryDescriptor {
            key: "disk_usage_percent".to_string(),
            name: "磁盘使用率".to_string(),
            name_en: "Disk Usage".to_string(),
            description: "根分区磁盘使用百分比".to_string(),
            value_type: "float".to_string(),
            unit: "%".to_string(),
            range: TelemetryRange { min: 0.0, max: 100.0 },
            update_interval_ms: 60000,
            aggregation: vec!["max".to_string()],
            alert: Some(AlertThreshold { warning: 80.0, critical: 95.0 }),
        },
        TelemetryDescriptor {
            key: "process_count".to_string(),
            name: "进程数量".to_string(),
            name_en: "Process Count".to_string(),
            description: "当前运行的进程/线程数量".to_string(),
            value_type: "int".to_string(),
            unit: "count".to_string(),
            range: TelemetryRange { min: 0.0, max: 100000.0 },
            update_interval_ms: 5000,
            aggregation: vec!["avg".to_string(), "max".to_string()],
            alert: Some(AlertThreshold { warning: 500.0, critical: 1000.0 }),
        },
    ]
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_telemetry_bundle() {
        let mut bundle = TelemetryBundle::new("test-station".to_string(), 1000);
        bundle.add_value("cpu_usage_percent", 45.5);
        bundle.add_value("memory_used_mb", 8192.0);
        
        assert_eq!(bundle.values.len(), 2);
        assert_eq!(bundle.get("cpu_usage_percent"), Some(45.5));
        assert_eq!(bundle.get("memory_used_mb"), Some(8192.0));
        assert_eq!(bundle.get("nonexistent"), None);
    }

    #[test]
    fn test_telemetry_serialization() {
        let mut bundle = TelemetryBundle::new("test-station".to_string(), 1000);
        bundle.add_value("cpu_usage_percent", 45.5);
        
        let json = serde_json::to_string(&bundle).unwrap();
        let parsed: TelemetryBundle = serde_json::from_str(&json).unwrap();
        
        assert_eq!(parsed.station_id, "test-station");
        assert_eq!(parsed.values[0].key, "cpu_usage_percent");
        assert_eq!(parsed.values[0].v, 45.5);
    }
}
