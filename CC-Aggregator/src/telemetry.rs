use serde::{Deserialize, Serialize};

pub const TELEMETRY_SCHEMA_VERSION: u32 = 2;

#[derive(Debug, Clone, Serialize, Deserialize, PartialEq)]
pub struct TelemetryBundle {
    pub ts: i64,
    pub station_id: String,
    pub schema_version: u32,
    pub profiles_version: u64,
    pub runtime: Option<TelemetryRuntimeSnapshot>,
    pub apps: Option<Vec<TelemetryAppSnapshot>>,
    pub network: Option<TelemetryNetworkSnapshot>,
    pub storage: Option<Vec<TelemetryStorageSnapshot>>,
    #[serde(default)]
    pub profiles: Vec<TelemetryProfileSnapshot>,
}

#[derive(Debug, Clone, Serialize, Deserialize, PartialEq)]
pub struct TelemetryRuntimeSnapshot {
    pub computer_name: Option<String>,
    pub cpu: Option<f32>,
    pub current_memory: Option<i64>,
    pub total_memory: Option<i64>,
    pub proc_count: Option<i32>,
    pub os_name: Option<String>,
    pub os_version: Option<String>,
    pub service_version: Option<String>,
    pub app_launcher_version: Option<String>,
    pub service_path: Option<String>,
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

#[derive(Debug, Clone, Serialize, Deserialize, PartialEq)]
pub struct TelemetryProfileSnapshot {
    pub id: String,
    pub name: String,
    pub enabled: bool,
    pub collection_interval_ms: u64,
    pub includes: Vec<String>,
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn telemetry_serialization_round_trip_preserves_enriched_payload() {
        let bundle = TelemetryBundle {
            ts: 1234567890,
            station_id: "test-station".to_string(),
            schema_version: TELEMETRY_SCHEMA_VERSION,
            profiles_version: 4,
            runtime: Some(TelemetryRuntimeSnapshot {
                computer_name: Some("alpha".to_string()),
                cpu: Some(45.5),
                current_memory: Some(8192),
                total_memory: Some(16384),
                proc_count: Some(99),
                os_name: Some("Linux".to_string()),
                os_version: Some("6.8".to_string()),
                service_version: Some("1.0.0".to_string()),
                app_launcher_version: Some("1.0.1".to_string()),
                service_path: Some("/srv/service".to_string()),
                app_launcher_path: Some("/srv/launcher".to_string()),
            }),
            apps: Some(vec![TelemetryAppSnapshot {
                monitor_name: "app".to_string(),
                process_name: "app".to_string(),
                process_id: 55,
                is_running: true,
                cpu: 12.5,
                proc_count: 1,
                thread_count: 4,
                current_memory: 4096,
                app_version: "2.0".to_string(),
                start_time: 44,
            }]),
            network: Some(TelemetryNetworkSnapshot {
                current_connections: 10,
                reset_connections: 0,
                udp_listeners: 1,
                datagrams_received: 2,
                datagrams_sent: 3,
                datagrams_discarded: 0,
                datagrams_with_errors: 0,
                segments_received: 4,
                segments_sent: 5,
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
                used_bytes: 50,
                available_bytes: 50,
                usage_percent: 50.0,
            }]),
            profiles: vec![TelemetryProfileSnapshot {
                id: "default".to_string(),
                name: "Default".to_string(),
                enabled: true,
                collection_interval_ms: 1000,
                includes: vec!["runtime_basic".to_string()],
            }],
        };

        let json = serde_json::to_string(&bundle).unwrap();
        let parsed: TelemetryBundle = serde_json::from_str(&json).unwrap();

        assert_eq!(parsed.station_id, "test-station");
        assert_eq!(parsed.schema_version, TELEMETRY_SCHEMA_VERSION);
        assert_eq!(parsed.network.unwrap().interfaces.len(), 1);
    }

    #[test]
    fn malformed_enriched_payload_is_rejected() {
        let json =
            r#"{"ts":"bad","station_id":"station-1","schema_version":2,"profiles_version":1}"#;
        assert!(serde_json::from_str::<TelemetryBundle>(json).is_err());
    }
}
