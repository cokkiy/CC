# Phase 8: 设备分组与标签体系

**目标**: 完善设备管理基础能力，支持灵活的设备组织方式

**工期**: 2-3周

**状态**: 🔄 开发中

---

## 1. 数据模型扩展

### 1.1 Station 模型扩展
```typescript
// 现有 Station 类型扩展
export type Station = {
  id: string;
  name: string;
  blocked: boolean;
  networkInterfaces: NetworkInterface[];
  startPrograms: StartProgram[];
  monitorProcesses: string[];
  lastAction?: string | null;
  // Phase 8 新增字段
  groups: string[];              // 所属分组 ID 列表
  tags: Record<string, string>;  // 标签键值对
  metadata: Record<string, string>; // 设备元数据
  location?: Location;          // 地理位置
};

export type Location = {
  latitude?: number;
  longitude?: number;
  altitude?: number;
  address?: string;
  region?: string;
  country?: string;
};
```

### 1.2 StationGroup 模型
```typescript
export type StationGroup = {
  id: string;
  name: string;
  description: string;
  color: string;           // UI 显示颜色
  icon?: string;           // 可选图标
  tags: string[];          // 分组标签（用于筛选）
  stationIds: string[];    // 组成员 ID
  createdAt: number;
  updatedAt: number;
  createdBy: string;
};
```

### 1.3 TagDefinition 模型
```typescript
export type TagDefinition = {
  key: string;             // 标签键 (e.g., "environment", "role")
  label: string;           // 显示名称 (e.g., "环境", "角色")
  type: 'string' | 'number' | 'boolean' | 'select';
  options?: string[];      // 对于 select 类型，可选值
  required: boolean;
  defaultValue?: string;
  description?: string;
};
```

---

## 2. 架构设计

### 2.1 目录结构
```
src/plugin/groups/
├── types.ts              # 类型定义
├── api.ts                # Tauri API 封装
├── GroupsContext.tsx     # 分组状态管理
├── TagsContext.tsx       # 标签状态管理
├── GroupsPage.tsx        # 分组管理页面
├── TagsPage.tsx          # 标签管理页面
├── GroupEditor.tsx       # 分组编辑器
├── TagEditor.tsx         # 标签编辑器
├── GroupCard.tsx         # 分组卡片
├── StationGroupBadge.tsx # 设备分组徽章
├── index.ts              # 导出模块
```

### 2.2 Context 设计

#### GroupsContext
```typescript
interface GroupsContextValue {
  groups: StationGroup[];
  selectedGroup: StationGroup | null;
  selectGroup(id: string): void;
  createGroup(data: CreateGroupDTO): Promise<StationGroup>;
  updateGroup(id: string, data: UpdateGroupDTO): Promise<StationGroup>;
  deleteGroup(id: string): Promise<void>;
  addStationToGroup(groupId: string, stationId: string): Promise<void>;
  removeStationFromGroup(groupId: string, stationId: string): Promise<void>;
  importGroups(groups: StationGroup[]): Promise<void>;
  exportGroups(): Promise<StationGroup[]>;
}
```

#### TagsContext
```typescript
interface TagsContextValue {
  tagDefinitions: TagDefinition[];
  selectedTag: TagDefinition | null;
  createTag(data: TagDefinitionDTO): Promise<TagDefinition>;
  updateTag(key: string, data: TagDefinitionDTO): Promise<TagDefinition>;
  deleteTag(key: string): Promise<void>;
  getStationsByTag(tagKey: string, tagValue: string): Station[];
}
```

---

## 3. API 设计

### 3.1 分组 API
```rust
// 后端分组管理
POST   /api/groups              - 创建分组
GET    /api/groups               - 获取分组列表
GET    /api/groups/{id}          - 获取分组详情
PUT    /api/groups/{id}          - 更新分组
DELETE /api/groups/{id}          - 删除分组
POST   /api/groups/{id}/stations - 添加设备到分组
DELETE /api/groups/{id}/stations/{stationId} - 从分组移除设备
POST   /api/groups/import        - 导入分组
GET    /api/groups/export         - 导出分组
```

### 3.2 标签 API
```rust
// 后端标签管理
POST   /api/tags                 - 创建标签定义
GET    /api/tags                 - 获取标签定义列表
PUT    /api/tags/{key}           - 更新标签定义
DELETE /api/tags/{key}           - 删除标签定义
GET    /api/stations/tags        - 获取所有设备标签统计
```

### 3.3 设备标签 API
```rust
// 设备标签管理
GET    /api/stations/{id}/tags       - 获取设备标签
PUT    /api/stations/{id}/tags       - 更新设备标签
PATCH  /api/stations/{id}/tags        - 批量更新设备标签
PUT    /api/stations/{id}/location   - 更新设备位置
```

---

## 4. 前端组件设计

### 4.1 组件层次
```
GroupsPage (容器)
├── GroupList
│   ├── GroupCard (分组卡片)
│   └── CreateGroupButton
├── GroupDetail
│   ├── GroupInfoCard
│   ├── StationListInGroup
│   └── GroupActions
└── GroupEditor (Modal)

TagsPage (容器)
├── TagDefinitionList
│   ├── TagDefinitionCard
│   └── CreateTagButton
├── TagDefinitionEditor (Modal)
└── TagUsageStats
```

### 4.2 设备详情页扩展
```
StationDetail
├── BasicInfoSection
├── NetworkInterfacesSection
├── GroupsSection (Phase 8 新增)
│   ├── GroupBadges
│   └── AddToGroupButton
├── TagsSection (Phase 8 新增)
│   ├── TagBadges
│   └── EditTagsButton
└── LocationSection (Phase 8 新增)
    ├── LocationMap
    └── EditLocationButton
```

---

## 5. 实现计划

### Week 1: 基础架构
- [ ] 扩展 Station 类型定义
- [ ] 创建分组和标签 Context
- [ ] 实现分组 CRUD API
- [ ] 实现标签定义 CRUD API
- [ ] 设备标签更新 API

### Week 2: UI 开发
- [ ] GroupsPage 分组管理页面
- [ ] TagsPage 标签定义页面
- [ ] Station 详情页分组/标签展示
- [ ] 批量添加设备到分组
- [ ] 导入/导出功能

### Week 3: 完善和优化
- [ ] 位置信息管理（可选）
- [ ] 分组统计和排序
- [ ] 标签筛选功能
- [ ] 性能优化
- [ ] 集成测试

---

## 6. 依赖关系

- Phase 7 (批量操作) - TargetSelector 复用分组/标签选择
- Phase 6 (脚本管理) - ScriptContext 可复用状态管理模式
- Phase 9 (告警规则) - 依赖分组和标签进行告警范围设置

---

## 7. 测试策略

1. **单元测试**: 分组逻辑、标签匹配、位置计算
2. **集成测试**: API 端到端、分组增删改
3. **UI 测试**: 组件渲染、交互流程
4. **性能测试**: 大量设备/分组的列表渲染
