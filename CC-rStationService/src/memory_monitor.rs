//! Memory Monitor Plugin
//!
//! This plugin collects memory usage metrics using the sysinfo crate.
//! It provides total, used, available memory and usage percentage.

use crate::plugins::{
    PluginCategory, PluginConfig, PluginError, PluginMetadata, PluginResult, PluginStats,
    PluginStatus, TelemetryData, TelemetryDataPoint, TelemetryPlugin,
};
use chrono::Utc;
use std::sync::RwLock;
use sysinfo::{MemoryRefreshKind, RefreshKind, System};

/// Memory Monitor Plugin implementation
pub struct MemoryMonitorPlugin {
    metadata: PluginMetadata,
    status: RwLock<PluginStatus>,
    config: RwLock<PluginConfig>,
    stats: RwLock<PluginStats>,
    system: RwLock<System>,
}

impl MemoryMonitorPlugin {
    /// Create a new Memory monitor plugin
    pub fn new() -> Self {
        let mut system = System::new_with_specifics(
            RefreshKind::nothing().with_memory(MemoryRefreshKind::everything()),
        );

        // Give sysinfo time to initialize memory data
        std::thread::sleep(std::time::Duration::from_millis(100));
        system.refresh_memory();

        Self {
            metadata: PluginMetadata::new(
                "cc-memory-monitor".to_string(),
                "Memory Monitor".to_string(),
                "1.0.0",
            )
            .with_description("Monitors memory usage using sysinfo"),
            status: RwLock::new(PluginStatus::Stopped),
            config: RwLock::new(PluginConfig::new()),
            stats: RwLock::new(PluginStats::new()),
            system: RwLock::new(system),
        }
    }

    /// Get memory usage details
    fn get_memory_usage(system: &System) -> (u64, u64, u64, f64) {
        let total = system.total_memory();
        let used = system.used_memory();
        let available = system.available_memory();

        // Calculate usage percentage, handling edge cases
        let usage_percent = if total > 0 {
            (used as f64 / total as f64) * 100.0
        } else {
            0.0
        };

        (total, used, available, usage_percent)
    }
}

impl Default for MemoryMonitorPlugin {
    fn default() -> Self {
        Self::new()
    }
}

impl TelemetryPlugin for MemoryMonitorPlugin {
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

        // Re-initialize system with memory refresh kind
        let mut system = System::new_with_specifics(
            RefreshKind::nothing().with_memory(MemoryRefreshKind::everything()),
        );
        *self.system.write().unwrap() = system;

        // Give sysinfo time to initialize memory data
        std::thread::sleep(std::time::Duration::from_millis(100));
        self.system.write().unwrap().refresh_memory();

        *self.status.write().unwrap() = PluginStatus::Running;
        tracing::info!(
            "[MemoryMonitorPlugin] Initialized with interval: {}s",
            self.config.read().unwrap().interval_seconds
        );
        Ok(())
    }

    fn start(&self) -> PluginResult<()> {
        if *self.status.read().unwrap() == PluginStatus::Running {
            return Err(PluginError::AlreadyInitialized(self.metadata.id.clone()));
        }
        *self.status.write().unwrap() = PluginStatus::Running;
        tracing::info!("[MemoryMonitorPlugin] Started");
        Ok(())
    }

    fn stop(&self) -> PluginResult<()> {
        *self.status.write().unwrap() = PluginStatus::Stopped;
        tracing::info!("[MemoryMonitorPlugin] Stopped");
        Ok(())
    }

    fn collect(&self) -> PluginResult<TelemetryData> {
        let _start_time = Utc::now();

        // Refresh memory data
        {
            let mut system = self.system.write().unwrap();
            system.refresh_memory();
        }

        let system = self.system.read().unwrap();

        let mut telemetry = TelemetryData::new(self.metadata.id.clone());

        // Get memory usage
        let (total, used, available, usage_percent) = Self::get_memory_usage(&system);

        // Total memory
        let total_data_point =
            TelemetryDataPoint::new("memory_total_bytes", total as f64, "bytes")
                .with_label("type", "total");
        telemetry.add_data_point(total_data_point);

        // Used memory
        let used_data_point =
            TelemetryDataPoint::new("memory_used_bytes", used as f64, "bytes")
                .with_label("type", "used");
        telemetry.add_data_point(used_data_point);

        // Available memory
        let available_data_point =
            TelemetryDataPoint::new("memory_available_bytes", available as f64, "bytes")
                .with_label("type", "available");
        telemetry.add_data_point(available_data_point);

        // Memory usage percentage
        let usage_data_point =
            TelemetryDataPoint::new("memory_usage_percent", usage_percent, "percent")
                .with_label("type", "usage");
        telemetry.add_data_point(usage_data_point);

        // Update stats
        let collection_end = Utc::now();
        let bytes = serde_json::to_string(&telemetry)
            .map(|s| s.len() as u64)
            .unwrap_or(0);
        self.stats.write().unwrap().record_collection(bytes);

        telemetry.collection_end = collection_end;

        tracing::debug!(
            "[MemoryMonitorPlugin] Collected {} data points, memory usage: {:.2}%",
            telemetry.data_points.len(),
            usage_percent
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
        PluginCategory::System
    }

    fn config(&self) -> Option<&PluginConfig> {
        // Return None since config is stored behind a lock and can't be borrowed safely
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
    fn test_memory_monitor_plugin_creation() {
        let plugin = MemoryMonitorPlugin::new();
        assert_eq!(plugin.metadata.id, "cc-memory-monitor");
        assert_eq!(plugin.metadata.name, "Memory Monitor");
        assert_eq!(plugin.category(), PluginCategory::System);
    }

    #[test]
    fn test_memory_monitor_plugin_collection() {
        let mut plugin = MemoryMonitorPlugin::new();
        plugin.init(&PluginConfig::new()).unwrap();

        let telemetry = plugin.collect().unwrap();

        // Should have multiple data points
        assert!(!telemetry.is_empty());

        // Should have total memory
        let total_points: Vec<_> = telemetry
            .data_points
            .iter()
            .filter(|p| {
                p.labels
                    .get("type")
                    .map(|t| t == "total")
                    .unwrap_or(false)
            })
            .collect();
        assert!(!total_points.is_empty());

        // Should have usage percent
        let usage_points: Vec<_> = telemetry
            .data_points
            .iter()
            .filter(|p| {
                p.labels
                    .get("type")
                    .map(|t| t == "usage")
                    .unwrap_or(false)
            })
            .collect();
        assert!(!usage_points.is_empty());
    }

    #[test]
    fn test_memory_monitor_plugin_status() {
        let mut plugin = MemoryMonitorPlugin::new();
        assert_eq!(plugin.status(), PluginStatus::Stopped);

        plugin.init(&PluginConfig::new()).unwrap();
        assert_eq!(plugin.status(), PluginStatus::Running);

        plugin.stop().unwrap();
        assert_eq!(plugin.status(), PluginStatus::Stopped);
    }
}
