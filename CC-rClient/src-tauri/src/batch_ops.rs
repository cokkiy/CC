use std::collections::HashMap;
use std::fs;
use std::path::PathBuf;

use chrono::Utc;
use futures_util::stream::{self, StreamExt};
use serde::{Deserialize, Serialize};
use tauri::command;
use tokio::time::{Duration, sleep, timeout};
use uuid::Uuid;

use crate::control::execute_station_action;
use crate::models::{Station, StationAction};
use crate::remote::{execute_station_command, set_station_watching_apps};
use crate::storage::StateStore;

const DEFAULT_BATCH_TIMEOUT_SECS: u64 = 300;

#[derive(Debug, Clone, Serialize, Deserialize, Default)]
#[serde(rename_all = "camelCase")]
pub struct BatchTaskParameter {
    pub name: String,
    pub param_type: String,
    pub default_value: String,
    pub required: bool,
    pub validation: Option<String>,
    pub description: Option<String>,
    pub options: Option<Vec<String>>,
}

#[derive(Debug, Clone, Serialize, Deserialize, Default)]
#[serde(rename_all = "camelCase")]
pub struct TargetSelector {
    pub selector_type: String,
    pub groups: Option<Vec<String>>,
    pub tags: Option<Vec<String>>,
    pub device_ids: Option<Vec<String>>,
    pub filter_expr: Option<String>,
}

#[derive(Debug, Clone, Serialize, Deserialize)]
#[serde(rename_all = "camelCase")]
pub struct ExecutionPolicy {
    pub mode: String,
    pub batch_size: Option<u32>,
    pub continue_on_failure: bool,
    pub failure_threshold_percent: u32,
    pub timeout_secs: u32,
    pub retry_count: u32,
    pub retry_delay_secs: Option<u32>,
}

impl Default for ExecutionPolicy {
    fn default() -> Self {
        Self {
            mode: "parallel".to_string(),
            batch_size: Some(5),
            continue_on_failure: true,
            failure_threshold_percent: 50,
            timeout_secs: DEFAULT_BATCH_TIMEOUT_SECS as u32,
            retry_count: 0,
            retry_delay_secs: Some(5),
        }
    }
}

#[derive(Debug, Clone, Serialize, Deserialize)]
#[serde(rename_all = "camelCase")]
pub struct BatchTask {
    pub id: String,
    pub name: String,
    pub description: Option<String>,
    pub task_type: String,
    pub target_selector: TargetSelector,
    pub content: String,
    #[serde(default)]
    pub parameters: Vec<BatchTaskParameter>,
    #[serde(default)]
    pub execution_policy: ExecutionPolicy,
    pub created_by: String,
    pub created_at: String,
    pub updated_at: String,
    pub status: String,
    pub version: u32,
    pub usage_count: u64,
    pub last_run_at: Option<String>,
    #[serde(default)]
    pub is_favorite: bool,
    #[serde(default)]
    pub tags: Vec<String>,
}

#[derive(Debug, Clone, Serialize, Deserialize, Default)]
#[serde(rename_all = "camelCase")]
pub struct BatchTaskDraft {
    pub id: Option<String>,
    pub name: Option<String>,
    pub description: Option<String>,
    pub task_type: Option<String>,
    pub target_selector: Option<TargetSelector>,
    pub content: Option<String>,
    pub parameters: Option<Vec<BatchTaskParameter>>,
    pub execution_policy: Option<ExecutionPolicy>,
    pub created_by: Option<String>,
    pub created_at: Option<String>,
    pub updated_at: Option<String>,
    pub status: Option<String>,
    pub version: Option<u32>,
    pub usage_count: Option<u64>,
    pub last_run_at: Option<String>,
    pub is_favorite: Option<bool>,
    pub tags: Option<Vec<String>>,
}

#[derive(Debug, Clone, Serialize, Deserialize)]
#[serde(rename_all = "camelCase")]
pub struct BatchTarget {
    pub id: String,
    pub name: String,
    pub group: Option<String>,
    pub tags: Option<HashMap<String, String>>,
    pub status: String,
    pub last_seen: Option<String>,
}

#[derive(Debug, Clone, Serialize, Deserialize)]
#[serde(rename_all = "camelCase")]
pub struct BatchTargetResult {
    pub target_id: String,
    pub target_name: String,
    pub status: String,
    pub started_at: String,
    pub completed_at: Option<String>,
    pub duration_ms: Option<u64>,
    pub output: Option<String>,
    pub error: Option<String>,
    pub exit_code: Option<i32>,
    pub retry_attempt: Option<u32>,
}

#[derive(Debug, Clone, Serialize, Deserialize)]
#[serde(rename_all = "camelCase")]
pub struct BatchExecutionResult {
    pub task_id: String,
    pub execution_id: String,
    pub status: String,
    pub started_at: String,
    pub completed_at: Option<String>,
    pub total_targets: usize,
    pub success_count: usize,
    pub failure_count: usize,
    pub skipped_count: usize,
    pub results: Vec<BatchTargetResult>,
    pub circuit_breaker_triggered: Option<bool>,
    pub failure_rate: Option<f64>,
}

#[derive(Debug, Clone, Serialize, Deserialize)]
#[serde(rename_all = "camelCase")]
pub struct BatchExecutionStatusResponse {
    pub execution_id: String,
    pub status: String,
    pub progress: BatchExecutionProgress,
}

#[derive(Debug, Clone, Serialize, Deserialize)]
#[serde(rename_all = "camelCase")]
pub struct BatchExecutionProgress {
    pub total: usize,
    pub completed: usize,
    pub failed: usize,
    pub running: usize,
}

#[derive(Debug, Clone, Serialize, Deserialize)]
#[serde(rename_all = "camelCase")]
pub struct BatchTaskPackageMetadata {
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
pub struct BatchTaskPackage {
    pub metadata: BatchTaskPackageMetadata,
    pub tasks: Vec<BatchTask>,
    pub signature: Option<String>,
}

#[derive(Debug, Clone, Serialize)]
#[serde(rename_all = "camelCase")]
pub struct BatchTaskImportResult {
    pub success: bool,
    pub imported: usize,
    pub skipped: usize,
    pub errors: Vec<String>,
    pub warnings: Vec<String>,
}

#[derive(Debug, Clone, Serialize)]
#[serde(rename_all = "camelCase")]
pub struct ValidateTaskResult {
    pub valid: bool,
    pub errors: Vec<String>,
    pub warnings: Vec<String>,
}

#[derive(Debug, Clone, Deserialize, Default)]
#[serde(rename_all = "camelCase")]
pub struct BatchTargetFilters {
    pub groups: Option<Vec<String>>,
    pub tags: Option<Vec<String>>,
    pub status: Option<Vec<String>>,
}

#[derive(Debug, Clone, Serialize)]
#[serde(rename_all = "camelCase")]
pub struct LoadBatchTasksResult {
    pub tasks: Vec<BatchTask>,
}

#[derive(Debug, Clone, Serialize)]
#[serde(rename_all = "camelCase")]
pub struct SaveBatchTaskResult {
    pub task: BatchTask,
}

#[derive(Debug, Clone, Serialize)]
#[serde(rename_all = "camelCase")]
pub struct ExecuteBatchTaskResult {
    pub execution: BatchExecutionResult,
}

#[derive(Debug, Clone, Serialize)]
#[serde(rename_all = "camelCase")]
pub struct GetTargetsResult {
    pub targets: Vec<BatchTarget>,
    pub total: usize,
}

#[derive(Debug, Clone, Serialize)]
#[serde(rename_all = "camelCase")]
pub struct GetBatchHistoryResult {
    pub history: Vec<BatchExecutionResult>,
}

fn cc_client_dir() -> Result<PathBuf, String> {
    let home =
        dirs::home_dir().ok_or_else(|| "Unable to resolve the user home directory.".to_string())?;
    let path = home.join(".CC-rClient");
    fs::create_dir_all(&path).map_err(|error| format!("create {}: {error}", path.display()))?;
    Ok(path)
}

fn batch_tasks_path() -> Result<PathBuf, String> {
    Ok(cc_client_dir()?.join("batch_tasks.json"))
}

fn batch_history_path() -> Result<PathBuf, String> {
    Ok(cc_client_dir()?.join("batch_history.json"))
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

fn load_tasks() -> Result<Vec<BatchTask>, String> {
    read_json_vec(&batch_tasks_path()?)
}

fn save_tasks(tasks: &[BatchTask]) -> Result<(), String> {
    write_json_vec(&batch_tasks_path()?, tasks)
}

fn load_history() -> Result<Vec<BatchExecutionResult>, String> {
    read_json_vec(&batch_history_path()?)
}

fn save_history(history: &[BatchExecutionResult]) -> Result<(), String> {
    write_json_vec(&batch_history_path()?, history)
}

fn now_iso() -> String {
    Utc::now().to_rfc3339()
}

fn normalize_task(task: BatchTaskDraft, existing: Option<&BatchTask>) -> BatchTask {
    let now = now_iso();
    let base = existing.cloned();
    BatchTask {
        id: task
            .id
            .or_else(|| base.as_ref().map(|item| item.id.clone()))
            .unwrap_or_else(|| Uuid::new_v4().to_string()),
        name: task
            .name
            .or_else(|| base.as_ref().map(|item| item.name.clone()))
            .unwrap_or_default(),
        description: task
            .description
            .or_else(|| base.as_ref().and_then(|item| item.description.clone())),
        task_type: task
            .task_type
            .or_else(|| base.as_ref().map(|item| item.task_type.clone()))
            .unwrap_or_else(|| "command".to_string()),
        target_selector: task
            .target_selector
            .or_else(|| base.as_ref().map(|item| item.target_selector.clone()))
            .unwrap_or_default(),
        content: task
            .content
            .or_else(|| base.as_ref().map(|item| item.content.clone()))
            .unwrap_or_default(),
        parameters: task
            .parameters
            .or_else(|| base.as_ref().map(|item| item.parameters.clone()))
            .unwrap_or_default(),
        execution_policy: task
            .execution_policy
            .or_else(|| base.as_ref().map(|item| item.execution_policy.clone()))
            .unwrap_or_default(),
        created_by: task
            .created_by
            .or_else(|| base.as_ref().map(|item| item.created_by.clone()))
            .unwrap_or_else(|| "current-user".to_string()),
        created_at: task
            .created_at
            .or_else(|| base.as_ref().map(|item| item.created_at.clone()))
            .unwrap_or_else(|| now.clone()),
        updated_at: task.updated_at.unwrap_or(now),
        status: task
            .status
            .or_else(|| base.as_ref().map(|item| item.status.clone()))
            .unwrap_or_else(|| "draft".to_string()),
        version: task
            .version
            .or_else(|| base.as_ref().map(|item| item.version + 1))
            .unwrap_or(1),
        usage_count: task
            .usage_count
            .or_else(|| base.as_ref().map(|item| item.usage_count))
            .unwrap_or(0),
        last_run_at: task
            .last_run_at
            .or_else(|| base.as_ref().and_then(|item| item.last_run_at.clone())),
        is_favorite: task
            .is_favorite
            .or_else(|| base.as_ref().map(|item| item.is_favorite))
            .unwrap_or(false),
        tags: task
            .tags
            .or_else(|| base.as_ref().map(|item| item.tags.clone()))
            .unwrap_or_default(),
    }
}

fn validate_task(task: &BatchTaskDraft) -> ValidateTaskResult {
    let mut errors = Vec::new();
    let mut warnings = Vec::new();
    let task_type = task
        .task_type
        .clone()
        .unwrap_or_else(|| "command".to_string());
    let name = task.name.as_deref().unwrap_or("").trim();
    let content = task.content.as_deref().unwrap_or("").trim();

    if name.is_empty() {
        errors.push("Task name is required".to_string());
    }

    let needs_content = matches!(task_type.as_str(), "command" | "script" | "watch_processes");

    if needs_content && content.is_empty() {
        errors.push("Task content is required".to_string());
    }

    if task_type == "watch_processes" && !content.contains('\n') && !content.contains(',') {
        warnings.push("Watch-processes content usually contains multiple process names separated by commas or new lines".to_string());
    }

    ValidateTaskResult {
        valid: errors.is_empty(),
        errors,
        warnings,
    }
}

fn resolve_targets_from_snapshot(
    stations: &[Station],
    selector: &TargetSelector,
    filters: Option<&BatchTargetFilters>,
) -> Vec<Station> {
    let selector_type = selector.selector_type.as_str();
    let mut resolved = stations
        .iter()
        .filter(|station| match selector_type {
            "all" | "" => true,
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
                    station.name.contains(expr)
                        || station.id.contains(expr)
                        || station.groups.iter().any(|group| group.contains(expr))
                }
                _ => false,
            },
            _ => false,
        })
        .cloned()
        .collect::<Vec<_>>();

    if let Some(filters) = filters {
        resolved.retain(|station| {
            let matches_groups = filters
                .groups
                .as_ref()
                .map(|groups| {
                    groups.is_empty()
                        || groups.iter().any(|group| {
                            station
                                .groups
                                .iter()
                                .any(|station_group| station_group == group)
                        })
                })
                .unwrap_or(true);
            let matches_tags = filters
                .tags
                .as_ref()
                .map(|tags| {
                    tags.is_empty() || tags.iter().any(|tag| station.tags.contains_key(tag))
                })
                .unwrap_or(true);
            let matches_status = filters
                .status
                .as_ref()
                .map(|statuses| {
                    statuses.is_empty()
                        || statuses.iter().any(|status| {
                            (status == "offline" && station.blocked)
                                || (status == "online" && !station.blocked)
                                || status == "unknown"
                        })
                })
                .unwrap_or(true);
            matches_groups && matches_tags && matches_status
        });
    }

    resolved
}

fn to_batch_target(station: &Station) -> BatchTarget {
    BatchTarget {
        id: station.id.clone(),
        name: station.name.clone(),
        group: station.groups.first().cloned(),
        tags: Some(station.tags.clone()),
        status: if station.blocked { "offline" } else { "online" }.to_string(),
        last_seen: station.metadata.get("mqttLastSeen").cloned(),
    }
}

fn render_content(
    content: &str,
    parameters: &HashMap<String, String>,
    defaults: &[BatchTaskParameter],
) -> String {
    let mut rendered = content.to_string();
    let mut merged = HashMap::new();
    for item in defaults {
        merged.insert(item.name.clone(), item.default_value.clone());
    }
    for (key, value) in parameters {
        merged.insert(key.clone(), value.clone());
    }
    for (key, value) in merged {
        rendered = rendered.replace(&format!("{{{{{key}}}}}"), &value);
        rendered = rendered.replace(&format!("${{{key}}}"), &value);
    }
    rendered
}

async fn execute_one_target(
    task: &BatchTask,
    station: Station,
    parameters: HashMap<String, String>,
) -> BatchTargetResult {
    let started_at = now_iso();
    let start_time = std::time::Instant::now();
    let rendered = render_content(&task.content, &parameters, &task.parameters);
    let timeout_secs = task.execution_policy.timeout_secs.max(10) as u64;
    let retries = task.execution_policy.retry_count;
    let retry_delay = task.execution_policy.retry_delay_secs.unwrap_or(5) as u64;
    let mut last_result = BatchTargetResult {
        target_id: station.id.clone(),
        target_name: station.name.clone(),
        status: "failed".to_string(),
        started_at: started_at.clone(),
        completed_at: Some(now_iso()),
        duration_ms: Some(0),
        output: None,
        error: Some("Task did not start".to_string()),
        exit_code: None,
        retry_attempt: Some(0),
    };

    for attempt in 0..=retries {
        let future = async {
            match task.task_type.as_str() {
                "power_on" => {
                    let mut stations = vec![station.clone()];
                    let result = execute_station_action(
                        StationAction::PowerOn,
                        vec![station.id.clone()],
                        &mut stations,
                    )
                    .await?;
                    Ok((Some(result.message), None, Some(0), "success".to_string()))
                }
                "shutdown" => {
                    let mut stations = vec![station.clone()];
                    let result = execute_station_action(
                        StationAction::Shutdown,
                        vec![station.id.clone()],
                        &mut stations,
                    )
                    .await?;
                    Ok((Some(result.message), None, Some(0), "success".to_string()))
                }
                "reboot" => {
                    let mut stations = vec![station.clone()];
                    let result = execute_station_action(
                        StationAction::Reboot,
                        vec![station.id.clone()],
                        &mut stations,
                    )
                    .await?;
                    Ok((Some(result.message), None, Some(0), "success".to_string()))
                }
                "start_app" => {
                    let mut stations = vec![station.clone()];
                    let result = execute_station_action(
                        StationAction::StartApp,
                        vec![station.id.clone()],
                        &mut stations,
                    )
                    .await?;
                    Ok((Some(result.message), None, Some(0), "success".to_string()))
                }
                "watch_processes" => {
                    let process_names = rendered
                        .split(['\n', ','])
                        .map(str::trim)
                        .filter(|value| !value.is_empty())
                        .map(ToOwned::to_owned)
                        .collect::<Vec<_>>();
                    if process_names.is_empty() {
                        return Err("No process names were provided.".to_string());
                    }
                    let message = set_station_watching_apps(&station, process_names).await?;
                    Ok((Some(message), None, Some(0), "success".to_string()))
                }
                "command" | "script" => {
                    let command_result =
                        execute_station_command(&station, &rendered, timeout_secs as i32).await?;
                    let status = if command_result.exit_code == 0 {
                        "success"
                    } else {
                        "failed"
                    }
                    .to_string();
                    let error = if command_result.exit_code == 0
                        || command_result.stderr.trim().is_empty()
                    {
                        None
                    } else {
                        Some(command_result.stderr.clone())
                    };
                    Ok((
                        Some(command_result.stdout),
                        error,
                        Some(command_result.exit_code),
                        status,
                    ))
                }
                other => Err(format!("Unsupported batch task type: {other}")),
            }
        };

        let outcome = timeout(Duration::from_secs(timeout_secs), future).await;
        match outcome {
            Ok(Ok((output, error, exit_code, status))) => {
                last_result = BatchTargetResult {
                    target_id: station.id.clone(),
                    target_name: station.name.clone(),
                    status,
                    started_at: started_at.clone(),
                    completed_at: Some(now_iso()),
                    duration_ms: Some(start_time.elapsed().as_millis() as u64),
                    output,
                    error,
                    exit_code,
                    retry_attempt: Some(attempt),
                };
                if last_result.status == "success" {
                    return last_result;
                }
            }
            Ok(Err(error)) => {
                last_result = BatchTargetResult {
                    target_id: station.id.clone(),
                    target_name: station.name.clone(),
                    status: "failed".to_string(),
                    started_at: started_at.clone(),
                    completed_at: Some(now_iso()),
                    duration_ms: Some(start_time.elapsed().as_millis() as u64),
                    output: None,
                    error: Some(error),
                    exit_code: None,
                    retry_attempt: Some(attempt),
                };
            }
            Err(_) => {
                last_result = BatchTargetResult {
                    target_id: station.id.clone(),
                    target_name: station.name.clone(),
                    status: "timeout".to_string(),
                    started_at: started_at.clone(),
                    completed_at: Some(now_iso()),
                    duration_ms: Some(start_time.elapsed().as_millis() as u64),
                    output: None,
                    error: Some(format!("Task timed out after {timeout_secs}s")),
                    exit_code: None,
                    retry_attempt: Some(attempt),
                };
            }
        }

        if attempt < retries {
            sleep(Duration::from_secs(retry_delay)).await;
        }
    }

    last_result
}

async fn execute_chunk(
    task: &BatchTask,
    stations: Vec<Station>,
    parameters: HashMap<String, String>,
    limit: usize,
) -> Vec<BatchTargetResult> {
    let concurrency = limit.max(1);
    stream::iter(stations.into_iter().map(|station| {
        let task = task.clone();
        let parameters = parameters.clone();
        async move { execute_one_target(&task, station, parameters).await }
    }))
    .buffer_unordered(concurrency)
    .collect::<Vec<_>>()
    .await
}

fn failure_rate(results: &[BatchTargetResult]) -> f64 {
    if results.is_empty() {
        return 0.0;
    }
    let failed = results
        .iter()
        .filter(|result| matches!(result.status.as_str(), "failed" | "timeout"))
        .count();
    failed as f64 / results.len() as f64 * 100.0
}

async fn execute_batch_task_internal(
    task: &BatchTask,
    stations: Vec<Station>,
    parameters: HashMap<String, String>,
) -> BatchExecutionResult {
    let execution_id = Uuid::new_v4().to_string();
    let started_at = now_iso();
    let total_targets = stations.len();
    let batch_size = task.execution_policy.batch_size.unwrap_or(5).max(1) as usize;
    let mode = task.execution_policy.mode.as_str();
    let threshold = task.execution_policy.failure_threshold_percent as f64;
    let mut results = Vec::new();
    let mut remaining = stations;
    let mut circuit_breaker_triggered = false;

    let schedule = match mode {
        "batch" => vec![batch_size],
        "rolling" => vec![1, batch_size, usize::MAX],
        _ => vec![usize::MAX],
    };

    for step in schedule {
        while !remaining.is_empty() {
            let chunk_size = if step == usize::MAX {
                remaining.len()
            } else {
                remaining.len().min(step)
            };
            let chunk = remaining.drain(..chunk_size).collect::<Vec<_>>();
            let limit = if mode == "parallel" {
                chunk.len().max(1)
            } else {
                chunk_size.max(1)
            };
            let mut chunk_results = execute_chunk(task, chunk, parameters.clone(), limit).await;
            results.append(&mut chunk_results);

            let current_failure_rate = failure_rate(&results);
            if current_failure_rate > threshold {
                circuit_breaker_triggered = true;
                if !task.execution_policy.continue_on_failure {
                    for station in remaining.drain(..) {
                        results.push(BatchTargetResult {
                            target_id: station.id,
                            target_name: station.name,
                            status: "skipped".to_string(),
                            started_at: now_iso(),
                            completed_at: Some(now_iso()),
                            duration_ms: Some(0),
                            output: None,
                            error: Some(
                                "Skipped after circuit breaker threshold was exceeded".to_string(),
                            ),
                            exit_code: None,
                            retry_attempt: Some(0),
                        });
                    }
                    break;
                }
            }

            if mode != "batch" {
                break;
            }
        }

        if circuit_breaker_triggered && !task.execution_policy.continue_on_failure {
            break;
        }
        if mode != "rolling" || remaining.is_empty() {
            break;
        }
    }

    let success_count = results
        .iter()
        .filter(|result| result.status == "success")
        .count();
    let failure_count = results
        .iter()
        .filter(|result| matches!(result.status.as_str(), "failed" | "timeout"))
        .count();
    let skipped_count = results
        .iter()
        .filter(|result| result.status == "skipped")
        .count();
    let status = if failure_count == 0 {
        "completed".to_string()
    } else if success_count == 0 {
        "failed".to_string()
    } else {
        "partial_failure".to_string()
    };

    BatchExecutionResult {
        task_id: task.id.clone(),
        execution_id,
        status,
        started_at,
        completed_at: Some(now_iso()),
        total_targets,
        success_count,
        failure_count,
        skipped_count,
        results,
        circuit_breaker_triggered: Some(circuit_breaker_triggered),
        failure_rate: Some(failure_rate(&[])),
    }
}

#[command]
pub fn load_batch_tasks() -> Result<LoadBatchTasksResult, String> {
    let mut tasks = load_tasks()?;
    tasks.sort_by(|left, right| right.updated_at.cmp(&left.updated_at));
    Ok(LoadBatchTasksResult { tasks })
}

#[command]
pub fn save_batch_task(task: BatchTaskDraft) -> Result<SaveBatchTaskResult, String> {
    let validation = validate_task(&task);
    if !validation.valid {
        return Err(validation.errors.join("; "));
    }

    let mut tasks = load_tasks()?;
    let existing_index = task
        .id
        .as_ref()
        .and_then(|id| tasks.iter().position(|existing| &existing.id == id));
    let existing = existing_index.and_then(|index| tasks.get(index).cloned());
    let normalized = normalize_task(task, existing.as_ref());

    if let Some(index) = existing_index {
        tasks[index] = normalized.clone();
    } else {
        tasks.push(normalized.clone());
    }
    save_tasks(&tasks)?;
    Ok(SaveBatchTaskResult { task: normalized })
}

#[command]
pub fn delete_batch_task(task_id: String) -> Result<HashMap<&'static str, bool>, String> {
    let mut tasks = load_tasks()?;
    let before = tasks.len();
    tasks.retain(|task| task.id != task_id);
    save_tasks(&tasks)?;
    Ok(HashMap::from([("success", before != tasks.len())]))
}

#[command]
pub fn duplicate_batch_task(task_id: String) -> Result<SaveBatchTaskResult, String> {
    let tasks = load_tasks()?;
    let original = tasks
        .iter()
        .find(|task| task.id == task_id)
        .cloned()
        .ok_or_else(|| format!("No batch task found for id {task_id}"))?;
    let copy = BatchTask {
        id: Uuid::new_v4().to_string(),
        name: format!("{} (Copy)", original.name),
        is_favorite: false,
        created_at: now_iso(),
        updated_at: now_iso(),
        usage_count: 0,
        last_run_at: None,
        ..original
    };
    let mut next = tasks;
    next.push(copy.clone());
    save_tasks(&next)?;
    Ok(SaveBatchTaskResult { task: copy })
}

#[command]
pub fn toggle_batch_task_favorite(task_id: String) -> Result<(), String> {
    let mut tasks = load_tasks()?;
    let task = tasks
        .iter_mut()
        .find(|task| task.id == task_id)
        .ok_or_else(|| format!("No batch task found for id {task_id}"))?;
    task.is_favorite = !task.is_favorite;
    task.updated_at = now_iso();
    save_tasks(&tasks)
}

#[command]
pub fn validate_batch_task(task: BatchTaskDraft) -> Result<ValidateTaskResult, String> {
    Ok(validate_task(&task))
}

#[command]
pub fn get_batch_targets(filters: Option<BatchTargetFilters>) -> Result<GetTargetsResult, String> {
    let snapshot = StateStore::load_snapshot().map_err(|error| error.to_string())?;
    let selector = TargetSelector {
        selector_type: "all".to_string(),
        groups: None,
        tags: None,
        device_ids: None,
        filter_expr: None,
    };
    let targets = resolve_targets_from_snapshot(&snapshot.stations, &selector, filters.as_ref())
        .into_iter()
        .map(|station| to_batch_target(&station))
        .collect::<Vec<_>>();
    Ok(GetTargetsResult {
        total: targets.len(),
        targets,
    })
}

#[command]
pub fn preview_batch_targets(selector: TargetSelector) -> Result<GetTargetsResult, String> {
    let snapshot = StateStore::load_snapshot().map_err(|error| error.to_string())?;
    let targets = resolve_targets_from_snapshot(&snapshot.stations, &selector, None)
        .into_iter()
        .map(|station| to_batch_target(&station))
        .collect::<Vec<_>>();
    Ok(GetTargetsResult {
        total: targets.len(),
        targets,
    })
}

#[command]
pub async fn execute_batch_task(
    task_id: String,
    targets: Vec<String>,
    parameters: Option<HashMap<String, String>>,
) -> Result<ExecuteBatchTaskResult, String> {
    let mut tasks = load_tasks()?;
    let index = tasks
        .iter()
        .position(|task| task.id == task_id)
        .ok_or_else(|| format!("No batch task found for id {task_id}"))?;
    let task = tasks[index].clone();

    let snapshot = StateStore::load_snapshot().map_err(|error| error.to_string())?;
    let selected_stations = if targets.is_empty() {
        resolve_targets_from_snapshot(&snapshot.stations, &task.target_selector, None)
    } else {
        snapshot
            .stations
            .into_iter()
            .filter(|station| targets.iter().any(|id| id == &station.id))
            .collect::<Vec<_>>()
    };

    let mut result =
        execute_batch_task_internal(&task, selected_stations, parameters.unwrap_or_default()).await;
    result.failure_rate = Some(failure_rate(&result.results));

    tasks[index].status = result.status.clone();
    tasks[index].updated_at = now_iso();
    tasks[index].last_run_at = result.completed_at.clone();
    tasks[index].usage_count += 1;
    save_tasks(&tasks)?;

    let mut history = load_history()?;
    history.push(result.clone());
    if history.len() > 200 {
        let drain_count = history.len() - 200;
        history.drain(..drain_count);
    }
    save_history(&history)?;

    Ok(ExecuteBatchTaskResult { execution: result })
}

#[command]
pub async fn execute_batch(
    task_type: String,
    content: String,
    target_selector: TargetSelector,
    execution_policy: ExecutionPolicy,
    parameters: Option<HashMap<String, String>>,
) -> Result<ExecuteBatchTaskResult, String> {
    let task = BatchTask {
        id: Uuid::new_v4().to_string(),
        name: format!("Ad hoc {}", task_type.replace('_', " ")),
        description: Some("Ad hoc batch task".to_string()),
        task_type,
        target_selector,
        content,
        parameters: Vec::new(),
        execution_policy,
        created_by: "current-user".to_string(),
        created_at: now_iso(),
        updated_at: now_iso(),
        status: "draft".to_string(),
        version: 1,
        usage_count: 0,
        last_run_at: None,
        is_favorite: false,
        tags: Vec::new(),
    };
    let snapshot = StateStore::load_snapshot().map_err(|error| error.to_string())?;
    let stations = resolve_targets_from_snapshot(&snapshot.stations, &task.target_selector, None);
    let mut result =
        execute_batch_task_internal(&task, stations, parameters.unwrap_or_default()).await;
    result.failure_rate = Some(failure_rate(&result.results));
    Ok(ExecuteBatchTaskResult { execution: result })
}

#[command]
pub fn get_batch_execution_status(
    execution_id: String,
) -> Result<BatchExecutionStatusResponse, String> {
    let history = load_history()?;
    let execution = history
        .iter()
        .find(|item| item.execution_id == execution_id)
        .ok_or_else(|| format!("No execution found for id {execution_id}"))?;
    Ok(BatchExecutionStatusResponse {
        execution_id,
        status: execution.status.clone(),
        progress: BatchExecutionProgress {
            total: execution.total_targets,
            completed: execution.success_count + execution.failure_count + execution.skipped_count,
            failed: execution.failure_count,
            running: 0,
        },
    })
}

#[command]
pub fn get_batch_history(limit: Option<usize>) -> Result<GetBatchHistoryResult, String> {
    let mut history = load_history()?;
    history.sort_by(|left, right| right.started_at.cmp(&left.started_at));
    if let Some(limit) = limit {
        history.truncate(limit);
    }
    Ok(GetBatchHistoryResult { history })
}

#[command]
pub fn cancel_batch_execution(_execution_id: String) -> Result<(), String> {
    Ok(())
}

#[command]
pub fn pause_batch_execution(_execution_id: String) -> Result<(), String> {
    Ok(())
}

#[command]
pub fn resume_batch_execution(_execution_id: String) -> Result<(), String> {
    Ok(())
}

#[command]
pub fn import_batch_package(
    pkg: BatchTaskPackage,
    options: Option<HashMap<String, bool>>,
) -> Result<BatchTaskImportResult, String> {
    let overwrite = options
        .as_ref()
        .and_then(|values| values.get("overwrite").copied())
        .unwrap_or(false);
    let import_as_copies = options
        .as_ref()
        .and_then(|values| values.get("importAsCopies").copied())
        .unwrap_or(false);

    let mut existing = load_tasks()?;
    let mut imported = 0usize;
    let mut skipped = 0usize;
    let mut errors = Vec::new();

    for mut task in pkg.tasks {
        if import_as_copies || existing.iter().any(|current| current.id == task.id) && !overwrite {
            task.id = Uuid::new_v4().to_string();
            task.name = format!("{} (Imported)", task.name);
        }

        if let Some(index) = existing.iter().position(|current| current.id == task.id) {
            if overwrite {
                existing[index] = task;
                imported += 1;
            } else {
                skipped += 1;
            }
        } else {
            existing.push(task);
            imported += 1;
        }
    }

    if let Err(error) = save_tasks(&existing) {
        errors.push(error);
    }

    Ok(BatchTaskImportResult {
        success: errors.is_empty(),
        imported,
        skipped,
        errors,
        warnings: Vec::new(),
    })
}

#[command]
pub fn export_batch_package(
    task_ids: Vec<String>,
    metadata: Option<BatchTaskPackageMetadata>,
) -> Result<HashMap<&'static str, BatchTaskPackage>, String> {
    let tasks = load_tasks()?
        .into_iter()
        .filter(|task| task_ids.iter().any(|id| id == &task.id))
        .collect::<Vec<_>>();
    let now = now_iso();
    let package = BatchTaskPackage {
        metadata: metadata.unwrap_or(BatchTaskPackageMetadata {
            id: Uuid::new_v4().to_string(),
            name: "Exported Batch Tasks".to_string(),
            version: "1.0.0".to_string(),
            description: Some("Exported from CC-rClient".to_string()),
            author: Some("CC-rClient".to_string()),
            tags: Vec::new(),
            created_at: now.clone(),
            updated_at: now,
        }),
        tasks,
        signature: None,
    };
    Ok(HashMap::from([("package", package)]))
}
