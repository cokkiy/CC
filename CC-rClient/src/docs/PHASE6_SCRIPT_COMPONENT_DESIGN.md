# Phase 6 Frontend Component Architecture Design
## ScriptEditor, ScriptList, ScriptRunner, ScriptMarketplace
## 参考 PluginHost 模式

---

## 1. 架构概览

### 1.1 设计目标

参考 `PluginHost` 的Facade模式，为脚本管理系统创建一个类似的分层架构：
- **ScriptHost**: 统一入口门面，协调各子模块
- **ScriptManager**: 脚本生命周期管理（类似 PluginManager）
- **ScriptRegistry**: 脚本存储和检索
- **ScriptExecutor**: 脚本执行管理
- **ScriptMarketplace**: 脚本导入导出市场

### 1.2 架构层次

```
┌─────────────────────────────────────────────────────────────┐
│                    UI Components                             │
│  ┌──────────────┐ ┌──────────────┐ ┌──────────────────┐   │
│  │ ScriptEditor  │ │  ScriptList  │ │  ScriptRunner    │   │
│  └──────────────┘ └──────────────┘ └──────────────────┘   │
│  ┌──────────────────────────────────────────────────────┐   │
│  │              ScriptMarketplace                       │   │
│  └──────────────────────────────────────────────────────┘   │
├─────────────────────────────────────────────────────────────┤
│                    ScriptHost (Facade)                      │
│  ┌──────────────┐ ┌──────────────┐ ┌──────────────────┐   │
│  │ScriptManager │ │ScriptRegistry│ │ ScriptExecutor   │   │
│  └──────────────┘ └──────────────┘ └──────────────────┘   │
├─────────────────────────────────────────────────────────────┤
│                    Tauri API Layer                          │
│  invoke('load_scripts'), invoke('execute_script'), etc.    │
├─────────────────────────────────────────────────────────────┤
│                    Rust Backend (Phase 6)                   │
│  scripts.rs, script_engine.rs, script_executor.rs           │
└─────────────────────────────────────────────────────────────┘
```

---

## 2. ScriptHost 设计

### 2.1 核心接口

```typescript
// src/plugin/script/ScriptHost.ts

import { invoke } from '@tauri-apps/api/core';
import type {
  CommandScript,
  ScriptParameter,
  ScriptExecutionContext,
  ScriptExecutionResult,
  ScriptPackage,
  ScriptImportResult,
  ScriptFilter,
  ScriptValidationResult,
  ScriptLogEntry,
  ScriptExecutionStatus,
} from './types';

// Script host configuration
export interface ScriptHostConfig {
  autoLoad?: boolean;           // 自动加载脚本
  maxHistory?: number;          // 最大执行历史记录
  defaultTimeout?: number;      // 默认超时时间(ms)
  enableLogging?: boolean;      // 启用详细日志
}

// Script execution tracking
interface ExecutionTrack {
  id: string;
  scriptId: string;
  status: ScriptExecutionStatus;
  startTime: number;
  results: ScriptExecutionResult[];
}

/**
 * ScriptHost - 脚本系统统一入口门面
 * 参考 PluginHost 的设计模式
 */
export class ScriptHost {
  private static instance: ScriptHost | null = null;
  private initialized: boolean = false;
  private config: ScriptHostConfig;

  // 子模块 - 类似 PluginHost 的 plugins, panels, actions, data
  private scripts: Map<string, CommandScript> = new Map();
  private executionTracks: Map<string, ExecutionTrack> = new Map();
  private executionHistory: ScriptLogEntry[] = [];
  private listeners: Map<string, Set<Function>> = new Map();

  private constructor(config: ScriptHostConfig = {}) {
    this.config = {
      autoLoad: true,
      maxHistory: 100,
      defaultTimeout: 60000,
      enableLogging: true,
      ...config,
    };
  }

  public static getInstance(config?: ScriptHostConfig): ScriptHost {
    if (!ScriptHost.instance) {
      ScriptHost.instance = new ScriptHost(config);
    }
    return ScriptHost.instance;
  }

  get isInitialized(): boolean {
    return this.initialized;
  }

  // ==================== 初始化 ====================

  async init(): Promise<void> {
    if (this.initialized) {
      console.warn('[ScriptHost] Already initialized');
      return;
    }

    console.log('[ScriptHost] Initializing...');
    this.setupEventForwarding();
    this.initialized = true;
    console.log('[ScriptHost] Initialized successfully');
  }

  async loadAll(): Promise<void> {
    if (!this.initialized) {
      await this.init();
    }
    await this.loadScriptsFromBackend();
  }

  // ==================== 脚本 CRUD ====================

  /**
   * 从后端加载所有脚本
   */
  async loadScriptsFromBackend(): Promise<CommandScript[]> {
    try {
      const scripts = await invoke<CommandScript[]>('load_scripts');
      this.scripts.clear();
      scripts.forEach(script => this.scripts.set(script.id, script));
      this.emit('scripts:loaded', { count: scripts.length });
      return scripts;
    } catch (error) {
      console.error('[ScriptHost] Failed to load scripts:', error);
      this.emit('scripts:error', { error });
      throw error;
    }
  }

  /**
   * 获取所有脚本
   */
  getAllScripts(): CommandScript[] {
    return Array.from(this.scripts.values());
  }

  /**
   * 根据ID获取脚本
   */
  getScript(id: string): CommandScript | null {
    return this.scripts.get(id) ?? null;
  }

  /**
   * 筛选脚本
   */
  filterScripts(filter: ScriptFilter): CommandScript[] {
    let results = Array.from(this.scripts.values());

    if (filter.search) {
      const search = filter.search.toLowerCase();
      results = results.filter(s =>
        s.name.toLowerCase().includes(search) ||
        s.description.toLowerCase().includes(search) ||
        s.tags.some(t => t.toLowerCase().includes(search))
      );
    }

    if (filter.scriptType) {
      results = results.filter(s => s.scriptType === filter.scriptType);
    }

    if (filter.tags?.length) {
      results = results.filter(s =>
        filter.tags!.some(t => s.tags.includes(t))
      );
    }

    if (filter.isTemplate !== undefined) {
      results = results.filter(s => s.isTemplate === filter.isTemplate);
    }

    if (filter.isFavorite !== undefined) {
      results = results.filter(s => s.isFavorite === filter.isFavorite);
    }

    return results;
  }

  /**
   * 创建新脚本
   */
  async createScript(script: Partial<CommandScript>): Promise<CommandScript> {
    const validation = this.validateScript(script);
    if (!validation.valid) {
      throw new Error(`Invalid script: ${validation.errors.join(', ')}`);
    }

    try {
      const saved = await invoke<CommandScript>('save_script', { script });
      this.scripts.set(saved.id, saved);
      this.emit('script:created', { script: saved });
      return saved;
    } catch (error) {
      console.error('[ScriptHost] Failed to create script:', error);
      throw error;
    }
  }

  /**
   * 更新脚本
   */
  async updateScript(id: string, updates: Partial<CommandScript>): Promise<CommandScript> {
    const existing = this.scripts.get(id);
    if (!existing) {
      throw new Error(`Script ${id} not found`);
    }

    const updated = { ...existing, ...updates, id, updatedAt: new Date().toISOString() };
    const validation = this.validateScript(updated);
    if (!validation.valid) {
      throw new Error(`Invalid script: ${validation.errors.join(', ')}`);
    }

    try {
      const saved = await invoke<CommandScript>('save_script', { script: updated });
      this.scripts.set(saved.id, saved);
      this.emit('script:updated', { script: saved });
      return saved;
    } catch (error) {
      console.error('[ScriptHost] Failed to update script:', error);
      throw error;
    }
  }

  /**
   * 删除脚本
   */
  async deleteScript(id: string): Promise<void> {
    const existing = this.scripts.get(id);
    if (!existing) {
      throw new Error(`Script ${id} not found`);
    }

    try {
      await invoke('delete_script', { script_id: id });
      this.scripts.delete(id);
      this.emit('script:deleted', { scriptId: id });
    } catch (error) {
      console.error('[ScriptHost] Failed to delete script:', error);
      throw error;
    }
  }

  /**
   * 收藏/取消收藏脚本
   */
  async toggleFavorite(id: string): Promise<boolean> {
    const script = this.scripts.get(id);
    if (!script) {
      throw new Error(`Script ${id} not found`);
    }

    const updated = await this.updateScript(id, { isFavorite: !script.isFavorite });
    return updated.isFavorite;
  }

  /**
   * 复制脚本
   */
  async duplicateScript(id: string): Promise<CommandScript> {
    const original = this.scripts.get(id);
    if (!original) {
      throw new Error(`Script ${id} not found`);
    }

    const copy: Partial<CommandScript> = {
      ...original,
      id: undefined, // 后端生成新ID
      name: `${original.name} (Copy)`,
      isFavorite: false,
      usageCount: 0,
      createdAt: new Date().toISOString(),
      updatedAt: new Date().toISOString(),
    };

    return this.createScript(copy);
  }

  // ==================== 脚本验证 ====================

  validateScript(script: Partial<CommandScript>): ScriptValidationResult {
    const errors: string[] = [];
    const warnings: string[] = [];
    const parameterErrors: Record<string, string[]> = {};

    // 基本验证
    if (!script.name?.trim()) {
      errors.push('Script name is required');
    }

    if (!script.scriptType) {
      errors.push('Script type is required');
    }

    if (!script.content?.trim()) {
      errors.push('Script content is required');
    }

    // 参数验证
    if (script.parameters) {
      const paramNames = new Set<string>();
      script.parameters.forEach((param, index) => {
        if (!param.name?.trim()) {
          parameterErrors[`param_${index}`] = ['Parameter name is required'];
          errors.push(`Parameter ${index + 1}: name is required`);
        } else if (paramNames.has(param.name)) {
          parameterErrors[param.name] = ['Duplicate parameter name'];
          errors.push(`Parameter "${param.name}": duplicate name`);
        } else {
          paramNames.add(param.name);
        }

        if (param.validation) {
          try {
            new RegExp(param.validation);
          } catch {
            parameterErrors[param.name] = ['Invalid validation regex'];
            errors.push(`Parameter "${param.name}": invalid validation regex`);
          }
        }
      });
    }

    // 警告
    if (script.parameters?.length === 0) {
      warnings.push('Script has no parameters defined');
    }

    if (!script.description?.trim()) {
      warnings.push('Script description is empty');
    }

    return {
      valid: errors.length === 0,
      errors,
      warnings,
      parameterErrors,
    };
  }

  // ==================== 脚本执行 ====================

  /**
   * 执行脚本
   */
  async executeScript(context: ScriptExecutionContext): Promise<ScriptExecutionResult[]> {
    const script = this.scripts.get(context.scriptId);
    if (!script) {
      throw new Error(`Script ${context.scriptId} not found`);
    }

    const executionId = `exec-${Date.now()}-${Math.random().toString(36).slice(2, 9)}`;

    // 创建执行跟踪
    const track: ExecutionTrack = {
      id: executionId,
      scriptId: context.scriptId,
      status: 'running',
      startTime: Date.now(),
      results: [],
    };
    this.executionTracks.set(executionId, track);

    this.emit('execution:started', {
      executionId,
      scriptId: context.scriptId,
      stationIds: context.stationIds,
    });

    this.log('info', `Executing script "${script.name}" on ${context.stationIds.length} station(s)`);

    try {
      const results = await invoke<ScriptExecutionResult[]>('execute_script', {
        context: {
          ...context,
          timeout: context.timeout ?? this.config.defaultTimeout,
        },
      });

      track.results = results;
      track.status = results.some(r => r.status === 'failed') ? 'failed' : 'success';

      this.emit('execution:completed', {
        executionId,
        scriptId: context.scriptId,
        results,
      });

      // 更新脚本使用统计
      if (script) {
        script.usageCount++;
        script.lastUsedAt = new Date().toISOString();
      }

      return results;
    } catch (error) {
      track.status = 'failed';
      this.emit('execution:failed', {
        executionId,
        scriptId: context.scriptId,
        error,
      });
      this.log('error', `Script execution failed: ${error}`);
      throw error;
    }
  }

  /**
   * 取消执行
   */
  async cancelExecution(executionId: string): Promise<void> {
    const track = this.executionTracks.get(executionId);
    if (!track) {
      throw new Error(`Execution ${executionId} not found`);
    }

    try {
      await invoke('cancel_script_execution', { execution_id: executionId });
      track.status = 'cancelled';
      this.emit('execution:cancelled', { executionId });
      this.log('info', `Execution ${executionId} cancelled`);
    } catch (error) {
      console.error('[ScriptHost] Failed to cancel execution:', error);
      throw error;
    }
  }

  /**
   * 获取执行结果
   */
  getExecutionResult(executionId: string): ScriptExecutionResult[] | null {
    return this.executionTracks.get(executionId)?.results ?? null;
  }

  /**
   * 获取执行状态
   */
  getExecutionStatus(executionId: string): ScriptExecutionStatus | null {
    return this.executionTracks.get(executionId)?.status ?? null;
  }

  // ==================== 导入/导出 ====================

  /**
   * 导入脚本包
   */
  async importScripts(
    pkg: ScriptPackage,
    options?: { overwrite?: boolean; importAsCopies?: boolean }
  ): Promise<ScriptImportResult> {
    try {
      const result = await invoke<ScriptImportResult>('import_script_package', { pkg, options });

      if (result.success) {
        // 重新加载脚本列表
        await this.loadScriptsFromBackend();
        this.emit('scripts:imported', { result });
      }

      return result;
    } catch (error) {
      console.error('[ScriptHost] Failed to import scripts:', error);
      throw error;
    }
  }

  /**
   * 导出脚本包
   */
  async exportScripts(
    scriptIds: string[],
    metadata?: Partial<ScriptPackage['metadata']>
  ): Promise<ScriptPackage> {
    const scripts = scriptIds
      .map(id => this.scripts.get(id))
      .filter((s): s is CommandScript => s !== null);

    if (scripts.length === 0) {
      throw new Error('No scripts to export');
    }

    try {
      const pkg = await invoke<ScriptPackage>('export_script_package', {
        scripts,
        metadata,
      });

      this.emit('scripts:exported', { count: scripts.length });
      return pkg;
    } catch (error) {
      console.error('[ScriptHost] Failed to export scripts:', error);
      throw error;
    }
  }

  // ==================== 日志 ====================

  private log(level: 'info' | 'warning' | 'error', message: string, stationId?: string): void {
    const entry: ScriptLogEntry = {
      timestamp: new Date().toISOString(),
      level,
      message,
      stationId,
    };

    this.executionHistory.push(entry);

    // 保持历史记录在限制内
    if (this.executionHistory.length > (this.config.maxHistory ?? 100)) {
      this.executionHistory = this.executionHistory.slice(-this.config.maxHistory!);
    }

    this.emit('log', entry);
  }

  getLogs(): ScriptLogEntry[] {
    return [...this.executionHistory];
  }

  // ==================== 事件系统 ====================

  on(event: string, handler: Function): () => void {
    if (!this.listeners.has(event)) {
      this.listeners.set(event, new Set());
    }
    this.listeners.get(event)!.add(handler);

    return () => {
      this.listeners.get(event)?.delete(handler);
    };
  }

  off(event: string, handler: Function): void {
    this.listeners.get(event)?.delete(handler);
  }

  private emit(event: string, data?: unknown): void {
    this.listeners.get(event)?.forEach(handler => {
      try {
        handler(data);
      } catch (error) {
        console.error(`[ScriptHost] Event handler error for ${event}:`, error);
      }
    });

    // 通配符处理
    this.listeners.get('*')?.forEach(handler => {
      try {
        handler({ type: event, data });
      } catch (error) {
        console.error(`[ScriptHost] Wildcard handler error:`, error);
      }
    });
  }

  private setupEventForwarding(): void {
    // 转发后端事件（通过Tauri事件系统）
    // 这部分需要与后端的事件发射机制配合
  }

  // ==================== 统计信息 ====================

  getStats() {
    const scripts = this.getAllScripts();
    const templates = scripts.filter(s => s.isTemplate);
    const favorites = scripts.filter(s => s.isFavorite);

    return {
      scripts: {
        total: scripts.length,
        templates: templates.length,
        favorites: favorites.length,
        byType: this.countByType(scripts),
      },
      executions: {
        total: this.executionTracks.size,
        running: Array.from(this.executionTracks.values()).filter(t => t.status === 'running').length,
        historySize: this.executionHistory.length,
      },
    };
  }

  private countByType(scripts: CommandScript[]): Record<string, number> {
    const counts: Record<string, number> = {};
    scripts.forEach(s => {
      counts[s.scriptType] = (counts[s.scriptType] || 0) + 1;
    });
    return counts;
  }
}

// 导出单例
export const scriptHost = ScriptHost.getInstance();
```

---

## 3. ScriptEditor 组件设计

### 3.1 组件接口

```typescript
// src/plugin/script/ScriptEditor.tsx

import type {
  CommandScript,
  ScriptParameter,
  ScriptType,
  ParameterType,
  ScriptValidationResult,
} from './types';

export interface ScriptEditorProps {
  /** 要编辑的脚本，null表示创建新脚本 */
  script?: CommandScript | null;
  /** 是否以只读模式打开 */
  readOnly?: boolean;
  /** 保存回调 */
  onSave?: (script: Partial<CommandScript>) => Promise<void>;
  /** 取消回调 */
  onCancel?: () => void;
  /** 关闭回调 */
  onClose?: () => void;
  /** 是否显示模态框 */
  isModal?: boolean;
}
```

### 3.2 组件状态

```typescript
interface ScriptEditorState {
  // 表单数据
  name: string;
  description: string;
  scriptType: ScriptType;
  content: string;
  tags: string[];
  isTemplate: boolean;
  parameters: ScriptParameter[];

  // UI状态
  isDirty: boolean;
  isSaving: boolean;
  activeTab: 'general' | 'parameters' | 'advanced';

  // 验证
  validation: ScriptValidationResult | null;
  selectedParamIndex: number | null;

  // 错误
  error: string | null;
}
```

### 3.3 组件实现结构

```tsx
// src/plugin/script/ScriptEditor.tsx

import React, { useState, useEffect, useCallback, useMemo } from 'react';
import { scriptHost } from './ScriptHost';
import type {
  CommandScript,
  ScriptParameter,
  ScriptType,
  ParameterType,
  ScriptValidationResult,
} from './types';
import './ScriptEditor.css';

const SCRIPT_TYPES: { value: ScriptType; label: string }[] = [
  { value: 'shell', label: 'Shell (Bash)' },
  { value: 'powershell', label: 'PowerShell' },
  { value: 'python', label: 'Python' },
  { value: 'lua', label: 'Lua' },
  { value: 'javascript', label: 'JavaScript' },
];

const PARAM_TYPES: { value: ParameterType; label: string }[] = [
  { value: 'string', label: 'String' },
  { value: 'number', label: 'Number' },
  { value: 'boolean', label: 'Boolean' },
  { value: 'select', label: 'Select' },
];

export function ScriptEditor({
  script,
  readOnly = false,
  onSave,
  onCancel,
  onClose,
  isModal = true,
}: ScriptEditorProps) {
  // 状态初始化
  const [state, setState] = useState<ScriptEditorState>(() => initializeState(script));

  // 参数处理
  const addParameter = useCallback(() => {
    setState(prev => ({
      ...prev,
      isDirty: true,
      parameters: [
        ...prev.parameters,
        {
          name: '',
          paramType: 'string',
          defaultValue: '',
          required: false,
        },
      ],
      selectedParamIndex: prev.parameters.length,
    }));
  }, []);

  const updateParameter = useCallback((index: number, updates: Partial<ScriptParameter>) => {
    setState(prev => {
      const newParams = [...prev.parameters];
      newParams[index] = { ...newParams[index], ...updates };
      return { ...prev, parameters: newParams, isDirty: true };
    });
  }, []);

  const removeParameter = useCallback((index: number) => {
    setState(prev => ({
      ...prev,
      isDirty: true,
      parameters: prev.parameters.filter((_, i) => i !== index),
      selectedParamIndex: null,
    }));
  }, []);

  // 验证
  const validation = useMemo(() => {
    return scriptHost.validateScript({
      name: state.name,
      description: state.description,
      scriptType: state.scriptType,
      content: state.content,
      parameters: state.parameters,
    });
  }, [state.name, state.description, state.scriptType, state.content, state.parameters]);

  // 保存处理
  const handleSave = useCallback(async () => {
    if (!validation.valid) return;

    setState(prev => ({ ...prev, isSaving: true, error: null }));

    try {
      const scriptData: Partial<CommandScript> = {
        name: state.name,
        description: state.description,
        scriptType: state.scriptType,
        content: state.content,
        tags: state.tags,
        isTemplate: state.isTemplate,
        parameters: state.parameters,
      };

      if (onSave) {
        await onSave(scriptData);
      } else if (script?.id) {
        await scriptHost.updateScript(script.id, scriptData);
      } else {
        await scriptHost.createScript(scriptData);
      }

      onClose?.();
    } catch (error) {
      setState(prev => ({
        ...prev,
        isSaving: false,
        error: error instanceof Error ? error.message : 'Save failed',
      }));
    }
  }, [validation, state, script, onSave, onClose]);

  // 渲染参数编辑器
  const renderParameterEditor = () => (
    <div className="parameter-editor">
      <div className="parameter-list">
        {state.parameters.map((param, index) => (
          <div key={index} className="parameter-card">
            <div className="parameter-header">
              <span className="parameter-index">#{index + 1}</span>
              <button
                className="btn-icon"
                onClick={() => removeParameter(index)}
                disabled={readOnly}
              >
                ×
              </button>
            </div>
            <div className="parameter-fields">
              <input
                type="text"
                placeholder="Parameter name"
                value={param.name}
                onChange={(e) => updateParameter(index, { name: e.target.value })}
                disabled={readOnly}
              />
              <select
                value={param.paramType}
                onChange={(e) => updateParameter(index, { paramType: e.target.value as ParameterType })}
                disabled={readOnly}
              >
                {PARAM_TYPES.map(t => (
                  <option key={t.value} value={t.value}>{t.label}</option>
                ))}
              </select>
              <input
                type="text"
                placeholder="Default value"
                value={param.defaultValue}
                onChange={(e) => updateParameter(index, { defaultValue: e.target.value })}
                disabled={readOnly}
              />
              {param.paramType === 'select' && (
                <input
                  type="text"
                  placeholder="Options (comma-separated)"
                  value={param.options?.join(', ') || ''}
                  onChange={(e) => updateParameter(index, {
                    options: e.target.value.split(',').map(s => s.trim())
                  })}
                  disabled={readOnly}
                />
              )}
              <label className="checkbox-label">
                <input
                  type="checkbox"
                  checked={param.required}
                  onChange={(e) => updateParameter(index, { required: e.target.checked })}
                  disabled={readOnly}
                />
                Required
              </label>
              <input
                type="text"
                placeholder="Validation regex (optional)"
                value={param.validation || ''}
                onChange={(e) => updateParameter(index, { validation: e.target.value })}
                disabled={readOnly}
              />
              <textarea
                placeholder="Description (optional)"
                value={param.description || ''}
                onChange={(e) => updateParameter(index, { description: e.target.value })}
                disabled={readOnly}
              />
            </div>
          </div>
        ))}
      </div>
      <button
        className="btn-secondary"
        onClick={addParameter}
        disabled={readOnly}
      >
        + Add Parameter
      </button>
    </div>
  );

  // 渲染内容编辑器（带语法高亮占位）
  const renderContentEditor = () => (
    <div className="content-editor">
      <div className="editor-toolbar">
        <span className="script-type-badge">{state.scriptType}</span>
        <span className="char-count">{state.content.length} characters</span>
      </div>
      <textarea
        className="code-textarea"
        value={state.content}
        onChange={(e) => setState(prev => ({
          ...prev,
          content: e.target.value,
          isDirty: true
        }))}
        placeholder={getPlaceholder(state.scriptType)}
        disabled={readOnly}
        spellCheck={false}
      />
      {state.parameters.length > 0 && (
        <div className="parameter-hints">
          <strong>Available parameters:</strong>
          {state.parameters.map(p => (
            <code key={p.name}>{`{{${p.name}}}`}</code>
          ))}
        </div>
      )}
    </div>
  );

  // 主渲染
  return (
    <div className={`script-editor ${isModal ? 'modal' : ''}`}>
      <div className="editor-header">
        <h2>{script ? (readOnly ? 'View Script' : 'Edit Script') : 'New Script'}</h2>
        <button className="btn-close" onClick={onClose}>×</button>
      </div>

      <div className="editor-tabs">
        <button
          className={state.activeTab === 'general' ? 'active' : ''}
          onClick={() => setState(prev => ({ ...prev, activeTab: 'general' }))}
        >
          General
        </button>
        <button
          className={state.activeTab === 'parameters' ? 'active' : ''}
          onClick={() => setState(prev => ({ ...prev, activeTab: 'parameters' }))}
        >
          Parameters ({state.parameters.length})
        </button>
        <button
          className={state.activeTab === 'advanced' ? 'active' : ''}
          onClick={() => setState(prev => ({ ...prev, activeTab: 'advanced' }))}
        >
          Advanced
        </button>
      </div>

      <div className="editor-body">
        {state.activeTab === 'general' && (
          <div className="general-tab">
            <div className="form-group">
              <label>Name *</label>
              <input
                type="text"
                value={state.name}
                onChange={(e) => setState(prev => ({ ...prev, name: e.target.value, isDirty: true }))}
                disabled={readOnly}
                placeholder="Enter script name"
              />
            </div>

            <div className="form-group">
              <label>Description</label>
              <textarea
                value={state.description}
                onChange={(e) => setState(prev => ({ ...prev, description: e.target.value, isDirty: true }))}
                disabled={readOnly}
                placeholder="What does this script do?"
              />
            </div>

            <div className="form-row">
              <div className="form-group">
                <label>Script Type *</label>
                <select
                  value={state.scriptType}
                  onChange={(e) => setState(prev => ({ ...prev, scriptType: e.target.value as ScriptType, isDirty: true }))}
                  disabled={readOnly}
                >
                  {SCRIPT_TYPES.map(t => (
                    <option key={t.value} value={t.value}>{t.label}</option>
                  ))}
                </select>
              </div>

              <div className="form-group">
                <label>Tags</label>
                <input
                  type="text"
                  value={state.tags.join(', ')}
                  onChange={(e) => setState(prev => ({ ...prev, tags: e.target.value.split(',').map(s => s.trim()).filter(Boolean), isDirty: true }))}
                  disabled={readOnly}
                  placeholder="Comma-separated tags"
                />
              </div>
            </div>

            <div className="form-group checkbox-group">
              <label className="checkbox-label">
                <input
                  type="checkbox"
                  checked={state.isTemplate}
                  onChange={(e) => setState(prev => ({ ...prev, isTemplate: e.target.checked, isDirty: true }))}
                  disabled={readOnly}
                />
                Template (can be used as base for other scripts)
              </label>
            </div>

            {renderContentEditor()}
          </div>
        )}

        {state.activeTab === 'parameters' && renderParameterEditor()}

        {state.activeTab === 'advanced' && (
          <div className="advanced-tab">
            <div className="form-group">
              <label>Metadata</label>
              <div className="metadata-display">
                {script && (
                  <>
                    <p><strong>ID:</strong> <code>{script.id}</code></p>
                    <p><strong>Version:</strong> {script.version}</p>
                    <p><strong>Created:</strong> {script.createdAt}</p>
                    <p><strong>Updated:</strong> {script.updatedAt}</p>
                    <p><strong>Created By:</strong> {script.createdBy}</p>
                    <p><strong>Usage Count:</strong> {script.usageCount}</p>
                  </>
                )}
              </div>
            </div>
          </div>
        )}
      </div>

      {state.error && (
        <div className="editor-error">
          {state.error}
        </div>
      )}

      <div className="editor-footer">
        {!readOnly && (
          <>
            <button
              className="btn-secondary"
              onClick={onCancel || onClose}
            >
              Cancel
            </button>
            <button
              className="btn-primary"
              onClick={handleSave}
              disabled={!validation.valid || state.isSaving}
            >
              {state.isSaving ? 'Saving...' : 'Save'}
            </button>
          </>
        )}
        {readOnly && (
          <button className="btn-secondary" onClick={onClose}>
            Close
          </button>
        )}
      </div>
    </div>
  );
}

function initializeState(script: CommandScript | null | undefined): ScriptEditorState {
  if (script) {
    return {
      name: script.name,
      description: script.description,
      scriptType: script.scriptType,
      content: script.content,
      tags: [...script.tags],
      isTemplate: script.isTemplate,
      parameters: [...script.parameters],
      isDirty: false,
      isSaving: false,
      activeTab: 'general',
      validation: null,
      selectedParamIndex: null,
      error: null,
    };
  }

  return {
    name: '',
    description: '',
    scriptType: 'shell',
    content: '',
    tags: [],
    isTemplate: false,
    parameters: [],
    isDirty: false,
    isSaving: false,
    activeTab: 'general',
    validation: null,
    selectedParamIndex: null,
    error: null,
  };
}

function getPlaceholder(scriptType: ScriptType): string {
  const placeholders: Record<ScriptType, string> = {
    shell: '#!/bin/bash\necho "Hello, {{name}}!"',
    powershell: 'Write-Host "Hello, {{name}}!"',
    python: '#!/usr/bin/env python3\nprint(f"Hello, {name}!")',
    lua: 'print("Hello, " .. name .. "!")',
    javascript: 'console.log(`Hello, ${name}!`);',
  };
  return placeholders[scriptType];
}
```

---

## 4. ScriptList 组件设计

### 4.1 组件接口

```typescript
// src/plugin/script/ScriptList.tsx

import type {
  CommandScript,
  ScriptFilter,
  ScriptExecutionContext,
} from './types';

export interface ScriptListProps {
  /** 是否显示加载状态 */
  loading?: boolean;
  /** 加载完成回调 */
  onLoad?: () => Promise<void>;
  /** 脚本选中回调 */
  onSelectScript?: (script: CommandScript) => void;
  /** 编辑脚本回调 */
  onEditScript?: (script: CommandScript) => void;
  /** 执行脚本回调 */
  onExecuteScript?: (script: CommandScript) => void;
  /** 删除脚本回调 */
  onDeleteScript?: (script: CommandScript) => void;
  /** 收藏回调 */
  onToggleFavorite?: (scriptId: string) => void;
  /** 复制脚本回调 */
  onDuplicateScript?: (script: CommandScript) => void;
  /** 导入回调 */
  onImport?: () => void;
  /** 导出回调 */
  onExport?: (scripts: CommandScript[]) => void;
}
```

### 4.2 组件实现结构

```tsx
// src/plugin/script/ScriptList.tsx

import React, { useState, useEffect, useCallback, useMemo } from 'react';
import { scriptHost } from './ScriptHost';
import type {
  CommandScript,
  ScriptFilter,
  ScriptType,
  ScriptCategory,
} from './types';
import './ScriptList.css';

type SortField = 'name' | 'updatedAt' | 'usageCount' | 'createdAt';
type SortOrder = 'asc' | 'desc';

const SCRIPT_TYPE_FILTERS: { value: ScriptType | ''; label: string }[] = [
  { value: '', label: 'All Types' },
  { value: 'shell', label: 'Shell' },
  { value: 'powershell', label: 'PowerShell' },
  { value: 'python', label: 'Python' },
  { value: 'lua', label: 'Lua' },
  { value: 'javascript', label: 'JavaScript' },
];

const SCRIPT_CATEGORIES: { value: ScriptCategory | ''; label: string }[] = [
  { value: '', label: 'All Categories' },
  { value: 'system', label: 'System' },
  { value: 'network', label: 'Network' },
  { value: 'security', label: 'Security' },
  { value: 'monitoring', label: 'Monitoring' },
  { value: 'automation', label: 'Automation' },
  { value: 'backup', label: 'Backup' },
  { value: 'diagnostics', label: 'Diagnostics' },
  { value: 'custom', label: 'Custom' },
];

export function ScriptList({
  loading = false,
  onLoad,
  onSelectScript,
  onEditScript,
  onExecuteScript,
  onDeleteScript,
  onToggleFavorite,
  onDuplicateScript,
  onImport,
  onExport,
}: ScriptListProps) {
  // 状态
  const [scripts, setScripts] = useState<CommandScript[]>([]);
  const [isLoading, setIsLoading] = useState(loading);
  const [error, setError] = useState<string | null>(null);

  // 筛选状态
  const [filter, setFilter] = useState<ScriptFilter>({
    search: '',
    scriptType: undefined,
    tags: [],
    isTemplate: undefined,
    isFavorite: undefined,
  });

  // 排序状态
  const [sortField, setSortField] = useState<SortField>('updatedAt');
  const [sortOrder, setSortOrder] = useState<SortOrder>('desc');

  // 选择状态
  const [selectedScripts, setSelectedScripts] = useState<Set<string>>(new Set());
  const [viewMode, setViewMode] = useState<'grid' | 'list'>('grid');

  // 加载数据
  const loadScripts = useCallback(async () => {
    setIsLoading(true);
    setError(null);

    try {
      if (onLoad) {
        await onLoad();
      }
      setScripts(scriptHost.getAllScripts());
    } catch (err) {
      setError(err instanceof Error ? err.message : 'Failed to load scripts');
    } finally {
      setIsLoading(false);
    }
  }, [onLoad]);

  useEffect(() => {
    loadScripts();
  }, [loadScripts]);

  // 筛选和排序
  const filteredScripts = useMemo(() => {
    let result = scriptHost.filterScripts(filter);

    // 排序
    result.sort((a, b) => {
      let comparison = 0;
      switch (sortField) {
        case 'name':
          comparison = a.name.localeCompare(b.name);
          break;
        case 'updatedAt':
          comparison = new Date(a.updatedAt).getTime() - new Date(b.updatedAt).getTime();
          break;
        case 'usageCount':
          comparison = a.usageCount - b.usageCount;
          break;
        case 'createdAt':
          comparison = new Date(a.createdAt).getTime() - new Date(b.createdAt).getTime();
          break;
      }
      return sortOrder === 'asc' ? comparison : -comparison;
    });

    return result;
  }, [filter, sortField, sortOrder]);

  // 选择处理
  const toggleScriptSelection = useCallback((scriptId: string) => {
    setSelectedScripts(prev => {
      const next = new Set(prev);
      if (next.has(scriptId)) {
        next.delete(scriptId);
      } else {
        next.add(scriptId);
      }
      return next;
    });
  }, []);

  const selectAll = useCallback(() => {
    if (selectedScripts.size === filteredScripts.length) {
      setSelectedScripts(new Set());
    } else {
      setSelectedScripts(new Set(filteredScripts.map(s => s.id)));
    }
  }, [filteredScripts, selectedScripts]);

  const handleToggleFavorite = useCallback(async (scriptId: string) => {
    try {
      await scriptHost.toggleFavorite(scriptId);
      setScripts(scriptHost.getAllScripts());
      onToggleFavorite?.(scriptId);
    } catch (err) {
      setError(err instanceof Error ? err.message : 'Failed to toggle favorite');
    }
  }, [onToggleFavorite]);

  const handleDelete = useCallback(async (script: CommandScript) => {
    if (!confirm(`Delete "${script.name}"? This cannot be undone.`)) {
      return;
    }

    try {
      await scriptHost.deleteScript(script.id);
      setScripts(scriptHost.getAllScripts());
      onDeleteScript?.(script);
    } catch (err) {
      setError(err instanceof Error ? err.message : 'Failed to delete script');
    }
  }, [onDeleteScript]);

  const handleDuplicate = useCallback(async (script: CommandScript) => {
    try {
      const newScript = await scriptHost.duplicateScript(script.id);
      setScripts(scriptHost.getAllScripts());
      onDuplicateScript?.(newScript);
    } catch (err) {
      setError(err instanceof Error ? err.message : 'Failed to duplicate script');
    }
  }, [onDuplicateScript]);

  const handleExport = useCallback(() => {
    const selected = filteredScripts.filter(s => selectedScripts.has(s.id));
    onExport?.(selected.length > 0 ? selected : scripts);
  }, [selectedScripts, filteredScripts, scripts, onExport]);

  // 渲染脚本卡片
  const renderScriptCard = (script: CommandScript) => (
    <div
      key={script.id}
      className={`script-card ${selectedScripts.has(script.id) ? 'selected' : ''}`}
    >
      <div className="card-header">
        <input
          type="checkbox"
          checked={selectedScripts.has(script.id)}
          onChange={() => toggleScriptSelection(script.id)}
        />
        <span className="script-type-badge">{script.scriptType}</span>
        <button
          className={`favorite-btn ${script.isFavorite ? 'active' : ''}`}
          onClick={() => handleToggleFavorite(script.id)}
        >
          ★
        </button>
      </div>

      <div className="card-body" onClick={() => onSelectScript?.(script)}>
        <h3 className="script-name">{script.name}</h3>
        <p className="script-description">{script.description || 'No description'}</p>

        <div className="script-tags">
          {script.tags.slice(0, 3).map(tag => (
            <span key={tag} className="tag">{tag}</span>
          ))}
          {script.tags.length > 3 && (
            <span className="tag more">+{script.tags.length - 3}</span>
          )}
        </div>

        <div className="script-meta">
          <span className="meta-item">
            <i className="icon-usage" />
            {script.usageCount} uses
          </span>
          <span className="meta-item">
            <i className="icon-time" />
            {formatRelativeTime(script.updatedAt)}
          </span>
        </div>
      </div>

      <div className="card-actions">
        <button
          className="btn-icon"
          title="Execute"
          onClick={() => onExecuteScript?.(script)}
        >
          ▶
        </button>
        <button
          className="btn-icon"
          title="Edit"
          onClick={() => onEditScript?.(script)}
        >
          ✎
        </button>
        <button
          className="btn-icon"
          title="Duplicate"
          onClick={() => handleDuplicate(script)}
        >
          ⧉
        </button>
        <button
          className="btn-icon danger"
          title="Delete"
          onClick={() => handleDelete(script)}
        >
          🗑
        </button>
      </div>
    </div>
  );

  // 渲染列表头
  const renderListHeader = () => (
    <div className="list-header">
      <div className="col-checkbox">
        <input
          type="checkbox"
          checked={selectedScripts.size === filteredScripts.length && filteredScripts.length > 0}
          onChange={selectAll}
        />
      </div>
      <div className="col-name" onClick={() => toggleSort('name')}>
        Name {sortField === 'name' && (sortOrder === 'asc' ? '↑' : '↓')}
      </div>
      <div className="col-type">Type</div>
      <div className="col-tags">Tags</div>
      <div className="col-uses" onClick={() => toggleSort('usageCount')}>
        Uses {sortField === 'usageCount' && (sortOrder === 'asc' ? '↑' : '↓')}
      </div>
      <div className="col-updated" onClick={() => toggleSort('updatedAt')}>
        Updated {sortField === 'updatedAt' && (sortOrder === 'asc' ? '↑' : '↓')}
      </div>
      <div className="col-actions">Actions</div>
    </div>
  );

  return (
    <div className="script-list-container">
      {/* 工具栏 */}
      <div className="list-toolbar">
        <div className="toolbar-left">
          <button className="btn-primary" onClick={() => {/* TODO: Open editor for new script */}}>
            + New Script
          </button>
          <button className="btn-secondary" onClick={onImport}>
            📥 Import
          </button>
          <button
            className="btn-secondary"
            onClick={handleExport}
            disabled={scripts.length === 0}
          >
            📤 Export
          </button>
        </div>

        <div className="toolbar-right">
          <div className="search-box">
            <input
              type="text"
              placeholder="Search scripts..."
              value={filter.search || ''}
              onChange={(e) => setFilter(prev => ({ ...prev, search: e.target.value }))}
            />
          </div>
          <select
            value={filter.scriptType || ''}
            onChange={(e) => setFilter(prev => ({
              ...prev,
              scriptType: e.target.value as ScriptType || undefined
            }))}
          >
            {SCRIPT_TYPE_FILTERS.map(f => (
              <option key={f.value} value={f.value}>{f.label}</option>
            ))}
          </select>
          <div className="view-toggle">
            <button
              className={viewMode === 'grid' ? 'active' : ''}
              onClick={() => setViewMode('grid')}
            >
              ▦
            </button>
            <button
              className={viewMode === 'list' ? 'active' : ''}
              onClick={() => setViewMode('list')}
            >
              ☰
            </button>
          </div>
        </div>
      </div>

      {/* 筛选标签栏 */}
      <div className="filter-bar">
        <label className="filter-checkbox">
          <input
            type="checkbox"
            checked={filter.isFavorite === true}
            onChange={(e) => setFilter(prev => ({
              ...prev,
              isFavorite: e.target.checked ? true : undefined
            }))}
          />
          ★ Favorites
        </label>
        <label className="filter-checkbox">
          <input
            type="checkbox"
            checked={filter.isTemplate === true}
            onChange={(e) => setFilter(prev => ({
              ...prev,
              isTemplate: e.target.checked ? true : undefined
            }))}
          />
          📋 Templates
        </label>
      </div>

      {/* 错误提示 */}
      {error && (
        <div className="list-error">
          {error}
          <button onClick={() => setError(null)}>×</button>
        </div>
      )}

      {/* 加载状态 */}
      {isLoading && (
        <div className="list-loading">
          <div className="spinner" />
          Loading scripts...
        </div>
      )}

      {/* 脚本列表/网格 */}
      {!isLoading && (
        <div className={`script-list ${viewMode}`}>
          {filteredScripts.length === 0 ? (
            <div className="empty-state">
              <p>No scripts found</p>
              <button className="btn-primary" onClick={() => {/* TODO */}}>
                Create your first script
              </button>
            </div>
          ) : viewMode === 'grid' ? (
            <div className="script-grid">
              {filteredScripts.map(renderScriptCard)}
            </div>
          ) : (
            <div className="script-table">
              {renderListHeader()}
              <div className="table-body">
                {filteredScripts.map(script => (
                  <div key={script.id} className="table-row">
                    {/* ... 列表模式渲染 ... */}
                  </div>
                ))}
              </div>
            </div>
          )}
        </div>
      )}

      {/* 底部状态栏 */}
      <div className="list-footer">
        <span>
          {selectedScripts.size > 0
            ? `${selectedScripts.size} selected`
            : `${filteredScripts.length} scripts`}
        </span>
      </div>
    </div>
  );
}
```

---

## 5. ScriptRunner 组件设计

### 5.1 组件接口

```typescript
// src/plugin/script/ScriptRunner.tsx

import type {
  CommandScript,
  ScriptExecutionContext,
  ScriptExecutionResult,
  ScriptLogEntry,
} from './types';

export interface ScriptRunnerProps {
  /** 要执行的脚本 */
  script: CommandScript;
  /** 可用的目标工作站 */
  targets?: Array<{ id: string; name: string; status?: string }>;
  /** 加载目标列表的回调 */
  onLoadTargets?: () => Promise<Array<{ id: string; name: string; status?: string }>>;
  /** 执行完成回调 */
  onExecutionComplete?: (results: ScriptExecutionResult[]) => void;
  /** 取消回调 */
  onCancel?: () => void;
  /** 关闭回调 */
  onClose?: () => void;
}
```

### 5.2 组件实现结构

```tsx
// src/plugin/script/ScriptRunner.tsx

import React, { useState, useEffect, useCallback, useMemo } from 'react';
import { scriptHost } from './ScriptHost';
import type {
  CommandScript,
  ScriptExecutionContext,
  ScriptExecutionResult,
  ScriptLogEntry,
  ScriptParameter,
} from './types';
import './ScriptRunner.css';

type ExecutionPhase = 'configure' | 'running' | 'completed';

export function ScriptRunner({
  script,
  targets: initialTargets,
  onLoadTargets,
  onExecutionComplete,
  onCancel,
  onClose,
}: ScriptRunnerProps) {
  // 执行阶段
  const [phase, setPhase] = useState<ExecutionPhase>('configure');

  // 目标选择
  const [targets, setTargets] = useState(initialTargets ?? []);
  const [selectedTargets, setSelectedTargets] = useState<Set<string>>(new Set());
  const [isLoadingTargets, setIsLoadingTargets] = useState(false);

  // 参数值
  const [parameterValues, setParameterValues] = useState<Record<string, string>>(() => {
    const values: Record<string, string> = {};
    script.parameters.forEach(p => {
      values[p.name] = p.defaultValue;
    });
    return values;
  });

  // 执行选项
  const [timeout, setTimeout] = useState(60000);
  const [continueOnError, setContinueOnError] = useState(true);

  // 执行状态
  const [isExecuting, setIsExecuting] = useState(false);
  const [results, setResults] = useState<ScriptExecutionResult[]>([]);
  const [logs, setLogs] = useState<ScriptLogEntry[]>([]);
  const [currentExecutionId, setCurrentExecutionId] = useState<string | null>(null);

  // 加载目标
  useEffect(() => {
    if (initialTargets) return;

    const loadTargets = async () => {
      setIsLoadingTargets(true);
      try {
        const loaded = onLoadTargets
          ? await onLoadTargets()
          : await scriptHost.getAvailableTargets?.() ?? [];
        setTargets(loaded);
      } catch (err) {
        console.error('Failed to load targets:', err);
      } finally {
        setIsLoadingTargets(false);
      }
    };

    loadTargets();
  }, [initialTargets, onLoadTargets]);

  // 订阅日志事件
  useEffect(() => {
    const unsubscribe = scriptHost.on('log', (entry: ScriptLogEntry) => {
      setLogs(prev => [...prev, entry]);
    });

    return unsubscribe;
  }, []);

  // 全选/取消全选
  const selectAllTargets = useCallback(() => {
    if (selectedTargets.size === targets.length) {
      setSelectedTargets(new Set());
    } else {
      setSelectedTargets(new Set(targets.map(t => t.id)));
    }
  }, [targets, selectedTargets]);

  // 切换目标选择
  const toggleTarget = useCallback((targetId: string) => {
    setSelectedTargets(prev => {
      const next = new Set(prev);
      if (next.has(targetId)) {
        next.delete(targetId);
      } else {
        next.add(targetId);
      }
      return next;
    });
  }, []);

  // 执行脚本
  const handleExecute = useCallback(async () => {
    if (selectedTargets.size === 0) {
      alert('Please select at least one target');
      return;
    }

    setPhase('running');
    setIsExecuting(true);
    setResults([]);
    setLogs([]);

    // 构建参数化内容
    let content = script.content;
    Object.entries(parameterValues).forEach(([name, value]) => {
      content = content.replace(new RegExp(`\\{\\{${name}\\}\\}`, 'g'), value);
    });

    const context: ScriptExecutionContext = {
      scriptId: script.id,
      stationIds: Array.from(selectedTargets),
      parameters: parameterValues,
      timeout,
      continueOnError,
    };

    try {
      const executionResults = await scriptHost.executeScript(context);
      setResults(executionResults);
      setPhase('completed');
      onExecutionComplete?.(executionResults);
    } catch (err) {
      setLogs(prev => [...prev, {
        timestamp: new Date().toISOString(),
        level: 'error',
        message: `Execution failed: ${err}`,
      }]);
    } finally {
      setIsExecuting(false);
    }
  }, [selectedTargets, parameterValues, script, timeout, continueOnError, onExecutionComplete]);

  // 取消执行
  const handleCancel = useCallback(async () => {
    if (currentExecutionId) {
      await scriptHost.cancelExecution(currentExecutionId);
    }
    setIsExecuting(false);
    setPhase('configure');
    onCancel?.();
  }, [currentExecutionId, onCancel]);

  // 重新执行
  const handleRerun = useCallback(() => {
    setPhase('configure');
    setResults([]);
    setLogs([]);
  }, []);

  // 状态统计
  const statusCounts = useMemo(() => {
    const counts = { success: 0, failed: 0, running: 0, pending: 0 };
    results.forEach(r => {
      counts[r.status] = (counts[r.status] || 0) + 1;
    });
    return counts;
  }, [results]);

  // 渲染参数输入
  const renderParameterInputs = () => (
    <div className="parameter-inputs">
      <h4>Parameters</h4>
      {script.parameters.length === 0 ? (
        <p className="no-params">This script has no parameters</p>
      ) : (
        script.parameters.map(param => (
          <div key={param.name} className="param-row">
            <label>
              {param.name}
              {param.required && <span className="required">*</span>}
            </label>
            {param.paramType === 'boolean' ? (
              <select
                value={parameterValues[param.name]}
                onChange={(e) => setParameterValues(prev => ({
                  ...prev,
                  [param.name]: e.target.value
                }))}
              >
                <option value="true">True</option>
                <option value="false">False</option>
              </select>
            ) : param.paramType === 'select' && param.options ? (
              <select
                value={parameterValues[param.name]}
                onChange={(e) => setParameterValues(prev => ({
                  ...prev,
                  [param.name]: e.target.value
                }))}
              >
                {param.options.map(opt => (
                  <option key={opt} value={opt}>{opt}</option>
                ))}
              </select>
            ) : (
              <input
                type={param.paramType === 'number' ? 'number' : 'text'}
                value={parameterValues[param.name]}
                onChange={(e) => setParameterValues(prev => ({
                  ...prev,
                  [param.name]: e.target.value
                }))}
                placeholder={param.defaultValue || `Enter ${param.name}`}
              />
            )}
            {param.description && (
              <span className="param-hint">{param.description}</span>
            )}
          </div>
        ))
      )}
    </div>
  );

  // 渲染目标选择
  const renderTargetSelection = () => (
    <div className="target-selection">
      <div className="target-header">
        <h4>Target Stations ({selectedTargets.size} selected)</h4>
        <div className="target-actions">
          <button className="btn-link" onClick={selectAllTargets}>
            {selectedTargets.size === targets.length ? 'Deselect All' : 'Select All'}
          </button>
        </div>
      </div>
      <div className="target-list">
        {isLoadingTargets ? (
          <div className="loading-targets">Loading targets...</div>
        ) : targets.length === 0 ? (
          <div className="no-targets">No available targets</div>
        ) : (
          targets.map(target => (
            <label
              key={target.id}
              className={`target-item ${target.status !== 'online' ? 'offline' : ''}`}
            >
              <input
                type="checkbox"
                checked={selectedTargets.has(target.id)}
                onChange={() => toggleTarget(target.id)}
                disabled={target.status !== 'online'}
              />
              <span className="target-name">{target.name}</span>
              <span className={`target-status ${target.status || 'unknown'}`}>
                {target.status || 'unknown'}
              </span>
            </label>
          ))
        )}
      </div>
    </div>
  );

  // 渲染执行选项
  const renderExecutionOptions = () => (
    <div className="execution-options">
      <h4>Execution Options</h4>
      <div className="option-row">
        <label>Timeout (ms)</label>
        <input
          type="number"
          value={timeout}
          onChange={(e) => setTimeout(Number(e.target.value))}
          min={1000}
          max={300000}
          step={1000}
        />
      </div>
      <div className="option-row">
        <label className="checkbox-label">
          <input
            type="checkbox"
            checked={continueOnError}
            onChange={(e) => setContinueOnError(e.target.checked)}
          />
          Continue on error
        </label>
      </div>
    </div>
  );

  // 渲染结果面板
  const renderResults = () => (
    <div className="execution-results">
      <div className="results-header">
        <h4>Results</h4>
        <div className="result-summary">
          {statusCounts.success > 0 && (
            <span className="badge success">{statusCounts.success} succeeded</span>
          )}
          {statusCounts.failed > 0 && (
            <span className="badge failed">{statusCounts.failed} failed</span>
          )}
          {statusCounts.running > 0 && (
            <span className="badge running">{statusCounts.running} running</span>
          )}
        </div>
      </div>

      <div className="results-list">
        {results.map((result, index) => (
          <div key={index} className={`result-item ${result.status}`}>
            <div className="result-header">
              <span className="station-id">{result.scriptId}</span>
              <span className={`status-badge ${result.status}`}>
                {result.status}
              </span>
            </div>
            {result.stdout && (
              <pre className="result-stdout">{result.stdout}</pre>
            )}
            {result.stderr && (
              <pre className="result-stderr">{result.stderr}</pre>
            )}
            {result.error && (
              <div className="result-error">{result.error}</div>
            )}
            <div className="result-meta">
              {result.durationMs && <span>Duration: {result.durationMs}ms</span>}
              {result.exitCode !== null && <span>Exit code: {result.exitCode}</span>}
            </div>
          </div>
        ))}
      </div>
    </div>
  );

  // 渲染日志面板
  const renderLogs = () => (
    <div className="execution-logs">
      <div className="logs-header">
        <h4>Logs</h4>
        <button
          className="btn-link"
          onClick={() => setLogs([])}
        >
          Clear
        </button>
      </div>
      <div className="logs-list">
        {logs.length === 0 ? (
          <div className="no-logs">No log entries</div>
        ) : (
          logs.map((log, index) => (
            <div key={index} className={`log-entry ${log.level}`}>
              <span className="log-time">
                {new Date(log.timestamp).toLocaleTimeString()}
              </span>
              <span className={`log-level ${log.level}`}>
                {log.level.toUpperCase()}
              </span>
              <span className="log-message">{log.message}</span>
            </div>
          ))
        )}
      </div>
    </div>
  );

  return (
    <div className="script-runner">
      <div className="runner-header">
        <h2>Execute: {script.name}</h2>
        <button className="btn-close" onClick={onClose}>×</button>
      </div>

      <div className="runner-tabs">
        <button
          className={phase === 'configure' ? 'active' : ''}
          disabled={phase === 'running'}
        >
          Configure
        </button>
        <button
          className={phase === 'running' ? 'active' : ''}
          disabled={phase !== 'running'}
        >
          {isExecuting ? 'Running...' : 'Running'}
        </button>
        <button
          className={phase === 'completed' ? 'active' : ''}
          disabled={phase !== 'completed'}
        >
          Results
        </button>
      </div>

      <div className="runner-body">
        {phase === 'configure' && (
          <div className="configure-panel">
            {renderParameterInputs()}
            {renderTargetSelection()}
            {renderExecutionOptions()}
          </div>
        )}

        {phase === 'running' && (
          <div className="running-panel">
            <div className="running-indicator">
              <div className="spinner" />
              <p>Executing script on {selectedTargets.size} target(s)...</p>
            </div>
            {renderLogs()}
          </div>
        )}

        {phase === 'completed' && (
          <div className="completed-panel">
            {renderResults()}
            {renderLogs()}
          </div>
        )}
      </div>

      <div className="runner-footer">
        {phase === 'configure' && (
          <>
            <button className="btn-secondary" onClick={onClose}>
              Cancel
            </button>
            <button
              className="btn-primary"
              onClick={handleExecute}
              disabled={selectedTargets.size === 0 || isExecuting}
            >
              Execute on {selectedTargets.size} Target(s)
            </button>
          </>
        )}

        {phase === 'running' && (
          <button className="btn-danger" onClick={handleCancel}>
            Cancel Execution
          </button>
        )}

        {phase === 'completed' && (
          <>
            <button className="btn-secondary" onClick={onClose}>
              Close
            </button>
            <button className="btn-secondary" onClick={handleRerun}>
              Run Again
            </button>
          </>
        )}
      </div>
    </div>
  );
}
```

---

## 6. ScriptMarketplace 组件设计

### 6.1 组件接口

```typescript
// src/plugin/script/ScriptMarketplace.tsx

import type {
  ScriptPackage,
  ScriptPackageMetadata,
  ScriptImportResult,
  CommandScript,
} from './types';

export interface ScriptMarketplaceProps {
  /** 当前模式 */
  mode?: 'import' | 'export';
  /** 预选的脚本（用于导出） */
  preselectedScripts?: CommandScript[];
  /** 导入完成回调 */
  onImportComplete?: (result: ScriptImportResult) => void;
  /** 导出完成回调 */
  onExportComplete?: (pkg: ScriptPackage) => void;
  /** 关闭回调 */
  onClose?: () => void;
}
```

### 6.2 组件实现结构

```tsx
// src/plugin/script/ScriptMarketplace.tsx

import React, { useState, useCallback, useMemo } from 'react';
import { scriptHost } from './ScriptHost';
import type {
  ScriptPackage,
  ScriptPackageMetadata,
  ScriptImportResult,
  CommandScript,
  ScriptSource,
} from './types';
import './ScriptMarketplace.css';

const MOCK_REMOTE_SOURCE: ScriptSource = {
  id: 'remote-cc-marketplace',
  name: 'CC Script Marketplace',
  url: 'https://marketplace.example.com/api/v1',
  type: 'official',
  enabled: true,
  priority: 1,
};

const MOCK_LOCAL_SOURCE: ScriptSource = {
  id: 'local-import',
  name: 'Local File',
  url: '',
  type: 'local',
  enabled: true,
};

export function ScriptMarketplace({
  mode: initialMode = 'import',
  preselectedScripts = [],
  onImportComplete,
  onExportComplete,
  onClose,
}: ScriptMarketplaceProps) {
  // 当前模式
  const [mode, setMode] = useState<'import' | 'export'>(initialMode);

  // 导入状态
  const [importSource, setImportSource] = useState<ScriptSource | null>(null);
  const [availablePackages, setAvailablePackages] = useState<ScriptPackage[]>([]);
  const [selectedPackage, setSelectedPackage] = useState<ScriptPackage | null>(null);
  const [isLoadingPackages, setIsLoadingPackages] = useState(false);
  const [importResult, setImportResult] = useState<ScriptImportResult | null>(null);

  // 导出状态
  const [scriptsToExport, setScriptsToExport] = useState<CommandScript[]>(preselectedScripts);
  const [exportMetadata, setExportMetadata] = useState<Partial<ScriptPackageMetadata>>({
    name: '',
    author: '',
    description: '',
    tags: [],
    categories: [],
    license: 'MIT',
  });

  // 错误状态
  const [error, setError] = useState<string | null>(null);

  // 从远程加载包列表
  const loadRemotePackages = useCallback(async () => {
    setIsLoadingPackages(true);
    setError(null);

    try {
      // 模拟从远程API加载
      const packages = await fetchRemotePackages(importSource!.url);
      setAvailablePackages(packages);
    } catch (err) {
      setError(err instanceof Error ? err.message : 'Failed to load packages');
    } finally {
      setIsLoadingPackages(false);
    }
  }, [importSource]);

  // 选择导入源
  const handleSelectSource = useCallback((source: ScriptSource) => {
    setImportSource(source);
    setAvailablePackages([]);
    setSelectedPackage(null);

    if (source.type === 'remote' || source.type === 'official') {
      loadRemotePackages();
    }
  }, [loadRemotePackages]);

  // 处理文件导入
  const handleFileImport = useCallback(async (file: File) => {
    setError(null);

    try {
      const text = await file.text();
      const pkg = JSON.parse(text) as ScriptPackage;

      // 验证包结构
      if (!pkg.metadata || !Array.isArray(pkg.scripts)) {
        throw new Error('Invalid package format');
      }

      setSelectedPackage(pkg);
    } catch (err) {
      setError(err instanceof Error ? err.message : 'Failed to parse file');
    }
  }, []);

  // 执行导入
  const handleImport = useCallback(async () => {
    if (!selectedPackage) return;

    setError(null);

    try {
      const result = await scriptHost.importScripts(selectedPackage, {
        overwrite: true,
        importAsCopies: false,
      });

      setImportResult(result);
      onImportComplete?.(result);
    } catch (err) {
      setError(err instanceof Error ? err.message : 'Import failed');
    }
  }, [selectedPackage, onImportComplete]);

  // 执行导出
  const handleExport = useCallback(async () => {
    if (scriptsToExport.length === 0) {
      setError('Please select at least one script to export');
      return;
    }

    if (!exportMetadata.name?.trim()) {
      setError('Please provide a package name');
      return;
    }

    setError(null);

    try {
      const pkg = await scriptHost.exportScripts(
        scriptsToExport.map(s => s.id),
        exportMetadata
      );

      // 触发下载
      downloadPackage(pkg);
      onExportComplete?.(pkg);
    } catch (err) {
      setError(err instanceof Error ? err.message : 'Export failed');
    }
  }, [scriptsToExport, exportMetadata, onExportComplete]);

  // 下载包为JSON文件
  const downloadPackage = (pkg: ScriptPackage) => {
    const blob = new Blob([JSON.stringify(pkg, null, 2)], { type: 'application/json' });
    const url = URL.createObjectURL(blob);
    const a = document.createElement('a');
    a.href = url;
    a.download = `${pkg.metadata.name.replace(/\s+/g, '-').toLowerCase()}-v${pkg.metadata.version}.ccscript`;
    document.body.appendChild(a);
    a.click();
    document.body.removeChild(a);
    URL.revokeObjectURL(url);
  };

  // 切换脚本选择（用于导出）
  const toggleScriptSelection = useCallback((scriptId: string) => {
    setScriptsToExport(prev => {
      if (prev.some(s => s.id === scriptId)) {
        return prev.filter(s => s.id !== scriptId);
      } else {
        return [...prev, scriptHost.getScript(scriptId)!].filter(Boolean);
      }
    });
  }, []);

  // 渲染导入模式
  const renderImportMode = () => (
    <div className="import-mode">
      {/* 来源选择 */}
      <div className="source-selection">
        <h4>Select Import Source</h4>
        <div className="source-list">
          <button
            className={`source-card ${importSource?.id === MOCK_LOCAL_SOURCE.id ? 'selected' : ''}`}
            onClick={() => handleSelectSource(MOCK_LOCAL_SOURCE)}
          >
            <span className="source-icon">📁</span>
            <span className="source-name">Local File</span>
            <span className="source-desc">Import from .ccscript file</span>
          </button>
          <button
            className={`source-card ${importSource?.id === MOCK_REMOTE_SOURCE.id ? 'selected' : ''}`}
            onClick={() => handleSelectSource(MOCK_REMOTE_SOURCE)}
          >
            <span className="source-icon">☁️</span>
            <span className="source-name">{MOCK_REMOTE_SOURCE.name}</span>
            <span className="source-desc">Browse community scripts</span>
          </button>
        </div>
      </div>

      {/* 文件上传 */}
      {importSource?.type === 'local' && (
        <div className="file-upload">
          <h4>Select File</h4>
          <input
            type="file"
            accept=".json,.ccscript"
            onChange={(e) => e.target.files?.[0] && handleFileImport(e.target.files[0])}
          />
          <p className="file-hint">
            Supported formats: .json, .ccscript
          </p>
        </div>
      )}

      {/* 远程包列表 */}
      {importSource?.type !== 'local' && isLoadingPackages && (
        <div className="loading-packages">
          <div className="spinner" />
          Loading packages...
        </div>
      )}

      {/* 包预览 */}
      {selectedPackage && (
        <div className="package-preview">
          <h4>Package Preview</h4>
          <div className="preview-card">
            <div className="preview-header">
              <h5>{selectedPackage.metadata.name}</h5>
              <span className="version">v{selectedPackage.metadata.version}</span>
            </div>
            <p className="description">{selectedPackage.metadata.description}</p>
            <div className="preview-meta">
              <span>By {selectedPackage.metadata.author}</span>
              <span>{selectedPackage.scripts.length} scripts</span>
            </div>
            <div className="preview-tags">
              {selectedPackage.metadata.tags.map(tag => (
                <span key={tag} className="tag">{tag}</span>
              ))}
            </div>
          </div>
          <div className="preview-scripts">
            <h5>Included Scripts:</h5>
            <ul>
              {selectedPackage.scripts.map((script, i) => (
                <li key={i}>
                  <span className="script-type">{script.scriptType}</span>
                  {script.name}
                </li>
              ))}
            </ul>
          </div>
        </div>
      )}
    </div>
  );

  // 渲染导出模式
  const renderExportMode = () => {
    const allScripts = scriptHost.getAllScripts();

    return (
      <div className="export-mode">
        <div className="export-metadata">
          <h4>Package Metadata</h4>
          <div className="form-group">
            <label>Package Name *</label>
            <input
              type="text"
              value={exportMetadata.name}
              onChange={(e) => setExportMetadata(prev => ({ ...prev, name: e.target.value }))}
              placeholder="My Script Collection"
            />
          </div>
          <div className="form-group">
            <label>Author</label>
            <input
              type="text"
              value={exportMetadata.author}
              onChange={(e) => setExportMetadata(prev => ({ ...prev, author: e.target.value }))}
              placeholder="Your name"
            />
          </div>
          <div className="form-group">
            <label>Description</label>
            <textarea
              value={exportMetadata.description}
              onChange={(e) => setExportMetadata(prev => ({ ...prev, description: e.target.value }))}
              placeholder="Describe your script package"
            />
          </div>
          <div className="form-group">
            <label>Tags (comma-separated)</label>
            <input
              type="text"
              value={exportMetadata.tags?.join(', ')}
              onChange={(e) => setExportMetadata(prev => ({
                ...prev,
                tags: e.target.value.split(',').map(s => s.trim()).filter(Boolean)
              }))}
              placeholder="admin, toolkit, automation"
            />
          </div>
        </div>

        <div className="script-selection">
          <h4>Select Scripts to Export ({scriptsToExport.length} selected)</h4>
          <div className="script-list">
            {allScripts.map(script => (
              <label key={script.id} className="script-item">
                <input
                  type="checkbox"
                  checked={scriptsToExport.some(s => s.id === script.id)}
                  onChange={() => toggleScriptSelection(script.id)}
                />
                <span className="script-info">
                  <span className="script-name">{script.name}</span>
                  <span className="script-type">{script.scriptType}</span>
                </span>
              </label>
            ))}
          </div>
        </div>
      </div>
    );
  };

  // 渲染导入结果
  const renderImportResult = () => {
    if (!importResult) return null;

    return (
      <div className="import-result">
        <div className={`result-banner ${importResult.success ? 'success' : 'failed'}`}>
          {importResult.success ? '✓ Import Successful' : '✗ Import Failed'}
        </div>
        <div className="result-details">
          <p>Imported: {importResult.imported} scripts</p>
          <p>Skipped: {importResult.skipped} scripts</p>
          {importResult.errors.length > 0 && (
            <div className="result-errors">
              <strong>Errors:</strong>
              <ul>
                {importResult.errors.map((err, i) => (
                  <li key={i}>{err}</li>
                ))}
              </ul>
            </div>
          )}
          {importResult.warnings.length > 0 && (
            <div className="result-warnings">
              <strong>Warnings:</strong>
              <ul>
                {importResult.warnings.map((warn, i) => (
                  <li key={i}>{warn}</li>
                ))}
              </ul>
            </div>
          )}
        </div>
        <button className="btn-secondary" onClick={() => setImportResult(null)}>
          Back
        </button>
      </div>
    );
  };

  return (
    <div className="script-marketplace">
      <div className="marketplace-header">
        <h2>Script Marketplace</h2>
        <button className="btn-close" onClick={onClose}>×</button>
      </div>

      <div className="marketplace-tabs">
        <button
          className={mode === 'import' ? 'active' : ''}
          onClick={() => setMode('import')}
        >
          📥 Import
        </button>
        <button
          className={mode === 'export' ? 'active' : ''}
          onClick={() => setMode('export')}
        >
          📤 Export
        </button>
      </div>

      <div className="marketplace-body">
        {error && (
          <div className="marketplace-error">
            {error}
            <button onClick={() => setError(null)}>×</button>
          </div>
        )}

        {importResult ? (
          renderImportResult()
        ) : mode === 'import' ? (
          renderImportMode()
        ) : (
          renderExportMode()
        )}
      </div>

      <div className="marketplace-footer">
        {mode === 'import' && selectedPackage && !importResult && (
          <>
            <button className="btn-secondary" onClick={() => setSelectedPackage(null)}>
              Back
            </button>
            <button className="btn-primary" onClick={handleImport}>
              Import Package
            </button>
          </>
        )}

        {mode === 'export' && !importResult && (
          <>
            <button className="btn-secondary" onClick={onClose}>
              Cancel
            </button>
            <button
              className="btn-primary"
              onClick={handleExport}
              disabled={scriptsToExport.length === 0}
            >
              Export {scriptsToExport.length} Script(s)
            </button>
          </>
        )}
      </div>
    </div>
  );
}
```

---

## 7. ScriptContext 设计

### 7.1 Context 定义

```typescript
// src/plugin/script/ScriptContext.tsx

import React, { createContext, useContext, useState, useCallback, useEffect } from 'react';
import { scriptHost } from './ScriptHost';
import type {
  CommandScript,
  ScriptFilter,
  ScriptExecutionContext,
  ScriptExecutionResult,
  ScriptPackage,
  ScriptImportResult,
} from './types';

interface ScriptContextState {
  scripts: CommandScript[];
  selectedScript: CommandScript | null;
  isLoading: boolean;
  error: string | null;
  executionHistory: ScriptExecutionResult[];
  logs: ScriptLogEntry[];
}

interface ScriptContextValue extends ScriptContextState {
  // CRUD
  loadScripts: () => Promise<void>;
  saveScript: (script: Partial<CommandScript>) => Promise<CommandScript>;
  deleteScript: (scriptId: string) => Promise<void>;
  duplicateScript: (scriptId: string) => Promise<CommandScript>;

  // 筛选
  filterScripts: (filter: ScriptFilter) => CommandScript[];
  setSelectedScript: (script: CommandScript | null) => void;

  // 收藏
  toggleFavorite: (scriptId: string) => Promise<void>;

  // 执行
  executeScript: (context: ScriptExecutionContext) => Promise<ScriptExecutionResult[]>;
  cancelExecution: (executionId: string) => Promise<void>;

  // 导入导出
  importScripts: (pkg: ScriptPackage, options?: object) => Promise<ScriptImportResult>;
  exportScripts: (scriptIds: string[], metadata?: object) => Promise<ScriptPackage>;

  // 工具
  getAvailableTargets: () => Promise<Target[]>;
  getStats: () => object;
}

const ScriptContext = createContext<ScriptContextValue | null>(null);

export function ScriptProvider({ children }: { children: React.ReactNode }) {
  const [state, setState] = useState<ScriptContextState>({
    scripts: [],
    selectedScript: null,
    isLoading: false,
    error: null,
    executionHistory: [],
    logs: [],
  });

  // 初始化
  useEffect(() => {
    scriptHost.init();
    loadScripts();

    // 订阅事件
    const unsubLoad = scriptHost.on('scripts:loaded', () => {
      setState(prev => ({ ...prev, scripts: scriptHost.getAllScripts() }));
    });

    return () => {
      unsubLoad();
    };
  }, []);

  const loadScripts = useCallback(async () => {
    setState(prev => ({ ...prev, isLoading: true, error: null }));
    try {
      await scriptHost.loadAll();
      setState(prev => ({
        ...prev,
        scripts: scriptHost.getAllScripts(),
        isLoading: false,
      }));
    } catch (err) {
      setState(prev => ({
        ...prev,
        error: err instanceof Error ? err.message : 'Failed to load scripts',
        isLoading: false,
      }));
    }
  }, []);

  const saveScript = useCallback(async (script: Partial<CommandScript>) => {
    const result = script.id
      ? await scriptHost.updateScript(script.id, script)
      : await scriptHost.createScript(script);

    setState(prev => ({
      ...prev,
      scripts: scriptHost.getAllScripts(),
    }));

    return result;
  }, []);

  const deleteScript = useCallback(async (scriptId: string) => {
    await scriptHost.deleteScript(scriptId);
    setState(prev => ({
      ...prev,
      scripts: scriptHost.getAllScripts(),
      selectedScript: prev.selectedScript?.id === scriptId ? null : prev.selectedScript,
    }));
  }, []);

  const duplicateScript = useCallback(async (scriptId: string) => {
    const result = await scriptHost.duplicateScript(scriptId);
    setState(prev => ({
      ...prev,
      scripts: scriptHost.getAllScripts(),
    }));
    return result;
  }, []);

  const filterScripts = useCallback((filter: ScriptFilter) => {
    return scriptHost.filterScripts(filter);
  }, []);

  const setSelectedScript = useCallback((script: CommandScript | null) => {
    setState(prev => ({ ...prev, selectedScript: script }));
  }, []);

  const toggleFavorite = useCallback(async (scriptId: string) => {
    await scriptHost.toggleFavorite(scriptId);
    setState(prev => ({
      ...prev,
      scripts: scriptHost.getAllScripts(),
    }));
  }, []);

  const executeScript = useCallback(async (context: ScriptExecutionContext) => {
    const results = await scriptHost.executeScript(context);
    setState(prev => ({
      ...prev,
      executionHistory: [...prev.executionHistory, ...results],
    }));
    return results;
  }, []);

  const cancelExecution = useCallback(async (executionId: string) => {
    await scriptHost.cancelExecution(executionId);
  }, []);

  const importScripts = useCallback(async (
    pkg: ScriptPackage,
    options?: { overwrite?: boolean; importAsCopies?: boolean }
  ) => {
    const result = await scriptHost.importScripts(pkg, options);
    setState(prev => ({
      ...prev,
      scripts: scriptHost.getAllScripts(),
    }));
    return result;
  }, []);

  const exportScripts = useCallback(async (
    scriptIds: string[],
    metadata?: Partial<ScriptPackageMetadata>
  ) => {
    return scriptHost.exportScripts(scriptIds, metadata);
  }, []);

  const getAvailableTargets = useCallback(async () => {
    // TODO: 实现从后端获取可用目标
    return [];
  }, []);

  const getStats = useCallback(() => {
    return scriptHost.getStats();
  }, []);

  const value: ScriptContextValue = {
    ...state,
    loadScripts,
    saveScript,
    deleteScript,
    duplicateScript,
    filterScripts,
    setSelectedScript,
    toggleFavorite,
    executeScript,
    cancelExecution,
    importScripts,
    exportScripts,
    getAvailableTargets,
    getStats,
  };

  return (
    <ScriptContext.Provider value={value}>
      {children}
    </ScriptContext.Provider>
  );
}

export function useScripts() {
  const context = useContext(ScriptContext);
  if (!context) {
    throw new Error('useScripts must be used within a ScriptProvider');
  }
  return context;
}
```

---

## 8. 文件结构

```
CC-rClient/src/plugin/script/
├── index.ts                      # 导出入口
├── types.ts                      # 类型定义（已存在）
├── ScriptHost.ts                 # 核心门面类
├── ScriptContext.tsx             # React Context
├── hooks.ts                      # 自定义 Hooks
│
├── components/
│   ├── ScriptEditor.tsx          # 脚本编辑器
│   ├── ScriptEditor.css
│   ├── ScriptList.tsx            # 脚本列表
│   ├── ScriptList.css
│   ├── ScriptRunner.tsx          # 脚本执行器
│   ├── ScriptRunner.css
│   └── ScriptMarketplace.tsx     # 脚本市场
│       └── ScriptMarketplace.css
│
├── api.ts                        # Tauri API 封装
├── mockData.ts                   # 开发用模拟数据
├── mockApi.ts                    # 开发用模拟 API
└── README.md                     # 组件使用文档
```

---

## 9. 实现优先级

| 优先级 | 组件 | 依赖 |
|--------|------|------|
| P0 | ScriptHost | 无 |
| P0 | ScriptContext | ScriptHost |
| P0 | api.ts (Tauri调用) | 后端API |
| P1 | ScriptList | ScriptHost, ScriptContext |
| P1 | ScriptEditor | ScriptHost |
| P1 | ScriptRunner | ScriptHost, ScriptList |
| P2 | ScriptMarketplace | ScriptHost, ScriptContext |
| P2 | mockData/mockApi | types |

---

## 10. 集成到 App.tsx

```tsx
// 在 App.tsx 中

import { ScriptProvider } from './plugin/script/ScriptContext';
import { ScriptList } from './plugin/script/ScriptList';
import { ScriptEditor } from './plugin/script/ScriptEditor';
import { ScriptRunner } from './plugin/script/ScriptRunner';
import { ScriptMarketplace } from './plugin/script/ScriptMarketplace';
import type { CommandScript } from './plugin/script/types';

// 添加到 state
const [activePage, setActivePage] = useState<"stations" | "settings" | "groups" | "messages" | "scripts">("stations");

// Script 相关的 state
const [scriptViewState, setScriptViewState] = useState<{
  mode: 'list' | 'editor' | 'runner' | 'marketplace';
  selectedScript: CommandScript | null;
  editorMode: 'create' | 'edit' | 'view';
  marketplaceMode: 'import' | 'export';
}>({
  mode: 'list',
  selectedScript: null,
  editorMode: 'create',
  marketplaceMode: 'import',
});

// Handler functions
const handleNewScript = () => {
  setScriptViewState({
    mode: 'editor',
    selectedScript: null,
    editorMode: 'create',
    marketplaceMode: 'import',
  });
};

const handleEditScript = (script: CommandScript) => {
  setScriptViewState({
    mode: 'editor',
    selectedScript: script,
    editorMode: 'edit',
    marketplaceMode: 'import',
  });
};

const handleExecuteScript = (script: CommandScript) => {
  setScriptViewState({
    mode: 'runner',
    selectedScript: script,
    editorMode: 'view',
    marketplaceMode: 'import',
  });
};

const handleCloseEditor = () => {
  setScriptViewState(prev => ({ ...prev, mode: 'list', selectedScript: null }));
};

// 渲染
return (
  <ScriptProvider>
    <div className="app">
      {/* Tab Bar */}
      <div className="tab-bar">
        <button
          className={activePage === 'scripts' ? 'active' : ''}
          onClick={() => setActivePage('scripts')}
        >
          Scripts
        </button>
        {/* other tabs */}
      </div>

      {/* Content */}
      <div className="page-content">
        {activePage === 'scripts' && scriptViewState.mode === 'list' && (
          <ScriptList
            onEditScript={handleEditScript}
            onExecuteScript={handleExecuteScript}
            onImport={() => setScriptViewState(prev => ({
              ...prev,
              mode: 'marketplace',
              marketplaceMode: 'import',
            }))}
            onExport={() => setScriptViewState(prev => ({
              ...prev,
              mode: 'marketplace',
              marketplaceMode: 'export',
            }))}
          />
        )}

        {activePage === 'scripts' && scriptViewState.mode === 'editor' && (
          <ScriptEditor
            script={scriptViewState.selectedScript}
            readOnly={scriptViewState.editorMode === 'view'}
            onSave={async (scriptData) => {
              // 调用 saveScript
              await scriptHost.updateScript(scriptData.id, scriptData);
              handleCloseEditor();
            }}
            onCancel={handleCloseEditor}
            onClose={handleCloseEditor}
            isModal={false}
          />
        )}

        {activePage === 'scripts' && scriptViewState.mode === 'runner' && (
          <ScriptRunner
            script={scriptViewState.selectedScript!}
            onExecutionComplete={() => {
              handleCloseEditor();
            }}
            onCancel={handleCloseEditor}
            onClose={handleCloseEditor}
          />
        )}

        {activePage === 'scripts' && scriptViewState.mode === 'marketplace' && (
          <ScriptMarketplace
            mode={scriptViewState.marketplaceMode}
            onImportComplete={() => handleCloseEditor()}
            onExportComplete={() => handleCloseEditor()}
            onClose={handleCloseEditor}
          />
        )}
      </div>
    </div>
  </ScriptProvider>
);
```

---

## 11. 总结

这个设计：

1. **遵循 PluginHost 模式**：ScriptHost 作为统一门面，内部协调 ScriptManager、ScriptRegistry、ScriptExecutor 等子模块
2. **分层清晰**：UI组件 → Context/Hooks → ScriptHost → Tauri API → Rust Backend
3. **组件独立**：每个组件职责单一，可独立测试和使用
4. **事件驱动**：使用事件系统进行组件间通信
5. **类型安全**：完整的 TypeScript 类型定义
6. **可扩展**：易于添加新功能（如版本管理、脚本分组等）
