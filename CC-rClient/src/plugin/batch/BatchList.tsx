/**
 * Batch Task List Component
 * Displays batch tasks with filtering, search, and bulk actions
 * Part of Phase 7: Batch Operations Support System
 */

import React, { useState, useMemo } from 'react';
import type {
  BatchTask,
  BatchTaskFilter,
  BatchTaskType,
  BatchTaskStatus,
} from './types';

// ============================================
// Batch Task Card Component
// ============================================

interface BatchTaskCardProps {
  task: BatchTask;
  onSelect?: (task: BatchTask) => void;
  onEdit?: (task: BatchTask) => void;
  onExecute?: (task: BatchTask) => void;
  onDelete?: (taskId: string) => void;
  onDuplicate?: (task: BatchTask) => void;
  onToggleFavorite?: (taskId: string) => void;
  onPause?: (taskId: string) => void;
  onCancel?: (taskId: string) => void;
}

const BatchTaskCard: React.FC<BatchTaskCardProps> = ({
  task,
  onSelect,
  onEdit,
  onExecute,
  onDelete,
  onDuplicate,
  onToggleFavorite,
  onPause,
  onCancel,
}) => {
  const [showMenu, setShowMenu] = useState(false);

  const formatDate = (dateStr: string) => {
    const date = new Date(dateStr);
    return date.toLocaleDateString() + ' ' + date.toLocaleTimeString([], { hour: '2-digit', minute: '2-digit' });
  };

  const getTaskTypeIcon = (type: BatchTaskType) => {
    const icons: Record<BatchTaskType, string> = {
      power_on: '⚡',
      shutdown: '⏻',
      reboot: '🔄',
      start_app: '🚀',
      command: '💻',
      watch_processes: '👁',
      script: '📜',
    };
    return icons[type] || '📄';
  };

  const getStatusBadge = (status: BatchTaskStatus) => {
    const statusConfig: Record<BatchTaskStatus, { color: string; bg: string }> = {
      draft: { color: '#6b7280', bg: '#f3f4f6' },
      pending: { color: '#f59e0b', bg: '#fef3c7' },
      running: { color: '#3b82f6', bg: '#dbeafe' },
      paused: { color: '#8b5cf6', bg: '#ede9fe' },
      completed: { color: '#22c55e', bg: '#dcfce7' },
      partial_failure: { color: '#f97316', bg: '#ffedd5' },
      failed: { color: '#ef4444', bg: '#fee2e2' },
      cancelled: { color: '#6b7280', bg: '#f3f4f6' },
    };
    const config = statusConfig[status] || statusConfig.draft;
    return (
      <span
        className="status-badge"
        style={{ color: config.color, backgroundColor: config.bg }}
      >
        {status.replace('_', ' ')}
      </span>
    );
  };

  const getTargetSummary = () => {
    const selector = task.targetSelector;
    switch (selector.selectorType) {
      case 'all':
        return 'All devices';
      case 'group':
        return `${selector.groups?.length || 0} groups`;
      case 'tag':
        return `${selector.tags?.length || 0} tags`;
      case 'device_ids':
        return `${selector.deviceIds?.length || 0} devices`;
      case 'filter':
        return 'Filtered';
      default:
        return 'Unknown';
    }
  };

  const getPolicySummary = () => {
    const policy = task.executionPolicy;
    const modeLabels = { parallel: 'Parallel', batch: 'Batch', rolling: 'Rolling' };
    return `${modeLabels[policy.mode]}, ${policy.failureThresholdPercent}% threshold`;
  };

  return (
    <div className={`script-card batch-task-card ${task.isFavorite ? 'favorite' : ''}`} onClick={() => onSelect?.(task)}>
      <div className="card-header">
        <div className="card-title-row">
          <span className="script-type-icon task-type-icon">{getTaskTypeIcon(task.taskType)}</span>
          <h3 className="card-title">{task.name}</h3>
          {getStatusBadge(task.status)}
        </div>
        <button
          className={`favorite-btn ${task.isFavorite ? 'active' : ''}`}
          onClick={(e) => {
            e.stopPropagation();
            onToggleFavorite?.(task.id);
          }}
          title={task.isFavorite ? 'Remove from favorites' : 'Add to favorites'}
        >
          {task.isFavorite ? '★' : '☆'}
        </button>
      </div>

      <p className="card-description">
        {task.description || 'No description'}
      </p>

      <div className="card-meta-row">
        <span className="meta-item" title="Target selection">
          🎯 {getTargetSummary()}
        </span>
        <span className="meta-item" title="Execution policy">
          ⚡ {getPolicySummary()}
        </span>
      </div>

      <div className="card-footer">
        <div className="card-meta">
          <span className="usage-count" title="Times used">
            ⚡ {task.usageCount}
          </span>
          <span className="version">v{task.version}</span>
          {task.lastRunAt && (
            <span className="last-run" title="Last run">
              🕐 {formatDate(task.lastRunAt)}
            </span>
          )}
        </div>
        <div className="card-actions">
          <button
            className="btn-execute"
            onClick={(e) => {
              e.stopPropagation();
              onExecute?.(task);
            }}
            title="Execute task"
            disabled={task.status === 'running'}
          >
            ▶
          </button>
          <button
            className="btn-edit"
            onClick={(e) => {
              e.stopPropagation();
              onEdit?.(task);
            }}
            title="Edit task"
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
                <button onClick={(e) => { e.stopPropagation(); onDuplicate?.(task); setShowMenu(false); }}>
                  📋 Duplicate
                </button>
                <button onClick={(e) => { e.stopPropagation(); onEdit?.(task); setShowMenu(false); }}>
                  ✏️ Edit
                </button>
                {task.status === 'running' && (
                  <button onClick={(e) => { e.stopPropagation(); onPause?.(task.id); setShowMenu(false); }}>
                    ⏸️ Pause
                  </button>
                )}
                {task.status === 'running' && (
                  <button onClick={(e) => { e.stopPropagation(); onCancel?.(task.id); setShowMenu(false); }}>
                    ⏹️ Cancel
                  </button>
                )}
                <button
                  className="danger"
                  onClick={(e) => { e.stopPropagation(); onDelete?.(task.id); setShowMenu(false); }}
                >
                  🗑️ Delete
                </button>
              </div>
            )}
          </div>
        </div>
      </div>

      <div className="card-updated">
        Updated {formatDate(task.updatedAt)}
      </div>
    </div>
  );
};

// ============================================
// Batch Task Type Filter
// ============================================

interface BatchTypeFilterProps {
  selected: BatchTaskType | 'all';
  onChange: (type: BatchTaskType | 'all') => void;
  counts: Record<string, number>;
}

const BatchTypeFilter: React.FC<BatchTypeFilterProps> = ({ selected, onChange, counts }) => {
  const types: { key: BatchTaskType | 'all'; label: string; icon: string }[] = [
    { key: 'all', label: 'All', icon: '📂' },
    { key: 'power_on', label: 'Power On', icon: '⚡' },
    { key: 'shutdown', label: 'Shutdown', icon: '⏻' },
    { key: 'reboot', label: 'Reboot', icon: '🔄' },
    { key: 'start_app', label: 'Start App', icon: '🚀' },
    { key: 'command', label: 'Command', icon: '💻' },
    { key: 'watch_processes', label: 'Watch', icon: '👁' },
    { key: 'script', label: 'Script', icon: '📜' },
  ];

  return (
    <div className="batch-type-filter">
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
// Status Filter Component
// ============================================

interface StatusFilterProps {
  selected: BatchTaskStatus[];
  onChange: (statuses: BatchTaskStatus[]) => void;
  counts: Record<string, number>;
}

const StatusFilter: React.FC<StatusFilterProps> = ({ selected, onChange, counts }) => {
  const statuses: { key: BatchTaskStatus; label: string; color: string }[] = [
    { key: 'draft', label: 'Draft', color: '#6b7280' },
    { key: 'pending', label: 'Pending', color: '#f59e0b' },
    { key: 'running', label: 'Running', color: '#3b82f6' },
    { key: 'completed', label: 'Completed', color: '#22c55e' },
    { key: 'partial_failure', label: 'Partial', color: '#f97316' },
    { key: 'failed', label: 'Failed', color: '#ef4444' },
  ];

  const toggleStatus = (status: BatchTaskStatus) => {
    if (selected.includes(status)) {
      onChange(selected.filter(s => s !== status));
    } else {
      onChange([...selected, status]);
    }
  };

  return (
    <div className="status-filter">
      {statuses.map(({ key, label, color }) => (
        <label key={key} className={`status-filter-item ${selected.includes(key) ? 'active' : ''}`}>
          <input
            type="checkbox"
            checked={selected.includes(key)}
            onChange={() => toggleStatus(key)}
          />
          <span className="status-dot" style={{ backgroundColor: color }}></span>
          <span className="status-label">{label}</span>
          {counts[key] !== undefined && counts[key] > 0 && (
            <span className="status-count">{counts[key]}</span>
          )}
        </label>
      ))}
    </div>
  );
};

// ============================================
// Main Batch Task List Component
// ============================================

export interface BatchTaskListProps {
  tasks: BatchTask[];
  onSelectTask?: (task: BatchTask) => void;
  onEditTask: (task?: BatchTask) => void;
  onExecuteTask: (task: BatchTask) => void;
  onDeleteTask: (taskId: string) => void;
  onDuplicateTask?: (task: BatchTask) => void;
  onToggleFavorite?: (taskId: string) => void;
  onImport?: () => void;
  onExport?: (tasks: BatchTask[]) => void;
  onPauseTask?: (taskId: string) => void;
  onCancelTask?: (taskId: string) => void;
  loading?: boolean;
  title?: string;
}

export const BatchTaskList: React.FC<BatchTaskListProps> = ({
  tasks,
  onSelectTask,
  onEditTask,
  onExecuteTask,
  onDeleteTask,
  onDuplicateTask,
  onToggleFavorite,
  onImport,
  onExport,
  onPauseTask,
  onCancelTask,
  loading = false,
  title = 'Batch Tasks',
}) => {
  const [searchQuery, setSearchQuery] = useState('');
  const [selectedType, setSelectedType] = useState<BatchTaskType | 'all'>('all');
  const [selectedStatuses, setSelectedStatuses] = useState<BatchTaskStatus[]>([]);
  const [showFavoritesOnly, setShowFavoritesOnly] = useState(false);
  const [sortBy, setSortBy] = useState<'name' | 'updated' | 'usage' | 'created'>('updated');
  const [sortOrder, setSortOrder] = useState<'asc' | 'desc'>('desc');
  const [selectedTask, setSelectedTask] = useState<BatchTask | null>(null);
  const [confirmDelete, setConfirmDelete] = useState<string | null>(null);

  // Filter tasks
  const filteredTasks = useMemo(() => {
    let result = tasks;

    // Search filter
    if (searchQuery) {
      const query = searchQuery.toLowerCase();
      result = result.filter(t =>
        t.name.toLowerCase().includes(query) ||
        t.description?.toLowerCase().includes(query) ||
        t.tags?.some(tag => tag.toLowerCase().includes(query))
      );
    }

    // Type filter
    if (selectedType !== 'all') {
      result = result.filter(t => t.taskType === selectedType);
    }

    // Status filter
    if (selectedStatuses.length > 0) {
      result = result.filter(t => selectedStatuses.includes(t.status));
    }

    // Favorites filter
    if (showFavoritesOnly) {
      result = result.filter(t => t.isFavorite);
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
  }, [tasks, searchQuery, selectedType, selectedStatuses, showFavoritesOnly, sortBy, sortOrder]);

  // Count by type
  const countsByType = useMemo(() => {
    const counts: Record<string, number> = { all: tasks.length };
    tasks.forEach(t => {
      counts[t.taskType] = (counts[t.taskType] || 0) + 1;
    });
    return counts;
  }, [tasks]);

  // Count by status
  const countsByStatus = useMemo(() => {
    const counts: Record<string, number> = {};
    tasks.forEach(t => {
      counts[t.status] = (counts[t.status] || 0) + 1;
    });
    return counts;
  }, [tasks]);

  // Handle task selection
  const handleSelect = (task: BatchTask) => {
    setSelectedTask(task);
    onSelectTask?.(task);
  };

  // Handle edit
  const handleEdit = (task: BatchTask) => {
    onEditTask(task);
  };

  // Handle execute
  const handleExecute = (task: BatchTask) => {
    onExecuteTask(task);
  };

  // Handle delete
  const handleDelete = (taskId: string) => {
    if (confirmDelete === taskId) {
      onDeleteTask(taskId);
      setConfirmDelete(null);
    } else {
      setConfirmDelete(taskId);
      setTimeout(() => setConfirmDelete(null), 3000);
    }
  };

  // Handle duplicate
  const handleDuplicate = (task: BatchTask) => {
    onDuplicateTask?.(task);
  };

  return (
    <div className="script-list batch-task-list">
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
                onClick={() => onExport(filteredTasks)}
                disabled={filteredTasks.length === 0}
              >
                📤 Export
              </button>
            )}
            <button className="btn-create" onClick={() => onEditTask()}>
              + New Task
            </button>
          </div>
        </div>

        {/* Search Bar with Type Filter */}
        <div className="search-bar">
          <div className="search-field">
            <div className="search-input-wrap">
              <span className="search-icon">🔍</span>
              <input
                type="text"
                className="search-input"
                placeholder="Search tasks by name, description, or tags..."
                value={searchQuery}
                onChange={(e) => setSearchQuery(e.target.value)}
              />
              {searchQuery && (
                <button className="search-clear" onClick={() => setSearchQuery('')}>
                  ×
                </button>
              )}
            </div>

            <div className="type-filter-inline">
              <BatchTypeFilter
                selected={selectedType}
                onChange={setSelectedType}
                counts={countsByType}
              />
            </div>
          </div>

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
              {filteredTasks.length} of {tasks.length} tasks
            </span>
          </div>
        </div>

        {/* Status Filter */}
        <div className="filter-row">
          <span className="filter-label">Status:</span>
          <StatusFilter
            selected={selectedStatuses}
            onChange={setSelectedStatuses}
            counts={countsByStatus}
          />
          <div className="filter-toggles">
            <label className="toggle-filter">
              <input
                type="checkbox"
                checked={showFavoritesOnly}
                onChange={(e) => setShowFavoritesOnly(e.target.checked)}
              />
              <span>★ Favorites</span>
            </label>
          </div>
        </div>

      </div>

      {/* Task Grid */}
      <div className="script-grid task-grid">
        {loading ? (
          <div className="loading-state">
            <div className="spinner"></div>
            <p>Loading tasks...</p>
          </div>
        ) : filteredTasks.length === 0 ? (
          <div className="empty-state">
            {tasks.length === 0 ? (
              <>
                <div className="empty-icon">📋</div>
                <h3>No batch tasks yet</h3>
                <p>Create your first batch task to get started</p>
                <button className="btn-create-empty" onClick={() => onEditTask()}>
                  + Create Task
                </button>
              </>
            ) : (
              <>
                <div className="empty-icon">🔍</div>
                <h3>No matching tasks</h3>
                <p>Try adjusting your filters or search query</p>
                <button className="btn-clear-filters" onClick={() => {
                  setSearchQuery('');
                  setSelectedType('all');
                  setSelectedStatuses([]);
                  setShowFavoritesOnly(false);
                }}>
                  Clear Filters
                </button>
              </>
            )}
          </div>
        ) : (
          filteredTasks.map(task => (
            <BatchTaskCard
              key={task.id}
              task={task}
              onSelect={handleSelect}
              onEdit={handleEdit}
              onExecute={handleExecute}
              onDelete={handleDelete}
              onDuplicate={handleDuplicate}
              onToggleFavorite={onToggleFavorite}
              onPause={onPauseTask}
              onCancel={onCancelTask}
            />
          ))
        )}
      </div>

      {confirmDelete && (
        <div className="delete-confirm-overlay" onClick={() => setConfirmDelete(null)}>
          <div className="delete-confirm-dialog" onClick={(event) => event.stopPropagation()}>
            <h3>Delete Batch Task?</h3>
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

      <style>{`
        .batch-task-list {
          display: flex;
          flex-direction: column;
          height: 100%;
          background: transparent;
          color: var(--text-main);
        }

        .list-header {
          position: sticky;
          top: 0;
          z-index: 20;
          padding: 16px 20px 14px;
          background: var(--bg-card);
          border-bottom: 1px solid var(--border-color);
          box-shadow: 0 10px 20px -18px rgba(11, 25, 44, 0.4);
        }

        .header-top {
          display: flex;
          justify-content: space-between;
          align-items: flex-end;
          gap: 12px;
          margin-bottom: 14px;
          flex-wrap: wrap;
        }

        .header-top h2 {
          margin: 0;
          font-size: 1.1rem;
          font-weight: 700;
          color: var(--text-main);
        }

        .header-actions {
          display: flex;
          gap: 8px;
          flex-wrap: wrap;
          justify-content: flex-end;
        }

        .btn-import, .btn-export, .btn-create {
          padding: 7px 12px;
          border-radius: 8px;
          font-size: 0.82rem;
          font-weight: 500;
          cursor: pointer;
          transition: all 0.2s;
        }

        .btn-import, .btn-export {
          background: transparent;
          border: 1px solid var(--border-color);
          color: var(--text-muted);
        }

        .btn-import:hover, .btn-export:hover:not(:disabled) {
          color: var(--text-main);
          border-color: var(--primary);
        }

        .btn-export:disabled {
          opacity: 0.5;
          cursor: not-allowed;
        }

        .btn-create {
          background: var(--primary);
          border: 1px solid var(--primary);
          color: white;
        }

        .btn-create:hover {
          background: var(--primary-hover);
          border-color: var(--primary-hover);
        }

        .search-bar {
          display: flex;
          align-items: center;
          gap: 10px;
          margin-bottom: 12px;
          flex-wrap: nowrap;
        }

        .search-field {
          display: flex;
          align-items: center;
          gap: 10px;
          flex: 1 1 auto;
          min-width: 220px;
          background: linear-gradient(180deg, rgba(45, 140, 240, 0.05) 0%, rgba(45, 140, 240, 0.01) 100%);
          border: 1px solid var(--border-color);
          border-radius: 10px;
          padding: 8px 10px;
        }

        .search-input-wrap {
          display: flex;
          align-items: center;
          gap: 10px;
          flex: 1 1 0;
          min-width: 0;
        }

        .search-icon {
          font-size: 14px;
          opacity: 0.65;
          flex-shrink: 0;
        }

        .search-input {
          flex: 1;
          min-width: 0;
          padding: 8px 2px;
          background: transparent;
          border: none;
          color: var(--text-main);
          font-size: 0.9rem;
          outline: none;
        }

        .search-input::placeholder {
          color: var(--text-muted);
        }

        .search-clear {
          background: none;
          border: none;
          color: var(--text-muted);
          font-size: 16px;
          cursor: pointer;
          padding: 0;
          line-height: 1;
          flex-shrink: 0;
        }

        .search-clear:hover {
          color: var(--text-main);
        }

        .type-filter-inline {
          display: flex;
          gap: 4px;
          flex-shrink: 0;
          border-left: 1px solid var(--border-color);
          padding-left: 10px;
          margin-left: auto;
          max-width: 100%;
          overflow-x: auto;
        }

        .batch-type-filter {
          display: flex;
          gap: 6px;
          flex-wrap: nowrap;
        }

        .batch-type-filter .filter-btn {
          padding: 4px 10px;
          font-size: 0.76rem;
        }

        .filter-btn {
          display: flex;
          align-items: center;
          gap: 6px;
          padding: 5px 10px;
          background: transparent;
          border: 1px solid var(--border-color);
          border-radius: 16px;
          color: var(--text-muted);
          font-size: 0.8rem;
          cursor: pointer;
          transition: all 0.2s;
        }

        .filter-btn:hover {
          border-color: var(--primary);
          color: var(--text-main);
        }

        .filter-btn.active {
          background: var(--primary);
          border-color: var(--primary);
          color: white;
        }

        .filter-count {
          background: rgba(255, 255, 255, 0.2);
          padding: 1px 5px;
          border-radius: 8px;
          font-size: 0.7rem;
        }

        .filter-row {
          display: flex;
          justify-content: space-between;
          align-items: flex-start;
          margin-bottom: 12px;
          flex-wrap: wrap;
          gap: 10px;
        }

        .filter-label {
          font-size: 0.8rem;
          color: var(--text-muted);
          min-width: fit-content;
          padding-top: 4px;
        }

        .status-filter {
          display: flex;
          gap: 8px;
          flex-wrap: wrap;
          flex: 1;
        }

        .status-filter-item {
          display: inline-flex;
          align-items: center;
          gap: 6px;
          padding: 5px 10px;
          border: 1px solid var(--border-color);
          border-radius: 16px;
          background: transparent;
          color: var(--text-muted);
          font-size: 0.78rem;
          cursor: pointer;
        }

        .status-filter-item.active {
          border-color: var(--primary);
          background: rgba(45, 140, 240, 0.08);
          color: var(--text-main);
        }

        .status-filter-item input {
          margin: 0;
        }

        .status-dot {
          width: 8px;
          height: 8px;
          border-radius: 999px;
          display: inline-block;
        }

        .status-count {
          color: var(--text-muted);
          font-size: 0.72rem;
        }

        .filter-toggles {
          display: flex;
          gap: 16px;
          align-items: center;
        }

        .toggle-filter {
          display: flex;
          align-items: center;
          gap: 6px;
          font-size: 0.8rem;
          color: var(--text-muted);
          cursor: pointer;
        }

        .toggle-filter input {
          cursor: pointer;
        }

        .sort-controls {
          display: flex;
          align-items: center;
          gap: 10px;
          flex-wrap: wrap;
          margin-left: auto;
          flex-shrink: 0;
        }

        .sort-label {
          font-size: 0.8rem;
          color: var(--text-muted);
        }

        .sort-select {
          padding: 6px 8px;
          background: var(--bg-main);
          border: 1px solid var(--border-color);
          border-radius: 8px;
          color: var(--text-main);
          font-size: 0.8rem;
          cursor: pointer;
        }

        .btn-sort-order {
          padding: 6px 10px;
          background: var(--bg-main);
          border: 1px solid var(--border-color);
          border-radius: 8px;
          color: var(--text-main);
          font-size: 0.85rem;
          cursor: pointer;
        }

        .btn-sort-order:hover {
          border-color: var(--primary);
        }

        .result-count {
          font-size: 0.8rem;
          color: var(--text-muted);
          white-space: nowrap;
        }

        .task-grid {
          grid-template-columns: repeat(auto-fill, minmax(320px, 1fr));
        }

        .script-grid {
          flex: 1;
          overflow-y: auto;
          padding: 18px;
          display: grid;
          gap: 14px;
          align-content: start;
          background: linear-gradient(180deg, rgba(244, 247, 249, 0.6) 0%, rgba(244, 247, 249, 0.95) 100%);
        }

        .script-card {
          background: var(--bg-card);
          border: 1px solid var(--border-color);
          border-radius: 12px;
          padding: 14px;
          cursor: pointer;
          transition: all 0.2s;
          position: relative;
          overflow: hidden;
          box-shadow: 0 8px 20px rgba(11, 25, 44, 0.05);
        }

        .script-card::before {
          content: '';
          position: absolute;
          top: 0;
          left: 0;
          right: 0;
          height: 3px;
          background: linear-gradient(90deg, var(--primary) 0%, #44b2ff 100%);
          opacity: 0;
          transition: opacity 0.2s;
        }

        .script-card:hover {
          border-color: var(--primary);
          transform: translateY(-3px);
          box-shadow: 0 12px 24px rgba(11, 25, 44, 0.12);
        }

        .script-card:hover::before {
          opacity: 1;
        }

        .script-card.favorite {
          border-color: var(--warning);
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
          flex-wrap: wrap;
        }

        .script-type-icon {
          font-size: 18px;
        }

        .card-title {
          margin: 0;
          font-size: 0.95rem;
          font-weight: 600;
          color: var(--text-main);
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

        .favorite-btn.active,
        .favorite-btn:hover {
          color: #f59e0b;
        }

        .status-badge {
          display: inline-flex;
          align-items: center;
          justify-content: center;
          padding: 2px 8px;
          border-radius: 999px;
          font-size: 0.68rem;
          font-weight: 600;
          text-transform: capitalize;
        }

        .card-description {
          margin: 0 0 12px 0;
          font-size: 0.82rem;
          color: var(--text-muted);
          line-height: 1.4;
          display: -webkit-box;
          -webkit-line-clamp: 2;
          -webkit-box-orient: vertical;
          overflow: hidden;
        }

        .card-meta-row {
          display: flex;
          gap: 8px;
          flex-wrap: wrap;
          margin-bottom: 12px;
        }

        .meta-item {
          background: rgba(45, 140, 240, 0.08);
          color: #1f4e89;
          padding: 4px 8px;
          border-radius: 6px;
          font-size: 0.73rem;
        }

        .card-footer {
          display: flex;
          justify-content: space-between;
          align-items: center;
          gap: 12px;
        }

        .card-meta {
          display: flex;
          gap: 12px;
          font-size: 12px;
          color: var(--text-muted, #666);
          flex-wrap: wrap;
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

        .btn-execute:hover:not(:disabled) {
          background: #16a34a;
        }

        .btn-execute:disabled {
          opacity: 0.5;
          cursor: not-allowed;
        }

        .btn-edit, .btn-menu {
          background: var(--bg-main);
          color: var(--text-muted);
          border: 1px solid var(--border-color);
        }

        .btn-edit:hover, .btn-menu:hover {
          border-color: var(--primary);
          color: var(--primary);
        }

        .dropdown-container {
          position: relative;
        }

        .dropdown-menu {
          position: absolute;
          top: 100%;
          right: 0;
          margin-top: 4px;
          background: var(--bg-card);
          border: 1px solid var(--border-color);
          border-radius: 8px;
          padding: 4px;
          min-width: 140px;
          z-index: 100;
          box-shadow: 0 10px 24px rgba(11, 25, 44, 0.16);
        }

        .dropdown-menu button {
          width: 100%;
          padding: 8px 12px;
          background: transparent;
          border: none;
          color: var(--text-main);
          font-size: 13px;
          text-align: left;
          cursor: pointer;
          border-radius: 4px;
          display: flex;
          align-items: center;
          gap: 8px;
        }

        .dropdown-menu button:hover {
          background: rgba(45, 140, 240, 0.08);
        }

        .dropdown-menu button.danger {
          color: #ef4444;
        }

        .card-updated {
          margin-top: 12px;
          padding-top: 12px;
          border-top: 1px solid var(--border-color);
          font-size: 11px;
          color: var(--text-muted);
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
          color: var(--text-main);
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
          border: 1px solid var(--border-color);
          color: var(--text-muted);
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
          background: var(--bg-card);
          border: 1px solid var(--border-color);
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
          color: var(--text-muted, #666);
        }

        .dialog-actions {
          display: flex;
          justify-content: center;
          gap: 12px;
        }

        .btn-cancel, .btn-delete {
          padding: 8px 16px;
          border-radius: 8px;
          font-size: 14px;
          font-weight: 500;
          cursor: pointer;
        }

        .btn-cancel {
          background: transparent;
          border: 1px solid var(--border-color);
          color: var(--text-muted);
        }

        .btn-delete {
          background: #ef4444;
          border: 1px solid #ef4444;
          color: white;
        }

        @media (max-width: 900px) {
          .filter-row {
            align-items: stretch;
          }

          .status-filter {
            width: 100%;
          }
        }

        @media (max-width: 980px) {
          .list-header {
            padding: 14px;
          }

          .search-bar {
            gap: 8px;
            flex-wrap: wrap;
          }

          .search-field {
            flex-basis: 100%;
            min-width: 0;
          }

          .search-input-wrap {
            flex-basis: 100%;
          }

          .search-input {
            min-width: 0;
          }

          .type-filter-inline {
            border-left: none;
            padding-left: 0;
            margin-left: 0;
          }

          .task-grid {
            grid-template-columns: 1fr;
            padding: 12px;
          }

          .result-count {
            width: 100%;
          }
        }

        @media (max-width: 768px) {
          .header-top {
            align-items: stretch;
          }

          .header-actions {
            justify-content: stretch;
          }

          .header-actions > button {
            flex: 1;
          }
        }
      `}</style>
    </div>
  );
};
