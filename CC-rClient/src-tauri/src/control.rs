use std::collections::BTreeSet;
use std::net::{IpAddr, SocketAddr};
use std::path::Path;

use serde::{Deserialize, Serialize};
use tonic::transport::Channel;

use crate::grpc::cc::{
    AppStartParameter, AppStartingResult, CloseAppRequest, Empty, GetAllProcessInfoResponse,
    GetCurrentTelemetrySchemaResponse, GetTelemetryProfilesResponse, RebootRequest,
    ReplaceTelemetryProfilesRequest, RestartAppRequest, SetStateGatheringIntervalRequest,
    ShutdownRequest, StartAppRequest, TelemetryInclude, TelemetryProfile,
    station_control_client::StationControlClient,
};
use crate::models::{ActionResult, StartProgram, Station, StationAction};
use crate::storage::StorageError;
use crate::wol;

const DEFAULT_STATION_CONTROL_PORT: u16 = 50051;

const APP_CONTROL_RESULT_ALREADY_RUNNING: i32 = 1;
const APP_CONTROL_RESULT_STARTED: i32 = 2;
const APP_CONTROL_RESULT_FAIL_TO_START: i32 = 3;
const APP_CONTROL_RESULT_NOT_RUNNING: i32 = 4;
const APP_CONTROL_RESULT_CLOSED: i32 = 5;
const APP_CONTROL_RESULT_FAIL_TO_CLOSE: i32 = 6;
const APP_CONTROL_RESULT_ERROR: i32 = 7;

#[derive(Debug, Clone, Serialize, Deserialize, PartialEq, Eq)]
#[serde(rename_all = "camelCase")]
pub struct TelemetryProfileDto {
    pub id: String,
    pub name: String,
    pub enabled: bool,
    pub collection_interval_ms: i64,
    pub includes: Vec<String>,
}

#[derive(Debug, Clone, Serialize, Deserialize, PartialEq, Eq)]
#[serde(rename_all = "camelCase")]
pub struct TelemetryIncludeDefinitionDto {
    pub key: String,
    pub label: String,
    pub description: String,
}

#[derive(Debug, Clone, Serialize, Deserialize, PartialEq, Eq)]
#[serde(rename_all = "camelCase")]
pub struct TelemetryProfilesResponseDto {
    pub schema_version: u32,
    pub profiles_version: u64,
    pub profiles: Vec<TelemetryProfileDto>,
}

#[derive(Debug, Clone, Serialize, Deserialize, PartialEq, Eq)]
#[serde(rename_all = "camelCase")]
pub struct TelemetrySchemaResponseDto {
    pub schema_version: u32,
    pub supported_includes: Vec<TelemetryIncludeDefinitionDto>,
}

pub async fn execute_station_action(
    action: StationAction,
    ids: Vec<String>,
    stations: &mut Vec<Station>,
) -> Result<ActionResult, String> {
    // For batch actions, operate on ALL stations (ignore the ids parameter)
    let target_indices: Vec<usize> = if matches!(
        action,
        StationAction::BatchPowerOn | StationAction::BatchShutdown | StationAction::BatchReboot
    ) {
        (0..stations.len()).collect()
    } else {
        stations
            .iter()
            .enumerate()
            .filter(|(_, station)| ids.iter().any(|id| id == &station.id))
            .map(|(index, _)| index)
            .collect::<Vec<_>>()
    };

    if target_indices.is_empty() {
        return Ok(ActionResult {
            ok: false,
            message: "No stations matched the requested action.".into(),
            stations: Some(stations.clone()),
            implemented: false,
        });
    }

    // Batch actions: loop through each target and execute the corresponding single action
    let result = if matches!(
        action,
        StationAction::BatchPowerOn | StationAction::BatchShutdown | StationAction::BatchReboot
    ) {
        execute_batch_action(&action, &target_indices, stations).await?
    } else if matches!(
        action,
        StationAction::PowerOn | StationAction::Block | StationAction::Unblock
    ) {
        execute_native_action(&action, &target_indices, stations)
            .map_err(|error| error.to_string())?
    } else {
        execute_remote_action(&action, &target_indices, stations).await?
    };

    Ok(ActionResult {
        stations: Some(stations.clone()),
        ..result
    })
}

fn execute_native_action(
    action: &StationAction,
    selected_indices: &[usize],
    stations: &mut [Station],
) -> Result<ActionResult, StorageError> {
    match action {
        StationAction::PowerOn => {
            for index in selected_indices {
                let station = &mut stations[*index];
                wol::send_magic_packets(station)?;
                station.last_action = Some("Wake-on-LAN packet sent".into());
            }
            Ok(ActionResult {
                ok: true,
                message: format!(
                    "Sent Wake-on-LAN packets to {} station(s).",
                    selected_indices.len()
                ),
                stations: None,
                implemented: true,
            })
        }
        StationAction::Block => {
            for index in selected_indices {
                let station = &mut stations[*index];
                station.blocked = true;
                station.last_action = Some("Marked blocked in Rust state".into());
            }
            Ok(ActionResult {
                ok: true,
                message: "Marked the selected station(s) as blocked.".into(),
                stations: None,
                implemented: true,
            })
        }
        StationAction::Unblock => {
            for index in selected_indices {
                let station = &mut stations[*index];
                station.blocked = false;
                station.last_action = Some("Marked unblocked in Rust state".into());
            }
            Ok(ActionResult {
                ok: true,
                message: "Marked the selected station(s) as unblocked.".into(),
                stations: None,
                implemented: true,
            })
        }
        _ => unreachable!("remote actions are handled separately"),
    }
}

async fn execute_batch_action(
    action: &StationAction,
    target_indices: &[usize],
    stations: &mut [Station],
) -> Result<ActionResult, String> {
    let single_action = match action {
        StationAction::BatchPowerOn => StationAction::PowerOn,
        StationAction::BatchShutdown => StationAction::Shutdown,
        StationAction::BatchReboot => StationAction::Reboot,
        _ => unreachable!(),
    };

    let mut success_count = 0usize;
    let mut failure_count = 0usize;
    let mut messages = Vec::with_capacity(target_indices.len());

    for index in target_indices {
        let station = &mut stations[*index];
        let result = if matches!(single_action, StationAction::PowerOn) {
            wol::send_magic_packets(station)
                .map(|_| {
                    station.last_action = Some("Wake-on-LAN packet sent".into());
                    "Wake-on-LAN packet sent".to_string()
                })
                .map_err(|e| e.to_string())
        } else {
            let (mut client, endpoint) = connect_station(station).await?;
            match &single_action {
                StationAction::Shutdown => client
                    .shutdown(ShutdownRequest {})
                    .await
                    .map(|_| {
                        station.last_action = Some(format!("Shutdown requested via {endpoint}"));
                        format!("Shutdown requested via {endpoint}")
                    })
                    .map_err(|e| format!("shutdown RPC failed via {endpoint}: {e}")),
                StationAction::Reboot => client
                    .reboot(RebootRequest { force: false })
                    .await
                    .map(|_| {
                        station.last_action = Some(format!("Reboot requested via {endpoint}"));
                        format!("Reboot requested via {endpoint}")
                    })
                    .map_err(|e| format!("reboot RPC failed via {endpoint}: {e}")),
                _ => unreachable!(),
            }
        };

        match result {
            Ok(msg) => {
                success_count += 1;
                messages.push(format!("{}: {msg}", station_label(station)));
            }
            Err(err) => {
                failure_count += 1;
                station.last_action = Some(err.clone());
                messages.push(format!("{}: {err}", station_label(station)));
            }
        }
    }

    let action_label = single_action.label();
    Ok(ActionResult {
        ok: failure_count == 0,
        message: format!(
            "Batch {action_label}: {success_count} success, {failure_count} failed. {}",
            messages.join(" | ")
        ),
        stations: None,
        implemented: true,
    })
}

async fn execute_remote_action(
    action: &StationAction,
    selected_indices: &[usize],
    stations: &mut [Station],
) -> Result<ActionResult, String> {
    let mut success_count = 0usize;
    let mut messages = Vec::with_capacity(selected_indices.len());

    for index in selected_indices {
        let station = &mut stations[*index];
        match execute_remote_action_for_station(action, station).await {
            Ok(message) => {
                station.last_action = Some(message.clone());
                success_count += 1;
                messages.push(format!("{}: {message}", station_label(station)));
            }
            Err(error) => {
                station.last_action = Some(error.clone());
                messages.push(format!("{}: {error}", station_label(station)));
            }
        }
    }

    let failure_count = selected_indices.len().saturating_sub(success_count);
    Ok(ActionResult {
        ok: failure_count == 0,
        message: format!(
            "{} finished with {} success(es) and {} failure(s). {}",
            action.label(),
            success_count,
            failure_count,
            messages.join(" | ")
        ),
        stations: None,
        implemented: true,
    })
}

async fn execute_remote_action_for_station(
    action: &StationAction,
    station: &Station,
) -> Result<String, String> {
    let (mut client, endpoint) = connect_station(station).await?;
    match action {
        StationAction::StartApp => {
            let response = client
                .start_app(StartAppRequest {
                    apps: build_app_parameters(&station.start_programs),
                })
                .await
                .map_err(|error| format!("start app RPC failed via {endpoint}: {error}"))?;
            Ok(format!(
                "{} via {endpoint}",
                summarize_app_start_results("start", &response.into_inner().results)
            ))
        }
        StationAction::RestartApp => {
            let response = client
                .restart_app(RestartAppRequest {
                    apps: build_app_parameters(&station.start_programs),
                })
                .await
                .map_err(|error| format!("restart app RPC failed via {endpoint}: {error}"))?;
            Ok(format!(
                "{} via {endpoint}",
                summarize_app_start_results("restart", &response.into_inner().results)
            ))
        }
        StationAction::ExitApp => {
            let process_ids = discover_station_process_ids(&mut client, station).await?;
            let response = client
                .close_app(CloseAppRequest { process_ids })
                .await
                .map_err(|error| format!("close app RPC failed via {endpoint}: {error}"))?;
            Ok(format!(
                "{} via {endpoint}",
                summarize_close_results(&response.into_inner().results)
            ))
        }
        StationAction::Shutdown => {
            client
                .shutdown(ShutdownRequest {})
                .await
                .map_err(|error| format!("shutdown RPC failed via {endpoint}: {error}"))?;
            Ok(format!("Shutdown requested via {endpoint}."))
        }
        StationAction::Reboot => {
            client
                .reboot(RebootRequest { force: false })
                .await
                .map_err(|error| format!("reboot RPC failed via {endpoint}: {error}"))?;
            Ok(format!("Reboot requested via {endpoint}."))
        }
        StationAction::PowerOn | StationAction::Block | StationAction::Unblock => {
            Err(format!("{} is handled locally.", action.label()))
        }
        StationAction::BatchPowerOn | StationAction::BatchShutdown | StationAction::BatchReboot => {
            Err(format!(
                "Batch action {} is handled in the batch dispatcher.",
                action.label()
            ))
        }
    }
}

async fn connect_station(
    station: &Station,
) -> Result<(StationControlClient<Channel>, String), String> {
    let endpoints = station_endpoints(station);
    if endpoints.is_empty() {
        return Err(format!(
            "No usable IP address is configured for {}.",
            station_label(station)
        ));
    }

    let mut last_error = String::new();
    for endpoint in endpoints {
        match StationControlClient::connect(endpoint.clone()).await {
            Ok(client) => return Ok((client, endpoint)),
            Err(error) => {
                last_error = format!("{error}");
            }
        }
    }

    Err(format!(
        "Unable to connect to {} on the Rust station-service endpoint(s): {}.",
        station_label(station),
        last_error
    ))
}

pub(crate) fn station_endpoints(station: &Station) -> Vec<String> {
    let mut endpoints = BTreeSet::new();
    for interface in &station.network_interfaces {
        for raw in &interface.ips {
            let value = raw.trim();
            if value.is_empty() {
                continue;
            }

            let endpoint = if value.starts_with("http://") || value.starts_with("https://") {
                value.to_string()
            } else if let Ok(socket) = value.parse::<SocketAddr>() {
                format!("http://{socket}")
            } else if let Ok(ip) = value.parse::<IpAddr>() {
                match ip {
                    IpAddr::V4(ipv4) => format!("http://{ipv4}:{DEFAULT_STATION_CONTROL_PORT}"),
                    IpAddr::V6(ipv6) => format!("http://[{ipv6}]:{DEFAULT_STATION_CONTROL_PORT}"),
                }
            } else {
                format!("http://{value}:{DEFAULT_STATION_CONTROL_PORT}")
            };

            endpoints.insert(endpoint);
        }
    }

    endpoints.into_iter().collect()
}

fn build_app_parameters(programs: &[StartProgram]) -> Vec<AppStartParameter> {
    programs
        .iter()
        .enumerate()
        .filter(|(_, program)| !program.path.trim().is_empty())
        .map(|(index, program)| AppStartParameter {
            param_id: (index + 1) as i32,
            app_path: program.path.clone(),
            arguments: program.arguments.clone(),
            process_name: effective_process_name(program),
            allow_multi_instance: program.allow_multi_instance,
        })
        .collect()
}

fn effective_process_name(program: &StartProgram) -> String {
    if !program.process_name.trim().is_empty() {
        return program.process_name.trim().to_string();
    }

    Path::new(&program.path)
        .file_stem()
        .and_then(|value| value.to_str())
        .unwrap_or_default()
        .to_string()
}

async fn discover_station_process_ids(
    client: &mut StationControlClient<Channel>,
    station: &Station,
) -> Result<Vec<i32>, String> {
    let candidates = process_name_candidates(station);
    if candidates.is_empty() {
        return Err("No station process names are configured to close.".into());
    }

    let response = client
        .get_all_process_info(Empty {})
        .await
        .map_err(|error| format!("get_all_process_info RPC failed: {error}"))?;

    let process_ids = match_process_ids(&response.into_inner(), &candidates);
    if process_ids.is_empty() {
        return Err("No configured station processes are currently running.".into());
    }

    Ok(process_ids)
}

fn process_name_candidates(station: &Station) -> Vec<String> {
    let mut names = BTreeSet::new();

    for program in &station.start_programs {
        let name = effective_process_name(program);
        if !name.trim().is_empty() {
            names.insert(name.trim().to_ascii_lowercase());
        }
    }

    for process in &station.monitor_processes {
        let trimmed = process.trim();
        if !trimmed.is_empty() {
            names.insert(trimmed.to_ascii_lowercase());
        }
    }

    names.into_iter().collect()
}

fn match_process_ids(response: &GetAllProcessInfoResponse, candidates: &[String]) -> Vec<i32> {
    response
        .items
        .iter()
        .filter_map(|item| {
            let process_name = item.name.trim().to_ascii_lowercase();
            candidates
                .iter()
                .any(|candidate| process_matches_candidate(&process_name, candidate))
                .then_some(item.id)
        })
        .collect()
}

fn process_matches_candidate(process_name: &str, candidate: &str) -> bool {
    if process_name == candidate {
        return true;
    }

    if let Some(base) = candidate.strip_suffix(".exe") {
        process_name == base
    } else {
        process_name == format!("{candidate}.exe")
    }
}

fn summarize_app_start_results(action: &str, results: &[AppStartingResult]) -> String {
    if results.is_empty() {
        return format!("No applications were configured to {action}.");
    }

    let mut started = 0usize;
    let mut already_running = 0usize;
    let mut failed = Vec::new();

    for result in results {
        match result.control_result {
            APP_CONTROL_RESULT_STARTED => started += 1,
            APP_CONTROL_RESULT_ALREADY_RUNNING => already_running += 1,
            APP_CONTROL_RESULT_FAIL_TO_START | APP_CONTROL_RESULT_ERROR => {
                failed.push(format!(
                    "{} ({})",
                    display_process_name(result),
                    result.result
                ));
            }
            _ => {
                failed.push(format!(
                    "{} ({})",
                    display_process_name(result),
                    result.result
                ));
            }
        }
    }

    let mut parts = Vec::new();
    if started > 0 {
        parts.push(format!("{started} started"));
    }
    if already_running > 0 {
        parts.push(format!("{already_running} already running"));
    }
    if !failed.is_empty() {
        parts.push(format!("{} failed: {}", failed.len(), failed.join(", ")));
    }

    if parts.is_empty() {
        format!("No applications were configured to {action}.")
    } else {
        format!("App {action} results: {}.", parts.join("; "))
    }
}

fn summarize_close_results(results: &[i32]) -> String {
    if results.is_empty() {
        return "No running applications were selected to close.".into();
    }

    let mut closed = 0usize;
    let mut not_running = 0usize;
    let mut failed = 0usize;

    for result in results {
        match *result {
            APP_CONTROL_RESULT_CLOSED => closed += 1,
            APP_CONTROL_RESULT_NOT_RUNNING => not_running += 1,
            APP_CONTROL_RESULT_FAIL_TO_CLOSE | APP_CONTROL_RESULT_ERROR => failed += 1,
            _ => failed += 1,
        }
    }

    let mut parts = Vec::new();
    if closed > 0 {
        parts.push(format!("{closed} closed"));
    }
    if not_running > 0 {
        parts.push(format!("{not_running} not running"));
    }
    if failed > 0 {
        parts.push(format!("{failed} failed"));
    }

    format!("App close results: {}.", parts.join("; "))
}

fn display_process_name(result: &AppStartingResult) -> String {
    if !result.process_name.trim().is_empty() {
        result.process_name.clone()
    } else {
        format!("param {}", result.param_id)
    }
}

pub(crate) fn station_label(station: &Station) -> &str {
    if station.name.trim().is_empty() {
        station.id.as_str()
    } else {
        station.name.as_str()
    }
}

fn include_key_from_proto(include: TelemetryInclude) -> Result<String, String> {
    match include {
        TelemetryInclude::RuntimeBasic => Ok("runtime_basic".to_string()),
        TelemetryInclude::RuntimeSystem => Ok("runtime_system".to_string()),
        TelemetryInclude::RuntimeApps => Ok("runtime_apps".to_string()),
        TelemetryInclude::RuntimeNetwork => Ok("runtime_network".to_string()),
        TelemetryInclude::RuntimeStorage => Ok("runtime_storage".to_string()),
        TelemetryInclude::Unspecified => Err("Telemetry include cannot be unspecified".to_string()),
    }
}

fn include_proto_from_key(key: &str) -> Result<TelemetryInclude, String> {
    match key {
        "runtime_basic" => Ok(TelemetryInclude::RuntimeBasic),
        "runtime_system" => Ok(TelemetryInclude::RuntimeSystem),
        "runtime_apps" => Ok(TelemetryInclude::RuntimeApps),
        "runtime_network" => Ok(TelemetryInclude::RuntimeNetwork),
        "runtime_storage" => Ok(TelemetryInclude::RuntimeStorage),
        _ => Err(format!("Unsupported telemetry include '{key}'")),
    }
}

fn map_profiles_response(
    response: GetTelemetryProfilesResponse,
) -> Result<TelemetryProfilesResponseDto, String> {
    Ok(TelemetryProfilesResponseDto {
        schema_version: response.schema_version,
        profiles_version: response.profiles_version,
        profiles: response
            .profiles
            .into_iter()
            .map(map_profile)
            .collect::<Result<Vec<_>, _>>()?,
    })
}

fn map_profile(profile: TelemetryProfile) -> Result<TelemetryProfileDto, String> {
    let includes = profile
        .includes
        .into_iter()
        .map(|value| {
            let include =
                TelemetryInclude::try_from(value).unwrap_or(TelemetryInclude::Unspecified);
            include_key_from_proto(include)
        })
        .collect::<Result<Vec<_>, _>>()?;

    Ok(TelemetryProfileDto {
        id: profile.id,
        name: profile.name,
        enabled: profile.enabled,
        collection_interval_ms: profile.collection_interval_ms,
        includes,
    })
}

fn map_schema_response(
    response: GetCurrentTelemetrySchemaResponse,
) -> Result<TelemetrySchemaResponseDto, String> {
    Ok(TelemetrySchemaResponseDto {
        schema_version: response.schema_version,
        supported_includes: response
            .supported_includes
            .into_iter()
            .map(|item| {
                let include = TelemetryInclude::try_from(item.include)
                    .unwrap_or(TelemetryInclude::Unspecified);
                include_key_from_proto(include).map(|key| TelemetryIncludeDefinitionDto {
                    key,
                    label: item.label,
                    description: item.description,
                })
            })
            .collect::<Result<Vec<_>, _>>()?,
    })
}

/// Set the telemetry gathering interval on a station via gRPC.
pub async fn set_station_gathering_interval(
    station: &Station,
    interval_seconds: i32,
) -> Result<(), String> {
    let (mut client, endpoint) = connect_station(station).await?;
    client
        .set_state_gathering_interval(SetStateGatheringIntervalRequest { interval_seconds })
        .await
        .map_err(|e| format!("set_state_gathering_interval RPC via {endpoint}: {e}"))?;
    Ok(())
}

pub async fn get_station_telemetry_profiles(
    station: &Station,
) -> Result<TelemetryProfilesResponseDto, String> {
    let (mut client, endpoint) = connect_station(station).await?;
    let response = client
        .get_telemetry_profiles(Empty {})
        .await
        .map_err(|error| format!("get_telemetry_profiles RPC via {endpoint}: {error}"))?
        .into_inner();
    map_profiles_response(response)
}

pub async fn replace_station_telemetry_profiles(
    station: &Station,
    profiles: Vec<TelemetryProfileDto>,
) -> Result<TelemetryProfilesResponseDto, String> {
    let (mut client, endpoint) = connect_station(station).await?;
    let profiles = profiles
        .into_iter()
        .map(|profile| {
            let includes = profile
                .includes
                .iter()
                .map(|key| include_proto_from_key(key))
                .collect::<Result<Vec<_>, _>>()?;
            Ok(TelemetryProfile {
                id: profile.id,
                name: profile.name,
                enabled: profile.enabled,
                collection_interval_ms: profile.collection_interval_ms,
                includes: includes.into_iter().map(|include| include as i32).collect(),
            })
        })
        .collect::<Result<Vec<_>, String>>()?;

    let response = client
        .replace_telemetry_profiles(ReplaceTelemetryProfilesRequest { profiles })
        .await
        .map_err(|error| format!("replace_telemetry_profiles RPC via {endpoint}: {error}"))?
        .into_inner();
    map_profiles_response(response)
}

pub async fn get_station_telemetry_schema(
    station: &Station,
) -> Result<TelemetrySchemaResponseDto, String> {
    let (mut client, endpoint) = connect_station(station).await?;
    let response = client
        .get_current_telemetry_schema(Empty {})
        .await
        .map_err(|error| format!("get_current_telemetry_schema RPC via {endpoint}: {error}"))?
        .into_inner();
    map_schema_response(response)
}
