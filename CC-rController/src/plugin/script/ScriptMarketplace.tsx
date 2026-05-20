/**
 * Script Marketplace Component
 * Import/Export functionality for scripts
 * Part of Phase 6: Command Script Management System
 */

import React, { useState, useCallback, useEffect, useRef } from 'react';
import type { 
  CommandScript, 
  ScriptPackage, 
  ScriptPackageMetadata,
  ScriptImportResult,
  ScriptSource,
  ScriptCategory
} from './types';

// ============================================
// Script Package Preview
// ============================================

interface ScriptPreviewProps {
  script: CommandScript;
  showFull?: boolean;
}

const ScriptPreview: React.FC<ScriptPreviewProps> = ({ script, showFull = false }) => {
  const [expanded, setExpanded] = useState(false);
  
  const getScriptTypeIcon = (type: string) => {
    const icons: Record<string, string> = {
      shell: '🐚',
      powershell: '⚡',
      python: '🐍',
      lua: '🌙',
      javascript: '📜',
    };
    return icons[type] || '📄';
  };

  const truncatedContent = script.content.slice(0, 200);

  return (
    <div className="script-preview">
      <div className="preview-header" onClick={() => setExpanded(!expanded)}>
        <div className="preview-title-row">
          <span className="script-type-icon">{getScriptTypeIcon(script.scriptType)}</span>
          <h4 className="preview-title">{script.name}</h4>
          {script.isTemplate && <span className="template-badge">Template</span>}
        </div>
        <span className={`expand-icon ${expanded ? 'expanded' : ''}`}>▼</span>
      </div>
      
      {script.description && (
        <p className="preview-description">{script.description}</p>
      )}
      
      <div className="preview-meta">
        <span className="meta-item">
          {script.parameters.length} parameter{script.parameters.length !== 1 ? 's' : ''}
        </span>
        <span className="meta-item">
          v{script.version}
        </span>
        <span className="meta-item">
          {script.usageCount} uses
        </span>
      </div>
      
      {(expanded || showFull) && (
        <div className="preview-content">
          <pre className="code-preview">{script.content}</pre>
        </div>
      )}
    </div>
  );
};

// ============================================
// Import Preview Modal
// ============================================

interface ImportPreviewModalProps {
  pkg: ScriptPackage;
  existingScripts: CommandScript[];
  onConfirm: () => void;
  onCancel: () => void;
  importing?: boolean;
}

const ImportPreviewModal: React.FC<ImportPreviewModalProps> = ({
  pkg,
  existingScripts,
  onConfirm,
  onCancel,
  importing = false,
}) => {
  const [selectedScripts, setSelectedScripts] = useState<Set<string>>(
    new Set(pkg.scripts.map(s => s.id))
  );
  const [overwriteExisting, setOverwriteExisting] = useState<boolean>(true);
  const [importAsCopies, setImportAsCopies] = useState<boolean>(false);

  const toggleScript = (scriptId: string) => {
    const newSelected = new Set(selectedScripts);
    if (newSelected.has(scriptId)) {
      newSelected.delete(scriptId);
    } else {
      newSelected.add(scriptId);
    }
    setSelectedScripts(newSelected);
  };

  const selectAll = () => {
    setSelectedScripts(new Set(pkg.scripts.map(s => s.id)));
  };

  const deselectAll = () => {
    setSelectedScripts(new Set());
  };

  const findExistingScript = (script: CommandScript) => {
    return existingScripts.find(
      s => s.name === script.name && s.scriptType === script.scriptType
    );
  };

  const getImportAction = (script: CommandScript): 'new' | 'skip' | 'overwrite' => {
    const existing = findExistingScript(script);
    if (!existing) return 'new';
    if (importAsCopies) return 'new';
    if (overwriteExisting) return 'overwrite';
    return 'skip';
  };

  const importStats = {
    new: pkg.scripts.filter(s => getImportAction(s) === 'new').length,
    overwrite: pkg.scripts.filter(s => getImportAction(s) === 'overwrite').length,
    skip: pkg.scripts.filter(s => getImportAction(s) === 'skip').length,
  };

  return (
    <div className="modal-overlay" onClick={onCancel}>
      <div className="import-modal" onClick={e => e.stopPropagation()}>
        <div className="modal-header">
          <h2>Import Preview</h2>
          <button className="btn-close" onClick={onCancel}>×</button>
        </div>

        <div className="modal-body">
          {/* Package Info */}
          <div className="package-info">
            <div className="package-header">
              <h3>{pkg.metadata.name}</h3>
              <span className="package-version">v{pkg.metadata.version}</span>
            </div>
            {pkg.metadata.description && (
              <p className="package-description">{pkg.metadata.description}</p>
            )}
            <div className="package-meta">
              <span>By {pkg.metadata.author || 'Unknown'}</span>
              <span>{pkg.scripts.length} script{pkg.scripts.length !== 1 ? 's' : ''}</span>
              {pkg.metadata.tags.length > 0 && (
                <span className="package-tags">
                  {pkg.metadata.tags.map(tag => (
                    <span key={tag} className="tag">{tag}</span>
                  ))}
                </span>
              )}
            </div>
          </div>

          {/* Import Options */}
          <div className="import-options">
            <label className="option-checkbox">
              <input
                type="checkbox"
                checked={overwriteExisting}
                onChange={(e) => setOverwriteExisting(e.target.checked)}
                disabled={importAsCopies}
              />
              <span>Overwrite existing scripts with same name</span>
            </label>
            <label className="option-checkbox">
              <input
                type="checkbox"
                checked={importAsCopies}
                onChange={(e) => setImportAsCopies(e.target.checked)}
              />
              <span>Import all scripts as copies (append " (Copy)")</span>
            </label>
          </div>

          {/* Scripts List */}
          <div className="scripts-selection">
            <div className="selection-header">
              <h4>Scripts to Import ({selectedScripts.size}/{pkg.scripts.length})</h4>
              <div className="selection-actions">
                <button className="btn-link" onClick={selectAll}>Select All</button>
                <button className="btn-link" onClick={deselectAll}>Deselect All</button>
              </div>
            </div>
            
            <div className="scripts-list">
              {pkg.scripts.map(script => {
                const existing = findExistingScript(script);
                const action = getImportAction(script);
                const isSelected = selectedScripts.has(script.id);
                
                return (
                  <div 
                    key={script.id} 
                    className={`script-item ${!isSelected ? 'not-selected' : ''} action-${action}`}
                  >
                    <label className="script-checkbox">
                      <input
                        type="checkbox"
                        checked={isSelected}
                        onChange={() => toggleScript(script.id)}
                      />
                    </label>
                    <div className="script-info">
                      <div className="script-name-row">
                        <span className="script-name">{script.name}</span>
                        {action === 'overwrite' && (
                          <span className="action-badge overwrite">Will overwrite</span>
                        )}
                        {action === 'skip' && (
                          <span className="action-badge skip">Will skip</span>
                        )}
                        {action === 'new' && !isSelected && (
                          <span className="action-badge new">Will add</span>
                        )}
                      </div>
                      {existing && (
                        <span className="existing-note">
                          Conflicts with: "{existing.name}" (v{existing.version})
                        </span>
                      )}
                    </div>
                  </div>
                );
              })}
            </div>
          </div>

          {/* Import Summary */}
          <div className="import-summary">
            <span className="summary-item new">{importStats.new} new</span>
            <span className="summary-item overwrite">{importStats.overwrite} overwrite</span>
            <span className="summary-item skip">{importStats.skip} skip</span>
          </div>
        </div>

        <div className="modal-footer">
          <button className="btn-cancel" onClick={onCancel} disabled={importing}>
            Cancel
          </button>
          <button 
            className="btn-import" 
            onClick={onConfirm}
            disabled={selectedScripts.size === 0 || importing}
          >
            {importing ? 'Importing...' : `Import ${selectedScripts.size} Script${selectedScripts.size !== 1 ? 's' : ''}`}
          </button>
        </div>
      </div>
    </div>
  );
};

// ============================================
// Export Modal Component
// ============================================

interface ExportModalProps {
  scripts: CommandScript[];
  onExport: (metadata: Partial<ScriptPackageMetadata>, selectedIds: Set<string>) => void;
  onCancel: () => void;
}

const ExportModal: React.FC<ExportModalProps> = ({ scripts, onExport, onCancel }) => {
  const [selectedIds, setSelectedIds] = useState<Set<string>>(new Set(scripts.map(s => s.id)));
  const [selectAll, setSelectAll] = useState(true);
  const [metadata, setMetadata] = useState<Partial<ScriptPackageMetadata>>({
    name: 'My Script Package',
    description: '',
    author: '',
    tags: [],
    categories: ['custom'],
  });
  const [tagInput, setTagInput] = useState('');
  const [includeMetadata, setIncludeMetadata] = useState(true);

  const toggleScript = (scriptId: string) => {
    const newSelected = new Set(selectedIds);
    if (newSelected.has(scriptId)) {
      newSelected.delete(scriptId);
    } else {
      newSelected.add(scriptId);
    }
    setSelectAll(newSelected.size === scripts.length);
  };

  const handleSelectAll = () => {
    if (selectAll) {
      setSelectedIds(new Set());
      setSelectAll(false);
    } else {
      setSelectedIds(new Set(scripts.map(s => s.id)));
      setSelectAll(true);
    }
  };

  const addTag = () => {
    const newTag = tagInput.trim();
    if (newTag && !metadata.tags?.includes(newTag)) {
      setMetadata(prev => ({
        ...prev,
        tags: [...(prev.tags || []), newTag],
      }));
    }
    setTagInput('');
  };

  const removeTag = (tag: string) => {
    setMetadata(prev => ({
      ...prev,
      tags: prev.tags?.filter(t => t !== tag) || [],
    }));
  };

  const handleExport = () => {
    onExport(metadata, selectedIds);
  };

  const scriptsToExport = scripts.filter(s => selectedIds.has(s.id));

  return (
    <div className="modal-overlay" onClick={onCancel}>
      <div className="export-modal" onClick={e => e.stopPropagation()}>
        <div className="modal-header">
          <h2>Export Scripts</h2>
          <button className="btn-close" onClick={onCancel}>×</button>
        </div>

        <div className="modal-body">
          {/* Package Metadata */}
          <div className="export-section">
            <label className="section-checkbox">
              <input
                type="checkbox"
                checked={includeMetadata}
                onChange={(e) => setIncludeMetadata(e.target.checked)}
              />
              <span>Include package metadata</span>
            </label>

            {includeMetadata && (
              <div className="metadata-form">
                <div className="field-group">
                  <label>Package Name</label>
                  <input
                    type="text"
                    value={metadata.name}
                    onChange={(e) => setMetadata(prev => ({ ...prev, name: e.target.value }))}
                    placeholder="My Script Package"
                  />
                </div>

                <div className="field-group">
                  <label>Description</label>
                  <textarea
                    value={metadata.description}
                    onChange={(e) => setMetadata(prev => ({ ...prev, description: e.target.value }))}
                    placeholder="A collection of useful scripts..."
                    rows={2}
                  />
                </div>

                <div className="field-group">
                  <label>Author</label>
                  <input
                    type="text"
                    value={metadata.author}
                    onChange={(e) => setMetadata(prev => ({ ...prev, author: e.target.value }))}
                    placeholder="Your name"
                  />
                </div>

                <div className="field-group">
                  <label>Tags</label>
                  <div className="tag-input-container">
                    <div className="tags-list">
                      {metadata.tags?.map(tag => (
                        <span key={tag} className="tag">
                          {tag}
                          <button onClick={() => removeTag(tag)}>×</button>
                        </span>
                      ))}
                      <input
                        type="text"
                        value={tagInput}
                        onChange={(e) => setTagInput(e.target.value)}
                        onKeyDown={(e) => {
                          if (e.key === 'Enter') {
                            e.preventDefault();
                            addTag();
                          }
                        }}
                        placeholder="Add tag..."
                      />
                    </div>
                  </div>
                </div>

                <div className="field-group">
                  <label>Category</label>
                  <select
                    value={metadata.categories?.[0] || 'custom'}
                    onChange={(e) => setMetadata(prev => ({ 
                      ...prev, 
                      categories: [e.target.value as ScriptCategory] 
                    }))}
                  >
                    <option value="system">System</option>
                    <option value="network">Network</option>
                    <option value="security">Security</option>
                    <option value="monitoring">Monitoring</option>
                    <option value="automation">Automation</option>
                    <option value="backup">Backup</option>
                    <option value="diagnostics">Diagnostics</option>
                    <option value="custom">Custom</option>
                  </select>
                </div>
              </div>
            )}
          </div>

          {/* Script Selection */}
          <div className="export-section">
            <div className="selection-header">
              <label className="section-checkbox">
                <input
                  type="checkbox"
                  checked={selectAll}
                  onChange={handleSelectAll}
                />
                <span>Select All ({scripts.length})</span>
              </label>
            </div>

            <div className="scripts-list">
              {scripts.map(script => (
                <div key={script.id} className={`script-item ${!selectedIds.has(script.id) ? 'not-selected' : ''}`}>
                  <label className="script-checkbox">
                    <input
                      type="checkbox"
                      checked={selectedIds.has(script.id)}
                      onChange={() => toggleScript(script.id)}
                    />
                  </label>
                  <div className="script-info">
                    <span className="script-name">{script.name}</span>
                    <span className="script-meta">{script.scriptType} • v{script.version}</span>
                  </div>
                </div>
              ))}
            </div>
          </div>
        </div>

        <div className="modal-footer">
          <button className="btn-cancel" onClick={onCancel}>
            Cancel
          </button>
          <button 
            className="btn-export"
            onClick={handleExport}
            disabled={selectedIds.size === 0}
          >
            Export {selectedIds.size} Script{selectedIds.size !== 1 ? 's' : ''} as JSON
          </button>
        </div>
      </div>
    </div>
  );
};

// ============================================
// Main Script Marketplace Component
// ============================================

export interface ScriptMarketplaceProps {
  scripts: CommandScript[];
  onImport: (pkg: ScriptPackage) => Promise<ScriptImportResult>;
  onExport: (pkg: ScriptPackage) => Promise<void>;
  onClose?: () => void;
}

export const ScriptMarketplace: React.FC<ScriptMarketplaceProps> = ({
  scripts,
  onImport,
  onExport,
  onClose,
}) => {
  const [activeTab, setActiveTab] = useState<'import' | 'export'>('import');
  const [importFile, setImportFile] = useState<File | null>(null);
  const [importData, setImportData] = useState<ScriptPackage | null>(null);
  const [importError, setImportError] = useState<string | null>(null);
  const [importing, setImporting] = useState(false);
  const [showPreview, setShowPreview] = useState(false);
  const [exportModalOpen, setExportModalOpen] = useState(false);
  const [selectedForExport, setSelectedForExport] = useState<CommandScript[]>([]);
  const [exportSuccess, setExportSuccess] = useState<string | null>(null);
  
  const fileInputRef = useRef<HTMLInputElement>(null);
  const dragOverRef = useRef<HTMLDivElement>(null);

  // Handle file selection
  const handleFileSelect = useCallback((file: File) => {
    setImportError(null);
    setImportData(null);
    setImportFile(file);

    const reader = new FileReader();
    reader.onload = (e) => {
      try {
        const content = e.target?.result as string;
        const parsed = JSON.parse(content) as ScriptPackage;
        
        // Validate package structure
        if (!parsed.metadata || !parsed.scripts || !Array.isArray(parsed.scripts)) {
          throw new Error('Invalid package format: missing required fields');
        }

        // Basic validation of scripts
        for (const script of parsed.scripts) {
          if (!script.name || !script.content || !script.scriptType) {
            throw new Error(`Invalid script: ${script.name || 'unnamed'} - missing required fields`);
          }
        }

        setImportData(parsed);
        setShowPreview(true);
      } catch (err) {
        setImportError(err instanceof Error ? err.message : 'Failed to parse file');
        setImportFile(null);
      }
    };
    
    reader.onerror = () => {
      setImportError('Failed to read file');
      setImportFile(null);
    };
    
    reader.readAsText(file);
  }, []);

  // Handle file input change
  const handleInputChange = (e: React.ChangeEvent<HTMLInputElement>) => {
    const file = e.target.files?.[0];
    if (file) {
      handleFileSelect(file);
    }
  };

  // Handle drag and drop
  const handleDragOver = (e: React.DragEvent) => {
    e.preventDefault();
    e.stopPropagation();
  };

  const handleDrop = (e: React.DragEvent) => {
    e.preventDefault();
    e.stopPropagation();
    
    const file = e.dataTransfer.files?.[0];
    if (file && (file.type === 'application/json' || file.name.endsWith('.json'))) {
      handleFileSelect(file);
    } else {
      setImportError('Please drop a JSON file');
    }
  };

  // Handle import confirmation
  const handleImportConfirm = async () => {
    if (!importData) return;
    
    setImporting(true);
    try {
      const result = await onImport(importData);
      if (result.success) {
        setShowPreview(false);
        setImportData(null);
        setImportFile(null);
        setExportSuccess(`Successfully imported ${result.imported} script(s)`);
        setTimeout(() => setExportSuccess(null), 5000);
      } else {
        setImportError(result.errors.join(', '));
      }
    } catch (err) {
      setImportError(err instanceof Error ? err.message : 'Import failed');
    } finally {
      setImporting(false);
    }
  };

  // Handle export
  const handleExport = async (metadata: Partial<ScriptPackageMetadata>, selectedIds: Set<string>) => {
    const selectedScripts = scripts.filter(s => selectedIds.has(s.id));
    
    const pkg: ScriptPackage = {
      metadata: {
        id: `pkg-${Date.now()}`,
        name: metadata.name || 'Exported Scripts',
        version: '1.0.0',
        description: metadata.description || '',
        author: metadata.author || '',
        tags: metadata.tags || [],
        categories: metadata.categories || ['custom'],
        createdAt: new Date().toISOString(),
        updatedAt: new Date().toISOString(),
      },
      scripts: selectedScripts,
    };

    try {
      await onExport(pkg);
      setExportModalOpen(false);
      setExportSuccess(`Successfully exported ${selectedScripts.length} script(s)`);
      setTimeout(() => setExportSuccess(null), 5000);
    } catch (err) {
      setImportError(err instanceof Error ? err.message : 'Export failed');
    }
  };

  // Quick export (all scripts)
  const handleQuickExport = () => {
    setSelectedForExport(scripts);
    setExportModalOpen(true);
  };

  // Quick export selected
  const handleExportSelected = (selected: CommandScript[]) => {
    setSelectedForExport(selected);
    setExportModalOpen(true);
  };

  return (
    <div className="script-marketplace">
      {/* Header */}
      <div className="marketplace-header">
        <div className="header-info">
          <h2>Script Marketplace</h2>
          <p>Import scripts from JSON packages or export your scripts for sharing</p>
        </div>
        {onClose && (
          <button className="btn-close" onClick={onClose}>×</button>
        )}
      </div>

      {/* Success Message */}
      {exportSuccess && (
        <div className="success-message">
          ✓ {exportSuccess}
        </div>
      )}

      {/* Tab Navigation */}
      <div className="tab-navigation">
        <button 
          className={`tab-btn ${activeTab === 'import' ? 'active' : ''}`}
          onClick={() => setActiveTab('import')}
        >
          📥 Import
        </button>
        <button 
          className={`tab-btn ${activeTab === 'export' ? 'active' : ''}`}
          onClick={() => setActiveTab('export')}
        >
          📤 Export
        </button>
      </div>

      {/* Tab Content */}
      <div className="tab-content">
        {/* Import Tab */}
        {activeTab === 'import' && (
          <div className="import-tab">
            <div 
              className="drop-zone"
              ref={dragOverRef}
              onDragOver={handleDragOver}
              onDrop={handleDrop}
              onClick={() => fileInputRef.current?.click()}
            >
              <input
                ref={fileInputRef}
                type="file"
                accept=".json,application/json"
                onChange={handleInputChange}
                style={{ display: 'none' }}
              />
              
              <div className="drop-zone-content">
                <div className="drop-icon">📁</div>
                <h3>Drop script package here</h3>
                <p>or click to browse</p>
                <span className="file-types">Supports JSON files (.json)</span>
              </div>
            </div>

            {importFile && !importData && !importError && (
              <div className="loading-file">
                <div className="spinner"></div>
                <span>Reading {importFile.name}...</span>
              </div>
            )}

            {importError && (
              <div className="import-error">
                <div className="error-icon">⚠️</div>
                <h4>Import Error</h4>
                <p>{importError}</p>
                <button className="btn-retry" onClick={() => {
                  setImportError(null);
                  setImportFile(null);
                }}>
                  Try Again
                </button>
              </div>
            )}

            {importData && (
              <div className="import-preview-summary">
                <div className="preview-summary-header">
                  <h3>{importData.metadata.name}</h3>
                  <span className="script-count">{importData.scripts.length} scripts</span>
                </div>
                <p>{importData.metadata.description}</p>
                <div className="preview-actions">
                  <button 
                    className="btn-preview-full"
                    onClick={() => setShowPreview(true)}
                  >
                    Preview All
                  </button>
                  <button 
                    className="btn-import-now"
                    onClick={() => setShowPreview(true)}
                  >
                    Continue Import
                  </button>
                </div>
              </div>
            )}

            {/* Import Instructions */}
            <div className="import-instructions">
              <h4>Import Instructions</h4>
              <ol>
                <li>Export a script package from another CC Client instance</li>
                <li>Or create a JSON file with the following structure:</li>
              </ol>
              <pre className="format-example">{`{
  "metadata": {
    "name": "Package Name",
    "version": "1.0.0",
    "description": "Description",
    "author": "Author Name",
    "tags": ["tag1", "tag2"]
  },
  "scripts": [
    {
      "name": "Script Name",
      "scriptType": "shell",
      "content": "echo 'Hello World'",
      "parameters": [],
      "tags": [],
      "isTemplate": false
    }
  ]
}`}</pre>
            </div>
          </div>
        )}

        {/* Export Tab */}
        {activeTab === 'export' && (
          <div className="export-tab">
            <div className="export-options">
              <div className="export-card" onClick={handleQuickExport}>
                <div className="export-icon">📋</div>
                <h3>Export All Scripts</h3>
                <p>Export all {scripts.length} scripts as a single package</p>
              </div>
            </div>

            {scripts.length > 0 && (
              <div className="export-selection-section">
                <h4>Or select specific scripts to export:</h4>
                <div className="export-scripts-list">
                  {scripts.map(script => (
                    <label key={script.id} className="export-script-item">
                      <input
                        type="checkbox"
                        onChange={(e) => {
                          if (e.target.checked) {
                            setSelectedForExport([...selectedForExport, script]);
                          } else {
                            setSelectedForExport(selectedForExport.filter(s => s.id !== script.id));
                          }
                        }}
                        checked={selectedForExport.some(s => s.id === script.id)}
                      />
                      <span className="script-name">{script.name}</span>
                      <span className="script-type">{script.scriptType}</span>
                    </label>
                  ))}
                </div>
                
                {selectedForExport.length > 0 && (
                  <button 
                    className="btn-export-selected"
                    onClick={() => setExportModalOpen(true)}
                  >
                    Export {selectedForExport.length} Selected Script{selectedForExport.length !== 1 ? 's' : ''}
                  </button>
                )}
              </div>
            )}

            {scripts.length === 0 && (
              <div className="no-scripts">
                <p>No scripts available for export.</p>
                <p>Create some scripts first!</p>
              </div>
            )}
          </div>
        )}
      </div>

      {/* Import Preview Modal */}
      {showPreview && importData && (
        <ImportPreviewModal
          pkg={importData}
          existingScripts={scripts}
          onConfirm={handleImportConfirm}
          onCancel={() => {
            setShowPreview(false);
            setImportData(null);
            setImportFile(null);
          }}
          importing={importing}
        />
      )}

      {/* Export Modal */}
      {exportModalOpen && (
        <ExportModal
          scripts={selectedForExport.length > 0 ? selectedForExport : scripts}
          onExport={handleExport}
          onCancel={() => setExportModalOpen(false)}
        />
      )}

      {/* Embedded Styles */}
      <style>{`
        .script-marketplace {
          display: flex;
          flex-direction: column;
          height: 100%;
          background: var(--bg-primary, #1a1a1a);
          color: var(--text-primary, #e0e0e0);
        }

        .marketplace-header {
          display: flex;
          justify-content: space-between;
          align-items: flex-start;
          padding: 20px 24px;
          background: var(--bg-secondary, #252525);
          border-bottom: 1px solid var(--border-color, #333);
        }

        .header-info h2 {
          margin: 0 0 4px;
          font-size: 20px;
          font-weight: 600;
        }

        .header-info p {
          margin: 0;
          font-size: 14px;
          color: var(--text-secondary, #a0a0a0);
        }

        .btn-close {
          width: 36px;
          height: 36px;
          background: transparent;
          border: 1px solid var(--border-color, #444);
          border-radius: 8px;
          color: var(--text-secondary, #a0a0a0);
          font-size: 20px;
          cursor: pointer;
          display: flex;
          align-items: center;
          justify-content: center;
        }

        .btn-close:hover {
          background: var(--bg-hover, #333);
          color: var(--text-primary, #e0e0e0);
        }

        .success-message {
          margin: 16px 24px;
          padding: 12px 16px;
          background: rgba(34, 197, 94, 0.1);
          border: 1px solid #22c55e;
          border-radius: 8px;
          color: #22c55e;
          font-size: 14px;
        }

        .tab-navigation {
          display: flex;
          padding: 0 24px;
          background: var(--bg-secondary, #252525);
          border-bottom: 1px solid var(--border-color, #333);
        }

        .tab-btn {
          padding: 12px 20px;
          background: transparent;
          border: none;
          color: var(--text-secondary, #a0a0a0);
          font-size: 14px;
          font-weight: 500;
          cursor: pointer;
          border-bottom: 2px solid transparent;
          margin-bottom: -1px;
          transition: all 0.2s;
        }

        .tab-btn:hover {
          color: var(--text-primary, #e0e0e0);
        }

        .tab-btn.active {
          color: var(--accent-color, #3b82f6);
          border-bottom-color: var(--accent-color, #3b82f6);
        }

        .tab-content {
          flex: 1;
          overflow-y: auto;
          padding: 24px;
        }

        /* Import Tab */
        .import-tab {
          max-width: 600px;
          margin: 0 auto;
        }

        .drop-zone {
          border: 2px dashed var(--border-color, #444);
          border-radius: 12px;
          padding: 40px;
          text-align: center;
          cursor: pointer;
          transition: all 0.2s;
        }

        .drop-zone:hover {
          border-color: var(--accent-color, #3b82f6);
          background: rgba(59, 130, 246, 0.05);
        }

        .drop-zone-content {
          display: flex;
          flex-direction: column;
          align-items: center;
          gap: 8px;
        }

        .drop-icon {
          font-size: 48px;
        }

        .drop-zone-content h3 {
          margin: 0;
          font-size: 16px;
          color: var(--text-primary, #e0e0e0);
        }

        .drop-zone-content p {
          margin: 0;
          font-size: 14px;
          color: var(--text-secondary, #a0a0a0);
        }

        .file-types {
          font-size: 12px;
          color: var(--text-muted, #666);
          margin-top: 8px;
        }

        .loading-file {
          display: flex;
          align-items: center;
          justify-content: center;
          gap: 12px;
          padding: 20px;
          color: var(--text-secondary, #a0a0a0);
        }

        .spinner {
          width: 20px;
          height: 20px;
          border: 2px solid var(--border-color, #444);
          border-top-color: var(--accent-color, #3b82f6);
          border-radius: 50%;
          animation: spin 1s linear infinite;
        }

        @keyframes spin {
          to { transform: rotate(360deg); }
        }

        .import-error {
          padding: 24px;
          background: rgba(239, 68, 68, 0.1);
          border: 1px solid #ef4444;
          border-radius: 12px;
          text-align: center;
        }

        .error-icon {
          font-size: 36px;
          margin-bottom: 8px;
        }

        .import-error h4 {
          margin: 0 0 8px;
          color: #ef4444;
        }

        .import-error p {
          margin: 0 0 16px;
          color: var(--text-secondary, #a0a0a0);
          font-size: 14px;
        }

        .btn-retry {
          padding: 8px 16px;
          background: transparent;
          border: 1px solid #ef4444;
          border-radius: 6px;
          color: #ef4444;
          font-size: 13px;
          cursor: pointer;
        }

        .btn-retry:hover {
          background: rgba(239, 68, 68, 0.1);
        }

        .import-preview-summary {
          margin-top: 20px;
          padding: 20px;
          background: var(--bg-secondary, #252525);
          border: 1px solid var(--border-color, #444);
          border-radius: 12px;
        }

        .preview-summary-header {
          display: flex;
          justify-content: space-between;
          align-items: center;
          margin-bottom: 8px;
        }

        .preview-summary-header h3 {
          margin: 0;
          font-size: 16px;
        }

        .script-count {
          background: var(--accent-color, #3b82f6);
          color: white;
          padding: 4px 10px;
          border-radius: 12px;
          font-size: 12px;
        }

        .import-preview-summary p {
          margin: 0 0 16px;
          color: var(--text-secondary, #a0a0a0);
          font-size: 14px;
        }

        .preview-actions {
          display: flex;
          gap: 12px;
        }

        .btn-preview-full, .btn-import-now {
          flex: 1;
          padding: 10px 16px;
          border-radius: 6px;
          font-size: 14px;
          font-weight: 500;
          cursor: pointer;
          transition: all 0.2s;
        }

        .btn-preview-full {
          background: transparent;
          border: 1px solid var(--border-color, #444);
          color: var(--text-primary, #e0e0e0);
        }

        .btn-preview-full:hover {
          border-color: var(--accent-color, #3b82f6);
        }

        .btn-import-now {
          background: var(--accent-color, #3b82f6);
          border: none;
          color: white;
        }

        .btn-import-now:hover {
          background: var(--accent-hover, #2563eb);
        }

        .import-instructions {
          margin-top: 32px;
          padding: 20px;
          background: var(--bg-secondary, #252525);
          border: 1px solid var(--border-color, #444);
          border-radius: 12px;
        }

        .import-instructions h4 {
          margin: 0 0 12px;
          font-size: 14px;
          color: var(--text-secondary, #a0a0a0);
        }

        .import-instructions ol {
          margin: 0 0 12px;
          padding-left: 20px;
          color: var(--text-secondary, #a0a0a0);
          font-size: 14px;
        }

        .import-instructions li {
          margin: 4px 0;
        }

        .format-example {
          margin: 0;
          padding: 12px;
          background: #1e1e1e;
          border-radius: 6px;
          font-family: 'Monaco', 'Menlo', monospace;
          font-size: 11px;
          color: #d4d4d4;
          overflow-x: auto;
          white-space: pre;
        }

        /* Export Tab */
        .export-tab {
          max-width: 600px;
          margin: 0 auto;
        }

        .export-options {
          display: grid;
          grid-template-columns: repeat(auto-fill, minmax(250px, 1fr));
          gap: 16px;
          margin-bottom: 32px;
        }

        .export-card {
          padding: 24px;
          background: var(--bg-secondary, #252525);
          border: 1px solid var(--border-color, #444);
          border-radius: 12px;
          cursor: pointer;
          transition: all 0.2s;
          text-align: center;
        }

        .export-card:hover {
          border-color: var(--accent-color, #3b82f6);
          transform: translateY(-2px);
        }

        .export-icon {
          font-size: 36px;
          margin-bottom: 12px;
        }

        .export-card h3 {
          margin: 0 0 8px;
          font-size: 16px;
        }

        .export-card p {
          margin: 0;
          font-size: 13px;
          color: var(--text-secondary, #a0a0a0);
        }

        .export-selection-section h4 {
          margin: 0 0 16px;
          font-size: 14px;
          color: var(--text-secondary, #a0a0a0);
        }

        .export-scripts-list {
          background: var(--bg-secondary, #252525);
          border: 1px solid var(--border-color, #444);
          border-radius: 8px;
          max-height: 300px;
          overflow-y: auto;
        }

        .export-script-item {
          display: flex;
          align-items: center;
          gap: 12px;
          padding: 12px 16px;
          cursor: pointer;
          border-bottom: 1px solid var(--border-color, #333);
        }

        .export-script-item:last-child {
          border-bottom: none;
        }

        .export-script-item:hover {
          background: var(--bg-hover, #333);
        }

        .export-script-item .script-name {
          flex: 1;
          font-size: 14px;
        }

        .export-script-item .script-type {
          font-size: 12px;
          color: var(--text-muted, #666);
          background: var(--bg-primary, #1a1a1a);
          padding: 2px 8px;
          border-radius: 4px;
        }

        .btn-export-selected {
          width: 100%;
          margin-top: 16px;
          padding: 12px;
          background: var(--accent-color, #3b82f6);
          border: none;
          border-radius: 8px;
          color: white;
          font-size: 14px;
          font-weight: 500;
          cursor: pointer;
        }

        .btn-export-selected:hover {
          background: var(--accent-hover, #2563eb);
        }

        .no-scripts {
          text-align: center;
          padding: 40px;
          color: var(--text-muted, #666);
        }

        /* Modal Styles */
        .modal-overlay {
          position: fixed;
          top: 0;
          left: 0;
          right: 0;
          bottom: 0;
          background: rgba(0, 0, 0, 0.6);
          display: flex;
          align-items: center;
          justify-content: center;
          z-index: 1000;
          padding: 20px;
        }

        .import-modal, .export-modal {
          background: var(--bg-secondary, #252525);
          border: 1px solid var(--border-color, #444);
          border-radius: 16px;
          width: 100%;
          max-width: 600px;
          max-height: 90vh;
          display: flex;
          flex-direction: column;
          overflow: hidden;
        }

        .modal-header {
          display: flex;
          justify-content: space-between;
          align-items: center;
          padding: 20px 24px;
          border-bottom: 1px solid var(--border-color, #333);
        }

        .modal-header h2 {
          margin: 0;
          font-size: 18px;
        }

        .modal-body {
          flex: 1;
          overflow-y: auto;
          padding: 24px;
        }

        .modal-footer {
          display: flex;
          justify-content: flex-end;
          gap: 12px;
          padding: 16px 24px;
          border-top: 1px solid var(--border-color, #333);
        }

        .btn-cancel {
          padding: 10px 20px;
          background: transparent;
          border: 1px solid var(--border-color, #444);
          border-radius: 6px;
          color: var(--text-primary, #e0e0e0);
          font-size: 14px;
          cursor: pointer;
        }

        .btn-cancel:hover:not(:disabled) {
          background: var(--bg-hover, #333);
        }

        .btn-cancel:disabled {
          opacity: 0.5;
          cursor: not-allowed;
        }

        .btn-import, .btn-export {
          padding: 10px 20px;
          border-radius: 6px;
          font-size: 14px;
          font-weight: 500;
          cursor: pointer;
          border: none;
        }

        .btn-import {
          background: var(--accent-color, #3b82f6);
          color: white;
        }

        .btn-import:hover:not(:disabled) {
          background: var(--accent-hover, #2563eb);
        }

        .btn-import:disabled {
          opacity: 0.5;
          cursor: not-allowed;
        }

        .btn-export {
          background: #22c55e;
          color: white;
        }

        .btn-export:hover:not(:disabled) {
          background: #16a34a;
        }

        /* Import Preview Modal */
        .package-info {
          margin-bottom: 24px;
          padding-bottom: 24px;
          border-bottom: 1px solid var(--border-color, #333);
        }

        .package-header {
          display: flex;
          align-items: center;
          gap: 12px;
          margin-bottom: 8px;
        }

        .package-header h3 {
          margin: 0;
          font-size: 18px;
        }

        .package-version {
          background: var(--bg-primary, #1a1a1a);
          padding: 2px 8px;
          border-radius: 4px;
          font-size: 12px;
          color: var(--text-muted, #666);
        }

        .package-description {
          margin: 0 0 12px;
          color: var(--text-secondary, #a0a0a0);
          font-size: 14px;
        }

        .package-meta {
          display: flex;
          gap: 16px;
          font-size: 13px;
          color: var(--text-muted, #666);
        }

        .package-tags {
          display: flex;
          gap: 6px;
        }

        .import-options {
          margin-bottom: 24px;
        }

        .option-checkbox {
          display: flex;
          align-items: center;
          gap: 8px;
          margin-bottom: 12px;
          font-size: 14px;
          cursor: pointer;
        }

        .selection-header {
          display: flex;
          justify-content: space-between;
          align-items: center;
          margin-bottom: 12px;
        }

        .selection-header h4 {
          margin: 0;
          font-size: 14px;
          color: var(--text-secondary, #a0a0a0);
        }

        .selection-actions {
          display: flex;
          gap: 12px;
        }

        .btn-link {
          background: none;
          border: none;
          color: var(--accent-color, #3b82f6);
          font-size: 13px;
          cursor: pointer;
        }

        .btn-link:hover {
          text-decoration: underline;
        }

        .scripts-list {
          background: var(--bg-primary, #1a1a1a);
          border: 1px solid var(--border-color, #444);
          border-radius: 8px;
          max-height: 250px;
          overflow-y: auto;
        }

        .script-item {
          display: flex;
          align-items: flex-start;
          gap: 12px;
          padding: 12px;
          border-bottom: 1px solid var(--border-color, #333);
        }

        .script-item:last-child {
          border-bottom: none;
        }

        .script-item.not-selected {
          opacity: 0.5;
        }

        .script-checkbox {
          margin-top: 2px;
          cursor: pointer;
        }

        .script-info {
          flex: 1;
        }

        .script-name-row {
          display: flex;
          align-items: center;
          gap: 8px;
          margin-bottom: 4px;
        }

        .script-name {
          font-size: 14px;
          font-weight: 500;
        }

        .action-badge {
          font-size: 10px;
          padding: 2px 6px;
          border-radius: 3px;
        }

        .action-badge.overwrite {
          background: rgba(234, 179, 8, 0.2);
          color: #eab308;
        }

        .action-badge.skip {
          background: rgba(107, 114, 128, 0.2);
          color: #6b7280;
        }

        .action-badge.new {
          background: rgba(34, 197, 94, 0.2);
          color: #22c55e;
        }

        .existing-note {
          display: block;
          font-size: 12px;
          color: var(--text-muted, #666);
        }

        .import-summary {
          display: flex;
          justify-content: center;
          gap: 24px;
          margin-top: 16px;
          padding-top: 16px;
          border-top: 1px solid var(--border-color, #333);
        }

        .summary-item {
          font-size: 13px;
          font-weight: 500;
        }

        .summary-item.new { color: #22c55e; }
        .summary-item.overwrite { color: #eab308; }
        .summary-item.skip { color: #6b7280; }

        /* Export Modal */
        .export-section {
          margin-bottom: 24px;
        }

        .section-checkbox {
          display: flex;
          align-items: center;
          gap: 8px;
          font-size: 14px;
          cursor: pointer;
          margin-bottom: 12px;
        }

        .metadata-form {
          padding: 16px;
          background: var(--bg-primary, #1a1a1a);
          border-radius: 8px;
        }

        .field-group {
          margin-bottom: 16px;
        }

        .field-group:last-child {
          margin-bottom: 0;
        }

        .field-group label {
          display: block;
          margin-bottom: 6px;
          font-size: 13px;
          color: var(--text-secondary, #a0a0a0);
        }

        .field-group input,
        .field-group textarea,
        .field-group select {
          width: 100%;
          padding: 10px 12px;
          background: var(--bg-secondary, #252525);
          border: 1px solid var(--border-color, #444);
          border-radius: 6px;
          color: var(--text-primary, #e0e0e0);
          font-size: 14px;
        }

        .field-group input:focus,
        .field-group textarea:focus,
        .field-group select:focus {
          outline: none;
          border-color: var(--accent-color, #3b82f6);
        }

        .field-group textarea {
          resize: vertical;
          min-height: 60px;
        }

        .tag-input-container {
          background: var(--bg-secondary, #252525);
          border: 1px solid var(--border-color, #444);
          border-radius: 6px;
          padding: 8px;
        }

        .tags-list {
          display: flex;
          flex-wrap: wrap;
          gap: 6px;
        }

        .tag {
          display: inline-flex;
          align-items: center;
          gap: 4px;
          padding: 4px 8px;
          background: var(--accent-color, #3b82f6);
          color: white;
          border-radius: 4px;
          font-size: 12px;
        }

        .tag button {
          background: none;
          border: none;
          color: white;
          cursor: pointer;
          padding: 0;
          font-size: 14px;
          line-height: 1;
          opacity: 0.8;
        }

        .tag button:hover {
          opacity: 1;
        }

        .tag-input-container input {
          flex: 1;
          min-width: 100px;
          background: transparent;
          border: none;
          padding: 4px;
        }

        .tag-input-container input:focus {
          outline: none;
        }

        .script-meta {
          font-size: 12px;
          color: var(--text-muted, #666);
        }

        /* Script Preview Component */
        .script-preview {
          background: var(--bg-primary, #1a1a1a);
          border: 1px solid var(--border-color, #444);
          border-radius: 8px;
          margin-bottom: 8px;
        }

        .preview-header {
          display: flex;
          justify-content: space-between;
          align-items: center;
          padding: 12px;
          cursor: pointer;
        }

        .preview-header:hover {
          background: var(--bg-hover, #333);
        }

        .preview-title-row {
          display: flex;
          align-items: center;
          gap: 8px;
        }

        .script-type-icon {
          font-size: 16px;
        }

        .preview-title {
          margin: 0;
          font-size: 14px;
          font-weight: 500;
        }

        .template-badge {
          background: var(--accent-color, #3b82f6);
          color: white;
          padding: 2px 6px;
          border-radius: 3px;
          font-size: 10px;
        }

        .expand-icon {
          font-size: 12px;
          color: var(--text-muted, #666);
          transition: transform 0.2s;
        }

        .expand-icon.expanded {
          transform: rotate(180deg);
        }

        .preview-description {
          margin: 0;
          padding: 0 12px 12px;
          font-size: 13px;
          color: var(--text-secondary, #a0a0a0);
        }

        .preview-meta {
          display: flex;
          gap: 16px;
          padding: 0 12px 12px;
          font-size: 12px;
          color: var(--text-muted, #666);
        }

        .preview-content {
          border-top: 1px solid var(--border-color, #444);
        }

        .code-preview {
          margin: 0;
          padding: 12px;
          background: #1e1e1e;
          border-radius: 0 0 8px 8px;
          font-family: 'Monaco', 'Menlo', monospace;
          font-size: 12px;
          line-height: 1.5;
          color: #d4d4d4;
          white-space: pre-wrap;
          word-wrap: break-word;
          max-height: 150px;
          overflow-y: auto;
        }

        /* Visual refresh: align Marketplace with Scripts redesigned style */
        .script-marketplace {
          background: var(--bg-card);
          color: var(--text-main);
        }

        .marketplace-header {
          position: sticky;
          top: 0;
          z-index: 20;
          align-items: center;
          padding: 14px 18px;
          background: var(--bg-card);
          border-bottom: 1px solid var(--border-color);
          box-shadow: 0 10px 20px -18px rgba(11, 25, 44, 0.4);
        }

        .header-info h2 {
          font-size: 1.05rem;
          color: var(--text-main);
        }

        .header-info p {
          font-size: 0.82rem;
          color: var(--text-muted);
        }

        .btn-close {
          border: 1px solid var(--border-color);
          color: var(--text-muted);
        }

        .btn-close:hover {
          background: rgba(45, 140, 240, 0.08);
          color: var(--text-main);
        }

        .success-message {
          margin: 14px 18px;
          background: rgba(25, 190, 107, 0.12);
          border: 1px solid rgba(25, 190, 107, 0.5);
          color: #14824a;
        }

        .tab-navigation {
          padding: 0 18px;
          background: var(--bg-card);
          border-bottom: 1px solid var(--border-color);
        }

        .tab-btn {
          color: var(--text-muted);
          border-bottom-width: 3px;
        }

        .tab-btn:hover {
          color: var(--text-main);
        }

        .tab-btn.active {
          color: var(--primary);
          border-bottom-color: var(--primary);
        }

        .tab-content {
          background: linear-gradient(180deg, rgba(244, 247, 249, 0.65) 0%, rgba(244, 247, 249, 1) 100%);
          padding: 18px;
        }

        .import-tab,
        .export-tab {
          max-width: 780px;
        }

        .drop-zone,
        .import-preview-summary,
        .import-instructions,
        .export-card,
        .export-scripts-list {
          background: var(--bg-card);
          border: 1px solid var(--border-color);
          box-shadow: 0 8px 20px rgba(11, 25, 44, 0.04);
        }

        .drop-zone {
          border-style: dashed;
          border-width: 2px;
          border-color: rgba(45, 140, 240, 0.35);
          border-radius: 14px;
          background: linear-gradient(180deg, rgba(45, 140, 240, 0.04) 0%, rgba(45, 140, 240, 0.01) 100%);
        }

        .drop-zone:hover {
          border-color: var(--primary);
          background: linear-gradient(180deg, rgba(45, 140, 240, 0.08) 0%, rgba(45, 140, 240, 0.02) 100%);
        }

        .drop-zone-content h3,
        .preview-summary-header h3,
        .export-card h3 {
          color: var(--text-main);
        }

        .drop-zone-content p,
        .file-types,
        .import-preview-summary p,
        .import-instructions ol,
        .export-card p,
        .export-selection-section h4,
        .no-scripts {
          color: var(--text-muted);
        }

        .script-count,
        .btn-import-now,
        .btn-export-selected,
        .btn-import {
          background: var(--primary);
          border: 1px solid var(--primary);
        }

        .btn-import-now:hover,
        .btn-export-selected:hover,
        .btn-import:hover:not(:disabled) {
          background: var(--primary-hover);
          border-color: var(--primary-hover);
        }

        .btn-preview-full,
        .btn-retry,
        .btn-cancel,
        .btn-export,
        .btn-link {
          color: var(--text-muted);
          border-color: var(--border-color);
        }

        .btn-preview-full,
        .btn-cancel,
        .btn-export {
          background: var(--bg-card);
          border: 1px solid var(--border-color);
        }

        .btn-preview-full:hover,
        .btn-cancel:hover:not(:disabled),
        .btn-export:hover:not(:disabled) {
          border-color: var(--primary);
          color: var(--text-main);
        }

        .btn-export {
          color: #14824a;
          border-color: rgba(25, 190, 107, 0.45);
          background: rgba(25, 190, 107, 0.08);
        }

        .import-instructions h4,
        .selection-header h4,
        .field-group label {
          color: var(--text-muted);
          text-transform: uppercase;
          letter-spacing: 0.04em;
          font-size: 0.76rem;
        }

        .format-example,
        .code-preview {
          background: #101b2f;
          border: 1px solid #152744;
          border-radius: 8px;
        }

        .export-script-item {
          border-color: var(--border-color);
        }

        .export-script-item:hover {
          background: rgba(45, 140, 240, 0.08);
        }

        .export-script-item .script-type,
        .script-meta,
        .package-version,
        .package-meta,
        .existing-note,
        .preview-meta,
        .preview-description,
        .selection-header h4,
        .package-description {
          color: var(--text-muted);
        }

        .scripts-list,
        .metadata-form,
        .script-preview,
        .tag-input-container,
        .field-group input,
        .field-group textarea,
        .field-group select {
          background: var(--bg-main);
          border-color: var(--border-color);
          color: var(--text-main);
        }

        .tag,
        .template-badge {
          background: rgba(45, 140, 240, 0.1);
          color: #1f4e89;
          border: 1px solid rgba(45, 140, 240, 0.22);
        }

        .tag button {
          color: #1f4e89;
        }

        .preview-header:hover {
          background: rgba(45, 140, 240, 0.08);
        }

        .expand-icon {
          color: var(--text-muted);
        }

        .modal-overlay {
          background: rgba(11, 25, 44, 0.42);
          backdrop-filter: blur(2px);
        }

        .import-modal,
        .export-modal {
          background: var(--bg-card);
          border: 1px solid var(--border-color);
          box-shadow: 0 22px 56px rgba(8, 20, 38, 0.24);
        }

        .modal-header,
        .modal-footer,
        .package-info,
        .import-summary,
        .script-item {
          border-color: var(--border-color);
        }

        @media (max-width: 900px) {
          .marketplace-header {
            padding: 12px;
          }

          .tab-navigation {
            padding: 0 12px;
          }

          .tab-content {
            padding: 12px;
          }

          .drop-zone {
            padding: 22px 16px;
          }

          .preview-actions {
            flex-direction: column;
          }

          .modal-body {
            padding: 14px;
          }

          .modal-footer {
            padding: 12px;
            flex-direction: column-reverse;
          }

          .btn-cancel,
          .btn-import,
          .btn-export {
            width: 100%;
          }
        }
      `}</style>
    </div>
  );
};

export default ScriptMarketplace;
