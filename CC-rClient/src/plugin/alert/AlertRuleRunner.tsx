/**
 * AlertRuleRunner - Modal for testing/executing Alert Rules
 * Part of Phase 9: Alert Rules Management System
 */

import React, { useState, useCallback } from 'react';
import type { AlertRule, AlertExecutionContext, AlertExecutionResult, AlertTargetResult } from './types';

export interface AlertRuleRunnerProps {
  rule: AlertRule;
  targets: Array<{ id: string; name: string; status?: string }>;
  onExecute: (context: AlertExecutionContext) => Promise<AlertExecutionResult>;
  onTest: (ruleId: string, targetIds: string[]) => Promise<void>;
  onCancel: () => void;
}

type RunMode = 'test' | 'live';
type RunStatus = 'idle' | 'running' | 'completed' | 'error';

export const AlertRuleRunner: React.FC<AlertRuleRunnerProps> = ({
  rule,
  targets,
  onExecute,
  onTest,
  onCancel,
}) => {
  // Run mode
  const [mode, setMode] = useState<RunMode>('test');

  // Selected targets for execution
  const [selectedTargetIds, setSelectedTargetIds] = useState<string[]>(() => {
    // Pre-select all if rule targets specific devices
    if (rule.targetSelector.selectorType === 'device_ids' && rule.targetSelector.deviceIds) {
      return rule.targetSelector.deviceIds;
    }
    // Otherwise select all available targets
    return targets.map((t) => t.id);
  });

  // Run status
  const [status, setStatus] = useState<RunStatus>('idle');
  const [error, setError] = useState<string | null>(null);

  // Execution result
  const [result, setResult] = useState<AlertExecutionResult | null>(null);

  // Toggle target selection
  const toggleTarget = (targetId: string) => {
    setSelectedTargetIds((prev) =>
      prev.includes(targetId)
        ? prev.filter((id) => id !== targetId)
        : [...prev, targetId]
    );
  };

  // Select all targets
  const selectAllTargets = () => {
    setSelectedTargetIds(targets.map((t) => t.id));
  };

  // Deselect all targets
  const deselectAllTargets = () => {
    setSelectedTargetIds([]);
  };

  // Handle test run
  const handleTest = useCallback(async () => {
    if (selectedTargetIds.length === 0) {
      setError('Please select at least one target');
      return;
    }

    setStatus('running');
    setError(null);
    setResult(null);

    try {
      await onTest(rule.id, selectedTargetIds);
      setStatus('completed');
      // Note: Test doesn't return detailed results, just success/failure
    } catch (err) {
      setStatus('error');
      setError(err instanceof Error ? err.message : 'Test execution failed');
    }
  }, [rule.id, selectedTargetIds, onTest]);

  // Handle live execution
  const handleExecute = useCallback(async () => {
    if (selectedTargetIds.length === 0) {
      setError('Please select at least one target');
      return;
    }

    setStatus('running');
    setError(null);
    setResult(null);

    try {
      const executionResult = await onExecute({
        ruleId: rule.id,
        targetIds: selectedTargetIds,
        dryRun: mode === 'test',
      });
      setResult(executionResult);
      setStatus('completed');
    } catch (err) {
      setStatus('error');
      setError(err instanceof Error ? err.message : 'Execution failed');
    }
  }, [rule.id, selectedTargetIds, mode, onExecute]);

  // Get triggered count from result
  const getTriggeredCount = (): number => {
    if (!result) return 0;
    return result.triggeredTargets.length;
  };

  // Format condition for display
  const formatCondition = (): string => {
    const conditions = rule.conditionGroup.conditions;
    if (conditions.length === 0) return 'No conditions';
    if (conditions.length === 1) {
      const c = conditions[0];
      return `${c.metricType} ${c.operator} ${c.threshold}${c.unit || '%'}`;
    }
    return `${conditions.length} conditions (${rule.conditionGroup.logic})`;
  };

  return (
    <div className="alert-rule-runner">
      <div className="runner-header">
        <div className="header-title">
          <h2>{mode === 'test' ? 'Test Alert Rule' : 'Execute Alert Rule'}</h2>
          <p className="rule-name">{rule.name}</p>
        </div>
        <button className="btn-close" onClick={onCancel}>×</button>
      </div>

      <div className="runner-content">
        {/* Mode Selection */}
        <div className="mode-selector">
          <button
            className={`mode-btn ${mode === 'test' ? 'active' : ''}`}
            onClick={() => setMode('test')}
          >
            🧪 Test Mode
          </button>
          <button
            className={`mode-btn ${mode === 'live' ? 'active' : ''}`}
            onClick={() => setMode('live')}
          >
            🚀 Live Execution
          </button>
        </div>

        {mode === 'test' && (
          <div className="info-box info-warning">
            <strong>Test Mode:</strong> This will evaluate conditions against selected targets
            without triggering any actions or notifications.
          </div>
        )}

        {mode === 'live' && (
          <div className="info-box info-danger">
            <strong>Live Execution:</strong> This will actually trigger the alert actions
            for matching targets. Use with caution!
          </div>
        )}

        {/* Rule Summary */}
        <section className="runner-section">
          <h3>Rule Summary</h3>
          <div className="summary-grid">
            <div className="summary-item">
              <span className="summary-label">Severity</span>
              <span className={`badge badge-${rule.severity}`}>{rule.severity}</span>
            </div>
            <div className="summary-item">
              <span className="summary-label">Condition</span>
              <span className="summary-value">{formatCondition()}</span>
            </div>
            <div className="summary-item">
              <span className="summary-label">Cooldown</span>
              <span className="summary-value">{rule.cooldownSecs}s</span>
            </div>
            <div className="summary-item">
              <span className="summary-label">Actions</span>
              <span className="summary-value">{rule.actions.length} action(s)</span>
            </div>
          </div>
        </section>

        {/* Target Selection */}
        <section className="runner-section">
          <div className="section-header">
            <h3>Select Targets ({selectedTargetIds.length} selected)</h3>
            <div className="target-actions">
              <button className="btn-link" onClick={selectAllTargets}>Select All</button>
              <button className="btn-link" onClick={deselectAllTargets}>Deselect All</button>
            </div>
          </div>

          <div className="target-grid">
            {targets.map((target) => (
              <label
                key={target.id}
                className={`target-card ${selectedTargetIds.includes(target.id) ? 'selected' : ''}`}
              >
                <input
                  type="checkbox"
                  checked={selectedTargetIds.includes(target.id)}
                  onChange={() => toggleTarget(target.id)}
                />
                <div className="target-info">
                  <span className="target-name">{target.name}</span>
                  <span className={`target-status status-${target.status || 'unknown'}`}>
                    {target.status || 'unknown'}
                  </span>
                </div>
              </label>
            ))}
          </div>

          {selectedTargetIds.length === 0 && (
            <p className="error-hint">Please select at least one target to run the alert.</p>
          )}
        </section>

        {/* Results Section */}
        {status === 'completed' && result && (
          <section className="runner-section results-section">
            <h3>Results</h3>
            <div className="results-summary">
              <div className={`result-card ${result.triggered ? 'triggered' : 'not-triggered'}`}>
                <div className="result-icon">
                  {result.triggered ? '⚠️' : '✅'}
                </div>
                <div className="result-info">
                  <span className="result-status">
                    {result.triggered ? 'ALERT TRIGGERED' : 'No Alert Triggered'}
                  </span>
                  <span className="result-detail">
                    {getTriggeredCount()} of {result.results.length} targets matched conditions
                  </span>
                </div>
              </div>
            </div>

            <div className="results-list">
              <h4>Target Results</h4>
              <table className="results-table">
                <thead>
                  <tr>
                    <th>Target</th>
                    <th>Status</th>
                    <th>Current Value</th>
                    <th>Threshold</th>
                    <th>Message</th>
                  </tr>
                </thead>
                <tbody>
                  {result.results.map((targetResult: AlertTargetResult) => (
                    <tr key={targetResult.targetId} className={targetResult.triggered ? 'triggered-row' : ''}>
                      <td>{targetResult.targetName}</td>
                      <td>
                        {targetResult.triggered ? (
                          <span className="badge badge-warning">Triggered</span>
                        ) : (
                          <span className="badge badge-success">OK</span>
                        )}
                      </td>
                      <td>{targetResult.currentValue}</td>
                      <td>{targetResult.threshold}</td>
                      <td className="message-cell">{targetResult.message || '-'}</td>
                    </tr>
                  ))}
                </tbody>
              </table>
            </div>
          </section>
        )}

        {status === 'error' && error && (
          <section className="runner-section">
            <div className="info-box info-danger">
              <strong>Error:</strong> {error}
            </div>
          </section>
        )}
      </div>

      <div className="runner-footer">
        <button className="btn btn-secondary" onClick={onCancel}>
          Close
        </button>
        {status === 'idle' || status === 'error' ? (
          <>
            <button
              className="btn btn-secondary"
              onClick={handleTest}
              disabled={selectedTargetIds.length === 0}
            >
              🧪 Test
            </button>
            <button
              className="btn btn-primary"
              onClick={handleExecute}
              disabled={selectedTargetIds.length === 0}
            >
              🚀 Execute
            </button>
          </>
        ) : status === 'running' ? (
          <button className="btn btn-primary" disabled>
            ⏳ Running...
          </button>
        ) : (
          <button className="btn btn-primary" onClick={() => setStatus('idle')}>
            Run Again
          </button>
        )}
      </div>

      <style>{`
        .alert-rule-runner {
          display: flex;
          flex-direction: column;
          height: 100%;
        }

        .runner-header {
          display: flex;
          justify-content: space-between;
          align-items: flex-start;
          padding: 20px 24px;
          border-bottom: 1px solid var(--border-color);
        }

        .header-title h2 {
          margin: 0;
          font-size: 18px;
        }

        .rule-name {
          margin: 4px 0 0 0;
          font-size: 14px;
          color: var(--text-secondary);
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

        .runner-content {
          flex: 1;
          overflow-y: auto;
          padding: 24px;
        }

        .mode-selector {
          display: flex;
          gap: 12px;
          margin-bottom: 20px;
        }

        .mode-btn {
          flex: 1;
          padding: 14px 20px;
          border: 2px solid var(--border-color);
          border-radius: 8px;
          background: var(--bg-card);
          cursor: pointer;
          font-size: 14px;
          font-weight: 500;
          transition: all 0.2s;
        }

        .mode-btn:hover {
          border-color: var(--color-primary);
        }

        .mode-btn.active {
          border-color: var(--color-primary);
          background: var(--color-primary);
          color: white;
        }

        .info-box {
          padding: 12px 16px;
          border-radius: 8px;
          margin-bottom: 20px;
          font-size: 14px;
        }

        .info-warning {
          background: rgba(245, 158, 11, 0.1);
          border: 1px solid rgba(245, 158, 11, 0.3);
          color: #f59e0b;
        }

        .info-danger {
          background: rgba(239, 68, 68, 0.1);
          border: 1px solid rgba(239, 68, 68, 0.3);
          color: #ef4444;
        }

        .runner-section {
          margin-bottom: 24px;
        }

        .runner-section h3 {
          margin: 0 0 12px 0;
          font-size: 15px;
          font-weight: 600;
        }

        .section-header {
          display: flex;
          justify-content: space-between;
          align-items: center;
          margin-bottom: 12px;
        }

        .section-header h3 {
          margin: 0;
        }

        .target-actions {
          display: flex;
          gap: 12px;
        }

        .btn-link {
          background: none;
          border: none;
          color: var(--color-primary);
          cursor: pointer;
          font-size: 13px;
          padding: 0;
        }

        .btn-link:hover {
          text-decoration: underline;
        }

        .summary-grid {
          display: grid;
          grid-template-columns: repeat(auto-fit, minmax(150px, 1fr));
          gap: 12px;
        }

        .summary-item {
          background: var(--bg-hover);
          padding: 12px;
          border-radius: 6px;
        }

        .summary-label {
          display: block;
          font-size: 12px;
          color: var(--text-secondary);
          margin-bottom: 4px;
        }

        .summary-value {
          font-size: 14px;
          font-weight: 500;
        }

        .target-grid {
          display: grid;
          grid-template-columns: repeat(auto-fill, minmax(200px, 1fr));
          gap: 8px;
          max-height: 240px;
          overflow-y: auto;
        }

        .target-card {
          display: flex;
          align-items: center;
          gap: 10px;
          padding: 10px 12px;
          border: 1px solid var(--border-color);
          border-radius: 6px;
          cursor: pointer;
          transition: all 0.2s;
        }

        .target-card:hover {
          background: var(--bg-hover);
        }

        .target-card.selected {
          border-color: var(--color-primary);
          background: rgba(59, 130, 246, 0.05);
        }

        .target-card input[type="checkbox"] {
          width: 16px;
          height: 16px;
          cursor: pointer;
        }

        .target-info {
          display: flex;
          flex-direction: column;
        }

        .target-name {
          font-size: 14px;
          font-weight: 500;
        }

        .target-status {
          font-size: 12px;
          color: var(--text-secondary);
        }

        .status-online {
          color: var(--color-success);
        }

        .status-offline {
          color: var(--color-danger);
        }

        .status-blocked {
          color: var(--color-warning);
        }

        .error-hint {
          margin: 8px 0 0 0;
          font-size: 13px;
          color: var(--color-danger);
        }

        .results-section {
          background: var(--bg-hover);
          padding: 16px;
          border-radius: 8px;
        }

        .results-summary {
          margin-bottom: 16px;
        }

        .result-card {
          display: flex;
          align-items: center;
          gap: 16px;
          padding: 16px;
          border-radius: 8px;
        }

        .result-card.triggered {
          background: rgba(239, 68, 68, 0.1);
          border: 1px solid rgba(239, 68, 68, 0.3);
        }

        .result-card.not-triggered {
          background: rgba(16, 185, 129, 0.1);
          border: 1px solid rgba(16, 185, 129, 0.3);
        }

        .result-icon {
          font-size: 32px;
        }

        .result-info {
          display: flex;
          flex-direction: column;
        }

        .result-status {
          font-size: 16px;
          font-weight: 600;
        }

        .result-detail {
          font-size: 13px;
          color: var(--text-secondary);
        }

        .results-list h4 {
          margin: 0 0 8px 0;
          font-size: 14px;
        }

        .results-table {
          width: 100%;
          border-collapse: collapse;
          font-size: 13px;
        }

        .results-table th,
        .results-table td {
          padding: 8px 12px;
          text-align: left;
          border-bottom: 1px solid var(--border-color);
        }

        .results-table th {
          font-weight: 600;
          color: var(--text-secondary);
        }

        .results-table tr.triggered-row {
          background: rgba(239, 68, 68, 0.05);
        }

        .message-cell {
          max-width: 200px;
          overflow: hidden;
          text-overflow: ellipsis;
          white-space: nowrap;
        }

        .runner-footer {
          display: flex;
          justify-content: flex-end;
          gap: 12px;
          padding: 16px 24px;
          border-top: 1px solid var(--border-color);
        }

        .btn {
          padding: 10px 20px;
          border: none;
          border-radius: 6px;
          cursor: pointer;
          font-weight: 500;
          font-size: 14px;
          transition: background 0.2s;
        }

        .btn:disabled {
          opacity: 0.5;
          cursor: not-allowed;
        }

        .btn-primary {
          background: var(--color-primary);
          color: white;
        }

        .btn-primary:hover:not(:disabled) {
          background: var(--color-primary-dark);
        }

        .btn-secondary {
          background: var(--bg-hover);
          color: var(--text-primary);
        }

        .btn-secondary:hover:not(:disabled) {
          background: var(--bg-disabled);
        }

        .badge {
          display: inline-block;
          padding: 4px 8px;
          border-radius: 4px;
          font-size: 12px;
          font-weight: 600;
          text-transform: uppercase;
        }

        .badge-success {
          background: rgba(16, 185, 129, 0.15);
          color: #10b981;
        }

        .badge-warning {
          background: rgba(245, 158, 11, 0.15);
          color: #f59e0b;
        }

        .badge-critical {
          background: rgba(239, 68, 68, 0.15);
          color: #ef4444;
        }

        .badge-info {
          background: rgba(59, 130, 246, 0.15);
          color: #3b82f6;
        }
      `}</style>
    </div>
  );
};
