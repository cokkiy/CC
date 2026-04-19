//! CPU Monitor Plugin
//!
//! This plugin collects CPU usage metrics using the sysinfo crate.
//! It provides both overall CPU usage and per-core usage statistics.

use crate::plugins::{
    PluginCategory, PluginConfig, PluginError, PluginMetadata, PluginResult, PluginStats,
    PluginStatus, TelemetryData, TelemetryPlugin, TelemetryDataPoint,
};
use chrono::{DateTime, Utc};
use std::sync::{Arc, RwLock};
use sysinfo::{CpuRefreshKind, RefreshKind, System};

/// CPU Monitor Plugin implementation
pub struct CpuMonitorPlugin {
    metadata: PluginMetadata,
    status: PluginStatus,
    config: PluginConfig,
    stats: PluginStats,
    system: RwLock<System>,
}

impl CpuMonitorPlugin {
    /// Create a new CPU monitor plugin
    pub fn new() -> Self {
        let system = System::new_with_specifics(
            RefreshKind::new().with_cpu(CpuRefreshKind::everything()),
        );
        
        // Give sysinfo time to initialize CPU data
        std::thread::sleep(std::time::Duration::from_millis(100));
        system.refresh_cpu_all();

        Self {
            metadata: PluginMetadata::new(
                "cc-cpu-monitor".to_string(),
                "CPU Monitor".to_string(),
                "1.0.0",
            )
            .with_description("Monitors CPU usage across all cores using sysinfo"),
            status: PluginStatus::Stopped,
            config: PluginConfig::new(),
            stats: PluginStats::new(),
            system: RwLock::new(system),
        }
    }

    /// Get overall CPU usage percentage
    fn get_overall_cpu_usage(system: &System) -> f64 {
        // Use the system's global CPU usage
        let global_cpu = system.global_cpu_usage();
        
        // Handle the case where CPU usage might be NaN or > 100
        if global_cpu.is_nan() || global_cpu > 100.0 {
            0.0
        } else {
            global_cpu
        }
    }

    /// Get per-core CPU usage
    fn get_per_core_usage(system: &System) -> Vec<(String, f64)> {
        system
            .cpus()
            .iter()
            .enumerate()
            .map(|(idx, cpu)| {
                let name = cpu.name().to_string_lossy().to_string();
                let usage = if cpu.cpu_usage().is_nan() || cpu.cpu_usage() > 100.0 {
                    0.0
                } else {
                    cpu.cpu_usage()
                };
                // Use core index if name is empty
                let core_name = if name.is_empty() {
                    format!("cpu{}", idx)
                } else {
                    name
                };
                (core_name, usage)
            })
            .collect()
    }
}

impl Default for CpuMonitorPlugin {
    fn default() -> Self {
        Self::new()
    }
}

impl TelemetryPlugin for CpuMonitorPlugin {
    fn metadata(&self) -> &PluginMetadata {
        &self.metadata
    }

    fn status(&self) -> PluginStatus {
        self.status
    }

    fn init(&mut self, config: &PluginConfig) -> PluginResult<()> {
        self.config = config.clone();
        
        // Validate config
        self.validate_config(config)?;
        
        // Re-initialize system with CPU refresh kind
        let system = System::new_with_specifics(
            RefreshKind::new().with_cpu(CpuRefreshKind::everything()),
        );
        *self.system.write().unwrap() = system;
        
        // Give sysinfo time to initialize CPU data
        std::thread::sleep(std::time::Duration::from_millis(100));
        self.system.read().unwrap().refresh_cpu_all();
        
        self.status = PluginStatus::Running;
        tracing::info!("[CpuMonitorPlugin] Initialized with interval: {}s", self.config.interval_seconds);
        Ok(())
    }

    fn start(&self) -> PluginResult<()> {
        if self.status == PluginStatus::Running {
            return Err(PluginError::AlreadyInitialized(self.metadata.id.clone()));
        }
        self.status = PluginStatus::Running;
        tracing::info!("[CpuMonitorPlugin] Started");
        Ok(())
    }

    fn stop(&self) -> PluginResult<()> {
        self.status = PluginStatus::Stopped;
        tracing::info!("[CpuMonitorPlugin] Stopped");
        Ok(())
    }

    fn collect(&self) -> PluginResult<TelemetryData> {
        let start_time = Utc::now();
        
        // Refresh CPU data
        {
            let mut system = self.system.write().unwrap();
            system.refresh_cpu_all();
        }
        
        let system = self.system.read().unwrap();
        
        let mut telemetry = TelemetryData::new(self.metadata.id.clone());
        
        // Collect overall CPU usage
        let overall_usage = Self::get_overall_cpu_usage(&system);
        let overall_data_point = TelemetryDataPoint::new("cpu_usage_percent", overall_usage, "percent")
            .with_label("type", "overall");
        telemetry.add_data_point(overall_data_point);
        
        // Collect per-core CPU usage
        let per_core_data = Self::get_per_core_usage(&system);
        for (core_name, usage) in per_core_data {
            let data_point = TelemetryDataPoint::new(
                "cpu_usage_percent",
                usage,
                "percent",
            )
            .with_label("type", "per_core")
            .with_label("core", &core_name);
            telemetry.add_data_point(data_point);
        }
        
        // Collect CPU load averages (if available)
        let load_avg = system.load_average();
        let load_avg_data_point = TelemetryDataPoint::new("cpu_load_average_1min", load_avg.one, "load")
            .with_label("type", "load_average");
        telemetry.add_data_point(load_avg_data_point);
        
        let load_avg_5_data_point = TelemetryDataPoint::new("cpu_load_average_5min", load_avg.five, "load")
            .with_label("type", "load_average");
        telemetry.add_data_point(load_avg_5_data_point);
        
        let load_avg_15_data_point = TelemetryDataPoint::new("cpu_load_average_15min", load_avg.fifteen, "load")
            .with_label("type", "load_average");
        telemetry.add_data_point(load_avg_15_data_point);
        
        // Collect CPU core count
        let core_count = system.cpus().len() as f64;
        let core_count_data_point = TelemetryDataPoint::new("cpu_core_count", core_count, "count")
            .with_label("type", "info");
        telemetry.add_data_point(core_count_data_point);
        
        // Collect CPU brand/model info
        if let Some(cpu) = system.cpus().first() {
            let brand_data_point = TelemetryDataPoint::new("cpu_brand", 1.0, "string")
                .with_label("type", "info")
                .with_label("brand", cpu.brand());
            telemetry.add_data_point(brand_data_point);
            
            let frequency_data_point = TelemetryDataPoint::new("cpu_frequency_mhz", cpu.frequency() as f64, "mhz")
                .with_label("type", "info");
            telemetry.add_data_point(frequency_data_point);
        }
        
        // Update stats
        let collection_end = Utc::now();
        let bytes = serde_json::to_string(&telemetry)
            .map(|s| s.len() as u64)
            .unwrap_or(0);
        self.stats.record_collection(bytes);
        
        telemetry.collection_end = collection_end;
        
        tracing::debug!(
            "[CpuMonitorPlugin] Collected {} data points, overall CPU: {:.2}%",
            telemetry.data_points.len(),
            overall_usage
        );
        
        Ok(telemetry)
    }

    fn interval(&self) -> u64 {
        self.config.interval_seconds
    }

    fn stats(&self) -> &PluginStats {
        &self.stats
    }

    fn category(&self) -> PluginCategory {
        PluginCategory::System
    }

    fn config(&self) -> Option<&PluginConfig> {
        Some(&self.config)
    }

    fn is_running(&self) -> bool {
        self.status == PluginStatus::Running
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
    fn test_cpu_monitor_plugin_creation() {
        let plugin = CpuMonitorPlugin::new();
        assert_eq!(plugin.metadata.id, "cc-cpu-monitor");
        assert_eq!(plugin.metadata.name, "CPU Monitor");
        assert_eq!(plugin.category(), PluginCategory::System);
    }

    #[test]
    fn test_cpu_monitor_plugin_collection() {
        let mut plugin = CpuMonitorPlugin::new();
        plugin.init(&PluginConfig::new()).unwrap();
        
        let telemetry = plugin.collect().unwrap();
        
        // Should have multiple data points
        assert!(!telemetry.is_empty());
        
        // Should have overall CPU usage
        let overall_points: Vec<_> = telemetry.data_points.iter()
            .filter(|p| p.labels.get("type").map(|t| t == "overall").unwrap_or(false))
            .collect();
        assert!(!overall_points.is_empty());
    }

    #[test]
    fn test_cpu_monitor_plugin_status() {
        let mut plugin = CpuMonitorPlugin::new();
        assert_eq!(plugin.status(), PluginStatus::Stopped);
        
        plugin.init(&PluginConfig::new()).unwrap();
        assert_eq!(plugin.status(), PluginStatus::Running);
        
        plugin.stop().unwrap();
        assert_eq!(plugin.status(), PluginStatus::Stopped);
    }
}
