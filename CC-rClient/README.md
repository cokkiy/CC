# CC-rClient

`CC-rClient` is the Rust/Tauri migration target for the legacy Qt-based `CC-Client`.

## What is already ported

- Tauri v2 desktop shell with a React/Vite frontend
- Rust-owned application state persisted at `~/.CC-rClient/state.json`
- One-time import of legacy station definitions from `~/.CC-Client/指显工作站信息.xml`
- One-time import of legacy client options from `~/.CC-Client/CC-Client.ini`
- Station editing for:
  - name
  - network interfaces
  - startup programs
  - monitored processes
- Working Wake-on-LAN power on action in Rust
- Working local blocked/unblocked state management in Rust
- gRPC-backed station control for:
  - start app
  - restart app
  - exit app
  - shutdown
  - reboot
  - full screen
  - real-time mode
  - previous page
  - next page
  - clear page
- Live runtime refresh backed by the station telemetry stream
- Remote screen capture preview backed by `CaptureScreen`
- Remote file browse / download / upload / rename backed by the Rust file APIs
- Debug build output through Tauri

## Current migration boundary

The original Qt client depended heavily on ZeroC Ice proxies generated from `.ice`
definitions. `CC-rClient` now targets the Rust gRPC surface exposed by
`CC-rStationService`, but some broader migration areas are still not complete:

- richer station-status modeling from the legacy Qt client
- more specialized remote-control/file workflows that were scattered across legacy dialogs

## Commands

```bash
npm install
npm run build
npm run tauri build -- --debug
```

Local end-to-end service + UI test:

```bash
./scripts/dev-with-rstationservice.sh
```

## Build artifacts verified

- Debug binary: `src-tauri/target/debug/cc-rclient`
- Linux bundle: `src-tauri/target/debug/bundle/deb/CC-rClient_0.1.0_amd64.deb`
