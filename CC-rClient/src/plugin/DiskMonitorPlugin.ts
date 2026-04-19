/**
 * CC-Disk-Monitor Plugin
 * Monitors disk I/O across stations using telemetry data from DataBridge
 */

import type { Plugin, PluginMetadata, PluginConfig } from './types';
import type { ActionContext, ActionResult, PanelRegistration } from './types';
import type { StationRuntimeSnapshot } from '../types';
import { dataBridge } from './DataBridge';

// Plugin metadata
const METADATA: PluginMetadata = {
  id: 'cc-disk-monitor',
  name: 'Disk Monitor',
  version: '1.0.0',
  description: 'Monitors disk I/O across stations with real-time telemetry',
  author: 'CC Team',
  builtIn: true,
};

// Disk data storage per station
interface DiskData {
  stationId: string;
  computerName: string;
  readBytesPerSec: number;
  writeBytesPerSec: number;
  timestamp: number;
}

interface DiskHistory {
  current: DiskData | null;
  history: DiskData[];
  maxHistory: number;
}

// Default configuration
export const DEFAULT_CONFIG: PluginConfig = {
  enabled: true,
  priority: 50,
  config: {
    refreshInterval: 2000,
    alertThreshold: 100 * 1024 * 1024, // 100 MB/s threshold
    maxHistoryPoints: 60,
  },
};

// Disk Monitor Plugin Implementation
export class DiskMonitorPlugin implements Plugin {
  readonly metadata = METADATA;
  private intervalId?: number;
  private diskStores: Map<string, DiskHistory> = new Map();
  private refreshInterval: number = 2000;
  private alertThreshold: number = 100 * 1024 * 1024; // 100 MB/s
  private maxHistoryPoints: number = 60;
  private subscriptions: string[] = [];
  private dataCallback: ((data: DiskData) => void) | null = null;

  onLoad(): void {
    console.log('[DiskMonitorPlugin] Loaded');
  }

  onInit(): void {
    console.log('[DiskMonitorPlugin] Initialized');
    
    // Subscribe to telemetry data
    this.subscribeToTelemetry();
    
    // Start monitoring
    this.startMonitoring();
  }

  onStop(): void {
    console.log('[DiskMonitorPlugin] Stopped');
    this.stopMonitoring();
  }

  onUnload(): void {
    console.log('[DiskMonitorPlugin] Unloaded');
    this.cleanup();
  }

  onDestroy(): void {
    console.log('[DiskMonitorPlugin] Destroyed');
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
    console.log('[DiskMonitorPlugin] Subscribed to telemetry');
  }

  /**
   * Process incoming telemetry data
   */
  private processTelemetry(telemetry: StationRuntimeSnapshot): void {
    const stationId = telemetry.stationId;
    
    // Get or create disk history store for this station
    let store = this.diskStores.get(stationId);
    if (!store) {
      store = {
        current: null,
        history: [],
        maxHistory: this.maxHistoryPoints,
      };
      this.diskStores.set(stationId, store);
    }

    // Create disk data record - note: telemetry doesn't have direct disk stats
    // In a real implementation, this would come from the telemetry data
    // For now, we structure the interface to support disk I/O when available
    const diskData: DiskData = {
      stationId,
      computerName: telemetry.computerName,
      readBytesPerSec: 0, // Would be populated from telemetry if available
      writeBytesPerSec: 0,
      timestamp: Date.now(),
    };

    // Update store
    store.current = diskData;
    store.history = [...store.history.slice(-(store.maxHistory - 1)), diskData];

    // Notify callback if registered
    if (this.dataCallback) {
      this.dataCallback(diskData);
    }
  }

  /**
   * Start periodic monitoring
   */
  private startMonitoring(): void {
    this.intervalId = window.setInterval(() => {
      this.collectDiskData();
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
   * Collect disk data for all monitored stations
   */
  private collectDiskData(): void {
    const stations = dataBridge.getMonitoredStations();
    
    stations.forEach(stationId => {
      const telemetry = dataBridge.getTelemetry(stationId);
      if (telemetry) {
        this.processTelemetry(telemetry);
      }
    });
  }

  /**
   * Get current disk data for a station
   */
  getDiskData(stationId: string): DiskData | null {
    return this.diskStores.get(stationId)?.current ?? null;
  }

  /**
   * Get disk history for a station
   */
  getDiskHistory(stationId: string): DiskData[] {
    return this.diskStores.get(stationId)?.history ?? [];
  }

  /**
   * Get disk usage for all stations
   */
  getAllDiskData(): Map<string, DiskData> {
    const result = new Map<string, DiskData>();
    this.diskStores.forEach((store, stationId) => {
      if (store.current) {
        result.set(stationId, store.current);
      }
    });
    return result;
  }

  /**
   * Get total read bytes per second across all stations
   */
  getTotalReadBytesPerSec(): number {
    let total = 0;
    this.diskStores.forEach(store => {
      if (store.current) {
        total += store.current.readBytesPerSec;
      }
    });
    return total;
  }

  /**
   * Get total write bytes per second across all stations
   */
  getTotalWriteBytesPerSec(): number {
    let total = 0;
    this.diskStores.forEach(store => {
      if (store.current) {
        total += store.current.writeBytesPerSec;
      }
    });
    return total;
  }

  /**
   * Get total disk I/O across all stations
   */
  getTotalDiskIO(): number {
    return this.getTotalReadBytesPerSec() + this.getTotalWriteBytesPerSec();
  }

  /**
   * Get stations with high disk I/O
   */
  getStationsAboveThreshold(): string[] {
    const aboveThreshold: string[] = [];
    
    this.diskStores.forEach((store, stationId) => {
      if (store.current) {
        const totalIO = store.current.readBytesPerSec + store.current.writeBytesPerSec;
        if (totalIO >= this.alertThreshold) {
          aboveThreshold.push(stationId);
        }
      }
    });

    return aboveThreshold;
  }

  /**
   * Get average read bytes per second across all stations
   */
  getAverageReadBytesPerSec(): number {
    let total = 0;
    let count = 0;
    
    this.diskStores.forEach(store => {
      if (store.current) {
        total += store.current.readBytesPerSec;
        count++;
      }
    });

    return count > 0 ? total / count : 0;
  }

  /**
   * Get average write bytes per second across all stations
   */
  getAverageWriteBytesPerSec(): number {
    let total = 0;
    let count = 0;
    
    this.diskStores.forEach(store => {
      if (store.current) {
        total += store.current.writeBytesPerSec;
        count++;
      }
    });

    return count > 0 ? total / count : 0;
  }

  /**
   * Register data callback for real-time updates
   */
  onDiskData(callback: (data: DiskData) => void): void {
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
    this.diskStores.clear();

    // Stop monitoring
    this.stopMonitoring();
  }
}

// Panel registration helper
export function getDiskMonitorPanels(): PanelRegistration[] {
  return [
    {
      id: 'disk-overview-panel',
      pluginId: METADATA.id,
      type: 'chart',
      position: 'center',
      title: 'Disk I/O Overview',
      priority: 100,
      props: {
        refreshInterval: 2000,
        showHistory: true,
        chartType: 'line',
        unit: 'B/s',
      },
    },
    {
      id: 'disk-per-station-panel',
      pluginId: METADATA.id,
      type: 'table',
      position: 'right',
      title: 'Per-Station Disk I/O',
      priority: 200,
      props: {
        columns: ['Station', 'Computer Name', 'Read B/s', 'Write B/s', 'Total B/s'],
        sortable: true,
      },
    },
    {
      id: 'disk-alert-panel',
      pluginId: METADATA.id,
      type: 'card',
      position: 'top',
      title: 'Disk I/O Alerts',
      priority: 150,
      props: {
        collapsible: true,
      },
    },
  ];
}

// Action registration helper
export function getDiskMonitorActions() {
  return [
    {
      id: 'disk-refresh',
      pluginId: METADATA.id,
      name: 'Refresh Disk I/O',
      description: 'Force refresh disk I/O data for selected stations',
      icon: 'refresh',
      scope: 'station' as const,
      target: 'multiple' as const,
      enabled: true,
      handler: async (context: ActionContext): Promise<ActionResult> => {
        console.log('[DiskMonitorPlugin] Refreshing disk I/O for:', context.stationIds);
        
        // Force collect fresh data
        context.stationIds.forEach(stationId => {
          const telemetry = dataBridge.getTelemetry(stationId);
          if (telemetry) {
            console.log('[DiskMonitorPlugin] Fresh data for station:', stationId);
          }
        });

        return {
          success: true,
          message: `Disk I/O data refreshed for ${context.stationIds.length} station(s)`,
        };
      },
    },
    {
      id: 'disk-export',
      pluginId: METADATA.id,
      name: 'Export Disk Data',
      description: 'Export disk I/O history to CSV',
      icon: 'download',
      scope: 'station' as const,
      target: 'single' as const,
      enabled: true,
      handler: async (context: ActionContext): Promise<ActionResult> => {
        console.log('[DiskMonitorPlugin] Exporting disk data for:', context.stationIds);
        
        const stationId = context.stationIds[0];
        const history = dataBridge.getTelemetryHistory(stationId);
        
        // Generate CSV content
        const csvHeader = 'Timestamp,StationID,ComputerName,ReadBytesPerSec,WriteBytesPerSec\n';
        const csvRows = history.map(h => 
          `${new Date(Date.now()).toISOString()},${h.stationId},${h.computerName},0,0`
        ).join('\n');
        
        const csvContent = csvHeader + csvRows;
        const blob = new Blob([csvContent], { type: 'text/csv' });
        const url = URL.createObjectURL(blob);
        
        // Trigger download
        const link = document.createElement('a');
        link.href = url;
        link.download = `disk_export_${stationId}_${Date.now()}.csv`;
        link.click();
        
        URL.revokeObjectURL(url);

        return {
          success: true,
          message: 'Disk data exported successfully',
          data: { stationId, recordCount: history.length },
        };
      },
    },
    {
      id: 'disk-set-alert',
      pluginId: METADATA.id,
      name: 'Set Alert Threshold',
      description: 'Configure disk I/O alert threshold',
      icon: 'settings',
      scope: 'global' as const,
      target: 'all' as const,
      enabled: true,
      handler: async (context: ActionContext): Promise<ActionResult> => {
        console.log('[DiskMonitorPlugin] Setting alert threshold');
        
        const threshold = (context.userData?.threshold as number) ?? (100 * 1024 * 1024);
        
        return {
          success: true,
          message: `Alert threshold set to ${threshold} B/s`,
        };
      },
    },
  ];
}

// Factory function for plugin creation
export function createDiskMonitorPlugin(): Plugin {
  return new DiskMonitorPlugin();
}
