/**
 * Layout Configuration Schema & Types
 * Defines the structure for config-driven UI layouts
 */

import type { PanelPosition, PanelType } from '../types';

// Re-export for方便内部组件使用
export type { PanelPosition, PanelType } from '../types';

// ============================================
// Layout Configuration Schema
// ============================================

/**
 * Root layout configuration
 */
export interface LayoutConfig {
  version: string;
  name: string;
  description?: string;
  theme?: LayoutTheme;
  panels: PanelConfig[];
  grid?: GridConfig;
  alerts?: AlertConfig;
}

export interface LayoutTheme {
  primaryColor?: string;
  secondaryColor?: string;
  backgroundColor?: string;
  textColor?: string;
  borderRadius?: number;
  spacing?: number;
}

export interface GridConfig {
  columns?: number;
  rowHeight?: number;
  gap?: number;
  responsive?: boolean;
}

/**
 * Individual panel configuration
 */
export interface PanelConfig {
  id: string;
  type: PanelType;
  position: PanelPosition;
  title: string;
  priority?: number;
  visible?: boolean;
  collapsible?: boolean;
  
  // Dimensions
  width?: number | string;
  height?: number | string;
  minWidth?: number;
  maxWidth?: number;
  minHeight?: number;
  maxHeight?: number;
  
  // Styling
  className?: string;
  style?: Record<string, string | number>;
  
  // Data binding
  dataSource?: DataSourceConfig;
  
  // Panel-specific props
  props?: Record<string, unknown>;
  
  // Children for container panels
  children?: PanelConfig[];
  
  // Conditional rendering
  condition?: ConditionConfig;
}

/**
 * Data source configuration for a panel
 */
export interface DataSourceConfig {
  type: 'telemetry' | 'station_status' | 'station_list' | 'command_result' | 'custom';
  stationIds?: string[];
  groupId?: string;
  refreshInterval?: number;
  transform?: string; // JSONPath or transform function name
}

/**
 * Conditional rendering configuration
 */
export interface ConditionConfig {
  type: 'always' | 'has_data' | 'equals' | 'not_equals' | 'greater_than' | 'less_than';
  field?: string;
  value?: unknown;
}

/**
 * Alert/Overlay configuration
 */
export interface AlertConfig {
  enabled: boolean;
  position?: PanelPosition;
  maxVisible?: number;
  autoHideAfter?: number; // milliseconds
  panels?: AlertPanelConfig[];
}

export interface AlertPanelConfig {
  id: string;
  title: string;
  message: string;
  severity: 'info' | 'warning' | 'error' | 'success';
  source?: string; // pluginId or stationId
  timestamp?: number;
  acknowledged?: boolean;
  actions?: AlertActionConfig[];
  condition?: ConditionConfig;
}

export interface AlertActionConfig {
  id: string;
  label: string;
  action: string; // action ID
  style?: 'primary' | 'secondary' | 'danger';
}

// ============================================
// Layout Manager Types
// ============================================

/**
 * Layout manager state
 */
export interface LayoutState {
  currentLayout: LayoutConfig | null;
  customLayouts: Record<string, LayoutConfig>;
  activeCustomLayoutId: string | null;
  panelStates: Record<string, PanelState>;
}

export interface PanelState {
  id: string;
  visible: boolean;
  collapsed: boolean;
  loading: boolean;
  error?: string;
}

/**
 * Layout validation result
 */
export interface LayoutValidationResult {
  valid: boolean;
  errors: LayoutValidationError[];
  warnings: LayoutValidationWarning[];
}

export interface LayoutValidationError {
  path: string;
  message: string;
  code: string;
}

export interface LayoutValidationWarning {
  path: string;
  message: string;
  code: string;
}

// ============================================
// Built-in Layout Presets
// ============================================

/**
 * Default layout preset
 */
export const DEFAULT_LAYOUT: LayoutConfig = {
  version: '1.0.0',
  name: 'Default Layout',
  description: 'Standard CC Client layout with CPU, Memory, and Network panels',
  panels: [
    {
      id: 'cpu-overview',
      type: 'chart',
      position: 'center',
      title: 'CPU Overview',
      priority: 100,
      dataSource: {
        type: 'telemetry',
        refreshInterval: 2000,
      },
      props: {
        chartType: 'line',
        dataKey: 'cpu',
        color: '#d64545',
      },
    },
    {
      id: 'memory-overview',
      type: 'chart',
      position: 'center',
      title: 'Memory Usage',
      priority: 101,
      dataSource: {
        type: 'telemetry',
        refreshInterval: 2000,
      },
      props: {
        chartType: 'line',
        dataKey: 'memory',
        color: '#c78a00',
      },
    },
    {
      id: 'network-traffic',
      type: 'chart',
      position: 'bottom',
      title: 'Network Traffic',
      priority: 200,
      dataSource: {
        type: 'telemetry',
        refreshInterval: 2000,
      },
      props: {
        chartType: 'line',
        dataKeys: ['rxbps', 'txbps'],
        colors: ['#1f9d55', '#2d8cf0'],
      },
    },
    {
      id: 'station-list',
      type: 'table',
      position: 'left',
      title: 'Stations',
      priority: 50,
      dataSource: {
        type: 'station_list',
      },
      props: {
        columns: ['name', 'status', 'ip', 'lastSeen'],
        sortable: true,
        filterable: true,
      },
    },
  ],
  alerts: {
    enabled: true,
    position: 'overlay',
    maxVisible: 5,
    autoHideAfter: 10000,
  },
};

/**
 * Minimal layout preset - for low-resource environments
 */
export const MINIMAL_LAYOUT: LayoutConfig = {
  version: '1.0.0',
  name: 'Minimal Layout',
  description: 'Minimal layout with only essential panels',
  panels: [
    {
      id: 'station-list-minimal',
      type: 'table',
      position: 'center',
      title: 'Stations',
      priority: 100,
      dataSource: {
        type: 'station_list',
      },
    },
  ],
  alerts: {
    enabled: true,
    maxVisible: 3,
  },
};

/**
 * Dashboard layout preset - focus on monitoring
 */
export const DASHBOARD_LAYOUT: LayoutConfig = {
  version: '1.0.0',
  name: 'Dashboard Layout',
  description: 'Monitoring-focused dashboard with multiple charts',
  panels: [
    {
      id: 'cpu-chart',
      type: 'chart',
      position: 'top',
      title: 'CPU Usage',
      priority: 100,
      dataSource: {
        type: 'telemetry',
        refreshInterval: 1000,
      },
      props: {
        chartType: 'area',
        dataKey: 'cpu',
        color: '#d64545',
      },
    },
    {
      id: 'memory-chart',
      type: 'chart',
      position: 'top',
      title: 'Memory Usage',
      priority: 101,
      dataSource: {
        type: 'telemetry',
        refreshInterval: 1000,
      },
      props: {
        chartType: 'area',
        dataKey: 'memory',
        color: '#c78a00',
      },
    },
    {
      id: 'station-grid',
      type: 'card',
      position: 'center',
      title: 'Station Status',
      priority: 200,
      dataSource: {
        type: 'station_status',
      },
      props: {
        showCpu: true,
        showMemory: true,
        compact: true,
      },
    },
  ],
  alerts: {
    enabled: true,
    position: 'overlay',
    maxVisible: 10,
    autoHideAfter: 15000,
  },
};

// ============================================
// Layout Preset Registry
// ============================================

export const LAYOUT_PRESETS: Record<string, LayoutConfig> = {
  default: DEFAULT_LAYOUT,
  minimal: MINIMAL_LAYOUT,
  dashboard: DASHBOARD_LAYOUT,
};

/**
 * Get a layout preset by name
 */
export function getLayoutPreset(name: string): LayoutConfig | null {
  return LAYOUT_PRESETS[name] ?? null;
}

/**
 * Get all available layout presets
 */
export function getAllLayoutPresets(): Array<{ id: string; name: string; description?: string }> {
  return Object.entries(LAYOUT_PRESETS).map(([id, config]) => ({
    id,
    name: config.name,
    description: config.description,
  }));
}

// ============================================
// Layout Validator
// ============================================

/**
 * Validate a layout configuration
 */
export function validateLayout(layout: LayoutConfig): LayoutValidationResult {
  const errors: LayoutValidationError[] = [];
  const warnings: LayoutValidationWarning[] = [];

  // Check version
  if (!layout.version) {
    warnings.push({
      path: 'version',
      message: 'Layout version is not specified',
      code: 'MISSING_VERSION',
    });
  }

  // Check panels
  if (!layout.panels || layout.panels.length === 0) {
    warnings.push({
      path: 'panels',
      message: 'Layout has no panels defined',
      code: 'NO_PANELS',
    });
  }

  // Check for duplicate panel IDs
  const panelIds = new Set<string>();
  layout.panels?.forEach((panel, index) => {
    if (panelIds.has(panel.id)) {
      errors.push({
        path: `panels[${index}].id`,
        message: `Duplicate panel ID: ${panel.id}`,
        code: 'DUPLICATE_PANEL_ID',
      });
    }
    panelIds.add(panel.id);

    // Check panel type
    const validTypes = ['card', 'chart', 'table', 'form', 'custom'];
    if (!validTypes.includes(panel.type)) {
      errors.push({
        path: `panels[${index}].type`,
        message: `Invalid panel type: ${panel.type}. Valid types: ${validTypes.join(', ')}`,
        code: 'INVALID_PANEL_TYPE',
      });
    }

    // Check panel position
    const validPositions = ['top', 'bottom', 'left', 'right', 'center', 'overlay'];
    if (!validPositions.includes(panel.position)) {
      errors.push({
        path: `panels[${index}].position`,
        message: `Invalid panel position: ${panel.position}. Valid positions: ${validPositions.join(', ')}`,
        code: 'INVALID_PANEL_POSITION',
      });
    }
  });

  return {
    valid: errors.length === 0,
    errors,
    warnings,
  };
}
