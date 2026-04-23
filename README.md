# CC - Workstation & IoT Centralized Management Platform

English | [中文](./README.zh.md)

## Project Overview

CC (Central Control) is a unified management platform for workstations and IoT devices. It provides real-time monitoring, remote control, plugin extensibility, and data visualization for distributed computing environments.

**Tech Stack:** Rust + Tauri + TypeScript + React + MQTT + WebSocket

## Architecture

```
┌─────────────────────────────────────────────────────────────────┐
│                        CC-rClient (Tauri)                       │
│  ┌─────────────┐  ┌─────────────┐  ┌─────────────────────────┐  │
│  │   React UI  │  │ Plugin Host │  │  WebSocket Bridge       │  │
│  └─────────────┘  └─────────────┘  └───────────┬─────────────┘  │
└──────────────────────────────────────────────┬──────────────────┘
                                               │ ws://localhost:8080
┌──────────────────────────────────────────────┴──────────────────┐
│                    CC-Aggregator (Rust)                         │
│  ┌─────────────────┐  ┌─────────────────────────────────────┐   │
│  │ MQTT Consumer   │  │ WebSocket Server                    │   │
│  │ (localhost:1883)│  │ (127.0.0.1:8080)                    │   │
│  └─────────────────┘  └─────────────────────────────────────┘   │
└──────────────────────────────────────────────┬───────────────────┘
                                             │ MQTT (localhost:1883)
┌──────────────────────────────────────────────┴───────────────────┐
│                CC-rStationService (Rust)                        │
│  ┌─────────────────┐  ┌─────────────────┐  ┌────────────────┐   │
│  │ Station Agent   │  │ Plugin Engine   │  │ Script Runner │   │
│  └─────────────────┘  └─────────────────┘  └────────────────┘   │
└─────────────────────────────────────────────────────────────────┘
                                              ▲
                                              │ Direct Execution
┌─────────────────────────────────────────────┴───────────────────┐
│                  Mosquitto MQTT Broker                          │
│                      (Docker)                                    │
└─────────────────────────────────────────────────────────────────┘
```

## Components

| Component | Description | Language |
|-----------|-------------|----------|
| CC-rClient | Desktop GUI client with plugin support | Tauri + React + TypeScript |
| CC-Aggregator | Data aggregation and WebSocket bridge | Rust |
| CC-rStationService | Workstation agent service | Rust |

## Features

### Completed Phases

- **Phase 1: Architecture Refactoring** - Migration from gRPC to MQTT message bus
- **Phase 2: Plugin System** - 5 built-in plugins (cpu, memory, network, process, disk) + Plugin Marketplace
- **Phase 3: Client Plugin Architecture** - Configuration-driven UI + Template Marketplace
- **Phase 4: Data Flow Testing** - MQTT + WebSocket integration
- **Phase 5: Fix Verification** - Bug fixes and validation
- **Phase 6: Command Script Management** - Script execution system

### Plugin System (5 Built-in Plugins)

1. **CPU Monitor** - Real-time CPU usage and metrics
2. **Memory Monitor** - RAM utilization tracking
3. **Network Monitor** - Network interface statistics
4. **Process Monitor** - Running process management
5. **Disk Monitor** - Storage usage and I/O metrics

### Marketplace Features

- **Plugin Marketplace** - Browse and install community plugins
- **Template Marketplace** - UI layout and visualization templates

## Project Structure

```
~/CC/
├── CC-rClient/           # Tauri desktop client (React + TypeScript)
│   ├── src/              # Frontend source
│   │   ├── plugin/       # Plugin system
│   │   └── App.tsx       # Main application
│   └── src-tauri/       # Rust backend
│       └── src/
│           ├── ws_bridge.rs    # WebSocket client
│           └── lib.rs           # Tauri commands
├── CC-Aggregator/       # Data aggregator (MQTT → WebSocket)
│   ├── src/
│   │   ├── mqtt.rs       # MQTT client
│   │   ├── websocket.rs  # WebSocket server
│   │   └── config.rs    # Configuration
│   └── CC-Aggregator.toml
├── CC-rStationService/  # Workstation agent
│   └── ...
├── scripts/              # Startup scripts
│   └── start-all.sh     # One-click start
└── logs/                # Application logs
```

## Quick Start

### Prerequisites

- Docker (for MQTT broker)
- Rust 1.70+
- Node.js 18+
- pnpm (recommended) or npm

### Step 1: Start MQTT Broker

```bash
# Start Mosquitto MQTT broker
docker run -d --name mosquitto \
  -p 1883:1883 \
  -p 9001:9001 \
  eclipse-mosquitto:latest \
  mosquitto -c /mosquitto-no-auth.conf
```

### Step 2: Start All Components

```bash
cd ~/CC
./scripts/start-all.sh
```

This script starts:
1. CC-Aggregator (MQTT → WebSocket bridge)
2. CC-rStationService (Workstation agent)
3. CC-rClient (Desktop UI)

### Step 3: Access the Application

- Desktop Client: Launch CC-rClient application
- WebSocket: `ws://127.0.0.1:8080`
- MQTT: `localhost:1883`

## Configuration

### CC-Aggregator (CC-Aggregator.toml)

```toml
server_id = "aggregator-001"

[mqtt]
host = "localhost"
port = 1883
client_id = "cc-aggregator-001"
keepalive_secs = 30

[websocket]
listen_addr = "127.0.0.1:8080"
max_connections = 1000

[logging]
level = "info"
```

### CC-rStationService (CC-rStationService.toml)

Configuration for workstation agent (MQTT client, plugins, etc.)

## Development

### Build Components

```bash
# Build Aggregator
cd CC-Aggregator && cargo build

# Build Station Service
cd CC-rStationService && cargo build

# Build Client
cd CC-rClient && pnpm install && pnpm tauri build
```

### Logs

Logs are stored in `~/CC/logs/`:
- `aggregator.log` - Aggregator output
- `rstationservice.log` - Station service output
- `rclient.log` - Client output

## Troubleshooting

### Connection Refused Errors

If you see "Could not connect to localhost: Connection refused":

1. **Check MQTT Broker**
   ```bash
   docker ps | grep mosquitto
   nc -zv localhost 1883
   ```

2. **Check Aggregator Running**
   ```bash
   ps aux | grep cc-aggregator
   tail -f logs/aggregator.log
   ```

3. **Check WebSocket Port**
   ```bash
   ss -tlnp | grep 8080
   ```

### Rebuild After Changes

```bash
cd CC-Aggregator && cargo build --release
./scripts/start-all.sh
```

## License

MIT License
