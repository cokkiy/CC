/**
 * Scripts Page - Main container for Script Management UI
 * Part of Phase 6: Frontend Integration
 */

import React, { useState } from 'react';
import { useScripts } from './ScriptContext';
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

  // Modal states
  const [scriptEditorOpen, setScriptEditorOpen] = useState(false);
  const [editingScript, setEditingScript] = useState<CommandScript | null>(null);
  const [scriptRunnerOpen, setScriptRunnerOpen] = useState(false);
  const [runningScript, setRunningScript] = useState<CommandScript | null>(null);
  const [scriptMarketplaceOpen, setScriptMarketplaceOpen] = useState(false);

  // Convert stations to targets for ScriptRunner
  const targets = stations.map(s => ({ id: s.id, name: s.name, status: s.blocked ? 'blocked' : 'ready' }));

  // ScriptList handlers
  const handleEditScript = (script: CommandScript | undefined) => {
    setEditingScript(script ?? null);
    setScriptEditorOpen(true);
  };

  const handleExecuteScript = (script: CommandScript) => {
    setRunningScript(script);
    setScriptRunnerOpen(true);
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
    setScriptEditorOpen(false);
    setEditingScript(null);
  };

  const handleCancelEdit = () => {
    setScriptEditorOpen(false);
    setEditingScript(null);
  };

  // ScriptRunner handlers
  const handleRunScripts = async (context: ScriptExecutionContext): Promise<ScriptExecutionResult[]> => {
    return executeScript(context);
  };

  const handleCancelRun = () => {
    setScriptRunnerOpen(false);
    setRunningScript(null);
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
            onImport={() => setScriptMarketplaceOpen(true)}
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

      {scriptEditorOpen && (
        <ScriptEditor
          script={editingScript ?? undefined}
          onSave={handleSaveScript}
          onCancel={handleCancelEdit}
        />
      )}

      {scriptRunnerOpen && runningScript && (
        <ScriptRunner
          script={runningScript}
          targets={targets}
          onExecute={handleRunScripts}
          onCancel={handleCancelRun}
        />
      )}

      {scriptMarketplaceOpen && (
        <ScriptMarketplace
          scripts={scripts}
          onImport={handleImportScripts}
          onExport={handleExportScripts}
          onClose={() => setScriptMarketplaceOpen(false)}
        />
      )}
    </>
  );
};
