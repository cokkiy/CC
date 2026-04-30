/**
 * AlertRuleEditor - Drawer panel for creating/editing Alert Rules
 * Part of Phase 9: Alert Rules Management System
 */

import React, { useState, useEffect } from 'react';
import type {
  AlertRule,
  AlertCondition,
  AlertConditionGroup,
  AlertAction,
  AlertSeverity,
  AlertTargetSelector,
  MetricType,
  ComparisonOperator,
  AlertActionType,
} from './types';

export interface AlertRuleEditorProps {
  rule?: AlertRule;
  targets: Array<{ id: string; name: string; status?: string }>;
  onSave: (rule: Partial<AlertRule>) => void;
  onCancel: () => void;
  isNew: boolean;
}

// Default empty condition
const createEmptyCondition = (): AlertCondition => ({
  id: crypto.randomUUID(),
  metricType: 'cpu_usage',
  operator: 'gt',
  threshold: 80,
  unit: '%',
});

// Default empty action
const createEmptyAction = (): AlertAction => ({
  id: crypto.randomUUID(),
  actionType: 'notification',
  name: 'New Action',
  enabled: true,
  config: {
    message: 'Alert triggered',
    channels: ['in_app'],
  },
});

export const AlertRuleEditor: React.FC<AlertRuleEditorProps> = ({
  rule,
  targets,
  onSave,
  onCancel,
  isNew,
}) => {
  // Form state
  const [name, setName] = useState(rule?.name || '');
  const [description, setDescription] = useState(rule?.description || '');
  const [severity, setSeverity] = useState<AlertSeverity>(rule?.severity || 'warning');
  const [cooldownSecs, setCooldownSecs] = useState(rule?.cooldownSecs || 300);
  const [maxTriggersPerHour, setMaxTriggersPerHour] = useState(rule?.maxTriggersPerHour || 10);
  const [tags, setTags] = useState<string[]>(rule?.tags || []);
  const [tagInput, setTagInput] = useState('');

  // Condition state
  const [conditionLogic, setConditionLogic] = useState<'and' | 'or'>(
    rule?.conditionGroup?.logic || 'and'
  );
  const [conditions, setConditions] = useState<AlertCondition[]>(
    rule?.conditionGroup?.conditions || [createEmptyCondition()]
  );

  // Target selector state
  const [selectorType, setSelectorType] = useState<AlertTargetSelector['selectorType']>(
    rule?.targetSelector?.selectorType || 'all'
  );
  const [selectedTargetIds, setSelectedTargetIds] = useState<string[]>(
    rule?.targetSelector?.deviceIds || []
  );
  const [selectedGroups, setSelectedGroups] = useState<string[]>(
    rule?.targetSelector?.groups || []
  );
  const [selectedTags, setSelectedTags] = useState<string[]>(
    rule?.targetSelector?.tags || []
  );

  // Actions state
  const [actions, setActions] = useState<AlertAction[]>(
    rule?.actions || [createEmptyAction()]
  );

  // Validation errors
  const [errors, setErrors] = useState<Record<string, string>>({});

  const validate = (): boolean => {
    const newErrors: Record<string, string> = {};

    if (!name.trim()) {
      newErrors.name = 'Name is required';
    }

    if (conditions.length === 0) {
      newErrors.conditions = 'At least one condition is required';
    }

    if (selectorType === 'device_ids' && selectedTargetIds.length === 0) {
      newErrors.targets = 'At least one target must be selected';
    }

    if (cooldownSecs < 0) {
      newErrors.cooldown = 'Cooldown must be non-negative';
    }

    setErrors(newErrors);
    return Object.keys(newErrors).length === 0;
  };

  const handleSubmit = () => {
    if (!validate()) return;

    const alertRule: Partial<AlertRule> = {
      ...(rule?.id ? { id: rule.id } : {}),
      name: name.trim(),
      description: description.trim(),
      severity,
      cooldownSecs,
      maxTriggersPerHour,
      tags,
      conditionGroup: {
        logic: conditionLogic,
        conditions,
      } as AlertConditionGroup,
      targetSelector: {
        selectorType,
        ...(selectorType === 'device_ids' ? { deviceIds: selectedTargetIds } : {}),
        ...(selectorType === 'group' ? { groups: selectedGroups } : {}),
        ...(selectorType === 'tag' ? { tags: selectedTags } : {}),
      } as AlertTargetSelector,
      actions,
      status: rule?.status || 'enabled',
      isFavorite: rule?.isFavorite || false,
      isTemplate: false,
      createdBy: rule?.createdBy || 'current_user',
      createdAt: rule?.createdAt || new Date().toISOString(),
      updatedAt: new Date().toISOString(),
      version: (rule?.version || 0) + 1,
      usageCount: rule?.usageCount || 0,
    };

    onSave(alertRule);
  };

  // Condition handlers
  const addCondition = () => {
    setConditions([...conditions, createEmptyCondition()]);
  };

  const removeCondition = (id: string) => {
    setConditions(conditions.filter((c) => c.id !== id));
  };

  const updateCondition = (id: string, updates: Partial<AlertCondition>) => {
    setConditions(
      conditions.map((c) => (c.id === id ? { ...c, ...updates } : c))
    );
  };

  // Action handlers
  const addAction = () => {
    setActions([...actions, createEmptyAction()]);
  };

  const removeAction = (id: string) => {
    setActions(actions.filter((a) => a.id !== id));
  };

  const updateAction = (id: string, updates: Partial<AlertAction>) => {
    setActions(actions.map((a) => (a.id === id ? { ...a, ...updates } : a)));
  };

  // Tag handlers
  const addTag = () => {
    if (tagInput.trim() && !tags.includes(tagInput.trim())) {
      setTags([...tags, tagInput.trim()]);
      setTagInput('');
    }
  };

  const removeTag = (tag: string) => {
    setTags(tags.filter((t) => t !== tag));
  };

  const METRIC_OPTIONS: { value: MetricType; label: string }[] = [
    { value: 'cpu_usage', label: 'CPU Usage (%)' },
    { value: 'cpu_load', label: 'CPU Load Average' },
    { value: 'memory_usage', label: 'Memory Usage (%)' },
    { value: 'memory_available', label: 'Memory Available' },
    { value: 'disk_usage', label: 'Disk Usage (%)' },
    { value: 'disk_free', label: 'Free Disk Space' },
    { value: 'network_rx', label: 'Network RX (bytes/sec)' },
    { value: 'network_tx', label: 'Network TX (bytes/sec)' },
    { value: 'process_count', label: 'Process Count' },
    { value: 'custom', label: 'Custom Metric' },
  ];

  const OPERATOR_OPTIONS: { value: ComparisonOperator; label: string }[] = [
    { value: 'gt', label: '> (Greater than)' },
    { value: 'gte', label: '>= (Greater or equal)' },
    { value: 'lt', label: '< (Less than)' },
    { value: 'lte', label: '<= (Less or equal)' },
    { value: 'eq', label: '= (Equal)' },
    { value: 'neq', label: '!= (Not equal)' },
    { value: 'between', label: 'Between (exclusive)' },
  ];

  const SEVERITY_OPTIONS: { value: AlertSeverity; label: string; color: string }[] = [
    { value: 'info', label: 'Info', color: '#3b82f6' },
    { value: 'warning', label: 'Warning', color: '#f59e0b' },
    { value: 'critical', label: 'Critical', color: '#ef4444' },
  ];

  return (
    <div className="alert-rule-editor">
      <div className="editor-header">
        <h2>{isNew ? 'Create Alert Rule' : 'Edit Alert Rule'}</h2>
        <div className="editor-actions">
          <button type="button" className="btn btn-secondary" onClick={onCancel}>
            Cancel
          </button>
          <button type="button" className="btn btn-primary" onClick={handleSubmit}>
            {isNew ? 'Create Alert Rule' : 'Save Changes'}
          </button>
        </div>
      </div>

      <div className="editor-body">
        {/* Basic Info Section */}
        <section className="editor-section">
          <h3>Basic Information</h3>

          <div className="form-group">
            <label htmlFor="name">Name *</label>
            <input
              id="name"
              type="text"
              value={name}
              onChange={(e) => setName(e.target.value)}
              placeholder="e.g., High CPU Alert"
              className={errors.name ? 'error' : ''}
            />
            {errors.name && <span className="error-text">{errors.name}</span>}
          </div>

          <div className="form-group">
            <label htmlFor="description">Description</label>
            <textarea
              id="description"
              value={description}
              onChange={(e) => setDescription(e.target.value)}
              placeholder="Describe when this alert should trigger..."
              rows={2}
            />
          </div>

          <div className="form-row">
            <div className="form-group">
              <label htmlFor="severity">Severity</label>
              <select
                id="severity"
                value={severity}
                onChange={(e) => setSeverity(e.target.value as AlertSeverity)}
              >
                {SEVERITY_OPTIONS.map((opt) => (
                  <option key={opt.value} value={opt.value}>
                    {opt.label}
                  </option>
                ))}
              </select>
            </div>

            <div className="form-group">
              <label htmlFor="cooldown">Cooldown (seconds)</label>
              <input
                id="cooldown"
                type="number"
                value={cooldownSecs}
                onChange={(e) => setCooldownSecs(parseInt(e.target.value) || 0)}
                min={0}
                className={errors.cooldown ? 'error' : ''}
              />
              {errors.cooldown && <span className="error-text">{errors.cooldown}</span>}
            </div>

            <div className="form-group">
              <label htmlFor="maxTriggers">Max Triggers/Hour</label>
              <input
                id="maxTriggers"
                type="number"
                value={maxTriggersPerHour}
                onChange={(e) => setMaxTriggersPerHour(parseInt(e.target.value) || 0)}
                min={0}
              />
            </div>
          </div>

          <div className="form-group">
            <label>Tags</label>
            <div className="tag-input-container">
              <input
                type="text"
                value={tagInput}
                onChange={(e) => setTagInput(e.target.value)}
                onKeyDown={(e) => e.key === 'Enter' && (e.preventDefault(), addTag())}
                placeholder="Add tag and press Enter"
              />
              <button type="button" className="btn btn-secondary" onClick={addTag}>Add</button>
            </div>
            {tags.length > 0 && (
              <div className="tags-list">
                {tags.map((tag) => (
                  <span key={tag} className="tag">
                    {tag}
                    <button type="button" onClick={() => removeTag(tag)}>×</button>
                  </span>
                ))}
              </div>
            )}
          </div>
        </section>

        {/* Conditions Section */}
        <section className="editor-section">
          <div className="section-header">
            <h3>Alert Conditions</h3>
            <div className="logic-toggle">
              <label>Match:</label>
              <button
                type="button"
                className={conditionLogic === 'and' ? 'active' : ''}
                onClick={() => setConditionLogic('and')}
              >
                ALL (AND)
              </button>
              <button
                type="button"
                className={conditionLogic === 'or' ? 'active' : ''}
                onClick={() => setConditionLogic('or')}
              >
                ANY (OR)
              </button>
            </div>
          </div>

          {errors.conditions && <span className="error-text">{errors.conditions}</span>}

          <div className="conditions-list">
            {conditions.map((condition, index) => (
              <div key={condition.id} className="condition-card">
                <div className="condition-header">
                  <span className="condition-number">Condition {index + 1}</span>
                  {conditions.length > 1 && (
                    <button
                      type="button"
                      className="btn-icon btn-danger"
                      onClick={() => removeCondition(condition.id)}
                    >
                      🗑️
                    </button>
                  )}
                </div>

                <div className="condition-body">
                  <div className="form-row">
                    <div className="form-group">
                      <label>Metric</label>
                      <select
                        value={condition.metricType}
                        onChange={(e) =>
                          updateCondition(condition.id, { metricType: e.target.value as MetricType })
                        }
                      >
                        {METRIC_OPTIONS.map((opt) => (
                          <option key={opt.value} value={opt.value}>
                            {opt.label}
                          </option>
                        ))}
                      </select>
                    </div>

                    <div className="form-group">
                      <label>Operator</label>
                      <select
                        value={condition.operator}
                        onChange={(e) =>
                          updateCondition(condition.id, { operator: e.target.value as ComparisonOperator })
                        }
                      >
                        {OPERATOR_OPTIONS.map((opt) => (
                          <option key={opt.value} value={opt.value}>
                            {opt.label}
                          </option>
                        ))}
                      </select>
                    </div>

                    <div className="form-group">
                      <label>Threshold</label>
                      <input
                        type="number"
                        value={condition.threshold}
                        onChange={(e) =>
                          updateCondition(condition.id, { threshold: parseFloat(e.target.value) })
                        }
                      />
                    </div>

                    {condition.operator === 'between' && (
                      <div className="form-group">
                        <label>Upper Threshold</label>
                        <input
                          type="number"
                          value={condition.thresholdHigh || 0}
                          onChange={(e) =>
                            updateCondition(condition.id, { thresholdHigh: parseFloat(e.target.value) })
                          }
                        />
                      </div>
                    )}
                  </div>
                </div>
              </div>
            ))}
          </div>

          <button type="button" className="btn btn-secondary" onClick={addCondition}>
            + Add Condition
          </button>
        </section>

        {/* Targets Section */}
        <section className="editor-section">
          <h3>Alert Targets</h3>

          <div className="form-group">
            <label>Target Selection</label>
            <select
              value={selectorType}
              onChange={(e) => setSelectorType(e.target.value as AlertTargetSelector['selectorType'])}
            >
              <option value="all">All Stations</option>
              <option value="group">By Group</option>
              <option value="tag">By Tag</option>
              <option value="device_ids">Select Specific Stations</option>
            </select>
          </div>

          {errors.targets && <span className="error-text">{errors.targets}</span>}

          {selectorType === 'device_ids' && (
            <div className="target-selector">
              <div className="target-list">
                {targets.map((target) => (
                  <label key={target.id} className="target-item">
                    <input
                      type="checkbox"
                      checked={selectedTargetIds.includes(target.id)}
                      onChange={(e) => {
                        if (e.target.checked) {
                          setSelectedTargetIds([...selectedTargetIds, target.id]);
                        } else {
                          setSelectedTargetIds(selectedTargetIds.filter((id) => id !== target.id));
                        }
                      }}
                    />
                    <span>{target.name}</span>
                  </label>
                ))}
              </div>
              {selectedTargetIds.length === 0 && (
                <p className="hint">No stations selected. Alert will not trigger for any station.</p>
              )}
            </div>
          )}

          {selectorType === 'group' && (
            <div className="form-group">
              <input
                type="text"
                placeholder="Enter group names (comma separated)"
                value={selectedGroups.join(', ')}
                onChange={(e) =>
                  setSelectedGroups(
                    e.target.value.split(',').map((g) => g.trim()).filter(Boolean)
                  )
                }
              />
            </div>
          )}

          {selectorType === 'tag' && (
            <div className="form-group">
              <input
                type="text"
                placeholder="Enter tags (comma separated)"
                value={selectedTags.join(', ')}
                onChange={(e) =>
                  setSelectedTags(
                    e.target.value.split(',').map((t) => t.trim()).filter(Boolean)
                  )
                }
              />
            </div>
          )}
        </section>

        {/* Actions Section */}
        <section className="editor-section">
          <h3>Alert Actions</h3>
          <p className="section-hint">Define what happens when this alert triggers</p>

          <div className="actions-list">
            {actions.map((action, index) => (
              <div key={action.id} className="action-card">
                <div className="action-header">
                  <span>Action {index + 1}</span>
                  <button
                    type="button"
                    className="btn-icon btn-danger"
                    onClick={() => removeAction(action.id)}
                  >
                    🗑️
                  </button>
                </div>

                <div className="form-row">
                  <div className="form-group">
                    <label>Type</label>
                    <select
                      value={action.actionType}
                      onChange={(e) =>
                        updateAction(action.id, { actionType: e.target.value as AlertActionType })
                      }
                    >
                      <option value="notification">In-App Notification</option>
                      <option value="email">Email</option>
                      <option value="script">Run Script</option>
                      <option value="webhook">Webhook</option>
                      <option value="auto_remediate">Auto-Remediate</option>
                    </select>
                  </div>

                  <div className="form-group">
                    <label>Name</label>
                    <input
                      type="text"
                      value={action.name}
                      onChange={(e) => updateAction(action.id, { name: e.target.value })}
                    />
                  </div>
                </div>

                {action.actionType === 'notification' && (
                  <div className="form-group">
                    <label>Message</label>
                    <textarea
                      value={action.config.message || ''}
                      onChange={(e) =>
                        updateAction(action.id, {
                          config: { ...action.config, message: e.target.value },
                        })
                      }
                      placeholder="Alert message template..."
                      rows={2}
                    />
                  </div>
                )}

                {action.actionType === 'email' && (
                  <div className="form-group">
                    <label>Recipients (comma separated)</label>
                    <input
                      type="text"
                      value={action.config.emailRecipients?.join(', ') || ''}
                      onChange={(e) =>
                        updateAction(action.id, {
                          config: {
                            ...action.config,
                            emailRecipients: e.target.value.split(',').map((s) => s.trim()),
                          },
                        })
                      }
                      placeholder="admin@example.com, ops@example.com"
                    />
                  </div>
                )}

                {action.actionType === 'webhook' && (
                  <>
                    <div className="form-group">
                      <label>Webhook URL</label>
                      <input
                        type="url"
                        value={action.config.webhookUrl || ''}
                        onChange={(e) =>
                          updateAction(action.id, {
                            config: { ...action.config, webhookUrl: e.target.value },
                          })
                        }
                        placeholder="https://api.example.com/webhook"
                      />
                    </div>
                    <div className="form-row">
                      <div className="form-group">
                        <label>Method</label>
                        <select
                          value={action.config.webhookMethod || 'POST'}
                          onChange={(e) =>
                            updateAction(action.id, {
                              config: { ...action.config, webhookMethod: e.target.value as 'GET' | 'POST' | 'PUT' },
                            })
                          }
                        >
                          <option value="GET">GET</option>
                          <option value="POST">POST</option>
                          <option value="PUT">PUT</option>
                        </select>
                      </div>
                    </div>
                  </>
                )}

                <div className="form-group">
                  <label className="checkbox-label">
                    <input
                      type="checkbox"
                      checked={action.enabled}
                      onChange={(e) => updateAction(action.id, { enabled: e.target.checked })}
                    />
                    Action Enabled
                  </label>
                </div>
              </div>
            ))}
          </div>

          <button type="button" className="btn btn-secondary" onClick={addAction}>
            + Add Action
          </button>
        </section>
      </div>

      <style>{`
        .alert-rule-editor {
          display: flex;
          flex-direction: column;
          height: 100%;
          background: var(--bg-card);
          color: var(--text-main);
        }

        .editor-header {
          display: flex;
          justify-content: space-between;
          align-items: center;
          position: sticky;
          top: 0;
          z-index: 30;
          padding: 14px 22px;
          border-bottom: 1px solid var(--border-color);
          background: var(--bg-card);
          box-shadow: 0 10px 20px -18px rgba(11, 25, 44, 0.42);
        }

        .editor-header h2 {
          margin: 0;
          font-size: 1.08rem;
          font-weight: 700;
        }

        .editor-actions {
          display: flex;
          gap: 12px;
        }

        .editor-body {
          flex: 1;
          overflow-y: auto;
          padding: 18px 22px 24px;
          display: grid;
          gap: 16px;
          align-content: start;
          background: linear-gradient(180deg, rgba(244, 247, 249, 0.78) 0%, rgba(244, 247, 249, 1) 100%);
        }

        .editor-section {
          margin: 0;
          padding: 16px;
          background: var(--bg-card);
          border-radius: 12px;
          border: 1px solid var(--border-color);
          box-shadow: 0 8px 20px rgba(11, 25, 44, 0.05);
        }

        .editor-section h3 {
          margin: 0 0 14px 0;
          font-size: 0.95rem;
          font-weight: 600;
          color: var(--text-main);
        }

        .section-header {
          display: flex;
          justify-content: space-between;
          align-items: center;
          margin-bottom: 14px;
        }

        .section-hint {
          font-size: 0.74rem;
          color: var(--text-muted);
          margin: -8px 0 16px 0;
        }

        .form-group {
          margin-bottom: 16px;
        }

        .form-group label {
          display: block;
          margin-bottom: 6px;
          font-size: 0.78rem;
          font-weight: 500;
          color: var(--text-muted);
          text-transform: uppercase;
          letter-spacing: 0.04em;
        }

        .form-group input[type="text"],
        .form-group input[type="number"],
        .form-group input[type="url"],
        .form-group textarea,
        .form-group select {
          width: 100%;
          padding: 10px 12px;
          border: 1px solid var(--border-color);
          border-radius: 8px;
          background: var(--bg-main);
          color: var(--text-main);
          font-size: 0.86rem;
          transition: border-color 0.2s;
        }

        .form-group input[type="text"]:focus,
        .form-group input[type="number"]:focus,
        .form-group input[type="url"]:focus,
        .form-group textarea:focus,
        .form-group select:focus {
          outline: none;
          border-color: var(--primary);
        }

        .form-group input.error {
          border-color: #ef4444;
        }

        .error-text {
          display: block;
          margin-top: 4px;
          font-size: 12px;
          color: #ef4444;
        }

        .form-row {
          display: flex;
          gap: 16px;
        }

        .form-row .form-group {
          flex: 1;
        }

        .tag-input-container {
          display: flex;
          gap: 8px;
        }

        .tag-input-container input {
          flex: 1;
        }

        .tags-list {
          display: flex;
          flex-wrap: wrap;
          gap: 8px;
          margin-top: 8px;
        }

        .tag {
          display: inline-flex;
          align-items: center;
          gap: 6px;
          padding: 4px 9px;
          background: rgba(45, 140, 240, 0.1);
          color: #1f4e89;
          border: 1px solid rgba(45, 140, 240, 0.2);
          border-radius: 4px;
          font-size: 12px;
        }

        .tag button {
          background: none;
          border: none;
          cursor: pointer;
          padding: 0;
          font-size: 14px;
          line-height: 1;
          color: #1f4e89;
        }

        .logic-toggle {
          display: flex;
          align-items: center;
          gap: 8px;
        }

        .logic-toggle label {
          margin: 0;
          text-transform: uppercase;
          letter-spacing: 0.04em;
        }

        .logic-toggle button {
          padding: 8px 12px;
          border: 1px solid var(--border-color);
          border-radius: 8px;
          background: var(--bg-main);
          cursor: pointer;
          font-size: 0.78rem;
          color: var(--text-muted);
        }

        .logic-toggle button.active {
          background: var(--primary);
          color: white;
          border-color: var(--primary);
        }

        .conditions-list,
        .actions-list {
          display: flex;
          flex-direction: column;
          gap: 12px;
          margin-bottom: 16px;
        }

        .condition-card,
        .action-card {
          border: 1px solid var(--border-color);
          border-radius: 10px;
          padding: 16px;
          background: var(--bg-main);
        }

        .condition-header,
        .action-header {
          display: flex;
          justify-content: space-between;
          align-items: center;
          margin-bottom: 12px;
        }

        .condition-number,
        .action-header span {
          font-size: 0.78rem;
          font-weight: 600;
          color: var(--text-muted);
          text-transform: uppercase;
          letter-spacing: 0.04em;
        }

        .btn-icon {
          padding: 6px 10px;
          border: 1px solid var(--border-color);
          background: var(--bg-card);
          cursor: pointer;
          border-radius: 8px;
          font-size: 14px;
        }

        .btn-icon:hover {
          border-color: var(--primary);
        }

        .btn-icon.btn-danger:hover {
          background: rgba(239, 68, 68, 0.1);
          border-color: #ef4444;
        }

        .target-selector {
          border: 1px solid var(--border-color);
          border-radius: 8px;
          padding: 12px;
          max-height: 200px;
          overflow-y: auto;
          background: var(--bg-main);
        }

        .target-list {
          display: flex;
          flex-direction: column;
          gap: 8px;
        }

        .target-item {
          display: flex;
          align-items: center;
          gap: 8px;
          cursor: pointer;
          padding: 4px;
          border-radius: 4px;
        }

        .target-item:hover {
          background: rgba(45, 140, 240, 0.08);
        }

        .hint {
          font-size: 12px;
          color: var(--text-muted);
          font-style: italic;
          margin: 8px 0 0 0;
        }

        .checkbox-label {
          display: flex !important;
          align-items: center;
          gap: 8px;
          cursor: pointer;
        }

        .btn {
          padding: 8px 14px;
          border: 1px solid transparent;
          border-radius: 8px;
          cursor: pointer;
          font-weight: 500;
          font-size: 0.84rem;
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
          color: var(--text-muted);
        }

        .btn-secondary:hover {
          background: transparent;
          border-color: var(--primary);
          color: var(--text-main);
        }

        @media (max-width: 920px) {
          .editor-header {
            padding: 12px;
            align-items: flex-start;
            flex-direction: column;
          }

          .editor-actions {
            width: 100%;
          }

          .editor-actions > button {
            flex: 1;
          }

          .editor-body {
            padding: 12px;
          }

          .form-row {
            flex-direction: column;
            gap: 0;
          }
        }
      `}</style>
    </div>
  );
};
