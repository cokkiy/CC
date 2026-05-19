# Phase 6 Frontend Architecture - Integration Design

## Executive Summary

This document outlines the integration plan for the Phase 6 Script Management System components into the CC Client application. The script components (`ScriptEditor`, `ScriptList`, `ScriptRunner`, `ScriptMarketplace`) are designed as self-contained, composable units that integrate via a **Context-based state layer** with **Tauri invoke** for backend communication.

---

## 1. App.tsx Layout Integration

### Proposed Tab-Based Layout

Add a new top-level tab `"scripts"` alongside existing tabs:

```
┌─────────────────────────────────────────────────────────────────┐
│  [Stations] [Groups] [Scripts] [Settings] [Messages]            │
├─────────────────────────────────────────────────────────────────┤
│                                                                 │
│  Scripts Tab Content Area                                       │
│  ┌─────────────────────────────────────────────────────────┐   │
│  │  ScriptList (full height, scrollable)                  │   │
│  │  - Search/filter bar                                    │   │
│  │  - Type filter pills                                    │   │
│  │  - Script card grid                                    │   │
│  │                                                         │   │
│  │  [ScriptEditor] → Modal overlay                         │   │
│  │  [ScriptRunner] → Modal overlay                         │   │
│  │  [ScriptMarketplace] → Modal overlay (Import/Export)    │   │
│  └─────────────────────────────────────────────────────────┘   │
│                                                                 │
└─────────────────────────────────────────────────────────────────┘
```

### Implementation in App.tsx

```tsx
// Add to existing state
const [activePage, setActivePage] = useState<"stations" | "settings" | "groups" | "messages" | "scripts">("stations");

// Add Scripts tab content
const renderScriptsPage = () => (
  <ScriptList
    scripts={scripts}
    onSelectScript={handleSelectScript}
    onEditScript={handleEditScript}
    onExecuteScript={handleExecuteScript}
    onDeleteScript={handleDeleteScript}
    onToggleFavorite={handleToggleFavorite}
    onDuplicateScript={handleDuplicateScript}
    onImport={() => setShowMarketplace(true)}
    onExport={(scripts) => handleExportScripts(scripts)}
    loading={scriptsLoading}
  />
);

// Render based on activePage
<div className="page-content">
  {activePage === 'scripts' && renderScriptsPage()}
  {/* other tabs */}
</div>
```

---

## 2. State Management Approach

### Layered Architecture

```
┌─────────────────────────────────────────────────────┐
│  UI Components (ScriptList, ScriptEditor, etc.)    │
├─────────────────────────────────────────────────────┤
│  Custom Hooks (useScripts, useScriptExecution)     │
├─────────────────────────────────────────────────────┤
│  ScriptContext (React Context - Client State)      │
├─────────────────────────────────────────────────────┤
│  Tauri API Layer (invoke wrappers)                  │
├─────────────────────────────────────────────────────┤
│  Rust Backend                                       │
└─────────────────────────────────────────────────────┘
```

### ScriptContext Design

```typescript
// src/plugin/script/ScriptContext.tsx

interface ScriptState {
  scripts: CommandScript[];
  selectedScript: CommandScript | null;
  isLoading: boolean;
  error: string | null;
  executionHistory: ScriptExecutionResult[];
}

interface ScriptContextValue extends ScriptState {
  // CRUD operations
  loadScripts: () => Promise<void>;
  saveScript: (script: Partial<CommandScript>) => Promise<CommandScript>;
  deleteScript: (scriptId: string) => Promise<void>;
  duplicateScript: (script: CommandScript) => Promise<CommandScript>;
  
  // Favorites
  toggleFavorite: (scriptId: string) => void;
  
  // Execution
  executeScript: (context: ScriptExecutionContext) => Promise<ScriptExecutionResult[]>;
  
  // Import/Export
  importScripts: (pkg: ScriptPackage) => Promise<ScriptImportResult>;
  exportScripts: (scripts: CommandScript[], metadata?: Partial<ScriptPackageMetadata>) => Promise<void>;
}

const ScriptContext = createContext<ScriptContextValue | null>(null);
```

### Custom Hooks

```typescript
// src/plugin/script/hooks.ts

export function useScripts() {
  const context = useContext(ScriptContext);
  if (!context) throw new Error('useScripts must be used within ScriptProvider');
  return context;
}

export function useScriptExecution() {
  const [isExecuting, setIsExecuting] = useState(false);
  const [results, setResults] = useState<ScriptExecutionResult[]>([]);
  const [logs, setLogs] = useState<ScriptLogEntry[]>([]);
  
  // Execution logic...
  
  return { isExecuting, results, logs, execute, cancel };
}
```

### Why This Approach?

| Approach | Pros | Cons |
|----------|------|------|
| **Context + Hooks (chosen)** | Shared state, testable, scoped | Slight boilerplate |
| Zustand/Jotai | Less boilerplate, good DX | Extra dependency |
| Redux | Familiar, time-travel | Overkill, verbose |
| Local state only | Simple | Can't share across components |

The Context approach is chosen because:
1. Script state is needed by multiple components (`ScriptList`, `ScriptEditor`, `ScriptRunner`)
2. Follows existing patterns in the codebase
3. No additional dependencies required
4. Easy to test and mock

---

## 3. Backend API Connection (Tauri invoke)

### API Surface (Rust Backend Commands)

```typescript
// src/plugin/script/api.ts

import { invoke } from '@tauri-apps/api/core';
import type {
  CommandScript,
  ScriptExecutionContext,
  ScriptExecutionResult,
  ScriptPackage,
  ScriptImportResult,
  ScriptPackageMetadata
} from './types';

// Script CRUD
async function load_scripts(): Promise<CommandScript[]> {
  return invoke('load_scripts');
}

async function save_script(script: Partial<CommandScript>): Promise<CommandScript> {
  return invoke('save_script', { script });
}

async function delete_script(script_id: string): Promise<void> {
  return invoke('delete_script', { script_id });
}

// Script Execution
async function execute_script(
  context: ScriptExecutionContext
): Promise<ScriptExecutionResult[]> {
  return invoke('execute_script', { context });
}

async function cancel_execution(execution_id: string): Promise<void> {
  return invoke('cancel_script_execution', { execution_id });
}

// Import/Export
async function import_script_package(
  pkg: ScriptPackage,
  options?: { overwrite?: boolean; import_as_copies?: boolean }
): Promise<ScriptImportResult> {
  return invoke('import_script_package', { pkg, options });
}

async function export_script_package(
  scripts: CommandScript[],
  metadata: Partial<ScriptPackageMetadata>
): Promise<ScriptPackage> {
  return invoke('export_script_package', { scripts, metadata });
}

// Station Discovery (for ScriptRunner target selection)
async function get_executable_targets(): Promise<Array<{id: string; name: string; status?: string}>> {
  return invoke('get_executable_targets');
}
```

### Error Handling Pattern

```typescript
// src/plugin/script/api.ts

async function load_scripts(): Promise<CommandScript[]> {
  try {
    return await invoke('load_scripts');
  } catch (error) {
    console.error('Failed to load scripts:', error);
    throw new ScriptAPIError('Failed to load scripts', error);
  }
}

class ScriptAPIError extends Error {
  constructor(message: string, public cause: unknown) {
    super(message);
    this.name = 'ScriptAPIError';
  }
}
```

---

## 4. Navigation Between Components

### Modal-Based Navigation

```
ScriptList (main view)
    │
    ├── [+ New Script] ──────────→ ScriptEditor (modal)
    │                                    │
    │                              [Save/Cancel]
    │                                    │
    ├── [▶ Execute] ──────────────→ ScriptRunner (modal)
    │    onExecuteScript                     │
    │                                   [Execute/Cancel]
    │                                        │
    ├── [✎ Edit] ──────────────────→ ScriptEditor (modal, pre-filled)
    │    onEditScript                         │
    │                                   [Save/Cancel]
    │                                        │
    ├── [📥 Import] ────────────────→ ScriptMarketplace (modal, Import tab)
    │                                        │
    ├── [📤 Export] ────────────────→ ScriptMarketplace (modal, Export tab)
    │                                        │
    └── [📋 Duplicate] ──────────────→ ScriptEditor (modal, pre-filled copy)
```

### Implementation with State Machine

```typescript
// Component visibility state
interface ScriptViewState {
  mode: 'list' | 'editor' | 'runner' | 'marketplace';
  editorScript: CommandScript | null;  // null = new script
  marketplaceTab: 'import' | 'export';
}

const [viewState, setViewState] = useState<ScriptViewState>({
  mode: 'list',
  editorScript: null,
  marketplaceTab: 'import'
});

// Navigation actions
const handleNewScript = () => setViewState({ mode: 'editor', editorScript: null, marketplaceTab: 'import' });
const handleEditScript = (script: CommandScript) => setViewState({ mode: 'editor', editorScript: script, marketplaceTab: 'import' });
const handleExecuteScript = (script: CommandScript) => setViewState({ mode: 'runner', editorScript: script, marketplaceTab: 'import' });
const handleImportExport = (tab: 'import' | 'export') => setViewState({ mode: 'marketplace', editorScript: null, marketplaceTab: tab });
const handleCloseModal = () => setViewState({ mode: 'list', editorScript: null, marketplaceTab: 'import' });
```

---

## 5. Mock Data for Development/Testing

### Mock Data File

```typescript
// src/plugin/script/mockData.ts

import type { 
  CommandScript, 
  ScriptPackage, 
  ScriptPackageMetadata,
  ScriptExecutionResult,
  ScriptExecutionStatus
} from './types';

export const MOCK_SCRIPTS: CommandScript[] = [
  {
    id: 'script-1',
    name: 'System Health Check',
    description: 'Checks CPU, memory, and disk usage on target stations',
    scriptType: 'shell',
    content: `#!/bin/bash
echo "=== System Health Check ==="
echo "CPU: {{cpu_threshold}}%"
echo "Memory: {{memory_threshold}}%"
# Health check logic...
`,
    parameters: [
      { name: 'cpu_threshold', paramType: 'number', defaultValue: '80', required: true, description: 'CPU threshold %' },
      { name: 'memory_threshold', paramType: 'number', defaultValue: '90', required: true, description: 'Memory threshold %' },
    ],
    isTemplate: true,
    isFavorite: true,
    tags: ['health', 'monitoring', 'system'],
    createdBy: 'admin',
    createdAt: '2024-01-15T10:00:00Z',
    updatedAt: '2024-01-20T14:30:00Z',
    version: 2,
    usageCount: 45,
    lastUsedAt: '2024-01-25T09:00:00Z',
  },
  {
    id: 'script-2',
    name: 'Deploy Update',
    description: 'Deploys software updates to target stations',
    scriptType: 'powershell',
    content: `param(
  [string]$UpdatePath,
  [bool]$ForceRestart = false
)

Write-Host "Deploying update from: $UpdatePath"
# Deployment logic...
`,
    parameters: [
      { name: 'UpdatePath', paramType: 'string', defaultValue: '', required: true, description: 'Path to update package' },
      { name: 'ForceRestart', paramType: 'boolean', defaultValue: 'false', required: false, description: 'Force restart after update' },
    ],
    isTemplate: false,
    isFavorite: false,
    tags: ['deployment', 'update', 'windows'],
    createdBy: 'devops',
    createdAt: '2024-01-10T08:00:00Z',
    updatedAt: '2024-01-18T16:45:00Z',
    version: 3,
    usageCount: 12,
  },
  {
    id: 'script-3',
    name: 'Network Diagnostics',
    description: 'Runs network connectivity and speed tests',
    scriptType: 'python',
    content: `#!/usr/bin/env python3
import speedtest
import json

def run_diagnostics():
    print("Running network diagnostics...")
    # Diagnostic logic...
`,
    parameters: [],
    isTemplate: false,
    isFavorite: true,
    tags: ['network', 'diagnostics'],
    createdBy: 'network-team',
    createdAt: '2024-01-05T12:00:00Z',
    updatedAt: '2024-01-22T11:20:00Z',
    version: 1,
    usageCount: 8,
  },
];

export const MOCK_TARGETS = [
  { id: 'station-1', name: 'Workstation-01', status: 'online' },
  { id: 'station-2', name: 'Workstation-02', status: 'online' },
  { id: 'station-3', name: 'Server-01', status: 'online' },
  { id: 'station-4', name: 'Server-02', status: 'offline' },
  { id: 'station-5', name: 'Test-PC', status: 'warning' },
];

export const MOCK_PACKAGE: ScriptPackage = {
  metadata: {
    id: 'pkg-1',
    name: 'IT Administration Toolkit',
    version: '1.0.0',
    description: 'Essential scripts for IT administrators',
    author: 'CC Team',
    tags: ['admin', 'toolkit', 'essentials'],
    createdAt: '2024-01-01T00:00:00Z',
    updatedAt: '2024-01-20T00:00:00Z',
    categories: ['system', 'automation'],
    downloadCount: 150,
    rating: 4.5,
  },
  scripts: [MOCK_SCRIPTS[0], MOCK_SCRIPTS[2]],
};

export function createMockExecutionResult(
  status: ScriptExecutionStatus = 'success',
  stationId = 'station-1'
): ScriptExecutionResult {
  return {
    scriptId: 'script-1',
    status,
    stdout: status === 'success' ? 'Health check completed successfully' : '',
    stderr: status === 'failed' ? 'Error: Connection timeout' : '',
    exitCode: status === 'success' ? 0 : 1,
    startedAt: new Date().toISOString(),
    completedAt: status !== 'running' ? new Date().toISOString() : undefined,
    durationMs: status === 'running' ? undefined : 2500,
    error: status === 'failed' ? 'Connection timeout after 30s' : undefined,
  };
}
```

### Mock API Implementation

```typescript
// src/plugin/script/mockApi.ts

import { MOCK_SCRIPTS, MOCK_TARGETS, MOCK_PACKAGE, createMockExecutionResult } from './mockData';
import type { CommandScript, ScriptPackage, ScriptExecutionContext } from './types';

// Simulated delay for realistic async behavior
const delay = (ms: number) => new Promise(resolve => setTimeout(resolve, ms));

// In-memory store (simulates backend)
let scriptsStore = [...MOCK_SCRIPTS];

export const mockApi = {
  async load_scripts() {
    await delay(300);
    return [...scriptsStore];
  },

  async save_script(script: Partial<CommandScript>) {
    await delay(200);
    const now = new Date().toISOString();
    if (script.id) {
      // Update existing
      const index = scriptsStore.findIndex(s => s.id === script.id);
      if (index >= 0) {
        scriptsStore[index] = { ...scriptsStore[index], ...script, updatedAt: now } as CommandScript;
        return scriptsStore[index];
      }
    }
    // Create new
    const newScript: CommandScript = {
      id: `script-${Date.now()}`,
      name: script.name || 'Untitled Script',
      description: script.description || '',
      scriptType: script.scriptType || 'shell',
      content: script.content || '',
      parameters: script.parameters || [],
      isTemplate: script.isTemplate || false,
      isFavorite: false,
      tags: script.tags || [],
      createdBy: 'mock-user',
      createdAt: now,
      updatedAt: now,
      version: 1,
      usageCount: 0,
    };
    scriptsStore.push(newScript);
    return newScript;
  },

  async delete_script(scriptId: string) {
    await delay(150);
    scriptsStore = scriptsStore.filter(s => s.id !== scriptId);
  },

  async execute_script(context: ScriptExecutionContext) {
    await delay(1500);
    // Return mock results for each target
    return context.stationIds.map(stationId => createMockExecutionResult('success', stationId));
  },

  async get_executable_targets() {
    await delay(100);
    return [...MOCK_TARGETS];
  },

  async import_script_package(pkg: ScriptPackage) {
    await delay(500);
    return {
      success: true,
      imported: pkg.scripts.length,
      skipped: 0,
      errors: [],
      warnings: [],
    };
  },

  async export_script_package(scripts: CommandScript[]) {
    await delay(300);
    return { ...MOCK_PACKAGE, scripts };
  },
};
```

### Using Mock vs Real API

```typescript
// src/plugin/script/apiFactory.ts

import { invoke } from '@tauri-apps/api/core';
import { mockApi } from './mockApi';
import type { CommandScript, ScriptPackage } from './types';

// Toggle for development
const USE_MOCK_API = import.meta.env.DEV && import.meta.env.VITE_USE_MOCK_API === 'true';

export const scriptApi = USE_MOCK_API ? {
  load_scripts: () => mockApi.load_scripts(),
  save_script: (s: Partial<CommandScript>) => mockApi.save_script(s),
  delete_script: (id: string) => mockApi.delete_script(id),
  execute_script: (ctx: any) => mockApi.execute_script(ctx),
  get_executable_targets: () => mockApi.get_executable_targets(),
  import_script_package: (pkg: ScriptPackage) => mockApi.import_script_package(pkg),
  export_script_package: (scripts: CommandScript[]) => mockApi.export_script_package(scripts),
} : {
  load_scripts: () => invoke('load_scripts'),
  save_script: (s: Partial<CommandScript>) => invoke('save_script', { script: s }),
  delete_script: (id: string) => invoke('delete_script', { script_id: id }),
  execute_script: (ctx: any) => invoke('execute_script', { context: ctx }),
  get_executable_targets: () => invoke('get_executable_targets'),
  import_script_package: (pkg: ScriptPackage) => invoke('import_script_package', { pkg }),
  export_script_package: (scripts: CommandScript[]) => invoke('export_script_package', { scripts }),
};
```

---

## 6. File Structure After Integration

```
src/
├── App.tsx                          # Main app with tab routing
├── main.tsx                         # Entry point
├── types.ts                         # Shared types
│
├── plugin/
│   ├── index.ts                     # Plugin exports
│   ├── script/
│   │   ├── index.ts                 # Component exports
│   │   ├── types.ts                 # Script-specific types
│   │   ├── ScriptEditor.tsx         # Existing
│   │   ├── ScriptList.tsx           # Existing
│   │   ├── ScriptRunner.tsx         # Existing
│   │   ├── ScriptMarketplace.tsx    # Existing
│   │   │
│   │   ├── ScriptContext.tsx        # NEW: State management
│   │   ├── hooks.ts                 # NEW: Custom hooks
│   │   ├── api.ts                   # NEW: Tauri invoke wrappers
│   │   ├── apiFactory.ts            # NEW: Mock/Real API toggle
│   │   ├── mockData.ts              # NEW: Mock data
│   │   ├── mockApi.ts               # NEW: Mock API implementation
│   │   └── docs/
│   │       └── PHASE6_INTEGRATION_DESIGN.md
│   │
│   └── components/
│       └── ...
│
└── docs/
    └── PHASE6_INTEGRATION_DESIGN.md
```

---

## 7. Implementation Priority

1. **Phase 1: Foundation**
   - Create `api.ts` with Tauri invoke wrappers
   - Create `mockApi.ts` and `mockData.ts` for development
   - Create `ScriptContext.tsx`

2. **Phase 2: Core Integration**
   - Add "scripts" tab to App.tsx
   - Integrate `ScriptList` as main view
   - Wire up ScriptContext

3. **Phase 3: Editor Integration**
   - Modal-based ScriptEditor
   - Create/Edit/Duplicate flows

4. **Phase 4: Runner Integration**
   - Modal-based ScriptRunner
   - Target selection with stations data
   - Execution with real backend

5. **Phase 5: Marketplace Integration**
   - Import/Export modals
   - Package handling

6. **Phase 6: Polish**
   - Error handling
   - Loading states
   - Empty states
   - Animations/transitions

---

## 8. Key Design Decisions Summary

| Question | Decision |
|----------|----------|
| **Layout** | Tab-based navigation, ScriptList as main view |
| **State Management** | React Context + Custom Hooks |
| **Backend Communication** | Tauri invoke with mock fallback |
| **Navigation** | Modal overlays for Editor/Runner/Marketplace |
| **Mock Data** | Complete mock dataset with toggleable API |
| **Component Props** | Keep existing interfaces, extend with callbacks |
