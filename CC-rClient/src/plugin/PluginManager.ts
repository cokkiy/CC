/**
 * PluginManager - Core plugin lifecycle management
 * Handles plugin loading, initialization, and lifecycle events
 */

import type {
  Plugin,
  PluginMetadata,
  PluginConfig,
  PluginState,
  PluginError,
  PluginEvent,
  PluginEventHandler,
  PluginErrorHandler,
} from './types';

// Plugin instance with runtime state
interface PluginInstance {
  plugin: Plugin;
  config: PluginConfig;
  state: PluginState;
  loadTime?: number;
  error?: PluginError;
}

// Event emitter mixin for plugin events
type EventCallback = PluginEventHandler | PluginErrorHandler;

export class PluginManager {
  private plugins: Map<string, PluginInstance> = new Map();
  private eventListeners: Map<string, Set<EventCallback>> = new Map();
  private pluginOrder: string[] = []; // Track load order for dependency resolution

  // Singleton instance
  private static instance: PluginManager | null = null;

  public static getInstance(): PluginManager {
    if (!PluginManager.instance) {
      PluginManager.instance = new PluginManager();
    }
    return PluginManager.instance;
  }

  /**
   * Register a plugin with the manager
   */
  register(plugin: Plugin, config: PluginConfig = { enabled: true, priority: 100 }): void {
    const { id } = plugin.metadata;

    if (this.plugins.has(id)) {
      console.warn(`[PluginManager] Plugin ${id} is already registered, updating config`);
      const instance = this.plugins.get(id)!;
      instance.config = { ...instance.config, ...config };
      return;
    }

    const instance: PluginInstance = {
      plugin,
      config: { ...config, enabled: config.enabled ?? true },
      state: 'pending',
    };

    this.plugins.set(id, instance);
    this.sortPlugins();
    
    console.log(`[PluginManager] Registered plugin: ${id} v${plugin.metadata.version}`);
    this.emit('plugin:registered', { pluginId: id, metadata: plugin.metadata });
  }

  /**
   * Load a plugin and call its onLoad hook
   */
  async loadPlugin(id: string): Promise<boolean> {
    const instance = this.plugins.get(id);
    if (!instance) {
      console.error(`[PluginManager] Plugin ${id} not found`);
      return false;
    }

    if (instance.state === 'loaded' || instance.state === 'running') {
      console.warn(`[PluginManager] Plugin ${id} is already ${instance.state}`);
      return true;
    }

    try {
      instance.state = 'loading';
      this.emit('plugin:loading', { pluginId: id });

      // Call plugin's onLoad if exists
      if (instance.plugin.onLoad) {
        await instance.plugin.onLoad();
      }

      instance.state = 'loaded';
      instance.loadTime = Date.now();
      this.emit('plugin:loaded', { pluginId: id, loadTime: instance.loadTime });
      
      return true;
    } catch (error) {
      instance.state = 'error';
      instance.error = {
        pluginId: id,
        code: 'LOAD_ERROR',
        message: error instanceof Error ? error.message : String(error),
        timestamp: Date.now(),
      };
      console.error(`[PluginManager] Failed to load plugin ${id}:`, error);
      this.emitError(instance.error);
      return false;
    }
  }

  /**
   * Initialize a plugin (call onInit) - starts the plugin
   */
  async initPlugin(id: string): Promise<boolean> {
    const instance = this.plugins.get(id);
    if (!instance) {
      console.error(`[PluginManager] Plugin ${id} not found`);
      return false;
    }

    if (instance.state === 'running') {
      console.warn(`[PluginManager] Plugin ${id} is already running`);
      return true;
    }

    if (instance.state !== 'loaded') {
      // Auto-load if not loaded
      const loaded = await this.loadPlugin(id);
      if (!loaded) return false;
    }

    try {
      instance.state = 'initializing';
      this.emit('plugin:initializing', { pluginId: id });

      if (instance.plugin.onInit) {
        await instance.plugin.onInit();
      }

      instance.state = 'running';
      this.emit('plugin:running', { pluginId: id });
      
      return true;
    } catch (error) {
      instance.state = 'error';
      instance.error = {
        pluginId: id,
        code: 'INIT_ERROR',
        message: error instanceof Error ? error.message : String(error),
        timestamp: Date.now(),
      };
      console.error(`[PluginManager] Failed to initialize plugin ${id}:`, error);
      this.emitError(instance.error);
      return false;
    }
  }

  /**
   * Stop a plugin (call onStop) - pauses the plugin
   */
  async stopPlugin(id: string): Promise<boolean> {
    const instance = this.plugins.get(id);
    if (!instance) {
      console.error(`[PluginManager] Plugin ${id} not found`);
      return false;
    }

    if (instance.state === 'stopped') {
      console.warn(`[PluginManager] Plugin ${id} is already stopped`);
      return true;
    }

    try {
      instance.state = 'stopping';
      this.emit('plugin:stopping', { pluginId: id });

      if (instance.plugin.onStop) {
        await instance.plugin.onStop();
      }

      instance.state = 'stopped';
      this.emit('plugin:stopped', { pluginId: id });
      
      return true;
    } catch (error) {
      instance.error = {
        pluginId: id,
        code: 'STOP_ERROR',
        message: error instanceof Error ? error.message : String(error),
        timestamp: Date.now(),
      };
      console.error(`[PluginManager] Failed to stop plugin ${id}:`, error);
      this.emitError(instance.error);
      return false;
    }
  }

  /**
   * Unload a plugin completely
   */
  async unloadPlugin(id: string): Promise<boolean> {
    const instance = this.plugins.get(id);
    if (!instance) {
      console.error(`[PluginManager] Plugin ${id} not found`);
      return false;
    }

    // Stop if running
    if (instance.state === 'running' || instance.state === 'initializing') {
      await this.stopPlugin(id);
    }

    try {
      instance.state = 'unloading';
      this.emit('plugin:unloading', { pluginId: id });

      if (instance.plugin.onUnload) {
        await instance.plugin.onUnload();
      }

      // Note: We don't call onDestroy here - that's for full cleanup
      instance.state = 'pending';
      this.emit('plugin:unloaded', { pluginId: id });
      
      return true;
    } catch (error) {
      instance.error = {
        pluginId: id,
        code: 'UNLOAD_ERROR',
        message: error instanceof Error ? error.message : String(error),
        timestamp: Date.now(),
      };
      console.error(`[PluginManager] Failed to unload plugin ${id}:`, error);
      this.emitError(instance.error);
      return false;
    }
  }

  /**
   * Destroy a plugin - full cleanup including calling onDestroy
   */
  async destroyPlugin(id: string): Promise<boolean> {
    const instance = this.plugins.get(id);
    if (!instance) {
      console.error(`[PluginManager] Plugin ${id} not found`);
      return false;
    }

    // Unload first
    await this.unloadPlugin(id);

    try {
      if (instance.plugin.onDestroy) {
        await instance.plugin.onDestroy();
      }

      this.plugins.delete(id);
      this.pluginOrder = this.pluginOrder.filter(pid => pid !== id);
      this.emit('plugin:destroyed', { pluginId: id });
      
      return true;
    } catch (error) {
      console.error(`[PluginManager] Failed to destroy plugin ${id}:`, error);
      return false;
    }
  }

  /**
   * Load and initialize all enabled plugins
   */
  async loadAllPlugins(): Promise<void> {
    console.log('[PluginManager] Loading all plugins...');
    
    for (const id of this.pluginOrder) {
      const instance = this.plugins.get(id);
      if (instance?.config.enabled) {
        await this.initPlugin(id);
      }
    }
    
    console.log('[PluginManager] All plugins loaded');
  }

  /**
   * Stop all running plugins
   */
  async stopAllPlugins(): Promise<void> {
    console.log('[PluginManager] Stopping all plugins...');
    
    // Stop in reverse order (higher priority first)
    for (const id of [...this.pluginOrder].reverse()) {
      const instance = this.plugins.get(id);
      if (instance?.state === 'running') {
        await this.stopPlugin(id);
      }
    }
    
    console.log('[PluginManager] All plugins stopped');
  }

  /**
   * Get a plugin by ID
   */
  getPlugin(id: string): Plugin | null {
    return this.plugins.get(id)?.plugin ?? null;
  }

  /**
   * Get plugin state
   */
  getPluginState(id: string): PluginState | null {
    return this.plugins.get(id)?.state ?? null;
  }

  /**
   * Get plugin config
   */
  getPluginConfig(id: string): PluginConfig | null {
    return this.plugins.get(id)?.config ?? null;
  }

  /**
   * Update plugin config
   */
  updatePluginConfig(id: string, config: Partial<PluginConfig>): void {
    const instance = this.plugins.get(id);
    if (instance) {
      instance.config = { ...instance.config, ...config };
      this.emit('plugin:config_updated', { pluginId: id, config: instance.config });
    }
  }

  /**
   * Get all registered plugins
   */
  getAllPlugins(): Array<{ metadata: PluginMetadata; config: PluginConfig; state: PluginState }> {
    return Array.from(this.plugins.values()).map(inst => ({
      metadata: inst.plugin.metadata,
      config: inst.config,
      state: inst.state,
    }));
  }

  /**
   * Get plugins in load order
   */
  getPluginsInOrder(): string[] {
    return [...this.pluginOrder];
  }

  /**
   * Check if a plugin is enabled
   */
  isPluginEnabled(id: string): boolean {
    return this.plugins.get(id)?.config.enabled ?? false;
  }

  /**
   * Enable a plugin
   */
  async enablePlugin(id: string): Promise<void> {
    const instance = this.plugins.get(id);
    if (instance) {
      instance.config.enabled = true;
      await this.initPlugin(id);
    }
  }

  /**
   * Disable a plugin
   */
  async disablePlugin(id: string): Promise<void> {
    const instance = this.plugins.get(id);
    if (instance) {
      await this.stopPlugin(id);
      instance.config.enabled = false;
    }
  }

  // Event handling
  on(event: string, handler: EventCallback): () => void {
    if (!this.eventListeners.has(event)) {
      this.eventListeners.set(event, new Set());
    }
    this.eventListeners.get(event)!.add(handler);
    
    // Return unsubscribe function
    return () => {
      this.eventListeners.get(event)?.delete(handler);
    };
  }

  off(event: string, handler: EventCallback): void {
    this.eventListeners.get(event)?.delete(handler);
  }

  private emit(type: string, data: Record<string, unknown>): void {
    const event: PluginEvent = {
      type,
      source: 'PluginManager',
      timestamp: Date.now(),
      data,
    };

    // Emit to specific event listeners
    this.eventListeners.get(type)?.forEach(handler => {
      try {
        (handler as PluginEventHandler)(event);
      } catch (error) {
        console.error(`[PluginManager] Event handler error for ${type}:`, error);
      }
    });

    // Also emit to wildcard listeners
    this.eventListeners.get('*')?.forEach(handler => {
      try {
        (handler as PluginEventHandler)(event);
      } catch (error) {
        console.error(`[PluginManager] Wildcard event handler error:`, error);
      }
    });
  }

  private emitError(error: PluginError): void {
    this.eventListeners.get('plugin:error')?.forEach(handler => {
      try {
        (handler as (error: PluginError) => void)(error);
      } catch (e) {
        console.error(`[PluginManager] Error handler error:`, e);
      }
    });
  }

  // Sort plugins by priority for load order
  private sortPlugins(): void {
    this.pluginOrder = Array.from(this.plugins.values())
      .sort((a, b) => {
        // Built-in plugins load first
        if (a.plugin.metadata.builtIn && !b.plugin.metadata.builtIn) return -1;
        if (!a.plugin.metadata.builtIn && b.plugin.metadata.builtIn) return 1;
        // Then by priority (lower = earlier)
        return a.config.priority - b.config.priority;
      })
      .map(inst => inst.plugin.metadata.id);
  }

  /**
   * Check plugin dependencies
   */
  async checkDependencies(id: string): Promise<{ ok: boolean; missing: string[] }> {
    const instance = this.plugins.get(id);
    if (!instance) {
      return { ok: false, missing: [id] };
    }

    const deps = instance.plugin.metadata.dependencies ?? [];
    const missing = deps.filter(depId => !this.plugins.has(depId));
    
    return { ok: missing.length === 0, missing };
  }

  /**
   * Get plugin load time statistics
   */
  getLoadStats(): Array<{ id: string; loadTime?: number; state: PluginState }> {
    return Array.from(this.plugins.entries()).map(([id, inst]) => ({
      id,
      loadTime: inst.loadTime,
      state: inst.state,
    }));
  }
}

// Export singleton getter
export const pluginManager = PluginManager.getInstance();
