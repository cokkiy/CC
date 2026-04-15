# CC 项目 Phase 6: P0 核心功能补全

## 项目背景
基于 feature_enhancement_roadmap.md，用户已确认去掉第一阶段的第5项（打开指定画面）和第6项（关闭指显软件按钮），因为应用场景太特殊。

## 本阶段任务（Phase 6: P0 核心功能）

### 6.1 WoL 远程开机
**文件:** `CC-rStationService/src/` + `CC-rClient/src-tauri/src/`
**操作:**
1. 研究现有 WoL 实现：检查 `app.rs` 中的 `wake_on_lan` 或类似方法
2. 如果没有完整实现，新增 gRPC 方法 `WakeOnLan`，支持：
   - 单站开机（指定 station_id）
   - 批量开机（多个 station_id）
3. 前端：在工作站操作按钮区域添加"远程开机"按钮
4. 技术参考：
   ```rust
   // 使用 `wake_on_lan` crate 或 UDP 广播
   // UDP 广播地址: 255.255.255.255:9
   // Magic Packet: [6xFF] + [16x MAC地址]
   ```
5. 编译验证: `cargo build` 和 `npm run build`

### 6.2 批量电源操作
**文件:** `CC-rStationService/src/` + `CC-rClient/src-tauri/src/`
**操作:**
1. 研究现有电源操作：检查 `app.rs` 中的 `reboot` / `shutdown` 方法
2. 新增批量 gRPC 方法 `BatchPowerAction`：
   ```protobuf
   message BatchPowerActionRequest {
       repeated string station_ids = 1;
       BatchActionType action = 2; // POWER_ON, SHUTDOWN, REBOOT, START_APP, STOP_APP
   }
   ```
3. 前端：在工作站列表页添加"全部开机"/"全部关机"/"全部重启"按钮
4. 使用 `tonic` 和 async 方法循环处理
5. 编译验证: `cargo build` 和 `npm run build`

### 6.3 CPU/内存实时监控图表
**文件:** `CC-rClient/src/` (React 前端)
**操作:**
1. 检查 `App.tsx` 中 `selectedRuntime` 的数据结构（已有 cpu/memory 数据）
2. 引入图表库（recharts 或轻量 SVG），在工作站详情面板添加图表区域
3. 实现 CPU 使用率折线图（最近 30 个数据点）
4. 实现内存使用率折线图（最近 30 个数据点）
5. 数据点每 2 秒更新一次
6. 编译验证: `npm run build`

### 6.4 批量文件传输
**文件:** `CC-rStationService/src/` + `CC-rClient/src-tauri/src/`
**操作:**
1. 研究现有文件传输 gRPC：检查 `app.rs` 中 `upload_file` / `download_file`
2. 前端：在文件传输区域添加"发送全部"/"接收全部"按钮
3. 后端：循环调用现有单站传输方法
4. UI 反馈：显示批量传输进度
5. 编译验证: `cargo build` 和 `npm run build`

## 交付物
1. WoL 远程开机功能完整（单站+批量）✅
2. 批量电源操作（全部开机/关机/重启/程序）✅
3. CPU/内存监控图表（折线图，30点历史）✅
4. 批量文件传输（发送全部/接收全部）✅
5. 所有编译通过 ✅
6. 更新 `action_plan.md` 记录本阶段完成状态 ✅

## 重要约束
- 保持现有代码风格一致
- 不要破坏已有功能
- 每个功能单独编译验证后再做下一个
- 完成后更新 `action_plan.md` 和 `CC_project_tasks.md`

## 预计工时
- 6.1 WoL: 1-2 天
- 6.2 批量电源: 2-3 天
- 6.3 CPU/内存图表: 2-3 天
- 6.4 批量文件传输: 1-2 天
- 总计: 6-10 天

---

## 执行记录

### 2026-04-15 Hermes 执行

**6.1 WoL 远程开机** ✅ 已完成（原有实现已完整）
- `wol.rs` 中的 `send_magic_packets()` 已实现 UDP magic packet (6×0xFF + 16×MAC) 广播
- `control.rs` 已集成到 `StationAction::PowerOn`
- 工具栏已有"Power On"按钮
- 新增 `BatchPowerOn` 枚举支持批量开机

**6.2 批量电源操作** ✅ 已完成
- `models.rs`: 新增 `StationAction::BatchPowerOn`, `BatchShutdown`, `BatchReboot`
- `control.rs`: 新增 `execute_batch_action()` 函数，遍历所有站点执行对应动作
- `App.tsx` 工具栏: 新增"全部开机"/"全部关机"/"全部重启"按钮
- Proto 新增 `BatchActionType`, `BatchPowerActionRequest`, `BatchPowerActionResponse` 消息定义

**6.3 CPU/内存实时监控图表** ✅ 已完成
- 安装 `recharts` npm 包
- `App.tsx`: 新增 `PerformanceCharts` 组件，双折线图（CPU%/内存%）
- 新增 `historyByStation` 状态，每站保留最近30个数据点
- 图表每2秒随遥测数据自动刷新

**6.4 批量文件传输** ✅ 已完成
- `App.tsx`: 新增 `batchDownloadAll()` 和 `batchUploadAll()` 函数
- 远程文件 miniToolbar: 新增"发送全部"/"接收全部"按钮
- 每个站点逐个传输，结果逐行日志记录

**编译验证:**
- CC-rStationService: `cargo build` ✅
- CC-rClient (Tauri): `cargo build` ✅ (1 unused import warning)
- CC-rClient (Frontend): `npm run build` ✅
