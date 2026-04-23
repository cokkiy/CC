/**
 * Batch Task Runner Component
 * Interface for executing batch tasks and monitoring progress
 * Part of Phase 7: Batch Operations Support System
 */

import React, { useState, useCallback, useEffect, useRef } from 'react';
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
  onViewDetails?: (targetId: string) => void;
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
                    onClick={() => onViewDetails?.(result.targetId)}
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

    if (selectedTargets.length === 0) {
      addLog('error', 'Please select at least one target');
      return;
    }

    setIsExecuting(true);
    setResult(null);
    setLogs([]);

    addLog('info', `Starting batch execution for ${selectedTargets.length} targets`);

    try {
      const executionResult = await onExecute(task.id, selectedTargets, parameters);
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
      addLog('error', `Execution error: ${err instanceof Error ? err.message : 'Unknown error'}`);
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

  return (
    <div className="batch-task-runner modal-overlay">
      <div className="modal-content runner-modal">
        <div className="runner-header">
          <h2>Execute Batch Task</h2>
          <button className="btn-close" onClick={handleClose} disabled={isExecuting}>×</button>
        </div>

        <div className="runner-body">
          {/* Task Info */}
          <div className="task-info-section">
            <div className="task-name-row">
              <span className="task-type-icon">
                {task.taskType === 'command' && '💻'}
                {task.taskType === 'script' && '📜'}
                {task.taskType === 'config' && '⚙️'}
                {task.taskType === 'file_transfer' && '📁'}
                {task.taskType === 'upgrade' && '⬆️'}
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

          {/* Target Selection */}
          {!result && (
            <div className="target-selection-section">
              <h3>Select Targets</h3>
              <TargetSelection
                selectedTargets={selectedTargets}
                availableTargets={targets}
                onChange={setSelectedTargets}
                disabled={isExecuting || readOnly}
              />
            </div>
          )}

          {/* Parameters */}
          {task.parameters && task.parameters.length > 0 && !result && (
            <div className="parameters-section">
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

          {/* Progress Display */}
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

          {/* Results Table */}
          {result && result.results.length > 0 && (
            <div className="results-section">
              <h3>Results</h3>
              <ResultsTable results={result.results} />
            </div>
          )}

          {/* Execution Logs */}
          <div className="logs-section">
            <h3>Execution Logs</h3>
            <ExecutionLog logs={logs} />
          </div>
        </div>

        <div className="runner-footer">
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
                className="btn-execute"
                onClick={handleExecute}
                disabled={selectedTargets.length === 0 || readOnly}
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
              className="btn-close-runner"
              onClick={handleClose}
            >
              Close
            </button>
          )}
        </div>
      </div>
    </div>
  );
};
