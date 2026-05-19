/**
 * Scripts Page - Main container for Script Management UI
 * Part of Phase 6: Frontend Integration
 */

import React from 'react';
import { useScripts } from './ScriptContext';
import { useScriptsUI } from './ScriptsUIContext';
import { ScriptList, ScriptEditor, ScriptRunner, ScriptMarketplace } from './index';
import type { CommandScript, ScriptExecutionContext, ScriptExecutionResult, ScriptPackage, ScriptImportResult } from './types';
import type { Station } from '../../types';

export interface ScriptsPageProps {
  stations: Station[];
}

export const ScriptsPage: React.FC<ScriptsPageProps> = ({ stations }) => {
  const {
    scripts,
    saveScript,
    deleteScript,
    duplicateScript,
    toggleFavorite,
    executeScript,
    cancelExecution,
    importScripts,
    exportScripts,
  } = useScripts();

  const { 
    editor,
    runner,
    marketplace,
    openEditor,
    closeEditor,
    openRunner,
    closeRunner,
    openMarketplace,
    closeMarketplace,
  } = useScriptsUI();

  // Convert stations to targets for ScriptRunner
  const targets = stations.map(s => ({ id: s.id, name: s.name, status: s.blocked ? 'blocked' : 'ready' }));

  // ScriptList handlers
  const handleEditScript = (script: CommandScript | undefined) => {
    openEditor(script ?? null);
  };

  const handleExecuteScript = (script: CommandScript) => {
    openRunner(script);
  };

  const handleDeleteScript = async (scriptId: string) => {
    await deleteScript(scriptId);
  };

  const handleToggleFavorite = async (scriptId: string) => {
    await toggleFavorite(scriptId);
  };

  const handleDuplicateScript = async (script: CommandScript) => {
    await duplicateScript(script.id);
  };

  // ScriptEditor handlers
  const handleSaveScript = async (script: Partial<CommandScript>) => {
    await saveScript(script);
    closeEditor();
  };

  const handleCancelEdit = () => {
    closeEditor();
  };

  // ScriptRunner handlers
  const handleRunScripts = async (context: ScriptExecutionContext): Promise<ScriptExecutionResult[]> => {
    return executeScript(context);
  };

  const handleCancelRun = () => {
    closeRunner();
  };

  // ScriptMarketplace handlers
  const handleImportScripts = async (pkg: ScriptPackage): Promise<ScriptImportResult> => {
    return importScripts(pkg);
  };

  const handleExportScripts = async (pkg: ScriptPackage): Promise<void> => {
    await exportScripts(pkg.scripts.map(s => s.id));
  };

  return (
    <div className="scripts-page-shell">
      <main className="grid gridScriptsMode scripts-main-grid">
        <section className="panel scripts-main-panel">
          <ScriptList
            scripts={scripts}
            onEditScript={handleEditScript}
            onExecuteScript={handleExecuteScript}
            onDeleteScript={handleDeleteScript}
            onToggleFavorite={handleToggleFavorite}
            onDuplicateScript={handleDuplicateScript}
            onImport={openMarketplace}
            onExport={(selectedScripts) => {
              const pkg: ScriptPackage = {
                metadata: {
                  id: crypto.randomUUID(),
                  name: 'Exported Scripts',
                  version: '1.0.0',
                  tags: [],
                  createdAt: new Date().toISOString(),
                  updatedAt: new Date().toISOString(),
                  categories: [],
                },
                scripts: selectedScripts,
              };
              handleExportScripts(pkg);
            }}
          />
        </section>
      </main>

      {editor.open && (
        <div className="scripts-layer scripts-layer-drawer" role="dialog" aria-modal="true">
          <div className="scripts-drawer-panel">
            <ScriptEditor
              script={editor.script ?? undefined}
              onSave={handleSaveScript}
              onCancel={handleCancelEdit}
            />
          </div>
        </div>
      )}

      {runner.open && runner.script && (
        <div className="scripts-layer" role="dialog" aria-modal="true">
          <div className="scripts-modal-panel">
            <ScriptRunner
              script={runner.script}
              targets={targets}
              onExecute={handleRunScripts}
              onCancel={handleCancelRun}
            />
          </div>
        </div>
      )}

      {marketplace.open && (
        <ScriptMarketplace
          scripts={scripts}
          onImport={handleImportScripts}
          onExport={handleExportScripts}
          onClose={closeMarketplace}
        />
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

        .scripts-modal-panel {
          width: min(1320px, 96vw);
          height: min(90vh, 920px);
          background: var(--bg-card);
          border: 1px solid var(--border-color);
          border-radius: 14px;
          box-shadow: 0 22px 56px rgba(8, 20, 38, 0.24);
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

          .scripts-modal-panel {
            width: 100%;
            height: 100%;
            border-radius: 0;
          }
        }
      `}</style>
    </div>
  );
};
