/**
 * Station Group and Tag System Type Definitions
 * Part of Phase 8: Device Group and Tag System
 */

// ============================================
// Location Type
// ============================================

export type Location = {
  latitude?: number;
  longitude?: number;
  altitude?: number;
  address?: string;
  region?: string;
  country?: string;
};

// ============================================
// Station Group Types
// ============================================

export interface StationGroup {
  id: string;
  name: string;
  description: string;
  color: string;
  icon?: string;
  tags: string[];
  stationIds: string[];
  createdAt: string;
  updatedAt: string;
  createdBy: string;
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
}

// ============================================
// Tag Definition Types
// ============================================

export type TagValueType = 'string' | 'number' | 'boolean' | 'select';

export interface TagDefinition {
  key: string;
  label: string;
  type: TagValueType;
  options?: string[];
  required: boolean;
  defaultValue?: string;
  description?: string;
}

export interface CreateTagDTO {
  key: string;
  label: string;
  type: TagValueType;
  options?: string[];
  required?: boolean;
  defaultValue?: string;
  description?: string;
}

export interface UpdateTagDTO {
  label?: string;
  type?: TagValueType;
  options?: string[];
  required?: boolean;
  defaultValue?: string;
  description?: string;
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