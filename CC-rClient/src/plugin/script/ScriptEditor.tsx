/**
 * Script Editor Component
 * Code editor interface for creating and editing command scripts
 * Part of Phase 6: Command Script Management System
 */

import React, { useState, useCallback, useEffect, useRef, useMemo } from 'react';
import type { 
  CommandScript, 
  ScriptParameter, 
  ScriptType, 
  ScriptValidationResult,
  ParameterType 
} from './types';

// ============================================
// Syntax Highlighting Patterns
// ============================================

const LANGUAGE_PATTERNS: Record<ScriptType, { pattern: RegExp; className: string }[]> = {
  shell: [
    { pattern: /(#.*$)/gm, className: 'comment' },
    { pattern: /(".*?"|'.*?'|`.*?`)/g, className: 'string' },
    { pattern: /(\$\w+|\$\{[^}]+\})/g, className: 'variable' },
    { pattern: /\b(if|then|else|elif|fi|for|do|done|while|until|case|esac|function|return|exit|break|continue)\b/g, className: 'keyword' },
    { pattern: /\b(echo|read|export|local|readonly|unset|shift|set|source|alias|unalias|cd|pwd|ls|mkdir|rm|cp|mv|cat|grep|sed|awk|find|xargs|sort|uniq|wc|head|tail)\b/g, className: 'builtin' },
    { pattern: /\b(true|false|exit|status)\b/g, className: 'literal' },
  ],
  powershell: [
    { pattern: /(#.*$)/gm, className: 'comment' },
    { pattern: /(".*?"|'.*?'|"[^"]*")/g, className: 'string' },
    { pattern: /(\$[a-zA-Z_]\w*)/g, className: 'variable' },
    { pattern: /\b(function|param|begin|process|end|if|else|elseif|switch|while|do|for|foreach|try|catch|finally|throw|return|break|continue|exit)\b/gi, className: 'keyword' },
    { pattern: /\b(Write-Host|Get-Item|Set-Item|Remove-Item|Get-Content|Set-Content|Copy-Item|Move-Item|Invoke-Command|Invoke-Expression|Start-Process|Stop-Process)\b/g, className: 'builtin' },
  ],
  python: [
    { pattern: /(#.*$)/gm, className: 'comment' },
    { pattern: /(".*?"|'.*?'|'\'\'\'[\s\S]*?\'\'\'|"""[\s\S]*?""")/g, className: 'string' },
    { pattern: /(\b[a-zA-Z_]\w*(?=\s*\()|def\s+\w+|class\s+\w+)/g, className: 'function' },
    { pattern: /\b(if|elif|else|for|while|try|except|finally|with|as|import|from|return|yield|raise|break|continue|pass|and|or|not|in|is|lambda|True|False|None|def|class)\b/gi, className: 'keyword' },
    { pattern: /\b(print|len|range|str|int|float|list|dict|set|tuple|open|input|isinstance|type)\b/gi, className: 'builtin' },
  ],
  lua: [
    { pattern: /(--.*$)/gm, className: 'comment' },
    { pattern: /(".*?"|'.*?'|\[=*\[[\s\S]*?\]=*\])/g, className: 'string' },
    { pattern: /(\b[a-zA-Z_]\w*(?=\s*\())/g, className: 'function' },
    { pattern: /\b(if|then|else|elseif|end|for|while|do|repeat|until|function|return|local|true|false|nil|and|or|not|in)\b/gi, className: 'keyword' },
    { pattern: /\b(print|pairs|ipairs|require|module|setmetatable|getmetatable|error|pcall|xpcall|tonumber|tostring|type)\b/gi, className: 'builtin' },
  ],
  javascript: [
    { pattern: /(\/\/.*$|\/\*[\s\S]*?\*\/)/gm, className: 'comment' },
    { pattern: /(".*?"|'.*?'|`.*?`)/g, className: 'string' },
    { pattern: /\b(const|let|var|function|return|if|else|switch|case|for|while|do|break|continue|try|catch|finally|throw|new|class|extends|import|export|from|default|async|await|yield|this|true|false|null|undefined)\b/gi, className: 'keyword' },
    { pattern: /\b(console|process|require|setTimeout|setInterval|Promise|Math|Array|Object|String|Number|JSON|Error)\b/g, className: 'builtin' },
    { pattern: /(\b[a-zA-Z_]\w*(?=\s*\())/g, className: 'function' },
  ],
};

// ============================================
// Syntax Highlighter Component
// ============================================

interface SyntaxHighlighterProps {
  code: string;
  language: ScriptType;
}

const SyntaxHighlighter: React.FC<SyntaxHighlighterProps> = ({ code, language }) => {
  const highlighted = useMemo(() => {
    const patterns = LANGUAGE_PATTERNS[language] || [];
    let result = escapeHtml(code);
    
    // Apply patterns in order
    for (const { pattern, className } of patterns) {
      result = result.replace(pattern, `<span class="syntax-${className}">$1</span>`);
    }
    
    return result;
  }, [code, language]);
  
  return (
    <div 
      className="syntax-highlighter"
      dangerouslySetInnerHTML={{ __html: highlighted }}
    />
  );
};

const escapeHtml = (text: string): string => {
  const div = document.createElement('div');
  div.textContent = text;
  return div.innerHTML;
};

// ============================================
// Parameter Editor Component
// ============================================

interface ParameterEditorProps {
  parameters: ScriptParameter[];
  onChange: (parameters: ScriptParameter[]) => void;
}

const ParameterEditor: React.FC<ParameterEditorProps> = ({ parameters, onChange }) => {
  const addParameter = () => {
    const newParam: ScriptParameter = {
      name: `param${parameters.length + 1}`,
      paramType: 'string',
      defaultValue: '',
      required: false,
      description: '',
    };
    onChange([...parameters, newParam]);
  };

  const updateParameter = (index: number, updates: Partial<ScriptParameter>) => {
    const updated = [...parameters];
    updated[index] = { ...updated[index], ...updates };
    onChange(updated);
  };

  const removeParameter = (index: number) => {
    onChange(parameters.filter((_, i) => i !== index));
  };

  return (
    <div className="parameter-editor">
      <div className="parameter-editor-header">
        <h4>Parameters</h4>
        <button type="button" className="btn-add-param" onClick={addParameter}>
          + Add Parameter
        </button>
      </div>
      
      {parameters.length === 0 ? (
        <div className="no-parameters">
          No parameters defined. Click "Add Parameter" to create one.
        </div>
      ) : (
        <div className="parameters-list">
          {parameters.map((param, index) => (
            <div key={index} className="parameter-item">
              <div className="parameter-row">
                <input
                  type="text"
                  className="param-name"
                  placeholder="Name"
                  value={param.name}
                  onChange={(e) => updateParameter(index, { name: e.target.value })}
                />
                <select
                  className="param-type"
                  value={param.paramType}
                  onChange={(e) => updateParameter(index, { paramType: e.target.value as ParameterType })}
                >
                  <option value="string">String</option>
                  <option value="number">Number</option>
                  <option value="boolean">Boolean</option>
                  <option value="select">Select</option>
                </select>
                <input
                  type="text"
                  className="param-default"
                  placeholder="Default value"
                  value={param.defaultValue}
                  onChange={(e) => updateParameter(index, { defaultValue: e.target.value })}
                />
                <label className="param-required">
                  <input
                    type="checkbox"
                    checked={param.required}
                    onChange={(e) => updateParameter(index, { required: e.target.checked })}
                  />
                  Required
                </label>
                <button
                  type="button"
                  className="btn-remove-param"
                  onClick={() => removeParameter(index)}
                >
                  ×
                </button>
              </div>
              
              <div className="parameter-advanced">
                <input
                  type="text"
                  className="param-description"
                  placeholder="Description (optional)"
                  value={param.description || ''}
                  onChange={(e) => updateParameter(index, { description: e.target.value })}
                />
                <input
                  type="text"
                  className="param-validation"
                  placeholder="Validation regex (optional)"
                  value={param.validation || ''}
                  onChange={(e) => updateParameter(index, { validation: e.target.value })}
                />
                {param.paramType === 'select' && (
                  <input
                    type="text"
                    className="param-options"
                    placeholder="Options (comma-separated)"
                    value={param.options?.join(', ') || ''}
                    onChange={(e) => updateParameter(index, { 
                      options: e.target.value.split(',').map(s => s.trim()).filter(Boolean) 
                    })}
                  />
                )}
              </div>
            </div>
          ))}
        </div>
      )}
    </div>
  );
};

// ============================================
// Main Script Editor Component
// ============================================

export interface ScriptEditorProps {
  script?: CommandScript;
  initialContent?: string;
  onSave: (script: Partial<CommandScript>) => void;
  onCancel: () => void;
  readOnly?: boolean;
}

export const ScriptEditor: React.FC<ScriptEditorProps> = ({
  script,
  initialContent = '',
  onSave,
  onCancel,
  readOnly = false,
}) => {
  const [name, setName] = useState(script?.name || '');
  const [description, setDescription] = useState(script?.description || '');
  const [scriptType, setScriptType] = useState<ScriptType>(script?.scriptType || 'shell');
  const [content, setContent] = useState(script?.content || initialContent);
  const [parameters, setParameters] = useState<ScriptParameter[]>(script?.parameters || []);
  const [tags, setTags] = useState<string[]>(script?.tags || []);
  const [tagInput, setTagInput] = useState('');
  const [isTemplate, setIsTemplate] = useState(script?.isTemplate || false);
  const [validation, setValidation] = useState<ScriptValidationResult | null>(null);
  const [showParameters, setShowParameters] = useState(false);
  
  const editorRef = useRef<HTMLTextAreaElement>(null);
  const highlightRef = useRef<HTMLDivElement>(null);

  // Validate script content
  const validateScript = useCallback((): ScriptValidationResult => {
    const errors: string[] = [];
    const warnings: string[] = [];
    const parameterErrors: Record<string, string[]> = {};

    if (!name.trim()) {
      errors.push('Script name is required');
    }

    if (!content.trim()) {
      errors.push('Script content is required');
    }

    // Check for parameter references in content
    const paramMatches = content.match(/\{\{(\w+)\}\}/g);
    const referencedParams = new Set(paramMatches?.map(m => m.slice(2, -2)) || []);
    
    parameters.forEach(param => {
      const paramErrors: string[] = [];
      
      if (referencedParams.has(param.name)) {
        // Validate default value against regex if provided
        if (param.validation) {
          try {
            const regex = new RegExp(param.validation);
            if (param.defaultValue && !regex.test(param.defaultValue)) {
              paramErrors.push(`Default value "${param.defaultValue}" does not match validation pattern`);
            }
          } catch {
            paramErrors.push('Invalid validation regex pattern');
          }
        }
      }
      
      if (paramErrors.length > 0) {
        parameterErrors[param.name] = paramErrors;
      }
    });

    // Check for unreferenced parameters
    parameters.forEach(param => {
      if (!referencedParams.has(param.name)) {
        warnings.push(`Parameter "${param.name}" is defined but not used in script`);
      }
    });

    // Check for missing parameters
    referencedParams.forEach(paramName => {
      if (!parameters.find(p => p.name === paramName)) {
        errors.push(`Script references undefined parameter: ${paramName}`);
      }
    });

    return {
      valid: errors.length === 0,
      errors,
      warnings,
      parameterErrors,
    };
  }, [name, content, parameters]);

  // Update validation on changes
  useEffect(() => {
    const result = validateScript();
    setValidation(result);
  }, [validateScript]);

  // Sync scroll between textarea and highlighter
  const handleScroll = useCallback(() => {
    if (editorRef.current && highlightRef.current) {
      highlightRef.current.scrollTop = editorRef.current.scrollTop;
      highlightRef.current.scrollLeft = editorRef.current.scrollLeft;
    }
  }, []);

  // Handle content change with parameter highlighting
  const handleContentChange = (e: React.ChangeEvent<HTMLTextAreaElement>) => {
    const newContent = e.target.value;
    setContent(newContent);
    
    // Auto-detect parameter placeholders and suggest parameters
    const paramMatches = newContent.match(/\{\{(\w+)\}\}/g);
    if (paramMatches) {
      const detectedParams = paramMatches.map(m => m.slice(2, -2));
      const existingNames = parameters.map(p => p.name);
      
      // Suggest adding new parameters
      const newParams = detectedParams
        .filter(name => !existingNames.includes(name))
        .map(name => ({
          name,
          paramType: 'string' as ParameterType,
          defaultValue: '',
          required: false,
          description: '',
        }));
      
      if (newParams.length > 0 && parameters.length === 0) {
        setParameters(newParams);
        setShowParameters(true);
      }
    }
  };

  // Handle tab key for indentation
  const handleKeyDown = (e: React.KeyboardEvent<HTMLTextAreaElement>) => {
    if (e.key === 'Tab') {
      e.preventDefault();
      const target = e.target as HTMLTextAreaElement;
      const start = target.selectionStart;
      const end = target.selectionEnd;
      
      const newValue = content.substring(0, start) + '  ' + content.substring(end);
      setContent(newValue);
      
      // Set cursor position after tab
      requestAnimationFrame(() => {
        target.selectionStart = target.selectionEnd = start + 2;
      });
    }
  };

  // Handle tag input
  const handleTagKeyDown = (e: React.KeyboardEvent<HTMLInputElement>) => {
    if (e.key === 'Enter' || e.key === ',') {
      e.preventDefault();
      const newTag = tagInput.trim();
      if (newTag && !tags.includes(newTag)) {
        setTags([...tags, newTag]);
      }
      setTagInput('');
    } else if (e.key === 'Backspace' && !tagInput && tags.length > 0) {
      setTags(tags.slice(0, -1));
    }
  };

  const removeTag = (tagToRemove: string) => {
    setTags(tags.filter(tag => tag !== tagToRemove));
  };

  // Handle save
  const handleSave = () => {
    const result = validateScript();
    if (!result.valid) {
      setValidation(result);
      return;
    }

    onSave({
      id: script?.id,
      name: name.trim(),
      description: description.trim(),
      scriptType,
      content,
      parameters,
      tags,
      isTemplate,
      version: script?.version || 1,
      createdBy: script?.createdBy || 'current-user',
      createdAt: script?.createdAt || new Date().toISOString(),
      updatedAt: new Date().toISOString(),
    });
  };

  return (
    <div className="script-editor">
      <div className="editor-header">
        <h2>{script ? 'Edit Script' : 'Create New Script'}</h2>
        <div className="editor-actions">
          <button 
            type="button" 
            className="btn-cancel"
            onClick={onCancel}
          >
            Cancel
          </button>
          <button 
            type="button" 
            className="btn-save"
            onClick={handleSave}
            disabled={!validation?.valid || readOnly}
          >
            {script ? 'Update Script' : 'Create Script'}
          </button>
        </div>
      </div>

      <div className="editor-body">
        {/* Metadata Section */}
        <div className="editor-section metadata-section">
          <div className="field-group">
            <label className="field-label">
              Script Name <span className="required">*</span>
            </label>
            <input
              type="text"
              className="field-input"
              placeholder="Enter script name"
              value={name}
              onChange={(e) => setName(e.target.value)}
              disabled={readOnly}
            />
          </div>

          <div className="field-group">
            <label className="field-label">Description</label>
            <textarea
              className="field-textarea"
              placeholder="Describe what this script does"
              value={description}
              onChange={(e) => setDescription(e.target.value)}
              disabled={readOnly}
              rows={2}
            />
          </div>

          <div className="field-row">
            <div className="field-group">
              <label className="field-label">Script Type</label>
              <select
                className="field-select"
                value={scriptType}
                onChange={(e) => setScriptType(e.target.value as ScriptType)}
                disabled={readOnly}
              >
                <option value="shell">Shell (Bash)</option>
                <option value="powershell">PowerShell</option>
                <option value="python">Python</option>
                <option value="lua">Lua</option>
                <option value="javascript">JavaScript</option>
              </select>
            </div>

            <div className="field-group">
              <label className="field-checkbox">
                <input
                  type="checkbox"
                  checked={isTemplate}
                  onChange={(e) => setIsTemplate(e.target.checked)}
                  disabled={readOnly}
                />
                <span>Save as Template</span>
              </label>
            </div>
          </div>

          <div className="field-group">
            <label className="field-label">Tags</label>
            <div className="tag-input-container">
              <div className="tags-list">
                {tags.map(tag => (
                  <span key={tag} className="tag">
                    {tag}
                    {!readOnly && (
                      <button
                        type="button"
                        className="tag-remove"
                        onClick={() => removeTag(tag)}
                      >
                        ×
                      </button>
                    )}
                  </span>
                ))}
                {!readOnly && (
                  <input
                    type="text"
                    className="tag-input"
                    placeholder={tags.length === 0 ? "Add tags..." : ""}
                    value={tagInput}
                    onChange={(e) => setTagInput(e.target.value)}
                    onKeyDown={handleTagKeyDown}
                  />
                )}
              </div>
            </div>
            <div className="field-hint">Press Enter or comma to add a tag</div>
          </div>
        </div>

        {/* Parameters Section */}
        <div className="editor-section">
          <div 
            className="section-header"
            onClick={() => setShowParameters(!showParameters)}
          >
            <h3>Parameters {parameters.length > 0 && `(${parameters.length})`}</h3>
            <span className={`toggle-icon ${showParameters ? 'open' : ''}`}>▼</span>
          </div>
          
          {showParameters && (
            <ParameterEditor
              parameters={parameters}
              onChange={readOnly ? () => {} : setParameters}
            />
          )}
        </div>

        {/* Code Editor Section */}
        <div className="editor-section code-section">
          <div className="section-header">
            <h3>Script Content <span className="required">*</span></h3>
            <div className="code-hints">
              Use <code>{'{{parameterName}}'}</code> for parameters
            </div>
          </div>
          
          <div className="code-editor-container">
            <div className="code-editor-wrapper">
              <SyntaxHighlighter code={content} language={scriptType} />
              <textarea
                ref={editorRef}
                className="code-textarea"
                value={content}
                onChange={handleContentChange}
                onScroll={handleScroll}
                onKeyDown={handleKeyDown}
                disabled={readOnly}
                placeholder="Enter your script here..."
                spellCheck={false}
              />
            </div>
          </div>
        </div>

        {/* Validation Messages */}
        {validation && (
          <div className="validation-section">
            {validation.errors.length > 0 && (
              <div className="validation-errors">
                <h4>Errors</h4>
                <ul>
                  {validation.errors.map((error, i) => (
                    <li key={i}>{error}</li>
                  ))}
                </ul>
              </div>
            )}
            
            {validation.warnings.length > 0 && (
              <div className="validation-warnings">
                <h4>Warnings</h4>
                <ul>
                  {validation.warnings.map((warning, i) => (
                    <li key={i}>{warning}</li>
                  ))}
                </ul>
              </div>
            )}
            
            {validation.valid && validation.warnings.length === 0 && (
              <div className="validation-success">
                ✓ Script is valid
              </div>
            )}
          </div>
        )}
      </div>

      {/* Embedded Styles */}
      <style>{`
        .script-editor {
          display: flex;
          flex-direction: column;
          height: 100%;
          background: var(--bg-primary, #1a1a1a);
          color: var(--text-primary, #e0e0e0);
        }

        .editor-header {
          display: flex;
          justify-content: space-between;
          align-items: center;
          padding: 16px 24px;
          border-bottom: 1px solid var(--border-color, #333);
          background: var(--bg-secondary, #252525);
        }

        .editor-header h2 {
          margin: 0;
          font-size: 20px;
          font-weight: 600;
        }

        .editor-actions {
          display: flex;
          gap: 12px;
        }

        .btn-cancel, .btn-save {
          padding: 8px 16px;
          border-radius: 6px;
          font-size: 14px;
          font-weight: 500;
          cursor: pointer;
          transition: all 0.2s;
        }

        .btn-cancel {
          background: transparent;
          border: 1px solid var(--border-color, #444);
          color: var(--text-secondary, #a0a0a0);
        }

        .btn-cancel:hover {
          background: var(--bg-hover, #333);
        }

        .btn-save {
          background: var(--accent-color, #3b82f6);
          border: none;
          color: white;
        }

        .btn-save:hover:not(:disabled) {
          background: var(--accent-hover, #2563eb);
        }

        .btn-save:disabled {
          opacity: 0.5;
          cursor: not-allowed;
        }

        .editor-body {
          flex: 1;
          overflow-y: auto;
          padding: 24px;
        }

        .editor-section {
          margin-bottom: 24px;
          padding: 20px;
          background: var(--bg-secondary, #252525);
          border-radius: 8px;
          border: 1px solid var(--border-color, #333);
        }

        .metadata-section {
          display: flex;
          flex-direction: column;
          gap: 16px;
        }

        .field-group {
          display: flex;
          flex-direction: column;
          gap: 6px;
        }

        .field-row {
          display: flex;
          gap: 16px;
          align-items: flex-start;
        }

        .field-row .field-group {
          flex: 1;
        }

        .field-label {
          font-size: 13px;
          font-weight: 500;
          color: var(--text-secondary, #a0a0a0);
        }

        .field-label .required {
          color: #ef4444;
        }

        .field-input, .field-textarea, .field-select {
          padding: 10px 12px;
          background: var(--bg-primary, #1a1a1a);
          border: 1px solid var(--border-color, #444);
          border-radius: 6px;
          color: var(--text-primary, #e0e0e0);
          font-size: 14px;
          transition: border-color 0.2s;
        }

        .field-input:focus, .field-textarea:focus, .field-select:focus {
          outline: none;
          border-color: var(--accent-color, #3b82f6);
        }

        .field-input:disabled, .field-textarea:disabled, .field-select:disabled {
          opacity: 0.6;
          cursor: not-allowed;
        }

        .field-textarea {
          resize: vertical;
          min-height: 60px;
        }

        .field-checkbox {
          display: flex;
          align-items: center;
          gap: 8px;
          cursor: pointer;
          padding-top: 24px;
        }

        .field-checkbox input {
          width: 18px;
          height: 18px;
          cursor: pointer;
        }

        .field-hint {
          font-size: 12px;
          color: var(--text-muted, #666);
        }

        .tag-input-container {
          background: var(--bg-primary, #1a1a1a);
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

        .tag-remove {
          background: none;
          border: none;
          color: white;
          cursor: pointer;
          padding: 0;
          font-size: 14px;
          line-height: 1;
          opacity: 0.8;
        }

        .tag-remove:hover {
          opacity: 1;
        }

        .tag-input {
          flex: 1;
          min-width: 100px;
          background: transparent;
          border: none;
          color: var(--text-primary, #e0e0e0);
          font-size: 14px;
          padding: 4px;
        }

        .tag-input:focus {
          outline: none;
        }

        .section-header {
          display: flex;
          justify-content: space-between;
          align-items: center;
          cursor: pointer;
          user-select: none;
        }

        .section-header h3 {
          margin: 0;
          font-size: 16px;
          font-weight: 500;
        }

        .toggle-icon {
          font-size: 12px;
          color: var(--text-muted, #666);
          transition: transform 0.2s;
        }

        .toggle-icon.open {
          transform: rotate(180deg);
        }

        .parameter-editor {
          margin-top: 16px;
        }

        .parameter-editor-header {
          display: flex;
          justify-content: space-between;
          align-items: center;
          margin-bottom: 12px;
        }

        .parameter-editor-header h4 {
          margin: 0;
          font-size: 14px;
          color: var(--text-secondary, #a0a0a0);
        }

        .btn-add-param {
          padding: 6px 12px;
          background: var(--accent-color, #3b82f6);
          border: none;
          border-radius: 4px;
          color: white;
          font-size: 13px;
          cursor: pointer;
        }

        .btn-add-param:hover {
          background: var(--accent-hover, #2563eb);
        }

        .no-parameters {
          padding: 20px;
          text-align: center;
          color: var(--text-muted, #666);
          font-size: 14px;
        }

        .parameters-list {
          display: flex;
          flex-direction: column;
          gap: 12px;
        }

        .parameter-item {
          padding: 12px;
          background: var(--bg-primary, #1a1a1a);
          border: 1px solid var(--border-color, #444);
          border-radius: 6px;
        }

        .parameter-row {
          display: flex;
          gap: 8px;
          align-items: center;
        }

        .param-name {
          width: 120px;
          padding: 6px 8px;
          background: var(--bg-secondary, #252525);
          border: 1px solid var(--border-color, #444);
          border-radius: 4px;
          color: var(--text-primary, #e0e0e0);
          font-size: 13px;
        }

        .param-type {
          width: 100px;
          padding: 6px 8px;
          background: var(--bg-secondary, #252525);
          border: 1px solid var(--border-color, #444);
          border-radius: 4px;
          color: var(--text-primary, #e0e0e0);
          font-size: 13px;
        }

        .param-default {
          flex: 1;
          padding: 6px 8px;
          background: var(--bg-secondary, #252525);
          border: 1px solid var(--border-color, #444);
          border-radius: 4px;
          color: var(--text-primary, #e0e0e0);
          font-size: 13px;
        }

        .param-required {
          display: flex;
          align-items: center;
          gap: 4px;
          font-size: 12px;
          color: var(--text-secondary, #a0a0a0);
          cursor: pointer;
        }

        .btn-remove-param {
          width: 28px;
          height: 28px;
          background: transparent;
          border: 1px solid var(--border-color, #444);
          border-radius: 4px;
          color: var(--text-muted, #666);
          font-size: 18px;
          cursor: pointer;
          display: flex;
          align-items: center;
          justify-content: center;
        }

        .btn-remove-param:hover {
          background: #ef4444;
          border-color: #ef4444;
          color: white;
        }

        .parameter-advanced {
          display: flex;
          gap: 8px;
          margin-top: 8px;
        }

        .param-description {
          flex: 2;
          padding: 6px 8px;
          background: var(--bg-secondary, #252525);
          border: 1px solid var(--border-color, #444);
          border-radius: 4px;
          color: var(--text-primary, #e0e0e0);
          font-size: 12px;
        }

        .param-validation {
          flex: 1;
          padding: 6px 8px;
          background: var(--bg-secondary, #252525);
          border: 1px solid var(--border-color, #444);
          border-radius: 4px;
          color: var(--text-primary, #e0e0e0);
          font-size: 12px;
        }

        .param-options {
          flex: 1;
          padding: 6px 8px;
          background: var(--bg-secondary, #252525);
          border: 1px solid var(--border-color, #444);
          border-radius: 4px;
          color: var(--text-primary, #e0e0e0);
          font-size: 12px;
        }

        .code-section .section-header {
          margin-bottom: 12px;
        }

        .code-hints {
          font-size: 12px;
          color: var(--text-muted, #666);
        }

        .code-hints code {
          background: var(--bg-primary, #1a1a1a);
          padding: 2px 6px;
          border-radius: 3px;
          color: var(--accent-color, #3b82f6);
        }

        .code-editor-container {
          border: 1px solid var(--border-color, #444);
          border-radius: 6px;
          overflow: hidden;
        }

        .code-editor-wrapper {
          position: relative;
          min-height: 300px;
          max-height: 500px;
        }

        .syntax-highlighter {
          position: absolute;
          top: 0;
          left: 0;
          right: 0;
          bottom: 0;
          padding: 12px;
          background: #1e1e1e;
          color: #d4d4d4;
          font-family: 'Monaco', 'Menlo', 'Ubuntu Mono', monospace;
          font-size: 13px;
          line-height: 1.5;
          white-space: pre-wrap;
          word-wrap: break-word;
          overflow: auto;
          pointer-events: none;
        }

        .syntax-highlighter .syntax-comment { color: #6a9955; }
        .syntax-highlighter .syntax-string { color: #ce9178; }
        .syntax-highlighter .syntax-variable { color: #9cdcfe; }
        .syntax-highlighter .syntax-keyword { color: #569cd6; }
        .syntax-highlighter .syntax-builtin { color: #4ec9b0; }
        .syntax-highlighter .syntax-literal { color: #569cd6; }
        .syntax-highlighter .syntax-function { color: #dcdcaa; }

        .code-textarea {
          position: relative;
          width: 100%;
          min-height: 300px;
          max-height: 500px;
          padding: 12px;
          background: transparent;
          border: none;
          color: transparent;
          caret-color: #d4d4d4;
          font-family: 'Monaco', 'Menlo', 'Ubuntu Mono', monospace;
          font-size: 13px;
          line-height: 1.5;
          resize: vertical;
          outline: none;
          white-space: pre-wrap;
          word-wrap: break-word;
        }

        .code-textarea::selection {
          background: #264f78;
        }

        .code-textarea:disabled {
          opacity: 0.7;
        }

        .validation-section {
          padding: 16px;
          border-radius: 6px;
          margin-top: 16px;
        }

        .validation-errors {
          background: rgba(239, 68, 68, 0.1);
          border: 1px solid #ef4444;
          border-radius: 6px;
          padding: 12px;
        }

        .validation-errors h4 {
          margin: 0 0 8px 0;
          color: #ef4444;
          font-size: 14px;
        }

        .validation-errors ul {
          margin: 0;
          padding-left: 20px;
        }

        .validation-errors li {
          color: #ef4444;
          font-size: 13px;
          margin: 4px 0;
        }

        .validation-warnings {
          background: rgba(234, 179, 8, 0.1);
          border: 1px solid #eab308;
          border-radius: 6px;
          padding: 12px;
          margin-top: 8px;
        }

        .validation-warnings h4 {
          margin: 0 0 8px 0;
          color: #eab308;
          font-size: 14px;
        }

        .validation-warnings ul {
          margin: 0;
          padding-left: 20px;
        }

        .validation-warnings li {
          color: #eab308;
          font-size: 13px;
          margin: 4px 0;
        }

        .validation-success {
          background: rgba(34, 197, 94, 0.1);
          border: 1px solid #22c55e;
          border-radius: 6px;
          padding: 12px;
          color: #22c55e;
          font-size: 14px;
        }
      `}</style>
    </div>
  );
};

export default ScriptEditor;
