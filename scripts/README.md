# CC Project - 启动脚本

本目录包含用于启动和管理 CC 项目所有组件的脚本。

## 快速开始

### 一键启动所有组件

```bash
cd ~/CC/scripts
./start-all.sh
```

脚本将按以下顺序启动所有组件：

1. **Mosquitto MQTT Broker** - 消息代理（如果未运行）
2. **CC-rStationService** - 工作站服务（Rust/Tauri 后台服务）
3. **CC-Aggregator** - 数据聚合器（MQTT → WebSocket）
4. **CC-rClient** - 前端界面（Tauri 应用）

### 查看组件状态

```bash
./start-all.sh --status
```

### 停止所有组件

在运行脚本的终端按 `Ctrl+C` 即可停止所有组件。

## 组件说明

| 组件 | 端口 | 说明 |
|------|------|------|
| Mosquitto | 1883 (MQTT), 9001 (WebSocket) | MQTT 消息代理 |
| CC-rStationService | 50051 (gRPC) | 工作站服务，提供设备遥测和控制 |
| CC-Aggregator | 8080 (WebSocket) | 数据聚合器，将 MQTT 消息转发为 WebSocket |
| CC-rClient | GUI | Tauri 前端应用 |

## 日志文件

日志文件位于 `~/CC/logs/` 目录：

- `rstationservice.log` - CC-rStationService 日志
- `aggregator.log` - CC-Aggregator 日志
- `rclient.log` - CC-rClient 日志

## 前置要求

### 系统依赖

- **Docker** - 用于运行 Mosquitto（如果使用 Docker 方式启动）
- **Bash** - 运行脚本需要 Bash shell

### 构建要求

启动脚本需要以下组件已经构建完成：

```bash
# 构建 CC-rStationService
cd ~/CC/CC-rStationService
cargo build

# 构建 CC-Aggregator
cd ~/CC/CC-Aggregator
cargo build

# 构建 CC-rClient
cd ~/CC/CC-rClient/src-tauri
cargo build
```

## 故障排除

### Mosquitto 启动失败

如果 Mosquitto 启动失败，可以手动通过 Docker 启动：

```bash
docker run -d --name mosquitto \
    -p 1883:1883 -p 9001:9001 \
    eclipse-mosquitto:latest \
    /mosquitto-no-auth.conf
```

### 组件启动失败

1. 检查日志文件了解详细错误信息
2. 确保所有组件已经构建
3. 确保端口未被占用：

```bash
# 检查端口占用
ss -tlnp | grep -E '1883|50051|8080'
```

### 查看运行中的组件

```bash
# 查看 Mosquitto 容器状态
docker ps | grep mosquitto

# 查看端口占用
ss -tlnp
```

## 常见问题

**Q: 为什么需要按特定顺序启动组件？**

A: 组件之间有依赖关系：
- Aggregator 需要连接 MQTT Broker
- Client 需要连接 StationService 和 Aggregator

**Q: 如何重启某个组件？**

A: 先用 Ctrl+C 停止所有组件，然后重新运行 `./start-all.sh`。

**Q: 脚本能否在后台运行？**

A: 可以使用 `nohup ./start-all.sh &` 在后台运行，但建议先在前台测试确保一切正常。
