# CC项目 Phase 1: 架构重构详细计划

## 阶段目标
从gRPC迁移到MQTT/NATS，实现可扩展的IoT设备和工作站集中管理控制系统。

## 当前状态分析

### 已完成
- CC-rStationService: Rust实现的工作站服务(gRPC)
- CC-rClient: Tauri + React实现的客户端
- Phase 3-5: 调试日志和数据流测试任务

### 待重构问题
1. gRPC不适合1:N订阅场景
2. Protobuf不支持动态扩展
3. 客户端单体架构，无插件系统
4. 无消息中间件

---

## Phase 1.1: 消息中间件选型和部署

### 1.1.1 NATS服务器部署
**任务**:
- [ ] 下载NATS Server二进制
- [ ] 创建NATS配置文件 `nats-server.conf`
- [ ] 创建systemd服务单元(可选)
- [ ] 验证NATS服务器启动

**输出**: `CC-Aggregator/nats/` 目录和配置

### 1.1.2 MQTT vs NATS对比测试
**任务**:
- [ ] 评估NATS JetStream vs EMQX功能
- [ ] 性能测试: 10W+连接场景
- [ ] 编写选型报告

**决定**: 采用NATS (单一二进制，零依赖，Rust生态友好)

---

## Phase 1.2: MQTT客户端集成

### 1.2.1 rumqttc集成到CC-rStationService
**任务**:
- [ ] 在Cargo.toml添加rumqttc依赖
- [ ] 创建 `CC-rStationService/src/mqtt.rs` 模块
- [ ] 实现MQTT客户端初始化
- [ ] 实现主题订阅和发布

**文件**: `CC-rStationService/src/mqtt.rs`

### 1.2.2 MQTT主题结构设计
**主题规范**:
```
cc/{station_id}/telemetry         # 遥测数据流 (发布)
cc/{station_id}/status            # 在线/离线/告警 (发布)
cc/{station_id}/command/ack       # 控制命令响应 (发布)
cc/{station_id}/descriptor/announce # 插件元信息广播 (发布)
cc/{station_id}/command           # 控制命令 (订阅)
cc/{station_id}/config/set       # 配置更新 (订阅)
```

### 1.2.3 遥测Schema设计
**Descriptor格式**:
```json
{
  "station_id": "ws-001",
  "descriptors": [
    {
      "key": "cpu_usage_percent",
      "name": "CPU 使用率",
      "value_type": "float",
      "unit": "%",
      "range": { "min": 0, "max": 100 },
      "update_interval_ms": 1000
    }
  ]
}
```

**Telemetry格式**:
```json
{
  "ts": 1713234567890,
  "station_id": "ws-001",
  "interval_ms": 1000,
  "values": [
    { "key": "cpu_usage_percent", "v": 45.2 }
  ]
}
```

---

## Phase 1.3: CC-Aggregator实现

### 1.3.1 创建Aggregator项目
**任务**:
- [ ] 创建 `CC-Aggregator/` 目录
- [ ] 初始化Rust项目 `cargo init`
- [ ] 添加核心依赖: tokio, rumqttc, serde_json, tracing

### 1.3.2 Aggregator核心功能
**任务**:
- [ ] 实现MQTT客户端连接到NATS
- [ ] 实现工作站注册和心跳
- [ ] 实现遥测数据转发
- [ ] 实现WebSocket端点(用于客户端连接)

### 1.3.3 WebSocket桥接
**任务**:
- [ ] 实现WebSocket服务器(Tokio-Tungstenite)
- [ ] 实现WS→MQTT桥接
- [ ] 实现MQTT→WS桥接
- [ ] 实现多客户端会话管理

---

## Phase 1.4: 数据格式迁移

### 1.4.1 Protobuf → JSON/CBOR
**任务**:
- [ ] 迁移所有遥测数据结构到JSON
- [ ] 实现CBOR编码支持(用于IoT设备)
- [ ] 实现编码自动协商

### 1.4.2 插件元数据广播
**任务**:
- [ ] 实现descriptor/announce协议
- [ ] 实现动态遥测注册

---

## Phase 1.5: CC-rClient改造

### 1.5.1 Tauri端MQTT桥接
**任务**:
- [ ] 在Tauri Rust端添加rumqttc依赖
- [ ] 实现WebSocket-MQTT桥接
- [ ] 移除或保留gRPC作为备选

### 1.5.2 前端适配
**任务**:
- [ ] 更新API调用从gRPC到WebSocket
- [ ] 更新数据处理逻辑

---

## 交付物清单

### 代码
1. `CC-Aggregator/` - Aggregator服务完整实现
2. `CC-rStationService/src/mqtt.rs` - MQTT客户端模块
3. `CC-rStationService/src/mqtt_bridge.rs` - MQTT桥接模块
4. `CC-rClient/src-tauri/src/mqtt_ws_bridge.rs` - WebSocket-MQTT桥接

### 配置
1. `nats-server.conf` - NATS服务器配置
2. `CC-rStationService.toml` - 添加MQTT配置项
3. `CC-Aggregator.toml` - Aggregator配置

### 文档
1. MQTT主题规范文档
2. 遥测Schema规范文档
3. Aggregator部署指南

---

## 工时估算

| 任务 | 预计工时 |
|------|----------|
| Phase 1.1 NATS部署 | 2小时 |
| Phase 1.2 rumqttc集成 | 4小时 |
| Phase 1.3 Aggregator | 8小时 |
| Phase 1.4 数据格式迁移 | 6小时 |
| Phase 1.5 Client改造 | 6小时 |
| 测试和修复 | 8小时 |
| **总计** | **34小时** |

---

## 风险评估

### 高风险
1. **gRPC向后兼容**: 需要保留gRPC作为备选，确保平滑迁移
   - 缓解: 双协议支持，逐步切流

2. **性能问题**: NATS能否支撑10W+连接
   - 缓解: 早期压力测试，水平扩展设计

### 中风险
1. **CBOR兼容性**: 设备和服务器CBOR编解码一致性
   - 缓解: 完善的测试用例

---

## 里程碑

1. **M1** (第1周): NATS部署完成，rumqttc集成完成
2. **M2** (第2周): Aggregator原型完成，WebSocket桥接完成
3. **M3** (第3周): 数据格式迁移完成，Client改造完成
4. **M4** (第4周): 集成测试完成，文档完善

---

## 团队分工建议

| 角色 | 任务 |
|------|------|
| 架构师(Hermes) | 主题设计，Schema规范，技术选型 |
| 后端开发A | CC-Aggregator实现 |
| 后端开发B | CC-rStationService MQTT改造 |
| 前端开发 | CC-rClient WebSocket适配 |
| 测试 | 集成测试，性能测试 |

---

**创建日期**: 2026-04-17
**更新日期**: 2026-04-17
**状态**: 规划中
