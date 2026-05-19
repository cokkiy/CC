/**
 * ScriptHost - Unified entry point for script management system
 * Part of Phase 6: Command Script Management System
 * References PluginHost design pattern
 */

import { invoke } from '@tauri-apps/api/core';
import type {
  CommandScript,
  ScriptParameter,
  ScriptExecutionContext,
  ScriptExecutionResult,
  ScriptPackage,
  ScriptImportResult,
  ScriptFilter,
  ScriptValidationResult,
  ScriptLogEntry,
  ScriptExecutionStatus,
} from './types';

// Script host configuration
export interface ScriptHostConfig {
  autoLoad?: boolean;           // Auto load scripts on init
  maxHistory?: number;          // Max execution history records
  defaultTimeout?: number;      // Default timeout (ms)
  enableLogging?: boolean;      // Enable detailed logging
}

// Script execution tracking
interface ExecutionTrack {
  id: string;
  scriptId: string;
  status: ScriptExecutionStatus;
  startTime: number;
  results: ScriptExecutionResult[];
}

/**
 * ScriptHost - Script system unified entry facade
 * References PluginHost design pattern
 */
export class ScriptHost {
  private static instance: ScriptHost | null = null;
  private initialized: boolean = false;
  private config: ScriptHostConfig;

  // Sub-modules - similar to PluginHost's plugins, panels, actions, data
  private scripts: Map<string, CommandScript> = new Map();
  private executionTracks: Map<string, ExecutionTrack> = new Map();
  private executionHistory: ScriptLogEntry[] = [];
  private listeners: Map<string, Set<Function>> = new Map();

  private constructor(config: ScriptHostConfig = {}) {
    this.config = {
      autoLoad: true,
      maxHistory: 100,
      defaultTimeout: 60000,
      enableLogging: true,
      ...config,
    };
  }

  public static getInstance(config?: ScriptHostConfig): ScriptHost {
    if (!ScriptHost.instance) {
      ScriptHost.instance = new ScriptHost(config);
    }
    return ScriptHost.instance;
  }

  get isInitialized(): boolean {
    return this.initialized;
  }

  // ==================== Initialization ====================

  async init(): Promise<void> {
    if (this.initialized) {
      console.warn('[ScriptHost] Already initialized');
      return;
    }

    console.log('[ScriptHost] Initializing...');
    this.setupEventForwarding();
    this.initialized = true;
    console.log('[ScriptHost] Initialized successfully');
  }

  async loadAll(): Promise<void> {
    if (!this.initialized) {
      await this.init();
    }
    await this.loadScriptsFromBackend();
  }

  // ==================== Script CRUD ====================

  /**
   * Load all scripts from backend
   */
  async loadScriptsFromBackend(): Promise<CommandScript[]> {
    try {
      const scripts = await invoke<CommandScript[]>('load_scripts');
      this.scripts.clear();
      scripts.forEach(script => this.scripts.set(script.id, script));
      this.emit('scripts:loaded', { count: scripts.length });
      return scripts;
    } catch (error) {
      console.error('[ScriptHost] Failed to load scripts:', error);
      this.emit('scripts:error', { error });
      throw error;
    }
  }

  /**
   * Get all scripts
   */
  getAllScripts(): CommandScript[] {
    return Array.from(this.scripts.values());
  }

  /**
   * Get script by ID
   */
  getScript(id: string): CommandScript | null {
    return this.scripts.get(id) ?? null;
  }

  /**
   * Filter scripts
   */
  filterScripts(filter: ScriptFilter): CommandScript[] {
    let results = Array.from(this.scripts.values());

    if (filter.search) {
      const search = filter.search.toLowerCase();
      results = results.filter(s =>
        s.name.toLowerCase().includes(search) ||
        s.description.toLowerCase().includes(search) ||
        s.tags.some(t => t.toLowerCase().includes(search))
      );
    }

    if (filter.scriptType) {
      results = results.filter(s => s.scriptType === filter.scriptType);
    }

    if (filter.tags?.length) {
      results = results.filter(s =>
        filter.tags!.some(t => s.tags.includes(t))
      );
    }

    if (filter.isTemplate !== undefined) {
      results = results.filter(s => s.isTemplate === filter.isTemplate);
    }

    if (filter.isFavorite !== undefined) {
      results = results.filter(s => s.isFavorite === filter.isFavorite);
    }

    return results;
  }

  /**
   * Create new script
   */
  async createScript(script: Partial<CommandScript>): Promise<CommandScript> {
    const validation = this.validateScript(script);
    if (!validation.valid) {
      throw new Error(`Invalid script: ${validation.errors.join(', ')}`);
    }

    try {
      const saved = await invoke<CommandScript>('save_script', { script });
      this.scripts.set(saved.id, saved);
      this.emit('script:created', { script: saved });
      return saved;
    } catch (error) {
      console.error('[ScriptHost] Failed to create script:', error);
      throw error;
    }
  }

  /**
   * Update script
   */
  async updateScript(id: string, updates: Partial<CommandScript>): Promise<CommandScript> {
    const existing = this.scripts.get(id);
    if (!existing) {
      throw new Error(`Script ${id} not found`);
    }

    const updated = { ...existing, ...updates, id, updatedAt: new Date().toISOString() };
    const validation = this.validateScript(updated);
    if (!validation.valid) {
      throw new Error(`Invalid script: ${validation.errors.join(', ')}`);
    }

    try {
      const saved = await invoke<CommandScript>('save_script', { script: updated });
      this.scripts.set(saved.id, saved);
      this.emit('script:updated', { script: saved });
      return saved;
    } catch (error) {
      console.error('[ScriptHost] Failed to update script:', error);
      throw error;
    }
  }

  /**
   * Delete script
   */
  async deleteScript(id: string): Promise<void> {
    const existing = this.scripts.get(id);
    if (!existing) {
      throw new Error(`Script ${id} not found`);
    }

    try {
      await invoke('delete_script', { script_id: id });
      this.scripts.delete(id);
      this.emit('script:deleted', { scriptId: id });
    } catch (error) {
      console.error('[ScriptHost] Failed to delete script:', error);
      throw error;
    }
  }

  /**
   * Toggle favorite status
   */
  async toggleFavorite(id: string): Promise<boolean> {
    const script = this.scripts.get(id);
    if (!script) {
      throw new Error(`Script ${id} not found`);
    }

    const updated = await this.updateScript(id, { isFavorite: !script.isFavorite });
    return updated.isFavorite;
  }

  /**
   * Duplicate script
   */
  async duplicateScript(id: string): Promise<CommandScript> {
    const original = this.scripts.get(id);
    if (!original) {
      throw new Error(`Script ${id} not found`);
    }

    const copy: Partial<CommandScript> = {
      ...original,
      id: undefined, // Backend generates new ID
      name: `${original.name} (Copy)`,
      isFavorite: false,
      usageCount: 0,
      createdAt: new Date().toISOString(),
      updatedAt: new Date().toISOString(),
    };

    return this.createScript(copy);
  }

  // ==================== Script Validation ====================

  validateScript(script: Partial<CommandScript>): ScriptValidationResult {
    const errors: string[] = [];
    const warnings: string[] = [];
    const parameterErrors: Record<string, string[]> = {};

    // Basic validation
    if (!script.name?.trim()) {
      errors.push('Script name is required');
    }

    if (!script.scriptType) {
      errors.push('Script type is required');
    }

    if (!script.content?.trim()) {
      errors.push('Script content is required');
    }

    // Parameter validation
    if (script.parameters) {
      const paramNames = new Set<string>();
      script.parameters.forEach((param, index) => {
        if (!param.name?.trim()) {
          parameterErrors[`param_${index}`] = ['Parameter name is required'];
          errors.push(`Parameter ${index + 1}: name is required`);
        } else if (paramNames.has(param.name)) {
          parameterErrors[param.name] = ['Duplicate parameter name'];
          errors.push(`Parameter "${param.name}": duplicate name`);
        } else {
          paramNames.add(param.name);
        }

        if (param.validation) {
          try {
            new RegExp(param.validation);
          } catch {
            parameterErrors[param.name] = ['Invalid validation regex'];
            errors.push(`Parameter "${param.name}": invalid validation regex`);
          }
        }
      });
    }

    // Warnings
    if (script.parameters?.length === 0) {
      warnings.push('Script has no parameters defined');
    }

    if (!script.description?.trim()) {
      warnings.push('Script description is empty');
    }

    return {
      valid: errors.length === 0,
      errors,
      warnings,
      parameterErrors,
    };
  }

  // ==================== Script Execution ====================

  /**
   * Execute script
   */
  async executeScript(context: ScriptExecutionContext): Promise<ScriptExecutionResult[]> {
    const script = this.scripts.get(context.scriptId);
    if (!script) {
      throw new Error(`Script ${context.scriptId} not found`);
    }

    const executionId = `exec-${Date.now()}-${Math.random().toString(36).slice(2, 9)}`;

    // Create execution track
    const track: ExecutionTrack = {
      id: executionId,
      scriptId: context.scriptId,
      status: 'running',
      startTime: Date.now(),
      results: [],
    };
    this.executionTracks.set(executionId, track);

    this.emit('execution:started', {
      executionId,
      scriptId: context.scriptId,
      stationIds: context.stationIds,
    });

    this.log('info', `Executing script "${script.name}" on ${context.stationIds.length} station(s)`);

    try {
      const results = await invoke<ScriptExecutionResult[]>('execute_script', {
        context: {
          ...context,
          timeout: context.timeout ?? this.config.defaultTimeout,
        },
      });

      track.results = results;
      track.status = results.some(r => r.status === 'failed') ? 'failed' : 'success';

      this.emit('execution:completed', {
        executionId,
        scriptId: context.scriptId,
        results,
      });

      // Update script usage statistics
      if (script) {
        script.usageCount++;
        script.lastUsedAt = new Date().toISOString();
      }

      return results;
    } catch (error) {
      track.status = 'failed';
      this.emit('execution:failed', {
        executionId,
        scriptId: context.scriptId,
        error,
      });
      this.log('error', `Script execution failed: ${error}`);
      throw error;
    }
  }

  /**
   * Cancel execution
   */
  async cancelExecution(executionId: string): Promise<void> {
    const track = this.executionTracks.get(executionId);
    if (!track) {
      throw new Error(`Execution ${executionId} not found`);
    }

    try {
      await invoke('cancel_script_execution', { execution_id: executionId });
      track.status = 'cancelled';
      this.emit('execution:cancelled', { executionId });
      this.log('info', `Execution ${executionId} cancelled`);
    } catch (error) {
      console.error('[ScriptHost] Failed to cancel execution:', error);
      throw error;
    }
  }

  /**
   * Get execution result
   */
  getExecutionResult(executionId: string): ScriptExecutionResult[] | null {
    return this.executionTracks.get(executionId)?.results ?? null;
  }

  /**
   * Get execution status
   */
  getExecutionStatus(executionId: string): ScriptExecutionStatus | null {
    return this.executionTracks.get(executionId)?.status ?? null;
  }

  // ==================== Import/Export ====================

  /**
   * Import script package
   */
  async importScripts(
    pkg: ScriptPackage,
    options?: { overwrite?: boolean; importAsCopies?: boolean }
  ): Promise<ScriptImportResult> {
    try {
      const result = await invoke<ScriptImportResult>('import_script_package', { pkg, options });

      if (result.success) {
        // Reload scripts list
        await this.loadScriptsFromBackend();
        this.emit('scripts:imported', { result });
      }

      return result;
    } catch (error) {
      console.error('[ScriptHost] Failed to import scripts:', error);
      throw error;
    }
  }

  /**
   * Export script package
   */
  async exportScripts(
    scriptIds: string[],
    metadata?: Partial<ScriptPackage['metadata']>
  ): Promise<ScriptPackage> {
    const scripts = scriptIds
      .map(id => this.scripts.get(id))
      .filter((s): s is CommandScript => s !== null);

    if (scripts.length === 0) {
      throw new Error('No scripts to export');
    }

    try {
      const pkg = await invoke<ScriptPackage>('export_script_package', {
        scripts,
        metadata,
      });

      this.emit('scripts:exported', { count: scripts.length });
      return pkg;
    } catch (error) {
      console.error('[ScriptHost] Failed to export scripts:', error);
      throw error;
    }
  }

  // ==================== Logging ====================

  private log(level: 'info' | 'warning' | 'error', message: string, stationId?: string): void {
    if (!this.config.enableLogging) return;

    const entry: ScriptLogEntry = {
      timestamp: new Date().toISOString(),
      level,
      message,
      stationId,
    };

    this.executionHistory.push(entry);

    // Keep history within limit
    if (this.executionHistory.length > (this.config.maxHistory ?? 100)) {
      this.executionHistory = this.executionHistory.slice(-this.config.maxHistory!);
    }

    this.emit('log', entry);
  }

  getLogs(): ScriptLogEntry[] {
    return [...this.executionHistory];
  }

  // ==================== Event System ====================

  on(event: string, handler: Function): () => void {
    if (!this.listeners.has(event)) {
      this.listeners.set(event, new Set());
    }
    this.listeners.get(event)!.add(handler);

    return () => {
      this.listeners.get(event)?.delete(handler);
    };
  }

  off(event: string, handler: Function): void {
    this.listeners.get(event)?.delete(handler);
  }

  private emit(event: string, data?: unknown): void {
    this.listeners.get(event)?.forEach(handler => {
      try {
        handler(data);
      } catch (error) {
        console.error(`[ScriptHost] Event handler error for ${event}:`, error);
      }
    });

    // Wildcard handling
    this.listeners.get('*')?.forEach(handler => {
      try {
        handler({ type: event, data });
      } catch (error) {
        console.error(`[ScriptHost] Wildcard handler error:`, error);
      }
    });
  }

  private setupEventForwarding(): void {
    // Forward backend events (via Tauri event system)
    // This needs to work with backend's event emission mechanism
  }

  // ==================== Statistics ====================

  getStats() {
    const scripts = this.getAllScripts();
    const templates = scripts.filter(s => s.isTemplate);
    const favorites = scripts.filter(s => s.isFavorite);

    return {
      scripts: {
        total: scripts.length,
        templates: templates.length,
        favorites: favorites.length,
        byType: this.countByType(scripts),
      },
      executions: {
        total: this.executionTracks.size,
        running: Array.from(this.executionTracks.values()).filter(t => t.status === 'running').length,
        historySize: this.executionHistory.length,
      },
    };
  }

  private countByType(scripts: CommandScript[]): Record<string, number> {
    const counts: Record<string, number> = {};
    scripts.forEach(s => {
      counts[s.scriptType] = (counts[s.scriptType] || 0) + 1;
    });
    return counts;
  }
}

// Export singleton
export const scriptHost = ScriptHost.getInstance();
