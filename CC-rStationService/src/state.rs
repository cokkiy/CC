use std::collections::HashMap;
use std::net::SocketAddr;
use std::path::{Path, PathBuf};
use std::sync::atomic::{AtomicU64, Ordering};
use std::sync::{Mutex, RwLock};
use std::thread;
use std::time::Duration;

use anyhow::{Context, Result};
use chrono::{DateTime, Local};
use netstat2::{AddressFamilyFlags, ProtocolFlags, ProtocolSocketInfo, TcpState, get_sockets_info};
use pnet_datalink::interfaces;
#[cfg(windows)]
use sysinfo::Disks;
use sysinfo::{MINIMUM_CPU_UPDATE_INTERVAL, Networks, Pid, ProcessesToUpdate, System};

use crate::config::AppConfig;
use crate::grpc::cc::{
    AppRunningState, AppsRunningStateEnvelope, ConnectionInformation, GetAllProcessInfoResponse,
    GetConnectionInformationsResponse, GetFileInfoResponse, GetNetworkInterfacesResponse,
    GetTcpListenerInfosResponse, GetUdpListenerInfosResponse, InterfaceStatistics, ListenerInfo,
    NetworkInterfaceInfo, Process, ProcessInfo, ServerVersionInfo, StationNetworkInterface,
    StationRunningState, StationSystemState, Statistics, VersionInfo,
};
use crate::mqtt::MqttClient;
use crate::network_counters;
use tracing::debug;

pub struct AppState {
    config: AppConfig,
    station_id: String,
    service_path: PathBuf,
    udp_target: SocketAddr,
    agent_target: SocketAddr,
    watched_processes: RwLock<Vec<String>>,
    interval_seconds: AtomicU64,
    network_sampler: Mutex<NetworkSampler>,
    server_version: ServerVersionInfo,
    mqtt_client: Option<MqttClient>,
}

impl AppState {
    pub fn new(config: AppConfig, service_path: PathBuf) -> Result<Self> {
        let station_id = config.resolved_station_id();
        let udp_target = config
            .service
            .udp_display_target
            .parse::<SocketAddr>()
            .with_context(|| format!("parse UDP target {}", config.service.udp_display_target))?;
        let agent_target = config
            .agent
            .listen_addr
            .parse::<SocketAddr>()
            .with_context(|| format!("parse agent target {}", config.agent.listen_addr))?;

        // Initialize MQTT client if enabled
        let mqtt_client = if config.mqtt.enabled {
            match MqttClient::new(
                &config.mqtt.broker_host,
                config.mqtt.broker_port,
                &station_id,
            ) {
                Ok(client) => {
                    tracing::info!("MQTT client initialized for station: {}", station_id);
                    Some(client)
                }
                Err(e) => {
                    tracing::error!("Failed to initialize MQTT client: {:?}", e);
                    None
                }
            }
        } else {
            None
        };

        Ok(Self {
            watched_processes: RwLock::new(config.service.watched_processes.clone()),
            interval_seconds: AtomicU64::new(config.service.state_interval_seconds.max(1)),
            network_sampler: Mutex::new(NetworkSampler::new()),
            server_version: build_server_version(),
            config,
            station_id,
            service_path,
            udp_target,
            agent_target,
            mqtt_client,
        })
    }

    pub fn station_id(&self) -> &str {
        &self.station_id
    }

    pub fn listen_addr(&self) -> Result<SocketAddr> {
        self.config
            .control
            .listen_addr
            .parse::<SocketAddr>()
            .with_context(|| format!("parse listen addr {}", self.config.control.listen_addr))
    }

    pub fn udp_target(&self) -> SocketAddr {
        self.udp_target
    }

    pub fn agent_target(&self) -> SocketAddr {
        self.agent_target
    }

    pub fn service_path(&self) -> &Path {
        &self.service_path
    }

    pub fn launcher_proxy_path(&self) -> &str {
        &self.config.service.launcher_proxy_path
    }

    pub fn agent_auth_token(&self) -> &str {
        &self.config.agent.auth_token
    }

    pub fn preferred_display_index(&self) -> u32 {
        self.config.agent.preferred_display_index
    }

    pub fn interval_seconds(&self) -> u64 {
        self.interval_seconds.load(Ordering::Relaxed).max(1)
    }

    pub fn set_interval_seconds(&self, seconds: u64) {
        self.interval_seconds
            .store(seconds.max(1), Ordering::Relaxed);
    }

    pub fn watched_processes(&self) -> Vec<String> {
        self.watched_processes
            .read()
            .map(|guard| guard.clone())
            .unwrap_or_default()
    }

    pub fn set_watched_processes(&self, processes: Vec<String>) {
        if let Ok(mut guard) = self.watched_processes.write() {
            *guard = processes;
        }
    }

    pub fn server_version(&self) -> ServerVersionInfo {
        self.server_version.clone()
    }

    pub fn system_state(&self) -> StationSystemState {
        let system = prepared_system_snapshot();

        StationSystemState {
            station_id: self.station_id.clone(),
            total_memory: saturating_i64(system.total_memory()),
            computer_name: System::host_name().unwrap_or_default(),
            network_interfaces: collect_station_network_interfaces(),
            os_name: System::name().unwrap_or_default(),
            os_version: System::os_version().unwrap_or_default(),
        }
    }

    pub fn running_state(&self) -> StationRunningState {
        let system = prepared_system_snapshot();

        StationRunningState {
            station_id: self.station_id.clone(),
            current_memory: saturating_i64(system.used_memory()),
            cpu: system.global_cpu_usage(),
            proc_count: system.processes().len() as i32,
            version: Some(self.server_version()),
        }
    }

    pub fn apps_running_state(&self) -> AppsRunningStateEnvelope {
        let watched = self.watched_processes();
        let system = prepared_system_snapshot();

        let items = watched
            .iter()
            .map(|name| collect_app_running_state(&system, &self.station_id, name))
            .collect();

        AppsRunningStateEnvelope { items }
    }

    /// Single async snapshot for both running_state and apps_running_state — avoids
    /// two separate blocking sleeps per telemetry cycle.
    pub async fn running_and_apps_state(
        &self,
    ) -> (StationRunningState, AppsRunningStateEnvelope) {
        let station_id = self.station_id.clone();
        let watched = self.watched_processes();
        let server_version = self.server_version();

        let system = prepared_system_snapshot_async().await;

        let running = StationRunningState {
            station_id: station_id.clone(),
            current_memory: saturating_i64(system.used_memory()),
            cpu: system.global_cpu_usage(),
            proc_count: system.processes().len() as i32,
            version: Some(server_version),
        };

        let items = watched
            .iter()
            .map(|name| collect_app_running_state(&system, &station_id, name))
            .collect();
        let apps = AppsRunningStateEnvelope { items };

        // Publish telemetry via MQTT if enabled
        if let Some(mqtt_client) = &self.mqtt_client {
            if self.config.mqtt.telemetry_enabled {
                let interval_ms = (self.interval_seconds.load(Ordering::Relaxed) * 1000) as u32;
                let telemetry_bundle = crate::mqtt::TelemetryBundle::from_station_state(&running, interval_ms);
                
                // Spawn async task to publish telemetry
                let client = mqtt_client.clone();
                let bundle = telemetry_bundle.clone();
                tokio::spawn(async move {
                    if let Err(e) = client.publish_telemetry(&bundle).await {
                        tracing::error!("Failed to publish telemetry via MQTT: {:?}", e);
                    }
                });
            }
        }

        (running, apps)
    }

    pub fn all_process_info(&self) -> GetAllProcessInfoResponse {
        let system = prepared_system_snapshot();

        let mut items = system
            .processes()
            .iter()
            .map(|(pid, process)| ProcessInfo {
                id: pid.as_u32() as i32,
                name: process.name().to_string_lossy().into_owned(),
                time: format!("{}s", process.run_time()),
                memory_size: saturating_i64(process.memory()),
            })
            .collect::<Vec<_>>();

        items.sort_by_key(|item| item.id);
        GetAllProcessInfoResponse { items }
    }

    pub fn file_info(&self, path: &str) -> Result<GetFileInfoResponse> {
        let path = resolve_browse_path(path);

        #[cfg(windows)]
        if path.as_os_str().is_empty()
            || path == PathBuf::from("/")
            || path == PathBuf::from("/home")
        {
            let disks = Disks::new_with_refreshed_list();
            let items = disks
                .iter()
                .map(|disk| crate::grpc::cc::FileInfo {
                    parent: "/".to_string(),
                    path: disk.mount_point().display().to_string(),
                    is_directory: true,
                    creation_time: String::new(),
                    last_modify: String::new(),
                })
                .collect();

            return Ok(GetFileInfoResponse { items });
        }

        if !path.exists() {
            anyhow::bail!("{} does not exist", path.display());
        }

        let read_dir = std::fs::read_dir(&path)
            .with_context(|| format!("read directory {}", path.display()))?;

        let mut items = Vec::new();
        for entry in read_dir {
            let entry = entry.with_context(|| format!("iterate directory {}", path.display()))?;
            let metadata = entry
                .metadata()
                .with_context(|| format!("read metadata {}", entry.path().display()))?;

            items.push(crate::grpc::cc::FileInfo {
                parent: path.display().to_string(),
                path: entry.file_name().to_string_lossy().into_owned(),
                is_directory: metadata.is_dir(),
                creation_time: format_system_time(metadata.created().ok()),
                last_modify: format_system_time(metadata.modified().ok()),
            });
        }

        items.sort_by(|left, right| left.path.cmp(&right.path));
        Ok(GetFileInfoResponse { items })
    }

    pub fn network_interfaces(&self) -> GetNetworkInterfacesResponse {
        GetNetworkInterfacesResponse {
            items: interfaces()
                .into_iter()
                .map(|iface| NetworkInterfaceInfo {
                    id: iface.name.clone(),
                    name: iface.name.clone(),
                    description: iface.name.clone(),
                    is_up: iface.is_up(),
                    dns_suffix: String::new(),
                    dns_enabled: false,
                    dynamic_dns: false,
                    ip_addresses: iface.ips.iter().map(|ip| ip.ip().to_string()).collect(),
                    multicast_addresses: Vec::new(),
                    dhcp_servers: Vec::new(),
                })
                .collect(),
        }
    }

    pub fn connection_infos(&self) -> Result<GetConnectionInformationsResponse> {
        let sockets = get_sockets_info(
            AddressFamilyFlags::IPV4 | AddressFamilyFlags::IPV6,
            ProtocolFlags::TCP,
        )
        .context("query TCP connections")?;

        let items = sockets
            .into_iter()
            .filter_map(|socket| match socket.protocol_socket_info {
                ProtocolSocketInfo::Tcp(tcp) => Some(ConnectionInformation {
                    local_address: tcp.local_addr.to_string(),
                    local_port: tcp.local_port as i32,
                    remote: tcp.remote_addr.to_string(),
                    remote_port: tcp.remote_port as i32,
                    state: format!("{:?}", tcp.state),
                }),
                ProtocolSocketInfo::Udp(_) => None,
            })
            .collect();

        Ok(GetConnectionInformationsResponse { items })
    }

    pub fn tcp_listener_infos(&self) -> Result<GetTcpListenerInfosResponse> {
        let sockets = get_sockets_info(
            AddressFamilyFlags::IPV4 | AddressFamilyFlags::IPV6,
            ProtocolFlags::TCP,
        )
        .context("query TCP listeners")?;

        let items = sockets
            .into_iter()
            .filter_map(|socket| match socket.protocol_socket_info {
                ProtocolSocketInfo::Tcp(tcp) if tcp.state == TcpState::Listen => {
                    Some(ListenerInfo {
                        address: tcp.local_addr.to_string(),
                        port: tcp.local_port as i32,
                    })
                }
                _ => None,
            })
            .collect();

        Ok(GetTcpListenerInfosResponse { items })
    }

    pub fn udp_listener_infos(&self) -> Result<GetUdpListenerInfosResponse> {
        let sockets = get_sockets_info(
            AddressFamilyFlags::IPV4 | AddressFamilyFlags::IPV6,
            ProtocolFlags::UDP,
        )
        .context("query UDP listeners")?;

        let items = sockets
            .into_iter()
            .filter_map(|socket| match socket.protocol_socket_info {
                ProtocolSocketInfo::Udp(udp) => Some(ListenerInfo {
                    address: udp.local_addr.to_string(),
                    port: udp.local_port as i32,
                }),
                ProtocolSocketInfo::Tcp(_) => None,
            })
            .collect();

        Ok(GetUdpListenerInfosResponse { items })
    }

    pub fn network_statistics(&self) -> Statistics {
        match self.network_sampler.lock() {
            Ok(mut sampler) => sampler.snapshot(&self.station_id, self),
            Err(_) => Statistics {
                station_id: self.station_id.clone(),
                ..Default::default()
            },
        }
    }
}

pub fn build_server_version() -> ServerVersionInfo {
    let pkg_version = env!("CARGO_PKG_VERSION");
    let mut parts = pkg_version
        .split('.')
        .filter_map(|part| part.parse::<i32>().ok());

    ServerVersionInfo {
        services_version: Some(VersionInfo {
            major: parts.next().unwrap_or_default(),
            minor: parts.next().unwrap_or_default(),
            build: parts.next().unwrap_or_default(),
            private: 0,
            product_version: pkg_version.to_string(),
        }),
        app_launcher_version: Some(VersionInfo {
            major: 0,
            minor: 0,
            build: 0,
            private: 0,
            product_version: String::new(),
        }),
    }
}

pub fn find_process_ids_by_name(name: &str) -> Vec<i32> {
    let mut system = System::new_all();
    system.refresh_all();

    let candidates = process_name_candidates(name);
    system
        .processes()
        .iter()
        .filter_map(|(pid, process)| {
            process_matches_name(process, &candidates).then_some(pid.as_u32() as i32)
        })
        .collect()
}

pub fn terminate_process(pid: i32) -> Result<bool> {
    if pid <= 0 {
        return Ok(false);
    }

    let mut system = System::new_all();
    system.refresh_all();

    let Some(process) = system.process(Pid::from_u32(pid as u32)) else {
        return Ok(false);
    };

    Ok(process.kill())
}

fn collect_station_network_interfaces() -> Vec<StationNetworkInterface> {
    interfaces()
        .into_iter()
        .filter(|iface| !iface.is_loopback())
        .map(|iface| StationNetworkInterface {
            mac: iface
                .mac
                .map(|value| value.to_string())
                .unwrap_or_else(|| iface.name.clone()),
            ips: iface.ips.iter().map(|ip| ip.ip().to_string()).collect(),
        })
        .collect()
}

fn collect_app_running_state(system: &System, station_id: &str, name: &str) -> AppRunningState {
    if let Some(id_text) = name.strip_prefix("Id:") {
        let pid = id_text.parse::<u32>().ok();
        if let Some(pid) = pid.and_then(|value| {
            system
                .process(Pid::from_u32(value))
                .map(|process| (value, process))
        }) {
            return AppRunningState {
                station_id: station_id.to_string(),
                process: Some(Process {
                    id: pid.0 as i32,
                    process_name: pid.1.name().to_string_lossy().into_owned(),
                    process_monitor_name: name.to_string(),
                }),
                is_running: true,
                cpu: pid.1.cpu_usage(),
                proc_count: 1,
                thread_count: thread_count(pid.1),
                current_memory: saturating_i64(pid.1.memory()),
                app_version: String::new(),
                start_time: saturating_i64(pid.1.start_time()),
            };
        }

        return AppRunningState {
            station_id: station_id.to_string(),
            process: Some(Process {
                id: -1,
                process_name: String::new(),
                process_monitor_name: name.to_string(),
            }),
            is_running: false,
            cpu: 0.0,
            proc_count: 0,
            thread_count: 0,
            current_memory: 0,
            app_version: String::new(),
            start_time: 0,
        };
    }

    let candidates = process_name_candidates(name);
    let mut current_memory = 0i64;
    let mut cpu = 0f32;
    let mut proc_count = 0i32;
    let mut process_id = -1;
    let mut process_name = String::new();
    let mut start_time = 0i64;
    let mut thread_count_total = 0i32;

    for (pid, process) in system.processes() {
        if process_matches_name(process, &candidates) {
            proc_count += 1;
            current_memory += saturating_i64(process.memory());
            cpu += process.cpu_usage();
            process_id = pid.as_u32() as i32;
            process_name = process.name().to_string_lossy().into_owned();
            start_time = saturating_i64(process.start_time());
            thread_count_total += process
                .tasks()
                .map(|tasks| i32::try_from(tasks.len()).unwrap_or(i32::MAX))
                .unwrap_or_default();
        }
    }

    AppRunningState {
        station_id: station_id.to_string(),
        process: Some(Process {
            id: process_id,
            process_name,
            process_monitor_name: name.to_string(),
        }),
        is_running: proc_count > 0,
        cpu,
        proc_count,
        thread_count: thread_count_total,
        current_memory,
        app_version: String::new(),
        start_time,
    }
}

fn process_name_candidates(name: &str) -> Vec<String> {
    let lower = name.trim().to_ascii_lowercase();
    if lower.is_empty() {
        return Vec::new();
    }

    if let Some(base) = lower.strip_suffix(".exe") {
        vec![lower.clone(), base.to_string()]
    } else {
        vec![lower.clone(), format!("{lower}.exe")]
    }
}

/// Returns true if `process` matches the watched `name`.
///
/// Two match strategies are used to handle Linux quirks:
/// 1. **Prefix match on process name**: Linux truncates comm (and thus `name()`) to 15
///    characters in `/proc/[pid]/stat`. So `cc-rstationservice` (18 chars) appears as
///    `cc-rstationserv`. We match when the watched name *starts with* the process name.
/// 2. **Cmdline substring match**: Script runners (node, python, sh) show up under the
///    interpreter name. We check whether any cmdline token *contains* the watched name,
///    so `vite` running as `node /path/to/vite` is found via its cmdline.
fn process_matches_name(process: &sysinfo::Process, candidates: &[String]) -> bool {
    let proc_name = process.name().to_string_lossy().to_ascii_lowercase();

    for candidate in candidates {
        // Exact match
        if proc_name == *candidate {
            return true;
        }

        // Prefix match: kernel truncates comm to 15 chars on Linux.
        // e.g. "cc-rstationserv" matches watched name "cc-rstationservice"
        if candidate.starts_with(&proc_name) && proc_name.len() == 15 {
            return true;
        }

        // Cmdline match: catch scripts running under an interpreter.
        // e.g. "node /usr/local/bin/vite" matched by watched name "vite"
        for arg in process.cmd() {
            let arg_lower = arg.to_string_lossy().to_ascii_lowercase();
            // Match on the file stem of any cmdline token to avoid false positives
            // from full paths like "/home/user/.nvm/node"
            let stem = std::path::Path::new(arg_lower.as_str())
                .file_stem()
                .map(|s| s.to_string_lossy().into_owned())
                .unwrap_or_default();
            if stem == *candidate {
                return true;
            }
        }
    }

    false
}

fn resolve_browse_path(path: &str) -> PathBuf {
    if !path.trim().is_empty() {
        return PathBuf::from(path);
    }

    #[cfg(windows)]
    {
        PathBuf::from("/")
    }

    #[cfg(not(windows))]
    {
        PathBuf::from("/home")
    }
}

fn format_system_time(value: Option<std::time::SystemTime>) -> String {
    value
        .map(|time| {
            let local: DateTime<Local> = time.into();
            local.format("%Y-%m-%d %H:%M:%S").to_string()
        })
        .unwrap_or_default()
}

fn saturating_i64(value: u64) -> i64 {
    i64::try_from(value).unwrap_or(i64::MAX)
}

fn prepared_system_snapshot() -> System {
    let mut system = System::new_all();
    system.refresh_all();

    // Use a short blocking sleep so cpu_usage() is meaningful. This is intentionally
    // blocking (not async) because System is not Send, but call sites that care about
    // not blocking the async executor should use spawn_blocking.
    thread::sleep(MINIMUM_CPU_UPDATE_INTERVAL.max(Duration::from_millis(200)));
    system.refresh_cpu_usage();
    system.refresh_processes(ProcessesToUpdate::All, true);

    system
}

/// Async-safe version: runs the blocking snapshot on a dedicated thread pool thread.
pub async fn prepared_system_snapshot_async() -> System {
    tokio::task::spawn_blocking(prepared_system_snapshot)
        .await
        .unwrap_or_else(|_| System::new_all())
}

fn thread_count(process: &sysinfo::Process) -> i32 {
    process
        .tasks()
        .map(|tasks| i32::try_from(tasks.len()).unwrap_or(i32::MAX))
        .unwrap_or_default()
}

struct NetworkSampler {
    networks: Networks,
}

impl NetworkSampler {
    fn new() -> Self {
        Self {
            networks: Networks::new_with_refreshed_list(),
        }
    }

    fn snapshot(&mut self, station_id: &str, state: &AppState) -> Statistics {
        self.networks.refresh(true);

        let rates = self
            .networks
            .iter()
                .map(|(name, data)| InterfaceStatistics {
                    if_name: name.to_string(),
                    bytes_received_per_sec: data.received() as f64,
                    bytes_sented_per_sec: data.transmitted() as f64,
                    total_bytes_per_sec: (data.received() + data.transmitted()) as f64,
                    bytes_received: 0,
                    bytes_sented: 0,
                    bytes_total: 0,
                    unicast_packet_received: 0,
                    unicast_packet_sented: 0,
                    multicast_packet_received: 0,
                    multicast_packet_sented: 0,
                })
            .map(|item| (item.if_name.clone(), item))
            .collect::<HashMap<_, _>>();

        let snapshot = network_counters::collect().unwrap_or_default();
        debug!("Collected network counters: {} interfaces", snapshot.interface_counters.len());
        let mut interface_statistics = snapshot
            .interface_counters
            .iter()
            .map(|counter| {
                let mut item =
                    rates
                        .get(&counter.if_name)
                        .cloned()
                        .unwrap_or(InterfaceStatistics {
                            if_name: counter.if_name.clone(),
                            bytes_received_per_sec: 0.0,
                            bytes_sented_per_sec: 0.0,
                            total_bytes_per_sec: 0.0,
                            bytes_received: 0,
                            bytes_sented: 0,
                            bytes_total: 0,
                            unicast_packet_received: 0,
                            unicast_packet_sented: 0,
                            multicast_packet_received: 0,
                            multicast_packet_sented: 0,
                        });

                item.bytes_received = saturating_i64(counter.bytes_received);
                item.bytes_sented = saturating_i64(counter.bytes_sented);
                item.bytes_total =
                    saturating_i64(counter.bytes_received.saturating_add(counter.bytes_sented));
                item.unicast_packet_received = saturating_i64(counter.unicast_packet_received);
                item.unicast_packet_sented = saturating_i64(counter.unicast_packet_sented);
                item.multicast_packet_received = saturating_i64(counter.multicast_packet_received);
                item.multicast_packet_sented = saturating_i64(counter.multicast_packet_sented);
                item
            })
            .collect::<Vec<_>>();

        interface_statistics.sort_by(|left, right| left.if_name.cmp(&right.if_name));

        let current_connections = state
            .connection_infos()
            .map(|response| response.items.len() as i32)
            .unwrap_or(snapshot.current_connections as i32);
        let udp_listeners = state
            .udp_listener_infos()
            .map(|response| response.items.len() as i32)
            .unwrap_or_default();

        Statistics {
            station_id: station_id.to_string(),
            datagrams_received: saturating_i64(snapshot.datagrams_received),
            datagrams_sent: saturating_i64(snapshot.datagrams_sent),
            datagrams_discarded: saturating_i64(snapshot.datagrams_discarded),
            datagrams_with_errors: saturating_i64(snapshot.datagrams_with_errors),
            udp_listeners,
            segments_received: saturating_i64(snapshot.segments_received),
            segments_sent: saturating_i64(snapshot.segments_sent),
            errors_received: saturating_i64(snapshot.errors_received),
            current_connections,
            reset_connections: saturating_i64(snapshot.reset_connections) as i32,
            interface_statistics,
        }
    }
}
