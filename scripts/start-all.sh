#!/usr/bin/env bash
# =================================================================
# CC Project - Start All Components Script
# =================================================================
# This script starts all CC project components in the correct order:
#   1. Mosquitto MQTT Broker (if not running)
#   2. CC-rStationService (workstation service)
#   3. CC-Aggregator (MQTT to WebSocket data aggregator)
#   4. CC-rClient (Tauri frontend application)
#
# Usage: ./start-all.sh [debug|release] [--status]
#   Default mode: release
#   Press Ctrl+C to stop all components
# =================================================================

set -euo pipefail

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Paths
REPO_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
SCRIPTS_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

# Build mode (default: release)
BUILD_MODE="release"

# Project directories
STATIONSERVICE_DIR="$REPO_DIR/CC-rStationService"
AGGREGATOR_DIR="$REPO_DIR/CC-Aggregator"
CLIENT_DIR="$REPO_DIR/CC-rClient"

# Binaries (resolved by mode)
STATIONSERVICE_BIN=""
AGGREGATOR_BIN=""
CLIENT_BIN=""

# Logs
LOG_DIR="$REPO_DIR/logs"
STATIONSERVICE_LOG="$LOG_DIR/rstationservice.log"
AGGREGATOR_LOG="$LOG_DIR/aggregator.log"
CLIENT_LOG="$LOG_DIR/rclient.log"
IOT_SIM_COMPOSE_FILE="$LOG_DIR/iot-sim/docker-compose.generated.yml"

# Ports
MQTT_PORT=1883
STATIONSERVICE_PORT=50051
AGGREGATOR_PORT=8080

# PIDs (empty means not started by this script)
STATIONSERVICE_PID=""
AGGREGATOR_PID=""
CLIENT_PID=""

# =================================================================
# Helper Functions
# =================================================================

log_info() {
    echo -e "${BLUE}[INFO]${NC} $1"
}

log_success() {
    echo -e "${GREEN}[OK]${NC} $1"
}

log_warning() {
    echo -e "${YELLOW}[WARN]${NC} $1"
}

log_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

# Check if a port is in use
is_port_listening() {
    local port=$1
    if command -v ss &> /dev/null; then
        ss -tlnp 2>/dev/null | grep -q ":${port} "
    elif command -v netstat &> /dev/null; then
        netstat -tlnp 2>/dev/null | grep -q ":${port} "
    else
        # Fallback: try to connect
        timeout 1 bash -c "echo >/dev/tcp/127.0.0.1/${port}" 2>/dev/null
    fi
}

# Check if Mosquitto container is running
is_mosquitto_running() {
    docker ps --filter "name=mosquitto" --filter "status=running" -q | grep -q .
}

# Check if a Mosquitto container exists in any state
is_mosquitto_container_exists() {
    docker ps -a --filter "name=mosquitto" -q | grep -q .
}

is_iot_sim_running() {
    if [[ ! -f "$IOT_SIM_COMPOSE_FILE" ]]; then
        return 1
    fi

    docker compose -p "cc-iot-sim" -f "$IOT_SIM_COMPOSE_FILE" ps --status running --services 2>/dev/null | grep -q .
}

# Check if a process with a specific PID is running
is_process_running() {
    local pid=$1
    kill -0 "$pid" 2>/dev/null
}

rust_binary_needs_rebuild() {
    local project_dir=$1
    local binary_path=$2

    if [[ ! -x "$binary_path" ]]; then
        return 0
    fi

    if [[ "$project_dir/Cargo.toml" -nt "$binary_path" ]] || \
       [[ "$project_dir/Cargo.lock" -nt "$binary_path" ]] || \
       [[ "$project_dir/build.rs" -nt "$binary_path" ]]; then
        return 0
    fi

    if find "$project_dir/src" -type f -newer "$binary_path" -print -quit | grep -q .; then
        return 0
    fi

    return 1
}

ensure_rust_binary() {
    local name=$1
    local project_dir=$2
    local binary_path=$3

    if [[ ! -x "$binary_path" ]] || rust_binary_needs_rebuild "$project_dir" "$binary_path"; then
        log_info "Building ${name} (${BUILD_MODE}) before startup"
        pushd "$project_dir" >/dev/null
        if [[ "$BUILD_MODE" == "release" ]]; then
            cargo build --release
        else
            cargo build
        fi
        popd >/dev/null
    fi

    if [[ ! -x "$binary_path" ]]; then
        log_error "${name} binary not found or not executable after build: $binary_path"
        return 1
    fi
}

# Configure binary paths by build mode
configure_binary_paths() {
    STATIONSERVICE_BIN="$STATIONSERVICE_DIR/target/$BUILD_MODE/cc-rstationservice"
    AGGREGATOR_BIN="$AGGREGATOR_DIR/target/$BUILD_MODE/cc-aggregator"
    CLIENT_BIN="$CLIENT_DIR/src-tauri/target/$BUILD_MODE/cc-rclient"
}

print_stop_commands() {
    echo "Stop command:"
    echo "  - ./scripts/stop-all.sh"

    if is_iot_sim_running; then
        echo "  - ./scripts/stop-all.sh --iot-sim"

        if is_mosquitto_running; then
            echo "  - ./scripts/stop-all.sh --iot-sim --broker"
        fi
    elif is_mosquitto_running; then
        echo "  - ./scripts/stop-all.sh --broker"
    fi
}

parse_args() {
    local arg
    for arg in "$@"; do
        case "$arg" in
            debug|release)
                BUILD_MODE="$arg"
                ;;
            --status)
                # handled in main after binary path setup
                ;;
            -h|--help)
                echo "Usage: ./start-all.sh [debug|release] [--status]"
                echo ""
                echo "Options:"
                echo "  debug      Run debug binaries"
                echo "  release    Run release binaries (default)"
                echo "  --status   Show component status and exit"
                echo "  -h, --help Show this help message"
                exit 0
                ;;
            *)
                log_error "Unknown argument: $arg"
                echo "Usage: ./start-all.sh [debug|release] [--status]"
                exit 1
                ;;
        esac
    done
}

# =================================================================
# Component Check Functions
# =================================================================

check_mosquitto() {
    if is_mosquitto_running; then
        return 0  # Running
    else
        return 1  # Not running
    fi
}

check_stationservice() {
    if is_port_listening $STATIONSERVICE_PORT; then
        return 0  # Running
    else
        return 1  # Not running
    fi
}

check_aggregator() {
    if is_port_listening $AGGREGATOR_PORT; then
        return 0  # Running
    else
        return 1  # Not running
    fi
}

check_client() {
    # Client is a GUI app, we just check if it was started by us
    if [[ -n "$CLIENT_PID" ]] && is_process_running "$CLIENT_PID"; then
        return 0
    else
        return 1
    fi
}

# =================================================================
# Component Start Functions
# =================================================================

start_mosquitto() {
    log_info "Starting Mosquitto MQTT Broker..."
    
    if check_mosquitto; then
        log_success "Mosquitto is already running (Docker container)"
        return 0
    fi

    # If an old mosquitto container exists but is stopped/exited, remove it first
    if is_mosquitto_container_exists; then
        local mosquitto_status
        mosquitto_status="$(docker inspect -f '{{.State.Status}}' mosquitto 2>/dev/null || true)"
        if [[ "$mosquitto_status" != "running" ]]; then
            log_warning "Found existing mosquitto container in status: ${mosquitto_status:-unknown}; removing it before restart"
            docker rm -f mosquitto >/dev/null 2>&1 || true
        fi
    fi
    
    # Try to start Mosquitto using docker-compose or docker run
    if [[ -f "$REPO_DIR/docker-compose.yml" ]] || [[ -f "$REPO_DIR/docker-compose.yaml" ]]; then
        cd "$REPO_DIR"
        if docker-compose up -d mosquitto 2>/dev/null || docker compose up -d mosquitto 2>/dev/null; then
            sleep 2
            if check_mosquitto; then
                log_success "Mosquitto started via docker-compose"
                return 0
            fi
        fi
    fi
    
    # Fallback: try to start directly with docker
    if docker run -d --name mosquitto \
        -p 1883:1883 -p 9001:9001 \
        eclipse-mosquitto:latest \
        >/dev/null 2>&1; then
        sleep 2
        if check_mosquitto; then
            log_success "Mosquitto started via docker run"
            return 0
        fi
    fi
    
    log_error "Failed to start Mosquitto. Please install and start it manually."
    return 1
}

start_stationservice() {
    log_info "Starting CC-rStationService..."
    
    if check_stationservice; then
        log_success "CC-rStationService is already running (port $STATIONSERVICE_PORT)"
        return 0
    fi
    
    # Check if binary exists
    if ! ensure_rust_binary "CC-rStationService" "$STATIONSERVICE_DIR" "$STATIONSERVICE_BIN"; then
        return 1
    fi
    
    # Create log directory
    mkdir -p "$LOG_DIR"
    
    # Start in foreground mode (background)
    cd "$STATIONSERVICE_DIR"
    nohup "$STATIONSERVICE_BIN" foreground --config "$STATIONSERVICE_DIR/CC-rStationService.toml" >"$STATIONSERVICE_LOG" 2>&1 &
    STATIONSERVICE_PID=$!
    
    # Wait for service to be ready
    for i in $(seq 1 30); do
        if check_stationservice; then
            log_success "CC-rStationService started (PID: $STATIONSERVICE_PID)"
            log_info "  Log file: $STATIONSERVICE_LOG"
            return 0
        fi
        sleep 0.5
    done
    
    log_error "CC-rStationService failed to start. Check log: $STATIONSERVICE_LOG"
    return 1
}

start_aggregator() {
    log_info "Starting CC-Aggregator..."
    
    if check_aggregator; then
        log_success "CC-Aggregator is already running (port $AGGREGATOR_PORT)"
        return 0
    fi
    
    # Check if binary exists
    if ! ensure_rust_binary "CC-Aggregator" "$AGGREGATOR_DIR" "$AGGREGATOR_BIN"; then
        return 1
    fi
    
    # Create log directory
    mkdir -p "$LOG_DIR"
    
    # Start in background
    cd "$AGGREGATOR_DIR"
    nohup "$AGGREGATOR_BIN" --config "$AGGREGATOR_DIR/CC-Aggregator.toml" >"$AGGREGATOR_LOG" 2>&1 &
    AGGREGATOR_PID=$!
    
    # Wait for service to be ready
    for i in $(seq 1 30); do
        if check_aggregator; then
            log_success "CC-Aggregator started (PID: $AGGREGATOR_PID)"
            log_info "  Log file: $AGGREGATOR_LOG"
            return 0
        fi
        sleep 0.5
    done
    
    log_error "CC-Aggregator failed to start. Check log: $AGGREGATOR_LOG"
    return 1
}

start_client() {
    if [[ "$BUILD_MODE" == "debug" ]]; then
        log_info "Starting CC-rClient with npm run tauri:dev..."

        mkdir -p "$LOG_DIR"
        cd "$CLIENT_DIR"
        if [[ ! -d node_modules ]]; then
            log_info "Installing CC-rClient npm dependencies..."
            npm install
        fi

        npm run tauri:dev >"$CLIENT_LOG" 2>&1 &
        CLIENT_PID=$!

        sleep 5
        if is_process_running "$CLIENT_PID"; then
            log_success "CC-rClient dev runner started (PID: $CLIENT_PID)"
            log_info "  Log file: $CLIENT_LOG"
            return 0
        fi

        log_error "CC-rClient dev runner failed to start. Check log: $CLIENT_LOG"
        return 1
    fi

    log_info "Building CC-rClient release bundle with npm run tauri:build..."
    mkdir -p "$LOG_DIR"
    cd "$CLIENT_DIR"
    if [[ ! -d node_modules ]]; then
        log_info "Installing CC-rClient npm dependencies..."
        npm install
    fi
    npm run tauri:build

    if [[ ! -x "$CLIENT_BIN" ]]; then
        log_error "CC-rClient binary not found or not executable after npm run tauri:build: $CLIENT_BIN"
        return 1
    fi

    log_info "Starting CC-rClient release binary..."
    "$CLIENT_BIN" >"$CLIENT_LOG" 2>&1 &
    CLIENT_PID=$!

    sleep 2

    if is_process_running "$CLIENT_PID"; then
        log_success "CC-rClient started (PID: $CLIENT_PID)"
        log_info "  Log file: $CLIENT_LOG"
        return 0
    else
        log_error "CC-rClient failed to start. Check log: $CLIENT_LOG"
        return 1
    fi
}

# =================================================================
# Stop Functions
# =================================================================

stop_stationservice() {
    if [[ -n "$STATIONSERVICE_PID" ]] && is_process_running "$STATIONSERVICE_PID"; then
        log_info "Stopping CC-rStationService (PID: $STATIONSERVICE_PID)..."
        kill "$STATIONSERVICE_PID" 2>/dev/null || true
        wait "$STATIONSERVICE_PID" 2>/dev/null || true
        log_success "CC-rStationService stopped"
    fi
}

stop_aggregator() {
    if [[ -n "$AGGREGATOR_PID" ]] && is_process_running "$AGGREGATOR_PID"; then
        log_info "Stopping CC-Aggregator (PID: $AGGREGATOR_PID)..."
        kill "$AGGREGATOR_PID" 2>/dev/null || true
        wait "$AGGREGATOR_PID" 2>/dev/null || true
        log_success "CC-Aggregator stopped"
    fi
}

stop_client() {
    if [[ -n "$CLIENT_PID" ]] && is_process_running "$CLIENT_PID"; then
        log_info "Stopping CC-rClient (PID: $CLIENT_PID)..."
        kill "$CLIENT_PID" 2>/dev/null || true
        wait "$CLIENT_PID" 2>/dev/null || true
        log_success "CC-rClient stopped"
    fi
}

# =================================================================
# Cleanup Function
# =================================================================

cleanup() {
    echo ""
    log_warning "Received interrupt signal. Stopping all components..."
    
    # Stop in reverse order
    stop_client
    stop_aggregator
    stop_stationservice
    
    log_success "All components stopped."
    exit 0
}

# =================================================================
# Status Function
# =================================================================

show_status() {
    echo ""
    echo "=========================================="
    echo "  CC Project - Component Status"
    echo "=========================================="
    echo "  Build mode: $BUILD_MODE"
    echo ""
    
    # Mosquitto
    if check_mosquitto; then
        log_success "Mosquitto MQTT Broker    - Running"
    else
        log_error   "Mosquitto MQTT Broker    - Not running"
    fi
    
    # StationService
    if check_stationservice; then
        log_success "CC-rStationService       - Running (port $STATIONSERVICE_PORT)"
    else
        log_error   "CC-rStationService       - Not running"
    fi
    
    # Aggregator
    if check_aggregator; then
        log_success "CC-Aggregator            - Running (port $AGGREGATOR_PORT)"
    else
        log_error   "CC-Aggregator            - Not running"
    fi
    
    echo ""
    echo "Log files:"
    echo "  StationService: $STATIONSERVICE_LOG"
    echo "  Aggregator:     $AGGREGATOR_LOG"
    echo "  Client:         $CLIENT_LOG"
    echo "=========================================="
}

# =================================================================
# Main
# =================================================================

main() {
    echo ""
    echo "=========================================="
    echo "  CC Project - Start All Script"
    echo "=========================================="
    echo ""
    
    # Check for --status flag
    parse_args "$@"
    configure_binary_paths

    if [[ " $* " == *" --status "* ]]; then
        show_status
        exit 0
    fi

    log_info "Using build mode: $BUILD_MODE"
    
    # Ensure log directory exists
    mkdir -p "$LOG_DIR"
    
    # Set trap for Ctrl+C
    trap cleanup SIGINT SIGTERM
    
    local failed=0
    
    # 1. Start Mosquitto
    if ! start_mosquitto; then
        log_error "Failed to start Mosquitto"
        failed=1
    fi
    
    # 2. Start StationService
    if ! start_stationservice; then
        log_error "Failed to start CC-rStationService"
        failed=1
    fi
    
    # 3. Start Aggregator
    if ! start_aggregator; then
        log_error "Failed to start CC-Aggregator"
        failed=1
    fi
    
    # 4. Start Client
    if ! start_client; then
        log_error "Failed to start CC-rClient"
        failed=1
    fi
    
    if [[ $failed -eq 1 ]]; then
        echo ""
        log_error "Some components failed to start. Check logs for details."
        show_status
        exit 1
    fi
    
    echo ""
    echo "=========================================="
    log_success "All components started successfully!"
    echo "=========================================="
    echo ""
    echo "Component endpoints:"
    echo "  - Mosquitto:     localhost:$MQTT_PORT (MQTT)"
    echo "  - StationService: localhost:$STATIONSERVICE_PORT (gRPC control)"
    echo "  - Aggregator:    localhost:$AGGREGATOR_PORT (WebSocket)"
    echo "  - Client:        Tauri GUI window (bundled CC-rClient/dist assets)"
    echo ""
    echo "Log files:"
    echo "  - StationService: $STATIONSERVICE_LOG"
    echo "  - Aggregator:     $AGGREGATOR_LOG"
    echo "  - Client:         $CLIENT_LOG"
    echo ""
    print_stop_commands
    echo ""
    echo "Press Ctrl+C to stop all components."
    echo "=========================================="
    
    # Wait for any background process to exit
    # Keep the script running to handle Ctrl+C
    while true; do
        sleep 1
        # Check if client is still running
        if ! check_client; then
            log_warning "CC-rClient has exited"
            break
        fi
    done
}

main "$@"
