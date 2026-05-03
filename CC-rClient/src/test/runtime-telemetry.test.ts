import { describe, expect, it } from "vitest";
import { buildRuntimeFromMqttTelemetry, runtimeHasFullDetails } from "../runtime-telemetry";

describe("runtime telemetry helpers", () => {
  it("builds a full runtime snapshot from enriched MQTT telemetry", () => {
    const runtime = buildRuntimeFromMqttTelemetry("station-7", {
      ts: 123456789,
      station_id: "station-7",
      schema_version: 2,
      profiles_version: 9,
      runtime: {
        computer_name: "alpha",
        cpu: 48.4,
        current_memory: 1536,
        total_memory: 4096,
        proc_count: 213,
        service_version: "1.0.0",
      },
      apps: [
        {
          monitor_name: "app",
          process_name: "app",
          process_id: 7,
          is_running: true,
          cpu: 9.1,
          proc_count: 1,
          thread_count: 3,
          current_memory: 128,
          app_version: "2.0",
          start_time: 55,
        },
      ],
      network: {
        current_connections: 3,
        reset_connections: 0,
        udp_listeners: 1,
        datagrams_received: 0,
        datagrams_sent: 0,
        datagrams_discarded: 0,
        datagrams_with_errors: 0,
        segments_received: 0,
        segments_sent: 0,
        errors_received: 0,
        interfaces: [
          {
            if_name: "eth0",
            bytes_received_per_sec: 10,
            bytes_sented_per_sec: 20,
            total_bytes_per_sec: 30,
            bytes_received: 40,
            bytes_sented: 50,
            bytes_total: 90,
            unicast_packet_received: 4,
            unicast_packet_sented: 5,
            multicast_packet_received: 6,
            multicast_packet_sented: 7,
          },
        ],
      },
      storage: [
        {
          mount_point: "/",
          total_bytes: 100,
          used_bytes: 40,
          available_bytes: 60,
          usage_percent: 40,
        },
      ],
      profiles: [
        {
          id: "default",
          name: "Default Runtime",
          enabled: true,
          collection_interval_ms: 1000,
          includes: ["runtime_basic", "runtime_system", "runtime_network"],
        },
      ],
    });

    expect(runtime).toMatchObject({
      endpoint: "mqtt:station-7",
      stationId: "station-7",
      computerName: "alpha",
      cpu: 48.4,
      currentMemory: 1536,
      totalMemory: 4096,
      procCount: 213,
      telemetrySource: "mqtt",
      detailLevel: "full",
      serviceVersion: "1.0.0",
      appStates: [{ monitorName: "app" }],
      networkStats: [{ ifName: "eth0" }],
      storageStats: [{ mountPoint: "/" }],
    });
    expect(runtime.message).toContain("Default Runtime");
    expect(runtimeHasFullDetails(runtime)).toBe(true);
  });

  it("maps omitted sections to empty defaults without dropping full detail mode", () => {
    const runtime = buildRuntimeFromMqttTelemetry("station-9", {
      ts: 123,
      station_id: "station-9",
      schema_version: 2,
      profiles_version: 1,
      runtime: {
        cpu: 11,
        current_memory: 22,
        proc_count: 3,
      },
      profiles: [
        {
          id: "basic",
          name: "Basic",
          enabled: true,
          collection_interval_ms: 1000,
          includes: ["runtime_basic"],
        },
      ],
    });

    expect(runtime.detailLevel).toBe("full");
    expect(runtime.networkStats).toEqual([]);
    expect(runtime.storageStats).toEqual([]);
    expect(runtime.appStates).toEqual([]);
  });

  it("preserves prior sections when alternating profile payloads omit them", () => {
    const baseline = buildRuntimeFromMqttTelemetry("station-5", {
      ts: 1000,
      station_id: "station-5",
      schema_version: 2,
      profiles_version: 1,
      runtime: {
        computer_name: "alpha",
        cpu: 30,
        current_memory: 2048,
        total_memory: 8192,
        proc_count: 99,
        os_name: "Linux",
        os_version: "6.8",
      },
      network: {
        current_connections: 0,
        reset_connections: 0,
        udp_listeners: 0,
        datagrams_received: 0,
        datagrams_sent: 0,
        datagrams_discarded: 0,
        datagrams_with_errors: 0,
        segments_received: 0,
        segments_sent: 0,
        errors_received: 0,
        interfaces: [
          {
            if_name: "eth0",
            bytes_received_per_sec: 11,
            bytes_sented_per_sec: 22,
            total_bytes_per_sec: 33,
            bytes_received: 44,
            bytes_sented: 55,
            bytes_total: 99,
            unicast_packet_received: 0,
            unicast_packet_sented: 0,
            multicast_packet_received: 0,
            multicast_packet_sented: 0,
          },
        ],
      },
      profiles: [
        {
          id: "default",
          name: "Default",
          enabled: true,
          collection_interval_ms: 1000,
          includes: ["runtime_basic", "runtime_system", "runtime_network"],
        },
      ],
    });

    const merged = buildRuntimeFromMqttTelemetry(
      "station-5",
      {
        ts: 2000,
        station_id: "station-5",
        schema_version: 2,
        profiles_version: 1,
        apps: [
          {
            monitor_name: "watcher",
            process_name: "watcher",
            process_id: 10,
            is_running: true,
            cpu: 7,
            proc_count: 1,
            thread_count: 2,
            current_memory: 512,
            app_version: "1.0.0",
            start_time: 77,
          },
        ],
        storage: [
          {
            mount_point: "/",
            total_bytes: 1000,
            used_bytes: 250,
            available_bytes: 750,
            usage_percent: 25,
          },
        ],
        profiles: [
          {
            id: "app",
            name: "AppWatch",
            enabled: true,
            collection_interval_ms: 2000,
            includes: ["runtime_apps", "runtime_storage"],
          },
        ],
      },
      baseline,
    );

    expect(merged.computerName).toBe("alpha");
    expect(merged.cpu).toBe(30);
    expect(merged.currentMemory).toBe(2048);
    expect(merged.networkStats).toEqual(baseline.networkStats);
    expect(merged.appStates).toMatchObject([{ monitorName: "watcher" }]);
    expect(merged.storageStats).toMatchObject([{ mountPoint: "/" }]);
  });
});
