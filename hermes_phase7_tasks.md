# CC 项目 Phase 7: P1 企业 IT 管理能力

## 项目背景
Phase 6 (P0) 已完成并提交。Phase 7 继续推进 P1 优先级功能。

## 本阶段任务（Phase 7: P1）

### 7.1 远程命令行 / 脚本执行 🔴 P0→P1 升级
**文件:** `CC-rStationService/src/` + `CC-rClient/src-tauri/src/`
**现状:** 完全缺失
**目标:** 在工作站上执行任意命令或预定义脚本

**技术方案：**
```protobuf
message ExecuteCommandRequest {
    string station_id = 1;
    string command = 2;
    int32 timeout_seconds = 3;
}
message ExecuteCommandResponse {
    int32 exit_code = 1;
    string stdout = 2;
    string stderr = 3;
}
```

**操作步骤：**
1. 在 `proto/cc.proto` 新增 `ExecuteCommandRequest/Response` 消息
2. 在 `app.rs` 新增 `execute_command` gRPC 方法，调用 `std::process::Command` 执行命令
3. 在 `control.rs` 新增 `execute_command()` 函数调用 gRPC
4. 在 `App.tsx` 新增命令输入框 + 执行按钮区域（可放在工作站详情面板）
5. 显示 stdout/stderr/exit_code 返回结果

**竞品参考:** 向日葵 CMD/PowerShell、TeamViewer SSH/SFTP
**工作量:** 5-7 天
**编译验证:** `cargo build` + `npm run build`

---

### 7.2 监视间隔配置 UI
**文件:** `CC-rClient/src/App.tsx` + 设置页面
**现状:** `SetStateGatheringInterval` gRPC 有，但前端无设置入口
**目标:** 在设置页面添加监视间隔配置滑块/输入框

**操作步骤：**
1. 在设置面板找到或创建"监视设置"区域
2. 添加数字输入框或滑块（范围：1-60秒，建议默认值：2秒）
3. 调用 `SetStateGatheringInterval` gRPC 保存设置
4. 显示当前值和保存状态

**工作量:** 0.5 天
**编译验证:** `npm run build`

---


### 7.3 设备分组和标签管理
**文件:** `CC-rStationService/src/` + `CC-rClient/src/`
**现状:** 只有 name/mac/ip 过滤，无分组
**目标:** 添加工作站分组功能

**操作步骤：**
1. 研究现有数据结构：在 `models.rs` 或 `state.rs` 中工作站的存储方式
2. 新增分组概念：`Group` 结构（id, name, station_ids）
3. 支持 CRUD 分组：创建/重命名/删除分组，添加/移除工作站
4. 前端：在侧边栏或列表顶部添加分组下拉选择器/标签页
5. 按分组过滤工作站列表
6. 持久化：分组数据存哪里？（文件 or gRPC 状态）

**竞品参考:** TeamViewer 设备管理、SCCM 集合
**工作量:** 3-5 天
**编译验证:** `cargo build` + `npm run build`

---

## 交付物
1. 远程命令行/脚本执行功能 ✅
2. 监视间隔配置 UI ✅
3. 气象云图配置表单 ✅
4. 设备分组和标签管理 ✅
5. 所有编译通过 ✅
6. 更新 `action_plan.md` 记录本阶段状态 ✅
7. 更新 `hermes_phase7_tasks.md` 执行记录 ✅

## 重要约束
- 保持现有代码风格一致
- 不要破坏已有功能（Phase 6 功能继续正常工作）
- 每个功能单独编译验证后再做下一个
- 完成后更新 `action_plan.md`、`hermes_phase7_tasks.md` 和 `CC_project_tasks.md`

## 预计工时
- 7.1 远程命令行: 5-7 天
- 7.2 监视间隔配置: 0.5 天
- 7.3 设备分组管理: 3-5 天
- 总计: 9.5-14.5 天

---