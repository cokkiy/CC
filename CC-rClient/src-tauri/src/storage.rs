use crate::models::{
    AppSnapshot, ClientOptions, NetworkInterface, PersistedState, StartProgram, Station,
};
use quick_xml::escape::escape;
use roxmltree::Document;
use std::collections::BTreeMap;
use std::fmt::{Display, Formatter};
use std::fs;
use std::net::{TcpStream, ToSocketAddrs};
use std::path::{Path, PathBuf};
use std::time::Duration;
use uuid::Uuid;

const LOCAL_STATION_ID: &str = "local-rstationservice";
const LOCAL_STATION_NAME: &str = "Local CC-rStationService";
const LOCAL_STATION_ENDPOINTS: [&str; 2] = ["127.0.0.1:50051", "localhost:50051"];

#[derive(Debug)]
pub enum StorageError {
    Io(std::io::Error),
    Json(serde_json::Error),
    Xml(roxmltree::Error),
    InvalidHomeDir,
    MissingParent(PathBuf),
}

impl Display for StorageError {
    fn fmt(&self, f: &mut Formatter<'_>) -> std::fmt::Result {
        match self {
            Self::Io(error) => write!(f, "{error}"),
            Self::Json(error) => write!(f, "{error}"),
            Self::Xml(error) => write!(f, "{error}"),
            Self::InvalidHomeDir => write!(f, "Unable to resolve the user home directory."),
            Self::MissingParent(path) => {
                write!(
                    f,
                    "Unable to determine a parent directory for {}.",
                    path.display()
                )
            }
        }
    }
}

impl std::error::Error for StorageError {}

impl From<std::io::Error> for StorageError {
    fn from(value: std::io::Error) -> Self {
        Self::Io(value)
    }
}

impl From<serde_json::Error> for StorageError {
    fn from(value: serde_json::Error) -> Self {
        Self::Json(value)
    }
}

impl From<roxmltree::Error> for StorageError {
    fn from(value: roxmltree::Error) -> Self {
        Self::Xml(value)
    }
}

pub struct StateStore;

impl StateStore {
    pub fn load_snapshot() -> Result<AppSnapshot, StorageError> {
        let storage_path = state_file_path()?;
        ensure_parent_dir(&storage_path)?;

        if storage_path.exists() {
            let data = fs::read_to_string(&storage_path)?;
            let mut payload = normalize_payload(serde_json::from_str::<PersistedState>(&data)?);
            maybe_inject_local_station(&mut payload, local_station_probe_available);
            let normalized_json = serde_json::to_string_pretty(&payload)?;
            if normalized_json != data {
                fs::write(&storage_path, normalized_json)?;
            }
            return Ok(AppSnapshot {
                stations: payload.stations,
                options: payload.options,
                groups: payload.groups,
                tags: payload.tags,
                storage_path: storage_path.display().to_string(),
                legacy_imported: false,
            });
        }

        let imported_payload = import_legacy_state()?;
        let legacy_imported = !imported_payload.stations.is_empty()
            || !imported_payload.options.start_apps.is_empty()
            || !imported_payload.options.monitor_processes.is_empty()
            || imported_payload.options.interval != 2
            || !imported_payload.options.is_first_time_run;

        let mut payload = normalize_payload(imported_payload);
        maybe_inject_local_station(&mut payload, local_station_probe_available);

        let snapshot = Self::save_payload(payload)?;
        Ok(AppSnapshot {
            legacy_imported,
            ..snapshot
        })
    }

    pub fn save_payload(payload: PersistedState) -> Result<AppSnapshot, StorageError> {
        let storage_path = state_file_path()?;
        ensure_parent_dir(&storage_path)?;
        let normalized = normalize_payload(payload);
        let json = serde_json::to_string_pretty(&normalized)?;
        fs::write(&storage_path, json)?;

        Ok(AppSnapshot {
            stations: normalized.stations,
            options: normalized.options,
            groups: normalized.groups,
            tags: normalized.tags,
            storage_path: storage_path.display().to_string(),
            legacy_imported: false,
        })
    }

    pub fn export_legacy_files() -> Result<String, StorageError> {
        let snapshot = Self::load_snapshot()?;
        let xml_path = legacy_xml_path()?;
        let ini_path = legacy_ini_path()?;
        ensure_parent_dir(&xml_path)?;
        ensure_parent_dir(&ini_path)?;

        fs::write(&xml_path, export_legacy_station_xml(&snapshot.stations))?;
        fs::write(&ini_path, export_legacy_ini(&snapshot.options))?;

        Ok(format!(
            "Exported legacy files to {} and {}.",
            xml_path.display(),
            ini_path.display()
        ))
    }
}

fn state_file_path() -> Result<PathBuf, StorageError> {
    let home = dirs::home_dir().ok_or(StorageError::InvalidHomeDir)?;
    Ok(home.join(".CC-rClient").join("state.json"))
}

fn legacy_xml_path() -> Result<PathBuf, StorageError> {
    let home = dirs::home_dir().ok_or(StorageError::InvalidHomeDir)?;
    Ok(home.join(".CC-Client").join("指显工作站信息.xml"))
}

fn legacy_ini_path() -> Result<PathBuf, StorageError> {
    let home = dirs::home_dir().ok_or(StorageError::InvalidHomeDir)?;
    Ok(home.join(".CC-Client").join("CC-Client.ini"))
}

fn ensure_parent_dir(path: &Path) -> Result<(), StorageError> {
    let Some(parent) = path.parent() else {
        return Err(StorageError::MissingParent(path.to_path_buf()));
    };
    fs::create_dir_all(parent)?;
    Ok(())
}

fn import_legacy_state() -> Result<PersistedState, StorageError> {
    let stations = load_legacy_stations()?;
    let options = load_legacy_options()?;
    Ok(PersistedState {
        stations,
        options,
        groups: Vec::new(),
        tags: Vec::new(),
    })
}

fn load_legacy_stations() -> Result<Vec<Station>, StorageError> {
    let path = legacy_xml_path()?;
    if !path.exists() {
        return Ok(Vec::new());
    }

    let content = fs::read_to_string(path)?;
    let document = Document::parse(&content)?;
    let stations = document
        .descendants()
        .filter(|node| node.has_tag_name("指显工作站"))
        .map(|node| {
            let name = child_text(&node, "名称");
            let network_interfaces = node
                .children()
                .filter(|child| child.has_tag_name("网卡"))
                .map(|ni| NetworkInterface {
                    mac: child_text(&ni, "MAC"),
                    ips: ni
                        .children()
                        .filter(|child| child.has_tag_name("IP"))
                        .filter_map(|child| child.text().map(|value| value.trim().to_string()))
                        .filter(|value| !value.is_empty())
                        .collect(),
                })
                .collect::<Vec<_>>();

            let start_programs = node
                .descendants()
                .filter(|child| child.has_tag_name("程序"))
                .filter(|program| {
                    program
                        .parent()
                        .and_then(|parent| parent.parent())
                        .is_some_and(|parent| parent.has_tag_name("指显工作站"))
                })
                .map(|program| StartProgram {
                    path: child_text(&program, "路径"),
                    arguments: child_text(&program, "参数"),
                    process_name: child_text(&program, "进程名"),
                    allow_multi_instance: child_text(&program, "允许多实例")
                        .parse::<i32>()
                        .unwrap_or_default()
                        == 1,
                })
                .collect::<Vec<_>>();

            let monitor_processes = node
                .descendants()
                .filter(|child| child.has_tag_name("进程"))
                .filter(|process| {
                    process
                        .parent()
                        .is_some_and(|parent| parent.has_tag_name("监视进程"))
                })
                .filter_map(|process| process.text().map(|value| value.trim().to_string()))
                .filter(|value| !value.is_empty())
                .collect::<Vec<_>>();

            Station {
                id: Uuid::new_v4().to_string(),
                name,
                blocked: false,
                network_interfaces,
                start_programs,
                monitor_processes,
                last_action: Some("Imported from legacy XML".into()),
                groups: Vec::new(),
                tags: std::collections::HashMap::new(),
                metadata: std::collections::HashMap::new(),
                location: None,
            }
        })
        .collect::<Vec<_>>();

    Ok(stations)
}

fn child_text(node: &roxmltree::Node<'_, '_>, tag_name: &str) -> String {
    node.children()
        .find(|child| child.has_tag_name(tag_name))
        .and_then(|child| child.text())
        .map(|value| value.trim().to_string())
        .unwrap_or_default()
}

fn load_legacy_options() -> Result<ClientOptions, StorageError> {
    let path = legacy_ini_path()?;
    if !path.exists() {
        return Ok(ClientOptions::default());
    }

    let content = fs::read_to_string(path)?;
    let sections = parse_ini_sections(&content);

    let interval = section_value(&sections, "", "Interval")
        .or_else(|| section_value(&sections, "General", "Interval"))
        .and_then(|value| value.parse::<i32>().ok())
        .unwrap_or(2);

    let is_first_time_run = section_value(&sections, "", "IsFirstTimeRun")
        .or_else(|| section_value(&sections, "General", "IsFirstTimeRun"))
        .map(parse_bool)
        .unwrap_or(true);

    let start_apps = section_value(&sections, "ControlAndMonitor", "StartApps")
        .map(parse_start_apps)
        .unwrap_or_default();

    let monitor_processes = section_value(&sections, "ControlAndMonitor", "MonitorProc")
        .map(parse_pipe_list)
        .unwrap_or_default();

    Ok(ClientOptions {
        interval,
        is_first_time_run,
        start_apps,
        monitor_processes,
    })
}

fn parse_ini_sections(content: &str) -> BTreeMap<String, BTreeMap<String, String>> {
    let mut sections = BTreeMap::<String, BTreeMap<String, String>>::new();
    let mut current = String::new();

    for raw_line in content.lines() {
        let line = raw_line.trim();
        if line.is_empty() || line.starts_with(';') || line.starts_with('#') {
            continue;
        }
        if line.starts_with('[') && line.ends_with(']') {
            current = line[1..line.len() - 1].trim().to_string();
            sections.entry(current.clone()).or_default();
            continue;
        }
        if let Some((key, value)) = line.split_once('=') {
            sections
                .entry(current.clone())
                .or_default()
                .insert(key.trim().to_string(), value.trim().to_string());
        }
    }

    sections
}

fn section_value(
    sections: &BTreeMap<String, BTreeMap<String, String>>,
    section: &str,
    key: &str,
) -> Option<String> {
    sections
        .get(section)
        .and_then(|values| values.get(key))
        .map(ToOwned::to_owned)
}

fn parse_bool(value: String) -> bool {
    matches!(
        value.trim().to_ascii_lowercase().as_str(),
        "1" | "true" | "yes"
    )
}

fn parse_pipe_list(value: String) -> Vec<String> {
    value
        .split('|')
        .map(str::trim)
        .filter(|part| !part.is_empty())
        .map(ToOwned::to_owned)
        .collect()
}

fn parse_start_apps(value: String) -> Vec<StartProgram> {
    parse_pipe_list(value)
        .into_iter()
        .filter_map(|item| {
            let parts = item
                .split(" && ")
                .map(str::to_string)
                .collect::<Vec<String>>();
            if parts.len() < 4 {
                return None;
            }
            Some(StartProgram {
                path: parts[0].clone(),
                arguments: parts[1].clone(),
                process_name: parts[2].clone(),
                allow_multi_instance: parts[3].parse::<i32>().unwrap_or_default() == 1,
            })
        })
        .collect()
}

#[allow(dead_code)]
pub fn export_legacy_station_xml(stations: &[Station]) -> String {
    let mut xml = String::from("<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>\n");
    xml.push_str("<指显工作站信息>\n");
    for station in stations {
        xml.push_str("  <指显工作站>\n");
        xml.push_str(&format!("    <名称>{}</名称>\n", escape(&station.name)));
        for ni in &station.network_interfaces {
            xml.push_str("    <网卡>\n");
            xml.push_str(&format!("      <MAC>{}</MAC>\n", escape(&ni.mac)));
            for ip in &ni.ips {
                xml.push_str(&format!("      <IP>{}</IP>\n", escape(ip)));
            }
            xml.push_str("    </网卡>\n");
        }
        xml.push_str("    <启动程序>\n");
        for program in &station.start_programs {
            xml.push_str("      <程序>\n");
            xml.push_str(&format!("        <路径>{}</路径>\n", escape(&program.path)));
            xml.push_str(&format!(
                "        <参数>{}</参数>\n",
                escape(&program.arguments)
            ));
            xml.push_str(&format!(
                "        <进程名>{}</进程名>\n",
                escape(&program.process_name)
            ));
            xml.push_str(&format!(
                "        <允许多实例>{}</允许多实例>\n",
                if program.allow_multi_instance { 1 } else { 0 }
            ));
            xml.push_str("      </程序>\n");
        }
        xml.push_str("    </启动程序>\n");
        xml.push_str("    <监视进程>\n");
        for process in &station.monitor_processes {
            xml.push_str(&format!("      <进程>{}</进程>\n", escape(process)));
        }
        xml.push_str("    </监视进程>\n");
        xml.push_str("  </指显工作站>\n");
    }
    xml.push_str("</指显工作站信息>\n");
    xml
}

pub fn export_legacy_ini(options: &ClientOptions) -> String {
    let start_apps = options
        .start_apps
        .iter()
        .map(|app| {
            format!(
                "{} && {} && {} && {}",
                app.path,
                app.arguments,
                app.process_name,
                if app.allow_multi_instance { 1 } else { 0 }
            )
        })
        .collect::<Vec<_>>()
        .join("|");

    let monitor_processes = options.monitor_processes.join("|");

    format!(
    "Interval={interval}\nIsFirstTimeRun={first_run}\n[ControlAndMonitor]\nStartApps={start_apps}\nMonitorProc={monitor_processes}\n",
    interval = options.interval,
    first_run = if options.is_first_time_run { "true" } else { "false" },
    start_apps = start_apps,
    monitor_processes = monitor_processes,
  )
}

fn normalize_payload(mut payload: PersistedState) -> PersistedState {
    for station in &mut payload.stations {
        if station.id.trim().is_empty() {
            station.id = Uuid::new_v4().to_string();
        }
        station.network_interfaces.retain(|ni| {
            !ni.mac.trim().is_empty() || ni.ips.iter().any(|ip| !ip.trim().is_empty())
        });
        for ni in &mut station.network_interfaces {
            ni.ips = ni
                .ips
                .iter()
                .map(|ip| ip.trim().to_string())
                .filter(|ip| !ip.is_empty())
                .collect();
        }
        station.monitor_processes = station
            .monitor_processes
            .iter()
            .map(|value| value.trim().to_string())
            .filter(|value| !value.is_empty())
            .collect();
        station.start_programs.retain(|program| {
            !program.path.trim().is_empty() || !program.process_name.trim().is_empty()
        });
    }
    payload
}

fn maybe_inject_local_station<F>(payload: &mut PersistedState, probe_available: F) -> bool
where
    F: Fn() -> bool,
{
    if payload.stations.iter().any(station_matches_localhost) {
        return false;
    }

    if !probe_available() {
        return false;
    }

    payload.stations.insert(0, build_local_station());
    true
}

fn build_local_station() -> Station {
    Station {
        id: LOCAL_STATION_ID.to_string(),
        name: LOCAL_STATION_NAME.to_string(),
        blocked: false,
        network_interfaces: vec![NetworkInterface {
            mac: "loopback".to_string(),
            ips: LOCAL_STATION_ENDPOINTS
                .iter()
                .map(|value| value.to_string())
                .collect(),
        }],
        start_programs: Vec::new(),
        monitor_processes: Vec::new(),
        last_action: Some("Auto-discovered local station endpoint".to_string()),
        groups: Vec::new(),
        tags: std::collections::HashMap::new(),
        metadata: std::collections::HashMap::new(),
        location: None,
    }
}

fn local_station_probe_available() -> bool {
    let timeout = Duration::from_millis(250);
    LOCAL_STATION_ENDPOINTS
        .iter()
        .filter_map(|endpoint| endpoint.split_once(':'))
        .filter_map(|(host, port_text)| {
            port_text
                .parse::<u16>()
                .ok()
                .map(|port| (host.to_string(), port))
        })
        .any(|(host, port)| can_connect_to_host_port(&host, port, timeout))
}

fn can_connect_to_host_port(host: &str, port: u16, timeout: Duration) -> bool {
    let Ok(addrs) = (host, port).to_socket_addrs() else {
        return false;
    };

    addrs
        .into_iter()
        .any(|addr| TcpStream::connect_timeout(&addr, timeout).is_ok())
}

fn station_matches_localhost(station: &Station) -> bool {
    if station.id.trim().eq_ignore_ascii_case(LOCAL_STATION_ID) {
        return true;
    }

    station
        .network_interfaces
        .iter()
        .flat_map(|iface| iface.ips.iter())
        .any(|ip| ip_matches_localhost(ip))
}

fn ip_matches_localhost(raw: &str) -> bool {
    let trimmed = raw.trim().to_ascii_lowercase();
    if trimmed.is_empty() {
        return false;
    }

    let without_scheme = trimmed
        .strip_prefix("http://")
        .or_else(|| trimmed.strip_prefix("https://"))
        .unwrap_or(trimmed.as_str());

    let authority = without_scheme
        .split('/')
        .next()
        .unwrap_or(without_scheme);

    if let Some(stripped) = authority.strip_prefix('[') {
        let host = stripped.split(']').next().unwrap_or_default();
        return host == "localhost" || host == "127.0.0.1";
    }

    let host = authority.split(':').next().unwrap_or(authority);
    host == "localhost" || host == "127.0.0.1"
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn parses_legacy_start_apps_and_processes() {
        let ini = r#"
Interval=5
IsFirstTimeRun=false
[ControlAndMonitor]
StartApps=/opt/a && --flag && proc-a && 1|/opt/b &&  && proc-b && 0|
MonitorProc=proc-x|proc-y|
[WeatherImageDownloadOption]
Download=2
Url=http://example.com
UserName=u
Password=p
LastHours=8
Interval=3
DeletePreviousFiles=true
DeleteHowHoursAgo=24
SubDirectory=weather
SavePathForLinux=/tmp/weather
SavePathForWindows=C:\weather
"#;

        let sections = parse_ini_sections(ini);
        let options = ClientOptions {
            interval: section_value(&sections, "", "Interval")
                .unwrap()
                .parse()
                .unwrap(),
            is_first_time_run: parse_bool(section_value(&sections, "", "IsFirstTimeRun").unwrap()),
            start_apps: parse_start_apps(
                section_value(&sections, "ControlAndMonitor", "StartApps").unwrap(),
            ),
            monitor_processes: parse_pipe_list(
                section_value(&sections, "ControlAndMonitor", "MonitorProc").unwrap(),
            ),
        };

        assert_eq!(options.interval, 5);
        assert!(!options.is_first_time_run);
        assert_eq!(options.start_apps.len(), 2);
        assert_eq!(options.start_apps[0].process_name, "proc-a");
        assert_eq!(options.monitor_processes, vec!["proc-x", "proc-y"]);
    }

    #[test]
    fn exports_legacy_station_xml_with_expected_tags() {
        let xml = export_legacy_station_xml(&[Station {
            id: "1".into(),
            name: "Station A".into(),
            blocked: false,
            network_interfaces: vec![NetworkInterface {
                mac: "aa:bb:cc:dd:ee:ff".into(),
                ips: vec!["10.0.0.1".into()],
            }],
            start_programs: vec![StartProgram {
                path: "/opt/app".into(),
                arguments: "--test".into(),
                process_name: "app".into(),
                allow_multi_instance: false,
            }],
            monitor_processes: vec!["proc1".into()],
            last_action: None,
            groups: Vec::new(),
            tags: std::collections::HashMap::new(),
            metadata: std::collections::HashMap::new(),
            location: None,
        }]);

        assert!(xml.contains("<指显工作站信息>"));
        assert!(xml.contains("<名称>Station A</名称>"));
        assert!(xml.contains("<MAC>aa:bb:cc:dd:ee:ff</MAC>"));
        assert!(xml.contains("<IP>10.0.0.1</IP>"));
        assert!(xml.contains("<进程>proc1</进程>"));
    }

    #[test]
    fn exports_legacy_ini_with_expected_keys() {
        let ini = export_legacy_ini(&ClientOptions {
            interval: 7,
            is_first_time_run: false,
            start_apps: vec![StartProgram {
                path: "/opt/app".into(),
                arguments: "--x".into(),
                process_name: "proc".into(),
                allow_multi_instance: true,
            }],
            monitor_processes: vec!["alpha".into(), "beta".into()],
        });

        assert!(ini.contains("Interval=7"));
        assert!(ini.contains("[ControlAndMonitor]"));
        assert!(ini.contains("StartApps=/opt/app && --x && proc && 1"));
        assert!(ini.contains("MonitorProc=alpha|beta"));
    }

    #[test]
    fn deserializes_legacy_state_without_groups_and_normalizes_ips() {
        let json = r#"
{
  "stations": [
    {
      "id": "local-rstationservice",
      "name": "Local CC-rStationService",
      "networkInterfaces": [
        {
          "mac": "",
          "ips": [" 127.0.0.1:50051 ", ""]
        }
      ],
      "monitorProcesses": ["vite", " ", "cc-rstationservice"]
    }
  ],
  "options": {
    "interval": 2,
    "isFirstTimeRun": false,
    "monitorProcesses": ["vite", "cc-rclient", "cc-rstationservice"],
    "weatherImageDownloadOption": {
      "download": 2
    }
  }
}
"#;

        let payload = normalize_payload(serde_json::from_str::<PersistedState>(json).unwrap());

        assert!(payload.groups.is_empty());
        assert_eq!(payload.stations.len(), 1);
        assert_eq!(
            payload.stations[0].network_interfaces[0].ips,
            vec!["127.0.0.1:50051"]
        );
        assert_eq!(
            payload.stations[0].monitor_processes,
            vec!["vite", "cc-rstationservice"]
        );
    }

    #[test]
    fn injects_local_station_when_probe_is_available() {
        let mut payload = PersistedState::default();

        let inserted = maybe_inject_local_station(&mut payload, || true);

        assert!(inserted);
        assert_eq!(payload.stations.len(), 1);
        assert_eq!(payload.stations[0].id, LOCAL_STATION_ID);
        assert!(payload.stations[0]
            .network_interfaces
            .iter()
            .flat_map(|iface| iface.ips.iter())
            .any(|ip| ip == "127.0.0.1:50051"));
    }

    #[test]
    fn skips_local_station_injection_when_existing_station_matches_localhost() {
        let mut payload = PersistedState {
            stations: vec![Station {
                id: "custom-station".to_string(),
                name: "Custom".to_string(),
                blocked: false,
                network_interfaces: vec![NetworkInterface {
                    mac: "".to_string(),
                    ips: vec!["http://localhost:50051".to_string()],
                }],
                start_programs: Vec::new(),
                monitor_processes: Vec::new(),
                last_action: None,
                groups: Vec::new(),
                tags: std::collections::HashMap::new(),
                metadata: std::collections::HashMap::new(),
                location: None,
            }],
            ..PersistedState::default()
        };

        let inserted = maybe_inject_local_station(&mut payload, || true);

        assert!(!inserted);
        assert_eq!(payload.stations.len(), 1);
        assert_eq!(payload.stations[0].id, "custom-station");
    }
}
