import { describe, expect, it } from "vitest";
import {
  MQTT_BASIC_RUNTIME_MESSAGE,
  buildRuntimeFromMqttTelemetry,
  runtimeHasFullDetails,
} from "../runtime-telemetry";

describe("runtime telemetry helpers", () => {
  it("builds a basic runtime snapshot from MQTT telemetry", () => {
    const runtime = buildRuntimeFromMqttTelemetry("station-7", {
      ts: 123456789,
      station_id: "station-7",
      interval_ms: 1000,
      values: [
        { key: "cpu_usage_percent", v: 48.4 },
        { key: "memory_used_mb", v: 1536.4 },
        { key: "process_count", v: 212.7 },
      ],
    });

    expect(runtime).toMatchObject({
      endpoint: "mqtt:station-7",
      stationId: "station-7",
      cpu: 48.4,
      currentMemory: Math.round(1536.4 * 1024 * 1024),
      procCount: 213,
      telemetrySource: "mqtt",
      detailLevel: "live-basic",
      message: MQTT_BASIC_RUNTIME_MESSAGE,
      appStates: [],
      networkStats: [],
      serviceVersion: "",
      appLauncherVersion: "",
      servicePath: "",
      appLauncherPath: "",
    });
    expect(runtimeHasFullDetails(runtime)).toBe(false);
  });
});
