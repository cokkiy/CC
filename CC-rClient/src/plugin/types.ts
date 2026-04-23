/**
 * Plugin System Type Definitions
 * Core types for the CC Client Plugin Architecture
 */

// Plugin lifecycle states
export type PluginState = 
  | 'pending'      // Plugin registered but not yet loaded
  | 'loading'      // Plugin is being loaded
  | 'loaded'       // Plugin loaded successfully
  | 'initializing' // Plugin onInit() is being called
  | 'running'      // Plugin is active and running
  | 'stopping'     // Plugin is being stopped
  | 'stopped'      // Plugin has been stopped
  | 'unloading'    // Plugin is being unloaded
  | 'error';       // Plugin encountered an error

// Plugin metadata
export interface PluginMetadata {
  id: string;                    // Unique plugin identifier (e.g., "cc-cpu-monitor")
  name: string;                  // Human-readable name
  version: string;               // Semantic version (e.g., "1.0.0")
  description?: string;          // Optional description
  author?: string;               // Plugin author
  dependencies?: string[];       // Required plugin IDs
  builtIn?: boolean;            // Whether this is a built-in plugin
}

// Plugin configuration
export interface PluginConfig {
  enabled: boolean;              // Whether plugin is enabled
  priority: number;              // Execution priority (lower = earlier)
  config?: Record<string, unknown>; // Plugin-specific configuration
}

// Base plugin interface
export interface Plugin {
  // Metadata
  readonly metadata: PluginMetadata;
  
  // Lifecycle hooks
  onLoad?(): Promise<void> | void;           // Called when plugin is first loaded
  onInit?(): Promise<void> | void;           // Called when plugin should start
  onStop?(): Promise<void> | void;            // Called when plugin should stop
  onUnload?(): Promise<void> | void;         // Called before plugin is unloaded
  
  // Cleanup
  onDestroy?(): Promise<void> | void;         // Called when plugin is destroyed
}

// UI Panel types
export type PanelType = 'card' | 'chart' | 'table' | 'form' | 'custom';
export type PanelPosition = 'top' | 'bottom' | 'left' | 'right' | 'center' | 'overlay';

export interface PanelRegistration {
  id: string;                    // Unique panel ID
  pluginId: string;              // Owner plugin ID
  type: PanelType;
  position: PanelPosition;
  title: string;                 // Panel title
  priority: number;              // Z-index or layout priority
  props?: Record<string, unknown>; // Panel-specific props
  component?: string;            // Component name or reference
}

// Action types for command registry
export type ActionScope = 'station' | 'group' | 'global';
export type ActionTarget = 'single' | 'multiple' | 'all';

export interface ActionDefinition {
  id: string;                    // Unique action ID
  pluginId: string;              // Owner plugin ID
  name: string;                  // Display name
  description?: string;          // Action description
  icon?: string;                 // Icon identifier
  scope: ActionScope;            // Where action can be applied
  target: ActionTarget;         // Number of targets
  enabled?: boolean;            // Whether action is currently enabled
  shortcut?: string;            // Keyboard shortcut
  handler: (context: ActionContext) => Promise<ActionResult>;
}

export interface ActionContext {
  stationIds: string[];          // Selected station IDs
  groupId?: string;              // If scope is 'group'
  userData?: Record<string, unknown>; // Additional context
}

export interface ActionResult {
  success: boolean;
  message?: string;
  data?: unknown;
}

// Data subscription types
export interface DataSubscription {
  id: string;
  pluginId: string;
  dataType: DataType;
  filter?: Record<string, unknown>;
  callback: (data: unknown) => void;
}

export type DataType = 
  | 'telemetry'           // Station telemetry data
  | 'station_status'       // Station online/offline status
  | 'station_list'         // Station list changes
  | 'command_result'       // Command execution results
  | 'file_transfer'        // File transfer progress
  | 'custom';              // Custom data type

// Plugin events
export interface PluginEvent {
  type: string;
  source: string;         // Plugin ID
  timestamp: number;
  data?: unknown;
}

export type PluginEventHandler = (event: PluginEvent) => void;

// Error handling
export interface PluginError {
  pluginId: string;
  code: string;
  message: string;
  details?: unknown;
  timestamp: number;
}

export type PluginErrorHandler = (error: PluginError) => void;
