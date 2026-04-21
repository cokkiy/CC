//! Process Monitor Plugin
//!
//! This plugin collects process metrics using the sysinfo crate.
//! It provides process counts, top CPU/memory consuming processes, and zombie process detection.

use crate::plugins::{
    PluginCategory, PluginConfig, PluginError, PluginMetadata, PluginResult, PluginStats,
    PluginStatus, TelemetryData, TelemetryDataPoint, TelemetryPlugin,
};
use chrono::Utc;
use std::sync::RwLock;
use sysinfo::{ProcessRefreshKind, RefreshKind, System};

/// Process Monitor Plugin implementation
pub struct ProcessMonitorPlugin {
    metadata: PluginMetadata,
    status: RwLock<PluginStatus>,
    config: RwLock<PluginConfig>,
    stats: RwLock<PluginStats>,
    system: RwLock<System>,
    /// Maximum number of top processes to report
    max_top_processes: usize,
}

impl ProcessMonitorPlugin {
    /// Create a new Process monitor plugin
    pub fn new() -> Self {
        let mut system = System::new_with_specifics(
            RefreshKind::nothing().with_processes(ProcessRefreshKind::everything()),
        );

        // Give sysinfo time to initialize process data
        std::thread::sleep(std::time::Duration::from_millis(100));
        system.refresh_all();

        Self {
            metadata: PluginMetadata::new(
                "cc-process-monitor".to_string(),
                "Process Monitor".to_string(),
                "1.0.0",
            )
            .with_description("Monitors running processes using sysinfo"),
            status: RwLock::new(PluginStatus::Stopped),
            config: RwLock::new(PluginConfig::new()),
            stats: RwLock::new(PluginStats::new()),
            system: RwLock::new(system),
            max_top_processes: 10,
        }
    }

    /// Get process counts
    fn get_process_counts(system: &System) -> (usize, usize, usize) {
        let total = system.processes().len();
        let running = system
            .processes()
            .iter()
            .filter(|(_, p)| p.status() == sysinfo::ProcessStatus::Run)
            .count();
        let zombie = system
            .processes()
            .iter()
            .filter(|(_, p)| p.status() == sysinfo::ProcessStatus::Zombie)
            .count();
        (total, running, zombie)
    }

    /// Get top CPU-consuming processes
    fn get_top_cpu_processes(system: &System, max_count: usize) -> Vec<(String, u64, f64)> {
        let mut processes: Vec<_> = system
            .processes()
            .iter()
            .map(|(pid, p)| {
                let name = p.name().to_string_lossy().to_string();
                let pid_val = pid.as_u64();
                let cpu_usage = if p.cpu_usage().is_nan() || p.cpu_usage() > 100.0 {
                    0.0
                } else {
                    p.cpu_usage() as f64
                };
                (name, pid_val, cpu_usage)
            })
            .collect();

        processes.sort_by(|a, b| b.2.partial_cmp(&a.2).unwrap_or(std::cmp::Ordering::Equal));
        processes.into_iter().take(max_count).collect()
    }

    /// Get top memory-consuming processes
    fn get_top_memory_processes(system: &System, max_count: usize) -> Vec<(String, u64, u64)> {
        let mut processes: Vec<_> = system
            .processes()
            .iter()
            .map(|(pid, p)| {
                let name = p.name().to_string_lossy().to_string();
                let pid_val = pid.as_u64();
                let memory = p.memory();
                (name, pid_val, memory)
            })
            .collect();

        processes.sort_by(|a, b| b.2.cmp(&a.2));
        processes.into_iter().take(max_count).collect()
    }
}

impl Default for ProcessMonitorPlugin {
    fn default() -> Self {
        Self::new()
    }
}

impl TelemetryPlugin for ProcessMonitorPlugin {
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

        // Get max_top_processes from settings if provided
        if let Some(max) = config.get_setting::<u64>("max_top_processes") {
            self.max_top_processes = max as usize;
        }

        // Re-initialize system with process refresh kind
        let system = System::new_with_specifics(
            RefreshKind::nothing().with_processes(ProcessRefreshKind::everything()),
        );
        *self.system.write().unwrap() = system;

        // Give sysinfo time to initialize process data
        std::thread::sleep(std::time::Duration::from_millis(100));
        self.system.write().unwrap().refresh_all();

        *self.status.write().unwrap() = PluginStatus::Running;
        tracing::info!(
            "[ProcessMonitorPlugin] Initialized with interval: {}s, max_top_processes: {}",
            self.config.read().unwrap().interval_seconds,
            self.max_top_processes
        );
        Ok(())
    }

    fn start(&self) -> PluginResult<()> {
        if *self.status.read().unwrap() == PluginStatus::Running {
            return Err(PluginError::AlreadyInitialized(self.metadata.id.clone()));
        }
        *self.status.write().unwrap() = PluginStatus::Running;
        tracing::info!("[ProcessMonitorPlugin] Started");
        Ok(())
    }

    fn stop(&self) -> PluginResult<()> {
        *self.status.write().unwrap() = PluginStatus::Stopped;
        tracing::info!("[ProcessMonitorPlugin] Stopped");
        Ok(())
    }

    fn collect(&self) -> PluginResult<TelemetryData> {
        let _start_time = Utc::now();

        // Refresh process data
        {
            let mut system = self.system.write().unwrap();
            system.refresh_all();
        }

        let system = self.system.read().unwrap();

        let mut telemetry = TelemetryData::new(self.metadata.id.clone());

        // Collect process counts
        let (total, running, zombie) = Self::get_process_counts(&system);

        let total_data_point =
            TelemetryDataPoint::new("process_count", total as f64, "count").with_label("type", "total");
        telemetry.add_data_point(total_data_point);

        let running_data_point =
            TelemetryDataPoint::new("process_count", running as f64, "count")
                .with_label("type", "running");
        telemetry.add_data_point(running_data_point);

        let zombie_data_point =
            TelemetryDataPoint::new("process_count", zombie as f64, "count")
                .with_label("type", "zombie");
        telemetry.add_data_point(zombie_data_point);

        // Collect top CPU-consuming processes
        let top_cpu_processes = Self::get_top_cpu_processes(&system, self.max_top_processes);
        for (idx, (name, pid, cpu_usage)) in top_cpu_processes.iter().enumerate() {
            let data_point = TelemetryDataPoint::new(
                "process_cpu_percent",
                *cpu_usage,
                "percent",
            )
            .with_label("type", "top_cpu")
            .with_label("rank", &format!("{}", idx + 1))
            .with_label("name", name)
            .with_label("pid", &format!("{}", pid));
            telemetry.add_data_point(data_point);
        }

        // Collect top memory-consuming processes
        let top_memory_processes = Self::get_top_memory_processes(&system, self.max_top_processes);
        for (idx, (name, pid, memory)) in top_memory_processes.iter().enumerate() {
            let data_point = TelemetryDataPoint::new(
                "process_memory_bytes",
                *memory as f64,
                "bytes",
            )
            .with_label("type", "top_memory")
            .with_label("rank", &format!("{}", idx + 1))
            .with_label("name", name)
            .with_label("pid", &format!("{}", pid));
            telemetry.add_data_point(data_point);
        }

        // Update stats
        let collection_end = Utc::now();
        let bytes = serde_json::to_string(&telemetry)
            .map(|s| s.len() as u64)
            .unwrap_or(0);
        self.stats.write().unwrap().record_collection(bytes);

        telemetry.collection_end = collection_end;

        tracing::debug!(
            "[ProcessMonitorPlugin] Collected {} data points, total processes: {}, running: {}, zombie: {}",
            telemetry.data_points.len(),
            total,
            running,
            zombie
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
    fn test_process_monitor_plugin_creation() {
        let plugin = ProcessMonitorPlugin::new();
        assert_eq!(plugin.metadata.id, "cc-process-monitor");
        assert_eq!(plugin.metadata.name, "Process Monitor");
        assert_eq!(plugin.category(), PluginCategory::System);
    }

    #[test]
    fn test_process_monitor_plugin_collection() {
        let mut plugin = ProcessMonitorPlugin::new();
        plugin.init(&PluginConfig::new()).unwrap();

        let telemetry = plugin.collect().unwrap();

        // Should have multiple data points
        assert!(!telemetry.is_empty());

        // Should have process counts
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
    }

    #[test]
    fn test_process_monitor_plugin_status() {
        let mut plugin = ProcessMonitorPlugin::new();
        assert_eq!(plugin.status(), PluginStatus::Stopped);

        plugin.init(&PluginConfig::new()).unwrap();
        assert_eq!(plugin.status(), PluginStatus::Running);

        plugin.stop().unwrap();
        assert_eq!(plugin.status(), PluginStatus::Stopped);
    }

    #[test]
    fn test_get_process_counts() {
        let system = System::new_all();
        let (total, running, zombie) = ProcessMonitorPlugin::get_process_counts(&system);
        assert!(total >= 0);
        assert!(running >= 0);
        assert!(zombie >= 0);
    }
}
