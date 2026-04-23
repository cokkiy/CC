# Phase 7: 批量操作支持系统 - 详细架构设计

> 项目: CC (Cloud Controller)  
> 版本: 1.0  
> 日期: 2026-04-21  
> 状态: 架构设计文档  

---

## 1. 概述

### 1.1 设计目标

Phase 7旨在为CC项目实现完整的批量操作支持系统，支持对多个设备（工作站）进行批量命令执行、脚本运行、配置下发、文件传输、软件升级和重启等操作。

### 1.2 设计原则

- **类型安全**: 所有数据结构使用强类型定义，支持序列化/反序列化
- **可扩展**: 支持新增任务类型和执行策略
- **容错性**: 内置失败熔断、重试机制
- **可观测性**: 完整的任务状态跟踪和日志记录
- **前端友好**: 提供直观的批量操作UI

### 1.3 技术栈

- **后端**: Rust (CC-rStationService)
- **前端**: React + TypeScript (CC-rClient)
- **通信**: gRPC (现有架构)
- **状态存储**: SQLite (复用script_store模式)

---

## 2. BatchTask 数据模型详细Schema

### 2.1 核心数据结构

```rust
// batch_task.rs - 批量任务核心数据模型

use chrono::{DateTime, Utc};
use serde::{Deserialize, Serialize};
use uuid::Uuid;

/// 任务类型枚举
#[derive(Debug, Clone, Copy, PartialEq, Eq, Hash, Serialize, Deserialize)]
#[serde(rename_all = "snake_case")]
pub enum TaskType {
    /// 命令执行
    Command,
    /// 脚本执行
    Script,
    /// 配置下发
    Config,
    /// 文件传输
    FileTransfer,
    /// 软件升级
    Upgrade,
    /// 设备重启
    Reboot,
}

impl Default for TaskType {
    fn default() -> Self {
        Self::Command
    }
}

impl TaskType {
    pub fn from_str(s: &str) -> Option<Self> {
        match s.to_lowercase().as_str() {
            "command" => Some(TaskType::Command),
            "script" => Some(TaskType::Script),
            "config" => Some(TaskType::Config),
            "file_transfer" | "filetransfer" => Some(TaskType::FileTransfer),
            "upgrade" => Some(TaskType::Upgrade),
            "reboot" => Some(TaskType::Reboot),
            _ => None,
        }
    }

    pub fn as_str(&self) -> &'static str {
        match self {
            TaskType::Command => "command",
            TaskType::Script => "script",
            TaskType::Config => "config",
            TaskType::FileTransfer => "file_transfer",
            TaskType::Upgrade => "upgrade",
            TaskType::Reboot => "reboot",
        }
    }
}

/// 任务状态枚举
#[derive(Debug, Clone, Copy, PartialEq, Eq, Hash, Serialize, Deserialize)]
#[serde(rename_all = "snake_case")]
pub enum TaskStatus {
    /// 任务已创建，待执行
    Pending,
    /// 任务正在执行中
    Running,
    /// 任务已暂停（手动或熔断触发）
    Paused,
    /// 任务已完成
    Completed,
    /// 任务已失败
    Failed,
    /// 任务已取消
    Cancelled,
}

impl TaskStatus {
    pub fn as_str(&self) -> &'static str {
        match self {
            TaskStatus::Pending => "pending",
            TaskStatus::Running => "running",
            TaskStatus::Paused => "paused",
            TaskStatus::Completed => "completed",
            TaskStatus::Failed => "failed",
            TaskStatus::Cancelled => "cancelled",
        }
    }

    pub fn is_terminal(&self) -> bool {
        matches!(self, TaskStatus::Completed | TaskStatus::Failed | TaskStatus::Cancelled)
    }
}

/// 目标选择器 - 定义任务执行的目标设备
#[derive(Debug, Clone, Default, Serialize, Deserialize)]
pub struct TargetSelector {
    /// 选择器类型
    pub selector_type: SelectorType,
    /// 设备分组列表 (当selector_type为group时使用)
    #[serde(default)]
    pub groups: Vec<String>,
    /// 标签列表 (当selector_type为tag时使用)
    #[serde(default)]
    pub tags: Vec<String>,
    /// 设备ID列表 (当selector_type为device_ids时使用)
    #[serde(default)]
    pub device_ids: Vec<String>,
    /// 过滤表达式 (当selector_type为filter时使用)
    /// 支持类似: "status == 'online' && os == 'windows'"
    #[serde(default)]
    pub filter_expr: Option<String>,
}

impl TargetSelector {
    /// 创建选择所有设备的选择器
    pub fn all() -> Self {
        Self {
            selector_type: SelectorType::All,
            groups: Vec::new(),
            tags: Vec::new(),
            device_ids: Vec::new(),
            filter_expr: None,
        }
    }

    /// 按分组创建设备选择器
    pub fn by_groups(groups: Vec<String>) -> Self {
        Self {
            selector_type: SelectorType::Group,
            groups,
            tags: Vec::new(),
            device_ids: Vec::new(),
            filter_expr: None,
        }
    }

    /// 按标签创建设备选择器
    pub fn by_tags(tags: Vec<String>) -> Self {
        Self {
            selector_type: SelectorType::Tag,
            groups: Vec::new(),
            tags,
            device_ids: Vec::new(),
            filter_expr: None,
        }
    }

    /// 按设备ID列表创建设备选择器
    pub fn by_device_ids(device_ids: Vec<String>) -> Self {
        Self {
            selector_type: SelectorType::DeviceIds,
            groups: Vec::new(),
            tags: Vec::new(),
            device_ids,
            filter_expr: None,
        }
    }

    /// 按过滤表达式创建设备选择器
    pub fn by_filter(filter_expr: String) -> Self {
        Self {
            selector_type: SelectorType::Filter,
            groups: Vec::new(),
            tags: Vec::new(),
            device_ids: Vec::new(),
            filter_expr: Some(filter_expr),
        }
    }
}

/// 选择器类型枚举
#[derive(Debug, Clone, Copy, PartialEq, Eq, Hash, Serialize, Deserialize)]
#[serde(rename_all = "snake_case")]
pub enum SelectorType {
    /// 所有设备
    All,
    /// 按分组选择
    Group,
    /// 按标签选择
    Tag,
    /// 按设备ID列表选择
    DeviceIds,
    /// 按过滤表达式选择
    Filter,
}

impl Default for SelectorType {
    fn default() -> Self {
        Self::All
    }
}

/// 执行策略 - 定义任务如何执行
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct ExecutionPolicy {
    /// 执行模式
    pub mode: ExecutionMode,
    /// 分批大小 (当mode为batch时使用)
    #[serde(default)]
    pub batch_size: Option<u32>,
    /// 失败时是否继续
    #[serde(default = "default_true")]
    pub continue_on_failure: bool,
    /// 失败阈值百分比 (0-100)，超过此阈值自动暂停
    #[serde(default = "default_failure_threshold")]
    pub failure_threshold_percent: f32,
    /// 单个设备执行超时时间（秒）
    #[serde(default = "default_timeout")]
    pub timeout_secs: u32,
    /// 重试次数
    #[serde(default)]
    pub retry_count: u32,
    /// 重试间隔（秒）
    #[serde(default = "default_retry_interval")]
    pub retry_interval_secs: u32,
    /// 滚动执行初始数量 (当mode为rolling时使用)
    #[serde(default)]
    pub rolling_initial_batch: Option<u32>,
    /// 滚动执行增量 (当mode为rolling时使用)
    #[serde(default)]
    pub rolling_increment: Option<u32>,
}

fn default_true() -> bool { true }
fn default_failure_threshold() -> f32 { 50.0 }
fn default_timeout() -> u32 { 300 }
fn default_retry_interval() -> u32 { 5 }

impl Default for ExecutionPolicy {
    fn default() -> Self {
        Self {
            mode: ExecutionMode::Parallel,
            batch_size: None,
            continue_on_failure: true,
            failure_threshold_percent: 50.0,
            timeout_secs: 300,
            retry_count: 0,
            retry_interval_secs: 5,
            rolling_initial_batch: None,
            rolling_increment: None,
        }
    }
}

impl ExecutionPolicy {
    /// 创建全量并发执行策略
    pub fn parallel() -> Self {
        Self::default()
    }

    /// 创建分批执行策略
    pub fn batch(batch_size: u32) -> Self {
        Self {
            mode: ExecutionMode::Batch,
            batch_size: Some(batch_size),
            ..Default::default()
        }
    }

    /// 创建滚动执行策略
    pub fn rolling(initial: u32, increment: u32) -> Self {
        Self {
            mode: ExecutionMode::Rolling,
            rolling_initial_batch: Some(initial),
            rolling_increment: Some(increment),
            ..Default::default()
        }
    }

    /// 创建严格模式策略（失败即停）
    pub fn strict() -> Self {
        Self {
            continue_on_failure: false,
            failure_threshold_percent: 0.0,
            ..Default::default()
        }
    }
}

/// 执行模式枚举
#[derive(Debug, Clone, Copy, PartialEq, Eq, Hash, Serialize, Deserialize)]
#[serde(rename_all = "snake_case")]
pub enum ExecutionMode {
    /// 全量并发 - 同时对所有目标执行
    Parallel,
    /// 分批执行 - 每批N台，执行完再下一批
    Batch,
    /// 滚动执行 - 先1台 → 5台 → 全部
    Rolling,
}

impl Default for ExecutionMode {
    fn default() -> Self {
        Self::Parallel
    }
}

/// 批量任务 - 核心数据结构
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct BatchTask {
    /// 任务唯一标识
    pub id: Uuid,
    /// 任务名称
    pub name: String,
    /// 任务描述
    pub description: Option<String>,
    /// 任务类型
    #[serde(default)]
    pub task_type: TaskType,
    /// 目标选择器
    pub target_selector: TargetSelector,
    /// 任务内容（根据task_type不同，内容格式不同）
    /// - command: 命令字符串
    /// - script: 脚本内容或脚本ID
    /// - config: 配置JSON
    /// - file_transfer: 文件路径或URL
    /// - upgrade: 升级包路径或URL
    /// - reboot: 无需内容
    pub content: String,
    /// 执行策略
    pub execution_policy: ExecutionPolicy,
    /// 创建者
    pub created_by: Option<String>,
    /// 创建时间
    pub created_at: DateTime<Utc>,
    /// 更新时间
    pub updated_at: DateTime<Utc>,
    /// 任务状态
    #[serde(default)]
    pub status: TaskStatus,
    /// 进度信息
    #[serde(default)]
    pub progress: TaskProgress,
    /// 任务优先级 (0-100, 越高越优先)
    #[serde(default = "default_priority")]
    pub priority: u8,
    /// 预定执行时间 (可选，用于定时任务)
    #[serde(default)]
    pub scheduled_at: Option<DateTime<Utc>>,
    /// 任务结果摘要
    #[serde(default)]
    pub summary: Option<TaskSummary>,
}

fn default_priority() -> u8 { 50 }

impl BatchTask {
    pub fn new(name: String, task_type: TaskType, target_selector: TargetSelector, content: String) -> Self {
        let now = Utc::now();
        Self {
            id: Uuid::new_v4(),
            name,
            description: None,
            task_type,
            target_selector,
            content,
            execution_policy: ExecutionPolicy::default(),
            created_by: None,
            created_at: now,
            updated_at: now,
            status: TaskStatus::Pending,
            progress: TaskProgress::default(),
            priority: 50,
            scheduled_at: None,
            summary: None,
        }
    }

    pub fn with_description(mut self, description: String) -> Self {
        self.description = Some(description);
        self
    }

    pub fn with_execution_policy(mut self, policy: ExecutionPolicy) -> Self {
        self.execution_policy = policy;
        self
    }

    pub fn with_created_by(mut self, creator: String) -> Self {
        self.created_by = Some(creator);
        self
    }

    pub fn with_priority(mut self, priority: u8) -> Self {
        self.priority = priority.min(100);
        self
    }

    pub fn schedule_at(mut self, time: DateTime<Utc>) -> Self {
        self.scheduled_at = Some(time);
        self
    }

    pub fn is_terminal(&self) -> bool {
        self.status.is_terminal()
    }
}

/// 任务进度
#[derive(Debug, Clone, Default, Serialize, Deserialize)]
pub struct TaskProgress {
    /// 目标设备总数
    pub total: u32,
    /// 已完成数量
    pub completed: u32,
    /// 失败数量
    pub failed: u32,
    /// 正在执行数量
    pub running: u32,
    /// 等待中数量
    pub pending: u32,
    /// 已取消数量
    pub cancelled: u32,
    /// 当前批次索引 (用于batch/rolling模式)
    pub current_batch: u32,
    /// 开始时间
    pub started_at: Option<DateTime<Utc>>,
    /// 完成时间
    pub completed_at: Option<DateTime<Utc>>,
    /// 预计剩余时间（秒）
    pub estimated_remaining_secs: Option<u64>,
}

impl TaskProgress {
    pub fn new(total: u32) -> Self {
        Self {
            total,
            completed: 0,
            failed: 0,
            running: 0,
            pending: total,
            cancelled: 0,
            current_batch: 0,
            started_at: None,
            completed_at: None,
            estimated_remaining_secs: None,
        }
    }

    pub fn completion_percent(&self) -> f32 {
        if self.total == 0 {
            return 100.0;
        }
        (self.completed as f32 / self.total as f32) * 100.0
    }

    pub fn failure_percent(&self) -> f32 {
        if self.total == 0 {
            return 0.0;
        }
        (self.failed as f32 / self.total as f32) * 100.0
    }

    pub fn is_exceeding_threshold(&self, threshold: f32) -> bool {
        self.failure_percent() > threshold
    }
}

/// 任务结果摘要
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct TaskSummary {
    /// 成功数量
    pub success_count: u32,
    /// 失败数量
    pub failure_count: u32,
    /// 总执行时间（秒）
    pub total_duration_secs: u64,
    /// 结果详情列表
    pub results: Vec<DeviceResult>,
}

/// 单个设备执行结果
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct DeviceResult {
    /// 设备ID
    pub device_id: String,
    /// 设备名称
    pub device_name: Option<String>,
    /// 执行状态
    pub status: DeviceResultStatus,
    /// 开始时间
    pub started_at: DateTime<Utc>,
    /// 结束时间
    pub completed_at: Option<DateTime<Utc>>,
    /// 执行时长（毫秒）
    pub duration_ms: Option<u64>,
    /// 输出内容
    pub output: Option<String>,
    /// 错误信息
    pub error: Option<String>,
    /// 重试次数
    pub retry_count: u32,
}

/// 设备执行结果状态
#[derive(Debug, Clone, Copy, PartialEq, Eq, Hash, Serialize, Deserialize)]
#[serde(rename_all = "snake_case")]
pub enum DeviceResultStatus {
    /// 成功
    Success,
    /// 失败
    Failed,
    /// 超时
    Timeout,
    /// 取消
    Cancelled,
    /// 跳过
    Skipped,
}
```

### 2.2 任务内容类型定义

```rust
// batch_task_content.rs - 不同任务类型的具体内容

use serde::{Deserialize, Serialize};

/// 命令任务内容
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct CommandContent {
    /// 要执行的命令
    pub command: String,
    /// 命令参数
    pub args: Vec<String>,
    /// 工作目录
    pub working_dir: Option<String>,
    /// 环境变量
    pub env: std::collections::HashMap<String, String>,
    /// 是否使用shell执行
    pub use_shell: bool,
}

/// 脚本任务内容
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct ScriptContent {
    /// 脚本ID (如果引用已有脚本)
    pub script_id: Option<Uuid>,
    /// 脚本内容 (如果是内联脚本)
    pub content: Option<String>,
    /// 脚本类型
    pub script_type: ScriptType,
    /// 脚本参数
    pub parameters: std::collections::HashMap<String, String>,
}

/// 配置任务内容
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct ConfigContent {
    /// 配置类型 (如: "network", "security", "app_settings")
    pub config_type: String,
    /// 配置内容 (JSON格式)
    pub config: serde_json::Value,
    /// 是否强制覆盖
    pub force: bool,
    /// 备份原配置
    pub backup: bool,
}

/// 文件传输任务内容
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct FileTransferContent {
    /// 源路径 (本地或URL)
    pub source: String,
    /// 目标路径 (远程设备)
    pub destination: String,
    /// 传输模式
    pub mode: FileTransferMode,
    /// 传输选项
    pub options: FileTransferOptions,
}

/// 文件传输模式
#[derive(Debug, Clone, Copy, PartialEq, Eq, Hash, Serialize, Deserialize)]
#[serde(rename_all = "snake_case")]
pub enum FileTransferMode {
    /// 上传
    Upload,
    /// 下载
    Download,
    /// 推送 (推送到所有目标)
    Push,
    /// 拉取 (从所有目标拉取)
    Pull,
}

/// 文件传输选项
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct FileTransferOptions {
    /// 是否压缩
    pub compress: bool,
    /// 是否验证完整性 (checksum)
    pub verify: bool,
    /// 带宽限制 (bytes/s)
    pub bandwidth_limit: Option<u64>,
    /// 覆盖模式
    pub overwrite: OverwriteMode,
}

/// 覆盖模式
#[derive(Debug, Clone, Copy, PartialEq, Eq, Hash, Serialize, Deserialize)]
#[serde(rename_all = "snake_case")]
pub enum OverwriteMode {
    /// 总是覆盖
    Always,
    /// 从不覆盖
    Never,
    /// 仅当源更新时覆盖
    IfNewer,
}

/// 升级任务内容
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct UpgradeContent {
    /// 升级包路径或URL
    pub package_url: String,
    /// 升级包校验和
    pub checksum: Option<String>,
    /// 升级前备份
    pub backup: bool,
    /// 升级后验证
    pub verify: bool,
    /// 升级命令 (可选，自定义升级脚本)
    pub upgrade_command: Option<String>,
    /// 回滚策略
    pub rollback_policy: RollbackPolicy,
}

/// 回滚策略
#[derive(Debug, Clone, Copy, PartialEq, Eq, Hash, Serialize, Deserialize)]
#[serde(rename_all = "snake_case")]
pub enum RollbackPolicy {
    /// 自动回滚
    Auto,
    /// 手动回滚
    Manual,
    /// 不回滚
    NoRollback,
}

/// 重启任务内容
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct RebootContent {
    /// 重启类型
    pub reboot_type: RebootType,
    /// 重启前延迟（秒）
    pub delay_secs: u32,
    /// 重启后等待时间（秒）
    pub wait_secs: u32,
    /// 是否强制重启 (忽略未保存的工作)
    pub force: bool,
    /// 重启后执行的验证命令
    pub verify_command: Option<String>,
}

/// 重启类型
#[derive(Debug, Clone, Copy, PartialEq, Eq, Hash, Serialize, Deserialize)]
#[serde(rename_all = "snake_case")]
pub enum RebootType {
    /// 正常重启
    Normal,
    /// 强制重启
    Force,
    /// 关机后开机
    Cycle,
}
```

### 2.3 数据库Schema

```sql
-- batch_tasks.sql

-- 批量任务表
CREATE TABLE IF NOT EXISTS batch_tasks (
    id TEXT PRIMARY KEY,
    name TEXT NOT NULL,
    description TEXT,
    task_type TEXT NOT NULL DEFAULT 'command',
    target_selector TEXT NOT NULL,  -- JSON
    content TEXT NOT NULL,
    execution_policy TEXT NOT NULL, -- JSON
    created_by TEXT,
    created_at TEXT NOT NULL,
    updated_at TEXT NOT NULL,
    status TEXT NOT NULL DEFAULT 'pending',
    progress TEXT NOT NULL DEFAULT '{}', -- JSON
    priority INTEGER NOT NULL DEFAULT 50,
    scheduled_at TEXT,
    summary TEXT, -- JSON
    
    -- 索引
    INDEX idx_tasks_status (status),
    INDEX idx_tasks_created_at (created_at),
    INDEX idx_tasks_scheduled_at (scheduled_at)
);

-- 任务执行历史表
CREATE TABLE IF NOT EXISTS batch_task_executions (
    id TEXT PRIMARY KEY,
    task_id TEXT NOT NULL,
    started_at TEXT NOT NULL,
    completed_at TEXT,
    status TEXT NOT NULL,
    
    FOREIGN KEY (task_id) REFERENCES batch_tasks(id) ON DELETE CASCADE,
    INDEX idx_executions_task_id (task_id)
);

-- 设备执行结果表
CREATE TABLE IF NOT EXISTS batch_device_results (
    id TEXT PRIMARY KEY,
    execution_id TEXT NOT NULL,
    device_id TEXT NOT NULL,
    device_name TEXT,
    status TEXT NOT NULL,
    started_at TEXT NOT NULL,
    completed_at TEXT,
    duration_ms INTEGER,
    output TEXT,
    error TEXT,
    retry_count INTEGER NOT NULL DEFAULT 0,
    
    FOREIGN KEY (execution_id) REFERENCES batch_task_executions(id) ON DELETE CASCADE,
    INDEX idx_results_device_id (device_id),
    INDEX idx_results_execution_id (execution_id)
);

-- 设备表 (用于目标选择)
CREATE TABLE IF NOT EXISTS devices (
    id TEXT PRIMARY KEY,
    name TEXT NOT NULL,
    groups TEXT NOT NULL DEFAULT '[]', -- JSON数组
    tags TEXT NOT NULL DEFAULT '{}',   -- JSON对象
    metadata TEXT NOT NULL DEFAULT '{}', -- JSON对象
    status TEXT NOT NULL DEFAULT 'unknown',
    last_seen_at TEXT,
    created_at TEXT NOT NULL,
    updated_at TEXT NOT NULL,
    
    INDEX idx_devices_status (status),
    INDEX idx_devices_groups (groups),
    INDEX idx_devices_tags (tags)
);

-- 设备分组表
CREATE TABLE IF NOT EXISTS device_groups (
    id TEXT PRIMARY KEY,
    name TEXT NOT NULL UNIQUE,
    description TEXT,
    parent_id TEXT,
    created_at TEXT NOT NULL,
    updated_at TEXT NOT NULL
);

-- 设备标签表
CREATE TABLE IF NOT EXISTS device_tags (
    key TEXT NOT NULL,
    value TEXT NOT NULL,
    device_id TEXT NOT NULL,
    created_at TEXT NOT NULL,
    
    PRIMARY KEY (device_id, key),
    FOREIGN KEY (device_id) REFERENCES devices(id) ON DELETE CASCADE,
    INDEX idx_tags_key_value (key, value)
);
```

---

## 3. TargetSelector 选择器实现方案

### 3.1 选择器解析器

```rust
// target_selector.rs - 目标选择器实现

use crate::batch_task::{SelectorType, TargetSelector};
use anyhow::{Context, Result};
use std::collections::HashSet;

/// 设备信息
#[derive(Debug, Clone)]
pub struct DeviceInfo {
    pub id: String,
    pub name: String,
    pub groups: Vec<String>,
    pub tags: HashMap<String, String>,
    pub status: DeviceStatus,
    pub metadata: HashMap<String, String>,
}

#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum DeviceStatus {
    Online,
    Offline,
    Unknown,
}

/// 设备注册表 - 管理所有设备信息
pub struct DeviceRegistry {
    devices: HashMap<String, DeviceInfo>,
    groups: HashMap<String, Vec<String>>,  // group_name -> device_ids
    tags: HashMap<String, HashMap<String, Vec<String>>>,  // tag_key -> tag_value -> device_ids
}

impl DeviceRegistry {
    pub fn new() -> Self {
        Self {
            devices: HashMap::new(),
            groups: HashMap::new(),
            tags: HashMap::new(),
        }
    }

    /// 注册设备
    pub fn register_device(&mut self, device: DeviceInfo) {
        let device_id = device.id.clone();
        
        // 添加到设备列表
        self.devices.insert(device_id.clone(), device.clone());
        
        // 更新分组索引
        for group in &device.groups {
            self.groups
                .entry(group.clone())
                .or_insert_with(Vec::new)
                .push(device_id.clone());
        }
        
        // 更新标签索引
        for (key, value) in &device.tags {
            self.tags
                .entry(key.clone())
                .or_insert_with(HashMap::new)
                .entry(value.clone())
                .or_insert_with(Vec::new)
                .push(device_id.clone());
        }
    }

    /// 根据选择器获取目标设备ID列表
    pub fn select_devices(&self, selector: &TargetSelector) -> Result<Vec<String>> {
        match selector.selector_type {
            SelectorType::All => self.select_all(),
            SelectorType::Group => self.select_by_groups(&selector.groups),
            SelectorType::Tag => self.select_by_tags(&selector.tags),
            SelectorType::DeviceIds => self.select_by_ids(&selector.device_ids),
            SelectorType::Filter => self.select_by_filter(
                selector.filter_expr.as_deref()
                    .context("filter expression is required")?
            ),
        }
    }

    /// 选择所有设备
    fn select_all(&self) -> Result<Vec<String>> {
        Ok(self.devices.keys().cloned().collect())
    }

    /// 按分组选择
    fn select_by_groups(&self, groups: &[String]) -> Result<Vec<String>> {
        let mut result = HashSet::new();
        for group in groups {
            if let Some(device_ids) = self.groups.get(group) {
                result.extend(device_ids.iter().cloned());
            }
        }
        Ok(result.into_iter().collect())
    }

    /// 按标签选择
    fn select_by_tags(&self, tags: &[String]) -> Result<Vec<String>> {
        // 标签选择支持 "key:value" 格式
        let mut result = HashSet::new();
        
        for tag in tags {
            if tag.contains(':') {
                // 精确匹配 key:value
                let parts: Vec<&str> = tag.splitn(2, ':').collect();
                if parts.len() == 2 {
                    if let Some(value_map) = self.tags.get(parts[0]) {
                        if let Some(device_ids) = value_map.get(parts[1]) {
                            result.extend(device_ids.iter().cloned());
                        }
                    }
                }
            } else {
                // 只匹配key，存在该标签的所有设备
                if let Some(value_map) = self.tags.get(tag) {
                    for device_ids in value_map.values() {
                        result.extend(device_ids.iter().cloned());
                    }
                }
            }
        }
        
        Ok(result.into_iter().collect())
    }

    /// 按设备ID列表选择
    fn select_by_ids(&self, device_ids: &[String]) -> Result<Vec<String>> {
        let mut result = Vec::new();
        for id in device_ids {
            if self.devices.contains_key(id) {
                result.push(id.clone());
            }
        }
        Ok(result)
    }

    /// 按过滤表达式选择
    fn select_by_filter(&self, filter_expr: &str) -> Result<Vec<String>> {
        let ast = FilterParser::parse(filter_expr)
            .context("failed to parse filter expression")?;
        
        let result = self.devices.values()
            .filter(|device| ast.evaluate(device))
            .map(|device| device.id.clone())
            .collect();
        
        Ok(result)
    }

    /// 获取设备信息
    pub fn get_device(&self, device_id: &str) -> Option<&DeviceInfo> {
        self.devices.get(device_id)
    }
}

impl Default for DeviceRegistry {
    fn default() -> Self {
        Self::new()
    }
}

/// 过滤表达式AST
#[derive(Debug, Clone)]
pub enum FilterExpr {
    /// 属性比较
    Compare {
        field: String,
        op: CompareOp,
        value: FilterValue,
    },
    /// 逻辑与
    And(Box<FilterExpr>, Box<FilterExpr>),
    /// 逻辑或
    Or(Box<FilterExpr>, Box<FilterExpr>),
    /// 逻辑非
    Not(Box<FilterExpr>),
    /// 分组表达式
    Group(Box<FilterExpr>),
}

#[derive(Debug, Clone, Copy)]
pub enum CompareOp {
    Eq,
    Ne,
    Lt,
    Le,
    Gt,
    Ge,
    Contains,
    StartsWith,
    EndsWith,
}

#[derive(Debug, Clone)]
pub enum FilterValue {
    String(String),
    Number(f64),
    Boolean(bool),
}

impl FilterExpr {
    pub fn evaluate(&self, device: &DeviceInfo) -> bool {
        match self {
            FilterExpr::Compare { field, op, value } => {
                Self::compare_device_field(device, field, *op, value)
            }
            FilterExpr::And(left, right) => {
                left.evaluate(device) && right.evaluate(device)
            }
            FilterExpr::Or(left, right) => {
                left.evaluate(device) || right.evaluate(device)
            }
            FilterExpr::Not(expr) => !expr.evaluate(device),
            FilterExpr::Group(expr) => expr.evaluate(device),
        }
    }

    fn compare_device_field(device: &DeviceInfo, field: &str, op: CompareOp, value: &FilterValue) -> bool {
        let field_value = match field {
            "id" => Some(FilterValue::String(device.id.clone())),
            "name" => Some(FilterValue::String(device.name.clone())),
            "status" => Some(FilterValue::String(format!("{:?}", device.status).to_lowercase())),
            _ => device.metadata.get(field).map(|v| FilterValue::String(v.clone())),
        };

        let Some(field_value) = field_value else {
            return false;
        };

        match (field_value, value, op) {
            (FilterValue::String(l), FilterValue::String(r), CompareOp::Eq) => l == r,
            (FilterValue::String(l), FilterValue::String(r), CompareOp::Ne) => l != r,
            (FilterValue::String(l), FilterValue::String(r), CompareOp::Contains) => l.contains(&r),
            (FilterValue::String(l), FilterValue::String(r), CompareOp::StartsWith) => l.starts_with(&r),
            (FilterValue::String(l), FilterValue::String(r), CompareOp::EndsWith) => l.ends_with(&r),
            _ => false,
        }
    }
}

/// 过滤表达式解析器
pub struct FilterParser;

impl FilterParser {
    pub fn parse(expr: &str) -> Result<FilterExpr> {
        let mut chars = expr.chars().peekable();
        Self::parse_expr(&mut chars)
    }

    fn parse_expr(chars: &mut std::iter::Peekable<std::str::Chars>) -> Result<FilterExpr> {
        let mut left = Self::parse_and(chars)?;
        
        while let Some(&'o') = chars.peek() {
            chars.next();
            if let Some(&'r') = chars.peek() {
                chars.next();
                let right = Self::parse_and(chars)?;
                left = FilterExpr::Or(Box::new(left), Box::new(right));
            }
        }
        
        Ok(left)
    }

    fn parse_and(chars: &mut std::iter::Peekable<std::str::Chars>) -> Result<FilterExpr> {
        let mut left = Self::parse_not(chars)?;
        
        while let Some(&'a') = chars.peek() {
            if let Some(next) = chars.clone().nth(1) {
                if next == 'n' {
                    chars.next(); // consume 'a'
                    chars.next(); // consume 'n'
                    let right = Self::parse_not(chars)?;
                    left = FilterExpr::And(Box::new(left), Box::new(right));
                } else {
                    break;
                }
            } else {
                break;
            }
        }
        
        Ok(left)
    }

    fn parse_not(chars: &mut std::iter::Peekable<std::str::Chars>) -> Result<FilterExpr> {
        if let Some(&'n') = chars.peek() {
            chars.next();
            if let Some(&'o') = chars.peek() {
                chars.next();
                if let Some(&'t') = chars.peek() {
                    chars.next();
                    let expr = Self::parse_not(chars)?;
                    return Ok(FilterExpr::Not(Box::new(expr)));
                }
            }
        }
        
        Self::parse_compare(chars)
    }

    fn parse_compare(chars: &mut std::iter::Peekable<std::str::Chars>) -> Result<FilterExpr> {
        // 跳过空白
        while chars.peek().map(|c| c.is_whitespace()).unwrap_or(false) {
            chars.next();
        }
        
        // 读取字段名
        let field = Self::read_identifier(chars);
        
        // 跳过空白
        while chars.peek().map(|c| c.is_whitespace()).unwrap_or(false) {
            chars.next();
        }
        
        // 读取操作符
        let op = Self::read_operator(chars);
        
        // 跳过空白
        while chars.peek().map(|c| c.is_whitespace()).unwrap_or(false) {
            chars.next();
        }
        
        // 读取值
        let value = Self::read_value(chars)?;
        
        Ok(FilterExpr::Compare {
            field,
            op,
            value,
        })
    }

    fn read_identifier(chars: &mut std::iter::Peekable<std::str::Chars>) -> String {
        let mut ident = String::new();
        while let Some(c) = chars.peek() {
            if c.is_alphanumeric() || *c == '_' {
                ident.push(*c);
                chars.next();
            } else {
                break;
            }
        }
        ident
    }

    fn read_operator(chars: &mut std::iter::Peekable<std::str::Chars>) -> CompareOp {
        let mut op_str = String::new();
        while let Some(c) = chars.peek() {
            if c.is_ascii_alphabetic() || *c == '=' || *c == '!' || *c == '<' || *c == '>' {
                op_str.push(*c);
                chars.next();
            } else {
                break;
            }
        }
        
        match op_str.as_str() {
            "==" => CompareOp::Eq,
            "!=" => CompareOp::Ne,
            "<" => CompareOp::Lt,
            "<=" => CompareOp::Le,
            ">" => CompareOp::Gt,
            ">=" => CompareOp::Ge,
            "contains" => CompareOp::Contains,
            "startswith" => CompareOp::StartsWith,
            "endswith" => CompareOp::EndsWith,
            _ => CompareOp::Eq,
        }
    }

    fn read_value(chars: &mut std::iter::Peekable<std::str::Chars>) -> Result<FilterValue> {
        let Some(c) = chars.next() else {
            anyhow::bail!("unexpected end of input");
        };
        
        if c == '\'' || c == '"' {
            // 字符串值
            let mut value = String::new();
            while let Some(ch) = chars.next() {
                if ch == c {
                    break;
                }
                if ch == '\\' {
                    if let Some escaped) = chars.next() {
                        value.push(escaped);
                    }
                } else {
                    value.push(ch);
                }
            }
            Ok(FilterValue::String(value))
        } else if c.is_digit(10) || c == '-' {
            // 数字值
            let mut num_str = String::from(c);
            while let Some(d) = chars.peek() {
                if d.is_digit(10) || *d == '.' {
                    num_str.push(*d);
                    chars.next();
                } else {
                    break;
                }
            }
            let num: f64 = num_str.parse()
                .context("invalid number")?;
            Ok(FilterValue::Number(num))
        } else if c.is_alphabetic() {
            // 布尔值或标识符
            let mut ident = String::from(c);
            while let Some(d) = chars.peek() {
                if d.is_alphabetic() {
                    ident.push(*d);
                    chars.next();
                } else {
                    break;
                }
            }
            match ident.to_lowercase().as_str() {
                "true" => Ok(FilterValue::Boolean(true)),
                "false" => Ok(FilterValue::Boolean(false)),
                _ => Ok(FilterValue::String(ident)),
            }
        } else {
            anyhow::bail!("unexpected character: {}", c);
        }
    }
}
```

### 3.2 选择器验证

```rust
impl TargetSelector {
    /// 验证选择器是否有效
    pub fn validate(&self) -> Result<()> {
        match self.selector_type {
            SelectorType::All => {
                // All类型不需要其他字段
                Ok(())
            }
            SelectorType::Group => {
                if self.groups.is_empty() {
                    anyhow::bail!("groups cannot be empty when selector_type is 'group'");
                }
                Ok(())
            }
            SelectorType::Tag => {
                if self.tags.is_empty() {
                    anyhow::bail!("tags cannot be empty when selector_type is 'tag'");
                }
                Ok(())
            }
            SelectorType::DeviceIds => {
                if self.device_ids.is_empty() {
                    anyhow::bail!("device_ids cannot be empty when selector_type is 'device_ids'");
                }
                for id in &self.device_ids {
                    if id.is_empty() {
                        anyhow::bail!("device_id cannot be empty");
                    }
                }
                Ok(())
            }
            SelectorType::Filter => {
                if self.filter_expr.is_none() || self.filter_expr.as_ref().unwrap().is_empty() {
                    anyhow::bail!("filter_expr cannot be empty when selector_type is 'filter'");
                }
                // 验证表达式可以被解析
                FilterParser::parse(self.filter_expr.as_ref().unwrap())?;
                Ok(())
            }
        }
    }
}
```

---

## 4. ExecutionPolicy 执行策略实现方案

### 4.1 执行引擎核心

```rust
// batch_executor.rs - 批量执行引擎

use crate::batch_task::{
    BatchTask, DeviceResult, DeviceResultStatus, ExecutionMode, ExecutionPolicy,
    TaskProgress, TaskStatus, TaskSummary,
};
use crate::target_selector::DeviceRegistry;
use anyhow::{Context, Result};
use chrono::{DateTime, Utc};
use std::collections::HashMap;
use std::sync::Arc;
use tokio::sync::{mpsc, watch, RwLock};
use tokio::time::{sleep, Duration};
use tracing::{debug, error, info, warn};
use uuid::Uuid;

/// 执行器错误类型
#[derive(Debug, thiserror::Error)]
pub enum ExecutorError {
    #[error("task not found: {0}")]
    TaskNotFound(Uuid),
    
    #[error("task already running: {0}")]
    TaskAlreadyRunning(Uuid),
    
    #[error("task is in terminal state: {0}")]
    TaskInTerminalState(Uuid),
    
    #[error("device execution failed: {0}")]
    DeviceExecutionFailed(String),
    
    #[error("execution cancelled")]
    Cancelled,
}

/// 批量执行器
pub struct BatchExecutor {
    /// 设备注册表
    device_registry: Arc<DeviceRegistry>,
    /// 任务存储
    task_store: Arc<dyn TaskStore>,
    /// 运行中的任务
    running_tasks: RwLock<HashMap<Uuid, RunningTask>>,
    /// 取消信号发送器
    cancel_txs: RwLock<HashMap<Uuid, watch::Sender<bool>>>,
}

struct RunningTask {
    task_id: Uuid,
    started_at: DateTime<Utc>,
    handle: tokio::task::JoinHandle<()>,
}

impl BatchExecutor {
    pub fn new(
        device_registry: Arc<DeviceRegistry>,
        task_store: Arc<dyn TaskStore>,
    ) -> Self {
        Self {
            device_registry,
            task_store,
            running_tasks: RwLock::new(HashMap::new()),
            cancel_txs: RwLock::new(HashMap::new()),
        }
    }

    /// 创建并启动批量任务
    pub async fn start_task(&self, task: BatchTask) -> Result<Uuid> {
        let task_id = task.id;
        
        // 验证任务不在终端状态
        if task.status.is_terminal() {
            return Err(ExecutorError::TaskInTerminalState(task_id).into());
        }
        
        // 检查是否已在运行
        {
            let running = self.running_tasks.read().await;
            if running.contains_key(&task_id) {
                return Err(ExecutorError::TaskAlreadyRunning(task_id).into());
            }
        }
        
        // 创建取消信号
        let (cancel_tx, cancel_rx) = watch::channel(false);
        
        // 获取目标设备列表
        let target_devices = self.device_registry.select_devices(&task.target_selector)
            .context("failed to select target devices")?;
        
        if target_devices.is_empty() {
            anyhow::bail!("no target devices selected");
        }
        
        info!(
            task_id = %task_id,
            target_count = target_devices.len(),
            mode = ?task.execution_policy.mode,
            "starting batch task"
        );
        
        // 更新任务状态
        let mut updated_task = task;
        updated_task.status = TaskStatus::Running;
        updated_task.progress = TaskProgress::new(target_devices.len() as u32);
        updated_task.progress.started_at = Some(Utc::now());
        
        self.task_store.save_task(&updated_task).await?;
        
        // 创建执行器处理函数
        let device_registry = Arc::clone(&self.device_registry);
        let task_store = Arc::clone(&self.task_store);
        
        let handle = tokio::spawn(async move {
            Self::execute_task(
                task_id,
                updated_task,
                target_devices,
                device_registry,
                task_store,
                cancel_rx,
            ).await;
        });
        
        // 保存运行状态
        {
            let mut running = self.running_tasks.write().await;
            running.insert(task_id, RunningTask {
                task_id,
                started_at: Utc::now(),
                handle,
            });
        }
        
        {
            let mut txs = self.cancel_txs.write().await;
            txs.insert(task_id, cancel_tx);
        }
        
        Ok(task_id)
    }

    /// 停止运行中的任务
    pub async fn stop_task(&self, task_id: Uuid) -> Result<()> {
        // 发送取消信号
        {
            let txs = self.cancel_txs.read().await;
            if let Some(tx) = txs.get(&task_id) {
                let _ = tx.send(true);
            }
        }
        
        // 等待任务结束
        {
            let mut running = self.running_tasks.write().await;
            if let Some(rt) = running.remove(&task_id) {
                rt.handle.abort();
            }
        }
        
        // 更新任务状态
        if let Some(mut task) = self.task_store.get_task(task_id).await? {
            task.status = TaskStatus::Cancelled;
            task.progress.completed_at = Some(Utc::now());
            self.task_store.save_task(&task).await?;
        }
        
        info!(task_id = %task_id, "task stopped");
        
        Ok(())
    }

    /// 暂停运行中的任务
    pub async fn pause_task(&self, task_id: Uuid) -> Result<()> {
        // 与stop不同，pause保留当前进度，可以恢复
        // 发送取消信号，但不标记为cancelled
        {
            let txs = self.cancel_txs.read().await;
            if let Some(tx) = txs.get(&task_id) {
                let _ = tx.send(true);
            }
        }
        
        // 等待当前批次完成
        // TODO: 实现暂停逻辑
        
        // 更新任务状态
        if let Some(mut task) = self.task_store.get_task(task_id).await? {
            task.status = TaskStatus::Paused;
            self.task_store.save_task(&task).await?;
        }
        
        Ok(())
    }

    /// 恢复暂停的任务
    pub async fn resume_task(&self, task_id: Uuid) -> Result<()> {
        let task = self.task_store.get_task(task_id).await?
            .context("task not found")?;
        
        if task.status != TaskStatus::Paused {
            anyhow::bail!("task is not paused");
        }
        
        // 重新启动任务，保持之前的进度
        self.start_task(task).await?;
        
        Ok(())
    }

    /// 获取任务进度
    pub async fn get_progress(&self, task_id: Uuid) -> Result<TaskProgress> {
        let task = self.task_store.get_task(task_id).await?
            .context("task not found")?;
        Ok(task.progress)
    }

    /// 获取任务结果
    pub async fn get_results(&self, task_id: Uuid) -> Result<Vec<DeviceResult>> {
        self.task_store.get_task_results(task_id).await
    }

    /// 执行任务的核心逻辑
    async fn execute_task(
        task_id: Uuid,
        mut task: BatchTask,
        target_devices: Vec<String>,
        device_registry: Arc<DeviceRegistry>,
        task_store: Arc<dyn TaskStore>,
        mut cancel_rx: watch::Receiver<bool>,
    ) {
        let policy = &task.execution_policy;
        let results = Arc::new(RwLock::new(Vec::new()));
        let total = target_devices.len();
        
        // 根据执行模式执行
        match policy.mode {
            ExecutionMode::Parallel => {
                Self::execute_parallel(
                    &task,
                    &target_devices,
                    &device_registry,
                    results.clone(),
                    &mut cancel_rx,
                    &mut task.progress,
                ).await;
            }
            ExecutionMode::Batch => {
                let batch_size = policy.batch_size.unwrap_or(10) as usize;
                Self::execute_batched(
                    &task,
                    &target_devices,
                    &device_registry,
                    results.clone(),
                    &mut cancel_rx,
                    &mut task.progress,
                    batch_size,
                ).await;
            }
            ExecutionMode::Rolling => {
                let initial = policy.rolling_initial_batch.unwrap_or(1) as usize;
                let increment = policy.rolling_increment.unwrap_or(5) as usize;
                Self::execute_rolling(
                    &task,
                    &target_devices,
                    &device_registry,
                    results.clone(),
                    &mut cancel_rx,
                    &mut task.progress,
                    initial,
                    increment,
                ).await;
            }
        }
        
        // 收集结果
        let all_results = results.read().await.clone();
        
        // 计算摘要
        let success_count = all_results.iter()
            .filter(|r| r.status == DeviceResultStatus::Success)
            .count() as u32;
        let failure_count = all_results.iter()
            .filter(|r| r.status == DeviceResultStatus::Failed)
            .count() as u32;
        
        let total_duration = all_results.iter()
            .filter_map(|r| r.duration_ms)
            .sum::<u64>();
        
        task.summary = Some(TaskSummary {
            success_count,
            failure_count,
            total_duration_secs: total_duration / 1000,
            results: all_results,
        });
        
        task.progress.completed_at = Some(Utc::now());
        
        // 确定最终状态
        task.status = if task.progress.failure_percent() > policy.failure_threshold_percent {
            TaskStatus::Paused  // 触发熔断
        } else if task.progress.cancelled > 0 {
            TaskStatus::Cancelled
        } else if task.progress.failed == total as u32 {
            TaskStatus::Failed
        } else {
            TaskStatus::Completed
        };
        
        // 保存最终状态
        if let Err(e) = task_store.save_task(&task).await {
            error!(task_id = %task_id, error = %e, "failed to save final task state");
        }
        
        // 清理运行状态
        info!(
            task_id = %task_id,
            status = ?task.status,
            success = success_count,
            failed = failure_count,
            "batch task completed"
        );
    }

    /// 全量并发执行
    async fn execute_parallel(
        task: &BatchTask,
        devices: &[String],
        device_registry: &Arc<DeviceRegistry>,
        results: Arc<RwLock<Vec<DeviceResult>>>,
        cancel_rx: &mut watch::Receiver<bool>,
        progress: &mut TaskProgress,
    ) {
        let policy = &task.execution_policy;
        
        // 创建执行器
        let (tx, mut rx) = mpsc::channel::<DeviceResult>(devices.len());
        
        for device_id in devices {
            let tx = tx.clone();
            let device_registry = Arc::clone(device_registry);
            let task = task.clone();
            let policy = policy.clone();
            
            tokio::spawn(async move {
                let result = Self::execute_on_device(
                    &task,
                    &device_id,
                    &device_registry,
                    &policy,
                ).await;
                
                let _ = tx.send(result).await;
            });
        }
        
        // 收集结果
        drop(tx);
        while let Some(result) = rx.recv().await {
            Self::update_progress(progress, &result);
            results.write().await.push(result);
            
            // 检查是否需要停止
            if *cancel_rx.borrow() {
                break;
            }
            
            // 检查熔断阈值
            if progress.is_exceeding_threshold(policy.failure_threshold_percent) && !policy.continue_on_failure {
                warn!("failure threshold exceeded, stopping execution");
                break;
            }
        }
    }

    /// 分批执行
    async fn execute_batched(
        task: &BatchTask,
        devices: &[String],
        device_registry: &Arc<DeviceRegistry>,
        results: Arc<RwLock<Vec<DeviceResult>>>,
        cancel_rx: &mut watch::Receiver<bool>,
        progress: &mut TaskProgress,
        batch_size: usize,
    ) {
        let policy = &task.execution_policy;
        
        for (batch_idx, chunk) in devices.chunks(batch_size).enumerate() {
            progress.current_batch = batch_idx as u32 + 1;
            
            info!(
                task_id = %task.id,
                batch = progress.current_batch,
                total_batches = (devices.len() + batch_size - 1) / batch_size,
                "executing batch"
            );
            
            // 执行当前批次
            let (tx, mut rx) = mpsc::channel::<DeviceResult>(chunk.len());
            
            for device_id in chunk {
                let tx = tx.clone();
                let device_registry = Arc::clone(device_registry);
                let task = task.clone();
                let policy = policy.clone();
                
                tokio::spawn(async move {
                    let result = Self::execute_on_device(
                        &task,
                        device_id,
                        &device_registry,
                        &policy,
                    ).await;
                    
                    let _ = tx.send(result).await;
                });
            }
            
            drop(tx);
            while let Some(result) = rx.recv().await {
                Self::update_progress(progress, &result);
                results.write().await.push(result);
                
                // 检查是否需要停止
                if *cancel_rx.borrow() {
                    break;
                }
            }
            
            // 检查熔断
            if progress.is_exceeding_threshold(policy.failure_threshold_percent) && !policy.continue_on_failure {
                warn!("failure threshold exceeded, stopping execution");
                break;
            }
            
            // 批次间延迟
            if batch_idx < devices.len() / batch_size {
                sleep(Duration::from_secs(1)).await;
            }
        }
    }

    /// 滚动执行
    async fn execute_rolling(
        task: &BatchTask,
        devices: &[String],
        device_registry: &Arc<DeviceRegistry>,
        results: Arc<RwLock<Vec<DeviceResult>>>,
        cancel_rx: &mut watch::Receiver<bool>,
        progress: &mut TaskProgress,
        initial_batch: usize,
        increment: usize,
    ) {
        let policy = &task.execution_policy;
        
        let mut current_batch_size = initial_batch;
        let mut start_idx = 0;
        
        while start_idx < devices.len() {
            let end_idx = (start_idx + current_batch_size).min(devices.len());
            let batch = &devices[start_idx..end_idx];
            
            progress.current_batch = ((start_idx / current_batch_size) + 1) as u32;
            
            info!(
                task_id = %task.id,
                batch = progress.current_batch,
                batch_size = batch.len(),
                "executing rolling batch"
            );
            
            // 执行当前批次
            let (tx, mut rx) = mpsc::channel::<DeviceResult>(batch.len());
            
            for device_id in batch {
                let tx = tx.clone();
                let device_registry = Arc::clone(device_registry);
                let task = task.clone();
                let policy = policy.clone();
                
                tokio::spawn(async move {
                    let result = Self::execute_on_device(
                        &task,
                        device_id,
                        &device_registry,
                        &policy,
                    ).await;
                    
                    let _ = tx.send(result).await;
                });
            }
            
            drop(tx);
            while let Some(result) = rx.recv().await {
                Self::update_progress(progress, &result);
                results.write().await.push(result);
                
                if *cancel_rx.borrow() {
                    break;
                }
            }
            
            // 检查熔断
            if progress.is_exceeding_threshold(policy.failure_threshold_percent) && !policy.continue_on_failure {
                warn!("failure threshold exceeded, stopping rolling execution");
                break;
            }
            
            start_idx = end_idx;
            current_batch_size = (current_batch_size + increment).min(devices.len());
            
            // 等待验证
            if start_idx < devices.len() {
                sleep(Duration::from_secs(5)).await;
            }
        }
    }

    /// 在单个设备上执行任务
    async fn execute_on_device(
        task: &BatchTask,
        device_id: &str,
        device_registry: &DeviceRegistry,
        policy: &ExecutionPolicy,
    ) -> DeviceResult {
        let started_at = Utc::now();
        let device_info = device_registry.get_device(device_id);
        
        let mut retry_count = 0;
        let max_retries = policy.retry_count;
        
        loop {
            let result = Self::do_execute_on_device(task, device_id, device_registry).await;
            
            match result {
                Ok(mut r) => {
                    r.retry_count = retry_count;
                    return r;
                }
                Err(e) => {
                    retry_count += 1;
                    if retry_count > max_retries {
                        return DeviceResult {
                            device_id: device_id.to_string(),
                            device_name: device_info.map(|d| d.name.clone()),
                            status: DeviceResultStatus::Failed,
                            started_at,
                            completed_at: Some(Utc::now()),
                            duration_ms: Some((Utc::now() - started_at).num_milliseconds() as u64),
                            output: None,
                            error: Some(e.to_string()),
                            retry_count,
                        };
                    }
                    
                    warn!(
                        device_id = device_id,
                        retry = retry_count,
                        max_retries = max_retries,
                        error = %e,
                        "device execution failed, retrying"
                    );
                    
                    sleep(Duration::from_secs(policy.retry_interval_secs as u64)).await;
                }
            }
        }
    }

    /// 实际执行逻辑
    async fn do_execute_on_device(
        task: &BatchTask,
        device_id: &str,
        device_registry: &DeviceRegistry,
    ) -> Result<DeviceResult> {
        let started_at = Utc::now();
        let device_info = device_registry.get_device(device_id);
        
        let (status, output, error) = match task.task_type {
            TaskType::Command => {
                Self::execute_command(device_id, &task.content).await
            }
            TaskType::Script => {
                Self::execute_script(device_id, &task.content).await
            }
            TaskType::Config => {
                Self::apply_config(device_id, &task.content).await
            }
            TaskType::FileTransfer => {
                Self::transfer_file(device_id, &task.content).await
            }
            TaskType::Upgrade => {
                Self::upgrade_device(device_id, &task.content).await
            }
            TaskType::Reboot => {
                Self::reboot_device(device_id, &task.content).await
            }
        };
        
        let completed_at = Utc::now();
        
        Ok(DeviceResult {
            device_id: device_id.to_string(),
            device_name: device_info.map(|d| d.name.clone()),
            status,
            started_at,
            completed_at: Some(completed_at),
            duration_ms: Some((completed_at - started_at).num_milliseconds() as u64),
            output,
            error,
            retry_count: 0,
        })
    }

    /// 更新进度
    fn update_progress(progress: &mut TaskProgress, result: &DeviceResult) {
        progress.pending = progress.pending.saturating_sub(1);
        
        match result.status {
            DeviceResultStatus::Success => {
                progress.completed += 1;
            }
            DeviceResultStatus::Failed | DeviceResultStatus::Timeout => {
                progress.failed += 1;
            }
            DeviceResultStatus::Cancelled => {
                progress.cancelled += 1;
            }
            DeviceResultStatus::Skipped => {
                progress.completed += 1;
            }
        }
    }

    // 以下是各任务类型的具体执行方法
    // 这些方法需要通过gRPC与远程设备通信
    
    async fn execute_command(device_id: &str, command: &str) -> (DeviceResultStatus, Option<String>, Option<String>) {
        // TODO: 通过gRPC调用远程设备执行命令
        (DeviceResultStatus::Success, Some("Command executed".to_string()), None)
    }

    async fn execute_script(device_id: &str, script: &str) -> (DeviceResultStatus, Option<String>, Option<String>) {
        // TODO: 通过gRPC调用远程设备执行脚本
        (DeviceResultStatus::Success, Some("Script executed".to_string()), None)
    }

    async fn apply_config(device_id: &str, config: &str) -> (DeviceResultStatus, Option<String>, Option<String>) {
        // TODO: 通过gRPC调用远程设备应用配置
        (DeviceResultStatus::Success, Some("Config applied".to_string()), None)
    }

    async fn transfer_file(device_id: &str, transfer_spec: &str) -> (DeviceResultStatus, Option<String>, Option<String>) {
        // TODO: 通过gRPC调用远程设备传输文件
        (DeviceResultStatus::Success, Some("File transferred".to_string()), None)
    }

    async fn upgrade_device(device_id: &str, upgrade_spec: &str) -> (DeviceResultStatus, Option<String>, Option<String>) {
        // TODO: 通过gRPC调用远程设备升级
        (DeviceResultStatus::Success, Some("Upgrade completed".to_string()), None)
    }

    async fn reboot_device(device_id: &str, reboot_spec: &str) -> (DeviceResultStatus, Option<String>, Option<String>) {
        // TODO: 通过gRPC调用远程设备重启
        (DeviceResultStatus::Success, Some("Reboot triggered".to_string()), None)
    }
}

/// 任务存储接口
#[async_trait::async_trait]
pub trait TaskStore: Send + Sync {
    async fn save_task(&self, task: &BatchTask) -> Result<()>;
    async fn get_task(&self, task_id: Uuid) -> Result<Option<BatchTask>>;
    async fn list_tasks(&self, filter: TaskFilter) -> Result<Vec<BatchTask>>;
    async fn delete_task(&self, task_id: Uuid) -> Result<()>;
    async fn save_result(&self, result: &DeviceResult) -> Result<()>;
    async fn get_task_results(&self, task_id: Uuid) -> Result<Vec<DeviceResult>>;
}

/// 任务过滤器
#[derive(Debug, Clone, Default)]
pub struct TaskFilter {
    pub status: Option<TaskStatus>,
    pub task_type: Option<TaskType>,
    pub created_by: Option<String>,
    pub from_date: Option<DateTime<Utc>>,
    pub to_date: Option<DateTime<Utc>>,
}
```

### 4.2 执行策略配置示例

```rust
// 执行策略使用示例

use crate::batch_task::ExecutionPolicy;

// 全量并发（默认）
let parallel = ExecutionPolicy::parallel();

// 分批执行，每批10台
let batched = ExecutionPolicy::batch(10);

// 滚动执行，从1台开始，每批增加5台
let rolling = ExecutionPolicy::rolling(1, 5);

// 严格模式（失败即停）
let strict = ExecutionPolicy::strict()
    .with_failure_threshold(0.0)
    .with_continue_on_failure(false);

// 带有重试的自定义策略
let custom = ExecutionPolicy {
    mode: ExecutionMode::Batch,
    batch_size: Some(20),
    continue_on_failure: true,
    failure_threshold_percent: 30.0,
    timeout_secs: 600,
    retry_count: 3,
    retry_interval_secs: 10,
    rolling_initial_batch: None,
    rolling_increment: None,
};
```

---

## 5. 后端API设计

### 5.1 gRPC服务定义

```protobuf
// batch_task.proto

syntax = "proto3";

package cc.batch;

import "google/protobuf/timestamp.proto";
import "google/protobuf/empty.proto";

// 批量任务服务
service BatchTaskService {
    // 任务管理
    rpc CreateTask(CreateTaskRequest) returns (BatchTask);
    rpc GetTask(GetTaskRequest) returns (BatchTask);
    rpc ListTasks(ListTasksRequest) returns (ListTasksResponse);
    rpc UpdateTask(UpdateTaskRequest) returns (BatchTask);
    rpc DeleteTask(DeleteTaskRequest) returns (google.protobuf.Empty);
    
    // 任务执行控制
    rpc StartTask(StartTaskRequest) returns (StartTaskResponse);
    rpc StopTask(StopTaskRequest) returns (google.protobuf.Empty);
    rpc PauseTask(PauseTaskRequest) returns (google.protobuf.Empty);
    rpc ResumeTask(ResumeTaskRequest) returns (StartTaskResponse);
    
    // 进度和结果查询
    rpc GetTaskProgress(GetTaskProgressRequest) returns (TaskProgress);
    rpc GetTaskResults(GetTaskResultsRequest) returns (TaskResultsResponse);
    rpc GetTaskLogs(GetTaskLogsRequest) returns (stream TaskLog);
    
    // 目标设备管理
    rpc RegisterDevice(RegisterDeviceRequest) returns (google.protobuf.Empty);
    rpc UnregisterDevice(UnregisterDeviceRequest) returns (google.protobuf.Empty);
    rpc ListDevices(ListDevicesRequest) returns (ListDevicesResponse);
    rpc GetDevice(GetDeviceRequest) returns (DeviceInfo);
    
    // 设备分组管理
    rpc CreateGroup(CreateGroupRequest) returns (DeviceGroup);
    rpc ListGroups(ListGroupsRequest) returns (ListGroupsResponse);
    rpc DeleteGroup(DeleteGroupRequest) returns (google.protobuf.Empty);
}

// 任务相关消息

message BatchTask {
    string id = 1;
    string name = 2;
    string description = 3;
    TaskType task_type = 4;
    TargetSelector target_selector = 5;
    string content = 6;
    ExecutionPolicy execution_policy = 7;
    string created_by = 8;
    google.protobuf.Timestamp created_at = 9;
    google.protobuf.Timestamp updated_at = 10;
    TaskStatus status = 11;
    TaskProgress progress = 12;
    uint32 priority = 13;
    google.protobuf.Timestamp scheduled_at = 14;
    TaskSummary summary = 15;
}

enum TaskType {
    TASK_TYPE_UNSPECIFIED = 0;
    TASK_TYPE_COMMAND = 1;
    TASK_TYPE_SCRIPT = 2;
    TASK_TYPE_CONFIG = 3;
    TASK_TYPE_FILE_TRANSFER = 4;
    TASK_TYPE_UPGRADE = 5;
    TASK_TYPE_REBOOT = 6;
}

enum TaskStatus {
    TASK_STATUS_UNSPECIFIED = 0;
    TASK_STATUS_PENDING = 1;
    TASK_STATUS_RUNNING = 2;
    TASK_STATUS_PAUSED = 3;
    TASK_STATUS_COMPLETED = 4;
    TASK_STATUS_FAILED = 5;
    TASK_STATUS_CANCELLED = 6;
}

message TargetSelector {
    SelectorType selector_type = 1;
    repeated string groups = 2;
    repeated string tags = 3;
    repeated string device_ids = 4;
    string filter_expr = 5;
}

enum SelectorType {
    SELECTOR_TYPE_UNSPECIFIED = 0;
    SELECTOR_TYPE_ALL = 1;
    SELECTOR_TYPE_GROUP = 2;
    SELECTOR_TYPE_TAG = 3;
    SELECTOR_TYPE_DEVICE_IDS = 4;
    SELECTOR_TYPE_FILTER = 5;
}

message ExecutionPolicy {
    ExecutionMode mode = 1;
    uint32 batch_size = 2;
    bool continue_on_failure = 3;
    float failure_threshold_percent = 4;
    uint32 timeout_secs = 5;
    uint32 retry_count = 6;
    uint32 retry_interval_secs = 7;
    uint32 rolling_initial_batch = 8;
    uint32 rolling_increment = 9;
}

enum ExecutionMode {
    EXECUTION_MODE_UNSPECIFIED = 0;
    EXECUTION_MODE_PARALLEL = 1;
    EXECUTION_MODE_BATCH = 2;
    EXECUTION_MODE_ROLLING = 3;
}

message TaskProgress {
    uint32 total = 1;
    uint32 completed = 2;
    uint32 failed = 3;
    uint32 running = 4;
    uint32 pending = 5;
    uint32 cancelled = 6;
    uint32 current_batch = 7;
    google.protobuf.Timestamp started_at = 8;
    google.protobuf.Timestamp completed_at = 9;
    uint64 estimated_remaining_secs = 10;
}

message TaskSummary {
    uint32 success_count = 1;
    uint32 failure_count = 2;
    uint64 total_duration_secs = 3;
    repeated DeviceResult results = 4;
}

message DeviceResult {
    string device_id = 1;
    string device_name = 2;
    DeviceResultStatus status = 3;
    google.protobuf.Timestamp started_at = 4;
    google.protobuf.Timestamp completed_at = 5;
    uint64 duration_ms = 6;
    string output = 7;
    string error = 8;
    uint32 retry_count = 9;
}

enum DeviceResultStatus {
    DEVICE_RESULT_STATUS_UNSPECIFIED = 0;
    DEVICE_RESULT_STATUS_SUCCESS = 1;
    DEVICE_RESULT_STATUS_FAILED = 2;
    DEVICE_RESULT_STATUS_TIMEOUT = 3;
    DEVICE_RESULT_STATUS_CANCELLED = 4;
    DEVICE_RESULT_STATUS_SKIPPED = 5;
}

// 设备相关消息

message DeviceInfo {
    string id = 1;
    string name = 2;
    repeated string groups = 3;
    map<string, string> tags = 4;
    map<string, string> metadata = 5;
    DeviceStatus status = 6;
    google.protobuf.Timestamp last_seen_at = 7;
}

enum DeviceStatus {
    DEVICE_STATUS_UNSPECIFIED = 0;
    DEVICE_STATUS_ONLINE = 1;
    DEVICE_STATUS_OFFLINE = 2;
    DEVICE_STATUS_UNKNOWN = 3;
}

message DeviceGroup {
    string id = 1;
    string name = 2;
    string description = 3;
    string parent_id = 4;
    google.protobuf.Timestamp created_at = 5;
    google.protobuf.Timestamp updated_at = 6;
}

message TaskLog {
    string task_id = 1;
    string device_id = 2;
    google.protobuf.Timestamp timestamp = 3;
    LogLevel level = 4;
    string message = 5;
}

enum LogLevel {
    LOG_LEVEL_UNSPECIFIED = 0;
    LOG_LEVEL_DEBUG = 1;
    LOG_LEVEL_INFO = 2;
    LOG_LEVEL_WARNING = 3;
    LOG_LEVEL_ERROR = 4;
}

// 请求和响应消息

message CreateTaskRequest {
    string name = 1;
    string description = 2;
    TaskType task_type = 3;
    TargetSelector target_selector = 4;
    string content = 5;
    ExecutionPolicy execution_policy = 6;
    string created_by = 7;
    uint32 priority = 8;
    google.protobuf.Timestamp scheduled_at = 9;
}

message GetTaskRequest {
    string task_id = 1;
}

message ListTasksRequest {
    TaskStatus status = 1;
    TaskType task_type = 2;
    string created_by = 3;
    google.protobuf.Timestamp from_date = 4;
    google.protobuf.Timestamp to_date = 5;
    uint32 offset = 6;
    uint32 limit = 7;
}

message ListTasksResponse {
    repeated BatchTask tasks = 1;
    uint32 total = 2;
}

message UpdateTaskRequest {
    string task_id = 1;
    string name = 2;
    string description = 3;
    ExecutionPolicy execution_policy = 4;
    uint32 priority = 5;
}

message DeleteTaskRequest {
    string task_id = 1;
}

message StartTaskRequest {
    string task_id = 1;
}

message StartTaskResponse {
    string execution_id = 1;
    uint32 target_count = 2;
}

message StopTaskRequest {
    string task_id = 1;
}

message PauseTaskRequest {
    string task_id = 1;
}

message ResumeTaskRequest {
    string task_id = 1;
}

message GetTaskProgressRequest {
    string task_id = 1;
}

message GetTaskResultsRequest {
    string task_id = 1;
    bool include_output = 2;
    uint32 offset = 3;
    uint32 limit = 4;
}

message TaskResultsResponse {
    repeated DeviceResult results = 1;
    uint32 total = 2;
}

message GetTaskLogsRequest {
    string task_id = 1;
    string device_id = 2;
    LogLevel min_level = 3;
    uint32 offset = 4;
    uint32 limit = 5;
}

message RegisterDeviceRequest {
    string id = 1;
    string name = 2;
    repeated string groups = 3;
    map<string, string> tags = 4;
    map<string, string> metadata = 5;
}

message UnregisterDeviceRequest {
    string device_id = 1;
}

message ListDevicesRequest {
    repeated string groups = 1;
    repeated string tags = 2;
    DeviceStatus status = 3;
    uint32 offset = 4;
    uint32 limit = 5;
}

message ListDevicesResponse {
    repeated DeviceInfo devices = 1;
    uint32 total = 2;
}

message GetDeviceRequest {
    string device_id = 1;
}

message CreateGroupRequest {
    string name = 1;
    string description = 2;
    string parent_id = 3;
}

message ListGroupsRequest {
    string parent_id = 1;
}

message ListGroupsResponse {
    repeated DeviceGroup groups = 1;
}

message DeleteGroupRequest {
    string group_id = 1;
}
```

### 5.2 Rust实现

```rust
// batch_task_service.rs - gRPC服务实现

use crate::batch_task::{
    BatchTask, BatchTaskFilter, DeviceInfo, DeviceRegistry, ExecutionPolicy,
    TaskProgress, TaskStatus, TaskSummary, TaskType, TargetSelector,
};
use crate::batch_executor::{BatchExecutor, TaskStore};
use crate::grpc::batch::{
    batch_task_service_server::{BatchTaskService, *},
    *,
};
use anyhow::Result;
use chrono::{DateTime, Utc};
use std::sync::Arc;
use tokio::sync::RwLock;
use tokio_stream::wrappers::ReceiverStream;
use tonic::{Request, Response, Status, Streaming};
use tracing::{debug, error, info};
use uuid::Uuid;

/// Batch任务gRPC服务实现
pub struct BatchTaskServiceImpl {
    executor: Arc<BatchExecutor>,
    device_registry: Arc<DeviceRegistry>,
}

impl BatchTaskServiceImpl {
    pub fn new(executor: Arc<BatchExecutor>, device_registry: Arc<DeviceRegistry>) -> Self {
        Self {
            executor,
            device_registry,
        }
    }
}

#[tonic::async_trait]
impl BatchTaskService for BatchTaskServiceImpl {
    async fn create_task(
        &self,
        request: Request<CreateTaskRequest>,
    ) -> Result<Response<BatchTask>, Status> {
        let req = request.into_inner();
        
        let target_selector = TargetSelector {
            selector_type: proto_to_selector_type(req.target_selector.as_ref()),
            groups: req.target_selector.groups,
            tags: req.target_selector.tags,
            device_ids: req.target_selector.device_ids,
            filter_expr: req.target_selector.filter_expr,
        };
        
        let execution_policy = proto_to_execution_policy(req.execution_policy.as_ref());
        
        let mut task = BatchTask::new(
            req.name,
            proto_to_task_type(req.task_type),
            target_selector,
            req.content,
        );
        
        if let Some(desc) = req.description {
            task.description = Some(desc);
        }
        
        if let Some(by) = req.created_by {
            task.created_by = Some(by);
        }
        
        task.priority = req.priority;
        
        if let Some(scheduled) = req.scheduled_at {
            task.scheduled_at = Some(protobuf_timestamp_to_chrono(scheduled));
        }
        
        task.execution_policy = execution_policy;
        
        // 保存任务
        self.executor.task_store().save_task(&task).await
            .map_err(|e| Status::internal(e.to_string()))?;
        
        Ok(Response::new(task_to_proto(task)))
    }

    async fn get_task(
        &self,
        request: Request<GetTaskRequest>,
    ) -> Result<Response<BatchTask>, Status> {
        let task_id = Uuid::parse_str(&request.into_inner().task_id)
            .map_err(|_| Status::invalid_argument("invalid task_id"))?;
        
        let task = self.executor.task_store().get_task(task_id).await
            .map_err(|e| Status::internal(e.to_string()))?
            .ok_or_else(|| Status::not_found("task not found"))?;
        
        Ok(Response::new(task_to_proto(task)))
    }

    async fn list_tasks(
        &self,
        request: Request<ListTasksRequest>,
    ) -> Result<Response<ListTasksResponse>, Status> {
        let req = request.into_inner();
        
        let filter = BatchTaskFilter {
            status: req.status.map(proto_to_task_status),
            task_type: req.task_type.map(proto_to_task_type),
            created_by: req.created_by,
            from_date: req.from_date.map(protobuf_timestamp_to_chrono),
            to_date: req.to_date.map(protobuf_timestamp_to_chrono),
            offset: req.offset,
            limit: req.limit,
        };
        
        let tasks = self.executor.task_store().list_tasks(filter).await
            .map_err(|e| Status::internal(e.to_string()))?;
        
        let total = tasks.len() as u32;
        let tasks: Vec<BatchTask> = tasks;
        
        Ok(Response::new(ListTasksResponse {
            tasks: tasks.into_iter().map(task_to_proto).collect(),
            total,
        }))
    }

    async fn start_task(
        &self,
        request: Request<StartTaskRequest>,
    ) -> Result<Response<StartTaskResponse>, Status> {
        let task_id = Uuid::parse_str(&request.into_inner().task_id)
            .map_err(|_| Status::invalid_argument("invalid task_id"))?;
        
        let task = self.executor.task_store().get_task(task_id).await
            .map_err(|e| Status::internal(e.to_string()))?
            .ok_or_else(|| Status::not_found("task not found"))?;
        
        let execution_id = self.executor.start_task(task).await
            .map_err(|e| Status::internal(e.to_string()))?;
        
        let targets = self.executor.device_registry()
            .select_devices(&task.target_selector)
            .map_err(|e| Status::internal(e.to_string()))?;
        
        Ok(Response::new(StartTaskResponse {
            execution_id: execution_id.to_string(),
            target_count: targets.len() as u32,
        }))
    }

    async fn stop_task(
        &self,
        request: Request<StopTaskRequest>,
    ) -> Result<Response<()>, Status> {
        let task_id = Uuid::parse_str(&request.into_inner().task_id)
            .map_err(|_| Status::invalid_argument("invalid task_id"))?;
        
        self.executor.stop_task(task_id).await
            .map_err(|e| Status::internal(e.to_string()))?;
        
        Ok(Response::new(()))
    }

    async fn get_task_progress(
        &self,
        request: Request<GetTaskProgressRequest>,
    ) -> Result<Response<TaskProgress>, Status> {
        let task_id = Uuid::parse_str(&request.into_inner().task_id)
            .map_err(|_| Status::invalid_argument("invalid task_id"))?;
        
        let progress = self.executor.get_progress(task_id).await
            .map_err(|e| Status::internal(e.to_string()))?;
        
        Ok(Response::new(progress_to_proto(progress)))
    }

    async fn get_task_results(
        &self,
        request: Request<GetTaskResultsRequest>,
    ) -> Result<Response<TaskResultsResponse>, Status> {
        let req = request.into_inner();
        let task_id = Uuid::parse_str(&req.task_id)
            .map_err(|_| Status::invalid_argument("invalid task_id"))?;
        
        let results = self.executor.get_results(task_id).await
            .map_err(|e| Status::internal(e.to_string()))?;
        
        let total = results.len() as u32;
        
        Ok(Response::new(TaskResultsResponse {
            results: results.into_iter().map(result_to_proto).collect(),
            total,
        }))
    }

    type GetTaskLogsStream = ReceiverStream<Result<TaskLog, Status>>;

    async fn get_task_logs(
        &self,
        request: Request<GetTaskLogsRequest>,
    ) -> Result<Response<Self::GetTaskLogsStream>, Status> {
        let req = request.into_inner();
        let task_id = Uuid::parse_str(&req.task_id)
            .map_err(|_| Status::invalid_argument("invalid task_id"))?;
        
        // 实现日志流
        // TODO: 实现完整的日志流
        let (_tx, rx) = tokio::sync::mpsc::channel(100);
        
        Ok(Response::new(ReceiverStream::new(rx)))
    }

    async fn register_device(
        &self,
        request: Request<RegisterDeviceRequest>,
    ) -> Result<Response<()>, Status> {
        let req = request.into_inner();
        
        let device = DeviceInfo {
            id: req.id,
            name: req.name,
            groups: req.groups,
            tags: req.tags,
            metadata: req.metadata,
            status: DeviceStatus::Online,
            last_seen_at: Utc::now(),
        };
        
        self.device_registry.register_device(device);
        
        Ok(Response::new(()))
    }

    async fn list_devices(
        &self,
        request: Request<ListDevicesRequest>,
    ) -> Result<Response<ListDevicesResponse>, Status> {
        let req = request.into_inner();
        
        // 根据请求筛选设备
        // TODO: 实现完整的设备列表
        let devices = self.device_registry.list_all();
        
        Ok(Response::new(ListDevicesResponse {
            devices: devices.into_iter().map(device_to_proto).collect(),
            total: devices.len() as u32,
        }))
    }
}

// 辅助转换函数

fn proto_to_task_type(t: i32) -> TaskType {
    match t {
        1 => TaskType::Command,
        2 => TaskType::Script,
        3 => TaskType::Config,
        4 => TaskType::FileTransfer,
        5 => TaskType::Upgrade,
        6 => TaskType::Reboot,
        _ => TaskType::Command,
    }
}

fn task_type_to_proto(t: TaskType) -> i32 {
    match t {
        TaskType::Command => 1,
        TaskType::Script => 2,
        TaskType::Config => 3,
        TaskType::FileTransfer => 4,
        TaskType::Upgrade => 5,
        TaskType::Reboot => 6,
    }
}

fn proto_to_task_status(s: i32) -> TaskStatus {
    match s {
        1 => TaskStatus::Pending,
        2 => TaskStatus::Running,
        3 => TaskStatus::Paused,
        4 => TaskStatus::Completed,
        5 => TaskStatus::Failed,
        6 => TaskStatus::Cancelled,
        _ => TaskStatus::Pending,
    }
}

fn task_status_to_proto(s: TaskStatus) -> i32 {
    match s {
        TaskStatus::Pending => 1,
        TaskStatus::Running => 2,
        TaskStatus::Paused => 3,
        TaskStatus::Completed => 4,
        TaskStatus::Failed => 5,
        TaskStatus::Cancelled => 6,
    }
}

// ... 更多转换函数
```

---

## 6. 前端批量操作UI设计方案

### 6.1 组件结构

```
CC-rClient/src/plugin/batch/
├── components/
│   ├── BatchTaskList.tsx       # 批量任务列表
│   ├── BatchTaskCard.tsx        # 任务卡片
│   ├── CreateTaskWizard.tsx     # 创建任务向导
│   ├── TaskTargetSelector.tsx   # 目标选择器
│   ├── ExecutionPolicyConfig.tsx # 执行策略配置
│   ├── TaskProgressPanel.tsx    # 进度面板
│   ├── TaskResultTable.tsx      # 结果表格
│   ├── DeviceSelector.tsx       # 设备选择器
│   ├── FilterExpressionEditor.tsx # 过滤表达式编辑器
│   └── common/
│       ├── StatusBadge.tsx      # 状态徽章
│       ├── ProgressBar.tsx       # 进度条
│       └── ExecuteButton.tsx     # 执行按钮
├── context/
│   └── BatchTaskContext.tsx     # 批量任务上下文
├── hooks/
│   ├── useBatchTasks.ts         # 批量任务Hook
│   ├── useTaskExecution.ts      # 任务执行Hook
│   └── useDeviceSelection.ts    # 设备选择Hook
├── api/
│   └── batchApi.ts              # 后端API调用
├── types/
│   └── index.ts                 # 类型定义
└── index.ts                     # 导出
```

### 6.2 核心组件设计

#### 6.2.1 BatchTaskList.tsx

```tsx
/**
 * BatchTaskList - 批量任务列表组件
 * 显示所有批量任务，支持筛选、排序、分页
 */

import React, { useState, useEffect } from 'react';
import { batchApi } from '../api/batchApi';
import { BatchTask, TaskStatus, TaskFilter } from '../types';
import BatchTaskCard from './BatchTaskCard';
import CreateTaskWizard from './CreateTaskWizard';
import { Button, Input, Select, Pagination } from '../common';

interface BatchTaskListProps {
  onTaskSelect?: (task: BatchTask) => void;
}

const BatchTaskList: React.FC<BatchTaskListProps> = ({ onTaskSelect }) => {
  const [tasks, setTasks] = useState<BatchTask[]>([]);
  const [loading, setLoading] = useState(true);
  const [filter, setFilter] = useState<TaskFilter>({
    status: undefined,
    taskType: undefined,
    offset: 0,
    limit: 20,
  });
  const [total, setTotal] = useState(0);
  const [showCreateWizard, setShowCreateWizard] = useState(false);

  useEffect(() => {
    loadTasks();
  }, [filter]);

  const loadTasks = async () => {
    setLoading(true);
    try {
      const result = await batchApi.listTasks(filter);
      setTasks(result.tasks);
      setTotal(result.total);
    } catch (error) {
      console.error('[BatchTaskList] Failed to load tasks:', error);
    } finally {
      setLoading(false);
    }
  };

  const handleTaskAction = async (taskId: string, action: 'start' | 'stop' | 'pause' | 'resume' | 'delete') => {
    try {
      switch (action) {
        case 'start':
          await batchApi.startTask(taskId);
          break;
        case 'stop':
          await batchApi.stopTask(taskId);
          break;
        case 'pause':
          await batchApi.pauseTask(taskId);
          break;
        case 'resume':
          await batchApi.resumeTask(taskId);
          break;
        case 'delete':
          await batchApi.deleteTask(taskId);
          break;
      }
      loadTasks();
    } catch (error) {
      console.error(`[BatchTaskList] Failed to ${action} task:`, error);
    }
  };

  return (
    <div className="batch-task-list">
      <div className="batch-task-list__header">
        <h2>批量任务</h2>
        <Button variant="primary" onClick={() => setShowCreateWizard(true)}>
          创建任务
        </Button>
      </div>

      <div className="batch-task-list__filters">
        <Select
          value={filter.status ?? ''}
          onChange={(e) => setFilter({ ...filter, status: e.target.value as TaskStatus || undefined })}
        >
          <option value="">全部状态</option>
          <option value="pending">待执行</option>
          <option value="running">执行中</option>
          <option value="paused">已暂停</option>
          <option value="completed">已完成</option>
          <option value="failed">失败</option>
          <option value="cancelled">已取消</option>
        </Select>

        <Select
          value={filter.taskType ?? ''}
          onChange={(e) => setFilter({ ...filter, taskType: e.target.value as any || undefined })}
        >
          <option value="">全部类型</option>
          <option value="command">命令执行</option>
          <option value="script">脚本执行</option>
          <option value="config">配置下发</option>
          <option value="file_transfer">文件传输</option>
          <option value="upgrade">软件升级</option>
          <option value="reboot">设备重启</option>
        </Select>

        <Input
          placeholder="搜索任务名称..."
          value={filter.search ?? ''}
          onChange={(e) => setFilter({ ...filter, search: e.target.value })}
        />
      </div>

      {loading ? (
        <div className="batch-task-list__loading">加载中...</div>
      ) : tasks.length === 0 ? (
        <div className="batch-task-list__empty">
          暂无任务，点击"创建任务"开始
        </div>
      ) : (
        <div className="batch-task-list__content">
          {tasks.map((task) => (
            <BatchTaskCard
              key={task.id}
              task={task}
              onAction={(action) => handleTaskAction(task.id, action)}
              onClick={() => onTaskSelect?.(task)}
            />
          ))}
        </div>
      )}

      <Pagination
        total={total}
        pageSize={filter.limit ?? 20}
        current={Math.floor((filter.offset ?? 0) / (filter.limit ?? 20)) + 1}
        onChange={(page) => setFilter({ ...filter, offset: (page - 1) * (filter.limit ?? 20) })}
      />

      {showCreateWizard && (
        <CreateTaskWizard
          onClose={() => setShowCreateWizard(false)}
          onCreated={(task) => {
            setShowCreateWizard(false);
            loadTasks();
            onTaskSelect?.(task);
          }}
        />
      )}
    </div>
  );
};

export default BatchTaskList;
```

#### 6.2.2 CreateTaskWizard.tsx

```tsx
/**
 * CreateTaskWizard - 创建批量任务向导
 * 引导用户完成：选择类型 → 选择目标 → 配置策略 → 填写内容
 */

import React, { useState } from 'react';
import { batchApi, deviceApi } from '../api';
import { BatchTask, TaskType, TargetSelector, ExecutionPolicy, DeviceInfo } from '../types';
import { Button, Stepper, Input, Select, Radio } from '../common';
import TaskTargetSelector from './TaskTargetSelector';
import ExecutionPolicyConfig from './ExecutionPolicyConfig';

interface CreateTaskWizardProps {
  onClose: () => void;
  onCreated: (task: BatchTask) => void;
}

type Step = 'type' | 'target' | 'policy' | 'content' | 'review';

const CreateTaskWizard: React.FC<CreateTaskWizardProps> = ({ onClose, onCreated }) => {
  const [currentStep, setCurrentStep] = useState<Step>('type');
  const [taskName, setTaskName] = useState('');
  const [taskDescription, setTaskDescription] = useState('');
  const [taskType, setTaskType] = useState<TaskType>('command');
  const [targetSelector, setTargetSelector] = useState<TargetSelector>({ selectorType: 'all' });
  const [executionPolicy, setExecutionPolicy] = useState<ExecutionPolicy>({
    mode: 'parallel',
    continueOnFailure: true,
    failureThresholdPercent: 50,
    timeoutSecs: 300,
    retryCount: 0,
  });
  const [content, setContent] = useState('');
  const [devices, setDevices] = useState<DeviceInfo[]>([]);
  const [creating, setCreating] = useState(false);

  const steps: Step[] = ['type', 'target', 'policy', 'content', 'review'];
  const currentStepIndex = steps.indexOf(currentStep);

  const loadDevices = async () => {
    try {
      const result = await deviceApi.listDevices({});
      setDevices(result.devices);
    } catch (error) {
      console.error('[CreateTaskWizard] Failed to load devices:', error);
    }
  };

  useEffect(() => {
    if (currentStep === 'target') {
      loadDevices();
    }
  }, [currentStep]);

  const handleNext = () => {
    const idx = currentStepIndex;
    if (idx < steps.length - 1) {
      setCurrentStep(steps[idx + 1]);
    }
  };

  const handleBack = () => {
    const idx = currentStepIndex;
    if (idx > 0) {
      setCurrentStep(steps[idx - 1]);
    }
  };

  const handleCreate = async () => {
    setCreating(true);
    try {
      const task = await batchApi.createTask({
        name: taskName,
        description: taskDescription,
        taskType,
        targetSelector,
        executionPolicy,
        content,
      });
      onCreated(task);
    } catch (error) {
      console.error('[CreateTaskWizard] Failed to create task:', error);
    } finally {
      setCreating(false);
    }
  };

  const canProceed = () => {
    switch (currentStep) {
      case 'type':
        return taskName.trim() !== '' && taskType !== undefined;
      case 'target':
        return targetSelector.selectorType !== undefined;
      case 'policy':
        return true;
      case 'content':
        return content.trim() !== '';
      case 'review':
        return true;
      default:
        return false;
    }
  };

  return (
    <div className="create-task-wizard">
      <div className="create-task-wizard__header">
        <h2>创建批量任务</h2>
        <Button variant="ghost" onClick={onClose}>×</Button>
      </div>

      <Stepper
        steps={[
          { id: 'type', label: '任务类型' },
          { id: 'target', label: '选择目标' },
          { id: 'policy', label: '执行策略' },
          { id: 'content', label: '任务内容' },
          { id: 'review', label: '确认创建' },
        ]}
        currentStep={currentStepIndex}
      />

      <div className="create-task-wizard__content">
        {currentStep === 'type' && (
          <div className="step-type">
            <div className="form-group">
              <label>任务名称</label>
              <Input
                value={taskName}
                onChange={(e) => setTaskName(e.target.value)}
                placeholder="输入任务名称"
              />
            </div>

            <div className="form-group">
              <label>任务描述 (可选)</label>
              <Input
                value={taskDescription}
                onChange={(e) => setTaskDescription(e.target.value)}
                placeholder="输入任务描述"
              />
            </div>

            <div className="form-group">
              <label>任务类型</label>
              <div className="task-type-grid">
                {[
                  { value: 'command', label: '命令执行', icon: '⌨️', desc: '在目标设备上执行命令' },
                  { value: 'script', label: '脚本执行', icon: '📜', desc: '执行脚本文件' },
                  { value: 'config', label: '配置下发', icon: '⚙️', desc: '下发配置文件' },
                  { value: 'file_transfer', label: '文件传输', icon: '📁', desc: '传输文件到目标' },
                  { value: 'upgrade', label: '软件升级', icon: '⬆️', desc: '升级设备软件' },
                  { value: 'reboot', label: '设备重启', icon: '🔄', desc: '重启目标设备' },
                ].map((type) => (
                  <div
                    key={type.value}
                    className={`task-type-card ${taskType === type.value ? 'selected' : ''}`}
                    onClick={() => setTaskType(type.value as TaskType)}
                  >
                    <span className="task-type-card__icon">{type.icon}</span>
                    <span className="task-type-card__label">{type.label}</span>
                    <span className="task-type-card__desc">{type.desc}</span>
                  </div>
                ))}
              </div>
            </div>
          </div>
        )}

        {currentStep === 'target' && (
          <div className="step-target">
            <TaskTargetSelector
              devices={devices}
              value={targetSelector}
              onChange={setTargetSelector}
            />
          </div>
        )}

        {currentStep === 'policy' && (
          <div className="step-policy">
            <ExecutionPolicyConfig
              value={executionPolicy}
              onChange={setExecutionPolicy}
              targetCount={
                targetSelector.selectorType === 'all' ? devices.length :
                targetSelector.selectorType === 'device_ids' ? targetSelector.deviceIds.length :
                devices.filter(d => d.groups.some(g => targetSelector.groups.includes(g))).length
              }
            />
          </div>
        )}

        {currentStep === 'content' && (
          <div className="step-content">
            <div className="form-group">
              <label>任务内容</label>
              {taskType === 'command' && (
                <Input
                  value={content}
                  onChange={(e) => setContent(e.target.value)}
                  placeholder="输入要执行的命令"
                  multiline
                  rows={5}
                />
              )}
              {taskType === 'script' && (
                <div className="script-editor">
                  <Select
                    value=""
                    onChange={(e) => setContent(e.target.value)}
                  >
                    <option value="">选择已有脚本...</option>
                  </Select>
                  <Input
                    value={content}
                    onChange={(e) => setContent(e.target.value)}
                    placeholder="或输入脚本内容"
                    multiline
                    rows={10}
                  />
                </div>
              )}
              {taskType === 'config' && (
                <Input
                  value={content}
                  onChange={(e) => setContent(e.target.value)}
                  placeholder="输入配置内容 (JSON格式)"
                  multiline
                  rows={10}
                />
              )}
              {taskType === 'file_transfer' && (
                <div className="file-transfer-config">
                  <Input
                    value={content}
                    onChange={(e) => setContent(e.target.value)}
                    placeholder="输入文件路径或URL"
                  />
                </div>
              )}
              {taskType === 'upgrade' && (
                <div className="upgrade-config">
                  <Input
                    value={content}
                    onChange={(e) => setContent(e.target.value)}
                    placeholder="输入升级包路径或URL"
                  />
                </div>
              )}
              {taskType === 'reboot' && (
                <div className="reboot-config">
                  <p>确认要重启选中的设备吗？</p>
                  <label>
                    <input
                      type="checkbox"
                      checked={content === 'force'}
                      onChange={(e) => setContent(e.target.checked ? 'force' : '')}
                    />
                    强制重启 (忽略未保存的工作)
                  </label>
                </div>
              )}
            </div>
          </div>
        )}

        {currentStep === 'review' && (
          <div className="step-review">
            <div className="review-section">
              <h3>任务信息</h3>
              <dl>
                <dt>名称</dt>
                <dd>{taskName}</dd>
                <dt>描述</dt>
                <dd>{taskDescription || '-'}</dd>
                <dt>类型</dt>
                <dd>{taskType}</dd>
              </dl>
            </div>

            <div className="review-section">
              <h3>执行目标</h3>
              <dl>
                <dt>选择方式</dt>
                <dd>{targetSelector.selectorType}</dd>
                <dt>目标数量</dt>
                <dd>
                  {targetSelector.selectorType === 'all' ? '全部设备' :
                   targetSelector.selectorType === 'device_ids' ? `${targetSelector.deviceIds.length} 台设备` :
                   targetSelector.selectorType === 'group' ? `分组: ${targetSelector.groups.join(', ')}` :
                   targetSelector.selectorType === 'tag' ? `标签: ${targetSelector.tags.join(', ')}` :
                   targetSelector.filterExpr || '未知'}
                </dd>
              </dl>
            </div>

            <div className="review-section">
              <h3>执行策略</h3>
              <dl>
                <dt>执行模式</dt>
                <dd>{executionPolicy.mode}</dd>
                {executionPolicy.mode === 'batch' && (
                  <>
                    <dt>批次大小</dt>
                    <dd>{executionPolicy.batchSize}</dd>
                  </>
                )}
                {executionPolicy.mode === 'rolling' && (
                  <>
                    <dt>初始数量</dt>
                    <dd>{executionPolicy.rollingInitialBatch}</dd>
                    <dt>增量</dt>
                    <dd>{executionPolicy.rollingIncrement}</dd>
                  </>
                )}
                <dt>失败继续</dt>
                <dd>{executionPolicy.continueOnFailure ? '是' : '否'}</dd>
                <dt>失败阈值</dt>
                <dd>{executionPolicy.failureThresholdPercent}%</dd>
                <dt>超时时间</dt>
                <dd>{executionPolicy.timeoutSecs}秒</dd>
                <dt>重试次数</dt>
                <dd>{executionPolicy.retryCount}</dd>
              </dl>
            </div>
          </div>
        )}
      </div>

      <div className="create-task-wizard__footer">
        <Button
          variant="secondary"
          onClick={handleBack}
          disabled={currentStepIndex === 0}
        >
          上一步
        </Button>
        
        {currentStepIndex < steps.length - 1 ? (
          <Button
            variant="primary"
            onClick={handleNext}
            disabled={!canProceed()}
          >
            下一步
          </Button>
        ) : (
          <Button
            variant="primary"
            onClick={handleCreate}
            disabled={!canProceed() || creating}
          >
            {creating ? '创建中...' : '创建任务'}
          </Button>
        )}
      </div>
    </div>
  );
};

export default CreateTaskWizard;
```

#### 6.2.3 TaskTargetSelector.tsx

```tsx
/**
 * TaskTargetSelector - 目标设备选择器
 * 支持多种选择模式：全部、分组、标签、设备ID、过滤表达式
 */

import React, { useState, useEffect } from 'react';
import { DeviceInfo, TargetSelector, SelectorType } from '../types';
import { Input, Select, Checkbox, Radio, Button } from '../common';
import FilterExpressionEditor from './FilterExpressionEditor';

interface TaskTargetSelectorProps {
  devices: DeviceInfo[];
  value: TargetSelector;
  onChange: (selector: TargetSelector) => void;
}

const TaskTargetSelector: React.FC<TaskTargetSelectorProps> = ({
  devices,
  value,
  onChange,
}) => {
  const [availableGroups, setAvailableGroups] = useState<string[]>([]);
  const [availableTags, setAvailableTags] = useState<Record<string, string[]>>({});
  const [previewDevices, setPreviewDevices] = useState<DeviceInfo[]>([]);

  // 提取可用的分组和标签
  useEffect(() => {
    const groups = new Set<string>();
    const tags: Record<string, Set<string>> = {};

    devices.forEach((device) => {
      device.groups.forEach((g) => groups.add(g));
      Object.entries(device.tags).forEach(([key, val]) => {
        if (!tags[key]) tags[key] = new Set();
        tags[key].add(val);
      });
    });

    setAvailableGroups(Array.from(groups).sort());
    setAvailableTags(
      Object.fromEntries(
        Object.entries(tags).map(([k, v]) => [k, Array.from(v).sort()])
      )
    );
  }, [devices]);

  // 预览选中的设备
  useEffect(() => {
    const selected = getSelectedDevices();
    setPreviewDevices(selected);
  }, [value, devices]);

  const getSelectedDevices = (): DeviceInfo[] => {
    switch (value.selectorType) {
      case SelectorType.All:
        return devices;
      case SelectorType.DeviceIds:
        return devices.filter((d) => value.deviceIds.includes(d.id));
      case SelectorType.Group:
        return devices.filter((d) =>
          d.groups.some((g) => value.groups.includes(g))
        );
      case SelectorType.Tag:
        return devices.filter((d) =>
          Object.entries(d.tags).some(([k, v]) =>
            value.tags.some((t) => {
              if (t.includes(':')) {
                const [key, val] = t.split(':');
                return key === k && v === val;
              }
              return k === t;
            })
          )
        );
      case SelectorType.Filter:
        // TODO: 实现过滤表达式
        return devices;
      default:
        return [];
    }
  };

  const handleSelectorTypeChange = (type: SelectorType) => {
    onChange({
      selectorType: type,
      groups: [],
      tags: [],
      deviceIds: [],
      filterExpr: undefined,
    });
  };

  const handleGroupToggle = (group: string) => {
    const groups = value.groups.includes(group)
      ? value.groups.filter((g) => g !== group)
      : [...value.groups, group];
    onChange({ ...value, groups });
  };

  const handleTagToggle = (tag: string) => {
    const tags = value.tags.includes(tag)
      ? value.tags.filter((t) => t !== tag)
      : [...value.tags, tag];
    onChange({ ...value, tags });
  };

  const handleDeviceToggle = (deviceId: string) => {
    const deviceIds = value.deviceIds.includes(deviceId)
      ? value.deviceIds.filter((id) => id !== deviceId)
      : [...value.deviceIds, deviceId];
    onChange({ ...value, deviceIds });
  };

  const handleSelectAll = () => {
    onChange({ ...value, deviceIds: devices.map((d) => d.id) });
  };

  const handleSelectNone = () => {
    onChange({ ...value, deviceIds: [] });
  };

  return (
    <div className="task-target-selector">
      <div className="selector-type-tabs">
        {[
          { value: SelectorType.All, label: '全部设备' },
          { value: SelectorType.Group, label: '按分组' },
          { value: SelectorType.Tag, label: '按标签' },
          { value: SelectorType.DeviceIds, label: '选择设备' },
          { value: SelectorType.Filter, label: '过滤表达式' },
        ].map((tab) => (
          <button
            key={tab.value}
            className={`selector-type-tab ${value.selectorType === tab.value ? 'active' : ''}`}
            onClick={() => handleSelectorTypeChange(tab.value)}
          >
            {tab.label}
          </button>
        ))}
      </div>

      <div className="selector-content">
        {value.selectorType === SelectorType.All && (
          <div className="selector-all">
            <p>将执行于 <strong>{devices.length}</strong> 台设备</p>
          </div>
        )}

        {value.selectorType === SelectorType.Group && (
          <div className="selector-groups">
            <p>选择设备分组：</p>
            <div className="group-list">
              {availableGroups.map((group) => (
                <Checkbox
                  key={group}
                  label={`${group} (${devices.filter((d) => d.groups.includes(group)).length})`}
                  checked={value.groups.includes(group)}
                  onChange={() => handleGroupToggle(group)}
                />
              ))}
            </div>
          </div>
        )}

        {value.selectorType === SelectorType.Tag && (
          <div className="selector-tags">
            <p>选择设备标签：</p>
            <div className="tag-list">
              {Object.entries(availableTags).map(([key, values]) => (
                <div key={key} className="tag-group">
                  <span className="tag-key">{key}:</span>
                  <div className="tag-values">
                    {values.map((val) => (
                      <Checkbox
                        key={`${key}:${val}`}
                        label={val}
                        checked={value.tags.includes(`${key}:${val}`)}
                        onChange={() => handleTagToggle(`${key}:${val}`)}
                      />
                    ))}
                  </div>
                </div>
              ))}
            </div>
          </div>
        )}

        {value.selectorType === SelectorType.DeviceIds && (
          <div className="selector-devices">
            <div className="device-actions">
              <Button size="small" onClick={handleSelectAll}>全选</Button>
              <Button size="small" onClick={handleSelectNone}>取消全选</Button>
              <span className="selected-count">
                已选择: {value.deviceIds.length} / {devices.length}
              </span>
            </div>
            <div className="device-list">
              {devices.map((device) => (
                <Checkbox
                  key={device.id}
                  label={`${device.name} (${device.id})`}
                  checked={value.deviceIds.includes(device.id)}
                  onChange={() => handleDeviceToggle(device.id)}
                />
              ))}
            </div>
          </div>
        )}

        {value.selectorType === SelectorType.Filter && (
          <div className="selector-filter">
            <FilterExpressionEditor
              value={value.filterExpr || ''}
              onChange={(expr) => onChange({ ...value, filterExpr: expr })}
              devices={devices}
            />
          </div>
        )}
      </div>

      <div className="selector-preview">
        <h4>预览 ({previewDevices.length} 台设备)</h4>
        <div className="preview-list">
          {previewDevices.slice(0, 10).map((device) => (
            <div key={device.id} className="preview-item">
              <span className="device-name">{device.name}</span>
              <span className="device-id">{device.id}</span>
              <span className={`device-status status-${device.status?.toLowerCase()}`}>
                {device.status}
              </span>
            </div>
          ))}
          {previewDevices.length > 10 && (
            <div className="preview-more">
              还有 {previewDevices.length - 10} 台设备...
            </div>
          )}
        </div>
      </div>
    </div>
  );
};

export default TaskTargetSelector;
```

#### 6.2.4 ExecutionPolicyConfig.tsx

```tsx
/**
 * ExecutionPolicyConfig - 执行策略配置组件
 * 配置任务的执行模式、批次大小、失败处理等
 */

import React from 'react';
import { ExecutionPolicy, ExecutionMode } from '../types';
import { Input, Select, Slider, Radio } from '../common';

interface ExecutionPolicyConfigProps {
  value: ExecutionPolicy;
  onChange: (policy: ExecutionPolicy) => void;
  targetCount: number;
}

const ExecutionPolicyConfig: React.FC<ExecutionPolicyConfigProps> = ({
  value,
  onChange,
  targetCount,
}) => {
  const updatePolicy = (updates: Partial<ExecutionPolicy>) => {
    onChange({ ...value, ...updates });
  };

  return (
    <div className="execution-policy-config">
      <div className="config-section">
        <h4>执行模式</h4>
        <div className="mode-cards">
          <div
            className={`mode-card ${value.mode === 'parallel' ? 'selected' : ''}`}
            onClick={() => updatePolicy({ mode: 'parallel' })}
          >
            <div className="mode-card__icon">⚡</div>
            <div className="mode-card__title">全量并发</div>
            <div className="mode-card__desc">
              同时对所有 {targetCount} 台设备执行
            </div>
          </div>

          <div
            className={`mode-card ${value.mode === 'batch' ? 'selected' : ''}`}
            onClick={() => updatePolicy({ mode: 'batch' })}
          >
            <div className="mode-card__icon">📦</div>
            <div className="mode-card__title">分批执行</div>
            <div className="mode-card__desc">
              每批执行后等待完成，再执行下一批
            </div>
          </div>

          <div
            className={`mode-card ${value.mode === 'rolling' ? 'selected' : ''}`}
            onClick={() => updatePolicy({ mode: 'rolling' })}
          >
            <div className="mode-card__icon">🔄</div>
            <div className="mode-card__title">滚动执行</div>
            <div className="mode-card__desc">
              先小批量验证，再逐步扩大
            </div>
          </div>
        </div>
      </div>

      {value.mode === 'batch' && (
        <div className="config-section">
          <h4>批次大小</h4>
          <Input
            type="number"
            value={value.batchSize || 10}
            onChange={(e) => updatePolicy({ batchSize: parseInt(e.target.value) || 10 })}
            min={1}
            max={targetCount}
          />
          <p className="config-hint">
            将 {targetCount} 台设备分成 {Math.ceil(targetCount / (value.batchSize || 10))} 批执行
          </p>
        </div>
      )}

      {value.mode === 'rolling' && (
        <div className="config-section">
          <h4>滚动参数</h4>
          <div className="rolling-params">
            <div className="param">
              <label>初始批次大小</label>
              <Input
                type="number"
                value={value.rollingInitialBatch || 1}
                onChange={(e) => updatePolicy({ rollingInitialBatch: parseInt(e.target.value) || 1 })}
                min={1}
                max={targetCount}
              />
            </div>
            <div className="param">
              <label>每批增量</label>
              <Input
                type="number"
                value={value.rollingIncrement || 5}
                onChange={(e) => updatePolicy({ rollingIncrement: parseInt(e.target.value) || 5 })}
                min={1}
              />
            </div>
          </div>
          <p className="config-hint">
            执行顺序: {value.rollingInitialBatch || 1} → {Math.min((value.rollingInitialBatch || 1) + (value.rollingIncrement || 5), targetCount)} → ... → {targetCount}
          </p>
        </div>
      )}

      <div className="config-section">
        <h4>失败处理</h4>
        <div className="failure-options">
          <label className="checkbox-label">
            <input
              type="checkbox"
              checked={value.continueOnFailure}
              onChange={(e) => updatePolicy({ continueOnFailure: e.target.checked })}
            />
            <span>失败后继续执行其他设备</span>
          </label>
        </div>

        <div className="threshold-config">
          <label>失败阈值: {value.failureThresholdPercent}%</label>
          <Slider
            value={value.failureThresholdPercent}
            onChange={(v) => updatePolicy({ failureThresholdPercent: v })}
            min={0}
            max={100}
            step={5}
          />
          <p className="config-hint">
            失败率超过此阈值时自动暂停任务
          </p>
        </div>
      </div>

      <div className="config-section">
        <h4>超时与重试</h4>
        <div className="timeout-retry-config">
          <div className="param">
            <label>单设备超时时间 (秒)</label>
            <Input
              type="number"
              value={value.timeoutSecs}
              onChange={(e) => updatePolicy({ timeoutSecs: parseInt(e.target.value) || 300 })}
              min={10}
              max={3600}
            />
          </div>

          <div className="param">
            <label>失败重试次数</label>
            <Input
              type="number"
              value={value.retryCount}
              onChange={(e) => updatePolicy({ retryCount: parseInt(e.target.value) || 0 })}
              min={0}
              max={5}
            />
          </div>

          {value.retryCount > 0 && (
            <div className="param">
              <label>重试间隔 (秒)</label>
              <Input
                type="number"
                value={value.retryIntervalSecs}
                onChange={(e) => updatePolicy({ retryIntervalSecs: parseInt(e.target.value) || 5 })}
                min={1}
                max={60}
              />
            </div>
          )}
        </div>
      </div>
    </div>
  );
};

export default ExecutionPolicyConfig;
```

#### 6.2.5 TaskProgressPanel.tsx

```tsx
/**
 * TaskProgressPanel - 任务进度面板
 * 实时显示任务执行进度、状态和结果
 */

import React, { useEffect, useState } from 'react';
import { batchApi } from '../api/batchApi';
import { BatchTask, DeviceResult, TaskProgress } from '../types';
import ProgressBar from '../common/ProgressBar';
import StatusBadge from '../common/StatusBadge';
import TaskResultTable from './TaskResultTable';
import { Button } from '../common';

interface TaskProgressPanelProps {
  task: BatchTask;
  onClose?: () => void;
}

const TaskProgressPanel: React.FC<TaskProgressPanelProps> = ({ task, onClose }) => {
  const [progress, setProgress] = useState<TaskProgress>(task.progress);
  const [results, setResults] = useState<DeviceResult[]>([]);
  const [autoRefresh, setAutoRefresh] = useState(true);

  useEffect(() => {
    if (!autoRefresh || task.status === 'completed' || task.status === 'failed') {
      return;
    }

    const interval = setInterval(async () => {
      try {
        const newProgress = await batchApi.getTaskProgress(task.id);
        setProgress(newProgress);

        if (newProgress.completed + newProgress.failed + newProgress.cancelled >= newProgress.total) {
          const newResults = await batchApi.getTaskResults(task.id);
          setResults(newResults.results);
          setAutoRefresh(false);
        }
      } catch (error) {
        console.error('[TaskProgressPanel] Failed to refresh:', error);
      }
    }, 2000);

    return () => clearInterval(interval);
  }, [task.id, autoRefresh, task.status]);

  const handleStop = async () => {
    try {
      await batchApi.stopTask(task.id);
    } catch (error) {
      console.error('[TaskProgressPanel] Failed to stop task:', error);
    }
  };

  const handlePause = async () => {
    try {
      await batchApi.pauseTask(task.id);
    } catch (error) {
      console.error('[TaskProgressPanel] Failed to pause task:', error);
    }
  };

  const completionPercent = progress.total > 0
    ? ((progress.completed / progress.total) * 100).toFixed(1)
    : 0;

  return (
    <div className="task-progress-panel">
      <div className="panel-header">
        <div className="header-info">
          <h3>{task.name}</h3>
          <StatusBadge status={task.status} />
        </div>
        <div className="header-actions">
          {task.status === 'running' && (
            <>
              <Button size="small" onClick={handlePause}>暂停</Button>
              <Button size="small" variant="danger" onClick={handleStop}>停止</Button>
            </>
          )}
          {onClose && <Button variant="ghost" onClick={onClose}>×</Button>}
        </div>
      </div>

      <div className="progress-overview">
        <div className="progress-main">
          <div className="progress-circle">
            <svg viewBox="0 0 100 100">
              <circle
                className="progress-bg"
                cx="50"
                cy="50"
                r="45"
                fill="none"
                strokeWidth="8"
              />
              <circle
                className="progress-fill"
                cx="50"
                cy="50"
                r="45"
                fill="none"
                strokeWidth="8"
                strokeDasharray={`${parseFloat(completionPercent) * 2.83} 283`}
                transform="rotate(-90 50 50)"
              />
            </svg>
            <div className="progress-text">
              <span className="percent">{completionPercent}%</span>
              <span className="label">完成</span>
            </div>
          </div>
        </div>

        <div className="progress-stats">
          <div className="stat">
            <span className="stat-value">{progress.completed}</span>
            <span className="stat-label">成功</span>
          </div>
          <div className="stat stat-failed">
            <span className="stat-value">{progress.failed}</span>
            <span className="stat-label">失败</span>
          </div>
          <div className="stat stat-running">
            <span className="stat-value">{progress.running}</span>
            <span className="stat-label">执行中</span>
          </div>
          <div className="stat stat-pending">
            <span className="stat-value">{progress.pending}</span>
            <span className="stat-label">等待中</span>
          </div>
        </div>
      </div>

      <div className="progress-bar-container">
        <ProgressBar
          value={progress.completed}
          total={progress.total}
          failed={progress.failed}
          showLabel
        />
      </div>

      {progress.startedAt && (
        <div className="time-info">
          <span>开始时间: {new Date(progress.startedAt).toLocaleString()}</span>
          {progress.completedAt && (
            <span>完成时间: {new Date(progress.completedAt).toLocaleString()}</span>
          )}
        </div>
      )}

      {results.length > 0 && (
        <div className="results-section">
          <h4>执行结果</h4>
          <TaskResultTable results={results} />
        </div>
      )}
    </div>
  );
};

export default TaskProgressPanel;
```

### 6.3 API层

```typescript
// batchApi.ts - 批量任务前端API

import { invoke } from '@tauri-apps/api/core';
import type {
  BatchTask,
  TaskProgress,
  DeviceResult,
  TargetSelector,
  ExecutionPolicy,
  DeviceInfo,
} from './types';

// API结果类型定义
interface CreateTaskResult { task: BatchTask; }
interface ListTasksResult { tasks: BatchTask[]; total: number; }
interface GetProgressResult { progress: TaskProgress; }
interface TaskResultsResult { results: DeviceResult[]; total: number; }
interface ListDevicesResult { devices: DeviceInfo[]; total: number; }

/**
 * 批量任务API - Tauri后端通信层
 */
export const batchApi = {
  /**
   * 创建批量任务
   */
  async createTask(params: {
    name: string;
    description?: string;
    taskType: string;
    targetSelector: TargetSelector;
    executionPolicy: ExecutionPolicy;
    content: string;
    createdBy?: string;
    priority?: number;
    scheduledAt?: string;
  }): Promise<BatchTask> {
    const result = await invoke<CreateTaskResult>('create_batch_task', { params });
    return result.task;
  },

  /**
   * 获取任务详情
   */
  async getTask(taskId: string): Promise<BatchTask> {
    return invoke<BatchTask>('get_batch_task', { taskId });
  },

  /**
   * 列出任务
   */
  async listTasks(filter: {
    status?: string;
    taskType?: string;
    createdBy?: string;
    fromDate?: string;
    toDate?: string;
    offset?: number;
    limit?: number;
  }): Promise<{ tasks: BatchTask[]; total: number }> {
    return invoke<ListTasksResult>('list_batch_tasks', { filter });
  },

  /**
   * 启动任务
   */
  async startTask(taskId: string): Promise<{ executionId: string; targetCount: number }> {
    return invoke('start_batch_task', { taskId });
  },

  /**
   * 停止任务
   */
  async stopTask(taskId: string): Promise<void> {
    return invoke('stop_batch_task', { taskId });
  },

  /**
   * 暂停任务
   */
  async pauseTask(taskId: string): Promise<void> {
    return invoke('pause_batch_task', { taskId });
  },

  /**
   * 恢复任务
   */
  async resumeTask(taskId: string): Promise<void> {
    return invoke('resume_batch_task', { taskId });
  },

  /**
   * 获取任务进度
   */
  async getTaskProgress(taskId: string): Promise<TaskProgress> {
    const result = await invoke<GetProgressResult>('get_batch_task_progress', { taskId });
    return result.progress;
  },

  /**
   * 获取任务结果
   */
  async getTaskResults(
    taskId: string,
    options?: { includeOutput?: boolean; offset?: number; limit?: number }
  ): Promise<{ results: DeviceResult[]; total: number }> {
    return invoke<TaskResultsResult>('get_batch_task_results', { taskId, options });
  },

  /**
   * 删除任务
   */
  async deleteTask(taskId: string): Promise<void> {
    return invoke('delete_batch_task', { taskId });
  },

  /**
   * 验证目标选择器
   */
  async validateTargetSelector(selector: TargetSelector): Promise<{ valid: boolean; deviceCount: number; errors: string[] }> {
    return invoke('validate_target_selector', { selector });
  },
};

/**
 * 设备API
 */
export const deviceApi = {
  /**
   * 获取设备列表
   */
  async listDevices(filter?: {
    groups?: string[];
    tags?: string[];
    status?: string;
    offset?: number;
    limit?: number;
  }): Promise<{ devices: DeviceInfo[]; total: number }> {
    return invoke<ListDevicesResult>('list_devices', { filter });
  },

  /**
   * 获取设备详情
   */
  async getDevice(deviceId: string): Promise<DeviceInfo> {
    return invoke<DeviceInfo>('get_device', { deviceId });
  },

  /**
   * 获取设备分组
   */
  async listGroups(parentId?: string): Promise<{ groups: Array<{ id: string; name: string; description?: string }> }> {
    return invoke('list_device_groups', { parentId });
  },
};

// 导出为默认
export default { batchApi, deviceApi };
```

### 6.4 类型定义

```typescript
// types/index.ts - 前端类型定义

/**
 * 任务类型
 */
export type TaskType = 'command' | 'script' | 'config' | 'file_transfer' | 'upgrade' | 'reboot';

/**
 * 任务状态
 */
export type TaskStatus = 'pending' | 'running' | 'paused' | 'completed' | 'failed' | 'cancelled';

/**
 * 选择器类型
 */
export type SelectorType = 'all' | 'group' | 'tag' | 'device_ids' | 'filter';

/**
 * 执行模式
 */
export type ExecutionMode = 'parallel' | 'batch' | 'rolling';

/**
 * 设备状态
 */
export type DeviceStatus = 'online' | 'offline' | 'unknown';

/**
 * 设备结果状态
 */
export type DeviceResultStatus = 'success' | 'failed' | 'timeout' | 'cancelled' | 'skipped';

/**
 * 目标选择器
 */
export interface TargetSelector {
  selectorType: SelectorType;
  groups?: string[];
  tags?: string[];
  deviceIds?: string[];
  filterExpr?: string;
}

/**
 * 执行策略
 */
export interface ExecutionPolicy {
  mode: ExecutionMode;
  batchSize?: number;
  continueOnFailure: boolean;
  failureThresholdPercent: number;
  timeoutSecs: number;
  retryCount: number;
  retryIntervalSecs?: number;
  rollingInitialBatch?: number;
  rollingIncrement?: number;
}

/**
 * 任务进度
 */
export interface TaskProgress {
  total: number;
  completed: number;
  failed: number;
  running: number;
  pending: number;
  cancelled: number;
  currentBatch: number;
  startedAt?: string;
  completedAt?: string;
  estimatedRemainingSecs?: number;
}

/**
 * 设备结果
 */
export interface DeviceResult {
  deviceId: string;
  deviceName?: string;
  status: DeviceResultStatus;
  startedAt: string;
  completedAt?: string;
  durationMs?: number;
  output?: string;
  error?: string;
  retryCount: number;
}

/**
 * 任务摘要
 */
export interface TaskSummary {
  successCount: number;
  failureCount: number;
  totalDurationSecs: number;
  results: DeviceResult[];
}

/**
 * 批量任务
 */
export interface BatchTask {
  id: string;
  name: string;
  description?: string;
  taskType: TaskType;
  targetSelector: TargetSelector;
  content: string;
  executionPolicy: ExecutionPolicy;
  createdBy?: string;
  createdAt: string;
  updatedAt: string;
  status: TaskStatus;
  progress: TaskProgress;
  priority: number;
  scheduledAt?: string;
  summary?: TaskSummary;
}

/**
 * 设备信息
 */
export interface DeviceInfo {
  id: string;
  name: string;
  groups: string[];
  tags: Record<string, string>;
  metadata: Record<string, string>;
  status: DeviceStatus;
  lastSeenAt?: string;
}

/**
 * 设备分组
 */
export interface DeviceGroup {
  id: string;
  name: string;
  description?: string;
  parentId?: string;
  createdAt: string;
  updatedAt: string;
}
```

### 6.5 状态管理

```typescript
// context/BatchTaskContext.tsx

import React, { createContext, useContext, useReducer, useCallback, useEffect } from 'react';
import { batchApi, deviceApi } from '../api';
import type { BatchTask, DeviceInfo, TaskFilter, TaskProgress, DeviceResult } from '../types';

interface BatchTaskState {
  tasks: BatchTask[];
  devices: DeviceInfo[];
  selectedTask: BatchTask | null;
  selectedTaskProgress: TaskProgress | null;
  selectedTaskResults: DeviceResult[];
  loading: boolean;
  error: string | null;
  filter: TaskFilter;
}

type BatchTaskAction =
  | { type: 'SET_LOADING'; payload: boolean }
  | { type: 'SET_ERROR'; payload: string | null }
  | { type: 'SET_TASKS'; payload: BatchTask[] }
  | { type: 'SET_DEVICES'; payload: DeviceInfo[] }
  | { type: 'SET_SELECTED_TASK'; payload: BatchTask | null }
  | { type: 'SET_TASK_PROGRESS'; payload: TaskProgress | null }
  | { type: 'SET_TASK_RESULTS'; payload: DeviceResult[] }
  | { type: 'UPDATE_TASK'; payload: BatchTask }
  | { type: 'ADD_TASK'; payload: BatchTask }
  | { type: 'REMOVE_TASK'; payload: string }
  | { type: 'SET_FILTER'; payload: TaskFilter };

const initialState: BatchTaskState = {
  tasks: [],
  devices: [],
  selectedTask: null,
  selectedTaskProgress: null,
  selectedTaskResults: [],
  loading: false,
  error: null,
  filter: {},
};

function batchTaskReducer(state: BatchTaskState, action: BatchTaskAction): BatchTaskState {
  switch (action.type) {
    case 'SET_LOADING':
      return { ...state, loading: action.payload };
    case 'SET_ERROR':
      return { ...state, error: action.payload };
    case 'SET_TASKS':
      return { ...state, tasks: action.payload };
    case 'SET_DEVICES':
      return { ...state, devices: action.payload };
    case 'SET_SELECTED_TASK':
      return { ...state, selectedTask: action.payload };
    case 'SET_TASK_PROGRESS':
      return { ...state, selectedTaskProgress: action.payload };
    case 'SET_TASK_RESULTS':
      return { ...state, selectedTaskResults: action.payload };
    case 'UPDATE_TASK':
      return {
        ...state,
        tasks: state.tasks.map((t) => (t.id === action.payload.id ? action.payload : t)),
        selectedTask: state.selectedTask?.id === action.payload.id ? action.payload : state.selectedTask,
      };
    case 'ADD_TASK':
      return { ...state, tasks: [action.payload, ...state.tasks] };
    case 'REMOVE_TASK':
      return {
        ...state,
        tasks: state.tasks.filter((t) => t.id !== action.payload),
        selectedTask: state.selectedTask?.id === action.payload ? null : state.selectedTask,
      };
    case 'SET_FILTER':
      return { ...state, filter: action.payload };
    default:
      return state;
  }
}

interface BatchTaskContextValue extends BatchTaskState {
  loadTasks: (filter?: TaskFilter) => Promise<void>;
  loadDevices: () => Promise<void>;
  selectTask: (taskId: string) => Promise<void>;
  createTask: (params: any) => Promise<BatchTask>;
  startTask: (taskId: string) => Promise<void>;
  stopTask: (taskId: string) => Promise<void>;
  pauseTask: (taskId: string) => Promise<void>;
  resumeTask: (taskId: string) => Promise<void>;
  deleteTask: (taskId: string) => Promise<void>;
  refreshProgress: (taskId: string) => Promise<void>;
  setFilter: (filter: TaskFilter) => void;
}

const BatchTaskContext = createContext<BatchTaskContextValue | null>(null);

export const BatchTaskProvider: React.FC<{ children: React.ReactNode }> = ({ children }) => {
  const [state, dispatch] = useReducer(batchTaskReducer, initialState);

  const loadTasks = useCallback(async (filter?: TaskFilter) => {
    dispatch({ type: 'SET_LOADING', payload: true });
    try {
      const result = await batchApi.listTasks(filter || state.filter);
      dispatch({ type: 'SET_TASKS', payload: result.tasks });
    } catch (error) {
      dispatch({ type: 'SET_ERROR', payload: (error as Error).message });
    } finally {
      dispatch({ type: 'SET_LOADING', payload: false });
    }
  }, [state.filter]);

  const loadDevices = useCallback(async () => {
    try {
      const result = await deviceApi.listDevices();
      dispatch({ type: 'SET_DEVICES', payload: result.devices });
    } catch (error) {
      dispatch({ type: 'SET_ERROR', payload: (error as Error).message });
    }
  }, []);

  const selectTask = useCallback(async (taskId: string) => {
    try {
      const task = await batchApi.getTask(taskId);
      dispatch({ type: 'SET_SELECTED_TASK', payload: task });
      if (task.status === 'running') {
        dispatch({ type: 'SET_TASK_PROGRESS', payload: task.progress });
      }
    } catch (error) {
      dispatch({ type: 'SET_ERROR', payload: (error as Error).message });
    }
  }, []);

  const createTask = useCallback(async (params: any) => {
    const task = await batchApi.createTask(params);
    dispatch({ type: 'ADD_TASK', payload: task });
    return task;
  }, []);

  const startTask = useCallback(async (taskId: string) => {
    await batchApi.startTask(taskId);
    const task = await batchApi.getTask(taskId);
    dispatch({ type: 'UPDATE_TASK', payload: task });
  }, []);

  const stopTask = useCallback(async (taskId: string) => {
    await batchApi.stopTask(taskId);
    const task = await batchApi.getTask(taskId);
    dispatch({ type: 'UPDATE_TASK', payload: task });
  }, []);

  const pauseTask = useCallback(async (taskId: string) => {
    await batchApi.pauseTask(taskId);
    const task = await batchApi.getTask(taskId);
    dispatch({ type: 'UPDATE_TASK', payload: task });
  }, []);

  const resumeTask = useCallback(async (taskId: string) => {
    await batchApi.resumeTask(taskId);
    const task = await batchApi.getTask(taskId);
    dispatch({ type: 'UPDATE_TASK', payload: task });
  }, []);

  const deleteTask = useCallback(async (taskId: string) => {
    await batchApi.deleteTask(taskId);
    dispatch({ type: 'REMOVE_TASK', payload: taskId });
  }, []);

  const refreshProgress = useCallback(async (taskId: string) => {
    const progress = await batchApi.getTaskProgress(taskId);
    dispatch({ type: 'SET_TASK_PROGRESS', payload: progress });
    
    if (progress.completed + progress.failed + progress.cancelled >= progress.total) {
      const results = await batchApi.getTaskResults(taskId);
      dispatch({ type: 'SET_TASK_RESULTS', payload: results.results });
    }
  }, []);

  const setFilter = useCallback((filter: TaskFilter) => {
    dispatch({ type: 'SET_FILTER', payload: filter });
  }, []);

  // 初始加载
  useEffect(() => {
    loadTasks();
    loadDevices();
  }, []);

  return (
    <BatchTaskContext.Provider
      value={{
        ...state,
        loadTasks,
        loadDevices,
        selectTask,
        createTask,
        startTask,
        stopTask,
        pauseTask,
        resumeTask,
        deleteTask,
        refreshProgress,
        setFilter,
      }}
    >
      {children}
    </BatchTaskContext.Provider>
  );
};

export const useBatchTasks = () => {
  const context = useContext(BatchTaskContext);
  if (!context) {
    throw new Error('useBatchTasks must be used within a BatchTaskProvider');
  }
  return context;
};
```

---

## 7. 实现文件清单

### 7.1 后端 (CC-rStationService/src/)

| 文件 | 说明 |
|------|------|
| `batch_task.rs` | BatchTask核心数据模型 |
| `batch_task_content.rs` | 各任务类型的具体内容结构 |
| `batch_store.rs` | SQLite持久化层 |
| `batch_executor.rs` | 批量执行引擎 |
| `target_selector.rs` | 目标选择器解析和执行 |
| `batch_task_service.rs` | gRPC服务实现 |
| `batch_task.proto` | Protobuf定义 |
| `lib.rs` | 添加`pub mod batch_*` |

### 7.2 前端 (CC-rClient/src/plugin/batch/)

| 文件 | 说明 |
|------|------|
| `types/index.ts` | TypeScript类型定义 |
| `api/batchApi.ts` | Tauri API调用封装 |
| `context/BatchTaskContext.tsx` | React Context状态管理 |
| `hooks/useBatchTasks.ts` | 批量任务Hook |
| `hooks/useTaskExecution.ts` | 任务执行Hook |
| `components/BatchTaskList.tsx` | 任务列表组件 |
| `components/BatchTaskCard.tsx` | 任务卡片组件 |
| `components/CreateTaskWizard.tsx` | 创建任务向导 |
| `components/TaskTargetSelector.tsx` | 目标选择器 |
| `components/ExecutionPolicyConfig.tsx` | 执行策略配置 |
| `components/TaskProgressPanel.tsx` | 进度面板 |
| `components/TaskResultTable.tsx` | 结果表格 |
| `components/FilterExpressionEditor.tsx` | 过滤表达式编辑器 |
| `components/DeviceSelector.tsx` | 设备选择器 |
| `components/common/StatusBadge.tsx` | 状态徽章 |
| `components/common/ProgressBar.tsx` | 进度条 |
| `index.ts` | 导出 |

### 7.3 Protobuf定义

```
CC-rStationService/proto/batch_task.proto
```

---

## 8. 依赖关系

```
Phase 7 批量操作支持
├── Phase 6 命令脚本管理系统 (已完成)
│   ├── script_engine.rs (复用)
│   ├── script_store.rs (复用模式)
│   └── ScriptContext (前端复用)
└── Phase 8 设备分组与标签 (规划中)
    └── 设备模型扩展 (被TargetSelector引用)
```

---

## 9. 风险与注意事项

1. **大规模设备并发**: 当目标设备数量超过100时，需要考虑连接池管理和限流
2. **gRPC流控**: 大量设备的日志流可能造成网络拥塞，需要实现背压
3. **状态一致性**: 任务执行中断后重启，需要正确恢复状态
4. **安全性**: 批量操作是危险操作，需要Phase 10的权限系统配合
5. **设备离线**: 执行过程中设备离线需要正确处理

---

## 10. 后续规划

- Phase 8: 设备分组与标签体系 - 完善设备管理基础
- Phase 9: 告警规则引擎 - 基于批量操作结果的告警
- Phase 10: 权限与审计系统 - 批量操作的权限控制和审计日志
