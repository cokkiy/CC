/**
 * BatchPage - Main container for Batch Operations Management UI
 * Part of Phase 7: Frontend Integration
 */

import React from 'react';
import { useBatch } from './BatchContext';
import { useBatchUI } from './BatchUIContext';
import { BatchTaskList, BatchTaskEditor, BatchTaskRunner } from './index';
import type { BatchTask, BatchTarget, BatchTaskFilter, BatchTaskImportResult, BatchTaskPackage } from './types';
import type { Station } from '../../types';

export interface BatchPageProps {
  stations: Station[];
}

export const BatchPage: React.FC<BatchPageProps> = ({ stations }) => {
  const {
    tasks,
    saveTask,
    deleteTask,
    duplicateTask,
    toggleFavorite,
    executeTask,
    cancelExecution,
    importTasks,
    exportTasks,
  } = useBatch();

  const { 
    editor,
    runner,
    openEditor,
    closeEditor,
    openRunner,
    closeRunner,
  } = useBatchUI();

  // Convert stations to targets for BatchTaskRunner
  // Convert stations to batch targets
  const targets: BatchTarget[] = stations.map(s => ({ 
    id: s.id, 
    name: s.name, 
    status: s.blocked ? 'offline' as const : 'online' as const,
    group: undefined,
    tags: undefined,
  }));

  // BatchTaskList handlers
  const handleEditTask = (task: BatchTask | undefined) => {
    openEditor(task ?? null);
  };

  const handleExecuteTask = (task: BatchTask) => {
    openRunner(task);
  };

  const handleDeleteTask = async (taskId: string) => {
    await deleteTask(taskId);
  };

  const handleToggleFavorite = async (taskId: string) => {
    await toggleFavorite(taskId);
  };

  const handleDuplicateTask = async (task: BatchTask) => {
    await duplicateTask(task.id);
  };

  // BatchTaskEditor handlers
  const handleSaveTask = async (task: Partial<BatchTask>) => {
    await saveTask(task);
    closeEditor();
  };

  const handleCancelEdit = () => {
    closeEditor();
  };

  // BatchTaskRunner handlers
  const handleRunBatch = async (taskId: string, targetIds: string[], parameters?: Record<string, string>) => {
    return executeTask(taskId, targetIds, parameters);
  };

  const handleCancelRun = () => {
    closeRunner();
  };

  return (
    <>
      <main className="grid">
        <section className="panel" style={{ flex: 1 }}>
          <BatchTaskList
            tasks={tasks}
            onSelectTask={(task) => openEditor(task)}
            onEditTask={handleEditTask}
            onExecuteTask={handleExecuteTask}
            onDeleteTask={handleDeleteTask}
            onToggleFavorite={handleToggleFavorite}
            onDuplicateTask={handleDuplicateTask}
          />
        </section>
      </main>

      {editor.open && (
        <BatchTaskEditor
          task={editor.task ?? undefined}
          targets={targets}
          onSave={handleSaveTask}
          onCancel={handleCancelEdit}
        />
      )}

      {runner.open && runner.task && (
        <BatchTaskRunner
          task={runner.task}
          targets={targets}
          onExecute={handleRunBatch}
          onCancel={handleCancelRun}
        />
      )}
    </>
  );
};