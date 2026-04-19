/**
 * PluginHost - Main entry point for the plugin system
 * Integrates PluginManager, PanelRegistry, ActionRegistry, and DataBridge
 */

import { pluginManager, PluginManager } from './PluginManager';
import { panelRegistry, PanelRegistry } from './PanelRegistry';
import { actionRegistry, ActionRegistry } from './ActionRegistry';
import { dataBridge, DataBridge } from './DataBridge';

import type { Plugin, PluginConfig, PluginMetadata } from './types';
import type { ActionDefinition, ActionContext, ActionResult } from './types';
import type { PanelRegistration } from './types';
import type { DataType } from './types';

// Plugin host configuration
export interface PluginHostConfig {
  autoLoad?: boolean;        // Automatically load plugins on init
  maxHistory?: number;       // Max telemetry history per station
  enableLogging?: boolean;   // Enable detailed logging
}

/**
 * PluginHost - Central facade for the plugin system
 * Provides a unified API for plugin management
 */
export class PluginHost {
  private static instance: PluginHost | null = null;
  private initialized: boolean = false;
  private config: PluginHostConfig;

  // Delegated components
  public readonly plugins: PluginManager;
  public readonly panels: PanelRegistry;
  public readonly actions: ActionRegistry;
  public readonly data: DataBridge;

  private constructor(config: PluginHostConfig = {}) {
    this.config = {
      autoLoad: true,
      maxHistory: 60,
      enableLogging: true,
      ...config,
    };

    // Initialize components
    this.plugins = pluginManager;
    this.panels = panelRegistry;
    this.actions = actionRegistry;
    this.data = dataBridge;

    // Apply config
    if (this.config.maxHistory) {
      this.data.setMaxHistory(this.config.maxHistory);
    }
  }

  /**
   * Get singleton instance
   */
  public static getInstance(config?: PluginHostConfig): PluginHost {
    if (!PluginHost.instance) {
      PluginHost.instance = new PluginHost(config);
    }
    return PluginHost.instance;
  }

  /**
   * Check if host is initialized
   */
  get isInitialized(): boolean {
    return this.initialized;
  }

  /**
   * Initialize the plugin host
   */
  async init(): Promise<void> {
    if (this.initialized) {
      console.warn('[PluginHost] Already initialized');
      return;
    }

    console.log('[PluginHost] Initializing...');

    // Set up cross-component event forwarding
    this.setupEventForwarding();

    this.initialized = true;
    console.log('[PluginHost] Initialized successfully');
  }

  /**
   * Load all registered plugins
   */
  async loadAll(): Promise<void> {
    if (!this.initialized) {
      await this.init();
    }
    await this.plugins.loadAllPlugins();
  }

  /**
   * Stop all plugins
   */
  async stopAll(): Promise<void> {
    await this.plugins.stopAllPlugins();
  }

  /**
   * Shutdown the plugin host
   */
  async shutdown(): Promise<void> {
    console.log('[PluginHost] Shutting down...');
    
    await this.plugins.stopAllPlugins();
    this.initialized = false;
    
    console.log('[PluginHost] Shutdown complete');
  }

  // Convenience methods delegating to PluginManager
  registerPlugin(plugin: Plugin, config?: PluginConfig): void {
    this.plugins.register(plugin, config);
  }

  async loadPlugin(id: string): Promise<boolean> {
    return this.plugins.loadPlugin(id);
  }

  async initPlugin(id: string): Promise<boolean> {
    return this.plugins.initPlugin(id);
  }

  async stopPlugin(id: string): Promise<boolean> {
    return this.plugins.stopPlugin(id);
  }

  async unloadPlugin(id: string): Promise<boolean> {
    return this.plugins.unloadPlugin(id);
  }

  async destroyPlugin(id: string): Promise<boolean> {
    // Also clean up panels and actions for this plugin
    this.panels.unregisterByPlugin(id);
    this.actions.unregisterByPlugin(id);
    this.data.unsubscribeByPlugin(id);
    return this.plugins.destroyPlugin(id);
  }

  getPlugin(id: string): Plugin | null {
    return this.plugins.getPlugin(id);
  }

  getPluginState(id: string) {
    return this.plugins.getPluginState(id);
  }

  getAllPlugins() {
    return this.plugins.getAllPlugins();
  }

  // Convenience methods delegating to PanelRegistry
  registerPanel(registration: PanelRegistration): boolean {
    return this.panels.register(registration);
  }

  unregisterPanel(panelId: string): boolean {
    return this.panels.unregister(panelId);
  }

  getPanel(id: string): PanelRegistration | null {
    return this.panels.getPanel(id);
  }

  getAllPanels(): PanelRegistration[] {
    return this.panels.getAllPanels();
  }

  getPanelsByPosition(position: import('./types').PanelPosition) {
    return this.panels.getPanelsByPosition(position);
  }

  getLayout() {
    return this.panels.getLayout();
  }

  setPanelVisible(panelId: string, visible: boolean): void {
    this.panels.setVisible(panelId, visible);
  }

  togglePanel(panelId: string): void {
    this.panels.toggle(panelId);
  }

  // Convenience methods delegating to ActionRegistry
  registerAction(definition: ActionDefinition): boolean {
    return this.actions.register(definition);
  }

  unregisterAction(actionId: string): boolean {
    return this.actions.unregister(actionId);
  }

  getAction(id: string): ActionDefinition | null {
    return this.actions.getAction(id);
  }

  getAllActions(): ActionDefinition[] {
    return this.actions.getAllActions();
  }

  async executeAction(actionId: string, context: ActionContext): Promise<ActionResult> {
    return this.actions.execute(actionId, context);
  }

  setActionEnabled(actionId: string, enabled: boolean): void {
    this.actions.setEnabled(actionId, enabled);
  }

  // Convenience methods delegating to DataBridge
  subscribe(
    dataType: DataType,
    callback: (data: unknown) => void,
    filter?: Record<string, unknown>
  ): string {
    return this.data.subscribe(dataType, callback, filter);
  }

  unsubscribe(subscriptionId: string): boolean {
    return this.data.unsubscribe(subscriptionId);
  }

  setConnected(connected: boolean): void {
    this.data.setConnected(connected);
  }

  getTelemetry(stationId: string) {
    return this.data.getTelemetry(stationId);
  }

  getTelemetryHistory(stationId: string) {
    return this.data.getTelemetryHistory(stationId);
  }

  // Event handling - forward to appropriate component
  on(event: string, handler: (...args: unknown[]) => void): () => void {
    // Route based on event prefix
    if (event.startsWith('plugin:')) {
      return this.plugins.on(event, handler as any);
    } else if (event.startsWith('panel:')) {
      return this.panels.on(event, handler as any);
    } else if (event.startsWith('action:')) {
      return this.actions.on(event, handler as any);
    }
    
    // Generic handler
    console.warn(`[PluginHost] Unknown event prefix: ${event}`);
    return () => {};
  }

  // Set up event forwarding between components
  private setupEventForwarding(): void {
    // When a plugin is destroyed, clean up its panels and actions
    this.plugins.on('plugin:destroyed', (event: any) => {
      const pluginId = event.data?.pluginId;
      if (pluginId) {
        this.panels.unregisterByPlugin(pluginId);
        this.actions.unregisterByPlugin(pluginId);
        this.data.unsubscribeByPlugin(pluginId);
      }
    });

    // Forward panel events as plugin events
    this.panels.on('panel:registered', (event: any) => {
      console.log(`[PluginHost] Panel ${event.panelId} registered`);
    });

    this.panels.on('panel:unregistered', (event: any) => {
      console.log(`[PluginHost] Panel ${event.panelId} unregistered`);
    });

    this.actions.on('action:registered', (event: any) => {
      console.log(`[PluginHost] Action ${event.actionId} registered`);
    });

    this.actions.on('action:unregistered', (event: any) => {
      console.log(`[PluginHost] Action ${event.actionId} unregistered`);
    });
  }

  /**
   * Get host statistics
   */
  getStats() {
    return {
      initialized: this.initialized,
      plugins: {
        total: this.plugins.getAllPlugins().length,
        byState: this.countByState(),
      },
      panels: {
        total: this.panels.count,
        visible: this.panels.visibleCount,
      },
      actions: {
        total: this.actions.count,
        enabled: this.actions.enabledCount,
      },
      data: {
        subscriptions: this.data.subscriptionCount,
        monitoredStations: this.data.getMonitoredStations().length,
      },
    };
  }

  private countByState() {
    const plugins = this.plugins.getAllPlugins();
    const counts: Record<string, number> = {};
    plugins.forEach(p => {
      counts[p.state] = (counts[p.state] || 0) + 1;
    });
    return counts;
  }
}

// Export singleton getter
export const pluginHost = PluginHost.getInstance();
