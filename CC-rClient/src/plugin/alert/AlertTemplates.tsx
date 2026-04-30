/**
 * AlertTemplates - Template marketplace for alert rules
 * Part of Phase 9: Alert Rules Management System
 */

import React, { useState, useEffect } from 'react';
import { alertApi } from './api';
import type { AlertRule, AlertTemplate, AlertTemplateCategory, AlertRuleImportResult } from './types';

export interface AlertTemplatesProps {
  onImport: (rule: AlertRule) => void;
  onClose: () => void;
}

type SortOption = 'popular' | 'rating' | 'newest' | 'name';

export const AlertTemplates: React.FC<AlertTemplatesProps> = ({ onImport, onClose }) => {
  const [templates, setTemplates] = useState<AlertTemplate[]>([]);
  const [isLoading, setIsLoading] = useState(true);
  const [error, setError] = useState<string | null>(null);
  const [selectedCategory, setSelectedCategory] = useState<AlertTemplateCategory | 'all'>('all');
  const [searchQuery, setSearchQuery] = useState('');
  const [sortBy, setSortBy] = useState<SortOption>('popular');

  // Load templates on mount
  useEffect(() => {
    loadTemplates();
  }, []);

  const loadTemplates = async () => {
    try {
      setIsLoading(true);
      setError(null);
      const rules = await alertApi.getAlertTemplates();
      // Convert rules to templates (they have same structure minus some fields)
      const mappedTemplates: AlertTemplate[] = rules.map((rule) => ({
        id: rule.id,
        name: rule.name,
        description: rule.description || '',
        category: 'custom',
        rule: rule as any,
        installCount: 0,
      }));
      setTemplates(mappedTemplates);
    } catch (err) {
      setError(err instanceof Error ? err.message : 'Failed to load templates');
    } finally {
      setIsLoading(false);
    }
  };

  // Filter and sort templates
  const filteredTemplates = React.useMemo(() => {
    let result = [...templates];

    // Category filter
    if (selectedCategory !== 'all') {
      result = result.filter((t) => t.category === selectedCategory);
    }

    // Search filter
    if (searchQuery) {
      const query = searchQuery.toLowerCase();
      result = result.filter(
        (t) =>
          t.name.toLowerCase().includes(query) ||
          t.description.toLowerCase().includes(query)
      );
    }

    // Sort
    switch (sortBy) {
      case 'popular':
        result.sort((a, b) => b.installCount - a.installCount);
        break;
      case 'rating':
        result.sort((a, b) => (b.rating || 0) - (a.rating || 0));
        break;
      case 'newest':
        // Would need createdAt field - for now just name sort
        result.sort((a, b) => a.name.localeCompare(b.name));
        break;
      case 'name':
        result.sort((a, b) => a.name.localeCompare(b.name));
        break;
    }

    return result;
  }, [templates, selectedCategory, searchQuery, sortBy]);

  const handleImport = (template: AlertTemplate) => {
    // Create a new rule from the template
    const newRule: AlertRule = {
      ...template.rule,
      id: crypto.randomUUID(),
      name: `${template.name} (Copy)`,
      createdAt: new Date().toISOString(),
      updatedAt: new Date().toISOString(),
      isTemplate: false,
      usageCount: 0,
      version: 1,
    };
    onImport(newRule);
  };

  const CATEGORIES: { value: AlertTemplateCategory | 'all'; label: string; icon: string }[] = [
    { value: 'all', label: 'All Templates', icon: '📋' },
    { value: 'system', label: 'System', icon: '🖥️' },
    { value: 'network', label: 'Network', icon: '🌐' },
    { value: 'security', label: 'Security', icon: '🔒' },
    { value: 'application', label: 'Application', icon: '📱' },
    { value: 'custom', label: 'Custom', icon: '✨' },
  ];

  return (
    <div className="alert-templates">
      <div className="templates-header">
        <h2>Alert Templates</h2>
        <button className="btn-close" onClick={onClose}>×</button>
      </div>

      <div className="templates-toolbar">
        <input
          type="text"
          className="search-input"
          placeholder="Search templates..."
          value={searchQuery}
          onChange={(e) => setSearchQuery(e.target.value)}
        />
        <select
          className="sort-select"
          value={sortBy}
          onChange={(e) => setSortBy(e.target.value as SortOption)}
        >
          <option value="popular">Most Popular</option>
          <option value="rating">Highest Rated</option>
          <option value="newest">Newest</option>
          <option value="name">Name (A-Z)</option>
        </select>
      </div>

      <div className="templates-body">
        <div className="categories-sidebar">
          <h3>Categories</h3>
          {CATEGORIES.map((cat) => (
            <button
              key={cat.value}
              className={`category-btn ${selectedCategory === cat.value ? 'active' : ''}`}
              onClick={() => setSelectedCategory(cat.value)}
            >
              <span className="category-icon">{cat.icon}</span>
              <span className="category-label">{cat.label}</span>
            </button>
          ))}
        </div>

        <div className="templates-grid">
          {isLoading ? (
            <div className="loading-state">
              <p>Loading templates...</p>
            </div>
          ) : error ? (
            <div className="error-state">
              <p>Error: {error}</p>
              <button className="btn btn-secondary" onClick={loadTemplates}>
                Retry
              </button>
            </div>
          ) : filteredTemplates.length === 0 ? (
            <div className="empty-state">
              <p>No templates found</p>
            </div>
          ) : (
            filteredTemplates.map((template) => (
              <div key={template.id} className="template-card">
                <div className="template-header">
                  <span className={`category-badge cat-${template.category}`}>
                    {CATEGORIES.find((c) => c.value === template.category)?.icon || '📋'}
                  </span>
                  <h4 className="template-name">{template.name}</h4>
                </div>
                <p className="template-description">{template.description}</p>
                <div className="template-meta">
                  {template.rating && (
                    <span className="meta-item">
                      ⭐ {template.rating.toFixed(1)}
                    </span>
                  )}
                  <span className="meta-item">
                    📥 {template.installCount} installs
                  </span>
                </div>
                <div className="template-actions">
                  <button
                    className="btn btn-primary btn-sm"
                    onClick={() => handleImport(template)}
                  >
                    Use Template
                  </button>
                </div>
              </div>
            ))
          )}
        </div>
      </div>

      <style>{`
        .alert-templates {
          position: fixed;
          inset: 0;
          background: var(--bg-primary);
          z-index: 1300;
          display: flex;
          flex-direction: column;
        }

        .templates-header {
          display: flex;
          justify-content: space-between;
          align-items: center;
          padding: 20px 24px;
          border-bottom: 1px solid var(--border-color);
          background: var(--bg-card);
        }

        .templates-header h2 {
          margin: 0;
          font-size: 18px;
        }

        .btn-close {
          background: none;
          border: none;
          font-size: 24px;
          cursor: pointer;
          padding: 0;
          line-height: 1;
          color: var(--text-secondary);
        }

        .templates-toolbar {
          display: flex;
          gap: 12px;
          padding: 16px 24px;
          border-bottom: 1px solid var(--border-color);
          background: var(--bg-hover);
        }

        .search-input {
          flex: 1;
          padding: 10px 14px;
          border: 1px solid var(--border-color);
          border-radius: 6px;
          background: var(--bg-input);
          color: var(--text-primary);
          font-size: 14px;
        }

        .sort-select {
          padding: 10px 14px;
          border: 1px solid var(--border-color);
          border-radius: 6px;
          background: var(--bg-input);
          color: var(--text-primary);
          font-size: 14px;
          min-width: 150px;
        }

        .templates-body {
          flex: 1;
          display: flex;
          overflow: hidden;
        }

        .categories-sidebar {
          width: 220px;
          padding: 20px;
          border-right: 1px solid var(--border-color);
          background: var(--bg-card);
          overflow-y: auto;
        }

        .categories-sidebar h3 {
          margin: 0 0 16px 0;
          font-size: 13px;
          text-transform: uppercase;
          color: var(--text-secondary);
        }

        .category-btn {
          display: flex;
          align-items: center;
          gap: 10px;
          width: 100%;
          padding: 10px 12px;
          border: none;
          border-radius: 6px;
          background: transparent;
          cursor: pointer;
          text-align: left;
          font-size: 14px;
          color: var(--text-primary);
          transition: background 0.2s;
        }

        .category-btn:hover {
          background: var(--bg-hover);
        }

        .category-btn.active {
          background: var(--color-primary);
          color: white;
        }

        .category-icon {
          font-size: 16px;
        }

        .templates-grid {
          flex: 1;
          padding: 20px;
          overflow-y: auto;
          display: grid;
          grid-template-columns: repeat(auto-fill, minmax(280px, 1fr));
          gap: 16px;
          align-content: start;
        }

        .template-card {
          border: 1px solid var(--border-color);
          border-radius: 10px;
          padding: 16px;
          background: var(--bg-card);
          display: flex;
          flex-direction: column;
          transition: border-color 0.2s, box-shadow 0.2s;
        }

        .template-card:hover {
          border-color: var(--color-primary);
          box-shadow: 0 4px 12px rgba(0, 0, 0, 0.08);
        }

        .template-header {
          display: flex;
          align-items: flex-start;
          gap: 10px;
          margin-bottom: 8px;
        }

        .category-badge {
          font-size: 18px;
        }

        .template-name {
          margin: 0;
          font-size: 15px;
          font-weight: 600;
          flex: 1;
        }

        .template-description {
          margin: 0 0 12px 0;
          font-size: 13px;
          color: var(--text-secondary);
          line-height: 1.4;
          flex: 1;
        }

        .template-meta {
          display: flex;
          gap: 16px;
          margin-bottom: 12px;
        }

        .meta-item {
          font-size: 12px;
          color: var(--text-muted);
        }

        .template-actions {
          display: flex;
          justify-content: flex-end;
        }

        .btn {
          padding: 8px 16px;
          border: none;
          border-radius: 6px;
          cursor: pointer;
          font-weight: 500;
          font-size: 14px;
          transition: background 0.2s;
        }

        .btn-primary {
          background: var(--color-primary);
          color: white;
        }

        .btn-primary:hover {
          background: var(--color-primary-dark);
        }

        .btn-secondary {
          background: var(--bg-hover);
          color: var(--text-primary);
        }

        .btn-secondary:hover {
          background: var(--bg-disabled);
        }

        .btn-sm {
          padding: 6px 12px;
          font-size: 13px;
        }

        .loading-state,
        .error-state,
        .empty-state {
          grid-column: 1 / -1;
          display: flex;
          flex-direction: column;
          align-items: center;
          justify-content: center;
          padding: 48px;
          color: var(--text-secondary);
        }

        .error-state {
          color: var(--color-danger);
        }
      `}</style>
    </div>
  );
};
