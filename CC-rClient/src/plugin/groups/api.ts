/**
 * Groups and Tags API - Tauri Backend Communication
 * Part of Phase 8: Device Group and Tag System
 */

import { invoke } from '@tauri-apps/api/core';
import type {
  StationGroup,
  CreateGroupDTO,
  UpdateGroupDTO,
  TagDefinition,
  CreateTagDTO,
  UpdateTagDTO,
  StationTags,
  GroupImportResult,
  TagImportResult,
} from './types';

// ============================================
// API Response Interfaces
// ============================================

interface LoadGroupsResult {
  groups: StationGroup[];
}

interface SaveGroupResult {
  group: StationGroup;
}

interface DeleteGroupResult {
  success: boolean;
}

interface LoadTagDefinitionsResult {
  tagDefinitions: TagDefinition[];
}

interface SaveTagResult {
  tagDefinition: TagDefinition;
}

interface DeleteTagResult {
  success: boolean;
}

interface UpdateStationTagsResult {
  tags: StationTags;
}

interface GroupStatsResult {
  groupId: string;
  stationCount: number;
}

type RawStationGroup = Omit<StationGroup, 'station_ids'> & {
  station_ids?: string[];
  stationIds?: string[];
};

function normalizeStationGroup(group: RawStationGroup): StationGroup {
  return {
    ...group,
    station_ids: group.station_ids ?? group.stationIds ?? [],
  };
}

// ============================================
// Groups API
// ============================================

export const groupsApi = {
  /**
   * Load all station groups
   */
  async loadGroups(): Promise<StationGroup[]> {
    try {
      const groups = await invoke<RawStationGroup[]>('load_groups');
      return groups.map(normalizeStationGroup);
    } catch (error) {
      console.error('[GroupsApi] Failed to load groups:', error);
      throw error;
    }
  },

  /**
   * Create a new station group
   */
  async createGroup(data: CreateGroupDTO): Promise<StationGroup> {
    try {
      const group = await invoke<RawStationGroup>('create_group', {
        name: data.name, 
        description: data.description || '',
        tags: data.tags || [],
        station_ids: [],
      });
      return normalizeStationGroup(group);
    } catch (error) {
      console.error('[GroupsApi] Failed to create group:', error);
      throw error;
    }
  },

  /**
   * Update an existing station group
   */
  async updateGroup(id: string, data: UpdateGroupDTO): Promise<StationGroup> {
    try {
      const group = await invoke<RawStationGroup>('update_group', {
        id,
        name: data.name || '',
        description: data.description || '',
        tags: data.tags || [],
        station_ids: data.stationIds || [],
        stationIds: data.stationIds || [],
      });
      return normalizeStationGroup(group);
    } catch (error) {
      console.error('[GroupsApi] Failed to update group:', error);
      throw error;
    }
  },

  /**
   * Delete a station group
   */
  async deleteGroup(groupId: string): Promise<void> {
    try {
      await invoke<string>('delete_group', { group_id: groupId, groupId });
    } catch (error) {
      console.error('[GroupsApi] Failed to delete group:', error);
      throw error;
    }
  },

  /**
   * Add a station to a group
   */
  async addStationToGroup(groupId: string, stationId: string): Promise<StationGroup> {
    try {
      const group = await invoke<RawStationGroup>('add_station_to_group', {
        group_id: groupId,
        station_id: stationId,
        groupId,
        stationId,
      });
      return normalizeStationGroup(group);
    } catch (error) {
      console.error('[GroupsApi] Failed to add station to group:', error);
      throw error;
    }
  },

  /**
   * Remove a station from a group
   */
  async removeStationFromGroup(groupId: string, stationId: string): Promise<StationGroup> {
    try {
      const group = await invoke<RawStationGroup>('remove_station_from_group', {
        group_id: groupId,
        station_id: stationId,
        groupId,
        stationId,
      });
      return normalizeStationGroup(group);
    } catch (error) {
      console.error('[GroupsApi] Failed to remove station from group:', error);
      throw error;
    }
  },

  /**
   * Get stations in a group
   */
  async getStationsInGroup(groupId: string): Promise<string[]> {
    try {
      return await invoke<string[]>('get_stations_in_group', { group_id: groupId, groupId });
    } catch (error) {
      console.error('[GroupsApi] Failed to get stations in group:', error);
      throw error;
    }
  },

  /**
   * Export all groups
   */
  async exportGroups(): Promise<StationGroup[]> {
    try {
      const groups = await invoke<RawStationGroup[]>('export_groups');
      return groups.map(normalizeStationGroup);
    } catch (error) {
      console.error('[GroupsApi] Failed to export groups:', error);
      throw error;
    }
  },

  /**
   * Get group statistics
   */
  async getGroupStats(): Promise<GroupStatsResult[]> {
    try {
      const groups = await invoke<RawStationGroup[]>('get_group_stats');
      return groups.map(g => {
        const normalized = normalizeStationGroup(g);
        return { groupId: normalized.id, stationCount: normalized.station_ids.length };
      });
    } catch (error) {
      console.error('[GroupsApi] Failed to get group stats:', error);
      throw error;
    }
  },
};

// ============================================
// Tags API
// ============================================

export const tagsApi = {
  /**
   * Load all tag definitions
   */
  async loadTagDefinitions(): Promise<TagDefinition[]> {
    try {
      return await invoke<TagDefinition[]>('load_tag_definitions');
    } catch (error) {
      console.error('[TagsApi] Failed to load tag definitions:', error);
      throw error;
    }
  },

  /**
   * Create a new tag definition
   */
  async createTag(data: CreateTagDTO): Promise<TagDefinition> {
    try {
      return await invoke<TagDefinition>('create_tag_definition', {
        key: data.key,
        name: data.name,
        description: data.description || '',
        color: data.color || '#3B82F6',
        type: data.type || 'string',
        options: data.options || [],
        required: data.required || false,
        default_value: data.defaultValue || null,
        defaultValue: data.defaultValue || null,
      });
    } catch (error) {
      console.error('[TagsApi] Failed to create tag definition:', error);
      throw error;
    }
  },

  /**
   * Update an existing tag definition
   */
  async updateTag(key: string, data: UpdateTagDTO): Promise<TagDefinition> {
    try {
      return await invoke<TagDefinition>('update_tag_definition', {
        id: key,
        name: data.name || '',
        description: data.description || '',
        color: data.color || '#3B82F6',
        type: data.type,
        options: data.options,
        required: data.required,
        default_value: data.defaultValue ?? null,
        defaultValue: data.defaultValue ?? null,
      });
    } catch (error) {
      console.error('[TagsApi] Failed to update tag definition:', error);
      throw error;
    }
  },

  /**
   * Delete a tag definition
   */
  async deleteTag(key: string): Promise<void> {
    try {
      await invoke<string>('delete_tag_definition', { key });
    } catch (error) {
      console.error('[TagsApi] Failed to delete tag definition:', error);
      throw error;
    }
  },

  /**
   * Get tag statistics across all stations
   * Note: Returns empty array as station tags are not yet implemented
   */
  async getTagStats(): Promise<Array<{ tagKey: string; tagLabel: string; tagValue: string; count: number }>> {
    try {
      const tags = await invoke<TagDefinition[]>('load_tag_definitions');
      return tags.map(t => ({ tagKey: t.id, tagLabel: t.name, tagValue: t.name, count: 0 }));
    } catch (error) {
      console.error('[TagsApi] Failed to get tag stats:', error);
      throw error;
    }
  },

  /**
   * Export all tag definitions
   */
  async exportTagDefinitions(): Promise<TagDefinition[]> {
    try {
      return await invoke<TagDefinition[]>('export_tag_definitions');
    } catch (error) {
      console.error('[TagsApi] Failed to export tag definitions:', error);
      throw error;
    }
  },
};

// ============================================
// Station Tags API
// Note: Per-station tags not yet implemented, returns empty
// ============================================

export const stationTagsApi = {
  /**
   * Get tags for a specific station
   */
  async getStationTags(stationId: string): Promise<StationTags> {
    try {
      return await invoke<StationTags>('get_station_tags', { station_id: stationId, stationId });
    } catch (error) {
      console.error('[StationTagsApi] Failed to get station tags:', error);
      return {};
    }
  },

  /**
   * Update tags for a specific station
   */
  async updateStationTags(stationId: string, tags: StationTags): Promise<StationTags> {
    try {
      return await invoke<StationTags>('update_station_tags', { station_id: stationId, stationId, tags });
    } catch (error) {
      console.error('[StationTagsApi] Failed to update station tags:', error);
      throw error;
    }
  },

  /**
   * Batch update tags for multiple stations
   */
  async batchUpdateStationTags(
    stationIds: string[],
    tags: Partial<StationTags>
  ): Promise<void> {
    try {
      await invoke('batch_update_station_tags', { station_ids: stationIds, stationIds, tags });
    } catch (error) {
      console.error('[StationTagsApi] Failed to batch update station tags:', error);
      throw error;
    }
  },
};

// Export as default
export default { groupsApi, tagsApi, stationTagsApi };