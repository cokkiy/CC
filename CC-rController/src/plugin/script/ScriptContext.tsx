/**
 * ScriptContext - React Context for Script Management
 * Part of Phase 6: Command Script Management System
 */

import React, { createContext, useContext, useState, useCallback, useEffect } from 'react';
import { scriptHost } from './ScriptHost';
import type {
  CommandScript,
  ScriptFilter,
  ScriptExecutionContext,
  ScriptExecutionResult,
  ScriptPackage,
  ScriptImportResult,
  ScriptLogEntry,
  ScriptPackageMetadata,
} from './types';

// Target type for execution
interface Target {
  id: string;
  name: string;
  status?: string;
}

// Context state interface
interface ScriptContextState {
  scripts: CommandScript[];
  selectedScript: CommandScript | null;
  isLoading: boolean;
  error: string | null;
  executionHistory: ScriptExecutionResult[];
  logs: ScriptLogEntry[];
}

// Context value interface
interface ScriptContextValue extends ScriptContextState {
  // CRUD operations
  loadScripts: () => Promise<void>;
  saveScript: (script: Partial<CommandScript>) => Promise<CommandScript>;
  deleteScript: (scriptId: string) => Promise<void>;
  duplicateScript: (scriptId: string) => Promise<CommandScript>;

  // Filtering
  filterScripts: (filter: ScriptFilter) => CommandScript[];
  setSelectedScript: (script: CommandScript | null) => void;

  // Favorites
  toggleFavorite: (scriptId: string) => Promise<void>;

  // Execution
  executeScript: (context: ScriptExecutionContext) => Promise<ScriptExecutionResult[]>;
  cancelExecution: (executionId: string) => Promise<void>;

  // Import/Export
  importScripts: (pkg: ScriptPackage, options?: object) => Promise<ScriptImportResult>;
  exportScripts: (scriptIds: string[], metadata?: object) => Promise<ScriptPackage>;

  // Utilities
  getAvailableTargets: () => Promise<Target[]>;
  getStats: () => object;
}

const ScriptContext = createContext<ScriptContextValue | null>(null);

export function ScriptProvider({ children }: { children: React.ReactNode }) {
  const [state, setState] = useState<ScriptContextState>({
    scripts: [],
    selectedScript: null,
    isLoading: false,
    error: null,
    executionHistory: [],
    logs: [],
  });

  // Initialize and load scripts
  useEffect(() => {
    scriptHost.init();
    loadScripts();

    // Subscribe to events
    const unsubLoad = scriptHost.on('scripts:loaded', () => {
      setState(prev => ({ ...prev, scripts: scriptHost.getAllScripts() }));
    });

    const unsubLog = scriptHost.on('log', (entry: ScriptLogEntry) => {
      setState(prev => ({ ...prev, logs: [...prev.logs, entry] }));
    });

    return () => {
      unsubLoad();
      unsubLog();
    };
  }, []);

  const loadScripts = useCallback(async () => {
    setState(prev => ({ ...prev, isLoading: true, error: null }));
    try {
      await scriptHost.loadAll();
      setState(prev => ({
        ...prev,
        scripts: scriptHost.getAllScripts(),
        isLoading: false,
      }));
    } catch (err) {
      setState(prev => ({
        ...prev,
        error: err instanceof Error ? err.message : 'Failed to load scripts',
        isLoading: false,
      }));
    }
  }, []);

  const saveScript = useCallback(async (script: Partial<CommandScript>) => {
    const result = script.id
      ? await scriptHost.updateScript(script.id, script)
      : await scriptHost.createScript(script);

    setState(prev => ({
      ...prev,
      scripts: scriptHost.getAllScripts(),
    }));

    return result;
  }, []);

  const deleteScript = useCallback(async (scriptId: string) => {
    await scriptHost.deleteScript(scriptId);
    setState(prev => ({
      ...prev,
      scripts: scriptHost.getAllScripts(),
      selectedScript: prev.selectedScript?.id === scriptId ? null : prev.selectedScript,
    }));
  }, []);

  const duplicateScript = useCallback(async (scriptId: string) => {
    const result = await scriptHost.duplicateScript(scriptId);
    setState(prev => ({
      ...prev,
      scripts: scriptHost.getAllScripts(),
    }));
    return result;
  }, []);

  const filterScripts = useCallback((filter: ScriptFilter) => {
    return scriptHost.filterScripts(filter);
  }, []);

  const setSelectedScript = useCallback((script: CommandScript | null) => {
    setState(prev => ({ ...prev, selectedScript: script }));
  }, []);

  const toggleFavorite = useCallback(async (scriptId: string) => {
    await scriptHost.toggleFavorite(scriptId);
    setState(prev => ({
      ...prev,
      scripts: scriptHost.getAllScripts(),
    }));
  }, []);

  const executeScript = useCallback(async (context: ScriptExecutionContext) => {
    const results = await scriptHost.executeScript(context);
    setState(prev => ({
      ...prev,
      executionHistory: [...prev.executionHistory, ...results],
    }));
    return results;
  }, []);

  const cancelExecution = useCallback(async (executionId: string) => {
    await scriptHost.cancelExecution(executionId);
  }, []);

  const importScripts = useCallback(async (
    pkg: ScriptPackage,
    options?: { overwrite?: boolean; importAsCopies?: boolean }
  ) => {
    const result = await scriptHost.importScripts(pkg, options);
    setState(prev => ({
      ...prev,
      scripts: scriptHost.getAllScripts(),
    }));
    return result;
  }, []);

  const exportScripts = useCallback(async (
    scriptIds: string[],
    metadata?: Partial<ScriptPackageMetadata>
  ) => {
    return scriptHost.exportScripts(scriptIds, metadata);
  }, []);

  const getAvailableTargets = useCallback(async () => {
    // TODO: Implement getting available targets from backend
    return [];
  }, []);

  const getStats = useCallback(() => {
    return scriptHost.getStats();
  }, []);

  const value: ScriptContextValue = {
    ...state,
    loadScripts,
    saveScript,
    deleteScript,
    duplicateScript,
    filterScripts,
    setSelectedScript,
    toggleFavorite,
    executeScript,
    cancelExecution,
    importScripts,
    exportScripts,
    getAvailableTargets,
    getStats,
  };

  return (
    <ScriptContext.Provider value={value}>
      {children}
    </ScriptContext.Provider>
  );
}

/**
 * Hook to access script context
 * @throws Error if used outside of ScriptProvider
 */
export function useScripts() {
  const context = useContext(ScriptContext);
  if (!context) {
    throw new Error('useScripts must be used within a ScriptProvider');
  }
  return context;
}

/**
 * Hook to access script context with error handling
 * Returns null if used outside of ScriptProvider
 */
export function useScriptsSafe() {
  return useContext(ScriptContext);
}
