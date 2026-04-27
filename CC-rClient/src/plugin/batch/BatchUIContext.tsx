/**
 * BatchUIContext - modal lifecycle + UI state for the Batch feature
 *
 * Design goals:
 * - Keep BatchProvider focused on data + Tauri actions
 * - Keep modal lifecycle centralized and explicit
 * - Follows the pattern established by ScriptsUIContext
 */

import React, { createContext, useCallback, useContext, useMemo, useState } from 'react';

import type { BatchTask } from './types';

interface BatchEditorUIState {
  open: boolean;
  task: BatchTask | null;
  isNew: boolean;
}

interface BatchRunnerUIState {
  open: boolean;
  task: BatchTask | null;
}

interface BatchImportExportUIState {
  open: boolean;
}

export interface BatchUIContextValue {
  editor: BatchEditorUIState;
  runner: BatchRunnerUIState;
  importExport: BatchImportExportUIState;

  openEditor: (task?: BatchTask | null) => void;
  closeEditor: () => void;

  openRunner: (task: BatchTask) => void;
  closeRunner: () => void;

  openImportExport: () => void;
  closeImportExport: () => void;
}

const BatchUIContext = createContext<BatchUIContextValue | null>(null);

export function BatchUIProvider({ children }: { children: React.ReactNode }) {
  const [editor, setEditor] = useState<BatchEditorUIState>({ open: false, task: null, isNew: false });
  const [runner, setRunner] = useState<BatchRunnerUIState>({ open: false, task: null });
  const [importExport, setImportExport] = useState<BatchImportExportUIState>({ open: false });

  // Ensure we don't end up with multiple heavy modals open at once.
  const openEditor = useCallback((task?: BatchTask | null) => {
    setRunner({ open: false, task: null });
    setImportExport({ open: false });
    setEditor({ open: true, task: task ?? null, isNew: !task });
  }, []);

  const closeEditor = useCallback(() => {
    setEditor((prev) => ({ ...prev, open: false, task: null }));
  }, []);

  const openRunner = useCallback((task: BatchTask) => {
    setEditor({ open: false, task: null, isNew: false });
    setImportExport({ open: false });
    setRunner({ open: true, task });
  }, []);

  const closeRunner = useCallback(() => {
    setRunner((prev) => ({ ...prev, open: false, task: null }));
  }, []);

  const openImportExport = useCallback(() => {
    setEditor({ open: false, task: null, isNew: false });
    setRunner({ open: false, task: null });
    setImportExport({ open: true });
  }, []);

  const closeImportExport = useCallback(() => {
    setImportExport((prev) => ({ ...prev, open: false }));
  }, []);

  const value = useMemo<BatchUIContextValue>(
    () => ({
      editor,
      runner,
      importExport,
      openEditor,
      closeEditor,
      openRunner,
      closeRunner,
      openImportExport,
      closeImportExport,
    }),
    [editor, runner, importExport, openEditor, closeEditor, openRunner, closeRunner, openImportExport, closeImportExport]
  );

  return <BatchUIContext.Provider value={value}>{children}</BatchUIContext.Provider>;
}

export function useBatchUI() {
  const ctx = useContext(BatchUIContext);
  if (!ctx) throw new Error('useBatchUI must be used within a BatchUIProvider');
  return ctx;
}