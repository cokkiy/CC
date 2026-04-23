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
  isFavorite?: boolean;
  tags?: string[];
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
// Validation
// ============================================

export interface ValidateTaskResult {
  valid: boolean;
  errors: string[];
  warnings: string[];
  parameterErrors?: Record<string, string[]>;
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
