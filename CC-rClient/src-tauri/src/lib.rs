use control::{execute_station_action, set_station_gathering_interval};
use models::{ActionResult, AppSnapshot, PersistedState, StationAction, StationGroup, TagDefinition};
use remote::{
    browse_station_files, capture_station_screen, download_station_file, execute_station_command,
    fetch_station_runtime, rename_station_file, upload_station_file, CommandExecutionResult,
    RemoteFileBrowserResult, StationRuntimeSnapshot, StationScreenCapture,
};
use storage::StateStore;
use std::collections::HashMap;
use tokio::sync::RwLock;
use ws_bridge::MqttWsBridge;


pub mod control;
pub mod grpc;
pub mod models;
pub mod remote;
pub mod storage;
pub mod websocket;
pub mod wol;
pub mod ws_bridge;
// Global WebSocket bridge instance - lazily initialized
static WS_BRIDGE: RwLock<Option<MqttWsBridge>> = RwLock::const_new(None);
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
fn create_station_group(name: String) -> Result<StationGroup, String> {
    let mut snapshot = StateStore::load_snapshot().map_err(|error| error.to_string())?;
    let group = StationGroup::new(&name);
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
    tags: Vec<String>,
    station_ids: Vec<String>,
) -> Result<StationGroup, String> {
    let mut snapshot = StateStore::load_snapshot().map_err(|error| error.to_string())?;
    let group = snapshot
        .groups
        .iter_mut()
        .find(|g| g.id == id)
        .ok_or_else(|| format!("No group found for id {id}"))?;
    group.name = name;
    group.description = description;
    group.tags = tags;
    group.station_ids = station_ids;
    let updated = group.clone();
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
fn create_group(name: String, description: String) -> Result<StationGroup, String> {
    let mut snapshot = StateStore::load_snapshot().map_err(|error| error.to_string())?;
    let mut group = StationGroup::new(&name);
    group.description = description;
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
fn update_group(id: String, name: String, description: String, tags: Vec<String>, station_ids: Vec<String>) -> Result<StationGroup, String> {
    let mut snapshot = StateStore::load_snapshot().map_err(|error| error.to_string())?;
    let group = snapshot.groups.iter_mut().find(|g| g.id == id)
        .ok_or_else(|| format!("No group found for id {id}"))?;

    let previous_station_ids = group.station_ids.clone();

    group.name = name;
    group.description = description;
    group.tags = tags;
    group.station_ids = station_ids;
    let updated = group.clone();

    // Keep reverse mapping in sync: station.groups must reflect group membership.
    for station in &mut snapshot.stations {
        let was_in_group = previous_station_ids.iter().any(|sid| sid == &station.id);
        let is_in_group = updated.station_ids.iter().any(|sid| sid == &station.id);

        if is_in_group && !station.groups.iter().any(|gid| gid == &id) {
            station.groups.push(id.clone());
        }

        if was_in_group && !is_in_group {
            station.groups.retain(|gid| gid != &id);
        }
    }

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
    let removed_station_ids = snapshot
        .groups
        .iter()
        .find(|g| g.id == group_id)
        .map(|g| g.station_ids.clone())
        .ok_or_else(|| format!("No group found for id {group_id}"))?;

    let initial_len = snapshot.groups.len();
    snapshot.groups.retain(|g| g.id != group_id);
    if snapshot.groups.len() == initial_len {
        return Err(format!("No group found for id {group_id}"));
    }

    for station in &mut snapshot.stations {
        if removed_station_ids.iter().any(|sid| sid == &station.id) {
            station.groups.retain(|gid| gid != &group_id);
        }
    }

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
    let group = snapshot.groups.iter_mut().find(|g| g.id == group_id)
        .ok_or_else(|| format!("No group found for id {group_id}"))?;
    if !group.station_ids.contains(&station_id) {
        group.station_ids.push(station_id.clone());
    }

    for station in &mut snapshot.stations {
        if station.id == station_id {
            if !station.groups.iter().any(|gid| gid == &group_id) {
                station.groups.push(group_id.clone());
            }
            break;
        }
    }

    let updated = group.clone();
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
    let group = snapshot.groups.iter_mut().find(|g| g.id == group_id)
        .ok_or_else(|| format!("No group found for id {group_id}"))?;
    group.station_ids.retain(|id| id != &station_id);

    for station in &mut snapshot.stations {
        if station.id == station_id {
            station.groups.retain(|gid| gid != &group_id);
            break;
        }
    }

    let updated = group.clone();
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
    let group = snapshot.groups.iter().find(|g| g.id == group_id)
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
    let tag = snapshot.tags.iter_mut().find(|t| t.id == id)
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
    let initial_len = snapshot.tags.len();
    snapshot.tags.retain(|t| t.id != key);
    if snapshot.tags.len() == initial_len {
        return Err(format!("No tag found for id {key}"));
    }
    // Also remove this tag from all groups that reference it
    for group in &mut snapshot.groups {
        group.tags.retain(|tag_id| tag_id != &key);
    }
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
    let initial_len = snapshot.tags.len();
    snapshot.tags.retain(|t| t.id != id);
    if snapshot.tags.len() == initial_len {
        return Err(format!("No tag found for id {id}"));
    }
    // Also remove this tag from all groups that reference it
    for group in &mut snapshot.groups {
        group.tags.retain(|tag_id| tag_id != &id);
    }
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
async fn set_station_gathering_interval_for_ui(
    interval_seconds: i32,
) -> Result<String, String> {
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
        ])
        .run(tauri::generate_context!())
        .expect("error while running tauri application");
}

#[cfg(test)]
mod tests {
    use super::*;
    use crate::models::Station;

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
    fn update_group_syncs_station_reverse_membership() {
        let mut station = make_station("s1");
        station.groups.push("g1".to_string());

        let mut group = StationGroup::new("g1");
        group.id = "g1".to_string();
        group.station_ids = vec!["s1".to_string()];

        let previous_station_ids = group.station_ids.clone();
        group.station_ids = Vec::new();

        let mut stations = vec![station];
        for target_station in &mut stations {
            let was_in_group = previous_station_ids.iter().any(|sid| sid == &target_station.id);
            let is_in_group = group.station_ids.iter().any(|sid| sid == &target_station.id);

            if is_in_group && !target_station.groups.iter().any(|gid| gid == &group.id) {
                target_station.groups.push(group.id.clone());
            }

            if was_in_group && !is_in_group {
                target_station.groups.retain(|gid| gid != &group.id);
            }
        }

        assert!(stations[0].groups.is_empty());
    }

    #[test]
    fn add_station_to_group_syncs_station_reverse_membership() {
        let mut station = make_station("s1");
        let group_id = "g1".to_string();

        if !station.groups.iter().any(|gid| gid == &group_id) {
            station.groups.push(group_id.clone());
        }

        assert_eq!(station.groups, vec!["g1".to_string()]);
    }
}
