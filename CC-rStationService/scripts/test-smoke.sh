#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
PROFILE="${PROFILE:-debug}"
CONFIG_PATH="${CONFIG_PATH:-$ROOT_DIR/CC-rStationService.toml}"
REQUIRE_CAPTURE="${REQUIRE_CAPTURE:-0}"

if [[ "$PROFILE" == "release" ]]; then
  BUILD_ARGS=(--release)
  TARGET_DIR="$ROOT_DIR/target/release"
else
  BUILD_ARGS=()
  TARGET_DIR="$ROOT_DIR/target/debug"
fi

SERVICE_BIN="$TARGET_DIR/cc-rstationservice"
AGENT_BIN="$TARGET_DIR/cc-rstationservice-agent"
SMOKETEST_BIN="$TARGET_DIR/cc-rstationservice-smoketest"

SERVICE_LOG="$(mktemp)"
AGENT_LOG="$(mktemp)"
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
  echo "service log: $SERVICE_LOG"
  echo "agent log:   $AGENT_LOG"
}
trap cleanup EXIT

cd "$ROOT_DIR"
cargo build "${BUILD_ARGS[@]}"

"$AGENT_BIN" --config "$CONFIG_PATH" >"$AGENT_LOG" 2>&1 &
AGENT_PID="$!"

"$SERVICE_BIN" foreground --config "$CONFIG_PATH" >"$SERVICE_LOG" 2>&1 &
SERVICE_PID="$!"

SMOKETEST_ARGS=(--config "$CONFIG_PATH")
if [[ "$REQUIRE_CAPTURE" == "1" ]]; then
  SMOKETEST_ARGS+=(--require-capture)
fi

"$SMOKETEST_BIN" "${SMOKETEST_ARGS[@]}"
