/**
 * Batch Task Editor Component
 * Interface for creating and editing batch tasks
 * Part of Phase 7: Batch Operations Support System
 */

import React, { useState, useCallback, useMemo } from 'react';
import type {
  BatchTask,
  BatchTaskType,
  BatchTaskParameter,
  BatchTaskStatus,
  TargetSelector,
  TargetSelectorType,
  ExecutionPolicy,
  ExecutionMode,
  BatchTarget,
  BatchTaskFilter,
  ValidateTaskResult,
} from './types';
import { DEFAULT_EXECUTION_POLICY } from './types';

// ============================================
// Target Selector Component
// ============================================

interface TargetSelectorEditorProps {
  selector: TargetSelector;
  targets: BatchTarget[];
  onChange: (selector: TargetSelector) => void;
  showPreview?: boolean;
  disabled?: boolean;
}

const TargetSelectorEditor: React.FC<TargetSelectorEditorProps> = ({
  selector,
  targets,
  onChange,
  showPreview = true,
  disabled = false,
}) => {
  const [previewCount, setPreviewCount] = useState<number>(0);

  const selectorTypes: { key: TargetSelectorType; label: string; icon: string }[] = [
    { key: 'all', label: 'All Devices', icon: '🌐' },
    { key: 'group', label: 'Groups', icon: '📁' },
    { key: 'tag', label: 'Tags', icon: '🏷️' },
    { key: 'device_ids', label: 'Device IDs', icon: '💻' },
    { key: 'filter', label: 'Filter Expression', icon: '🔍' },
  ];

  const availableGroups = useMemo(() => {
    const groups = new Set<string>();
    targets.forEach(t => t.group && groups.add(t.group));
    return Array.from(groups).sort();
  }, [targets]);

  const availableTags = useMemo(() => {
    const tags = new Set<string>();
    targets.forEach(t => t.tags && Object.keys(t.tags).forEach(k => tags.add(k)));
    return Array.from(tags).sort();
  }, [targets]);

  const handleSelectorTypeChange = (type: TargetSelectorType) => {
    onChange({
      selectorType: type,
      groups: type === 'group' ? [] : undefined,
      tags: type === 'tag' ? [] : undefined,
      deviceIds: type === 'device_ids' ? [] : undefined,
      filterExpr: type === 'filter' ? '' : undefined,
    });
  };

  const handleGroupToggle = (group: string) => {
    const current = selector.groups || [];
    const updated = current.includes(group)
      ? current.filter(g => g !== group)
      : [...current, group];
    onChange({ ...selector, groups: updated });
  };

  const handleTagToggle = (tag: string) => {
    const current = selector.tags || [];
    const updated = current.includes(tag)
      ? current.filter(t => t !== tag)
      : [...current, tag];
    onChange({ ...selector, tags: updated });
  };

  const handleDeviceIdChange = (value: string) => {
    const deviceIds = value.split(',').map(s => s.trim()).filter(Boolean);
    onChange({ ...selector, deviceIds });
  };

  const handleFilterExprChange = (value: string) => {
    onChange({ ...selector, filterExpr: value });
  };

  // Calculate preview count based on selector
  const calculatePreview = useCallback(() => {
    switch (selector.selectorType) {
      case 'all':
        setPreviewCount(targets.length);
        break;
      case 'group':
        setPreviewCount(targets.filter(t => t.group && selector.groups?.includes(t.group)).length);
        break;
      case 'tag':
        setPreviewCount(targets.filter(t => t.tags && selector.tags?.some(tag => t.tags && tag in t.tags)).length);
        break;
      case 'device_ids':
        setPreviewCount(targets.filter(t => selector.deviceIds?.includes(t.id)).length);
        break;
      case 'filter':
        setPreviewCount(0); // Cannot calculate without backend
        break;
      default:
        setPreviewCount(0);
    }
  }, [selector, targets]);

  React.useEffect(() => {
    if (showPreview) {
      calculatePreview();
    }
  }, [selector, targets, showPreview, calculatePreview]);

  return (
    <div className="target-selector-editor">
      <div className="selector-type-tabs">
        {selectorTypes.map(({ key, label, icon }) => (
          <button
            key={key}
            className={`selector-tab ${selector.selectorType === key ? 'active' : ''}`}
            onClick={() => handleSelectorTypeChange(key)}
            disabled={disabled}
          >
            <span className="tab-icon">{icon}</span>
            <span className="tab-label">{label}</span>
          </button>
        ))}
      </div>

      <div className="selector-content">
        {selector.selectorType === 'all' && (
          <div className="selector-all-info">
            <p>This task will target all {targets.length} available devices.</p>
          </div>
        )}

        {selector.selectorType === 'group' && (
          <div className="selector-groups">
            <p>Select device groups:</p>
            <div className="group-list">
              {availableGroups.length === 0 ? (
                <p className="no-groups">No groups available</p>
              ) : (
                availableGroups.map(group => (
                  <label key={group} className="group-item">
                    <input
                      type="checkbox"
                      checked={selector.groups?.includes(group) || false}
                      onChange={() => handleGroupToggle(group)}
                      disabled={disabled}
                    />
                    <span>{group}</span>
                  </label>
                ))
              )}
            </div>
          </div>
        )}

        {selector.selectorType === 'tag' && (
          <div className="selector-tags">
            <p>Select tags:</p>
            <div className="tag-list">
              {availableTags.length === 0 ? (
                <p className="no-tags">No tags available</p>
              ) : (
                availableTags.map(tag => (
                  <label key={tag} className="tag-item">
                    <input
                      type="checkbox"
                      checked={selector.tags?.includes(tag) || false}
                      onChange={() => handleTagToggle(tag)}
                      disabled={disabled}
                    />
                    <span>{tag}</span>
                  </label>
                ))
              )}
            </div>
          </div>
        )}

        {selector.selectorType === 'device_ids' && (
          <div className="selector-device-ids">
            <p>Enter device IDs (comma-separated):</p>
            <textarea
              className="device-ids-input"
              value={selector.deviceIds?.join(', ') || ''}
              onChange={(e) => handleDeviceIdChange(e.target.value)}
              placeholder="device-id-1, device-id-2, device-id-3"
              disabled={disabled}
              rows={3}
            />
            <p className="input-hint">Found {selector.deviceIds?.length || 0} device IDs</p>
          </div>
        )}

        {selector.selectorType === 'filter' && (
          <div className="selector-filter">
            <p>Enter filter expression:</p>
            <input
              type="text"
              className="filter-expr-input"
              value={selector.filterExpr || ''}
              onChange={(e) => handleFilterExprChange(e.target.value)}
              placeholder="status == 'online' && group == 'production'"
              disabled={disabled}
            />
            <p className="input-hint">Use expressions like: status == 'online', group == 'production'</p>
          </div>
        )}
      </div>

      {showPreview && (
        <div className="selector-preview">
          <span className="preview-label">Matching devices:</span>
          <span className="preview-count">{previewCount}</span>
        </div>
      )}
    </div>
  );
};

// ============================================
// Execution Policy Config Component
// ============================================

interface ExecutionPolicyConfigProps {
  policy: ExecutionPolicy;
  onChange: (policy: ExecutionPolicy) => void;
  disabled?: boolean;
}

const ExecutionPolicyConfig: React.FC<ExecutionPolicyConfigProps> = ({
  policy,
  onChange,
  disabled = false,
}) => {
  const modes: { key: ExecutionMode; label: string; icon: string; description: string }[] = [
    {
      key: 'parallel',
      label: 'Parallel',
      icon: '⚡',
      description: 'Execute on all targets simultaneously',
    },
    {
      key: 'batch',
      label: 'Batch',
      icon: '📦',
      description: 'Execute in batches of N devices',
    },
    {
      key: 'rolling',
      label: 'Rolling',
      icon: '🔄',
      description: 'Gradually increase targets (1 → 5 → all)',
    },
  ];

  const handleModeChange = (mode: ExecutionMode) => {
    onChange({
      ...policy,
      mode,
      batchSize: mode === 'batch' ? (policy.batchSize || 5) : undefined,
    });
  };

  return (
    <div className="execution-policy-config">
      <div className="policy-section">
        <label className="section-label">Execution Mode</label>
        <div className="mode-selector">
          {modes.map(({ key, label, icon, description }) => (
            <label
              key={key}
              className={`mode-option ${policy.mode === key ? 'selected' : ''}`}
              title={description}
            >
              <input
                type="radio"
                name="execution-mode"
                value={key}
                checked={policy.mode === key}
                onChange={() => handleModeChange(key)}
                disabled={disabled}
              />
              <span className="mode-icon">{icon}</span>
              <span className="mode-label">{label}</span>
            </label>
          ))}
        </div>
        <p className="mode-description">
          {modes.find(m => m.key === policy.mode)?.description}
        </p>
      </div>

      {policy.mode === 'batch' && (
        <div className="policy-section">
          <label className="section-label">Batch Size</label>
          <input
            type="number"
            className="batch-size-input"
            value={policy.batchSize || 5}
            onChange={(e) => onChange({ ...policy, batchSize: parseInt(e.target.value) || 5 })}
            min={1}
            max={100}
            disabled={disabled}
          />
          <p className="input-hint">Number of devices per batch</p>
        </div>
      )}

      <div className="policy-section">
        <label className="section-label">Failure Handling</label>
        <div className="policy-row">
          <label className="checkbox-label">
            <input
              type="checkbox"
              checked={policy.continueOnFailure}
              onChange={(e) => onChange({ ...policy, continueOnFailure: e.target.checked })}
              disabled={disabled}
            />
            <span>Continue on failure</span>
          </label>
        </div>
      </div>

      <div className="policy-section">
        <label className="section-label">Circuit Breaker Threshold</label>
        <div className="threshold-input">
          <input
            type="range"
            min={0}
            max={100}
            value={policy.failureThresholdPercent}
            onChange={(e) => onChange({ ...policy, failureThresholdPercent: parseInt(e.target.value) })}
            disabled={disabled}
          />
          <span className="threshold-value">{policy.failureThresholdPercent}%</span>
        </div>
        <p className="input-hint">Pause execution when failure rate exceeds this threshold</p>
      </div>

      <div className="policy-section">
        <label className="section-label">Timeout</label>
        <div className="timeout-input">
          <input
            type="number"
            value={policy.timeoutSecs}
            onChange={(e) => onChange({ ...policy, timeoutSecs: parseInt(e.target.value) || 300 })}
            min={10}
            max={3600}
            disabled={disabled}
          />
          <span className="timeout-unit">seconds</span>
        </div>
      </div>

      <div className="policy-section">
        <label className="section-label">Retry Settings</label>
        <div className="retry-input">
          <div className="retry-count">
            <label>Retry count:</label>
            <input
              type="number"
              value={policy.retryCount}
              onChange={(e) => onChange({ ...policy, retryCount: parseInt(e.target.value) || 0 })}
              min={0}
              max={5}
              disabled={disabled}
            />
          </div>
          <div className="retry-delay">
            <label>Retry delay:</label>
            <input
              type="number"
              value={policy.retryDelaySecs || 5}
              onChange={(e) => onChange({ ...policy, retryDelaySecs: parseInt(e.target.value) || 5 })}
              min={1}
              max={60}
              disabled={disabled}
            />
            <span>seconds</span>
          </div>
        </div>
      </div>
    </div>
  );
};

// ============================================
// Parameter Editor Component
// ============================================

interface ParameterEditorProps {
  parameters: BatchTaskParameter[];
  onChange: (parameters: BatchTaskParameter[]) => void;
  disabled?: boolean;
}

const ParameterEditor: React.FC<ParameterEditorProps> = ({ parameters, onChange, disabled }) => {
  const addParameter = () => {
    const newParam: BatchTaskParameter = {
      name: `param${parameters.length + 1}`,
      paramType: 'string',
      defaultValue: '',
      required: false,
      description: '',
    };
    onChange([...parameters, newParam]);
  };

  const updateParameter = (index: number, updates: Partial<BatchTaskParameter>) => {
    const updated = [...parameters];
    updated[index] = { ...updated[index], ...updates };
    onChange(updated);
  };

  const removeParameter = (index: number) => {
    onChange(parameters.filter((_, i) => i !== index));
  };

  return (
    <div className="parameter-editor">
      <div className="parameter-editor-header">
        <h4>Parameters</h4>
        <button type="button" className="btn-add-param" onClick={addParameter} disabled={disabled}>
          + Add Parameter
        </button>
      </div>

      {parameters.length === 0 ? (
        <div className="no-parameters">
          No parameters defined. Click "Add Parameter" to create one.
        </div>
      ) : (
        <div className="parameters-list">
          {parameters.map((param, index) => (
            <div key={index} className="parameter-item">
              <div className="parameter-row">
                <input
                  type="text"
                  className="param-name"
                  placeholder="Name"
                  value={param.name}
                  onChange={(e) => updateParameter(index, { name: e.target.value })}
                  disabled={disabled}
                />
                <select
                  className="param-type"
                  value={param.paramType}
                  onChange={(e) => updateParameter(index, { paramType: e.target.value as BatchTaskParameter['paramType'] })}
                  disabled={disabled}
                >
                  <option value="string">String</option>
                  <option value="number">Number</option>
                  <option value="boolean">Boolean</option>
                  <option value="select">Select</option>
                </select>
                <input
                  type="text"
                  className="param-default"
                  placeholder="Default value"
                  value={param.defaultValue}
                  onChange={(e) => updateParameter(index, { defaultValue: e.target.value })}
                  disabled={disabled}
                />
                <label className="param-required">
                  <input
                    type="checkbox"
                    checked={param.required}
                    onChange={(e) => updateParameter(index, { required: e.target.checked })}
                    disabled={disabled}
                  />
                  Required
                </label>
                <button
                  type="button"
                  className="btn-remove-param"
                  onClick={() => removeParameter(index)}
                  disabled={disabled}
                >
                  ×
                </button>
              </div>

              <div className="parameter-advanced">
                <input
                  type="text"
                  className="param-description"
                  placeholder="Description (optional)"
                  value={param.description || ''}
                  onChange={(e) => updateParameter(index, { description: e.target.value })}
                  disabled={disabled}
                />
                {param.paramType === 'select' && (
                  <input
                    type="text"
                    className="param-options"
                    placeholder="Options (comma-separated)"
                    value={param.options?.join(', ') || ''}
                    onChange={(e) => updateParameter(index, {
                      options: e.target.value.split(',').map(s => s.trim()).filter(Boolean)
                    })}
                    disabled={disabled}
                  />
                )}
              </div>
            </div>
          ))}
        </div>
      )}
    </div>
  );
};

// ============================================
// Main Batch Task Editor Component
// ============================================

export interface BatchTaskEditorProps {
  task?: BatchTask;
  targets: BatchTarget[];
  onSave: (task: Partial<BatchTask>) => void;
  onCancel: () => void;
  onValidate?: (task: Partial<BatchTask>) => Promise<ValidateTaskResult>;
  readOnly?: boolean;
}

export const BatchTaskEditor: React.FC<BatchTaskEditorProps> = ({
  task,
  targets,
  onSave,
  onCancel,
  onValidate,
  readOnly = false,
}) => {
  const [name, setName] = useState(task?.name || '');
  const [description, setDescription] = useState(task?.description || '');
  const [taskType, setTaskType] = useState<BatchTaskType>(task?.taskType || 'command');
  const [content, setContent] = useState(task?.content || '');
  const [parameters, setParameters] = useState<BatchTaskParameter[]>(task?.parameters || []);
  const [targetSelector, setTargetSelector] = useState<TargetSelector>(
    task?.targetSelector || { selectorType: 'all' }
  );
  const [executionPolicy, setExecutionPolicy] = useState<ExecutionPolicy>(
    task?.executionPolicy || DEFAULT_EXECUTION_POLICY
  );
  const [tags, setTags] = useState<string[]>(task?.tags || []);
  const [tagInput, setTagInput] = useState('');
  const [validation, setValidation] = useState<ValidateTaskResult | null>(null);
  const [showAdvanced, setShowAdvanced] = useState(false);

  // Task types
  const taskTypes: { key: BatchTaskType; label: string; icon: string }[] = [
    { key: 'command', label: 'Command', icon: '💻' },
    { key: 'script', label: 'Script', icon: '📜' },
    { key: 'config', label: 'Config', icon: '⚙️' },
    { key: 'file_transfer', label: 'File Transfer', icon: '📁' },
    { key: 'upgrade', label: 'Upgrade', icon: '⬆️' },
    { key: 'reboot', label: 'Reboot', icon: '🔄' },
  ];

  // Validate task
  const validateTask = useCallback((): ValidateTaskResult => {
    const errors: string[] = [];
    const warnings: string[] = [];

    if (!name.trim()) {
      errors.push('Task name is required');
    }

    if (!content.trim()) {
      errors.push('Task content is required');
    }

    if (taskType === 'file_transfer' && !content.includes(':')) {
      warnings.push('File transfer content should specify source:destination paths');
    }

    return {
      valid: errors.length === 0,
      errors,
      warnings,
    };
  }, [name, content, taskType]);

  // Update validation on changes
  React.useEffect(() => {
    const result = validateTask();
    setValidation(result);
  }, [validateTask]);

  // Handle tag input
  const handleTagKeyDown = (e: React.KeyboardEvent<HTMLInputElement>) => {
    if (e.key === 'Enter' || e.key === ',') {
      e.preventDefault();
      const newTag = tagInput.trim();
      if (newTag && !tags.includes(newTag)) {
        setTags([...tags, newTag]);
      }
      setTagInput('');
    } else if (e.key === 'Backspace' && !tagInput && tags.length > 0) {
      setTags(tags.slice(0, -1));
    }
  };

  const removeTag = (tagToRemove: string) => {
    setTags(tags.filter(tag => tag !== tagToRemove));
  };

  // Handle save
  const handleSave = async () => {
    const result = validateTask();
    if (!result.valid) {
      setValidation(result);
      return;
    }

    if (onValidate) {
      const fullValidation = await onValidate({
        id: task?.id,
        name: name.trim(),
        description: description.trim(),
        taskType,
        content,
        parameters,
        targetSelector,
        executionPolicy,
        tags,
      });
      if (!fullValidation.valid) {
        setValidation(fullValidation);
        return;
      }
    }

    onSave({
      id: task?.id,
      name: name.trim(),
      description: description.trim(),
      taskType,
      content,
      parameters,
      targetSelector,
      executionPolicy,
      tags,
      version: task?.version || 1,
      createdBy: task?.createdBy || 'current-user',
      createdAt: task?.createdAt || new Date().toISOString(),
      updatedAt: new Date().toISOString(),
      status: task?.status || 'draft' as BatchTaskStatus,
      usageCount: task?.usageCount || 0,
    });
  };

  return (
    <div className="batch-task-editor modal-overlay">
      <div className="modal-content editor-modal">
        <div className="editor-header">
          <h2>{task ? 'Edit Batch Task' : 'Create New Batch Task'}</h2>
          <button className="btn-close" onClick={onCancel} disabled={readOnly}>×</button>
        </div>

        <div className="editor-body">
          {/* Basic Info Section */}
          <div className="editor-section basic-section">
            <div className="field-group">
              <label className="field-label">
                Task Name <span className="required">*</span>
              </label>
              <input
                type="text"
                className="field-input"
                placeholder="Enter task name"
                value={name}
                onChange={(e) => setName(e.target.value)}
                disabled={readOnly}
              />
            </div>

            <div className="field-group">
              <label className="field-label">Description</label>
              <textarea
                className="field-textarea"
                placeholder="Describe what this task does"
                value={description}
                onChange={(e) => setDescription(e.target.value)}
                disabled={readOnly}
                rows={2}
              />
            </div>

            <div className="field-group">
              <label className="field-label">Task Type</label>
              <div className="task-type-selector">
                {taskTypes.map(({ key, label, icon }) => (
                  <button
                    key={key}
                    className={`task-type-btn ${taskType === key ? 'active' : ''}`}
                    onClick={() => setTaskType(key)}
                    disabled={readOnly}
                    type="button"
                  >
                    <span className="type-icon">{icon}</span>
                    <span className="type-label">{label}</span>
                  </button>
                ))}
              </div>
            </div>
          </div>

          {/* Target Selector Section */}
          <div className="editor-section target-section">
            <h3>Target Selection</h3>
            <TargetSelectorEditor
              selector={targetSelector}
              targets={targets}
              onChange={setTargetSelector}
              disabled={readOnly}
            />
          </div>

          {/* Content Editor Section */}
          <div className="editor-section content-section">
            <h3>Task Content</h3>
            <textarea
              className="content-editor"
              placeholder={
                taskType === 'command' ? 'Enter command to execute...' :
                taskType === 'script' ? 'Enter script content...' :
                taskType === 'config' ? 'Enter configuration content...' :
                taskType === 'file_transfer' ? 'Enter source:destination paths...' :
                taskType === 'upgrade' ? 'Enter upgrade package path or command...' :
                'Enter reboot command or parameters...'
              }
              value={content}
              onChange={(e) => setContent(e.target.value)}
              disabled={readOnly}
              rows={10}
            />
          </div>

          {/* Parameters Section */}
          <div className="editor-section parameters-section">
            <button
              type="button"
              className="section-toggle"
              onClick={() => setShowAdvanced(!showAdvanced)}
            >
              <span>Parameters</span>
              <span className="toggle-icon">{showAdvanced ? '▼' : '▶'}</span>
            </button>
            {showAdvanced && (
              <ParameterEditor
                parameters={parameters}
                onChange={setParameters}
                disabled={readOnly}
              />
            )}
          </div>

          {/* Execution Policy Section */}
          <div className="editor-section policy-section">
            <h3>Execution Policy</h3>
            <ExecutionPolicyConfig
              policy={executionPolicy}
              onChange={setExecutionPolicy}
              disabled={readOnly}
            />
          </div>

          {/* Tags Section */}
          <div className="editor-section tags-section">
            <label className="field-label">Tags</label>
            <div className="tags-input-container">
              <div className="tags-list">
                {tags.map(tag => (
                  <span key={tag} className="tag">
                    {tag}
                    <button
                      type="button"
                      className="tag-remove"
                      onClick={() => removeTag(tag)}
                      disabled={readOnly}
                    >
                      ×
                    </button>
                  </span>
                ))}
                <input
                  type="text"
                  className="tag-input"
                  placeholder={tags.length === 0 ? "Add tags..." : ""}
                  value={tagInput}
                  onChange={(e) => setTagInput(e.target.value)}
                  onKeyDown={handleTagKeyDown}
                  disabled={readOnly}
                />
              </div>
            </div>
          </div>

          {/* Validation Messages */}
          {validation && !validation.valid && (
            <div className="validation-errors">
              {validation.errors.map((error: string, i: number) => (
                <div key={i} className="error-message">⚠️ {error}</div>
              ))}
            </div>
          )}
          {validation && validation.warnings.length > 0 && (
            <div className="validation-warnings">
              {validation.warnings.map((warning: string, i: number) => (
                <div key={i} className="warning-message">⚡ {warning}</div>
              ))}
            </div>
          )}
        </div>

        <div className="editor-footer">
          <button
            type="button"
            className="btn-cancel"
            onClick={onCancel}
          >
            Cancel
          </button>
          <button
            type="button"
            className="btn-save"
            onClick={handleSave}
            disabled={!validation?.valid || readOnly}
          >
            {task ? 'Update Task' : 'Create Task'}
          </button>
        </div>
      </div>
    </div>
  );
};
