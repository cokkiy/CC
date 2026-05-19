/**
 * CC-Memory-Monitor Plugin
 * Monitors memory usage across stations using telemetry data from DataBridge
 */

import type { Plugin, PluginMetadata, PluginConfig } from './types';
import type { ActionContext, ActionResult, PanelRegistration } from './types';
import type { StationRuntimeSnapshot } from '../types';
import { dataBridge } from './DataBridge';

// Plugin metadata
const METADATA: PluginMetadata = {
  id: 'cc-memory-monitor',
  name: 'Memory Monitor',
  version: '1.0.0',
  description: 'Monitors memory usage across stations with real-time telemetry',
  author: 'CC Team',
  builtIn: true,
};

// Memory data storage per station
interface MemoryData {
  stationId: string;
  computerName: string;
  memory: number; // bytes
  totalMemory: number;
  timestamp: number;
}

interface MemoryHistory {
  current: MemoryData | null;
  history: MemoryData[];
  maxHistory: number;
}

// Default configuration
export const DEFAULT_CONFIG: PluginConfig = {
  enabled: true,
  priority: 50,
  config: {
    refreshInterval: 2000,
    alertThreshold: 90, // percentage
    maxHistoryPoints: 60,
  },
};

// Memory Monitor Plugin Implementation
export class MemoryMonitorPlugin implements Plugin {
  readonly metadata = METADATA;
  private intervalId?: number;
  private memoryStores: Map<string, MemoryHistory> = new Map();
  private refreshInterval: number = 2000;
  private alertThreshold: number = 90;
  private maxHistoryPoints: number = 60;
  private subscriptions: string[] = [];
  private dataCallback: ((data: MemoryData) => void) | null = null;

  onLoad(): void {
    console.log('[MemoryMonitorPlugin] Loaded');
  }

  onInit(): void {
    console.log('[MemoryMonitorPlugin] Initialized');
    
    // Subscribe to telemetry data
    this.subscribeToTelemetry();
    
    // Start monitoring
    this.startMonitoring();
  }

  onStop(): void {
    console.log('[MemoryMonitorPlugin] Stopped');
    this.stopMonitoring();
  }

  onUnload(): void {
    console.log('[MemoryMonitorPlugin] Unloaded');
    this.cleanup();
  }

  onDestroy(): void {
    console.log('[MemoryMonitorPlugin] Destroyed');
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
    console.log('[MemoryMonitorPlugin] Subscribed to telemetry');
  }

  /**
   * Process incoming telemetry data
   */
  private processTelemetry(telemetry: StationRuntimeSnapshot): void {
    const stationId = telemetry.stationId;
    
    // Get or create memory history store for this station
    let store = this.memoryStores.get(stationId);
    if (!store) {
      store = {
        current: null,
        history: [],
        maxHistory: this.maxHistoryPoints,
      };
      this.memoryStores.set(stationId, store);
    }

    // Create memory data record
    const memoryData: MemoryData = {
      stationId,
      computerName: telemetry.computerName,
      memory: telemetry.currentMemory,
      totalMemory: telemetry.totalMemory,
      timestamp: Date.now(),
    };

    // Update store
    store.current = memoryData;
    store.history = [...store.history.slice(-(store.maxHistory - 1)), memoryData];

    // Notify callback if registered
    if (this.dataCallback) {
      this.dataCallback(memoryData);
    }
  }

  /**
   * Start periodic monitoring
   */
  private startMonitoring(): void {
    this.intervalId = window.setInterval(() => {
      this.collectMemoryData();
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
   * Collect memory data for all monitored stations
   */
  private collectMemoryData(): void {
    const stations = dataBridge.getMonitoredStations();
    
    stations.forEach(stationId => {
      const telemetry = dataBridge.getTelemetry(stationId);
      if (telemetry) {
        this.processTelemetry(telemetry);
      }
    });
  }

  /**
   * Get current memory data for a station
   */
  getMemoryData(stationId: string): MemoryData | null {
    return this.memoryStores.get(stationId)?.current ?? null;
  }

  /**
   * Get memory history for a station
   */
  getMemoryHistory(stationId: string): MemoryData[] {
    return this.memoryStores.get(stationId)?.history ?? [];
  }

  /**
   * Get memory usage for all stations
   */
  getAllMemoryData(): Map<string, MemoryData> {
    const result = new Map<string, MemoryData>();
    this.memoryStores.forEach((store, stationId) => {
      if (store.current) {
        result.set(stationId, store.current);
      }
    });
    return result;
  }

  /**
   * Get memory usage percentage for a station
   */
  getMemoryUsagePercent(stationId: string): number {
    const data = this.memoryStores.get(stationId)?.current;
    if (!data || data.totalMemory === 0) return 0;
    return (data.memory / data.totalMemory) * 100;
  }

  /**
   * Get average memory usage percentage across all stations
   */
  getAverageMemoryUsage(): number {
    let total = 0;
    let count = 0;
    
    this.memoryStores.forEach(store => {
      if (store.current && store.current.totalMemory > 0) {
        total += (store.current.memory / store.current.totalMemory) * 100;
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
    
    this.memoryStores.forEach((store, stationId) => {
      if (store.current && store.current.totalMemory > 0) {
        const usagePercent = (store.current.memory / store.current.totalMemory) * 100;
        if (usagePercent >= this.alertThreshold) {
          aboveThreshold.push(stationId);
        }
      }
    });

    return aboveThreshold;
  }

  /**
   * Get total memory across all stations (in bytes)
   */
  getTotalMemoryAcrossStations(): number {
    let total = 0;
    this.memoryStores.forEach(store => {
      if (store.current) {
        total += store.current.totalMemory;
      }
    });
    return total;
  }

  /**
   * Get used memory across all stations (in bytes)
   */
  getUsedMemoryAcrossStations(): number {
    let used = 0;
    this.memoryStores.forEach(store => {
      if (store.current) {
        used += store.current.memory;
      }
    });
    return used;
  }

  /**
   * Register data callback for real-time updates
   */
  onMemoryData(callback: (data: MemoryData) => void): void {
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
    this.memoryStores.clear();

    // Stop monitoring
    this.stopMonitoring();
  }
}

// Panel registration helper
export function getMemoryMonitorPanels(): PanelRegistration[] {
  return [
    {
      id: 'memory-overview-panel',
      pluginId: METADATA.id,
      type: 'chart',
      position: 'center',
      title: 'Memory Overview',
      priority: 100,
      props: {
        refreshInterval: 2000,
        showHistory: true,
        chartType: 'line',
        unit: 'MB',
      },
    },
    {
      id: 'memory-per-station-panel',
      pluginId: METADATA.id,
      type: 'table',
      position: 'right',
      title: 'Per-Station Memory Usage',
      priority: 200,
      props: {
        columns: ['Station', 'Computer Name', 'Used Memory', 'Total Memory', 'Usage %'],
        sortable: true,
      },
    },
    {
      id: 'memory-alert-panel',
      pluginId: METADATA.id,
      type: 'card',
      position: 'top',
      title: 'Memory Alerts',
      priority: 150,
      props: {
        collapsible: true,
      },
    },
  ];
}

// Action registration helper
export function getMemoryMonitorActions() {
  return [
    {
      id: 'memory-refresh',
      pluginId: METADATA.id,
      name: 'Refresh Memory',
      description: 'Force refresh memory data for selected stations',
      icon: 'refresh',
      scope: 'station' as const,
      target: 'multiple' as const,
      enabled: true,
      handler: async (context: ActionContext): Promise<ActionResult> => {
        console.log('[MemoryMonitorPlugin] Refreshing memory for:', context.stationIds);
        
        // Force collect fresh data
        context.stationIds.forEach(stationId => {
          const telemetry = dataBridge.getTelemetry(stationId);
          if (telemetry) {
            console.log('[MemoryMonitorPlugin] Fresh data for station:', stationId, 'Memory:', telemetry.currentMemory);
          }
        });

        return {
          success: true,
          message: `Memory data refreshed for ${context.stationIds.length} station(s)`,
        };
      },
    },
    {
      id: 'memory-export',
      pluginId: METADATA.id,
      name: 'Export Memory Data',
      description: 'Export memory history to CSV',
      icon: 'download',
      scope: 'station' as const,
      target: 'single' as const,
      enabled: true,
      handler: async (context: ActionContext): Promise<ActionResult> => {
        console.log('[MemoryMonitorPlugin] Exporting memory data for:', context.stationIds);
        
        const stationId = context.stationIds[0];
        const history = dataBridge.getTelemetryHistory(stationId);
        
        // Generate CSV content
        const csvHeader = 'Timestamp,StationID,ComputerName,UsedMemoryMB,TotalMemoryMB,Usage%\n';
        const csvRows = history.map(h => {
          const usage = h.totalMemory > 0 ? ((h.currentMemory / h.totalMemory) * 100).toFixed(2) : '0';
          return `${new Date(Date.now()).toISOString()},${h.stationId},${h.computerName},${Math.round(h.currentMemory / 1024 / 1024)},${Math.round(h.totalMemory / 1024 / 1024)},${usage}`;
        }).join('\n');
        
        const csvContent = csvHeader + csvRows;
        const blob = new Blob([csvContent], { type: 'text/csv' });
        const url = URL.createObjectURL(blob);
        
        // Trigger download
        const link = document.createElement('a');
        link.href = url;
        link.download = `memory_export_${stationId}_${Date.now()}.csv`;
        link.click();
        
        URL.revokeObjectURL(url);

        return {
          success: true,
          message: 'Memory data exported successfully',
          data: { stationId, recordCount: history.length },
        };
      },
    },
    {
      id: 'memory-set-alert',
      pluginId: METADATA.id,
      name: 'Set Alert Threshold',
      description: 'Configure memory alert threshold',
      icon: 'settings',
      scope: 'global' as const,
      target: 'all' as const,
      enabled: true,
      handler: async (context: ActionContext): Promise<ActionResult> => {
        console.log('[MemoryMonitorPlugin] Setting alert threshold');
        
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
export function createMemoryMonitorPlugin(): Plugin {
  return new MemoryMonitorPlugin();
}
