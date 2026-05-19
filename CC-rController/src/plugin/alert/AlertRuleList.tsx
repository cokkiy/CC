/**
 * AlertRuleList - List/Table view for Alert Rules
 * Part of Phase 9: Alert Rules Management System
 */

import React, { useState, useMemo } from 'react';
import type { AlertRule, AlertRuleFilter, AlertSeverity } from './types';

export interface AlertRuleListProps {
  rules: AlertRule[];
  onEditRule: (rule: AlertRule | undefined) => void;
  onExecuteRule: (rule: AlertRule) => void;
  onDeleteRule: (ruleId: string) => void;
  onToggleFavorite: (ruleId: string) => void;
  onToggleStatus: (ruleId: string) => void;
  onDuplicateRule: (rule: AlertRule) => void;
  onViewHistory: (ruleId?: string) => void;
  onImport?: () => void;
}

type SortField = 'name' | 'severity' | 'status' | 'lastTriggeredAt' | 'createdAt';
type SortDirection = 'asc' | 'desc';

export const AlertRuleList: React.FC<AlertRuleListProps> = ({
  rules,
  onEditRule,
  onExecuteRule,
  onDeleteRule,
  onToggleFavorite,
  onToggleStatus,
  onDuplicateRule,
  onViewHistory,
  onImport,
}) => {
  const [searchQuery, setSearchQuery] = useState('');
  const [severityFilter, setSeverityFilter] = useState<AlertSeverity | 'all'>('all');
  const [statusFilter, setStatusFilter] = useState<'all' | 'enabled' | 'disabled'>('all');
  const [sortField, setSortField] = useState<SortField>('name');
  const [sortDirection, setSortDirection] = useState<SortDirection>('asc');
  const [selectedRules, setSelectedRules] = useState<Set<string>>(new Set());

  // Filter and sort rules
  const filteredRules = useMemo(() => {
    let result = rules;

    // Search filter
    if (searchQuery) {
      const query = searchQuery.toLowerCase();
      result = result.filter(
        (rule) =>
          rule.name.toLowerCase().includes(query) ||
          rule.description?.toLowerCase().includes(query) ||
          rule.tags.some((tag) => tag.toLowerCase().includes(query))
      );
    }

    // Severity filter
    if (severityFilter !== 'all') {
      result = result.filter((rule) => rule.severity === severityFilter);
    }

    // Status filter
    if (statusFilter !== 'all') {
      result = result.filter((rule) => rule.status === statusFilter);
    }

    // Sort
    result = [...result].sort((a, b) => {
      let comparison = 0;
      switch (sortField) {
        case 'name':
          comparison = a.name.localeCompare(b.name);
          break;
        case 'severity':
          const severityOrder = { critical: 0, warning: 1, info: 2 };
          comparison = severityOrder[a.severity] - severityOrder[b.severity];
          break;
        case 'status':
          comparison = a.status.localeCompare(b.status);
          break;
        case 'lastTriggeredAt':
          const aTime = a.lastTriggeredAt ? new Date(a.lastTriggeredAt).getTime() : 0;
          const bTime = b.lastTriggeredAt ? new Date(b.lastTriggeredAt).getTime() : 0;
          comparison = aTime - bTime;
          break;
        case 'createdAt':
          comparison = new Date(a.createdAt).getTime() - new Date(b.createdAt).getTime();
          break;
      }
      return sortDirection === 'asc' ? comparison : -comparison;
    });

    return result;
  }, [rules, searchQuery, severityFilter, statusFilter, sortField, sortDirection]);

  const handleSort = (field: SortField) => {
    if (sortField === field) {
      setSortDirection((prev) => (prev === 'asc' ? 'desc' : 'asc'));
    } else {
      setSortField(field);
      setSortDirection('asc');
    }
  };

  const handleSelectAll = (checked: boolean) => {
    if (checked) {
      setSelectedRules(new Set(filteredRules.map((r) => r.id)));
    } else {
      setSelectedRules(new Set());
    }
  };

  const handleSelectRule = (ruleId: string, checked: boolean) => {
    const newSelected = new Set(selectedRules);
    if (checked) {
      newSelected.add(ruleId);
    } else {
      newSelected.delete(ruleId);
    }
    setSelectedRules(newSelected);
  };

  const formatCondition = (rule: AlertRule): string => {
    const conditions = rule.conditionGroup.conditions;
    if (conditions.length === 0) return 'No conditions';
    if (conditions.length === 1) {
      const c = conditions[0];
      return `${c.metricType} ${c.operator} ${c.threshold}${c.unit || '%'}`;
    }
    return `${conditions.length} conditions (${rule.conditionGroup.logic})`;
  };

  const formatLastTriggered = (rule: AlertRule): string => {
    if (!rule.lastTriggeredAt) return 'Never';
    const date = new Date(rule.lastTriggeredAt);
    const now = new Date();
    const diffMs = now.getTime() - date.getTime();
    const diffHours = Math.floor(diffMs / (1000 * 60 * 60));
    if (diffHours < 1) return 'Just now';
    if (diffHours < 24) return `${diffHours}h ago`;
    const diffDays = Math.floor(diffHours / 24);
    if (diffDays < 7) return `${diffDays}d ago`;
    return date.toLocaleDateString();
  };

  const getSeverityBadgeClass = (severity: AlertSeverity): string => {
    switch (severity) {
      case 'critical':
        return 'badge-critical';
      case 'warning':
        return 'badge-warning';
      case 'info':
        return 'badge-info';
    }
  };

  const SortIcon = ({ field }: { field: SortField }) => {
    if (sortField !== field) return <span className="sort-icon">⇅</span>;
    return <span className="sort-icon">{sortDirection === 'asc' ? '↑' : '↓'}</span>;
  };

  return (
    <div className="alert-rule-list">
      {/* Toolbar */}
      <div className="list-toolbar">
        <div className="toolbar-left">
          <button className="btn btn-primary" onClick={() => onEditRule(undefined)}>
            + New Alert Rule
          </button>
          {onImport && (
            <button className="btn btn-secondary" onClick={onImport}>
              Import
            </button>
          )}
        </div>
        <div className="toolbar-right">
          <input
            type="text"
            className="search-input"
            placeholder="Search rules..."
            value={searchQuery}
            onChange={(e) => setSearchQuery(e.target.value)}
          />
          <div className="filter-group">
            <label htmlFor="alert-severity-filter">Severity</label>
            <select
              id="alert-severity-filter"
              value={severityFilter}
              onChange={(e) => setSeverityFilter(e.target.value as AlertSeverity | 'all')}
            >
              <option value="all">All</option>
              <option value="critical">Critical</option>
              <option value="warning">Warning</option>
              <option value="info">Info</option>
            </select>
          </div>
          <div className="filter-group">
            <label htmlFor="alert-status-filter">Status</label>
            <select
              id="alert-status-filter"
              value={statusFilter}
              onChange={(e) => setStatusFilter(e.target.value as 'all' | 'enabled' | 'disabled')}
            >
              <option value="all">All</option>
              <option value="enabled">Enabled</option>
              <option value="disabled">Disabled</option>
            </select>
          </div>
        </div>
      </div>

      {/* Table */}
      <div className="list-table-container">
        <table className="list-table">
          <thead>
            <tr>
              <th className="col-checkbox">
                <input
                  type="checkbox"
                  checked={selectedRules.size === filteredRules.length && filteredRules.length > 0}
                  onChange={(e) => handleSelectAll(e.target.checked)}
                />
              </th>
              <th className="col-name sortable" onClick={() => handleSort('name')}>
                Name <SortIcon field="name" />
              </th>
              <th className="col-severity sortable" onClick={() => handleSort('severity')}>
                Severity <SortIcon field="severity" />
              </th>
              <th className="col-condition">Condition</th>
              <th className="col-status sortable" onClick={() => handleSort('status')}>
                Status <SortIcon field="status" />
              </th>
              <th className="col-last-triggered sortable" onClick={() => handleSort('lastTriggeredAt')}>
                Last Triggered <SortIcon field="lastTriggeredAt" />
              </th>
              <th className="col-actions">Actions</th>
            </tr>
          </thead>
          <tbody>
            {filteredRules.length === 0 ? (
              <tr className="empty-row">
                <td colSpan={7}>
                  <div className="empty-state">
                    <p>No alert rules found</p>
                    <button className="btn btn-primary" onClick={() => onEditRule(undefined)}>
                      Create your first alert rule
                    </button>
                  </div>
                </td>
              </tr>
            ) : (
              filteredRules.map((rule) => (
                <tr key={rule.id} className={selectedRules.has(rule.id) ? 'selected' : ''}>
                  <td className="col-checkbox">
                    <input
                      type="checkbox"
                      checked={selectedRules.has(rule.id)}
                      onChange={(e) => handleSelectRule(rule.id, e.target.checked)}
                    />
                  </td>
                  <td className="col-name">
                    <div className="rule-name-cell">
                      <button
                        type="button"
                        className={`favorite-btn ${rule.isFavorite ? 'active' : ''}`}
                        onClick={() => onToggleFavorite(rule.id)}
                        aria-pressed={rule.isFavorite}
                        aria-label={`${rule.isFavorite ? 'Unfavorite' : 'Favorite'} alert rule ${rule.name}`}
                        title={rule.isFavorite ? 'Unfavorite alert rule' : 'Favorite alert rule'}
                      >
                        {rule.isFavorite ? '★' : '☆'}
                      </button>
                      <span className="rule-name">{rule.name}</span>
                    </div>
                  </td>
                  <td className="col-severity">
                    <span className={`badge ${getSeverityBadgeClass(rule.severity)}`}>
                      {rule.severity}
                    </span>
                  </td>
                  <td className="col-condition">{formatCondition(rule)}</td>
                  <td className="col-status">
                    <div className="status-cell">
                      <span className={`status-label ${rule.status === 'enabled' ? 'status-label-enabled' : 'status-label-disabled'}`}>
                        {rule.status === 'enabled' ? 'Enabled' : 'Disabled'}
                      </span>
                      <button
                        type="button"
                        className={`status-switch ${rule.status === 'enabled' ? 'is-enabled' : 'is-disabled'}`}
                        onClick={(e) => {
                          e.stopPropagation();
                          onToggleStatus(rule.id);
                        }}
                        aria-label={`Set ${rule.name} ${rule.status === 'enabled' ? 'disabled' : 'enabled'}`}
                        aria-pressed={rule.status === 'enabled'}
                        title={rule.status === 'enabled' ? 'Disable alert rule' : 'Enable alert rule'}
                      >
                        <span className="status-switch-thumb" />
                      </button>
                    </div>
                  </td>
                  <td className="col-last-triggered">{formatLastTriggered(rule)}</td>
                  <td className="col-actions">
                    <div className="action-buttons">
                      <button
                        className="btn-icon"
                        title="Edit"
                        onClick={() => onEditRule(rule)}
                      >
                        ✏️
                      </button>
                      <button
                        className="btn-icon"
                        title="Test / Run"
                        onClick={() => onExecuteRule(rule)}
                      >
                        ▶️
                      </button>
                      <button
                        className="btn-icon"
                        title="History"
                        onClick={() => onViewHistory(rule.id)}
                      >
                        📋
                      </button>
                      <button
                        className="btn-icon"
                        title="Clone"
                        onClick={() => onDuplicateRule(rule)}
                      >
                        📑
                      </button>
                      <button
                        className="btn-icon btn-danger"
                        title="Delete"
                        onClick={() => onDeleteRule(rule.id)}
                      >
                        🗑️
                      </button>
                    </div>
                  </td>
                </tr>
              ))
            )}
          </tbody>
        </table>
      </div>

      {/* Summary */}
      <div className="list-summary">
        Showing {filteredRules.length} of {rules.length} rules
        {selectedRules.size > 0 && ` (${selectedRules.size} selected)`}
      </div>

      <style>{`
        .alert-rule-list {
          height: 100%;
          display: flex;
          flex-direction: column;
        }

        .list-toolbar {
          display: flex;
          justify-content: space-between;
          align-items: center;
          padding: 16px;
          border-bottom: 1px solid var(--border-color);
          gap: 12px;
          flex-wrap: wrap;
        }

        .toolbar-left {
          display: flex;
          gap: 8px;
          flex-wrap: wrap;
        }

        .toolbar-right {
          display: flex;
          gap: 12px;
          align-items: center;
          flex: 1;
          justify-content: flex-end;
          flex-wrap: wrap;
        }

        .search-input {
          padding: 8px 12px;
          border: 1px solid var(--border-color);
          border-radius: 6px;
          background: var(--bg-input);
          color: var(--text-primary);
          width: min(280px, 100%);
        }

        .filter-group {
          display: flex;
          align-items: center;
          gap: 8px;
          flex-shrink: 0;
        }

        .filter-group label {
          font-size: 13px;
          color: var(--text-secondary);
        }

        .filter-group select {
          padding: 6px 10px;
          border: 1px solid var(--border-color);
          border-radius: 4px;
          background: var(--bg-input);
          color: var(--text-primary);
        }

        @media (max-width: 900px) {
          .toolbar-right {
            justify-content: flex-start;
            width: 100%;
          }

          .search-input {
            flex: 1 1 220px;
          }
        }

        .list-table-container {
          flex: 1;
          overflow: auto;
        }

        .list-table {
          width: 100%;
          border-collapse: collapse;
        }

        .list-table th,
        .list-table td {
          padding: 12px 16px;
          text-align: left;
          border-bottom: 1px solid var(--border-color);
        }

        .list-table th {
          font-weight: 600;
          font-size: 13px;
          color: var(--text-secondary);
          background: var(--bg-card);
          position: sticky;
          top: 0;
          z-index: 1;
        }

        .list-table th.sortable {
          cursor: pointer;
          user-select: none;
        }

        .list-table th.sortable:hover {
          background: var(--bg-hover);
        }

        .sort-icon {
          margin-left: 4px;
          opacity: 0.5;
        }

        .col-checkbox {
          width: 40px;
        }

        .col-name {
          min-width: 200px;
        }

        .col-severity {
          width: 100px;
        }

        .col-condition {
          min-width: 180px;
        }

        .col-status {
          width: 140px;
        }

        .col-last-triggered {
          width: 120px;
        }

        .col-actions {
          width: 200px;
        }

        .rule-name-cell {
          display: flex;
          align-items: center;
          gap: 8px;
        }

        .favorite-btn {
          cursor: pointer;
          font-size: 16px;
          color: var(--text-muted);
          transition: color 0.2s;
        }

        .favorite-btn:hover,
        .favorite-btn.active {
          color: #f0b429;
        }

        .badge {
          display: inline-block;
          padding: 4px 8px;
          border-radius: 4px;
          font-size: 12px;
          font-weight: 600;
          text-transform: uppercase;
        }

        .badge-critical {
          background: rgba(239, 68, 68, 0.15);
          color: #ef4444;
        }

        .badge-warning {
          background: rgba(245, 158, 11, 0.15);
          color: #f59e0b;
        }

        .badge-info {
          background: rgba(59, 130, 246, 0.15);
          color: #3b82f6;
        }

        .toggle-switch {
          position: relative;
          display: inline-block;
          width: 40px;
          height: 22px;
        }

        .status-cell {
          display: inline-flex;
          align-items: center;
          gap: 10px;
        }

        .status-label {
          font-size: 12px;
          font-weight: 600;
          line-height: 1;
        }

        .status-label-enabled {
          color: #15803d;
        }

        .status-label-disabled {
          color: var(--text-muted);
        }

        .status-switch {
          position: relative;
          width: 40px;
          height: 22px;
          padding: 0;
          border-radius: 999px;
          border: 1px solid var(--border-color);
          background: rgba(148, 163, 184, 0.18);
          cursor: pointer;
          transition: background 0.2s, border-color 0.2s;
        }

        .status-switch.is-disabled {
          background: rgba(148, 163, 184, 0.18);
          border-color: var(--border-color);
        }

        .status-switch.is-enabled {
          background: rgba(25, 190, 107, 0.22);
          border-color: rgba(25, 190, 107, 0.42);
        }

        .status-switch:hover {
          background: rgba(148, 163, 184, 0.24);
          border-color: var(--border-color);
          transform: none;
          box-shadow: none;
          color: transparent;
        }

        .status-switch.is-enabled:hover {
          background: rgba(25, 190, 107, 0.28);
          border-color: rgba(25, 190, 107, 0.5);
        }

        .status-switch-thumb {
          position: absolute;
          top: 0;
          left: 0;
          height: 16px;
          width: 16px;
          margin: 2px;
          background: var(--bg-card);
          box-shadow: 0 1px 2px rgba(15, 23, 42, 0.2);
          transition: transform 0.2s ease;
          border-radius: 50%;
        }

        .status-switch.is-enabled .status-switch-thumb {
          transform: translateX(18px);
        }

        .status-switch:focus-visible {
          outline: 2px solid rgba(45, 140, 240, 0.35);
          outline-offset: 2px;
        }

        .action-buttons {
          display: flex;
          gap: 4px;
        }

        .btn-icon {
          padding: 6px;
          border: none;
          background: transparent;
          cursor: pointer;
          border-radius: 4px;
          font-size: 14px;
          transition: background 0.2s;
        }

        .btn-icon:hover {
          background: var(--bg-hover);
        }

        .btn-icon.btn-danger:hover {
          background: rgba(239, 68, 68, 0.1);
        }

        .empty-row td {
          padding: 0;
        }

        .empty-state {
          padding: 48px;
          text-align: center;
          color: var(--text-secondary);
        }

        .empty-state p {
          margin-bottom: 16px;
        }

        .list-summary {
          padding: 12px 16px;
          font-size: 13px;
          color: var(--text-secondary);
          border-top: 1px solid var(--border-color);
        }

        .btn {
          padding: 8px 16px;
          border: 1px solid transparent;
          border-radius: 6px;
          cursor: pointer;
          font-weight: 500;
          transition: background 0.2s, border-color 0.2s, color 0.2s;
        }

        .btn-primary {
          background: var(--primary);
          border-color: var(--primary);
          color: white;
          -webkit-text-fill-color: white;
        }

        .btn-primary:hover {
          background: var(--primary-hover);
          border-color: var(--primary-hover);
        }

        .btn-secondary {
          background: transparent;
          border-color: var(--border-color);
          color: var(--text-primary);
        }

        .btn-secondary:hover {
          background: var(--bg-hover);
          border-color: var(--primary);
        }
      `}</style>
    </div>
  );
};
