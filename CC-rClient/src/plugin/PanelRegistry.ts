/**
 * PanelRegistry - UI Panel registration and management
 * Manages dynamic UI panels registered by plugins
 */

import type {
  PanelRegistration,
  PanelType,
  PanelPosition,
} from './types';

// Layout slot definitions
export interface LayoutSlot {
  position: PanelPosition;
  panels: PanelRegistration[];
}

export interface PanelRenderContext {
  stationId?: string;
  groupId?: string;
  userData?: Record<string, unknown>;
}

// Event types for panel updates
export interface PanelEvent {
  type: 'panel:registered' | 'panel:unregistered' | 'panel:updated' | 'panel:visibility_changed';
  panelId: string;
  pluginId: string;
  timestamp: number;
  data?: unknown;
}

type PanelEventHandler = (event: PanelEvent) => void;

export class PanelRegistry {
  private panels: Map<string, PanelRegistration> = new Map();
  private eventListeners: Map<string, Set<PanelEventHandler>> = new Map();
  private visiblePanels: Set<string> = new Set();

  // Singleton
  private static instance: PanelRegistry | null = null;

  public static getInstance(): PanelRegistry {
    if (!PanelRegistry.instance) {
      PanelRegistry.instance = new PanelRegistry();
    }
    return PanelRegistry.instance;
  }

  constructor() {
    // Initialize default visible panels
  }

  /**
   * Register a new panel
   */
  register(registration: PanelRegistration): boolean {
    const { id, pluginId } = registration;

    if (this.panels.has(id)) {
      console.warn(`[PanelRegistry] Panel ${id} is already registered`);
      return false;
    }

    this.panels.set(id, registration);
    this.visiblePanels.add(id);

    console.log(`[PanelRegistry] Registered panel: ${id} for plugin ${pluginId}`);
    this.emit({
      type: 'panel:registered',
      panelId: id,
      pluginId,
      timestamp: Date.now(),
    });

    return true;
  }

  /**
   * Unregister a panel
   */
  unregister(panelId: string): boolean {
    const panel = this.panels.get(panelId);
    if (!panel) {
      console.warn(`[PanelRegistry] Panel ${panelId} not found`);
      return false;
    }

    this.panels.delete(panelId);
    this.visiblePanels.delete(panelId);

    console.log(`[PanelRegistry] Unregistered panel: ${panelId}`);
    this.emit({
      type: 'panel:unregistered',
      panelId,
      pluginId: panel.pluginId,
      timestamp: Date.now(),
    });

    return true;
  }

  /**
   * Unregister all panels for a plugin
   */
  unregisterByPlugin(pluginId: string): string[] {
    const removed: string[] = [];
    
    this.panels.forEach((panel, id) => {
      if (panel.pluginId === pluginId) {
        this.panels.delete(id);
        this.visiblePanels.delete(id);
        removed.push(id);
      }
    });

    removed.forEach(id => {
      this.emit({
        type: 'panel:unregistered',
        panelId: id,
        pluginId,
        timestamp: Date.now(),
      });
    });

    console.log(`[PanelRegistry] Unregistered ${removed.length} panels for plugin ${pluginId}`);
    return removed;
  }

  /**
   * Get a panel by ID
   */
  getPanel(id: string): PanelRegistration | null {
    return this.panels.get(id) ?? null;
  }

  /**
   * Get all panels
   */
  getAllPanels(): PanelRegistration[] {
    return Array.from(this.panels.values());
  }

  /**
   * Get panels by position
   */
  getPanelsByPosition(position: PanelPosition): PanelRegistration[] {
    return Array.from(this.panels.values())
      .filter(p => p.position === position)
      .sort((a, b) => a.priority - b.priority);
  }

  /**
   * Get panels by plugin
   */
  getPanelsByPlugin(pluginId: string): PanelRegistration[] {
    return Array.from(this.panels.values())
      .filter(p => p.pluginId === pluginId);
  }

  /**
   * Get panels by type
   */
  getPanelsByType(type: PanelType): PanelRegistration[] {
    return Array.from(this.panels.values())
      .filter(p => p.type === type);
  }

  /**
   * Update panel properties
   */
  updatePanel(panelId: string, updates: Partial<PanelRegistration>): boolean {
    const panel = this.panels.get(panelId);
    if (!panel) {
      console.warn(`[PanelRegistry] Panel ${panelId} not found`);
      return false;
    }

    const updated = { ...panel, ...updates };
    this.panels.set(panelId, updated);

    this.emit({
      type: 'panel:updated',
      panelId,
      pluginId: panel.pluginId,
      timestamp: Date.now(),
      data: updates,
    });

    return true;
  }

  /**
   * Set panel visibility
   */
  setVisible(panelId: string, visible: boolean): void {
    if (visible) {
      this.visiblePanels.add(panelId);
    } else {
      this.visiblePanels.delete(panelId);
    }

    const panel = this.panels.get(panelId);
    if (panel) {
      this.emit({
        type: 'panel:visibility_changed',
        panelId,
        pluginId: panel.pluginId,
        timestamp: Date.now(),
        data: { visible },
      });
    }
  }

  /**
   * Check if panel is visible
   */
  isVisible(panelId: string): boolean {
    return this.visiblePanels.has(panelId);
  }

  /**
   * Get visible panels
   */
  getVisiblePanels(): PanelRegistration[] {
    return Array.from(this.panels.values())
      .filter(p => this.visiblePanels.has(p.id));
  }

  /**
   * Get visible panels by position
   */
  getVisiblePanelsByPosition(position: PanelPosition): PanelRegistration[] {
    return this.getPanelsByPosition(position)
      .filter(p => this.visiblePanels.has(p.id));
  }

  /**
   * Get layout structure for rendering
   */
  getLayout(): Record<PanelPosition, PanelRegistration[]> {
    const positions: PanelPosition[] = ['top', 'bottom', 'left', 'right', 'center', 'overlay'];
    const layout: Record<PanelPosition, PanelRegistration[]> = {
      top: [],
      bottom: [],
      left: [],
      right: [],
      center: [],
      overlay: [],
    };

    positions.forEach(pos => {
      layout[pos] = this.getVisiblePanelsByPosition(pos)
        .sort((a, b) => a.priority - b.priority);
    });

    return layout;
  }

  /**
   * Toggle panel visibility
   */
  toggle(panelId: string): void {
    this.setVisible(panelId, !this.isVisible(panelId));
  }

  /**
   * Hide all panels for a plugin
   */
  hideAllByPlugin(pluginId: string): void {
    this.getPanelsByPlugin(pluginId).forEach(panel => {
      this.visiblePanels.delete(panel.id);
      this.emit({
        type: 'panel:visibility_changed',
        panelId: panel.id,
        pluginId,
        timestamp: Date.now(),
        data: { visible: false },
      });
    });
  }

  /**
   * Show all panels for a plugin
   */
  showAllByPlugin(pluginId: string): void {
    this.getPanelsByPlugin(pluginId).forEach(panel => {
      this.visiblePanels.add(panel.id);
      this.emit({
        type: 'panel:visibility_changed',
        panelId: panel.id,
        pluginId,
        timestamp: Date.now(),
        data: { visible: true },
      });
    });
  }

  /**
   * Check if a panel exists
   */
  has(panelId: string): boolean {
    return this.panels.has(panelId);
  }

  /**
   * Get panel count
   */
  get count(): number {
    return this.panels.size;
  }

  /**
   * Get visible panel count
   */
  get visibleCount(): number {
    return this.visiblePanels.size;
  }

  // Event handling
  on(event: string, handler: PanelEventHandler): () => void {
    if (!this.eventListeners.has(event)) {
      this.eventListeners.set(event, new Set());
    }
    this.eventListeners.get(event)!.add(handler);
    
    return () => {
      this.eventListeners.get(event)?.delete(handler);
    };
  }

  off(event: string, handler: PanelEventHandler): void {
    this.eventListeners.get(event)?.delete(handler);
  }

  private emit(event: PanelEvent): void {
    this.eventListeners.get(event.type)?.forEach(handler => {
      try {
        handler(event);
      } catch (error) {
        console.error(`[PanelRegistry] Event handler error for ${event.type}:`, error);
      }
    });

    // Also emit to wildcard listeners
    this.eventListeners.get('*')?.forEach(handler => {
      try {
        handler(event);
      } catch (error) {
        console.error(`[PanelRegistry] Wildcard event handler error:`, error);
      }
    });
  }
}

// Export singleton
export const panelRegistry = PanelRegistry.getInstance();
