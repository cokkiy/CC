# CC 项目架构审查报告（激进重构版）

**审查人：** Arch（系统架构师）
**日期：** 2026-04-16（更新：10:14）
**项目：** ~/CC（中控系统）
**版本状态：** 前期研发，可完全替换不合适的技术

---

## 核心立场：扔掉包袱，从零设计

当前架构的根问题是**用迁移思维做研发**——把 ZeroC ICE 的设计惯性带进了新架构，gRPC/Protobuf 是从老系统平移过来的，不是为当前需求选的最优解。研发阶段不解决的技术债，生产环境就是拦路虎。

**本报告原则：**

- 哪个技术不合适，哪个就换掉
- 不保留"过渡方案"
- 选型基于目标需求，不基于现状
- **Rust 生态优先**：所有新增基础设施必须选用 Rust 生态成熟库，不引入非 Rust 运行时依赖
- **部署最小化**：Aggregator = 单一 Rust 二进制，开发环境零额外进程
- **IoT 兼容**：工作站服务需支持资源受限设备（ARM Cortex-M、OpenWrt、LoRaWAN），支持断网续传、低功耗、窄带网络

---

## 一、现有架构的问题清单

### 问题 1：gRPC 根本不适合这个场景

| 维度 | gRPC 现实 | 500+ 站需求 |
|------|-----------|------------|
| 连接模型 | 1:1 长连接 | 1:N 订阅，N 远大于 1 |
| 消息模式 | 请求-响应 / 双向流 | 发布-订阅，多对多 |
| 协议格式 | Protobuf（静态） | 需要动态自描述 |
| 连接数 | 单 client 500 连接耗尽 | 代理模式天然解决 |
| 调试 | 调试困难 | 调试友好 |

**结论：gRPC 是遗留思维的产物，必须替换。**

### 问题 2：Protobuf 与动态扩展水火不容

Telemetry 数据目前硬编码在 `.proto` 里：

```protobuf
message StationRunningState {
  string station_id = 1;
  int64 current_memory = 2;
  float cpu = 3;
  // 每加一个遥测项 = 重编译 + 重部署全部
}
```

这是编译时绑定，不支持运行时注册新遥测项、插件动态添加数据、第三方设备接入。

**结论：Protobuf 用于 RPC 控制消息尚可，用于遥测数据必须用动态格式替换。**

### 问题 3：客户端是单体单文件，无任何扩展性

`App.tsx` 约 2000 行，涵盖工作站列表、运行时面板、截图、文件管理、远程命令、组管理、设置——全部写死，无模块边界，无插件机制，无界面模板系统。

### 问题 4：无消息中间件，客户端直接暴露给 500 站

工作站和客户端是 1:1 直连，客户端需要维护 500+ 连接，承受 500 路的推送洪流。这是架构性缺陷，不是参数调优能解决的。

---

## 二、重构后的目标架构

### 2.1 整体架构图

```
  ┌─────────────────────────────────────────────────────────────────────────────┐
  │                              CC-rClient (Tauri + React)                      │
  │                                                                              │
  │   Plugin Host ←→ Config-Driven UI ←→ Template Marketplace                    │
  │         ↕ WebSocket（Tauri Rust 端处理）                                     │
  └────────────────────────────────┬────────────────────────────────────────────┘
                                   │
                     ┌─────────────┴─────────────┐
                     │    CC-Aggregator         │
                     │   单一 Rust 二进制        │
                     │                           │
                     │  NATS 二进制（单文件）     │  ← 直接下载 NATS，零项目依赖
                     │  rumqttc (MQTT Client)    │
                     │  tokio-tungstenite (WS)   │
                     │  libloading (插件加载)    │
                     │  rusqlite / sqlx          │
                     └─────────────┬─────────────┘
                                   │
              ┌────────────────────┼────────────────────┐
              │ MQTT (rumqttc)     │                    │
    ┌─────────┴──────────┐    ┌─────┴──────────┐    ┌────┴─────────┐
    │  CC-rStationService │    │  CC-rStationService │    │  CC-rStationService │
    │  插件: cpu-monitor   │    │  插件: gpu-monitor  │    │  插件: disk-io      │
    │  插件: disk-info     │    │  插件: custom-sensor│    │  插件: iptables     │
    │  (rumqttc publish)   │    │  (rumqttc publish)   │    │  (rumqttc publish)   │
    │                       │    │                       │    │                       │
    │  ─── IoT 设备 ───    │    │                       │    │                       │
    │  CC-rStationService  │    │                       │    │                       │
    │  (embassy-rs/CBOR)   │    │                       │    │                       │
    │  MQTT-SN (UDP/DTLS)  │    │                       │    │                       │
    └─────────────────────┘    └──────────────────────┘    └──────────────────────┘
```

**部署核心原则：**
- `CC-Aggregator` = 单一 Rust 二进制，开发模式内嵌 NATS（零依赖）
- `CC-rStationService` = 现有 Rust 服务 + `rumqttc` 插件 + `libloading` 插件 Host
- `CC-rClient` = 现有 Tauri 服务，Tauri Rust 端处理 WebSocket/MQTT 桥接
- 所有组件均为 `cargo build --release` 输出，无额外运行时脚本

---

### 2.2 传输层：全场景覆盖（MQTT + MQTT-SN + WebSocket）

**标准工作站（Linux）→ Aggregator：** `rumqttc` MQTT 客户端（TCP/TLS）

```
主题设计：
  cc/{station_id}/telemetry         ← 遥测数据流（JSON）
  cc/{station_id}/status            ← 在线/离线/告警
  cc/{station_id}/command/ack       ← 控制命令响应
  cc/{station_id}/descriptor/announce← 插件元信息广播
```

**IoT 设备（资源受限）→ Aggregator：** `mqtt-sn` + CBOR 编码（UDP/DTLS）

支持场景：
- ARM Cortex-M（RAM < 64KB）→ NoStd + CBOR + MQTT-SN
- OpenWrt 路由器（RAM 128MB）→ 标准 Rust + CBOR + MQTT
- LoRaWAN / NB-IoT 网关 → DTLS + MQTT-SN（断网续传）
- 卫星链路（高延迟、低带宽）→ CBOR（体积减少 50-70%）+ QoS 1/2

**CBOR / JSON 自动协商：**

```rust
// 设备能力在 descriptor/announce 时声明
enum Encoding { Json, Cbor }  // 设备选择，Aggregator 自动编解码

fn encode(bundle: &TelemetryBundle, encoding: Encoding) -> Vec<u8> {
    match encoding {
        Encoding::Cbor => ciborium::to_vec(bundle).unwrap(),
        Encoding::Json => serde_json::to_vec(bundle).unwrap(),
    }
}
```

**客户端 → Aggregator：** WebSocket（Tauri Rust 端处理，无浏览器 MQTT.js 依赖）

```rust
// CC-rClient Tauri 端：WebSocket ↔ MQTT 桥接
// 浏览器前端通过 invoke() 订阅/取消订阅，工作站侧无感知

async fn start_mqtt_bridge(state: &AppState, ws_stream: WebSocketStream) {
    let mqtt_client = MqttClient::new(broker_url).await?;
    // 将 WS 订阅请求映射为 MQTT 订阅
    // 将收到的 MQTT 消息通过 WS 推送给前端
}
```

**为什么用 `rumqttc`：**
- Rust 生态最成熟异步 MQTT 客户端，活跃维护
- 纯 Rust，无 C 绑定，跨平台编译零额外依赖
- 支持 QoS 0/1/2，遗愿遗嘱（Last Will）自动检测工作站掉线

**为什么 Aggregator 到浏览器用 WebSocket 而非浏览器 MQTT.js：**
- 浏览器开 MQTT 连接需要 MQTT.js 库（~200KB），电池消耗高
- WebSocket 是浏览器原生支持，Tauri Rust 端处理 MQTT 更稳定
- 单一连接多路复用，比浏览器多 MQTT 连接更可靠

---

### 2.3 数据层：自描述遥测（完全抛弃 Protobuf 静态绑定）

#### 遥测 Schema 注册协议

工作站插件启动时向 Aggregator 广播自己提供哪些遥测项：

```json
// MQTT: cc/{station_id}/descriptor/announce
// 或 REST: POST /api/v1/stations/{station_id}/telemetry/descriptors

{
  "station_id": "ws-001",
  "descriptors": [
    {
      "key": "cpu_usage_percent",
      "name": "CPU 使用率",
      "name_en": "CPU Usage",
      "description": "最近采样周期的 CPU 占用百分比",
      "value_type": "float",
      "unit": "%",
      "range": { "min": 0, "max": 100 },
      "update_interval_ms": 1000,
      "aggregation": ["avg", "max", "min"],
      "alert": { "warning": 80, "critical": 95 }
    },
    {
      "key": "gpu_temperature_c",
      "name": "GPU 温度",
      "value_type": "float",
      "unit": "°C",
      "range": { "min": 0, "max": 120 },
      "alert": { "critical": 95, "warning": 80 }
    },
    {
      "key": "process_list",
      "name": "进程列表",
      "value_type": "json",
      "description": "当前运行进程快照，JSON 数组"
    }
  ]
}
```

#### 遥测数据格式

```json
// MQTT: cc/ws-001/telemetry
{
  "ts": 1713234567890,
  "station_id": "ws-001",
  "interval_ms": 1000,
  "values": [
    { "key": "cpu_usage_percent", "v": 45.2 },
    { "key": "memory_used_mb",   "v": 8192 },
    { "key": "gpu_temperature_c","v": 72.0 }
  ]
}
```

**客户端获取遥测 Schema 的流程：**
1. 订阅 `cc/{station_id}/descriptor/announce`（新站上线时广播）
2. 订阅 `cc/{station_id}/telemetry`（实时数据）
3. UI 组件通过 schema 动态渲染：
   - `value_type: float + unit: %` → 进度条 + 百分比
   - `value_type: json` → 可展开树
   - `alert.critical` 存在 → 叠加红色警告层

---

### 2.4 工作站服务：插件化是核心

#### 插件接口（Rust）

```rust
use libloading::{Library, Symbol};
use serde::{Deserialize, Serialize};

#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct TelemetryDescriptor {
    pub key: String,
    pub name: String,
    pub name_en: Option<String>,
    pub description: Option<String>,
    pub value_type: ValueType,
    pub unit: Option<String>,
    pub range_min: Option<f64>,
    pub range_max: Option<f64>,
    pub update_interval_ms: Option<u64>,
    pub alert_warning: Option<f64>,
    pub alert_critical: Option<f64>,
}

#[derive(Debug, Clone, Serialize, Deserialize)]
pub enum ValueType { Int, Float, String, Bool, Json }

#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct TelemetryValue {
    pub key: String,
    pub v: serde_json::Value,  // 兼容所有类型
    pub ts_ms: i64,
}

pub trait TelemetryPlugin: Send + Sync {
    fn id(&self) -> &str;
    fn version(&self) -> &str;
    fn descriptors(&self) -> Vec<TelemetryDescriptor>;
    fn sample(&self) -> Vec<TelemetryValue>;
    fn shutdown(&self) {}
}
```

#### 插件加载

```toml
# CC-rStationService.toml

[plugins]
dir = "./plugins"
auto_discover = true

[plugin.cpu]
path = "plugins/cc-cpu-monitor.so"

[plugin.disk]
path = "plugins/cc-disk-info.so"

[plugin.gpu]
path = "plugins/cc-gpu-nvidia.so"
config = { temperature_unit = "celsius", fanspeed_enabled = true }
```

#### 主进程职责

```rust
// 主进程 = 插件生命周期管理 + rumqttc MQTT 客户端
// 不再包含任何硬编码的遥测采集逻辑

use rumqttc::{MqttClient, Event, Packet};
use libloading::Library;

fn main() -> anyhow::Result<()> {
    let config = AppConfig::load("CC-rStationService.toml")?;
    let (mqtt_client, mut eventloop) = MqttClient::new(
        config.mqtt.broker_url,
        config.mqtt.client_id.clone(),
    )?;

    let plugin_host = PluginHost::load_from_config(&config.plugins)?;

    // 启动时：注册所有插件的 descriptors
    for plugin in &plugin_host.plugins {
        let descriptors = plugin.descriptors();
        let payload = serde_json::to_vec(&descriptors).unwrap();
        mqtt_client.publish(
            format!("cc/{}/descriptor/announce", config.station_id),
            QoS::AtLeastOnce,
            true,
            payload,
        )?;
    }

    // 主循环：采样 → 发布
    loop {
        // 采样并发布遥测
        let values: Vec<_> = plugin_host
            .plugins
            .iter()
            .flat_map(|p| p.sample())
            .collect();

        let telemetry = TelemetryBundle {
            ts: Utc::now().timestamp_millis(),
            station_id: config.station_id.clone(),
            values,
        };

        let payload = serde_json::to_vec(&telemetry).unwrap();
        mqtt_client.publish(
            format!("cc/{}/telemetry", config.station_id),
            QoS::AtLeastOnce,
            false,
            payload,
        )?;

        std::thread::sleep(Duration::from_secs(1));
    }
}
```

#### 内置插件（开发阶段）

| 插件 | 功能 | 依赖 |
|------|------|------|
| `cc-cpu-monitor` | CPU 使用率、每核使用率 | `sysinfo`（已有）|
| `cc-memory-monitor` | 内存使用量、使用率 | `sysinfo`（已有）|
| `cc-network-monitor` | 网卡流量、丢包率 | `netstat2`（已有）|
| `cc-process-monitor` | 进程列表、CPU/内存占用 | `sysinfo`（已有）|
| `cc-disk-monitor` | 磁盘使用量、IO 吞吐 | `sysinfo`（已有）|

---

### 2.5 聚合服务（Aggregator）—— 单一 Rust 二进制

**设计原则：Aggregator 是一整个 Rust 二进制，内嵌 NATS JetStream（开发模式），无独立进程依赖。**

```
开发环境：  cargo run --bin cc-aggregator
生产环境：  ./cc-aggregator --nats-url nats://prod-cluster:4222
```

```
┌─────────────────────────────────────────────────────────────┐
│              CC-Aggregator (单 Rust 二进制)                   │
│                                                              │
│  rumqttc          ←→  NATS JetStream（embedded / 独立集群）  │
│  tokio-tungstenite←→  Tauri WebSocket 客户端接入              │
│  libloading       ←→  插件 .so 动态加载（扩展功能）           │
│  rusqlite         ←→  SQLite（开发）/ PostgreSQL（生产）      │
│  tracing          ←→  日志（已存在于现有代码）                │
│  tokio            ←→  全异步运行时                            │
└─────────────────────────────────────────────────────────────┘

功能模块：
  · MQTT Client       订阅 cc/+/telemetry / cc/+/status / cc/+/descriptor/announce
  · WebSocket Server  浏览器/Tauri 客户端接入
  · Subscription      按 topic 过滤 + 路由到正确的 WS 订阅者
  · Auth / ACL        token-based 站点访问控制
  · Alert Engine      读取 descriptor alert 规则，触发告警通知
  · History Store     SQLite（开发）/ TimescaleDB（生产）
  · REST API          控制命令下发、历史查询、告警确认、Schema 查询
  · Plugin Registry   查询各站点的插件元信息
  · MQTT-SN Gateway   IoT 窄带设备接入（UDP / DTLS / 卫星链路）
  · Edge Aggregator   可选：站端本地预聚合（减少云端带宽）
```

**部署验证：**

```bash
# 开发：单机零依赖
cargo build --release -p cc-aggregator
./target/release/cc-aggregator --mode embedded

# 生产：连接 NATS 集群
./cc-aggregator --mode nats --nats-url nats://nats-cluster:4222
```

---

### 2.6 客户端架构：配置驱动 + 插件化

**原则：客户端 = 插件 Host + 配置文件，UI 代码量应随功能增长而非随站点数量增长。**

```
CC-rClient/
├── src/
│   ├── host/               # 插件 Host（固定不变）
│   │   ├── PluginManager.ts    # 插件加载/卸载/生命周期
│   │   ├── PanelRegistry.ts    # UI 区域注册表
│   │   ├── ActionRegistry.ts   # 控制命令注册表
│   │   └── DataBridge.ts       # WS 数据 → Store 桥接
│   │
│   ├── core/               # 核心 UI 组件（固定不变）
│   │   ├── StationList.tsx     # 工作站列表（配置驱动）
│   │   ├── DynamicPanel.tsx     # 根据 schema 动态渲染
│   │   └── AlertOverlay.tsx     # 告警覆盖层
│   │
│   └── config/             # 界面配置（JSON，可从市场下载）
│       ├── layout.default.json
│       └── themes/
│
├── templates/              # 从市场下载的界面模板
│   └── enterprise-dashboard/
│       ├── manifest.json
│       ├── layout.json
│       ├── components/
│       └── theme.css
│
└── plugins/                # 从市场下载的插件包
    └── third-party/
```

#### 界面配置文件示例

```json
// layout.json
{
  "version": "1.0",
  "name": "GPU 监控面板",
  "author": "第三方开发者",
  "panels": [
    {
      "id": "gpu-temp",
      "type": "gauge",
      "data_binding": {
        "station_id": "{selected_station}",
        "key": "gpu_temperature_c"
      },
      "position": { "x": 0, "y": 0, "w": 4, "h": 2 },
      "thresholds": { "warning": 80, "critical": 95 }
    },
    {
      "id": "gpu-util",
      "type": "line_chart",
      "data_binding": {
        "station_id": "{selected_station}",
        "key": "gpu_utilization_percent",
        "history_points": 60
      },
      "position": { "x": 4, "y": 0, "w": 8, "h": 2 }
    }
  ]
}
```

#### 插件接口

```typescript
interface CCExtension {
  id: string;
  name: string;
  version: string;
  onLoad(ctx: ExtensionContext): void;
  onUnload(): void;
  registerPanels(registry: PanelRegistry): void;
  registerActions(registry: ActionRegistry): void;
  onTelemetry(stationId: string, bundle: TelemetryBundle): void;
}
```

---

## 三、技术选型总表

| 层级 | 当前技术 | 重构后技术 | Rust 生态库 | IoT 说明 |
|------|----------|------------|-------------|----------|
| **遥测传输** | gRPC 双向流 | MQTT + MQTT-SN + WebSocket | `rumqttc` + `mqtt-sn`
| **遥测传输** | gRPC 双向流 | MQTT + MQTT-SN + WebSocket | `rumqttc` + `mqtt-sn`
`mqtt-sn` 转软\uff0c有候选\uff1bIoT 可选 Eclipse MQTT-SN C SDK | 标准站用 MQTT\uff0cIoT 设备用 MQTT-SN\uff08UDP/DTLS\uff09|
| **遥测数据格式** | Protobuf（静态） | JSON + CBOR 双轏 | `serde` + `serde_json` + `ciborium` | 宽带用 JSON，窄带/IoT 用 CBOR（体积 -50%）|
| **工作站服务** | Rust 单体 | Rust + 插件 Host（标准）/ `embassy-rs`（IoT）| `libloading` / `embassy-rs` | IoT 版：无 heap 分配、< 64KB RAM |
| **消息中间件** | 无 | NATS 集群 / NATS JetStream | `nats` Rust 客户端 | embedded-nats 尚未生产局度，1-100 台直接下载 NATS 二进制（1行命令），无项目依赖 |
| **客户端协议** | gRPC | WebSocket（Tauri Rust 端处理） | `tokio-tungstenite` | 浏览器无需任何 MQTT 库 |
| **客户端 UI** | React 单体 | React + Config + 插件 | — | — |
| **控制命令通道** | gRPC StationControl | REST → MQTT（低频命令不走流式） | `axum` | — |
| **Aggregator 运行时** | — | 单一 Rust 二进制 | 零额外依赖 | — |
| **数据存储** | 无 | SQLite（开发）/ TimescaleDB（生产） | `rusqlite` / `sqlx` | — |
| **日志** | — | `tracing`（已在现有代码中）| `tracing`（已有）| — |
| **异步运行时** | — | `tokio`（标准）/ `embassy-executor`（IoT）| `tokio`（已有）| IoT 设备用 `embassy-rs` 替代 `tokio` |

---

## 九、组件成熟度说明

选型原则：**Rust 生态优先，但** 若该组件未到生产局度，接受非 Rust 生态或顶级交付代替方案。

| 组件 | 当前状态 | 备选方案 | 实际开发建议 |
|-------------|--------|-------------|---------|
| `embedded-nats` | 尚未到生产局度，示例少，无组件测试 | NATS 二进制（单文件直接下载，1-100 台无项目依赖） | ✨ NATS 二进制即可，不用嵌入库 |
| `ciborium` (CBOR) | 年轮，测试骑，Rust CBOR 库标准 | MessagePack (`rmp`) 或直接 JSON 单台测试 | ✨ 先用 JSON，IoT 需求明确后再添 CBOR |
| `mqtt-sn` | 年轮较低，`mqtt-sn` 包名有少量代码没有市场驼证 | Eclipse Mosquitto MQTT-SN C SDK，或 Java SDK | ✨ 先用标准 MQTT，MQTT-SN 第二阶段考虑 |
| `embassy-rs` (IoT) | 生产局度但 NoStd + 资源级联集成本高 | 标准 Rust + `smoltcp`，或 C SDK + FFI | ✨ 先用标准 Rust，IoT 特殊需求明确后再实施 |
| `rusqlite` | 年轮，生产局度，无外部依赖 | `sqlx` + PostgreSQL（生产环境） | ✨ 已安全，直接使用 |
| `libloading` | 年轮，生产局度 | `kernel32`/`dlfcn` 原生 API | ✨ 直接使用，已在代码中使用 |
| `rumqttc` | 年轮，测试骑，Synne Robotics 保兽 | `paho-mqtt` (C)，或 NATS 本身 | ✨ 已安全，直接使用 |
| `tokio-tungstenite` | 年轮，生产局度，已在代码中使用 | `tokio-websocket` (同一代码库) | ✨ 已安全，直接使用 |
| `axum` | 年轮，生产局度，Tower 生态 | `actix-web` | ✨ 已安全，直接使用 |

※ ✨ 表示已经高成熟，直接采用。

**核心结论：**
- 目前唯一需要接受非 Rust 生态的是 **NATS 运行时**（NATS 本身是 Go 写的，但接受较多连接、实现生产级的 MQTT broker），它是一个 **1 行命令下载的单文件**，没有部署复杂度。
- `embedded-nats` 未到生产局度，**不引入**，改用 NATS 二进制。
- IoT 备选路径仅作为将来考野，第一阶段实施不包含它们。

---

## 四、实施计划

### 第一阶段（4-6 周）：核心骨架

**目标：MQTT + 聚合服务 + 自描述遥测，替换 gRPC**

1. **CC-Aggregator 开发**（纯 Rust，零额外依赖）
   - `rumqttc` MQTT 客户端：订阅全部 `cc/+/telemetry`
   - `tokio-tungstenite` WebSocket 服务端
   - `embedded-nats` 尚未生产局度，改为直接下载 NATS 二进制（单文件，无项目依赖）
   - `rusqlite` 本地存储（开发），`sqlx`+PostgreSQL（生产）
   - 构建：`cargo build --release -p cc-aggregator`

2. **CC-rStationService 改造**
   - 移除 `tonic` gRPC Server（`StationControl` 降级为 `axum` REST）
   - 接入 `rumqttc` MQTT Publisher
   - `libloading` 插件 Host 实现
   - 内置 5 个核心插件（CPU、内存、网络、进程、磁盘）

3. **CC-rClient 改造**
   - Tauri Rust 端 WebSocket 客户端（连接 Aggregator）
   - 前端 DynamicPanel + 配置化 UI

**部署验证：**
```bash
# 开发：单机零依赖
cargo build --release -p cc-aggregator
./target/release/cc-aggregator --mode embedded

# 生产：
./cc-aggregator --mode nats --nats-url nats://prod-nats:4222
```

### 第二阶段（3-4 周）：插件生态

1. 工作站端插件系统完整实现
2. 客户端 Plugin Manager + Panel Registry
3. Descriptor 注册/查询 API 完善
4. Alert Engine（基于 descriptor 告警规则）

### 第三阶段（4-6 周）：客户端开放

1. 界面配置 Schema 定义（JSON 布局格式）
2. 内置 UI 组件完全配置化改造
3. 主题/模板系统
4. 插件市场基础设施（签名、分发、安装）

---

## 五、技术债清理清单

| 债项 | 影响 | 处理方式 |
|------|------|----------|
| ZeroC ICE 迁移残留 | 维护两套协议 | 本次彻底移除 |
| Protobuf Schema 膨胀 | 每次加字段重编译 | JSON Schema Registry 替代 |
| App.tsx 单体 | 无法团队协作开发 | 模块化 + 插件化 |
| 1:1 连接模型 | 无法 scale | MQTT 代理模式替代 |
| 硬编码遥测字段 | 新需求 = 重部署 | 插件动态注册替代 |
| gRPC 控制命令 | 调试困难，生态有限 | `axum` REST 替代（低频命令）|

---

## 六、风险评估

| 风险 | 影响 | 缓解 |
|------|------|------|
| NATS 二进制扩展上限 | 低 | 直接下载 NATS 集群，改配置不改代码 |
| MQTT Broker 单点 | 高 | NATS 集群模式（3节点），客户端本地缓存 + 重连退避 |
| JSON 性能不如 Protobuf | 低 | Protobuf 保留用于 StationControl（低频），遥测全量 JSON |
| 插件稳定性（进程内 `libloading`） | 高 | 插件 panic 由主进程捕获隔离；未来可升级为进程外 IPC |
| 第三方恶意插件 | 中 | 签名验证 + 权限模型（只允许读遥测数据，无系统权限）|
| IoT 设备资源不足 | 高 | `embassy-rs` NoStd 版本；CBOR 压缩；MQTT-SN 替代 MQTT |
| 部署复杂度 | 中 | 所有组件均为 Rust 二进制，`cargo build --release` 即可部署 |

---

## 七、结论

这次重构的核心逻辑只有一条：**MQTT 是正确答案，gRPC 是错误答案。**

- 500+ 工作站的遥测收集，pub/sub 是唯一经过工业验证的方案
- 动态自描述是插件化的前提，Protobuf 在这里是用错武器
- 客户端 UI 配置化是第三方生态的基础，单体文件做不了这件事

**Rust 生态保障：** 所有新增组件（`rumqttc` / `tokio-tungstenite` / `libloading` / `axum`）均为 Rust 生态常\u6210\u5e93\uff1bNATS \u8fd0行时\u4e3a Go 二进制\uff081行命令下载\uff09\uff0c\u63a5受\u4e3a外部\u4f9d赖，与现有 `tokio` / `tracing` / `serde` 完美兼容，依赖树干净。

**部署最小化保障：** Aggregator = 单一 Rust 二进制，开发模式零额外进程，生产模式仅需 NATS 集群，不强制引入任何非 Rust 基础设施。

**IoT 兼容保障：** 从 Cortex-M（64KB RAM）到标准 Linux 服务器，共用同一套 MQTT 主题和 Descriptor 协议，仅编码格式（CBOR/JSON）和传输层（MQTT-SN/MQTT）按设备能力自动协商，无需为 IoT 设备开发独立协议栈。

---


## 八、10W+ 规模扩展架构

### 8.1 核心设计原则

从 **100 台单机房无服务器**到 **10W+ 跨云跨网**，架构无需重写，只做水平扩展：

| 部署规模 | 组件变更 | 运维操作 |
|---------|---------|---------|
| 1-100 台 | NATS 二进制（单文件）|  + NATS 二进制，零配置 |
| 100-1000 台 | 外接 NATS 集群（3 节点）| 改  |
| 1000-10W 台 | NATS 超集群 + 区域 Aggregator | 改配置，新增  实例 |
| 跨云跨网 10W+ | 多空地联合 + MQTT 联合桥接 | 按区域部署 NATS，集群间 MQTT 桥接 |

**关键：所有规模使用同一套二进制 ，不同规模只表示实例数量和配置不同。**

### 8.2 100 台单机房（最小 footprint）



- Aggregator 进程内嵌 NATS JetStream，无需独立消息队列服务
- NATS 支持 10 万+ 主题订阅，100 台没压力
- 一行命令启动：

### 8.3 1000-10W 台：区域聚合 + NATS 超集群



- 每个地理区域或子网部署一个 （区域聚合）
- 区域 Aggregator 做本地数据过滤、告警降频、历史缓存
- 全局 Aggregator 做跨区域聚合查询、跨区域告警
- NATS 超集群（NATS JetStream Cluster）支持跨 AZ 复制，单集群可支弹 5W+ 连接

### 8.4 跨云跨网 10W+：MQTT 联合



**NATS 联邦桥接（NATS Mirror / JetStream Replica）：**
- 各云厂商的 NATS 集群通过  组成联邦
- 跨集群主题自动路由（ 跨集群传播）
- 工厂内网断线时， 启用 Store-and-Forward，断线恢复后自动补发积压数据

**多秄户屋隔离：**
- 主题前缀按秄户隔离：
- Aggregator 按秄户独立配置 ACL，不跨秄户访问

### 8.5 NATS 生态与规模能力

| NATS 部署模式 | 最大连接数 | 消息吃吞 | 适用规模 |
|-------------|-----------|---------|---------|
| NATS 二进制（单文件） | 1,000 | 10M msg/s | 1-100 台 |
| 单节点（小型） | 10,000 | 20M msg/s | 100-1,000 台 |
| 3 节点集群 | 50,000 | 40M msg/s | 1,000-10,000 台 |
| 超集群（5 节点 x3 AZ） | 200,000+ | 80M msg/s | 10,000-100,000 台 |
| 全球联合（多云） | 无固定上限 | 线性扩展 | 10W+ |

**来源：NATS官方基准测试，JetStream 持久化模式下吃吞效率会稍有下降，但仍远超 10W 设备需求。**

### 8.6 部署弹性：配置驱动扩缩容



**扩缩容操作都是修改启动参数，不重新编译，不改代码。**



*报告更新时间：2026-04-16 12:12 GMT+8（新增 架构优化：零预算约束下的最终选型）
*文件路径：~/CC/docs/architecture-review.md*

---

## 九、架构优化：零预算约束下的最终选型

> 基于竞品分析（P0/P1 功能需求）+ 预算约束（零商业License）+ 规模扩展路径的综合决策
> 更新原因：原方案 NATS 存在 MQTT-SN 协议不支持、工业 IoT 场景集成困难、资源占用偏高等问题，Mosquitto 在零预算 + 工业 IoT 场景下更具优势

### 9.1 预算约束下的 Broker 决策

| Broker | 费用 | MQTT-SN | 集群 | MQTT 5.0 | 评估 |
|--------|------|---------|------|----------|------|
| EMQX Enterprise | ¥0（开源版限4节点）| ❌ | ✅ | ✅ | 可用但有限制 |
| NATS | 免费 | ❌ MQTT-SN桥接 | ✅ | N/A（非MQTT协议）| 不适合IoT |
| **rumqttd** | **免费，开源** | ❌ | ❌ | ✅ | 1-100台嵌入，零依赖 |
| **Mosquitto** | **免费，开源，C** | ✅ 原生 | ❌（需手动桥接）| ❌（仅3.1.1） | 大量IoT设备首选 |
| VerneMQ | 免费开源 | ❌ | ✅ | ✅ | 备选 |

**结论：两层 MQTT Broker 覆盖全场景：**

```
100台以下：rumqttd（嵌入Aggregator进程）
100台以上：Mosquitto集群（原生MQTT-SN支持，零预算工业级）
```

---

### 9.2 架构优化后的最终方案

```
工作站 ──MQTT──▶ Mosquitto Broker集群 ◀── MQTT ── CC-Aggregator
 (MQTT-SN/UDP) (rumqttc)
 │
  少量IoT设备 ◀──MQTT-SN──▶ │
 NATS联邦(可选) ▼
 CC-rClient
 (WebSocket)
```

**核心变化：用 Mosquitto 替代 NATS 作为主要 MQTT Broker**

Mosquitto 替代 NATS 的理由：

| 维度 | NATS | Mosquitto | 胜出 |
|------|------|-----------|------|
| MQTT-SN 原生支持 | ❌ 需额外网关 | ✅ 原生支持 | Mosquitto |
| IoT 设备接入 | 需 MQTT-SN bridge | 直接接 | Mosquitto |
| 零预算 | NATS Server免费 | 完全免费 | 平局 |
| 资源占用 | 轻量 | **极轻量**（<10MB RAM）| Mosquitto |
| 工业验证 | ✅ 云厂商托管成熟 | ✅ **工业IoT标准** | Mosquitto |
| 集群 | ✅ JetStream | ⚠️ 需要 LB | NATS |
| 跨区域 | ✅ Federation | ⚠️ 需要脚本 | NATS |

---

### 9.3 零预算下的规模扩展路径

```
阶段1: 0-100台
 rumqttd 嵌入 Aggregator
 单二进制，零外部依赖
 命令: cargo build --release

阶段2: 100-10000台
 Mosquitto Broker集群（3节点 + HAProxy LB）
 零预算：Mosquitto + HAProxy（开源）
 成本：3台云主机的钱

阶段3: 10000+台（多区域）
 每区域部署 Mosquitto 集群
 区域间通过 Mosquitto Bridge（开源内置）
 主题前缀隔离多租户

阶段4: 10W+跨云
 按云厂商region部署Mosquitto联邦
 使用 Mosquitto 的 bridge 配置互联
 零预算：纯开源方案
```

---

### 9.4 与竞品功能对照的架构支撑

竞品分析中 P0/P1 功能在 MQTT 架构下的实现路径：

| 竞品功能 | CC实现方案 | MQTT架构支撑 |
|---------|----------|------------|
| Wake-on-LAN | **无法走MQTT**（magic packet需L2广播）| 设计退化：WoL由工作站同网段节点代发 |
| 批量电源操作 | `cc/batch/{action}` 主题广播 | QoS 1 广播，1:N一次完成 |
| 实时监控图表 | 工作站发布 `telemetry` 主题 | MQTT 天然支持，多客户端订阅同一主题 |
| 批量文件传输 | 独立文件传输协议（不压缩MQTT）| 建议走 HTTP/WebSocket（大文件）|
| 远程命令行 | **不走MQTT**：gRPC 控制通道执行 | 继续保留 `axum` REST/控制命令 |
| 设备分组 | Aggregator内存分组 + 主题过滤 | `cc/{group}/{station}/telemetry` |
| 屏幕快照 | 工作站推送（文件内容走专用主题）| MQTT Payload传图片二进制（限制<1MB）|

---

### 9.5 Wake-on-LAN 的架构解法

这是竞品分析中 P0 功能，**MQTT 无法传输 magic packet**（这是架构设计的经典陷阱），正确解法：

```
方案A（推荐）：Aggregator本身担任WoL网关
 CC-Aggregator部署在每个子网的管理VLAN
 WoL请求 → Aggregator在本机发UDP广播（端口9）
 前提：Aggregator有网卡在目标子网

方案B：指定一台工作站作为WoL relay
 工作站安装轻量WoL agent
 订阅 `cc/{station}/wol` 主题
 收到WoL命令 → 在本网络发magic packet
 优点：Aggregator无需在各子网部署
 缺点：目标站须在在线状态

方案C：硬件WoL（向日葵方案）
 指定一台物理WoL设备（开机棒/智能插座）
 REST API控制硬件开关 → 触发开机
 不依赖任何软件，最可靠
```

**架构建议**：优先实现方案A（Aggregator共网段部署）+ 方案C（重要场景硬件兜底）

---

### 9.6 关键技术决策更新

| 决策 | 原方案 | 更新方案 | 原因 |
|------|--------|---------|------|
| MQTT Broker | NATS 超集群 | **Mosquitto 集群** | 原生MQTT-SN，零预算，工业标准 |
| 轻量嵌入Broker | embedded-nats | **rumqttd** | 纯Rust，支持MQTT 5.0，可嵌入Aggregator |
| IoT协议 | MQTT-SN via NATS | **MQTT-SN直连Mosquitto** | Mosquitto原生支持，省去协议转换 |
| 远程命令通道 | 全量MQTT | **MQTT控制+HTTP文件** | 命令走REST，文件走HTTP，MQTT专注遥测 |
| WoL实现 | 未规划 | **Aggregator本机UDP广播** | magic packet无法走MQTT |

---

### 9.7 零预算运维清单

```
月度运维成本（按规模）：

0-100台：0元
 rumqttd嵌入，Aggregator进程本身
 一台2C/4GB云主机即可

100-1000台：约300-500元/月
 3节点Mosquitto集群
 3台 1C/1GB 云主机（按量计费）
 HAProxy 负载均衡（免费）

1000-10000台：约1000-2000元/月
 每区域Mosquitto集群（按需扩展）
 对象存储（遥测历史，图片快照）

10W+：按需扩展，Mosquitto联邦无需商业License
```

---

### 9.8 架构更新影响分析

**需要修改的组件：**

| 组件 | 影响 | 变更内容 |
|------|------|----------|
| CC-Aggregator | 高 | 内嵌 rumqttd 替代 embedded-nats；Mosquitto 连接替换 NATS 连接 |
| CC-rStationService | 低 | MQTT Client 库保持 rumqttc，仅 Broker 地址变更 |
| CC-rClient | 低 | WebSocket 桥接逻辑不变（Aggregator 地址变更）|
| 部署文档 | 中 | NATS 部署步骤替换为 Mosquitto 部署 |
| 监控告警 | 低 | NATS 监控替换为 Mosquitto 监控（mosquitto_sub / mqtt_exporter）|

**不需要修改的组件：**
- 遥测 Schema 注册协议（JSON + Descriptor 机制不变）
- 插件系统（libloading + TelemetryPlugin trait 不变）
- 客户端配置驱动 UI（与 Broker 类型无关）
- 客户端 WebSocket 桥接（Tauri 端逻辑不变）
