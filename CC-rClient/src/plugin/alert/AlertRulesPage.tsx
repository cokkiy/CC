/**
 * AlertRulesPage - Main container for Alert Rules Management UI
 * Part of Phase 9: Alert Rules Management System
 */

import React from 'react';
import { useAlerts } from './AlertContext';
import { useAlertsUI } from './AlertUIContext';
import { AlertRuleList, AlertRuleEditor, AlertRuleRunner, AlertHistoryPanel, AlertTemplates } from './index';
import type { AlertRule, AlertExecutionContext, AlertRulePackage, AlertRuleImportResult } from './types';
import type { Station } from '../../types';

export interface AlertRulesPageProps {
  stations: Station[];
}

export const AlertRulesPage: React.FC<AlertRulesPageProps> = ({ stations }) => {
  const {
    rules,
    saveRule,
    deleteRule,
    duplicateRule,
    toggleFavorite,
    toggleRuleStatus,
    executeRule,
    testRule,
    importRules,
    exportRules,
  } = useAlerts();

  const {
    editor,
    runner,
    history,
    importExport,
    openEditor,
    closeEditor,
    openRunner,
    closeRunner,
    openHistory,
    closeHistory,
    openImportExport,
    closeImportExport,
  } = useAlertsUI();

  // Convert stations to targets for AlertRuleRunner
  const targets = stations.map((s) => ({
    id: s.id,
    name: s.name,
    status: s.blocked ? 'blocked' : 'ready',
    ips: s.networkInterfaces.flatMap((network) => network.ips).filter(Boolean),
    groups: s.groups,
    tags: s.tags,
  }));

  // AlertRuleList handlers
  const handleEditRule = (rule: AlertRule | undefined) => {
    openEditor(rule ?? null);
  };

  const handleExecuteRule = (rule: AlertRule) => {
    openRunner(rule);
  };

  const handleDeleteRule = async (ruleId: string) => {
    await deleteRule(ruleId);
  };

  const handleToggleFavorite = async (ruleId: string) => {
    await toggleFavorite(ruleId);
  };

  const handleToggleStatus = async (ruleId: string) => {
    await toggleRuleStatus(ruleId);
  };

  const handleDuplicateRule = async (rule: AlertRule) => {
    await duplicateRule(rule.id);
  };

  // AlertRuleEditor handlers
  const handleSaveRule = async (rule: Partial<AlertRule>) => {
    await saveRule(rule);
    closeEditor();
  };

  const handleCancelEdit = () => {
    closeEditor();
  };

  // AlertRuleRunner handlers
  const handleRunAlert = async (context: AlertExecutionContext) => {
    return executeRule(context);
  };

  const handleTestAlert = async (ruleId: string, targetIds: string[]) => {
    await testRule(ruleId, targetIds);
  };

  const handleCancelRun = () => {
    closeRunner();
  };

  return (
    <div className="scripts-page-shell">
      <main className="grid gridScriptsMode scripts-main-grid">
        <section className="panel scripts-main-panel">
          <AlertRuleList
            rules={rules}
            onEditRule={handleEditRule}
            onExecuteRule={handleExecuteRule}
            onDeleteRule={handleDeleteRule}
            onToggleFavorite={handleToggleFavorite}
            onToggleStatus={handleToggleStatus}
            onDuplicateRule={handleDuplicateRule}
            onViewHistory={openHistory}
            onImport={openImportExport}
          />
        </section>
      </main>

      {/* Editor Drawer - slides in from right */}
      {editor.open && (
        <div className="scripts-layer scripts-layer-drawer" role="dialog" aria-modal="true">
          <div className="scripts-drawer-panel">
            <AlertRuleEditor
              rule={editor.rule ?? undefined}
              targets={targets}
              onSave={handleSaveRule}
              onCancel={handleCancelEdit}
              isNew={editor.isNew}
            />
          </div>
        </div>
      )}

      {/* Runner Modal - centered overlay */}
      {runner.open && runner.rule && (
        <div className="scripts-layer" role="dialog" aria-modal="true">
          <div className="scripts-modal-panel">
            <AlertRuleRunner
              rule={runner.rule}
              targets={targets}
              onExecute={handleRunAlert}
              onTest={handleTestAlert}
              onCancel={handleCancelRun}
            />
          </div>
        </div>
      )}

      {/* History Panel - bottom panel */}
      {history.open && (
        <AlertHistoryPanel
          onClose={closeHistory}
          filterRuleId={history.filterRuleId}
        />
      )}

      {importExport.open && (
        <AlertTemplates
          onImport={(rule) => {
            void saveRule(rule);
            closeImportExport();
          }}
          onClose={closeImportExport}
        />
      )}

      <style>{`
        .scripts-page-shell {
          position: relative;
          min-height: calc(100vh - 64px - 48px);
        }

        .scripts-main-grid {
          height: 100%;
        }

        .scripts-main-panel {
          padding: 0;
          overflow: hidden;
          min-height: calc(100vh - 64px - 48px);
        }

        .scripts-layer {
          position: fixed;
          inset: 0;
          background: rgba(11, 25, 44, 0.42);
          backdrop-filter: blur(2px);
          display: flex;
          align-items: center;
          justify-content: center;
          z-index: 1200;
          padding: 24px;
        }

        .scripts-layer-drawer {
          justify-content: flex-end;
          padding: 0;
        }

        .scripts-drawer-panel {
          width: min(980px, 94vw);
          height: 100vh;
          background: var(--bg-card);
          border-left: 1px solid var(--border-color);
          box-shadow: -18px 0 48px rgba(8, 20, 38, 0.24);
        }

        .scripts-modal-panel {
          width: min(1320px, 96vw);
          height: min(90vh, 920px);
          background: var(--bg-card);
          border: 1px solid var(--border-color);
          border-radius: 14px;
          box-shadow: 0 22px 56px rgba(8, 20, 38, 0.24);
          overflow: hidden;
        }

        @media (max-width: 1024px) {
          .scripts-layer {
            padding: 12px;
          }

          .scripts-layer-drawer {
            padding: 0;
          }

          .scripts-drawer-panel {
            width: 100vw;
          }

          .scripts-modal-panel {
            width: 100%;
            height: 100%;
            border-radius: 0;
          }
        }
      `}</style>
    </div>
  );
};
