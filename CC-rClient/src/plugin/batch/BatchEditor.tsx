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
import type { CommandScript } from '../script/types';
import type { StationGroup } from '../groups/types';

function mapScriptParametersToBatchParameters(script: CommandScript): BatchTaskParameter[] {
  return (script.parameters || []).map((parameter) => ({
    name: parameter.name,
    paramType: parameter.paramType,
    defaultValue: parameter.defaultValue,
    required: parameter.required,
    validation: parameter.validation,
    description: parameter.description,
    options: parameter.options,
  }));
}

// ============================================
// Target Selector Component
// ============================================

interface TargetSelectorEditorProps {
  selector: TargetSelector;
  targets: BatchTarget[];
  groups: StationGroup[];
  onChange: (selector: TargetSelector) => void;
  showPreview?: boolean;
  disabled?: boolean;
}

const TargetSelectorEditor: React.FC<TargetSelectorEditorProps> = ({
  selector,
  targets,
  groups,
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

  const availableGroups = useMemo(
    () => [...groups].sort((left, right) => left.name.localeCompare(right.name)),
    [groups],
  );

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
        setPreviewCount(
          targets.filter((target) =>
            selector.groups?.some((groupId) => target.groups?.includes(groupId)),
          ).length,
        );
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
                  <label key={group.id} className="group-item">
                    <input
                      type="checkbox"
                      checked={selector.groups?.includes(group.id) || false}
                      onChange={() => handleGroupToggle(group.id)}
                      disabled={disabled}
                    />
                    <span>{group.name}</span>
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
  groups: StationGroup[];
  scripts: CommandScript[];
  onSave: (task: Partial<BatchTask>) => void;
  onCancel: () => void;
  onValidate?: (task: Partial<BatchTask>) => Promise<ValidateTaskResult>;
  readOnly?: boolean;
}

export const BatchTaskEditor: React.FC<BatchTaskEditorProps> = ({
  task,
  targets,
  groups,
  scripts,
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
  const [selectedScriptId, setSelectedScriptId] = useState('');
  const [scriptSearchQuery, setScriptSearchQuery] = useState('');
  const [validation, setValidation] = useState<ValidateTaskResult | null>(null);
  const [showAdvanced, setShowAdvanced] = useState(false);

  const selectedScript = useMemo(
    () => scripts.find((script) => script.id === selectedScriptId) ?? null,
    [scripts, selectedScriptId],
  );

  const filteredScripts = useMemo(() => {
    const query = scriptSearchQuery.trim().toLowerCase();
    if (!query) {
      return scripts;
    }

    return scripts.filter((script) => script.name.toLowerCase().includes(query));
  }, [scriptSearchQuery, scripts]);

  React.useEffect(() => {
    if (taskType !== 'script') {
      return;
    }

    if (selectedScriptId) {
      return;
    }

    const matchedScript = scripts.find((script) => script.content === content);
    if (matchedScript) {
      setSelectedScriptId(matchedScript.id);
      if ((!task?.parameters || task.parameters.length === 0) && parameters.length === 0) {
        setParameters(mapScriptParametersToBatchParameters(matchedScript));
      }
    }
  }, [content, parameters.length, scripts, selectedScriptId, task?.parameters, taskType]);

  // Task types
  const taskTypes: { key: BatchTaskType; label: string; icon: string }[] = [
    { key: 'power_on', label: 'Power On', icon: '⚡' },
    { key: 'shutdown', label: 'Shutdown', icon: '⏻' },
    { key: 'reboot', label: 'Reboot', icon: '🔄' },
    { key: 'start_app', label: 'Start App', icon: '🚀' },
    { key: 'command', label: 'Command', icon: '💻' },
    { key: 'watch_processes', label: 'Watch Processes', icon: '👁' },
    { key: 'script', label: 'Script', icon: '📜' },
  ];

  // Validate task
  const validateTask = useCallback((): ValidateTaskResult => {
    const errors: string[] = [];
    const warnings: string[] = [];

    if (!name.trim()) {
      errors.push('Task name is required');
    }

    const needsContent = taskType === 'command' || taskType === 'script' || taskType === 'watch_processes';

    if (needsContent && !content.trim()) {
      errors.push('Task content is required');
    }

    if (taskType === 'script' && !selectedScriptId && !content.trim()) {
      errors.push('Select a script to run');
    }

    if (taskType === 'watch_processes' && content.trim() && !content.includes(',') && !content.includes('\n')) {
      warnings.push('Multiple watched processes are usually separated by commas or new lines');
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
        content: taskType === 'script' ? (selectedScript?.content ?? content) : content,
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
      content: taskType === 'script' ? (selectedScript?.content ?? content) : content,
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
    <div className="batch-task-editor">
      <div className="editor-header">
        <h2>{task ? 'Edit Batch Task' : 'Create New Batch Task'}</h2>
        <div className="editor-actions">
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

      <div className="editor-body">
          {/* Basic Info Section */}
          <div className="editor-section metadata-section">
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
              groups={groups}
              onChange={setTargetSelector}
              disabled={readOnly}
            />
          </div>

          {/* Content Editor Section */}
          <div className="editor-section content-section">
            <h3>Task Content</h3>
            {taskType === 'script' ? (
              <div className="script-selector">
                <div className="field-group">
                  <input
                    type="text"
                    className="field-input"
                    placeholder="Search scripts by name..."
                    value={scriptSearchQuery}
                    onChange={(e) => setScriptSearchQuery(e.target.value)}
                    disabled={readOnly}
                  />
                </div>

                <div className="script-selector-list">
                  {filteredScripts.length === 0 ? (
                    <div className="script-selector-empty">
                      {scripts.length === 0 ? 'No scripts available yet. Create one on the Scripts page first.' : 'No scripts match the current filter.'}
                    </div>
                  ) : (
                    filteredScripts.map((script) => (
                      <button
                        key={script.id}
                        type="button"
                      className={`script-option ${selectedScriptId === script.id ? 'active' : ''}`}
                      onClick={() => {
                        setSelectedScriptId(script.id);
                        setContent(script.content);
                        setParameters(mapScriptParametersToBatchParameters(script));
                      }}
                      disabled={readOnly}
                    >
                        <div className="script-option-header">
                          <strong>{script.name}</strong>
                          <span>{script.scriptType}</span>
                        </div>
                        <p>{script.description || 'No description'}</p>
                      </button>
                    ))
                  )}
                </div>

                {selectedScript ? (
                  <div className="script-selection-summary">
                    <span className="summary-label">Selected script:</span>
                    <strong>{selectedScript.name}</strong>
                    <span className="summary-type">{selectedScript.scriptType}</span>
                  </div>
                ) : content.trim() ? (
                  <div className="script-selection-summary script-selection-summary--legacy">
                    This task currently contains saved script content that is not linked to an existing script. Select a script above to relink it.
                  </div>
                ) : null}
              </div>
            ) : (
              <textarea
                className="content-editor"
                placeholder={
                  taskType === 'power_on' ? 'Wake-on-LAN does not require extra content.' :
                  taskType === 'shutdown' ? 'Shutdown uses the station control RPC and does not require extra content.' :
                  taskType === 'reboot' ? 'Reboot uses the station control RPC and does not require extra content.' :
                  taskType === 'start_app' ? 'Start App uses each station’s configured startup programs.' :
                  taskType === 'command' ? 'Enter command to execute...' :
                  'Enter process names separated by commas or new lines...'
                }
                value={content}
                onChange={(e) => setContent(e.target.value)}
                disabled={readOnly}
                rows={10}
              />
            )}
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
            <div className="validation-section">
              <div className="validation-errors">
                <h4>Errors</h4>
                <ul>
                  {validation.errors.map((error: string, i: number) => (
                    <li key={i}>{error}</li>
                  ))}
                </ul>
              </div>
            </div>
          )}
          {validation && validation.warnings.length > 0 && (
            <div className="validation-section">
              <div className="validation-warnings">
                <h4>Warnings</h4>
                <ul>
                  {validation.warnings.map((warning: string, i: number) => (
                    <li key={i}>{warning}</li>
                  ))}
                </ul>
              </div>
            </div>
          )}
      </div>

      <style>{`
        .batch-task-editor {
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

        .btn-cancel, .btn-save {
          padding: 8px 14px;
          border-radius: 8px;
          font-size: 0.84rem;
          font-weight: 500;
          cursor: pointer;
          transition: all 0.2s;
        }

        .btn-cancel {
          background: transparent;
          border: 1px solid var(--border-color);
          color: var(--text-muted);
        }

        .btn-cancel:hover {
          border-color: var(--primary);
          color: var(--text-main);
        }

        .btn-save {
          background: var(--primary);
          border: 1px solid var(--primary);
          color: white;
        }

        .btn-save:hover:not(:disabled) {
          background: var(--primary-hover);
          border-color: var(--primary-hover);
        }

        .btn-save:disabled,
        .btn-add-param:disabled,
        .task-type-btn:disabled,
        .selector-tab:disabled {
          opacity: 0.5;
          cursor: not-allowed;
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
          padding: 16px;
          background: var(--bg-card);
          border-radius: 12px;
          border: 1px solid var(--border-color);
          box-shadow: 0 8px 20px rgba(11, 25, 44, 0.05);
        }

        .editor-section h3 {
          margin: 0 0 14px;
          font-size: 0.95rem;
          font-weight: 600;
        }

        .metadata-section {
          display: grid;
          gap: 14px;
        }

        .field-group {
          display: flex;
          flex-direction: column;
          gap: 6px;
        }

        .field-label,
        .section-label {
          font-size: 0.78rem;
          font-weight: 500;
          color: var(--text-muted);
          text-transform: uppercase;
          letter-spacing: 0.04em;
        }

        .required {
          color: #ef4444;
        }

        .field-input,
        .field-textarea,
        .content-editor,
        .batch-size-input,
        .device-ids-input,
        .filter-expr-input,
        .param-name,
        .param-type,
        .param-default,
        .param-description,
        .param-options,
        .retry-count input,
        .retry-delay input,
        .timeout-input input {
          width: 100%;
          padding: 10px 12px;
          background: var(--bg-main);
          border: 1px solid var(--border-color);
          border-radius: 8px;
          color: var(--text-main);
          font-size: 0.86rem;
          transition: border-color 0.2s;
        }

        .field-input:focus,
        .field-textarea:focus,
        .content-editor:focus,
        .batch-size-input:focus,
        .device-ids-input:focus,
        .filter-expr-input:focus,
        .param-name:focus,
        .param-type:focus,
        .param-default:focus,
        .param-description:focus,
        .param-options:focus,
        .retry-count input:focus,
        .retry-delay input:focus,
        .timeout-input input:focus {
          outline: none;
          border-color: var(--primary);
        }

        .field-textarea,
        .content-editor,
        .device-ids-input {
          resize: vertical;
        }

        .task-type-selector,
        .selector-type-tabs,
        .mode-selector {
          display: flex;
          flex-wrap: wrap;
          gap: 10px;
        }

        .task-type-btn,
        .selector-tab,
        .mode-option {
          display: inline-flex;
          align-items: center;
          gap: 8px;
          padding: 9px 12px;
          border: 1px solid var(--border-color);
          border-radius: 10px;
          background: var(--bg-main);
          color: var(--text-muted);
          cursor: pointer;
          transition: all 0.2s;
          font-size: 0.83rem;
        }

        .task-type-btn.active,
        .selector-tab.active,
        .mode-option.selected {
          background: var(--primary);
          border-color: var(--primary);
          color: white;
        }

        .selector-tab input,
        .mode-option input {
          display: none;
        }

        .selector-content,
        .policy-section + .policy-section,
        .parameter-editor {
          margin-top: 14px;
        }

        .group-list,
        .tag-list,
        .status-filter,
        .parameters-list {
          display: flex;
          flex-wrap: wrap;
          gap: 10px;
        }

        .group-item,
        .tag-item {
          display: inline-flex;
          align-items: center;
          gap: 8px;
          padding: 8px 10px;
          border: 1px solid var(--border-color);
          border-radius: 8px;
          background: var(--bg-main);
          cursor: pointer;
        }

        .selector-preview,
        .mode-description,
        .input-hint {
          margin-top: 10px;
          font-size: 0.74rem;
          color: var(--text-muted);
        }

        .preview-count,
        .threshold-value {
          font-weight: 600;
          color: var(--text-main);
          margin-left: 6px;
        }

        .policy-row,
        .threshold-input,
        .timeout-input,
        .retry-input,
        .parameter-row,
        .parameter-advanced,
        .tags-list {
          display: flex;
          gap: 10px;
          align-items: center;
          flex-wrap: wrap;
        }

        .script-selector {
          display: grid;
          gap: 14px;
        }

        .script-selector-list {
          display: grid;
          gap: 10px;
          max-height: 320px;
          overflow-y: auto;
          padding-right: 4px;
        }

        .script-option {
          display: grid;
          gap: 6px;
          text-align: left;
          padding: 12px 14px;
          border: 1px solid var(--border-color);
          border-radius: 10px;
          background: var(--bg-main);
          cursor: pointer;
          transition: all 0.2s;
        }

        .script-option:hover {
          border-color: var(--primary);
        }

        .script-option.active {
          border-color: var(--primary);
          background: rgba(45, 140, 240, 0.08);
          box-shadow: inset 0 0 0 1px rgba(45, 140, 240, 0.18);
        }

        .script-option-header {
          display: flex;
          justify-content: space-between;
          gap: 12px;
          align-items: center;
        }

        .script-option-header strong {
          font-size: 0.88rem;
          color: var(--text-main);
        }

        .script-option-header span,
        .summary-type {
          font-size: 0.74rem;
          text-transform: uppercase;
          letter-spacing: 0.04em;
          color: var(--text-muted);
        }

        .script-option p {
          margin: 0;
          font-size: 0.8rem;
          color: var(--text-muted);
          line-height: 1.4;
        }

        .script-selector-empty,
        .script-selection-summary {
          padding: 12px 14px;
          border-radius: 10px;
          border: 1px solid var(--border-color);
          background: var(--bg-main);
          font-size: 0.8rem;
          color: var(--text-muted);
        }

        .script-selection-summary {
          display: flex;
          gap: 8px;
          align-items: center;
          flex-wrap: wrap;
        }

        .summary-label {
          color: var(--text-muted);
        }

        .script-selection-summary--legacy {
          background: rgba(234, 179, 8, 0.1);
          border-color: #eab308;
          color: #a16207;
        }

        .checkbox-label,
        .param-required {
          display: inline-flex;
          align-items: center;
          gap: 8px;
          color: var(--text-main);
          cursor: pointer;
        }

        .retry-count,
        .retry-delay {
          display: flex;
          align-items: center;
          gap: 8px;
          min-width: 220px;
        }

        .retry-count label,
        .retry-delay label {
          font-size: 0.8rem;
          color: var(--text-muted);
          white-space: nowrap;
        }

        .parameter-editor-header,
        .section-toggle {
          display: flex;
          justify-content: space-between;
          align-items: center;
          width: 100%;
          gap: 12px;
        }

        .section-toggle {
          border: none;
          background: transparent;
          padding: 0;
          cursor: pointer;
          font-size: 0.92rem;
          font-weight: 600;
          color: var(--text-main);
        }

        .toggle-icon {
          color: var(--text-muted);
        }

        .btn-add-param,
        .btn-remove-param {
          padding: 6px 12px;
          border-radius: 8px;
          border: 1px solid var(--border-color);
          background: var(--bg-main);
          color: var(--text-main);
          cursor: pointer;
        }

        .btn-add-param {
          background: var(--primary);
          border-color: var(--primary);
          color: white;
        }

        .parameter-item {
          width: 100%;
          padding: 12px;
          background: var(--bg-main);
          border: 1px solid var(--border-color);
          border-radius: 10px;
        }

        .btn-remove-param,
        .tag-remove {
          display: inline-flex;
          align-items: center;
          justify-content: center;
          width: 28px;
          height: 28px;
          padding: 0;
        }

        .tags-input-container {
          background: var(--bg-main);
          border: 1px solid var(--border-color);
          border-radius: 8px;
          padding: 8px;
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

        .tag-input {
          flex: 1;
          min-width: 120px;
          background: transparent;
          border: none;
          color: var(--text-main);
          font-size: 14px;
          padding: 4px;
        }

        .tag-input:focus {
          outline: none;
        }

        .validation-section {
          padding: 0;
          border-radius: 6px;
        }

        .validation-errors,
        .validation-warnings {
          border-radius: 10px;
          padding: 12px 14px;
        }

        .validation-errors {
          background: rgba(239, 68, 68, 0.1);
          border: 1px solid #ef4444;
        }

        .validation-warnings {
          background: rgba(234, 179, 8, 0.1);
          border: 1px solid #eab308;
        }

        .validation-errors h4,
        .validation-warnings h4 {
          margin: 0 0 8px;
          font-size: 0.84rem;
        }

        .validation-errors h4,
        .validation-errors li {
          color: #ef4444;
        }

        .validation-warnings h4,
        .validation-warnings li {
          color: #a16207;
        }

        .validation-errors ul,
        .validation-warnings ul {
          margin: 0;
          padding-left: 18px;
        }

        .validation-errors li,
        .validation-warnings li {
          font-size: 0.8rem;
          margin: 4px 0;
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

          .retry-count,
          .retry-delay {
            min-width: 100%;
          }
        }
      `}</style>
    </div>
  );
};
