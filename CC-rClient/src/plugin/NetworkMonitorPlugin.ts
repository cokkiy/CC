/**
 * CC-Network-Monitor Plugin
 * Monitors network usage across stations using telemetry data from DataBridge
 */

import type { Plugin, PluginMetadata, PluginConfig } from './types';
import type { ActionContext, ActionResult, PanelRegistration } from './types';
import type { StationRuntimeSnapshot } from '../types';
import { dataBridge } from './DataBridge';

// Plugin metadata
const METADATA: PluginMetadata = {
  id: 'cc-network-monitor',
  name: 'Network Monitor',
  version: '1.0.0',
  description: 'Monitors network usage across stations with real-time telemetry',
  author: 'CC Team',
  builtIn: true,
};

// Network data storage per station
interface NetworkData {
  stationId: string;
  computerName: string;
  bytesReceivedPerSec: number;
  bytesSentPerSec: number;
  packetsReceivedPerSec: number;
  packetsSentPerSec: number;
  errorsReceivedPerSec: number;
  errorsSentPerSec: number;
  timestamp: number;
}

interface NetworkHistory {
  current: NetworkData | null;
  history: NetworkData[];
  maxHistory: number;
}

// Default configuration
export const DEFAULT_CONFIG: PluginConfig = {
  enabled: true,
  priority: 50,
  config: {
    refreshInterval: 2000,
    alertThreshold: 1000, // errors per second threshold
    maxHistoryPoints: 60,
  },
};

// Network Monitor Plugin Implementation
export class NetworkMonitorPlugin implements Plugin {
  readonly metadata = METADATA;
  private intervalId?: number;
  private networkStores: Map<string, NetworkHistory> = new Map();
  private refreshInterval: number = 2000;
  private alertThreshold: number = 1000;
  private maxHistoryPoints: number = 60;
  private subscriptions: string[] = [];
  private dataCallback: ((data: NetworkData) => void) | null = null;

  onLoad(): void {
    console.log('[NetworkMonitorPlugin] Loaded');
  }

  onInit(): void {
    console.log('[NetworkMonitorPlugin] Initialized');
    
    // Subscribe to telemetry data
    this.subscribeToTelemetry();
    
    // Start monitoring
    this.startMonitoring();
  }

  onStop(): void {
    console.log('[NetworkMonitorPlugin] Stopped');
    this.stopMonitoring();
  }

  onUnload(): void {
    console.log('[NetworkMonitorPlugin] Unloaded');
    this.cleanup();
  }

  onDestroy(): void {
    console.log('[NetworkMonitorPlugin] Destroyed');
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
    console.log('[NetworkMonitorPlugin] Subscribed to telemetry');
  }

  /**
   * Process incoming telemetry data
   */
  private processTelemetry(telemetry: StationRuntimeSnapshot): void {
    const stationId = telemetry.stationId;
    
    // Get or create network history store for this station
    let store = this.networkStores.get(stationId);
    if (!store) {
      store = {
        current: null,
        history: [],
        maxHistory: this.maxHistoryPoints,
      };
      this.networkStores.set(stationId, store);
    }

    // Aggregate network stats from all interfaces
    const networkStats = telemetry.networkStats || [];
    let totalBytesReceived = 0;
    let totalBytesSent = 0;
    let totalPacketsReceived = 0;
    let totalPacketsSent = 0;
    let totalErrorsReceived = 0;
    let totalErrorsSent = 0;

    networkStats.forEach(stat => {
      totalBytesReceived += stat.bytesReceivedPerSec || 0;
      totalBytesSent += stat.bytesSentedPerSec || 0;
      totalPacketsReceived += stat.unicastPacketReceived || 0;
      totalPacketsSent += stat.unicastPacketSented || 0;
    });

    // Create network data record
    const networkData: NetworkData = {
      stationId,
      computerName: telemetry.computerName,
      bytesReceivedPerSec: totalBytesReceived,
      bytesSentPerSec: totalBytesSent,
      packetsReceivedPerSec: totalPacketsReceived,
      packetsSentPerSec: totalPacketsSent,
      errorsReceivedPerSec: totalErrorsReceived,
      errorsSentPerSec: totalErrorsSent,
      timestamp: Date.now(),
    };

    // Update store
    store.current = networkData;
    store.history = [...store.history.slice(-(store.maxHistory - 1)), networkData];

    // Notify callback if registered
    if (this.dataCallback) {
      this.dataCallback(networkData);
    }
  }

  /**
   * Start periodic monitoring
   */
  private startMonitoring(): void {
    this.intervalId = window.setInterval(() => {
      this.collectNetworkData();
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
   * Collect network data for all monitored stations
   */
  private collectNetworkData(): void {
    const stations = dataBridge.getMonitoredStations();
    
    stations.forEach(stationId => {
      const telemetry = dataBridge.getTelemetry(stationId);
      if (telemetry) {
        this.processTelemetry(telemetry);
      }
    });
  }

  /**
   * Get current network data for a station
   */
  getNetworkData(stationId: string): NetworkData | null {
    return this.networkStores.get(stationId)?.current ?? null;
  }

  /**
   * Get network history for a station
   */
  getNetworkHistory(stationId: string): NetworkData[] {
    return this.networkStores.get(stationId)?.history ?? [];
  }

  /**
   * Get network usage for all stations
   */
  getAllNetworkData(): Map<string, NetworkData> {
    const result = new Map<string, NetworkData>();
    this.networkStores.forEach((store, stationId) => {
      if (store.current) {
        result.set(stationId, store.current);
      }
    });
    return result;
  }

  /**
   * Get total bytes received per second across all stations
   */
  getTotalBytesReceived(): number {
    let total = 0;
    this.networkStores.forEach(store => {
      if (store.current) {
        total += store.current.bytesReceivedPerSec;
      }
    });
    return total;
  }

  /**
   * Get total bytes sent per second across all stations
   */
  getTotalBytesSent(): number {
    let total = 0;
    this.networkStores.forEach(store => {
      if (store.current) {
        total += store.current.bytesSentPerSec;
      }
    });
    return total;
  }

  /**
   * Get total errors per second across all stations
   */
  getTotalErrors(): number {
    let total = 0;
    this.networkStores.forEach(store => {
      if (store.current) {
        total += store.current.errorsReceivedPerSec + store.current.errorsSentPerSec;
      }
    });
    return total;
  }

  /**
   * Get stations with high error rates
   */
  getStationsWithHighErrors(): string[] {
    const highErrorStations: string[] = [];
    
    this.networkStores.forEach((store, stationId) => {
      if (store.current) {
        const totalErrors = store.current.errorsReceivedPerSec + store.current.errorsSentPerSec;
        if (totalErrors >= this.alertThreshold) {
          highErrorStations.push(stationId);
        }
      }
    });

    return highErrorStations;
  }

  /**
   * Get average bytes received per second across all stations
   */
  getAverageBytesReceived(): number {
    let total = 0;
    let count = 0;
    
    this.networkStores.forEach(store => {
      if (store.current) {
        total += store.current.bytesReceivedPerSec;
        count++;
      }
    });

    return count > 0 ? total / count : 0;
  }

  /**
   * Get average bytes sent per second across all stations
   */
  getAverageBytesSent(): number {
    let total = 0;
    let count = 0;
    
    this.networkStores.forEach(store => {
      if (store.current) {
        total += store.current.bytesSentPerSec;
        count++;
      }
    });

    return count > 0 ? total / count : 0;
  }

  /**
   * Register data callback for real-time updates
   */
  onNetworkData(callback: (data: NetworkData) => void): void {
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
    this.networkStores.clear();

    // Stop monitoring
    this.stopMonitoring();
  }
}

// Panel registration helper
export function getNetworkMonitorPanels(): PanelRegistration[] {
  return [
    {
      id: 'network-overview-panel',
      pluginId: METADATA.id,
      type: 'chart',
      position: 'center',
      title: 'Network Overview',
      priority: 100,
      props: {
        refreshInterval: 2000,
        showHistory: true,
        chartType: 'line',
        unit: 'B/s',
      },
    },
    {
      id: 'network-per-station-panel',
      pluginId: METADATA.id,
      type: 'table',
      position: 'right',
      title: 'Per-Station Network Usage',
      priority: 200,
      props: {
        columns: ['Station', 'Computer Name', 'Bytes Received/s', 'Bytes Sent/s', 'Packets Received/s', 'Packets Sent/s'],
        sortable: true,
      },
    },
    {
      id: 'network-errors-panel',
      pluginId: METADATA.id,
      type: 'card',
      position: 'top',
      title: 'Network Errors',
      priority: 150,
      props: {
        collapsible: true,
      },
    },
  ];
}

// Action registration helper
export function getNetworkMonitorActions() {
  return [
    {
      id: 'network-refresh',
      pluginId: METADATA.id,
      name: 'Refresh Network',
      description: 'Force refresh network data for selected stations',
      icon: 'refresh',
      scope: 'station' as const,
      target: 'multiple' as const,
      enabled: true,
      handler: async (context: ActionContext): Promise<ActionResult> => {
        console.log('[NetworkMonitorPlugin] Refreshing network for:', context.stationIds);
        
        // Force collect fresh data
        context.stationIds.forEach(stationId => {
          const telemetry = dataBridge.getTelemetry(stationId);
          if (telemetry) {
            console.log('[NetworkMonitorPlugin] Fresh data for station:', stationId, 'NetworkStats:', telemetry.networkStats?.length);
          }
        });

        return {
          success: true,
          message: `Network data refreshed for ${context.stationIds.length} station(s)`,
        };
      },
    },
    {
      id: 'network-export',
      pluginId: METADATA.id,
      name: 'Export Network Data',
      description: 'Export network history to CSV',
      icon: 'download',
      scope: 'station' as const,
      target: 'single' as const,
      enabled: true,
      handler: async (context: ActionContext): Promise<ActionResult> => {
        console.log('[NetworkMonitorPlugin] Exporting network data for:', context.stationIds);
        
        const stationId = context.stationIds[0];
        const history = dataBridge.getTelemetryHistory(stationId);
        
        // Generate CSV content
        const csvHeader = 'Timestamp,StationID,ComputerName,BytesReceived/s,BytesSent/s,PacketsReceived/s,PacketsSent/s\n';
        const csvRows = history.map(h => {
          const networkStats = h.networkStats || [];
          let bytesRecv = 0;
          let bytesSent = 0;
          let packetsRecv = 0;
          let packetsSent = 0;
          
          networkStats.forEach(stat => {
            bytesRecv += stat.bytesReceivedPerSec || 0;
            bytesSent += stat.bytesSentedPerSec || 0;
            packetsRecv += stat.unicastPacketReceived || 0;
            packetsSent += stat.unicastPacketSented || 0;
          });
          
          return `${new Date(Date.now()).toISOString()},${h.stationId},${h.computerName},${bytesRecv},${bytesSent},${packetsRecv},${packetsSent}`;
        }).join('\n');
        
        const csvContent = csvHeader + csvRows;
        const blob = new Blob([csvContent], { type: 'text/csv' });
        const url = URL.createObjectURL(blob);
        
        // Trigger download
        const link = document.createElement('a');
        link.href = url;
        link.download = `network_export_${stationId}_${Date.now()}.csv`;
        link.click();
        
        URL.revokeObjectURL(url);

        return {
          success: true,
          message: 'Network data exported successfully',
          data: { stationId, recordCount: history.length },
        };
      },
    },
    {
      id: 'network-set-alert',
      pluginId: METADATA.id,
      name: 'Set Error Alert Threshold',
      description: 'Configure network error alert threshold',
      icon: 'settings',
      scope: 'global' as const,
      target: 'all' as const,
      enabled: true,
      handler: async (context: ActionContext): Promise<ActionResult> => {
        console.log('[NetworkMonitorPlugin] Setting alert threshold');
        
        const threshold = (context.userData?.threshold as number) ?? 1000;
        
        return {
          success: true,
          message: `Alert threshold set to ${threshold} errors/s`,
        };
      },
    },
  ];
}

// Factory function for plugin creation
export function createNetworkMonitorPlugin(): Plugin {
  return new NetworkMonitorPlugin();
}
