/**
 * TagEditor - Component for creating and editing tag definitions
 * Part of Phase 8: Device Group and Tag System
 */

import React, { useState, useCallback } from 'react';
import type { TagDefinition, TagValueType, CreateTagDTO, UpdateTagDTO } from './types';

export interface TagEditorProps {
  tag?: TagDefinition | null;
  onSave: (data: CreateTagDTO | UpdateTagDTO) => void;
  onCancel: () => void;
  isLoading?: boolean;
}

const TAG_TYPES: Array<{ value: TagValueType; label: string; description: string }> = [
  { value: 'string', label: 'Text', description: 'Free-form text value' },
  { value: 'number', label: 'Number', description: 'Numeric value' },
  { value: 'boolean', label: 'Yes/No', description: 'True or false value' },
  { value: 'select', label: 'Select', description: 'Choose from a list of options' },
];

// TagDefinitionList - Component for displaying tag definitions list
export const TagDefinitionList: React.FC<{
  tagDefinitions: import('./types').TagDefinition[];
  stats: Array<{ tagKey: string; tagLabel: string; tagValue: string; count: number }>;
  selectedTag: import('./types').TagDefinition | null;
  onSelectTag: (tag: import('./types').TagDefinition | null) => void;
  onEditTag: (tag: import('./types').TagDefinition) => void;
  onDeleteTag: (key: string) => void;
  onCreateTag: () => void;
  onImport: (tags: import('./types').TagDefinition[]) => void;
  onExport: () => void;
}> = ({ tagDefinitions, stats, selectedTag, onSelectTag, onEditTag, onDeleteTag, onCreateTag, onImport, onExport }) => {
  const [searchTerm, setSearchTerm] = React.useState('');

  const filteredTags = tagDefinitions.filter(
    t => (t.label || t.name || '').toLowerCase().includes(searchTerm.toLowerCase()) ||
         (t.key || t.id || '').toLowerCase().includes(searchTerm.toLowerCase())
  );

  const handleImportClick = () => {
    const input = document.createElement('input');
    input.type = 'file';
    input.accept = '.json';
    input.onchange = async (e) => {
      const file = (e.target as HTMLInputElement).files?.[0];
      if (!file) return;
      try {
        const text = await file.text();
        const imported = JSON.parse(text) as import('./types').TagDefinition[];
        onImport(imported);
      } catch (err) {
        console.error('[TagDefinitionList] Failed to parse import:', err);
      }
    };
    input.click();
  };

  return (
    <div className="tag-def-list">
      <div className="tag-list-header">
        <input
          type="text"
          placeholder="Search tags..."
          value={searchTerm}
          onChange={(e) => setSearchTerm(e.target.value)}
          className="tag-search"
        />
        <div className="tag-header-actions">
          <button className="tag-btn" onClick={handleImportClick}>Import</button>
          <button className="tag-btn" onClick={onExport}>Export</button>
          <button className="tag-btn primary" onClick={onCreateTag}>+ New Tag</button>
        </div>
      </div>
      <div className="tag-cards">
        {filteredTags.length === 0 ? (
          <div className="tag-empty">
            {tagDefinitions.length === 0 ? (
              <p>No tag definitions yet</p>
            ) : <p>No tags match your search</p>}
          </div>
        ) : filteredTags.map(tag => {
          const tagKey = tag.key || tag.id;
          const tagLabel = tag.label || tag.name || tagKey;
          const selectedTagKey = selectedTag?.key || selectedTag?.id;
          return (
          <div key={tagKey} className={`tag-card ${selectedTagKey === tagKey ? 'selected' : ''}`}>
            <div className="tag-card-header">
              <span className="tag-key">{tagKey}</span>
              <span className="tag-type">{tag.type || 'string'}</span>
            </div>
            <div className="tag-card-label">{tagLabel}</div>
            {tag.description && <div className="tag-card-desc">{tag.description}</div>}
            {tag.options && tag.options.length > 0 && (
              <div className="tag-options">{tag.options.join(', ')}</div>
            )}
            <div className="tag-card-actions">
              <button className="tag-action-btn" onClick={() => onSelectTag(tag)}>Select</button>
              <button className="tag-action-btn" onClick={() => onEditTag(tag)}>Edit</button>
              <button className="tag-action-btn delete" onClick={() => onDeleteTag(tagKey)}>Delete</button>
            </div>
          </div>
        )})}
      </div>
      <style>{`
        .tag-def-list { display: flex; flex-direction: column; height: 100%; }
        .tag-list-header { display: flex; justify-content: space-between; padding: 12px 16px; border-bottom: 1px solid var(--border-color); gap: 12px; }
        .tag-search { padding: 8px 12px; border: 1px solid var(--border-color); border-radius: 6px; font-size: 0.85rem; width: 200px; background: var(--bg-main); color: var(--text-main); }
        .tag-search:focus { outline: none; border-color: var(--primary); }
        .tag-header-actions { display: flex; gap: 8px; }
        .tag-btn { padding: 8px 14px; border: 1px solid var(--border-color); border-radius: 6px; background: transparent; color: var(--text-main); font-size: 0.82rem; cursor: pointer; }
        .tag-btn:hover { border-color: var(--primary); color: var(--primary); }
        .tag-btn.primary { background: var(--primary); border-color: var(--primary); color: white; }
        .tag-cards { flex: 1; overflow-y: auto; padding: 16px; display: grid; grid-template-columns: repeat(auto-fill, minmax(220px, 1fr)); gap: 12px; }
        .tag-card { background: var(--bg-card); border: 1px solid var(--border-color); border-radius: 8px; padding: 12px; cursor: pointer; transition: all 0.2s; }
        .tag-card:hover { border-color: var(--primary); }
        .tag-card.selected { border-color: var(--primary); background: rgba(99,102,241,0.05); }
        .tag-card-header { display: flex; justify-content: space-between; margin-bottom: 6px; }
        .tag-key { font-family: monospace; font-size: 0.8rem; color: var(--primary); }
        .tag-type { font-size: 0.7rem; padding: 2px 6px; background: var(--bg-main); border-radius: 4px; color: var(--text-secondary); }
        .tag-card-label { font-size: 0.9rem; font-weight: 600; color: var(--text-main); margin-bottom: 4px; }
        .tag-card-desc { font-size: 0.75rem; color: var(--text-secondary); margin-bottom: 6px; }
        .tag-options { font-size: 0.7rem; color: var(--text-secondary); font-style: italic; }
        .tag-card-actions { display: flex; gap: 8px; margin-top: 8px; }
        .tag-action-btn { padding: 4px 10px; border: 1px solid var(--border-color); border-radius: 4px; background: transparent; font-size: 0.75rem; cursor: pointer; color: var(--text-main); }
        .tag-action-btn:hover { border-color: var(--primary); color: var(--primary); }
        .tag-action-btn.delete:hover { border-color: #ef4444; color: #ef4444; }
        .tag-empty { display: flex; align-items: center; justify-content: center; height: 150px; color: var(--text-secondary); }
      `}</style>
    </div>
  );
};

export const TagEditor: React.FC<TagEditorProps> = ({
  tag,
  onSave,
  onCancel,
  isLoading = false,
}) => {
  const [key, setKey] = useState(tag?.key || '');
  const [label, setLabel] = useState(tag?.label || '');
  const [type, setType] = useState<TagValueType>(tag?.type || 'string');
  const [options, setOptions] = useState<string[]>(tag?.options || []);
  const [optionInput, setOptionInput] = useState('');
  const [required, setRequired] = useState(tag?.required || false);
  const [defaultValue, setDefaultValue] = useState(tag?.defaultValue || '');
  const [description, setDescription] = useState(tag?.description || '');
  const [errors, setErrors] = useState<string[]>([]);

  // Validate form
  const validate = useCallback((): boolean => {
    const validationErrors: string[] = [];

    if (!key.trim()) {
      validationErrors.push('Tag key is required');
    } else if (!/^[a-zA-Z][a-zA-Z0-9_]*$/.test(key.trim())) {
      validationErrors.push('Tag key must start with a letter and contain only letters, numbers, and underscores');
    }

    if (!label.trim()) {
      validationErrors.push('Tag label is required');
    }

    if (type === 'select' && options.length === 0) {
      validationErrors.push('Select type requires at least one option');
    }

    if (key.length > 30) {
      validationErrors.push('Tag key must be 30 characters or less');
    }

    if (label.length > 50) {
      validationErrors.push('Tag label must be 50 characters or less');
    }

    setErrors(validationErrors);
    return validationErrors.length === 0;
  }, [key, label, type, options]);

  // Handle option input
  const handleOptionKeyDown = (e: React.KeyboardEvent<HTMLInputElement>) => {
    if (e.key === 'Enter' || e.key === ',') {
      e.preventDefault();
      const newOption = optionInput.trim();
      if (newOption && !options.includes(newOption)) {
        setOptions([...options, newOption]);
      }
      setOptionInput('');
    } else if (e.key === 'Backspace' && !optionInput && options.length > 0) {
      setOptions(options.slice(0, -1));
    }
  };

  const removeOption = (optionToRemove: string) => {
    setOptions(options.filter(o => o !== optionToRemove));
  };

  // Handle save
  const handleSave = () => {
    if (!validate()) return;

    onSave({
      name: label.trim(),
      type,
      options: type === 'select' ? options : undefined,
      required,
      defaultValue: defaultValue.trim() || undefined,
      description: description.trim() || undefined,
    });
  };

  return (
    <div className="tag-editor">
      <div className="editor-header">
        <h2>{tag ? 'Edit Tag Definition' : 'Create New Tag'}</h2>
        <div className="editor-actions">
          <button
            type="button"
            className="btn-cancel"
            onClick={onCancel}
            disabled={isLoading}
          >
            Cancel
          </button>
          <button
            type="button"
            className="btn-save"
            onClick={handleSave}
            disabled={isLoading}
          >
            {isLoading ? 'Saving...' : tag ? 'Update Tag' : 'Create Tag'}
          </button>
        </div>
      </div>

      <div className="editor-body">
        {/* Error messages */}
        {errors.length > 0 && (
          <div className="validation-errors">
            <h4>Please fix the following errors:</h4>
            <ul>
              {errors.map((error, index) => (
                <li key={index}>{error}</li>
              ))}
            </ul>
          </div>
        )}

        {/* Key and Label */}
        <div className="editor-section">
          <div className="field-row">
            <div className="field-group">
              <label className="field-label">
                Tag Key <span className="required">*</span>
              </label>
              <input
                type="text"
                className="field-input"
                placeholder="e.g., environment, role"
                value={key}
                onChange={(e) => setKey(e.target.value.toLowerCase().replace(/[^a-zA-Z0-9_]/g, ''))}
                disabled={isLoading || !!tag}
                maxLength={30}
              />
              <span className="field-hint">Used in API and code. Cannot be changed later.</span>
            </div>

            <div className="field-group">
              <label className="field-label">
                Display Label <span className="required">*</span>
              </label>
              <input
                type="text"
                className="field-input"
                placeholder="e.g., Environment, Role"
                value={label}
                onChange={(e) => setLabel(e.target.value)}
                disabled={isLoading}
                maxLength={50}
              />
            </div>
          </div>
        </div>

        {/* Type selection */}
        <div className="editor-section">
          <h3 className="section-title">Value Type</h3>
          <div className="type-selector">
            {TAG_TYPES.map((t) => (
              <label
                key={t.value}
                className={`type-option ${type === t.value ? 'selected' : ''}`}
              >
                <input
                  type="radio"
                  name="tagType"
                  value={t.value}
                  checked={type === t.value}
                  onChange={() => setType(t.value)}
                  disabled={isLoading}
                />
                <span className="type-label">{t.label}</span>
                <span className="type-description">{t.description}</span>
              </label>
            ))}
          </div>
        </div>

        {/* Options for select type */}
        {type === 'select' && (
          <div className="editor-section">
            <div className="field-group">
              <label className="field-label">Options</label>
              <div className="tag-input-container">
                <div className="tags-list">
                  {options.map((option) => (
                    <span key={option} className="tag">
                      {option}
                      <button
                        type="button"
                        className="tag-remove"
                        onClick={() => removeOption(option)}
                        disabled={isLoading}
                      >
                        ×
                      </button>
                    </span>
                  ))}
                  <input
                    type="text"
                    className="tag-input"
                    placeholder={options.length === 0 ? 'Add options...' : ''}
                    value={optionInput}
                    onChange={(e) => setOptionInput(e.target.value)}
                    onKeyDown={handleOptionKeyDown}
                    disabled={isLoading}
                  />
                </div>
              </div>
              <span className="field-hint">Press Enter or comma to add an option</span>
            </div>
          </div>
        )}

        {/* Default value */}
        <div className="editor-section">
          <h3 className="section-title">Settings</h3>
          
          <div className="field-row">
            <div className="field-group">
              <label className="field-label">Default Value</label>
              <input
                type="text"
                className="field-input"
                placeholder="Optional default value"
                value={defaultValue}
                onChange={(e) => setDefaultValue(e.target.value)}
                disabled={isLoading}
              />
            </div>

            <div className="field-group">
              <label className="field-checkbox">
                <input
                  type="checkbox"
                  checked={required}
                  onChange={(e) => setRequired(e.target.checked)}
                  disabled={isLoading}
                />
                <span>Required</span>
              </label>
            </div>
          </div>

          <div className="field-group">
            <label className="field-label">Description</label>
            <textarea
              className="field-textarea"
              placeholder="Describe what this tag is used for (optional)"
              value={description}
              onChange={(e) => setDescription(e.target.value)}
              disabled={isLoading}
              rows={2}
            />
          </div>
        </div>
      </div>

      <style>{`
        .tag-editor {
          display: flex;
          flex-direction: column;
        }

        .editor-header {
          display: flex;
          justify-content: space-between;
          align-items: center;
          padding: 16px 20px;
          border-bottom: 1px solid var(--border-color);
          background: var(--bg-card);
        }

        .editor-header h2 {
          margin: 0;
          font-size: 1.1rem;
          font-weight: 600;
        }

        .editor-actions {
          display: flex;
          gap: 10px;
        }

        .btn-cancel, .btn-save {
          padding: 8px 16px;
          border-radius: 6px;
          font-size: 0.86rem;
          font-weight: 500;
          cursor: pointer;
          transition: all 0.2s;
        }

        .btn-cancel {
          background: transparent;
          border: 1px solid var(--border-color);
          color: var(--text-main);
        }

        .btn-cancel:hover:not(:disabled) {
          background: var(--bg-main);
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

        .btn-cancel:disabled, .btn-save:disabled {
          opacity: 0.6;
          cursor: not-allowed;
        }

        .editor-body {
          padding: 20px;
          max-height: calc(90vh - 140px);
          overflow-y: auto;
        }

        .editor-section {
          margin-bottom: 20px;
        }

        .section-title {
          font-size: 0.82rem;
          font-weight: 500;
          color: var(--text-muted);
          text-transform: uppercase;
          letter-spacing: 0.04em;
          margin: 0 0 12px 0;
        }

        .field-row {
          display: flex;
          gap: 16px;
        }

        .field-row .field-group {
          flex: 1;
        }

        .field-group {
          display: flex;
          flex-direction: column;
          gap: 6px;
          margin-bottom: 14px;
        }

        .field-label {
          font-size: 0.78rem;
          font-weight: 500;
          color: var(--text-muted);
          text-transform: uppercase;
          letter-spacing: 0.04em;
        }

        .field-label .required {
          color: #ef4444;
        }

        .field-input, .field-textarea {
          padding: 10px 12px;
          background: var(--bg-main);
          border: 1px solid var(--border-color);
          border-radius: 8px;
          color: var(--text-main);
          font-size: 0.86rem;
          transition: border-color 0.2s;
        }

        .field-input:focus, .field-textarea:focus {
          outline: none;
          border-color: var(--primary);
        }

        .field-input:disabled, .field-textarea:disabled {
          opacity: 0.6;
          cursor: not-allowed;
        }

        .field-textarea {
          resize: vertical;
          min-height: 60px;
        }

        .field-checkbox {
          display: flex;
          align-items: center;
          gap: 8px;
          cursor: pointer;
          padding-top: 28px;
        }

        .field-checkbox input {
          width: 18px;
          height: 18px;
          cursor: pointer;
        }

        .field-hint {
          font-size: 0.74rem;
          color: var(--text-muted);
        }

        .type-selector {
          display: grid;
          grid-template-columns: repeat(2, 1fr);
          gap: 10px;
        }

        .type-option {
          display: flex;
          flex-direction: column;
          gap: 4px;
          padding: 12px;
          background: var(--bg-main);
          border: 1px solid var(--border-color);
          border-radius: 8px;
          cursor: pointer;
          transition: all 0.2s;
        }

        .type-option:hover {
          border-color: var(--primary);
        }

        .type-option.selected {
          border-color: var(--primary);
          background: rgba(45, 140, 240, 0.08);
        }

        .type-option input {
          display: none;
        }

        .type-label {
          font-weight: 500;
          color: var(--text-main);
        }

        .type-description {
          font-size: 0.78rem;
          color: var(--text-muted);
        }

        .tag-input-container {
          background: var(--bg-main);
          border: 1px solid var(--border-color);
          border-radius: 8px;
          padding: 8px;
        }

        .tags-list {
          display: flex;
          flex-wrap: wrap;
          gap: 6px;
        }

        .tag {
          display: inline-flex;
          align-items: center;
          gap: 4px;
          padding: 4px 9px;
          background: rgba(45, 140, 240, 0.1);
          color: #1f4e89;
          border: 1px solid rgba(45, 140, 240, 0.2);
          border-radius: 4px;
          font-size: 12px;
        }

        .tag-remove {
          background: none;
          border: none;
          color: white;
          cursor: pointer;
          padding: 0;
          font-size: 14px;
          line-height: 1;
          opacity: 0.8;
        }

        .tag-remove:hover {
          opacity: 1;
        }

        .tag-input {
          flex: 1;
          min-width: 100px;
          background: transparent;
          border: none;
          color: var(--text-main);
          font-size: 14px;
          padding: 4px;
        }

        .tag-input:focus {
          outline: none;
        }

        .validation-errors {
          background: rgba(239, 68, 68, 0.1);
          border: 1px solid #ef4444;
          border-radius: 6px;
          padding: 12px;
          margin-bottom: 16px;
        }

        .validation-errors h4 {
          margin: 0 0 8px 0;
          color: #ef4444;
          font-size: 14px;
        }

        .validation-errors ul {
          margin: 0;
          padding-left: 20px;
        }

        .validation-errors li {
          color: #ef4444;
          font-size: 13px;
          margin: 4px 0;
        }

        @media (max-width: 560px) {
          .field-row {
            flex-direction: column;
          }

          .type-selector {
            grid-template-columns: 1fr;
          }
        }
      `}</style>
    </div>
  );
};