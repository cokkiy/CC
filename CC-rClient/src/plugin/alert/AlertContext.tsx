/**
 * AlertContext - React Context for Alert Rules Management
 * Part of Phase 9: Alert Rules Management System
 */

import { invoke } from '@tauri-apps/api/core';
import React, { createContext, useCallback, useContext, useEffect, useMemo, useRef, useState } from 'react';

import type { Station, StationRuntimeSnapshot } from '../../types';
import { alertApi } from './api';
import type {
  AlertExecutionContext,
  AlertExecutionResult,
  AlertHistory,
  AlertHistoryFilter,
  AlertRule,
  AlertRuleFilter,
  AlertRuleImportResult,
  AlertRulePackage,
  AlertStats,
  AlertTargetResult,
  AlertTriggerStatus,
} from './types';

const ALERT_RUNTIME_EVENT = 'cc-alert-runtime';

type AlertTarget = {
  id: string;
  name: string;
  status?: string;
};

type AlertRuntimeEventDetail = {
  station: Station;
  runtime: StationRuntimeSnapshot;
};

type EvaluatedCondition = {
  triggered: boolean;
  currentValue: number;
  threshold: number;
  message: string;
};

interface AlertContextState {
  rules: AlertRule[];
  selectedRule: AlertRule | null;
  history: AlertHistory[];
  stats: AlertStats | null;
  isLoading: boolean;
  error: string | null;
  targets: AlertTarget[];
}

interface AlertContextValue extends AlertContextState {
  loadRules: () => Promise<void>;
  saveRule: (rule: Partial<AlertRule>) => Promise<AlertRule>;
  deleteRule: (ruleId: string) => Promise<void>;
  duplicateRule: (ruleId: string) => Promise<AlertRule>;
  filterRules: (filter: AlertRuleFilter) => AlertRule[];
  setSelectedRule: (rule: AlertRule | null) => void;
  toggleFavorite: (ruleId: string) => Promise<void>;
  toggleRuleStatus: (ruleId: string) => Promise<void>;
  executeRule: (context: AlertExecutionContext) => Promise<AlertExecutionResult>;
  testRule: (ruleId: string, targetIds: string[]) => Promise<void>;
  loadHistory: (filters?: AlertHistoryFilter, limit?: number) => Promise<{ history: AlertHistory[]; total: number }>;
  acknowledgeAlert: (historyId: string, note?: string) => Promise<void>;
  resolveAlert: (historyId: string) => Promise<void>;
  loadStats: () => Promise<void>;
  loadTargets: () => Promise<void>;
  previewTargets: (selector: AlertRule['targetSelector']) => Promise<AlertTarget[]>;
  importRules: (pkg: AlertRulePackage, options?: object) => Promise<AlertRuleImportResult>;
  exportRules: (ruleIds: string[], metadata?: object) => Promise<AlertRulePackage>;
  validateRule: (rule: Partial<AlertRule>) => Promise<{ valid: boolean; errors: string[] }>;
  getStats: () => { totalRules: number; enabledRules: number; triggeredToday: number };
}

const AlertContext = createContext<AlertContextValue | null>(null);

function isRuleEnabled(rule: AlertRule) {
  return rule.status === 'enabled';
}

function ruleMatchesTarget(rule: AlertRule, station: Station) {
  const selector = rule.targetSelector;
  switch (selector.selectorType) {
    case 'all':
      return true;
    case 'group':
      return (selector.groups ?? []).some((groupId) => station.groups.includes(groupId));
    case 'tag':
      return (selector.tags ?? []).some((tagKey) => Object.prototype.hasOwnProperty.call(station.tags, tagKey));
    case 'device_ids':
      return (selector.deviceIds ?? []).includes(station.id);
    case 'filter': {
      const filterValue = selector.filterExpr?.trim().toLowerCase() ?? '';
      if (!filterValue) {
        return false;
      }
      return (
        station.id.toLowerCase().includes(filterValue)
        || station.name.toLowerCase().includes(filterValue)
        || station.groups.some((group) => group.toLowerCase().includes(filterValue))
      );
    }
    default:
      return false;
  }
}

function getMetricValue(runtime: StationRuntimeSnapshot, metricType: AlertRule['conditionGroup']['conditions'][number]['metricType']) {
  switch (metricType) {
    case 'cpu_usage':
    case 'cpu_load':
      return runtime.cpu;
    case 'memory_usage':
      return runtime.totalMemory > 0 ? (runtime.currentMemory / runtime.totalMemory) * 100 : 0;
    case 'memory_available':
      return Math.max(runtime.totalMemory - runtime.currentMemory, 0) / (1024 * 1024);
    case 'network_rx':
      return runtime.networkStats.reduce((sum, item) => sum + item.bytesReceivedPerSec, 0);
    case 'network_tx':
      return runtime.networkStats.reduce((sum, item) => sum + item.bytesSentedPerSec, 0);
    case 'process_count':
      return runtime.procCount;
    case 'disk_usage':
    case 'disk_free':
    case 'custom':
    default:
      return NaN;
  }
}

function compareValue(
  operator: AlertRule['conditionGroup']['conditions'][number]['operator'],
  currentValue: number,
  threshold: number,
  thresholdHigh?: number,
) {
  switch (operator) {
    case 'gt':
      return currentValue > threshold;
    case 'gte':
      return currentValue >= threshold;
    case 'lt':
      return currentValue < threshold;
    case 'lte':
      return currentValue <= threshold;
    case 'eq':
      return currentValue === threshold;
    case 'neq':
      return currentValue !== threshold;
    case 'between':
      return thresholdHigh !== undefined && currentValue > threshold && currentValue < thresholdHigh;
    default:
      return false;
  }
}

function evaluateRule(rule: AlertRule, runtime: StationRuntimeSnapshot): { triggered: boolean; result: AlertTargetResult; summary: string } {
  const conditions = rule.conditionGroup.conditions;
  if (conditions.length === 0) {
    return {
      triggered: false,
      result: {
        targetId: runtime.stationId,
        targetName: runtime.computerName || runtime.stationId,
        triggered: false,
        currentValue: 0,
        threshold: 0,
        message: 'No conditions configured',
      },
      summary: 'No conditions configured',
    };
  }

  const evaluations = conditions.map((condition): EvaluatedCondition => {
    const currentValue = getMetricValue(runtime, condition.metricType);
    if (Number.isNaN(currentValue)) {
      return {
        triggered: false,
        currentValue: 0,
        threshold: condition.threshold,
        message: `${condition.metricType} is not available in the current telemetry payload`,
      };
    }

    const triggered = compareValue(condition.operator, currentValue, condition.threshold, condition.thresholdHigh);
    return {
      triggered,
      currentValue,
      threshold: condition.threshold,
      message: `${condition.metricType} ${condition.operator} ${condition.threshold}`,
    };
  });

  const triggered = rule.conditionGroup.logic === 'or'
    ? evaluations.some((evaluation) => evaluation.triggered)
    : evaluations.every((evaluation) => evaluation.triggered);
  const primary = evaluations.find((evaluation) => evaluation.triggered) ?? evaluations[0];

  return {
    triggered,
    result: {
      targetId: runtime.stationId,
      targetName: runtime.computerName || runtime.stationId,
      triggered,
      currentValue: Number.isFinite(primary.currentValue) ? Number(primary.currentValue.toFixed(2)) : 0,
      threshold: primary.threshold,
      message: primary.message,
    },
    summary: primary.message,
  };
}

export function AlertProvider({ children }: { children: React.ReactNode }) {
  const [state, setState] = useState<AlertContextState>({
    rules: [],
    selectedRule: null,
    history: [],
    stats: null,
    isLoading: false,
    error: null,
    targets: [],
  });
  const rulesRef = useRef<AlertRule[]>([]);
  const historyRef = useRef<AlertHistory[]>([]);
  const runtimeRef = useRef<Record<string, StationRuntimeSnapshot>>({});
  const stationRef = useRef<Record<string, Station>>({});

  const updateRulesState = useCallback((nextRules: AlertRule[]) => {
    rulesRef.current = nextRules;
    setState((prev) => ({ ...prev, rules: nextRules }));
  }, []);

  const updateHistoryState = useCallback((nextHistory: AlertHistory[]) => {
    historyRef.current = nextHistory;
    setState((prev) => ({ ...prev, history: nextHistory }));
  }, []);

  const loadRules = useCallback(async () => {
    try {
      setState((prev) => ({ ...prev, isLoading: true, error: null }));
      const rules = await alertApi.loadAlertRules();
      updateRulesState(rules);
      setState((prev) => ({ ...prev, isLoading: false }));
    } catch (error) {
      setState((prev) => ({
        ...prev,
        isLoading: false,
        error: error instanceof Error ? error.message : 'Failed to load alert rules',
      }));
    }
  }, [updateRulesState]);

  const loadHistory = useCallback(async (filters?: AlertHistoryFilter, limit?: number) => {
    const result = await alertApi.getAlertHistory(filters, limit);
    updateHistoryState(result.history);
    return result;
  }, [updateHistoryState]);

  const loadStats = useCallback(async () => {
    try {
      const stats = await alertApi.getAlertStats();
      setState((prev) => ({ ...prev, stats }));
    } catch (error) {
      console.error('[AlertContext] Failed to load stats:', error);
    }
  }, []);

  const loadTargets = useCallback(async () => {
    try {
      const targets = await alertApi.getAvailableTargets();
      setState((prev) => ({ ...prev, targets }));
    } catch (error) {
      console.error('[AlertContext] Failed to load targets:', error);
    }
  }, []);

  useEffect(() => {
    void loadRules();
    void loadHistory(undefined, 100);
    void loadTargets();
    void loadStats();
  }, [loadHistory, loadRules, loadStats, loadTargets]);

  const persistTriggeredAlert = useCallback(async (
    rule: AlertRule,
    station: Station,
    runtime: StationRuntimeSnapshot,
    evaluation: ReturnType<typeof evaluateRule>,
    dryRun: boolean,
  ) => {
    const existingRule = rulesRef.current.find((currentRule) => currentRule.id === rule.id) ?? rule;
    const lastTriggeredAt = existingRule.lastTriggeredAt ? new Date(existingRule.lastTriggeredAt).getTime() : 0;
    const now = Date.now();
    if (!dryRun && lastTriggeredAt > 0 && now - lastTriggeredAt < existingRule.cooldownSecs * 1000) {
      return null;
    }

    const recentTriggers = historyRef.current.filter((entry) => {
      if (entry.ruleId !== existingRule.id) {
        return false;
      }
      const entryTime = new Date(entry.triggeredAt).getTime();
      return now - entryTime <= 60 * 60 * 1000;
    }).length;
    if (!dryRun && existingRule.maxTriggersPerHour && recentTriggers >= existingRule.maxTriggersPerHour) {
      return null;
    }

    const triggeredAt = new Date(now).toISOString();
    const historyEntry: AlertHistory = {
      id: crypto.randomUUID(),
      ruleId: existingRule.id,
      ruleName: existingRule.name,
      targetId: station.id,
      targetName: station.name,
      severity: existingRule.severity,
      status: 'triggered',
      triggerValue: evaluation.result.currentValue,
      threshold: evaluation.result.threshold,
      condition: evaluation.summary,
      triggeredAt,
      actionsTriggered: existingRule.actions.filter((action) => action.enabled).map((action) => action.id),
      actionResults: existingRule.actions
        .filter((action) => action.enabled)
        .map((action) => ({
          actionId: action.id,
          actionType: action.actionType,
          status: action.actionType === 'notification' || action.actionType === 'auto_remediate' ? 'success' : 'skipped',
          message: action.config.message || evaluation.result.message,
          executedAt: triggeredAt,
        })),
      metadata: {
        stationId: station.id,
        stationName: station.name,
        runtimeSource: runtime.endpoint,
      },
    };

    if (!dryRun) {
      await invoke('append_alert_history', {
        entry: historyEntry,
        ruleId: existingRule.id,
        rule_id: existingRule.id,
      });
      updateHistoryState([historyEntry, ...historyRef.current].slice(0, 500));
      updateRulesState(
        rulesRef.current.map((currentRule) =>
          currentRule.id === existingRule.id
            ? {
                ...currentRule,
                lastTriggeredAt: triggeredAt,
                usageCount: currentRule.usageCount + 1,
                updatedAt: triggeredAt,
              }
            : currentRule,
        ),
      );
      void loadStats();
    }

    return historyEntry;
  }, [loadStats, updateHistoryState, updateRulesState]);

  const executeRule = useCallback(async (context: AlertExecutionContext): Promise<AlertExecutionResult> => {
    const rule = rulesRef.current.find((currentRule) => currentRule.id === context.ruleId);
    if (!rule) {
      throw new Error(`Alert rule ${context.ruleId} not found`);
    }

    const dryRun = context.dryRun ?? false;
    const results: AlertTargetResult[] = [];
    const triggeredTargets: string[] = [];

    for (const targetId of context.targetIds) {
      const station = stationRef.current[targetId];
      const runtime = runtimeRef.current[targetId];

      if (!station || !runtime) {
        results.push({
          targetId,
          targetName: station?.name ?? targetId,
          triggered: false,
          currentValue: 0,
          threshold: rule.conditionGroup.conditions[0]?.threshold ?? 0,
          message: 'No live telemetry available for this target',
        });
        continue;
      }

      const evaluation = evaluateRule(rule, runtime);
      const result = {
        ...evaluation.result,
        targetId,
        targetName: station.name,
      };
      results.push(result);

      if (evaluation.triggered) {
        triggeredTargets.push(targetId);
        await persistTriggeredAlert(rule, station, runtime, evaluation, dryRun);
      }
    }

    return {
      ruleId: rule.id,
      executionId: crypto.randomUUID(),
      triggered: triggeredTargets.length > 0,
      triggeredTargets,
      timestamp: new Date().toISOString(),
      results,
    };
  }, [persistTriggeredAlert]);

  const handleRuntimeEvent = useCallback(async (detail: AlertRuntimeEventDetail) => {
    stationRef.current[detail.station.id] = detail.station;
    runtimeRef.current[detail.station.id] = detail.runtime;

    const activeRules = rulesRef.current.filter((rule) => isRuleEnabled(rule) && ruleMatchesTarget(rule, detail.station));
    for (const rule of activeRules) {
      const evaluation = evaluateRule(rule, detail.runtime);
      if (!evaluation.triggered) {
        continue;
      }
      await persistTriggeredAlert(rule, detail.station, detail.runtime, evaluation, false);
    }
  }, [persistTriggeredAlert]);

  useEffect(() => {
    const listener = (event: Event) => {
      const customEvent = event as CustomEvent<AlertRuntimeEventDetail>;
      if (!customEvent.detail?.station || !customEvent.detail?.runtime) {
        return;
      }
      void handleRuntimeEvent(customEvent.detail);
    };
    window.addEventListener(ALERT_RUNTIME_EVENT, listener as EventListener);
    return () => {
      window.removeEventListener(ALERT_RUNTIME_EVENT, listener as EventListener);
    };
  }, [handleRuntimeEvent]);

  const saveRule = useCallback(async (ruleData: Partial<AlertRule>) => {
    const savedRule = await alertApi.saveAlertRule(ruleData);
    const existingIndex = rulesRef.current.findIndex((rule) => rule.id === savedRule.id);
    const nextRules = [...rulesRef.current];
    if (existingIndex >= 0) {
      nextRules[existingIndex] = savedRule;
    } else {
      nextRules.push(savedRule);
    }
    updateRulesState(nextRules);
    void loadStats();
    return savedRule;
  }, [loadStats, updateRulesState]);

  const deleteRule = useCallback(async (ruleId: string) => {
    await alertApi.deleteAlertRule(ruleId);
    updateRulesState(rulesRef.current.filter((rule) => rule.id !== ruleId));
  }, [updateRulesState]);

  const duplicateRule = useCallback(async (ruleId: string) => {
    const duplicated = await alertApi.duplicateAlertRule(ruleId);
    updateRulesState([...rulesRef.current, duplicated]);
    return duplicated;
  }, [updateRulesState]);

  const filterRules = useCallback((filter: AlertRuleFilter) => {
    return rulesRef.current.filter((rule) => {
      if (filter.search) {
        const search = filter.search.toLowerCase();
        if (
          !rule.name.toLowerCase().includes(search)
          && !rule.description?.toLowerCase().includes(search)
          && !rule.tags.some((tag) => tag.toLowerCase().includes(search))
        ) {
          return false;
        }
      }
      if (filter.severity?.length && !filter.severity.includes(rule.severity)) {
        return false;
      }
      if (filter.status?.length && !filter.status.includes(rule.status)) {
        return false;
      }
      if (filter.tags?.length && !filter.tags.some((tag) => rule.tags.includes(tag))) {
        return false;
      }
      return true;
    });
  }, []);

  const setSelectedRule = useCallback((rule: AlertRule | null) => {
    setState((prev) => ({ ...prev, selectedRule: rule }));
  }, []);

  const toggleFavorite = useCallback(async (ruleId: string) => {
    const rule = rulesRef.current.find((currentRule) => currentRule.id === ruleId);
    if (!rule) {
      throw new Error(`Alert rule ${ruleId} not found`);
    }
    await saveRule({ ...rule, isFavorite: !rule.isFavorite });
  }, [saveRule]);

  const toggleRuleStatus = useCallback(async (ruleId: string) => {
    await alertApi.toggleAlertRule(ruleId);
    updateRulesState(
      rulesRef.current.map((rule) =>
        rule.id === ruleId
          ? { ...rule, status: rule.status === 'enabled' ? 'disabled' : 'enabled' }
          : rule,
      ),
    );
    void loadStats();
  }, [loadStats, updateRulesState]);

  const testRule = useCallback(async (ruleId: string, targetIds: string[]) => {
    await executeRule({
      ruleId,
      targetIds,
      dryRun: true,
    });
  }, [executeRule]);

  const acknowledgeAlert = useCallback(async (historyId: string, note?: string) => {
    await alertApi.acknowledgeAlert(historyId, note);
    updateHistoryState(
      historyRef.current.map((entry) =>
        entry.id === historyId
          ? {
              ...entry,
              status: 'acknowledged' as AlertTriggerStatus,
              acknowledgedAt: new Date().toISOString(),
              acknowledgedBy: note || 'current-user',
            }
          : entry,
      ),
    );
    void loadStats();
  }, [loadStats, updateHistoryState]);

  const resolveAlert = useCallback(async (historyId: string) => {
    await alertApi.resolveAlert(historyId);
    updateHistoryState(
      historyRef.current.map((entry) =>
        entry.id === historyId
          ? {
              ...entry,
              status: 'resolved' as AlertTriggerStatus,
              resolvedAt: new Date().toISOString(),
            }
          : entry,
      ),
    );
    void loadStats();
  }, [loadStats, updateHistoryState]);

  const previewTargets = useCallback(async (selector: AlertRule['targetSelector']) => {
    return alertApi.previewTargets(selector);
  }, []);

  const importRules = useCallback(async (pkg: AlertRulePackage, options?: object) => {
    const result = await alertApi.importAlertRules(pkg, options);
    await loadRules();
    return result;
  }, [loadRules]);

  const exportRules = useCallback(async (ruleIds: string[], metadata?: object) => {
    return alertApi.exportAlertRules(ruleIds, metadata);
  }, []);

  const validateRule = useCallback(async (rule: Partial<AlertRule>) => {
    return alertApi.validateAlertRule(rule);
  }, []);

  const getStats = useCallback(() => {
    const today = new Date().toDateString();
    return {
      totalRules: rulesRef.current.length,
      enabledRules: rulesRef.current.filter((rule) => rule.status === 'enabled').length,
      triggeredToday: historyRef.current.filter((entry) => new Date(entry.triggeredAt).toDateString() === today).length,
    };
  }, []);

  const value = useMemo<AlertContextValue>(() => ({
    ...state,
    loadRules,
    saveRule,
    deleteRule,
    duplicateRule,
    filterRules,
    setSelectedRule,
    toggleFavorite,
    toggleRuleStatus,
    executeRule,
    testRule,
    loadHistory,
    acknowledgeAlert,
    resolveAlert,
    loadStats,
    loadTargets,
    previewTargets,
    importRules,
    exportRules,
    validateRule,
    getStats,
  }), [
    acknowledgeAlert,
    deleteRule,
    duplicateRule,
    executeRule,
    exportRules,
    filterRules,
    getStats,
    importRules,
    loadHistory,
    loadRules,
    loadStats,
    loadTargets,
    previewTargets,
    resolveAlert,
    saveRule,
    setSelectedRule,
    state,
    testRule,
    toggleFavorite,
    toggleRuleStatus,
    validateRule,
  ]);

  return <AlertContext.Provider value={value}>{children}</AlertContext.Provider>;
}

export function useAlerts() {
  const ctx = useContext(AlertContext);
  if (!ctx) {
    throw new Error('useAlerts must be used within an AlertProvider');
  }
  return ctx;
}

export function useAlertsSafe() {
  return useContext(AlertContext);
}
