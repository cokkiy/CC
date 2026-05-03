use std::collections::BTreeSet;

use serde::{Deserialize, Serialize};

pub const TELEMETRY_SCHEMA_VERSION: u32 = 2;

#[derive(
    Debug, Clone, Copy, Serialize, Deserialize, PartialEq, Eq, PartialOrd, Ord, Hash, Default,
)]
#[serde(rename_all = "snake_case")]
pub enum TelemetryInclude {
    #[default]
    RuntimeBasic,
    RuntimeSystem,
    RuntimeApps,
    RuntimeNetwork,
    RuntimeStorage,
}

impl TelemetryInclude {
    pub const ALL: [Self; 5] = [
        Self::RuntimeBasic,
        Self::RuntimeSystem,
        Self::RuntimeApps,
        Self::RuntimeNetwork,
        Self::RuntimeStorage,
    ];

    pub fn key(self) -> &'static str {
        match self {
            Self::RuntimeBasic => "runtime_basic",
            Self::RuntimeSystem => "runtime_system",
            Self::RuntimeApps => "runtime_apps",
            Self::RuntimeNetwork => "runtime_network",
            Self::RuntimeStorage => "runtime_storage",
        }
    }

    pub fn label(self) -> &'static str {
        match self {
            Self::RuntimeBasic => "Runtime Basics",
            Self::RuntimeSystem => "Runtime System",
            Self::RuntimeApps => "Watched Apps",
            Self::RuntimeNetwork => "Network",
            Self::RuntimeStorage => "Storage",
        }
    }

    pub fn description(self) -> &'static str {
        match self {
            Self::RuntimeBasic => "CPU, current memory usage, and process count.",
            Self::RuntimeSystem => "Host, OS, memory capacity, and service metadata.",
            Self::RuntimeApps => "Watched application runtime state.",
            Self::RuntimeNetwork => "Per-interface counters and connection totals.",
            Self::RuntimeStorage => "Per-volume storage capacity and usage.",
        }
    }
}

#[derive(Debug, Clone, Serialize, Deserialize, PartialEq, Eq)]
pub struct TelemetryIncludeDefinition {
    pub key: String,
    pub label: String,
    pub description: String,
}

impl From<TelemetryInclude> for TelemetryIncludeDefinition {
    fn from(include: TelemetryInclude) -> Self {
        Self {
            key: include.key().to_string(),
            label: include.label().to_string(),
            description: include.description().to_string(),
        }
    }
}

#[derive(Debug, Clone, Serialize, Deserialize, PartialEq, Eq)]
pub struct TelemetrySchema {
    pub schema_version: u32,
    pub supported_includes: Vec<TelemetryIncludeDefinition>,
}

impl TelemetrySchema {
    pub fn current() -> Self {
        Self {
            schema_version: TELEMETRY_SCHEMA_VERSION,
            supported_includes: TelemetryInclude::ALL
                .into_iter()
                .map(TelemetryIncludeDefinition::from)
                .collect(),
        }
    }
}

fn default_enabled() -> bool {
    true
}

#[derive(Debug, Clone, Serialize, Deserialize, PartialEq, Eq)]
pub struct TelemetryProfileConfig {
    pub id: String,
    pub name: String,
    #[serde(default = "default_enabled")]
    pub enabled: bool,
    pub collection_interval_ms: u64,
    pub includes: Vec<TelemetryInclude>,
}

impl TelemetryProfileConfig {
    pub fn default_full(interval_ms: u64) -> Self {
        Self {
            id: "default".to_string(),
            name: "Default Runtime".to_string(),
            enabled: true,
            collection_interval_ms: interval_ms.max(1),
            includes: vec![
                TelemetryInclude::RuntimeBasic,
                TelemetryInclude::RuntimeSystem,
                TelemetryInclude::RuntimeApps,
                TelemetryInclude::RuntimeNetwork,
                TelemetryInclude::RuntimeStorage,
            ],
        }
    }

    pub fn normalized(&self) -> Self {
        let includes = self
            .includes
            .iter()
            .copied()
            .collect::<BTreeSet<_>>()
            .into_iter()
            .collect();

        Self {
            id: self.id.trim().to_string(),
            name: self.name.trim().to_string(),
            enabled: self.enabled,
            collection_interval_ms: self.collection_interval_ms,
            includes,
        }
    }

    pub fn snapshot(&self) -> TelemetryProfileSnapshot {
        TelemetryProfileSnapshot {
            id: self.id.clone(),
            name: self.name.clone(),
            enabled: self.enabled,
            collection_interval_ms: self.collection_interval_ms,
            includes: self
                .includes
                .iter()
                .map(|include| include.key().to_string())
                .collect(),
        }
    }
}

pub fn validate_profiles(profiles: &[TelemetryProfileConfig]) -> Result<(), String> {
    let mut ids = BTreeSet::new();

    for profile in profiles {
        let normalized = profile.normalized();
        if normalized.id.is_empty() {
            return Err("telemetry profile id is required".to_string());
        }
        if normalized.name.is_empty() {
            return Err(format!(
                "telemetry profile '{}' requires a name",
                normalized.id
            ));
        }
        if normalized.collection_interval_ms == 0 {
            return Err(format!(
                "telemetry profile '{}' collection_interval_ms must be positive",
                normalized.id
            ));
        }
        if normalized.includes.is_empty() {
            return Err(format!(
                "telemetry profile '{}' must include at least one telemetry section",
                normalized.id
            ));
        }
        if !ids.insert(normalized.id.clone()) {
            return Err(format!(
                "duplicate telemetry profile id '{}'",
                normalized.id
            ));
        }
    }

    Ok(())
}

#[derive(Debug, Clone, Serialize, Deserialize, PartialEq, Eq)]
pub struct TelemetryProfileSnapshot {
    pub id: String,
    pub name: String,
    pub enabled: bool,
    pub collection_interval_ms: u64,
    pub includes: Vec<String>,
}

#[derive(Debug, Clone, Serialize, Deserialize, PartialEq)]
pub struct TelemetryBundle {
    pub ts: i64,
    pub station_id: String,
    pub schema_version: u32,
    pub profiles_version: u64,
    #[serde(skip_serializing_if = "Option::is_none")]
    pub runtime: Option<TelemetryRuntimeSnapshot>,
    #[serde(skip_serializing_if = "Option::is_none")]
    pub apps: Option<Vec<TelemetryAppSnapshot>>,
    #[serde(skip_serializing_if = "Option::is_none")]
    pub network: Option<TelemetryNetworkSnapshot>,
    #[serde(skip_serializing_if = "Option::is_none")]
    pub storage: Option<Vec<TelemetryStorageSnapshot>>,
    #[serde(default, skip_serializing_if = "Vec::is_empty")]
    pub profiles: Vec<TelemetryProfileSnapshot>,
}

#[derive(Debug, Clone, Serialize, Deserialize, PartialEq)]
pub struct TelemetryRuntimeSnapshot {
    #[serde(skip_serializing_if = "Option::is_none")]
    pub computer_name: Option<String>,
    #[serde(skip_serializing_if = "Option::is_none")]
    pub cpu: Option<f32>,
    #[serde(skip_serializing_if = "Option::is_none")]
    pub current_memory: Option<i64>,
    #[serde(skip_serializing_if = "Option::is_none")]
    pub total_memory: Option<i64>,
    #[serde(skip_serializing_if = "Option::is_none")]
    pub proc_count: Option<i32>,
    #[serde(skip_serializing_if = "Option::is_none")]
    pub os_name: Option<String>,
    #[serde(skip_serializing_if = "Option::is_none")]
    pub os_version: Option<String>,
    #[serde(skip_serializing_if = "Option::is_none")]
    pub service_version: Option<String>,
    #[serde(skip_serializing_if = "Option::is_none")]
    pub app_launcher_version: Option<String>,
    #[serde(skip_serializing_if = "Option::is_none")]
    pub service_path: Option<String>,
    #[serde(skip_serializing_if = "Option::is_none")]
    pub app_launcher_path: Option<String>,
}

#[derive(Debug, Clone, Serialize, Deserialize, PartialEq)]
pub struct TelemetryAppSnapshot {
    pub monitor_name: String,
    pub process_name: String,
    pub process_id: i32,
    pub is_running: bool,
    pub cpu: f32,
    pub proc_count: i32,
    pub thread_count: i32,
    pub current_memory: i64,
    pub app_version: String,
    pub start_time: i64,
}

#[derive(Debug, Clone, Serialize, Deserialize, PartialEq)]
pub struct TelemetryNetworkSnapshot {
    pub current_connections: i32,
    pub reset_connections: i32,
    pub udp_listeners: i32,
    pub datagrams_received: i64,
    pub datagrams_sent: i64,
    pub datagrams_discarded: i64,
    pub datagrams_with_errors: i64,
    pub segments_received: i64,
    pub segments_sent: i64,
    pub errors_received: i64,
    pub interfaces: Vec<TelemetryNetworkInterfaceSnapshot>,
}

#[derive(Debug, Clone, Serialize, Deserialize, PartialEq)]
pub struct TelemetryNetworkInterfaceSnapshot {
    pub if_name: String,
    pub bytes_received_per_sec: f64,
    pub bytes_sented_per_sec: f64,
    pub total_bytes_per_sec: f64,
    pub bytes_received: i64,
    pub bytes_sented: i64,
    pub bytes_total: i64,
    pub unicast_packet_received: i64,
    pub unicast_packet_sented: i64,
    pub multicast_packet_received: i64,
    pub multicast_packet_sented: i64,
}

#[derive(Debug, Clone, Serialize, Deserialize, PartialEq)]
pub struct TelemetryStorageSnapshot {
    pub mount_point: String,
    pub total_bytes: i64,
    pub used_bytes: i64,
    pub available_bytes: i64,
    pub usage_percent: f64,
}

#[derive(Debug, Clone, PartialEq)]
pub struct CollectedTelemetrySections {
    pub ts: i64,
    pub runtime: Option<CollectedRuntimeTelemetry>,
    pub apps: Option<Vec<TelemetryAppSnapshot>>,
    pub network: Option<TelemetryNetworkSnapshot>,
    pub storage: Option<Vec<TelemetryStorageSnapshot>>,
}

#[derive(Debug, Clone, PartialEq)]
pub struct CollectedRuntimeTelemetry {
    pub computer_name: String,
    pub cpu: f32,
    pub current_memory: i64,
    pub total_memory: i64,
    pub proc_count: i32,
    pub os_name: String,
    pub os_version: String,
    pub service_version: String,
    pub app_launcher_version: String,
    pub service_path: String,
    pub app_launcher_path: String,
}

impl CollectedTelemetrySections {
    pub fn filter_to_bundle(
        &self,
        station_id: &str,
        profiles_version: u64,
        profile: &TelemetryProfileConfig,
    ) -> TelemetryBundle {
        let includes = profile.includes.iter().copied().collect::<BTreeSet<_>>();
        let include_basic = includes.contains(&TelemetryInclude::RuntimeBasic);
        let include_system = includes.contains(&TelemetryInclude::RuntimeSystem);

        let runtime = self
            .runtime
            .as_ref()
            .map(|runtime| TelemetryRuntimeSnapshot {
                computer_name: include_system.then(|| runtime.computer_name.clone()),
                cpu: include_basic.then_some(runtime.cpu),
                current_memory: include_basic.then_some(runtime.current_memory),
                total_memory: include_system.then_some(runtime.total_memory),
                proc_count: include_basic.then_some(runtime.proc_count),
                os_name: include_system.then(|| runtime.os_name.clone()),
                os_version: include_system.then(|| runtime.os_version.clone()),
                service_version: include_system.then(|| runtime.service_version.clone()),
                app_launcher_version: include_system.then(|| runtime.app_launcher_version.clone()),
                service_path: include_system.then(|| runtime.service_path.clone()),
                app_launcher_path: include_system.then(|| runtime.app_launcher_path.clone()),
            });

        TelemetryBundle {
            ts: self.ts,
            station_id: station_id.to_string(),
            schema_version: TELEMETRY_SCHEMA_VERSION,
            profiles_version,
            runtime: runtime.filter(|value| {
                value.computer_name.is_some()
                    || value.cpu.is_some()
                    || value.current_memory.is_some()
                    || value.total_memory.is_some()
                    || value.proc_count.is_some()
                    || value.os_name.is_some()
                    || value.os_version.is_some()
                    || value.service_version.is_some()
                    || value.app_launcher_version.is_some()
                    || value.service_path.is_some()
                    || value.app_launcher_path.is_some()
            }),
            apps: includes
                .contains(&TelemetryInclude::RuntimeApps)
                .then(|| self.apps.clone().unwrap_or_default()),
            network: includes
                .contains(&TelemetryInclude::RuntimeNetwork)
                .then(|| self.network.clone())
                .flatten(),
            storage: includes
                .contains(&TelemetryInclude::RuntimeStorage)
                .then(|| self.storage.clone().unwrap_or_default()),
            profiles: vec![profile.snapshot()],
        }
    }
}

#[derive(Debug, Clone)]
pub struct TelemetryProfileState {
    pub version: u64,
    pub profiles: Vec<TelemetryProfileConfig>,
}

#[derive(Debug, Clone)]
pub struct ScheduledTelemetryProfile {
    pub profile: TelemetryProfileConfig,
    pub next_collection_ms: u64,
}

#[derive(Debug, Clone)]
pub struct TelemetryScheduler {
    profiles: Vec<ScheduledTelemetryProfile>,
}

impl TelemetryScheduler {
    pub fn new(profiles: &[TelemetryProfileConfig], start_ms: u64) -> Self {
        let profiles = profiles
            .iter()
            .filter(|profile| profile.enabled)
            .cloned()
            .map(|profile| ScheduledTelemetryProfile {
                profile,
                next_collection_ms: start_ms,
            })
            .collect();

        Self { profiles }
    }

    pub fn is_empty(&self) -> bool {
        self.profiles.is_empty()
    }

    pub fn due_collection_indices(&self, now_ms: u64) -> Vec<usize> {
        self.profiles
            .iter()
            .enumerate()
            .filter_map(|(index, profile)| (profile.next_collection_ms <= now_ms).then_some(index))
            .collect()
    }

    pub fn collection_includes(&self, indices: &[usize]) -> Vec<TelemetryInclude> {
        let mut includes = BTreeSet::new();
        for index in indices {
            includes.extend(self.profiles[*index].profile.includes.iter().copied());
        }
        includes.into_iter().collect()
    }

    pub fn collect_due_bundles(
        &mut self,
        indices: &[usize],
        now_ms: u64,
        collected: &CollectedTelemetrySections,
        station_id: &str,
        profiles_version: u64,
    ) -> Vec<TelemetryBundle> {
        let mut bundles = Vec::with_capacity(indices.len());
        for index in indices {
            let profile = &mut self.profiles[*index];
            bundles.push(collected.filter_to_bundle(station_id, profiles_version, &profile.profile));
            profile.next_collection_ms = advance_due_time(
                profile.next_collection_ms,
                profile.profile.collection_interval_ms,
                now_ms,
            );
        }
        bundles
    }

    pub fn next_deadline_ms(&self) -> Option<u64> {
        self.profiles.iter().map(|profile| profile.next_collection_ms).min()
    }
}

fn advance_due_time(previous_deadline_ms: u64, interval_ms: u64, now_ms: u64) -> u64 {
    let mut next = previous_deadline_ms.saturating_add(interval_ms.max(1));
    while next <= now_ms {
        next = next.saturating_add(interval_ms.max(1));
    }
    next
}

#[cfg(test)]
mod tests {
    use super::*;

    fn sample_collected(ts: i64) -> CollectedTelemetrySections {
        CollectedTelemetrySections {
            ts,
            runtime: Some(CollectedRuntimeTelemetry {
                computer_name: "alpha".to_string(),
                cpu: 48.5,
                current_memory: 123,
                total_memory: 456,
                proc_count: 7,
                os_name: "Linux".to_string(),
                os_version: "6.8".to_string(),
                service_version: "1.2.3".to_string(),
                app_launcher_version: "4.5.6".to_string(),
                service_path: "/srv/service".to_string(),
                app_launcher_path: "/srv/launcher".to_string(),
            }),
            apps: Some(vec![TelemetryAppSnapshot {
                monitor_name: "app".to_string(),
                process_name: "app".to_string(),
                process_id: 5,
                is_running: true,
                cpu: 10.0,
                proc_count: 1,
                thread_count: 2,
                current_memory: 2048,
                app_version: "1.0".to_string(),
                start_time: 42,
            }]),
            network: Some(TelemetryNetworkSnapshot {
                current_connections: 2,
                reset_connections: 0,
                udp_listeners: 1,
                datagrams_received: 3,
                datagrams_sent: 4,
                datagrams_discarded: 0,
                datagrams_with_errors: 0,
                segments_received: 5,
                segments_sent: 6,
                errors_received: 0,
                interfaces: vec![TelemetryNetworkInterfaceSnapshot {
                    if_name: "eth0".to_string(),
                    bytes_received_per_sec: 1.0,
                    bytes_sented_per_sec: 2.0,
                    total_bytes_per_sec: 3.0,
                    bytes_received: 4,
                    bytes_sented: 5,
                    bytes_total: 9,
                    unicast_packet_received: 6,
                    unicast_packet_sented: 7,
                    multicast_packet_received: 8,
                    multicast_packet_sented: 9,
                }],
            }),
            storage: Some(vec![TelemetryStorageSnapshot {
                mount_point: "/".to_string(),
                total_bytes: 100,
                used_bytes: 40,
                available_bytes: 60,
                usage_percent: 40.0,
            }]),
        }
    }

    #[test]
    fn telemetry_bundle_round_trip_preserves_enriched_payload() {
        let profile = TelemetryProfileConfig::default_full(5000);
        let bundle = sample_collected(1000).filter_to_bundle("station-a", 3, &profile);

        let json = serde_json::to_string(&bundle).unwrap();
        let parsed: TelemetryBundle = serde_json::from_str(&json).unwrap();

        assert_eq!(parsed.station_id, "station-a");
        assert_eq!(parsed.schema_version, TELEMETRY_SCHEMA_VERSION);
        assert_eq!(parsed.profiles_version, 3);
        assert_eq!(parsed.apps.unwrap().len(), 1);
        assert_eq!(parsed.network.unwrap().interfaces.len(), 1);
        assert_eq!(parsed.storage.unwrap().len(), 1);
    }

    #[test]
    fn filter_to_bundle_omits_sections_not_included() {
        let profile = TelemetryProfileConfig {
            id: "basic".to_string(),
            name: "Basic".to_string(),
            enabled: true,
            collection_interval_ms: 1000,
            includes: vec![TelemetryInclude::RuntimeBasic],
        };

        let bundle = sample_collected(1000).filter_to_bundle("station-a", 2, &profile);

        assert!(bundle.apps.is_none());
        assert!(bundle.network.is_none());
        assert!(bundle.storage.is_none());
        assert_eq!(
            bundle.runtime.as_ref().and_then(|runtime| runtime.cpu),
            Some(48.5)
        );
        assert_eq!(
            bundle
                .runtime
                .as_ref()
                .and_then(|runtime| runtime.service_version.clone()),
            None
        );
    }

    #[test]
    fn validate_profiles_rejects_duplicate_ids_zero_intervals_and_empty_includes() {
        let duplicate = vec![
            TelemetryProfileConfig::default_full(1000),
            TelemetryProfileConfig::default_full(2000),
        ];
        assert!(validate_profiles(&duplicate).is_err());

        let invalid_collection_interval = vec![TelemetryProfileConfig {
            collection_interval_ms: 0,
            ..TelemetryProfileConfig::default_full(1000)
        }];
        assert!(validate_profiles(&invalid_collection_interval).is_err());

        let empty_includes = vec![TelemetryProfileConfig {
            includes: Vec::new(),
            ..TelemetryProfileConfig::default_full(1000)
        }];
        assert!(validate_profiles(&empty_includes).is_err());
    }

    #[test]
    fn scheduler_collects_and_builds_bundles_on_collection_deadline() {
        let profiles = vec![
            TelemetryProfileConfig {
                id: "fast".to_string(),
                name: "Fast".to_string(),
                enabled: true,
                collection_interval_ms: 1000,
                includes: vec![TelemetryInclude::RuntimeBasic],
            },
            TelemetryProfileConfig {
                id: "slow".to_string(),
                name: "Slow".to_string(),
                enabled: true,
                collection_interval_ms: 1000,
                includes: vec![TelemetryInclude::RuntimeNetwork],
            },
        ];

        let mut scheduler = TelemetryScheduler::new(&profiles, 0);
        let due_collect = scheduler.due_collection_indices(0);

        assert_eq!(due_collect, vec![0, 1]);
        assert_eq!(
            scheduler.collection_includes(&due_collect),
            vec![
                TelemetryInclude::RuntimeBasic,
                TelemetryInclude::RuntimeNetwork
            ]
        );

        let collected = sample_collected(0);
        let bundles = scheduler.collect_due_bundles(&due_collect, 0, &collected, "station-a", 1);

        let fast = &bundles[0];
        let slow = &bundles[1];
        assert!(fast.runtime.is_some());
        assert!(fast.network.is_none());
        assert!(slow.runtime.is_none());
        assert!(slow.network.is_some());
        assert_eq!(scheduler.due_collection_indices(999), Vec::<usize>::new());
        assert_eq!(scheduler.due_collection_indices(1000), vec![0, 1]);
    }
}
