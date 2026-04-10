use control::execute_station_action;
use models::{ActionResult, AppSnapshot, PersistedState, StationAction};
use remote::{
    browse_station_files, capture_station_screen, download_station_file, fetch_station_runtime,
    rename_station_file, upload_station_file, RemoteFileBrowserResult, StationRuntimeSnapshot,
    StationScreenCapture,
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
            capture_station_screen_for_ui
        ])
        .run(tauri::generate_context!())
        .expect("error while running tauri application");
}
