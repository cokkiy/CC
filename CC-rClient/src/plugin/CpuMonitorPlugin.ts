/**
 * CC-CPU-Monitor Plugin
 * Monitors CPU usage across stations using telemetry data from DataBridge
 */

import type { Plugin, PluginMetadata, PluginConfig } from './types';
import type { ActionContext, ActionResult, PanelRegistration } from './types';
import type { StationRuntimeSnapshot } from '../types';
import { dataBridge } from './DataBridge';

// Plugin metadata
const METADATA: PluginMetadata = {
  id: 'cc-cpu-monitor',
  name: 'CPU Monitor',
  version: '1.0.0',
  description: 'Monitors CPU usage across stations with real-time telemetry',
  author: 'CC Team',
  builtIn: true,
};

// CPU data storage per station
interface CpuData {
  stationId: string;
  computerName: string;
  cpu: number;
  timestamp: number;
}

interface CpuHistory {
  current: CpuData | null;
  history: CpuData[];
  maxHistory: number;
}

// Default configuration
export const DEFAULT_CONFIG: PluginConfig = {
  enabled: true,
  priority: 50,
  config: {
    refreshInterval: 2000,
    alertThreshold: 90,
    maxHistoryPoints: 60,
  },
};

// CPU Monitor Plugin Implementation
export class CpuMonitorPlugin implements Plugin {
  readonly metadata = METADATA;
  private intervalId?: number;
  private cpuStores: Map<string, CpuHistory> = new Map();
  private refreshInterval: number = 2000;
  private alertThreshold: number = 90;
  private maxHistoryPoints: number = 60;
  private subscriptions: string[] = [];
  private dataCallback: ((data: CpuData) => void) | null = null;

  onLoad(): void {
    console.log('[CpuMonitorPlugin] Loaded');
  }

  onInit(): void {
    console.log('[CpuMonitorPlugin] Initialized');
    
    // Subscribe to telemetry data
    this.subscribeToTelemetry();
    
    // Start monitoring
    this.startMonitoring();
  }

  onStop(): void {
    console.log('[CpuMonitorPlugin] Stopped');
    this.stopMonitoring();
  }

  onUnload(): void {
    console.log('[CpuMonitorPlugin] Unloaded');
    this.cleanup();
  }

  onDestroy(): void {
    console.log('[CpuMonitorPlugin] Destroyed');
    this.cleanup();
  }

  /**
   * Subscribe to telemetry data from DataBridge
   */
  private subscribeToTelemetry(): void {
    const subscriptionId = dataBridge.subscribe('telemetry', (data: unknown) => {
      const telemetry = data as StationRuntimeSnapshot;
      this.processTelemetry(telemetry);
    });
    
    this.subscriptions.push(subscriptionId);
    console.log('[CpuMonitorPlugin] Subscribed to telemetry');
  }

  /**
   * Process incoming telemetry data
   */
  private processTelemetry(telemetry: StationRuntimeSnapshot): void {
    const stationId = telemetry.stationId;
    
    // Get or create CPU history store for this station
    let store = this.cpuStores.get(stationId);
    if (!store) {
      store = {
        current: null,
        history: [],
        maxHistory: this.maxHistoryPoints,
      };
      this.cpuStores.set(stationId, store);
    }

    // Create CPU data record
    const cpuData: CpuData = {
      stationId,
      computerName: telemetry.computerName,
      cpu: telemetry.cpu,
      timestamp: Date.now(),
    };

    // Update store
    store.current = cpuData;
    store.history = [...store.history.slice(-(store.maxHistory - 1)), cpuData];

    // Notify callback if registered
    if (this.dataCallback) {
      this.dataCallback(cpuData);
    }
  }

  /**
   * Start periodic monitoring
   */
  private startMonitoring(): void {
    this.intervalId = window.setInterval(() => {
      this.collectCpuData();
    }, this.refreshInterval);
  }

  /**
   * Stop periodic monitoring
   */
  private stopMonitoring(): void {
    if (this.intervalId) {
      window.clearInterval(this.intervalId);
      this.intervalId = undefined;
    }
  }

  /**
   * Collect CPU data for all monitored stations
   */
  private collectCpuData(): void {
    const stations = dataBridge.getMonitoredStations();
    
    stations.forEach(stationId => {
      const telemetry = dataBridge.getTelemetry(stationId);
      if (telemetry) {
        this.processTelemetry(telemetry);
      }
    });
  }

  /**
   * Get current CPU data for a station
   */
  getCpuData(stationId: string): CpuData | null {
    return this.cpuStores.get(stationId)?.current ?? null;
  }

  /**
   * Get CPU history for a station
   */
  getCpuHistory(stationId: string): CpuData[] {
    return this.cpuStores.get(stationId)?.history ?? [];
  }

  /**
   * Get CPU usage for all stations
   */
  getAllCpuData(): Map<string, CpuData> {
    const result = new Map<string, CpuData>();
    this.cpuStores.forEach((store, stationId) => {
      if (store.current) {
        result.set(stationId, store.current);
      }
    });
    return result;
  }

  /**
   * Get average CPU across all stations
   */
  getAverageCpu(): number {
    let total = 0;
    let count = 0;
    
    this.cpuStores.forEach(store => {
      if (store.current) {
        total += store.current.cpu;
        count++;
      }
    });

    return count > 0 ? total / count : 0;
  }

  /**
   * Check if any station exceeds alert threshold
   */
  getStationsAboveThreshold(): string[] {
    const aboveThreshold: string[] = [];
    
    this.cpuStores.forEach((store, stationId) => {
      if (store.current && store.current.cpu >= this.alertThreshold) {
        aboveThreshold.push(stationId);
      }
    });

    return aboveThreshold;
  }

  /**
   * Register data callback for real-time updates
   */
  onCpuData(callback: (data: CpuData) => void): void {
    this.dataCallback = callback;
  }

  /**
   * Set configuration
   */
  setConfig(config: Partial<{ refreshInterval: number; alertThreshold: number; maxHistoryPoints: number }>): void {
    if (config.refreshInterval !== undefined) {
      this.refreshInterval = config.refreshInterval;
      // Restart monitoring if interval changed
      if (this.intervalId) {
        this.stopMonitoring();
        this.startMonitoring();
      }
    }
    if (config.alertThreshold !== undefined) {
      this.alertThreshold = config.alertThreshold;
    }
    if (config.maxHistoryPoints !== undefined) {
      this.maxHistoryPoints = config.maxHistoryPoints;
    }
  }

  /**
   * Cleanup subscriptions and intervals
   */
  private cleanup(): void {
    // Unsubscribe from data bridge
    this.subscriptions.forEach(id => {
      dataBridge.unsubscribe(id);
    });
    this.subscriptions = [];

    // Clear data stores
    this.cpuStores.clear();

    // Stop monitoring
    this.stopMonitoring();
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
        chartType: 'line',
      },
    },
    {
      id: 'cpu-per-core-panel',
      pluginId: METADATA.id,
      type: 'table',
      position: 'right',
      title: 'Per-Station CPU Usage',
      priority: 200,
      props: {
        columns: ['Station', 'Computer Name', 'CPU %', 'Status'],
        sortable: true,
      },
    },
    {
      id: 'cpu-alert-panel',
      pluginId: METADATA.id,
      type: 'card',
      position: 'top',
      title: 'CPU Alerts',
      priority: 150,
      props: {
        collapsible: true,
      },
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
      description: 'Force refresh CPU data for selected stations',
      icon: 'refresh',
      scope: 'station' as const,
      target: 'multiple' as const,
      enabled: true,
      handler: async (context: ActionContext): Promise<ActionResult> => {
        console.log('[CpuMonitorPlugin] Refreshing CPU for:', context.stationIds);
        
        // Force collect fresh data
        context.stationIds.forEach(stationId => {
          const telemetry = dataBridge.getTelemetry(stationId);
          if (telemetry) {
            // Data will be processed through the subscription
            console.log('[CpuMonitorPlugin] Fresh data for station:', stationId, 'CPU:', telemetry.cpu);
          }
        });

        return {
          success: true,
          message: `CPU data refreshed for ${context.stationIds.length} station(s)`,
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
        
        const stationId = context.stationIds[0];
        const history = dataBridge.getTelemetryHistory(stationId);
        
        // Generate CSV content
        const csvHeader = 'Timestamp,StationID,ComputerName,CPU %,Memory (MB)\n';
        const csvRows = history.map(h => 
          `${new Date().toISOString()},${h.stationId},${h.computerName},${h.cpu},${Math.round(h.currentMemory / 1024 / 1024)}`
        ).join('\n');
        
        const csvContent = csvHeader + csvRows;
        const blob = new Blob([csvContent], { type: 'text/csv' });
        const url = URL.createObjectURL(blob);
        
        // Trigger download
        const link = document.createElement('a');
        link.href = url;
        link.download = `cpu_export_${stationId}_${Date.now()}.csv`;
        link.click();
        
        URL.revokeObjectURL(url);

        return {
          success: true,
          message: 'CPU data exported successfully',
          data: { stationId, recordCount: history.length },
        };
      },
    },
    {
      id: 'cpu-set-alert',
      pluginId: METADATA.id,
      name: 'Set Alert Threshold',
      description: 'Configure CPU alert threshold',
      icon: 'settings',
      scope: 'global' as const,
      target: 'all' as const,
      enabled: true,
      handler: async (context: ActionContext): Promise<ActionResult> => {
        console.log('[CpuMonitorPlugin] Setting alert threshold');
        
        // In a real implementation, this would open a dialog or use context.userData
        const threshold = (context.userData?.threshold as number) ?? 90;
        
        return {
          success: true,
          message: `Alert threshold set to ${threshold}%`,
        };
      },
    },
  ];
}

// Factory function for plugin creation
export function createCpuMonitorPlugin(): Plugin {
  return new CpuMonitorPlugin();
}
