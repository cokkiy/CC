import type { StationRuntimeSnapshot } from "./types";

export type MqttTelemetryValue = {
  key: string;
  v: number;
};

export type MqttTelemetryBundle = {
  ts: number;
  station_id: string;
  interval_ms: number;
  values: MqttTelemetryValue[];
};

export const MQTT_BASIC_RUNTIME_MESSAGE = "Live MQTT telemetry (basic runtime)";

export function buildRuntimeFromMqttTelemetry(
  stationId: string,
  telemetry: MqttTelemetryBundle,
): StationRuntimeSnapshot {
  const valueByKey = new Map(telemetry.values.map((value) => [value.key, value.v]));
  const currentMemoryMb = valueByKey.get("memory_used_mb") ?? 0;

  return {
    endpoint: `mqtt:${stationId}`,
    stationId,
    computerName: stationId,
    osName: "",
    osVersion: "",
    totalMemory: 0,
    currentMemory: Math.round(currentMemoryMb * 1024 * 1024),
    cpu: valueByKey.get("cpu_usage_percent") ?? 0,
    procCount: Math.round(valueByKey.get("process_count") ?? 0),
    serviceVersion: "",
    appLauncherVersion: "",
    servicePath: "",
    appLauncherPath: "",
    appStates: [],
    networkStats: [],
    message: MQTT_BASIC_RUNTIME_MESSAGE,
    telemetrySource: "mqtt",
    detailLevel: "live-basic",
  };
}

export function runtimeHasFullDetails(
  runtime: StationRuntimeSnapshot | null | undefined,
): runtime is StationRuntimeSnapshot {
  return runtime?.detailLevel === "full";
}
