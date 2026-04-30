/**
 * AlertHistoryPanel - Panel showing alert trigger history
 * Part of Phase 9: Alert Rules Management System
 */

import React, { useState, useEffect, useMemo } from 'react';
import { useAlerts } from './AlertContext';
import type { AlertHistory, AlertHistoryFilter, AlertSeverity, AlertTriggerStatus } from './types';

export interface AlertHistoryPanelProps {
  onClose: () => void;
  filterRuleId?: string;
}

type SortField = 'triggeredAt' | 'severity' | 'status' | 'ruleName';
type SortDirection = 'asc' | 'desc';

export const AlertHistoryPanel: React.FC<AlertHistoryPanelProps> = ({ onClose, filterRuleId }) => {
  const { history, loadHistory, acknowledgeAlert, resolveAlert } = useAlerts();

  // Filters
  const [severityFilter, setSeverityFilter] = useState<AlertSeverity | 'all'>('all');
  const [statusFilter, setStatusFilter] = useState<AlertTriggerStatus | 'all'>('all');
  const [searchQuery, setSearchQuery] = useState('');
  const [dateFrom, setDateFrom] = useState<string>('');
  const [dateTo, setDateTo] = useState<string>('');

  // Sorting
  const [sortField, setSortField] = useState<SortField>('triggeredAt');
  const [sortDirection, setSortDirection] = useState<SortDirection>('desc');

  // Load history on mount
  useEffect(() => {
    const filters: AlertHistoryFilter = {
      ...(filterRuleId ? { ruleId: filterRuleId } : {}),
    };
    loadHistory(filters, 100);
  }, [filterRuleId, loadHistory]);

  // Filter and sort history
  const filteredHistory = useMemo(() => {
    let result = [...history];

    // Search filter
    if (searchQuery) {
      const query = searchQuery.toLowerCase();
      result = result.filter(
        (h) =>
          h.ruleName.toLowerCase().includes(query) ||
          h.targetName.toLowerCase().includes(query)
      );
    }

    // Severity filter
    if (severityFilter !== 'all') {
      result = result.filter((h) => h.severity === severityFilter);
    }

    // Status filter
    if (statusFilter !== 'all') {
      result = result.filter((h) => h.status === statusFilter);
    }

    // Date range filter
    if (dateFrom) {
      const fromDate = new Date(dateFrom);
      result = result.filter((h) => new Date(h.triggeredAt) >= fromDate);
    }
    if (dateTo) {
      const toDate = new Date(dateTo);
      result = result.filter((h) => new Date(h.triggeredAt) <= toDate);
    }

    // Sort
    result.sort((a, b) => {
      let comparison = 0;
      switch (sortField) {
        case 'triggeredAt':
          comparison = new Date(a.triggeredAt).getTime() - new Date(b.triggeredAt).getTime();
          break;
        case 'severity':
          const severityOrder = { critical: 0, warning: 1, info: 2 };
          comparison = severityOrder[a.severity] - severityOrder[b.severity];
          break;
        case 'status':
          comparison = a.status.localeCompare(b.status);
          break;
        case 'ruleName':
          comparison = a.ruleName.localeCompare(b.ruleName);
          break;
      }
      return sortDirection === 'asc' ? comparison : -comparison;
    });

    return result;
  }, [history, searchQuery, severityFilter, statusFilter, dateFrom, dateTo, sortField, sortDirection]);

  const handleSort = (field: SortField) => {
    if (sortField === field) {
      setSortDirection((prev) => (prev === 'asc' ? 'desc' : 'asc'));
    } else {
      setSortField(field);
      setSortDirection('desc');
    }
  };

  const formatTimestamp = (timestamp: string): string => {
    const date = new Date(timestamp);
    const now = new Date();
    const diffMs = now.getTime() - date.getTime();
    const diffMins = Math.floor(diffMs / (1000 * 60));
    
    if (diffMins < 1) return 'Just now';
    if (diffMins < 60) return `${diffMins}m ago`;
    
    const diffHours = Math.floor(diffMins / 60);
    if (diffHours < 24) return `${diffHours}h ago`;
    
    const diffDays = Math.floor(diffHours / 24);
    if (diffDays < 7) return `${diffDays}d ago`;
    
    return date.toLocaleDateString() + ' ' + date.toLocaleTimeString([], { hour: '2-digit', minute: '2-digit' });
  };

  const formatDuration = (start: string, end?: string): string => {
    if (!end) return ' ongoing ';
    const startDate = new Date(start);
    const endDate = new Date(end);
    const diffMs = endDate.getTime() - startDate.getTime();
    const diffMins = Math.floor(diffMs / (1000 * 60));
    if (diffMins < 60) return `${diffMins}m`;
    const diffHours = Math.floor(diffMins / 60);
    if (diffHours < 24) return `${diffHours}h ${diffMins % 60}m`;
    const diffDays = Math.floor(diffHours / 24);
    return `${diffDays}d`;
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

  const getStatusBadgeClass = (status: AlertTriggerStatus): string => {
    switch (status) {
      case 'triggered':
        return 'badge-critical';
      case 'resolved':
        return 'badge-success';
      case 'acknowledged':
        return 'badge-warning';
      case 'expired':
        return 'badge-muted';
    }
  };

  const handleAcknowledge = async (historyId: string) => {
    await acknowledgeAlert(historyId);
  };

  const handleResolve = async (historyId: string) => {
    await resolveAlert(historyId);
  };

  const SortIcon = ({ field }: { field: SortField }) => {
    if (sortField !== field) return <span className="sort-icon">⇅</span>;
    return <span className="sort-icon">{sortDirection === 'asc' ? '↑' : '↓'}</span>;
  };

  return (
    <div className="alert-history-panel">
      <div className="history-header">
        <div className="header-title">
          <h2>Alert History</h2>
          {filterRuleId && <span className="filter-badge">Filtered</span>}
        </div>
        <button className="btn-close" onClick={onClose}>×</button>
      </div>

      <div className="history-filters">
        <input
          type="text"
          className="search-input"
          placeholder="Search by rule or station name..."
          value={searchQuery}
          onChange={(e) => setSearchQuery(e.target.value)}
        />

        <div className="filter-row">
          <div className="filter-group">
            <label>Severity:</label>
            <select
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
            <label>Status:</label>
            <select
              value={statusFilter}
              onChange={(e) => setStatusFilter(e.target.value as AlertTriggerStatus | 'all')}
            >
              <option value="all">All</option>
              <option value="triggered">Triggered</option>
              <option value="resolved">Resolved</option>
              <option value="acknowledged">Acknowledged</option>
              <option value="expired">Expired</option>
            </select>
          </div>

          <div className="filter-group">
            <label>From:</label>
            <input
              type="date"
              value={dateFrom}
              onChange={(e) => setDateFrom(e.target.value)}
            />
          </div>

          <div className="filter-group">
            <label>To:</label>
            <input
              type="date"
              value={dateTo}
              onChange={(e) => setDateTo(e.target.value)}
            />
          </div>
        </div>
      </div>

      <div className="history-content">
        {filteredHistory.length === 0 ? (
          <div className="empty-state">
            <p>No alert history found</p>
          </div>
        ) : (
          <table className="history-table">
            <thead>
              <tr>
                <th className="col-time sortable" onClick={() => handleSort('triggeredAt')}>
                  Time <SortIcon field="triggeredAt" />
                </th>
                <th className="col-rule sortable" onClick={() => handleSort('ruleName')}>
                  Rule <SortIcon field="ruleName" />
                </th>
                <th className="col-target">Target</th>
                <th className="col-severity sortable" onClick={() => handleSort('severity')}>
                  Severity <SortIcon field="severity" />
                </th>
                <th className="col-condition">Condition</th>
                <th className="col-status sortable" onClick={() => handleSort('status')}>
                  Status <SortIcon field="status" />
                </th>
                <th className="col-actions">Actions</th>
              </tr>
            </thead>
            <tbody>
              {filteredHistory.map((record) => (
                <tr key={record.id} className={`status-${record.status}`}>
                  <td className="col-time">
                    <div className="time-cell">
                      <span className="timestamp">{formatTimestamp(record.triggeredAt)}</span>
                      <span className="duration">{formatDuration(record.triggeredAt, record.resolvedAt)}</span>
                    </div>
                  </td>
                  <td className="col-rule">{record.ruleName}</td>
                  <td className="col-target">{record.targetName}</td>
                  <td className="col-severity">
                    <span className={`badge ${getSeverityBadgeClass(record.severity)}`}>
                      {record.severity}
                    </span>
                  </td>
                  <td className="col-condition">
                    <span className="condition-text">{record.condition}</span>
                    <span className="condition-value">
                      {record.triggerValue} vs {record.threshold}
                    </span>
                  </td>
                  <td className="col-status">
                    <span className={`badge ${getStatusBadgeClass(record.status)}`}>
                      {record.status}
                    </span>
                  </td>
                  <td className="col-actions">
                    <div className="action-buttons">
                      {record.status === 'triggered' && (
                        <>
                          <button
                            className="btn-icon"
                            title="Acknowledge"
                            onClick={() => handleAcknowledge(record.id)}
                          >
                            ✓
                          </button>
                          <button
                            className="btn-icon"
                            title="Resolve"
                            onClick={() => handleResolve(record.id)}
                          >
                            ●
                          </button>
                        </>
                      )}
                      {record.status === 'acknowledged' && (
                        <button
                          className="btn-icon"
                          title="Resolve"
                          onClick={() => handleResolve(record.id)}
                        >
                          ●
                        </button>
                      )}
                    </div>
                  </td>
                </tr>
              ))}
            </tbody>
          </table>
        )}
      </div>

      <div className="history-footer">
        <span className="history-count">
          Showing {filteredHistory.length} of {history.length} records
        </span>
      </div>

      <style>{`
        .alert-history-panel {
          position: fixed;
          bottom: 0;
          left: 0;
          right: 0;
          height: 45vh;
          min-height: 300px;
          background: var(--bg-card);
          border-top: 1px solid var(--border-color);
          display: flex;
          flex-direction: column;
          z-index: 1100;
          box-shadow: 0 -4px 24px rgba(8, 20, 38, 0.12);
        }

        .history-header {
          display: flex;
          justify-content: space-between;
          align-items: center;
          padding: 16px 20px;
          border-bottom: 1px solid var(--border-color);
          background: var(--bg-card);
        }

        .header-title {
          display: flex;
          align-items: center;
          gap: 12px;
        }

        .header-title h2 {
          margin: 0;
          font-size: 16px;
        }

        .filter-badge {
          padding: 4px 8px;
          background: var(--color-primary);
          color: white;
          border-radius: 4px;
          font-size: 11px;
          font-weight: 600;
        }

        .btn-close {
          background: none;
          border: none;
          font-size: 24px;
          cursor: pointer;
          padding: 0;
          line-height: 1;
          color: var(--text-secondary);
        }

        .history-filters {
          padding: 12px 20px;
          border-bottom: 1px solid var(--border-color);
          background: var(--bg-hover);
        }

        .search-input {
          width: 100%;
          padding: 8px 12px;
          border: 1px solid var(--border-color);
          border-radius: 6px;
          background: var(--bg-input);
          color: var(--text-primary);
          font-size: 14px;
          margin-bottom: 12px;
        }

        .filter-row {
          display: flex;
          gap: 16px;
          flex-wrap: wrap;
        }

        .filter-group {
          display: flex;
          align-items: center;
          gap: 8px;
        }

        .filter-group label {
          font-size: 13px;
          color: var(--text-secondary);
          white-space: nowrap;
        }

        .filter-group select,
        .filter-group input[type="date"] {
          padding: 6px 10px;
          border: 1px solid var(--border-color);
          border-radius: 4px;
          background: var(--bg-input);
          color: var(--text-primary);
          font-size: 13px;
        }

        .history-content {
          flex: 1;
          overflow-y: auto;
        }

        .empty-state {
          display: flex;
          align-items: center;
          justify-content: center;
          height: 100%;
          color: var(--text-secondary);
        }

        .history-table {
          width: 100%;
          border-collapse: collapse;
        }

        .history-table th,
        .history-table td {
          padding: 10px 16px;
          text-align: left;
          border-bottom: 1px solid var(--border-color);
        }

        .history-table th {
          font-weight: 600;
          font-size: 12px;
          color: var(--text-secondary);
          background: var(--bg-card);
          position: sticky;
          top: 0;
          z-index: 1;
        }

        .history-table th.sortable {
          cursor: pointer;
          user-select: none;
        }

        .history-table th.sortable:hover {
          background: var(--bg-hover);
        }

        .sort-icon {
          margin-left: 4px;
          opacity: 0.5;
        }

        .col-time {
          width: 120px;
        }

        .col-rule {
          min-width: 150px;
        }

        .col-target {
          min-width: 120px;
        }

        .col-severity {
          width: 90px;
        }

        .col-condition {
          min-width: 180px;
        }

        .col-status {
          width: 110px;
        }

        .col-actions {
          width: 100px;
        }

        .time-cell {
          display: flex;
          flex-direction: column;
        }

        .timestamp {
          font-size: 13px;
        }

        .duration {
          font-size: 11px;
          color: var(--text-secondary);
        }

        .condition-text {
          display: block;
          font-size: 13px;
        }

        .condition-value {
          display: block;
          font-size: 11px;
          color: var(--text-secondary);
        }

        .badge {
          display: inline-block;
          padding: 3px 8px;
          border-radius: 4px;
          font-size: 11px;
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

        .badge-success {
          background: rgba(16, 185, 129, 0.15);
          color: #10b981;
        }

        .badge-muted {
          background: var(--bg-hover);
          color: var(--text-secondary);
        }

        .action-buttons {
          display: flex;
          gap: 4px;
        }

        .btn-icon {
          padding: 4px 8px;
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

        .history-footer {
          padding: 10px 20px;
          border-top: 1px solid var(--border-color);
          background: var(--bg-card);
        }

        .history-count {
          font-size: 13px;
          color: var(--text-secondary);
        }

        .status-triggered {
          background: rgba(239, 68, 68, 0.03);
        }

        .status-acknowledged {
          background: rgba(245, 158, 11, 0.03);
        }
      `}</style>
    </div>
  );
};
