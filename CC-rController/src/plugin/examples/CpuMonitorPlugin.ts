/**
 * Example Plugin: CPU Monitor
 * Demonstrates how to create a plugin with panels and actions
 */

import type { Plugin, PluginMetadata, PluginConfig } from '../types';
import type { ActionContext, ActionResult, PanelRegistration } from '../types';

// Plugin metadata
const METADATA: PluginMetadata = {
  id: 'cc-cpu-monitor',
  name: 'CPU Monitor',
  version: '1.0.0',
  description: 'Monitors CPU usage across stations',
  author: 'CC Team',
  builtIn: true,
};

// Example CPU Monitor Plugin
export class CpuMonitorPlugin implements Plugin {
  readonly metadata = METADATA;
  private intervalId?: number;

  onLoad(): void {
    console.log('[CpuMonitorPlugin] Loaded');
  }

  onInit(): void {
    console.log('[CpuMonitorPlugin] Initialized');
    // Start monitoring
    this.startMonitoring();
  }

  onStop(): void {
    console.log('[CpuMonitorPlugin] Stopped');
    this.stopMonitoring();
  }

  onUnload(): void {
    console.log('[CpuMonitorPlugin] Unloaded');
  }

  onDestroy(): void {
    console.log('[CpuMonitorPlugin] Destroyed');
    this.stopMonitoring();
  }

  private startMonitoring(): void {
    // Example: poll every 2 seconds
    this.intervalId = window.setInterval(() => {
      this.collectCpuData();
    }, 2000);
  }

  private stopMonitoring(): void {
    if (this.intervalId) {
      window.clearInterval(this.intervalId);
      this.intervalId = undefined;
    }
  }

  private collectCpuData(): void {
    // Example implementation - would connect to actual data source
    // This would typically use the DataBridge to get telemetry data
  }
}

// Panel registration helper
export function getCpuMonitorPanels(): PanelRegistration[] {
  return [
    {
      id: 'cpu-overview-panel',
      pluginId: METADATA.id,
      type: 'chart',
      position: 'center',
      title: 'CPU Overview',
      priority: 100,
      props: {
        refreshInterval: 2000,
        showHistory: true,
      },
    },
    {
      id: 'cpu-per-core-panel',
      pluginId: METADATA.id,
      type: 'table',
      position: 'right',
      title: 'Per-Core Usage',
      priority: 200,
    },
  ];
}

// Action registration helper
export function getCpuMonitorActions() {
  return [
    {
      id: 'cpu-refresh',
      pluginId: METADATA.id,
      name: 'Refresh CPU',
      description: 'Force refresh CPU data',
      icon: 'refresh',
      scope: 'station' as const,
      target: 'single' as const,
      enabled: true,
      handler: async (context: ActionContext): Promise<ActionResult> => {
        console.log('[CpuMonitorPlugin] Refreshing CPU for:', context.stationIds);
        // Implementation would trigger data refresh
        return {
          success: true,
          message: 'CPU data refreshed',
        };
      },
    },
    {
      id: 'cpu-export',
      pluginId: METADATA.id,
      name: 'Export CPU Data',
      description: 'Export CPU history to CSV',
      icon: 'download',
      scope: 'station' as const,
      target: 'single' as const,
      enabled: true,
      handler: async (context: ActionContext): Promise<ActionResult> => {
        console.log('[CpuMonitorPlugin] Exporting CPU data for:', context.stationIds);
        return {
          success: true,
          message: 'CPU data exported',
          data: { path: '/tmp/cpu_export.csv' },
        };
      },
    },
  ];
}

// Factory function for plugin creation
export function createCpuMonitorPlugin(): Plugin {
  return new CpuMonitorPlugin();
}

// Default config
export const DEFAULT_CONFIG: PluginConfig = {
  enabled: true,
  priority: 50,
  config: {
    refreshInterval: 2000,
    alertThreshold: 90,
  },
};
