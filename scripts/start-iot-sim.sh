#!/usr/bin/env bash
set -euo pipefail

RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m'

log_info() {
    echo -e "${BLUE}[INFO]${NC} $1"
}

log_ok() {
    echo -e "${GREEN}[OK]${NC} $1"
}

log_warn() {
    echo -e "${YELLOW}[WARN]${NC} $1"
}

log_err() {
    echo -e "${RED}[ERR]${NC} $1" >&2
}

usage() {
    cat <<'EOF'
Usage:
  ./scripts/start-iot-sim.sh <device_count> [--dry-run]
  ./scripts/start-iot-sim.sh --status
  ./scripts/start-iot-sim.sh --stop

Examples:
  ./scripts/start-iot-sim.sh 10
  BUILD_MODE=debug ./scripts/start-iot-sim.sh 10
  BROKER_PORT=1884 STATE_INTERVAL_SECONDS=2 ./scripts/start-iot-sim.sh 25
  ./scripts/start-iot-sim.sh --status
  ./scripts/start-iot-sim.sh --stop

Environment:
  BROKER_PORT              Host MQTT port to publish (default: 1883)
  STATE_INTERVAL_SECONDS   Telemetry interval per device (default: 5)
  IMAGE_TAG                Docker image tag (default: cc-rstationservice-iot-sim:latest)
  DEVICE_PREFIX            Station ID prefix (default: iot)
  BUILD_MODE               Local cargo profile to package (default: release)
  REBUILD_BINARY           Set to 1 to rebuild locally before packaging
  STATION_BINARY_PATH      Override the binary path copied into the image
  USE_HOST_BROKER          auto, 0, or 1 (default: auto)
  HOST_BROKER_HOST         Hostname used when reusing an existing broker (default: host.docker.internal)
  HOST_BROKER_PORT         Port used when reusing an existing broker (default: BROKER_PORT)
EOF
}

require_command() {
    local command_name=$1
    if ! command -v "$command_name" >/dev/null 2>&1; then
        log_err "Required command not found: $command_name"
        exit 1
    fi
}

require_file() {
    local file_path=$1
    if [[ ! -f "$file_path" ]]; then
        log_err "Required file not found: $file_path"
        exit 1
    fi
}

validate_integer() {
    local value=$1
    local name=$2
    if ! [[ "$value" =~ ^[0-9]+$ ]] || [[ "$value" -lt 1 ]]; then
        log_err "$name must be an integer >= 1"
        exit 1
    fi
}

station_binary_needs_rebuild() {
    if [[ ! -x "$STATION_BINARY_PATH" ]]; then
        return 0
    fi

    if [[ "$STATION_DIR/Cargo.toml" -nt "$STATION_BINARY_PATH" ]] || \
       [[ "$STATION_DIR/Cargo.lock" -nt "$STATION_BINARY_PATH" ]] || \
       [[ "$STATION_DIR/build.rs" -nt "$STATION_BINARY_PATH" ]]; then
        return 0
    fi

    if find "$STATION_DIR/src" -type f -newer "$STATION_BINARY_PATH" -print -quit | grep -q .; then
        return 0
    fi

    return 1
}

is_port_listening() {
    local port=$1
    if command -v ss >/dev/null 2>&1; then
        if ss -tln 2>/dev/null | grep -q ":${port} "; then
            return 0
        fi
    fi

    if command -v netstat >/dev/null 2>&1; then
        if netstat -tln 2>/dev/null | grep -q ":${port} "; then
            return 0
        fi
    fi

    if docker ps --format '{{.Ports}}' 2>/dev/null | grep -Eq "(^|[ ,])0\\.0\\.0\\.0:${port}->|\\[::\\]:${port}->"; then
        return 0
    fi

    if command -v timeout >/dev/null 2>&1; then
        timeout 1 bash -c "echo >/dev/tcp/127.0.0.1/${port}" 2>/dev/null
        return $?
    fi

    return 1
}

prepare_image_context() {
    rm -rf "$IMAGE_CONTEXT_DIR"
    mkdir -p "$IMAGE_CONTEXT_DIR"

    install -m 755 "$STATION_BINARY_PATH" "$IMAGE_CONTEXT_DIR/cc-rstationservice"
    install -m 755 "$ENTRYPOINT_SOURCE" "$IMAGE_CONTEXT_DIR/iot-sim-entrypoint.sh"
    install -m 644 "$CONFIG_TEMPLATE_SOURCE" "$IMAGE_CONTEXT_DIR/CC-rStationService.iot.toml.template"

    if [[ "$BUILD_MODE" == "release" ]] && command -v strip >/dev/null 2>&1; then
        strip --strip-unneeded "$IMAGE_CONTEXT_DIR/cc-rstationservice" 2>/dev/null || true
    fi
}

ensure_station_binary() {
    local rebuild_reason=""

    if [[ "$REBUILD_BINARY" == "1" ]]; then
        rebuild_reason="REBUILD_BINARY=1"
    elif [[ ! -x "$STATION_BINARY_PATH" ]]; then
        rebuild_reason="binary missing"
    elif station_binary_needs_rebuild; then
        rebuild_reason="source newer than $STATION_BINARY_PATH"
    fi

    if [[ -n "$rebuild_reason" ]]; then
        require_command cargo
        log_info "Building local CC-rStationService binary (${BUILD_MODE}) (${rebuild_reason})"
        pushd "$STATION_DIR" >/dev/null
        if [[ "$BUILD_MODE" == "release" ]]; then
            cargo build --release
        else
            cargo build
        fi
        popd >/dev/null
    fi

    require_file "$STATION_BINARY_PATH"
}

is_project_broker_running() {
    if [[ ! -f "$COMPOSE_FILE" ]]; then
        return 1
    fi

    if ! command -v docker >/dev/null 2>&1; then
        return 1
    fi

    docker compose -p "$PROJECT_NAME" -f "$COMPOSE_FILE" ps --status running --services mosquitto 2>/dev/null | grep -q '^mosquitto$'
}

resolve_broker_mode() {
    ACTIVE_BROKER_HOST="mosquitto"
    ACTIVE_BROKER_PORT="1883"
    BROKER_MODE="embedded"

    case "$USE_HOST_BROKER" in
        1|true|TRUE|yes|YES)
            BROKER_MODE="host"
            ;;
        0|false|FALSE|no|NO)
            BROKER_MODE="embedded"
            ;;
        auto|AUTO)
            if is_port_listening "$BROKER_PORT"; then
                if is_project_broker_running; then
                    log_info "Port ${BROKER_PORT} is occupied by the existing ${PROJECT_NAME} broker; keeping embedded broker mode"
                else
                    BROKER_MODE="host"
                    log_warn "Port ${BROKER_PORT} is already in use; reusing the host MQTT broker"
                fi
            fi
            ;;
        *)
            log_err "USE_HOST_BROKER must be one of: auto, 0, 1"
            exit 1
            ;;
    esac

    if [[ "$BROKER_MODE" == "host" ]]; then
        ACTIVE_BROKER_HOST="$HOST_BROKER_HOST"
        ACTIVE_BROKER_PORT="$HOST_BROKER_PORT"
        validate_integer "$ACTIVE_BROKER_PORT" "HOST_BROKER_PORT"
    fi
}

render_compose_file() {
    mkdir -p "$SIM_DIR"

    cat >"$COMPOSE_FILE" <<EOF
services:
EOF

    if [[ "$BROKER_MODE" == "embedded" ]]; then
        cat >>"$COMPOSE_FILE" <<EOF
  mosquitto:
    image: eclipse-mosquitto:2
    container_name: ${PROJECT_NAME}-mosquitto
    command: ["mosquitto", "-c", "/mosquitto/config/mosquitto.conf"]
    ports:
      - "${BROKER_PORT}:1883"
    volumes:
      - ${MOSQUITTO_CONF}:/mosquitto/config/mosquitto.conf:ro
    restart: unless-stopped
EOF
    fi

    local device_number
    local station_id
    for device_number in $(seq 1 "$DEVICE_COUNT"); do
        station_id=$(printf "%s-%03d" "$DEVICE_PREFIX" "$device_number")
        if [[ "$BROKER_MODE" == "embedded" ]]; then
            cat >>"$COMPOSE_FILE" <<EOF
  ${station_id}:
    image: ${IMAGE_TAG}
    container_name: ${PROJECT_NAME}-${station_id}
    depends_on:
      - mosquitto
    environment:
      CC_STATION_ID: ${station_id}
      CC_MQTT_BROKER_HOST: ${ACTIVE_BROKER_HOST}
      CC_MQTT_BROKER_PORT: "${ACTIVE_BROKER_PORT}"
      CC_STATE_INTERVAL_SECONDS: "${STATE_INTERVAL_SECONDS}"
    restart: unless-stopped
EOF
        elif [[ "$ACTIVE_BROKER_HOST" == "host.docker.internal" ]]; then
            cat >>"$COMPOSE_FILE" <<EOF
  ${station_id}:
    image: ${IMAGE_TAG}
    container_name: ${PROJECT_NAME}-${station_id}
    extra_hosts:
      - "host.docker.internal:host-gateway"
    environment:
      CC_STATION_ID: ${station_id}
      CC_MQTT_BROKER_HOST: ${ACTIVE_BROKER_HOST}
      CC_MQTT_BROKER_PORT: "${ACTIVE_BROKER_PORT}"
      CC_STATE_INTERVAL_SECONDS: "${STATE_INTERVAL_SECONDS}"
    restart: unless-stopped
EOF
        else
            cat >>"$COMPOSE_FILE" <<EOF
  ${station_id}:
    image: ${IMAGE_TAG}
    container_name: ${PROJECT_NAME}-${station_id}
    environment:
      CC_STATION_ID: ${station_id}
      CC_MQTT_BROKER_HOST: ${ACTIVE_BROKER_HOST}
      CC_MQTT_BROKER_PORT: "${ACTIVE_BROKER_PORT}"
      CC_STATE_INTERVAL_SECONDS: "${STATE_INTERVAL_SECONDS}"
    restart: unless-stopped
EOF
        fi
    done
}

show_status() {
    if [[ -f "$COMPOSE_FILE" ]]; then
        docker compose -p "$PROJECT_NAME" -f "$COMPOSE_FILE" ps
        return 0
    fi

    log_warn "No generated compose file found at $COMPOSE_FILE"
    log_info "Falling back to docker ps project filter"
    docker ps --filter "label=com.docker.compose.project=$PROJECT_NAME"
}

stop_simulation() {
    if [[ ! -f "$COMPOSE_FILE" ]]; then
        log_warn "No generated compose file found at $COMPOSE_FILE"
        log_warn "Nothing to stop for project $PROJECT_NAME"
        return 0
    fi

    docker compose -p "$PROJECT_NAME" -f "$COMPOSE_FILE" down --remove-orphans
    log_ok "IoT simulation stopped"
}

build_image() {
    log_info "Building ${IMAGE_TAG}"
    ensure_station_binary
    prepare_image_context
    docker build \
        -f "$DOCKERFILE_PATH" \
        -t "$IMAGE_TAG" \
        "$IMAGE_CONTEXT_DIR"
    log_ok "Built ${IMAGE_TAG}"
}

start_simulation() {
    validate_integer "$DEVICE_COUNT" "device_count"
    validate_integer "$BROKER_PORT" "BROKER_PORT"
    validate_integer "$STATE_INTERVAL_SECONDS" "STATE_INTERVAL_SECONDS"
    resolve_broker_mode

    render_compose_file

    log_info "Generated compose file: $COMPOSE_FILE"

    if [[ "$DRY_RUN" == "1" ]]; then
        log_ok "Dry run complete"
        return 0
    fi

    build_image

    if [[ "$BROKER_MODE" == "embedded" ]]; then
        log_info "Starting embedded MQTT broker and ${DEVICE_COUNT} IoT devices"
    else
        log_info "Starting ${DEVICE_COUNT} IoT devices against host broker ${ACTIVE_BROKER_HOST}:${ACTIVE_BROKER_PORT}"
    fi
    docker compose -p "$PROJECT_NAME" -f "$COMPOSE_FILE" up -d --remove-orphans --force-recreate
    log_ok "IoT simulation started"
    show_status
}

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO_DIR="$(cd "$SCRIPT_DIR/.." && pwd)"
STATION_DIR="$REPO_DIR/CC-rStationService"
SIM_DIR="$REPO_DIR/logs/iot-sim"
COMPOSE_FILE="$SIM_DIR/docker-compose.generated.yml"
PROJECT_NAME="cc-iot-sim"
IMAGE_TAG="${IMAGE_TAG:-cc-rstationservice-iot-sim:latest}"
BROKER_PORT="${BROKER_PORT:-1883}"
STATE_INTERVAL_SECONDS="${STATE_INTERVAL_SECONDS:-5}"
DEVICE_PREFIX="${DEVICE_PREFIX:-iot}"
BUILD_MODE="${BUILD_MODE:-release}"
REBUILD_BINARY="${REBUILD_BINARY:-0}"
STATION_BINARY_PATH="${STATION_BINARY_PATH:-$STATION_DIR/target/$BUILD_MODE/cc-rstationservice}"
USE_HOST_BROKER="${USE_HOST_BROKER:-auto}"
HOST_BROKER_HOST="${HOST_BROKER_HOST:-host.docker.internal}"
HOST_BROKER_PORT="${HOST_BROKER_PORT:-$BROKER_PORT}"
BROKER_MODE="embedded"
ACTIVE_BROKER_HOST="mosquitto"
ACTIVE_BROKER_PORT="1883"
MOSQUITTO_CONF="$SCRIPT_DIR/mosquitto.iot-sim.conf"
DOCKERFILE_PATH="$STATION_DIR/packaging/docker/Dockerfile.iot-sim"
ENTRYPOINT_SOURCE="$STATION_DIR/packaging/docker/iot-sim-entrypoint.sh"
CONFIG_TEMPLATE_SOURCE="$STATION_DIR/packaging/docker/CC-rStationService.iot.toml.template"
IMAGE_CONTEXT_DIR="$SIM_DIR/image-context"
DRY_RUN=0
ACTION="start"
DEVICE_COUNT=""

main() {
    require_command docker
    require_file "$MOSQUITTO_CONF"
    require_file "$DOCKERFILE_PATH"
    require_file "$ENTRYPOINT_SOURCE"
    require_file "$CONFIG_TEMPLATE_SOURCE"

    if [[ $# -eq 0 ]]; then
        usage
        exit 1
    fi

    local arg
    for arg in "$@"; do
        case "$arg" in
            --stop)
                ACTION="stop"
                ;;
            --status)
                ACTION="status"
                ;;
            --dry-run)
                DRY_RUN=1
                ;;
            -h|--help)
                usage
                exit 0
                ;;
            *)
                if [[ -z "$DEVICE_COUNT" ]] && [[ "$arg" =~ ^[0-9]+$ ]]; then
                    DEVICE_COUNT="$arg"
                else
                    log_err "Unknown argument: $arg"
                    usage
                    exit 1
                fi
                ;;
        esac
    done

    case "$ACTION" in
        start)
            if [[ -z "$DEVICE_COUNT" ]]; then
                log_err "device_count is required for start"
                usage
                exit 1
            fi
            start_simulation
            ;;
        stop)
            stop_simulation
            ;;
        status)
            show_status
            ;;
        *)
            log_err "Unsupported action: $ACTION"
            exit 1
            ;;
    esac
}

main "$@"
