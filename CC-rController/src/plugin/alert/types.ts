/**
 * Alert Rules Type Definitions
 * Part of Phase 9: Alert Rules Management System
 */

// ============================================
// Enums and Unions
// ============================================

// Alert severity levels
export type AlertSeverity = 'info' | 'warning' | 'critical';

// Alert rule status
export type AlertRuleStatus = 'enabled' | 'disabled';

// Alert trigger status
export type AlertTriggerStatus = 'triggered' | 'resolved' | 'acknowledged' | 'expired';

// Metric types for conditions
export type MetricType =
  | 'cpu_usage'      // CPU utilization percentage
  | 'cpu_load'       // CPU load average
  | 'memory_usage'   // Memory utilization percentage
  | 'memory_available' // Available memory in MB/GB
  | 'disk_usage'     // Disk utilization percentage
  | 'disk_free'      // Free disk space
  | 'network_rx'     // Network receive rate (bytes/sec)
  | 'network_tx'     // Network transmit rate (bytes/sec)
  | 'process_count'  // Number of processes
  | 'custom';        // Custom metric via script

// Comparison operators for conditions
export type ComparisonOperator = 'gt' | 'gte' | 'lt' | 'lte' | 'eq' | 'neq' | 'between';

// Action types when alert triggers
export type AlertActionType = 'notification' | 'script' | 'webhook' | 'email' | 'auto_remediate';

// Target selector type
export type AlertTargetSelectorType = 'all' | 'group' | 'tag' | 'device_ids' | 'filter';

// ============================================
// Alert Condition
// ============================================

export interface AlertCondition {
  id: string;
  metricType: MetricType;
  operator: ComparisonOperator;
  threshold: number;
  thresholdHigh?: number;  // For 'between' operator
  unit?: string;           // %, MB, GB, etc.
  evaluationInterval?: number; // Seconds between checks
}

// Composite condition with AND/OR
export interface AlertConditionGroup {
  logic: 'and' | 'or';
  conditions: AlertCondition[];
}

// ============================================
// Alert Actions
// ============================================

export interface AlertAction {
  id: string;
  actionType: AlertActionType;
  name: string;
  enabled: boolean;
  config: AlertActionConfig;
}

export interface AlertActionConfig {
  // For notification
  message?: string;
  channels?: string[];  // 'in_app', 'email', 'sms', etc.

  // For script
  scriptId?: string;
  scriptParams?: Record<string, string>;

  // For webhook
  webhookUrl?: string;
  webhookMethod?: 'GET' | 'POST' | 'PUT';
  webhookHeaders?: Record<string, string>;
  webhookPayload?: string;

  // For email
  emailRecipients?: string[];

  // For auto_remediate
  remediationScriptId?: string;
}

// ============================================
// Target Selector
// ============================================

export interface AlertTargetSelector {
  selectorType: AlertTargetSelectorType;
  groups?: string[];
  tags?: string[];
  deviceIds?: string[];
  filterExpr?: string;
}

// ============================================
// Alert Rule
// ============================================

export interface AlertRule {
  id: string;
  name: string;
  description?: string;
  conditionGroup: AlertConditionGroup;
  targetSelector: AlertTargetSelector;
  severity: AlertSeverity;
  status: AlertRuleStatus;
  cooldownSecs: number;          // Minimum time between triggers
  maxTriggersPerHour?: number;   // Rate limiting
  actions: AlertAction[];
  isTemplate: boolean;
  isFavorite: boolean;
  tags: string[];
  createdBy: string;
  createdAt: string;
  updatedAt: string;
  version: number;
  usageCount: number;
  lastTriggeredAt?: string;
}

// ============================================
// Alert History
// ============================================

export interface AlertHistory {
  id: string;
  ruleId: string;
  ruleName: string;
  targetId: string;
  targetName: string;
  severity: AlertSeverity;
  status: AlertTriggerStatus;
  triggerValue: number;
  threshold: number;
  condition: string;           // Human-readable condition
  triggeredAt: string;
  resolvedAt?: string;
  acknowledgedAt?: string;
  acknowledgedBy?: string;
  actionsTriggered: string[]; // Action IDs that were executed
  actionResults: AlertActionResult[];
  metadata?: Record<string, unknown>;
}

export interface AlertActionResult {
  actionId: string;
  actionType: AlertActionType;
  status: 'success' | 'failed' | 'skipped';
  message?: string;
  executedAt: string;
  durationMs?: number;
}

// ============================================
// Execution Result Types
// ============================================

export interface AlertExecutionResult {
  ruleId: string;
  executionId: string;
  triggered: boolean;
  triggeredTargets: string[];
  timestamp: string;
  results: AlertTargetResult[];
}

export interface AlertTargetResult {
  targetId: string;
  targetName: string;
  triggered: boolean;
  currentValue: number;
  threshold: number;
  message?: string;
}

// ============================================
// Validation
// ============================================

export interface AlertRuleValidationResult {
  valid: boolean;
  errors: string[];
  warnings: string[];
  conditionErrors?: Record<string, string[]>;
}

// ============================================
// Filter and Search
// ============================================

export interface AlertRuleFilter {
  search?: string;
  severity?: AlertSeverity[];
  status?: AlertRuleStatus[];
  tags?: string[];
  createdBy?: string;
  dateFrom?: string;
  dateTo?: string;
}

export interface AlertHistoryFilter {
  ruleId?: string;
  targetId?: string;
  severity?: AlertSeverity[];
  status?: AlertTriggerStatus[];
  dateFrom?: string;
  dateTo?: string;
}

// ============================================
// Import/Export
// ============================================

export interface AlertRulePackage {
  metadata: AlertRulePackageMetadata;
  rules: AlertRule[];
  signature?: string;
}

export interface AlertRulePackageMetadata {
  id: string;
  name: string;
  version: string;
  description?: string;
  author?: string;
  tags: string[];
  createdAt: string;
  updatedAt: string;
}

export interface AlertRuleImportResult {
  success: boolean;
  imported: number;
  skipped: number;
  errors: string[];
  warnings: string[];
}

// ============================================
// Statistics
// ============================================

export interface AlertStats {
  totalRules: number;
  enabledRules: number;
  triggeredToday: number;
  criticalAlerts: number;
  averageResponseTimeMs: number;
}

// ============================================
// Execution Context
// ============================================

export interface AlertExecutionContext {
  ruleId: string;
  targetIds: string[];
  dryRun?: boolean;  // Test without triggering actions
}

// ============================================
// Predefined Templates
// ============================================

export type AlertTemplateCategory =
  | 'system'        // System resource alerts
  | 'network'       // Network related alerts
  | 'security'      // Security threshold alerts
  | 'application'   // Application specific
  | 'custom';       // User custom templates

export interface AlertTemplate {
  id: string;
  name: string;
  description: string;
  category: AlertTemplateCategory;
  rule: Omit<AlertRule, 'id' | 'createdAt' | 'updatedAt' | 'version' | 'usageCount' | 'lastTriggeredAt'>;
  installCount: number;
  rating?: number;
}
