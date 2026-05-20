/**
 * Script System API - Tauri Backend Communication
 * Part of Phase 6: Command Script Management System
 */

import { invoke } from '@tauri-apps/api/core';
import type {
  CommandScript,
  ScriptExecutionContext,
  ScriptExecutionResult,
  ScriptPackage,
  ScriptImportResult,
  ScriptPackageMetadata,
} from './types';

// API function type definitions
interface LoadScriptsResult {
  scripts: CommandScript[];
}

interface SaveScriptResult {
  script: CommandScript;
}

interface DeleteScriptResult {
  success: boolean;
}

interface ImportScriptPackageResult {
  success: boolean;
  imported: number;
  skipped: number;
  errors: string[];
  warnings: string[];
}

interface ExportScriptPackageResult {
  package: ScriptPackage;
}

interface CancelScriptExecutionResult {
  success: boolean;
}

interface GetAvailableTargetsResult {
  targets: Array<{
    id: string;
    name: string;
    status?: string;
  }>;
}

/**
 * Script API - Tauri backend communication layer
 * Provides type-safe wrappers around Tauri invoke calls
 */
export const scriptApi = {
  /**
   * Load all scripts from the backend
   */
  async loadScripts(): Promise<CommandScript[]> {
    try {
      const result = await invoke<LoadScriptsResult>('load_scripts');
      return result.scripts;
    } catch (error) {
      console.error('[ScriptApi] Failed to load scripts:', error);
      throw error;
    }
  },

  /**
   * Save a script (create or update)
   */
  async saveScript(script: Partial<CommandScript>): Promise<CommandScript> {
    try {
      const result = await invoke<SaveScriptResult>('save_script', { script });
      return result.script;
    } catch (error) {
      console.error('[ScriptApi] Failed to save script:', error);
      throw error;
    }
  },

  /**
   * Delete a script by ID
   */
  async deleteScript(scriptId: string): Promise<void> {
    try {
      await invoke<DeleteScriptResult>('delete_script', { script_id: scriptId });
    } catch (error) {
      console.error('[ScriptApi] Failed to delete script:', error);
      throw error;
    }
  },

  /**
   * Execute a script on specified stations
   */
  async executeScript(context: ScriptExecutionContext): Promise<ScriptExecutionResult[]> {
    try {
      const result = await invoke<{ results: ScriptExecutionResult[] }>('execute_script', {
        context,
      });
      return result.results;
    } catch (error) {
      console.error('[ScriptApi] Failed to execute script:', error);
      throw error;
    }
  },

  /**
   * Cancel a running script execution
   */
  async cancelScriptExecution(executionId: string): Promise<void> {
    try {
      await invoke<CancelScriptExecutionResult>('cancel_script_execution', {
        execution_id: executionId,
      });
    } catch (error) {
      console.error('[ScriptApi] Failed to cancel script execution:', error);
      throw error;
    }
  },

  /**
   * Import a script package
   */
  async importScriptPackage(
    pkg: ScriptPackage,
    options?: { overwrite?: boolean; importAsCopies?: boolean }
  ): Promise<ScriptImportResult> {
    try {
      const result = await invoke<ImportScriptPackageResult>('import_script_package', {
        pkg,
        options,
      });
      return {
        success: result.success,
        imported: result.imported,
        skipped: result.skipped,
        errors: result.errors,
        warnings: result.warnings,
      };
    } catch (error) {
      console.error('[ScriptApi] Failed to import script package:', error);
      throw error;
    }
  },

  /**
   * Export scripts as a package
   */
  async exportScriptPackage(
    scripts: CommandScript[],
    metadata?: Partial<ScriptPackageMetadata>
  ): Promise<ScriptPackage> {
    try {
      const result = await invoke<ExportScriptPackageResult>('export_script_package', {
        scripts,
        metadata,
      });
      return result.package;
    } catch (error) {
      console.error('[ScriptApi] Failed to export script package:', error);
      throw error;
    }
  },

  /**
   * Get available execution targets (stations)
   */
  async getAvailableTargets(): Promise<Array<{ id: string; name: string; status?: string }>> {
    try {
      const result = await invoke<GetAvailableTargetsResult>('get_available_targets');
      return result.targets;
    } catch (error) {
      console.error('[ScriptApi] Failed to get available targets:', error);
      throw error;
    }
  },

  /**
   * Validate script content without saving
   */
  async validateScript(script: Partial<CommandScript>): Promise<{ valid: boolean; errors: string[] }> {
    try {
      const result = await invoke<{ valid: boolean; errors: string[] }>('validate_script', {
        script,
      });
      return result;
    } catch (error) {
      console.error('[ScriptApi] Failed to validate script:', error);
      throw error;
    }
  },

  /**
   * Get script execution history
   */
  async getExecutionHistory(limit?: number): Promise<ScriptExecutionResult[]> {
    try {
      const result = await invoke<{ history: ScriptExecutionResult[] }>('get_script_history', {
        limit,
      });
      return result.history;
    } catch (error) {
      console.error('[ScriptApi] Failed to get execution history:', error);
      throw error;
    }
  },
};

// Export as default
export default scriptApi;
