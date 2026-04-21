//! Network Monitor Plugin
//!
//! This plugin collects network usage metrics using the network_counters module.
//! It provides TCP/UDP statistics and per-interface network traffic metrics.

use crate::network_counters;
use crate::plugins::{
    PluginCategory, PluginConfig, PluginError, PluginMetadata, PluginResult, PluginStats,
    PluginStatus, TelemetryData, TelemetryDataPoint, TelemetryPlugin,
};
use chrono::Utc;
use std::sync::RwLock;

/// Network Monitor Plugin implementation
pub struct NetworkMonitorPlugin {
    metadata: PluginMetadata,
    status: RwLock<PluginStatus>,
    config: RwLock<PluginConfig>,
    stats: RwLock<PluginStats>,
    /// Cache for previous interface counters to calculate rates
    prev_interface_counters: RwLock<Vec<network_counters::InterfaceCounterSnapshot>>,
    /// Previous collection timestamp for rate calculation
    prev_collection_time: RwLock<Option<chrono::DateTime<chrono::Utc>>>,
}

impl NetworkMonitorPlugin {
    /// Create a new Network monitor plugin
    pub fn new() -> Self {
        Self {
            metadata: PluginMetadata::new(
                "cc-network-monitor".to_string(),
                "Network Monitor".to_string(),
                "1.0.0",
            )
            .with_description("Monitors network usage including TCP/UDP stats and interface traffic"),
            status: RwLock::new(PluginStatus::Stopped),
            config: RwLock::new(PluginConfig::new()),
            stats: RwLock::new(PluginStats::new()),
            prev_interface_counters: RwLock::new(Vec::new()),
            prev_collection_time: RwLock::new(None),
        }
    }

    /// Calculate bytes per second between two samples
    fn calculate_rate(current: u64, previous: u64, time_diff_secs: f64) -> f64 {
        if time_diff_secs > 0.0 {
            (current as f64 - previous as f64) / time_diff_secs
        } else {
            0.0
        }
    }
}

impl Default for NetworkMonitorPlugin {
    fn default() -> Self {
        Self::new()
    }
}

impl TelemetryPlugin for NetworkMonitorPlugin {
    fn metadata(&self) -> &PluginMetadata {
        &self.metadata
    }

    fn status(&self) -> PluginStatus {
        *self.status.read().unwrap()
    }

    fn init(&mut self, config: &PluginConfig) -> PluginResult<()> {
        *self.config.write().unwrap() = config.clone();

        // Validate config
        self.validate_config(config)?;

        *self.status.write().unwrap() = PluginStatus::Running;
        tracing::info!(
            "[NetworkMonitorPlugin] Initialized with interval: {}s",
            self.config.read().unwrap().interval_seconds
        );
        Ok(())
    }

    fn start(&self) -> PluginResult<()> {
        if *self.status.read().unwrap() == PluginStatus::Running {
            return Err(PluginError::AlreadyInitialized(self.metadata.id.clone()));
        }
        *self.status.write().unwrap() = PluginStatus::Running;
        tracing::info!("[NetworkMonitorPlugin] Started");
        Ok(())
    }

    fn stop(&self) -> PluginResult<()> {
        *self.status.write().unwrap() = PluginStatus::Stopped;
        tracing::info!("[NetworkMonitorPlugin] Stopped");
        Ok(())
    }

    fn collect(&self) -> PluginResult<TelemetryData> {
        let collection_start = Utc::now();

        // Collect network counters
        let snapshot = network_counters::collect().map_err(|e| {
            PluginError::CollectionFailed(self.metadata.id.clone(), e.to_string())
        })?;

        let mut telemetry = TelemetryData::new(self.metadata.id.clone());
        telemetry.collection_start = collection_start;

        // Calculate time difference for rate calculation
        let time_diff_secs = {
            let prev_time = self.prev_collection_time.read().unwrap();
            let diff = prev_time.map(|pt| (collection_start - pt).num_seconds() as f64);
            drop(prev_time);  // Release read lock before acquiring write lock
            *self.prev_collection_time.write().unwrap() = Some(collection_start);
            diff.unwrap_or(0.0)
        };

        // TCP metrics
        telemetry.add_data_point(
            TelemetryDataPoint::new("network_tcp_segments_received", snapshot.segments_received as f64, "count")
                .with_label("protocol", "tcp")
                .with_label("type", "segments"),
        );
        telemetry.add_data_point(
            TelemetryDataPoint::new("network_tcp_segments_sent", snapshot.segments_sent as f64, "count")
                .with_label("protocol", "tcp")
                .with_label("type", "segments"),
        );
        telemetry.add_data_point(
            TelemetryDataPoint::new("network_tcp_errors", snapshot.errors_received as f64, "count")
                .with_label("protocol", "tcp"),
        );
        telemetry.add_data_point(
            TelemetryDataPoint::new("network_tcp_current_connections", snapshot.current_connections as f64, "count")
                .with_label("protocol", "tcp"),
        );
        telemetry.add_data_point(
            TelemetryDataPoint::new("network_tcp_reset_connections", snapshot.reset_connections as f64, "count")
                .with_label("protocol", "tcp"),
        );

        // UDP metrics
        telemetry.add_data_point(
            TelemetryDataPoint::new("network_udp_datagrams_received", snapshot.datagrams_received as f64, "count")
                .with_label("protocol", "udp"),
        );
        telemetry.add_data_point(
            TelemetryDataPoint::new("network_udp_datagrams_sent", snapshot.datagrams_sent as f64, "count")
                .with_label("protocol", "udp"),
        );
        telemetry.add_data_point(
            TelemetryDataPoint::new("network_udp_datagrams_discarded", snapshot.datagrams_discarded as f64, "count")
                .with_label("protocol", "udp"),
        );
        telemetry.add_data_point(
            TelemetryDataPoint::new("network_udp_datagrams_errors", snapshot.datagrams_with_errors as f64, "count")
                .with_label("protocol", "udp"),
        );

        // Per-interface metrics
        let prev_counters = self.prev_interface_counters.read().unwrap();
        for interface in &snapshot.interface_counters {
            let interface_name = &interface.if_name;

            // Find previous counter for this interface to calculate rates
            let (rx_rate, tx_rate) = if time_diff_secs > 0.0 {
                if let Some(prev) = prev_counters.iter().find(|p| p.if_name == *interface_name) {
                    (
                        Self::calculate_rate(interface.bytes_received, prev.bytes_received, time_diff_secs),
                        Self::calculate_rate(interface.bytes_sented, prev.bytes_sented, time_diff_secs),
                    )
                } else {
                    (0.0, 0.0)
                }
            } else {
                (0.0, 0.0)
            };

            // Bytes received/sent
            telemetry.add_data_point(
                TelemetryDataPoint::new("network_interface_bytes_received", interface.bytes_received as f64, "bytes")
                    .with_label("interface", interface_name),
            );
            telemetry.add_data_point(
                TelemetryDataPoint::new("network_interface_bytes_sent", interface.bytes_sented as f64, "bytes")
                    .with_label("interface", interface_name),
            );

            // Bytes per second rates
            telemetry.add_data_point(
                TelemetryDataPoint::new("network_interface_rx_bytes_per_sec", rx_rate, "bytes_per_sec")
                    .with_label("interface", interface_name),
            );
            telemetry.add_data_point(
                TelemetryDataPoint::new("network_interface_tx_bytes_per_sec", tx_rate, "bytes_per_sec")
                    .with_label("interface", interface_name),
            );

            // Unicast packets
            telemetry.add_data_point(
                TelemetryDataPoint::new("network_interface_unicast_packets_received", interface.unicast_packet_received as f64, "count")
                    .with_label("interface", interface_name),
            );
            telemetry.add_data_point(
                TelemetryDataPoint::new("network_interface_unicast_packets_sent", interface.unicast_packet_sented as f64, "count")
                    .with_label("interface", interface_name),
            );

            // Multicast packets
            telemetry.add_data_point(
                TelemetryDataPoint::new("network_interface_multicast_packets_received", interface.multicast_packet_received as f64, "count")
                    .with_label("interface", interface_name),
            );
            telemetry.add_data_point(
                TelemetryDataPoint::new("network_interface_multicast_packets_sent", interface.multicast_packet_sented as f64, "count")
                    .with_label("interface", interface_name),
            );
        }

        // Store current counters for next collection
        *self.prev_interface_counters.write().unwrap() = snapshot.interface_counters;

        // Update stats
        let collection_end = Utc::now();
        let bytes = serde_json::to_string(&telemetry)
            .map(|s| s.len() as u64)
            .unwrap_or(0);
        self.stats.write().unwrap().record_collection(bytes);

        telemetry.collection_end = collection_end;

        tracing::debug!(
            "[NetworkMonitorPlugin] Collected {} data points",
            telemetry.data_points.len()
        );

        Ok(telemetry)
    }

    fn interval(&self) -> u64 {
        self.config.read().unwrap().interval_seconds
    }

    fn stats(&self) -> PluginStats {
        self.stats.read().unwrap().clone()
    }

    fn category(&self) -> PluginCategory {
        PluginCategory::Network
    }

    fn config(&self) -> Option<&PluginConfig> {
        None
    }

    fn is_running(&self) -> bool {
        *self.status.read().unwrap() == PluginStatus::Running
    }

    fn validate_config(&self, config: &PluginConfig) -> PluginResult<()> {
        if config.interval_seconds == 0 {
            return Err(PluginError::ConfigError(
                self.metadata.id.clone(),
                "interval_seconds must be greater than 0".to_string(),
            ));
        }
        Ok(())
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_network_monitor_plugin_creation() {
        let plugin = NetworkMonitorPlugin::new();
        assert_eq!(plugin.metadata.id, "cc-network-monitor");
        assert_eq!(plugin.metadata.name, "Network Monitor");
        assert_eq!(plugin.category(), PluginCategory::Network);
    }

    #[test]
    fn test_network_monitor_plugin_collection() {
        let mut plugin = NetworkMonitorPlugin::new();
        plugin.init(&PluginConfig::new()).unwrap();

        let telemetry = plugin.collect().unwrap();

        // Should have multiple data points
        assert!(!telemetry.is_empty());

        // Should have TCP metrics
        let tcp_points: Vec<_> = telemetry
            .data_points
            .iter()
            .filter(|p| p.labels.get("protocol").map(|p| p == "tcp").unwrap_or(false))
            .collect();
        assert!(!tcp_points.is_empty());

        // Should have UDP metrics
        let udp_points: Vec<_> = telemetry
            .data_points
            .iter()
            .filter(|p| p.labels.get("protocol").map(|p| p == "udp").unwrap_or(false))
            .collect();
        assert!(!udp_points.is_empty());
    }

    #[test]
    fn test_network_monitor_plugin_status() {
        let mut plugin = NetworkMonitorPlugin::new();
        assert_eq!(plugin.status(), PluginStatus::Stopped);

        plugin.init(&PluginConfig::new()).unwrap();
        assert_eq!(plugin.status(), PluginStatus::Running);

        plugin.stop().unwrap();
        assert_eq!(plugin.status(), PluginStatus::Stopped);
    }

    #[test]
    fn test_network_monitor_plugin_rate_calculation() {
        let mut plugin = NetworkMonitorPlugin::new();
        plugin.init(&PluginConfig::new()).unwrap();

        // First collection - should get base values with 0 rates
        let telemetry1 = plugin.collect().unwrap();
        assert!(!telemetry1.is_empty());

        // Second collection - should get rate calculations
        let telemetry2 = plugin.collect().unwrap();
        assert!(!telemetry2.is_empty());
    }
}
