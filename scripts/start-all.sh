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
#   Default mode: debug
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

# Build mode (default: debug)
BUILD_MODE="debug"

# Binaries (resolved by mode)
STATIONSERVICE_BIN=""
AGGREGATOR_BIN=""
CLIENT_BIN=""

# Logs
LOG_DIR="$REPO_DIR/logs"
STATIONSERVICE_LOG="$LOG_DIR/rstationservice.log"
AGGREGATOR_LOG="$LOG_DIR/aggregator.log"
VITE_LOG="$LOG_DIR/vite.log"
CLIENT_LOG="$LOG_DIR/rclient.log"

# Ports
MQTT_PORT=1883
STATIONSERVICE_PORT=50051
AGGREGATOR_PORT=8080
VITE_PORT=5173

# PIDs (empty means not started by this script)
STATIONSERVICE_PID=""
AGGREGATOR_PID=""
VITE_PID=""
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

# Check if a process with a specific PID is running
is_process_running() {
    local pid=$1
    kill -0 "$pid" 2>/dev/null
}

# Configure binary paths by build mode
configure_binary_paths() {
    STATIONSERVICE_BIN="$REPO_DIR/CC-rStationService/target/$BUILD_MODE/cc-rstationservice"
    AGGREGATOR_BIN="$REPO_DIR/CC-Aggregator/target/$BUILD_MODE/cc-aggregator"
    CLIENT_BIN="$REPO_DIR/CC-rClient/src-tauri/target/$BUILD_MODE/cc-rclient"
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
                echo "  debug      Run debug binaries (default)"
                echo "  release    Run release binaries"
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
    if [[ ! -x "$STATIONSERVICE_BIN" ]]; then
        log_error "CC-rStationService binary not found or not executable: $STATIONSERVICE_BIN"
        log_error "Please build the project first: cd $REPO_DIR/CC-rStationService && $( [[ \"$BUILD_MODE\" == \"release\" ]] && echo \"cargo build --release\" || echo \"cargo build\" )"
        return 1
    fi
    
    # Create log directory
    mkdir -p "$LOG_DIR"
    
    # Start in foreground mode (background)
    cd "$REPO_DIR/CC-rStationService"
    nohup "$STATIONSERVICE_BIN" foreground --config "$REPO_DIR/CC-rStationService/CC-rStationService.toml" >"$STATIONSERVICE_LOG" 2>&1 &
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
    if [[ ! -x "$AGGREGATOR_BIN" ]]; then
        log_error "CC-Aggregator binary not found or not executable: $AGGREGATOR_BIN"
        log_error "Please build the project first: cd $REPO_DIR/CC-Aggregator && $( [[ \"$BUILD_MODE\" == \"release\" ]] && echo \"cargo build --release\" || echo \"cargo build\" )"
        return 1
    fi
    
    # Create log directory
    mkdir -p "$LOG_DIR"
    
    # Start in background
    cd "$REPO_DIR/CC-Aggregator"
    nohup "$AGGREGATOR_BIN" --config "$REPO_DIR/CC-Aggregator/CC-Aggregator.toml" >"$AGGREGATOR_LOG" 2>&1 &
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
        log_info "Starting Vite frontend dev server..."
    else
        log_info "Release mode detected, skipping Vite dev server"
    fi

    # Check if binary exists
    if [[ ! -x "$CLIENT_BIN" ]]; then
        log_error "CC-rClient binary not found or not executable: $CLIENT_BIN"
        log_error "Please build the project first: cd $REPO_DIR/CC-rClient/src-tauri && $( [[ \"$BUILD_MODE\" == \"release\" ]] && echo \"cargo build --release\" || echo \"cargo build\" )"
        return 1
    fi

    # Create log directory
    mkdir -p "$LOG_DIR"

    if [[ "$BUILD_MODE" == "debug" ]]; then
        # Debug binary loads frontend from localhost:5173
        cd "$REPO_DIR/CC-rClient"
        nohup npm run dev >"$VITE_LOG" 2>&1 &
        VITE_PID=$!

        # Wait for Vite to be ready
        for i in $(seq 1 30); do
            if is_port_listening $VITE_PORT; then
                log_success "Vite dev server ready (PID: $VITE_PID, port $VITE_PORT)"
                break
            fi
            sleep 0.5
            if [[ $i -eq 30 ]]; then
                log_error "Vite dev server failed to start. Check log: $VITE_LOG"
                return 1
            fi
        done
    fi

    log_info "Starting CC-rClient (Tauri application)..."

    # Start the Tauri application
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

stop_vite() {
    if [[ -n "$VITE_PID" ]] && is_process_running "$VITE_PID"; then
        log_info "Stopping Vite dev server (PID: $VITE_PID)..."
        kill "$VITE_PID" 2>/dev/null || true
        wait "$VITE_PID" 2>/dev/null || true
        log_success "Vite dev server stopped"
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
    stop_vite
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
    echo "  Vite:           $VITE_LOG"
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
    echo "  - Vite:          localhost:$VITE_PORT (frontend dev server)"
    echo "  - Client:        Tauri GUI window"
    echo ""
    echo "Log files:"
    echo "  - StationService: $STATIONSERVICE_LOG"
    echo "  - Aggregator:     $AGGREGATOR_LOG"
    echo "  - Vite:           $VITE_LOG"
    echo "  - Client:         $CLIENT_LOG"
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
