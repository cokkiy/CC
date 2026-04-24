/**
 * GroupsPage - Main container for Station Group Management UI
 * Part of Phase 8: Device Group and Tag System
 */

import React, { useState } from 'react';
import { useGroups } from './GroupsContext';
import { GroupList } from './GroupCard';
import { GroupEditor } from './GroupEditor';
import type { StationGroup, CreateGroupDTO, UpdateGroupDTO } from './types';
import type { Station } from '../../types';

export interface GroupsPageProps {
  stations: Station[];
}

export const GroupsPage: React.FC<GroupsPageProps> = ({ stations }) => {
  const {
    groups,
    selectedGroup,
    createGroup,
    updateGroup,
    deleteGroup,
    addStationToGroup,
    removeStationFromGroup,
    selectGroup,
    importGroups,
    exportGroups,
  } = useGroups();

  const [editorOpen, setEditorOpen] = useState(false);
  const [editingGroup, setEditingGroup] = useState<StationGroup | null>(null);
  const [isLoading, setIsLoading] = useState(false);

  // Open editor for new group
  const handleCreateGroup = () => {
    setEditingGroup(null);
    setEditorOpen(true);
  };

  // Open editor for existing group
  const handleEditGroup = (group: StationGroup) => {
    setEditingGroup(group);
    setEditorOpen(true);
  };

  // Close editor
  const handleCloseEditor = () => {
    setEditorOpen(false);
    setEditingGroup(null);
  };

  // Save group (create or update)
  const handleSaveGroup = async (data: CreateGroupDTO | UpdateGroupDTO) => {
    setIsLoading(true);
    try {
      if (editingGroup) {
        await updateGroup(editingGroup.id, data as UpdateGroupDTO);
      } else {
        await createGroup(data as CreateGroupDTO);
      }
      handleCloseEditor();
    } catch (error) {
      console.error('[GroupsPage] Failed to save group:', error);
    } finally {
      setIsLoading(false);
    }
  };

  // Delete group
  const handleDeleteGroup = async (groupId: string) => {
    if (!confirm('Are you sure you want to delete this group?')) return;
    setIsLoading(true);
    try {
      await deleteGroup(groupId);
    } catch (error) {
      console.error('[GroupsPage] Failed to delete group:', error);
    } finally {
      setIsLoading(false);
    }
  };

  // Add station to group
  const handleAddStationToGroup = async (groupId: string, stationId: string) => {
    try {
      await addStationToGroup(groupId, stationId);
    } catch (error) {
      console.error('[GroupsPage] Failed to add station to group:', error);
    }
  };

  // Remove station from group
  const handleRemoveStationFromGroup = async (groupId: string, stationId: string) => {
    try {
      await removeStationFromGroup(groupId, stationId);
    } catch (error) {
      console.error('[GroupsPage] Failed to remove station from group:', error);
    }
  };

  // Import groups
  const handleImportGroups = async (groupsToImport: StationGroup[]) => {
    setIsLoading(true);
    try {
      await importGroups(groupsToImport);
    } catch (error) {
      console.error('[GroupsPage] Failed to import groups:', error);
    } finally {
      setIsLoading(false);
    }
  };

  // Export groups
  const handleExportGroups = async () => {
    try {
      const groups = await exportGroups();
      // Create and download JSON file
      const blob = new Blob([JSON.stringify(groups, null, 2)], { type: 'application/json' });
      const url = URL.createObjectURL(blob);
      const a = document.createElement('a');
      a.href = url;
      a.download = `groups-export-${new Date().toISOString().split('T')[0]}.json`;
      a.click();
      URL.revokeObjectURL(url);
    } catch (error) {
      console.error('[GroupsPage] Failed to export groups:', error);
    }
  };

  // Get station names for a group
  const getStationNames = (stationIds: string[]): string[] => {
    return stationIds
      .map(id => stations.find(s => s.id === id)?.name)
      .filter((name): name is string => !!name);
  };

  return (
    <div className="groups-page-shell">
      <main className="grid gridGroupsMode groups-main-grid">
        <section className="panel groups-main-panel">
          <GroupList
            groups={groups}
            stations={stations}
            selectedGroup={selectedGroup}
            onSelectGroup={selectGroup}
            onEditGroup={handleEditGroup}
            onDeleteGroup={handleDeleteGroup}
            onAddStation={handleAddStationToGroup}
            onRemoveStation={handleRemoveStationFromGroup}
            onCreateGroup={handleCreateGroup}
            onImport={handleImportGroups}
            onExport={handleExportGroups}
          />
        </section>
      </main>

      {editorOpen && (
        <div className="groups-layer groups-layer-drawer" role="dialog" aria-modal="true">
          <div className="groups-drawer-panel">
            <GroupEditor
              group={editingGroup}
              onSave={handleSaveGroup}
              onCancel={handleCloseEditor}
              isLoading={isLoading}
            />
          </div>
        </div>
      )}

      <style>{`
        .groups-page-shell {
          position: relative;
          min-height: calc(100vh - 64px - 48px);
        }

        .groups-main-grid {
          height: 100%;
        }

        .groups-main-panel {
          padding: 0;
          overflow: hidden;
          min-height: calc(100vh - 64px - 48px);
        }

        .groups-layer {
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

        .groups-layer-drawer {
          justify-content: flex-end;
          padding: 0;
        }

        .groups-drawer-panel {
          width: min(680px, 94vw);
          height: 100vh;
          background: var(--bg-card);
          border-left: 1px solid var(--border-color);
          box-shadow: -18px 0 48px rgba(8, 20, 38, 0.24);
        }

        @media (max-width: 1024px) {
          .groups-layer {
            padding: 12px;
          }

          .groups-layer-drawer {
            padding: 0;
          }

          .groups-drawer-panel {
            width: 100vw;
          }
        }
      `}</style>
    </div>
  );
};