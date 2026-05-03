import type {
  RemoteAppState,
  RemoteInterfaceStat,
  RemoteStorageStat,
  StationRuntimeSnapshot,
} from "./types";

export type MqttTelemetryProfileSnapshot = {
  id: string;
  name: string;
  enabled: boolean;
  collection_interval_ms: number;
  includes: string[];
};

export type MqttTelemetryBundle = {
  ts: number;
  station_id: string;
  schema_version: number;
  profiles_version: number;
  runtime?: {
    computer_name?: string;
    cpu?: number;
    current_memory?: number;
    total_memory?: number;
    proc_count?: number;
    os_name?: string;
    os_version?: string;
    service_version?: string;
    app_launcher_version?: string;
    service_path?: string;
    app_launcher_path?: string;
  } | null;
  apps?: Array<{
    monitor_name: string;
    process_name: string;
    process_id: number;
    is_running: boolean;
    cpu: number;
    proc_count: number;
    thread_count: number;
    current_memory: number;
    app_version: string;
    start_time: number;
  }> | null;
  network?: {
    current_connections: number;
    reset_connections: number;
    udp_listeners: number;
    datagrams_received: number;
    datagrams_sent: number;
    datagrams_discarded: number;
    datagrams_with_errors: number;
    segments_received: number;
    segments_sent: number;
    errors_received: number;
    interfaces: Array<{
      if_name: string;
      bytes_received_per_sec: number;
      bytes_sented_per_sec: number;
      total_bytes_per_sec: number;
      bytes_received: number;
      bytes_sented: number;
      bytes_total: number;
      unicast_packet_received: number;
      unicast_packet_sented: number;
      multicast_packet_received: number;
      multicast_packet_sented: number;
    }>;
  } | null;
  storage?: Array<{
    mount_point: string;
    total_bytes: number;
    used_bytes: number;
    available_bytes: number;
    usage_percent: number;
  }> | null;
  profiles?: MqttTelemetryProfileSnapshot[];
};

function profileSummary(profiles: MqttTelemetryProfileSnapshot[] | undefined) {
  if (!profiles || profiles.length === 0) {
    return "MQTT telemetry profile applied";
  }

  const [primary] = profiles;
  const includes = primary.includes.join(", ");
  return `${primary.name} (${primary.id}) · ${includes}`;
}

function mapApps(apps: MqttTelemetryBundle["apps"]): RemoteAppState[] {
  return (apps ?? []).map((item) => ({
    monitorName: item.monitor_name,
    processName: item.process_name,
    processId: item.process_id,
    isRunning: item.is_running,
    cpu: item.cpu,
    procCount: item.proc_count,
    threadCount: item.thread_count,
    currentMemory: item.current_memory,
    appVersion: item.app_version,
    startTime: item.start_time,
  }));
}

function mapNetwork(network: MqttTelemetryBundle["network"]): RemoteInterfaceStat[] {
  return (network?.interfaces ?? []).map((item) => ({
    ifName: item.if_name,
    bytesReceivedPerSec: item.bytes_received_per_sec,
    bytesSentedPerSec: item.bytes_sented_per_sec,
    totalBytesPerSec: item.total_bytes_per_sec,
    bytesReceived: item.bytes_received,
    bytesSented: item.bytes_sented,
    bytesTotal: item.bytes_total,
    unicastPacketReceived: item.unicast_packet_received,
    unicastPacketSented: item.unicast_packet_sented,
    multicastPacketReceived: item.multicast_packet_received,
    multicastPacketSented: item.multicast_packet_sented,
  }));
}

function mapStorage(storage: MqttTelemetryBundle["storage"]): RemoteStorageStat[] {
  return (storage ?? []).map((item) => ({
    mountPoint: item.mount_point,
    totalBytes: item.total_bytes,
    usedBytes: item.used_bytes,
    availableBytes: item.available_bytes,
    usagePercent: item.usage_percent,
  }));
}

export function buildRuntimeFromMqttTelemetry(
  stationId: string,
  telemetry: MqttTelemetryBundle,
): StationRuntimeSnapshot {
  const runtime = telemetry.runtime ?? {};

  return {
    endpoint: `mqtt:${stationId}`,
    stationId,
    computerName: runtime.computer_name ?? stationId,
    osName: runtime.os_name ?? "",
    osVersion: runtime.os_version ?? "",
    totalMemory: runtime.total_memory ?? 0,
    currentMemory: runtime.current_memory ?? 0,
    cpu: runtime.cpu ?? 0,
    procCount: runtime.proc_count ?? 0,
    serviceVersion: runtime.service_version ?? "",
    appLauncherVersion: runtime.app_launcher_version ?? "",
    servicePath: runtime.service_path ?? "",
    appLauncherPath: runtime.app_launcher_path ?? "",
    appStates: mapApps(telemetry.apps),
    networkStats: mapNetwork(telemetry.network),
    storageStats: mapStorage(telemetry.storage),
    message: profileSummary(telemetry.profiles),
    telemetrySource: "mqtt",
    detailLevel: "full",
  };
}

export function runtimeHasFullDetails(
  runtime: StationRuntimeSnapshot | null | undefined,
): runtime is StationRuntimeSnapshot {
  return runtime?.detailLevel === "full";
}
