/**
 * DynamicPanel - Configuration-driven UI Panel Component
 * Renders panels based on LayoutConfig definitions
 */

import React, { useEffect, useState, useCallback, useMemo } from 'react';
import type { 
  PanelConfig, 
  DataSourceConfig, 
  ConditionConfig,
  PanelState,
  PanelPosition as LayoutPanelPosition,
} from './LayoutConfig';
import type { PanelType } from '../types';
import type { StationRuntimeSnapshot, Station } from '../../types';

// ============================================
// Chart Components (using recharts)
// ============================================

import {
  LineChart,
  Line,
  AreaChart,
  Area,
  BarChart,
  Bar,
  PieChart,
  Pie,
  Cell,
  XAxis,
  YAxis,
  CartesianGrid,
  Tooltip,
  Legend,
  ResponsiveContainer,
} from 'recharts';

// Chart colors
const CHART_COLORS = ['#d64545', '#c78a00', '#1f9d55', '#2d8cf0', '#9b59b6', '#e74c3c'];

// ============================================
// Panel Props Interface
// ============================================

interface DynamicPanelProps {
  config: PanelConfig;
  stationId?: string;
  stationIds?: string[];
  stations?: Station[];
  runtimeData?: Record<string, StationRuntimeSnapshot>;
  onAction?: (actionId: string, context: unknown) => void;
  className?: string;
}

// ============================================
// DynamicPanel Component
// ============================================

export const DynamicPanel: React.FC<DynamicPanelProps> = ({
  config,
  stationId,
  stationIds = [],
  stations = [],
  runtimeData = {},
  onAction,
  className = '',
}) => {
  const [panelState, setPanelState] = useState<PanelState>({
    id: config.id,
    visible: config.visible ?? true,
    collapsed: false,
    loading: false,
  });

  const [data, setData] = useState<unknown>(null);
  const [error, setError] = useState<string | null>(null);

  // Subscribe to data source
  useEffect(() => {
    if (!config.dataSource) return;

    const subscription = subscribeToDataSource(config.dataSource, (newData) => {
      setData(newData);
      setPanelState(prev => ({ ...prev, loading: false }));
    }, (err) => {
      setError(err.message);
      setPanelState(prev => ({ ...prev, loading: false, error: err.message }));
    });

    setPanelState(prev => ({ ...prev, loading: true }));

    return () => {
      subscription?.unsubscribe();
    };
  }, [config.dataSource]);

  // Check condition for rendering
  const shouldRender = useMemo(() => {
    return checkCondition(config.condition, { data, stations, runtimeData });
  }, [config.condition, data, stations, runtimeData]);

  // Handle visibility toggle
  const handleToggle = useCallback(() => {
    setPanelState(prev => ({
      ...prev,
      collapsed: !prev.collapsed,
    }));
  }, []);

  // Handle close
  const handleClose = useCallback(() => {
    setPanelState(prev => ({
      ...prev,
      visible: false,
    }));
  }, []);

  // If should not render, return null
  if (!shouldRender || !panelState.visible) {
    return null;
  }

  // Build class names
  const panelClasses = [
    'dynamic-panel',
    `dynamic-panel--${config.type}`,
    `dynamic-panel--${config.position}`,
    panelState.collapsed ? 'dynamic-panel--collapsed' : '',
    panelState.loading ? 'dynamic-panel--loading' : '',
    error ? 'dynamic-panel--error' : '',
    config.className || '',
    className,
  ].filter(Boolean).join(' ');

  // Build style
  const panelStyle: React.CSSProperties = {
    width: config.width ?? '100%',
    height: config.height ?? 'auto',
    minWidth: config.minWidth,
    maxWidth: config.maxWidth,
    minHeight: config.minHeight,
    maxHeight: config.maxHeight,
    ...config.style,
  };

  return (
    <div className={panelClasses} style={panelStyle} data-panel-id={config.id}>
      {/* Panel Header */}
      <div className="dynamic-panel__header" onClick={config.collapsible ? handleToggle : undefined}>
        <h3 className="dynamic-panel__title">{config.title}</h3>
        <div className="dynamic-panel__actions">
          {config.collapsible && (
            <button 
              className="dynamic-panel__btn dynamic-panel__btn--collapse"
              onClick={handleToggle}
              aria-label={panelState.collapsed ? 'Expand' : 'Collapse'}
            >
              {panelState.collapsed ? '▼' : '▲'}
            </button>
          )}
          <button 
            className="dynamic-panel__btn dynamic-panel__btn--close"
            onClick={handleClose}
            aria-label="Close"
          >
            ×
          </button>
        </div>
      </div>

      {/* Panel Content */}
      {!panelState.collapsed && (
        <div className="dynamic-panel__content">
          {error ? (
            <div className="dynamic-panel__error">
              <span className="dynamic-panel__error-icon">⚠</span>
              <span>{error}</span>
            </div>
          ) : panelState.loading ? (
            <div className="dynamic-panel__loading">
              <div className="dynamic-panel__spinner" />
              <span>Loading...</span>
            </div>
          ) : (
            renderPanelContent(config, data, stationId, stationIds, stations, runtimeData, onAction)
          )}
        </div>
      )}
    </div>
  );
};

// ============================================
// Panel Content Renderers
// ============================================

function renderPanelContent(
  config: PanelConfig,
  data: unknown,
  stationId?: string,
  stationIds?: string[],
  stations?: Station[],
  runtimeData?: Record<string, StationRuntimeSnapshot>,
  onAction?: (actionId: string, context: unknown) => void
): React.ReactNode {
  switch (config.type) {
    case 'chart':
      return renderChartPanel(config, data, stationId, runtimeData);
    case 'table':
      return renderTablePanel(config, data, stationId, stations);
    case 'card':
      return renderCardPanel(config, data, stationId, stations, runtimeData);
    case 'form':
      return renderFormPanel(config, data, onAction);
    case 'custom':
      return renderCustomPanel(config, data);
    default:
      return <div className="dynamic-panel__unknown">Unknown panel type: {config.type}</div>;
  }
}

function renderChartPanel(
  config: PanelConfig,
  data: unknown,
  stationId?: string,
  runtimeData?: Record<string, StationRuntimeSnapshot>
): React.ReactNode {
  const props = config.props || {};
  const chartType = (props.chartType as string) || 'line';
  const dataKey = props.dataKey as string;
  const dataKeys = (props.dataKeys as string[]) || [];
  const color = (props.color as string) || CHART_COLORS[0];
  const colors = (props.colors as string[]) || CHART_COLORS;

  // Transform data for chart
  const chartData = useMemo(() => {
    if (!data) return [];
    if (Array.isArray(data)) return data;
    
    // If we have runtimeData, transform it
    if (runtimeData) {
      const entries = Object.entries(runtimeData).map(([id, runtime]) => ({
        timestamp: runtime.endpoint ? Date.now() : 0,
        ...runtime,
        stationId: id, // 确保 stationId 正确
      }));
      return entries;
    }
    
    return [data];
  }, [data, runtimeData]);

  if (chartData.length === 0) {
    return <div className="dynamic-panel__empty">No data available for chart</div>;
  }

  const yAxisDomain = props.yAxisDomain as [number, number] || [0, 100];

  const renderAxis = () => (
    <>
      <CartesianGrid strokeDasharray="3 3" stroke="var(--cpuPie-track)" />
      <XAxis 
        dataKey="timestamp" 
        tickFormatter={(v) => new Date(v).toLocaleTimeString()}
        tick={{ fontSize: 9 }}
      />
      <YAxis 
        domain={yAxisDomain} 
        tick={{ fontSize: 9 }}
        tickFormatter={props.yAxisFormatter as ((v: number) => string) || ((v: number) => `${v}%`)}
      />
      <Tooltip
        formatter={(v) => [Number(v).toFixed(1), dataKey || 'Value']}
        labelFormatter={(v) => new Date(Number(v)).toLocaleTimeString()}
      />
      {dataKeys.length > 1 && <Legend />}
    </>
  );

  const renderLines = () => {
    if (dataKeys.length > 0) {
      return dataKeys.map((key, index) => (
        <Line
          key={key}
          type="monotone"
          dataKey={key}
          stroke={colors[index % colors.length]}
          strokeWidth={1.5}
          dot={false}
          name={key}
          isAnimationActive={false}
        />
      ));
    }
    
    return (
      <Line
        type="monotone"
        dataKey={dataKey || 'value'}
        stroke={color}
        strokeWidth={1.5}
        dot={false}
        isAnimationActive={false}
      />
    );
  };

  return (
    <div className="dynamic-panel__chart">
      <ResponsiveContainer width="100%" height={200}>
        {chartType === 'area' ? (
          <AreaChart data={chartData} margin={{ top: 5, right: 5, left: -20, bottom: 0 }}>
            {renderAxis()}
            <Area
              type="monotone"
              dataKey={dataKey || 'value'}
              stroke={color}
              fill={color}
              fillOpacity={0.3}
              isAnimationActive={false}
            />
          </AreaChart>
        ) : chartType === 'bar' ? (
          <BarChart data={chartData} margin={{ top: 5, right: 5, left: -20, bottom: 0 }}>
            {renderAxis()}
            <Bar dataKey={dataKey || 'value'} fill={color} isAnimationActive={false} />
          </BarChart>
        ) : (
          <LineChart data={chartData} margin={{ top: 5, right: 5, left: -20, bottom: 0 }}>
            {renderAxis()}
            {renderLines()}
          </LineChart>
        )}
      </ResponsiveContainer>
    </div>
  );
}

function renderTablePanel(
  config: PanelConfig,
  data: unknown,
  stationId?: string,
  stations?: Station[]
): React.ReactNode {
  const props = config.props || {};
  const columns = (props.columns as string[]) || ['name', 'status'];
  const sortable = props.sortable !== false;
  const filterable = props.filterable === true;

  // Get table data
  const tableData = useMemo(() => {
    if (Array.isArray(data)) return data;
    if (stations && stations.length > 0) return stations;
    return [];
  }, [data, stations]);

  const [sortColumn, setSortColumn] = useState<string | null>(null);
  const [sortDirection, setSortDirection] = useState<'asc' | 'desc'>('asc');
  const [filterText, setFilterText] = useState('');

  const sortedData = useMemo(() => {
    let result = [...tableData];
    
    // Filter
    if (filterText) {
      result = result.filter(row => 
        Object.values(row).some(val => 
          String(val).toLowerCase().includes(filterText.toLowerCase())
        )
      );
    }
    
    // Sort
    if (sortColumn) {
      result.sort((a, b) => {
        const aVal = a[sortColumn];
        const bVal = b[sortColumn];
        const comparison = String(aVal).localeCompare(String(bVal), undefined, { numeric: true });
        return sortDirection === 'asc' ? comparison : -comparison;
      });
    }
    
    return result;
  }, [tableData, sortColumn, sortDirection, filterText]);

  const handleSort = (column: string) => {
    if (!sortable) return;
    if (sortColumn === column) {
      setSortDirection(prev => prev === 'asc' ? 'desc' : 'asc');
    } else {
      setSortColumn(column);
      setSortDirection('asc');
    }
  };

  if (tableData.length === 0) {
    return <div className="dynamic-panel__empty">No data available for table</div>;
  }

  return (
    <div className="dynamic-panel__table-container">
      {filterable && (
        <input
          type="text"
          className="dynamic-panel__filter"
          placeholder="Filter..."
          value={filterText}
          onChange={(e) => setFilterText(e.target.value)}
        />
      )}
      <table className="dynamic-panel__table">
        <thead>
          <tr>
            {columns.map(col => (
              <th 
                key={col} 
                onClick={() => handleSort(col)}
                className={sortable ? 'sortable' : ''}
              >
                {col}
                {sortColumn === col && (
                  <span className="dynamic-panel__sort-icon">
                    {sortDirection === 'asc' ? ' ▲' : ' ▼'}
                  </span>
                )}
              </th>
            ))}
          </tr>
        </thead>
        <tbody>
          {sortedData.map((row: Record<string, unknown>, index: number) => (
            <tr key={index}>
              {columns.map(col => (
                <td key={col}>{formatCellValue(row[col])}</td>
              ))}
            </tr>
          ))}
        </tbody>
      </table>
    </div>
  );
}

function renderCardPanel(
  config: PanelConfig,
  data: unknown,
  stationId?: string,
  stations?: Station[],
  runtimeData?: Record<string, StationRuntimeSnapshot>
): React.ReactNode {
  const props = config.props || {};
  const showCpu = props.showCpu !== false;
  const showMemory = props.showMemory !== false;
  const compact = props.compact === true;

  // Build cards from stations
  const cards = useMemo(() => {
    const result: Array<{
      id: string;
      name: string;
      status: string;
      cpu?: number;
      memory?: number;
    }> = [];

    if (stations && stations.length > 0) {
      stations.forEach(station => {
        const runtime = runtimeData?.[station.id];
        result.push({
          id: station.id,
          name: station.name,
          status: runtime ? 'online' : 'offline',
          cpu: runtime?.cpu,
          memory: runtime?.currentMemory,
        });
      });
    } else if (data && typeof data === 'object' && !Array.isArray(data)) {
      // Single station data
      const runtime = data as StationRuntimeSnapshot;
      result.push({
        id: stationId || 'unknown',
        name: (runtime as Record<string, unknown>).computerName as string || 'Unknown',
        status: 'online',
        cpu: runtime.cpu,
        memory: runtime.currentMemory,
      });
    }

    return result;
  }, [stations, runtimeData, data, stationId]);

  if (cards.length === 0) {
    return <div className="dynamic-panel__empty">No cards to display</div>;
  }

  return (
    <div className={`dynamic-panel__cards ${compact ? 'dynamic-panel__cards--compact' : ''}`}>
      {cards.map(card => (
        <div key={card.id} className={`dynamic-panel__card dynamic-panel__card--${card.status}`}>
          <div className="dynamic-panel__card-header">
            <span className="dynamic-panel__card-name">{card.name}</span>
            <span className={`dynamic-panel__card-status dynamic-panel__card-status--${card.status}`}>
              {card.status}
            </span>
          </div>
          {(showCpu || showMemory) && (
            <div className="dynamic-panel__card-metrics">
              {showCpu && card.cpu !== undefined && (
                <div className="dynamic-panel__card-metric">
                  <span className="dynamic-panel__card-metric-label">CPU</span>
                  <span className="dynamic-panel__card-metric-value">{card.cpu.toFixed(1)}%</span>
                </div>
              )}
              {showMemory && card.memory !== undefined && (
                <div className="dynamic-panel__card-metric">
                  <span className="dynamic-panel__card-metric-label">MEM</span>
                  <span className="dynamic-panel__card-metric-value">
                    {formatBytes(card.memory)}
                  </span>
                </div>
              )}
            </div>
          )}
        </div>
      ))}
    </div>
  );
}

function renderFormPanel(
  config: PanelConfig,
  data: unknown,
  onAction?: (actionId: string, context: unknown) => void
): React.ReactNode {
  const props = config.props || {};
  const fields = (props.fields as Array<{
    name: string;
    type: string;
    label: string;
    defaultValue?: unknown;
    options?: Array<{ label: string; value: unknown }>;
  }>) || [];

  return (
    <div className="dynamic-panel__form">
      {fields.map(field => (
        <div key={field.name} className="dynamic-panel__form-field">
          <label className="dynamic-panel__form-label">{field.label}</label>
          {field.type === 'select' ? (
            <select className="dynamic-panel__form-select" name={field.name} defaultValue={field.defaultValue as string}>
              {field.options?.map(opt => (
                <option key={String(opt.value)} value={String(opt.value)}>{opt.label}</option>
              ))}
            </select>
          ) : field.type === 'checkbox' ? (
            <input type="checkbox" name={field.name} defaultChecked={field.defaultValue as boolean} />
          ) : (
            <input 
              type={field.type || 'text'} 
              className="dynamic-panel__form-input"
              name={field.name} 
              defaultValue={field.defaultValue as string}
            />
          )}
        </div>
      ))}
      {onAction && (
        <div className="dynamic-panel__form-actions">
          <button 
            className="dynamic-panel__btn dynamic-panel__btn--primary"
            onClick={() => onAction('submit', {})}
          >
            Submit
          </button>
        </div>
      )}
    </div>
  );
}

function renderCustomPanel(
  config: PanelConfig,
  data: unknown
): React.ReactNode {
  const props = config.props || {};
  const render = props.render as (data: unknown) => React.ReactNode;
  
  if (typeof render === 'function') {
    return render(data);
  }
  
  return (
    <div className="dynamic-panel__custom">
      {props.content as React.ReactNode || 'Custom panel content'}
    </div>
  );
}

// ============================================
// Helper Functions
// ============================================

function subscribeToDataSource(
  dataSource: DataSourceConfig,
  onData: (data: unknown) => void,
  onError: (error: Error) => void
): { unsubscribe: () => void } | null {
  // This would integrate with the DataBridge in a real implementation
  // For now, return a dummy subscription
  return {
    unsubscribe: () => {},
  };
}

function checkCondition(
  condition: ConditionConfig | undefined,
  context: { data: unknown; stations: Station[]; runtimeData: Record<string, StationRuntimeSnapshot> }
): boolean {
  if (!condition || condition.type === 'always') {
    return true;
  }

  const { data, stations, runtimeData } = context;

  switch (condition.type) {
    case 'has_data':
      return data !== null && data !== undefined && 
        (Array.isArray(data) ? data.length > 0 : Object.keys(data).length > 0);
    
    case 'equals':
      return getNestedValue(data, condition.field || '') === condition.value;
    
    case 'not_equals':
      return getNestedValue(data, condition.field || '') !== condition.value;
    
    case 'greater_than': {
      const value = getNestedValue(data, condition.field || '');
      return typeof value === 'number' && value > (condition.value as number);
    }
    
    case 'less_than': {
      const value = getNestedValue(data, condition.field || '');
      return typeof value === 'number' && value < (condition.value as number);
    }
    
    default:
      return true;
  }
}

function getNestedValue(obj: unknown, path: string): unknown {
  if (!path) return obj;
  const keys = path.split('.');
  let current: unknown = obj;
  
  for (const key of keys) {
    if (current === null || current === undefined) return undefined;
    current = (current as Record<string, unknown>)[key];
  }
  
  return current;
}

function formatCellValue(value: unknown): string {
  if (value === null || value === undefined) return '-';
  if (typeof value === 'boolean') return value ? 'Yes' : 'No';
  if (typeof value === 'number') return value.toString();
  if (typeof value === 'string') return value;
  return JSON.stringify(value);
}

function formatBytes(bytes: number): string {
  if (!Number.isFinite(bytes) || bytes <= 0) return '0 B';
  const units = ['B', 'KB', 'MB', 'GB', 'TB'];
  let next = bytes;
  let index = 0;
  while (next >= 1024 && index < units.length - 1) {
    next /= 1024;
    index += 1;
  }
  return `${next.toFixed(index === 0 ? 0 : 1)} ${units[index]}`;
}

// Hooks for DataBridge integration (placeholder)
function useDataBridge() {
  return {
    subscribe: (type: string, callback: (data: unknown) => void) => ({
      unsubscribe: () => {},
    }),
    getTelemetry: (stationId: string) => null,
    getMonitoredStations: () => [],
  };
}

function usePanelRegistry() {
  return {
    register: (config: PanelConfig) => {},
    unregister: (panelId: string) => {},
    getPanel: (panelId: string) => null,
    getAllPanels: () => [],
  };
}

export default DynamicPanel;
