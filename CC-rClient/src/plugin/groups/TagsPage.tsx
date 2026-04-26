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
  const [isStationTagsLoading, setIsStationTagsLoading] = useState(false);
  const [isStationTagsSaving, setIsStationTagsSaving] = useState(false);
  const [stationTagNotice, setStationTagNotice] = useState<{ type: 'success' | 'error'; message: string } | null>(null);
  const [isNoticeLeaving, setIsNoticeLeaving] = useState(false);

  // Load tag stats on mount
  React.useEffect(() => {
    loadStats();
  }, []);

  React.useEffect(() => {
    if (!selectedStationId || stations.length === 0) {
      setStationTags({});
      return;
    }

    setIsStationTagsLoading(true);
    getStationTags(selectedStationId)
      .then((tags) => setStationTags(tags || {}))
      .catch((error) => {
        console.error('[TagsPage] Failed to load station tags:', error);
        setStationTags({});
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
      const updated = await updateStationTags(selectedStationId, stationTags);
      setStationTags(updated || {});
      setStationTagNotice({ type: 'success', message: 'Station tag values saved successfully.' });
    } catch (error) {
      console.error('[TagsPage] Failed to update station tags:', error);
      setStationTagNotice({ type: 'error', message: 'Failed to save station tag values.' });
    } finally {
      setIsStationTagsSaving(false);
    }
  };

  const handleApplyToAllStations = async () => {
    if (stations.length === 0) return;
    if (!confirm('Apply current tag values to all stations? Existing keys will be overwritten for each station.')) {
      return;
    }

    setIsStationTagsSaving(true);
    setStationTagNotice(null);
    try {
      let tagsToApply = stationTags;

      if (selectedStationId && Object.keys(tagsToApply).length === 0) {
        const refreshed = await getStationTags(selectedStationId);
        if (refreshed && Object.keys(refreshed).length > 0) {
          tagsToApply = refreshed;
          setStationTags(refreshed);
        }
      }

      await batchUpdateStationTags(
        stations.map((station) => station.id),
        tagsToApply,
      );
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

            <div className="station-tags-actions">
              <button
                type="button"
                data-testid="station-tags-clear-btn"
                onClick={() => setStationTags({})}
                disabled={!selectedStationId || isStationTagsSaving}
              >
                Clear Local Values
              </button>
              <button
                type="button"
                data-testid="station-tags-apply-all-btn"
                onClick={() => void handleApplyToAllStations()}
                disabled={!selectedStationId || stations.length === 0 || isStationTagsSaving}
              >
                Apply To All Stations
              </button>
              <button
                type="button"
                data-testid="station-tags-save-btn"
                className="primary"
                onClick={() => void handleSaveStationTags()}
                disabled={!selectedStationId || isStationTagsSaving}
              >
                {isStationTagsSaving ? 'Saving...' : 'Save Station Tags'}
              </button>
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
        .tags-page-shell {
          position: relative;
          min-height: calc(100vh - 64px - 48px);
          height: calc(100vh - 64px - 48px);
        }

        .tags-main-grid {
          height: 100%;
          min-height: 0;
        }

        .tags-main-panel {
          padding: 0;
          overflow: auto;
          min-height: 0;
          height: 100%;
          display: grid;
          grid-template-rows: 1fr auto;
        }

        .station-tags-editor {
          border-top: 1px solid var(--border-color);
          padding: 14px 16px;
          background: color-mix(in srgb, var(--bg-card) 92%, #000 8%);
          display: grid;
          gap: 12px;
          max-height: min(45vh, 420px);
          overflow-y: auto;
          overflow-x: hidden;
        }

        .station-tags-header h3 {
          margin: 0;
          font-size: 0.95rem;
        }

        .station-tags-hint {
          font-size: 0.78rem;
          color: var(--text-secondary);
        }

        .station-picker-row {
          display: grid;
          gap: 6px;
          max-width: 380px;
        }

        .station-picker-row label {
          font-size: 0.8rem;
          color: var(--text-secondary);
        }

        .station-picker-row select,
        .station-tag-field input,
        .station-tag-field select {
          border: 1px solid var(--border-color);
          border-radius: 8px;
          background: var(--bg-main);
          color: var(--text-main);
          padding: 8px 10px;
          font-size: 0.85rem;
        }

        .station-tags-empty {
          margin: 0;
          color: var(--text-secondary);
          font-size: 0.84rem;
        }

        .station-tags-notice {
          margin: 0;
          font-size: 0.84rem;
          border-radius: 8px;
          padding: 8px 10px;
          border: 1px solid var(--border-color);
          opacity: 1;
          transform: translateY(0);
          transition: opacity 220ms ease, transform 220ms ease;
        }

        .station-tags-notice.leaving {
          opacity: 0;
          transform: translateY(-3px);
        }

        .station-tags-notice.success {
          color: #0f5132;
          background: #d1e7dd;
          border-color: #badbcc;
        }

        .station-tags-notice.error {
          color: #842029;
          background: #f8d7da;
          border-color: #f5c2c7;
        }

        .station-tags-form {
          display: grid;
          grid-template-columns: repeat(auto-fit, minmax(220px, 1fr));
          gap: 10px;
        }

        .station-tag-field {
          display: grid;
          gap: 6px;
        }

        .station-tag-field label {
          font-size: 0.78rem;
          color: var(--text-secondary);
        }

        .station-tags-actions {
          display: flex;
          gap: 10px;
          justify-content: flex-end;
        }

        .station-tags-actions button {
          border: 1px solid var(--border-color);
          background: transparent;
          color: var(--text-main);
          border-radius: 8px;
          padding: 8px 12px;
          font-size: 0.82rem;
          cursor: pointer;
        }

        .station-tags-actions button.primary {
          border-color: var(--primary);
          background: var(--primary);
          color: white;
        }

        .station-tags-actions button:disabled {
          opacity: 0.5;
          cursor: not-allowed;
        }

        .tags-layer {
          position: fixed;
          inset: 0;
          background: rgba(11, 25, 44, 0.42);
          backdrop-filter: blur(2px);
          display: flex;
          align-items: center;
          justify-content: center;
          z-index: 1200;
          padding: 24px;
        }

        .tags-layer-modal {
          padding: 0;
        }

        .tags-modal-panel {
          width: min(560px, 94vw);
          height: auto;
          max-height: 90vh;
          background: var(--bg-card);
          border: 1px solid var(--border-color);
          border-radius: 14px;
          box-shadow: 0 22px 56px rgba(8, 20, 38, 0.24);
          overflow: hidden;
        }

        @media (max-width: 768px) {
          .tags-modal-panel {
            width: 100%;
            height: 100%;
            max-height: none;
            border-radius: 0;
          }
        }
      `}</style>
    </div>
  );
};