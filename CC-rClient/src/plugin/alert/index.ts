/**
 * Alert Rules Management System - Main Export
 * Part of Phase 9: Alert Rules Management System
 */

// Types
export * from './types';

// Components
export { AlertRuleEditor } from './AlertRuleEditor';
export type { AlertRuleEditorProps } from './AlertRuleEditor';

export { AlertRuleList } from './AlertRuleList';
export type { AlertRuleListProps } from './AlertRuleList';

export { AlertRuleRunner } from './AlertRuleRunner';
export type { AlertRuleRunnerProps } from './AlertRuleRunner';

export { AlertHistoryPanel } from './AlertHistoryPanel';
export type { AlertHistoryPanelProps } from './AlertHistoryPanel';

export { AlertRulesPage } from './AlertRulesPage';
export type { AlertRulesPageProps } from './AlertRulesPage';
export { AlertTemplates } from './AlertTemplates';
export type { AlertTemplatesProps } from './AlertTemplates';

// AlertContext
export { AlertProvider, useAlerts, useAlertsSafe } from './AlertContext';

// API
export { alertApi } from './api';
export { default as alertApiDefault } from './api';

// UI state / modal lifecycle
export { AlertUIProvider, useAlertsUI } from './AlertUIContext';
