/**
 * BatchPage - Main container for Batch Operations Management UI
 * Part of Phase 7: Frontend Integration
 */

import React, { useEffect, useState } from 'react';
import { useBatch } from './BatchContext';
import { useBatchUI } from './BatchUIContext';
import { BatchTaskList, BatchTaskEditor } from './index';
import type { BatchTask, BatchTarget } from './types';
import { scriptHost } from '../script/ScriptHost';
import type { CommandScript } from '../script/types';
import { groupsApi } from '../groups/api';
import type { StationGroup } from '../groups/types';
import type { Station } from '../../types';

export interface BatchPageProps {
  stations: Station[];
}

export const BatchPage: React.FC<BatchPageProps> = ({ stations }) => {
  const [scripts, setScripts] = useState<CommandScript[]>([]);
  const [groups, setGroups] = useState<StationGroup[]>([]);
  const {
    tasks,
    saveTask,
    deleteTask,
    duplicateTask,
    toggleFavorite,
    importTasks,
    exportTasks,
  } = useBatch();

  const { 
    editor,
    openEditor,
    closeEditor,
    openRunner,
  } = useBatchUI();

  useEffect(() => {
    let mounted = true;

    const loadSupportingData = async () => {
      try {
        const [, loadedGroups] = await Promise.all([
          scriptHost.init().then(() => scriptHost.loadAll()),
          groupsApi.loadGroups(),
        ]);
        if (mounted) {
          setScripts(scriptHost.getAllScripts());
          setGroups(loadedGroups);
        }
      } catch (error) {
        console.error('[BatchPage] Failed to load supporting selector data:', error);
      }
    };

    void loadSupportingData();

    const unsubscribe = scriptHost.on('scripts:loaded', () => {
      if (mounted) {
        setScripts(scriptHost.getAllScripts());
      }
    });

    return () => {
      mounted = false;
      unsubscribe();
    };
  }, []);

  // Convert stations to targets for BatchTaskRunner
  // Convert stations to batch targets
  const targets: BatchTarget[] = stations.map(s => ({ 
    id: s.id, 
    name: s.name, 
    status: s.blocked ? 'offline' as const : 'online' as const,
    group: s.groups[0],
    groups: s.groups,
    tags: s.tags,
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

  const handleToggleToolbarPin = async (task: BatchTask) => {
    await saveTask({
      ...task,
      showInToolbar: !(task.showInToolbar ?? false),
    });
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

  return (
    <div className="scripts-page-shell">
      <main className="grid gridScriptsMode scripts-main-grid">
        <section className="panel scripts-main-panel">
          <BatchTaskList
            tasks={tasks}
            onSelectTask={(task) => openEditor(task)}
            onEditTask={handleEditTask}
            onExecuteTask={handleExecuteTask}
            onDeleteTask={handleDeleteTask}
            onToggleFavorite={handleToggleFavorite}
            onToggleToolbarPin={handleToggleToolbarPin}
            onDuplicateTask={handleDuplicateTask}
          />
        </section>
      </main>

      {editor.open && (
        <div className="scripts-layer scripts-layer-drawer" role="dialog" aria-modal="true">
          <div className="scripts-drawer-panel">
            <BatchTaskEditor
              task={editor.task ?? undefined}
              targets={targets}
              groups={groups}
              scripts={scripts}
              onSave={handleSaveTask}
              onCancel={handleCancelEdit}
            />
          </div>
        </div>
      )}

      <style>{`
        .scripts-page-shell {
          position: relative;
          min-height: calc(100vh - 64px - 48px);
        }

        .scripts-main-grid {
          height: 100%;
        }

        .scripts-main-panel {
          padding: 0;
          overflow: hidden;
          min-height: calc(100vh - 64px - 48px);
        }

        .scripts-layer {
          position: fixed;
          inset: 0;
          background: rgba(11, 25, 44, 0.42);
          backdrop-filter: blur(2px);
          display: flex;
          align-items: center;
          justify-content: center;
          z-index: 1200;
          padding: 24px;
        }

        .scripts-layer-drawer {
          justify-content: flex-end;
          padding: 0;
        }

        .scripts-drawer-panel {
          width: min(980px, 94vw);
          height: 100vh;
          background: var(--bg-card);
          border-left: 1px solid var(--border-color);
          box-shadow: -18px 0 48px rgba(8, 20, 38, 0.24);
        }

        .scripts-drawer-panel .batch-task-editor.modal-overlay {
          position: static;
          inset: auto;
          background: transparent;
          padding: 0;
          height: 100%;
          display: flex;
        }

        .scripts-drawer-panel .batch-task-editor .modal-content.editor-modal {
          width: 100%;
          max-width: none;
          max-height: none;
          height: 100%;
          margin: 0;
          border: none;
          border-radius: 0;
          box-shadow: none;
          display: flex;
          flex-direction: column;
          overflow: hidden;
        }

        @media (max-width: 1024px) {
          .scripts-layer {
            padding: 12px;
          }

          .scripts-layer-drawer {
            padding: 0;
          }

          .scripts-drawer-panel {
            width: 100vw;
          }
        }
      `}</style>
    </div>
  );
};
