use serde::{Deserialize, Serialize};

#[derive(Debug, Clone, Serialize, Deserialize)]
#[serde(rename_all = "camelCase")]
pub struct NetworkInterface {
    pub mac: String,
    pub ips: Vec<String>,
}

#[derive(Debug, Clone, Serialize, Deserialize)]
#[serde(rename_all = "camelCase")]
pub struct StartProgram {
    pub path: String,
    pub arguments: String,
    pub process_name: String,
    pub allow_multi_instance: bool,
}

#[derive(Debug, Clone, Serialize, Deserialize)]
#[serde(rename_all = "camelCase")]
pub struct Station {
    pub id: String,
    pub name: String,
    pub blocked: bool,
    pub network_interfaces: Vec<NetworkInterface>,
    pub start_programs: Vec<StartProgram>,
    pub monitor_processes: Vec<String>,
    pub last_action: Option<String>,
}

#[derive(Debug, Clone, Serialize, Deserialize)]
#[serde(rename_all = "camelCase")]
pub struct ClientOptions {
    pub interval: i32,
    pub is_first_time_run: bool,
    pub start_apps: Vec<StartProgram>,
    pub monitor_processes: Vec<String>,
}

impl Default for ClientOptions {
    fn default() -> Self {
        Self {
            interval: 2,
            is_first_time_run: true,
            start_apps: Vec::new(),
            monitor_processes: Vec::new(),
        }
    }
}

#[derive(Debug, Clone, Serialize, Deserialize)]
#[serde(rename_all = "camelCase")]
pub struct PersistedState {
    pub stations: Vec<Station>,
    pub options: ClientOptions,
    pub groups: Vec<StationGroup>,
}

impl Default for PersistedState {
    fn default() -> Self {
        Self {
            stations: Vec::new(),
            options: ClientOptions::default(),
            groups: Vec::new(),
        }
    }
}

#[derive(Debug, Clone, Serialize, Deserialize)]
#[serde(rename_all = "camelCase")]
pub struct AppSnapshot {
    pub stations: Vec<Station>,
    pub options: ClientOptions,
    pub groups: Vec<StationGroup>,
    pub storage_path: String,
    pub legacy_imported: bool,
}

#[derive(Debug, Clone, Serialize, Deserialize)]
#[serde(rename_all = "snake_case")]
pub enum StationAction {
    PowerOn,
    Block,
    Unblock,
    StartApp,
    RestartApp,
    ExitApp,
    Shutdown,
    Reboot,
    FullScreen,
    RealTime,
    PrevPage,
    NextPage,
    ClearPage,
    BatchPowerOn,
    BatchShutdown,
    BatchReboot,
}

impl StationAction {
    pub fn label(&self) -> &'static str {
        match self {
            Self::PowerOn => "Power On",
            Self::Block => "Block",
            Self::Unblock => "Unblock",
            Self::StartApp => "Start App",
            Self::RestartApp => "Restart App",
            Self::ExitApp => "Exit App",
            Self::Shutdown => "Shutdown",
            Self::Reboot => "Reboot",
            Self::FullScreen => "Full Screen",
            Self::RealTime => "Real Time",
            Self::PrevPage => "Previous Page",
            Self::NextPage => "Next Page",
            Self::ClearPage => "Clear Page",
            Self::BatchPowerOn => "Batch Power On",
            Self::BatchShutdown => "Batch Shutdown",
            Self::BatchReboot => "Batch Reboot",
        }
    }
}

#[derive(Debug, Clone, Serialize, Deserialize)]
#[serde(rename_all = "camelCase")]
pub struct ActionResult {
    pub ok: bool,
    pub message: String,
    pub stations: Option<Vec<Station>>,
    pub implemented: bool,
}

#[derive(Debug, Clone, Serialize, Deserialize)]
#[serde(rename_all = "camelCase")]
pub struct StationGroup {
    pub id: String,
    pub name: String,
    pub description: String,
    pub tags: Vec<String>,
    pub station_ids: Vec<String>,
}

impl StationGroup {
    pub fn new(name: &str) -> Self {
        Self {
            id: uuid::Uuid::new_v4().to_string(),
            name: name.to_string(),
            description: String::new(),
            tags: Vec::new(),
            station_ids: Vec::new(),
        }
    }
}
