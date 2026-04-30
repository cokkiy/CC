/**
 * Batch Task Runner Component
 * Interface for executing batch tasks and monitoring progress
 * Part of Phase 7: Batch Operations Support System
 */

import React, { useState, useCallback, useEffect, useRef, useMemo } from 'react';
import type {
  BatchTask,
  BatchTaskParameter,
  BatchTarget,
  BatchExecutionResult,
  BatchTargetResult,
  BatchTaskStatus,
  BatchExecutionLogEntry,
  TargetSelector,
  ExecutionPolicy,
} from './types';

// ============================================
// Parameter Input Component
// ============================================

interface ParameterInputProps {
  parameter: BatchTaskParameter;
  value: string;
  onChange: (value: string) => void;
  disabled?: boolean;
}

const ParameterInput: React.FC<ParameterInputProps> = ({ parameter, value, onChange, disabled }) => {
  const [error, setError] = useState<string | null>(null);

  const validateValue = useCallback((val: string) => {
    if (parameter.required && !val.trim()) {
      return 'This field is required';
    }

    if (parameter.validation) {
      try {
        const regex = new RegExp(parameter.validation);
        if (!regex.test(val)) {
          return `Value does not match pattern: ${parameter.validation}`;
        }
      } catch {
        return 'Invalid validation pattern';
      }
    }

    if (parameter.paramType === 'number' && val) {
      if (isNaN(Number(val))) {
        return 'Please enter a valid number';
      }
    }

    return null;
  }, [parameter]);

  const handleChange = (newValue: string) => {
    const error = validateValue(newValue);
    setError(error);
    onChange(newValue);
  };

  const renderInput = () => {
    if (parameter.paramType === 'boolean') {
      return (
        <select
          className="param-select"
          value={value || 'false'}
          onChange={(e) => handleChange(e.target.value)}
          disabled={disabled}
        >
          <option value="true">True</option>
          <option value="false">False</option>
        </select>
      );
    }

    if (parameter.paramType === 'select' && parameter.options) {
      return (
        <select
          className="param-select"
          value={value || parameter.options[0]}
          onChange={(e) => handleChange(e.target.value)}
          disabled={disabled}
        >
          {parameter.options.map(opt => (
            <option key={opt} value={opt}>{opt}</option>
          ))}
        </select>
      );
    }

    if (parameter.paramType === 'number') {
      return (
        <input
          type="number"
          className="param-input"
          value={value}
          onChange={(e) => handleChange(e.target.value)}
          disabled={disabled}
          placeholder={parameter.defaultValue || 'Enter a number...'}
        />
      );
    }

    return (
      <input
        type="text"
        className="param-input"
        value={value}
        onChange={(e) => handleChange(e.target.value)}
        disabled={disabled}
        placeholder={parameter.defaultValue || 'Enter a value...'}
      />
    );
  };

  return (
    <div className={`parameter-input ${error ? 'has-error' : ''}`}>
      <div className="param-label-row">
        <label className="param-label">
          {parameter.name}
          {parameter.required && <span className="required">*</span>}
        </label>
        {parameter.description && (
          <span className="param-description-text">{parameter.description}</span>
        )}
      </div>
      {renderInput()}
      {error && <span className="param-error">{error}</span>}
    </div>
  );
};

// ============================================
// Execution Log Component
// ============================================

interface ExecutionLogProps {
  logs: BatchExecutionLogEntry[];
  maxLogs?: number;
}

const ExecutionLog: React.FC<ExecutionLogProps> = ({ logs, maxLogs = 500 }) => {
  const logRef = useRef<HTMLDivElement>(null);
  const [autoScroll, setAutoScroll] = useState(true);

  const displayLogs = logs.slice(-maxLogs);

  useEffect(() => {
    if (autoScroll && logRef.current) {
      logRef.current.scrollTop = logRef.current.scrollHeight;
    }
  }, [displayLogs, autoScroll]);

  const handleScroll = (e: React.UIEvent<HTMLDivElement>) => {
    const target = e.target as HTMLDivElement;
    const isAtBottom = target.scrollHeight - target.scrollTop <= target.clientHeight + 50;
    setAutoScroll(isAtBottom);
  };

  const formatTimestamp = (timestamp: string) => {
    const date = new Date(timestamp);
    return date.toLocaleTimeString([], { hour: '2-digit', minute: '2-digit', second: '2-digit' });
  };

  const getLevelColor = (level: string) => {
    switch (level) {
      case 'info': return '#3b82f6';
      case 'warning': return '#f59e0b';
      case 'error': return '#ef4444';
      case 'debug': return '#6b7280';
      default: return '#6b7280';
    }
  };

  return (
    <div className="execution-log" onScroll={handleScroll} ref={logRef}>
      {displayLogs.length === 0 ? (
        <div className="log-empty">No logs yet</div>
      ) : (
        displayLogs.map((log, i) => (
          <div key={i} className={`log-entry log-${log.level}`}>
            <span className="log-timestamp">{formatTimestamp(log.timestamp)}</span>
            <span className="log-level-badge" style={{ color: getLevelColor(log.level) }}>
              {log.level.toUpperCase()}
            </span>
            {log.targetName && <span className="log-target">[{log.targetName}]</span>}
            <span className="log-message">{log.message}</span>
          </div>
        ))
      )}
    </div>
  );
};

// ============================================
// Target Selection Component
// ============================================

interface TargetSelectionProps {
  selectedTargets: string[];
  availableTargets: BatchTarget[];
  onChange: (targets: string[]) => void;
  disabled?: boolean;
}

const TargetSelection: React.FC<TargetSelectionProps> = ({
  selectedTargets,
  availableTargets,
  onChange,
  disabled = false,
}) => {
  const [searchQuery, setSearchQuery] = useState('');
  const [selectAll, setSelectAll] = useState(selectedTargets.length === availableTargets.length);

  const filteredTargets = availableTargets.filter(t =>
    t.name.toLowerCase().includes(searchQuery.toLowerCase()) ||
    t.id.toLowerCase().includes(searchQuery.toLowerCase())
  );

  const handleToggle = (targetId: string) => {
    if (selectedTargets.includes(targetId)) {
      onChange(selectedTargets.filter(id => id !== targetId));
    } else {
      onChange([...selectedTargets, targetId]);
    }
  };

  const handleSelectAll = () => {
    if (selectAll) {
      onChange([]);
    } else {
      onChange(availableTargets.map(t => t.id));
    }
    setSelectAll(!selectAll);
  };

  const getStatusIcon = (status: string) => {
    switch (status) {
      case 'online': return '🟢';
      case 'offline': return '🔴';
      case 'busy': return '🟡';
      default: return '⚪';
    }
  };

  return (
    <div className="target-selection">
      <div className="target-search">
        <input
          type="text"
          placeholder="Search targets..."
          value={searchQuery}
          onChange={(e) => setSearchQuery(e.target.value)}
          disabled={disabled}
        />
      </div>

      <div className="target-select-all">
        <label>
          <input
            type="checkbox"
            checked={selectAll}
            onChange={handleSelectAll}
            disabled={disabled}
          />
          <span>Select All ({availableTargets.length})</span>
        </label>
      </div>

      <div className="target-list">
        {filteredTargets.length === 0 ? (
          <div className="target-empty">No matching targets</div>
        ) : (
          filteredTargets.map(target => (
            <label key={target.id} className="target-item">
              <input
                type="checkbox"
                checked={selectedTargets.includes(target.id)}
                onChange={() => handleToggle(target.id)}
                disabled={disabled}
              />
              <span className="target-status-icon">{getStatusIcon(target.status)}</span>
              <span className="target-name">{target.name}</span>
              {target.group && <span className="target-group">({target.group})</span>}
            </label>
          ))
        )}
      </div>

      <div className="target-count">
        {selectedTargets.length} of {availableTargets.length} selected
      </div>
    </div>
  );
};

interface ResolvedTargetPreviewProps {
  selector: TargetSelector;
  targets: BatchTarget[];
}

const ResolvedTargetPreview: React.FC<ResolvedTargetPreviewProps> = ({ selector, targets }) => {
  const describeSelector = () => {
    switch (selector.selectorType) {
      case 'all':
        return 'Uses the task’s saved "All Devices" target selector.';
      case 'group':
        return `Uses the task’s saved group selector${selector.groups?.length ? `: ${selector.groups.join(', ')}` : '.'}`;
      case 'tag':
        return `Uses the task’s saved tag selector${selector.tags?.length ? `: ${selector.tags.join(', ')}` : '.'}`;
      case 'device_ids':
        return 'Uses the task’s saved device ID selector.';
      case 'filter':
        return `Uses the task’s saved filter expression${selector.filterExpr ? `: ${selector.filterExpr}` : '.'}`;
      default:
        return 'Uses the task’s saved target selector.';
    }
  };

  return (
    <div className="resolved-target-preview">
      <div className="selector-summary">{describeSelector()}</div>
      <div className="resolved-target-list">
        {targets.length === 0 ? (
          <div className="target-empty">No matching targets resolved from the saved selector.</div>
        ) : (
          targets.map((target) => (
            <div key={target.id} className="resolved-target-item">
              <span className="target-status-icon">
                {target.status === 'online' && '🟢'}
                {target.status === 'offline' && '🔴'}
                {target.status === 'busy' && '🟡'}
                {target.status === 'unknown' && '⚪'}
              </span>
              <span className="target-name">{target.name}</span>
              {target.group && <span className="target-group">({target.group})</span>}
            </div>
          ))
        )}
      </div>
      <div className="target-count">
        {targets.length} target{targets.length === 1 ? '' : 's'} resolved from saved task settings
      </div>
    </div>
  );
};

function formatExecutionError(error: unknown): string {
  if (error instanceof Error && error.message) {
    return error.message;
  }

  if (typeof error === 'string') {
    return error;
  }

  if (error && typeof error === 'object') {
    const maybeMessage = Reflect.get(error, 'message');
    if (typeof maybeMessage === 'string' && maybeMessage.trim()) {
      return maybeMessage;
    }

    try {
      return JSON.stringify(error);
    } catch {
      return 'Unknown error';
    }
  }

  return 'Unknown error';
}

// ============================================
// Progress Display Component
// ============================================

interface ProgressDisplayProps {
  result: BatchExecutionResult | null;
  isRunning: boolean;
  mode: 'parallel' | 'batch' | 'rolling';
}

const ProgressDisplay: React.FC<ProgressDisplayProps> = ({ result, isRunning, mode }) => {
  const [elapsed, setElapsed] = useState(0);
  const intervalRef = useRef<ReturnType<typeof setInterval> | null>(null);

  useEffect(() => {
    if (isRunning) {
      intervalRef.current = setInterval(() => {
        setElapsed(e => e + 1);
      }, 1000);
    } else {
      if (intervalRef.current) {
        clearInterval(intervalRef.current);
      }
    }

    return () => {
      if (intervalRef.current) {
        clearInterval(intervalRef.current);
      }
    };
  }, [isRunning]);

  const formatElapsed = (seconds: number) => {
    const mins = Math.floor(seconds / 60);
    const secs = seconds % 60;
    return `${mins}:${secs.toString().padStart(2, '0')}`;
  };

  const progress = result
    ? ((result.successCount + result.failureCount + result.skippedCount) / result.totalTargets) * 100
    : 0;

  return (
    <div className="progress-display">
      <div className="progress-header">
        <div className="progress-info">
          {isRunning && (
            <span className="running-indicator">● Running</span>
          )}
          <span className="elapsed-time">Elapsed: {formatElapsed(elapsed)}</span>
        </div>
        {result && (
          <div className="progress-stats">
            <span className="stat success">✓ {result.successCount}</span>
            <span className="stat failed">✗ {result.failureCount}</span>
            <span className="stat skipped">⊘ {result.skippedCount}</span>
            <span className="stat total">/ {result.totalTargets}</span>
          </div>
        )}
      </div>

      <div className="progress-bar-container">
        <div
          className="progress-bar"
          style={{ width: `${progress}%` }}
        />
      </div>

      <div className="progress-percentage">{progress.toFixed(1)}%</div>

      {result && result.circuitBreakerTriggered && (
        <div className="circuit-breaker-warning">
          ⚠️ Circuit breaker triggered! Failure rate exceeded threshold.
        </div>
      )}

      {result && result.failureRate !== undefined && (
        <div className="failure-rate">
          Failure rate: {result.failureRate.toFixed(1)}%
        </div>
      )}
    </div>
  );
};

// ============================================
// Results Table Component
// ============================================

interface ResultsTableProps {
  results: BatchTargetResult[];
  onViewDetails?: (result: BatchTargetResult) => void;
}

const ResultsTable: React.FC<ResultsTableProps> = ({ results, onViewDetails }) => {
  const [sortBy, setSortBy] = useState<'target' | 'status' | 'duration'>('target');
  const [sortOrder, setSortOrder] = useState<'asc' | 'desc'>('asc');
  const [filterStatus, setFilterStatus] = useState<string>('all');

  const sortedResults = [...results].sort((a, b) => {
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

  const filteredResults = filterStatus === 'all'
    ? sortedResults
    : sortedResults.filter(r => r.status === filterStatus);

  const handleSort = (column: typeof sortBy) => {
    if (sortBy === column) {
      setSortOrder(prev => prev === 'asc' ? 'desc' : 'asc');
    } else {
      setSortBy(column);
      setSortOrder('asc');
    }
  };

  const formatDuration = (ms?: number) => {
    if (!ms) return '-';
    if (ms < 1000) return `${ms}ms`;
    if (ms < 60000) return `${(ms / 1000).toFixed(1)}s`;
    return `${(ms / 60000).toFixed(1)}m`;
  };

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
    <div className="results-table-container">
      <div className="results-toolbar">
        <label>
          Filter by status:
          <select value={filterStatus} onChange={(e) => setFilterStatus(e.target.value)}>
            <option value="all">All</option>
            <option value="success">Success</option>
            <option value="failed">Failed</option>
            <option value="timeout">Timeout</option>
            <option value="skipped">Skipped</option>
          </select>
        </label>
        <span className="results-count">{filteredResults.length} results</span>
      </div>

      <div className="results-table-wrapper">
        <table className="results-table">
          <thead>
            <tr>
              <th onClick={() => handleSort('target')} className="sortable">
                Target {sortBy === 'target' && (sortOrder === 'asc' ? '↑' : '↓')}
              </th>
              <th onClick={() => handleSort('status')} className="sortable">
                Status {sortBy === 'status' && (sortOrder === 'asc' ? '↑' : '↓')}
              </th>
              <th onClick={() => handleSort('duration')} className="sortable">
                Duration {sortBy === 'duration' && (sortOrder === 'asc' ? '↑' : '↓')}
              </th>
              <th>Output / Error</th>
              <th>Actions</th>
            </tr>
          </thead>
          <tbody>
            {filteredResults.map((result, i) => (
              <tr key={i}>
                <td className="cell-target">
                  <div className="target-name">{result.targetName}</div>
                  <div className="target-id">{result.targetId}</div>
                </td>
                <td className="cell-status">
                  {getStatusBadge(result.status)}
                  {result.retryAttempt !== undefined && result.retryAttempt > 0 && (
                    <span className="retry-badge">Retry #{result.retryAttempt}</span>
                  )}
                </td>
                <td className="cell-duration">{formatDuration(result.durationMs)}</td>
                <td className="cell-output">
                  {result.error ? (
                    <span className="error-preview" title={result.error}>
                      {result.error.substring(0, 50)}...
                    </span>
                  ) : result.output ? (
                    <span className="output-preview" title={result.output}>
                      {result.output.substring(0, 50)}...
                    </span>
                  ) : (
                    <span className="no-output">-</span>
                  )}
                </td>
                <td className="cell-actions">
                  <button
                    className="btn-view-details"
                    onClick={() => onViewDetails?.(result)}
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
  );
};

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

  return (
    <div className="result-details-overlay" onClick={onClose}>
      <div className="result-details-modal" onClick={(event) => event.stopPropagation()}>
        <div className="result-details-header">
          <h3>Execution Details</h3>
          <button type="button" className="btn-close" onClick={onClose}>×</button>
        </div>

        <div className="result-details-body">
          <div className="detail-card">
            <h4>Target</h4>
            <div className="detail-row">
              <span className="detail-label">Name</span>
              <span className="detail-value">{result.targetName}</span>
            </div>
            <div className="detail-row">
              <span className="detail-label">ID</span>
              <span className="detail-value detail-code">{result.targetId}</span>
            </div>
            <div className="detail-row">
              <span className="detail-label">Status</span>
              <span className="detail-value">{result.status}</span>
            </div>
            <div className="detail-row">
              <span className="detail-label">Duration</span>
              <span className="detail-value">{formatDuration(result.durationMs)}</span>
            </div>
            <div className="detail-row">
              <span className="detail-label">Exit Code</span>
              <span className="detail-value">{result.exitCode ?? 'N/A'}</span>
            </div>
            {result.retryAttempt !== undefined && (
              <div className="detail-row">
                <span className="detail-label">Retry</span>
                <span className="detail-value">{result.retryAttempt}</span>
              </div>
            )}
          </div>

          {result.output && (
            <div className="detail-card">
              <div className="detail-card-header">
                <h4>Output</h4>
                <button
                  type="button"
                  className="btn-copy"
                  onClick={() => navigator.clipboard.writeText(result.output || '')}
                >
                  📋 Copy
                </button>
              </div>
              <pre className="detail-pre">{result.output}</pre>
            </div>
          )}

          {result.error && (
            <div className="detail-card">
              <div className="detail-card-header">
                <h4>Error</h4>
                <button
                  type="button"
                  className="btn-copy"
                  onClick={() => navigator.clipboard.writeText(result.error || '')}
                >
                  📋 Copy
                </button>
              </div>
              <pre className="detail-pre detail-pre-error">{result.error}</pre>
            </div>
          )}
        </div>
      </div>
    </div>
  );
};

// ============================================
// Main Batch Task Runner Component
// ============================================

export interface BatchTaskRunnerProps {
  task: BatchTask;
  targets: BatchTarget[];
  onExecute: (
    taskId: string,
    targetIds: string[],
    parameters?: Record<string, string>
  ) => Promise<BatchExecutionResult>;
  onCancel?: () => void;
  onPause?: () => void;
  onResume?: () => void;
  defaultTargets?: string[];
  readOnly?: boolean;
}

export const BatchTaskRunner: React.FC<BatchTaskRunnerProps> = ({
  task,
  targets,
  onExecute,
  onCancel,
  onPause,
  onResume,
  defaultTargets = [],
  readOnly = false,
}) => {
  const usesTaskSelector = defaultTargets.length === 0;
  const [selectedTargets, setSelectedTargets] = useState<string[]>(defaultTargets);
  const [parameters, setParameters] = useState<Record<string, string>>(() => {
    const defaults: Record<string, string> = {};
    task.parameters?.forEach(p => {
      if (p.defaultValue) defaults[p.name] = p.defaultValue;
    });
    return defaults;
  });

  const [isExecuting, setIsExecuting] = useState(false);
  const [isPaused, setIsPaused] = useState(false);
  const [result, setResult] = useState<BatchExecutionResult | null>(null);
  const [logs, setLogs] = useState<BatchExecutionLogEntry[]>([]);
  const [selectedResult, setSelectedResult] = useState<BatchTargetResult | null>(null);

  const resolvedTargets = useMemo(() => {
    const selector = task.targetSelector;

    switch (selector.selectorType) {
      case 'all':
        return targets;
      case 'group':
        return targets.filter((target) =>
          selector.groups?.some((groupId) => target.groups?.includes(groupId)),
        );
      case 'tag':
        return targets.filter((target) =>
          target.tags && selector.tags?.some((tag) => target.tags && tag in target.tags),
        );
      case 'device_ids':
        return targets.filter((target) => selector.deviceIds?.includes(target.id));
      case 'filter': {
        const expr = selector.filterExpr?.trim().toLowerCase();
        if (!expr) {
          return [];
        }
        return targets.filter((target) =>
          target.name.toLowerCase().includes(expr)
          || target.id.toLowerCase().includes(expr)
          || target.group?.toLowerCase().includes(expr),
        );
      }
      default:
        return [];
    }
  }, [task.targetSelector, targets]);

  // Validate parameters
  const validateParameters = useCallback(() => {
    const errors: string[] = [];

    task.parameters?.forEach(p => {
      const value = parameters[p.name] || '';

      if (p.required && !value.trim()) {
        errors.push(`${p.name} is required`);
      }

      if (p.validation && value) {
        try {
          const regex = new RegExp(p.validation);
          if (!regex.test(value)) {
            errors.push(`${p.name} does not match validation pattern`);
          }
        } catch {
          errors.push(`${p.name} has invalid validation pattern`);
        }
      }
    });

    return errors;
  }, [task.parameters, parameters]);

  // Add log entry
  const addLog = useCallback((level: BatchExecutionLogEntry['level'], message: string, targetId?: string, targetName?: string) => {
    setLogs(prev => [
      ...prev,
      {
        timestamp: new Date().toISOString(),
        level,
        executionId: result?.executionId || 'new',
        targetId,
        targetName,
        message,
      },
    ]);
  }, [result?.executionId]);

  // Handle execution
  const handleExecute = async () => {
    const validationErrors = validateParameters();
    if (validationErrors.length > 0) {
      validationErrors.forEach(msg => addLog('error', msg));
      return;
    }

    const executionTargets = usesTaskSelector ? resolvedTargets.map((target) => target.id) : selectedTargets;

    if (executionTargets.length === 0) {
      addLog('error', 'Please select at least one target');
      return;
    }

    setIsExecuting(true);
    setResult(null);
    setLogs([]);
    setSelectedResult(null);

    addLog('info', `Starting batch execution for ${executionTargets.length} targets`);

    try {
      const executionResult = await onExecute(
        task.id,
        usesTaskSelector ? [] : executionTargets,
        parameters,
      );
      setResult(executionResult);

      if (executionResult.status === 'completed') {
        addLog('info', `Execution completed successfully`);
      } else if (executionResult.status === 'partial_failure') {
        addLog('warning', `Execution completed with ${executionResult.failureCount} failures`);
      } else if (executionResult.status === 'failed') {
        addLog('error', `Execution failed entirely`);
      }

      // Add logs for each result
      executionResult.results.forEach(r => {
        if (r.status === 'success') {
          addLog('info', `Target ${r.targetName} succeeded`, r.targetId, r.targetName);
        } else if (r.status === 'failed') {
          addLog('error', `Target ${r.targetName} failed: ${r.error}`, r.targetId, r.targetName);
        } else if (r.status === 'timeout') {
          addLog('warning', `Target ${r.targetName} timed out`, r.targetId, r.targetName);
        }
      });
    } catch (err) {
      addLog('error', `Execution error: ${formatExecutionError(err)}`);
    } finally {
      setIsExecuting(false);
    }
  };

  // Handle pause/resume
  const handlePause = () => {
    setIsPaused(true);
    onPause?.();
    addLog('warning', 'Execution paused by user');
  };

  const handleResume = () => {
    setIsPaused(false);
    onResume?.();
    addLog('info', 'Execution resumed');
  };

  // Handle cancel
  const handleCancel = () => {
    setIsExecuting(false);
    setIsPaused(false);
    onCancel?.();
    addLog('warning', 'Execution cancelled by user');
  };

  // Handle close
  const handleClose = () => {
    if (!isExecuting) {
      onCancel?.();
    }
  };

  const handleBackToSetup = () => {
    if (isExecuting) {
      return;
    }
    setResult(null);
    setSelectedResult(null);
    setLogs([]);
    setIsPaused(false);
  };

  return (
    <div className="batch-task-runner modal-overlay">
      <div className="modal-content runner-modal">
        <div className="runner-header">
          <div className="header-info">
            <h2>Execute Batch Task</h2>
            <span className="task-type-badge">{task.taskType}</span>
          </div>
          <button className="btn-close" onClick={handleClose} disabled={isExecuting}>×</button>
        </div>

        <div className="runner-body">
          <div className="config-panel">
            <div className="config-section task-info-section">
              <div className="task-name-row">
                <span className="task-type-icon">
                  {task.taskType === 'power_on' && '⚡'}
                  {task.taskType === 'shutdown' && '⏻'}
                  {task.taskType === 'start_app' && '🚀'}
                  {task.taskType === 'command' && '💻'}
                  {task.taskType === 'watch_processes' && '👁'}
                  {task.taskType === 'script' && '📜'}
                  {task.taskType === 'reboot' && '🔄'}
                </span>
                <h3>{task.name}</h3>
              </div>
              <p className="task-description">{task.description || 'No description'}</p>
              <div className="task-policy-summary">
                <span>Mode: {task.executionPolicy.mode}</span>
                <span>Timeout: {task.executionPolicy.timeoutSecs}s</span>
                <span>Threshold: {task.executionPolicy.failureThresholdPercent}%</span>
              </div>
            </div>

            {!result && (
              <div className="config-section target-selection-section">
                <h3>{usesTaskSelector ? 'Resolved Targets' : 'Select Targets'}</h3>
                {usesTaskSelector ? (
                  <ResolvedTargetPreview
                    selector={task.targetSelector}
                    targets={resolvedTargets}
                  />
                ) : (
                  <TargetSelection
                    selectedTargets={selectedTargets}
                    availableTargets={targets}
                    onChange={setSelectedTargets}
                    disabled={isExecuting || readOnly}
                  />
                )}
              </div>
            )}

            {task.parameters && task.parameters.length > 0 && !result && (
              <div className="config-section parameters-section">
                <h3>Parameters</h3>
                <div className="parameters-form">
                  {task.parameters.map(param => (
                    <ParameterInput
                      key={param.name}
                      parameter={param}
                      value={parameters[param.name] || ''}
                      onChange={(value) => setParameters(prev => ({ ...prev, [param.name]: value }))}
                      disabled={isExecuting || readOnly}
                    />
                  ))}
                </div>
              </div>
            )}
          </div>

          <div className="execution-panel">
            {(!isExecuting && !result) || isExecuting || isPaused || (result && !isExecuting) ? (
              <div className="execution-controls">
                {!isExecuting && !result && (
                  <>
                    <button
                      type="button"
                      className="btn-cancel"
                      onClick={handleClose}
                    >
                      Cancel
                    </button>
                    <button
                      type="button"
                      className="btn-runner-execute"
                      onClick={handleExecute}
                      disabled={(usesTaskSelector ? resolvedTargets.length === 0 : selectedTargets.length === 0) || readOnly}
                    >
                      ▶ Start Execution
                    </button>
                  </>
                )}

                {isExecuting && !isPaused && (
                  <>
                    <button
                      type="button"
                      className="btn-pause"
                      onClick={handlePause}
                    >
                      ⏸ Pause
                    </button>
                    <button
                      type="button"
                      className="btn-cancel"
                      onClick={handleCancel}
                    >
                      ⏹ Cancel
                    </button>
                  </>
                )}

                {isPaused && (
                  <>
                    <button
                      type="button"
                      className="btn-resume"
                      onClick={handleResume}
                    >
                      ▶ Resume
                    </button>
                    <button
                      type="button"
                      className="btn-cancel"
                      onClick={handleCancel}
                    >
                      ⏹ Cancel
                    </button>
                  </>
                )}

                {result && !isExecuting && (
                  <button
                    type="button"
                    className="btn-back"
                    onClick={handleBackToSetup}
                  >
                    ← Back
                  </button>
                )}
              </div>
            ) : null}

            {(isExecuting || isPaused || result) && (
              <div className="progress-section">
                <h3>Execution Progress</h3>
                <ProgressDisplay
                  result={result}
                  isRunning={isExecuting}
                  mode={task.executionPolicy.mode}
                />
              </div>
            )}

            {result && result.results.length > 0 && (
              <div className="results-section">
                <h3>Results</h3>
                <ResultsTable
                  results={result.results}
                  onViewDetails={setSelectedResult}
                />
              </div>
            )}

            <div className="logs-section">
              <h3>Execution Logs</h3>
              <ExecutionLog logs={logs} />
            </div>
          </div>
        </div>

        <style>{`
          .batch-task-runner {
            display: flex;
            flex-direction: column;
            height: 100%;
            background: var(--bg-card);
            color: var(--text-main);
          }

          .runner-modal {
            display: flex;
            flex-direction: column;
            height: 100%;
            background: var(--bg-card);
            color: var(--text-main);
          }

          .runner-header {
            position: sticky;
            top: 0;
            z-index: 20;
            display: flex;
            justify-content: space-between;
            align-items: center;
            padding: 14px 18px;
            background: var(--bg-card);
            border-bottom: 1px solid var(--border-color);
            box-shadow: 0 10px 20px -18px rgba(11, 25, 44, 0.42);
          }

          .header-info {
            display: flex;
            align-items: center;
            gap: 12px;
          }

          .header-info h2 {
            margin: 0;
            font-size: 1rem;
            color: var(--text-main);
          }

          .task-type-badge {
            background: rgba(45, 140, 240, 0.1);
            border: 1px solid rgba(45, 140, 240, 0.22);
            color: #1f4e89;
            padding: 4px 10px;
            border-radius: 999px;
            font-size: 0.72rem;
            font-weight: 600;
            text-transform: capitalize;
          }

          .btn-close {
            width: 36px;
            height: 36px;
            background: transparent;
            border: 1px solid var(--border-color);
            border-radius: 8px;
            color: var(--text-muted);
            font-size: 20px;
            cursor: pointer;
            display: flex;
            align-items: center;
            justify-content: center;
          }

          .btn-close:hover:not(:disabled) {
            background: rgba(45, 140, 240, 0.08);
            color: var(--text-main);
          }

          .btn-close:disabled {
            opacity: 0.5;
            cursor: not-allowed;
          }

          .runner-body {
            flex: 1;
            display: flex;
            overflow: hidden;
          }

          .config-panel {
            width: 420px;
            border-right: 1px solid var(--border-color);
            padding: 16px;
            overflow-y: auto;
            background: linear-gradient(180deg, rgba(244, 247, 249, 0.62) 0%, rgba(244, 247, 249, 0.95) 100%);
          }

          .execution-panel {
            flex: 1;
            display: flex;
            flex-direction: column;
            overflow: hidden;
            padding: 16px;
            background: var(--bg-card);
          }

          .config-section,
          .progress-section,
          .results-section {
            margin-bottom: 14px;
            background: var(--bg-card);
            border: 1px solid var(--border-color);
            border-radius: 12px;
            padding: 12px;
            box-shadow: 0 8px 20px rgba(11, 25, 44, 0.04);
          }

          .config-section h3,
          .progress-section h3,
          .results-section h3,
          .logs-section h3 {
            margin: 0 0 10px;
            font-size: 0.78rem;
            letter-spacing: 0.04em;
            text-transform: uppercase;
            color: var(--text-muted);
          }

          .task-info-section {
            background: var(--bg-card);
          }

          .task-name-row {
            display: flex;
            align-items: center;
            gap: 10px;
            margin-bottom: 8px;
          }

          .task-name-row h3 {
            margin: 0;
            font-size: 1rem;
            color: var(--text-main);
            text-transform: none;
            letter-spacing: normal;
          }

          .task-type-icon {
            font-size: 1.1rem;
            line-height: 1;
          }

          .task-description {
            margin: 0 0 12px;
            color: var(--text-muted);
            line-height: 1.5;
            font-size: 0.88rem;
          }

          .task-policy-summary {
            display: flex;
            flex-wrap: wrap;
            gap: 8px;
          }

          .task-policy-summary span {
            padding: 5px 10px;
            border-radius: 999px;
            background: rgba(45, 140, 240, 0.08);
            border: 1px solid rgba(45, 140, 240, 0.12);
            color: #1f4e89;
            font-size: 0.76rem;
          }

          .target-selection,
          .resolved-target-preview,
          .progress-display {
            background: var(--bg-main);
            border: 1px solid var(--border-color);
            border-radius: 10px;
            overflow: hidden;
          }

          .target-search,
          .target-select-all {
            padding: 10px;
            border-bottom: 1px solid var(--border-color);
          }

          .target-search input,
          .param-input,
          .param-select,
          .results-toolbar select {
            width: 100%;
            padding: 10px 12px;
            background: #fff;
            border: 1px solid var(--border-color);
            border-radius: 8px;
            color: var(--text-main);
            font-size: 0.86rem;
          }

          .target-search input:focus,
          .param-input:focus,
          .param-select:focus,
          .results-toolbar select:focus {
            outline: none;
            border-color: var(--primary);
          }

          .target-select-all label,
          .target-item {
            display: flex;
            align-items: center;
            gap: 8px;
            font-size: 0.84rem;
            color: var(--text-main);
          }

          .target-item {
            padding: 10px;
            cursor: pointer;
            border-bottom: 1px solid rgba(130, 162, 202, 0.12);
          }

          .target-item:hover {
            background: rgba(45, 140, 240, 0.08);
          }

          .target-list {
            max-height: 220px;
            overflow-y: auto;
          }

          .selector-summary {
            padding: 10px 12px;
            border-bottom: 1px solid var(--border-color);
            font-size: 0.8rem;
            color: var(--text-muted);
            line-height: 1.5;
          }

          .resolved-target-list {
            max-height: 220px;
            overflow-y: auto;
          }

          .resolved-target-item {
            display: flex;
            align-items: center;
            gap: 8px;
            padding: 10px;
            border-bottom: 1px solid rgba(130, 162, 202, 0.12);
            font-size: 0.84rem;
            color: var(--text-main);
          }

          .target-group,
          .target-count,
          .param-description-text,
          .log-empty,
          .results-count,
          .failure-rate,
          .progress-percentage {
            color: var(--text-muted);
          }

          .target-count {
            padding: 8px 10px;
            text-align: center;
            font-size: 0.78rem;
            border-top: 1px solid var(--border-color);
          }

          .target-empty {
            padding: 20px;
            text-align: center;
            color: var(--text-muted);
            font-size: 0.82rem;
          }

          .parameters-form {
            display: flex;
            flex-direction: column;
            gap: 14px;
          }

          .parameter-input {
            display: flex;
            flex-direction: column;
            gap: 6px;
          }

          .param-label-row {
            display: flex;
            justify-content: space-between;
            align-items: center;
            gap: 12px;
          }

          .param-label {
            font-size: 0.84rem;
            font-weight: 500;
            color: var(--text-main);
          }

          .param-description-text {
            font-size: 0.72rem;
          }

          .required,
          .error-preview {
            color: #ef4444;
          }

          .param-error {
            font-size: 0.75rem;
            color: #ef4444;
          }

          .execution-controls {
            position: sticky;
            top: 0;
            z-index: 10;
            display: flex;
            align-items: center;
            gap: 12px;
            padding: 10px 12px;
            margin-bottom: 12px;
            border: 1px solid var(--border-color);
            border-radius: 10px;
            background: var(--bg-card);
          }

          .btn-runner-execute,
          .btn-cancel,
          .btn-back,
          .btn-pause,
          .btn-resume,
          .btn-close-runner {
            min-height: 40px;
            padding: 10px 16px;
            border-radius: 10px;
            font-size: 0.84rem;
            font-weight: 600;
            cursor: pointer;
            transition: all 0.2s;
            display: inline-flex;
            align-items: center;
            justify-content: center;
            gap: 8px;
            white-space: nowrap;
          }

          .btn-runner-execute {
            background: #22c55e;
            border: 1px solid #22c55e;
            color: white;
          }

          .btn-runner-execute:hover:not(:disabled) {
            background: #16a34a;
            border-color: #16a34a;
          }

          .btn-runner-execute:disabled {
            opacity: 0.5;
            cursor: not-allowed;
          }

          .btn-cancel,
          .btn-back,
          .btn-close-runner {
            background: var(--bg-card);
            border: 1px solid var(--border-color);
            color: var(--text-muted);
          }

          .btn-cancel:hover,
          .btn-back:hover,
          .btn-close-runner:hover {
            border-color: var(--primary);
            color: var(--text-main);
          }

          .btn-pause {
            background: #f59e0b;
            border: 1px solid #f59e0b;
            color: white;
          }

          .btn-pause:hover {
            background: #d97706;
            border-color: #d97706;
          }

          .btn-resume {
            background: #3b82f6;
            border: 1px solid #3b82f6;
            color: white;
          }

          .btn-resume:hover {
            background: #2563eb;
            border-color: #2563eb;
          }

          .progress-display {
            padding: 14px;
          }

          .progress-header {
            display: flex;
            justify-content: space-between;
            align-items: center;
            gap: 12px;
            margin-bottom: 12px;
          }

          .progress-info,
          .progress-stats {
            display: flex;
            align-items: center;
            gap: 10px;
            flex-wrap: wrap;
            font-size: 0.82rem;
          }

          .running-indicator {
            color: #22c55e;
            font-weight: 600;
          }

          .progress-bar-container {
            height: 10px;
            border-radius: 999px;
            background: rgba(45, 140, 240, 0.1);
            overflow: hidden;
            margin-bottom: 8px;
          }

          .progress-bar {
            height: 100%;
            background: linear-gradient(90deg, #2d8cf0 0%, #44b2ff 100%);
            border-radius: 999px;
            transition: width 0.2s ease;
          }

          .circuit-breaker-warning {
            margin-top: 10px;
            padding: 10px 12px;
            border-radius: 8px;
            background: rgba(245, 158, 11, 0.12);
            color: #b45309;
            font-size: 0.8rem;
          }

          .results-section {
            flex: 1;
            display: flex;
            flex-direction: column;
            overflow: hidden;
          }

          .results-table-container {
            display: flex;
            flex-direction: column;
            min-height: 0;
          }

          .results-toolbar {
            display: flex;
            justify-content: space-between;
            align-items: center;
            gap: 12px;
            margin-bottom: 10px;
            font-size: 0.8rem;
          }

          .results-toolbar label {
            display: flex;
            align-items: center;
            gap: 8px;
            color: var(--text-muted);
          }

          .results-table-wrapper {
            overflow: auto;
            border: 1px solid var(--border-color);
            border-radius: 10px;
            background: var(--bg-main);
          }

          .results-table {
            width: 100%;
            border-collapse: collapse;
            font-size: 0.82rem;
          }

          .results-table th,
          .results-table td {
            padding: 10px 12px;
            text-align: left;
            border-bottom: 1px solid rgba(130, 162, 202, 0.12);
            vertical-align: top;
          }

          .results-table th {
            position: sticky;
            top: 0;
            background: var(--bg-card);
            color: var(--text-muted);
            font-size: 0.75rem;
            text-transform: uppercase;
            letter-spacing: 0.04em;
          }

          .results-table th.sortable {
            cursor: pointer;
          }

          .status-badge,
          .retry-badge {
            display: inline-flex;
            align-items: center;
            padding: 3px 8px;
            border-radius: 999px;
            font-size: 0.72rem;
            font-weight: 600;
          }

          .retry-badge {
            margin-left: 6px;
            background: rgba(59, 130, 246, 0.1);
            color: #1d4ed8;
          }

          .output-preview,
          .no-output,
          .cell-duration,
          .target-id {
            color: var(--text-muted);
          }

          .btn-view-details {
            width: 32px;
            height: 32px;
            padding: 0;
            border-radius: 8px;
            border: 1px solid var(--border-color);
            background: var(--bg-card);
            color: var(--text-muted);
            cursor: pointer;
            display: inline-flex;
            align-items: center;
            justify-content: center;
            line-height: 1;
            font-size: 14px;
          }

          .btn-view-details:hover {
            border-color: var(--primary);
            color: var(--text-main);
          }

          .result-details-overlay {
            position: fixed;
            inset: 0;
            background: rgba(11, 25, 44, 0.42);
            backdrop-filter: blur(2px);
            display: flex;
            align-items: center;
            justify-content: center;
            z-index: 1300;
            padding: 24px;
          }

          .result-details-modal {
            width: min(900px, 94vw);
            max-height: 86vh;
            display: flex;
            flex-direction: column;
            background: var(--bg-card);
            border: 1px solid var(--border-color);
            border-radius: 14px;
            box-shadow: 0 22px 56px rgba(8, 20, 38, 0.24);
            overflow: hidden;
          }

          .result-details-header {
            display: flex;
            align-items: center;
            justify-content: space-between;
            padding: 14px 18px;
            border-bottom: 1px solid var(--border-color);
          }

          .result-details-header h3 {
            margin: 0;
            font-size: 1rem;
            color: var(--text-main);
          }

          .result-details-body {
            display: grid;
            gap: 14px;
            padding: 16px;
            overflow: auto;
          }

          .detail-card {
            background: var(--bg-main);
            border: 1px solid var(--border-color);
            border-radius: 12px;
            padding: 14px;
          }

          .detail-card h4 {
            margin: 0 0 12px;
            font-size: 0.82rem;
            text-transform: uppercase;
            letter-spacing: 0.04em;
            color: var(--text-muted);
          }

          .detail-card-header {
            display: flex;
            align-items: center;
            justify-content: space-between;
            gap: 12px;
            margin-bottom: 12px;
          }

          .detail-card-header h4 {
            margin: 0;
          }

          .detail-row {
            display: grid;
            grid-template-columns: 120px minmax(0, 1fr);
            gap: 12px;
            padding: 6px 0;
            align-items: start;
          }

          .detail-label {
            color: var(--text-muted);
            font-size: 0.8rem;
          }

          .detail-value {
            color: var(--text-main);
            font-size: 0.84rem;
            word-break: break-word;
          }

          .detail-code {
            font-family: 'Monaco', 'Menlo', 'Ubuntu Mono', monospace;
          }

          .detail-pre {
            margin: 0;
            padding: 12px;
            border-radius: 10px;
            background: #101b2f;
            color: #d7e2f1;
            overflow: auto;
            white-space: pre-wrap;
            word-break: break-word;
            font-family: 'Monaco', 'Menlo', 'Ubuntu Mono', monospace;
            font-size: 12px;
            line-height: 1.5;
          }

          .detail-pre-error {
            background: rgba(239, 68, 68, 0.08);
            color: #b91c1c;
          }

          .logs-section {
            height: 230px;
            display: flex;
            flex-direction: column;
          }

          .execution-log {
            flex: 1;
            background: #0f1728;
            border: 1px solid #213553;
            border-radius: 10px;
            overflow-y: auto;
            padding: 12px;
            font-family: 'Monaco', 'Menlo', 'Ubuntu Mono', monospace;
            font-size: 12px;
          }

          .log-entry {
            display: flex;
            gap: 8px;
            padding: 4px 0;
            border-bottom: 1px solid rgba(130, 162, 202, 0.15);
          }

          .log-timestamp,
          .log-level-badge,
          .log-target {
            flex-shrink: 0;
          }

          .log-timestamp {
            color: #6b7280;
          }

          .log-target {
            color: #9ca3af;
          }

          .log-message {
            color: #d7e2f1;
            word-break: break-word;
          }

          .log-entry.log-error .log-message {
            color: #f87171;
          }

          .log-entry.log-warning .log-message {
            color: #fbbf24;
          }

          @media (max-width: 1100px) {
            .runner-body {
              flex-direction: column;
            }

            .config-panel {
              width: 100%;
              border-right: none;
              border-bottom: 1px solid var(--border-color);
              max-height: 48vh;
            }

            .execution-panel {
              padding-top: 12px;
            }

            .execution-controls,
            .progress-header,
            .results-toolbar {
              flex-wrap: wrap;
            }

            .detail-row {
              grid-template-columns: 1fr;
              gap: 4px;
            }
          }
        `}</style>

        {selectedResult && (
          <ResultDetailsModal
            result={selectedResult}
            onClose={() => setSelectedResult(null)}
          />
        )}
      </div>
    </div>
  );
};
