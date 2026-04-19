/**
 * Plugin System - Main Export
 * Re-exports all plugin system components
 */

// Types
export * from './types';

// Core components
export { PluginManager, pluginManager } from './PluginManager';
export { PanelRegistry, panelRegistry } from './PanelRegistry';
export { ActionRegistry, actionRegistry } from './ActionRegistry';
export { DataBridge, dataBridge } from './DataBridge';
export { PluginHost, pluginHost } from './PluginHost';
export type { PluginHostConfig } from './PluginHost';
