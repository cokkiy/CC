use alert_ops::{
    acknowledge_alert, append_alert_history, delete_alert_rule, duplicate_alert_rule,
    export_alert_rules, get_alert_available_targets, get_alert_history, get_alert_stats,
    get_alert_templates, import_alert_rules, load_alert_rules, preview_alert_targets,
    resolve_alert, save_alert_rule, toggle_alert_rule, validate_alert_rule,
};
use batch_ops::{
    cancel_batch_execution, delete_batch_task, duplicate_batch_task, execute_batch,
    execute_batch_task, export_batch_package, get_batch_execution_status, get_batch_history,
    get_batch_targets, import_batch_package, load_batch_tasks, pause_batch_execution,
    preview_batch_targets, resume_batch_execution, save_batch_task, toggle_batch_task_favorite,
    validate_batch_task,
};
use control::{execute_station_action, set_station_gathering_interval};
use models::{
    ActionResult, AppSnapshot, PersistedState, Station, StationAction, StationGroup, TagDefinition,
};
use remote::{
    CommandExecutionResult, RemoteFileBrowserResult, StationRuntimeSnapshot, StationScreenCapture,
    browse_station_files, capture_station_screen, download_station_file, execute_station_command,
    fetch_station_runtime, rename_station_file, upload_station_file,
};
use std::collections::HashMap;
use storage::StateStore;
use ws_bridge::MqttWsBridge;

pub mod control;
pub mod grpc;
pub mod models;
pub mod remote;
pub mod storage;
pub mod websocket;
pub mod wol;
pub mod ws_bridge;
pub mod batch_ops;
pub mod alert_ops;

fn normalize_group_color(color: String) -> String {
    let trimmed = color.trim();
    if trimmed.is_empty() {
        "#3b82f6".to_string()
    } else {
        trimmed.to_string()
    }
}

fn normalize_group_icon(icon: Option<String>) -> Option<String> {
    icon.and_then(|value| {
        let trimmed = value.trim();
        if trimmed.is_empty() {
            None
        } else {
            Some(trimmed.to_string())
        }
    })
}

fn normalize_group_station_ids(station_ids: Vec<String>) -> Vec<String> {
    let mut normalized = Vec::new();
    for station_id in station_ids {
        let trimmed = station_id.trim();
        if trimmed.is_empty() {
            continue;
        }
        let station_id = trimmed.to_string();
        if !normalized.contains(&station_id) {
            normalized.push(station_id);
        }
    }
    normalized
}

fn build_station_group(
    name: String,
    description: String,
    color: String,
    icon: Option<String>,
    station_ids: Option<Vec<String>>,
) -> StationGroup {
    let mut group = StationGroup::new(&name);
    group.description = description;
    group.color = normalize_group_color(color);
    group.icon = normalize_group_icon(icon);
    group.station_ids = normalize_group_station_ids(station_ids.unwrap_or_default());
    group
}

fn sync_station_group_membership(stations: &mut [Station], group_id: &str, station_ids: &[String]) {
    for station in stations {
        let is_in_group = station_ids
            .iter()
            .any(|station_id| station_id == &station.id);
        let has_group = station
            .groups
            .iter()
            .any(|existing_group_id| existing_group_id == group_id);

        if is_in_group && !has_group {
            station.groups.push(group_id.to_string());
        }

        if !is_in_group && has_group {
            station
                .groups
                .retain(|existing_group_id| existing_group_id != group_id);
        }
    }
}

fn remove_group_membership(stations: &mut [Station], group_id: &str) {
    for station in stations {
        station
            .groups
            .retain(|existing_group_id| existing_group_id != group_id);
    }
}

fn delete_tag_from_collection(tags: &mut Vec<TagDefinition>, tag_id: &str) -> Result<(), String> {
    let initial_len = tags.len();
    tags.retain(|tag| tag.id != tag_id);
    if tags.len() == initial_len {
        return Err(format!("No tag found for id {tag_id}"));
    }

    Ok(())
}
#[tauri::command]
fn load_state() -> Result<AppSnapshot, String> {
    StateStore::load_snapshot().map_err(|error| error.to_string())
}

#[tauri::command]
fn save_state(payload: PersistedState) -> Result<AppSnapshot, String> {
    StateStore::save_payload(payload).map_err(|error| error.to_string())
}

#[tauri::command]
fn export_legacy_files() -> Result<String, String> {
    StateStore::export_legacy_files().map_err(|error| error.to_string())
}

#[tauri::command]
async fn run_station_action(
    action: StationAction,
    ids: Vec<String>,
) -> Result<ActionResult, String> {
    let snapshot = StateStore::load_snapshot().map_err(|error| error.to_string())?;
    let mut stations = snapshot.stations;
    let result = execute_station_action(action, ids, &mut stations).await?;

    if let Some(next_stations) = result.stations.clone() {
        StateStore::save_payload(PersistedState {
            stations: next_stations,
            options: snapshot.options,
            groups: snapshot.groups,
            tags: snapshot.tags,
        })
        .map_err(|error| error.to_string())?;
    }

    Ok(result)
}

#[tauri::command]
async fn fetch_station_runtime_for_ui(
    id: String,
    interval_seconds: i32,
) -> Result<StationRuntimeSnapshot, String> {
    let snapshot = StateStore::load_snapshot().map_err(|error| error.to_string())?;
    let station = snapshot
        .stations
        .iter()
        .find(|station| station.id == id)
        .ok_or_else(|| format!("No station found for id {id}"))?;
    fetch_station_runtime(station, interval_seconds).await
}

#[tauri::command]
async fn browse_station_files_for_ui(
    id: String,
    path: String,
) -> Result<RemoteFileBrowserResult, String> {
    let snapshot = StateStore::load_snapshot().map_err(|error| error.to_string())?;
    let station = snapshot
        .stations
        .iter()
        .find(|station| station.id == id)
        .ok_or_else(|| format!("No station found for id {id}"))?;
    browse_station_files(station, &path).await
}

#[tauri::command]
async fn rename_station_file_for_ui(
    id: String,
    old_path: String,
    new_path: String,
) -> Result<String, String> {
    let snapshot = StateStore::load_snapshot().map_err(|error| error.to_string())?;
    let station = snapshot
        .stations
        .iter()
        .find(|station| station.id == id)
        .ok_or_else(|| format!("No station found for id {id}"))?;
    rename_station_file(station, &old_path, &new_path).await
}

#[tauri::command]
async fn download_station_file_for_ui(
    id: String,
    remote_path: String,
    local_path: String,
) -> Result<String, String> {
    let snapshot = StateStore::load_snapshot().map_err(|error| error.to_string())?;
    let station = snapshot
        .stations
        .iter()
        .find(|station| station.id == id)
        .ok_or_else(|| format!("No station found for id {id}"))?;
    download_station_file(station, &remote_path, &local_path).await
}

#[tauri::command]
async fn upload_station_file_for_ui(
    id: String,
    local_path: String,
    remote_path: String,
) -> Result<String, String> {
    let snapshot = StateStore::load_snapshot().map_err(|error| error.to_string())?;
    let station = snapshot
        .stations
        .iter()
        .find(|station| station.id == id)
        .ok_or_else(|| format!("No station found for id {id}"))?;
    upload_station_file(station, &local_path, &remote_path).await
}

#[tauri::command]
async fn capture_station_screen_for_ui(id: String) -> Result<StationScreenCapture, String> {
    let snapshot = StateStore::load_snapshot().map_err(|error| error.to_string())?;
    let station = snapshot
        .stations
        .iter()
        .find(|station| station.id == id)
        .ok_or_else(|| format!("No station found for id {id}"))?;
    capture_station_screen(station).await
}

#[tauri::command]
async fn execute_station_command_for_ui(
    id: String,
    command: String,
    timeout_seconds: i32,
) -> Result<CommandExecutionResult, String> {
    let snapshot = StateStore::load_snapshot().map_err(|error| error.to_string())?;
    let station = snapshot
        .stations
        .iter()
        .find(|station| station.id == id)
        .ok_or_else(|| format!("No station found for id {id}"))?;
    execute_station_command(station, &command, timeout_seconds).await
}

#[tauri::command]
fn get_station_groups() -> Result<Vec<StationGroup>, String> {
    let snapshot = StateStore::load_snapshot().map_err(|error| error.to_string())?;
    Ok(snapshot.groups)
}

#[tauri::command]
fn create_station_group(
    name: String,
    description: String,
    color: String,
    icon: Option<String>,
    station_ids: Option<Vec<String>>,
) -> Result<StationGroup, String> {
    let mut snapshot = StateStore::load_snapshot().map_err(|error| error.to_string())?;
    let group = build_station_group(name, description, color, icon, station_ids);
    sync_station_group_membership(&mut snapshot.stations, &group.id, &group.station_ids);
    snapshot.groups.push(group.clone());
    StateStore::save_payload(PersistedState {
        stations: snapshot.stations,
        options: snapshot.options,
        groups: snapshot.groups,
        tags: snapshot.tags,
    })
    .map_err(|error| error.to_string())?;
    Ok(group)
}

#[tauri::command]
fn update_station_group(
    id: String,
    name: String,
    description: String,
    color: String,
    icon: Option<String>,
    station_ids: Vec<String>,
) -> Result<StationGroup, String> {
    let mut snapshot = StateStore::load_snapshot().map_err(|error| error.to_string())?;
    let group = snapshot
        .groups
        .iter_mut()
        .find(|g| g.id == id)
        .ok_or_else(|| format!("No group found for id {id}"))?;
    let station_ids = normalize_group_station_ids(station_ids);
    group.name = name;
    group.description = description;
    group.color = normalize_group_color(color);
    group.icon = normalize_group_icon(icon);
    group.station_ids = station_ids;
    let updated = group.clone();
    sync_station_group_membership(&mut snapshot.stations, &id, &updated.station_ids);
    StateStore::save_payload(PersistedState {
        stations: snapshot.stations,
        options: snapshot.options,
        groups: snapshot.groups,
        tags: snapshot.tags,
    })
    .map_err(|error| error.to_string())?;
    Ok(updated)
}

#[tauri::command]
fn delete_station_group(id: String) -> Result<String, String> {
    let mut snapshot = StateStore::load_snapshot().map_err(|error| error.to_string())?;
    let initial_len = snapshot.groups.len();
    snapshot.groups.retain(|g| g.id != id);
    if snapshot.groups.len() == initial_len {
        return Err(format!("No group found for id {id}"));
    }
    remove_group_membership(&mut snapshot.stations, &id);
    StateStore::save_payload(PersistedState {
        stations: snapshot.stations,
        options: snapshot.options,
        groups: snapshot.groups,
        tags: snapshot.tags,
    })
    .map_err(|error| error.to_string())?;
    Ok(format!("Group {id} deleted."))
}

// ============================================
// Phase 8: Alias commands matching frontend API
// ============================================

#[tauri::command]
fn load_groups() -> Result<Vec<StationGroup>, String> {
    let snapshot = StateStore::load_snapshot().map_err(|error| error.to_string())?;
    Ok(snapshot.groups)
}

#[tauri::command]
fn create_group(
    name: String,
    description: String,
    color: String,
    icon: Option<String>,
    station_ids: Option<Vec<String>>,
) -> Result<StationGroup, String> {
    let mut snapshot = StateStore::load_snapshot().map_err(|error| error.to_string())?;
    let group = build_station_group(name, description, color, icon, station_ids);
    sync_station_group_membership(&mut snapshot.stations, &group.id, &group.station_ids);
    snapshot.groups.push(group.clone());
    StateStore::save_payload(PersistedState {
        stations: snapshot.stations,
        options: snapshot.options,
        groups: snapshot.groups.clone(),
        tags: snapshot.tags.clone(),
    })
    .map_err(|error| error.to_string())?;
    Ok(group)
}

#[tauri::command]
fn update_group(
    id: String,
    name: String,
    description: String,
    color: String,
    icon: Option<String>,
    station_ids: Vec<String>,
) -> Result<StationGroup, String> {
    let mut snapshot = StateStore::load_snapshot().map_err(|error| error.to_string())?;
    let group = snapshot
        .groups
        .iter_mut()
        .find(|g| g.id == id)
        .ok_or_else(|| format!("No group found for id {id}"))?;

    let station_ids = normalize_group_station_ids(station_ids);
    group.name = name;
    group.description = description;
    group.color = normalize_group_color(color);
    group.icon = normalize_group_icon(icon);
    group.station_ids = station_ids;
    let updated = group.clone();
    sync_station_group_membership(&mut snapshot.stations, &id, &updated.station_ids);

    StateStore::save_payload(PersistedState {
        stations: snapshot.stations,
        options: snapshot.options,
        groups: snapshot.groups.clone(),
        tags: snapshot.tags.clone(),
    })
    .map_err(|error| error.to_string())?;
    Ok(updated)
}

#[tauri::command]
fn delete_group(group_id: String) -> Result<String, String> {
    let mut snapshot = StateStore::load_snapshot().map_err(|error| error.to_string())?;
    snapshot
        .groups
        .iter()
        .find(|g| g.id == group_id)
        .ok_or_else(|| format!("No group found for id {group_id}"))?;

    let initial_len = snapshot.groups.len();
    snapshot.groups.retain(|g| g.id != group_id);
    if snapshot.groups.len() == initial_len {
        return Err(format!("No group found for id {group_id}"));
    }
    remove_group_membership(&mut snapshot.stations, &group_id);

    StateStore::save_payload(PersistedState {
        stations: snapshot.stations,
        options: snapshot.options,
        groups: snapshot.groups,
        tags: snapshot.tags,
    })
    .map_err(|error| error.to_string())?;
    Ok(format!("Group {group_id} deleted."))
}

#[tauri::command]
fn add_station_to_group(group_id: String, station_id: String) -> Result<StationGroup, String> {
    let mut snapshot = StateStore::load_snapshot().map_err(|error| error.to_string())?;
    let group = snapshot
        .groups
        .iter_mut()
        .find(|g| g.id == group_id)
        .ok_or_else(|| format!("No group found for id {group_id}"))?;
    if !group.station_ids.contains(&station_id) {
        group.station_ids.push(station_id.clone());
    }
    group.station_ids = normalize_group_station_ids(group.station_ids.clone());
    let updated = group.clone();
    sync_station_group_membership(&mut snapshot.stations, &group_id, &updated.station_ids);
    StateStore::save_payload(PersistedState {
        stations: snapshot.stations,
        options: snapshot.options,
        groups: snapshot.groups,
        tags: snapshot.tags,
    })
    .map_err(|error| error.to_string())?;
    Ok(updated)
}

#[tauri::command]
fn remove_station_from_group(group_id: String, station_id: String) -> Result<StationGroup, String> {
    let mut snapshot = StateStore::load_snapshot().map_err(|error| error.to_string())?;
    let group = snapshot
        .groups
        .iter_mut()
        .find(|g| g.id == group_id)
        .ok_or_else(|| format!("No group found for id {group_id}"))?;
    group.station_ids.retain(|id| id != &station_id);
    let updated = group.clone();
    sync_station_group_membership(&mut snapshot.stations, &group_id, &updated.station_ids);
    StateStore::save_payload(PersistedState {
        stations: snapshot.stations,
        options: snapshot.options,
        groups: snapshot.groups,
        tags: snapshot.tags,
    })
    .map_err(|error| error.to_string())?;
    Ok(updated)
}

#[tauri::command]
fn get_stations_in_group(group_id: String) -> Result<Vec<String>, String> {
    let snapshot = StateStore::load_snapshot().map_err(|error| error.to_string())?;
    let group = snapshot
        .groups
        .iter()
        .find(|g| g.id == group_id)
        .ok_or_else(|| format!("No group found for id {group_id}"))?;
    Ok(group.station_ids.clone())
}

#[tauri::command]
fn export_groups() -> Result<Vec<StationGroup>, String> {
    let snapshot = StateStore::load_snapshot().map_err(|error| error.to_string())?;
    Ok(snapshot.groups)
}

#[tauri::command]
fn get_group_stats() -> Result<Vec<StationGroup>, String> {
    let snapshot = StateStore::load_snapshot().map_err(|error| error.to_string())?;
    Ok(snapshot.groups)
}

#[tauri::command]
fn load_tag_definitions() -> Result<Vec<TagDefinition>, String> {
    let snapshot = StateStore::load_snapshot().map_err(|error| error.to_string())?;
    Ok(snapshot.tags)
}

#[tauri::command]
fn create_tag_definition(
    key: String,
    name: String,
    description: String,
    color: String,
    r#type: Option<String>,
    options: Option<Vec<String>>,
    required: Option<bool>,
    default_value: Option<String>,
) -> Result<TagDefinition, String> {
    let mut snapshot = StateStore::load_snapshot().map_err(|error| error.to_string())?;
    if key.trim().is_empty() {
        return Err("Tag key is required".to_string());
    }
    if snapshot.tags.iter().any(|existing| existing.id == key) {
        return Err(format!("Tag key '{key}' already exists"));
    }

    let mut tag = TagDefinition::new(&name);
    tag.id = key;
    tag.description = description;
    tag.color = color;
    if let Some(next_type) = r#type {
        tag.r#type = next_type;
    }
    if let Some(next_options) = options {
        tag.options = next_options;
    }
    if let Some(next_required) = required {
        tag.required = next_required;
    }
    tag.default_value = default_value;
    snapshot.tags.push(tag.clone());
    StateStore::save_payload(PersistedState {
        stations: snapshot.stations,
        options: snapshot.options,
        groups: snapshot.groups.clone(),
        tags: snapshot.tags.clone(),
    })
    .map_err(|error| error.to_string())?;
    Ok(tag)
}

#[tauri::command]
fn update_tag_definition(
    id: String,
    name: String,
    description: String,
    color: String,
    r#type: Option<String>,
    options: Option<Vec<String>>,
    required: Option<bool>,
    default_value: Option<String>,
) -> Result<TagDefinition, String> {
    let mut snapshot = StateStore::load_snapshot().map_err(|error| error.to_string())?;
    let tag = snapshot
        .tags
        .iter_mut()
        .find(|t| t.id == id)
        .ok_or_else(|| format!("No tag found for id {id}"))?;
    tag.name = name;
    tag.description = description;
    tag.color = color;
    if let Some(next_type) = r#type {
        tag.r#type = next_type;
    }
    if let Some(next_options) = options {
        tag.options = next_options;
    }
    if let Some(next_required) = required {
        tag.required = next_required;
    }
    tag.default_value = default_value;
    tag.updated_at = chrono::Utc::now();
    let updated = tag.clone();
    StateStore::save_payload(PersistedState {
        stations: snapshot.stations,
        options: snapshot.options,
        groups: snapshot.groups.clone(),
        tags: snapshot.tags.clone(),
    })
    .map_err(|error| error.to_string())?;
    Ok(updated)
}

#[tauri::command]
fn delete_tag_definition(key: String) -> Result<String, String> {
    let mut snapshot = StateStore::load_snapshot().map_err(|error| error.to_string())?;
    delete_tag_from_collection(&mut snapshot.tags, &key)?;
    StateStore::save_payload(PersistedState {
        stations: snapshot.stations,
        options: snapshot.options,
        groups: snapshot.groups,
        tags: snapshot.tags,
    })
    .map_err(|error| error.to_string())?;
    Ok(format!("Tag {key} deleted."))
}

#[tauri::command]
fn export_tag_definitions() -> Result<Vec<TagDefinition>, String> {
    let snapshot = StateStore::load_snapshot().map_err(|error| error.to_string())?;
    Ok(snapshot.tags)
}

#[tauri::command]
fn get_station_tags(station_id: String) -> Result<HashMap<String, String>, String> {
    let snapshot = StateStore::load_snapshot().map_err(|error| error.to_string())?;
    let station = snapshot
        .stations
        .iter()
        .find(|station| station.id == station_id)
        .ok_or_else(|| format!("No station found for id {station_id}"))?;
    Ok(station.tags.clone())
}

#[tauri::command]
fn update_station_tags(
    station_id: String,
    tags: HashMap<String, String>,
) -> Result<HashMap<String, String>, String> {
    let mut snapshot = StateStore::load_snapshot().map_err(|error| error.to_string())?;
    let station = snapshot
        .stations
        .iter_mut()
        .find(|station| station.id == station_id)
        .ok_or_else(|| format!("No station found for id {station_id}"))?;

    station.tags = tags.clone();

    StateStore::save_payload(PersistedState {
        stations: snapshot.stations,
        options: snapshot.options,
        groups: snapshot.groups,
        tags: snapshot.tags,
    })
    .map_err(|error| error.to_string())?;

    Ok(tags)
}

#[tauri::command]
fn batch_update_station_tags(
    station_ids: Vec<String>,
    tags: HashMap<String, String>,
) -> Result<String, String> {
    let mut snapshot = StateStore::load_snapshot().map_err(|error| error.to_string())?;
    let mut updated = 0usize;

    for station in &mut snapshot.stations {
        if station_ids.iter().any(|id| id == &station.id) {
            for (key, value) in &tags {
                station.tags.insert(key.clone(), value.clone());
            }
            updated += 1;
        }
    }

    StateStore::save_payload(PersistedState {
        stations: snapshot.stations,
        options: snapshot.options,
        groups: snapshot.groups,
        tags: snapshot.tags,
    })
    .map_err(|error| error.to_string())?;

    Ok(format!("Updated tags for {updated} station(s)."))
}

#[tauri::command]
fn get_tags() -> Result<Vec<TagDefinition>, String> {
    let snapshot = StateStore::load_snapshot().map_err(|error| error.to_string())?;
    Ok(snapshot.tags)
}

#[tauri::command]
fn create_tag(name: String, description: String, color: String) -> Result<TagDefinition, String> {
    let mut snapshot = StateStore::load_snapshot().map_err(|error| error.to_string())?;
    let mut tag = TagDefinition::new(&name);
    tag.description = description;
    tag.color = color;
    snapshot.tags.push(tag.clone());
    StateStore::save_payload(PersistedState {
        stations: snapshot.stations,
        options: snapshot.options,
        groups: snapshot.groups,
        tags: snapshot.tags,
    })
    .map_err(|error| error.to_string())?;
    Ok(tag)
}

#[tauri::command]
fn update_tag(
    id: String,
    name: String,
    description: String,
    color: String,
) -> Result<TagDefinition, String> {
    let mut snapshot = StateStore::load_snapshot().map_err(|error| error.to_string())?;
    let tag = snapshot
        .tags
        .iter_mut()
        .find(|t| t.id == id)
        .ok_or_else(|| format!("No tag found for id {id}"))?;
    tag.name = name;
    tag.description = description;
    tag.color = color;
    tag.updated_at = chrono::Utc::now();
    let updated = tag.clone();
    StateStore::save_payload(PersistedState {
        stations: snapshot.stations,
        options: snapshot.options,
        groups: snapshot.groups,
        tags: snapshot.tags,
    })
    .map_err(|error| error.to_string())?;
    Ok(updated)
}

#[tauri::command]
fn delete_tag(id: String) -> Result<String, String> {
    let mut snapshot = StateStore::load_snapshot().map_err(|error| error.to_string())?;
    delete_tag_from_collection(&mut snapshot.tags, &id)?;
    StateStore::save_payload(PersistedState {
        stations: snapshot.stations,
        options: snapshot.options,
        groups: snapshot.groups,
        tags: snapshot.tags,
    })
    .map_err(|error| error.to_string())?;
    Ok(format!("Tag {id} deleted."))
}

#[tauri::command]
async fn set_station_gathering_interval_for_ui(interval_seconds: i32) -> Result<String, String> {
    let snapshot = StateStore::load_snapshot().map_err(|error| error.to_string())?;
    if snapshot.stations.is_empty() {
        return Err("No stations configured.".into());
    }
    let mut ok_count = 0usize;
    let mut errs = Vec::new();
    for station in &snapshot.stations {
        match set_station_gathering_interval(station, interval_seconds).await {
            Ok(()) => ok_count += 1,
            Err(e) => errs.push(format!("{}: {e}", station.name)),
        }
    }
    if errs.is_empty() {
        Ok(format!("Interval set to {interval_seconds}s on {ok_count} station(s).").into())
    } else {
        Err(format!(
            "Set on {ok_count} station(s); failed on: {}",
            errs.join("; ")
        ))
    }
}

#[cfg_attr(mobile, tauri::mobile_entry_point)]
pub fn run() {
    tauri::Builder::default()
        .setup(|app| {
            let app_handle = app.handle().clone();
            tauri::async_runtime::spawn(async move {
                let mut bridge = MqttWsBridge::new(app_handle, "ws://127.0.0.1:8080");
                if let Err(error) = bridge.start().await {
                    eprintln!("failed to start MQTT websocket bridge: {error}");
                }
            });
            Ok(())
        })
        .invoke_handler(tauri::generate_handler![
            load_state,
            save_state,
            export_legacy_files,
            run_station_action,
            fetch_station_runtime_for_ui,
            browse_station_files_for_ui,
            rename_station_file_for_ui,
            download_station_file_for_ui,
            upload_station_file_for_ui,
            capture_station_screen_for_ui,
            execute_station_command_for_ui,
            get_station_groups,
            create_station_group,
            update_station_group,
            delete_station_group,
            get_tags,
            create_tag,
            update_tag,
            delete_tag,
            set_station_gathering_interval_for_ui,
            // Phase 8 frontend API aliases
            load_groups,
            create_group,
            update_group,
            delete_group,
            add_station_to_group,
            remove_station_from_group,
            get_stations_in_group,
            export_groups,
            get_group_stats,
            load_tag_definitions,
            create_tag_definition,
            update_tag_definition,
            delete_tag_definition,
            export_tag_definitions,
            get_station_tags,
            update_station_tags,
            batch_update_station_tags,
            load_batch_tasks,
            save_batch_task,
            delete_batch_task,
            execute_batch_task,
            execute_batch,
            cancel_batch_execution,
            pause_batch_execution,
            resume_batch_execution,
            get_batch_targets,
            get_batch_execution_status,
            get_batch_history,
            validate_batch_task,
            preview_batch_targets,
            import_batch_package,
            export_batch_package,
            duplicate_batch_task,
            toggle_batch_task_favorite,
            load_alert_rules,
            save_alert_rule,
            delete_alert_rule,
            toggle_alert_rule,
            duplicate_alert_rule,
            get_alert_history,
            get_alert_stats,
            validate_alert_rule,
            preview_alert_targets,
            import_alert_rules,
            export_alert_rules,
            acknowledge_alert,
            resolve_alert,
            get_alert_templates,
            get_alert_available_targets,
            append_alert_history,
        ])
        .run(tauri::generate_context!())
        .expect("error while running tauri application");
}

#[cfg(test)]
mod tests {
    use super::*;

    fn make_station(id: &str) -> Station {
        Station {
            id: id.to_string(),
            name: format!("station-{id}"),
            blocked: false,
            network_interfaces: Vec::new(),
            start_programs: Vec::new(),
            monitor_processes: Vec::new(),
            last_action: None,
            groups: Vec::new(),
            tags: HashMap::new(),
            metadata: HashMap::new(),
            location: None,
        }
    }

    #[test]
    fn build_station_group_sets_description_appearance_and_membership_defaults() {
        let group = build_station_group(
            "Render".to_string(),
            "Render nodes".to_string(),
            "".to_string(),
            Some("  rack  ".to_string()),
            None,
        );

        assert_eq!(group.name, "Render");
        assert_eq!(group.description, "Render nodes");
        assert_eq!(group.color, "#3b82f6");
        assert_eq!(group.icon.as_deref(), Some("rack"));
        assert!(group.station_ids.is_empty());
    }

    #[test]
    fn update_group_syncs_station_reverse_membership() {
        let mut s1 = make_station("s1");
        s1.groups.push("g1".to_string());
        let mut stations = vec![s1, make_station("s2")];

        sync_station_group_membership(&mut stations, "g1", &["s2".to_string()]);

        assert!(stations[0].groups.is_empty());
        assert_eq!(stations[1].groups, vec!["g1".to_string()]);
    }

    #[test]
    fn add_station_to_group_syncs_station_reverse_membership() {
        let mut stations = vec![make_station("s1")];

        sync_station_group_membership(&mut stations, "g1", &["s1".to_string()]);

        assert_eq!(stations[0].groups, vec!["g1".to_string()]);
    }

    #[test]
    fn remove_station_from_group_syncs_station_reverse_membership() {
        let mut station = make_station("s1");
        station.groups.push("g1".to_string());
        let mut stations = vec![station];

        sync_station_group_membership(&mut stations, "g1", &[]);

        assert!(stations[0].groups.is_empty());
    }

    #[test]
    fn deleting_tag_definition_does_not_mutate_groups() {
        let group = build_station_group(
            "Ops".to_string(),
            String::new(),
            "#0ea5e9".to_string(),
            Some("servers".to_string()),
            Some(vec!["s1".to_string()]),
        );
        let preserved_group = serde_json::to_value(&group).unwrap();
        let mut snapshot = PersistedState {
            groups: vec![group],
            tags: vec![TagDefinition::new("Environment")],
            ..PersistedState::default()
        };
        let tag_id = snapshot.tags[0].id.clone();

        delete_tag_from_collection(&mut snapshot.tags, &tag_id).unwrap();

        assert!(snapshot.tags.is_empty());
        assert_eq!(snapshot.groups.len(), 1);
        assert_eq!(
            serde_json::to_value(&snapshot.groups[0]).unwrap(),
            preserved_group
        );
    }

    #[test]
    fn legacy_group_snapshots_with_tags_still_deserialize() {
        let payload = serde_json::from_str::<PersistedState>(
            r#"{
                "stations": [],
                "options": {
                    "interval": 2,
                    "isFirstTimeRun": false,
                    "startApps": [],
                    "monitorProcesses": []
                },
                "groups": [
                    {
                        "id": "g1",
                        "name": "Legacy",
                        "description": "Imported",
                        "tags": ["deprecated"],
                        "stationIds": ["s1"]
                    }
                ],
                "tags": []
            }"#,
        )
        .unwrap();

        assert_eq!(payload.groups.len(), 1);
        assert_eq!(payload.groups[0].color, "#3b82f6");
        assert_eq!(payload.groups[0].icon, None);
        assert_eq!(payload.groups[0].station_ids, vec!["s1".to_string()]);
        assert!(
            serde_json::to_value(&payload.groups[0])
                .unwrap()
                .get("tags")
                .is_none()
        );
    }
}
