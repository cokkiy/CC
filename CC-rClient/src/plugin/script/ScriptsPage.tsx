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
    <>
      <main className="grid">
        <section className="panel" style={{ flex: 1 }}>
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
        <ScriptEditor
          script={editor.script ?? undefined}
          onSave={handleSaveScript}
          onCancel={handleCancelEdit}
        />
      )}

      {runner.open && runner.script && (
        <ScriptRunner
          script={runner.script}
          targets={targets}
          onExecute={handleRunScripts}
          onCancel={handleCancelRun}
        />
      )}

      {marketplace.open && (
        <ScriptMarketplace
          scripts={scripts}
          onImport={handleImportScripts}
          onExport={handleExportScripts}
          onClose={closeMarketplace}
        />
      )}
    </>
  );
};
