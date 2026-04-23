# CC项目功能扩充实施方案

## 一、现状评估

### 1.1 已完成的核心功能

| 模块 | 状态 | 说明 |
|------|------|------|
| 架构重构 (gRPC→MQTT) | ✅ 完成 | NATS/MQTT消息中间件已部署，CC-rStationService已改造 |
| 插件系统 | ✅ 完成 | TelemetryPlugin trait定义，加载机制，5个内置插件(CPU/内存/网络/进程/磁盘) |
| 插件市场基础设施 | ✅ 完成 | PluginSigner签名验证，PluginMarketplace分发安装API |
| 客户端插件化改造 | ✅ 完成 | PluginHost, PanelRegistry, ActionRegistry, DataBridge |
| 配置驱动UI | ✅ 完成 | layout.json Schema, DynamicPanel动态渲染, AlertOverlay |
| 界面模板市场 | ✅ 完成 | TemplateMarketplaceProvider, 模板安装/切换/导入/导出 |
| 数据流测试 | 🔄 进行中 | Phase 4.2运行时日志测试因MQTT Broker问题阻塞 |

### 1.2 项目目标差距分析

根据`README.md`定义的项目目标：

| 目标 | 完成度 | 说明 |
|------|--------|------|
| 实现工作站和IoT的遥测及远程控制 | 70% | 遥测已完成，远程控制基础能力待完善 |
| 支持100台~10W+设备规模 | 40% | 基础架构就绪，扩展性待验证 |
| 遥测端容易扩展，自说明特性 | 90% | 插件系统已完成 |
| **控制端命令脚本可持久化、模板化、导入、收藏、编辑** | **0%** | **未实现** |
| **支持对工作站、IoT设备的批量操作** | **0%** | **未实现** |

### 1.3 待扩充功能优先级矩阵

```
        高价值
           │
    P1     │  ★命令脚本管理
    (目标)  │  ★批量操作支持
           │
    P2     │  设备分组/标签体系  告警规则引擎
    (体验)  │  时序数据存储      权限与审计
           │
    P3     │  设备注册/发现     配置管理
    (基础)  │  文件管理/分发     升级管理
           │
    P4     │  多租户支持        作业编排
    (增强)  │  离线/弱网适配     高可用能力
           │
           └────────────────────────────────→ 高难度
```

---

## 二、功能扩充路线图

### Phase 6: 命令脚本管理系统 🔜 下一阶段

**目标**: 实现控制命令和脚本的持久化、模板化、导入导出、收藏和编辑功能

**工期**: 3-4周

#### 6.1 命令脚本数据模型

```
CommandScript {
    id: UUID
    name: String
    description: String
    script_type: enum[shell, powershell, python, lua]
    content: String
    parameters: Vec<ScriptParameter>
    is_template: bool
    is_favorite: bool
    tags: Vec<String>
    created_by: String
    created_at: DateTime
    updated_at: DateTime
    version: u32
    usage_count: u64
}

ScriptParameter {
    name: String
    param_type: enum[string, number, boolean, select]
    default_value: String
    required: bool
    validation: Option<String>  // regex pattern
}
```

#### 6.2 核心功能

| 功能 | 说明 | 优先级 |
|------|------|--------|
| 命令创建/编辑/删除 | 完整的CRUD操作，支持代码高亮 | P0 |
| 参数化模板 | 支持变量占位符 `{{param}}` | P0 |
| 命令收藏 | 一键收藏常用命令 | P1 |
| 命令分类/标签 | 按类型/场景/设备分组 | P1 |
| 导入/导出 | JSON格式，支持批量导入 | P1 |
| 使用统计 | 记录命令使用频率，智能推荐 | P2 |
| 版本管理 | 命令历史版本，回滚能力 | P2 |

#### 6.3 技术实现

**后端 (CC-rStationService)**:
- `scripts.rs`: 命令脚本数据结构定义
- `script_engine.rs`: 脚本解析和参数替换
- `script_executor.rs`: 脚本执行和结果收集
- 存储: SQLite本地持久化

**前端 (CC-rClient)**:
- `ScriptEditor.tsx`: 命令编辑界面
- `ScriptList.tsx`: 命令列表和搜索
- `ScriptRunner.tsx`: 命令执行界面
- `ScriptMarketplace.tsx`: 脚本市场(导入/导出)

**API设计**:
```
POST   /api/scripts              - 创建命令
GET    /api/scripts              - 获取命令列表
GET    /api/scripts/{id}         - 获取命令详情
PUT    /api/scripts/{id}         - 更新命令
DELETE /api/scripts/{id}         - 删除命令
POST   /api/scripts/{id}/execute - 执行命令
POST   /api/scripts/import       - 导入命令
GET    /api/scripts/export       - 导出命令
POST   /api/scripts/{id}/favorite - 收藏/取消收藏
```

---

### Phase 7: 批量操作支持系统

**目标**: 实现对多设备的批量操作能力

**工期**: 4-5周

#### 7.1 批量操作模型

```
BatchTask {
    id: UUID
    name: String
    description: String
    task_type: enum[command, script, config, file_transfer, upgrade, reboot]
    target_selector: TargetSelector  // 设备选择器
    content: String                   // 命令内容或配置
    parameters: HashMap<String, String>
    execution_policy: ExecutionPolicy
    created_by: String
    created_at: DateTime
    status: TaskStatus
}

TargetSelector {
    selector_type: enum[all, group, tag, device_ids, filter]
    groups: Option<Vec<String>>
    tags: Option<Vec<String>>
    device_ids: Option<Vec<String>>
    filter_expr: Option<String>  // 表达式如: "cpu > 80 && memory > 90"
}

ExecutionPolicy {
    mode: enum[parallel, batch, rolling]
    batch_size: Option<u32>      // 分批执行数量
    batch_interval_secs: u32     // 批次间隔
    continue_on_failure: bool
    failure_threshold_percent: f32  // 失败率阈值，超过则暂停
    timeout_secs: u32
    retry_count: u32
}

TaskExecution {
    id: UUID
    task_id: UUID
    device_id: String
    status: enum[pending, running, success, failed, timeout]
    started_at: DateTime
    completed_at: Option<DateTime>
    result: Option<ExecutionResult>
    logs: Vec<String>
}
```

#### 7.2 执行策略

| 策略 | 说明 | 适用场景 |
|------|------|----------|
| 全量并发 | 同时对所有目标执行 | 小规模(<20)，快速验证 |
| 分批执行 | 每批N台，执行完再下一批 | 中等规模，敏感操作 |
| 滚动执行 | 先1台 → 5台 → 全部 | 重要生产环境，金丝雀发布 |
| 失败熔断 | 失败率超过阈值自动暂停 | 高风险操作 |

#### 7.3 技术实现

**后端**:
- `batch_task.rs`: 批量任务模型和状态机
- `task_scheduler.rs`: 任务调度器
- `task_executor.rs`: 分布式执行器
- `device_selector.rs`: 设备选择器解析

**前端**:
- `BatchTaskWizard.tsx`: 批量任务创建向导
- `TaskList.tsx`: 任务列表和状态
- `TaskExecutionDetail.tsx`: 执行详情和日志
- `DeviceSelector.tsx`: 设备选择组件

**API设计**:
```
POST   /api/tasks                 - 创建批量任务
GET    /api/tasks                 - 获取任务列表
GET    /api/tasks/{id}            - 获取任务详情
POST   /api/tasks/{id}/execute     - 执行任务
POST   /api/tasks/{id}/pause       - 暂停任务
POST   /api/tasks/{id}/resume      - 恢复任务
POST   /api/tasks/{id}/cancel      - 取消任务
GET    /api/tasks/{id}/executions  - 获取执行详情列表
GET    /api/tasks/{id}/executions/{device_id} - 获取特定设备执行结果
GET    /api/devices/selector       - 设备选择器预览
```

---

### Phase 8: 设备分组与标签体系

**目标**: 完善设备管理基础能力，支持灵活的设备组织方式

**工期**: 2-3周

#### 8.1 设备模型扩展

```rust
// CC-rStationService/state.rs 扩展
struct DeviceInfo {
    // 已有字段...
    
    // 新增字段
    groups: Vec<String>,           // 所属分组
    tags: HashMap<String, String>, // 标签键值对
    metadata: HashMap<String, String>, // 设备元数据
    location: Option<Location>,    // 地理位置
}

struct DeviceGroup {
    id: String,
    name: String,
    parent_id: Option<String>,     // 支持层级
    description: String,
    device_count: u32,
}

struct TagDefinition {
    key: String,
    value_type: enum[string, number, boolean],
    allowed_values: Option<Vec<String>>, // 枚举限制
    description: String,
}
```

#### 8.2 核心功能

| 功能 | 说明 | 优先级 |
|------|------|--------|
| 设备分组管理 | 创建/编辑/删除分组，支持层级 | P0 |
| 标签管理 | 定义标签键值，支持枚举限制 | P1 |
| 静态分组 | 手动分配设备到分组 | P1 |
| 动态分组 | 基于条件表达式自动归类 | P2 |
| 分组批量操作 | 对整个分组执行操作 | P1 |

---

### Phase 9: 告警规则引擎

**目标**: 实现基于遥测数据的实时告警能力

**工期**: 3-4周

#### 9.1 告警模型

```
AlertRule {
    id: UUID
    name: String
    description: String
    condition: AlertCondition
    severity: enum[info, warning, error, critical]
    scope: Scope  // 应用范围
    actions: Vec<AlertAction>
    enabled: bool
    cooldown_secs: u32  // 告警抑制时间
}

AlertCondition {
    metric: String,           // 指标名，如 "cpu.usage"
    operator: enum[gt, lt, eq, ne, gte, lte]
    threshold: f64
    duration_secs: u32        // 持续多久才触发
    // 支持组合条件
    logic: enum[and, or]
    conditions: Vec<AlertCondition>
}

AlertAction {
    action_type: enum[email, webhook, script, notification]
    config: HashMap<String, String>
}

Alert {
    id: UUID
    rule_id: UUID
    device_id: String
    severity: enum[info, warning, error, critical]
    message: String
    metric_value: f64
    threshold: f64
    triggered_at: DateTime
    status: enum[active, acknowledged, resolved]
    acknowledged_by: Option<String>
    acknowledged_at: Option<DateTime>
}
```

#### 9.2 告警流程

```
1. 遥测数据流入
       ↓
2. 规则引擎评估所有启用规则
       ↓
3. 条件匹配?
   ├── 否 → 忽略
   └── 是 → 4. 检查cooldown是否过期
              ├── 未过期 → 忽略
              └── 已过期 → 5. 生成告警事件
                            ↓
                        6. 执行告警动作
                            ↓
                        7. 推送至UI/通知渠道
```

---

### Phase 10: 权限与审计系统

**目标**: 实现企业级安全能力

**工期**: 3-4周

#### 10.1 权限模型 (RBAC)

```
User {
    id: String
    username: String
    password_hash: String
    email: String
    roles: Vec<String>
    groups: Vec<String>  // 用户所属组织
}

Role {
    id: String
    name: String
    permissions: Vec<Permission>
}

Permission {
    resource: enum[device, task, script, config, user, system]
    action: enum[read, write, execute, admin]
    scope: enum[all, group, own]  // 权限范围
}
```

#### 10.2 高危操作保护

| 机制 | 说明 |
|------|------|
| 二次确认 | 重启、删除等操作需二次确认 |
| 审批流 | 高危操作需上级审批 |
| 执行窗口 | 仅在允许时间段执行 |
| 命令黑名单 | 禁止执行特定命令 |
| 审计日志 | 完整记录所有操作 |

---

## 三、技术架构建议

### 3.1 模块划分

```
CC项目 Rust crate 划分:

cc-core          # 通用模型、协议、错误定义
├── types.rs      # 共享数据类型
├── error.rs      # 统一错误处理
├── protocol.rs   # 通信协议定义
└── crypto.rs     # 加密签名工具

CC-rStationService # Agent端(已存在)
├── plugins/       # 插件系统
├── monitors/      # 内置监控
├── scripts.rs     # [新增] 命令脚本管理
├── batch.rs       # [新增] 批量任务执行
└── alerts.rs      # [新增] 告警规则

CC-Aggregator      # 数据聚合服务(已存在)
├── mqtt.rs        # MQTT客户端
├── ws_server.rs   # WebSocket服务
└── rules/         # [新增] 告警规则引擎

CC-rClient         # 客户端(已存在)
├── plugin/        # 插件系统
├── script/        # [新增] 脚本管理UI
├── batch/         # [新增] 批量操作UI
└── alert/         # [新增] 告警管理UI
```

### 3.2 存储策略

| 数据类型 | 存储方案 | 说明 |
|----------|----------|------|
| 设备信息 | SQLite | 结构化数据，本地持久化 |
| 遥测数据 | SQLite/InfluxDB | 时序数据，根据规模选择 |
| 命令脚本 | SQLite | JSON字段存储脚本内容 |
| 任务记录 | SQLite | 执行历史和日志 |
| 告警规则 | SQLite | 规则配置 |
| 告警事件 | SQLite | 时序数据 |
| 用户数据 | SQLite | 用户、角色、权限 |

### 3.3 通信协议

| 通道 | 协议 | 用途 |
|------|------|------|
| 遥测数据 | MQTT | 设备→服务端 |
| 命令下发 | MQTT | 服务端→设备 |
| 命令结果 | MQTT | 设备→服务端 |
| 实时推送 | WebSocket | 服务端→客户端 |
| 管理API | HTTP/REST | 客户端↔服务端 |

---

## 四、实施计划总览

| Phase | 内容 | 工期 | 优先级 | 依赖 |
|-------|------|------|--------|------|
| Phase 6 | 命令脚本管理系统 | 3-4周 | P0 | 无 |
| Phase 7 | 批量操作支持 | 4-5周 | P0 | Phase 6 |
| Phase 8 | 设备分组与标签 | 2-3周 | P1 | 无 |
| Phase 9 | 告警规则引擎 | 3-4周 | P1 | Phase 6, Phase 8 |
| Phase 10 | 权限与审计系统 | 3-4周 | P2 | Phase 6 |

**总工期估算**: 15-20周

---

## 五、立即行动项

### 5.1 短期计划 (1-2周)

1. **完成Phase 4数据流测试** - 解除当前阻塞
   - 解决MQTT Broker连接问题
   - 验证端到端数据流

2. **Phase 6启动准备**
   - 设计命令脚本数据模型
   - 确定脚本存储方案
   - 编写脚本解析器原型

### 5.2 中期计划 (1-2月)

1. 完成Phase 6命令脚本管理系统
2. 完成Phase 7批量操作支持
3. 完成Phase 8设备分组与标签

### 5.3 长期计划 (3-5月)

1. 完成Phase 9告警规则引擎
2. 完成Phase 10权限与审计系统
3. 性能优化和规模扩展验证

---

## 六、风险与缓解

| 风险 | 影响 | 缓解措施 |
|------|------|----------|
| 脚本执行安全风险 | 高 | 沙箱执行，命令黑名单，权限控制 |
| 批量操作不可逆 | 高 | 预执行预览，失败暂停，执行回滚 |
| 告警风暴 | 中 | cooldown机制，告警收敛，优先级 |
| 存储规模压力 | 中 | 数据分层策略，定期归档 |
| 插件稳定性 | 中 | 插件隔离(WASM/子进程)，超时控制 |

---

## 附录

### A. 相关文档

- [功能扩充建议](./function-extends.md) - 详细功能需求说明
- [开发计划](./dev-plan.md) - 架构重构和项目计划
- [架构审查报告](./architecture-review.md) - 技术架构分析

### B. 参考项目

- [Grafana Alerting](https://grafana.com/docs/grafana/latest/alerting/) - 告警规则引擎参考
- [Ansible Task Execution](https://docs.ansible.com/ansible/latest/user_guide/playbooks.html) - 批量执行策略参考
- [Portainer Template](https://documentation.portainer.io/) - 模板市场参考