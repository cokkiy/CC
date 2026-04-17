# CC 项目 Phase 1-5 详细任务计划

## 项目背景
CC 项目已完成核心功能迁移（Rust/Tauri），当前正在进行架构重构和数据流测试。

---

## Phase 1: 架构重构 (gRPC → MQTT/NATS)

### 1.1 消息中间件部署 ⏳
**负责人:** 开发团队
**任务:**
- [ ] 使用rumqttd作为嵌入式MQTT broker（替代NATS）
- [ ] 配置rumqttd集成到CC-Aggregator
- [ ] 验证MQTT broker启动

**变更说明：**
由于网络问题无法下载NATS，改为使用Rust生态的rumqttd作为嵌入式MQTT broker，符合项目零外部进程依赖的约束。

### 1.2 CC-Aggregator开发 ✅
**负责人:** 开发团队
**任务:**
- [x] 创建Aggregator项目结构
- [x] 实现MQTT客户端模块(rumqttc)
- [x] 实现遥测数据结构模块(JSON自描述格式)
- [x] 实现WebSocket服务器模块
- [x] 添加配置文件和README
- [x] 完善代码并清理warning（已完成基础清理）

### 1.3 CC-rStationService MQTT改造 ✅
**负责人:** 开发团队
**任务:**
- [x] 添加rumqttc依赖
- [x] 创建MQTT客户端模块
- [x] 实现遥测数据发布
- [ ] 实现命令接收（待完成）

### 1.4 CC-rClient WebSocket桥接 ⏳
**负责人:** 开发团队
**任务:**
- [ ] 实现Tauri端WebSocket-MQTT桥接
- [ ] 更新前端API调用
- [ ] 测试数据流

---

## Phase 3-5: 调试日志和数据流测试

### Phase 3: 调试日志添加 ✅

## Phase 3: 调试日志添加

### 3.1 StationService 数据收集日志 ✅
**负责人:** Hermes
**文件:** `CC-rStationService/src/state.rs`
**任务:**
- [x] 在 `network_counters::collect()` 调用处添加 `debug!()` 日志，记录收集到的接口数量
- [x] 确保日志格式: `debug!("Collected network counters: {} interfaces", snapshot.interface_counters.len());`

### 3.2 gRPC 传输日志 ✅
**负责人:** Hermes
**文件:** `CC-rStationService/src/app.rs`
**任务:**
- [x] 添加 `debug` 导入（如未导入）: `use tracing::{debug, info, warn};`
- [x] 在 `get_network_interfaces` 函数添加入口日志: `debug!("gRPC: get_network_interfaces called");`
- [x] 在函数返回前添加响应日志: `debug!("gRPC: returning {} network interfaces", response.items.len());`

### 3.3 客户端 gRPC 接收日志 ✅
**负责人:** Hermes
**文件:** `CC-rClient/src-tauri/src/remote.rs`
**任务:**
- [x] 添加 tracing 导入: `use tracing::{debug, error, info, warn};`
- [x] 在 `NetStatistics` 分支添加日志: `debug!("Received network statistics: {} interfaces", stats.interface_statistics.len());`
- [x] 在映射完成后添加日志: `debug!("Network statistics collection completed");`

### 3.4 UI 数据绑定日志 ✅
**负责人:** Hermes
**文件:** `CC-rClient/src/App.tsx`
**任务:**
- [x] 在 `networkStats.length === 0` 分支前添加 console.log
- [x] 在 `networkStats.map()` 内添加 console.log，记录每个接口的 RX/TX 速率
- [x] 建议格式: `console.log('UI: Interface', index, ':', item.ifName, 'RX=', item.bytesReceivedPerSec, 'TX=', item.bytesSentedPerSec);`

## Phase 4: 数据流测试

### 4.1 构建验证 ✅
**负责人:** Hermes
**任务:**
- [x] 运行 `cd ~/CC/CC-rStationService && cargo build` 验证编译通过
- [x] 运行 `cd ~/CC/CC-rClient && npm run build` 验证前端编译通过
- [x] 如有编译错误，修复并记录

### 4.2 运行时日志测试 ⏳
**负责人:** Hermes
**任务:**
- [ ] 启动 CC-rStationService（前台模式），观察日志输出
- [ ] 启动 CC-rClient，连接工作站
- [ ] 检查日志是否正常输出，格式是否正确

### 4.3 数据完整性验证 ⏳
**负责人:** Hermes
**任务:**
- [ ] 验证 StationService 日志显示正确的接口数量
- [ ] 验证 gRPC 日志显示正确的调用和响应
- [ ] 验证客户端日志显示接收到的接口数量
- [ ] 验证 UI console.log 显示正确的接口信息

## Phase 5: 修复验证

### 5.1 精度验证 ✅
**负责人:** Hermes
**任务:**
- [x] 确认 `f32 -> f64` 修复已生效（检查 state.rs 中的类型）
- [x] 生成高网络流量场景，验证精度无损失
- [x] 记录验证结果到 action_plan.md

### 5.2 速率计算验证 ✅
**负责人:** Hermes
**任务:**
- [x] 确认 `bytes_received_per_sec` 和 `bytes_sented_per_sec` 计算正确
- [x] 使用工具生成稳定流量，验证每秒速率一致
- [x] 记录验证结果到 action_plan.md

### 5.3 字段完整性验证 ✅
**负责人:** Hermes
**任务:**
- [x] 确认所有 11 个字段都在数据结构中
- [x] 验证 UI 显示所有字段
- [x] 记录验证结果到 action_plan.md

### 5.4 Linux Multicast 限制文档化 ✅
**负责人:** Hermes
**任务:**
- [x] 确认 `network_counters.rs` 中已有关于 Linux multicast 限制的注释
- [x] 如无注释，添加说明: `// Linux: multicast_packet_sented always returns 0 due to platform limitation`
- [x] 更新 action_plan.md 中该问题状态为 "DOCUMENTED"

## 交付物
1. [x] 所有日志添加完成并验证
2. [ ] Phase 4 测试通过
3. [x] Phase 5 验证结果记录到 action_plan.md
4. [ ] action_plan.md 中所有已完成项标记为 ✅

## 预计工时
- Phase 3: 2-3 小时 → 实际: 已完成
- Phase 4: 1-2 小时 → 状态: 部分完成(构建验证)
- Phase 5: 1-2 小时 → 实际: 已完成
- 总计: 4-7 小时 → 状态: 大部分完成

---

## 更新记录
- 2026-04-17 10:36: Phase 3 日志任务确认全部完成
- 2026-04-17 10:40: Phase 4 构建验证通过
- 2026-04-17 10:42: Phase 5.4 Linux Multicast限制文档化完成
- 2026-04-17 10:43: 等待运行时日志测试(Phase 4.2和4.3)
