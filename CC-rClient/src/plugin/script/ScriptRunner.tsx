/**
 * Script Runner Component
 * Interface for executing scripts on target stations/devices
 * Part of Phase 6: Command Script Management System
 */

import React, { useState, useCallback, useEffect, useRef } from 'react';
import type { 
  CommandScript, 
  ScriptParameter, 
  ScriptExecutionContext,
  ScriptExecutionResult,
  ScriptExecutionStatus,
  ScriptLogEntry
} from './types';

// ============================================
// Parameter Input Component
// ============================================

interface ParameterInputProps {
  parameter: ScriptParameter;
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

    if (parameter.paramType === 'boolean') {
      if (!['true', 'false', '0', '1'].includes(val.toLowerCase())) {
        return 'Please enter true/false or 0/1';
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
  logs: ScriptLogEntry[];
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

  return (
    <div className="execution-log" onScroll={handleScroll} ref={logRef}>
      {displayLogs.length === 0 ? (
        <div className="log-empty">No logs yet</div>
      ) : (
        displayLogs.map((log, i) => (
          <div key={i} className={`log-entry log-${log.level}`}>
            <span className="log-timestamp">{formatTimestamp(log.timestamp)}</span>
            <span className={`log-level-badge ${log.level}`}>{log.level.toUpperCase()}</span>
            {log.stationId && <span className="log-station">[{log.stationId}]</span>}
            <span className="log-message">{log.message}</span>
          </div>
        ))
      )}
    </div>
  );
};

// ============================================
// Target Selector Component
// ============================================

interface TargetSelectorProps {
  selectedTargets: string[];
  allTargets: { id: string; name: string; status?: string }[];
  onChange: (targets: string[]) => void;
  mode?: 'single' | 'multiple';
  disabled?: boolean;
}

const TargetSelector: React.FC<TargetSelectorProps> = ({
  selectedTargets,
  allTargets,
  onChange,
  mode = 'multiple',
  disabled = false,
}) => {
  const [searchQuery, setSearchQuery] = useState('');
  const [selectAll, setSelectAll] = useState(selectedTargets.length === allTargets.length);

  const filteredTargets = allTargets.filter(t => 
    t.name.toLowerCase().includes(searchQuery.toLowerCase()) ||
    t.id.toLowerCase().includes(searchQuery.toLowerCase())
  );

  const handleToggle = (targetId: string) => {
    if (mode === 'single') {
      onChange([targetId]);
    } else {
      if (selectedTargets.includes(targetId)) {
        onChange(selectedTargets.filter(id => id !== targetId));
      } else {
        onChange([...selectedTargets, targetId]);
      }
    }
  };

  const handleSelectAll = () => {
    if (selectAll) {
      onChange([]);
    } else {
      onChange(allTargets.map(t => t.id));
    }
    setSelectAll(!selectAll);
  };

  return (
    <div className="target-selector">
      <div className="target-search">
        <input
          type="text"
          placeholder="Search targets..."
          value={searchQuery}
          onChange={(e) => setSearchQuery(e.target.value)}
          disabled={disabled}
        />
      </div>
      
      {mode === 'multiple' && (
        <div className="target-select-all">
          <label>
            <input
              type="checkbox"
              checked={selectAll}
              onChange={handleSelectAll}
              disabled={disabled}
            />
            <span>Select All ({allTargets.length})</span>
          </label>
        </div>
      )}
      
      <div className="target-list">
        {filteredTargets.length === 0 ? (
          <div className="target-empty">No matching targets</div>
        ) : (
          filteredTargets.map(target => (
            <label key={target.id} className="target-item">
              <input
                type={mode === 'single' ? 'radio' : 'checkbox'}
                name={mode === 'single' ? 'target' : undefined}
                checked={selectedTargets.includes(target.id)}
                onChange={() => handleToggle(target.id)}
                disabled={disabled}
              />
              <span className="target-name">{target.name}</span>
              {target.status && (
                <span className={`target-status status-${target.status}`}>
                  {target.status}
                </span>
              )}
              <span className="target-id">{target.id}</span>
            </label>
          ))
        )}
      </div>
      
      <div className="target-count">
        {selectedTargets.length} of {allTargets.length} selected
      </div>
    </div>
  );
};

// ============================================
// Execution Result Component
// ============================================

interface ExecutionResultProps {
  result: ScriptExecutionResult;
  onClose?: () => void;
}

const ExecutionResult: React.FC<ExecutionResultProps> = ({ result, onClose }) => {
  const getStatusColor = (status: ScriptExecutionStatus) => {
    switch (status) {
      case 'success': return '#22c55e';
      case 'failed': return '#ef4444';
      case 'running': return '#3b82f6';
      case 'pending': return '#f59e0b';
      case 'timeout': return '#f97316';
      case 'cancelled': return '#6b7280';
      default: return '#6b7280';
    }
  };

  const formatDuration = (ms?: number) => {
    if (!ms) return '-';
    if (ms < 1000) return `${ms}ms`;
    if (ms < 60000) return `${(ms / 1000).toFixed(1)}s`;
    return `${(ms / 60000).toFixed(1)}m`;
  };

  return (
    <div className="execution-result">
      <div className="result-header">
        <div className="result-status" style={{ color: getStatusColor(result.status) }}>
          <span className={`status-indicator ${result.status}`}></span>
          {result.status.charAt(0).toUpperCase() + result.status.slice(1)}
        </div>
        {result.durationMs !== undefined && (
          <span className="result-duration">Duration: {formatDuration(result.durationMs)}</span>
        )}
      </div>

      {result.error && (
        <div className="result-error">
          <strong>Error:</strong> {result.error}
        </div>
      )}

      <div className="result-output">
        <div className="output-section">
          <div className="output-header">
            <h4>Standard Output</h4>
            {result.stdout && (
              <button 
                className="btn-copy"
                onClick={() => navigator.clipboard.writeText(result.stdout || '')}
              >
                📋 Copy
              </button>
            )}
          </div>
          <pre className="output-content stdout">{result.stdout || '(empty)'}</pre>
        </div>

        <div className="output-section">
          <div className="output-header">
            <h4>Standard Error</h4>
            {result.stderr && (
              <button 
                className="btn-copy"
                onClick={() => navigator.clipboard.writeText(result.stderr || '')}
              >
                📋 Copy
              </button>
            )}
          </div>
          <pre className="output-content stderr">{result.stderr || '(empty)'}</pre>
        </div>
      </div>

      <div className="result-footer">
        <span className="exit-code">
          Exit Code: {result.exitCode ?? 'N/A'}
        </span>
        <span className="result-time">
          Started: {new Date(result.startedAt).toLocaleString()}
          {result.completedAt && (
            <> | Completed: {new Date(result.completedAt).toLocaleString()}</>
          )}
        </span>
      </div>

      {onClose && (
        <button className="btn-close-result" onClick={onClose}>
          Close
        </button>
      )}
    </div>
  );
};

// ============================================
// Main Script Runner Component
// ============================================

export interface ScriptRunnerProps {
  script: CommandScript;
  targets: { id: string; name: string; status?: string }[];
  onExecute: (context: ScriptExecutionContext) => Promise<ScriptExecutionResult[]>;
  onCancel?: () => void;
  defaultTargets?: string[];
  readOnly?: boolean;
}

export const ScriptRunner: React.FC<ScriptRunnerProps> = ({
  script,
  targets,
  onExecute,
  onCancel,
  defaultTargets = [],
  readOnly = false,
}) => {
  const [selectedTargets, setSelectedTargets] = useState<string[]>(defaultTargets);
  const [parameters, setParameters] = useState<Record<string, string>>(() => {
    const defaults: Record<string, string> = {};
    script.parameters.forEach(p => {
      if (p.defaultValue) defaults[p.name] = p.defaultValue;
    });
    return defaults;
  });
  const [timeout, setTimeout] = useState<number>(30000); // 30 seconds default
  const [continueOnError, setContinueOnError] = useState<boolean>(false);
  
  const [isExecuting, setIsExecuting] = useState(false);
  const [results, setResults] = useState<ScriptExecutionResult[]>([]);
  const [logs, setLogs] = useState<ScriptLogEntry[]>([]);
  const [currentExecution, setCurrentExecution] = useState<string | null>(null);
  
  const [showAdvanced, setShowAdvanced] = useState(false);
  const [activeResultIndex, setActiveResultIndex] = useState<number | null>(null);

  // Generate preview with substituted parameters
  const getPreviewContent = useCallback(() => {
    let content = script.content;
    Object.entries(parameters).forEach(([key, value]) => {
      content = content.replace(new RegExp(`\\{\\{${key}\\}\\}`, 'g'), value || '');
    });
    return content;
  }, [script.content, parameters]);

  // Validate all parameters
  const validateParameters = useCallback(() => {
    const errors: string[] = [];
    
    script.parameters.forEach(p => {
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
  }, [script.parameters, parameters]);

  // Handle execution
  const handleExecute = async () => {
    const validationErrors = validateParameters();
    if (validationErrors.length > 0) {
      setLogs(prev => [
        ...prev,
        ...validationErrors.map(msg => ({
          timestamp: new Date().toISOString(),
          level: 'error' as const,
          message: msg,
        })),
      ]);
      return;
    }

    if (selectedTargets.length === 0) {
      setLogs(prev => [
        ...prev,
        {
          timestamp: new Date().toISOString(),
          level: 'error',
          message: 'Please select at least one target',
        },
      ]);
      return;
    }

    setIsExecuting(true);
    setResults([]);
    setLogs([]);
    setActiveResultIndex(null);

    // Add execution start log
    setLogs(prev => [{
      timestamp: new Date().toISOString(),
      level: 'info',
      message: `Starting execution of "${script.name}" on ${selectedTargets.length} target(s)`,
    }]);

    const context: ScriptExecutionContext = {
      scriptId: script.id,
      stationIds: selectedTargets,
      parameters,
      timeout,
      continueOnError,
    };

    try {
      const executionResults = await onExecute(context);
      setResults(executionResults);

      // Process results
      executionResults.forEach((result, index) => {
        setLogs(prev => [
          ...prev,
          {
            timestamp: new Date().toISOString(),
            level: result.status === 'success' ? 'info' : 'error',
            message: `Target ${result.scriptId}: ${result.status.toUpperCase()}${
              result.stderr ? `: ${result.stderr.slice(0, 100)}` : ''
            }`,
            stationId: result.scriptId,
          },
        ]);
      });

      const successCount = executionResults.filter(r => r.status === 'success').length;
      const failCount = executionResults.filter(r => r.status === 'failed').length;

      setLogs(prev => [
        ...prev,
        {
          timestamp: new Date().toISOString(),
          level: failCount > 0 ? 'warning' : 'info',
          message: `Execution complete: ${successCount} succeeded, ${failCount} failed`,
        },
      ]);

      if (executionResults.length > 0) {
        setActiveResultIndex(0);
      }
    } catch (error) {
      setLogs(prev => [
        ...prev,
        {
          timestamp: new Date().toISOString(),
          level: 'error',
          message: `Execution failed: ${error instanceof Error ? error.message : 'Unknown error'}`,
        },
      ]);
    } finally {
      setIsExecuting(false);
    }
  };

  // Handle cancel
  const handleCancel = () => {
    if (currentExecution) {
      setLogs(prev => [
        ...prev,
        {
          timestamp: new Date().toISOString(),
          level: 'warning',
          message: 'Cancelling execution...',
        },
      ]);
      // The parent should handle the actual cancellation
      onCancel?.();
    }
  };

  // Handle parameter change
  const handleParameterChange = (name: string, value: string) => {
    setParameters(prev => ({ ...prev, [name]: value }));
  };

  // Detect parameter placeholders in script
  const parameterPlaceholders = script.content.match(/\{\{(\w+)\}\}/g) || [];
  const usedParameters = [...new Set(parameterPlaceholders.map(p => p.slice(2, -2)))];

  return (
    <div className="script-runner">
      {/* Header */}
      <div className="runner-header">
        <div className="header-info">
          <h2>Execute: {script.name}</h2>
          <span className="script-type-badge">{script.scriptType}</span>
        </div>
        <button className="btn-close" onClick={onCancel}>×</button>
      </div>

      <div className="runner-body">
        {/* Left Panel - Configuration */}
        <div className="config-panel">
          {/* Target Selection */}
          <div className="config-section">
            <h3>Target Stations</h3>
            <TargetSelector
              selectedTargets={selectedTargets}
              allTargets={targets}
              onChange={setSelectedTargets}
              mode="multiple"
              disabled={isExecuting || readOnly}
            />
          </div>

          {/* Parameters */}
          {script.parameters.length > 0 && (
            <div className="config-section">
              <h3>Parameters</h3>
              <div className="parameters-form">
                {script.parameters.map(param => (
                  <ParameterInput
                    key={param.name}
                    parameter={param}
                    value={parameters[param.name] || ''}
                    onChange={(val) => handleParameterChange(param.name, val)}
                    disabled={isExecuting || readOnly}
                  />
                ))}
              </div>
            </div>
          )}

          {/* Advanced Options */}
          <div className="config-section">
            <div 
              className="section-header collapsible"
              onClick={() => setShowAdvanced(!showAdvanced)}
            >
              <h3>Advanced Options</h3>
              <span className={`toggle-icon ${showAdvanced ? 'open' : ''}`}>▼</span>
            </div>
            
            {showAdvanced && (
              <div className="advanced-options">
                <div className="option-row">
                  <label className="option-label">
                    Timeout (ms)
                    <input
                      type="number"
                      className="option-input"
                      value={timeout}
                      onChange={(e) => setTimeout(Number(e.target.value))}
                      disabled={isExecuting || readOnly}
                      min={1000}
                      max={3600000}
                      step={1000}
                    />
                  </label>
                </div>
                
                <div className="option-row">
                  <label className="option-checkbox">
                    <input
                      type="checkbox"
                      checked={continueOnError}
                      onChange={(e) => setContinueOnError(e.target.checked)}
                      disabled={isExecuting || readOnly}
                    />
                    <span>Continue on error</span>
                  </label>
                  <span className="option-hint">
                    Continue executing on remaining targets if one fails
                  </span>
                </div>
              </div>
            )}
          </div>

          {/* Script Preview */}
          <div className="config-section">
            <h3>Script Preview</h3>
            <pre className="script-preview">{getPreviewContent()}</pre>
          </div>
        </div>

        {/* Right Panel - Execution */}
        <div className="execution-panel">
          {/* Execution Controls */}
          <div className="execution-controls">
            {!isExecuting ? (
              <button
                className="btn-execute"
                onClick={handleExecute}
                disabled={selectedTargets.length === 0 || readOnly}
              >
                ▶ Execute Script
              </button>
            ) : (
              <button
                className="btn-cancel-execution"
                onClick={handleCancel}
              >
                ⏹ Cancel Execution
              </button>
            )}
            
            {results.length > 0 && (
              <div className="execution-summary">
                <span className="summary-item success">
                  ✓ {results.filter(r => r.status === 'success').length} succeeded
                </span>
                <span className="summary-item failed">
                  ✗ {results.filter(r => r.status === 'failed').length} failed
                </span>
              </div>
            )}
          </div>

          {/* Results Tabs */}
          {results.length > 0 && (
            <div className="results-section">
              <h3>Results</h3>
              <div className="results-tabs">
                {results.map((result, index) => (
                  <button
                    key={index}
                    className={`result-tab ${activeResultIndex === index ? 'active' : ''} ${result.status}`}
                    onClick={() => setActiveResultIndex(index)}
                  >
                    <span className="tab-indicator"></span>
                    <span className="tab-label">{result.scriptId}</span>
                  </button>
                ))}
              </div>
              {activeResultIndex !== null && results[activeResultIndex] && (
                <ExecutionResult 
                  result={results[activeResultIndex]} 
                />
              )}
            </div>
          )}

          {/* Execution Log */}
          <div className="log-section">
            <h3>Execution Log</h3>
            <ExecutionLog logs={logs} />
          </div>
        </div>
      </div>

      {/* Embedded Styles */}
      <style>{`
        .script-runner {
          display: flex;
          flex-direction: column;
          height: 100%;
          background: var(--bg-primary, #1a1a1a);
          color: var(--text-primary, #e0e0e0);
        }

        .runner-header {
          display: flex;
          justify-content: space-between;
          align-items: center;
          padding: 16px 24px;
          background: var(--bg-secondary, #252525);
          border-bottom: 1px solid var(--border-color, #333);
        }

        .header-info {
          display: flex;
          align-items: center;
          gap: 12px;
        }

        .header-info h2 {
          margin: 0;
          font-size: 20px;
          font-weight: 600;
        }

        .script-type-badge {
          background: var(--accent-color, #3b82f6);
          color: white;
          padding: 4px 10px;
          border-radius: 4px;
          font-size: 12px;
          font-weight: 500;
        }

        .btn-close {
          width: 36px;
          height: 36px;
          background: transparent;
          border: 1px solid var(--border-color, #444);
          border-radius: 8px;
          color: var(--text-secondary, #a0a0a0);
          font-size: 20px;
          cursor: pointer;
          display: flex;
          align-items: center;
          justify-content: center;
        }

        .btn-close:hover {
          background: var(--bg-hover, #333);
          color: var(--text-primary, #e0e0e0);
        }

        .runner-body {
          flex: 1;
          display: flex;
          overflow: hidden;
        }

        .config-panel {
          width: 400px;
          border-right: 1px solid var(--border-color, #333);
          overflow-y: auto;
          padding: 20px;
        }

        .execution-panel {
          flex: 1;
          display: flex;
          flex-direction: column;
          overflow: hidden;
          padding: 20px;
        }

        .config-section {
          margin-bottom: 24px;
        }

        .config-section h3 {
          margin: 0 0 12px;
          font-size: 14px;
          font-weight: 600;
          color: var(--text-secondary, #a0a0a0);
          text-transform: uppercase;
          letter-spacing: 0.5px;
        }

        .section-header.collapsible {
          display: flex;
          justify-content: space-between;
          align-items: center;
          cursor: pointer;
          user-select: none;
        }

        .section-header.collapsible h3 {
          margin: 0;
        }

        .toggle-icon {
          font-size: 12px;
          color: var(--text-muted, #666);
          transition: transform 0.2s;
        }

        .toggle-icon.open {
          transform: rotate(180deg);
        }

        /* Target Selector */
        .target-selector {
          background: var(--bg-primary, #1a1a1a);
          border: 1px solid var(--border-color, #444);
          border-radius: 8px;
          overflow: hidden;
        }

        .target-search {
          padding: 10px;
          border-bottom: 1px solid var(--border-color, #444);
        }

        .target-search input {
          width: 100%;
          padding: 8px 10px;
          background: var(--bg-secondary, #252525);
          border: 1px solid var(--border-color, #444);
          border-radius: 4px;
          color: var(--text-primary, #e0e0e0);
          font-size: 13px;
        }

        .target-search input:focus {
          outline: none;
          border-color: var(--accent-color, #3b82f6);
        }

        .target-select-all {
          padding: 8px 10px;
          border-bottom: 1px solid var(--border-color, #444);
        }

        .target-select-all label {
          display: flex;
          align-items: center;
          gap: 8px;
          font-size: 13px;
          cursor: pointer;
        }

        .target-list {
          max-height: 200px;
          overflow-y: auto;
        }

        .target-item {
          display: flex;
          align-items: center;
          gap: 8px;
          padding: 8px 10px;
          cursor: pointer;
          font-size: 13px;
        }

        .target-item:hover {
          background: var(--bg-hover, #333);
        }

        .target-name {
          flex: 1;
        }

        .target-status {
          font-size: 10px;
          padding: 2px 6px;
          border-radius: 3px;
        }

        .target-status.status-online {
          background: rgba(34, 197, 94, 0.2);
          color: #22c55e;
        }

        .target-status.status-offline {
          background: rgba(107, 114, 128, 0.2);
          color: #6b7280;
        }

        .target-id {
          font-size: 11px;
          color: var(--text-muted, #666);
        }

        .target-empty {
          padding: 20px;
          text-align: center;
          color: var(--text-muted, #666);
          font-size: 13px;
        }

        .target-count {
          padding: 8px 10px;
          border-top: 1px solid var(--border-color, #444);
          font-size: 12px;
          color: var(--text-muted, #666);
          text-align: center;
        }

        /* Parameters Form */
        .parameters-form {
          display: flex;
          flex-direction: column;
          gap: 16px;
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
        }

        .param-label {
          font-size: 13px;
          font-weight: 500;
          color: var(--text-primary, #e0e0e0);
        }

        .param-label .required {
          color: #ef4444;
          margin-left: 2px;
        }

        .param-description-text {
          font-size: 11px;
          color: var(--text-muted, #666);
        }

        .param-input, .param-select {
          padding: 10px 12px;
          background: var(--bg-primary, #1a1a1a);
          border: 1px solid var(--border-color, #444);
          border-radius: 6px;
          color: var(--text-primary, #e0e0e0);
          font-size: 14px;
        }

        .param-input:focus, .param-select:focus {
          outline: none;
          border-color: var(--accent-color, #3b82f6);
        }

        .param-input.has-error, .param-select.has-error {
          border-color: #ef4444;
        }

        .param-error {
          font-size: 12px;
          color: #ef4444;
        }

        /* Advanced Options */
        .advanced-options {
          margin-top: 12px;
          padding: 12px;
          background: var(--bg-primary, #1a1a1a);
          border-radius: 6px;
        }

        .option-row {
          margin-bottom: 12px;
        }

        .option-row:last-child {
          margin-bottom: 0;
        }

        .option-label {
          display: flex;
          flex-direction: column;
          gap: 6px;
          font-size: 13px;
          color: var(--text-secondary, #a0a0a0);
        }

        .option-input {
          padding: 8px 10px;
          background: var(--bg-secondary, #252525);
          border: 1px solid var(--border-color, #444);
          border-radius: 4px;
          color: var(--text-primary, #e0e0e0);
          font-size: 13px;
        }

        .option-checkbox {
          display: flex;
          align-items: center;
          gap: 8px;
          font-size: 13px;
          cursor: pointer;
        }

        .option-hint {
          display: block;
          margin-top: 4px;
          font-size: 11px;
          color: var(--text-muted, #666);
        }

        /* Script Preview */
        .script-preview {
          margin: 0;
          padding: 12px;
          background: #1e1e1e;
          border-radius: 6px;
          font-family: 'Monaco', 'Menlo', 'Ubuntu Mono', monospace;
          font-size: 12px;
          line-height: 1.5;
          color: #d4d4d4;
          white-space: pre-wrap;
          word-wrap: break-word;
          max-height: 200px;
          overflow-y: auto;
        }

        /* Execution Controls */
        .execution-controls {
          display: flex;
          align-items: center;
          gap: 16px;
          margin-bottom: 20px;
        }

        .btn-execute, .btn-cancel-execution {
          padding: 12px 24px;
          border-radius: 8px;
          font-size: 14px;
          font-weight: 600;
          cursor: pointer;
          transition: all 0.2s;
          border: none;
        }

        .btn-execute {
          background: #22c55e;
          color: white;
        }

        .btn-execute:hover:not(:disabled) {
          background: #16a34a;
        }

        .btn-execute:disabled {
          opacity: 0.5;
          cursor: not-allowed;
        }

        .btn-cancel-execution {
          background: #ef4444;
          color: white;
        }

        .btn-cancel-execution:hover {
          background: #dc2626;
        }

        .execution-summary {
          display: flex;
          gap: 16px;
        }

        .summary-item {
          font-size: 13px;
          font-weight: 500;
        }

        .summary-item.success {
          color: #22c55e;
        }

        .summary-item.failed {
          color: #ef4444;
        }

        /* Results Section */
        .results-section {
          flex: 1;
          display: flex;
          flex-direction: column;
          overflow: hidden;
          margin-bottom: 20px;
        }

        .results-section h3 {
          margin: 0 0 12px;
          font-size: 14px;
          font-weight: 600;
          color: var(--text-secondary, #a0a0a0);
        }

        .results-tabs {
          display: flex;
          gap: 4px;
          margin-bottom: 12px;
          overflow-x: auto;
        }

        .result-tab {
          display: flex;
          align-items: center;
          gap: 6px;
          padding: 8px 12px;
          background: var(--bg-secondary, #252525);
          border: 1px solid var(--border-color, #444);
          border-radius: 6px;
          color: var(--text-secondary, #a0a0a0);
          font-size: 12px;
          cursor: pointer;
          white-space: nowrap;
        }

        .result-tab:hover {
          border-color: var(--accent-color, #3b82f6);
        }

        .result-tab.active {
          background: var(--bg-hover, #333);
          border-color: var(--accent-color, #3b82f6);
          color: var(--text-primary, #e0e0e0);
        }

        .tab-indicator {
          width: 8px;
          height: 8px;
          border-radius: 50%;
          background: #6b7280;
        }

        .result-tab.success .tab-indicator {
          background: #22c55e;
        }

        .result-tab.failed .tab-indicator {
          background: #ef4444;
        }

        .result-tab.running .tab-indicator {
          background: #3b82f6;
          animation: pulse 1s infinite;
        }

        @keyframes pulse {
          0%, 100% { opacity: 1; }
          50% { opacity: 0.5; }
        }

        /* Execution Result */
        .execution-result {
          flex: 1;
          display: flex;
          flex-direction: column;
          background: var(--bg-secondary, #252525);
          border: 1px solid var(--border-color, #444);
          border-radius: 8px;
          overflow: hidden;
        }

        .result-header {
          display: flex;
          justify-content: space-between;
          align-items: center;
          padding: 12px 16px;
          border-bottom: 1px solid var(--border-color, #444);
        }

        .result-status {
          display: flex;
          align-items: center;
          gap: 8px;
          font-size: 14px;
          font-weight: 600;
        }

        .status-indicator {
          width: 10px;
          height: 10px;
          border-radius: 50%;
          background: #6b7280;
        }

        .status-indicator.success { background: #22c55e; }
        .status-indicator.failed { background: #ef4444; }
        .status-indicator.running { 
          background: #3b82f6; 
          animation: pulse 1s infinite;
        }
        .status-indicator.pending { background: #f59e0b; }

        .result-duration {
          font-size: 12px;
          color: var(--text-muted, #666);
        }

        .result-error {
          padding: 12px 16px;
          background: rgba(239, 68, 68, 0.1);
          border-bottom: 1px solid #ef4444;
          color: #ef4444;
          font-size: 13px;
        }

        .result-output {
          flex: 1;
          display: flex;
          flex-direction: column;
          overflow: hidden;
        }

        .output-section {
          flex: 1;
          display: flex;
          flex-direction: column;
          overflow: hidden;
        }

        .output-section:first-child {
          border-bottom: 1px solid var(--border-color, #333);
        }

        .output-header {
          display: flex;
          justify-content: space-between;
          align-items: center;
          padding: 8px 16px;
          background: var(--bg-primary, #1a1a1a);
        }

        .output-header h4 {
          margin: 0;
          font-size: 12px;
          font-weight: 500;
          color: var(--text-secondary, #a0a0a0);
        }

        .btn-copy {
          padding: 4px 8px;
          background: transparent;
          border: 1px solid var(--border-color, #444);
          border-radius: 4px;
          color: var(--text-secondary, #a0a0a0);
          font-size: 11px;
          cursor: pointer;
        }

        .btn-copy:hover {
          border-color: var(--accent-color, #3b82f6);
        }

        .output-content {
          flex: 1;
          margin: 0;
          padding: 12px 16px;
          font-family: 'Monaco', 'Menlo', 'Ubuntu Mono', monospace;
          font-size: 12px;
          line-height: 1.5;
          overflow: auto;
          white-space: pre-wrap;
          word-wrap: break-word;
        }

        .output-content.stdout {
          color: #d4d4d4;
        }

        .output-content.stderr {
          color: #f87171;
          background: rgba(239, 68, 68, 0.05);
        }

        .result-footer {
          display: flex;
          justify-content: space-between;
          padding: 8px 16px;
          border-top: 1px solid var(--border-color, #444);
          font-size: 11px;
          color: var(--text-muted, #666);
        }

        .btn-close-result {
          padding: 8px 16px;
          background: transparent;
          border: 1px solid var(--border-color, #444);
          border-radius: 6px;
          color: var(--text-secondary, #a0a0a0);
          font-size: 13px;
          cursor: pointer;
          margin: 12px 16px;
        }

        .btn-close-result:hover {
          border-color: var(--accent-color, #3b82f6);
        }

        /* Log Section */
        .log-section {
          height: 250px;
          display: flex;
          flex-direction: column;
        }

        .log-section h3 {
          margin: 0 0 12px;
          font-size: 14px;
          font-weight: 600;
          color: var(--text-secondary, #a0a0a0);
        }

        .execution-log {
          flex: 1;
          background: #1e1e1e;
          border: 1px solid var(--border-color, #444);
          border-radius: 8px;
          overflow-y: auto;
          padding: 12px;
          font-family: 'Monaco', 'Menlo', 'Ubuntu Mono', monospace;
          font-size: 12px;
        }

        .log-empty {
          color: var(--text-muted, #666);
          text-align: center;
          padding: 20px;
        }

        .log-entry {
          display: flex;
          gap: 8px;
          padding: 4px 0;
          border-bottom: 1px solid rgba(255, 255, 255, 0.05);
        }

        .log-timestamp {
          color: #6b7280;
          flex-shrink: 0;
        }

        .log-level-badge {
          font-size: 10px;
          padding: 1px 4px;
          border-radius: 3px;
          flex-shrink: 0;
        }

        .log-level-badge.info {
          background: rgba(59, 130, 246, 0.2);
          color: #60a5fa;
        }

        .log-level-badge.warning {
          background: rgba(234, 179, 8, 0.2);
          color: #fbbf24;
        }

        .log-level-badge.error {
          background: rgba(239, 68, 68, 0.2);
          color: #f87171;
        }

        .log-station {
          color: #9ca3af;
          flex-shrink: 0;
        }

        .log-message {
          color: #d4d4d4;
          word-break: break-word;
        }

        .log-entry.log-error .log-message {
          color: #f87171;
        }

        .log-entry.log-warning .log-message {
          color: #fbbf24;
        }
      `}</style>
    </div>
  );
};

export default ScriptRunner;
