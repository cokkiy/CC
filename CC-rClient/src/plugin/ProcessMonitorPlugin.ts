/**
 * CC-Process-Monitor Plugin
 * Monitors process count across stations using telemetry data from DataBridge
 */

import type { Plugin, PluginMetadata, PluginConfig } from './types';
import type { ActionContext, ActionResult, PanelRegistration } from './types';
import type { StationRuntimeSnapshot } from '../types';
import { dataBridge } from './DataBridge';

// Plugin metadata
const METADATA: PluginMetadata = {
  id: 'cc-process-monitor',
  name: 'Process Monitor',
  version: '1.0.0',
  description: 'Monitors process count across stations with real-time telemetry',
  author: 'CC Team',
  builtIn: true,
};

// Process data storage per station
interface ProcessData {
  stationId: string;
  computerName: string;
  processCount: number;
  timestamp: number;
}

interface ProcessHistory {
  current: ProcessData | null;
  history: ProcessData[];
  maxHistory: number;
}

// Default configuration
export const DEFAULT_CONFIG: PluginConfig = {
  enabled: true,
  priority: 50,
  config: {
    refreshInterval: 2000,
    alertThreshold: 500, // high process count threshold
    maxHistoryPoints: 60,
  },
};

// Process Monitor Plugin Implementation
export class ProcessMonitorPlugin implements Plugin {
  readonly metadata = METADATA;
  private intervalId?: number;
  private processStores: Map<string, ProcessHistory> = new Map();
  private refreshInterval: number = 2000;
  private alertThreshold: number = 500;
  private maxHistoryPoints: number = 60;
  private subscriptions: string[] = [];
  private dataCallback: ((data: ProcessData) => void) | null = null;

  onLoad(): void {
    console.log('[ProcessMonitorPlugin] Loaded');
  }

  onInit(): void {
    console.log('[ProcessMonitorPlugin] Initialized');
    
    // Subscribe to telemetry data
    this.subscribeToTelemetry();
    
    // Start monitoring
    this.startMonitoring();
  }

  onStop(): void {
    console.log('[ProcessMonitorPlugin] Stopped');
    this.stopMonitoring();
  }

  onUnload(): void {
    console.log('[ProcessMonitorPlugin] Unloaded');
    this.cleanup();
  }

  onDestroy(): void {
    console.log('[ProcessMonitorPlugin] Destroyed');
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
    console.log('[ProcessMonitorPlugin] Subscribed to telemetry');
  }

  /**
   * Process incoming telemetry data
   */
  private processTelemetry(telemetry: StationRuntimeSnapshot): void {
    const stationId = telemetry.stationId;
    
    // Get or create process history store for this station
    let store = this.processStores.get(stationId);
    if (!store) {
      store = {
        current: null,
        history: [],
        maxHistory: this.maxHistoryPoints,
      };
      this.processStores.set(stationId, store);
    }

    // Create process data record
    const processData: ProcessData = {
      stationId,
      computerName: telemetry.computerName,
      processCount: telemetry.procCount,
      timestamp: Date.now(),
    };

    // Update store
    store.current = processData;
    store.history = [...store.history.slice(-(store.maxHistory - 1)), processData];

    // Notify callback if registered
    if (this.dataCallback) {
      this.dataCallback(processData);
    }
  }

  /**
   * Start periodic monitoring
   */
  private startMonitoring(): void {
    this.intervalId = window.setInterval(() => {
      this.collectProcessData();
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
   * Collect process data for all monitored stations
   */
  private collectProcessData(): void {
    const stations = dataBridge.getMonitoredStations();
    
    stations.forEach(stationId => {
      const telemetry = dataBridge.getTelemetry(stationId);
      if (telemetry) {
        this.processTelemetry(telemetry);
      }
    });
  }

  /**
   * Get current process data for a station
   */
  getProcessData(stationId: string): ProcessData | null {
    return this.processStores.get(stationId)?.current ?? null;
  }

  /**
   * Get process history for a station
   */
  getProcessHistory(stationId: string): ProcessData[] {
    return this.processStores.get(stationId)?.history ?? [];
  }

  /**
   * Get process count for all stations
   */
  getAllProcessData(): Map<string, ProcessData> {
    const result = new Map<string, ProcessData>();
    this.processStores.forEach((store, stationId) => {
      if (store.current) {
        result.set(stationId, store.current);
      }
    });
    return result;
  }

  /**
   * Get total process count across all stations
   */
  getTotalProcessCount(): number {
    let total = 0;
    this.processStores.forEach(store => {
      if (store.current) {
        total += store.current.processCount;
      }
    });
    return total;
  }

  /**
   * Get average process count across all stations
   */
  getAverageProcessCount(): number {
    let total = 0;
    let count = 0;
    
    this.processStores.forEach(store => {
      if (store.current) {
        total += store.current.processCount;
        count++;
      }
    });

    return count > 0 ? total / count : 0;
  }

  /**
   * Get stations with high process count
   */
  getStationsAboveThreshold(): string[] {
    const aboveThreshold: string[] = [];
    
    this.processStores.forEach((store, stationId) => {
      if (store.current && store.current.processCount >= this.alertThreshold) {
        aboveThreshold.push(stationId);
      }
    });

    return aboveThreshold;
  }

  /**
   * Get station with highest process count
   */
  getStationWithHighestProcessCount(): { stationId: string; processCount: number } | null {
    let highest: { stationId: string; processCount: number } | null = null;
    
    this.processStores.forEach((store, stationId) => {
      if (store.current) {
        if (!highest || store.current.processCount > highest.processCount) {
          highest = { stationId, processCount: store.current.processCount };
        }
      }
    });

    return highest;
  }

  /**
   * Register data callback for real-time updates
   */
  onProcessData(callback: (data: ProcessData) => void): void {
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
    this.processStores.clear();

    // Stop monitoring
    this.stopMonitoring();
  }
}

// Panel registration helper
export function getProcessMonitorPanels(): PanelRegistration[] {
  return [
    {
      id: 'process-overview-panel',
      pluginId: METADATA.id,
      type: 'chart',
      position: 'center',
      title: 'Process Count Overview',
      priority: 100,
      props: {
        refreshInterval: 2000,
        showHistory: true,
        chartType: 'line',
      },
    },
    {
      id: 'process-per-station-panel',
      pluginId: METADATA.id,
      type: 'table',
      position: 'right',
      title: 'Per-Station Process Count',
      priority: 200,
      props: {
        columns: ['Station', 'Computer Name', 'Process Count', 'Status'],
        sortable: true,
      },
    },
    {
      id: 'process-alert-panel',
      pluginId: METADATA.id,
      type: 'card',
      position: 'top',
      title: 'Process Alerts',
      priority: 150,
      props: {
        collapsible: true,
      },
    },
  ];
}

// Action registration helper
export function getProcessMonitorActions() {
  return [
    {
      id: 'process-refresh',
      pluginId: METADATA.id,
      name: 'Refresh Process Count',
      description: 'Force refresh process count for selected stations',
      icon: 'refresh',
      scope: 'station' as const,
      target: 'multiple' as const,
      enabled: true,
      handler: async (context: ActionContext): Promise<ActionResult> => {
        console.log('[ProcessMonitorPlugin] Refreshing process count for:', context.stationIds);
        
        // Force collect fresh data
        context.stationIds.forEach(stationId => {
          const telemetry = dataBridge.getTelemetry(stationId);
          if (telemetry) {
            console.log('[ProcessMonitorPlugin] Fresh data for station:', stationId, 'ProcessCount:', telemetry.procCount);
          }
        });

        return {
          success: true,
          message: `Process count refreshed for ${context.stationIds.length} station(s)`,
        };
      },
    },
    {
      id: 'process-export',
      pluginId: METADATA.id,
      name: 'Export Process Data',
      description: 'Export process count history to CSV',
      icon: 'download',
      scope: 'station' as const,
      target: 'single' as const,
      enabled: true,
      handler: async (context: ActionContext): Promise<ActionResult> => {
        console.log('[ProcessMonitorPlugin] Exporting process data for:', context.stationIds);
        
        const stationId = context.stationIds[0];
        const history = dataBridge.getTelemetryHistory(stationId);
        
        // Generate CSV content
        const csvHeader = 'Timestamp,StationID,ComputerName,ProcessCount\n';
        const csvRows = history.map(h => 
          `${new Date(Date.now()).toISOString()},${h.stationId},${h.computerName},${h.procCount}`
        ).join('\n');
        
        const csvContent = csvHeader + csvRows;
        const blob = new Blob([csvContent], { type: 'text/csv' });
        const url = URL.createObjectURL(blob);
        
        // Trigger download
        const link = document.createElement('a');
        link.href = url;
        link.download = `process_export_${stationId}_${Date.now()}.csv`;
        link.click();
        
        URL.revokeObjectURL(url);

        return {
          success: true,
          message: 'Process data exported successfully',
          data: { stationId, recordCount: history.length },
        };
      },
    },
    {
      id: 'process-set-alert',
      pluginId: METADATA.id,
      name: 'Set Alert Threshold',
      description: 'Configure process count alert threshold',
      icon: 'settings',
      scope: 'global' as const,
      target: 'all' as const,
      enabled: true,
      handler: async (context: ActionContext): Promise<ActionResult> => {
        console.log('[ProcessMonitorPlugin] Setting alert threshold');
        
        const threshold = (context.userData?.threshold as number) ?? 500;
        
        return {
          success: true,
          message: `Alert threshold set to ${threshold} processes`,
        };
      },
    },
  ];
}

// Factory function for plugin creation
export function createProcessMonitorPlugin(): Plugin {
  return new ProcessMonitorPlugin();
}
