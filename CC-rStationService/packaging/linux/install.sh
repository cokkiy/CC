#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
INSTALL_ROOT="${INSTALL_ROOT:-/opt/cc-rstationservice}"
SERVICE_UNIT_DEST="/etc/systemd/system/cc-rstationservice.service"
AGENT_USER="${AGENT_USER:-${SUDO_USER:-}}"

SERVICE_BIN="$ROOT_DIR/target/release/cc-rstationservice"
AGENT_BIN="$ROOT_DIR/target/release/cc-rstationservice-agent"
CONFIG_SRC="$ROOT_DIR/CC-rStationService.toml"
SERVICE_UNIT_SRC="$ROOT_DIR/packaging/linux/systemd/cc-rstationservice.service"
AGENT_UNIT_SRC="$ROOT_DIR/packaging/linux/systemd/cc-rstationservice-agent.service"
AUTH_TOKEN="${AUTH_TOKEN:-}"

if [[ ! -x "$SERVICE_BIN" || ! -x "$AGENT_BIN" ]]; then
  echo "release binaries missing, run: cargo build --release" >&2
  exit 1
fi

if [[ $EUID -ne 0 ]]; then
  echo "run install.sh as root" >&2
  exit 1
fi

mkdir -p "$INSTALL_ROOT"
install -m 0755 "$SERVICE_BIN" "$INSTALL_ROOT/cc-rstationservice"
install -m 0755 "$AGENT_BIN" "$INSTALL_ROOT/cc-rstationservice-agent"

install -m 0644 "$CONFIG_SRC" "$INSTALL_ROOT/CC-rStationService.toml"

if [[ -z "$AUTH_TOKEN" ]]; then
  AUTH_TOKEN="$(head -c 32 /dev/urandom | od -An -tx1 | tr -d ' \n')"
fi

sed -i "s/^auth_token = .*/auth_token = \"$AUTH_TOKEN\"/" "$INSTALL_ROOT/CC-rStationService.toml"

install -m 0644 "$SERVICE_UNIT_SRC" "$SERVICE_UNIT_DEST"
systemctl daemon-reload
systemctl enable cc-rstationservice.service
systemctl restart cc-rstationservice.service

if [[ -n "$AGENT_USER" ]]; then
  USER_HOME="$(getent passwd "$AGENT_USER" | cut -d: -f6)"
  USER_LIB_DIR="$USER_HOME/.local/lib/cc-rstationservice"
  USER_UNIT_DIR="$USER_HOME/.config/systemd/user"

  install -d -m 0755 -o "$AGENT_USER" -g "$AGENT_USER" "$USER_LIB_DIR"
  install -d -m 0755 -o "$AGENT_USER" -g "$AGENT_USER" "$USER_UNIT_DIR"
  install -m 0755 -o "$AGENT_USER" -g "$AGENT_USER" "$AGENT_BIN" "$USER_LIB_DIR/cc-rstationservice-agent"
  install -m 0644 -o "$AGENT_USER" -g "$AGENT_USER" "$INSTALL_ROOT/CC-rStationService.toml" "$USER_LIB_DIR/CC-rStationService.toml"

  install -m 0644 -o "$AGENT_USER" -g "$AGENT_USER" "$AGENT_UNIT_SRC" "$USER_UNIT_DIR/cc-rstationservice-agent.service"

  if command -v loginctl >/dev/null 2>&1; then
    loginctl enable-linger "$AGENT_USER" || true
  fi

  sudo -u "$AGENT_USER" XDG_RUNTIME_DIR="/run/user/$(id -u "$AGENT_USER")" systemctl --user daemon-reload
  sudo -u "$AGENT_USER" XDG_RUNTIME_DIR="/run/user/$(id -u "$AGENT_USER")" systemctl --user enable cc-rstationservice-agent.service
  sudo -u "$AGENT_USER" XDG_RUNTIME_DIR="/run/user/$(id -u "$AGENT_USER")" systemctl --user restart cc-rstationservice-agent.service
else
  echo "AGENT_USER is not set; skipping desktop-agent installation" >&2
fi

echo "installed service to $INSTALL_ROOT"
