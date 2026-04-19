/**
 * UI Template Marketplace
 * Template format definition and installation/switching mechanism
 * Part of Phase 3.3: UI Template Marketplace
 */

import React, { createContext, useContext, useState, useCallback, useEffect, useMemo } from 'react';
import type { LayoutConfig } from './LayoutConfig';
import { DEFAULT_LAYOUT, validateLayout } from './LayoutConfig';
import type { LayoutValidationResult } from './LayoutConfig';

// ============================================
// Template Package Format Definition
// ============================================

/**
 * UI Template Package - Complete template structure for marketplace distribution
 */
export interface UITemplatePackage {
  /** Template metadata */
  metadata: TemplateMetadata;
  /** Template content - the actual layout configuration */
  content: TemplateContent;
  /** Optional preview images (base64 or URLs) */
  preview?: TemplatePreview[];
  /** Bundle info for packaged templates */
  bundle?: TemplateBundle;
  /** Template dependencies */
  dependencies?: TemplateDependency[];
  /** Digital signature for verification */
  signature?: string;
}

/**
 * Template metadata
 */
export interface TemplateMetadata {
  /** Unique template identifier (e.g., "cc-dark-monitoring-v1") */
  id: string;
  /** Human-readable template name */
  name: string;
  /** Template version (semver) */
  version: string;
  /** Template description */
  description?: string;
  /** Template author */
  author?: string;
  /** Author contact (email or URL) */
  authorUrl?: string;
  /** Template category */
  category: TemplateCategory;
  /** Tags for search/filter */
  tags?: string[];
  /** Minimum CC Client version required */
  minClientVersion?: string;
  /** When this template was created */
  createdAt?: string;
  /** When this template was last updated */
  updatedAt?: string;
  /** Template license */
  license?: string;
  /** Download/install count */
  downloads?: number;
  /** User rating (1-5) */
  rating?: number;
}

/**
 * Template category
 */
export type TemplateCategory = 
  | 'monitoring'      // System monitoring dashboards
  | 'control'         // Control/command panels
  | 'analytics'       // Data analytics views
  | 'overview'        // Overview/summary views
  | 'custom'          // Custom/mixed layouts
  | 'minimal'         // Minimal/lightweight layouts
  | 'dark'            // Dark theme optimized
  | 'light'           // Light theme optimized;

/**
 * Template content - the actual layout and theme data
 */
export interface TemplateContent {
  /** The layout configuration */
  layout: LayoutConfig;
  /** Custom theme overrides */
  theme?: TemplateTheme;
  /** Custom CSS to apply */
  customCss?: string;
  /** Panel configurations */
  panels?: TemplatePanelConfig[];
}

/**
 * Template-specific theme overrides
 */
export interface TemplateTheme {
  /** Primary brand color */
  primaryColor?: string;
  /** Secondary color */
  secondaryColor?: string;
  /** Background color */
  backgroundColor?: string;
  /** Card/panel background */
  cardBackground?: string;
  /** Text colors */
  textColor?: string;
  /** Border color */
  borderColor?: string;
  /** Accent color */
  accentColor?: string;
  /** Error color */
  errorColor?: string;
  /** Warning color */
  warningColor?: string;
  /** Success color */
  successColor?: string;
  /** Chart colors palette */
  chartColors?: string[];
  /** Dark mode overrides */
  dark?: TemplateTheme;
  /** Light mode overrides */
  light?: TemplateTheme;
}

/**
 * Extended panel configuration for templates
 */
export interface TemplatePanelConfig {
  /** Panel ID in the layout */
  panelId: string;
  /** Override title */
  title?: string;
  /** Override icon */
  icon?: string;
  /** Whether panel is pinned */
  pinned?: boolean;
  /** Default collapsed state */
  defaultCollapsed?: boolean;
  /** Refresh interval override */
  refreshInterval?: number;
}

/**
 * Template preview (screenshot/thumbnail)
 */
export interface TemplatePreview {
  /** Preview type */
  type: 'thumbnail' | 'full' | 'dark' | 'light';
  /** Image URL or base64 data */
  url: string;
  /** Alt text */
  alt?: string;
  /** Display order */
  order?: number;
}

/**
 * Template bundle info (for downloadable packages)
 */
export interface TemplateBundle {
  /** Bundle format (zip, tar.gz, etc.) */
  format: 'zip' | 'tar.gz' | 'json';
  /** Download URL */
  downloadUrl?: string;
  /** Direct CDN URL */
  cdnUrl?: string;
  /** Bundle size in bytes */
  size?: number;
  /** SHA256 checksum */
  checksum?: string;
  /** Whether bundled assets need extraction */
  needsExtraction?: boolean;
}

/**
 * Template dependency (required plugins/features)
 */
export interface TemplateDependency {
  /** Plugin or feature ID */
  id: string;
  /** Minimum version required */
  minVersion?: string;
  /** Whether it's required or optional */
  required: boolean;
  /** Dependency description */
  description?: string;
}

// ============================================
// Template Registry & Storage
// ============================================

/**
 * Installed template registry entry
 */
export interface InstalledTemplate {
  /** Template ID */
  id: string;
  /** Source package */
  package: UITemplatePackage;
  /** When it was installed */
  installedAt: string;
  /** Who installed it */
  installedBy?: string;
  /** Installation source */
  source?: string;
  /** Whether it's currently active */
  active: boolean;
  /** Local overrides (user customizations) */
  overrides?: Partial<TemplateContent>;
}

/**
 * Template marketplace source
 */
export interface TemplateSource {
  /** Source ID */
  id: string;
  /** Source name */
  name: string;
  /** Source URL/API endpoint */
  url: string;
  /** Source type */
  type: 'local' | 'remote' | 'official' | 'community' | 'builtin';
  /** Whether source is enabled */
  enabled: boolean;
  /** Priority (lower = higher priority) */
  priority?: number;
}

/**
 * Template filter options
 */
export interface TemplateFilter {
  /** Category filter */
  category?: TemplateCategory;
  /** Search query */
  search?: string;
  /** Tags filter */
  tags?: string[];
  /** Minimum rating */
  minRating?: number;
  /** Source filter */
  sourceId?: string;
  /** Show only installed */
  installedOnly?: boolean;
  /** Show only active */
  activeOnly?: boolean;
}

// ============================================
// Template Marketplace Context
// ============================================

/**
 * Template marketplace state
 */
export interface TemplateMarketplaceState {
  /** All installed templates */
  installedTemplates: Map<string, InstalledTemplate>;
  /** Template sources */
  sources: TemplateSource[];
  /** Currently active template ID */
  activeTemplateId: string | null;
  /** Loading state */
  loading: boolean;
  /** Error message */
  error: string | null;
  /** Last sync time */
  lastSync: Date | null;
}

/**
 * Template marketplace context value
 */
export interface TemplateMarketplaceContextValue extends TemplateMarketplaceState {
  // Template operations
  installTemplate: (template: UITemplatePackage, source?: string) => Promise<TemplateInstallResult>;
  uninstallTemplate: (templateId: string) => Promise<boolean>;
  activateTemplate: (templateId: string) => Promise<boolean>;
  deactivateTemplate: (templateId: string) => Promise<boolean>;
  updateTemplate: (templateId: string, update: Partial<UITemplatePackage>) => Promise<boolean>;
  
  // Template queries
  getTemplate: (templateId: string) => InstalledTemplate | null;
  getActiveTemplate: () => InstalledTemplate | null;
  getTemplatesByCategory: (category: TemplateCategory) => InstalledTemplate[];
  searchTemplates: (filter: TemplateFilter) => InstalledTemplate[];
  
  // Source management
  addSource: (source: TemplateSource) => void;
  removeSource: (sourceId: string) => void;
  syncFromSource: (sourceId: string) => Promise<SyncResult>;
  
  // Utility
  exportTemplate: (templateId: string) => UITemplatePackage | null;
  importTemplate: (pkg: UITemplatePackage) => Promise<TemplateInstallResult>;
  validateTemplate: (template: UITemplatePackage) => TemplateValidationResult;
}

/**
 * Template install result
 */
export interface TemplateInstallResult {
  success: boolean;
  templateId?: string;
  error?: string;
  warnings?: string[];
}

/**
 * Sync result from a source
 */
export interface SyncResult {
  success: boolean;
  sourceId: string;
  templatesAdded: number;
  templatesUpdated: number;
  templatesRemoved: number;
  error?: string;
}

/**
 * Template validation result
 */
export interface TemplateValidationResult {
  valid: boolean;
  errors: string[];
  warnings: string[];
  layoutValidation?: LayoutValidationResult;
}

// ============================================
// Template Marketplace Provider
// ============================================

const STORAGE_KEY = 'cc-template-marketplace';
const ACTIVE_TEMPLATE_KEY = 'cc-active-template';

function loadFromStorage(): { installed: InstalledTemplate[]; activeId: string | null } {
  try {
    const stored = localStorage.getItem(STORAGE_KEY);
    const activeId = localStorage.getItem(ACTIVE_TEMPLATE_KEY);
    if (stored) {
      const parsed = JSON.parse(stored);
      return {
        installed: parsed.installed || [],
        activeId: activeId || null,
      };
    }
  } catch (e) {
    console.warn('[TemplateMarketplace] Failed to load from storage:', e);
  }
  return { installed: [], activeId: null };
}

function saveToStorage(installed: InstalledTemplate[], activeId: string | null): void {
  try {
    localStorage.setItem(STORAGE_KEY, JSON.stringify({ installed }));
    if (activeId) {
      localStorage.setItem(ACTIVE_TEMPLATE_KEY, activeId);
    } else {
      localStorage.removeItem(ACTIVE_TEMPLATE_KEY);
    }
  } catch (e) {
    console.warn('[TemplateMarketplace] Failed to save to storage:', e);
  }
}

/**
 * Default template sources
 */
const DEFAULT_SOURCES: TemplateSource[] = [
  {
    id: 'builtin',
    name: 'Built-in Templates',
    url: '',
    type: 'local',
    enabled: true,
    priority: 0,
  },
  {
    id: 'community',
    name: 'Community Templates',
    url: 'https://api.cc-templates.example/community',
    type: 'community',
    enabled: false,
    priority: 10,
  },
];

/**
 * Built-in templates
 */
const BUILTIN_TEMPLATES: UITemplatePackage[] = [
  {
    metadata: {
      id: 'cc-default',
      name: 'Default Layout',
      version: '1.0.0',
      description: 'Standard CC Client layout with CPU, Memory, and Network panels',
      author: 'CC Team',
      category: 'monitoring',
      tags: ['default', 'cpu', 'memory', 'network'],
      minClientVersion: '1.0.0',
      createdAt: '2024-01-01T00:00:00Z',
      updatedAt: '2024-01-01T00:00:00Z',
      license: 'MIT',
      downloads: 0,
      rating: 5,
    },
    content: {
      layout: DEFAULT_LAYOUT,
    },
  },
  {
    metadata: {
      id: 'cc-minimal',
      name: 'Minimal Layout',
      version: '1.0.0',
      description: 'Minimal layout with only essential panels for low-resource environments',
      author: 'CC Team',
      category: 'minimal',
      tags: ['minimal', 'lightweight'],
      minClientVersion: '1.0.0',
      createdAt: '2024-01-01T00:00:00Z',
      updatedAt: '2024-01-01T00:00:00Z',
      license: 'MIT',
      downloads: 0,
      rating: 4,
    },
    content: {
      layout: {
        version: '1.0.0',
        name: 'Minimal Layout',
        description: 'Minimal layout with only essential panels',
        panels: [
          {
            id: 'station-list-minimal',
            type: 'table',
            position: 'center',
            title: 'Stations',
            priority: 100,
            dataSource: {
              type: 'station_list',
            },
          },
        ],
        alerts: {
          enabled: true,
          maxVisible: 3,
        },
      },
    },
  },
  {
    metadata: {
      id: 'cc-dashboard',
      name: 'Dashboard Layout',
      version: '1.0.0',
      description: 'Monitoring-focused dashboard with multiple charts',
      author: 'CC Team',
      category: 'monitoring',
      tags: ['dashboard', 'charts', 'monitoring'],
      minClientVersion: '1.0.0',
      createdAt: '2024-01-01T00:00:00Z',
      updatedAt: '2024-01-01T00:00:00Z',
      license: 'MIT',
      downloads: 0,
      rating: 5,
    },
    content: {
      layout: {
        version: '1.0.0',
        name: 'Dashboard Layout',
        description: 'Monitoring-focused dashboard with multiple charts',
        panels: [
          {
            id: 'cpu-chart',
            type: 'chart',
            position: 'top',
            title: 'CPU Usage',
            priority: 100,
            dataSource: {
              type: 'telemetry',
              refreshInterval: 1000,
            },
            props: {
              chartType: 'area',
              dataKey: 'cpu',
              color: '#d64545',
            },
          },
          {
            id: 'memory-chart',
            type: 'chart',
            position: 'top',
            title: 'Memory Usage',
            priority: 101,
            dataSource: {
              type: 'telemetry',
              refreshInterval: 1000,
            },
            props: {
              chartType: 'area',
              dataKey: 'memory',
              color: '#c78a00',
            },
          },
          {
            id: 'station-grid',
            type: 'card',
            position: 'center',
            title: 'Station Status',
            priority: 200,
            dataSource: {
              type: 'station_status',
            },
            props: {
              showCpu: true,
              showMemory: true,
              compact: true,
            },
          },
        ],
        alerts: {
          enabled: true,
          position: 'overlay',
          maxVisible: 10,
          autoHideAfter: 15000,
        },
      },
    },
  },
  {
    metadata: {
      id: 'cc-dark-monitoring',
      name: 'Dark Monitoring Theme',
      version: '1.0.0',
      description: 'Dark theme optimized for long monitoring sessions with high contrast charts',
      author: 'CC Team',
      category: 'dark',
      tags: ['dark', 'monitoring', 'night-mode'],
      minClientVersion: '1.0.0',
      createdAt: '2024-01-15T00:00:00Z',
      updatedAt: '2024-01-15T00:00:00Z',
      license: 'MIT',
      downloads: 0,
      rating: 5,
    },
    content: {
      layout: DEFAULT_LAYOUT,
      theme: {
        primaryColor: '#6366f1',
        secondaryColor: '#818cf8',
        backgroundColor: '#0f172a',
        cardBackground: '#1e293b',
        textColor: '#e2e8f0',
        borderColor: '#334155',
        accentColor: '#3b82f6',
        errorColor: '#ef4444',
        warningColor: '#f59e0b',
        successColor: '#22c55e',
        chartColors: ['#6366f1', '#22c55e', '#f59e0b', '#ef4444', '#ec4899'],
      },
    },
  },
  {
    metadata: {
      id: 'cc-light-overview',
      name: 'Light Overview',
      version: '1.0.0',
      description: 'Clean light theme with overview of all key metrics',
      author: 'CC Team',
      category: 'light',
      tags: ['light', 'overview', 'clean'],
      minClientVersion: '1.0.0',
      createdAt: '2024-01-20T00:00:00Z',
      updatedAt: '2024-01-20T00:00:00Z',
      license: 'MIT',
      downloads: 0,
      rating: 4,
    },
    content: {
      layout: DEFAULT_LAYOUT,
      theme: {
        primaryColor: '#3b82f6',
        secondaryColor: '#60a5fa',
        backgroundColor: '#f8fafc',
        cardBackground: '#ffffff',
        textColor: '#1e293b',
        borderColor: '#e2e8f0',
        accentColor: '#6366f1',
        errorColor: '#ef4444',
        warningColor: '#f59e0b',
        successColor: '#22c55e',
        chartColors: ['#3b82f6', '#22c55e', '#f59e0b', '#ef4444', '#8b5cf6'],
      },
    },
  },
];

/**
 * Create built-in template entries
 */
function createBuiltinEntries(): Map<string, InstalledTemplate> {
  const entries = new Map<string, InstalledTemplate>();
  const now = new Date().toISOString();
  
  for (const pkg of BUILTIN_TEMPLATES) {
    entries.set(pkg.metadata.id, {
      id: pkg.metadata.id,
      package: pkg,
      installedAt: pkg.metadata.createdAt || now,
      source: 'builtin',
      active: pkg.metadata.id === 'cc-default',
    });
  }
  
  return entries;
}

// ============================================
// Context Creation
// ============================================

const TemplateMarketplaceContext = createContext<TemplateMarketplaceContextValue | null>(null);

/**
 * Template Marketplace Provider Component
 */
export const TemplateMarketplaceProvider: React.FC<{ children: React.ReactNode }> = ({ children }) => {
  // Load saved state
  const savedState = useMemo(() => loadFromStorage(), []);
  
  // Initialize installed templates
  const [installedTemplates, setInstalledTemplates] = useState<Map<string, InstalledTemplate>>(() => {
    const builtins = createBuiltinEntries();
    
    // Merge with saved templates (saved templates override builtins if newer)
    if (savedState.installed.length > 0) {
      for (const saved of savedState.installed) {
        if (!saved.source || saved.source === 'builtin') continue; // Skip builtin duplicates
        builtins.set(saved.id, saved);
      }
    }
    
    return builtins;
  });
  
  // Active template ID
  const [activeTemplateId, setActiveTemplateId] = useState<string | null>(() => {
    // First check saved state
    if (savedState.activeId && installedTemplates.has(savedState.activeId)) {
      return savedState.activeId;
    }
    // Default to 'cc-default'
    return 'cc-default';
  });
  
  // Template sources
  const [sources] = useState<TemplateSource[]>(DEFAULT_SOURCES);
  
  // Loading and error states
  const [loading, setLoading] = useState(false);
  const [error, setError] = useState<string | null>(null);
  
  // Last sync time
  const [lastSync, setLastSync] = useState<Date | null>(null);
  
  // Persist state changes
  useEffect(() => {
    saveToStorage(Array.from(installedTemplates.values()), activeTemplateId);
  }, [installedTemplates, activeTemplateId]);
  
  // ============================================
  // Template Operations
  // ============================================
  
  /**
   * Validate a template package
   */
  const validateTemplate = useCallback((template: UITemplatePackage): TemplateValidationResult => {
    const errors: string[] = [];
    const warnings: string[] = [];
    
    // Check required metadata
    if (!template.metadata?.id) {
      errors.push('Template ID is required');
    }
    if (!template.metadata?.name) {
      errors.push('Template name is required');
    }
    if (!template.metadata?.version) {
      warnings.push('Template version is not specified');
    }
    if (!template.metadata?.category) {
      errors.push('Template category is required');
    }
    
    // Check content
    if (!template.content?.layout) {
      errors.push('Template layout is required');
    } else {
      // Validate layout
      const layoutValidation = validateLayout(template.content.layout);
      if (!layoutValidation.valid) {
        errors.push(...layoutValidation.errors.map(e => `Layout: ${e.message}`));
      }
      warnings.push(...layoutValidation.warnings.map(w => `Layout: ${w.message}`));
    }
    
    // Check version format (semver)
    if (template.metadata?.version && !/^\d+\.\d+\.\d+/.test(template.metadata.version)) {
      warnings.push('Version should be in semver format (e.g., 1.0.0)');
    }
    
    return {
      valid: errors.length === 0,
      errors,
      warnings,
      layoutValidation: template.content?.layout ? validateLayout(template.content.layout) : undefined,
    };
  }, []);
  
  /**
   * Install a template
   */
  const installTemplate = useCallback(async (
    template: UITemplatePackage,
    source?: string
  ): Promise<TemplateInstallResult> => {
    setLoading(true);
    setError(null);
    
    try {
      // Validate template
      const validation = validateTemplate(template);
      if (!validation.valid) {
        return {
          success: false,
          error: `Validation failed: ${validation.errors.join(', ')}`,
          warnings: validation.warnings,
        };
      }
      
      // Check if already installed
      const existing = installedTemplates.get(template.metadata.id);
      if (existing) {
        // Check versions
        if (existing.package.metadata.version === template.metadata.version) {
          return {
            success: false,
            error: 'Template is already installed with the same version',
            templateId: template.metadata.id,
          };
        }
        
        // Update existing
        const updated: InstalledTemplate = {
          ...existing,
          package: template,
          installedAt: new Date().toISOString(),
          source: source || existing.source,
        };
        
        setInstalledTemplates(prev => {
          const next = new Map(prev);
          next.set(template.metadata.id, updated);
          return next;
        });
        
        return {
          success: true,
          templateId: template.metadata.id,
          warnings: validation.warnings,
        };
      }
      
      // Create new entry
      const entry: InstalledTemplate = {
        id: template.metadata.id,
        package: template,
        installedAt: new Date().toISOString(),
        source: source || 'local',
        active: false,
      };
      
      setInstalledTemplates(prev => {
        const next = new Map(prev);
        next.set(template.metadata.id, entry);
        return next;
      });
      
      return {
        success: true,
        templateId: template.metadata.id,
        warnings: validation.warnings,
      };
    } catch (e) {
      const errorMessage = e instanceof Error ? e.message : 'Unknown error';
      setError(errorMessage);
      return {
        success: false,
        error: errorMessage,
      };
    } finally {
      setLoading(false);
    }
  }, [installedTemplates, validateTemplate]);
  
  /**
   * Uninstall a template
   */
  const uninstallTemplate = useCallback(async (templateId: string): Promise<boolean> => {
    const template = installedTemplates.get(templateId);
    if (!template) return false;
    
    // Cannot uninstall built-in templates
    if (template.source === 'builtin' || template.id.startsWith('cc-')) {
      setError('Cannot uninstall built-in templates');
      return false;
    }
    
    // If active, switch to default first
    if (activeTemplateId === templateId) {
      setActiveTemplateId('cc-default');
    }
    
    setInstalledTemplates(prev => {
      const next = new Map(prev);
      next.delete(templateId);
      return next;
    });
    
    return true;
  }, [installedTemplates, activeTemplateId]);
  
  /**
   * Activate a template
   */
  const activateTemplate = useCallback(async (templateId: string): Promise<boolean> => {
    const template = installedTemplates.get(templateId);
    if (!template) {
      setError('Template not found');
      return false;
    }
    
    // Deactivate current
    if (activeTemplateId && activeTemplateId !== templateId) {
      setInstalledTemplates(prev => {
        const next = new Map(prev);
        const current = next.get(activeTemplateId);
        if (current) {
          next.set(activeTemplateId, { ...current, active: false });
        }
        // Activate new
        next.set(templateId, { ...template, active: true });
        return next;
      });
    } else {
      setInstalledTemplates(prev => {
        const next = new Map(prev);
        next.set(templateId, { ...template, active: true });
        return next;
      });
    }
    
    setActiveTemplateId(templateId);
    return true;
  }, [installedTemplates, activeTemplateId]);
  
  /**
   * Deactivate a template (switch to default)
   */
  const deactivateTemplate = useCallback(async (templateId: string): Promise<boolean> => {
    if (activeTemplateId === templateId) {
      setActiveTemplateId('cc-default');
    }
    
    const template = installedTemplates.get(templateId);
    if (template) {
      setInstalledTemplates(prev => {
        const next = new Map(prev);
        next.set(templateId, { ...template, active: false });
        return next;
      });
    }
    
    return true;
  }, [installedTemplates, activeTemplateId]);
  
  /**
   * Update a template
   */
  const updateTemplate = useCallback(async (
    templateId: string,
    update: Partial<UITemplatePackage>
  ): Promise<boolean> => {
    const template = installedTemplates.get(templateId);
    if (!template) {
      setError('Template not found');
      return false;
    }
    
    // Merge update
    const updatedPackage: UITemplatePackage = {
      ...template.package,
      ...update,
      metadata: {
        ...template.package.metadata,
        ...update.metadata,
      },
      content: {
        ...template.package.content,
        ...update.content,
      },
    };
    
    setInstalledTemplates(prev => {
      const next = new Map(prev);
      next.set(templateId, {
        ...template,
        package: updatedPackage,
      });
      return next;
    });
    
    return true;
  }, [installedTemplates]);
  
  // ============================================
  // Template Queries
  // ============================================
  
  /**
   * Get a template by ID
   */
  const getTemplate = useCallback((templateId: string): InstalledTemplate | null => {
    return installedTemplates.get(templateId) || null;
  }, [installedTemplates]);
  
  /**
   * Get the currently active template
   */
  const getActiveTemplate = useCallback((): InstalledTemplate | null => {
    if (!activeTemplateId) return null;
    return installedTemplates.get(activeTemplateId) || null;
  }, [activeTemplateId, installedTemplates]);
  
  /**
   * Get templates by category
   */
  const getTemplatesByCategory = useCallback((category: TemplateCategory): InstalledTemplate[] => {
    return Array.from(installedTemplates.values()).filter(
      t => t.package.metadata.category === category
    );
  }, [installedTemplates]);
  
  /**
   * Search templates
   */
  const searchTemplates = useCallback((filter: TemplateFilter): InstalledTemplate[] => {
    let results = Array.from(installedTemplates.values());
    
    if (filter.category) {
      results = results.filter(t => t.package.metadata.category === filter.category);
    }
    
    if (filter.tags && filter.tags.length > 0) {
      results = results.filter(t => {
        const templateTags = t.package.metadata.tags || [];
        return filter.tags!.some(tag => templateTags.includes(tag));
      });
    }
    
    if (filter.search) {
      const searchLower = filter.search.toLowerCase();
      results = results.filter(t =>
        t.package.metadata.name.toLowerCase().includes(searchLower) ||
        t.package.metadata.description?.toLowerCase().includes(searchLower) ||
        t.package.metadata.tags?.some(tag => tag.toLowerCase().includes(searchLower))
      );
    }
    
    if (filter.minRating !== undefined) {
      results = results.filter(t => 
        (t.package.metadata.rating || 0) >= filter.minRating!
      );
    }
    
    if (filter.sourceId) {
      results = results.filter(t => t.source === filter.sourceId);
    }
    
    if (filter.installedOnly) {
      // Already filtered by installedTemplates
    }
    
    if (filter.activeOnly) {
      results = results.filter(t => t.active);
    }
    
    return results;
  }, [installedTemplates]);
  
  // ============================================
  // Source Management
  // ============================================
  
  /**
   * Add a template source
   */
  const addSource = useCallback((source: TemplateSource) => {
    // Implemented if dynamic source addition is needed
    console.log('[TemplateMarketplace] Add source:', source);
  }, []);
  
  /**
   * Remove a template source
   */
  const removeSource = useCallback((sourceId: string) => {
    console.log('[TemplateMarketplace] Remove source:', sourceId);
  }, []);
  
  /**
   * Sync templates from a source
   */
  const syncFromSource = useCallback(async (sourceId: string): Promise<SyncResult> => {
    const source = sources.find(s => s.id === sourceId);
    if (!source) {
      return {
        success: false,
        sourceId,
        templatesAdded: 0,
        templatesUpdated: 0,
        templatesRemoved: 0,
        error: 'Source not found',
      };
    }
    
    if (!source.enabled) {
      return {
        success: false,
        sourceId,
        templatesAdded: 0,
        templatesUpdated: 0,
        templatesRemoved: 0,
        error: 'Source is disabled',
      };
    }
    
    // For local/builtin sources, just update last sync
    if (source.type === 'local' || source.type === 'builtin') {
      setLastSync(new Date());
      return {
        success: true,
        sourceId,
        templatesAdded: 0,
        templatesUpdated: 0,
        templatesRemoved: 0,
      };
    }
    
    // For remote sources, would fetch from URL
    // This is a placeholder for actual remote fetching
    setLoading(true);
    try {
      // Simulated fetch - in real implementation, would call the source URL
      await new Promise(resolve => setTimeout(resolve, 500));
      
      setLastSync(new Date());
      return {
        success: true,
        sourceId,
        templatesAdded: 0,
        templatesUpdated: 0,
        templatesRemoved: 0,
      };
    } catch (e) {
      return {
        success: false,
        sourceId,
        templatesAdded: 0,
        templatesUpdated: 0,
        templatesRemoved: 0,
        error: e instanceof Error ? e.message : 'Sync failed',
      };
    } finally {
      setLoading(false);
    }
  }, [sources]);
  
  // ============================================
  // Import/Export
  // ============================================
  
  /**
   * Export a template as a package
   */
  const exportTemplate = useCallback((templateId: string): UITemplatePackage | null => {
    const template = installedTemplates.get(templateId);
    if (!template) return null;
    return template.package;
  }, [installedTemplates]);
  
  /**
   * Import a template package
   */
  const importTemplate = useCallback(async (pkg: UITemplatePackage): Promise<TemplateInstallResult> => {
    return installTemplate(pkg, 'imported');
  }, [installTemplate]);
  
  // ============================================
  // Context Value
  // ============================================
  
  const value: TemplateMarketplaceContextValue = useMemo(() => ({
    installedTemplates,
    sources,
    activeTemplateId,
    loading,
    error,
    lastSync,
    installTemplate,
    uninstallTemplate,
    activateTemplate,
    deactivateTemplate,
    updateTemplate,
    getTemplate,
    getActiveTemplate,
    getTemplatesByCategory,
    searchTemplates,
    addSource,
    removeSource,
    syncFromSource,
    exportTemplate,
    importTemplate,
    validateTemplate,
  }), [
    installedTemplates,
    sources,
    activeTemplateId,
    loading,
    error,
    lastSync,
    installTemplate,
    uninstallTemplate,
    activateTemplate,
    deactivateTemplate,
    updateTemplate,
    getTemplate,
    getActiveTemplate,
    getTemplatesByCategory,
    searchTemplates,
    addSource,
    removeSource,
    syncFromSource,
    exportTemplate,
    importTemplate,
    validateTemplate,
  ]);
  
  return (
    <TemplateMarketplaceContext.Provider value={value}>
      {children}
    </TemplateMarketplaceContext.Provider>
  );
};

// ============================================
// Hook
// ============================================

/**
 * Hook to access the template marketplace
 */
export function useTemplateMarketplace(): TemplateMarketplaceContextValue {
  const context = useContext(TemplateMarketplaceContext);
  if (!context) {
    throw new Error('useTemplateMarketplace must be used within a TemplateMarketplaceProvider');
  }
  return context;
}

// ============================================
// UI Components
// ============================================

/**
 * Template Card Component
 */
interface TemplateCardProps {
  template: InstalledTemplate;
  onActivate?: (id: string) => void;
  onPreview?: (id: string) => void;
  onDelete?: (id: string) => void;
  isActive?: boolean;
}

export const TemplateCard: React.FC<TemplateCardProps> = ({
  template,
  onActivate,
  onPreview,
  onDelete,
  isActive,
}) => {
  const { metadata } = template.package;
  const categoryColors: Record<TemplateCategory, string> = {
    monitoring: '#3b82f6',
    control: '#22c55e',
    analytics: '#f59e0b',
    overview: '#8b5cf6',
    custom: '#ec4899',
    minimal: '#64748b',
    dark: '#1e293b',
    light: '#f8fafc',
  };
  
  return (
    <div className={`template-card ${isActive ? 'template-card--active' : ''}`} data-template-id={metadata.id}>
      {/* Preview placeholder */}
      <div 
        className="template-card__preview"
        style={{ 
          backgroundColor: template.package.content.theme?.backgroundColor || '#1e293b',
        }}
      >
        {template.package.content.theme?.primaryColor && (
          <div 
            className="template-card__preview-accent"
            style={{ backgroundColor: template.package.content.theme.primaryColor }}
          />
        )}
        <span className="template-card__preview-label">{metadata.category}</span>
      </div>
      
      {/* Info */}
      <div className="template-card__info">
        <h3 className="template-card__title">{metadata.name}</h3>
        <p className="template-card__description">{metadata.description}</p>
        
        <div className="template-card__meta">
          <span className="template-card__version">v{metadata.version}</span>
          {metadata.author && (
            <span className="template-card__author">by {metadata.author}</span>
          )}
          {metadata.rating && (
            <span className="template-card__rating">★ {metadata.rating}</span>
          )}
        </div>
        
        <div className="template-card__tags">
          {metadata.tags?.slice(0, 3).map(tag => (
            <span key={tag} className="template-card__tag">{tag}</span>
          ))}
        </div>
      </div>
      
      {/* Actions */}
      <div className="template-card__actions">
        {isActive ? (
          <span className="template-card__status template-card__status--active">Active</span>
        ) : (
          <button 
            className="template-card__btn template-card__btn--activate"
            onClick={() => onActivate?.(metadata.id)}
          >
            Activate
          </button>
        )}
        <button 
          className="template-card__btn template-card__btn--preview"
          onClick={() => onPreview?.(metadata.id)}
        >
          Preview
        </button>
        {!template.source?.includes('builtin') && (
          <button 
            className="template-card__btn template-card__btn--delete"
            onClick={() => onDelete?.(metadata.id)}
          >
            ×
          </button>
        )}
      </div>
    </div>
  );
};

/**
 * Template Marketplace Panel Component
 */
interface TemplateMarketplacePanelProps {
  className?: string;
  onSelectTemplate?: (templateId: string) => void;
}

export const TemplateMarketplacePanel: React.FC<TemplateMarketplacePanelProps> = ({
  className = '',
  onSelectTemplate,
}) => {
  const {
    installedTemplates,
    activeTemplateId,
    loading,
    error,
    activateTemplate,
    getActiveTemplate,
  } = useTemplateMarketplace();
  
  const [selectedCategory, setSelectedCategory] = useState<TemplateCategory | 'all'>('all');
  const [searchQuery, setSearchQuery] = useState('');
  const [showPreview, setShowPreview] = useState<string | null>(null);
  
  // Filter templates
  const filteredTemplates = useMemo(() => {
    let templates = Array.from(installedTemplates.values());
    
    if (selectedCategory !== 'all') {
      templates = templates.filter(t => t.package.metadata.category === selectedCategory);
    }
    
    if (searchQuery) {
      const query = searchQuery.toLowerCase();
      templates = templates.filter(t =>
        t.package.metadata.name.toLowerCase().includes(query) ||
        t.package.metadata.description?.toLowerCase().includes(query)
      );
    }
    
    // Sort: active first, then by name
    templates.sort((a, b) => {
      if (a.id === activeTemplateId) return -1;
      if (b.id === activeTemplateId) return 1;
      return a.package.metadata.name.localeCompare(b.package.metadata.name);
    });
    
    return templates;
  }, [installedTemplates, selectedCategory, searchQuery, activeTemplateId]);
  
  const categories: Array<{ value: TemplateCategory | 'all'; label: string }> = [
    { value: 'all', label: 'All' },
    { value: 'monitoring', label: 'Monitoring' },
    { value: 'control', label: 'Control' },
    { value: 'analytics', label: 'Analytics' },
    { value: 'overview', label: 'Overview' },
    { value: 'minimal', label: 'Minimal' },
    { value: 'dark', label: 'Dark' },
    { value: 'light', label: 'Light' },
  ];
  
  return (
    <div className={`template-marketplace ${className}`}>
      {/* Header */}
      <div className="template-marketplace__header">
        <h2 className="template-marketplace__title">Template Marketplace</h2>
        <div className="template-marketplace__search">
          <input
            type="text"
            placeholder="Search templates..."
            value={searchQuery}
            onChange={(e) => setSearchQuery(e.target.value)}
            className="template-marketplace__search-input"
          />
        </div>
      </div>
      
      {/* Category filters */}
      <div className="template-marketplace__filters">
        {categories.map(cat => (
          <button
            key={cat.value}
            className={`template-marketplace__filter ${selectedCategory === cat.value ? 'template-marketplace__filter--active' : ''}`}
            onClick={() => setSelectedCategory(cat.value)}
          >
            {cat.label}
          </button>
        ))}
      </div>
      
      {/* Error */}
      {error && (
        <div className="template-marketplace__error">{error}</div>
      )}
      
      {/* Loading */}
      {loading && (
        <div className="template-marketplace__loading">
          <div className="template-marketplace__spinner" />
          <span>Loading templates...</span>
        </div>
      )}
      
      {/* Template grid */}
      <div className="template-marketplace__grid">
        {filteredTemplates.map(template => (
          <TemplateCard
            key={template.id}
            template={template}
            isActive={template.id === activeTemplateId}
            onActivate={async (id) => {
              await activateTemplate(id);
              onSelectTemplate?.(id);
            }}
            onPreview={(id) => setShowPreview(id)}
            onDelete={(id) => {
              if (confirm('Are you sure you want to delete this template?')) {
                // Would call uninstallTemplate
                console.log('Delete template:', id);
              }
            }}
          />
        ))}
      </div>
      
      {/* Empty state */}
      {filteredTemplates.length === 0 && !loading && (
        <div className="template-marketplace__empty">
          <p>No templates found</p>
        </div>
      )}
      
      {/* Preview modal */}
      {showPreview && (
        <TemplatePreviewModal
          templateId={showPreview}
          onClose={() => setShowPreview(null)}
          onActivate={async (id) => {
            await activateTemplate(id);
            setShowPreview(null);
            onSelectTemplate?.(id);
          }}
        />
      )}
    </div>
  );
};

/**
 * Template Preview Modal
 */
interface TemplatePreviewModalProps {
  templateId: string;
  onClose: () => void;
  onActivate: (id: string) => void;
}

const TemplatePreviewModal: React.FC<TemplatePreviewModalProps> = ({
  templateId,
  onClose,
  onActivate,
}) => {
  const { getTemplate, activeTemplateId } = useTemplateMarketplace();
  const template = getTemplate(templateId);
  
  if (!template) return null;
  
  const { metadata, content } = template.package;
  const isActive = templateId === activeTemplateId;
  
  return (
    <div className="template-preview-modal__overlay" onClick={onClose}>
      <div className="template-preview-modal" onClick={e => e.stopPropagation()}>
        {/* Header */}
        <div className="template-preview-modal__header">
          <h2>{metadata.name}</h2>
          <button className="template-preview-modal__close" onClick={onClose}>×</button>
        </div>
        
        {/* Preview area */}
        <div 
          className="template-preview-modal__preview"
          style={{ 
            backgroundColor: content.theme?.backgroundColor || '#1e293b',
            color: content.theme?.textColor || '#e2e8f0',
          }}
        >
          <div className="template-preview-modal__layout">
            {/* Simulated layout preview */}
            {content.layout.panels.map(panel => (
              <div 
                key={panel.id}
                className="template-preview-modal__panel"
                style={{
                  borderColor: content.theme?.borderColor || '#334155',
                  backgroundColor: content.theme?.cardBackground || '#1e293b',
                }}
              >
                <div className="template-preview-modal__panel-header">
                  {panel.title}
                </div>
                <div className="template-preview-modal__panel-content">
                  {panel.type === 'chart' && (
                    <div className="template-preview-modal__chart-placeholder">
                      Chart ({(panel.props?.chartType as string) || 'line'})
                    </div>
                  )}
                  {panel.type === 'table' && (
                    <div className="template-preview-modal__table-placeholder">
                      Table
                    </div>
                  )}
                  {panel.type === 'card' && (
                    <div className="template-preview-modal__card-placeholder">
                      Cards
                    </div>
                  )}
                </div>
              </div>
            ))}
          </div>
        </div>
        
        {/* Info */}
        <div className="template-preview-modal__info">
          <p className="template-preview-modal__description">{metadata.description}</p>
          
          <div className="template-preview-modal__meta">
            <span>Version: {metadata.version}</span>
            {metadata.author && <span>Author: {metadata.author}</span>}
            <span>Category: {metadata.category}</span>
          </div>
          
          {metadata.tags && (
            <div className="template-preview-modal__tags">
              {metadata.tags.map(tag => (
                <span key={tag} className="template-preview-modal__tag">{tag}</span>
              ))}
            </div>
          )}
        </div>
        
        {/* Actions */}
        <div className="template-preview-modal__actions">
          <button className="template-preview-modal__btn" onClick={onClose}>
            Close
          </button>
          {!isActive && (
            <button 
              className="template-preview-modal__btn template-preview-modal__btn--primary"
              onClick={() => onActivate(templateId)}
            >
              Activate Template
            </button>
          )}
          {isActive && (
            <span className="template-preview-modal__active-badge">Currently Active</span>
          )}
        </div>
      </div>
    </div>
  );
};

// ============================================
// Utility Functions
// ============================================

/**
 * Convert a LayoutConfig to a UITemplatePackage
 */
export function layoutToTemplate(
  layout: LayoutConfig,
  metadata: Partial<TemplateMetadata>
): UITemplatePackage {
  return {
    metadata: {
      id: metadata.id || `custom-${Date.now()}`,
      name: metadata.name || layout.name || 'Custom Layout',
      version: metadata.version || '1.0.0',
      description: metadata.description || layout.description || 'Custom layout',
      author: metadata.author,
      category: metadata.category || 'custom',
      tags: metadata.tags || ['custom'],
      createdAt: new Date().toISOString(),
      updatedAt: new Date().toISOString(),
    },
    content: {
      layout,
      theme: metadata.category ? undefined : {
        primaryColor: '#3b82f6',
        secondaryColor: '#60a5fa',
        backgroundColor: '#0f172a',
        cardBackground: '#1e293b',
        textColor: '#e2e8f0',
        borderColor: '#334155',
      },
    },
  };
}

/**
 * Download a template as JSON file
 */
export function downloadTemplate(template: UITemplatePackage): void {
  const blob = new Blob([JSON.stringify(template, null, 2)], { type: 'application/json' });
  const url = URL.createObjectURL(blob);
  const a = document.createElement('a');
  a.href = url;
  a.download = `${template.metadata.id}-v${template.metadata.version}.json`;
  document.body.appendChild(a);
  a.click();
  document.body.removeChild(a);
  URL.revokeObjectURL(url);
}

/**
 * Read a template from a JSON file
 */
export function readTemplateFromFile(file: File): Promise<UITemplatePackage> {
  return new Promise((resolve, reject) => {
    const reader = new FileReader();
    reader.onload = (e) => {
      try {
        const content = e.target?.result as string;
        const template = JSON.parse(content) as UITemplatePackage;
        resolve(template);
      } catch (err) {
        reject(new Error('Invalid template file format'));
      }
    };
    reader.onerror = () => reject(new Error('Failed to read file'));
    reader.readAsText(file);
  });
}

// ============================================
// Exports
// ============================================

export default TemplateMarketplaceProvider;
