use std::collections::HashMap;
use std::fs;
use std::path::PathBuf;

use chrono::Utc;
use serde::{Deserialize, Serialize};
use tauri::command;
use uuid::Uuid;

use crate::remote::execute_station_command;
use crate::storage::StateStore;

const DEFAULT_SCRIPT_TIMEOUT_SECS: i32 = 60;
const MAX_SCRIPT_HISTORY: usize = 500;

#[derive(Debug, Clone, Serialize, Deserialize, Default)]
#[serde(rename_all = "camelCase")]
pub struct ScriptParameter {
    pub name: String,
    pub param_type: String,
    pub default_value: String,
    pub required: bool,
    pub validation: Option<String>,
    pub description: Option<String>,
    pub options: Option<Vec<String>>,
}

#[derive(Debug, Clone, Serialize, Deserialize)]
#[serde(rename_all = "camelCase")]
pub struct CommandScript {
    pub id: String,
    pub name: String,
    pub description: String,
    pub script_type: String,
    pub content: String,
    #[serde(default)]
    pub parameters: Vec<ScriptParameter>,
    pub is_template: bool,
    pub is_favorite: bool,
    #[serde(default)]
    pub tags: Vec<String>,
    pub created_by: String,
    pub created_at: String,
    pub updated_at: String,
    pub version: u32,
    pub usage_count: u64,
    pub last_used_at: Option<String>,
}

#[derive(Debug, Clone, Serialize, Deserialize, Default)]
#[serde(rename_all = "camelCase")]
pub struct CommandScriptDraft {
    pub id: Option<String>,
    pub name: Option<String>,
    pub description: Option<String>,
    pub script_type: Option<String>,
    pub content: Option<String>,
    pub parameters: Option<Vec<ScriptParameter>>,
    pub is_template: Option<bool>,
    pub is_favorite: Option<bool>,
    pub tags: Option<Vec<String>>,
    pub created_by: Option<String>,
    pub created_at: Option<String>,
    pub updated_at: Option<String>,
    pub version: Option<u32>,
    pub usage_count: Option<u64>,
    pub last_used_at: Option<String>,
}

#[derive(Debug, Clone, Serialize)]
#[serde(rename_all = "camelCase")]
pub struct ScriptValidationResult {
    pub valid: bool,
    pub errors: Vec<String>,
    pub warnings: Vec<String>,
    pub parameter_errors: HashMap<String, Vec<String>>,
}

#[derive(Debug, Clone, Serialize, Deserialize)]
#[serde(rename_all = "camelCase")]
pub struct ScriptExecutionContext {
    pub script_id: String,
    pub station_ids: Vec<String>,
    pub parameters: Option<HashMap<String, String>>,
    pub timeout: Option<u64>,
    pub continue_on_error: Option<bool>,
}

#[derive(Debug, Clone, Serialize, Deserialize)]
#[serde(rename_all = "camelCase")]
pub struct ScriptExecutionResult {
    pub script_id: String,
    pub status: String,
    pub stdout: String,
    pub stderr: String,
    pub exit_code: Option<i32>,
    pub started_at: String,
    pub completed_at: Option<String>,
    pub duration_ms: Option<u64>,
    pub error: Option<String>,
}

#[derive(Debug, Clone, Serialize)]
#[serde(rename_all = "camelCase")]
pub struct GetScriptHistoryResult {
    pub history: Vec<ScriptExecutionResult>,
}

#[derive(Debug, Clone, Serialize)]
#[serde(rename_all = "camelCase")]
pub struct ScriptTargetInfo {
    pub id: String,
    pub name: String,
    pub status: Option<String>,
}

#[derive(Debug, Clone, Serialize)]
#[serde(rename_all = "camelCase")]
pub struct GetAvailableTargetsResult {
    pub targets: Vec<ScriptTargetInfo>,
}

#[derive(Debug, Clone, Serialize, Deserialize)]
#[serde(rename_all = "camelCase")]
pub struct ScriptPackageMetadata {
    pub id: String,
    pub name: String,
    pub version: String,
    pub description: Option<String>,
    pub author: Option<String>,
    #[serde(default)]
    pub tags: Vec<String>,
    pub created_at: String,
    pub updated_at: String,
    #[serde(default)]
    pub categories: Vec<String>,
    pub license: Option<String>,
}

#[derive(Debug, Clone, Serialize, Deserialize, Default)]
#[serde(rename_all = "camelCase")]
pub struct ScriptPackageMetadataDraft {
    pub id: Option<String>,
    pub name: Option<String>,
    pub version: Option<String>,
    pub description: Option<String>,
    pub author: Option<String>,
    pub tags: Option<Vec<String>>,
    pub created_at: Option<String>,
    pub updated_at: Option<String>,
    pub categories: Option<Vec<String>>,
    pub license: Option<String>,
}

#[derive(Debug, Clone, Serialize, Deserialize)]
#[serde(rename_all = "camelCase")]
pub struct ScriptPackage {
    pub metadata: ScriptPackageMetadata,
    pub scripts: Vec<CommandScript>,
    pub signature: Option<String>,
}

#[derive(Debug, Clone, Serialize)]
#[serde(rename_all = "camelCase")]
pub struct ScriptImportResult {
    pub success: bool,
    pub imported: usize,
    pub skipped: usize,
    pub errors: Vec<String>,
    pub warnings: Vec<String>,
}

fn cc_client_dir() -> Result<PathBuf, String> {
    let home =
        dirs::home_dir().ok_or_else(|| "Unable to resolve the user home directory.".to_string())?;
    let path = home.join(".CC-rController");
    fs::create_dir_all(&path).map_err(|error| format!("create {}: {error}", path.display()))?;
    Ok(path)
}

fn scripts_path() -> Result<PathBuf, String> {
    Ok(cc_client_dir()?.join("scripts.json"))
}

fn script_history_path() -> Result<PathBuf, String> {
    Ok(cc_client_dir()?.join("script_history.json"))
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

fn load_scripts_store() -> Result<Vec<CommandScript>, String> {
    read_json_vec(&scripts_path()?)
}

fn save_scripts_store(scripts: &[CommandScript]) -> Result<(), String> {
    write_json_vec(&scripts_path()?, scripts)
}

fn load_script_history_store() -> Result<Vec<ScriptExecutionResult>, String> {
    read_json_vec(&script_history_path()?)
}

fn save_script_history_store(history: &[ScriptExecutionResult]) -> Result<(), String> {
    write_json_vec(&script_history_path()?, history)
}

fn now_iso() -> String {
    Utc::now().to_rfc3339()
}

fn supported_script_type(script_type: &str) -> bool {
    matches!(
        script_type,
        "shell" | "powershell" | "python" | "lua" | "javascript"
    )
}

fn validate_script_draft(script: &CommandScriptDraft) -> ScriptValidationResult {
    let mut errors = Vec::new();
    let mut warnings = Vec::new();
    let mut parameter_errors = HashMap::new();

    let name = script.name.as_deref().unwrap_or("").trim();
    let script_type = script.script_type.as_deref().unwrap_or("").trim();
    let content = script.content.as_deref().unwrap_or("").trim();
    let parameters = script.parameters.clone().unwrap_or_default();

    if name.is_empty() {
        errors.push("Script name is required".to_string());
    }

    if script_type.is_empty() {
        errors.push("Script type is required".to_string());
    } else if !supported_script_type(script_type) {
        errors.push(format!("Unsupported script type: {script_type}"));
    }

    if content.is_empty() {
        errors.push("Script content is required".to_string());
    }

    let mut seen = std::collections::HashSet::new();
    for (index, parameter) in parameters.iter().enumerate() {
        let key = if parameter.name.trim().is_empty() {
            format!("param_{index}")
        } else {
            parameter.name.clone()
        };
        let mut current = Vec::new();

        if parameter.name.trim().is_empty() {
            current.push("Parameter name is required".to_string());
            errors.push(format!("Parameter {}: name is required", index + 1));
        } else if !seen.insert(parameter.name.clone()) {
            current.push("Duplicate parameter name".to_string());
            errors.push(format!("Parameter \"{}\": duplicate name", parameter.name));
        }

        if !current.is_empty() {
            parameter_errors.insert(key, current);
        }
    }

    if parameters.is_empty() {
        warnings.push("Script has no parameters defined".to_string());
    }

    if script
        .description
        .as_deref()
        .unwrap_or("")
        .trim()
        .is_empty()
    {
        warnings.push("Script description is empty".to_string());
    }

    ScriptValidationResult {
        valid: errors.is_empty(),
        errors,
        warnings,
        parameter_errors,
    }
}

fn normalize_script(script: CommandScriptDraft, existing: Option<&CommandScript>) -> CommandScript {
    let now = now_iso();
    let base = existing.cloned();

    CommandScript {
        id: script
            .id
            .or_else(|| base.as_ref().map(|item| item.id.clone()))
            .unwrap_or_else(|| Uuid::new_v4().to_string()),
        name: script
            .name
            .or_else(|| base.as_ref().map(|item| item.name.clone()))
            .unwrap_or_default()
            .trim()
            .to_string(),
        description: script
            .description
            .or_else(|| base.as_ref().map(|item| item.description.clone()))
            .unwrap_or_default()
            .trim()
            .to_string(),
        script_type: script
            .script_type
            .or_else(|| base.as_ref().map(|item| item.script_type.clone()))
            .unwrap_or_else(|| "shell".to_string()),
        content: script
            .content
            .or_else(|| base.as_ref().map(|item| item.content.clone()))
            .unwrap_or_default(),
        parameters: script
            .parameters
            .or_else(|| base.as_ref().map(|item| item.parameters.clone()))
            .unwrap_or_default(),
        is_template: script
            .is_template
            .or_else(|| base.as_ref().map(|item| item.is_template))
            .unwrap_or(false),
        is_favorite: script
            .is_favorite
            .or_else(|| base.as_ref().map(|item| item.is_favorite))
            .unwrap_or(false),
        tags: script
            .tags
            .or_else(|| base.as_ref().map(|item| item.tags.clone()))
            .unwrap_or_default(),
        created_by: script
            .created_by
            .or_else(|| base.as_ref().map(|item| item.created_by.clone()))
            .unwrap_or_else(|| "current-user".to_string()),
        created_at: script
            .created_at
            .or_else(|| base.as_ref().map(|item| item.created_at.clone()))
            .unwrap_or_else(|| now.clone()),
        updated_at: script.updated_at.unwrap_or(now),
        version: script
            .version
            .or_else(|| base.as_ref().map(|item| item.version + 1))
            .unwrap_or(1),
        usage_count: script
            .usage_count
            .or_else(|| base.as_ref().map(|item| item.usage_count))
            .unwrap_or(0),
        last_used_at: script
            .last_used_at
            .or_else(|| base.as_ref().and_then(|item| item.last_used_at.clone())),
    }
}

fn render_script_content(
    content: &str,
    parameters: &HashMap<String, String>,
    defaults: &[ScriptParameter],
) -> String {
    let mut rendered = content.to_string();
    let mut merged = HashMap::new();

    for parameter in defaults {
        merged.insert(parameter.name.clone(), parameter.default_value.clone());
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

fn timeout_secs_from_millis(timeout_ms: Option<u64>) -> i32 {
    let millis = timeout_ms.unwrap_or((DEFAULT_SCRIPT_TIMEOUT_SECS as u64) * 1000);
    let secs = ((millis + 999) / 1000).max(1);
    secs.min(i32::MAX as u64) as i32
}

fn append_history(results: &[ScriptExecutionResult]) -> Result<(), String> {
    let mut history = load_script_history_store()?;
    history.extend_from_slice(results);
    if history.len() > MAX_SCRIPT_HISTORY {
        let drain_count = history.len() - MAX_SCRIPT_HISTORY;
        history.drain(..drain_count);
    }
    save_script_history_store(&history)
}

fn build_export_metadata(
    metadata: Option<ScriptPackageMetadataDraft>,
    scripts: &[CommandScript],
) -> ScriptPackageMetadata {
    let now = now_iso();
    let draft = metadata.unwrap_or_default();
    let default_name = if scripts.len() == 1 {
        format!("{} Export", scripts[0].name)
    } else {
        "Script Export".to_string()
    };

    ScriptPackageMetadata {
        id: draft.id.unwrap_or_else(|| Uuid::new_v4().to_string()),
        name: draft.name.unwrap_or(default_name),
        version: draft.version.unwrap_or_else(|| "1.0.0".to_string()),
        description: draft.description,
        author: draft.author,
        tags: draft.tags.unwrap_or_default(),
        created_at: draft.created_at.unwrap_or_else(|| now.clone()),
        updated_at: draft.updated_at.unwrap_or(now),
        categories: draft.categories.unwrap_or_default(),
        license: draft.license,
    }
}

#[command]
pub fn load_scripts() -> Result<Vec<CommandScript>, String> {
    let mut scripts = load_scripts_store()?;
    scripts.sort_by(|left, right| right.updated_at.cmp(&left.updated_at));
    Ok(scripts)
}

#[command]
pub fn save_script(script: CommandScriptDraft) -> Result<CommandScript, String> {
    let validation = validate_script_draft(&script);
    if !validation.valid {
        return Err(validation.errors.join("; "));
    }

    let mut scripts = load_scripts_store()?;
    let existing_index = script
        .id
        .as_ref()
        .and_then(|id| scripts.iter().position(|existing| &existing.id == id));
    let existing = existing_index.and_then(|index| scripts.get(index).cloned());
    let normalized = normalize_script(script, existing.as_ref());

    if let Some(index) = existing_index {
        scripts[index] = normalized.clone();
    } else {
        scripts.push(normalized.clone());
    }

    save_scripts_store(&scripts)?;
    Ok(normalized)
}

#[command]
pub fn delete_script(script_id: String) -> Result<HashMap<&'static str, bool>, String> {
    let mut scripts = load_scripts_store()?;
    let before = scripts.len();
    scripts.retain(|script| script.id != script_id);
    save_scripts_store(&scripts)?;
    Ok(HashMap::from([("success", before != scripts.len())]))
}

#[command]
pub fn validate_script(script: CommandScriptDraft) -> Result<ScriptValidationResult, String> {
    Ok(validate_script_draft(&script))
}

#[command]
pub fn get_available_targets() -> Result<GetAvailableTargetsResult, String> {
    let snapshot = StateStore::load_snapshot().map_err(|error| error.to_string())?;
    let targets = snapshot
        .stations
        .into_iter()
        .map(|station| ScriptTargetInfo {
            id: station.id,
            name: station.name,
            status: Some(if station.blocked { "offline" } else { "online" }.to_string()),
        })
        .collect::<Vec<_>>();
    Ok(GetAvailableTargetsResult { targets })
}

#[command]
pub async fn execute_script(
    context: ScriptExecutionContext,
) -> Result<Vec<ScriptExecutionResult>, String> {
    let mut scripts = load_scripts_store()?;
    let script_index = scripts
        .iter()
        .position(|script| script.id == context.script_id)
        .ok_or_else(|| format!("No script found for id {}", context.script_id))?;
    let script = scripts[script_index].clone();

    let snapshot = StateStore::load_snapshot().map_err(|error| error.to_string())?;
    let targets = snapshot
        .stations
        .into_iter()
        .filter(|station| context.station_ids.iter().any(|id| id == &station.id))
        .collect::<Vec<_>>();

    if targets.is_empty() {
        return Err("No execution targets were provided.".to_string());
    }

    let parameters = context.parameters.unwrap_or_default();
    let continue_on_error = context.continue_on_error.unwrap_or(true);
    let timeout_secs = timeout_secs_from_millis(context.timeout);
    let mut results = Vec::with_capacity(targets.len());

    for station in targets {
        let started_at = now_iso();
        let start = std::time::Instant::now();
        let content = render_script_content(&script.content, &parameters, &script.parameters);

        let result = match execute_station_command(&station, &content, timeout_secs).await {
            Ok(command_result) => ScriptExecutionResult {
                script_id: script.id.clone(),
                status: if command_result.exit_code == 0 {
                    "success".to_string()
                } else {
                    "failed".to_string()
                },
                stdout: command_result.stdout,
                stderr: command_result.stderr.clone(),
                exit_code: Some(command_result.exit_code),
                started_at,
                completed_at: Some(now_iso()),
                duration_ms: Some(start.elapsed().as_millis() as u64),
                error: if command_result.exit_code == 0 || command_result.stderr.trim().is_empty() {
                    None
                } else {
                    Some(command_result.stderr)
                },
            },
            Err(error) => ScriptExecutionResult {
                script_id: script.id.clone(),
                status: "failed".to_string(),
                stdout: String::new(),
                stderr: error.clone(),
                exit_code: None,
                started_at,
                completed_at: Some(now_iso()),
                duration_ms: Some(start.elapsed().as_millis() as u64),
                error: Some(error),
            },
        };

        let failed = result.status == "failed";
        results.push(result);
        if failed && !continue_on_error {
            break;
        }
    }

    scripts[script_index].usage_count += 1;
    scripts[script_index].last_used_at = Some(now_iso());
    scripts[script_index].updated_at = now_iso();
    save_scripts_store(&scripts)?;
    append_history(&results)?;

    Ok(results)
}

#[command]
pub fn cancel_script_execution(_execution_id: String) -> Result<(), String> {
    Ok(())
}

#[command]
pub fn get_script_history(limit: Option<usize>) -> Result<GetScriptHistoryResult, String> {
    let mut history = load_script_history_store()?;
    history.sort_by(|left, right| right.started_at.cmp(&left.started_at));
    if let Some(limit) = limit {
        history.truncate(limit);
    }
    Ok(GetScriptHistoryResult { history })
}

#[command]
pub fn import_script_package(
    pkg: ScriptPackage,
    options: Option<HashMap<String, bool>>,
) -> Result<ScriptImportResult, String> {
    let overwrite = options
        .as_ref()
        .and_then(|values| values.get("overwrite").copied())
        .unwrap_or(false);
    let import_as_copies = options
        .as_ref()
        .and_then(|values| values.get("importAsCopies").copied())
        .unwrap_or(false);

    let mut existing = load_scripts_store()?;
    let mut imported = 0usize;
    let skipped = 0usize;
    let mut errors = Vec::new();
    let warnings = Vec::new();

    for mut script in pkg.scripts {
        if import_as_copies || existing.iter().any(|current| current.id == script.id) && !overwrite
        {
            script.id = Uuid::new_v4().to_string();
            script.name = format!("{} (Imported)", script.name);
            script.created_at = now_iso();
        }

        script.updated_at = now_iso();

        if !supported_script_type(&script.script_type) {
            errors.push(format!(
                "Skipped script \"{}\" due to unsupported type {}",
                script.name, script.script_type
            ));
            continue;
        }

        if let Some(index) = existing.iter().position(|current| current.id == script.id) {
            existing[index] = script;
        } else {
            existing.push(script);
        }
        imported += 1;
    }

    save_scripts_store(&existing)?;

    Ok(ScriptImportResult {
        success: errors.is_empty(),
        imported,
        skipped,
        errors,
        warnings,
    })
}

#[command]
pub fn export_script_package(
    scripts: Vec<CommandScript>,
    metadata: Option<ScriptPackageMetadataDraft>,
) -> Result<ScriptPackage, String> {
    if scripts.is_empty() {
        return Err("No scripts were provided for export.".to_string());
    }

    Ok(ScriptPackage {
        metadata: build_export_metadata(metadata, &scripts),
        scripts,
        signature: None,
    })
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn validate_script_requires_basic_fields() {
        let result = validate_script_draft(&CommandScriptDraft::default());

        assert!(!result.valid);
        assert!(
            result
                .errors
                .iter()
                .any(|item| item == "Script name is required")
        );
        assert!(
            result
                .errors
                .iter()
                .any(|item| item == "Script type is required")
        );
        assert!(
            result
                .errors
                .iter()
                .any(|item| item == "Script content is required")
        );
    }

    #[test]
    fn normalize_script_preserves_existing_identity_and_increments_version() {
        let existing = CommandScript {
            id: "script-1".to_string(),
            name: "Restart".to_string(),
            description: "desc".to_string(),
            script_type: "shell".to_string(),
            content: "echo hi".to_string(),
            parameters: Vec::new(),
            is_template: false,
            is_favorite: true,
            tags: vec!["ops".to_string()],
            created_by: "tester".to_string(),
            created_at: "2024-01-01T00:00:00Z".to_string(),
            updated_at: "2024-01-01T00:00:00Z".to_string(),
            version: 2,
            usage_count: 4,
            last_used_at: None,
        };

        let normalized = normalize_script(
            CommandScriptDraft {
                id: Some(existing.id.clone()),
                name: Some("Restart App".to_string()),
                description: Some("updated".to_string()),
                script_type: Some("shell".to_string()),
                content: Some("echo updated".to_string()),
                ..CommandScriptDraft::default()
            },
            Some(&existing),
        );

        assert_eq!(normalized.id, existing.id);
        assert_eq!(normalized.created_at, existing.created_at);
        assert_eq!(normalized.version, 3);
        assert_eq!(normalized.name, "Restart App");
        assert_eq!(normalized.content, "echo updated");
    }
}
