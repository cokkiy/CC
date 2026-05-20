/**
 * DataBridge - WebSocket data to Store bridging
 * Handles real-time data flow from WebSocket to plugin data stores
 */

import type {
  DataSubscription,
  DataType,
} from './types';

import type {
  StationRuntimeSnapshot,
  Station,
} from '../types';

// Typed callback definitions
export type TelemetryCallback = (data: StationRuntimeSnapshot) => void;
export type StationStatusCallback = (data: { stationId: string; online: boolean }) => void;
export type StationListCallback = (data: { stations: Station[]; action: 'add' | 'remove' | 'update' }) => void;
export type CommandResultCallback = (data: { stationId: string; command: string; result: unknown }) => void;
export type CustomDataCallback = (data: unknown) => void;
export type GenericDataCallback = (data: unknown) => void;

interface BridgeEventMap {
  'telemetry': TelemetryCallback;
  'station_status': StationStatusCallback;
  'station_list': StationListCallback;
  'command_result': CommandResultCallback;
  'custom': CustomDataCallback;
  '*': GenericDataCallback;
}

type BridgeEventHandler<K extends keyof BridgeEventMap> = BridgeEventMap[K];

// Event types for data bridge
export interface DataBridgeEvent {
  type: DataType;
  timestamp: number;
  data: unknown;
  source?: string;
}

// Store for telemetry data per station
interface TelemetryStore {
  current: StationRuntimeSnapshot | null;
  history: StationRuntimeSnapshot[];
  maxHistory: number;
}

export class DataBridge {
  private subscriptions: Map<string, DataSubscription> = new Map();
  private telemetryStores: Map<string, TelemetryStore> = new Map();
  private handlers: Map<string, Set<BridgeEventHandler<keyof BridgeEventMap>>> = new Map();
  private maxHistory: number = 60; // Keep 60 data points per station
  private isConnected: boolean = false;

  // Singleton
  private static instance: DataBridge | null = null;

  public static getInstance(): DataBridge {
    if (!DataBridge.instance) {
      DataBridge.instance = new DataBridge();
    }
    return DataBridge.instance;
  }

  constructor() {
    // Initialize
  }

  /**
   * Set WebSocket connection state
   */
  setConnected(connected: boolean): void {
    const wasConnected = this.isConnected;
    this.isConnected = connected;
    
    if (!connected && wasConnected) {
      // Clear all subscriptions on disconnect
      console.log('[DataBridge] WebSocket disconnected, subscriptions preserved');
    } else if (connected && !wasConnected) {
      console.log('[DataBridge] WebSocket connected');
    }
  }

  /**
   * Check if connected
   */
  get connected(): boolean {
    return this.isConnected;
  }

  /**
   * Subscribe to a data type
   */
  subscribe(
    dataType: DataType,
    callback: (data: unknown) => void,
    filter?: Record<string, unknown>
  ): string {
    const id = `sub_${Date.now()}_${Math.random().toString(36).slice(2, 9)}`;
    
    const subscription: DataSubscription = {
      id,
      pluginId: 'unknown', // Will be set by plugin
      dataType,
      filter,
      callback,
    };

    this.subscriptions.set(id, subscription);
    console.log(`[DataBridge] Subscribed to ${dataType} with id ${id}`);
    
    return id;
  }

  /**
   * Unsubscribe by ID
   */
  unsubscribe(subscriptionId: string): boolean {
    const deleted = this.subscriptions.delete(subscriptionId);
    if (deleted) {
      console.log(`[DataBridge] Unsubscribed ${subscriptionId}`);
    }
    return deleted;
  }

  /**
   * Unsubscribe all for a plugin
   */
  unsubscribeByPlugin(pluginId: string): string[] {
    const removed: string[] = [];
    
    this.subscriptions.forEach((sub, id) => {
      if (sub.pluginId === pluginId) {
        this.subscriptions.delete(id);
        removed.push(id);
      }
    });

    console.log(`[DataBridge] Unsubscribed ${removed.length} subscriptions for plugin ${pluginId}`);
    return removed;
  }

  /**
   * Get subscription by ID
   */
  getSubscription(id: string): DataSubscription | null {
    return this.subscriptions.get(id) ?? null;
  }

  /**
   * Get all subscriptions
   */
  getAllSubscriptions(): DataSubscription[] {
    return Array.from(this.subscriptions.values());
  }

  /**
   * Get subscriptions by data type
   */
  getSubscriptionsByType(dataType: DataType): DataSubscription[] {
    return Array.from(this.subscriptions.values())
      .filter(sub => sub.dataType === dataType);
  }

  /**
   * Get subscriptions by plugin
   */
  getSubscriptionsByPlugin(pluginId: string): DataSubscription[] {
    return Array.from(this.subscriptions.values())
      .filter(sub => sub.pluginId === pluginId);
  }

  /**
   * Set the plugin ID for a subscription
   */
  setSubscriptionPlugin(subscriptionId: string, pluginId: string): void {
    const sub = this.subscriptions.get(subscriptionId);
    if (sub) {
      sub.pluginId = pluginId;
    }
  }

  // Event handlers
  on<K extends keyof BridgeEventMap>(event: K, handler: BridgeEventHandler<K>): () => void {
    if (!this.handlers.has(event)) {
      this.handlers.set(event, new Set());
    }
    this.handlers.get(event)!.add(handler as BridgeEventHandler<keyof BridgeEventMap>);
    
    return () => {
      this.handlers.get(event)?.delete(handler as BridgeEventHandler<keyof BridgeEventMap>);
    };
  }

  off<K extends keyof BridgeEventMap>(event: K, handler: BridgeEventHandler<K>): void {
    this.handlers.get(event)?.delete(handler as BridgeEventHandler<keyof BridgeEventMap>);
  }

  /**
   * Process incoming telemetry data
   */
  processTelemetry(data: StationRuntimeSnapshot): void {
    const stationId = data.stationId;
    
    // Update store
    let store = this.telemetryStores.get(stationId);
    if (!store) {
      store = {
        current: null,
        history: [],
        maxHistory: this.maxHistory,
      };
      this.telemetryStores.set(stationId, store);
    }

    store.current = data;
    store.history = [...store.history.slice(-(store.maxHistory - 1)), data];

    // Emit to specific handlers
    const event: DataBridgeEvent = {
      type: 'telemetry',
      timestamp: Date.now(),
      data,
      source: stationId,
    };

    this.emit('telemetry', data, event);
  }

  /**
   * Process station status change
   */
  processStationStatus(stationId: string, online: boolean): void {
    const event: DataBridgeEvent = {
      type: 'station_status',
      timestamp: Date.now(),
      data: { stationId, online },
      source: stationId,
    };

    this.emit('station_status', { stationId, online }, event);
  }

  /**
   * Process station list change
   */
  processStationList(stations: Station[], action: 'add' | 'remove' | 'update'): void {
    const event: DataBridgeEvent = {
      type: 'station_list',
      timestamp: Date.now(),
      data: { stations, action },
    };

    this.emit('station_list', { stations, action }, event);
  }

  /**
   * Process command result
   */
  processCommandResult(stationId: string, command: string, result: unknown): void {
    const event: DataBridgeEvent = {
      type: 'command_result',
      timestamp: Date.now(),
      data: { stationId, command, result },
      source: stationId,
    };

    this.emit('command_result', { stationId, command, result }, event);
  }

  /**
   * Process custom data type
   */
  processCustomData(data: unknown, source?: string): void {
    const event: DataBridgeEvent = {
      type: 'custom',
      timestamp: Date.now(),
      data,
      source,
    };

    this.emit('custom', data, event);
  }

  /**
   * Process generic data
   */
  processData(dataType: DataType, data: unknown, source?: string): void {
    switch (dataType) {
      case 'telemetry':
        this.processTelemetry(data as StationRuntimeSnapshot);
        break;
      case 'station_status':
        const { stationId, online } = data as { stationId: string; online: boolean };
        this.processStationStatus(stationId, online);
        break;
      case 'station_list':
        const { stations, action } = data as { stations: Station[]; action: 'add' | 'remove' | 'update' };
        this.processStationList(stations, action);
        break;
      case 'command_result':
        const { stationId: sid, command, result } = data as { stationId: string; command: string; result: unknown };
        this.processCommandResult(sid, command, result);
        break;
      default:
        this.processCustomData(data, source);
    }
  }

  /**
   * Get current telemetry for a station
   */
  getTelemetry(stationId: string): StationRuntimeSnapshot | null {
    return this.telemetryStores.get(stationId)?.current ?? null;
  }

  /**
   * Get telemetry history for a station
   */
  getTelemetryHistory(stationId: string): StationRuntimeSnapshot[] {
    return this.telemetryStores.get(stationId)?.history ?? [];
  }

  /**
   * Get all stations with telemetry data
   */
  getMonitoredStations(): string[] {
    return Array.from(this.telemetryStores.keys());
  }

  /**
   * Clear telemetry for a station
   */
  clearTelemetry(stationId: string): void {
    this.telemetryStores.delete(stationId);
  }

  /**
   * Clear all telemetry data
   */
  clearAllTelemetry(): void {
    this.telemetryStores.clear();
  }

  /**
   * Set max history size per station
   */
  setMaxHistory(max: number): void {
    this.maxHistory = max;
    this.telemetryStores.forEach(store => {
      store.maxHistory = max;
      if (store.history.length > max) {
        store.history = store.history.slice(-max);
      }
    });
  }

  /**
   * Get subscription count
   */
  get subscriptionCount(): number {
    return this.subscriptions.size;
  }

  /**
   * Check if has subscriptions for a data type
   */
  hasSubscriptionsFor(dataType: DataType): boolean {
    return Array.from(this.subscriptions.values())
      .some(sub => sub.dataType === dataType);
  }

  // eslint-disable-next-line @typescript-eslint/no-explicit-any
  private emit(type: DataType, data: any, event: DataBridgeEvent): void {
    // Call specific handlers
    const handlers = this.handlers.get(type);
    if (handlers) {
      handlers.forEach(handler => {
        try {
          handler(data);
        } catch (error) {
          console.error(`[DataBridge] Handler error for ${type}:`, error);
        }
      });
    }

    // Call wildcard handlers
    const wildcardHandlers = this.handlers.get('*');
    if (wildcardHandlers) {
      wildcardHandlers.forEach(handler => {
        try {
          handler(data);
        } catch (error) {
          console.error(`[DataBridge] Wildcard handler error:`, error);
        }
      });
    }

    // Call matching subscriptions
    this.subscriptions.forEach(sub => {
      if (sub.dataType === type) {
        try {
          // Apply filter if exists
          if (sub.filter && !this.matchesFilter(data, sub.filter)) {
            return;
          }
          sub.callback(data);
        } catch (error) {
          console.error(`[DataBridge] Subscription callback error for ${type}:`, error);
        }
      }
    });
  }

  private matchesFilter(data: unknown, filter: Record<string, unknown>): boolean {
    for (const [key, value] of Object.entries(filter)) {
      const dataValue = (data as Record<string, unknown>)[key];
      if (dataValue !== value) {
        return false;
      }
    }
    return true;
  }
}

// Export singleton
export const dataBridge = DataBridge.getInstance();
