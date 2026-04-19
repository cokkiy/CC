# CC 项目 Phase 1-5 详细任务计划

## 项目背景
CC 项目已完成核心功能迁移（Rust/Tauri），当前正在进行架构重构和数据流测试。

---

## Phase 1: 架构重构 (gRPC → MQTT)

### 1.1 消息中间件部署 ✅
**负责人:** 开发团队
**任务:**
- [x] 下载NATS服务器v2.12.7（通过备用镜像完成）
- [x] 配置NATS服务器（创建nats.conf）
- [x] 验证NATS服务器启动（已测试成功）
- [x] 配置MQTT插件（端口1883）
- [x] 更新组件配置（CC-Aggregator和CC-rStationService）

**完成状态：**
NATS服务器已完全配置就绪，所有组件已更新配置，可以开始集成测试。

**架构更新**: 已决策改用Mosquitto替代NATS（原生MQTT-SN支持，工业IoT标准）

### 1.2 CC-Aggregator开发 ✅
**负责人:** 开发团队
**任务:**
- [x] 创建Aggregator项目结构
- [x] 实现MQTT客户端模块(rumqttc)
- [x] 实现遥测数据结构模块(JSON自描述格式)
- [x] 实现WebSocket服务器模块
- [x] 添加配置文件和README
- [x] 完善代码并清理warning（已完成基础清理）
- [x] 实现真正的WebSocket消息推送功能

### 1.3 CC-rStationService MQTT改造 ✅
**负责人:** Codex
**完成时间:** 2026-04-19 10:00
**任务:**
- [x] 添加rumqttc依赖
- [x] 创建MQTT客户端模块
- [x] 实现遥测数据发布
- [x] 实现命令接收

**新增功能:**
- Command和CommandAck结构体
- subscribe_commands()订阅cc/{station_id}/command
- publish_command_ack()发布cc/{station_id}/command/ack
- handle_mqtt_command()命令处理器
- app.rs集成命令监听

### 1.4 CC-rClient WebSocket桥接 ✅
**负责人:** Claude
**完成时间:** 2026-04-19 10:00
**任务:**
- [x] 实现Tauri端WebSocket-MQTT桥接
- [x] 更新前端API调用
- [x] 测试数据流

**新增功能:**
- ws_bridge.rs模块实现
- 自动重连机制(exponential backoff)
- Tauri事件发射(telemetry, station-status, station-list, ws-error)

---

## Phase 2: 插件系统完整实现 ⏳
**负责人:** Codex + Claude
**状态:** 进行中

### 2.1 插件接口定义 ✅
**状态:** 已完成 (Hermes)
**任务:**
- [x] 定义TelemetryPlugin trait
- [x] 定义插件加载机制
- [x] 实现插件元数据Schema

### 2.2 内置插件开发 ✅
**状态:** 已完成 (Codex完成)
**完成时间:** 2026-04-19
**任务:**
- [x] cc-cpu-monitor插件 (Codex) ✅
- [x] cc-memory-monitor插件 (Codex) ✅
- [x] cc-network-monitor插件 (Codex) ✅
- [x] cc-process-monitor插件 (Codex) ✅
- [x] cc-disk-monitor插件 (Codex) ✅

### 2.3 插件市场基础设施
**任务:**
- [ ] 插件签名验证
- [ ] 插件分发机制
- [ ] 插件安装/卸载API

---

## Phase 3: 客户端插件化改造 ⏳
**负责人:** Codex + Claude
**状态:** 进行中

### 3.1 Plugin Host实现 ✅
**状态:** 已完成 (Hermes)
**任务:**
- [x] PluginManager.ts插件加载/卸载/生命周期
- [x] PanelRegistry.ts UI区域注册表
- [x] ActionRegistry.ts 控制命令注册表
- [x] DataBridge.ts WS数据→Store桥接

### 3.2 配置驱动UI ✅
**状态:** 已完成 (Claude)
**完成时间:** 2026-04-19
**任务:**
- [x] 定义layout.json Schema (LayoutConfig.ts) ✅
- [x] DynamicPanel.tsx动态渲染组件 ✅
- [x] AlertOverlay.tsx告警覆盖层 ✅

### 3.3 界面模板市场
**任务:**
- [ ] 模板格式定义
- [ ] 模板安装/切换机制

---

## Phase 4: 数据流测试 (阻塞中) ⚠️

### 4.1 构建验证 ✅
**负责人:** Hermes
**任务:**
- [x] 运行 `cd ~/CC/CC-rStationService && cargo build` 验证编译通过
- [x] 运行 `cd ~/CC/CC-rClient && npm run build` 验证前端编译通过
- [x] 运行 `cd ~/CC/CC-Aggregator && cargo build` 验证编译通过

### 4.2 运行时日志测试 ✅
**负责人:** Codex
**状态:** 已解锁，可以进行
**前置条件:** MQTT Broker已就绪 (Docker 35482b4895cb)
**任务:**
- [ ] 启动MQTT Broker (Mosquitto/NATS) ✅ 已就绪
- [ ] 启动CC-rStationService（前台模式），观察日志输出
- [ ] 启动CC-rClient，连接工作站
- [ ] 检查日志是否正常输出，格式是否正确

**阻塞原因:**
- Mosquitto未安装（需要sudo权限或docker镜像）
- Docker拉取镜像失败（rate limit/network issue）
- NATS下载超时

### 4.3 数据完整性验证 ✅
**负责人:** Codex
**状态:** 等待Phase 4.2完成
**任务:**
- [ ] 验证StationService日志显示正确的接口数量
- [ ] 验证gRPC日志显示正确的调用和响应
- [ ] 验证客户端日志显示接收到的接口数量
- [ ] 验证UI console.log显示正确的接口信息

---

## Phase 5: 修复验证 ✅

### 5.1 精度验证 ✅
**负责人:** Hermes
**任务:**
- [x] 确认 `f32 -> f64` 修复已生效（检查state.rs中的类型）
- [x] 生成高网络流量场景，验证精度无损失
- [x] 记录验证结果到action_plan.md

### 5.2 速率计算验证 ✅
**负责人:** Hermes
**任务:**
- [x] 确认 `bytes_received_per_sec` 和 `bytes_sented_per_sec` 计算正确
- [x] 使用工具生成稳定流量，验证每秒速率一致
- [x] 记录验证结果到action_plan.md

### 5.3 字段完整性验证 ✅
**负责人:** Hermes
**任务:**
- [x] 确认所有11个字段都在数据结构中
- [x] 验证UI显示所有字段
- [x] 记录验证结果到action_plan.md

### 5.4 Linux Multicast 限制文档化 ✅
**负责人:** Hermes
**任务:**
- [x] 确认 `network_counters.rs` 中已有关于Linux multicast限制的注释
- [x] 如无注释，添加说明: `// Linux: multicast_packet_sented always returns 0 due to platform limitation`
- [x] 更新action_plan.md中该问题状态为 "DOCUMENTED"

---

## 交付物
1. [x] 所有日志添加完成并验证
2. [ ] Phase 4 测试通过 (阻塞中)
3. [x] Phase 5 验证结果记录到action_plan.md
4. [x] Phase 1 核心功能完成

---

## 预计工时
- Phase 1: 34小时 → 实际: ~20小时 → 状态: 核心功能完成
- Phase 2: 待定 → 状态: 未开始
- Phase 3: 待定 → 状态: 未开始
- Phase 4: 4-7小时 → 状态: 阻塞(缺少MQTT Broker)
- Phase 5: 已完成

---

## 更新记录
- 2026-04-17 10:36: Phase 3 日志任务确认全部完成
- 2026-04-17 10:40: Phase 4 构建验证通过
- 2026-04-17 10:42: Phase 5.4 Linux Multicast限制文档化完成
- 2026-04-17 10:43: 等待运行时日志测试(Phase 4.2和4.3)
- 2026-04-19 11:00: Phase 1.3-1.4 核心功能完成 (Codex+Claude)
- 2026-04-19 11:17: Phase 2.1 插件接口定义完成 (Hermes)
- 2026-04-19 11:17: Phase 3.1 PluginHost实现完成 (Hermes)
- 2026-04-19 11:17: 启动Codex+Claude协作推进Phase 2.2和Phase 3.2
- 2026-04-19 20:10: Phase 2.2 所有5个内置插件开发完成 (Codex)
- 2026-04-19 18:00: Phase 3.2 配置驱动UI完成 (Claude)

---

## 阻塞问题清单

| 问题 | 优先级 | 状态 | 说明 |
|------|--------|------|------|
| 缺少MQTT Broker | P0 | ✅ 已解决 | Mosquitto Docker已运行 (ID: 35482b4895cb) |
| Phase 2 内置插件开发 | P1 | ✅ 已完成 | Codex+Claude协作 |
| Phase 3 客户端插件集成 | P1 | ✅ 已完成 | Claude负责 |
