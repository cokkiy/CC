//! Alert Rule Engine for CC-rStationService
//!
//! This module provides the alert rule model and core engine logic
//! for evaluating telemetry data against user-defined rules.

use chrono::{DateTime, Utc};
use serde::{Deserialize, Serialize};
use std::collections::HashMap;

/// Alert severity levels
#[derive(Debug, Clone, Copy, PartialEq, Eq, Hash, Serialize, Deserialize, Default)]
#[serde(rename_all = "snake_case")]
pub enum AlertSeverity {
    /// Info-level alert (no immediate action needed)
    #[default]
    Info,
    /// Warning-level alert (investigation recommended)
    Warning,
    /// Critical-level alert (immediate action required)
    Critical,
}

impl std::fmt::Display for AlertSeverity {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        match self {
            AlertSeverity::Info => write!(f, "info"),
            AlertSeverity::Warning => write!(f, "warning"),
            AlertSeverity::Critical => write!(f, "critical"),
        }
    }
}

/// Comparison operators for condition evaluation
#[derive(Debug, Clone, Copy, PartialEq, Eq, Serialize, Deserialize)]
#[serde(rename_all = "snake_case")]
pub enum ComparisonOp {
    /// Greater than
    Gt,
    /// Greater than or equal
    Gte,
    /// Less than
    Lt,
    /// Less than or equal
    Lte,
    /// Equal
    Eq,
    /// Not equal
    Neq,
}

impl ComparisonOp {
    /// Evaluate the comparison: `left op right`
    pub fn evaluate(&self, left: f64, right: f64) -> bool {
        match self {
            ComparisonOp::Gt => left > right,
            ComparisonOp::Gte => left >= right,
            ComparisonOp::Lt => left < right,
            ComparisonOp::Lte => left <= right,
            ComparisonOp::Eq => (left - right).abs() < f64::EPSILON,
            ComparisonOp::Neq => (left - right).abs() >= f64::EPSILON,
        }
    }
}

/// Aggregation function for time-series data
#[derive(Debug, Clone, Copy, PartialEq, Eq, Serialize, Deserialize)]
#[serde(rename_all = "snake_case")]
pub enum AggregationFunc {
    /// Latest value
    Latest,
    /// Minimum value
    Min,
    /// Maximum value
    Max,
    /// Average value
    Avg,
    /// Sum of values
    Sum,
    /// Count of values
    Count,
    /// Standard deviation
    StdDev,
    /// Rate of change (delta per second)
    Rate,
}

impl Default for AggregationFunc {
    fn default() -> Self {
        AggregationFunc::Latest
    }
}

/// Single condition in an alert rule
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct AlertCondition {
    /// Metric name pattern (e.g., "cpu_usage_percent", "memory_*")
    /// Supports wildcard patterns: * matches any sequence
    pub metric_name: String,

    /// Optional label filter - all key-value pairs must match
    /// Empty means no label filtering (match all labels)
    #[serde(default)]
    pub label_filter: HashMap<String, String>,

    /// Comparison operator
    pub operator: ComparisonOp,

    /// Threshold value to compare against
    pub threshold: f64,

    /// Aggregation function to apply before comparison
    #[serde(default)]
    pub aggregation: AggregationFunc,

    /// Severity level for this condition
    #[serde(default)]
    pub severity: AlertSeverity,
}

impl AlertCondition {
    /// Check if this condition matches a given TelemetryDataPoint
    pub fn matches_metric(&self, metric_name: &str) -> bool {
        self.matches_pattern(metric_name)
    }

    /// Check if metric name matches the pattern (supports * wildcard)
    fn matches_pattern(&self, metric_name: &str) -> bool {
        let pattern = &self.metric_name;
        if pattern.contains('*') {
            // Convert wildcard pattern to regex-like matching
            let regex_pattern = pattern
                .replace('*', ".*")
                .replace('?', ".");
            regex::Regex::new(&format!("^{}$", regex_pattern))
                .map(|re| re.is_match(metric_name))
                .unwrap_or(false)
        } else {
            pattern == metric_name
        }
    }

    /// Check if labels match the filter criteria
    pub fn matches_labels(&self, labels: &HashMap<String, String>) -> bool {
        for (key, value) in &self.label_filter {
            match labels.get(key) {
                Some(v) if v == value => {}
                _ => return false,
            }
        }
        true
    }

    /// Evaluate condition against a single data point
    pub fn evaluate_point(&self, value: f64) -> bool {
        self.operator.evaluate(value, self.threshold)
    }
}

/// Logical operator for combining multiple conditions
#[derive(Debug, Clone, Copy, PartialEq, Eq, Serialize, Deserialize, Default)]
#[serde(rename_all = "snake_case")]
pub enum LogicalOp {
    /// AND - all conditions must be true
    #[default]
    And,
    /// OR - at least one condition must be true
    Or,
}

/// Duration requirement for alert state
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct DurationRequirement {
    /// Minimum duration in seconds the condition must be true
    pub for_seconds: u64,
    /// Percentage of samples that must exceed threshold (0.0-1.0)
    #[serde(default = "default_min_percentage")]
    pub min_percentage: f64,
}

fn default_min_percentage() -> f64 {
    1.0
}

/// Alert rule definition
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct AlertRule {
    /// Unique identifier for the rule
    pub id: String,
    /// Human-readable name
    pub name: String,
    /// Optional description
    pub description: Option<String>,
    /// Enabled/disabled state
    #[serde(default = "default_enabled")]
    pub enabled: bool,
    /// List of conditions (combined with logical operator)
    #[serde(default)]
    pub conditions: Vec<AlertCondition>,
    /// Logical operator to combine conditions
    #[serde(default)]
    pub condition_op: LogicalOp,
    /// Duration requirement before alert fires
    pub duration: Option<DurationRequirement>,
    /// Cooldown period in seconds (prevents alert spam)
    #[serde(default = "default_cooldown")]
    pub cooldown_seconds: u64,
    /// Action to take when alert fires
    #[serde(default)]
    pub actions: Vec<AlertAction>,
    /// Severity level (derived from conditions if not set)
    #[serde(default)]
    pub severity: Option<AlertSeverity>,
    /// Custom tags/labels for this rule
    #[serde(default)]
    pub tags: HashMap<String, String>,
    /// Creation timestamp
    pub created_at: DateTime<Utc>,
    /// Last modification timestamp
    pub updated_at: DateTime<Utc>,
}

fn default_enabled() -> bool {
    true
}

fn default_cooldown() -> u64 {
    300 // 5 minutes default
}

/// Alert action types
#[derive(Debug, Clone, Serialize, Deserialize)]
#[serde(tag = "type", rename_all = "snake_case")]
pub enum AlertAction {
    /// Log the alert
    Log {
        #[serde(default = "default_log_level")]
        level: String,
    },
    /// Publish to MQTT
    Mqtt {
        topic: String,
        #[serde(default)]
        payload_template: Option<String>,
    },
    /// Execute a script
    Script {
        script_id: String,
        #[serde(default)]
        timeout_seconds: u64,
    },
    /// Webhook callback
    Webhook {
        url: String,
        #[serde(default)]
        headers: HashMap<String, String>,
    },
}

fn default_log_level() -> String {
    "warn".to_string()
}

/// Alert state tracking
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct AlertState {
    /// Rule ID this state belongs to
    pub rule_id: String,
    /// Whether condition is currently firing
    pub is_firing: bool,
    /// Timestamp when condition first became true
    pub firing_since: Option<DateTime<Utc>>,
    /// Timestamp when alert was last fired
    pub last_fired_at: Option<DateTime<Utc>>,
    /// Number of times alert has fired
    pub fire_count: u64,
    /// Last evaluation timestamp
    pub last_evaluated_at: Option<DateTime<Utc>>,
}

impl Default for AlertState {
    fn default() -> Self {
        Self {
            rule_id: String::new(),
            is_firing: false,
            firing_since: None,
            last_fired_at: None,
            fire_count: 0,
            last_evaluated_at: None,
        }
    }
}

impl AlertState {
    pub fn new(rule_id: String) -> Self {
        Self {
            rule_id,
            ..Default::default()
        }
    }
}

/// Fired alert instance
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct Alert {
    /// Unique alert ID
    pub id: String,
    /// Rule ID that generated this alert
    pub rule_id: String,
    /// Rule name for display
    pub rule_name: String,
    /// Alert severity
    pub severity: AlertSeverity,
    /// Summary message
    pub message: String,
    /// Detailed description
    pub description: Option<String>,
    /// Metric data that triggered the alert
    pub metric_name: String,
    /// Metric value that triggered the alert
    pub metric_value: f64,
    /// Threshold that was exceeded
    pub threshold: f64,
    /// Operator used
    pub operator: ComparisonOp,
    /// Labels of the triggering metric
    #[serde(default)]
    pub labels: HashMap<String, String>,
    /// Timestamp when alert was fired
    pub fired_at: DateTime<Utc>,
    /// Timestamp when alert was resolved (if resolved)
    pub resolved_at: Option<DateTime<Utc>>,
    /// Custom tags
    #[serde(default)]
    pub tags: HashMap<String, String>,
}

impl Alert {
    /// Generate a unique alert ID
    pub fn generate_id() -> String {
        uuid::Uuid::new_v4().to_string()
    }

    /// Create a new alert from a rule and condition evaluation
    pub fn from_rule(
        rule: &AlertRule,
        condition: &AlertCondition,
        metric_name: String,
        metric_value: f64,
        labels: HashMap<String, String>,
    ) -> Self {
        let severity = condition.severity;
        let message = format!(
            "[{}] {}: {} {} {} (current: {:.2})",
            severity,
            rule.name,
            metric_name,
            format_operator(&condition.operator),
            condition.threshold,
            metric_value
        );

        Self {
            id: Self::generate_id(),
            rule_id: rule.id.clone(),
            rule_name: rule.name.clone(),
            severity,
            message,
            description: rule.description.clone(),
            metric_name,
            metric_value,
            threshold: condition.threshold,
            operator: condition.operator,
            labels,
            fired_at: Utc::now(),
            resolved_at: None,
            tags: rule.tags.clone(),
        }
    }
}

/// Format operator for display
fn format_operator(op: &ComparisonOp) -> &'static str {
    match op {
        ComparisonOp::Gt => ">",
        ComparisonOp::Gte => ">=",
        ComparisonOp::Lt => "<",
        ComparisonOp::Lte => "<=",
        ComparisonOp::Eq => "==",
        ComparisonOp::Neq => "!=",
    }
}

/// Telemetry value with metadata for rule evaluation
#[derive(Debug, Clone)]
pub struct EvaluatedMetric {
    pub metric_name: String,
    pub value: f64,
    pub labels: HashMap<String, String>,
    pub timestamp: DateTime<Utc>,
}

/// Result of rule evaluation
#[derive(Debug, Clone)]
pub struct EvaluationResult {
    /// Rule ID
    pub rule_id: String,
    /// Whether the rule conditions are met
    pub is_matched: bool,
    /// Matched conditions
    pub matched_conditions: Vec<usize>,
    /// Metric values that triggered the match
    pub triggering_metrics: Vec<EvaluatedMetric>,
    /// Timestamp of evaluation
    pub evaluated_at: DateTime<Utc>,
}

/// Alert rule engine - core evaluation logic
pub struct AlertEngine {
    /// Registered rules
    rules: HashMap<String, AlertRule>,
    /// Rule states for tracking duration/cooldown
    states: HashMap<String, AlertState>,
    /// Alert history (recent alerts)
    recent_alerts: Vec<Alert>,
    /// Maximum alert history size
    max_history: usize,
}

impl AlertEngine {
    /// Create a new alert engine
    pub fn new() -> Self {
        Self {
            rules: HashMap::new(),
            states: HashMap::new(),
            recent_alerts: Vec::new(),
            max_history: 1000,
        }
    }

    /// Add or update a rule
    pub fn add_rule(&mut self, rule: AlertRule) {
        let rule_id = rule.id.clone();
        self.rules.insert(rule_id.clone(), rule);
        self.states.entry(rule_id.clone()).or_insert_with(|| AlertState::new(rule_id));
    }

    /// Remove a rule
    pub fn remove_rule(&mut self, rule_id: &str) {
        self.rules.remove(rule_id);
        self.states.remove(rule_id);
    }

    /// Get a rule by ID
    pub fn get_rule(&self, rule_id: &str) -> Option<&AlertRule> {
        self.rules.get(rule_id)
    }

    /// Get all rules
    pub fn get_all_rules(&self) -> impl Iterator<Item = &AlertRule> {
        self.rules.values()
    }

    /// Get rule state
    pub fn get_state(&self, rule_id: &str) -> Option<&AlertState> {
        self.states.get(rule_id)
    }

    /// Evaluate all rules against a collection of telemetry data
    pub fn evaluate(&mut self, telemetry_data: &crate::plugins::TelemetryData) -> Vec<EvaluationResult> {
        let now = Utc::now();
        let mut results = Vec::new();

        // Collect rule IDs first to avoid borrow conflict
        let rule_ids: Vec<String> = self.rules.values()
            .filter(|rule| rule.enabled)
            .map(|rule| rule.id.clone())
            .collect();

        for rule_id in rule_ids {
            let result = self.evaluate_rule_internal(&rule_id, telemetry_data, now);
            self.update_state(&rule_id, &result, now);
            results.push(result);
        }

        results
    }

    /// Internal evaluation of a single rule (uses rule_id to avoid borrow issues)
    fn evaluate_rule_internal(&self, rule_id: &str, telemetry_data: &crate::plugins::TelemetryData, now: DateTime<Utc>) -> EvaluationResult {
        let rule = match self.rules.get(rule_id) {
            Some(r) => r,
            None => return EvaluationResult {
                rule_id: rule_id.to_string(),
                is_matched: false,
                matched_conditions: Vec::new(),
                triggering_metrics: Vec::new(),
                evaluated_at: now,
            },
        };

        let mut matched_conditions = Vec::new();
        let mut triggering_metrics = Vec::new();

        for (idx, condition) in rule.conditions.iter().enumerate() {
            for point in &telemetry_data.data_points {
                if !condition.matches_metric(&point.metric_name) {
                    continue;
                }

                if !condition.matches_labels(&point.labels) {
                    continue;
                }

                // Apply aggregation (simplified - just use latest value)
                let value = point.value;

                if condition.evaluate_point(value) {
                    matched_conditions.push(idx);
                    triggering_metrics.push(EvaluatedMetric {
                        metric_name: point.metric_name.clone(),
                        value,
                        labels: point.labels.clone(),
                        timestamp: point.timestamp,
                    });
                }
            }
        }

        let is_matched = self.check_logical_op(&matched_conditions, &rule.conditions, rule.condition_op);

        EvaluationResult {
            rule_id: rule.id.clone(),
            is_matched,
            matched_conditions,
            triggering_metrics,
            evaluated_at: now,
        }
    }

    /// Check if conditions match based on logical operator
    fn check_logical_op(
        &self,
        matched: &[usize],
        conditions: &[AlertCondition],
        op: LogicalOp,
    ) -> bool {
        if conditions.is_empty() {
            return false;
        }

        match op {
            LogicalOp::And => {
                // All conditions must be matched
                conditions.iter().enumerate().all(|(idx, _)| matched.contains(&idx))
            }
            LogicalOp::Or => {
                // At least one condition must be matched
                !matched.is_empty()
            }
        }
    }

    /// Update state after evaluation
    fn update_state(&mut self, rule_id: &str, result: &EvaluationResult, now: DateTime<Utc>) {
        let state = self.states.get_mut(rule_id);
        let Some(state) = state else {
            return;
        };

        state.last_evaluated_at = Some(now);

        if result.is_matched {
            if !state.is_firing {
                // Condition just started firing
                state.firing_since = Some(now);
                state.is_firing = true;
            }

            // Check cooldown
            if let Some(last_fired) = state.last_fired_at {
                let cooldown = self.rules.get(rule_id)
                    .map(|r| r.cooldown_seconds as i64)
                    .unwrap_or(300);
                if (now.timestamp() - last_fired.timestamp()) < cooldown {
                    // Still in cooldown, don't fire
                    return;
                }
            }

            // Check duration requirement
            let rule = self.rules.get(rule_id);
            if let (Some(rule), Some(since)) = (rule, state.firing_since) {
                if let Some(duration) = &rule.duration {
                    let firing_duration = now.timestamp() - since.timestamp();
                    if firing_duration < duration.for_seconds as i64 {
                        // Duration not met yet
                        return;
                    }
                }
            }

            // Fire the alert!
            self.fire_alert(rule_id, result, now);
        } else {
            // Condition no longer firing
            if state.is_firing {
                state.firing_since = None;
                state.is_firing = false;
            }
        }
    }

    /// Fire an alert
    fn fire_alert(&mut self, rule_id: &str, result: &EvaluationResult, now: DateTime<Utc>) {
        // Get rule (immutable borrow)
        let rule = match self.rules.get(rule_id) {
            Some(r) => r,
            None => return,
        };

        // Get the first triggered condition
        let condition = match result.matched_conditions.iter()
            .filter_map(|&idx| rule.conditions.get(idx))
            .next()
        {
            Some(c) => c,
            None => return,
        };

        let metric = result.triggering_metrics.first();

        // Create alert
        let alert = Alert {
            id: Alert::generate_id(),
            rule_id: rule_id.to_string(),
            rule_name: rule.name.clone(),
            severity: condition.severity,
            message: format!("[{}] {} triggered", condition.severity, rule.name),
            description: rule.description.clone(),
            metric_name: metric.map(|m| m.metric_name.clone()).unwrap_or_default(),
            metric_value: metric.map(|m| m.value).unwrap_or(0.0),
            threshold: condition.threshold,
            operator: condition.operator,
            labels: metric.map(|m| m.labels.clone()).unwrap_or_default(),
            fired_at: now,
            resolved_at: None,
            tags: rule.tags.clone(),
        };

        // Execute actions (needs immutable borrow of self)
        for action in &rule.actions {
            self.execute_action(action, &alert);
        }

        // Update state (mutable borrow)
        if let Some(state) = self.states.get_mut(rule_id) {
            state.last_fired_at = Some(now);
            state.fire_count += 1;
        }

        // Add to history
        self.recent_alerts.push(alert);
        if self.recent_alerts.len() > self.max_history {
            self.recent_alerts.remove(0);
        }
    }

    /// Execute an alert action
    fn execute_action(&self, action: &AlertAction, alert: &Alert) {
        match action {
            AlertAction::Log { level } => {
                match level.as_str() {
                    "error" => tracing::error!("[ALERT] {}", alert.message),
                    "warn" => tracing::warn!("[ALERT] {}", alert.message),
                    "info" => tracing::info!("[ALERT] {}", alert.message),
                    _ => tracing::debug!("[ALERT] {}", alert.message),
                }
            }
            AlertAction::Mqtt { topic, payload_template } => {
                // MQTT publishing would be handled by integration with mqtt module
                tracing::debug!(
                    "[ALERT:MQTT] topic={}, payload={:?}",
                    topic,
                    payload_template.as_ref().unwrap_or(&alert.message)
                );
            }
            AlertAction::Script { script_id, timeout_seconds } => {
                tracing::debug!(
                    "[ALERT:SCRIPT] script_id={}, timeout={}s",
                    script_id,
                    timeout_seconds
                );
                // Script execution would be handled by script_engine module
            }
            AlertAction::Webhook { url, headers } => {
                tracing::debug!(
                    "[ALERT:WEBHOOK] url={}, headers={:?}",
                    url,
                    headers
                );
                // Webhook execution would use reqwest or similar
            }
        }
    }

    /// Get recent alerts
    pub fn get_recent_alerts(&self, limit: usize) -> Vec<&Alert> {
        self.recent_alerts.iter().rev().take(limit).collect()
    }

    /// Get active (unresolved) alerts
    pub fn get_active_alerts(&self) -> Vec<&Alert> {
        self.recent_alerts.iter()
            .filter(|a| a.resolved_at.is_none())
            .collect()
    }

    /// Resolve an alert by ID
    pub fn resolve_alert(&mut self, alert_id: &str, resolved_at: DateTime<Utc>) {
        if let Some(alert) = self.recent_alerts.iter_mut().find(|a| a.id == alert_id) {
            alert.resolved_at = Some(resolved_at);
        }
    }
}

impl Default for AlertEngine {
    fn default() -> Self {
        Self::new()
    }
}

// ============================================================================
// Design Summary: AlertRule Model and Alert Engine
// ============================================================================
//
// ## 1. Telemetry Data Structure Summary
//
// The telemetry system uses these core types from plugins.rs:
//
//   TelemetryDataPoint {
//       metric_name: String,    // e.g., "cpu_usage_percent"
//       value: f64,             // metric value
//       unit: String,           // e.g., "percent", "bytes"
//       timestamp: DateTime<Utc>,
//       labels: HashMap<String, String>,  // e.g., {"type": "overall", "core": "cpu0"}
//   }
//
//   TelemetryData {
//       plugin_id: PluginId,
//       data_points: Vec<TelemetryDataPoint>,
//       collection_start: DateTime<Utc>,
//       collection_end: DateTime<Utc>,
//   }
//
// ## 2. Existing Alert Threshold (mqtt.rs)
//
//   AlertThreshold { warning: f64, critical: f64 }
//
// ## 3. AlertRule Model Design
//
//   AlertRule {
//       id: String,
//       name: String,
//       description: Option<String>,
//       enabled: bool,
//       conditions: Vec<AlertCondition>,  // combined with condition_op
//       condition_op: LogicalOp,          // And/Or
//       duration: Option<DurationRequirement>,  // for_seconds, min_percentage
//       cooldown_seconds: u64,            // prevent alert spam
//       actions: Vec<AlertAction>,
//       severity: Option<AlertSeverity>,
//       tags: HashMap<String, String>,
//       created_at/updated_at: DateTime<Utc>,
//   }
//
//   AlertCondition {
//       metric_name: String,      // supports wildcards: "cpu_*", "memory_*"
//       label_filter: HashMap,    // labels that must match
//       operator: ComparisonOp,  // Gt, Gte, Lt, Lte, Eq, Neq
//       threshold: f64,
//       aggregation: AggregationFunc,  // Latest, Min, Max, Avg, Sum, Count, StdDev, Rate
//       severity: AlertSeverity,
//   }
//
//   AlertAction types: Log, Mqtt, Script, Webhook
//
// ## 4. AlertEngine Core Logic
//
//   AlertEngine evaluates rules against TelemetryData:
//
//   evaluate(telemetry_data) -> Vec<EvaluationResult>
//     1. For each enabled rule:
//        - Check each condition against each data point
//        - Apply logical operator (AND/OR) to combine results
//        - Track matched conditions and triggering metrics
//     2. Update state (firing_since, is_firing)
//     3. Check duration requirement
//     4. Check cooldown period
//     5. If all checks pass, fire alert and execute actions
//
//   State machine:
//     - Normal -> Firing (condition met)
//     - Firing -> Firing (duration check, cooldown check)
//     - Firing -> Normal (condition no longer met)
//
// ## 5. Supported Metric Patterns
//
//   CPU Monitor (cc-cpu-monitor):
//     - cpu_usage_percent{type="overall"} -> 0-100
//     - cpu_usage_percent{type="per_core", core="cpu0"}
//     - cpu_load_average_1min/5min/15min{type="load_average"}
//     - cpu_core_count{type="info"}
//     - cpu_frequency_mhz{type="info"}
//
//   Memory Monitor (cc-memory-monitor):
//     - memory_total_bytes{type="total"}
//     - memory_used_bytes{type="used"}
//     - memory_available_bytes{type="available"}
//     - memory_usage_percent{type="usage"} -> 0-100
//
//   Disk Monitor (cc-disk-monitor):
//     - disk_total_bytes{type="total", mount_point="/"}
//     - disk_used_bytes{type="used", mount_point="/"}
//     - disk_available_bytes{type="available", mount_point="/"}
//     - disk_usage_percent{type="usage", mount_point="/"} -> 0-100
//
//   Network Monitor (cc-network-monitor):
//     - network_tcp_*/network_udp_*
//     - network_interface_*{interface="eth0"}
//
//   Process Monitor (cc-process-monitor):
//     - process_count{type="total|running|zombie"}
//     - process_cpu_percent{type="top_cpu", rank="1", name="nginx", pid="1234"}
//     - process_memory_bytes{type="top_memory", ...}
//
// ## 6. Example Alert Rules
//
//   # High CPU usage warning
//   AlertRule {
//       id: "high-cpu-warning",
//       name: "High CPU Usage Warning",
//       conditions: [{
//           metric_name: "cpu_usage_percent",
//           label_filter: {"type": "overall"},
//           operator: Gte,
//           threshold: 80.0,
//           severity: Warning,
//       }],
//       condition_op: And,
//       duration: { for_seconds: 60, min_percentage: 0.8 },
//       cooldown_seconds: 300,
//       actions: [Log{level: "warn"}, Mqtt{topic: "alerts/cpu"}],
//   }
//
//   # Critical memory usage
//   AlertRule {
//       id: "critical-memory",
//       name: "Critical Memory Usage",
//       conditions: [{
//           metric_name: "memory_usage_percent",
//           operator: Gt,
//           threshold: 95.0,
//           severity: Critical,
//       }],
//       cooldown_seconds: 60,
//       actions: [Log{level: "error"}, Script{script_id: "escalate"}],
//   }
//
//   # Disk space warning for root partition
//   AlertRule {
//       id: "disk-space-warning",
//       name: "Low Disk Space Warning",
//       conditions: [{
//           metric_name: "disk_usage_percent",
//           label_filter: {"mount_point": "/"},
//           operator: Gte,
//           threshold: 85.0,
//           severity: Warning,
//       }],
//       actions: [Log{level: "warn"}, Webhook{url: "https://hooks.example.com/disk"}],
//   }
//
// ============================================================================
