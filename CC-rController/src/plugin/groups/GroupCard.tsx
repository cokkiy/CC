/**
 * GroupCard - Device Group Card and List Components
 * Part of Phase 8: Device Group and Tag System
 */

import React, { useState } from 'react';
import type { StationGroup } from './types';
import type { Station } from '../../types';

// ============================================
// Type Definitions
// ============================================

export interface GroupListProps {
  groups: StationGroup[];
  stations: Station[];
  selectedGroup: StationGroup | null;
  onSelectGroup: (group: StationGroup | null) => void;
  onEditGroup: (group: StationGroup) => void;
  onDeleteGroup: (groupId: string) => void;
  onAddStation: (groupId: string, stationId: string) => Promise<void>;
  onRemoveStation: (groupId: string, stationId: string) => void;
  onCreateGroup: () => void;
  onImport: (groups: StationGroup[]) => void;
  onExport: () => void;
  /** Batch add multiple stations to a group */
  onBatchAddStations?: (groupId: string, stationIds: string[]) => Promise<void>;
}

export interface GroupCardProps {
  group: StationGroup;
  stations: Station[];
  isSelected: boolean;
  onSelect: () => void;
  onEdit: () => void;
  onDelete: () => void;
  onAddStation: (stationId: string) => Promise<void>;
  onRemoveStation: (stationId: string) => void;
  /** Batch add multiple stations at once */
  onBatchAddStations?: (stationIds: string[]) => Promise<void>;
}

// ============================================
// GroupCard Component
// ============================================

export const GroupCard: React.FC<GroupCardProps> = ({
  group,
  stations,
  isSelected,
  onSelect,
  onEdit,
  onDelete,
  onAddStation,
  onRemoveStation,
  onBatchAddStations,
}) => {
  const [showStationPicker, setShowStationPicker] = useState(false);
  const [addNotice, setAddNotice] = useState<{ type: 'success' | 'error'; message: string } | null>(null);
  // Multi-device selection state
  const [selectedStationIds, setSelectedStationIds] = useState<string[]>([]);
  // Filter state for device search (name or IP)
  const [deviceFilter, setDeviceFilter] = useState('');
  const [isBatchAdding, setIsBatchAdding] = useState(false);

  const clearNoticeLater = () => {
    window.setTimeout(() => setAddNotice(null), 2200);
  };

  // Get station objects for this group
  const groupStations = group.station_ids
    .map(id => stations.find(s => s.id === id))
    .filter((s): s is Station => !!s);

  // Get available stations (not in this group)
  const availableStations = stations.filter(s => !(group.station_ids || []).includes(s.id));

  // Get first IP from station's network interfaces
  const getStationIp = (station: Station): string | undefined => {
    return station.networkInterfaces?.[0]?.ips?.[0];
  };

  // Filter available stations by name or IP
  const filteredAvailableStations = availableStations.filter(s => {
    if (!deviceFilter.trim()) return true;
    const query = deviceFilter.toLowerCase();
    const ip = getStationIp(s);
    return (
      s.name.toLowerCase().includes(query) ||
      (ip && ip.toLowerCase().includes(query))
    );
  });

  // Toggle selection of a station in multi-select mode
  const handleMultiSelectToggle = (stationId: string) => {
    setSelectedStationIds(prev =>
      prev.includes(stationId)
        ? prev.filter(id => id !== stationId)
        : [...prev, stationId]
    );
  };

  // Select/deselect all filtered stations
  const handleSelectAllToggle = () => {
    const filteredIds = filteredAvailableStations.map(s => s.id);
    const allSelected = filteredIds.every(id => selectedStationIds.includes(id));
    if (allSelected) {
      setSelectedStationIds(prev => prev.filter(id => !filteredIds.includes(id)));
    } else {
      setSelectedStationIds(prev => [...new Set([...prev, ...filteredIds])]);
    }
  };

  // Batch add selected stations
  const handleBatchAdd = async () => {
    if (selectedStationIds.length === 0) return;
    setIsBatchAdding(true);
    try {
      if (onBatchAddStations) {
        await onBatchAddStations(selectedStationIds);
      } else {
        // Fallback: add stations one by one
        for (const stationId of selectedStationIds) {
          await onAddStation(stationId);
        }
      }
      setAddNotice({ type: 'success', message: `Added ${selectedStationIds.length} device(s)` });
      clearNoticeLater();
      setSelectedStationIds([]);
      setDeviceFilter('');
      setShowStationPicker(false);
    } catch (error) {
      const reason = error instanceof Error ? error.message : String(error);
      setAddNotice({ type: 'error', message: `Failed to add devices: ${reason}` });
      clearNoticeLater();
    } finally {
      setIsBatchAdding(false);
    }
  };

  // Reset selection when closing picker
  const handleClosePicker = () => {
    setShowStationPicker(false);
    setSelectedStationIds([]);
    setDeviceFilter('');
  };

  return (
    <div className={`group-card ${isSelected ? 'selected' : ''}`} onClick={onSelect}>
      <div className="group-card-header">
        <div className="group-color-indicator" style={{ backgroundColor: group.color || '#6366f1' }} />
        <div className="group-info">
          <h3 className="group-name">{group.name}</h3>
          {group.description && (
            <p className="group-description">{group.description}</p>
          )}
        </div>
        <div className="group-actions">
          <button
            className="action-btn edit"
            onClick={(e) => { e.stopPropagation(); onEdit(); }}
            title="Edit group"
          >
            ✎
          </button>
          <button
            className="action-btn delete"
            onClick={(e) => { e.stopPropagation(); onDelete(); }}
            title="Delete group"
          >
            ✕
          </button>
        </div>
      </div>

      <div className="group-stats">
        <span className="station-count">{(group.station_ids || []).length} devices</span>
      </div>

      <div className="group-stations">
        {groupStations.length > 0 ? (
          <div className="station-badges">
            {groupStations.slice(0, 5).map(station => (
              <span key={station.id} className="station-badge">
                {station.name}
                <button
                  className="badge-remove"
                  onClick={(e) => { e.stopPropagation(); onRemoveStation(station.id); }}
                >
                  ×
                </button>
              </span>
            ))}
            {groupStations.length > 5 && (
              <span className="station-badge more">+{groupStations.length - 5} more</span>
            )}
          </div>
        ) : (
          <span className="no-stations">No devices assigned</span>
        )}
      </div>

      <div className="group-card-footer">
        <button
          className="add-station-btn"
          onClick={(e) => { e.stopPropagation(); setShowStationPicker(!showStationPicker); }}
        >
          {showStationPicker ? 'Cancel' : '+ Add Devices'}
        </button>
      </div>

      {showStationPicker && (
        <div className="station-picker multi-select" onClick={(e) => e.stopPropagation()}>
          <h4>Add Devices To Group</h4>
          <p className="station-picker-hint">Filter by device name or IP, then add several devices at once.</p>
          {availableStations.length === 0 ? (
            <p className="station-picker-empty">All devices are already assigned to this group.</p>
          ) : (
            <>
              {/* Filter input */}
              <div className="device-filter">
                <input
                  type="text"
                  placeholder="Filter by Name or IP..."
                  value={deviceFilter}
                  onChange={(e) => setDeviceFilter(e.target.value)}
                  className="filter-input"
                />
              </div>

              {/* Select all / selection info */}
              <div className="selection-bar">
                <label className="select-all-label">
                  <input
                    type="checkbox"
                    aria-label="Select all filtered devices"
                    checked={filteredAvailableStations.length > 0 && filteredAvailableStations.every(s => selectedStationIds.includes(s.id))}
                    onChange={handleSelectAllToggle}
                    disabled={filteredAvailableStations.length === 0}
                  />
                  <span>Select All{deviceFilter && ` (${filteredAvailableStations.length})`}</span>
                </label>
                {selectedStationIds.length > 0 && (
                  <span className="selection-count">{selectedStationIds.length} selected</span>
                )}
              </div>

              {/* Device list with checkboxes */}
              <div className="device-list">
                {filteredAvailableStations.length === 0 ? (
                  <p className="no-results">No devices match your filter</p>
                ) : (
                  filteredAvailableStations.map(station => {
                    const stationIp = getStationIp(station);
                    return (
                      <label key={station.id} className="device-option">
                        <input
                          type="checkbox"
                          aria-label={`Select ${station.name}`}
                          checked={selectedStationIds.includes(station.id)}
                          onChange={() => handleMultiSelectToggle(station.id)}
                        />
                        <span className="device-name">{station.name}</span>
                        {stationIp && <span className="device-ip">{stationIp}</span>}
                      </label>
                    );
                  })
                )}
              </div>

              {/* Action buttons */}
              <div className="station-picker-actions">
                <button
                  type="button"
                  className="cancel-btn"
                  onClick={handleClosePicker}
                >
                  Cancel
                </button>
                <button
                  type="button"
                  className="batch-add-btn"
                  onClick={handleBatchAdd}
                  disabled={selectedStationIds.length === 0 || isBatchAdding}
                >
                  {isBatchAdding ? 'Adding...' : `Add Selected (${selectedStationIds.length})`}
                </button>
              </div>
            </>
          )}
        </div>
      )}

      {addNotice ? <p className={`station-added-notice ${addNotice.type}`}>{addNotice.message}</p> : null}

      <style>{`
        .group-card {
          background: var(--bg-card);
          border: 1px solid var(--border-color);
          border-radius: 10px;
          padding: 14px;
          cursor: pointer;
          transition: all 0.2s;
          position: relative;
        }

        .group-card:hover {
          border-color: var(--primary);
          box-shadow: 0 2px 8px rgba(99, 102, 241, 0.15);
        }

        .group-card.selected {
          border-color: var(--primary);
          background: rgba(99, 102, 241, 0.05);
        }

        .group-card-header {
          display: flex;
          align-items: flex-start;
          gap: 10px;
          margin-bottom: 10px;
        }

        .group-color-indicator {
          width: 4px;
          height: 40px;
          border-radius: 2px;
          flex-shrink: 0;
        }

        .group-info {
          flex: 1;
          min-width: 0;
        }

        .group-name {
          margin: 0;
          font-size: 0.95rem;
          font-weight: 600;
          color: var(--text-main);
        }

        .group-description {
          margin: 4px 0 0;
          font-size: 0.8rem;
          color: var(--text-secondary);
          overflow: hidden;
          text-overflow: ellipsis;
          white-space: nowrap;
        }

        .group-actions {
          display: flex;
          gap: 4px;
          opacity: 0;
          transition: opacity 0.2s;
        }

        .group-card:hover .group-actions {
          opacity: 1;
        }

        .action-btn {
          width: 26px;
          height: 26px;
          border: none;
          border-radius: 4px;
          cursor: pointer;
          font-size: 12px;
          transition: all 0.2s;
        }

        .action-btn.edit {
          background: var(--bg-main);
          color: var(--text-secondary);
        }

        .action-btn.edit:hover {
          background: var(--primary);
          color: white;
        }

        .action-btn.delete {
          background: var(--bg-main);
          color: var(--text-secondary);
        }

        .action-btn.delete:hover {
          background: #ef4444;
          color: white;
        }

        .group-stats {
          display: flex;
          gap: 12px;
          margin-bottom: 10px;
          font-size: 0.75rem;
          color: var(--text-secondary);
        }

        .station-count {
          display: flex;
          align-items: center;
          gap: 4px;
        }

        .station-picker-hint {
          margin: -4px 0 12px;
          font-size: 0.78rem;
          color: var(--text-secondary);
        }

        .group-stations {
          margin-bottom: 10px;
        }

        .station-badges {
          display: flex;
          flex-wrap: wrap;
          gap: 6px;
        }

        .station-badge {
          display: inline-flex;
          align-items: center;
          gap: 4px;
          padding: 3px 8px;
          background: var(--bg-main);
          border: 1px solid var(--border-color);
          border-radius: 12px;
          font-size: 0.72rem;
          color: var(--text-main);
        }

        .station-badge.more {
          background: transparent;
          border-color: var(--text-secondary);
          color: var(--text-secondary);
        }

        .badge-remove {
          background: none;
          border: none;
          cursor: pointer;
          color: var(--text-secondary);
          font-size: 14px;
          line-height: 1;
          padding: 0;
          margin-left: 2px;
        }

        .badge-remove:hover {
          color: #ef4444;
        }

        .no-stations {
          font-size: 0.75rem;
          color: var(--text-secondary);
          font-style: italic;
        }

        .group-card-footer {
          display: flex;
          justify-content: flex-end;
        }

        .add-station-btn {
          padding: 4px 10px;
          background: transparent;
          border: 1px dashed var(--border-color);
          border-radius: 4px;
          font-size: 0.75rem;
          color: var(--text-secondary);
          cursor: pointer;
          transition: all 0.2s;
        }

        .add-station-btn:hover {
          border-color: var(--primary);
          color: var(--primary);
        }

        .station-picker {
          position: absolute;
          top: 100%;
          left: 0;
          right: 0;
          margin-top: 8px;
          background: var(--bg-card);
          border: 1px solid var(--border-color);
          border-radius: 8px;
          padding: 12px;
          box-shadow: 0 4px 12px rgba(0, 0, 0, 0.15);
          z-index: 10;
        }

        .station-picker h4 {
          margin: 0 0 8px;
          font-size: 0.85rem;
          color: var(--text-main);
        }

        .station-picker-empty {
          margin: 0;
          font-size: 0.8rem;
          color: var(--text-secondary);
        }

        .station-picker-inline {
          display: flex;
          gap: 8px;
          align-items: center;
        }

        .station-picker-inline select {
          flex: 1;
          min-width: 0;
          padding: 6px 8px;
          border: 1px solid var(--border-color);
          border-radius: 6px;
          background: var(--bg-main);
          color: var(--text-main);
          font-size: 0.82rem;
        }

        .station-picker-inline button {
          padding: 6px 10px;
          border: 1px solid var(--primary);
          border-radius: 6px;
          background: var(--primary);
          color: white;
          font-size: 0.78rem;
          cursor: pointer;
        }

        .station-picker-inline button:disabled {
          opacity: 0.5;
          cursor: not-allowed;
        }

        .station-added-notice {
          margin: 8px 0 0;
          font-size: 0.75rem;
          border-radius: 6px;
          padding: 6px 8px;
        }

        .station-added-notice.success {
          color: #0f5132;
          background: #d1e7dd;
          border: 1px solid #badbcc;
        }

        .station-added-notice.error {
          color: #842029;
          background: #f8d7da;
          border: 1px solid #f5c2c7;
        }

        .station-list {
          max-height: 150px;
          overflow-y: auto;
        }

        .station-option {
          display: flex;
          align-items: center;
          gap: 8px;
          padding: 6px;
          border-radius: 4px;
          cursor: pointer;
          font-size: 0.8rem;
        }

        .station-option:hover {
          background: var(--bg-main);
        }

        .station-option input {
          cursor: pointer;
        }

        /* Multi-select station picker styles */
        .station-picker.multi-select {
          position: absolute;
          top: 100%;
          left: 0;
          right: 0;
          margin-top: 8px;
          background: var(--bg-card);
          border: 1px solid var(--border-color);
          border-radius: 8px;
          padding: 12px;
          box-shadow: 0 4px 12px rgba(0, 0, 0, 0.15);
          z-index: 10;
          min-width: 280px;
        }

        .station-picker.multi-select h4 {
          margin: 0 0 10px;
          font-size: 0.88rem;
          color: var(--text-main);
          font-weight: 600;
        }

        .device-filter {
          margin-bottom: 10px;
        }

        .device-filter .filter-input {
          width: 100%;
          padding: 7px 10px;
          border: 1px solid var(--border-color);
          border-radius: 6px;
          background: var(--bg-main);
          color: var(--text-main);
          font-size: 0.82rem;
          box-sizing: border-box;
        }

        .device-filter .filter-input:focus {
          outline: none;
          border-color: var(--primary);
        }

        .device-filter .filter-input::placeholder {
          color: var(--text-secondary);
        }

        .selection-bar {
          display: flex;
          align-items: center;
          justify-content: space-between;
          padding: 6px 0;
          border-bottom: 1px solid var(--border-color);
          margin-bottom: 8px;
        }

        .select-all-label {
          display: flex;
          align-items: center;
          gap: 6px;
          font-size: 0.8rem;
          color: var(--text-main);
          cursor: pointer;
        }

        .select-all-label input {
          cursor: pointer;
        }

        .selection-count {
          font-size: 0.75rem;
          color: var(--primary);
          font-weight: 500;
        }

        .device-list {
          max-height: 180px;
          overflow-y: auto;
          margin-bottom: 10px;
        }

        .device-option {
          display: flex;
          align-items: center;
          gap: 8px;
          padding: 7px 6px;
          border-radius: 4px;
          cursor: pointer;
          font-size: 0.82rem;
          transition: background 0.15s;
        }

        .device-option:hover {
          background: var(--bg-main);
        }

        .device-option input {
          cursor: pointer;
          flex-shrink: 0;
        }

        .device-option .device-name {
          flex: 1;
          color: var(--text-main);
          overflow: hidden;
          text-overflow: ellipsis;
          white-space: nowrap;
        }

        .device-option .device-ip {
          font-size: 0.72rem;
          color: var(--text-secondary);
          flex-shrink: 0;
        }

        .no-results {
          padding: 12px;
          text-align: center;
          color: var(--text-secondary);
          font-size: 0.8rem;
          margin: 0;
        }

        .station-picker-actions {
          display: flex;
          gap: 8px;
          justify-content: flex-end;
          padding-top: 8px;
          border-top: 1px solid var(--border-color);
        }

        .station-picker-actions .cancel-btn {
          padding: 6px 12px;
          border: 1px solid var(--border-color);
          border-radius: 6px;
          background: transparent;
          color: var(--text-secondary);
          font-size: 0.78rem;
          cursor: pointer;
          transition: all 0.2s;
        }

        .station-picker-actions .cancel-btn:hover {
          border-color: var(--text-secondary);
          color: var(--text-main);
        }

        .station-picker-actions .batch-add-btn {
          padding: 6px 14px;
          border: 1px solid var(--primary);
          border-radius: 6px;
          background: var(--primary);
          color: white;
          font-size: 0.78rem;
          cursor: pointer;
          transition: all 0.2s;
        }

        .station-picker-actions .batch-add-btn:hover:not(:disabled) {
          opacity: 0.9;
        }

        .station-picker-actions .batch-add-btn:disabled {
          opacity: 0.5;
          cursor: not-allowed;
        }
      `}</style>
    </div>
  );
};

// ============================================
// GroupList Component
// ============================================

export const GroupList: React.FC<GroupListProps> = ({
  groups,
  stations,
  selectedGroup,
  onSelectGroup,
  onEditGroup,
  onDeleteGroup,
  onAddStation,
  onRemoveStation,
  onCreateGroup,
  onImport,
  onExport,
  onBatchAddStations,
}) => {
  const [searchTerm, setSearchTerm] = useState('');

  // Filter groups by search term
  const filteredGroups = groups.filter(
    g =>
      g.name.toLowerCase().includes(searchTerm.toLowerCase()) ||
      g.description?.toLowerCase().includes(searchTerm.toLowerCase())
  );

  // Handle file import
  const handleImportClick = () => {
    const input = document.createElement('input');
    input.type = 'file';
    input.accept = '.json';
    input.onchange = async (e) => {
      const file = (e.target as HTMLInputElement).files?.[0];
      if (!file) return;
      try {
        const text = await file.text();
        const imported = JSON.parse(text) as StationGroup[];
        onImport(imported);
      } catch (err) {
        console.error('[GroupList] Failed to parse import file:', err);
      }
    };
    input.click();
  };

  return (
    <div className="group-list-container">
      <div className="group-list-header">
        <div className="search-box">
          <input
            type="text"
            placeholder="Search groups..."
            value={searchTerm}
            onChange={(e) => setSearchTerm(e.target.value)}
          />
        </div>
        <div className="header-actions">
          <button className="header-btn" onClick={handleImportClick}>
            Import
          </button>
          <button className="header-btn" onClick={onExport}>
            Export
          </button>
          <button className="header-btn primary" onClick={onCreateGroup}>
            + New Group
          </button>
        </div>
      </div>

      <div className="group-list-content">
        {filteredGroups.length === 0 ? (
          <div className="empty-state">
            {groups.length === 0 ? (
              <>
                <p>No groups created yet</p>
                <button onClick={onCreateGroup}>Create your first group</button>
              </>
            ) : (
              <p>No groups match your search</p>
            )}
          </div>
        ) : (
          <div className="group-grid">
            {filteredGroups.map(group => (
              <GroupCard
                key={group.id}
                group={group}
                stations={stations}
                isSelected={selectedGroup?.id === group.id}
                onSelect={() => onSelectGroup(group)}
                onEdit={() => onEditGroup(group)}
                onDelete={() => onDeleteGroup(group.id)}
                onAddStation={(stationId) => onAddStation(group.id, stationId)}
                onRemoveStation={(stationId) => onRemoveStation(group.id, stationId)}
                onBatchAddStations={onBatchAddStations ? (stationIds) => onBatchAddStations(group.id, stationIds) : undefined}
              />
            ))}
          </div>
        )}
      </div>

      <style>{`
        .group-list-container {
          display: flex;
          flex-direction: column;
          height: 100%;
        }

        .group-list-header {
          display: flex;
          justify-content: space-between;
          align-items: center;
          padding: 12px 16px;
          border-bottom: 1px solid var(--border-color);
          gap: 12px;
        }

        .search-box input {
          padding: 8px 12px;
          border: 1px solid var(--border-color);
          border-radius: 6px;
          font-size: 0.85rem;
          width: 220px;
          background: var(--bg-main);
          color: var(--text-main);
        }

        .search-box input:focus {
          outline: none;
          border-color: var(--primary);
        }

        .header-actions {
          display: flex;
          gap: 8px;
        }

        .header-btn {
          padding: 8px 14px;
          border: 1px solid var(--border-color);
          border-radius: 6px;
          background: transparent;
          color: var(--text-main);
          font-size: 0.82rem;
          cursor: pointer;
          transition: all 0.2s;
        }

        .header-btn:hover {
          border-color: var(--primary);
          color: var(--primary);
        }

        .header-btn.primary {
          background: var(--primary);
          border-color: var(--primary);
          color: white;
        }

        .header-btn.primary:hover {
          opacity: 0.9;
        }

        .group-list-content {
          flex: 1;
          overflow-y: auto;
          padding: 16px;
        }

        .group-grid {
          display: grid;
          grid-template-columns: repeat(auto-fill, minmax(280px, 1fr));
          gap: 14px;
        }

        .empty-state {
          display: flex;
          flex-direction: column;
          align-items: center;
          justify-content: center;
          height: 200px;
          color: var(--text-secondary);
        }

        .empty-state p {
          margin-bottom: 12px;
        }

        .empty-state button {
          padding: 8px 16px;
          background: var(--primary);
          border: none;
          border-radius: 6px;
          color: white;
          cursor: pointer;
        }
      `}</style>
    </div>
  );
};

export default GroupList;
