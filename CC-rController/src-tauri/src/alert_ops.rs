use std::collections::HashMap;
use std::fs;
use std::path::PathBuf;

use chrono::{DateTime, Utc};
use serde::{Deserialize, Serialize};
use tauri::command;
use uuid::Uuid;

use crate::models::Station;
use crate::storage::StateStore;

#[derive(Debug, Clone, Serialize, Deserialize, Default)]
#[serde(rename_all = "camelCase")]
pub struct AlertCondition {
    pub id: String,
    pub metric_type: String,
    pub operator: String,
    pub threshold: f64,
    pub threshold_high: Option<f64>,
    pub unit: Option<String>,
    pub evaluation_interval: Option<u32>,
}

#[derive(Debug, Clone, Serialize, Deserialize, Default)]
#[serde(rename_all = "camelCase")]
pub struct AlertConditionGroup {
    pub logic: String,
    #[serde(default)]
    pub conditions: Vec<AlertCondition>,
}

#[derive(Debug, Clone, Serialize, Deserialize, Default)]
#[serde(rename_all = "camelCase")]
pub struct AlertActionConfig {
    pub message: Option<String>,
    pub channels: Option<Vec<String>>,
    pub script_id: Option<String>,
    pub script_params: Option<HashMap<String, String>>,
    pub webhook_url: Option<String>,
    pub webhook_method: Option<String>,
    pub webhook_headers: Option<HashMap<String, String>>,
    pub webhook_payload: Option<String>,
    pub email_recipients: Option<Vec<String>>,
    pub remediation_script_id: Option<String>,
}

#[derive(Debug, Clone, Serialize, Deserialize, Default)]
#[serde(rename_all = "camelCase")]
pub struct AlertAction {
    pub id: String,
    pub action_type: String,
    pub name: String,
    pub enabled: bool,
    pub config: AlertActionConfig,
}

#[derive(Debug, Clone, Serialize, Deserialize, Default)]
#[serde(rename_all = "camelCase")]
pub struct AlertTargetSelector {
    pub selector_type: String,
    pub groups: Option<Vec<String>>,
    pub tags: Option<Vec<String>>,
    pub device_ids: Option<Vec<String>>,
    pub filter_expr: Option<String>,
}

#[derive(Debug, Clone, Serialize, Deserialize)]
#[serde(rename_all = "camelCase")]
pub struct AlertRule {
    pub id: String,
    pub name: String,
    pub description: Option<String>,
    pub condition_group: AlertConditionGroup,
    pub target_selector: AlertTargetSelector,
    pub severity: String,
    pub status: String,
    pub cooldown_secs: u32,
    pub max_triggers_per_hour: Option<u32>,
    #[serde(default)]
    pub actions: Vec<AlertAction>,
    pub is_template: bool,
    pub is_favorite: bool,
    #[serde(default)]
    pub tags: Vec<String>,
    pub created_by: String,
    pub created_at: String,
    pub updated_at: String,
    pub version: u32,
    pub usage_count: u64,
    pub last_triggered_at: Option<String>,
}

#[derive(Debug, Clone, Serialize, Deserialize, Default)]
#[serde(rename_all = "camelCase")]
pub struct AlertRuleDraft {
    pub id: Option<String>,
    pub name: Option<String>,
    pub description: Option<String>,
    pub condition_group: Option<AlertConditionGroup>,
    pub target_selector: Option<AlertTargetSelector>,
    pub severity: Option<String>,
    pub status: Option<String>,
    pub cooldown_secs: Option<u32>,
    pub max_triggers_per_hour: Option<u32>,
    pub actions: Option<Vec<AlertAction>>,
    pub is_template: Option<bool>,
    pub is_favorite: Option<bool>,
    pub tags: Option<Vec<String>>,
    pub created_by: Option<String>,
    pub created_at: Option<String>,
    pub updated_at: Option<String>,
    pub version: Option<u32>,
    pub usage_count: Option<u64>,
    pub last_triggered_at: Option<String>,
}

#[derive(Debug, Clone, Serialize, Deserialize)]
#[serde(rename_all = "camelCase")]
pub struct AlertActionResult {
    pub action_id: String,
    pub action_type: String,
    pub status: String,
    pub message: Option<String>,
    pub executed_at: String,
    pub duration_ms: Option<u64>,
}

#[derive(Debug, Clone, Serialize, Deserialize)]
#[serde(rename_all = "camelCase")]
pub struct AlertHistoryEntry {
    pub id: String,
    pub rule_id: String,
    pub rule_name: String,
    pub target_id: String,
    pub target_name: String,
    pub severity: String,
    pub status: String,
    pub trigger_value: f64,
    pub threshold: f64,
    pub condition: String,
    pub triggered_at: String,
    pub resolved_at: Option<String>,
    pub acknowledged_at: Option<String>,
    pub acknowledged_by: Option<String>,
    #[serde(default)]
    pub actions_triggered: Vec<String>,
    #[serde(default)]
    pub action_results: Vec<AlertActionResult>,
    pub metadata: Option<HashMap<String, serde_json::Value>>,
}

#[derive(Debug, Clone, Deserialize, Default)]
#[serde(rename_all = "camelCase")]
pub struct AlertHistoryFilter {
    pub rule_id: Option<String>,
    pub target_id: Option<String>,
    pub severity: Option<Vec<String>>,
    pub status: Option<Vec<String>>,
    pub date_from: Option<String>,
    pub date_to: Option<String>,
}

#[derive(Debug, Clone, Serialize)]
#[serde(rename_all = "camelCase")]
pub struct AlertStats {
    pub total_rules: usize,
    pub enabled_rules: usize,
    pub triggered_today: usize,
    pub critical_alerts: usize,
    pub average_response_time_ms: f64,
}

#[derive(Debug, Clone, Serialize, Deserialize)]
#[serde(rename_all = "camelCase")]
pub struct AlertRulePackageMetadata {
    pub id: String,
    pub name: String,
    pub version: String,
    pub description: Option<String>,
    pub author: Option<String>,
    pub tags: Vec<String>,
    pub created_at: String,
    pub updated_at: String,
}

#[derive(Debug, Clone, Serialize, Deserialize)]
#[serde(rename_all = "camelCase")]
pub struct AlertRulePackage {
    pub metadata: AlertRulePackageMetadata,
    pub rules: Vec<AlertRule>,
    pub signature: Option<String>,
}

#[derive(Debug, Clone, Serialize)]
#[serde(rename_all = "camelCase")]
pub struct AlertRuleImportResult {
    pub success: bool,
    pub imported: usize,
    pub skipped: usize,
    pub errors: Vec<String>,
    pub warnings: Vec<String>,
}

#[derive(Debug, Clone, Serialize)]
#[serde(rename_all = "camelCase")]
pub struct AlertRuleValidationResult {
    pub valid: bool,
    pub errors: Vec<String>,
    pub warnings: Vec<String>,
}

#[derive(Debug, Clone, Serialize)]
#[serde(rename_all = "camelCase")]
pub struct LoadAlertRulesResult {
    pub rules: Vec<AlertRule>,
}

#[derive(Debug, Clone, Serialize)]
#[serde(rename_all = "camelCase")]
pub struct SaveAlertRuleResult {
    pub rule: AlertRule,
}

#[derive(Debug, Clone, Serialize)]
#[serde(rename_all = "camelCase")]
pub struct GetAlertHistoryResult {
    pub history: Vec<AlertHistoryEntry>,
    pub total: usize,
}

#[derive(Debug, Clone, Serialize)]
#[serde(rename_all = "camelCase")]
pub struct GetAlertStatsResult {
    pub stats: AlertStats,
}

#[derive(Debug, Clone, Serialize)]
#[serde(rename_all = "camelCase")]
pub struct GetAlertTemplatesResult {
    pub templates: Vec<AlertRule>,
}

#[derive(Debug, Clone, Serialize)]
#[serde(rename_all = "camelCase")]
pub struct AlertTargetInfo {
    pub id: String,
    pub name: String,
    pub status: Option<String>,
}

#[derive(Debug, Clone, Serialize)]
#[serde(rename_all = "camelCase")]
pub struct PreviewTargetsResult {
    pub targets: Vec<AlertTargetInfo>,
    pub total: usize,
}

fn cc_client_dir() -> Result<PathBuf, String> {
    let home =
        dirs::home_dir().ok_or_else(|| "Unable to resolve the user home directory.".to_string())?;
    let path = home.join(".CC-rController");
    fs::create_dir_all(&path).map_err(|error| format!("create {}: {error}", path.display()))?;
    Ok(path)
}

fn alert_rules_path() -> Result<PathBuf, String> {
    Ok(cc_client_dir()?.join("alert_rules.json"))
}

fn alert_history_path() -> Result<PathBuf, String> {
    Ok(cc_client_dir()?.join("alert_history.json"))
}

fn read_json_vec<T>(path: &PathBuf) -> Result<Vec<T>, String>
where
    T: for<'de> Deserialize<'de>,
{
    if !path.exists() {
        return Ok(Vec::new());
    }
    let raw =
        fs::read_to_string(path).map_err(|error| format!("read {}: {error}", path.display()))?;
    serde_json::from_str(&raw).map_err(|error| format!("parse {}: {error}", path.display()))
}

fn write_json_vec<T>(path: &PathBuf, items: &[T]) -> Result<(), String>
where
    T: Serialize,
{
    let raw = serde_json::to_string_pretty(items)
        .map_err(|error| format!("serialize {}: {error}", path.display()))?;
    fs::write(path, raw).map_err(|error| format!("write {}: {error}", path.display()))
}

fn load_rules() -> Result<Vec<AlertRule>, String> {
    read_json_vec(&alert_rules_path()?)
}

fn save_rules(rules: &[AlertRule]) -> Result<(), String> {
    write_json_vec(&alert_rules_path()?, rules)
}

fn load_history_entries() -> Result<Vec<AlertHistoryEntry>, String> {
    read_json_vec(&alert_history_path()?)
}

fn save_history_entries(history: &[AlertHistoryEntry]) -> Result<(), String> {
    write_json_vec(&alert_history_path()?, history)
}

fn now_iso() -> String {
    Utc::now().to_rfc3339()
}

fn normalize_rule(rule: AlertRuleDraft, existing: Option<&AlertRule>) -> AlertRule {
    let now = now_iso();
    let base = existing.cloned();
    AlertRule {
        id: rule
            .id
            .or_else(|| base.as_ref().map(|item| item.id.clone()))
            .unwrap_or_else(|| Uuid::new_v4().to_string()),
        name: rule
            .name
            .or_else(|| base.as_ref().map(|item| item.name.clone()))
            .unwrap_or_default(),
        description: rule
            .description
            .or_else(|| base.as_ref().and_then(|item| item.description.clone())),
        condition_group: rule
            .condition_group
            .or_else(|| base.as_ref().map(|item| item.condition_group.clone()))
            .unwrap_or_default(),
        target_selector: rule
            .target_selector
            .or_else(|| base.as_ref().map(|item| item.target_selector.clone()))
            .unwrap_or_default(),
        severity: rule
            .severity
            .or_else(|| base.as_ref().map(|item| item.severity.clone()))
            .unwrap_or_else(|| "warning".to_string()),
        status: rule
            .status
            .or_else(|| base.as_ref().map(|item| item.status.clone()))
            .unwrap_or_else(|| "enabled".to_string()),
        cooldown_secs: rule
            .cooldown_secs
            .or_else(|| base.as_ref().map(|item| item.cooldown_secs))
            .unwrap_or(300),
        max_triggers_per_hour: rule
            .max_triggers_per_hour
            .or_else(|| base.as_ref().and_then(|item| item.max_triggers_per_hour)),
        actions: rule
            .actions
            .or_else(|| base.as_ref().map(|item| item.actions.clone()))
            .unwrap_or_default(),
        is_template: rule
            .is_template
            .or_else(|| base.as_ref().map(|item| item.is_template))
            .unwrap_or(false),
        is_favorite: rule
            .is_favorite
            .or_else(|| base.as_ref().map(|item| item.is_favorite))
            .unwrap_or(false),
        tags: rule
            .tags
            .or_else(|| base.as_ref().map(|item| item.tags.clone()))
            .unwrap_or_default(),
        created_by: rule
            .created_by
            .or_else(|| base.as_ref().map(|item| item.created_by.clone()))
            .unwrap_or_else(|| "current-user".to_string()),
        created_at: rule
            .created_at
            .or_else(|| base.as_ref().map(|item| item.created_at.clone()))
            .unwrap_or_else(|| now.clone()),
        updated_at: rule.updated_at.unwrap_or(now),
        version: rule
            .version
            .or_else(|| base.as_ref().map(|item| item.version + 1))
            .unwrap_or(1),
        usage_count: rule
            .usage_count
            .or_else(|| base.as_ref().map(|item| item.usage_count))
            .unwrap_or(0),
        last_triggered_at: rule.last_triggered_at.or_else(|| {
            base.as_ref()
                .and_then(|item| item.last_triggered_at.clone())
        }),
    }
}

fn validate_rule(rule: &AlertRuleDraft) -> AlertRuleValidationResult {
    let mut errors = Vec::new();
    let mut warnings = Vec::new();

    if rule.name.as_deref().unwrap_or("").trim().is_empty() {
        errors.push("Rule name is required".to_string());
    }

    let condition_count = rule
        .condition_group
        .as_ref()
        .map(|group| group.conditions.len())
        .unwrap_or(0);
    if condition_count == 0 {
        errors.push("At least one alert condition is required".to_string());
    }

    if let Some(actions) = &rule.actions {
        if actions.is_empty() {
            warnings.push(
                "No alert actions are configured; the rule will only write history".to_string(),
            );
        }
    }

    AlertRuleValidationResult {
        valid: errors.is_empty(),
        errors,
        warnings,
    }
}

fn station_matches_selector(station: &Station, selector: &AlertTargetSelector) -> bool {
    match selector.selector_type.as_str() {
        "" | "all" => true,
        "group" => selector
            .groups
            .as_ref()
            .map(|groups| {
                groups.iter().any(|group_id| {
                    station
                        .groups
                        .iter()
                        .any(|station_group| station_group == group_id)
                })
            })
            .unwrap_or(false),
        "tag" => selector
            .tags
            .as_ref()
            .map(|tags| tags.iter().any(|tag| station.tags.contains_key(tag)))
            .unwrap_or(false),
        "device_ids" => selector
            .device_ids
            .as_ref()
            .map(|ids| ids.iter().any(|id| id == &station.id))
            .unwrap_or(false),
        "filter" => match selector.filter_expr.as_deref().map(str::trim) {
            Some(expr) if !expr.is_empty() => {
                station.id.contains(expr)
                    || station.name.contains(expr)
                    || station.groups.iter().any(|group| group.contains(expr))
            }
            _ => false,
        },
        _ => false,
    }
}

fn resolve_targets(selector: &AlertTargetSelector) -> Result<Vec<AlertTargetInfo>, String> {
    let snapshot = StateStore::load_snapshot().map_err(|error| error.to_string())?;
    Ok(snapshot
        .stations
        .into_iter()
        .filter(|station| station_matches_selector(station, selector))
        .map(|station| AlertTargetInfo {
            id: station.id,
            name: station.name,
            status: Some(if station.blocked { "offline" } else { "online" }.to_string()),
        })
        .collect::<Vec<_>>())
}

fn parse_date(value: &str) -> Option<DateTime<Utc>> {
    DateTime::parse_from_rfc3339(value)
        .ok()
        .map(|parsed| parsed.with_timezone(&Utc))
}

fn build_templates() -> Vec<AlertRule> {
    let now = now_iso();
    vec![
        AlertRule {
            id: "template-high-cpu".to_string(),
            name: "High CPU Usage".to_string(),
            description: Some("Triggers when CPU usage stays high.".to_string()),
            condition_group: AlertConditionGroup {
                logic: "and".to_string(),
                conditions: vec![AlertCondition {
                    id: Uuid::new_v4().to_string(),
                    metric_type: "cpu_usage".to_string(),
                    operator: "gte".to_string(),
                    threshold: 85.0,
                    threshold_high: None,
                    unit: Some("%".to_string()),
                    evaluation_interval: Some(10),
                }],
            },
            target_selector: AlertTargetSelector {
                selector_type: "all".to_string(),
                groups: None,
                tags: None,
                device_ids: None,
                filter_expr: None,
            },
            severity: "warning".to_string(),
            status: "enabled".to_string(),
            cooldown_secs: 300,
            max_triggers_per_hour: Some(6),
            actions: vec![AlertAction {
                id: Uuid::new_v4().to_string(),
                action_type: "notification".to_string(),
                name: "In-app notification".to_string(),
                enabled: true,
                config: AlertActionConfig {
                    message: Some("CPU usage is above threshold".to_string()),
                    channels: Some(vec!["in_app".to_string()]),
                    ..AlertActionConfig::default()
                },
            }],
            is_template: true,
            is_favorite: false,
            tags: vec!["cpu".to_string(), "system".to_string()],
            created_by: "CC-rController".to_string(),
            created_at: now.clone(),
            updated_at: now.clone(),
            version: 1,
            usage_count: 0,
            last_triggered_at: None,
        },
        AlertRule {
            id: "template-low-memory".to_string(),
            name: "Low Memory Available".to_string(),
            description: Some("Triggers when available memory drops too low.".to_string()),
            condition_group: AlertConditionGroup {
                logic: "and".to_string(),
                conditions: vec![AlertCondition {
                    id: Uuid::new_v4().to_string(),
                    metric_type: "memory_available".to_string(),
                    operator: "lt".to_string(),
                    threshold: 1024.0,
                    threshold_high: None,
                    unit: Some("MB".to_string()),
                    evaluation_interval: Some(10),
                }],
            },
            target_selector: AlertTargetSelector {
                selector_type: "all".to_string(),
                groups: None,
                tags: None,
                device_ids: None,
                filter_expr: None,
            },
            severity: "critical".to_string(),
            status: "enabled".to_string(),
            cooldown_secs: 180,
            max_triggers_per_hour: Some(8),
            actions: vec![AlertAction {
                id: Uuid::new_v4().to_string(),
                action_type: "notification".to_string(),
                name: "In-app notification".to_string(),
                enabled: true,
                config: AlertActionConfig {
                    message: Some("Available memory dropped below threshold".to_string()),
                    channels: Some(vec!["in_app".to_string()]),
                    ..AlertActionConfig::default()
                },
            }],
            is_template: true,
            is_favorite: false,
            tags: vec!["memory".to_string(), "system".to_string()],
            created_by: "CC-rController".to_string(),
            created_at: now.clone(),
            updated_at: now,
            version: 1,
            usage_count: 0,
            last_triggered_at: None,
        },
    ]
}

#[command]
pub fn load_alert_rules() -> Result<LoadAlertRulesResult, String> {
    let mut rules = load_rules()?;
    rules.sort_by(|left, right| right.updated_at.cmp(&left.updated_at));
    Ok(LoadAlertRulesResult { rules })
}

#[command]
pub fn save_alert_rule(rule: AlertRuleDraft) -> Result<SaveAlertRuleResult, String> {
    let validation = validate_rule(&rule);
    if !validation.valid {
        return Err(validation.errors.join("; "));
    }

    let mut rules = load_rules()?;
    let existing_index = rule
        .id
        .as_ref()
        .and_then(|id| rules.iter().position(|existing| &existing.id == id));
    let existing = existing_index.and_then(|index| rules.get(index).cloned());
    let normalized = normalize_rule(rule, existing.as_ref());

    if let Some(index) = existing_index {
        rules[index] = normalized.clone();
    } else {
        rules.push(normalized.clone());
    }
    save_rules(&rules)?;
    Ok(SaveAlertRuleResult { rule: normalized })
}

#[command]
pub fn delete_alert_rule(rule_id: String) -> Result<HashMap<&'static str, bool>, String> {
    let mut rules = load_rules()?;
    let before = rules.len();
    rules.retain(|rule| rule.id != rule_id);
    save_rules(&rules)?;
    Ok(HashMap::from([("success", before != rules.len())]))
}

#[command]
pub fn toggle_alert_rule(rule_id: String) -> Result<(), String> {
    let mut rules = load_rules()?;
    let rule = rules
        .iter_mut()
        .find(|rule| rule.id == rule_id)
        .ok_or_else(|| format!("No alert rule found for id {rule_id}"))?;
    rule.status = if rule.status == "enabled" {
        "disabled".to_string()
    } else {
        "enabled".to_string()
    };
    rule.updated_at = now_iso();
    save_rules(&rules)
}

#[command]
pub fn duplicate_alert_rule(rule_id: String) -> Result<SaveAlertRuleResult, String> {
    let rules = load_rules()?;
    let original = rules
        .iter()
        .find(|rule| rule.id == rule_id)
        .cloned()
        .ok_or_else(|| format!("No alert rule found for id {rule_id}"))?;
    let copy = AlertRule {
        id: Uuid::new_v4().to_string(),
        name: format!("{} (Copy)", original.name.clone()),
        created_at: now_iso(),
        updated_at: now_iso(),
        usage_count: 0,
        last_triggered_at: None,
        is_favorite: false,
        ..original
    };
    let mut next = rules;
    next.push(copy.clone());
    save_rules(&next)?;
    Ok(SaveAlertRuleResult { rule: copy })
}

#[command]
pub fn get_alert_history(
    filters: Option<AlertHistoryFilter>,
    limit: Option<usize>,
) -> Result<GetAlertHistoryResult, String> {
    let mut history = load_history_entries()?;
    if let Some(filters) = filters {
        history.retain(|entry| {
            let rule_match = filters
                .rule_id
                .as_ref()
                .map(|rule_id| &entry.rule_id == rule_id)
                .unwrap_or(true);
            let target_match = filters
                .target_id
                .as_ref()
                .map(|target_id| &entry.target_id == target_id)
                .unwrap_or(true);
            let severity_match = filters
                .severity
                .as_ref()
                .map(|values| {
                    values.is_empty() || values.iter().any(|value| value == &entry.severity)
                })
                .unwrap_or(true);
            let status_match = filters
                .status
                .as_ref()
                .map(|values| {
                    values.is_empty() || values.iter().any(|value| value == &entry.status)
                })
                .unwrap_or(true);
            let from_match = filters
                .date_from
                .as_ref()
                .and_then(|value| parse_date(value))
                .map(|from| {
                    parse_date(&entry.triggered_at)
                        .map(|entry_date| entry_date >= from)
                        .unwrap_or(false)
                })
                .unwrap_or(true);
            let to_match = filters
                .date_to
                .as_ref()
                .and_then(|value| parse_date(value))
                .map(|to| {
                    parse_date(&entry.triggered_at)
                        .map(|entry_date| entry_date <= to)
                        .unwrap_or(false)
                })
                .unwrap_or(true);
            rule_match && target_match && severity_match && status_match && from_match && to_match
        });
    }
    history.sort_by(|left, right| right.triggered_at.cmp(&left.triggered_at));
    let total = history.len();
    if let Some(limit) = limit {
        history.truncate(limit);
    }
    Ok(GetAlertHistoryResult { history, total })
}

#[command]
pub fn get_alert_stats() -> Result<GetAlertStatsResult, String> {
    let rules = load_rules()?;
    let history = load_history_entries()?;
    let today = Utc::now().date_naive();
    let triggered_today = history
        .iter()
        .filter(|entry| {
            parse_date(&entry.triggered_at)
                .map(|date| date.date_naive() == today)
                .unwrap_or(false)
        })
        .count();
    let critical_alerts = history
        .iter()
        .filter(|entry| entry.severity == "critical" && entry.status == "triggered")
        .count();
    let average_response_time_ms = {
        let durations = history
            .iter()
            .filter_map(|entry| match (&entry.acknowledged_at, &entry.resolved_at) {
                (Some(ack), _) => parse_date(&entry.triggered_at)
                    .zip(parse_date(ack))
                    .map(|(start, end)| (end - start).num_milliseconds().max(0) as f64),
                (_, Some(resolved)) => parse_date(&entry.triggered_at)
                    .zip(parse_date(resolved))
                    .map(|(start, end)| (end - start).num_milliseconds().max(0) as f64),
                _ => None,
            })
            .collect::<Vec<_>>();
        if durations.is_empty() {
            0.0
        } else {
            durations.iter().sum::<f64>() / durations.len() as f64
        }
    };
    Ok(GetAlertStatsResult {
        stats: AlertStats {
            total_rules: rules.len(),
            enabled_rules: rules.iter().filter(|rule| rule.status == "enabled").count(),
            triggered_today,
            critical_alerts,
            average_response_time_ms,
        },
    })
}

#[command]
pub fn validate_alert_rule(rule: AlertRuleDraft) -> Result<AlertRuleValidationResult, String> {
    Ok(validate_rule(&rule))
}

#[command]
pub fn preview_alert_targets(
    selector: AlertTargetSelector,
) -> Result<PreviewTargetsResult, String> {
    let targets = resolve_targets(&selector)?;
    Ok(PreviewTargetsResult {
        total: targets.len(),
        targets,
    })
}

#[command]
pub fn get_alert_available_targets() -> Result<PreviewTargetsResult, String> {
    let targets = resolve_targets(&AlertTargetSelector {
        selector_type: "all".to_string(),
        groups: None,
        tags: None,
        device_ids: None,
        filter_expr: None,
    })?;
    Ok(PreviewTargetsResult {
        total: targets.len(),
        targets,
    })
}

#[command]
pub fn import_alert_rules(
    pkg: AlertRulePackage,
    options: Option<HashMap<String, bool>>,
) -> Result<AlertRuleImportResult, String> {
    let overwrite = options
        .as_ref()
        .and_then(|values| values.get("overwrite").copied())
        .unwrap_or(false);
    let import_as_copies = options
        .as_ref()
        .and_then(|values| values.get("importAsCopies").copied())
        .unwrap_or(false);

    let mut existing = load_rules()?;
    let mut imported = 0usize;
    let mut skipped = 0usize;

    for mut rule in pkg.rules {
        if import_as_copies || existing.iter().any(|current| current.id == rule.id) && !overwrite {
            rule.id = Uuid::new_v4().to_string();
            rule.name = format!("{} (Imported)", rule.name);
            rule.created_at = now_iso();
            rule.updated_at = now_iso();
        }

        if let Some(index) = existing.iter().position(|current| current.id == rule.id) {
            if overwrite {
                existing[index] = rule;
                imported += 1;
            } else {
                skipped += 1;
            }
        } else {
            existing.push(rule);
            imported += 1;
        }
    }

    save_rules(&existing)?;
    Ok(AlertRuleImportResult {
        success: true,
        imported,
        skipped,
        errors: Vec::new(),
        warnings: Vec::new(),
    })
}

#[command]
pub fn export_alert_rules(
    rule_ids: Vec<String>,
    metadata: Option<AlertRulePackageMetadata>,
) -> Result<HashMap<&'static str, AlertRulePackage>, String> {
    let rules = load_rules()?
        .into_iter()
        .filter(|rule| rule_ids.iter().any(|id| id == &rule.id))
        .collect::<Vec<_>>();
    let now = now_iso();
    let package = AlertRulePackage {
        metadata: metadata.unwrap_or(AlertRulePackageMetadata {
            id: Uuid::new_v4().to_string(),
            name: "Exported Alert Rules".to_string(),
            version: "1.0.0".to_string(),
            description: Some("Exported from CC-rController".to_string()),
            author: Some("CC-rController".to_string()),
            tags: Vec::new(),
            created_at: now.clone(),
            updated_at: now,
        }),
        rules,
        signature: None,
    };
    Ok(HashMap::from([("package", package)]))
}

#[command]
pub fn acknowledge_alert(history_id: String, note: Option<String>) -> Result<(), String> {
    let mut history = load_history_entries()?;
    let entry = history
        .iter_mut()
        .find(|entry| entry.id == history_id)
        .ok_or_else(|| format!("No alert history entry found for id {history_id}"))?;
    entry.status = "acknowledged".to_string();
    entry.acknowledged_at = Some(now_iso());
    entry.acknowledged_by = note.or_else(|| Some("current-user".to_string()));
    save_history_entries(&history)
}

#[command]
pub fn resolve_alert(history_id: String) -> Result<(), String> {
    let mut history = load_history_entries()?;
    let entry = history
        .iter_mut()
        .find(|entry| entry.id == history_id)
        .ok_or_else(|| format!("No alert history entry found for id {history_id}"))?;
    entry.status = "resolved".to_string();
    entry.resolved_at = Some(now_iso());
    save_history_entries(&history)
}

#[command]
pub fn get_alert_templates() -> Result<GetAlertTemplatesResult, String> {
    Ok(GetAlertTemplatesResult {
        templates: build_templates(),
    })
}

#[command]
pub fn append_alert_history(
    entry: AlertHistoryEntry,
    rule_id: Option<String>,
) -> Result<AlertHistoryEntry, String> {
    let mut history = load_history_entries()?;
    history.push(entry.clone());
    if history.len() > 500 {
        let drain_count = history.len() - 500;
        history.drain(..drain_count);
    }
    save_history_entries(&history)?;

    if let Some(rule_id) = rule_id {
        let mut rules = load_rules()?;
        if let Some(rule) = rules.iter_mut().find(|rule| rule.id == rule_id) {
            rule.last_triggered_at = Some(entry.triggered_at.clone());
            rule.updated_at = now_iso();
            rule.usage_count += 1;
            save_rules(&rules)?;
        }
    }

    Ok(entry)
}
