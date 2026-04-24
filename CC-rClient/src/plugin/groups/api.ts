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

// ============================================
// Groups API
// ============================================

export const groupsApi = {
  /**
   * Load all station groups
   */
  async loadGroups(): Promise<StationGroup[]> {
    try {
      const result = await invoke<LoadGroupsResult>('load_groups');
      return result.groups;
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
      const result = await invoke<SaveGroupResult>('create_group', { data });
      return result.group;
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
      const result = await invoke<SaveGroupResult>('update_group', { id, data });
      return result.group;
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
      await invoke<DeleteGroupResult>('delete_group', { group_id: groupId });
    } catch (error) {
      console.error('[GroupsApi] Failed to delete group:', error);
      throw error;
    }
  },

  /**
   * Add a station to a group
   */
  async addStationToGroup(groupId: string, stationId: string): Promise<void> {
    try {
      await invoke('add_station_to_group', { group_id: groupId, station_id: stationId });
    } catch (error) {
      console.error('[GroupsApi] Failed to add station to group:', error);
      throw error;
    }
  },

  /**
   * Remove a station from a group
   */
  async removeStationFromGroup(groupId: string, stationId: string): Promise<void> {
    try {
      await invoke('remove_station_from_group', { group_id: groupId, station_id: stationId });
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
      const result = await invoke<{ station_ids: string[] }>('get_stations_in_group', {
        group_id: groupId,
      });
      return result.station_ids;
    } catch (error) {
      console.error('[GroupsApi] Failed to get stations in group:', error);
      throw error;
    }
  },

  /**
   * Import groups from a package
   */
  async importGroups(groups: StationGroup[], options?: { overwrite?: boolean }): Promise<GroupImportResult> {
    try {
      const result = await invoke<GroupImportResult>('import_groups', { groups, options });
      return result;
    } catch (error) {
      console.error('[GroupsApi] Failed to import groups:', error);
      throw error;
    }
  },

  /**
   * Export all groups
   */
  async exportGroups(): Promise<StationGroup[]> {
    try {
      const result = await invoke<{ groups: StationGroup[] }>('export_groups');
      return result.groups;
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
      const result = await invoke<{ stats: GroupStatsResult[] }>('get_group_stats');
      return result.stats;
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
      const result = await invoke<LoadTagDefinitionsResult>('load_tag_definitions');
      return result.tagDefinitions;
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
      const result = await invoke<SaveTagResult>('create_tag_definition', { data });
      return result.tagDefinition;
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
      const result = await invoke<SaveTagResult>('update_tag_definition', { key, data });
      return result.tagDefinition;
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
      await invoke<DeleteTagResult>('delete_tag_definition', { key });
    } catch (error) {
      console.error('[TagsApi] Failed to delete tag definition:', error);
      throw error;
    }
  },

  /**
   * Get tag statistics across all stations
   */
  async getTagStats(): Promise<Array<{ tagKey: string; tagLabel: string; tagValue: string; count: number }>> {
    try {
      const result = await invoke<{ stats: Array<{ tagKey: string; tagLabel: string; tagValue: string; count: number }> }>('get_tag_stats');
      return result.stats;
    } catch (error) {
      console.error('[TagsApi] Failed to get tag stats:', error);
      throw error;
    }
  },

  /**
   * Import tag definitions from a package
   */
  async importTagDefinitions(
    tagDefinitions: TagDefinition[],
    options?: { overwrite?: boolean }
  ): Promise<TagImportResult> {
    try {
      const result = await invoke<TagImportResult>('import_tag_definitions', { tagDefinitions, options });
      return result;
    } catch (error) {
      console.error('[TagsApi] Failed to import tag definitions:', error);
      throw error;
    }
  },

  /**
   * Export all tag definitions
   */
  async exportTagDefinitions(): Promise<TagDefinition[]> {
    try {
      const result = await invoke<{ tagDefinitions: TagDefinition[] }>('export_tag_definitions');
      return result.tagDefinitions;
    } catch (error) {
      console.error('[TagsApi] Failed to export tag definitions:', error);
      throw error;
    }
  },
};

// ============================================
// Station Tags API
// ============================================

export const stationTagsApi = {
  /**
   * Get tags for a specific station
   */
  async getStationTags(stationId: string): Promise<StationTags> {
    try {
      const result = await invoke<UpdateStationTagsResult>('get_station_tags', { station_id: stationId });
      return result.tags;
    } catch (error) {
      console.error('[StationTagsApi] Failed to get station tags:', error);
      throw error;
    }
  },

  /**
   * Update tags for a specific station
   */
  async updateStationTags(stationId: string, tags: StationTags): Promise<StationTags> {
    try {
      const result = await invoke<UpdateStationTagsResult>('update_station_tags', { station_id: stationId, tags });
      return result.tags;
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
      await invoke('batch_update_station_tags', { station_ids: stationIds, tags });
    } catch (error) {
      console.error('[StationTagsApi] Failed to batch update station tags:', error);
      throw error;
    }
  },
};

// Export as default
export default { groupsApi, tagsApi, stationTagsApi };