/**
 * Script List Component
 * Script listing, search, and management interface
 * Part of Phase 6: Command Script Management System
 */

import React, { useState, useCallback, useEffect, useMemo } from 'react';
import type { 
  CommandScript, 
  ScriptFilter, 
  ScriptType,
  ScriptExecutionContext 
} from './types';

// ============================================
// Script Card Component
// ============================================

interface ScriptCardProps {
  script: CommandScript;
  onSelect: (script: CommandScript) => void;
  onEdit: (script: CommandScript) => void;
  onExecute: (script: CommandScript) => void;
  onToggleFavorite: (scriptId: string) => void;
  onDelete: (scriptId: string) => void;
  onDuplicate: (script: CommandScript) => void;
}

const ScriptCard: React.FC<ScriptCardProps> = ({
  script,
  onSelect,
  onEdit,
  onExecute,
  onToggleFavorite,
  onDelete,
  onDuplicate,
}) => {
  const [showMenu, setShowMenu] = useState(false);

  const formatDate = (dateStr: string) => {
    const date = new Date(dateStr);
    return date.toLocaleDateString() + ' ' + date.toLocaleTimeString([], { hour: '2-digit', minute: '2-digit' });
  };

  const getScriptTypeIcon = (type: ScriptType) => {
    const icons: Record<ScriptType, string> = {
      shell: '🐚',
      powershell: '⚡',
      python: '🐍',
      lua: '🌙',
      javascript: '📜',
    };
    return icons[type] || '📄';
  };

  return (
    <div className={`script-card ${script.isFavorite ? 'favorite' : ''}`} onClick={() => onSelect(script)}>
      <div className="card-header">
        <div className="card-title-row">
          <span className="script-type-icon">{getScriptTypeIcon(script.scriptType)}</span>
          <h3 className="card-title">{script.name}</h3>
          {script.isTemplate && <span className="template-badge">Template</span>}
        </div>
        <button
          className={`favorite-btn ${script.isFavorite ? 'active' : ''}`}
          onClick={(e) => {
            e.stopPropagation();
            onToggleFavorite(script.id);
          }}
          title={script.isFavorite ? 'Remove from favorites' : 'Add to favorites'}
        >
          {script.isFavorite ? '★' : '☆'}
        </button>
      </div>

      <p className="card-description">
        {script.description || 'No description'}
      </p>

      <div className="card-tags">
        {script.tags.slice(0, 3).map(tag => (
          <span key={tag} className="tag">{tag}</span>
        ))}
        {script.tags.length > 3 && (
          <span className="tag-more">+{script.tags.length - 3}</span>
        )}
      </div>

      <div className="card-footer">
        <div className="card-meta">
          <span className="usage-count" title="Times used">
            ⚡ {script.usageCount}
          </span>
          <span className="version">v{script.version}</span>
        </div>
        <div className="card-actions">
          <button
            className="btn-execute"
            onClick={(e) => {
              e.stopPropagation();
              onExecute(script);
            }}
            title="Execute script"
          >
            ▶
          </button>
          <button
            className="btn-edit"
            onClick={(e) => {
              e.stopPropagation();
              onEdit(script);
            }}
            title="Edit script"
          >
            ✎
          </button>
          <div className="dropdown-container">
            <button
              className="btn-menu"
              onClick={(e) => {
                e.stopPropagation();
                setShowMenu(!showMenu);
              }}
              title="More actions"
            >
              ⋮
            </button>
            {showMenu && (
              <div className="dropdown-menu">
                <button onClick={(e) => { e.stopPropagation(); onDuplicate(script); setShowMenu(false); }}>
                  📋 Duplicate
                </button>
                <button onClick={(e) => { e.stopPropagation(); onEdit(script); setShowMenu(false); }}>
                  ✏️ Edit
                </button>
                <button 
                  className="danger"
                  onClick={(e) => { e.stopPropagation(); onDelete(script.id); setShowMenu(false); }}
                >
                  🗑️ Delete
                </button>
              </div>
            )}
          </div>
        </div>
      </div>

      <div className="card-updated">
        Updated {formatDate(script.updatedAt)}
      </div>
    </div>
  );
};

// ============================================
// Script Type Filter
// ============================================

interface ScriptTypeFilterProps {
  selected: ScriptType | 'all';
  onChange: (type: ScriptType | 'all') => void;
  counts: Record<string, number>;
}

const ScriptTypeFilter: React.FC<ScriptTypeFilterProps> = ({ selected, onChange, counts }) => {
  const types: { key: ScriptType | 'all'; label: string; icon: string }[] = [
    { key: 'all', label: 'All', icon: '📂' },
    { key: 'shell', label: 'Shell', icon: '🐚' },
    { key: 'powershell', label: 'PowerShell', icon: '⚡' },
    { key: 'python', label: 'Python', icon: '🐍' },
    { key: 'lua', label: 'Lua', icon: '🌙' },
    { key: 'javascript', label: 'JavaScript', icon: '📜' },
  ];

  return (
    <div className="script-type-filter">
      {types.map(({ key, label, icon }) => (
        <button
          key={key}
          className={`filter-btn ${selected === key ? 'active' : ''}`}
          onClick={() => onChange(key)}
        >
          <span className="filter-icon">{icon}</span>
          <span className="filter-label">{label}</span>
          {counts[key] !== undefined && counts[key] > 0 && (
            <span className="filter-count">{counts[key]}</span>
          )}
        </button>
      ))}
    </div>
  );
};

// ============================================
// Main Script List Component
// ============================================

export interface ScriptListProps {
  scripts: CommandScript[];
  onSelectScript?: (script: CommandScript) => void;
  onEditScript: (script?: CommandScript) => void;
  onExecuteScript: (script: CommandScript, context?: Partial<ScriptExecutionContext>) => void;
  onDeleteScript: (scriptId: string) => void;
  onToggleFavorite: (scriptId: string) => void;
  onDuplicateScript: (script: CommandScript) => void;
  onImport?: () => void;
  onExport?: (scripts: CommandScript[]) => void;
  loading?: boolean;
  title?: string;
}

export const ScriptList: React.FC<ScriptListProps> = ({
  scripts,
  onSelectScript,
  onEditScript,
  onExecuteScript,
  onDeleteScript,
  onToggleFavorite,
  onDuplicateScript,
  onImport,
  onExport,
  loading = false,
  title = 'Scripts',
}) => {
  const [searchQuery, setSearchQuery] = useState('');
  const [selectedType, setSelectedType] = useState<ScriptType | 'all'>('all');
  const [selectedTags, setSelectedTags] = useState<string[]>([]);
  const [showFavoritesOnly, setShowFavoritesOnly] = useState(false);
  const [showTemplatesOnly, setShowTemplatesOnly] = useState(false);
  const [sortBy, setSortBy] = useState<'name' | 'updated' | 'usage' | 'created'>('updated');
  const [sortOrder, setSortOrder] = useState<'asc' | 'desc'>('desc');
  const [selectedScript, setSelectedScript] = useState<CommandScript | null>(null);
  const [confirmDelete, setConfirmDelete] = useState<string | null>(null);

  // Extract all unique tags
  const allTags = useMemo(() => {
    const tagSet = new Set<string>();
    scripts.forEach(s => s.tags.forEach(t => tagSet.add(t)));
    return Array.from(tagSet).sort();
  }, [scripts]);

  // Filter scripts
  const filteredScripts = useMemo(() => {
    let result = scripts;

    // Search filter
    if (searchQuery) {
      const query = searchQuery.toLowerCase();
      result = result.filter(s => 
        s.name.toLowerCase().includes(query) ||
        s.description.toLowerCase().includes(query) ||
        s.tags.some(t => t.toLowerCase().includes(query))
      );
    }

    // Type filter
    if (selectedType !== 'all') {
      result = result.filter(s => s.scriptType === selectedType);
    }

    // Tags filter
    if (selectedTags.length > 0) {
      result = result.filter(s => selectedTags.every(tag => s.tags.includes(tag)));
    }

    // Favorites filter
    if (showFavoritesOnly) {
      result = result.filter(s => s.isFavorite);
    }

    // Templates filter
    if (showTemplatesOnly) {
      result = result.filter(s => s.isTemplate);
    }

    // Sort
    result = [...result].sort((a, b) => {
      let comparison = 0;
      switch (sortBy) {
        case 'name':
          comparison = a.name.localeCompare(b.name);
          break;
        case 'updated':
          comparison = new Date(a.updatedAt).getTime() - new Date(b.updatedAt).getTime();
          break;
        case 'usage':
          comparison = a.usageCount - b.usageCount;
          break;
        case 'created':
          comparison = new Date(a.createdAt).getTime() - new Date(b.createdAt).getTime();
          break;
      }
      return sortOrder === 'asc' ? comparison : -comparison;
    });

    return result;
  }, [scripts, searchQuery, selectedType, selectedTags, showFavoritesOnly, showTemplatesOnly, sortBy, sortOrder]);

  // Count by type
  const countsByType = useMemo(() => {
    const counts: Record<string, number> = { all: scripts.length };
    scripts.forEach(s => {
      counts[s.scriptType] = (counts[s.scriptType] || 0) + 1;
    });
    return counts;
  }, [scripts]);

  // Handle tag toggle
  const toggleTag = (tag: string) => {
    setSelectedTags(prev => 
      prev.includes(tag) ? prev.filter(t => t !== tag) : [...prev, tag]
    );
  };

  // Handle script selection
  const handleSelect = (script: CommandScript) => {
    setSelectedScript(script);
    onSelectScript?.(script);
  };

  // Handle edit
  const handleEdit = (script: CommandScript) => {
    onEditScript(script);
  };

  // Handle execute
  const handleExecute = (script: CommandScript) => {
    onExecuteScript(script);
  };

  // Handle delete
  const handleDelete = (scriptId: string) => {
    if (confirmDelete === scriptId) {
      onDeleteScript(scriptId);
      setConfirmDelete(null);
    } else {
      setConfirmDelete(scriptId);
      setTimeout(() => setConfirmDelete(null), 3000);
    }
  };

  // Handle duplicate
  const handleDuplicate = (script: CommandScript) => {
    const duplicate: CommandScript = {
      ...script,
      id: `script-${Date.now()}`,
      name: `${script.name} (Copy)`,
      isFavorite: false,
      usageCount: 0,
      createdAt: new Date().toISOString(),
      updatedAt: new Date().toISOString(),
    };
    onDuplicateScript(duplicate);
  };

  return (
    <div className="script-list">
      {/* Header */}
      <div className="list-header">
        <div className="header-top">
          <h2>{title}</h2>
          <div className="header-actions">
            {onImport && (
              <button className="btn-import" onClick={onImport}>
                📥 Import
              </button>
            )}
            {onExport && (
              <button 
                className="btn-export" 
                onClick={() => onExport(filteredScripts)}
                disabled={filteredScripts.length === 0}
              >
                📤 Export
              </button>
            )}
            <button className="btn-create" onClick={() => onEditScript()}>
              + New Script
            </button>
          </div>
        </div>

        {/* Search Bar */}
        <div className="search-bar">
          <span className="search-icon">🔍</span>
          <input
            type="text"
            className="search-input"
            placeholder="Search scripts by name, description, or tags..."
            value={searchQuery}
            onChange={(e) => setSearchQuery(e.target.value)}
          />
          {searchQuery && (
            <button className="search-clear" onClick={() => setSearchQuery('')}>
              ×
            </button>
          )}
        </div>

        {/* Type Filter */}
        <ScriptTypeFilter
          selected={selectedType}
          onChange={setSelectedType}
          counts={countsByType}
        />

        {/* Additional Filters */}
        <div className="filter-row">
          <div className="filter-tags">
            <span className="filter-label">Tags:</span>
            {allTags.slice(0, 8).map(tag => (
              <button
                key={tag}
                className={`tag-filter-btn ${selectedTags.includes(tag) ? 'active' : ''}`}
                onClick={() => toggleTag(tag)}
              >
                {tag}
              </button>
            ))}
          </div>

          <div className="filter-toggles">
            <label className="toggle-filter">
              <input
                type="checkbox"
                checked={showFavoritesOnly}
                onChange={(e) => setShowFavoritesOnly(e.target.checked)}
              />
              <span>★ Favorites</span>
            </label>
            <label className="toggle-filter">
              <input
                type="checkbox"
                checked={showTemplatesOnly}
                onChange={(e) => setShowTemplatesOnly(e.target.checked)}
              />
              <span>📋 Templates</span>
            </label>
          </div>
        </div>

        {/* Sort Controls */}
        <div className="sort-controls">
          <span className="sort-label">Sort by:</span>
          <select
            className="sort-select"
            value={sortBy}
            onChange={(e) => setSortBy(e.target.value as typeof sortBy)}
          >
            <option value="updated">Last Updated</option>
            <option value="name">Name</option>
            <option value="usage">Usage Count</option>
            <option value="created">Date Created</option>
          </select>
          <button
            className="btn-sort-order"
            onClick={() => setSortOrder(prev => prev === 'asc' ? 'desc' : 'asc')}
            title={sortOrder === 'asc' ? 'Ascending' : 'Descending'}
          >
            {sortOrder === 'asc' ? '↑' : '↓'}
          </button>
          <span className="result-count">
            {filteredScripts.length} of {scripts.length} scripts
          </span>
        </div>
      </div>

      {/* Script Grid */}
      <div className="script-grid">
        {loading ? (
          <div className="loading-state">
            <div className="spinner"></div>
            <p>Loading scripts...</p>
          </div>
        ) : filteredScripts.length === 0 ? (
          <div className="empty-state">
            {scripts.length === 0 ? (
              <>
                <div className="empty-icon">📜</div>
                <h3>No scripts yet</h3>
                <p>Create your first script to get started</p>
                <button className="btn-create-empty" onClick={() => onEditScript()}>
                  + Create Script
                </button>
              </>
            ) : (
              <>
                <div className="empty-icon">🔍</div>
                <h3>No matching scripts</h3>
                <p>Try adjusting your filters or search query</p>
                <button className="btn-clear-filters" onClick={() => {
                  setSearchQuery('');
                  setSelectedType('all');
                  setSelectedTags([]);
                  setShowFavoritesOnly(false);
                  setShowTemplatesOnly(false);
                }}>
                  Clear Filters
                </button>
              </>
            )}
          </div>
        ) : (
          filteredScripts.map(script => (
            <ScriptCard
              key={script.id}
              script={script}
              onSelect={handleSelect}
              onEdit={handleEdit}
              onExecute={handleExecute}
              onToggleFavorite={onToggleFavorite}
              onDelete={handleDelete}
              onDuplicate={handleDuplicate}
            />
          ))
        )}
      </div>

      {/* Delete Confirmation */}
      {confirmDelete && (
        <div className="delete-confirm-overlay" onClick={() => setConfirmDelete(null)}>
          <div className="delete-confirm-dialog" onClick={e => e.stopPropagation()}>
            <h3>Delete Script?</h3>
            <p>This action cannot be undone.</p>
            <div className="dialog-actions">
              <button className="btn-cancel" onClick={() => setConfirmDelete(null)}>
                Cancel
              </button>
              <button className="btn-delete" onClick={() => handleDelete(confirmDelete)}>
                Delete
              </button>
            </div>
          </div>
        </div>
      )}

      {/* Embedded Styles */}
      <style>{`
        .script-list {
          display: flex;
          flex-direction: column;
          height: 100%;
          background: var(--bg-primary, #1a1a1a);
          color: var(--text-primary, #e0e0e0);
        }

        .list-header {
          padding: 20px 24px;
          background: var(--bg-secondary, #252525);
          border-bottom: 1px solid var(--border-color, #333);
        }

        .header-top {
          display: flex;
          justify-content: space-between;
          align-items: center;
          margin-bottom: 16px;
        }

        .header-top h2 {
          margin: 0;
          font-size: 24px;
          font-weight: 600;
        }

        .header-actions {
          display: flex;
          gap: 8px;
        }

        .btn-import, .btn-export, .btn-create {
          padding: 8px 16px;
          border-radius: 6px;
          font-size: 14px;
          font-weight: 500;
          cursor: pointer;
          transition: all 0.2s;
        }

        .btn-import {
          background: transparent;
          border: 1px solid var(--border-color, #444);
          color: var(--text-secondary, #a0a0a0);
        }

        .btn-import:hover {
          background: var(--bg-hover, #333);
        }

        .btn-export {
          background: transparent;
          border: 1px solid var(--border-color, #444);
          color: var(--text-secondary, #a0a0a0);
        }

        .btn-export:hover:not(:disabled) {
          background: var(--bg-hover, #333);
        }

        .btn-export:disabled {
          opacity: 0.5;
          cursor: not-allowed;
        }

        .btn-create {
          background: var(--accent-color, #3b82f6);
          border: none;
          color: white;
        }

        .btn-create:hover {
          background: var(--accent-hover, #2563eb);
        }

        .search-bar {
          display: flex;
          align-items: center;
          background: var(--bg-primary, #1a1a1a);
          border: 1px solid var(--border-color, #444);
          border-radius: 8px;
          padding: 0 12px;
          margin-bottom: 16px;
        }

        .search-icon {
          font-size: 16px;
          margin-right: 8px;
        }

        .search-input {
          flex: 1;
          padding: 10px 0;
          background: transparent;
          border: none;
          color: var(--text-primary, #e0e0e0);
          font-size: 14px;
          outline: none;
        }

        .search-input::placeholder {
          color: var(--text-muted, #666);
        }

        .search-clear {
          background: none;
          border: none;
          color: var(--text-muted, #666);
          font-size: 20px;
          cursor: pointer;
          padding: 0;
          line-height: 1;
        }

        .search-clear:hover {
          color: var(--text-primary, #e0e0e0);
        }

        .script-type-filter {
          display: flex;
          gap: 8px;
          margin-bottom: 16px;
          flex-wrap: wrap;
        }

        .filter-btn {
          display: flex;
          align-items: center;
          gap: 6px;
          padding: 6px 12px;
          background: var(--bg-primary, #1a1a1a);
          border: 1px solid var(--border-color, #444);
          border-radius: 20px;
          color: var(--text-secondary, #a0a0a0);
          font-size: 13px;
          cursor: pointer;
          transition: all 0.2s;
        }

        .filter-btn:hover {
          border-color: var(--accent-color, #3b82f6);
          color: var(--text-primary, #e0e0e0);
        }

        .filter-btn.active {
          background: var(--accent-color, #3b82f6);
          border-color: var(--accent-color, #3b82f6);
          color: white;
        }

        .filter-count {
          background: rgba(255, 255, 255, 0.2);
          padding: 2px 6px;
          border-radius: 10px;
          font-size: 11px;
        }

        .filter-row {
          display: flex;
          justify-content: space-between;
          align-items: center;
          margin-bottom: 12px;
          flex-wrap: wrap;
          gap: 12px;
        }

        .filter-tags {
          display: flex;
          align-items: center;
          gap: 6px;
          flex-wrap: wrap;
        }

        .filter-label {
          font-size: 13px;
          color: var(--text-muted, #666);
        }

        .tag-filter-btn {
          padding: 4px 10px;
          background: transparent;
          border: 1px solid var(--border-color, #444);
          border-radius: 4px;
          color: var(--text-secondary, #a0a0a0);
          font-size: 12px;
          cursor: pointer;
          transition: all 0.2s;
        }

        .tag-filter-btn:hover {
          border-color: var(--accent-color, #3b82f6);
        }

        .tag-filter-btn.active {
          background: var(--accent-color, #3b82f6);
          border-color: var(--accent-color, #3b82f6);
          color: white;
        }

        .filter-toggles {
          display: flex;
          gap: 16px;
        }

        .toggle-filter {
          display: flex;
          align-items: center;
          gap: 6px;
          font-size: 13px;
          color: var(--text-secondary, #a0a0a0);
          cursor: pointer;
        }

        .toggle-filter input {
          cursor: pointer;
        }

        .sort-controls {
          display: flex;
          align-items: center;
          gap: 8px;
        }

        .sort-label {
          font-size: 13px;
          color: var(--text-muted, #666);
        }

        .sort-select {
          padding: 6px 10px;
          background: var(--bg-primary, #1a1a1a);
          border: 1px solid var(--border-color, #444);
          border-radius: 4px;
          color: var(--text-primary, #e0e0e0);
          font-size: 13px;
          cursor: pointer;
        }

        .btn-sort-order {
          padding: 6px 10px;
          background: var(--bg-primary, #1a1a1a);
          border: 1px solid var(--border-color, #444);
          border-radius: 4px;
          color: var(--text-primary, #e0e0e0);
          font-size: 14px;
          cursor: pointer;
        }

        .btn-sort-order:hover {
          border-color: var(--accent-color, #3b82f6);
        }

        .result-count {
          margin-left: auto;
          font-size: 13px;
          color: var(--text-muted, #666);
        }

        .script-grid {
          flex: 1;
          overflow-y: auto;
          padding: 24px;
          display: grid;
          grid-template-columns: repeat(auto-fill, minmax(320px, 1fr));
          gap: 16px;
          align-content: start;
        }

        .script-card {
          background: var(--bg-secondary, #252525);
          border: 1px solid var(--border-color, #333);
          border-radius: 12px;
          padding: 16px;
          cursor: pointer;
          transition: all 0.2s;
          position: relative;
        }

        .script-card:hover {
          border-color: var(--accent-color, #3b82f6);
          transform: translateY(-2px);
        }

        .script-card.favorite {
          border-color: #f59e0b;
        }

        .card-header {
          display: flex;
          justify-content: space-between;
          align-items: flex-start;
          margin-bottom: 8px;
        }

        .card-title-row {
          display: flex;
          align-items: center;
          gap: 8px;
        }

        .script-type-icon {
          font-size: 18px;
        }

        .card-title {
          margin: 0;
          font-size: 16px;
          font-weight: 600;
          color: var(--text-primary, #e0e0e0);
        }

        .template-badge {
          background: var(--accent-color, #3b82f6);
          color: white;
          padding: 2px 6px;
          border-radius: 4px;
          font-size: 10px;
          font-weight: 500;
        }

        .favorite-btn {
          background: none;
          border: none;
          font-size: 20px;
          color: var(--text-muted, #666);
          cursor: pointer;
          padding: 0;
          line-height: 1;
        }

        .favorite-btn.active {
          color: #f59e0b;
        }

        .favorite-btn:hover {
          color: #f59e0b;
        }

        .card-description {
          margin: 0 0 12px 0;
          font-size: 13px;
          color: var(--text-secondary, #a0a0a0);
          line-height: 1.4;
          display: -webkit-box;
          -webkit-line-clamp: 2;
          -webkit-box-orient: vertical;
          overflow: hidden;
        }

        .card-tags {
          display: flex;
          gap: 6px;
          flex-wrap: wrap;
          margin-bottom: 12px;
        }

        .tag {
          background: var(--bg-primary, #1a1a1a);
          color: var(--text-secondary, #a0a0a0);
          padding: 3px 8px;
          border-radius: 4px;
          font-size: 11px;
        }

        .tag-more {
          color: var(--text-muted, #666);
          font-size: 11px;
          padding: 3px 4px;
        }

        .card-footer {
          display: flex;
          justify-content: space-between;
          align-items: center;
        }

        .card-meta {
          display: flex;
          gap: 12px;
          font-size: 12px;
          color: var(--text-muted, #666);
        }

        .card-actions {
          display: flex;
          gap: 6px;
        }

        .btn-execute, .btn-edit, .btn-menu {
          width: 32px;
          height: 32px;
          border-radius: 6px;
          border: none;
          cursor: pointer;
          font-size: 14px;
          display: flex;
          align-items: center;
          justify-content: center;
          transition: all 0.2s;
        }

        .btn-execute {
          background: #22c55e;
          color: white;
        }

        .btn-execute:hover {
          background: #16a34a;
        }

        .btn-edit {
          background: var(--bg-primary, #1a1a1a);
          color: var(--text-secondary, #a0a0a0);
          border: 1px solid var(--border-color, #444);
        }

        .btn-edit:hover {
          border-color: var(--accent-color, #3b82f6);
          color: var(--accent-color, #3b82f6);
        }

        .btn-menu {
          background: var(--bg-primary, #1a1a1a);
          color: var(--text-secondary, #a0a0a0);
          border: 1px solid var(--border-color, #444);
        }

        .btn-menu:hover {
          border-color: var(--accent-color, #3b82f6);
        }

        .dropdown-container {
          position: relative;
        }

        .dropdown-menu {
          position: absolute;
          top: 100%;
          right: 0;
          margin-top: 4px;
          background: var(--bg-primary, #1a1a1a);
          border: 1px solid var(--border-color, #444);
          border-radius: 8px;
          padding: 4px;
          min-width: 140px;
          z-index: 100;
          box-shadow: 0 4px 12px rgba(0, 0, 0, 0.3);
        }

        .dropdown-menu button {
          width: 100%;
          padding: 8px 12px;
          background: transparent;
          border: none;
          color: var(--text-primary, #e0e0e0);
          font-size: 13px;
          text-align: left;
          cursor: pointer;
          border-radius: 4px;
          display: flex;
          align-items: center;
          gap: 8px;
        }

        .dropdown-menu button:hover {
          background: var(--bg-hover, #333);
        }

        .dropdown-menu button.danger {
          color: #ef4444;
        }

        .card-updated {
          margin-top: 12px;
          padding-top: 12px;
          border-top: 1px solid var(--border-color, #333);
          font-size: 11px;
          color: var(--text-muted, #666);
        }

        .loading-state, .empty-state {
          grid-column: 1 / -1;
          display: flex;
          flex-direction: column;
          align-items: center;
          justify-content: center;
          padding: 60px 20px;
          text-align: center;
        }

        .spinner {
          width: 40px;
          height: 40px;
          border: 3px solid var(--border-color, #333);
          border-top-color: var(--accent-color, #3b82f6);
          border-radius: 50%;
          animation: spin 1s linear infinite;
        }

        @keyframes spin {
          to { transform: rotate(360deg); }
        }

        .empty-state h3 {
          margin: 16px 0 8px;
          font-size: 18px;
          color: var(--text-primary, #e0e0e0);
        }

        .empty-state p {
          margin: 0 0 16px;
          color: var(--text-muted, #666);
        }

        .empty-icon {
          font-size: 48px;
        }

        .btn-create-empty, .btn-clear-filters {
          padding: 10px 20px;
          background: var(--accent-color, #3b82f6);
          border: none;
          border-radius: 8px;
          color: white;
          font-size: 14px;
          font-weight: 500;
          cursor: pointer;
        }

        .btn-clear-filters {
          background: transparent;
          border: 1px solid var(--border-color, #444);
          color: var(--text-secondary, #a0a0a0);
        }

        .btn-create-empty:hover {
          background: var(--accent-hover, #2563eb);
        }

        .delete-confirm-overlay {
          position: fixed;
          top: 0;
          left: 0;
          right: 0;
          bottom: 0;
          background: rgba(0, 0, 0, 0.5);
          display: flex;
          align-items: center;
          justify-content: center;
          z-index: 1000;
        }

        .delete-confirm-dialog {
          background: var(--bg-secondary, #252525);
          border: 1px solid var(--border-color, #333);
          border-radius: 12px;
          padding: 24px;
          max-width: 400px;
          text-align: center;
        }

        .delete-confirm-dialog h3 {
          margin: 0 0 8px;
          font-size: 18px;
        }

        .delete-confirm-dialog p {
          margin: 0 0 20px;
          color: var(--text-secondary, #a0a0a0);
        }

        .dialog-actions {
          display: flex;
          gap: 12px;
          justify-content: center;
        }

        .btn-cancel {
          padding: 10px 20px;
          background: transparent;
          border: 1px solid var(--border-color, #444);
          border-radius: 6px;
          color: var(--text-primary, #e0e0e0);
          font-size: 14px;
          cursor: pointer;
        }

        .btn-delete {
          padding: 10px 20px;
          background: #ef4444;
          border: none;
          border-radius: 6px;
          color: white;
          font-size: 14px;
          cursor: pointer;
        }

        .btn-delete:hover {
          background: #dc2626;
        }
      `}</style>
    </div>
  );
};

export default ScriptList;
