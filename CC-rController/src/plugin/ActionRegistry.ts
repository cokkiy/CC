/**
 * ActionRegistry - Control action registration and management
 * Manages dynamic actions registered by plugins for station control
 */

import type {
  ActionDefinition,
  ActionContext,
  ActionResult,
  ActionScope,
  ActionTarget,
} from './types';

// Action category for grouping
export type ActionCategory = 
  | 'monitoring'    // Monitoring/observation actions
  | 'control'       // Control/power actions
  | 'file'          // File operations
  | 'network'       // Network operations
  | 'system'        // System operations
  | 'custom';       // Plugin-specific actions

export interface ActionGroup {
  id: string;
  name: string;
  category: ActionCategory;
  actionIds: string[];
}

// Event types
export interface ActionEvent {
  type: 'action:registered' | 'action:unregistered' | 'action:executed' | 'action:enabled_changed';
  actionId: string;
  pluginId: string;
  timestamp: number;
  data?: unknown;
}

type ActionEventHandler = (event: ActionEvent) => void;

export class ActionRegistry {
  private actions: Map<string, ActionDefinition> = new Map();
  private groups: Map<string, ActionGroup> = new Map();
  private eventListeners: Map<string, Set<ActionEventHandler>> = new Map();

  // Singleton
  private static instance: ActionRegistry | null = null;

  public static getInstance(): ActionRegistry {
    if (!ActionRegistry.instance) {
      ActionRegistry.instance = new ActionRegistry();
    }
    return ActionRegistry.instance;
  }

  constructor() {
    // Initialize with default categories
    this.initDefaultGroups();
  }

  private initDefaultGroups(): void {
    const categories: ActionCategory[] = ['monitoring', 'control', 'file', 'network', 'system', 'custom'];
    
    categories.forEach(category => {
      this.groups.set(category, {
        id: category,
        name: this.getCategoryName(category),
        category,
        actionIds: [],
      });
    });
  }

  private getCategoryName(category: ActionCategory): string {
    const names: Record<ActionCategory, string> = {
      monitoring: 'Monitoring',
      control: 'Control',
      file: 'File Operations',
      network: 'Network',
      system: 'System',
      custom: 'Custom',
    };
    return names[category];
  }

  /**
   * Register a new action
   */
  register(definition: ActionDefinition): boolean {
    const { id, pluginId } = definition;

    if (this.actions.has(id)) {
      console.warn(`[ActionRegistry] Action ${id} is already registered`);
      return false;
    }

    this.actions.set(id, { ...definition, enabled: definition.enabled ?? true });

    console.log(`[ActionRegistry] Registered action: ${id} for plugin ${pluginId}`);
    this.emit({
      type: 'action:registered',
      actionId: id,
      pluginId,
      timestamp: Date.now(),
    });

    return true;
  }

  /**
   * Unregister an action
   */
  unregister(actionId: string): boolean {
    const action = this.actions.get(actionId);
    if (!action) {
      console.warn(`[ActionRegistry] Action ${actionId} not found`);
      return false;
    }

    this.actions.delete(actionId);

    // Remove from groups
    this.groups.forEach(group => {
      group.actionIds = group.actionIds.filter(id => id !== actionId);
    });

    console.log(`[ActionRegistry] Unregistered action: ${actionId}`);
    this.emit({
      type: 'action:unregistered',
      actionId,
      pluginId: action.pluginId,
      timestamp: Date.now(),
    });

    return true;
  }

  /**
   * Unregister all actions for a plugin
   */
  unregisterByPlugin(pluginId: string): string[] {
    const removed: string[] = [];
    
    this.actions.forEach((action, id) => {
      if (action.pluginId === pluginId) {
        this.actions.delete(id);
        removed.push(id);
      }
    });

    // Remove from groups
    this.groups.forEach(group => {
      group.actionIds = group.actionIds.filter(id => !removed.includes(id));
    });

    removed.forEach(id => {
      this.emit({
        type: 'action:unregistered',
        actionId: id,
        pluginId,
        timestamp: Date.now(),
      });
    });

    console.log(`[ActionRegistry] Unregistered ${removed.length} actions for plugin ${pluginId}`);
    return removed;
  }

  /**
   * Get an action by ID
   */
  getAction(id: string): ActionDefinition | null {
    return this.actions.get(id) ?? null;
  }

  /**
   * Get all actions
   */
  getAllActions(): ActionDefinition[] {
    return Array.from(this.actions.values());
  }

  /**
   * Get actions by plugin
   */
  getActionsByPlugin(pluginId: string): ActionDefinition[] {
    return Array.from(this.actions.values())
      .filter(a => a.pluginId === pluginId);
  }

  /**
   * Get actions by scope
   */
  getActionsByScope(scope: ActionScope): ActionDefinition[] {
    return Array.from(this.actions.values())
      .filter(a => a.scope === scope);
  }

  /**
   * Get actions by target
   */
  getActionsByTarget(target: ActionTarget): ActionDefinition[] {
    return Array.from(this.actions.values())
      .filter(a => a.target === target);
  }

  /**
   * Get enabled actions
   */
  getEnabledActions(): ActionDefinition[] {
    return Array.from(this.actions.values())
      .filter(a => a.enabled);
  }

  /**
   * Get actions for a specific station context
   */
  getActionsForStation(stationId: string, groupId?: string): ActionDefinition[] {
    return Array.from(this.actions.values()).filter(action => {
      if (!action.enabled) return false;
      
      switch (action.scope) {
        case 'global':
          return true;
        case 'station':
          return true; // All stations can use station-scoped actions
        case 'group':
          return !!groupId; // Only available when a group is selected
        default:
          return false;
      }
    });
  }

  /**
   * Execute an action
   */
  async execute(actionId: string, context: ActionContext): Promise<ActionResult> {
    const action = this.actions.get(actionId);
    if (!action) {
      return {
        success: false,
        message: `Action ${actionId} not found`,
      };
    }

    if (!action.enabled) {
      return {
        success: false,
        message: `Action ${actionId} is disabled`,
      };
    }

    // Validate target count
    if (action.target === 'single' && context.stationIds.length !== 1) {
      return {
        success: false,
        message: `Action ${actionId} requires exactly one station`,
      };
    }

    if (action.target === 'multiple' && context.stationIds.length < 1) {
      return {
        success: false,
        message: `Action ${actionId} requires at least one station`,
      };
    }

    try {
      console.log(`[ActionRegistry] Executing action: ${actionId} for ${context.stationIds.length} stations`);
      const result = await action.handler(context);

      this.emit({
        type: 'action:executed',
        actionId,
        pluginId: action.pluginId,
        timestamp: Date.now(),
        data: { context, result },
      });

      return result;
    } catch (error) {
      const errorResult: ActionResult = {
        success: false,
        message: error instanceof Error ? error.message : String(error),
      };

      this.emit({
        type: 'action:executed',
        actionId,
        pluginId: action.pluginId,
        timestamp: Date.now(),
        data: { context, error: errorResult },
      });

      return errorResult;
    }
  }

  /**
   * Update action properties
   */
  updateAction(actionId: string, updates: Partial<ActionDefinition>): boolean {
    const action = this.actions.get(actionId);
    if (!action) {
      console.warn(`[ActionRegistry] Action ${actionId} not found`);
      return false;
    }

    const updated = { ...action, ...updates };
    this.actions.set(actionId, updated);

    return true;
  }

  /**
   * Enable/disable an action
   */
  setEnabled(actionId: string, enabled: boolean): void {
    const action = this.actions.get(actionId);
    if (action) {
      action.enabled = enabled !== false;
      
      this.emit({
        type: 'action:enabled_changed',
        actionId,
        pluginId: action.pluginId,
        timestamp: Date.now(),
        data: { enabled },
      });
    }
  }

  /**
   * Check if action is enabled
   */
  isEnabled(actionId: string): boolean {
    return this.actions.get(actionId)?.enabled ?? false;
  }

  /**
   * Create an action group
   */
  createGroup(group: ActionGroup): void {
    this.groups.set(group.id, group);
  }

  /**
   * Add action to group
   */
  addToGroup(actionId: string, groupId: string): void {
    const group = this.groups.get(groupId);
    const action = this.actions.get(actionId);
    
    if (group && action) {
      if (!group.actionIds.includes(actionId)) {
        group.actionIds.push(actionId);
      }
    }
  }

  /**
   * Get all groups
   */
  getAllGroups(): ActionGroup[] {
    return Array.from(this.groups.values());
  }

  /**
   * Get actions by group
   */
  getActionsByGroup(groupId: string): ActionDefinition[] {
    const group = this.groups.get(groupId);
    if (!group) return [];
    
    return group.actionIds
      .map(id => this.actions.get(id))
      .filter((a): a is ActionDefinition => a !== undefined);
  }

  /**
   * Get grouped actions for UI
   */
  getGroupedActions(): Array<{ group: ActionGroup; actions: ActionDefinition[] }> {
    return Array.from(this.groups.values())
      .filter(group => group.actionIds.length > 0)
      .map(group => ({
        group,
        actions: group.actionIds
          .map(id => this.actions.get(id))
          .filter((a): a is ActionDefinition => a !== undefined && a.enabled === true),
      }));
  }

  /**
   * Check if action exists
   */
  has(actionId: string): boolean {
    return this.actions.has(actionId);
  }

  /**
   * Get action count
   */
  get count(): number {
    return this.actions.size;
  }

  /**
   * Get enabled action count
   */
  get enabledCount(): number {
    return Array.from(this.actions.values()).filter(a => a.enabled).length;
  }

  // Event handling
  on(event: string, handler: ActionEventHandler): () => void {
    if (!this.eventListeners.has(event)) {
      this.eventListeners.set(event, new Set());
    }
    this.eventListeners.get(event)!.add(handler);
    
    return () => {
      this.eventListeners.get(event)?.delete(handler);
    };
  }

  off(event: string, handler: ActionEventHandler): void {
    this.eventListeners.get(event)?.delete(handler);
  }

  private emit(event: ActionEvent): void {
    this.eventListeners.get(event.type)?.forEach(handler => {
      try {
        handler(event);
      } catch (error) {
        console.error(`[ActionRegistry] Event handler error for ${event.type}:`, error);
      }
    });

    this.eventListeners.get('*')?.forEach(handler => {
      try {
        handler(event);
      } catch (error) {
        console.error(`[ActionRegistry] Wildcard event handler error:`, error);
      }
    });
  }
}

// Export singleton
export const actionRegistry = ActionRegistry.getInstance();
