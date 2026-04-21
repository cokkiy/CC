//! Disk Monitor Plugin
//!
//! This plugin collects disk usage metrics using the sysinfo crate.
//! It provides total, used, available space and usage percentage per disk.

use crate::plugins::{
    PluginCategory, PluginConfig, PluginError, PluginMetadata, PluginResult, PluginStats,
    PluginStatus, TelemetryData, TelemetryDataPoint, TelemetryPlugin,
};
use chrono::Utc;
use std::sync::RwLock;
use sysinfo::Disks;

/// Disk Monitor Plugin implementation
pub struct DiskMonitorPlugin {
    metadata: PluginMetadata,
    status: RwLock<PluginStatus>,
    config: RwLock<PluginConfig>,
    stats: RwLock<PluginStats>,
    disks: RwLock<Disks>,
}

impl DiskMonitorPlugin {
    /// Create a new Disk monitor plugin
    pub fn new() -> Self {
        let disks = Disks::new_with_refreshed_list();

        Self {
            metadata: PluginMetadata::new(
                "cc-disk-monitor".to_string(),
                "Disk Monitor".to_string(),
                "1.0.0",
            )
            .with_description("Monitors disk usage across all disks using sysinfo"),
            status: RwLock::new(PluginStatus::Stopped),
            config: RwLock::new(PluginConfig::new()),
            stats: RwLock::new(PluginStats::new()),
            disks: RwLock::new(disks),
        }
    }

    /// Get disk usage details for all disks
    fn get_disk_usage(disks: &Disks) -> Vec<(String, u64, u64, u64, f64)> {
        disks
            .iter()
            .map(|disk| {
                let mount_point = disk.mount_point().display().to_string();
                let total = disk.total_space();
                let available = disk.available_space();
                let used = total.saturating_sub(available);

                // Calculate usage percentage, handling edge cases
                let usage_percent = if total > 0 {
                    (used as f64 / total as f64) * 100.0
                } else {
                    0.0
                };

                (mount_point, total, used, available, usage_percent)
            })
            .collect()
    }

    /// Get per-disk I/O statistics (read/write bytes if available)
    fn get_disk_io_stats(_disks: &Disks) -> Vec<(String, u64, u64)> {
        // Note: sysinfo's Disks API doesn't directly provide I/O statistics.
        // For now, return empty vector. I/O stats could be enhanced via platform-specific APIs.
        Vec::new()
    }
}

impl Default for DiskMonitorPlugin {
    fn default() -> Self {
        Self::new()
    }
}

impl TelemetryPlugin for DiskMonitorPlugin {
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

        // Re-initialize disks
        let disks = Disks::new_with_refreshed_list();
        *self.disks.write().unwrap() = disks;

        *self.status.write().unwrap() = PluginStatus::Running;
        tracing::info!(
            "[DiskMonitorPlugin] Initialized with interval: {}s",
            self.config.read().unwrap().interval_seconds
        );
        Ok(())
    }

    fn start(&self) -> PluginResult<()> {
        if *self.status.read().unwrap() == PluginStatus::Running {
            return Err(PluginError::AlreadyInitialized(self.metadata.id.clone()));
        }
        *self.status.write().unwrap() = PluginStatus::Running;
        tracing::info!("[DiskMonitorPlugin] Started");
        Ok(())
    }

    fn stop(&self) -> PluginResult<()> {
        *self.status.write().unwrap() = PluginStatus::Stopped;
        tracing::info!("[DiskMonitorPlugin] Stopped");
        Ok(())
    }

    fn collect(&self) -> PluginResult<TelemetryData> {
        let _start_time = Utc::now();

        // Refresh disk data
        {
            let mut disks = self.disks.write().unwrap();
            disks.refresh(true);
        }

        let disks = self.disks.read().unwrap();

        let mut telemetry = TelemetryData::new(self.metadata.id.clone());

        // Get disk usage for all disks
        let disk_usage = Self::get_disk_usage(&disks);
        let disk_count = disk_usage.len();

        for (mount_point, total, used, available, usage_percent) in disk_usage {
            // Total disk space
            let total_data_point = TelemetryDataPoint::new(
                "disk_total_bytes",
                total as f64,
                "bytes",
            )
            .with_label("type", "total")
            .with_label("mount_point", &mount_point);
            telemetry.add_data_point(total_data_point);

            // Used disk space
            let used_data_point = TelemetryDataPoint::new(
                "disk_used_bytes",
                used as f64,
                "bytes",
            )
            .with_label("type", "used")
            .with_label("mount_point", &mount_point);
            telemetry.add_data_point(used_data_point);

            // Available disk space
            let available_data_point = TelemetryDataPoint::new(
                "disk_available_bytes",
                available as f64,
                "bytes",
            )
            .with_label("type", "available")
            .with_label("mount_point", &mount_point);
            telemetry.add_data_point(available_data_point);

            // Disk usage percentage
            let usage_data_point = TelemetryDataPoint::new(
                "disk_usage_percent",
                usage_percent,
                "percent",
            )
            .with_label("type", "usage")
            .with_label("mount_point", &mount_point);
            telemetry.add_data_point(usage_data_point);
        }

        // Collect disk I/O stats (if available)
        let disk_io_stats = Self::get_disk_io_stats(&disks);
        for (mount_point, read_bytes, write_bytes) in disk_io_stats {
            let read_data_point = TelemetryDataPoint::new(
                "disk_read_bytes",
                read_bytes as f64,
                "bytes",
            )
            .with_label("type", "io_read")
            .with_label("mount_point", &mount_point);
            telemetry.add_data_point(read_data_point);

            let write_data_point = TelemetryDataPoint::new(
                "disk_write_bytes",
                write_bytes as f64,
                "bytes",
            )
            .with_label("type", "io_write")
            .with_label("mount_point", &mount_point);
            telemetry.add_data_point(write_data_point);
        }

        // Update stats
        let collection_end = Utc::now();
        let bytes = serde_json::to_string(&telemetry)
            .map(|s| s.len() as u64)
            .unwrap_or(0);
        self.stats.write().unwrap().record_collection(bytes);

        telemetry.collection_end = collection_end;

        tracing::debug!(
            "[DiskMonitorPlugin] Collected {} data points for {} disks",
            telemetry.data_points.len(),
            disk_count
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
    fn test_disk_monitor_plugin_creation() {
        let plugin = DiskMonitorPlugin::new();
        assert_eq!(plugin.metadata.id, "cc-disk-monitor");
        assert_eq!(plugin.metadata.name, "Disk Monitor");
        assert_eq!(plugin.category(), PluginCategory::System);
    }

    #[test]
    fn test_disk_monitor_plugin_collection() {
        let mut plugin = DiskMonitorPlugin::new();
        plugin.init(&PluginConfig::new()).unwrap();

        let telemetry = plugin.collect().unwrap();

        // Should have multiple data points
        assert!(!telemetry.is_empty());

        // Should have total disk space
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
    fn test_disk_monitor_plugin_status() {
        let mut plugin = DiskMonitorPlugin::new();
        assert_eq!(plugin.status(), PluginStatus::Stopped);

        plugin.init(&PluginConfig::new()).unwrap();
        assert_eq!(plugin.status(), PluginStatus::Running);

        plugin.stop().unwrap();
        assert_eq!(plugin.status(), PluginStatus::Stopped);
    }
}
