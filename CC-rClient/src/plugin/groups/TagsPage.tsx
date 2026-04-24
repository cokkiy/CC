/**
 * TagsPage - Main container for Tag Definition Management UI
 * Part of Phase 8: Device Group and Tag System
 */

import React, { useState } from 'react';
import { useTags } from './TagsContext';
import { TagDefinitionList, TagEditor } from './TagEditor';
import type { TagDefinition, CreateTagDTO, UpdateTagDTO } from './types';

export interface TagsPageProps {
  // Tags page doesn't need stations for its main functionality
}

export const TagsPage: React.FC<TagsPageProps> = () => {
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
  } = useTags();

  const [editorOpen, setEditorOpen] = useState(false);
  const [editingTag, setEditingTag] = useState<TagDefinition | null>(null);
  const [isLoading, setIsLoading] = useState(false);
  const [stats, setStats] = useState<Array<{ tagKey: string; tagLabel: string; tagValue: string; count: number }>>([]);

  // Load tag stats on mount
  React.useEffect(() => {
    loadStats();
  }, []);

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
        }

        .tags-main-grid {
          height: 100%;
        }

        .tags-main-panel {
          padding: 0;
          overflow: hidden;
          min-height: calc(100vh - 64px - 48px);
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