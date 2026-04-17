# CC-Aggregator

CC项目消息聚合服务 - 基于MQTT/NATS的消息代理

## 概述

CC-Aggregator是CC项目的核心消息聚合服务，负责：
- 连接MQTT消息代理(NATS)
- 收集来自工作站和IoT设备的遥测数据
- 通过WebSocket将数据转发给客户端

## 技术架构

- **消息代理**: NATS (通过MQTT/rumqttc)
- **异步运行时**: Tokio
- **WebSocket**: tokio-tungstenite
- **数据格式**: JSON (自描述遥测格式)

## MQTT主题结构

```
cc/{station_id}/telemetry         # 遥测数据流
cc/{station_id}/status            # 在线/离线状态
cc/{station_id}/command            # 控制命令
cc/{station_id}/command/ack        # 命令响应
cc/{station_id}/descriptor/announce # 插件元信息
```

## 构建

```bash
cargo build --release
```

## 运行

```bash
./target/release/cc-aggregator
```

## 配置

编辑 `CC-Aggregator.toml`:

```toml
[mqtt]
host = "localhost"
port = 4222

[websocket]
listen_addr = "127.0.0.1:8080"
```

## 架构设计

详见 `docs/architecture-review.md`
