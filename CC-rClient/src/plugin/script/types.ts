/**
 * Script Management System Type Definitions
 * Core types for the CC Client Script Architecture
 * Part of Phase 6: Command Script Management System
 */

// Script types supported
export type ScriptType = 'shell' | 'powershell' | 'python' | 'lua' | 'javascript';

// Parameter types for script templates
export type ParameterType = 'string' | 'number' | 'boolean' | 'select';

// Script parameter definition
export interface ScriptParameter {
  name: string;
  paramType: ParameterType;
  defaultValue: string;
  required: boolean;
  validation?: string; // Regex pattern for validation
  description?: string;
  options?: string[]; // For select type
}

// Script execution status
export type ScriptExecutionStatus = 
  | 'pending'    // Script queued for execution
  | 'running'    // Script currently executing
  | 'success'    // Script completed successfully
  | 'failed'     // Script failed
  | 'timeout'    // Script timed out
  | 'cancelled'; // Script was cancelled

// Script execution result
export interface ScriptExecutionResult {
  scriptId: string;
  status: ScriptExecutionStatus;
  stdout: string;
  stderr: string;
  exitCode: number | null;
  startedAt: string;
  completedAt?: string;
  durationMs?: number;
  error?: string;
}

// Command script model
export interface CommandScript {
  id: string;
  name: string;
  description: string;
  scriptType: ScriptType;
  content: string;
  parameters: ScriptParameter[];
  isTemplate: boolean;
  isFavorite: boolean;
  tags: string[];
  createdBy: string;
  createdAt: string;
  updatedAt: string;
  version: number;
  usageCount: number;
  lastUsedAt?: string;
}

// Script filter options
export interface ScriptFilter {
  search?: string;
  scriptType?: ScriptType;
  tags?: string[];
  isTemplate?: boolean;
  isFavorite?: boolean;
  createdBy?: string;
}

// Script execution context
export interface ScriptExecutionContext {
  scriptId: string;
  stationIds: string[];
  parameters?: Record<string, string>;
  timeout?: number; // Timeout in milliseconds
  continueOnError?: boolean;
}

// Script validation result
export interface ScriptValidationResult {
  valid: boolean;
  errors: string[];
  warnings: string[];
  parameterErrors: Record<string, string[]>;
}

// Script marketplace package format
export interface ScriptPackage {
  metadata: ScriptPackageMetadata;
  scripts: CommandScript[];
  signature?: string;
}

// Script package metadata
export interface ScriptPackageMetadata {
  id: string;
  name: string;
  version: string;
  description?: string;
  author?: string;
  tags: string[];
  createdAt: string;
  updatedAt: string;
  downloadCount?: number;
  rating?: number;
  categories: string[];
  license?: string;
}

// Script import/export result
export interface ScriptImportResult {
  success: boolean;
  imported: number;
  skipped: number;
  errors: string[];
  warnings: string[];
}

// Script marketplace source
export interface ScriptSource {
  id: string;
  name: string;
  url: string;
  type: 'local' | 'remote' | 'official' | 'community';
  enabled: boolean;
  priority?: number;
}

// Script execution log entry
export interface ScriptLogEntry {
  timestamp: string;
  level: 'info' | 'warning' | 'error';
  message: string;
  stationId?: string;
}

// Script template category
export type ScriptCategory = 
  | 'system'       // System maintenance scripts
  | 'network'       // Network related scripts
  | 'security'      // Security/patch scripts
  | 'monitoring'    // Monitoring scripts
  | 'automation'    // Automation scripts
  | 'backup'        // Backup scripts
  | 'diagnostics'   // Diagnostic scripts
  | 'custom';       // Custom scripts
