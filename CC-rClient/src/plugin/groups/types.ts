/**
 * Station Group and Tag System Type Definitions
 * Part of Phase 8: Device Group and Tag System
 *
 * NOTE: Types are aligned with Rust backend models in CC-rClient/src-tauri/src/
 * - StationGroup: { id, name, description, tags, station_ids }
 * - TagDefinition: { id, name, description, color, created_at, updated_at }
 */

// ============================================
// Station Group Types (matches Rust StationGroup)
// ============================================

export interface StationGroup {
  id: string;
  name: string;
  description: string;
  tags: string[];        // tag definition IDs
  station_ids: string[]; // matches Rust field name
  // Optional UI fields
  color?: string;
  icon?: string;
  createdAt?: string;
  updatedAt?: string;
  createdBy?: string;
}

export interface CreateGroupDTO {
  name: string;
  description?: string;
  color?: string;
  icon?: string;
  tags?: string[];
}

export interface UpdateGroupDTO {
  name?: string;
  description?: string;
  color?: string;
  icon?: string;
  tags?: string[];
  stationIds?: string[];  // frontend uses camelCase
}

// ============================================
// Tag Definition Types (matches Rust TagDefinition)
// ============================================

export type TagValueType = 'string' | 'number' | 'boolean' | 'select';

export interface TagDefinition {
  // Backend fields (Rust)
  id: string;
  name: string;
  description: string;
  color: string;
  created_at?: string;
  updated_at?: string;
  // UI aliases
  label?: string;  // alias for name
  key?: string;      // alias for id
  // Optional UI fields
  type?: TagValueType;
  options?: string[];
  required?: boolean;
  defaultValue?: string;
}

export interface CreateTagDTO {
  key: string;          // used as Rust TagDefinition.id
  name: string;         // matches Rust create_tag_definition param
  description?: string;
  color?: string;
  type?: TagValueType;
  options?: string[];
  required?: boolean;
  defaultValue?: string;
}

export interface UpdateTagDTO {
  name?: string;
  description?: string;
  color?: string;
  type?: TagValueType;
  options?: string[];
  required?: boolean;
  defaultValue?: string;
}

// ============================================
// Station Tag Types
// ============================================

export type StationTags = Record<string, string>;

export interface StationTagUpdate {
  tags: StationTags;
}

// ============================================
// Group Filter Types
// ============================================

export interface GroupFilter {
  search?: string;
  tags?: string[];
  createdBy?: string;
}

// ============================================
// Import/Export Types
// ============================================

export interface GroupImportResult {
  success: boolean;
  imported: number;
  skipped: number;
  errors: string[];
  warnings: string[];
}

export interface TagImportResult {
  success: boolean;
  imported: number;
  skipped: number;
  errors: string[];
  warnings: string[];
}

export interface GroupStatsResult {
  groupId: string;
  stationCount: number;
}

// ============================================
// API Response Types
// ============================================

export interface GroupListResponse {
  groups: StationGroup[];
}

export interface TagDefinitionListResponse {
  tagDefinitions: TagDefinition[];
}

export interface TagStatsResponse {
  stats: Array<{
    tagKey: string;
    tagLabel: string;
    tagValue: string;
    count: number;
  }>;
}
