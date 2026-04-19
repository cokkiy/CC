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

// Built-in Plugins
export {
  CpuMonitorPlugin,
  createCpuMonitorPlugin,
  getCpuMonitorPanels,
  getCpuMonitorActions,
  DEFAULT_CONFIG as CPU_MONITOR_DEFAULT_CONFIG,
} from './CpuMonitorPlugin';

export {
  MemoryMonitorPlugin,
  createMemoryMonitorPlugin,
  getMemoryMonitorPanels,
  getMemoryMonitorActions,
  DEFAULT_CONFIG as MEMORY_MONITOR_DEFAULT_CONFIG,
} from './MemoryMonitorPlugin';

export {
  NetworkMonitorPlugin,
  createNetworkMonitorPlugin,
  getNetworkMonitorPanels,
  getNetworkMonitorActions,
  DEFAULT_CONFIG as NETWORK_MONITOR_DEFAULT_CONFIG,
} from './NetworkMonitorPlugin';

export {
  ProcessMonitorPlugin,
  createProcessMonitorPlugin,
  getProcessMonitorPanels,
  getProcessMonitorActions,
  DEFAULT_CONFIG as PROCESS_MONITOR_DEFAULT_CONFIG,
} from './ProcessMonitorPlugin';

export {
  DiskMonitorPlugin,
  createDiskMonitorPlugin,
  getDiskMonitorPanels,
  getDiskMonitorActions,
  DEFAULT_CONFIG as DISK_MONITOR_DEFAULT_CONFIG,
} from './DiskMonitorPlugin';

// Configuration-driven UI Components
export * from './components';

// Plugin Marketplace Infrastructure
export * from './marketplace';
