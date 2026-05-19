#!/usr/bin/env bash
# =================================================================
# CC Project - Stop All Components Script
# =================================================================
# Stops components commonly started by scripts/start-all.sh:
#   - CC-rClient
#   - Vite dev server
#   - CC-Aggregator
#   - CC-rDeviceAgent
#
# Safe defaults:
#   - Leaves Mosquitto running unless --broker is provided
#   - Leaves IoT simulation containers running unless --iot-sim is provided
#
# Usage: ./stop-all.sh [--broker] [--iot-sim] [--status]
# Set CC_RDEVICEAGENT_DIR to point at a split CC-rDeviceAgent checkout.
# =================================================================

set -euo pipefail

RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m'

REPO_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
SCRIPTS_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

CLIENT_DEBUG_BIN="$REPO_DIR/CC-rClient/src-tauri/target/debug/cc-rclient"
CLIENT_RELEASE_BIN="$REPO_DIR/CC-rClient/src-tauri/target/release/cc-rclient"
AGGREGATOR_DEBUG_BIN="$REPO_DIR/CC-Aggregator/target/debug/cc-aggregator"
AGGREGATOR_RELEASE_BIN="$REPO_DIR/CC-Aggregator/target/release/cc-aggregator"

resolve_station_dir() {
    local configured_dir="${CC_RDEVICEAGENT_DIR:-${CC_RSTATIONSERVICE_DIR:-}}"
    local candidate

    if [[ -n "$configured_dir" ]]; then
        candidate="$configured_dir"
    elif [[ -f "$REPO_DIR/CC-rDeviceAgent/Cargo.toml" ]]; then
        candidate="$REPO_DIR/CC-rDeviceAgent"
    else
        candidate="$(cd "$REPO_DIR/.." && pwd)/CC-rDeviceAgent"
    fi

    cd "$candidate" 2>/dev/null && pwd || printf '%s' "$candidate"
}

DEVICEAGENT_DIR="$(resolve_station_dir)"
DEVICEAGENT_DEBUG_BIN="$DEVICEAGENT_DIR/target/debug/cc-rdeviceagent"
DEVICEAGENT_RELEASE_BIN="$DEVICEAGENT_DIR/target/release/cc-rdeviceagent"

STOP_BROKER=0
STOP_IOT_SIM=0
STATUS_ONLY=0

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
    echo -e "${RED}[ERROR]${NC} $1"
}

usage() {
    cat <<EOF
Usage: ./stop-all.sh [--broker] [--iot-sim] [--status]

Options:
  --broker   Also stop Docker container named "mosquitto"
  --iot-sim  Also stop IoT simulation containers via ./start-iot-sim.sh --stop
  --status   Show which components appear to be running
  -h, --help Show this help message
EOF
}

is_process_running() {
    local pid=$1
    kill -0 "$pid" 2>/dev/null
}

collect_matching_pids() {
    local pattern=$1
    pgrep -f "$pattern" 2>/dev/null || true
}

append_unique_pids() {
    local pid
    for pid in "$@"; do
        [[ -n "$pid" ]] || continue
        if [[ " ${MATCHED_PIDS[*]:-} " != *" ${pid} "* ]]; then
            MATCHED_PIDS+=("$pid")
        fi
    done
}

stop_pids() {
    local label=$1
    shift
    local pids=("$@")
    local pid
    local stopped_any=0

    if [[ ${#pids[@]} -eq 0 ]]; then
        log_info "${label} is not running"
        return 0
    fi

    log_info "Stopping ${label}: ${pids[*]}"
    for pid in "${pids[@]}"; do
        if is_process_running "$pid"; then
            kill "$pid" 2>/dev/null || true
            stopped_any=1
        fi
    done

    for _ in $(seq 1 20); do
        local still_running=0
        for pid in "${pids[@]}"; do
            if is_process_running "$pid"; then
                still_running=1
                break
            fi
        done
        if [[ $still_running -eq 0 ]]; then
            log_ok "${label} stopped"
            return 0
        fi
        sleep 0.2
    done

    for pid in "${pids[@]}"; do
        if is_process_running "$pid"; then
            log_warn "${label} did not exit after SIGTERM, sending SIGKILL to PID ${pid}"
            kill -9 "$pid" 2>/dev/null || true
        fi
    done

    if [[ $stopped_any -eq 1 ]]; then
        log_ok "${label} stopped"
    else
        log_info "${label} is not running"
    fi
}

find_component_pids() {
    MATCHED_PIDS=()
    local pattern
    for pattern in "$@"; do
        append_unique_pids $(collect_matching_pids "$pattern")
    done
}

show_status() {
    echo ""
    echo "=========================================="
    echo "  CC Project - Stop Status"
    echo "=========================================="

    find_component_pids "$CLIENT_DEBUG_BIN" "$CLIENT_RELEASE_BIN"
    if [[ ${#MATCHED_PIDS[@]} -gt 0 ]]; then
        log_ok "CC-rClient              - Running (${MATCHED_PIDS[*]})"
    else
        log_info "CC-rClient              - Not running"
    fi

    find_component_pids "$REPO_DIR/CC-rClient.*vite" "$REPO_DIR/CC-rClient.*npm run dev"
    if [[ ${#MATCHED_PIDS[@]} -gt 0 ]]; then
        log_ok "Vite dev server         - Running (${MATCHED_PIDS[*]})"
    else
        log_info "Vite dev server         - Not running"
    fi

    find_component_pids "$AGGREGATOR_DEBUG_BIN" "$AGGREGATOR_RELEASE_BIN"
    if [[ ${#MATCHED_PIDS[@]} -gt 0 ]]; then
        log_ok "CC-Aggregator           - Running (${MATCHED_PIDS[*]})"
    else
        log_info "CC-Aggregator           - Not running"
    fi

    find_component_pids "$DEVICEAGENT_DEBUG_BIN.*foreground" "$DEVICEAGENT_RELEASE_BIN.*foreground"
    if [[ ${#MATCHED_PIDS[@]} -gt 0 ]]; then
        log_ok "CC-rDeviceAgent      - Running (${MATCHED_PIDS[*]})"
    else
        log_info "CC-rDeviceAgent      - Not running"
    fi

    if docker ps --filter "name=mosquitto" --filter "status=running" -q | grep -q .; then
        log_ok "Mosquitto container     - Running"
    else
        log_info "Mosquitto container     - Not running"
    fi

    if [[ -f "$REPO_DIR/logs/iot-sim/docker-compose.generated.yml" ]]; then
        log_info "IoT simulation status:"
        docker compose -p "cc-iot-sim" -f "$REPO_DIR/logs/iot-sim/docker-compose.generated.yml" ps || true
    fi

    echo "=========================================="
}

stop_host_components() {
    find_component_pids "$CLIENT_DEBUG_BIN" "$CLIENT_RELEASE_BIN"
    stop_pids "CC-rClient" "${MATCHED_PIDS[@]}"

    find_component_pids "$REPO_DIR/CC-rClient.*vite" "$REPO_DIR/CC-rClient.*npm run dev"
    stop_pids "Vite dev server" "${MATCHED_PIDS[@]}"

    find_component_pids "$AGGREGATOR_DEBUG_BIN" "$AGGREGATOR_RELEASE_BIN"
    stop_pids "CC-Aggregator" "${MATCHED_PIDS[@]}"

    find_component_pids "$DEVICEAGENT_DEBUG_BIN.*foreground" "$DEVICEAGENT_RELEASE_BIN.*foreground"
    stop_pids "CC-rDeviceAgent" "${MATCHED_PIDS[@]}"
}

stop_iot_sim() {
    if [[ $STOP_IOT_SIM -ne 1 ]]; then
        return 0
    fi

    log_info "Stopping IoT simulation containers"
    "$SCRIPTS_DIR/start-iot-sim.sh" --stop || log_warn "IoT simulation stop command returned a non-zero status"
}

stop_broker() {
    if [[ $STOP_BROKER -ne 1 ]]; then
        return 0
    fi

    if docker ps --filter "name=mosquitto" --filter "status=running" -q | grep -q .; then
        log_info "Stopping mosquitto container"
        docker stop mosquitto >/dev/null
        log_ok "Mosquitto stopped"
    else
        log_info "Mosquitto container is not running"
    fi
}

parse_args() {
    local arg
    for arg in "$@"; do
        case "$arg" in
            --broker)
                STOP_BROKER=1
                ;;
            --iot-sim)
                STOP_IOT_SIM=1
                ;;
            --status)
                STATUS_ONLY=1
                ;;
            -h|--help)
                usage
                exit 0
                ;;
            *)
                log_err "Unknown argument: $arg"
                usage
                exit 1
                ;;
        esac
    done
}

main() {
    parse_args "$@"

    if [[ $STATUS_ONLY -eq 1 ]]; then
        show_status
        exit 0
    fi

    echo ""
    echo "=========================================="
    echo "  CC Project - Stop All Script"
    echo "=========================================="

    stop_host_components
    stop_iot_sim
    stop_broker

    log_ok "Stop sequence complete"
}

main "$@"
