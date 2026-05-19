/**
 * TagsContext - React Context for Tag Definition Management
 * Part of Phase 8: Device Group and Tag System
 */

import React, { createContext, useContext, useState, useCallback, useEffect } from 'react';
import { tagsApi, stationTagsApi } from './api';
import type {
  TagDefinition,
  CreateTagDTO,
  UpdateTagDTO,
  StationTags,
} from './types';
import type { Station } from '../../types';

// ============================================
// Type Definitions
// ============================================

interface TagsContextState {
  tagDefinitions: TagDefinition[];
  selectedTag: TagDefinition | null;
  isLoading: boolean;
  error: string | null;
}

interface TagsContextValue extends TagsContextState {
  // CRUD operations
  loadTagDefinitions: () => Promise<void>;
  createTag: (data: CreateTagDTO) => Promise<TagDefinition>;
  updateTag: (key: string, data: UpdateTagDTO) => Promise<TagDefinition>;
  deleteTag: (key: string) => Promise<void>;

  // Tag definition selection
  selectTag: (tag: TagDefinition | null) => void;

  // Station tags
  getStationTags: (stationId: string) => Promise<StationTags>;
  updateStationTags: (stationId: string, tags: StationTags) => Promise<StationTags>;
  batchUpdateStationTags: (stationIds: string[], tags: Partial<StationTags>) => Promise<void>;

  // Utilities
  getTagByKey: (key: string) => TagDefinition | undefined;
  getStationsByTag: (stations: Station[], tagKey: string, tagValue?: string) => Station[];
  getTagStats: () => Promise<Array<{ tagKey: string; tagLabel: string; tagValue: string; count: number }>>;
  importTagDefinitions: (tags: TagDefinition[]) => Promise<void>;
  exportTagDefinitions: () => Promise<TagDefinition[]>;
}

const TagsContext = createContext<TagsContextValue | null>(null);

// ============================================
// Provider Component
// ============================================

export function TagsProvider({ children }: { children: React.ReactNode }) {
  const [state, setState] = useState<TagsContextState>({
    tagDefinitions: [],
    selectedTag: null,
    isLoading: false,
    error: null,
  });

  // Load tag definitions on mount
  useEffect(() => {
    loadTagDefinitions();
  }, []);

  const loadTagDefinitions = useCallback(async () => {
    setState(prev => ({ ...prev, isLoading: true, error: null }));
    try {
      const tagDefinitions = await tagsApi.loadTagDefinitions();
      setState(prev => ({
        ...prev,
        tagDefinitions,
        isLoading: false,
      }));
    } catch (err) {
      setState(prev => ({
        ...prev,
        error: err instanceof Error ? err.message : 'Failed to load tag definitions',
        isLoading: false,
      }));
    }
  }, []);

  const createTag = useCallback(async (data: CreateTagDTO) => {
    const tagDefinition = await tagsApi.createTag(data);
    setState(prev => ({
      ...prev,
      tagDefinitions: [...prev.tagDefinitions, tagDefinition],
    }));
    return tagDefinition;
  }, []);

  const updateTag = useCallback(async (key: string, data: UpdateTagDTO) => {
    const tagDefinition = await tagsApi.updateTag(key, data);
    setState(prev => ({
      ...prev,
      tagDefinitions: prev.tagDefinitions.map(t => ((t.key || t.id) === key ? tagDefinition : t)),
      selectedTag: (prev.selectedTag?.key || prev.selectedTag?.id) === key ? tagDefinition : prev.selectedTag,
    }));
    return tagDefinition;
  }, []);

  const deleteTag = useCallback(async (key: string) => {
    await tagsApi.deleteTag(key);
    setState(prev => ({
      ...prev,
      tagDefinitions: prev.tagDefinitions.filter(t => (t.key || t.id) !== key),
      selectedTag: (prev.selectedTag?.key || prev.selectedTag?.id) === key ? null : prev.selectedTag,
    }));
  }, []);

  const selectTag = useCallback((tag: TagDefinition | null) => {
    setState(prev => ({ ...prev, selectedTag: tag }));
  }, []);

  const getStationTags = useCallback(async (stationId: string) => {
    return stationTagsApi.getStationTags(stationId);
  }, []);

  const updateStationTags = useCallback(async (stationId: string, tags: StationTags) => {
    return stationTagsApi.updateStationTags(stationId, tags);
  }, []);

  const batchUpdateStationTags = useCallback(async (stationIds: string[], tags: Partial<StationTags>) => {
    await stationTagsApi.batchUpdateStationTags(stationIds, tags);
  }, []);

  const getTagByKey = useCallback((key: string) => {
    return state.tagDefinitions.find(t => t.key === key);
  }, [state.tagDefinitions]);

  const getStationsByTag = useCallback((
    stations: Station[],
    tagKey: string,
    tagValue?: string
  ) => {
    return stations.filter(s => {
      const tags = s.tags || {};
      const matchesKey = tagKey in tags;
      if (!matchesKey) return false;
      if (tagValue === undefined) return true;
      return tags[tagKey] === tagValue;
    });
  }, []);

  const getTagStats = useCallback(async () => {
    return tagsApi.getTagStats();
  }, []);

  const importTagDefinitions = useCallback(async (tags: TagDefinition[]) => {
    for (const tag of tags) {
      try {
        await tagsApi.createTag({
          key: tag.key || tag.id || 'unknown',
          name: tag.name || tag.label || tag.key || 'Unknown',
          type: tag.type,
          options: tag.options,
          required: tag.required,
          defaultValue: tag.defaultValue,
          description: tag.description,
        });
      } catch (err) {
        console.warn(`[TagsContext] Failed to import tag ${tag.key}:`, err);
      }
    }
    await loadTagDefinitions();
  }, [loadTagDefinitions]);

  const exportTagDefinitions = useCallback(async () => {
    return state.tagDefinitions;
  }, [state.tagDefinitions]);

  const value: TagsContextValue = {
    ...state,
    loadTagDefinitions,
    createTag,
    updateTag,
    deleteTag,
    selectTag,
    getStationTags,
    updateStationTags,
    batchUpdateStationTags,
    getTagByKey,
    getStationsByTag,
    getTagStats,
    importTagDefinitions,
    exportTagDefinitions,
  };

  return (
    <TagsContext.Provider value={value}>
      {children}
    </TagsContext.Provider>
  );
}

// ============================================
// Hooks
// ============================================

export function useTags() {
  const context = useContext(TagsContext);
  if (!context) {
    throw new Error('useTags must be used within a TagsProvider');
  }
  return context;
}

export function useTagsSafe() {
  return useContext(TagsContext);
}