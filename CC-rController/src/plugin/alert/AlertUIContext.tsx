/**
 * AlertUIContext - modal lifecycle + UI state for the Alert Rules feature
 *
 * Design goals:
 * - Keep AlertProvider focused on data + Tauri actions
 * - Keep modal lifecycle centralized and explicit
 * - Follows the pattern established by ScriptsUIContext and BatchUIContext
 */

import React, { createContext, useCallback, useContext, useMemo, useState } from 'react';

import type { AlertRule } from './types';

interface AlertEditorUIState {
  open: boolean;
  rule: AlertRule | null;
  isNew: boolean;
}

interface AlertRunnerUIState {
  open: boolean;
  rule: AlertRule | null;
}

interface AlertHistoryUIState {
  open: boolean;
  filterRuleId?: string;
}

interface AlertImportExportUIState {
  open: boolean;
}

export interface AlertUIContextValue {
  editor: AlertEditorUIState;
  runner: AlertRunnerUIState;
  history: AlertHistoryUIState;
  importExport: AlertImportExportUIState;

  // Editor controls
  openEditor: (rule?: AlertRule | null) => void;
  closeEditor: () => void;

  // Runner controls
  openRunner: (rule: AlertRule) => void;
  closeRunner: () => void;

  // History panel controls
  openHistory: (filterRuleId?: string) => void;
  closeHistory: () => void;

  // Import/Export controls
  openImportExport: () => void;
  closeImportExport: () => void;
}

const AlertUIContext = createContext<AlertUIContextValue | null>(null);

export function AlertUIProvider({ children }: { children: React.ReactNode }) {
  const [editor, setEditor] = useState<AlertEditorUIState>({ open: false, rule: null, isNew: false });
  const [runner, setRunner] = useState<AlertRunnerUIState>({ open: false, rule: null });
  const [history, setHistory] = useState<AlertHistoryUIState>({ open: false, filterRuleId: undefined });
  const [importExport, setImportExport] = useState<AlertImportExportUIState>({ open: false });

  // Ensure we don't end up with multiple heavy modals open at once.
  const openEditor = useCallback((rule?: AlertRule | null) => {
    setRunner({ open: false, rule: null });
    setHistory({ open: false, filterRuleId: undefined });
    setImportExport({ open: false });
    setEditor({ open: true, rule: rule ?? null, isNew: !rule });
  }, []);

  const closeEditor = useCallback(() => {
    setEditor((prev) => ({ ...prev, open: false, rule: null }));
  }, []);

  const openRunner = useCallback((rule: AlertRule) => {
    setEditor({ open: false, rule: null, isNew: false });
    setHistory({ open: false, filterRuleId: undefined });
    setImportExport({ open: false });
    setRunner({ open: true, rule });
  }, []);

  const closeRunner = useCallback(() => {
    setRunner((prev) => ({ ...prev, open: false, rule: null }));
  }, []);

  const openHistory = useCallback((filterRuleId?: string) => {
    setEditor({ open: false, rule: null, isNew: false });
    setRunner({ open: false, rule: null });
    setImportExport({ open: false });
    setHistory({ open: true, filterRuleId });
  }, []);

  const closeHistory = useCallback(() => {
    setHistory((prev) => ({ ...prev, open: false }));
  }, []);

  const openImportExport = useCallback(() => {
    setEditor({ open: false, rule: null, isNew: false });
    setRunner({ open: false, rule: null });
    setHistory({ open: false, filterRuleId: undefined });
    setImportExport({ open: true });
  }, []);

  const closeImportExport = useCallback(() => {
    setImportExport((prev) => ({ ...prev, open: false }));
  }, []);

  const value = useMemo<AlertUIContextValue>(
    () => ({
      editor,
      runner,
      history,
      importExport,
      openEditor,
      closeEditor,
      openRunner,
      closeRunner,
      openHistory,
      closeHistory,
      openImportExport,
      closeImportExport,
    }),
    [editor, runner, history, importExport, openEditor, closeEditor, openRunner, closeRunner, openHistory, closeHistory, openImportExport, closeImportExport]
  );

  return <AlertUIContext.Provider value={value}>{children}</AlertUIContext.Provider>;
}

export function useAlertsUI() {
  const ctx = useContext(AlertUIContext);
  if (!ctx) throw new Error('useAlertsUI must be used within an AlertUIProvider');
  return ctx;
}
