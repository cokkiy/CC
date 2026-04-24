/**
 * GroupsContext - React Context for Station Group Management
 * Part of Phase 8: Device Group and Tag System
 */

import React, { createContext, useContext, useState, useCallback, useEffect } from 'react';
import { groupsApi } from './api';
import type {
  StationGroup,
  CreateGroupDTO,
  UpdateGroupDTO,
  GroupFilter,
  GroupImportResult,
} from './types';

// ============================================
// Type Definitions
// ============================================

interface GroupsContextState {
  groups: StationGroup[];
  selectedGroup: StationGroup | null;
  isLoading: boolean;
  error: string | null;
}

interface GroupsContextValue extends GroupsContextState {
  // CRUD operations
  loadGroups: () => Promise<void>;
  createGroup: (data: CreateGroupDTO) => Promise<StationGroup>;
  updateGroup: (id: string, data: UpdateGroupDTO) => Promise<StationGroup>;
  deleteGroup: (id: string) => Promise<void>;

  // Station membership
  addStationToGroup: (groupId: string, stationId: string) => Promise<void>;
  removeStationFromGroup: (groupId: string, stationId: string) => Promise<void>;

  // Selection
  selectGroup: (group: StationGroup | null) => void;
  filterGroups: (filter: GroupFilter) => StationGroup[];

  // Import/Export
  importGroups: (groups: StationGroup[], options?: { overwrite?: boolean }) => Promise<GroupImportResult>;
  exportGroups: () => Promise<StationGroup[]>;

  // Utilities
  getGroupById: (id: string) => StationGroup | undefined;
  getGroupsByStation: (stationId: string) => StationGroup[];
  getGroupStats: () => Promise<Array<{ groupId: string; stationCount: number }>>;
}

const GroupsContext = createContext<GroupsContextValue | null>(null);

// ============================================
// Provider Component
// ============================================

export function GroupsProvider({ children }: { children: React.ReactNode }) {
  const [state, setState] = useState<GroupsContextState>({
    groups: [],
    selectedGroup: null,
    isLoading: false,
    error: null,
  });

  // Load groups on mount
  useEffect(() => {
    loadGroups();
  }, []);

  const loadGroups = useCallback(async () => {
    setState(prev => ({ ...prev, isLoading: true, error: null }));
    try {
      const groups = await groupsApi.loadGroups();
      setState(prev => ({
        ...prev,
        groups,
        isLoading: false,
      }));
    } catch (err) {
      setState(prev => ({
        ...prev,
        error: err instanceof Error ? err.message : 'Failed to load groups',
        isLoading: false,
      }));
    }
  }, []);

  const createGroup = useCallback(async (data: CreateGroupDTO) => {
    const group = await groupsApi.createGroup(data);
    setState(prev => ({
      ...prev,
      groups: [...prev.groups, group],
    }));
    return group;
  }, []);

  const updateGroup = useCallback(async (id: string, data: UpdateGroupDTO) => {
    const group = await groupsApi.updateGroup(id, data);
    setState(prev => ({
      ...prev,
      groups: prev.groups.map(g => (g.id === id ? group : g)),
      selectedGroup: prev.selectedGroup?.id === id ? group : prev.selectedGroup,
    }));
    return group;
  }, []);

  const deleteGroup = useCallback(async (id: string) => {
    await groupsApi.deleteGroup(id);
    setState(prev => ({
      ...prev,
      groups: prev.groups.filter(g => g.id !== id),
      selectedGroup: prev.selectedGroup?.id === id ? null : prev.selectedGroup,
    }));
  }, []);

  const addStationToGroup = useCallback(async (groupId: string, stationId: string) => {
    await groupsApi.addStationToGroup(groupId, stationId);
    setState(prev => ({
      ...prev,
      groups: prev.groups.map(g =>
        g.id === groupId
          ? { ...g, stationIds: [...g.stationIds, stationId] }
          : g
      ),
    }));
  }, []);

  const removeStationFromGroup = useCallback(async (groupId: string, stationId: string) => {
    await groupsApi.removeStationFromGroup(groupId, stationId);
    setState(prev => ({
      ...prev,
      groups: prev.groups.map(g =>
        g.id === groupId
          ? { ...g, stationIds: g.stationIds.filter(id => id !== stationId) }
          : g
      ),
    }));
  }, []);

  const selectGroup = useCallback((group: StationGroup | null) => {
    setState(prev => ({ ...prev, selectedGroup: group }));
  }, []);

  const filterGroups = useCallback((filter: GroupFilter) => {
    let result = state.groups;

    if (filter.search) {
      const search = filter.search.toLowerCase();
      result = result.filter(
        g =>
          g.name.toLowerCase().includes(search) ||
          g.description.toLowerCase().includes(search)
      );
    }

    if (filter.tags && filter.tags.length > 0) {
      result = result.filter(g =>
        filter.tags!.some(tag => g.tags.includes(tag))
      );
    }

    if (filter.createdBy) {
      result = result.filter(g => g.createdBy === filter.createdBy);
    }

    return result;
  }, [state.groups]);

  const importGroups = useCallback(async (
    groups: StationGroup[],
    options?: { overwrite?: boolean }
  ) => {
    const result = await groupsApi.importGroups(groups, options);
    // Reload groups to get updated state
    await loadGroups();
    return result;
  }, [loadGroups]);

  const exportGroups = useCallback(async () => {
    return groupsApi.exportGroups();
  }, []);

  const getGroupById = useCallback((id: string) => {
    return state.groups.find(g => g.id === id);
  }, [state.groups]);

  const getGroupsByStation = useCallback((stationId: string) => {
    return state.groups.filter(g => g.stationIds.includes(stationId));
  }, [state.groups]);

  const getGroupStats = useCallback(async () => {
    return groupsApi.getGroupStats();
  }, []);

  const value: GroupsContextValue = {
    ...state,
    loadGroups,
    createGroup,
    updateGroup,
    deleteGroup,
    addStationToGroup,
    removeStationFromGroup,
    selectGroup,
    filterGroups,
    importGroups,
    exportGroups,
    getGroupById,
    getGroupsByStation,
    getGroupStats,
  };

  return (
    <GroupsContext.Provider value={value}>
      {children}
    </GroupsContext.Provider>
  );
}

// ============================================
// Hooks
// ============================================

export function useGroups() {
  const context = useContext(GroupsContext);
  if (!context) {
    throw new Error('useGroups must be used within a GroupsProvider');
  }
  return context;
}

export function useGroupsSafe() {
  return useContext(GroupsContext);
}