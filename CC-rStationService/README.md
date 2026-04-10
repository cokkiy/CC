# CC-rStationService

Rust replacement for `CC-StationService`.

## Components

- `cc-rstationservice`
  - privileged background service
  - Windows: runs as an SCM service
  - Linux: runs as a daemon by default, or foreground under `systemd`
- `cc-rstationservice-agent`
  - user-session desktop helper
  - handles screen capture for the logged-in desktop
  - binds only to loopback and requires a shared token header from the service

## Build

```bash
cargo build --release
```

## Run

Service in foreground:

```bash
./target/release/cc-rstationservice foreground --config ./CC-rStationService.toml
```

Desktop agent:

```bash
./target/release/cc-rstationservice-agent --config ./CC-rStationService.toml
```

## Capture notes

- `CaptureScreen` is proxied through the desktop agent, not performed inside the service.
- `agent.preferred_display_index` selects the monitor used for capture.
- On Linux, the `screenshots` crate already chooses Wayland vs X11; if Wayland capture fails, the agent retries through `grim` when available.
- `agent.auth_token` must match between the service and the desktop agent. The install scripts generate and stamp a shared random token automatically.

## Packaging

- Linux install artifacts: `packaging/linux`
- Windows install artifacts: `packaging/windows`

## Smoke test

```bash
./scripts/test-smoke.sh
```

Optional strict desktop-capture validation:

```bash
REQUIRE_CAPTURE=1 ./scripts/test-smoke.sh
```
