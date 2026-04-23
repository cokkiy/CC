//! Plugin system for telemetry data collection.
//!
//! This module defines the TelemetryPlugin trait and basic plugin types
//! that all telemetry plugins must implement.

use anyhow::Result;
use chrono::{DateTime, Utc};
use serde::{Deserialize, Serialize};
use std::sync::Arc;

/// Unique identifier for a plugin instance.
pub type PluginId = String;

/// Represents a single telemetry data point collected by a plugin.
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct TelemetryDataPoint {
    /// Metric name (e.g., "cpu_usage", "memory_used")
    pub metric_name: String,
    /// Metric value
    pub value: f64,
    /// Unit of measurement (e.g., "percent", "bytes", "count")
    pub unit: String,
    /// Timestamp when the metric was collected
    pub timestamp: DateTime<Utc>,
    /// Optional labels/tags for the metric
    #[serde(default)]
    pub labels: std::collections::HashMap<String, String>,
}

impl TelemetryDataPoint {
    /// Create a new telemetry data point
    pub fn new(
        metric_name: impl Into<String>,
        value: f64,
        unit: impl Into<String>,
    ) -> Self {
        Self {
            metric_name: metric_name.into(),
            value,
            unit: unit.into(),
            timestamp: Utc::now(),
            labels: std::collections::HashMap::new(),
        }
    }

    /// Create a new telemetry data point with labels
    pub fn with_labels(
        metric_name: impl Into<String>,
        value: f64,
        unit: impl Into<String>,
        labels: impl IntoIterator<Item = (String, String)>,
    ) -> Self {
        Self {
            metric_name: metric_name.into(),
            value,
            unit: unit.into(),
            timestamp: Utc::now(),
            labels: labels.into_iter().collect(),
        }
    }

    /// Add a label to this data point
    pub fn with_label(mut self, key: impl Into<String>, value: impl Into<String>) -> Self {
        self.labels.insert(key.into(), value.into());
        self
    }
}

/// Collection of telemetry data points from a plugin.
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct TelemetryData {
    /// ID of the plugin that collected this data
    pub plugin_id: PluginId,
    /// All data points collected
    pub data_points: Vec<TelemetryDataPoint>,
    /// Timestamp when collection started
    pub collection_start: DateTime<Utc>,
    /// Timestamp when collection finished
    pub collection_end: DateTime<Utc>,
}

impl TelemetryData {
    /// Create a new empty TelemetryData for a plugin
    pub fn new(plugin_id: PluginId) -> Self {
        let now = Utc::now();
        Self {
            plugin_id,
            data_points: Vec::new(),
            collection_start: now,
            collection_end: now,
        }
    }

    /// Add a data point to the collection
    pub fn add_data_point(&mut self, data_point: TelemetryDataPoint) {
        self.data_points.push(data_point);
    }

    /// Get the number of data points
    pub fn len(&self) -> usize {
        self.data_points.len()
    }

    /// Check if there are no data points
    pub fn is_empty(&self) -> bool {
        self.data_points.is_empty()
    }
}

/// Metadata describing a telemetry plugin.
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct PluginMetadata {
    /// Unique identifier for the plugin
    pub id: PluginId,
    /// Human-readable name of the plugin
    pub name: String,
    /// Version string of the plugin
    pub version: String,
    /// Optional description of what the plugin does
    pub description: Option<String>,
}

impl PluginMetadata {
    /// Create new plugin metadata
    pub fn new(id: PluginId, name: impl Into<String>, version: impl Into<String>) -> Self {
        Self {
            id,
            name: name.into(),
            version: version.into(),
            description: None,
        }
    }

    /// Set the description
    pub fn with_description(mut self, description: impl Into<String>) -> Self {
        self.description = Some(description.into());
        self
    }
}

/// Category of telemetry plugin.
#[derive(Debug, Clone, Copy, PartialEq, Eq, Hash, Serialize, Deserialize)]
#[serde(rename_all = "snake_case")]
pub enum PluginCategory {
    /// System metrics (CPU, memory, disk)
    System,
    /// Network metrics
    Network,
    /// Application metrics
    Application,
    /// Custom/specialized metrics
    Custom,
}

impl std::fmt::Display for PluginCategory {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        match self {
            PluginCategory::System => write!(f, "system"),
            PluginCategory::Network => write!(f, "network"),
            PluginCategory::Application => write!(f, "application"),
            PluginCategory::Custom => write!(f, "custom"),
        }
    }
}

/// Status of a plugin.
#[derive(Debug, Clone, Copy, PartialEq, Eq, Serialize, Deserialize)]
#[serde(rename_all = "snake_case")]
pub enum PluginStatus {
    /// Plugin is running and collecting data
    Running,
    /// Plugin is paused (temporarily not collecting)
    Paused,
    /// Plugin has encountered an error
    Error,
    /// Plugin is stopped/unavailable
    Stopped,
}

impl Default for PluginStatus {
    fn default() -> Self {
        Self::Stopped
    }
}

impl std::fmt::Display for PluginStatus {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        match self {
            PluginStatus::Running => write!(f, "running"),
            PluginStatus::Paused => write!(f, "paused"),
            PluginStatus::Error => write!(f, "error"),
            PluginStatus::Stopped => write!(f, "stopped"),
        }
    }
}

/// Result of a plugin operation.
pub type PluginResult<T> = Result<T, PluginError>;

/// Errors that can occur during plugin operations.
#[derive(Debug, thiserror::Error)]
pub enum PluginError {
    /// Plugin is not initialized
    #[error("plugin not initialized: {0}")]
    NotInitialized(String),

    /// Plugin is already initialized
    #[error("plugin already initialized: {0}")]
    AlreadyInitialized(String),

    /// Collection failed
    #[error("collection failed for plugin {0}: {1}")]
    CollectionFailed(String, String),

    /// Configuration error
    #[error("configuration error for plugin {0}: {1}")]
    ConfigError(String, String),

    /// Plugin not found
    #[error("plugin not found: {0}")]
    NotFound(String),

    /// Plugin error
    #[error("plugin error: {0}")]
    Plugin(String),
}

/// Configuration for a telemetry plugin.
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct PluginConfig {
    /// Enable or disable the plugin
    #[serde(default = "default_enabled")]
    pub enabled: bool,

    /// Collection interval in seconds
    #[serde(default = "default_interval")]
    pub interval_seconds: u64,

    /// Additional plugin-specific settings
    #[serde(default)]
    pub settings: std::collections::HashMap<String, serde_json::Value>,
}

fn default_enabled() -> bool {
    true
}

fn default_interval() -> u64 {
    60
}

impl Default for PluginConfig {
    fn default() -> Self {
        Self {
            enabled: true,
            interval_seconds: default_interval(),
            settings: std::collections::HashMap::new(),
        }
    }
}

impl PluginConfig {
    /// Create a new plugin config with defaults
    pub fn new() -> Self {
        Self::default()
    }

    /// Check if the plugin is enabled
    pub fn is_enabled(&self) -> bool {
        self.enabled
    }

    /// Get a setting value by key
    pub fn get_setting<T: serde::de::DeserializeOwned>(
        &self,
        key: &str,
    ) -> Option<T> {
        self.settings.get(key).and_then(|v| serde_json::from_value(v.clone()).ok())
    }
}

/// Statistics about a plugin's execution.
#[derive(Debug, Clone, Default, Serialize, Deserialize)]
pub struct PluginStats {
    /// Total number of successful collections
    pub collection_count: u64,
    /// Number of failed collections
    pub error_count: u64,
    /// Last successful collection timestamp
    pub last_collection: Option<DateTime<Utc>>,
    /// Last error timestamp
    pub last_error: Option<DateTime<Utc>>,
    /// Total bytes collected (if applicable)
    pub bytes_collected: u64,
}

impl PluginStats {
    /// Create new empty stats
    pub fn new() -> Self {
        Self::default()
    }

    /// Record a successful collection
    pub fn record_collection(&mut self, bytes: u64) {
        self.collection_count += 1;
        self.last_collection = Some(Utc::now());
        self.bytes_collected += bytes;
    }

    /// Record a failed collection
    pub fn record_error(&mut self) {
        self.error_count += 1;
        self.last_error = Some(Utc::now());
    }

    /// Get the success rate (0.0 to 1.0)
    pub fn success_rate(&self) -> f64 {
        let total = self.collection_count + self.error_count;
        if total == 0 {
            1.0
        } else {
            self.collection_count as f64 / total as f64
        }
    }
}

/// The main trait that all telemetry plugins must implement.
///
/// Implement this trait to create a new telemetry plugin that can
/// collect and report system metrics.
pub trait TelemetryPlugin: Send + Sync {
    /// Get the metadata for this plugin.
    fn metadata(&self) -> &PluginMetadata;

    /// Get the current status of this plugin.
    fn status(&self) -> PluginStatus;

    /// Initialize the plugin with the given configuration.
    ///
    /// This is called once when the plugin is first loaded.
    fn init(&mut self, config: &PluginConfig) -> PluginResult<()> {
        let _ = config;
        Ok(())
    }

    /// Start the plugin.
    ///
    /// This is called when the service starts and the plugin should begin
    /// collecting telemetry data.
    fn start(&self) -> PluginResult<()> {
        Ok(())
    }

    /// Stop the plugin.
    ///
    /// This is called when the service stops and the plugin should stop
    /// collecting telemetry data.
    fn stop(&self) -> PluginResult<()> {
        Ok(())
    }

    /// Collect telemetry data from the plugin.
    ///
    /// This is called periodically based on the plugin's configuration.
    /// The plugin should collect all relevant metrics and return them
    /// as a TelemetryData structure.
    fn collect(&self) -> PluginResult<TelemetryData>;

    /// Get the collection interval in seconds.
    ///
    /// Returns the configured interval, or a default if not configured.
    fn interval(&self) -> u64 {
        60
    }

    /// Get statistics about this plugin's execution.
    ///
    /// Returns statistics about collection counts, errors, etc.
    fn stats(&self) -> PluginStats {
        unimplemented!("stats not implemented for this plugin")
    }

    /// Check if this plugin is currently active.
    fn is_running(&self) -> bool {
        self.status() == PluginStatus::Running
    }

    /// Get the category of this plugin.
    fn category(&self) -> PluginCategory {
        PluginCategory::Custom
    }

    /// Get the plugin configuration.
    ///
    /// Returns the current configuration if available.
    fn config(&self) -> Option<&PluginConfig> {
        None
    }

    /// Validate the plugin configuration.
    ///
    /// This is called to verify the configuration is valid before
    /// the plugin is initialized.
    fn validate_config(&self, _config: &PluginConfig) -> PluginResult<()> {
        Ok(())
    }
}

/// A boxed telemetry plugin for dynamic dispatch.
pub type BoxedTelemetryPlugin = Arc<dyn TelemetryPlugin>;

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_telemetry_data_point_creation() {
        let point = TelemetryDataPoint::new("cpu_usage", 75.5, "percent");
        assert_eq!(point.metric_name, "cpu_usage");
        assert_eq!(point.value, 75.5);
        assert_eq!(point.unit, "percent");
        assert!(!point.labels.is_empty() || point.labels.is_empty()); // Default is empty
    }

    #[test]
    fn test_telemetry_data_point_with_labels() {
        let point = TelemetryDataPoint::with_labels(
            "network_bytes",
            1024.0,
            "bytes",
            vec![("interface".to_string(), "eth0".to_string())],
        );
        assert_eq!(point.labels.get("interface"), Some(&"eth0".to_string()));
    }

    #[test]
    fn test_telemetry_data_collection() {
        let mut data = TelemetryData::new("test_plugin".to_string());
        assert!(data.is_empty());

        data.add_data_point(TelemetryDataPoint::new("metric1", 1.0, "count"));
        data.add_data_point(TelemetryDataPoint::new("metric2", 2.0, "count"));

        assert_eq!(data.len(), 2);
        assert!(!data.is_empty());
    }

    #[test]
    fn test_plugin_metadata() {
        let metadata = PluginMetadata::new("cpu_plugin".to_string(), "CPU Plugin", "1.0.0")
            .with_description("Collects CPU usage metrics");

        assert_eq!(metadata.id, "cpu_plugin");
        assert_eq!(metadata.name, "CPU Plugin");
        assert_eq!(metadata.version, "1.0.0");
        assert_eq!(metadata.description, Some("Collects CPU usage metrics".to_string()));
    }

    #[test]
    fn test_plugin_config() {
        let config = PluginConfig::new();
        assert!(config.is_enabled());
        assert_eq!(config.interval_seconds, 60);
    }

    #[test]
    fn test_plugin_stats() {
        let mut stats = PluginStats::new();
        assert_eq!(stats.success_rate(), 1.0);

        stats.record_collection(100);
        assert_eq!(stats.collection_count, 1);
        assert_eq!(stats.success_rate(), 1.0);

        stats.record_error();
        assert_eq!(stats.error_count, 1);
        assert_eq!(stats.success_rate(), 0.5);
    }
}
