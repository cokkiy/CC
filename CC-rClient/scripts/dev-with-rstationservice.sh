#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
CLIENT_DIR="$(cd "$SCRIPT_DIR/.." && pwd)"
REPO_DIR="$(cd "$CLIENT_DIR/.." && pwd)"
SERVICE_DIR="$REPO_DIR/CC-rStationService"

PROFILE="${PROFILE:-debug}"
SERVICE_HOST="${SERVICE_HOST:-127.0.0.1}"
SERVICE_PORT="${SERVICE_PORT:-50051}"
AGENT_PORT="${AGENT_PORT:-50052}"
STATE_INTERVAL_SECONDS="${STATE_INTERVAL_SECONDS:-2}"
TEST_HOME="${TEST_HOME:-$REPO_DIR/.tmp/cc-rclient-dev-home}"
AUTO_INSTALL_NPM="${AUTO_INSTALL_NPM:-1}"
START_AGENT="${START_AGENT:-auto}"
HOST_HOME="${HOME:-$(getent passwd "$(id -un)" | cut -d: -f6)}"
HOST_CARGO_HOME="${CARGO_HOME:-$HOST_HOME/.cargo}"
HOST_RUSTUP_HOME="${RUSTUP_HOME:-$HOST_HOME/.rustup}"

if [[ "$PROFILE" == "release" ]]; then
  BUILD_ARGS=(--release)
  SERVICE_TARGET_DIR="$SERVICE_DIR/target/release"
else
  BUILD_ARGS=()
  SERVICE_TARGET_DIR="$SERVICE_DIR/target/debug"
fi

SERVICE_BIN="$SERVICE_TARGET_DIR/cc-rstationservice"
AGENT_BIN="$SERVICE_TARGET_DIR/cc-rstationservice-agent"
SERVICE_LOG="$TEST_HOME/logs/rstationservice.log"
AGENT_LOG="$TEST_HOME/logs/rstationservice-agent.log"
SERVICE_CONFIG_PATH="$TEST_HOME/CC-rStationService.toml"
CLIENT_STATE_DIR="$TEST_HOME/.CC-rClient"
CLIENT_STATE_PATH="$CLIENT_STATE_DIR/state.json"
SERVICE_PID=""
AGENT_PID=""

cleanup() {
  set +e
  if [[ -n "$SERVICE_PID" ]]; then
    kill "$SERVICE_PID" 2>/dev/null || true
    wait "$SERVICE_PID" 2>/dev/null || true
  fi
  if [[ -n "$AGENT_PID" ]]; then
    kill "$AGENT_PID" 2>/dev/null || true
    wait "$AGENT_PID" 2>/dev/null || true
  fi
  echo
  echo "test home:   $TEST_HOME"
  echo "service log: $SERVICE_LOG"
  if [[ -n "$AGENT_PID" || -f "$AGENT_LOG" ]]; then
    echo "agent log:   $AGENT_LOG"
  fi
}
trap cleanup EXIT

mkdir -p "$TEST_HOME/logs" "$CLIENT_STATE_DIR"

cat >"$SERVICE_CONFIG_PATH" <<EOF
[service]
service_name = "CC-rStationService"
station_id = "cc-rclient-dev-station"
state_interval_seconds = $STATE_INTERVAL_SECONDS
watched_processes = ["vite", "cc-rclient", "cc-rstationservice"]
udp_display_target = "127.0.0.1:9008"
launcher_proxy_path = ""

[control]
listen_addr = "$SERVICE_HOST:$SERVICE_PORT"

[agent]
listen_addr = "127.0.0.1:$AGENT_PORT"
auth_token = "local-change-me"
preferred_display_index = 0
EOF

cat >"$CLIENT_STATE_PATH" <<EOF
{
  "stations": [
    {
      "id": "local-rstationservice",
      "name": "Local CC-rStationService",
      "blocked": false,
      "networkInterfaces": [
        {
          "mac": "",
          "ips": ["$SERVICE_HOST:$SERVICE_PORT"]
        }
      ],
      "startPrograms": [],
      "monitorProcesses": ["vite", "cc-rclient", "cc-rstationservice"],
      "lastAction": null
    }
  ],
  "options": {
    "interval": $STATE_INTERVAL_SECONDS,
    "isFirstTimeRun": false,
    "startApps": [],
    "monitorProcesses": ["vite", "cc-rclient", "cc-rstationservice"],
    "weatherImageDownloadOption": {
      "download": 2,
      "url": "",
      "userName": "",
      "password": "",
      "lastHours": 0,
      "interval": 2,
      "deletePreviousFiles": false,
      "deleteHowHoursAgo": 0,
      "subDirectory": "",
      "savePathForLinux": "",
      "savePathForWindows": ""
    }
  }
}
EOF

cd "$SERVICE_DIR"
cargo build "${BUILD_ARGS[@]}"

if [[ ! -x "$SERVICE_BIN" ]]; then
  echo "missing station service binary: $SERVICE_BIN" >&2
  exit 1
fi

if [[ "$START_AGENT" == "auto" ]]; then
  if [[ -n "${DISPLAY:-}" || -n "${WAYLAND_DISPLAY:-}" ]]; then
    START_AGENT="1"
  else
    START_AGENT="0"
  fi
fi

if [[ "$START_AGENT" == "1" ]]; then
  if [[ ! -x "$AGENT_BIN" ]]; then
    echo "missing station agent binary: $AGENT_BIN" >&2
    exit 1
  fi
  HOME="$TEST_HOME" \
  CARGO_HOME="$HOST_CARGO_HOME" \
  RUSTUP_HOME="$HOST_RUSTUP_HOME" \
  "$AGENT_BIN" --config "$SERVICE_CONFIG_PATH" >"$AGENT_LOG" 2>&1 &
  AGENT_PID="$!"
fi

HOME="$TEST_HOME" \
CARGO_HOME="$HOST_CARGO_HOME" \
RUSTUP_HOME="$HOST_RUSTUP_HOME" \
"$SERVICE_BIN" foreground  --config "$SERVICE_CONFIG_PATH" >"$SERVICE_LOG" 2>&1 &
SERVICE_PID="$!"

for _ in $(seq 1 50); do
  if bash -lc "exec 3<>/dev/tcp/$SERVICE_HOST/$SERVICE_PORT" >/dev/null 2>&1; then
    break
  fi
  sleep 0.2
done

if ! bash -lc "exec 3<>/dev/tcp/$SERVICE_HOST/$SERVICE_PORT" >/dev/null 2>&1; then
  echo "station service did not start on $SERVICE_HOST:$SERVICE_PORT" >&2
  exit 1
fi

cd "$CLIENT_DIR"
if [[ "$AUTO_INSTALL_NPM" == "1" && ! -d node_modules ]]; then
  npm install
fi

echo "Launching CC-rClient against local CC-rStationService"
echo "  HOME=$TEST_HOME"
echo "  Service endpoint=$SERVICE_HOST:$SERVICE_PORT"
echo "  Config=$SERVICE_CONFIG_PATH"

HOME="$TEST_HOME" \
CARGO_HOME="$HOST_CARGO_HOME" \
RUSTUP_HOME="$HOST_RUSTUP_HOME" \
npm run tauri dev
