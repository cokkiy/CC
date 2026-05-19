#!/usr/bin/env bash
set -euo pipefail

# Build all CC artifacts:
# - CC-rController (frontend + tauri binary)
# - CC-Aggregator
# - CC-rDeviceAgent from a split checkout

RED='\033[0;31m'
GREEN='\033[0;32m'
BLUE='\033[0;34m'
NC='\033[0m'

log_info() {
  echo -e "${BLUE}[INFO]${NC} $1"
}

log_ok() {
  echo -e "${GREEN}[OK]${NC} $1"
}

log_err() {
  echo -e "${RED}[ERR]${NC} $1"
}

usage() {
  cat <<EOF
Usage: $(basename "$0") [--debug|--release]

Options:
  --debug     Build debug artifacts
  --release   Build release artifacts (default)
  -h, --help  Show this help

Environment:
  CC_RDEVICEAGENT_DIR  Path to CC-rDeviceAgent checkout
EOF
}

MODE="release"
for arg in "$@"; do
  case "$arg" in
    --debug)
      MODE="debug"
      ;;
    --release)
      MODE="release"
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

REPO_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
CLIENT_DIR="$REPO_DIR/CC-rController"
CLIENT_TAURI_DIR="$CLIENT_DIR/src-tauri"
AGGREGATOR_DIR="$REPO_DIR/CC-Aggregator"

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

  if [[ ! -f "$candidate/Cargo.toml" ]]; then
    log_err "CC-rDeviceAgent checkout not found: $candidate"
    log_err "Clone it next to this repository or set CC_RDEVICEAGENT_DIR."
    exit 1
  fi

  cd "$candidate" && pwd
}

STATION_DIR="$(resolve_station_dir)"

if ! command -v cargo >/dev/null 2>&1; then
  log_err "cargo not found in PATH"
  exit 1
fi

if ! command -v npm >/dev/null 2>&1; then
  log_err "npm not found in PATH"
  exit 1
fi

build_rust_project() {
  local name="$1"
  local dir="$2"

  log_info "Building ${name} (${MODE})..."
  pushd "$dir" >/dev/null
  if [[ "$MODE" == "release" ]]; then
    cargo build --release
  else
    cargo build
  fi
  popd >/dev/null
  log_ok "${name} build complete"
}

build_client() {
  log_info "Building CC-rController Tauri app (${MODE})..."

  pushd "$CLIENT_DIR" >/dev/null
  if [[ ! -d node_modules ]]; then
    log_info "Installing CC-rController npm dependencies..."
    npm install
  fi
  if [[ "$MODE" == "release" ]]; then
    npm run tauri:build
  else
    log_info "Debug mode skips Linux bundle packaging; building web assets and debug binary only..."
    npm run build
    cargo build --manifest-path src-tauri/Cargo.toml
  fi
  popd >/dev/null

  log_ok "CC-rController build complete"
}

main() {
  log_info "Repository: $REPO_DIR"
  log_info "Build mode: $MODE"

  build_client
  build_rust_project "CC-Aggregator" "$AGGREGATOR_DIR"
  build_rust_project "CC-rDeviceAgent" "$STATION_DIR"

  log_ok "All builds completed successfully"

  if [[ "$MODE" == "release" ]]; then
    echo
    echo "Artifacts:"
    echo "- $CLIENT_TAURI_DIR/target/release/cc-rcontroller"
    echo "- $AGGREGATOR_DIR/target/release/cc-aggregator"
    echo "- $STATION_DIR/target/release/cc-rdeviceagent"
  else
    echo
    echo "Artifacts:"
    echo "- $CLIENT_TAURI_DIR/target/debug/cc-rcontroller"
    echo "- $AGGREGATOR_DIR/target/debug/cc-aggregator"
    echo "- $STATION_DIR/target/debug/cc-rdeviceagent"
  fi
}

main
