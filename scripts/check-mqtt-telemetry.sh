#!/usr/bin/env bash
set -euo pipefail

# Usage: ./check-mqtt-telemetry.sh [timeout_seconds] [message_count]
TIMEOUT_SECONDS="${1:-12}"
MESSAGE_COUNT="${2:-1}"
TOPIC="${TOPIC:-cc/+/telemetry}"
BROKER_HOST="${BROKER_HOST:-127.0.0.1}"
BROKER_PORT="${BROKER_PORT:-1883}"

if ! [[ "$TIMEOUT_SECONDS" =~ ^[0-9]+$ ]] || ! [[ "$MESSAGE_COUNT" =~ ^[0-9]+$ ]]; then
  echo "[FAIL] timeout_seconds and message_count must be integers"
  exit 2
fi

if [[ "$MESSAGE_COUNT" -lt 1 ]]; then
  echo "[FAIL] message_count must be >= 1"
  exit 2
fi

if ! command -v timeout >/dev/null 2>&1; then
  echo "[FAIL] timeout command not found"
  exit 2
fi

run_with_docker() {
  timeout "$TIMEOUT_SECONDS" docker exec mosquitto sh -lc \
    "mosquitto_sub -h localhost -p $BROKER_PORT -t '$TOPIC' -C $MESSAGE_COUNT -v"
}

run_local() {
  timeout "$TIMEOUT_SECONDS" mosquitto_sub -h "$BROKER_HOST" -p "$BROKER_PORT" -t "$TOPIC" -C "$MESSAGE_COUNT" -v
}

echo "[INFO] Checking MQTT telemetry flow"
echo "[INFO] topic=$TOPIC timeout=${TIMEOUT_SECONDS}s count=$MESSAGE_COUNT"

set +e
OUTPUT=""
STATUS=0

if docker ps --format '{{.Names}}' | grep -qx 'mosquitto'; then
  OUTPUT="$(run_with_docker 2>&1)"
  STATUS=$?
elif command -v mosquitto_sub >/dev/null 2>&1; then
  OUTPUT="$(run_local 2>&1)"
  STATUS=$?
else
  echo "[FAIL] mosquitto_sub not available (neither in docker container nor host PATH)"
  exit 2
fi
set -e

if [[ $STATUS -eq 0 ]] && [[ -n "$OUTPUT" ]]; then
  echo "[PASS] Telemetry messages received"
  echo "$OUTPUT"
  exit 0
fi

if [[ $STATUS -eq 124 ]]; then
  echo "[FAIL] Timeout waiting for telemetry messages"
  echo "[HINT] Verify StationService telemetry publisher and logs"
  exit 1
fi

echo "[FAIL] Telemetry check failed"
if [[ -n "$OUTPUT" ]]; then
  echo "$OUTPUT"
fi
exit 1
