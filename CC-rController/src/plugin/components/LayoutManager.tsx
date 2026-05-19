/**
 * LayoutManager - Layout Configuration Manager Component
 * Manages layout loading, rendering, and state
 */

import React, { useEffect, useState, useCallback, useMemo } from 'react';
import type { 
  LayoutConfig, 
  PanelConfig, 
  AlertConfig,
  LayoutState,
  LayoutValidationResult,
} from './LayoutConfig';
import { 
  DEFAULT_LAYOUT, 
  getLayoutPreset, 
  getAllLayoutPresets 
} from './LayoutConfig';
import type { PanelPosition } from '../types';
import { DynamicPanel } from './DynamicPanel';
import { AlertOverlay, useAlertManager, DEFAULT_ALERT_CONFIG } from './AlertOverlay';
import { panelRegistry } from '../PanelRegistry';
import { dataBridge } from '../DataBridge';
import type { StationRuntimeSnapshot, Station } from '../../types';

// ============================================
// LayoutManager Props
// ============================================

interface LayoutManagerProps {
  /** Initial layout config or preset name */
  initialLayout?: string | LayoutConfig;
  /** Stations data */
  stations?: Station[];
  /** Runtime data per station */
  runtimeData?: Record<string, StationRuntimeSnapshot>;
  /** Callback when panel action is triggered */
  onPanelAction?: (panelId: string, actionId: string, context: unknown) => void;
  /** Callback when layout changes */
  onLayoutChange?: (layout: LayoutConfig) => void;
  /** CSS class name */
  className?: string;
}

// ============================================
// LayoutManager Component
// ============================================

export const LayoutManager: React.FC<LayoutManagerProps> = ({
  initialLayout,
  stations = [],
  runtimeData = {},
  onPanelAction,
  onLayoutChange,
  className = '',
}) => {
  // Layout state
  const [currentLayout, setCurrentLayout] = useState<LayoutConfig | null>(null);
  const [customLayouts, setCustomLayouts] = useState<Record<string, LayoutConfig>>({});
  const [activeCustomLayoutId, setActiveCustomLayoutId] = useState<string | null>(null);
  
  // Panel visibility/collapse state
  const [panelStates, setPanelStates] = useState<Record<string, {
    visible: boolean;
    collapsed: boolean;
  }>>({});

  // Alert manager
  const {
    alerts,
    addAlert,
    removeAlert,
    acknowledgeAlert,
    clearAlerts,
    unreadCount,
  } = useAlertManager({ autoHideAfter: 10000 });

  // Initialize layout
  useEffect(() => {
    let layout: LayoutConfig;

    if (typeof initialLayout === 'string') {
      // It's a preset name
      const preset = getLayoutPreset(initialLayout);
      layout = preset || DEFAULT_LAYOUT;
    } else if (initialLayout) {
      layout = initialLayout;
    } else {
      layout = DEFAULT_LAYOUT;
    }

    // Validate and apply layout
    const validation = validateLayout(layout);
    if (!validation.valid) {
      console.warn('[LayoutManager] Layout validation warnings:', validation.warnings);
    }

    setCurrentLayout(layout);
    onLayoutChange?.(layout);

    // Initialize panel states
    const initialStates: Record<string, { visible: boolean; collapsed: boolean }> = {};
    layout.panels.forEach(panel => {
      initialStates[panel.id] = {
        visible: panel.visible ?? true,
        collapsed: false,
      };
    });
    setPanelStates(initialStates);

    // Register panels with PanelRegistry
    layout.panels.forEach(panel => {
      panelRegistry.register({
        id: panel.id,
        pluginId: 'layout-manager',
        type: panel.type,
        position: panel.position,
        title: panel.title,
        priority: panel.priority || 100,
        props: panel.props,
      });
    });

    return () => {
      // Cleanup: unregister all panels
      layout.panels.forEach(panel => {
        panelRegistry.unregister(panel.id);
      });
    };
  }, []);

  // Handle layout change
  const handleLayoutChange = useCallback((newLayout: LayoutConfig) => {
    setCurrentLayout(newLayout);
    onLayoutChange?.(newLayout);

    // Update panel states for new panels
    setPanelStates(prev => {
      const next = { ...prev };
      newLayout.panels.forEach(panel => {
        if (!next[panel.id]) {
          next[panel.id] = {
            visible: panel.visible ?? true,
            collapsed: false,
          };
        }
      });
      return next;
    });
  }, [onLayoutChange]);

  // Switch to a preset layout
  const switchToPreset = useCallback((presetName: string) => {
    const preset = getLayoutPreset(presetName);
    if (preset) {
      handleLayoutChange(preset);
      setActiveCustomLayoutId(null);
    }
  }, [handleLayoutChange]);

  // Toggle panel visibility
  const handleTogglePanel = useCallback((panelId: string) => {
    setPanelStates(prev => ({
      ...prev,
      [panelId]: {
        ...prev[panelId],
        visible: !prev[panelId]?.visible,
      },
    }));
  }, []);

  // Toggle panel collapse
  const handleCollapsePanel = useCallback((panelId: string) => {
    setPanelStates(prev => ({
      ...prev,
      [panelId]: {
        ...prev[panelId],
        collapsed: !prev[panelId]?.collapsed,
      },
    }));
  }, []);

  // Handle panel action
  const handlePanelAction = useCallback((panelId: string, actionId: string, context: unknown) => {
    onPanelAction?.(panelId, actionId, context);
  }, [onPanelAction]);

  // Get panels by position
  const panelsByPosition = useMemo(() => {
    if (!currentLayout) return {} as Record<PanelPosition, PanelConfig[]>;

    const positions: PanelPosition[] = ['top', 'bottom', 'left', 'right', 'center', 'overlay'];
    const result: Partial<Record<PanelPosition, PanelConfig[]>> = {};

    positions.forEach(position => {
      result[position] = currentLayout.panels
        .filter(panel => panel.position === position)
        .filter(panel => panelStates[panel.id]?.visible !== false)
        .sort((a, b) => (a.priority || 100) - (b.priority || 100));
    });

    return result as Record<PanelPosition, PanelConfig[]>;
  }, [currentLayout, panelStates]);

  // If no layout, render nothing
  if (!currentLayout) {
    return (
      <div className={`layout-manager layout-manager--loading ${className}`}>
        <div className="layout-manager__spinner" />
        <span>Loading layout...</span>
      </div>
    );
  }

  return (
    <div className={`layout-manager ${className}`} data-layout={currentLayout.name}>
      {/* Top Panels */}
      {panelsByPosition.top && panelsByPosition.top.length > 0 && (
        <div className="layout-manager__top">
          {panelsByPosition.top.map(panel => (
            <DynamicPanel
              key={panel.id}
              config={{
                ...panel,
                visible: panelStates[panel.id]?.visible,
              }}
              stations={stations}
              runtimeData={runtimeData}
              onAction={(actionId, context) => handlePanelAction(panel.id, actionId, context)}
            />
          ))}
        </div>
      )}

      {/* Main Content Area */}
      <div className="layout-manager__main">
        {/* Left Panels */}
        {panelsByPosition.left && panelsByPosition.left.length > 0 && (
          <div className="layout-manager__left">
            {panelsByPosition.left.map(panel => (
              <DynamicPanel
                key={panel.id}
                config={{
                  ...panel,
                  visible: panelStates[panel.id]?.visible,
                }}
                stations={stations}
                runtimeData={runtimeData}
                onAction={(actionId, context) => handlePanelAction(panel.id, actionId, context)}
              />
            ))}
          </div>
        )}

        {/* Center Panels */}
        {panelsByPosition.center && panelsByPosition.center.length > 0 && (
          <div className="layout-manager__center">
            {panelsByPosition.center.map(panel => (
              <DynamicPanel
                key={panel.id}
                config={{
                  ...panel,
                  visible: panelStates[panel.id]?.visible,
                }}
                stations={stations}
                runtimeData={runtimeData}
                onAction={(actionId, context) => handlePanelAction(panel.id, actionId, context)}
              />
            ))}
          </div>
        )}

        {/* Right Panels */}
        {panelsByPosition.right && panelsByPosition.right.length > 0 && (
          <div className="layout-manager__right">
            {panelsByPosition.right.map(panel => (
              <DynamicPanel
                key={panel.id}
                config={{
                  ...panel,
                  visible: panelStates[panel.id]?.visible,
                }}
                stations={stations}
                runtimeData={runtimeData}
                onAction={(actionId, context) => handlePanelAction(panel.id, actionId, context)}
              />
            ))}
          </div>
        )}
      </div>

      {/* Bottom Panels */}
      {panelsByPosition.bottom && panelsByPosition.bottom.length > 0 && (
        <div className="layout-manager__bottom">
          {panelsByPosition.bottom.map(panel => (
            <DynamicPanel
              key={panel.id}
              config={{
                ...panel,
                visible: panelStates[panel.id]?.visible,
              }}
              stations={stations}
              runtimeData={runtimeData}
              onAction={(actionId, context) => handlePanelAction(panel.id, actionId, context)}
            />
          ))}
        </div>
      )}

      {/* Alert Overlay */}
      <AlertOverlay
        config={currentLayout.alerts || DEFAULT_ALERT_CONFIG}
        alerts={alerts}
        onAcknowledge={acknowledgeAlert}
        onDismiss={removeAlert}
        onLoadMore={() => {}}
        visible={true}
      />
    </div>
  );
};

// ============================================
// Layout Validator
// ============================================

function validateLayout(layout: LayoutConfig): LayoutValidationResult {
  const errors: LayoutValidationResult['errors'] = [];
  const warnings: LayoutValidationResult['warnings'] = [];

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

// ============================================
// Layout Preset Selector Component
// ============================================

interface LayoutPresetSelectorProps {
  currentPreset: string | null;
  onSelectPreset: (presetName: string) => void;
  className?: string;
}

export const LayoutPresetSelector: React.FC<LayoutPresetSelectorProps> = ({
  currentPreset,
  onSelectPreset,
  className = '',
}) => {
  const presets = getAllLayoutPresets();

  return (
    <div className={`layout-preset-selector ${className}`}>
      <label className="layout-preset-selector__label">Layout:</label>
      <select
        className="layout-preset-selector__select"
        value={currentPreset || ''}
        onChange={(e) => onSelectPreset(e.target.value)}
      >
        {presets.map(preset => (
          <option key={preset.id} value={preset.id}>
            {preset.name}
          </option>
        ))}
      </select>
    </div>
  );
};

export default LayoutManager;
