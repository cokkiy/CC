use control::{execute_station_action, set_station_gathering_interval, set_station_weather_picture_option};
use models::{ActionResult, AppSnapshot, PersistedState, StationAction, StationGroup, WeatherImageOption};
use remote::{
    browse_station_files, capture_station_screen, download_station_file, execute_station_command,
    fetch_station_runtime, rename_station_file, upload_station_file, CommandExecutionResult,
    RemoteFileBrowserResult, StationRuntimeSnapshot, StationScreenCapture,
};
use storage::StateStore;

pub mod control;
pub mod remote;

mod grpc;
mod models;
mod storage;
mod wol;
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
    })
    .map_err(|error| error.to_string())?;
    Ok(format!("Group {id} deleted."))
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

#[tauri::command]
async fn set_station_weather_option_for_ui(
    option: WeatherImageOption,
) -> Result<String, String> {
    let snapshot = StateStore::load_snapshot().map_err(|error| error.to_string())?;
    if snapshot.stations.is_empty() {
        return Err("No stations configured.".into());
    }
    let mut ok_count = 0usize;
    let mut errs = Vec::new();
    for station in &snapshot.stations {
        match set_station_weather_picture_option(station, &option).await {
            Ok(()) => ok_count += 1,
            Err(e) => errs.push(format!("{}: {e}", station.name)),
        }
    }
    if errs.is_empty() {
        Ok(format!("Weather options saved to {ok_count} station(s).").into())
    } else {
        Err(format!(
            "Saved to {ok_count} station(s); failed on: {}",
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
            set_station_gathering_interval_for_ui,
            set_station_weather_option_for_ui,
        ])
        .run(tauri::generate_context!())
        .expect("error while running tauri application");
}
