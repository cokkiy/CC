/**
 * ScriptsUIContext - modal lifecycle + UI state for the Scripts feature
 *
 * Design goals:
 * - Keep ScriptProvider focused on data + Tauri actions
 * - Keep modal lifecycle centralized and explicit
 */

import React, { createContext, useCallback, useContext, useMemo, useState } from 'react';

import type { CommandScript } from './types';

interface ScriptEditorUIState {
  open: boolean;
  script: CommandScript | null;
}

interface ScriptRunnerUIState {
  open: boolean;
  script: CommandScript | null;
}

interface ScriptsMarketplaceUIState {
  open: boolean;
}

export interface ScriptsUIContextValue {
  editor: ScriptEditorUIState;
  runner: ScriptRunnerUIState;
  marketplace: ScriptsMarketplaceUIState;

  openEditor: (script?: CommandScript | null) => void;
  closeEditor: () => void;

  openRunner: (script: CommandScript) => void;
  closeRunner: () => void;

  openMarketplace: () => void;
  closeMarketplace: () => void;
}

const ScriptsUIContext = createContext<ScriptsUIContextValue | null>(null);

export function ScriptsUIProvider({ children }: { children: React.ReactNode }) {
  const [editor, setEditor] = useState<ScriptEditorUIState>({ open: false, script: null });
  const [runner, setRunner] = useState<ScriptRunnerUIState>({ open: false, script: null });
  const [marketplace, setMarketplace] = useState<ScriptsMarketplaceUIState>({ open: false });

  // Ensure we don't end up with multiple heavy modals open at once.
  const openEditor = useCallback((script?: CommandScript | null) => {
    setRunner({ open: false, script: null });
    setMarketplace({ open: false });
    setEditor({ open: true, script: script ?? null });
  }, []);

  const closeEditor = useCallback(() => {
    setEditor((prev) => ({ ...prev, open: false, script: null }));
  }, []);

  const openRunner = useCallback((script: CommandScript) => {
    setEditor({ open: false, script: null });
    setMarketplace({ open: false });
    setRunner({ open: true, script });
  }, []);

  const closeRunner = useCallback(() => {
    setRunner((prev) => ({ ...prev, open: false, script: null }));
  }, []);

  const openMarketplace = useCallback(() => {
    setEditor({ open: false, script: null });
    setRunner({ open: false, script: null });
    setMarketplace({ open: true });
  }, []);

  const closeMarketplace = useCallback(() => {
    setMarketplace({ open: false });
  }, []);

  const value = useMemo<ScriptsUIContextValue>(
    () => ({
      editor,
      runner,
      marketplace,
      openEditor,
      closeEditor,
      openRunner,
      closeRunner,
      openMarketplace,
      closeMarketplace,
    }),
    [editor, runner, marketplace, openEditor, closeEditor, openRunner, closeRunner, openMarketplace, closeMarketplace]
  );

  return <ScriptsUIContext.Provider value={value}>{children}</ScriptsUIContext.Provider>;
}

export function useScriptsUI() {
  const ctx = useContext(ScriptsUIContext);
  if (!ctx) throw new Error('useScriptsUI must be used within a ScriptsUIProvider');
  return ctx;
}
