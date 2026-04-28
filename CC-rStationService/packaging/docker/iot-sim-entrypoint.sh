#!/bin/sh
set -eu

TEMPLATE_PATH="/etc/cc-rstationservice/CC-rStationService.toml.template"
CONFIG_PATH="${CC_CONFIG_PATH:-/var/lib/cc-rstationservice/CC-rStationService.toml}"
STATION_ID="${CC_STATION_ID:-${HOSTNAME:-iot-device}}"
BROKER_HOST="${CC_MQTT_BROKER_HOST:-mosquitto}"
BROKER_PORT="${CC_MQTT_BROKER_PORT:-1883}"
STATE_INTERVAL_SECONDS="${CC_STATE_INTERVAL_SECONDS:-5}"
AUTH_TOKEN="${CC_AGENT_AUTH_TOKEN:-iot-sim-token}"

escape_sed() {
    printf '%s' "$1" | sed 's/[|&]/\\&/g'
}

mkdir -p "$(dirname "$CONFIG_PATH")"

sed \
    -e "s|__STATION_ID__|$(escape_sed "$STATION_ID")|g" \
    -e "s|__STATE_INTERVAL_SECONDS__|$(escape_sed "$STATE_INTERVAL_SECONDS")|g" \
    -e "s|__BROKER_HOST__|$(escape_sed "$BROKER_HOST")|g" \
    -e "s|__BROKER_PORT__|$(escape_sed "$BROKER_PORT")|g" \
    -e "s|__AUTH_TOKEN__|$(escape_sed "$AUTH_TOKEN")|g" \
    "$TEMPLATE_PATH" >"$CONFIG_PATH"

exec /usr/local/bin/cc-rstationservice foreground --config "$CONFIG_PATH"

