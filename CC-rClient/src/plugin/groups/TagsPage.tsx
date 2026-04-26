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
  const [selectedStationId, setSelectedStationId] = useState('');
  const [stationTags, setStationTags] = useState<Record<string, string>>({});
  const [savedStationTags, setSavedStationTags] = useState<Record<string, string>>({});
  const [isStationTagsLoading, setIsStationTagsLoading] = useState(false);
  const [isStationTagsSaving, setIsStationTagsSaving] = useState(false);
  const [stationTagNotice, setStationTagNotice] = useState<{ type: 'success' | 'error'; message: string } | null>(null);
  const [isNoticeLeaving, setIsNoticeLeaving] = useState(false);

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
        setStationTagNotice({ type: 'error', message: 'Failed to load station tags.' });
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
    if (!confirm('Are you sure you want to delete this tag definition? This will not remove tag values from stations.')) return;
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
      setStationTagNotice({ type: 'success', message: 'Station tag values saved successfully.' });
    } catch (error) {
      console.error('[TagsPage] Failed to update station tags:', error);
      setStationTagNotice({ type: 'error', message: 'Failed to save station tag values.' });
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
      setStationTagNotice({ type: 'success', message: 'Tag values applied to all stations.' });
    } catch (error) {
      console.error('[TagsPage] Failed to batch update station tags:', error);
      setStationTagNotice({ type: 'error', message: 'Failed to apply tag values to all stations.' });
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
              <h3>Station Tag Values</h3>
              <span className="station-tags-hint">Define values for a specific station using the tag definitions above.</span>
            </div>

            <div className="station-tags-body">
              <div className="station-picker-row">
                <label htmlFor="station-tag-picker">Station</label>
                <select
                  id="station-tag-picker"
                  value={selectedStationId}
                  onChange={(event) => setSelectedStationId(event.target.value)}
                >
                  <option value="">Select a station</option>
                  {stations.map((station) => (
                    <option key={station.id} value={station.id}>
                      {station.name}
                    </option>
                  ))}
                </select>
              </div>

              {stationTagNotice ? (
                <p
                  className={`station-tags-notice ${stationTagNotice.type} ${isNoticeLeaving ? 'leaving' : ''}`}
                  role={stationTagNotice.type === 'error' ? 'alert' : 'status'}
                >
                  {stationTagNotice.message}
                </p>
              ) : null}

              {!selectedStationId ? (
                <p className="station-tags-empty">Select a station to edit its tag values.</p>
              ) : isStationTagsLoading ? (
                <p className="station-tags-empty">Loading station tags...</p>
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
                    disabled={!selectedStationId || isStationTagsSaving || !hasUnsavedChanges}
                  >
                    Discard Changes
                  </button>
                  <button
                    type="button"
                    data-testid="station-tags-apply-all-btn"
                    onClick={() => void handleApplyToAllStations()}
                    disabled={!selectedStationId || stations.length === 0 || isStationTagsSaving}
                  >
                    Apply to All Stations
                  </button>
                  <button
                    type="button"
                    data-testid="station-tags-save-btn"
                    className={`primary ${hasUnsavedChanges ? 'unsaved' : ''}`}
                    onClick={() => void handleSaveStationTags()}
                    disabled={!selectedStationId || isStationTagsSaving || !hasUnsavedChanges}
                  >
                    {isStationTagsSaving ? 'Saving...' : 'Save Changes'}
                  </button>
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
    </div>
  );
};