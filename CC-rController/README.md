# CC-rController

`CC-rController` is the Rust/Tauri migration target for the legacy Qt-based `CC-Client`.

## What is already ported

- Tauri v2 desktop shell with a React/Vite frontend
- Rust-owned application state persisted at `~/.CC-rController/state.json`
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
- Reusable batch task management with optional main-toolbar shortcuts for frequently used tasks
- Debug build output through Tauri

## Current migration boundary

The original Qt client depended heavily on ZeroC Ice proxies generated from `.ice`
definitions. `CC-rController` now targets the Rust gRPC surface exposed by
`CC-rDeviceAgent`, but some broader migration areas are still not complete:

- richer station-status modeling from the legacy Qt client
- more specialized remote-control/file workflows that were scattered across legacy dialogs

## Commands

```bash
npm install
npm run build
npm run tauri:build:debug
```

Run the desktop app with bundled local UI assets instead of the Vite dev server:

```bash
npm run desktop:run
```

Use the Vite server only for frontend development:

```bash
npm run tauri:dev
```

Local end-to-end service + UI test:

```bash
./scripts/dev-with-rdeviceagent.sh
```

## Build artifacts verified

- Debug binary: `src-tauri/target/debug/cc-rcontroller`
- Linux bundle: `src-tauri/target/debug/bundle/deb/CC-rController_0.1.0_amd64.deb`
