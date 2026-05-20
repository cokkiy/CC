# Phase 9: Alert Rules Management System - Frontend Architecture

## 1. Overview

The Alert Rules system allows users to define conditions that trigger alerts when station telemetry data crosses defined thresholds. This follows the established patterns from Phase 6 (Scripts) and Phase 7 (Batch Operations).

## 2. Component Structure

```
src/plugin/alert/
├── types.ts                 # Type definitions
├── api.ts                    # Tauri backend API communication
├── AlertContext.tsx          # React context for data management
├── AlertUIContext.tsx        # UI state (modals, drawers)
├── AlertRulesPage.tsx        # Main page container
├── AlertRuleList.tsx         # List/table view of alert rules
├── AlertRuleEditor.tsx       # Create/edit alert rule (drawer)
├── AlertRuleRunner.tsx       # Execute/test alert rule (modal)
├── AlertHistoryPanel.tsx     # Alert history log panel
├── AlertTemplates.tsx        # Template marketplace
├── index.ts                  # Main exports
└── PHASE9_ALERT_RULES_ARCHITECTURE.md
```

## 3. Component Responsibilities

### AlertRulesPage
Main container following the ScriptsPage pattern:
- Renders AlertRuleList as main content
- Manages drawer (AlertRuleEditor) and modal (AlertRuleRunner) overlays
- Converts stations to targets for runner

### AlertRuleList
Data table displaying all alert rules:
- Columns: Name, Condition, Severity, Status, Target, Last Triggered, Actions
- Sortable, filterable
- Row actions: Edit, Clone, Delete, Toggle Enable, Run Now

### AlertRuleEditor
Drawer panel for create/edit:
- Form fields: Name, Description, Condition (metric + operator + threshold), Severity, Cooldown, Actions
- Target selector (station/group/tag)
- Validation before save

### AlertRuleRunner
Modal for testing/executing alert rule:
- Shows selected targets
- Parameter input (if any)
- Real-time execution status
- Results display with triggered/not triggered

### AlertHistoryPanel
Bottom/side panel showing alert history:
- Chronological list of triggered alerts
- Filter by: Rule, Severity, Station, Time range
- Shows: Timestamp, Rule name, Station, Value, Status

### AlertTemplates
Template marketplace (optional Phase 9.1):
- Browse pre-built alert rule templates
- Categories: CPU, Memory, Disk, Network, Custom
- One-click import

## 4. Data Flow

```
AlertRulesPage
    ├── AlertContext (data management)
    │   ├── loadAlertRules()
    │   ├── saveAlertRule()
    │   ├── deleteAlertRule()
    │   ├── executeAlertRule()
    │   └── getAlertHistory()
    │
    └── AlertUIContext (UI state)
        ├── openEditor(task?)
        ├── closeEditor()
        ├── openRunner(task)
        └── closeRunner()
```

## 5. Type Definitions (types.ts)

```typescript
// AlertRule: Core rule definition
// AlertCondition: metric + operator + threshold
// AlertAction: what happens when triggered
// AlertHistory: triggered alert record
// AlertSeverity: info | warning | critical
// AlertStatus: enabled | disabled
// MetricType: cpu | memory | disk | network | custom
```

## 6. API Endpoints (Tauri invoke)

```typescript
load_alert_rules()
save_alert_rule(rule)
delete_alert_rule(rule_id)
toggle_alert_rule(rule_id)
execute_alert_rule(rule_id, target_ids)
get_alert_history(limit, filters)
validate_alert_rule(rule)
import_alert_rules(pkg)
export_alert_rules(rule_ids)
```

## 7. Pattern Alignment with Phase 6/7

| Aspect | Phase 6 (Scripts) | Phase 7 (Batch) | Phase 9 (Alerts) |
|--------|------------------|------------------|------------------|
| Page | ScriptsPage | BatchPage | AlertRulesPage |
| List | ScriptList | BatchTaskList | AlertRuleList |
| Editor | ScriptEditor | BatchTaskEditor | AlertRuleEditor |
| Runner | ScriptRunner | BatchTaskRunner | AlertRuleRunner |
| Data Context | ScriptContext | BatchContext | AlertContext |
| UI Context | ScriptsUIContext | BatchUIContext | AlertUIContext |
| API | scriptApi | batchApi | alertApi |

## 8. Key Design Decisions

1. **Drawer for Editor**: Matches ScriptsPage - editor slides in from right
2. **Modal for Runner**: Alert rule execution/test uses modal overlay
3. **History Panel**: Collapsible panel at bottom, can be resized
4. **Cooldown Period**: Prevents alert spam - minimum time between triggers
5. **Multiple Actions**: Each rule can have multiple actions (notify, script, webhook)
6. **Template Support**: Alert templates marketplace for common monitoring scenarios
