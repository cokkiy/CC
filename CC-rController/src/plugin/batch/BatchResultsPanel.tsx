/**
 * Batch Results Panel Component
 * Displays execution results summary after batch task completion
 * Part of Phase 7: Batch Operations Support System
 */

import React, { useState, useMemo } from 'react';
import type {
  BatchExecutionResult,
  BatchTargetResult,
  BatchTaskStatus,
} from './types';

// ============================================
// Status Badge Component
// ============================================

interface StatusBadgeProps {
  status: BatchTaskStatus;
}

const StatusBadge: React.FC<StatusBadgeProps> = ({ status }) => {
  const statusConfig: Record<BatchTaskStatus, { color: string; bg: string; icon: string }> = {
    draft: { color: '#6b7280', bg: '#f3f4f6', icon: '📝' },
    pending: { color: '#f59e0b', bg: '#fef3c7', icon: '⏳' },
    running: { color: '#3b82f6', bg: '#dbeafe', icon: '🔄' },
    paused: { color: '#8b5cf6', bg: '#ede9fe', icon: '⏸' },
    completed: { color: '#22c55e', bg: '#dcfce7', icon: '✅' },
    partial_failure: { color: '#f97316', bg: '#ffedd5', icon: '⚠️' },
    failed: { color: '#ef4444', bg: '#fee2e2', icon: '❌' },
    cancelled: { color: '#6b7280', bg: '#f3f4f6', icon: '🚫' },
  };

  const config = statusConfig[status] || statusConfig.draft;

  return (
    <span
      className="status-badge"
      style={{ color: config.color, backgroundColor: config.bg }}
    >
      {config.icon} {status.replace('_', ' ')}
    </span>
  );
};

// ============================================
// Result Details Modal Component
// ============================================

interface ResultDetailsModalProps {
  result: BatchTargetResult;
  onClose: () => void;
}

const ResultDetailsModal: React.FC<ResultDetailsModalProps> = ({ result, onClose }) => {
  const formatDuration = (ms?: number) => {
    if (!ms) return '-';
    if (ms < 1000) return `${ms}ms`;
    if (ms < 60000) return `${(ms / 1000).toFixed(1)}s`;
    return `${(ms / 60000).toFixed(1)}m`;
  };

  const formatTimestamp = (timestamp: string) => {
    const date = new Date(timestamp);
    return date.toLocaleString();
  };

  return (
    <div className="modal-overlay">
      <div className="modal-content details-modal">
        <div className="modal-header">
          <h3>Target Result Details</h3>
          <button className="btn-close" onClick={onClose}>×</button>
        </div>

        <div className="modal-body">
          <div className="detail-section">
            <h4>Target Information</h4>
            <div className="detail-row">
              <span className="detail-label">Target ID:</span>
              <span className="detail-value">{result.targetId}</span>
            </div>
            <div className="detail-row">
              <span className="detail-label">Target Name:</span>
              <span className="detail-value">{result.targetName}</span>
            </div>
            <div className="detail-row">
              <span className="detail-label">Status:</span>
              <span className="detail-value">
                <StatusBadge status={result.status as BatchTaskStatus} />
              </span>
            </div>
          </div>

          <div className="detail-section">
            <h4>Timing</h4>
            <div className="detail-row">
              <span className="detail-label">Started:</span>
              <span className="detail-value">{formatTimestamp(result.startedAt)}</span>
            </div>
            {result.completedAt && (
              <div className="detail-row">
                <span className="detail-label">Completed:</span>
                <span className="detail-value">{formatTimestamp(result.completedAt)}</span>
              </div>
            )}
            <div className="detail-row">
              <span className="detail-label">Duration:</span>
              <span className="detail-value">{formatDuration(result.durationMs)}</span>
            </div>
            {result.retryAttempt !== undefined && result.retryAttempt > 0 && (
              <div className="detail-row">
                <span className="detail-label">Retry Attempt:</span>
                <span className="detail-value">{result.retryAttempt}</span>
              </div>
            )}
          </div>

          {result.exitCode !== undefined && (
            <div className="detail-section">
              <h4>Exit Code</h4>
              <div className="exit-code-display">
                {result.exitCode === 0 ? (
                  <span className="exit-code success">{result.exitCode}</span>
                ) : (
                  <span className="exit-code error">{result.exitCode}</span>
                )}
              </div>
            </div>
          )}

          {result.error && (
            <div className="detail-section">
              <h4>Error</h4>
              <pre className="error-content">{result.error}</pre>
            </div>
          )}

          {result.output && (
            <div className="detail-section">
              <div className="output-header">
                <h4>Output</h4>
                <button
                  className="btn-copy"
                  onClick={() => navigator.clipboard.writeText(result.output || '')}
                >
                  📋 Copy
                </button>
              </div>
              <pre className="output-content">{result.output}</pre>
            </div>
          )}
        </div>

        <div className="modal-footer">
          <button className="btn-close-modal" onClick={onClose}>
            Close
          </button>
        </div>
      </div>
    </div>
  );
};

// ============================================
// Main Batch Results Panel Component
// ============================================

export interface BatchResultsPanelProps {
  result: BatchExecutionResult;
  onClose?: () => void;
  onRetryFailed?: (targetIds: string[]) => void;
  onViewDetails?: (targetId: string) => void;
  onExportResults?: (result: BatchExecutionResult) => void;
}

export const BatchResultsPanel: React.FC<BatchResultsPanelProps> = ({
  result,
  onClose,
  onRetryFailed,
  onViewDetails,
  onExportResults,
}) => {
  const [sortBy, setSortBy] = useState<'target' | 'status' | 'duration'>('target');
  const [sortOrder, setSortOrder] = useState<'asc' | 'desc'>('asc');
  const [filterStatus, setFilterStatus] = useState<string>('all');
  const [selectedResult, setSelectedResult] = useState<BatchTargetResult | null>(null);

  // Sort results
  const sortedResults = useMemo(() => {
    return [...result.results].sort((a, b) => {
      let comparison = 0;
      switch (sortBy) {
        case 'target':
          comparison = a.targetName.localeCompare(b.targetName);
          break;
        case 'status':
          comparison = a.status.localeCompare(b.status);
          break;
        case 'duration':
          comparison = (a.durationMs || 0) - (b.durationMs || 0);
          break;
      }
      return sortOrder === 'asc' ? comparison : -comparison;
    });
  }, [result.results, sortBy, sortOrder]);

  // Filter results
  const filteredResults = useMemo(() => {
    if (filterStatus === 'all') return sortedResults;
    return sortedResults.filter(r => r.status === filterStatus);
  }, [sortedResults, filterStatus]);

  // Handle sort
  const handleSort = (column: typeof sortBy) => {
    if (sortBy === column) {
      setSortOrder(prev => prev === 'asc' ? 'desc' : 'asc');
    } else {
      setSortBy(column);
      setSortOrder('asc');
    }
  };

  // Format duration
  const formatDuration = (start?: string, end?: string) => {
    if (!start) return '-';
    const startDate = new Date(start);
    const endDate = end ? new Date(end) : new Date();
    const diff = endDate.getTime() - startDate.getTime();
    if (diff < 1000) return `${diff}ms`;
    if (diff < 60000) return `${(diff / 1000).toFixed(1)}s`;
    return `${(diff / 60000).toFixed(1)}m`;
  };

  // Get failed targets
  const failedTargets = result.results.filter(r => r.status === 'failed' || r.status === 'timeout');

  // Get status badge for result
  const getStatusBadge = (status: string) => {
    const config: Record<string, { color: string; bg: string }> = {
      success: { color: '#22c55e', bg: '#dcfce7' },
      failed: { color: '#ef4444', bg: '#fee2e2' },
      timeout: { color: '#f97316', bg: '#ffedd5' },
      skipped: { color: '#6b7280', bg: '#f3f4f6' },
      cancelled: { color: '#6b7280', bg: '#f3f4f6' },
    };
    const style = config[status] || config.skipped;
    return (
      <span style={{ color: style.color, backgroundColor: style.bg }} className="status-badge">
        {status}
      </span>
    );
  };

  return (
    <div className="batch-results-panel modal-overlay">
      <div className="modal-content results-modal">
        <div className="results-header">
          <h2>Batch Execution Results</h2>
          <button className="btn-close" onClick={onClose}>×</button>
        </div>

        <div className="results-body">
          {/* Summary Section */}
          <div className="summary-section">
            <div className="summary-header">
              <div className="overall-status">
                <StatusBadge status={result.status} />
              </div>
              <div className="summary-stats">
                <div className="stat-card success">
                  <span className="stat-icon">✓</span>
                  <span className="stat-value">{result.successCount}</span>
                  <span className="stat-label">Success</span>
                </div>
                <div className="stat-card failed">
                  <span className="stat-icon">✗</span>
                  <span className="stat-value">{result.failureCount}</span>
                  <span className="stat-label">Failed</span>
                </div>
                <div className="stat-card skipped">
                  <span className="stat-icon">⊘</span>
                  <span className="stat-value">{result.skippedCount}</span>
                  <span className="stat-label">Skipped</span>
                </div>
                <div className="stat-card total">
                  <span className="stat-icon">📊</span>
                  <span className="stat-value">{result.totalTargets}</span>
                  <span className="stat-label">Total</span>
                </div>
              </div>
            </div>

            <div className="summary-details">
              <div className="detail-item">
                <span className="detail-icon">🕐</span>
                <span className="detail-text">
                  Duration: {formatDuration(result.startedAt, result.completedAt)}
                </span>
              </div>
              {result.failureRate !== undefined && (
                <div className="detail-item">
                  <span className="detail-icon">⚠️</span>
                  <span className="detail-text">
                    Failure Rate: {result.failureRate.toFixed(1)}%
                  </span>
                </div>
              )}
              {result.circuitBreakerTriggered && (
                <div className="detail-item circuit-breaker">
                  <span className="detail-icon">🔴</span>
                  <span className="detail-text">
                    Circuit Breaker Triggered
                  </span>
                </div>
              )}
            </div>

            <div className="summary-timing">
              <div className="timing-row">
                <span className="timing-label">Started:</span>
                <span className="timing-value">{new Date(result.startedAt).toLocaleString()}</span>
              </div>
              {result.completedAt && (
                <div className="timing-row">
                  <span className="timing-label">Completed:</span>
                  <span className="timing-value">{new Date(result.completedAt).toLocaleString()}</span>
                </div>
              )}
            </div>
          </div>

          {/* Failed Targets Warning */}
          {failedTargets.length > 0 && onRetryFailed && (
            <div className="failed-warning">
              <div className="warning-header">
                <span className="warning-icon">⚠️</span>
                <span>{failedTargets.length} target(s) failed or timed out</span>
              </div>
              <div className="warning-actions">
                <button
                  className="btn-retry-failed"
                  onClick={() => onRetryFailed(failedTargets.map(t => t.targetId))}
                >
                  🔄 Retry Failed
                </button>
              </div>
            </div>
          )}

          {/* Results Table */}
          <div className="results-table-section">
            <div className="table-toolbar">
              <div className="filter-control">
                <label>Filter by status:</label>
                <select
                  value={filterStatus}
                  onChange={(e) => setFilterStatus(e.target.value)}
                >
                  <option value="all">All</option>
                  <option value="success">Success</option>
                  <option value="failed">Failed</option>
                  <option value="timeout">Timeout</option>
                  <option value="skipped">Skipped</option>
                </select>
              </div>
              <div className="table-actions">
                {onExportResults && (
                  <button
                    className="btn-export"
                    onClick={() => onExportResults(result)}
                  >
                    📤 Export Results
                  </button>
                )}
                <span className="results-count">
                  Showing {filteredResults.length} of {result.results.length} results
                </span>
              </div>
            </div>

            <div className="table-wrapper">
              <table className="results-table">
                <thead>
                  <tr>
                    <th
                      className="sortable"
                      onClick={() => handleSort('target')}
                    >
                      Target {sortBy === 'target' && (sortOrder === 'asc' ? '↑' : '↓')}
                    </th>
                    <th
                      className="sortable"
                      onClick={() => handleSort('status')}
                    >
                      Status {sortBy === 'status' && (sortOrder === 'asc' ? '↑' : '↓')}
                    </th>
                    <th
                      className="sortable"
                      onClick={() => handleSort('duration')}
                    >
                      Duration {sortBy === 'duration' && (sortOrder === 'asc' ? '↑' : '↓')}
                    </th>
                    <th>Exit Code</th>
                    <th>Output / Error</th>
                    <th>Actions</th>
                  </tr>
                </thead>
                <tbody>
                  {filteredResults.map((r, i) => (
                    <tr key={i}>
                      <td className="cell-target">
                        <div className="target-name">{r.targetName}</div>
                        <div className="target-id">{r.targetId}</div>
                      </td>
                      <td className="cell-status">
                        {getStatusBadge(r.status)}
                        {r.retryAttempt !== undefined && r.retryAttempt > 0 && (
                          <span className="retry-badge" title={`Retry attempt #${r.retryAttempt}`}>
                            ↻ #{r.retryAttempt}
                          </span>
                        )}
                      </td>
                      <td className="cell-duration">
                        {formatDuration(r.startedAt, r.completedAt)}
                      </td>
                      <td className="cell-exit-code">
                        {r.exitCode !== undefined ? (
                          <span className={`exit-code ${r.exitCode === 0 ? 'success' : 'error'}`}>
                            {r.exitCode}
                          </span>
                        ) : '-'}
                      </td>
                      <td className="cell-output">
                        {r.error ? (
                          <span className="error-text" title={r.error}>
                            {r.error.substring(0, 60)}
                            {r.error.length > 60 ? '...' : ''}
                          </span>
                        ) : r.output ? (
                          <span className="output-text" title={r.output}>
                            {r.output.substring(0, 60)}
                            {r.output.length > 60 ? '...' : ''}
                          </span>
                        ) : (
                          <span className="no-output">-</span>
                        )}
                      </td>
                      <td className="cell-actions">
                        <button
                          className="btn-view"
                          onClick={() => setSelectedResult(r)}
                          title="View details"
                        >
                          👁
                        </button>
                      </td>
                    </tr>
                  ))}
                </tbody>
              </table>
            </div>
          </div>
        </div>

        <div className="results-footer">
          <button className="btn-close-results" onClick={onClose}>
            Close
          </button>
        </div>
      </div>

      {/* Details Modal */}
      {selectedResult && (
        <ResultDetailsModal
          result={selectedResult}
          onClose={() => setSelectedResult(null)}
        />
      )}
    </div>
  );
};
