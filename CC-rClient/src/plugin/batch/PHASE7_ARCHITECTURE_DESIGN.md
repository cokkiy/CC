# Phase 7: Batch Operations Support System - Frontend Architecture Design

**Document Version:** 1.0  
**Created:** 2026-04-21  
**Phase:** Phase 7 - Batch Operations Support System  
**Reference Pattern:** Phase 6 Script Management System

---

## 1. Overview

Phase 7 implements a Batch Operations Support System for managing operations across multiple devices. The architecture follows the established Phase 6 pattern (ScriptEditor, ScriptList, ScriptRunner, ScriptMarketplace) with adaptations for batch operations.

### 1.1 Core Models

```typescript
// From CC_project_tasks.md Phase 7 specification

BatchTask {
    id: UUID
    name: String
    task_type: enum[command, script, config, file_transfer, upgrade, reboot]
    target_selector: TargetSelector
    content: String
    execution_policy: ExecutionPolicy
    created_by: String
    status: TaskStatus
}

TargetSelector {
    selector_type: enum[all, group, tag, device_ids, filter]
    groups: Option<Vec<String>>
    tags: Option<Vec<String>>
    device_ids: Option<Vec<String>>
    filter_expr: Option<String>
}

ExecutionPolicy {
    mode: enum[parallel, batch, rolling]
    batch_size: Option<u32>
    continue_on_failure: bool
    failure_threshold_percent: f32
    timeout_secs: u32
    retry_count: u32
}
```

### 1.2 Execution Strategies

| Strategy | Description |
|----------|-------------|
| Parallel | Execute on all targets simultaneously |
| Batch | Execute in batches of N devices |
| Rolling | Gradually increase targets (1 → 5 → all) |
| Circuit Breaker | Auto-pause when failure rate exceeds threshold |

---

## 2. Component Hierarchy

```
BatchPage (Container)
├── BatchTaskList
│   ├── BatchTaskCard[]
│   ├── BatchTaskFilters
│   └── BatchTaskToolbar
├── BatchTaskEditor (Modal)
│   ├── TargetSelector
│   ├── ExecutionPolicyConfig
│   └── ContentEditor
├── BatchTaskRunner (Modal)
│   ├── TargetSelectorDisplay
│   ├── ExecutionProgress
│   └── BatchResultSummary
└── BatchTaskImportExport (Modal)
    └── (Similar to ScriptMarketplace pattern)
```

---

## 3. Type Definitions (`types.ts`)

```typescript
/**
 * Batch Operations Type Definitions
 * Part of Phase 7: Batch Operations Support System
 */

// ============================================
// Enums and Unions
// ============================================

// Task type enum
export type BatchTaskType = 'command' | 'script' | 'config' | 'file_transfer' | 'upgrade' | 'reboot';

// Target selector type enum
export type TargetSelectorType = 'all' | 'group' | 'tag' | 'device_ids' | 'filter';

// Execution mode enum
export type ExecutionMode = 'parallel' | 'batch' | 'rolling';

// Task status enum
export type BatchTaskStatus = 
  | 'draft'           // Task created but not started
  | 'pending'         // Task queued for execution
  | 'running'         // Task currently executing
  | 'paused'          // Task paused (e.g., circuit breaker triggered)
  | 'completed'       // Task completed successfully
  | 'partial_failure' // Task completed with some failures
  | 'failed'          // Task failed entirely
  | 'cancelled';      // Task was cancelled

// ============================================
// Target Selector
// ============================================

export interface TargetSelector {
  selectorType: TargetSelectorType;
  groups?: string[];           // For 'group' type
  tags?: string[];             // For 'tag' type
  deviceIds?: string[];        // For 'device_ids' type
  filterExpr?: string;         // For 'filter' type - filter expression
}

// Target device representation
export interface BatchTarget {
  id: string;
  name: string;
  group?: string;
  tags?: Record<string, string>;
  status: 'online' | 'offline' | 'busy' | 'unknown';
  lastSeen?: string;
}

// ============================================
// Execution Policy
// ============================================

export interface ExecutionPolicy {
  mode: ExecutionMode;
  batchSize?: number;                    // For 'batch' mode
  continueOnFailure: boolean;
  failureThresholdPercent: number;        // 0-100, triggers circuit breaker
  timeoutSecs: number;
  retryCount: number;
  retryDelaySecs?: number;               // Delay between retries
}

// Default execution policy
export const DEFAULT_EXECUTION_POLICY: ExecutionPolicy = {
  mode: 'parallel',
  batchSize: 5,
  continueOnFailure: true,
  failureThresholdPercent: 50,
  timeoutSecs: 300,
  retryCount: 0,
  retryDelaySecs: 5,
};

// ============================================
// Batch Task
// ============================================

export interface BatchTask {
  id: string;
  name: string;
  description?: string;
  taskType: BatchTaskType;
  targetSelector: TargetSelector;
  content: string;                       // Script content, command, config, etc.
  parameters?: BatchTaskParameter[];     // For parameterized tasks
  executionPolicy: ExecutionPolicy;
  createdBy: string;
  createdAt: string;
  updatedAt: string;
  status: BatchTaskStatus;
  version: number;
  usageCount: number;
  lastRunAt?: string;
}

// Batch task parameter (similar to ScriptParameter)
export interface BatchTaskParameter {
  name: string;
  paramType: 'string' | 'number' | 'boolean' | 'select';
  defaultValue: string;
  required: boolean;
  validation?: string;
  description?: string;
  options?: string[];
}

// ============================================
// Execution Result
// ============================================

// Result for a single target execution
export interface BatchTargetResult {
  targetId: string;
  targetName: string;
  status: 'success' | 'failed' | 'timeout' | 'skipped' | 'cancelled';
  startedAt: string;
  completedAt?: string;
  durationMs?: number;
  output?: string;
  error?: string;
  exitCode?: number;
  retryAttempt?: number;
}

// Overall batch execution result
export interface BatchExecutionResult {
  taskId: string;
  executionId: string;
  status: BatchTaskStatus;
  startedAt: string;
  completedAt?: string;
  totalTargets: number;
  successCount: number;
  failureCount: number;
  skippedCount: number;
  results: BatchTargetResult[];
  circuitBreakerTriggered?: boolean;
  failureRate?: number;
}

// ============================================
// Filter and Search
// ============================================

export interface BatchTaskFilter {
  search?: string;
  taskType?: BatchTaskType;
  status?: BatchTaskStatus[];
  createdBy?: string;
  tags?: string[];
  dateFrom?: string;
  dateTo?: string;
}

// ============================================
// Import/Export
// ============================================

export interface BatchTaskPackage {
  metadata: BatchTaskPackageMetadata;
  tasks: BatchTask[];
  signature?: string;
}

export interface BatchTaskPackageMetadata {
  id: string;
  name: string;
  version: string;
  description?: string;
  author?: string;
  tags: string[];
  createdAt: string;
  updatedAt: string;
}

export interface BatchTaskImportResult {
  success: boolean;
  imported: number;
  skipped: number;
  errors: string[];
  warnings: string[];
}

// ============================================
// Execution Logs
// ============================================

export interface BatchExecutionLogEntry {
  timestamp: string;
  level: 'info' | 'warning' | 'error' | 'debug';
  executionId: string;
  targetId?: string;
  targetName?: string;
  message: string;
  details?: Record<string, unknown>;
}
```

---

## 4. API Layer (`api.ts`)

```typescript
/**
 * Batch Operations API - Tauri Backend Communication
 * Part of Phase 7: Batch Operations Support System
 */

import { invoke } from '@tauri-apps/api/core';
import type {
  BatchTask,
  BatchTaskFilter,
  BatchExecutionResult,
  BatchTaskPackage,
  BatchTaskImportResult,
  BatchTarget,
  ExecutionPolicy,
  TargetSelector,
} from './types';

// ============================================
// API Response Types
// ============================================

interface LoadBatchTasksResult {
  tasks: BatchTask[];
}

interface SaveBatchTaskResult {
  task: BatchTask;
}

interface DeleteBatchTaskResult {
  success: boolean;
}

interface ExecuteBatchTaskResult {
  execution: BatchExecutionResult;
}

interface GetTargetsResult {
  targets: BatchTarget[];
  total: number;
}

interface ValidateTaskResult {
  valid: boolean;
  errors: string[];
  warnings: string[];
}

interface GetExecutionStatusResult {
  executionId: string;
  status: BatchTaskStatus;
  progress: {
    total: number;
    completed: number;
    failed: number;
    running: number;
  };
}

// ============================================
// Batch API
// ============================================

export const batchApi = {
  /**
   * Load all batch tasks
   */
  async loadTasks(): Promise<BatchTask[]> {
    try {
      const result = await invoke<LoadBatchTasksResult>('load_batch_tasks');
      return result.tasks;
    } catch (error) {
      console.error('[BatchApi] Failed to load tasks:', error);
      throw error;
    }
  },

  /**
   * Save a batch task (create or update)
   */
  async saveTask(task: Partial<BatchTask>): Promise<BatchTask> {
    try {
      const result = await invoke<SaveBatchTaskResult>('save_batch_task', { task });
      return result.task;
    } catch (error) {
      console.error('[BatchApi] Failed to save task:', error);
      throw error;
    }
  },

  /**
   * Delete a batch task
   */
  async deleteTask(taskId: string): Promise<void> {
    try {
      await invoke<DeleteBatchTaskResult>('delete_batch_task', { task_id: taskId });
    } catch (error) {
      console.error('[BatchApi] Failed to delete task:', error);
      throw error;
    }
  },

  /**
   * Execute a batch task
   */
  async executeTask(
    taskId: string,
    targets: string[],
    parameters?: Record<string, string>
  ): Promise<BatchExecutionResult> {
    try {
      const result = await invoke<ExecuteBatchTaskResult>('execute_batch_task', {
        task_id: taskId,
        targets,
        parameters,
      });
      return result.execution;
    } catch (error) {
      console.error('[BatchApi] Failed to execute task:', error);
      throw error;
    }
  },

  /**
   * Execute a batch task with inline content (without saving)
   */
  async executeBatch(
    taskType: string,
    content: string,
    targetSelector: TargetSelector,
    executionPolicy: ExecutionPolicy,
    parameters?: Record<string, string>
  ): Promise<BatchExecutionResult> {
    try {
      const result = await invoke<ExecuteBatchTaskResult>('execute_batch', {
        task_type: taskType,
        content,
        target_selector: targetSelector,
        execution_policy: executionPolicy,
        parameters,
      });
      return result.execution;
    } catch (error) {
      console.error('[BatchApi] Failed to execute batch:', error);
      throw error;
    }
  },

  /**
   * Cancel a running batch execution
   */
  async cancelExecution(executionId: string): Promise<void> {
    try {
      await invoke('cancel_batch_execution', { execution_id: executionId });
    } catch (error) {
      console.error('[BatchApi] Failed to cancel execution:', error);
      throw error;
    }
  },

  /**
   * Pause a running batch execution
   */
  async pauseExecution(executionId: string): Promise<void> {
    try {
      await invoke('pause_batch_execution', { execution_id: executionId });
    } catch (error) {
      console.error('[BatchApi] Failed to pause execution:', error);
      throw error;
    }
  },

  /**
   * Resume a paused batch execution
   */
  async resumeExecution(executionId: string): Promise<void> {
    try {
      await invoke('resume_batch_execution', { execution_id: executionId });
    } catch (error) {
      console.error('[BatchApi] Failed to resume execution:', error);
      throw error;
    }
  },

  /**
   * Get available targets for batch operations
   */
  async getTargets(filters?: {
    groups?: string[];
    tags?: string[];
    status?: string[];
  }): Promise<BatchTarget[]> {
    try {
      const result = await invoke<GetTargetsResult>('get_batch_targets', { filters });
      return result.targets;
    } catch (error) {
      console.error('[BatchApi] Failed to get targets:', error);
      throw error;
    }
  },

  /**
   * Get execution status for a running task
   */
  async getExecutionStatus(executionId: string): Promise<GetExecutionStatusResult> {
    try {
      const result = await invoke<GetExecutionStatusResult>('get_batch_execution_status', {
        execution_id: executionId,
      });
      return result;
    } catch (error) {
      console.error('[BatchApi] Failed to get execution status:', error);
      throw error;
    }
  },

  /**
   * Get execution history
   */
  async getExecutionHistory(limit?: number): Promise<BatchExecutionResult[]> {
    try {
      const result = await invoke<{ history: BatchExecutionResult[] }>('get_batch_history', {
        limit,
      });
      return result.history;
    } catch (error) {
      console.error('[BatchApi] Failed to get execution history:', error);
      throw error;
    }
  },

  /**
   * Validate task configuration without saving
   */
  async validateTask(task: Partial<BatchTask>): Promise<ValidateTaskResult> {
    try {
      const result = await invoke<ValidateTaskResult>('validate_batch_task', { task });
      return result;
    } catch (error) {
      console.error('[BatchApi] Failed to validate task:', error);
      throw error;
    }
  },

  /**
   * Preview target selection without executing
   */
  async previewTargets(selector: TargetSelector): Promise<BatchTarget[]> {
    try {
      const result = await invoke<GetTargetsResult>('preview_batch_targets', {
        selector,
      });
      return result.targets;
    } catch (error) {
      console.error('[BatchApi] Failed to preview targets:', error);
      throw error;
    }
  },

  /**
   * Import batch task package
   */
  async importTaskPackage(
    pkg: BatchTaskPackage,
    options?: { overwrite?: boolean; importAsCopies?: boolean }
  ): Promise<BatchTaskImportResult> {
    try {
      const result = await invoke<BatchTaskImportResult>('import_batch_package', {
        pkg,
        options,
      });
      return result;
    } catch (error) {
      console.error('[BatchApi] Failed to import package:', error);
      throw error;
    }
  },

  /**
   * Export batch tasks as package
   */
  async exportTaskPackage(
    taskIds: string[],
    metadata?: Partial<BatchTaskPackageMetadata>
  ): Promise<BatchTaskPackage> {
    try {
      const result = await invoke<{ package: BatchTaskPackage }>('export_batch_package', {
        task_ids: taskIds,
        metadata,
      });
      return result.package;
    } catch (error) {
      console.error('[BatchApi] Failed to export package:', error);
      throw error;
    }
  },

  /**
   * Duplicate a batch task
   */
  async duplicateTask(taskId: string): Promise<BatchTask> {
    try {
      const result = await invoke<SaveBatchTaskResult>('duplicate_batch_task', {
        task_id: taskId,
      });
      return result.task;
    } catch (error) {
      console.error('[BatchApi] Failed to duplicate task:', error);
      throw error;
    }
  },

  /**
   * Toggle task favorite status
   */
  async toggleFavorite(taskId: string): Promise<void> {
    try {
      await invoke('toggle_batch_task_favorite', { task_id: taskId });
    } catch (error) {
      console.error('[BatchApi] Failed to toggle favorite:', error);
      throw error;
    }
  },
};

export default batchApi;
```

---

## 5. Component Specifications

### 5.1 BatchTaskList Component

**File:** `BatchTaskList.tsx`

**Purpose:** Displays a grid/list of batch tasks with filtering, sorting, and bulk actions.

**Props:**
```typescript
export interface BatchTaskListProps {
  tasks: BatchTask[];
  onSelectTask?: (task: BatchTask) => void;
  onEditTask: (task?: BatchTask) => void;
  onExecuteTask: (task: BatchTask) => void;
  onDeleteTask: (taskId: string) => void;
  onDuplicateTask?: (task: BatchTask) => void;
  onToggleFavorite?: (taskId: string) => void;
  onImport?: () => void;
  onExport?: (tasks: BatchTask[]) => void;
  onPauseTask?: (taskId: string) => void;      // For running tasks
  onCancelTask?: (taskId: string) => void;     // For running tasks
  loading?: boolean;
  title?: string;
}
```

**Key Features:**
- Grid view with task cards
- Type filter tabs (All, Command, Script, Config, FileTransfer, Upgrade, Reboot)
- Status filter (Draft, Pending, Running, Completed, Failed)
- Search by name/description
- Sort by name, created date, last run, usage count
- Favorites toggle
- Pagination (optional)

**Internal Components:**
- `BatchTaskCard` - Individual task display card
- `BatchTaskFilters` - Filter controls
- `BatchTaskToolbar` - Action buttons

---

### 5.2 BatchTaskCard Component

**Purpose:** Displays a single batch task summary.

**Props:**
```typescript
interface BatchTaskCardProps {
  task: BatchTask;
  onSelect?: (task: BatchTask) => void;
  onEdit?: (task: BatchTask) => void;
  onExecute?: (task: BatchTask) => void;
  onDelete?: (taskId: string) => void;
  onDuplicate?: (task: BatchTask) => void;
  onToggleFavorite?: (taskId: string) => void;
  onPause?: (taskId: string) => void;
  onCancel?: (taskId: string) => void;
}
```

**Display Elements:**
- Task name and description
- Task type icon/label
- Status badge
- Target selector summary (e.g., "All devices" or "5 groups")
- Execution policy summary (e.g., "Parallel, 50% threshold")
- Last run timestamp
- Usage count
- Version
- Favorite star

---

### 5.3 BatchTaskEditor Component

**File:** `BatchTaskEditor.tsx`

**Purpose:** Create or edit batch tasks with target selection and execution policy configuration.

**Props:**
```typescript
export interface BatchTaskEditorProps {
  task?: BatchTask;                      // Existing task for editing
  initialTaskType?: BatchTaskType;       // Default type for new tasks
  onSave: (task: Partial<BatchTask>) => void;
  onCancel: () => void;
  onValidate?: (task: Partial<BatchTask>) => Promise<ValidateTaskResult>;
  readOnly?: boolean;
}
```

**Sections:**
1. **Basic Info**
   - Name (required)
   - Description
   - Task Type selector

2. **Target Selector**
   - Selector type tabs: All, Groups, Tags, Device IDs, Filter
   - Dynamic input based on selector type
   - Target preview count

3. **Content Editor**
   - Code editor for script/content
   - Syntax highlighting based on task type
   - Template parameter support (similar to ScriptEditor)

4. **Execution Policy**
   - Mode selector (Parallel, Batch, Rolling)
   - Batch size input (for batch mode)
   - Continue on failure toggle
   - Failure threshold percent input
   - Timeout seconds input
   - Retry count input

5. **Parameters (optional)**
   - Parameter list editor (similar to ScriptEditor)

---

### 5.4 TargetSelector Component

**File:** `TargetSelector.tsx`

**Purpose:** Flexible target selection interface supporting multiple selector types.

**Props:**
```typescript
export interface TargetSelectorProps {
  selector: TargetSelector;
  targets: BatchTarget[];                // Available targets
  onChange: (selector: TargetSelector) => void;
  mode?: 'simple' | 'advanced';          // Show all options or simplified
  disabled?: boolean;
  showPreview?: boolean;                  // Show target count preview
}
```

**Selector Types:**
1. **All** - Select all available targets
2. **Groups** - Select from device groups
3. **Tags** - Select by tag key-value pairs
4. **Device IDs** - Direct device ID selection
5. **Filter** - Expression-based filtering (e.g., `status == 'online' && group == 'production'`)

**Internal Sub-components:**
- `SelectorTypeTabs` - Tab navigation for selector types
- `GroupSelector` - Multi-select for groups
- `TagSelector` - Tag-based selection with key-value
- `DeviceIdSelector` - Searchable device list
- `FilterExpressionEditor` - Expression input with validation
- `TargetPreview` - Shows count and list of selected targets

---

### 5.5 ExecutionPolicyConfig Component

**File:** `ExecutionPolicyConfig.tsx`

**Purpose:** Configure execution policy settings.

**Props:**
```typescript
export interface ExecutionPolicyConfigProps {
  policy: ExecutionPolicy;
  onChange: (policy: ExecutionPolicy) => void;
  disabled?: boolean;
  mode?: 'simple' | 'advanced';
}
```

**Fields:**
| Field | Type | Default | Description |
|-------|------|---------|-------------|
| mode | enum | parallel | Execution mode |
| batchSize | number | 5 | Targets per batch |
| continueOnFailure | boolean | true | Continue after failure |
| failureThresholdPercent | number | 50 | Circuit breaker trigger |
| timeoutSecs | number | 300 | Per-target timeout |
| retryCount | number | 0 | Number of retries |
| retryDelaySecs | number | 5 | Delay between retries |

**Visual Aids:**
- Mode diagram illustrations
- Estimated duration calculator
- Risk indicator based on settings

---

### 5.6 BatchTaskRunner Component

**File:** `BatchTaskRunner.tsx`

**Purpose:** Execute batch tasks and monitor progress in real-time.

**Props:**
```typescript
export interface BatchTaskRunnerProps {
  task: BatchTask;
  targets: BatchTarget[];
  onExecute: (context: BatchExecutionContext) => Promise<BatchExecutionResult>;
  onCancel?: () => void;
  defaultTargets?: string[];
  readOnly?: boolean;
}

export interface BatchExecutionContext {
  taskId: string;
  targetIds: string[];
  parameters?: Record<string, string>;
}
```

**Sections:**
1. **Header**
   - Task name and type
   - Target count
   - Quick policy summary

2. **Target Selection**
   - Pre-filtered target list based on task's target selector
   - Ability to override/limit targets
   - Target status indicators

3. **Parameters (if task has parameters)**
   - Dynamic form based on task parameters

4. **Execution Progress**
   - Overall progress bar
   - Mode-specific visualization:
     - Parallel: All targets progress simultaneously
     - Batch: Batch-by-batch progress
     - Rolling: Expanding ring visualization
   - Success/failure counters
   - Circuit breaker status indicator
   - Elapsed time / ETA

5. **Live Results Table**
   - Target | Status | Duration | Output/Error
   - Real-time updates
   - Expandable rows for details
   - Sort/filter by status

6. **Controls**
   - Start button
   - Pause/Resume button
   - Cancel button (with confirmation)

---

### 5.7 BatchResultSummary Component

**File:** `BatchResultSummary.tsx`

**Purpose:** Display execution results summary after completion.

**Props:**
```typescript
export interface BatchResultSummaryProps {
  result: BatchExecutionResult;
  onClose?: () => void;
  onRetryFailed?: (targetIds: string[]) => void;
  onViewDetails?: (targetId: string) => void;
}
```

**Display:**
- Overall status badge
- Success/failure/skipped counts
- Total duration
- Circuit breaker triggered indicator
- Failure rate percentage
- Results table (sortable by target, status, duration)
- Export results button
- Retry failed button

---

### 5.8 BatchPage Component

**File:** `BatchPage.tsx`

**Purpose:** Main container page for batch operations.

**Props:**
```typescript
export interface BatchPageProps {
  stations: Station[];                    // From parent App
}
```

**Layout:**
```
┌─────────────────────────────────────────────────────────────┐
│  Batch Operations                                           │
├─────────────────────────────────────────────────────────────┤
│  [Toolbar: New Task | Import | Export | Refresh]           │
├─────────────────────────────────────────────────────────────┤
│  [Filters: Type ▾ | Status ▾ | Search...          ]        │
├─────────────────────────────────────────────────────────────┤
│  ┌─────────────┐ ┌─────────────┐ ┌─────────────┐           │
│  │ Task Card   │ │ Task Card   │ │ Task Card   │           │
│  │             │ │             │ │             │           │
│  │ [Run][Edit]  │ │ [Run][Edit] │ │ [Run][Edit]  │           │
│  └─────────────┘ └─────────────┘ └─────────────┘           │
│                                                             │
│  ┌─────────────┐ ┌─────────────┐                            │
│  │ Task Card   │ │ Task Card   │                            │
│  │             │ │             │                            │
│  │ [Run][Edit]  │ │ [Run][Edit]  │                            │
│  └─────────────┘ └─────────────┘                            │
└─────────────────────────────────────────────────────────────┘

[Modals:]
- BatchTaskEditor (create/edit)
- BatchTaskRunner (execute)
- BatchTaskImportExport (import/export)
```

---

## 6. Context Management (`BatchContext.tsx`)

```typescript
/**
 * BatchContext - React Context for Batch Operations Management
 * Part of Phase 7: Batch Operations Support System
 */

import React, { createContext, useContext, useState, useCallback, useEffect } from 'react';
import { batchApi } from './api';
import type {
  BatchTask,
  BatchTaskFilter,
  BatchExecutionResult,
  BatchTaskPackage,
  BatchTaskImportResult,
  BatchTarget,
  BatchExecutionLogEntry,
  ExecutionPolicy,
} from './types';

// Context state interface
interface BatchContextState {
  tasks: BatchTask[];
  selectedTask: BatchTask | null;
  isLoading: boolean;
  error: string | null;
  executionHistory: BatchExecutionResult[];
  logs: BatchExecutionLogEntry[];
  targets: BatchTarget[];
  activeExecution: BatchExecutionResult | null;
}

// Context value interface
interface BatchContextValue extends BatchContextState {
  // CRUD operations
  loadTasks: () => Promise<void>;
  saveTask: (task: Partial<BatchTask>) => Promise<BatchTask>;
  deleteTask: (taskId: string) => Promise<void>;
  duplicateTask: (taskId: string) => Promise<BatchTask>;

  // Filtering
  filterTasks: (filter: BatchTaskFilter) => BatchTask[];
  setSelectedTask: (task: BatchTask | null) => void;

  // Favorites
  toggleFavorite: (taskId: string) => Promise<void>;

  // Execution
  executeTask: (
    taskId: string,
    targetIds: string[],
    parameters?: Record<string, string>
  ) => Promise<BatchExecutionResult>;
  executeBatch: (
    taskType: string,
    content: string,
    targetSelector: TargetSelector,
    policy: ExecutionPolicy,
    parameters?: Record<string, string>
  ) => Promise<BatchExecutionResult>;
  cancelExecution: (executionId: string) => Promise<void>;
  pauseExecution: (executionId: string) => Promise<void>;
  resumeExecution: (executionId: string) => Promise<void>;

  // Targets
  loadTargets: (filters?: object) => Promise<void>;
  previewTargets: (selector: TargetSelector) => Promise<BatchTarget[]>;

  // Import/Export
  importTasks: (pkg: BatchTaskPackage, options?: object) => Promise<BatchTaskImportResult>;
  exportTasks: (taskIds: string[], metadata?: object) => Promise<BatchTaskPackage>;

  // Logs
  addLog: (entry: BatchExecutionLogEntry) => void;
  clearLogs: () => void;

  // Utilities
  getStats: () => object;
}

const BatchContext = createContext<BatchContextValue | null>(null);

export function BatchProvider({ children }: { children: React.ReactNode }) {
  const [state, setState] = useState<BatchContextState>({
    tasks: [],
    selectedTask: null,
    isLoading: false,
    error: null,
    executionHistory: [],
    logs: [],
    targets: [],
    activeExecution: null,
  });

  // Initialize and load tasks
  useEffect(() => {
    loadTasks();
    loadTargets();
  }, []);

  // Subscribe to real-time execution updates (via Tauri events)
  useEffect(() => {
    // TODO: Subscribe to batch_execution_update events
    // const unsubProgress = batchApi.on('batch:progress', handleProgress);
    // const unsubComplete = batchApi.on('batch:complete', handleComplete);
    // return () => { unsubProgress(); unsubComplete(); };
  }, []);

  const loadTasks = useCallback(async () => {
    setState(prev => ({ ...prev, isLoading: true, error: null }));
    try {
      const tasks = await batchApi.loadTasks();
      setState(prev => ({ ...prev, tasks, isLoading: false }));
    } catch (err) {
      setState(prev => ({
        ...prev,
        error: err instanceof Error ? err.message : 'Failed to load tasks',
        isLoading: false,
      }));
    }
  }, []);

  const saveTask = useCallback(async (task: Partial<BatchTask>) => {
    const result = await batchApi.saveTask(task);
    setState(prev => ({ ...prev, tasks: [...prev.tasks.filter(t => t.id !== result.id), result] }));
    return result;
  }, []);

  const deleteTask = useCallback(async (taskId: string) => {
    await batchApi.deleteTask(taskId);
    setState(prev => ({
      ...prev,
      tasks: prev.tasks.filter(t => t.id !== taskId),
      selectedTask: prev.selectedTask?.id === taskId ? null : prev.selectedTask,
    }));
  }, []);

  const duplicateTask = useCallback(async (taskId: string) => {
    const result = await batchApi.duplicateTask(taskId);
    setState(prev => ({ ...prev, tasks: [...prev.tasks, result] }));
    return result;
  }, []);

  const filterTasks = useCallback((filter: BatchTaskFilter) => {
    // Implementation similar to ScriptContext.filterScripts
    return [];
  }, []);

  const setSelectedTask = useCallback((task: BatchTask | null) => {
    setState(prev => ({ ...prev, selectedTask: task }));
  }, []);

  const toggleFavorite = useCallback(async (taskId: string) => {
    await batchApi.toggleFavorite(taskId);
    setState(prev => ({
      ...prev,
      tasks: prev.tasks.map(t => t.id === taskId ? { ...t, isFavorite: !t.isFavorite } : t),
    }));
  }, []);

  const executeTask = useCallback(async (
    taskId: string,
    targetIds: string[],
    parameters?: Record<string, string>
  ) => {
    const result = await batchApi.executeTask(taskId, targetIds, parameters);
    setState(prev => ({
      ...prev,
      executionHistory: [...prev.executionHistory, result],
      activeExecution: result,
    }));
    return result;
  }, []);

  const executeBatch = useCallback(async (
    taskType: string,
    content: string,
    targetSelector: TargetSelector,
    policy: ExecutionPolicy,
    parameters?: Record<string, string>
  ) => {
    const result = await batchApi.executeBatch(taskType, content, targetSelector, policy, parameters);
    setState(prev => ({
      ...prev,
      executionHistory: [...prev.executionHistory, result],
      activeExecution: result,
    }));
    return result;
  }, []);

  const cancelExecution = useCallback(async (executionId: string) => {
    await batchApi.cancelExecution(executionId);
  }, []);

  const pauseExecution = useCallback(async (executionId: string) => {
    await batchApi.pauseExecution(executionId);
  }, []);

  const resumeExecution = useCallback(async (executionId: string) => {
    await batchApi.resumeExecution(executionId);
  }, []);

  const loadTargets = useCallback(async (filters?: object) => {
    try {
      const targets = await batchApi.getTargets(filters);
      setState(prev => ({ ...prev, targets }));
    } catch (err) {
      console.error('[BatchContext] Failed to load targets:', err);
    }
  }, []);

  const previewTargets = useCallback(async (selector: TargetSelector) => {
    return batchApi.previewTargets(selector);
  }, []);

  const importTasks = useCallback(async (
    pkg: BatchTaskPackage,
    options?: { overwrite?: boolean; importAsCopies?: boolean }
  ) => {
    const result = await batchApi.importTaskPackage(pkg, options);
    await loadTasks();
    return result;
  }, [loadTasks]);

  const exportTasks = useCallback(async (
    taskIds: string[],
    metadata?: object
  ) => {
    return batchApi.exportTaskPackage(taskIds, metadata);
  }, []);

  const addLog = useCallback((entry: BatchExecutionLogEntry) => {
    setState(prev => ({ ...prev, logs: [...prev.logs, entry] }));
  }, []);

  const clearLogs = useCallback(() => {
    setState(prev => ({ ...prev, logs: [] }));
  }, []);

  const getStats = useCallback(() => {
    return {
      totalTasks: state.tasks.length,
      runningTasks: state.tasks.filter(t => t.status === 'running').length,
      completedToday: 0, // Calculate from history
    };
  }, [state.tasks]);

  const value: BatchContextValue = {
    ...state,
    loadTasks,
    saveTask,
    deleteTask,
    duplicateTask,
    filterTasks,
    setSelectedTask,
    toggleFavorite,
    executeTask,
    executeBatch,
    cancelExecution,
    pauseExecution,
    resumeExecution,
    loadTargets,
    previewTargets,
    importTasks,
    exportTasks,
    addLog,
    clearLogs,
    getStats,
  };

  return (
    <BatchContext.Provider value={value}>
      {children}
    </BatchContext.Provider>
  );
}

export function useBatch() {
  const context = useContext(BatchContext);
  if (!context) {
    throw new Error('useBatch must be used within a BatchProvider');
  }
  return context;
}

export function useBatchSafe() {
  return useContext(BatchContext);
}
```

---

## 7. File Structure

```
src/plugin/batch/
├── PHASE7_ARCHITECTURE_DESIGN.md   # This document
├── types.ts                        # Type definitions
├── api.ts                          # Tauri invoke wrappers
├── BatchContext.tsx                # React Context + Hooks
├── BatchHost.ts                    # Business logic layer (optional)
├── index.ts                        # Main exports
├── BatchPage.tsx                   # Main container page
├── BatchTaskList.tsx               # Task list with cards
├── BatchTaskEditor.tsx             # Create/edit task modal
├── BatchTaskRunner.tsx             # Execute task modal
├── TargetSelector.tsx              # Target selection component
├── ExecutionPolicyConfig.tsx       # Policy configuration
├── BatchResultSummary.tsx          # Results display
├── BatchTaskCard.tsx               # Individual task card (internal)
├── BatchTaskFilters.tsx            # Filter controls (internal)
├── BatchTaskImportExport.tsx       # Import/export modal
└── components/
    ├── ProgressVisualizer.tsx      # Mode-specific progress viz
    ├── ResultTable.tsx             # Results table component
    └── CircuitBreakerIndicator.tsx # CB status display
```

---

## 8. Tauri Backend API Mapping

| Frontend Function | Tauri Command | Description |
|-------------------|---------------|-------------|
| loadTasks | `load_batch_tasks` | Load all batch tasks |
| saveTask | `save_batch_task` | Create/update batch task |
| deleteTask | `delete_batch_task` | Delete batch task |
| executeTask | `execute_batch_task` | Execute saved task |
| executeBatch | `execute_batch` | Execute inline batch |
| cancelExecution | `cancel_batch_execution` | Cancel running execution |
| pauseExecution | `pause_batch_execution` | Pause execution |
| resumeExecution | `resume_batch_execution` | Resume paused execution |
| getTargets | `get_batch_targets` | Get available targets |
| previewTargets | `preview_batch_targets` | Preview target selection |
| getExecutionStatus | `get_batch_execution_status` | Get execution progress |
| getExecutionHistory | `get_batch_history` | Get execution history |
| validateTask | `validate_batch_task` | Validate task config |
| importTaskPackage | `import_batch_package` | Import task package |
| exportTaskPackage | `export_batch_package` | Export task package |
| duplicateTask | `duplicate_batch_task` | Duplicate a task |
| toggleFavorite | `toggle_batch_task_favorite` | Toggle favorite status |

---

## 9. Real-time Updates (Tauri Events)

The frontend should subscribe to these Tauri events for real-time updates:

```typescript
// Event names (to be implemented in backend)
const BATCH_EVENTS = {
  EXECUTION_STARTED: 'batch:execution:started',
  EXECUTION_PROGRESS: 'batch:execution:progress',
  EXECUTION_COMPLETED: 'batch:execution:completed',
  EXECUTION_FAILED: 'batch:execution:failed',
  CIRCUIT_BREAKER_TRIGGERED: 'batch:circuit_breaker:triggered',
  TARGET_RESULT: 'batch:target:result',
};

// Usage in BatchContext
useEffect(() => {
  const unlisten = Promise.all([
    listen(BATCH_EVENTS.EXECUTION_PROGRESS, (event) => {
      // Update activeExecution with progress
    }),
    listen(BATCH_EVENTS.TARGET_RESULT, (event) => {
      // Update individual target result
    }),
    listen(BATCH_EVENTS.CIRCUIT_BREAKER_TRIGGERED, (event) => {
      // Show warning, pause if needed
    }),
  ]);

  return () => { unlisten.then(fns => fns.forEach(fn => fn())); };
}, []);
```

---

## 10. Integration with Phase 6 (Script System)

Phase 7 Batch Operations should reuse/extend Phase 6 patterns:

1. **ScriptExecutor integration:** Batch tasks with `task_type: 'script'` should reuse the script execution engine from Phase 6.

2. **ScriptEditor adaptation:** BatchTaskEditor content section can embed ScriptEditor for script-type tasks.

3. **TargetSelector reuse:** The TargetSelector component in ScriptRunner can be adapted/extended for batch operations with more selector types.

4. **ExecutionPolicy concept:** Similar to script execution context but with batch-specific options (batch size, rolling, circuit breaker).

---

## 11. Differences from Phase 6 Pattern

| Aspect | Phase 6 (Scripts) | Phase 7 (Batch) |
|--------|-------------------|-----------------|
| Target Selection | Multi-select from list | Complex selector (groups, tags, filter) |
| Execution | Single script, multiple targets | Multiple modes (parallel, batch, rolling) |
| Progress | Simple progress bar | Mode-specific visualization |
| Circuit Breaker | N/A | Failure threshold monitoring |
| Task Types | Single type | Multiple types (command, script, config, etc.) |
| Content | Script content only | Varies by task type |
| Parameters | Script parameters | Task-specific parameters |

---

## 12. CSS/Styling Recommendations

```css
/* Suggested CSS class naming convention */
/* Follows BEM-like pattern from Phase 6 */

.batch-page {}
.batch-task-list {}
.batch-task-card {}
.batch-task-card--favorite {}
.batch-task-card__header {}
.batch-task-card__title {}
.batch-task-card__status--running {}
.batch-task-card__status--completed {}

.batch-task-editor {}
.batch-task-editor__section {}
.batch-task-editor__target-selector {}
.batch-task-editor__execution-policy {}

.target-selector {}
.target-selector__tabs {}
.target-selector__group-list {}
.target-selector__preview {}

.execution-policy {}
.execution-policy__mode-selector {}
.execution-policy__mode--parallel {}
.execution-policy__mode--batch {}

.batch-task-runner {}
.batch-task-runner__progress {}
.batch-task-runner__results {}

.progress-visualizer {}
.progress-visualizer--parallel {}
.progress-visualizer--batch {}
.progress-visualizer--rolling {}

.batch-result-summary {}
.batch-result-summary__stats {}
.batch-result-summary__table {}

.circuit-breaker-indicator {}
.circuit-breaker-indicator--triggered {}
```

---

## 13. Implementation Priority

### P0 (Must Have)
1. Type definitions (types.ts)
2. API layer (api.ts)
3. BatchContext
4. BatchTaskList + BatchTaskCard
5. BatchTaskEditor (basic)
6. BatchTaskRunner (basic)
7. TargetSelector (All, DeviceIds modes)
8. ExecutionPolicyConfig

### P1 (Should Have)
1. TargetSelector (Groups, Tags, Filter modes)
2. Circuit breaker visualization
3. Batch result export
4. Import/Export functionality
5. Full execution policy controls

### P2 (Nice to Have)
1. Rolling execution visualization
2. Batch task templates
3. Scheduled batch execution
4. Batch task scheduling/automation
5. Advanced filter expressions

---

## 14. Summary

Phase 7 Batch Operations frontend follows the established Phase 6 pattern with these key components:

| Component | Purpose | Key Props |
|-----------|---------|-----------|
| `BatchPage` | Container page | stations |
| `BatchTaskList` | Task listing | tasks, onEdit, onExecute, onDelete |
| `BatchTaskCard` | Task summary card | task, onRun, onEdit |
| `BatchTaskEditor` | Create/edit modal | task, onSave, onCancel |
| `BatchTaskRunner` | Execute modal | task, targets, onExecute |
| `TargetSelector` | Target selection | selector, targets, onChange |
| `ExecutionPolicyConfig` | Policy settings | policy, onChange |
| `BatchResultSummary` | Results display | result |
| `BatchContext` | State management | tasks, execute*, save*, delete* |
| `batchApi` | Tauri bridge | load*, save*, execute*, etc. |

The architecture enables complex batch operations with multiple execution strategies, flexible target selection, and real-time progress monitoring with circuit breaker support.
