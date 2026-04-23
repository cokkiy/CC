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
      command: '💻',
      script: '📜',
      config: '⚙️',
      file_transfer: '📁',
      upgrade: '⬆️',
      reboot: '🔄',
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
    <div className={`batch-task-card ${task.isFavorite ? 'favorite' : ''}`} onClick={() => onSelect?.(task)}>
      <div className="card-header">
        <div className="card-title-row">
          <span className="task-type-icon">{getTaskTypeIcon(task.taskType)}</span>
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
    { key: 'command', label: 'Command', icon: '💻' },
    { key: 'script', label: 'Script', icon: '📜' },
    { key: 'config', label: 'Config', icon: '⚙️' },
    { key: 'file_transfer', label: 'File', icon: '📁' },
    { key: 'upgrade', label: 'Upgrade', icon: '⬆️' },
    { key: 'reboot', label: 'Reboot', icon: '🔄' },
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
    const duplicate: BatchTask = {
      ...task,
      id: `batch-${Date.now()}`,
      name: `${task.name} (Copy)`,
      isFavorite: false,
      usageCount: 0,
      createdAt: new Date().toISOString(),
      updatedAt: new Date().toISOString(),
      status: 'draft',
    };
    onDuplicateTask?.(duplicate);
  };

  return (
    <div className="batch-task-list">
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

        {/* Search Bar */}
        <div className="search-bar">
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

        {/* Type Filter */}
        <BatchTypeFilter
          selected={selectedType}
          onChange={setSelectedType}
          counts={countsByType}
        />

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
            {filteredTasks.length} of {tasks.length} tasks
          </span>
        </div>
      </div>

      {/* Task Grid */}
      <div className="task-grid">
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
    </div>
  );
};
