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
  BatchTaskPackageMetadata,
  BatchTarget,
  ExecutionPolicy,
  TargetSelector,
  BatchTaskStatus,
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
