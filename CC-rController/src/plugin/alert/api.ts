/**
 * Alert Rules API - Tauri Backend Communication
 * Part of Phase 9: Alert Rules Management System
 */

import { invoke } from '@tauri-apps/api/core';
import type {
  AlertRule,
  AlertHistory,
  AlertRuleFilter,
  AlertHistoryFilter,
  AlertRulePackage,
  AlertRuleImportResult,
  AlertRuleValidationResult,
  AlertStats,
  AlertExecutionContext,
  AlertRulePackageMetadata,
} from './types';

// ============================================
// API Response Types
// ============================================

interface LoadAlertRulesResult {
  rules: AlertRule[];
}

interface SaveAlertRuleResult {
  rule: AlertRule;
}

interface DeleteAlertRuleResult {
  success: boolean;
}

interface ExecuteAlertRuleResult {
  execution: AlertExecutionResult;
}

interface AlertExecutionResult {
  ruleId: string;
  executionId: string;
  triggered: boolean;
  triggeredTargets: string[];
  timestamp: string;
  results: AlertTargetResult[];
}

interface AlertTargetResult {
  targetId: string;
  targetName: string;
  triggered: boolean;
  currentValue: number;
  threshold: number;
  message?: string;
}

interface GetAlertHistoryResult {
  history: AlertHistory[];
  total: number;
}

interface GetAlertStatsResult {
  stats: AlertStats;
}

interface GetAlertTemplatesResult {
  templates: AlertRule[];
}

interface PreviewTargetsResult {
  targets: Array<{
    id: string;
    name: string;
    status?: string;
  }>;
  total: number;
}

// ============================================
// Alert API
// ============================================

export const alertApi = {
  /**
   * Load all alert rules
   */
  async loadAlertRules(): Promise<AlertRule[]> {
    try {
      const result = await invoke<LoadAlertRulesResult>('load_alert_rules');
      return result.rules;
    } catch (error) {
      console.error('[AlertApi] Failed to load alert rules:', error);
      throw error;
    }
  },

  /**
   * Save an alert rule (create or update)
   */
  async saveAlertRule(rule: Partial<AlertRule>): Promise<AlertRule> {
    try {
      const result = await invoke<SaveAlertRuleResult>('save_alert_rule', { rule });
      return result.rule;
    } catch (error) {
      console.error('[AlertApi] Failed to save alert rule:', error);
      throw error;
    }
  },

  /**
   * Delete an alert rule
   */
  async deleteAlertRule(ruleId: string): Promise<void> {
    try {
      await invoke<DeleteAlertRuleResult>('delete_alert_rule', { rule_id: ruleId });
    } catch (error) {
      console.error('[AlertApi] Failed to delete alert rule:', error);
      throw error;
    }
  },

  /**
   * Toggle alert rule enabled/disabled status
   */
  async toggleAlertRule(ruleId: string): Promise<void> {
    try {
      await invoke('toggle_alert_rule', { rule_id: ruleId });
    } catch (error) {
      console.error('[AlertApi] Failed to toggle alert rule:', error);
      throw error;
    }
  },

  /**
   * Duplicate an alert rule
   */
  async duplicateAlertRule(ruleId: string): Promise<AlertRule> {
    try {
      const result = await invoke<SaveAlertRuleResult>('duplicate_alert_rule', {
        rule_id: ruleId,
      });
      return result.rule;
    } catch (error) {
      console.error('[AlertApi] Failed to duplicate alert rule:', error);
      throw error;
    }
  },

  /**
   * Execute/test an alert rule on specified targets
   */
  async executeAlertRule(context: AlertExecutionContext): Promise<AlertExecutionResult> {
    try {
      const result = await invoke<ExecuteAlertRuleResult>('execute_alert_rule', {
        context,
      });
      return result.execution;
    } catch (error) {
      console.error('[AlertApi] Failed to execute alert rule:', error);
      throw error;
    }
  },

  /**
   * Get alert history
   */
  async getAlertHistory(
    filters?: AlertHistoryFilter,
    limit?: number
  ): Promise<{ history: AlertHistory[]; total: number }> {
    try {
      const result = await invoke<GetAlertHistoryResult>('get_alert_history', {
        filters,
        limit,
      });
      return { history: result.history, total: result.total };
    } catch (error) {
      console.error('[AlertApi] Failed to get alert history:', error);
      throw error;
    }
  },

  /**
   * Get alert statistics
   */
  async getAlertStats(): Promise<AlertStats> {
    try {
      const result = await invoke<GetAlertStatsResult>('get_alert_stats');
      return result.stats;
    } catch (error) {
      console.error('[AlertApi] Failed to get alert stats:', error);
      throw error;
    }
  },

  /**
   * Validate alert rule configuration without saving
   */
  async validateAlertRule(rule: Partial<AlertRule>): Promise<AlertRuleValidationResult> {
    try {
      const result = await invoke<AlertRuleValidationResult>('validate_alert_rule', {
        rule,
      });
      return result;
    } catch (error) {
      console.error('[AlertApi] Failed to validate alert rule:', error);
      throw error;
    }
  },

  /**
   * Preview target selection without executing
   */
  async previewTargets(
    selector: AlertRule['targetSelector']
  ): Promise<Array<{ id: string; name: string; status?: string }>> {
    try {
      const result = await invoke<PreviewTargetsResult>('preview_alert_targets', {
        selector,
      });
      return result.targets;
    } catch (error) {
      console.error('[AlertApi] Failed to preview targets:', error);
      throw error;
    }
  },

  /**
   * Import alert rules package
   */
  async importAlertRules(
    pkg: AlertRulePackage,
    options?: { overwrite?: boolean; importAsCopies?: boolean }
  ): Promise<AlertRuleImportResult> {
    try {
      const result = await invoke<AlertRuleImportResult>('import_alert_rules', {
        pkg,
        options,
      });
      return result;
    } catch (error) {
      console.error('[AlertApi] Failed to import alert rules:', error);
      throw error;
    }
  },

  /**
   * Export alert rules as package
   */
  async exportAlertRules(
    ruleIds: string[],
    metadata?: Partial<AlertRulePackageMetadata>
  ): Promise<AlertRulePackage> {
    try {
      const result = await invoke<{ package: AlertRulePackage }>('export_alert_rules', {
        rule_ids: ruleIds,
        metadata,
      });
      return result.package;
    } catch (error) {
      console.error('[AlertApi] Failed to export alert rules:', error);
      throw error;
    }
  },

  /**
   * Acknowledge an alert
   */
  async acknowledgeAlert(historyId: string, note?: string): Promise<void> {
    try {
      await invoke('acknowledge_alert', { history_id: historyId, note });
    } catch (error) {
      console.error('[AlertApi] Failed to acknowledge alert:', error);
      throw error;
    }
  },

  /**
   * Resolve an alert manually
   */
  async resolveAlert(historyId: string): Promise<void> {
    try {
      await invoke('resolve_alert', { history_id: historyId });
    } catch (error) {
      console.error('[AlertApi] Failed to resolve alert:', error);
      throw error;
    }
  },

  /**
   * Get available alert templates
   */
  async getAlertTemplates(): Promise<AlertRule[]> {
    try {
      const result = await invoke<GetAlertTemplatesResult>('get_alert_templates');
      return result.templates;
    } catch (error) {
      console.error('[AlertApi] Failed to get alert templates:', error);
      throw error;
    }
  },

  /**
   * Get available targets for alert rules
   */
  async getAvailableTargets(): Promise<Array<{ id: string; name: string; status?: string }>> {
    try {
      const result = await invoke<PreviewTargetsResult>('get_alert_available_targets');
      return result.targets;
    } catch (error) {
      console.error('[AlertApi] Failed to get available targets:', error);
      throw error;
    }
  },
};

export default alertApi;
