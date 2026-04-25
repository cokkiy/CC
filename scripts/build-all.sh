#!/usr/bin/env bash
set -euo pipefail

# Build all CC artifacts:
# - CC-rClient (frontend + tauri binary)
# - CC-Aggregator
# - CC-rStationService

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
CLIENT_DIR="$REPO_DIR/CC-rClient"
CLIENT_TAURI_DIR="$CLIENT_DIR/src-tauri"
AGGREGATOR_DIR="$REPO_DIR/CC-Aggregator"
STATION_DIR="$REPO_DIR/CC-rStationService"

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
  log_info "Building CC-rClient frontend (${MODE})..."

  pushd "$CLIENT_DIR" >/dev/null
  if [[ ! -d node_modules ]]; then
    log_info "Installing CC-rClient npm dependencies..."
    npm install
  fi
  npm run build
  popd >/dev/null

  log_info "Building CC-rClient tauri binary (${MODE})..."
  pushd "$CLIENT_TAURI_DIR" >/dev/null
  if [[ "$MODE" == "release" ]]; then
    cargo build --release
  else
    cargo build
  fi
  popd >/dev/null

  log_ok "CC-rClient build complete"
}

main() {
  log_info "Repository: $REPO_DIR"
  log_info "Build mode: $MODE"

  build_client
  build_rust_project "CC-Aggregator" "$AGGREGATOR_DIR"
  build_rust_project "CC-rStationService" "$STATION_DIR"

  log_ok "All builds completed successfully"

  if [[ "$MODE" == "release" ]]; then
    echo
    echo "Artifacts:"
    echo "- $CLIENT_TAURI_DIR/target/release/cc-rclient"
    echo "- $AGGREGATOR_DIR/target/release/cc-aggregator"
    echo "- $STATION_DIR/target/release/cc-rstationservice"
  else
    echo
    echo "Artifacts:"
    echo "- $CLIENT_TAURI_DIR/target/debug/cc-rclient"
    echo "- $AGGREGATOR_DIR/target/debug/cc-aggregator"
    echo "- $STATION_DIR/target/debug/cc-rstationservice"
  fi
}

main
