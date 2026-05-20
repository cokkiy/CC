/**
 * TagsPage - Main container for Tag Definition Management UI
 * Part of Phase 8: Device Group and Tag System
 */

import React, { useState } from 'react';
import { useTags } from './TagsContext';
import { TagDefinitionList, TagEditor } from './TagEditor';
import type { TagDefinition, CreateTagDTO, UpdateTagDTO } from './types';
import type { Station } from '../../types';

export interface TagsPageProps {
  stations: Station[];
}

export const TagsPage: React.FC<TagsPageProps> = ({ stations }) => {
  const {
    tagDefinitions,
    selectedTag,
    createTag,
    updateTag,
    deleteTag,
    selectTag,
    importTagDefinitions,
    exportTagDefinitions,
    getTagStats,
    getStationTags,
    updateStationTags,
    batchUpdateStationTags,
  } = useTags();

  const [editorOpen, setEditorOpen] = useState(false);
  const [editingTag, setEditingTag] = useState<TagDefinition | null>(null);
  const [isLoading, setIsLoading] = useState(false);
  const [stats, setStats] = useState<Array<{ tagKey: string; tagLabel: string; tagValue: string; count: number }>>([]);
  // Multi-device selection state
  const [selectedStationIds, setSelectedStationIds] = useState<string[]>([]);
  const [deviceFilter, setDeviceFilter] = useState('');
  const [showDevicePicker, setShowDevicePicker] = useState(false);
  // Single device tag editing state (for editing tags of a single device)
  const [selectedStationId, setSelectedStationId] = useState('');
  const [stationTags, setStationTags] = useState<Record<string, string>>({});
  const [savedStationTags, setSavedStationTags] = useState<Record<string, string>>({});
  const [isStationTagsLoading, setIsStationTagsLoading] = useState(false);
  const [isStationTagsSaving, setIsStationTagsSaving] = useState(false);
  const [stationTagNotice, setStationTagNotice] = useState<{ type: 'success' | 'error'; message: string } | null>(null);
  const [isNoticeLeaving, setIsNoticeLeaving] = useState(false);
  const [isBatchSaving, setIsBatchSaving] = useState(false);
  const [batchNotice, setBatchNotice] = useState<{ type: 'success' | 'error'; message: string } | null>(null);

  const normalizeTagMap = React.useCallback((tags: Record<string, string>): Record<string, string> => {
    const normalized: Record<string, string> = {};
    Object.entries(tags).forEach(([key, value]) => {
      const trimmed = value.trim();
      if (trimmed) {
        normalized[key] = trimmed;
      }
    });
    return normalized;
  }, []);

  const hasUnsavedChanges = React.useMemo(() => {
    const current = normalizeTagMap(stationTags);
    const saved = normalizeTagMap(savedStationTags);
    const currentKeys = Object.keys(current);
    const savedKeys = Object.keys(saved);

    if (currentKeys.length !== savedKeys.length) return true;
    return currentKeys.some((key) => saved[key] !== current[key]);
  }, [stationTags, savedStationTags, normalizeTagMap]);

  const hasSelectedDevices = Boolean(selectedStationId) || selectedStationIds.length > 0;
  const selectedDeviceCount = selectedStationIds.length > 0 ? selectedStationIds.length : selectedStationId ? 1 : 0;
  const isBatchSelection = selectedStationIds.length > 1;

  // Load tag stats on mount
  React.useEffect(() => {
    loadStats();
  }, []);

  React.useEffect(() => {
    if (!selectedStationId || stations.length === 0) {
      setStationTags({});
      setSavedStationTags({});
      return;
    }

    setIsStationTagsLoading(true);
    getStationTags(selectedStationId)
      .then((tags) => {
        const nextTags = tags || {};
        setStationTags(nextTags);
        setSavedStationTags(nextTags);
      })
      .catch((error) => {
        console.error('[TagsPage] Failed to load station tags:', error);
        setStationTags({});
        setSavedStationTags({});
        setStationTagNotice({ type: 'error', message: 'Failed to load device tags.' });
      })
      .finally(() => setIsStationTagsLoading(false));
  }, [selectedStationId, stations.length, getStationTags]);

  React.useEffect(() => {
    if (!stationTagNotice) return;

    setIsNoticeLeaving(false);

    const fadeTimer = window.setTimeout(() => {
      setIsNoticeLeaving(true);
    }, 2700);

    const timer = window.setTimeout(() => {
      setStationTagNotice(null);
      setIsNoticeLeaving(false);
    }, 3000);

    return () => {
      window.clearTimeout(fadeTimer);
      window.clearTimeout(timer);
    };
  }, [stationTagNotice]);

  // Get first IP from station's network interfaces
  const getStationIp = (station: Station): string | undefined => {
    return station.networkInterfaces?.[0]?.ips?.[0];
  };

  // Filter stations by name or IP
  const filteredStations = React.useMemo(() => {
    if (!deviceFilter.trim()) return stations;
    const query = deviceFilter.toLowerCase();
    return stations.filter(s => {
      const ip = getStationIp(s);
      return (
        s.name.toLowerCase().includes(query) ||
        (ip && ip.toLowerCase().includes(query))
      );
    });
  }, [stations, deviceFilter]);

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
    const filteredIds = filteredStations.map(s => s.id);
    const allSelected = filteredIds.every(id => selectedStationIds.includes(id));
    if (allSelected) {
      setSelectedStationIds(prev => prev.filter(id => !filteredIds.includes(id)));
    } else {
      setSelectedStationIds(prev => [...new Set([...prev, ...filteredIds])]);
    }
  };

  // Batch save tags to multiple devices
  const handleBatchSaveTags = async () => {
    if (selectedStationIds.length === 0) return;

    setIsBatchSaving(true);
    setBatchNotice(null);
    try {
      const tagsToApply = normalizeTagMap(stationTags);
      await batchUpdateStationTags(selectedStationIds, tagsToApply);
      setBatchNotice({ type: 'success', message: `Tag values saved to ${selectedStationIds.length} device(s).` });
      // Clear the editing form after batch save
      setStationTags({});
      setSavedStationTags({});
      setSelectedStationIds([]);
      setSelectedStationId('');
      setDeviceFilter('');
      setShowDevicePicker(false);
      loadStats();
    } catch (error) {
      console.error('[TagsPage] Failed to batch update station tags:', error);
      setBatchNotice({ type: 'error', message: 'Failed to save tag values to devices.' });
    } finally {
      setIsBatchSaving(false);
    }
  };

  // Reset selection when closing picker
  const handleCloseDevicePicker = () => {
    setShowDevicePicker(false);
    setDeviceFilter('');
  };

  const handleApplyDeviceSelection = () => {
    if (selectedStationIds.length === 0) return;

    if (selectedStationIds.length === 1) {
      setSelectedStationId(selectedStationIds[0]);
      setBatchNotice(null);
    } else {
      setSelectedStationId('');
      setStationTags({});
      setSavedStationTags({});
      setStationTagNotice(null);
    }

    setShowDevicePicker(false);
  };

  const handleClearDeviceSelection = () => {
    setSelectedStationIds([]);
    setSelectedStationId('');
    setStationTags({});
    setSavedStationTags({});
    setDeviceFilter('');
    setBatchNotice(null);
    setStationTagNotice(null);
    setShowDevicePicker(false);
  };

  // Clear batch notice after delay
  React.useEffect(() => {
    if (!batchNotice) return;
    const timer = window.setTimeout(() => setBatchNotice(null), 3000);
    return () => window.clearTimeout(timer);
  }, [batchNotice]);

  const loadStats = async () => {
    try {
      const tagStats = await getTagStats();
      setStats(tagStats);
    } catch (error) {
      console.error('[TagsPage] Failed to load tag stats:', error);
    }
  };

  // Open editor for new tag
  const handleCreateTag = () => {
    setEditingTag(null);
    setEditorOpen(true);
  };

  // Open editor for existing tag
  const handleEditTag = (tag: TagDefinition) => {
    setEditingTag(tag);
    setEditorOpen(true);
  };

  // Close editor
  const handleCloseEditor = () => {
    setEditorOpen(false);
    setEditingTag(null);
  };

  // Save tag (create or update)
  const handleSaveTag = async (data: CreateTagDTO | UpdateTagDTO) => {
    setIsLoading(true);
    try {
      if (editingTag) {
        await updateTag(editingTag.key || editingTag.id, data as UpdateTagDTO);
      } else {
        await createTag(data as CreateTagDTO);
      }
      handleCloseEditor();
      loadStats();
    } catch (error) {
      console.error('[TagsPage] Failed to save tag:', error);
    } finally {
      setIsLoading(false);
    }
  };

  // Delete tag
  const handleDeleteTag = async (key: string) => {
    if (!confirm('Are you sure you want to delete this tag definition? This will not remove tag values from devices.')) return;
    setIsLoading(true);
    try {
      await deleteTag(key);
      loadStats();
    } catch (error) {
      console.error('[TagsPage] Failed to delete tag:', error);
    } finally {
      setIsLoading(false);
    }
  };

  // Import tag definitions
  const handleImportTags = async (tagsToImport: TagDefinition[]) => {
    setIsLoading(true);
    try {
      await importTagDefinitions(tagsToImport);
      loadStats();
    } catch (error) {
      console.error('[TagsPage] Failed to import tags:', error);
    } finally {
      setIsLoading(false);
    }
  };

  // Export tag definitions
  const handleExportTags = async () => {
    try {
      const tags = await exportTagDefinitions();
      // Create and download JSON file
      const blob = new Blob([JSON.stringify(tags, null, 2)], { type: 'application/json' });
      const url = URL.createObjectURL(blob);
      const a = document.createElement('a');
      a.href = url;
      a.download = `tags-export-${new Date().toISOString().split('T')[0]}.json`;
      a.click();
      URL.revokeObjectURL(url);
    } catch (error) {
      console.error('[TagsPage] Failed to export tags:', error);
    }
  };

  const handleStationTagChange = (key: string, value: string) => {
    setStationTags((prev) => {
      const next = { ...prev };
      if (!value.trim()) {
        delete next[key];
      } else {
        next[key] = value;
      }
      return next;
    });
  };

  const handleSaveStationTags = async () => {
    if (!selectedStationId) return;

    setIsStationTagsSaving(true);
    setStationTagNotice(null);
    try {
      const updated = await updateStationTags(selectedStationId, normalizeTagMap(stationTags));
      const normalizedUpdated = normalizeTagMap(updated || {});
      setStationTags(normalizedUpdated);
      setSavedStationTags(normalizedUpdated);
      setStationTagNotice({ type: 'success', message: 'Device tag values saved successfully.' });
    } catch (error) {
      console.error('[TagsPage] Failed to update station tags:', error);
      setStationTagNotice({ type: 'error', message: 'Failed to save device tag values.' });
    } finally {
      setIsStationTagsSaving(false);
    }
  };

  const handleDiscardStationTagChanges = () => {
    if (!selectedStationId) {
      setStationTags({});
      return;
    }

    setStationTagNotice(null);
    setStationTags(savedStationTags);
    setStationTagNotice({ type: 'success', message: 'Discarded local changes and restored saved values.' });
  };

  const handleApplyToAllStations = async () => {
    if (stations.length === 0) return;
    if (!confirm('Apply current tag values to all stations? Existing keys will be overwritten for each station.')) {
      return;
    }

    setIsStationTagsSaving(true);
    setStationTagNotice(null);
    try {
      let tagsToApply = normalizeTagMap(stationTags);

      if (selectedStationId && Object.keys(tagsToApply).length === 0) {
        const refreshed = await getStationTags(selectedStationId);
        if (refreshed && Object.keys(refreshed).length > 0) {
          tagsToApply = normalizeTagMap(refreshed);
          setStationTags(tagsToApply);
        }
      }

      await batchUpdateStationTags(
        stations.map((station) => station.id),
        tagsToApply,
      );
      setSavedStationTags(tagsToApply);
      setStationTagNotice({ type: 'success', message: 'Tag values applied to all devices.' });
    } catch (error) {
      console.error('[TagsPage] Failed to batch update station tags:', error);
      setStationTagNotice({ type: 'error', message: 'Failed to apply tag values to all devices.' });
    } finally {
      setIsStationTagsSaving(false);
    }
  };

  return (
    <div className="tags-page-shell">
      <main className="grid gridTagsMode tags-main-grid">
        <section className="panel tags-main-panel">
          <TagDefinitionList
            tagDefinitions={tagDefinitions}
            stats={stats}
            selectedTag={selectedTag}
            onSelectTag={selectTag}
            onEditTag={handleEditTag}
            onDeleteTag={handleDeleteTag}
            onCreateTag={handleCreateTag}
            onImport={handleImportTags}
            onExport={handleExportTags}
          />

          <div className="station-tags-editor">
            <div className="station-tags-header">
              <h3>Device Tag Values</h3>
              <span className="station-tags-hint">Select one device to edit existing values, or several devices to batch assign values once.</span>
            </div>

            <div className="station-tags-body">
              <div className="station-picker-row">
                <div className="device-picker-label">
                  <label htmlFor="device-mode-picker">Devices</label>
                  <span className="device-mode-hint">Choose one device to edit tags, or select multiple to batch assign</span>
                </div>
                <div className="device-mode-controls">
                  <button
                    type="button"
                    className={`device-picker-btn ${showDevicePicker ? 'active' : ''}`}
                    onClick={() => setShowDevicePicker(!showDevicePicker)}
                  >
                    {selectedDeviceCount > 0 ? `${selectedDeviceCount} device(s) selected` : 'Select Devices...'}
                  </button>
                  {hasSelectedDevices ? (
                    <button
                      type="button"
                      className="device-clear-btn"
                      onClick={handleClearDeviceSelection}
                    >
                      Clear
                    </button>
                  ) : null}
                </div>
              </div>

              {batchNotice ? (
                <p
                  className={`station-tags-notice ${batchNotice.type}`}
                  role={batchNotice.type === 'error' ? 'alert' : 'status'}
                >
                  {batchNotice.message}
                </p>
              ) : null}

              {showDevicePicker && (
                <div className="device-picker-panel">
                  <h4>Select Devices</h4>

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
                        checked={filteredStations.length > 0 && filteredStations.every(s => selectedStationIds.includes(s.id))}
                        onChange={handleSelectAllToggle}
                        disabled={filteredStations.length === 0}
                      />
                      <span>Select All{deviceFilter && ` (${filteredStations.length})`}</span>
                    </label>
                    {selectedStationIds.length > 0 && (
                      <span className="selection-count">{selectedStationIds.length} selected</span>
                    )}
                  </div>

                  {/* Device list with checkboxes */}
                  <div className="device-list">
                    {filteredStations.length === 0 ? (
                      <p className="no-results">No devices match your filter</p>
                    ) : (
                      filteredStations.map(station => {
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
                  <div className="device-picker-actions">
                    <button
                      type="button"
                      className="cancel-btn"
                      onClick={handleCloseDevicePicker}
                    >
                      Cancel
                    </button>
                    <button
                      type="button"
                      className="apply-btn"
                      onClick={handleApplyDeviceSelection}
                      disabled={selectedStationIds.length === 0}
                    >
                      Apply Selection
                    </button>
                  </div>
                </div>
              )}

              {stationTagNotice ? (
                <p
                  className={`station-tags-notice ${stationTagNotice.type} ${isNoticeLeaving ? 'leaving' : ''}`}
                  role={stationTagNotice.type === 'error' ? 'alert' : 'status'}
                >
                  {stationTagNotice.message}
                </p>
              ) : null}

              {!hasSelectedDevices ? (
                <p className="station-tags-empty">Select one or more devices to edit their tag values.</p>
              ) : isStationTagsLoading ? (
                <p className="station-tags-empty">Loading device tags...</p>
              ) : tagDefinitions.length === 0 ? (
                <p className="station-tags-empty">Create at least one tag definition to start assigning values.</p>
              ) : (
                <div className="station-tags-form">
                  {tagDefinitions.map((definition) => {
                    const tagKey = definition.key || definition.id;
                    const tagLabel = definition.label || definition.name || tagKey;
                    const currentValue = stationTags[tagKey] ?? '';

                    if (definition.type === 'boolean') {
                      return (
                        <div className="station-tag-field" key={tagKey}>
                          <label>{tagLabel}</label>
                          <select
                            value={currentValue}
                            onChange={(event) => handleStationTagChange(tagKey, event.target.value)}
                          >
                            <option value="">Unset</option>
                            <option value="true">True</option>
                            <option value="false">False</option>
                          </select>
                        </div>
                      );
                    }

                    if (definition.type === 'select' && definition.options && definition.options.length > 0) {
                      return (
                        <div className="station-tag-field" key={tagKey}>
                          <label>{tagLabel}</label>
                          <select
                            value={currentValue}
                            onChange={(event) => handleStationTagChange(tagKey, event.target.value)}
                          >
                            <option value="">Unset</option>
                            {definition.options.map((option) => (
                              <option key={option} value={option}>
                                {option}
                              </option>
                            ))}
                          </select>
                        </div>
                      );
                    }

                    return (
                      <div className="station-tag-field" key={tagKey}>
                        <label>{tagLabel}</label>
                        <input
                          type={definition.type === 'number' ? 'number' : 'text'}
                          value={currentValue}
                          onChange={(event) => handleStationTagChange(tagKey, event.target.value)}
                          placeholder={`Value for ${tagLabel}`}
                        />
                      </div>
                    );
                  })}
                </div>
              )}

              <div className="station-tags-toolbar station-tags-toolbar-bottom">
                <div className="station-tags-actions">
                  <button
                    type="button"
                    data-testid="station-tags-discard-btn"
                    onClick={handleDiscardStationTagChanges}
                    disabled={!selectedStationId || isBatchSelection || isStationTagsSaving || !hasUnsavedChanges}
                  >
                    Discard Changes
                  </button>
                  <button
                    type="button"
                    data-testid="station-tags-apply-all-btn"
                    onClick={() => void handleApplyToAllStations()}
                    disabled={stations.length === 0 || isStationTagsSaving}
                  >
                    Apply to All Devices
                  </button>
                  <button
                    type="button"
                    data-testid="station-tags-save-btn"
                    className={`primary ${hasUnsavedChanges ? 'unsaved' : ''}`}
                    onClick={() => void handleSaveStationTags()}
                    disabled={!selectedStationId || isBatchSelection || isStationTagsSaving || !hasUnsavedChanges}
                  >
                    {isStationTagsSaving ? 'Saving...' : 'Save Changes'}
                  </button>
                  {isBatchSelection && (
                    <button
                      type="button"
                      className="batch-save-btn primary"
                      onClick={() => void handleBatchSaveTags()}
                      disabled={!hasUnsavedChanges || isBatchSaving}
                    >
                      {isBatchSaving ? 'Saving...' : `Save to ${selectedStationIds.length} Devices`}
                    </button>
                  )}
                </div>
              </div>
            </div>

          </div>
        </section>
      </main>

      {editorOpen && (
        <div className="tags-layer tags-layer-modal" role="dialog" aria-modal="true">
          <div className="tags-modal-panel">
            <TagEditor
              tag={editingTag}
              onSave={handleSaveTag}
              onCancel={handleCloseEditor}
              isLoading={isLoading}
            />
          </div>
        </div>
      )}
      <style>{`
        /* Device picker styles */
        .device-picker-label {
          display: flex;
          flex-direction: column;
          gap: 2px;
        }

        .device-picker-label label {
          font-size: 0.78rem;
          font-weight: 500;
          color: var(--text-muted);
          text-transform: uppercase;
          letter-spacing: 0.04em;
        }

        .device-mode-hint {
          font-size: 0.72rem;
          color: var(--text-secondary);
          font-weight: normal;
          text-transform: none;
          letter-spacing: normal;
        }

        .device-mode-controls {
          display: flex;
          gap: 8px;
          align-items: center;
        }

        .device-picker-btn {
          padding: 8px 14px;
          border: 1px solid var(--border-color);
          border-radius: 6px;
          background: var(--bg-main);
          color: var(--text-main);
          font-size: 0.82rem;
          cursor: pointer;
          transition: all 0.2s;
          min-width: 180px;
          text-align: left;
        }

        .device-picker-btn:hover {
          border-color: var(--primary);
        }

        .device-picker-btn.active {
          border-color: var(--primary);
          background: rgba(99, 102, 241, 0.05);
        }

        .device-clear-btn {
          padding: 8px 12px;
          border-radius: 6px;
          font-size: 0.8rem;
          white-space: nowrap;
        }

        .device-picker-panel {
          background: var(--bg-card);
          border: 1px solid var(--border-color);
          border-radius: 8px;
          padding: 12px;
          margin-bottom: 12px;
          box-shadow: 0 4px 12px rgba(0, 0, 0, 0.15);
        }

        .device-picker-panel h4 {
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

        .device-picker-actions {
          display: flex;
          gap: 8px;
          justify-content: flex-end;
          padding-top: 8px;
          border-top: 1px solid var(--border-color);
        }

        .device-picker-actions .cancel-btn {
          padding: 6px 12px;
          border: 1px solid var(--border-color);
          border-radius: 6px;
          background: transparent;
          color: var(--text-secondary);
          font-size: 0.78rem;
          cursor: pointer;
          transition: all 0.2s;
        }

        .device-picker-actions .cancel-btn:hover {
          border-color: var(--text-secondary);
          color: var(--text-main);
        }

        .device-picker-actions .apply-btn {
          padding: 6px 14px;
          border: 1px solid var(--primary);
          border-radius: 6px;
          background: var(--primary);
          color: white;
          font-size: 0.78rem;
          cursor: pointer;
          transition: all 0.2s;
        }

        .device-picker-actions .apply-btn:hover:not(:disabled) {
          opacity: 0.9;
        }

        .device-picker-actions .apply-btn:disabled {
          opacity: 0.5;
          cursor: not-allowed;
        }

        .batch-save-btn {
          padding: 8px 16px;
          border-radius: 6px;
          font-size: 0.82rem;
          font-weight: 500;
          cursor: pointer;
          transition: all 0.2s;
          background: var(--primary);
          border: 1px solid var(--primary);
          color: white;
        }

        .batch-save-btn:hover:not(:disabled) {
          background: var(--primary-hover);
          border-color: var(--primary-hover);
        }

        .batch-save-btn:disabled {
          opacity: 0.6;
          cursor: not-allowed;
        }
      `}</style>
    </div>
  );
};
