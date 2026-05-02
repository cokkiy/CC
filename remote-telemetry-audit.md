# Remote Telemetry Audit

Date: 2026-05-02

## Conclusion

Legacy gRPC telemetry code for remote runtime is still active.

The project already has an MQTT -> WebSocket -> frontend path for live telemetry and station status, but the Runtime panel still performs a separate gRPC fetch/stream for remote stations. That means telemetry has not fully shifted to MQTT and WebSocket yet.

## Active MQTT and WebSocket Path

### Station publishes MQTT telemetry

- [CC-rStationService/src/mqtt.rs](/home/cokkiy/CC/CC-rStationService/src/mqtt.rs:194)
  `publish_telemetry()` publishes to topic `cc/{station_id}/telemetry`.

- [CC-rStationService/src/app.rs](/home/cokkiy/CC/CC-rStationService/src/app.rs:87)
  The service starts an MQTT telemetry publisher loop when MQTT telemetry is enabled.

### Tauri bridge forwards MQTT data to the frontend

- [CC-rClient/src-tauri/src/ws_bridge.rs](/home/cokkiy/CC/CC-rClient/src-tauri/src/ws_bridge.rs:304)
  The WebSocket bridge emits `telemetry` events to the frontend.

- [CC-rClient/src-tauri/src/ws_bridge.rs](/home/cokkiy/CC/CC-rClient/src-tauri/src/ws_bridge.rs:315)
  The bridge also emits `station-status` events.

### Frontend consumes live MQTT telemetry

- [CC-rClient/src/App.tsx](/home/cokkiy/CC/CC-rClient/src/App.tsx:1089)
  The frontend listens for `telemetry`.

- [CC-rClient/src/App.tsx](/home/cokkiy/CC/CC-rClient/src/App.tsx:1143)
  The frontend listens for `station-status`.

- [CC-rClient/src/App.tsx](/home/cokkiy/CC/CC-rClient/src/App.tsx:135)
  `buildRuntimeFromMqttTelemetry()` builds a runtime snapshot from MQTT values.

## Remaining Active Legacy gRPC Telemetry Path

### Frontend still polls runtime via Tauri command

- [CC-rClient/src/App.tsx](/home/cokkiy/CC/CC-rClient/src/App.tsx:1303)
  Selected non-MQTT-discovered stations still trigger `refreshRuntime()` on an interval.

- [CC-rClient/src/App.tsx](/home/cokkiy/CC/CC-rClient/src/App.tsx:1336)
  `refreshRuntime()` invokes `fetch_station_runtime_for_ui`.

### Tauri command still fetches runtime via gRPC

- [CC-rClient/src-tauri/src/lib.rs](/home/cokkiy/CC/CC-rClient/src-tauri/src/lib.rs:204)
  `fetch_station_runtime_for_ui()` loads the station and forwards to `fetch_station_runtime(...)`.

- [CC-rClient/src-tauri/src/remote.rs](/home/cokkiy/CC/CC-rClient/src-tauri/src/remote.rs:120)
  `fetch_station_runtime(...)` connects `StationControlClient`.

- [CC-rClient/src-tauri/src/remote.rs](/home/cokkiy/CC/CC-rClient/src-tauri/src/remote.rs:133)
  It still calls `set_state_gathering_interval(...)`.

- [CC-rClient/src-tauri/src/remote.rs](/home/cokkiy/CC/CC-rClient/src-tauri/src/remote.rs:184)
  It still connects `TelemetryClient`.

- [CC-rClient/src-tauri/src/remote.rs](/home/cokkiy/CC/CC-rClient/src-tauri/src/remote.rs:202)
  It still opens `stream_telemetry(...)`.

### Station service still serves the telemetry RPC

- [CC-rStationService/src/app.rs](/home/cokkiy/CC/CC-rStationService/src/app.rs:575)
  `stream_telemetry()` is still implemented and active.

- [CC-rStationService/src/app.rs](/home/cokkiy/CC/CC-rStationService/src/app.rs:666)
  The service still emits `StationRunningState`, `AppsRunningState`, and `NetStatistics` over gRPC telemetry.

## What MQTT and WebSocket Currently Cover

The current MQTT-derived runtime snapshot includes:

- CPU usage
- Memory used
- Process count
- Live station status / online state

Relevant mapping:

- [CC-rClient/src/App.tsx](/home/cokkiy/CC/CC-rClient/src/App.tsx:142)

## What Still Depends on gRPC

The current Runtime panel still depends on gRPC for:

- App running states
- Network statistics
- Service version
- Service path
- App launcher path
- System state details fetched from control RPCs

Relevant reads:

- [CC-rClient/src-tauri/src/remote.rs](/home/cokkiy/CC/CC-rClient/src-tauri/src/remote.rs:142)
- [CC-rClient/src-tauri/src/remote.rs](/home/cokkiy/CC/CC-rClient/src-tauri/src/remote.rs:147)
- [CC-rClient/src-tauri/src/remote.rs](/home/cokkiy/CC/CC-rClient/src-tauri/src/remote.rs:152)
- [CC-rClient/src-tauri/src/remote.rs](/home/cokkiy/CC/CC-rClient/src-tauri/src/remote.rs:158)
- [CC-rClient/src-tauri/src/remote.rs](/home/cokkiy/CC/CC-rClient/src-tauri/src/remote.rs:248)
- [CC-rClient/src-tauri/src/remote.rs](/home/cokkiy/CC/CC-rClient/src-tauri/src/remote.rs:255)
- [CC-rClient/src-tauri/src/remote.rs](/home/cokkiy/CC/CC-rClient/src-tauri/src/remote.rs:285)
- [CC-rClient/src-tauri/src/remote.rs](/home/cokkiy/CC/CC-rClient/src-tauri/src/remote.rs:307)

## Recommended Refactor Direction

If the target architecture is "transform telemetry into MQTT and WebSocket early", the next cleanup step should be:

1. Stop using `TelemetryClient::stream_telemetry(...)` for runtime updates.
2. Stop periodic frontend `refreshRuntime()` polling for telemetry data.
3. Enrich MQTT payloads if the UI still needs:
   - app states
   - network stats
   - version info
   - service/app launcher paths
   - disk/volume usage and total capicity
4. Keep gRPC only for control and file operations if those are still needed.

## Short Answer

Yes. There is still active legacy gRPC code for remote telemetry, and the telemetry path has not fully moved to MQTT + WebSocket yet.
