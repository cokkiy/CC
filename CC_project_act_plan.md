# CC 项目 ACT 执行计划（基于 works.md）

> 适用范围：CC-rClient 为主，必要时联动 CC-rStationService / CC-Aggregator  
> 目标：把 `works.md` 中的 9 项待办拆成可落地、可验证、可回归的执行步骤  
> 原则：先修 Bug，再改交互，再补能力；先明确模型，再改 UI；每一步都可独立验证

---

## 0. 计划总览

### 0.1 待办分类

#### A. 直接 Bugfix
1. Device 页 “clear filters” 按钮改为 icon-only
2. Device 面板无法滚动到底
3. IoT simulator 设备无法分配 group/tag
4. Device editor 无法保存 “Startup Programs” 和 “Monitor Processes”

#### B. 界面优化
5. Runtime & Tools 面板布局调整
6. Batch 页任务创建支持更多任务类型
7. Task Content 编辑器支持语法高亮

#### C. 模型/语义澄清
8. 明确 batch task 中 `Command` / `Script` / `App Control` / 文件传输 的边界与能力

---

## 1. 目标与交付标准

### 1.1 总目标
将设备页、批量页和设备编辑器中的关键交互问题修复，并完成 batch task 类型的语义收敛，使前端 UI 与后端数据模型保持一致。

### 1.2 交付标准
- Device 页交互可用且无滚动/布局缺陷
- IoT simulator 设备可正常分配 group/tag
- Device editor 可持久化保存 Startup Programs / Monitor Processes
- Batch task 创建界面支持扩展后的任务类型
- Task Content 编辑器具备语法高亮能力
- `works.md` 对应事项有明确落地结果或后续拆分结论

---

## 2. 实施顺序

### Phase A：先修复高优先级 Bug
**目标**：先把影响使用的页面问题修掉，避免后续改模型时被 UI 问题干扰。

#### A.1 Device 页按钮与布局修复
- [ ] 定位 Device 页顶部筛选区和 clear filters 按钮实现
- [ ] 将按钮替换为 icon-only 形式
- [ ] 保留无障碍文本或 tooltip
- [ ] 检查按钮在不同屏宽下的点击区域和对齐
- [ ] 回归验证：筛选后清空功能正常

**验收标准**
- 按钮视觉上变为小图标按钮
- 不影响筛选重置功能
- 移动/窄屏下不挤压其它控件

#### A.2 Device 面板滚动问题修复
- [ ] 定位 Device panel 的滚动容器和父级布局
- [ ] 检查是否存在固定高度、`overflow: hidden`、flex 子元素未正确收缩等问题
- [ ] 修正滚动容器，确保 stationCard 可滚动到底
- [ ] 检查横向溢出与滚动条行为
- [ ] 回归验证：长列表可完整滚动

**验收标准**
- 面板底部内容可见
- stationCard 不再只能看到标题
- 无明显布局跳动

#### A.3 IoT simulator 分组/标签赋值修复
- [ ] 定位 group/tag 赋值入口
- [ ] 检查 IoT simulator 设备类型和普通设备类型的条件分支
- [ ] 统一赋值逻辑，避免 simulator 被错误过滤
- [ ] 补充回归测试或手动验证路径

**验收标准**
- IoT simulator 可以选择并保存 group/tag
- 与普通设备行为一致

#### A.4 Device editor 保存问题修复
- [ ] 定位 Startup Programs 和 Monitor Processes 的表单状态来源
- [ ] 检查保存时的数据序列化字段是否遗漏
- [ ] 检查前后端字段名是否一致
- [ ] 修复保存/更新流程
- [ ] 验证编辑后刷新仍保持数据

**验收标准**
- 两个字段可保存、可读取、可回显
- 无静默丢失

---

### Phase B：Runtime & Tools 面板重排
**目标**：优化设备页信息密度，减少拥挤和视觉噪音。

#### B.1 面板布局调整
- [ ] 定位 Runtime & Tools 面板结构
- [ ] 将 endpoint block 拆成独立一行
- [ ] 移除 “Batch Captures” block
- [ ] 移除 “Remote Files” block
- [ ] 调整剩余内容的 spacing 与响应式布局
- [ ] 验证窄屏下不会换行混乱

**验收标准**
- endpoint 独立成行
- 两个 block 被移除
- 其它内容布局稳定

---

### Phase C：Batch 任务类型收敛与扩展
**目标**：先把 batch task 类型语义定清，再扩展 UI 和后端结构。

#### C.1 任务类型语义澄清
- [ ] 梳理现有 `Command` 和 `Script` 的实际用途
- [ ] 明确以下问题：
  - `Command` 是否代表控制消息、检测消息、文件操作消息，还是仅仅是执行类命令
  - `Script` 是否限定为设备端执行
  - `start app / stop app / restart app` 是否合并为 `App Control`
  - `App Control` 是否需要 subtype 参数
- [ ] 输出最终结论：
  - 方案 1：保留独立 task type
  - 方案 2：合并为 `App Control` + subtype
  - 方案 3：Command 作为底层控制通道，Script 为设备脚本执行，File Transfer 独立成类
- [ ] 形成文档级约定，作为后续实现依据

**建议结论方向**
- `Script`：设备端脚本执行
- `Command`：控制类动作/平台消息/非脚本型设备交互
- `App Control`：统一承载 start/stop/restart，使用 `subtype` 指定动作
- `File Transfer`：单独任务类型，支持 push/pull file/folder

#### C.2 批量任务类型扩展
- [ ] 在 batch task 类型中增加 `stop app`、`restart app`
- [ ] 或将 start/stop/restart 统一为 `App Control`
- [ ] 增加 `push file`
- [ ] 增加 `pull file`
- [ ] 增加 `push folder`
- [ ] 增加 `pull folder`
- [ ] 为每种类型定义必要参数结构
- [ ] 与执行器能力对齐

**验收标准**
- Batch Editor 能正确创建这些任务
- 类型在后端可序列化、可持久化
- 任务列表和详情页可识别新类型

#### C.3 批量任务编辑器联动
- [ ] 根据 task type 切换不同表单字段
- [ ] 对 App Control 显示 subtype 选择器
- [ ] 对 File Transfer 显示 source/target/path 参数
- [ ] 对 Script 显示脚本内容与参数
- [ ] 对 Command 显示通用控制字段
- [ ] 检查校验规则是否按类型生效

**验收标准**
- 不同类型只显示必要字段
- 表单提交结果符合模型定义

---

### Phase D：Task Content 编辑器语法高亮
**目标**：提升脚本/命令编辑体验，并为后续复杂内容编辑打基础。

#### D.1 编辑器能力设计
- [ ] 确定编辑器是复用单一组件还是按 task type 分支
- [ ] 明确高亮模式映射：
  - Script → shell / powershell / python
  - Command → JSON / plain text / 自定义控制语法
  - File Transfer → 结构化表单为主，内容编辑为辅
- [ ] 选择实现方式：现有文本编辑器增强或引入专用语法高亮库

#### D.2 编辑器实现
- [ ] 给 Task Content editor 加入 mode 配置
- [ ] 按 task type 切换语法高亮
- [ ] 保持复制、粘贴、撤销、换行等基本体验
- [ ] 处理超长文本与滚动
- [ ] 验证在浅色/深色主题下可读性

**验收标准**
- 不同任务类型显示对应高亮模式
- 编辑体验不卡顿
- 输入内容不会丢失

---

## 3. 推荐里程碑

### Milestone 1：基础可用性修复
完成以下项：
- Device 页按钮优化
- Device 面板滚动修复
- IoT simulator group/tag 修复
- Device editor 保存修复

### Milestone 2：界面整理
完成以下项：
- Runtime & Tools 面板重排
- 其它明显布局噪点整理

### Milestone 3：Batch 模型收敛
完成以下项：
- Command / Script / App Control / File Transfer 语义定稿
- 扩展 batch task 类型
- Batch 编辑器联动改造

### Milestone 4：编辑器增强
完成以下项：
- Task Content 语法高亮
- 按任务类型切换编辑模式

---

## 4. 依赖关系

### 4.1 前置依赖
- 先确认 batch 任务语义，再改编辑器 UI
- 先修设备页基础布局，再做批量页联动
- 先修数据保存问题，再改表单逻辑

### 4.2 风险点
- batch task 语义不清会导致 UI 与后端反复返工
- 编辑器高亮如果过早引入，可能和最终 task type 设计冲突
- Device editor 保存问题若是字段映射错误，可能需要同步修后端类型

---

## 5. 验证策略

### 5.1 前端验证
- 启动客户端
- 打开 Device 页、Batch 页、Device editor
- 验证以下行为：
  - 按钮显示和点击正常
  - 滚动到底正常
  - group/tag 可保存
  - Editor 可保存回显
  - 新 batch 类型可创建

### 5.2 回归检查
- 检查现有测试是否覆盖相关页面
- 必要时补充/更新前端测试
- 对 batch 类型变更补充最小单元测试或类型测试

---

## 6. 建议的执行细化清单

### 第一轮
- [ ] 读取 Device 页相关文件
- [ ] 读取 Batch 页相关文件
- [ ] 读取 Device editor 相关文件
- [ ] 读取 types 定义和 API 层
- [ ] 确定 task type 最终语义

### 第二轮
- [ ] 实施 Device 页修复
- [ ] 实施 Device editor 保存修复
- [ ] 实施 IoT simulator group/tag 修复

### 第三轮
- [ ] 改 Runtime & Tools 布局
- [ ] 扩展 batch task 类型
- [ ] 改 Batch Editor 表单逻辑

### 第四轮
- [ ] 接入 Task Content 高亮编辑器
- [ ] 进行前端构建验证
- [ ] 执行回归测试

---

## 7. 结论

这份 ACT 计划的核心策略是：

1. **先修 bug，再做增强**
2. **先统一 batch 语义，再改编辑器**
3. **先保证保存与滚动等基础体验，再做高亮和新类型**
4. **每个改动都要能独立验证**

如果后续进入 ACT 模式，建议按上述 Milestone 顺序逐步实施，并在每一步完成后更新回归结果。
