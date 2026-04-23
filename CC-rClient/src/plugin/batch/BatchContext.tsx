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
  TargetSelector,
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
  loadTargets: (filters?: { groups?: string[]; tags?: string[]; status?: string[] }) => Promise<void>;
  previewTargets: (selector: TargetSelector) => Promise<BatchTarget[]>;

  // Import/Export
  importTasks: (pkg: BatchTaskPackage, options?: { overwrite?: boolean; importAsCopies?: boolean }) => Promise<BatchTaskImportResult>;
  exportTasks: (taskIds: string[], metadata?: object) => Promise<BatchTaskPackage>;

  // Logs
  addLog: (entry: BatchExecutionLogEntry) => void;
  clearLogs: () => void;

  // Utilities
  getStats: () => { totalTasks: number; runningTasks: number; completedToday: number };
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
    setState(prev => ({
      ...prev,
      tasks: prev.tasks.filter(t => t.id !== result.id).concat(result),
    }));
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
    let result = state.tasks;

    if (filter.search) {
      const query = filter.search.toLowerCase();
      result = result.filter(t =>
        t.name.toLowerCase().includes(query) ||
        t.description?.toLowerCase().includes(query)
      );
    }

    if (filter.taskType) {
      result = result.filter(t => t.taskType === filter.taskType);
    }

    if (filter.status && filter.status.length > 0) {
      result = result.filter(t => filter.status!.includes(t.status));
    }

    if (filter.createdBy) {
      result = result.filter(t => t.createdBy === filter.createdBy);
    }

    if (filter.tags && filter.tags.length > 0) {
      result = result.filter(t => filter.tags!.every(tag => t.tags?.includes(tag)));
    }

    return result;
  }, [state.tasks]);

  const setSelectedTask = useCallback((task: BatchTask | null) => {
    setState(prev => ({ ...prev, selectedTask: task }));
  }, []);

  const toggleFavorite = useCallback(async (taskId: string) => {
    await batchApi.toggleFavorite(taskId);
    setState(prev => ({
      ...prev,
      tasks: prev.tasks.map(t =>
        t.id === taskId ? { ...t, isFavorite: !t.isFavorite } : t
      ),
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

  const loadTargets = useCallback(async (filters?: { groups?: string[]; tags?: string[]; status?: string[] }) => {
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
    const today = new Date().toISOString().split('T')[0];
    return {
      totalTasks: state.tasks.length,
      runningTasks: state.tasks.filter(t => t.status === 'running').length,
      completedToday: state.executionHistory.filter(e =>
        e.completedAt?.startsWith(today) && e.status === 'completed'
      ).length,
    };
  }, [state.tasks, state.executionHistory]);

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

/**
 * Hook to access batch context
 * @throws Error if used outside of BatchProvider
 */
export function useBatch() {
  const context = useContext(BatchContext);
  if (!context) {
    throw new Error('useBatch must be used within a BatchProvider');
  }
  return context;
}

/**
 * Hook to access batch context with error handling
 * Returns null if used outside of BatchProvider
 */
export function useBatchSafe() {
  return useContext(BatchContext);
}
