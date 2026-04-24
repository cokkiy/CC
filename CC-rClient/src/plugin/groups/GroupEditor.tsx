/**
 * GroupEditor - Component for creating and editing station groups
 * Part of Phase 8: Device Group and Tag System
 */

import React, { useState, useCallback } from 'react';
import type { StationGroup, CreateGroupDTO, UpdateGroupDTO } from './types';

export interface GroupEditorProps {
  group?: StationGroup | null;
  onSave: (data: CreateGroupDTO | UpdateGroupDTO) => void;
  onCancel: () => void;
  isLoading?: boolean;
}

// Predefined colors for groups
const GROUP_COLORS = [
  '#3b82f6', // blue
  '#10b981', // green
  '#f59e0b', // amber
  '#ef4444', // red
  '#8b5cf6', // violet
  '#ec4899', // pink
  '#06b6d4', // cyan
  '#84cc16', // lime
  '#f97316', // orange
  '#6366f1', // indigo
];

export const GroupEditor: React.FC<GroupEditorProps> = ({
  group,
  onSave,
  onCancel,
  isLoading = false,
}) => {
  const [name, setName] = useState(group?.name || '');
  const [description, setDescription] = useState(group?.description || '');
  const [color, setColor] = useState(group?.color || GROUP_COLORS[0]);
  const [icon, setIcon] = useState(group?.icon || '');
  const [tags, setTags] = useState<string[]>(group?.tags || []);
  const [tagInput, setTagInput] = useState('');
  const [errors, setErrors] = useState<string[]>([]);

  // Validate form
  const validate = useCallback((): boolean => {
    const validationErrors: string[] = [];

    if (!name.trim()) {
      validationErrors.push('Group name is required');
    }

    if (name.length > 50) {
      validationErrors.push('Group name must be 50 characters or less');
    }

    if (description.length > 200) {
      validationErrors.push('Description must be 200 characters or less');
    }

    setErrors(validationErrors);
    return validationErrors.length === 0;
  }, [name, description]);

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
  const handleSave = () => {
    if (!validate()) return;

    onSave({
      name: name.trim(),
      description: description.trim(),
      color,
      icon: icon.trim() || undefined,
      tags,
    });
  };

  return (
    <div className="group-editor">
      <div className="editor-header">
        <h2>{group ? 'Edit Group' : 'Create New Group'}</h2>
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
            {isLoading ? 'Saving...' : group ? 'Update Group' : 'Create Group'}
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

        {/* Name field */}
        <div className="editor-section">
          <div className="field-group">
            <label className="field-label">
              Group Name <span className="required">*</span>
            </label>
            <input
              type="text"
              className="field-input"
              placeholder="Enter group name"
              value={name}
              onChange={(e) => setName(e.target.value)}
              disabled={isLoading}
              maxLength={50}
            />
          </div>

          <div className="field-group">
            <label className="field-label">Description</label>
            <textarea
              className="field-textarea"
              placeholder="Describe this group (optional)"
              value={description}
              onChange={(e) => setDescription(e.target.value)}
              disabled={isLoading}
              rows={3}
              maxLength={200}
            />
          </div>
        </div>

        {/* Color and Icon section */}
        <div className="editor-section">
          <h3 className="section-title">Appearance</h3>
          
          <div className="field-group">
            <label className="field-label">Color</label>
            <div className="color-picker">
              {GROUP_COLORS.map((c) => (
                <button
                  key={c}
                  type="button"
                  className={`color-swatch ${color === c ? 'selected' : ''}`}
                  style={{ backgroundColor: c }}
                  onClick={() => setColor(c)}
                  disabled={isLoading}
                  aria-label={`Select color ${c}`}
                />
              ))}
            </div>
          </div>

          <div className="field-group">
            <label className="field-label">Icon (optional)</label>
            <input
              type="text"
              className="field-input"
              placeholder="e.g., server, users, box"
              value={icon}
              onChange={(e) => setIcon(e.target.value)}
              disabled={isLoading}
            />
            <span className="field-hint">Use an icon name from your icon library</span>
          </div>
        </div>

        {/* Tags section */}
        <div className="editor-section">
          <div className="field-group">
            <label className="field-label">Tags</label>
            <div className="tag-input-container">
              <div className="tags-list">
                {tags.map((tag) => (
                  <span key={tag} className="tag">
                    {tag}
                    <button
                      type="button"
                      className="tag-remove"
                      onClick={() => removeTag(tag)}
                      disabled={isLoading}
                    >
                      ×
                    </button>
                  </span>
                ))}
                <input
                  type="text"
                  className="tag-input"
                  placeholder={tags.length === 0 ? 'Add tags...' : ''}
                  value={tagInput}
                  onChange={(e) => setTagInput(e.target.value)}
                  onKeyDown={handleTagKeyDown}
                  disabled={isLoading}
                />
              </div>
            </div>
            <span className="field-hint">Press Enter or comma to add a tag</span>
          </div>
        </div>

        {/* Group info (for existing groups) */}
        {group && (
          <div className="editor-section group-info">
            <h3 className="section-title">Information</h3>
            <div className="info-grid">
              <div className="info-item">
                <span className="info-label">Created</span>
                <span className="info-value">{group.createdAt ? new Date(group.createdAt).toLocaleString() : 'N/A'}</span>
              </div>
              <div className="info-item">
                <span className="info-label">Stations</span>
                <span className="info-value">{group.station_ids?.length || 0}</span>
              </div>
            </div>
          </div>
        )}
      </div>

      <style>{`
        .group-editor {
          display: flex;
          flex-direction: column;
          height: 100%;
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
          flex: 1;
          overflow-y: auto;
          padding: 20px;
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

        .field-hint {
          font-size: 0.74rem;
          color: var(--text-muted);
        }

        .color-picker {
          display: flex;
          gap: 8px;
          flex-wrap: wrap;
        }

        .color-swatch {
          width: 32px;
          height: 32px;
          border-radius: 50%;
          border: 2px solid transparent;
          cursor: pointer;
          transition: all 0.2s;
        }

        .color-swatch:hover {
          transform: scale(1.1);
        }

        .color-swatch.selected {
          border-color: var(--text-main);
          box-shadow: 0 0 0 2px var(--bg-card);
        }

        .color-swatch:disabled {
          opacity: 0.6;
          cursor: not-allowed;
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

        .group-info {
          background: var(--bg-main);
          border-radius: 8px;
          padding: 12px;
        }

        .info-grid {
          display: grid;
          grid-template-columns: repeat(2, 1fr);
          gap: 12px;
        }

        .info-item {
          display: flex;
          flex-direction: column;
          gap: 4px;
        }

        .info-label {
          font-size: 0.74rem;
          color: var(--text-muted);
          text-transform: uppercase;
        }

        .info-value {
          font-size: 0.86rem;
          color: var(--text-main);
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

        @media (max-width: 768px) {
          .editor-header {
            padding: 12px;
          }

          .info-grid {
            grid-template-columns: 1fr;
          }
        }
      `}</style>
    </div>
  );
};