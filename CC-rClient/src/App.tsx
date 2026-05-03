import { invoke } from "@tauri-apps/api/core";
import { listen, type UnlistenFn } from "@tauri-apps/api/event";
import { useEffect, useMemo, useRef, useState } from "react";
import {
  LineChart,
  Line,
  XAxis,
  YAxis,
  CartesianGrid,
  Tooltip,
  ResponsiveContainer,
  Legend,
} from "recharts";
import topBanner from "../images/top_banner.png";
import topBannerSmall from "../images/top_banner_small.png";
import type {
  AppSnapshot,
  BatchCapture,
  ClientOptions,
  CommandExecutionResult,
  PersistedState,
  RemoteFileBrowserResult,
  StartProgram,
  Station,
  StationGroup,
  StationRuntimeSnapshot,
  StationScreenCapture
} from "./types";
import { ScriptProvider, ScriptsPage, ScriptsUIProvider } from "./plugin/script";
import { AlertProvider, AlertRulesPage, AlertUIProvider } from "./plugin/alert";
import { BatchProvider, BatchTaskRunner, useBatch } from "./plugin/batch";
import { BatchPage } from "./plugin/batch/BatchPage";
import { BatchUIProvider, useBatchUI } from "./plugin/batch/BatchUIContext";
import { GroupsPage, TagsPage } from "./plugin/groups";
import {
  TelemetryProfilesEditor,
  SUPPORTED_TELEMETRY_INCLUDES,
  validateEnabledTelemetrySectionConflicts,
  validateTelemetryProfileDrafts,
  type TelemetryProfileDraft,
  type TelemetrySchemaResponse,
} from "./plugin/telemetry";
import { GroupsProvider } from "./plugin/groups/GroupsContext";
import { TagsProvider } from "./plugin/groups/TagsContext";
import type { TagDefinition } from "./plugin/groups/types";
import {
  buildRuntimeFromMqttTelemetry,
  runtimeHasFullDetails,
  type MqttTelemetryBundle,
} from "./runtime-telemetry";
import {
  deriveTagValueOptions,
  filterStations,
  getPrimaryIp,
  getStationGroupNames,
  getStationRuntimeSummary,
  getStationStatusLabel,
  getStationTagSummary,
  getTagDefinitionKey,
  getTagDefinitionLabel,
  type StationSortDirection,
  type StationSortBy,
  type StationViewMode,
} from "./stations-browser";

const STATION_VIEW_MODE_STORAGE_KEY = "cc-rclient.station-view-mode";
const STATION_LIST_COLUMNS_STORAGE_KEY = "cc-rclient.station-list-columns";
const STATION_GRID_COLUMNS_STORAGE_KEY = "cc-rclient.station-grid-columns";
const STATION_PANEL_WIDTH_STORAGE_KEY = "cc-rclient.station-panel-width";

const MIN_BROWSER_PANEL_WIDTH = 320;
const MIN_DETAIL_PANEL_WIDTH = 360;
const STATIONS_PANEL_RESIZER_WIDTH = 10;
const STATIONS_STACK_BREAKPOINT = 1180;

const emptyOptions: ClientOptions = {
  interval: 2,
  isFirstTimeRun: true,
  startApps: [],
  monitorProcesses: [],
  defaultTelemetryProfiles: [],
};

const emptyStation = (): Station => ({
  id: crypto.randomUUID(),
  name: "New Device",
  blocked: false,
  networkInterfaces: [{ mac: "", ips: [""] }],
  startPrograms: [],
  monitorProcesses: [],
  lastAction: null,
  groups: [],
  tags: {},
  metadata: {}
});

const MQTT_DISCOVERY_SOURCE = "mqtt";
const LOCAL_STATION_ID = "local-rstationservice";

type MqttTelemetryEventPayload = {
  station_id: string;
  data: MqttTelemetryBundle;
};

type MqttStatusEventPayload = {
  station_id: string;
  status: {
    station_id: string;
    online: boolean;
    last_seen: number;
    version?: string | null;
    alert?: string | null;
  };
};

type RemoteStationIdentity = {
  endpoint: string;
  stationId: string;
  computerName: string;
};

function createDiscoveredStation(stationId: string, metadata?: Record<string, string>): Station {
  return {
    id: stationId,
    name: stationId,
    blocked: false,
    networkInterfaces: [],
    startPrograms: [],
    monitorProcesses: [],
    lastAction: "Discovered via MQTT",
    groups: [],
    tags: {},
    metadata: {
      source: MQTT_DISCOVERY_SOURCE,
      ...metadata,
    },
  };
}

function mergeStringArrays(left: string[] = [], right: string[] = []) {
  return Array.from(new Set([...left, ...right]));
}

function mergeNetworkInterfaces(
  left: Station["networkInterfaces"] = [],
  right: Station["networkInterfaces"] = [],
) {
  const byMac = new Map<string, Station["networkInterfaces"][number]>();

  [...left, ...right].forEach((item) => {
    const key = item.mac || `ips:${item.ips.join(",")}`;
    const existing = byMac.get(key);
    if (!existing) {
      byMac.set(key, { ...item, ips: [...item.ips] });
      return;
    }

    byMac.set(key, {
      ...existing,
      ips: mergeStringArrays(existing.ips, item.ips),
    });
  });

  return Array.from(byMac.values());
}

function reconcileStationIdentity(
  current: Station[],
  fromId: string,
  toId: string,
  computerName?: string,
) {
  if (!toId || fromId === toId) {
    return current;
  }

  const source = current.find((station) => station.id === fromId);
  if (!source) {
    return current;
  }

  const target = current.find((station) => station.id === toId);
  if (!target) {
    return current.map((station) =>
      station.id === fromId
        ? {
            ...station,
            id: toId,
            name: station.name || computerName || toId,
            metadata: {
              ...station.metadata,
              mqttCanonicalId: toId,
            },
          }
        : station,
    );
  }

  const merged: Station = {
    ...target,
    id: toId,
    name:
      source.name === "Local CC-rStationService"
        ? source.name
        : target.name || source.name || computerName || toId,
    blocked: target.blocked || source.blocked,
    networkInterfaces: mergeNetworkInterfaces(source.networkInterfaces, target.networkInterfaces),
    startPrograms: target.startPrograms.length > 0 ? target.startPrograms : source.startPrograms,
    monitorProcesses:
      target.monitorProcesses.length > 0 ? target.monitorProcesses : source.monitorProcesses,
    lastAction: target.lastAction ?? source.lastAction,
    groups: mergeStringArrays(source.groups, target.groups),
    tags: { ...source.tags, ...target.tags },
    metadata: {
      ...source.metadata,
      ...target.metadata,
      mqttCanonicalId: toId,
    },
    location: target.location ?? source.location,
  };

  return current
    .filter((station) => station.id !== fromId && station.id !== toId)
    .concat(merged);
}

function remapRecordKey<T>(current: Record<string, T>, fromId: string, toId: string) {
  if (fromId === toId || !(fromId in current)) {
    return current;
  }

  const next = { ...current };
  const value = next[fromId];
  delete next[fromId];
  next[toId] = value;
  return next;
}

function emitAlertRuntimeUpdate(station: Station, runtime: StationRuntimeSnapshot) {
  if (typeof window === "undefined") {
    return;
  }

  window.dispatchEvent(
    new CustomEvent("cc-alert-runtime", {
      detail: {
        station,
        runtime,
      },
    }),
  );
}

function formatRuntimeMemory(runtime: StationRuntimeSnapshot) {
  if (runtime.totalMemory > 0) {
    return `${formatBytes(runtime.currentMemory)} / ${formatBytes(runtime.totalMemory)}`;
  }

  return `${formatBytes(runtime.currentMemory)} used`;
}

function getRuntimeUnavailableText(
  runtime: StationRuntimeSnapshot,
  subject: string,
) {
  if (runtimeHasFullDetails(runtime)) {
    return "n/a";
  }

  return `${subject} unavailable in MQTT-only mode`;
}

function CpuPie({ cpu }: { cpu: number }) {
  const pct = Math.min(Math.max(cpu, 0), 100);
  const r = 40;
  const cx = 56;
  const cy = 56;
  const circumference = 2 * Math.PI * r;
  const used = (pct / 100) * circumference;
  const free = circumference - used;

  const color =
    pct >= 90 ? "#d64545" :
    pct >= 70 ? "#c78a00" :
    "#1f9d55";

  return (
    <div className="cpuPie">
      <svg viewBox="0 0 112 112" width="112" height="112" aria-label={`CPU ${pct.toFixed(1)}%`}>
        {/* track */}
        <circle cx={cx} cy={cy} r={r} fill="none" stroke="var(--cpuPie-track)" strokeWidth="16" />
        {/* used arc — starts at top (−90°) */}
        <circle
          cx={cx} cy={cy} r={r}
          fill="none"
          stroke={color}
          strokeWidth="16"
          strokeDasharray={`${used} ${free}`}
          strokeLinecap="round"
          transform={`rotate(-90 ${cx} ${cy})`}
          style={{ transition: "stroke-dasharray 0.5s ease, stroke 0.5s ease" }}
        />
        {/* centre label */}
        <text x={cx} y={cy - 6} textAnchor="middle" className="cpuPie-value">{pct.toFixed(1)}%</text>
        <text x={cx} y={cy + 12} textAnchor="middle" className="cpuPie-label">CPU</text>
      </svg>
    </div>
  );
}

function formatBytes(value: number) {
  if (!Number.isFinite(value) || value <= 0) {
    return "0 B";
  }
  const units = ["B", "KB", "MB", "GB", "TB"];
  let next = value;
  let index = 0;
  while (next >= 1024 && index < units.length - 1) {
    next /= 1024;
    index += 1;
  }
  return `${next.toFixed(index === 0 ? 0 : 1)} ${units[index]}`;
}

function clampBrowserColumns(value: number, fallback: number) {
  if (!Number.isFinite(value)) {
    return fallback;
  }

  return Math.min(5, Math.max(1, Math.round(value)));
}

function readStoredViewMode(): StationViewMode {
  if (typeof window === "undefined") {
    return "list";
  }

  const stored = window.localStorage.getItem(STATION_VIEW_MODE_STORAGE_KEY);
  if (stored === "list" || stored === "grid" || stored === "split") {
    return stored;
  }

  return "list";
}

function readStoredColumns(storageKey: string, fallback: number) {
  if (typeof window === "undefined") {
    return fallback;
  }

  const stored = window.localStorage.getItem(storageKey);
  if (!stored) {
    return fallback;
  }

  return clampBrowserColumns(Number(stored), fallback);
}

function readStoredPanelWidth() {
  if (typeof window === "undefined") {
    return null;
  }

  const stored = window.localStorage.getItem(STATION_PANEL_WIDTH_STORAGE_KEY);
  if (!stored) {
    return null;
  }

  const parsed = Number(stored);
  return Number.isFinite(parsed) && parsed > 0 ? Math.round(parsed) : null;
}

type StationVisualState = "ready" | "warning" | "error" | "offline";

function getStationVisualState(
  station: Station,
  runtime: StationRuntimeSnapshot | null | undefined
): StationVisualState {
  if (!runtime) {
    return "offline";
  }

  const runtimeMessage = runtime.message.toLowerCase();
  const hasErrorSignal =
    runtimeMessage.includes("error") ||
    runtimeMessage.includes("failed") ||
    runtimeMessage.includes("unavailable") ||
    runtimeMessage.includes("timeout");

  if (hasErrorSignal) {
    return "error";
  }

  const hasWarningSignal =
    station.blocked || runtime.appStates.some((app) => !app.isRunning);

  if (hasWarningSignal) {
    return "warning";
  }

  return "ready";
}

function ComputerStatusIcon({ state }: { state: StationVisualState }) {
  return (
    <span className={`computerIcon computerIcon--${state}`} aria-hidden="true">
      <svg viewBox="0 0 24 24" focusable="false">
        <rect x="3.5" y="4.5" width="17" height="11" rx="2" />
        <path d="M9 19h6" />
        <path d="M12 15.5V19" />
        <path d="M7 19h10" />
      </svg>
    </span>
  );
}

function CpuIcon() {
  return (
    <svg className="stationStatIconSvg" viewBox="0 0 16 16" fill="currentColor">
      <rect x="3" y="3" width="10" height="10" rx="1" fill="none" stroke="currentColor" strokeWidth="1.2" />
      <rect x="5.5" y="5.5" width="5" height="5" rx="0.5" />
      <line x1="1" y1="5" x2="3" y2="5" stroke="currentColor" strokeWidth="1" />
      <line x1="1" y1="8" x2="3" y2="8" stroke="currentColor" strokeWidth="1" />
      <line x1="1" y1="11" x2="3" y2="11" stroke="currentColor" strokeWidth="1" />
      <line x1="13" y1="5" x2="15" y2="5" stroke="currentColor" strokeWidth="1" />
      <line x1="13" y1="8" x2="15" y2="8" stroke="currentColor" strokeWidth="1" />
      <line x1="13" y1="11" x2="15" y2="11" stroke="currentColor" strokeWidth="1" />
      <line x1="5" y1="1" x2="5" y2="3" stroke="currentColor" strokeWidth="1" />
      <line x1="8" y1="1" x2="8" y2="3" stroke="currentColor" strokeWidth="1" />
      <line x1="11" y1="1" x2="11" y2="3" stroke="currentColor" strokeWidth="1" />
      <line x1="5" y1="13" x2="5" y2="15" stroke="currentColor" strokeWidth="1" />
      <line x1="8" y1="13" x2="8" y2="15" stroke="currentColor" strokeWidth="1" />
      <line x1="11" y1="13" x2="11" y2="15" stroke="currentColor" strokeWidth="1" />
    </svg>
  );
}

function MemoryIcon() {
  return (
    <svg className="stationStatIconSvg" viewBox="0 0 16 16" fill="currentColor">
      <rect x="1" y="5" width="14" height="6" rx="0.5" fill="none" stroke="currentColor" strokeWidth="1.1" />
      <rect x="2.5" y="6.5" width="1.5" height="3" rx="0.2" />
      <rect x="5" y="6.5" width="1.5" height="3" rx="0.2" />
      <rect x="7.5" y="6.5" width="1.5" height="3" rx="0.2" />
      <rect x="10" y="6.5" width="1.5" height="3" rx="0.2" />
      <rect x="12.5" y="6.5" width="1.5" height="3" rx="0.2" />
      <line x1="2.5" y1="11" x2="2.5" y2="13" stroke="currentColor" strokeWidth="0.8" />
      <line x1="5" y1="11" x2="5" y2="13" stroke="currentColor" strokeWidth="0.8" />
      <line x1="7.5" y1="11" x2="7.5" y2="13" stroke="currentColor" strokeWidth="0.8" />
      <line x1="10" y1="11" x2="10" y2="13" stroke="currentColor" strokeWidth="0.8" />
      <line x1="12.5" y1="11" x2="12.5" y2="13" stroke="currentColor" strokeWidth="0.8" />
    </svg>
  );
}

function ProcessIcon() {
  return (
    <svg className="stationStatIconSvg" viewBox="0 0 16 16" fill="currentColor">
      <rect x="1" y="2" width="14" height="3.5" rx="0.5" fill="none" stroke="currentColor" strokeWidth="1.1" />
      <rect x="1" y="6.2" width="14" height="3.5" rx="0.5" fill="none" stroke="currentColor" strokeWidth="1.1" />
      <rect x="1" y="10.4" width="14" height="3.5" rx="0.5" fill="none" stroke="currentColor" strokeWidth="1.1" />
      <circle cx="2.8" cy="3.75" r="0.7" />
      <circle cx="2.8" cy="7.95" r="0.7" />
      <circle cx="2.8" cy="12.15" r="0.7" />
      <line x1="4.5" y1="3.75" x2="12" y2="3.75" stroke="currentColor" strokeWidth="0.8" strokeLinecap="round" />
      <line x1="4.5" y1="7.95" x2="12" y2="7.95" stroke="currentColor" strokeWidth="0.8" strokeLinecap="round" />
      <line x1="4.5" y1="12.15" x2="9" y2="12.15" stroke="currentColor" strokeWidth="0.8" strokeLinecap="round" />
    </svg>
  );
}

type NetworkDataPoint = {
  ts: number;
  rxbps: number;
  txbps: number;
};

type ChartDataPoint = { ts: number; cpu: number; memory: number; memPct: number };
type ProcessChartDataPoint = { ts: number; procCount: number };

function PerformanceCharts({
  history,
  totalMemory,
}: {
  history: { cpu: number; memory: number; ts: number }[];
  totalMemory: number;
}) {
  const data: ChartDataPoint[] = history.map((h) => ({
    ts: h.ts,
    cpu: h.cpu,
    memory: h.memory,
    memPct: totalMemory > 0 ? (h.memory / totalMemory) * 100 : 0,
  }));

  if (data.length < 2) {
    return <p className="emptyInline">Collecting performance data…</p>;
  }

  return (
    <div style={{ display: "flex", gap: "1rem", flexWrap: "wrap" }}>
      <div style={{ flex: "1 1 240px", minWidth: "240px" }}>
        <p style={{ margin: "0 0 0.25rem", fontWeight: 600, fontSize: "0.8rem" }}>CPU %</p>
        <ResponsiveContainer width="100%" height={100}>
          <LineChart data={data} margin={{ top: 2, right: 4, left: -20, bottom: 0 }}>
            <CartesianGrid strokeDasharray="3 3" stroke="var(--cpuPie-track)" />
            <XAxis dataKey="ts" tickFormatter={(v) => new Date(v).toLocaleTimeString()} tick={{ fontSize: 9 }} />
            <YAxis domain={[0, 100]} tick={{ fontSize: 9 }} tickFormatter={(v) => `${v}%`} />
            <Tooltip
              formatter={(v) => [`${Number(v).toFixed(1)}%`, "CPU"]}
              labelFormatter={(v) => new Date(Number(v)).toLocaleTimeString()}
            />
            <Line
              type="monotone"
              dataKey="cpu"
              stroke="#d64545"
              strokeWidth={1.5}
              dot={false}
              name="CPU %"
              isAnimationActive={false}
            />
          </LineChart>
        </ResponsiveContainer>
      </div>
      <div style={{ flex: "1 1 240px", minWidth: "240px" }}>
        <p style={{ margin: "0 0 0.25rem", fontWeight: 600, fontSize: "0.8rem" }}>Memory %</p>
        <ResponsiveContainer width="100%" height={100}>
          <LineChart data={data} margin={{ top: 2, right: 4, left: -20, bottom: 0 }}>
            <CartesianGrid strokeDasharray="3 3" stroke="var(--cpuPie-track)" />
            <XAxis dataKey="ts" tickFormatter={(v) => new Date(v).toLocaleTimeString()} tick={{ fontSize: 9 }} />
            <YAxis domain={[0, 100]} tick={{ fontSize: 9 }} tickFormatter={(v) => `${v}%`} />
            <Tooltip
              formatter={(v) => [`${Number(v).toFixed(1)}%`, "Memory"]}
              labelFormatter={(v) => new Date(Number(v)).toLocaleTimeString()}
            />
            <Line
              type="monotone"
              dataKey="memPct"
              stroke="#c78a00"
              strokeWidth={1.5}
              dot={false}
              name="Memory %"
              isAnimationActive={false}
            />
          </LineChart>
        </ResponsiveContainer>
      </div>
    </div>
  );
}

function NetworkTrafficChart({
  history,
}: {
  history: { ts: number; rxbps: number; txbps: number }[];
}) {
  const data = history.map((h) => ({
    ts: h.ts,
    rxbps: h.rxbps,
    txbps: h.txbps,
  }));

  if (data.length < 2) {
    return <p className="emptyInline">Collecting network traffic data…</p>;
  }

  return (
    <div style={{ flex: "1 1 300px", minWidth: "300px" }}>
      <p style={{ margin: "0 0 0.25rem", fontWeight: 600, fontSize: "0.8rem" }}>Network Traffic</p>
      <ResponsiveContainer width="100%" height={100}>
        <LineChart data={data} margin={{ top: 2, right: 4, left: -20, bottom: 0 }}>
          <CartesianGrid strokeDasharray="3 3" stroke="var(--cpuPie-track)" />
          <XAxis dataKey="ts" tickFormatter={(v) => new Date(v).toLocaleTimeString()} tick={{ fontSize: 9 }} />
          <YAxis tick={{ fontSize: 9 }} tickFormatter={(v) => formatBytes(v)} />
          <Tooltip
            formatter={(v, name) => [formatBytes(Number(v)), name === "rxbps" ? "RX/s" : "TX/s"]}
            labelFormatter={(v) => new Date(Number(v)).toLocaleTimeString()}
          />
          <Legend formatter={(v) => (v === "rxbps" ? "RX/s" : "TX/s")} />
          <Line
            type="monotone"
            dataKey="rxbps"
            stroke="#1f9d55"
            strokeWidth={1.5}
            dot={false}
            name="rxbps"
            isAnimationActive={false}
          />
          <Line
            type="monotone"
            dataKey="txbps"
            stroke="#2d8cf0"
            strokeWidth={1.5}
            dot={false}
            name="txbps"
            isAnimationActive={false}
          />
        </LineChart>
      </ResponsiveContainer>
    </div>
  );
}

function ProcessCountMiniChart({
  history,
}: {
  history: { ts: number; procCount: number }[];
}) {
  const data: ProcessChartDataPoint[] = history
    .filter((entry) => Number.isFinite(entry.procCount))
    .map((entry) => ({
      ts: entry.ts,
      procCount: entry.procCount,
    }));

  if (data.length < 2) {
    return null;
  }

  const values = data.map((entry) => entry.procCount);
  const minValue = Math.min(...values);
  const maxValue = Math.max(...values);
  const domain =
    minValue === maxValue
      ? [Math.max(0, minValue - 1), maxValue + 1]
      : [Math.max(0, minValue), maxValue];

  return (
    <div className="processMiniChart">
      <ResponsiveContainer width="100%" height={42}>
        <LineChart data={data} margin={{ top: 4, right: 2, left: 2, bottom: 2 }}>
          <Tooltip
            formatter={(value) => [String(value), "Processes"]}
            labelFormatter={(value) => new Date(Number(value)).toLocaleTimeString()}
          />
          <Line
            type="monotone"
            dataKey="procCount"
            stroke="#2d8cf0"
            strokeWidth={1.3}
            dot={{ r: 2, fill: "#2d8cf0", strokeWidth: 0 }}
            activeDot={{ r: 3 }}
            isAnimationActive={false}
          />
          <YAxis hide domain={domain} />
          <XAxis hide dataKey="ts" />
        </LineChart>
      </ResponsiveContainer>
    </div>
  );
}

type StationDetailTab = "overview" | "edit";

interface StationBrowserPanelProps {
  filteredStations: Station[];
  totalStationCount: number;
  groups: StationGroup[];
  tagDefinitions: TagDefinition[];
  runtimeByStation: Record<string, StationRuntimeSnapshot>;
  loading: boolean;
  selectedId: string;
  search: string;
  sortBy: StationSortBy;
  sortDirection: StationSortDirection;
  groupFilter: string;
  tagKeyFilter: string;
  tagValueFilter: string;
  tagValueOptions: string[];
  viewMode: StationViewMode;
  listColumns: number;
  gridColumns: number;
  panelMaxHeight: number | null;
  hasActiveFilter: boolean;
  filtersHideStations: boolean;
  selectedStationHiddenByFilters: boolean;
  onSearchChange: (value: string) => void;
  onSortChange: (value: StationSortBy) => void;
  onSortDirectionChange: (value: StationSortDirection) => void;
  onGroupFilterChange: (value: string) => void;
  onTagKeyFilterChange: (value: string) => void;
  onTagValueFilterChange: (value: string) => void;
  onViewModeChange: (value: StationViewMode) => void;
  onListColumnsChange: (value: number) => void;
  onGridColumnsChange: (value: number) => void;
  onSelectStation: (stationId: string) => void;
  onEditStation: (stationId: string) => void;
  onClearFilters: () => void;
}

function StationBrowserPanel({
  filteredStations,
  totalStationCount,
  groups,
  tagDefinitions,
  runtimeByStation,
  loading,
  selectedId,
  search,
  sortBy,
  sortDirection,
  groupFilter,
  tagKeyFilter,
  tagValueFilter,
  tagValueOptions,
  viewMode,
  listColumns,
  gridColumns,
  panelMaxHeight,
  hasActiveFilter,
  filtersHideStations,
  selectedStationHiddenByFilters,
  onSearchChange,
  onSortChange,
  onSortDirectionChange,
  onGroupFilterChange,
  onTagKeyFilterChange,
  onTagValueFilterChange,
  onViewModeChange,
  onListColumnsChange,
  onGridColumnsChange,
  onSelectStation,
  onEditStation,
  onClearFilters,
}: StationBrowserPanelProps) {
  const activeTagDefinition =
    tagDefinitions.find((definition) => getTagDefinitionKey(definition) === tagKeyFilter) ?? null;

  return (
    <aside
      className="panel stationsBrowserPanel"
      style={panelMaxHeight ? { maxHeight: `${panelMaxHeight}px` } : undefined}
    >
      <div className="stationsBrowserHeader">
        <div className="panelHeader stationsBrowserTitleRow">
          <div>
            <h2>Devices</h2>
            <p className="stationsBrowserCount">
              Showing {filteredStations.length} of {totalStationCount} devices
            </p>
          </div>
          <div className="stationsViewToggle" role="group" aria-label="Device view mode">
            {(["list", "grid", "split"] as const).map((mode) => (
              <button
                key={mode}
                type="button"
                className={viewMode === mode ? "accent" : ""}
                onClick={() => onViewModeChange(mode)}
                aria-pressed={viewMode === mode}
              >
                {mode === "list" ? "List" : mode === "grid" ? "Grid" : "Split"}
              </button>
            ))}
          </div>
        </div>

        <div className="stationsFilterBar">
          <input
            value={search}
            onChange={(event) => onSearchChange(event.target.value)}
            placeholder="Search name, MAC, or IP"
            aria-label="Search stations"
          />
          <div className="stationsSortControls">
            <select
              value={sortBy}
              onChange={(event) => onSortChange(event.target.value as StationSortBy)}
              aria-label="Sort devices"
            >
              <option value="name">Sort: Name</option>
              <option value="ip">Sort: IP</option>
              <option value="group">Sort: Group</option>
              <option value="tag">Sort: Tag</option>
            </select>
            <button
              type="button"
              className="stationsSortDirectionButton"
              onClick={() => onSortDirectionChange(sortDirection === "asc" ? "desc" : "asc")}
              aria-label={`Sort ${sortDirection === "asc" ? "ascending" : "descending"}`}
              title={sortDirection === "asc" ? "Ascending order" : "Descending order"}
            >
              <span
                className={`stationsSortDirectionIcon stationsSortDirectionIcon--${sortDirection}`}
                aria-hidden="true"
              >
                ↑
              </span>
            </button>
          </div>
          <select
            value={groupFilter}
            onChange={(event) => onGroupFilterChange(event.target.value)}
            aria-label="Filter by group"
          >
            <option value="">All Groups</option>
            {groups.map((group) => (
              <option key={group.id} value={group.id}>
                {group.name}
              </option>
            ))}
          </select>
          <select
            value={tagKeyFilter}
            onChange={(event) => onTagKeyFilterChange(event.target.value)}
            aria-label="Filter by tag"
          >
            <option value="">All Tags</option>
            {tagDefinitions.map((definition) => (
              <option key={definition.id} value={getTagDefinitionKey(definition)}>
                {getTagDefinitionLabel(definition)}
              </option>
            ))}
          </select>
          {activeTagDefinition ? (
            activeTagDefinition.type === "boolean" ? (
              <select
                value={tagValueFilter}
                onChange={(event) => onTagValueFilterChange(event.target.value)}
                aria-label="Filter by tag value"
              >
                <option value="">Any Value</option>
                <option value="true">True</option>
                <option value="false">False</option>
              </select>
            ) : activeTagDefinition.type === "select" && activeTagDefinition.options?.length ? (
              <select
                value={tagValueFilter}
                onChange={(event) => onTagValueFilterChange(event.target.value)}
                aria-label="Filter by tag value"
              >
                <option value="">Any Value</option>
                {Array.from(new Set([...(activeTagDefinition.options ?? []), ...tagValueOptions])).map((value) => (
                  <option key={value} value={value}>
                    {value}
                  </option>
                ))}
              </select>
            ) : (
              <>
                <input
                  list="station-tag-values"
                  value={tagValueFilter}
                  onChange={(event) => onTagValueFilterChange(event.target.value)}
                  placeholder="Tag value"
                  aria-label="Filter by tag value"
                />
                <datalist id="station-tag-values">
                  {tagValueOptions.map((value) => (
                    <option key={value} value={value} />
                  ))}
                </datalist>
              </>
            )
          ) : null}
          <button
            type="button"
            className="stationsClearFiltersButton"
            onClick={onClearFilters}
            disabled={!hasActiveFilter}
            aria-label="Clear filters"
            title="Clear filters"
          >
            <span aria-hidden="true">×</span>
          </button>
        </div>

        <p className="stationsSortHint">
          Group sorts by the first assigned group; tag sorts by the selected tag value when available.
        </p>

        <div className="stationsBrowserMetaRow">
          <span>
            {viewMode === "grid" || viewMode === "list" ? (
              <span className="stationsColumnSelector">
                <span className="stationsColumnSelectorLabel">Columns:</span>
                {[1, 2, 3, 4, 5].map((cols) => (
                  <button
                    key={cols}
                    type="button"
                    className={`stationsColumnBtn ${
                      (viewMode === "grid" ? gridColumns : listColumns) === cols ? "accent" : ""
                    }`}
                    onClick={() =>
                      viewMode === "grid" ? onGridColumnsChange(cols) : onListColumnsChange(cols)
                    }
                    aria-pressed={(viewMode === "grid" ? gridColumns : listColumns) === cols}
                  >
                    {cols}
                  </button>
                ))}
              </span>
            ) : viewMode === "split" ? (
              "Split workspace"
            ) : (
              "Dense device list"
            )}
          </span>
          {selectedStationHiddenByFilters ? (
            <span className="stationsHiddenSelectionNotice">
              Selected station is hidden by current filters.
            </span>
          ) : null}
        </div>
      </div>

      <div
        className={`stationBrowser stationBrowser--${viewMode}`}
        style={
          viewMode === "grid"
            ? { gridTemplateColumns: `repeat(${gridColumns}, minmax(0, 1fr))` }
            : viewMode === "list"
              ? { gridTemplateColumns: `repeat(${listColumns}, minmax(0, 1fr))` }
              : undefined
        }
      >
        {loading ? (
          <p className="emptyState">Loading state…</p>
        ) : filteredStations.length === 0 ? (
          <p className="emptyState">
            {filtersHideStations
              ? hasActiveFilter
                ? "Current search, group, or tag filters are hiding all stations."
                : "Devices exist but are currently hidden by filters."
              : "No devices configured yet. Add a device to begin."}
          </p>
        ) : (
          filteredStations.map((station) => {
            const stationVisualState = getStationVisualState(
              station,
              runtimeByStation[station.id] ?? null,
            );
            const groupNames = getStationGroupNames(station, groups);
            const tagSummary = getStationTagSummary(station, tagDefinitions);
            const runtimeSummary = getStationRuntimeSummary(runtimeByStation[station.id] ?? null);

            return (
              <div
                key={station.id}
                className={[
                  "stationCard",
                  "stationBrowserItem",
                  viewMode === "list" || viewMode === "split" ? "stationBrowserRow" : "stationBrowserGridCard",
                  station.id === selectedId ? "selected" : "",
                ].join(" ")}
                role="button"
                tabIndex={0}
                onClick={() => onSelectStation(station.id)}
                onKeyDown={(event) => {
                  if (event.key === "Enter" || event.key === " ") {
                    event.preventDefault();
                    onSelectStation(station.id);
                  }
                }}
              >
                <div className="stationCardTopRow">
                  <div className="stationCardIdentity">
                    <ComputerStatusIcon state={stationVisualState} />
                    <div className="stationIdentityText">
                      <span className="stationName">{station.name || "Unnamed Device"}</span>
                      <span className="stationIp">{getPrimaryIp(station)}</span>
                    </div>
                  </div>
                  <button
                    type="button"
                    className="stationEditButton"
                    onClick={(event) => {
                      event.stopPropagation();
                      onEditStation(station.id);
                    }}
                    aria-label={`Edit ${station.name || "station"}`}
                  >
                    Edit
                  </button>
                </div>

                <div className="stationMeta stationMeta--stacked">
                  <span className={`badge ${stationVisualState}`}>
                    {getStationStatusLabel(stationVisualState)}
                  </span>
                  {runtimeSummary.length > 0 ? (
                    <span className="stationStats">
                      {runtimeSummary.map((item) => (
                        <span key={item} className="stationStatItem">
                          {item}
                        </span>
                      ))}
                    </span>
                  ) : null}
                </div>

                {groupNames.length > 0 ? (
                  <div className="stationMeta stationMetaChips">
                    {groupNames.slice(0, viewMode === "grid" ? 2 : 3).map((groupName) => (
                      <span key={groupName} className="stationFilterChip">
                        {groupName}
                      </span>
                    ))}
                  </div>
                ) : null}

                {tagSummary.length > 0 ? (
                  <div className="stationMeta stationMetaChips">
                    {tagSummary.slice(0, viewMode === "grid" ? 2 : 3).map((entry) => (
                      <span key={entry} className="stationFilterChip stationFilterChip--muted">
                        {entry}
                      </span>
                    ))}
                  </div>
                ) : null}
              </div>
            );
          })
        )}
      </div>
    </aside>
  );
}

function AppShell() {
  const { tasks, executeTask } = useBatch();
  const { runner, openRunner, closeRunner } = useBatchUI();
  const [snapshot, setSnapshot] = useState<AppSnapshot | null>(null);
  const [stations, setStations] = useState<Station[]>([]);
  const stationsRef = useRef<Station[]>([]);
  const [options, setOptions] = useState<ClientOptions>(emptyOptions);
  const [selectedId, setSelectedId] = useState<string>("");
  const [search, setSearch] = useState("");
  const [sortBy, setSortBy] = useState<StationSortBy>("name");
  const [sortDirection, setSortDirection] = useState<StationSortDirection>("asc");
  const [loading, setLoading] = useState(true);
  const [saving, setSaving] = useState(false);
  const [log, setLog] = useState<string[]>([]);
  const [runtimeByStation, setRuntimeByStation] = useState<Record<string, StationRuntimeSnapshot>>({});
  const runtimeByStationRef = useRef<Record<string, StationRuntimeSnapshot>>({});
  const [historyByStation, setHistoryByStation] = useState<Record<string, { cpu: number; memory: number; procCount: number; ts: number; rxbps: number; txbps: number }[]>>({});
  const MAX_HISTORY = 30;
  const [browserByStation, setBrowserByStation] = useState<Record<string, RemoteFileBrowserResult>>({});
  const [captureByStation, setCaptureByStation] = useState<Record<string, StationScreenCapture>>({});
  const [batchCaptures, setBatchCaptures] = useState<BatchCapture[]>([]);
  const [remoteBusy, setRemoteBusy] = useState<string>("");
  const [remotePath, setRemotePath] = useState("");
  const [downloadLocalPath, setDownloadLocalPath] = useState("");
  const [uploadLocalPath, setUploadLocalPath] = useState("");
  const [renameSourcePath, setRenameSourcePath] = useState("");
  const [renameTargetPath, setRenameTargetPath] = useState("");
  const [commandInput, setCommandInput] = useState("");
  const [commandTimeout, setCommandTimeout] = useState(30);
  const [commandOutput, setCommandOutput] = useState<CommandExecutionResult | null>(null);
  const [groups, setGroups] = useState<StationGroup[]>([]);
  const [groupFilter, setGroupFilter] = useState<string>("");
  const [tagKeyFilter, setTagKeyFilter] = useState<string>("");
  const [tagValueFilter, setTagValueFilter] = useState<string>("");
  const [tagDefinitions, setTagDefinitions] = useState<TagDefinition[]>([]);
  const [editingGroup, setEditingGroup] = useState<StationGroup | null>(null);
  const [activePage, setActivePage] = useState<"stations" | "settings" | "groups" | "tags" | "messages" | "scripts" | "batch" | "alerts">("stations");
  const [viewMode, setViewMode] = useState<StationViewMode>(() => readStoredViewMode());
  const [listColumns, setListColumns] = useState(() => readStoredColumns(STATION_LIST_COLUMNS_STORAGE_KEY, 2));
  const [gridColumns, setGridColumns] = useState(() => readStoredColumns(STATION_GRID_COLUMNS_STORAGE_KEY, 3));
  const [detailTab, setDetailTab] = useState<StationDetailTab>("overview");
  const [stationPanelWidth, setStationPanelWidth] = useState<number | null>(() => readStoredPanelWidth());
  const [isResizingPanels, setIsResizingPanels] = useState(false);
  const [isStationsStacked, setIsStationsStacked] = useState(
    () => typeof window !== "undefined" && window.innerWidth <= STATIONS_STACK_BREAKPOINT,
  );
  const [stationPanelsMaxHeight, setStationPanelsMaxHeight] = useState<number | null>(null);
  const [pendingStationTagValues, setPendingStationTagValues] = useState<Record<string, string>>({});
  const [dirtyStationTagKeys, setDirtyStationTagKeys] = useState<Record<string, boolean>>({});
  const [telemetrySchema, setTelemetrySchema] = useState<TelemetrySchemaResponse | null>(null);
  const [telemetrySchemaLoading, setTelemetrySchemaLoading] = useState(false);
  const [stationTelemetryProfiles, setStationTelemetryProfiles] = useState<TelemetryProfileDraft[]>([]);
  const [stationTelemetryLoading, setStationTelemetryLoading] = useState(false);
  const [stationTelemetrySaving, setStationTelemetrySaving] = useState(false);
  const [stationTelemetryNotice, setStationTelemetryNotice] = useState<string | null>(null);
  const [stationTelemetryError, setStationTelemetryError] = useState<string | null>(null);
  const [globalTelemetryNotice, setGlobalTelemetryNotice] = useState<string | null>(null);
  const [globalTelemetryError, setGlobalTelemetryError] = useState<string | null>(null);
  const [globalTelemetrySaving, setGlobalTelemetrySaving] = useState(false);
  const stationsWorkspaceRef = useRef<HTMLElement | null>(null);
  const resizingPanelsRef = useRef(false);

  useEffect(() => {
    window.localStorage.setItem(STATION_VIEW_MODE_STORAGE_KEY, viewMode);
  }, [viewMode]);

  useEffect(() => {
    window.localStorage.setItem(STATION_LIST_COLUMNS_STORAGE_KEY, String(listColumns));
  }, [listColumns]);

  useEffect(() => {
    window.localStorage.setItem(STATION_GRID_COLUMNS_STORAGE_KEY, String(gridColumns));
  }, [gridColumns]);

  useEffect(() => {
    if (stationPanelWidth === null) {
      window.localStorage.removeItem(STATION_PANEL_WIDTH_STORAGE_KEY);
      return;
    }

    window.localStorage.setItem(STATION_PANEL_WIDTH_STORAGE_KEY, String(stationPanelWidth));
  }, [stationPanelWidth]);

  useEffect(() => {
    stationsRef.current = stations;
  }, [stations]);

  useEffect(() => {
    runtimeByStationRef.current = runtimeByStation;
  }, [runtimeByStation]);

  useEffect(() => {
    const onResize = () => {
      const isStacked = window.innerWidth <= STATIONS_STACK_BREAKPOINT;
      setIsStationsStacked(isStacked);

      if (isStacked || stationPanelWidth === null || !stationsWorkspaceRef.current) {
        return;
      }

      const workspaceWidth = stationsWorkspaceRef.current.getBoundingClientRect().width;
      const maxLeft = Math.max(
        MIN_BROWSER_PANEL_WIDTH,
        workspaceWidth - MIN_DETAIL_PANEL_WIDTH - STATIONS_PANEL_RESIZER_WIDTH,
      );
      const clamped = Math.min(Math.max(stationPanelWidth, MIN_BROWSER_PANEL_WIDTH), maxLeft);
      if (clamped !== stationPanelWidth) {
        setStationPanelWidth(clamped);
      }
    };

    onResize();
    window.addEventListener("resize", onResize);
    return () => window.removeEventListener("resize", onResize);
  }, [stationPanelWidth]);

  useEffect(() => {
    const updatePanelHeight = () => {
      if (!stationsWorkspaceRef.current) {
        setStationPanelsMaxHeight(null);
        return;
      }

      const rect = stationsWorkspaceRef.current.getBoundingClientRect();
      const viewportBottomPadding = 8;
      const availableHeight = Math.floor(window.innerHeight - Math.max(rect.top, 0) - viewportBottomPadding);
      setStationPanelsMaxHeight(availableHeight > 0 ? availableHeight : null);
    };

    updatePanelHeight();
    const frameId = window.requestAnimationFrame(updatePanelHeight);
    const transitionTimer = window.setTimeout(updatePanelHeight, 450);
    window.addEventListener("resize", updatePanelHeight);
    const scrollContainer =
      stationsWorkspaceRef.current?.closest(".app-content") ??
      stationsWorkspaceRef.current?.closest(".shell");
    scrollContainer?.addEventListener("scroll", updatePanelHeight, { passive: true });

    return () => {
      window.cancelAnimationFrame(frameId);
      window.clearTimeout(transitionTimer);
      window.removeEventListener("resize", updatePanelHeight);
      scrollContainer?.removeEventListener("scroll", updatePanelHeight);
    };
  }, [activePage, loading, runtimeByStation, stations.length, viewMode]);

  useEffect(() => {
    const onPointerMove = (event: PointerEvent) => {
      if (!resizingPanelsRef.current || !stationsWorkspaceRef.current || isStationsStacked) {
        return;
      }

      const rect = stationsWorkspaceRef.current.getBoundingClientRect();
      const maxLeft = Math.max(
        MIN_BROWSER_PANEL_WIDTH,
        rect.width - MIN_DETAIL_PANEL_WIDTH - STATIONS_PANEL_RESIZER_WIDTH,
      );
      const next = Math.min(Math.max(event.clientX - rect.left, MIN_BROWSER_PANEL_WIDTH), maxLeft);
      setStationPanelWidth(Math.round(next));
    };

    const onPointerUp = () => {
      if (!resizingPanelsRef.current) {
        return;
      }

      resizingPanelsRef.current = false;
      setIsResizingPanels(false);
    };

    window.addEventListener("pointermove", onPointerMove);
    window.addEventListener("pointerup", onPointerUp);

    return () => {
      window.removeEventListener("pointermove", onPointerMove);
      window.removeEventListener("pointerup", onPointerUp);
    };
  }, [isStationsStacked]);

  useEffect(() => {
    void loadSnapshot();
  }, []);

  useEffect(() => {
    const localStation = stations.find((station) => station.id === LOCAL_STATION_ID);
    if (!localStation) {
      return;
    }

    let cancelled = false;

    void invoke<RemoteStationIdentity>("probe_station_identity_for_ui", {
      id: LOCAL_STATION_ID,
    })
      .then((identity) => {
        if (cancelled || !identity.stationId || identity.stationId === LOCAL_STATION_ID) {
          return;
        }

        setStations((current) =>
          reconcileStationIdentity(
            current,
            LOCAL_STATION_ID,
            identity.stationId,
            identity.computerName,
          ),
        );
        setRuntimeByStation((current) =>
          remapRecordKey(current, LOCAL_STATION_ID, identity.stationId),
        );
        setHistoryByStation((current) =>
          remapRecordKey(current, LOCAL_STATION_ID, identity.stationId),
        );
        setBrowserByStation((current) =>
          remapRecordKey(current, LOCAL_STATION_ID, identity.stationId),
        );
        setCaptureByStation((current) =>
          remapRecordKey(current, LOCAL_STATION_ID, identity.stationId),
        );
        setSelectedId((current) =>
          current === LOCAL_STATION_ID ? identity.stationId : current,
        );
      })
      .catch(() => {
        // Keep the placeholder when the local service probe is unavailable.
      });

    return () => {
      cancelled = true;
    };
  }, [stations]);

  useEffect(() => {
    const unlistenFns: UnlistenFn[] = [];

    const registerListeners = async () => {
      const unlistenTelemetry = await listen<MqttTelemetryEventPayload>("telemetry", (event) => {
        const { station_id: stationId, data } = event.payload;
        const runtime = buildRuntimeFromMqttTelemetry(
          stationId,
          data,
          runtimeByStationRef.current[stationId],
        );
        const existingStation = stationsRef.current.find((station) => station.id === stationId) ?? null;
        const alertStation =
          existingStation
          ?? createDiscoveredStation(stationId, {
            mqttLastSeen: String(data.ts),
          });

        setStations((current) => {
          if (current.some((station) => station.id === stationId)) {
            return current.map((station) =>
              station.id === stationId
                ? {
                    ...station,
                    metadata: {
                      ...station.metadata,
                      source: station.metadata.source || MQTT_DISCOVERY_SOURCE,
                      mqttLastSeen: String(data.ts),
                    },
                  }
                : station,
            );
          }

          return [
            createDiscoveredStation(stationId, {
              mqttLastSeen: String(data.ts),
            }),
            ...current,
          ];
        });

        setRuntimeByStation((current) => ({ ...current, [stationId]: runtime }));
        setHistoryByStation((current) => {
          const prev = current[stationId] ?? [];
          const rxbps = runtime.networkStats.reduce((sum, item) => sum + item.bytesReceivedPerSec, 0);
          const txbps = runtime.networkStats.reduce((sum, item) => sum + item.bytesSentedPerSec, 0);
          const next = [
            ...prev.slice(-(MAX_HISTORY - 1)),
            {
              cpu: runtime.cpu,
              memory: runtime.currentMemory,
              procCount: runtime.procCount,
              ts: Date.now(),
              rxbps,
              txbps,
            },
          ];
          return { ...current, [stationId]: next };
        });
        emitAlertRuntimeUpdate(alertStation, runtime);
      });
      unlistenFns.push(unlistenTelemetry);

      const unlistenStatus = await listen<MqttStatusEventPayload>("station-status", (event) => {
        const { station_id: stationId, status } = event.payload;

        setStations((current) => {
          const existing = current.find((station) => station.id === stationId);
          if (!existing) {
            return [
              createDiscoveredStation(stationId, {
                mqttOnline: String(status.online),
                mqttLastSeen: String(status.last_seen),
              }),
              ...current,
            ];
          }

          return current.map((station) =>
            station.id === stationId
              ? {
                  ...station,
                  metadata: {
                    ...station.metadata,
                    source: station.metadata.source || MQTT_DISCOVERY_SOURCE,
                    mqttOnline: String(status.online),
                    mqttLastSeen: String(status.last_seen),
                  },
                }
              : station,
          );
        });

        if (!status.online) {
          setRuntimeByStation((current) => {
            const next = { ...current };
            delete next[stationId];
            return next;
          });
        }
      });
      unlistenFns.push(unlistenStatus);
    };

    void registerListeners();

    return () => {
      unlistenFns.forEach((fn) => fn());
    };
  }, []);

  useEffect(() => {
    if (activePage === "stations") {
      void loadSnapshot();
    }
  }, [activePage]);

  const filteredStations = useMemo(() => {
    return filterStations({
      stations,
      search,
      sortBy,
      sortDirection,
      groupId: groupFilter,
      tagKey: tagKeyFilter,
      tagValue: tagValueFilter,
      groups,
      tagDefinitions,
    });
  }, [groupFilter, groups, search, sortBy, sortDirection, stations, tagDefinitions, tagKeyFilter, tagValueFilter]);

  const tagValueOptions = useMemo(
    () => deriveTagValueOptions(stations, tagKeyFilter),
    [stations, tagKeyFilter],
  );
  const batchTargets = useMemo(
    () =>
      stations.map((station) => ({
        id: station.id,
        name: station.name,
        status: station.blocked ? ("offline" as const) : ("online" as const),
        group: station.groups[0],
        groups: station.groups,
        tags: station.tags,
      })),
    [stations],
  );
  const toolbarBatchTasks = useMemo(
    () => tasks.filter((task) => task.showInToolbar),
    [tasks],
  );

  const selectedStation =
    filteredStations.find((station) => station.id === selectedId) ??
    filteredStations[0] ??
    stations.find((station) => station.id === selectedId) ??
    stations[0] ??
    null;
  const selectedRuntime = selectedStation ? runtimeByStation[selectedStation.id] ?? null : null;
  const selectedBrowser = selectedStation ? browserByStation[selectedStation.id] ?? null : null;
  const selectedCapture = selectedStation ? captureByStation[selectedStation.id] ?? null : null;
  const hasRuntimeData = Object.keys(runtimeByStation).length > 0;
  const hasStationMessage = Object.values(runtimeByStation).some(
    (runtime) => runtime.message.trim().length > 0,
  );
  const hasActiveFilter =
    search.trim().length > 0 ||
    Boolean(groupFilter) ||
    Boolean(tagKeyFilter) ||
    Boolean(tagValueFilter);
  const filtersHideStations = stations.length > 0 && filteredStations.length === 0;
  const selectedStationHiddenByFilters =
    Boolean(selectedStation) &&
    filteredStations.every((station) => station.id !== selectedStation?.id);
  const stationTelemetryValidationErrors = useMemo(
    () => validateTelemetryProfileDrafts(stationTelemetryProfiles),
    [stationTelemetryProfiles],
  );
  const stationTelemetryValidationWarnings = useMemo(
    () => validateEnabledTelemetrySectionConflicts(stationTelemetryProfiles),
    [stationTelemetryProfiles],
  );
  const globalTelemetryProfiles = useMemo(
    () => options.defaultTelemetryProfiles as TelemetryProfileDraft[],
    [options.defaultTelemetryProfiles],
  );
  const globalTelemetryValidationErrors = useMemo(
    () => validateTelemetryProfileDrafts(globalTelemetryProfiles),
    [globalTelemetryProfiles],
  );
  const globalTelemetryValidationWarnings = useMemo(
    () => validateEnabledTelemetrySectionConflicts(globalTelemetryProfiles),
    [globalTelemetryProfiles],
  );

  function normalizeOptionsTelemetryProfiles(nextOptions: ClientOptions): ClientOptions {
    return {
      ...nextOptions,
      interval: Math.max(1, nextOptions.interval),
      defaultTelemetryProfiles: nextOptions.defaultTelemetryProfiles,
    };
  }

  function updateDefaultTelemetryProfiles(nextProfiles: TelemetryProfileDraft[]) {
    setGlobalTelemetryNotice(null);
    setGlobalTelemetryError(null);
    setOptions((current) => ({
      ...current,
      defaultTelemetryProfiles: nextProfiles,
    }));
  }

  useEffect(() => {
    setPendingStationTagValues({});
    setDirtyStationTagKeys({});
  }, [selectedStation?.id]);

  useEffect(() => {
    if (activePage !== "stations") {
      return;
    }

    const schemaStationId =
      selectedStation?.id ??
      filteredStations[0]?.id ??
      stations[0]?.id ??
      "";

    if (!schemaStationId) {
      setTelemetrySchema(null);
      setTelemetrySchemaLoading(false);
      return;
    }

    if (telemetrySchema) {
      return;
    }

    let cancelled = false;
    setTelemetrySchemaLoading(true);
    void invoke<TelemetrySchemaResponse>("get_station_telemetry_schema_for_ui", {
      id: schemaStationId,
    })
      .then((schema) => {
        if (cancelled) {
          return;
        }
        setTelemetrySchema(schema);
      })
      .catch(() => {
        // Leave schema unset until a reachable station is available.
      })
      .finally(() => {
        if (!cancelled) {
          setTelemetrySchemaLoading(false);
        }
      });

    return () => {
      cancelled = true;
    };
  }, [activePage, filteredStations, selectedStation?.id, stations, telemetrySchema]);

  useEffect(() => {
    if (detailTab !== "edit" || !selectedStation) {
      return;
    }

    let cancelled = false;
    setStationTelemetryLoading(true);
    setStationTelemetryError(null);
    setStationTelemetryNotice(null);

    void Promise.all([
      invoke<TelemetrySchemaResponse>("get_station_telemetry_schema_for_ui", {
        id: selectedStation.id,
      }),
      invoke<{ schemaVersion: number; profilesVersion: number; profiles: TelemetryProfileDraft[] }>(
        "get_station_telemetry_profiles_for_ui",
        { id: selectedStation.id },
      ),
    ])
      .then(([schema, response]) => {
        if (cancelled) {
          return;
        }
        setTelemetrySchema(schema);
        setStationTelemetryProfiles(response.profiles);
      })
      .catch((error) => {
        if (cancelled) {
          return;
        }
        setStationTelemetryError(
          error instanceof Error ? error.message : String(error),
        );
        setStationTelemetryProfiles([]);
      })
      .finally(() => {
        if (!cancelled) {
          setStationTelemetryLoading(false);
        }
      });

    return () => {
      cancelled = true;
    };
  }, [detailTab, selectedStation?.id]);

  async function loadSnapshot() {
    setLoading(true);
    try {
      const next = await invoke<AppSnapshot>("load_state");
      setSnapshot(next);
      setStations(next.stations);
      setOptions(next.options);
      setGroups(next.groups ?? []);
      setTagDefinitions(next.tags ?? []);
      setSelectedId("");
      setLog((current) => [
        next.legacyImported
          ? "Imported legacy ~/.CC-Client data into the Rust state store."
          : "Loaded Rust-native state store.",
        ...current
      ]);
    } catch (error) {
      setLog((current) => [`Failed to load state: ${String(error)}`, ...current]);
    } finally {
      setLoading(false);
    }
  }

  function updateStation(next: Station) {
    setStations((current) =>
      current.map((station) => (station.id === next.id ? next : station))
    );
  }

  function ensureSelected() {
    if (filteredStations[0] && !filteredStations.some((station) => station.id === selectedId)) {
      setSelectedId(filteredStations[0].id);
    }
  }

  useEffect(() => {
    ensureSelected();
  }, [filteredStations]);

  useEffect(() => {
    if (!selectedStation) {
      return;
    }

    setRemotePath(browserByStation[selectedStation.id]?.requestedPath ?? "");
  }, [browserByStation, selectedStation?.id]);

  async function saveState() {
    setSaving(true);
    try {
      const payload: PersistedState = {
        stations,
        options: normalizeOptionsTelemetryProfiles(options),
        groups,
        tags: tagDefinitions,
      };
      const next = await invoke<AppSnapshot>("save_state", { payload });
      setSnapshot(next);
      setStations(next.stations);
      setOptions(next.options);
      setLog((current) => ["Saved state.json to the Rust storage directory.", ...current]);
    } catch (error) {
      setLog((current) => [`Save failed: ${String(error)}`, ...current]);
    } finally {
      setSaving(false);
    }
  }

  async function saveStationTelemetryProfiles() {
    if (!selectedStation || stationTelemetryValidationErrors.length > 0) {
      return;
    }

    if (stationTelemetryValidationWarnings.length > 0) {
      setStationTelemetryNotice(null);
      setStationTelemetryError(stationTelemetryValidationWarnings.join(" "));
      return;
    }

    const profiles = stationTelemetryProfiles;

    setStationTelemetrySaving(true);
    setStationTelemetryError(null);
    setStationTelemetryNotice(null);
    try {
      const response = await invoke<{
        schemaVersion: number;
        profilesVersion: number;
        profiles: TelemetryProfileDraft[];
      }>("replace_station_telemetry_profiles_for_ui", {
        id: selectedStation.id,
        profiles,
      });
      setStationTelemetryProfiles(response.profiles);
      setStationTelemetryNotice("Telemetry profiles saved and applied live.");
      setLog((current) => [`Telemetry profiles saved for ${selectedStation.name}.`, ...current]);
    } catch (error) {
      const message = error instanceof Error ? error.message : String(error);
      setStationTelemetryError(message);
      setLog((current) => [`Telemetry profile save failed for ${selectedStation.name}: ${message}`, ...current]);
    } finally {
      setStationTelemetrySaving(false);
    }
  }

  async function applyDefaultTelemetryProfilesToAllStations() {
    if (globalTelemetryValidationErrors.length > 0) {
      return;
    }

    if (globalTelemetryValidationWarnings.length > 0) {
      setGlobalTelemetryNotice(null);
      setGlobalTelemetryError(globalTelemetryValidationWarnings.join(" "));
      return;
    }

    const profiles = globalTelemetryProfiles;
    if (profiles.length === 0) {
      setGlobalTelemetryError("Create at least one default telemetry profile before applying.");
      return;
    }

    setGlobalTelemetrySaving(true);
    setGlobalTelemetryError(null);
    setGlobalTelemetryNotice(null);

    let profilesToApply = profiles;
    try {
      const nextOptions = normalizeOptionsTelemetryProfiles({
        ...options,
        defaultTelemetryProfiles: profiles,
      });
      const payload: PersistedState = {
        stations,
        options: nextOptions,
        groups,
        tags: tagDefinitions,
      };
      const next = await invoke<AppSnapshot>("save_state", { payload });
      setSnapshot(next);
      setStations(next.stations);
      setOptions(next.options);
      profilesToApply = next.options.defaultTelemetryProfiles as TelemetryProfileDraft[];
      setLog((current) => ["Saved default telemetry profiles locally.", ...current]);
    } catch (error) {
      const message = `Failed to save default telemetry profiles locally: ${String(error)}`;
      setGlobalTelemetryError(message);
      setLog((current) => [message, ...current]);
      setGlobalTelemetrySaving(false);
      return;
    }

    let success = 0;
    let failed = 0;
    for (const station of stations) {
      try {
        await invoke("replace_station_telemetry_profiles_for_ui", {
          id: station.id,
          profiles: profilesToApply,
        });
        success += 1;
      } catch (error) {
        failed += 1;
        setLog((current) => [
          `Default telemetry apply failed for ${station.name}: ${String(error)}`,
          ...current,
        ]);
      }
    }

    const notice =
      failed === 0
        ? `Applied default telemetry profiles to ${success} device${success === 1 ? "" : "s"}.`
        : `Applied defaults to ${success} device${success === 1 ? "" : "s"}; ${failed} failed.`;
    setGlobalTelemetryNotice(notice);
    setLog((current) => [notice, ...current]);
    setGlobalTelemetrySaving(false);
  }

  async function exportLegacyFiles() {
    try {
      const message = await invoke<string>("export_legacy_files");
      setLog((current) => [message, ...current]);
    } catch (error) {
      setLog((current) => [`Legacy export failed: ${String(error)}`, ...current]);
    }
  }

  async function browseRemote(path = remotePath || "") {
    if (!selectedStation) {
      return;
    }

    setRemoteBusy("browse");
    try {
      const result = await invoke<RemoteFileBrowserResult>("browse_station_files_for_ui", {
        id: selectedStation.id,
        path
      });
      setBrowserByStation((current) => ({ ...current, [selectedStation.id]: result }));
      setRemotePath(result.requestedPath);
      setLog((current) => [`Browsed ${result.requestedPath || "/"} via ${result.endpoint}.`, ...current]);
    } catch (error) {
      setLog((current) => [`Browse failed: ${String(error)}`, ...current]);
    } finally {
      setRemoteBusy("");
    }
  }

  async function renameRemote() {
    if (!selectedStation || !renameSourcePath.trim() || !renameTargetPath.trim()) {
      return;
    }

    setRemoteBusy("rename");
    try {
      const message = await invoke<string>("rename_station_file_for_ui", {
        id: selectedStation.id,
        oldPath: renameSourcePath,
        newPath: renameTargetPath
      });
      setLog((current) => [message, ...current]);
      await browseRemote(selectedBrowser?.requestedPath ?? remotePath);
    } catch (error) {
      setLog((current) => [`Rename failed: ${String(error)}`, ...current]);
    } finally {
      setRemoteBusy("");
    }
  }

  async function downloadRemote() {
    if (!selectedStation || !renameSourcePath.trim() || !downloadLocalPath.trim()) {
      return;
    }

    setRemoteBusy("download");
    try {
      const message = await invoke<string>("download_station_file_for_ui", {
        id: selectedStation.id,
        remotePath: renameSourcePath,
        localPath: downloadLocalPath
      });
      setLog((current) => [message, ...current]);
    } catch (error) {
      setLog((current) => [`Download failed: ${String(error)}`, ...current]);
    } finally {
      setRemoteBusy("");
    }
  }

  async function uploadRemote() {
    if (!selectedStation || !uploadLocalPath.trim() || !renameTargetPath.trim()) {
      return;
    }

    setRemoteBusy("upload");
    try {
      const message = await invoke<string>("upload_station_file_for_ui", {
        id: selectedStation.id,
        localPath: uploadLocalPath,
        remotePath: renameTargetPath
      });
      setLog((current) => [message, ...current]);
      await browseRemote(selectedBrowser?.requestedPath ?? remotePath);
    } catch (error) {
      setLog((current) => [`Upload failed: ${String(error)}`, ...current]);
    } finally {
      setRemoteBusy("");
    }
  }

  async function batchDownloadAll() {
    if (!renameSourcePath.trim() || !downloadLocalPath.trim()) {
      setLog((current) => ["Batch download requires remote source path and local download path.", ...current]);
      return;
    }

    setRemoteBusy("batchDownload");
    let success = 0;
    let failed = 0;
    try {
      for (const station of stations) {
        try {
          const message = await invoke<string>("download_station_file_for_ui", {
            id: station.id,
            remotePath: renameSourcePath,
            localPath: downloadLocalPath
          });
          setLog((current) => [`[${station.name}] ${message}`, ...current]);
          success += 1;
        } catch (error) {
          setLog((current) => [`[${station.name}] Download failed: ${String(error)}`, ...current]);
          failed += 1;
        }
      }
      setLog((current) => [`Batch download finished: ${success} success, ${failed} failed.`, ...current]);
    } finally {
      setRemoteBusy("");
    }
  }

  async function batchUploadAll() {
    if (!uploadLocalPath.trim() || !renameTargetPath.trim()) {
      setLog((current) => ["Batch upload requires local upload path and remote target path.", ...current]);
      return;
    }

    setRemoteBusy("batchUpload");
    let success = 0;
    let failed = 0;
    try {
      for (const station of stations) {
        try {
          const message = await invoke<string>("upload_station_file_for_ui", {
            id: station.id,
            localPath: uploadLocalPath,
            remotePath: renameTargetPath
          });
          setLog((current) => [`[${station.name}] ${message}`, ...current]);
          success += 1;
        } catch (error) {
          setLog((current) => [`[${station.name}] Upload failed: ${String(error)}`, ...current]);
          failed += 1;
        }
      }
      setLog((current) => [`Batch upload finished: ${success} success, ${failed} failed.`, ...current]);
    } finally {
      setRemoteBusy("");
    }
  }

  async function captureScreen() {
    if (!selectedStation) {
      return;
    }

    setRemoteBusy("capture");
    try {
      const capture = await invoke<StationScreenCapture>("capture_station_screen_for_ui", {
        id: selectedStation.id
      });
      setCaptureByStation((current) => ({ ...current, [selectedStation.id]: capture }));
      setLog((current) => [`Captured screen from ${capture.endpoint}.`, ...current]);
    } catch (error) {
      setLog((current) => [`Screen capture failed: ${String(error)}`, ...current]);
    } finally {
      setRemoteBusy("");
    }
  }

  async function batchCaptureScreen() {
    if (filteredStations.length === 0) {
      setLog((current) => ["No stations available for batch capture.", ...current]);
      return;
    }

    setRemoteBusy("batchCapture");
    setBatchCaptures([]);
    let success = 0;
    let failed = 0;
    const results: BatchCapture[] = [];
    try {
      for (const station of filteredStations) {
        try {
          const capture = await invoke<StationScreenCapture>("capture_station_screen_for_ui", {
            id: station.id
          });
          results.push({
            stationId: station.id,
            stationName: station.name,
            endpoint: capture.endpoint,
            byteLen: capture.byteLen,
            dataUrl: capture.dataUrl,
          });
          success += 1;
        } catch (error) {
          results.push({
            stationId: station.id,
            stationName: station.name,
            endpoint: "",
            byteLen: 0,
            dataUrl: "",
            error: String(error),
          });
          failed += 1;
        }
      }
      setBatchCaptures(results);
      setLog((current) => [
        `Batch capture finished: ${success} success, ${failed} failed.`,
        ...current
      ]);
    } finally {
      setRemoteBusy("");
    }
  }

  async function executeRemoteCommand() {
    if (!selectedStation || !commandInput.trim()) {
      return;
    }

    setRemoteBusy("command");
    setCommandOutput(null);
    try {
      const result = await invoke<CommandExecutionResult>("execute_station_command_for_ui", {
        id: selectedStation.id,
        command: commandInput.trim(),
        timeoutSeconds: commandTimeout
      });
      setCommandOutput(result);
      setLog((current) => [
        `Command "${commandInput.trim().slice(0, 60)}" on ${selectedStation.name} → exit ${result.exitCode}.`,
        ...current
      ]);
    } catch (error) {
      setLog((current) => [`Remote command failed: ${String(error)}`, ...current]);
    } finally {
      setRemoteBusy("");
    }
  }

  async function loadGroups() {
    try {
      const result = await invoke<StationGroup[]>("get_station_groups");
      setGroups(result);
    } catch (error) {
      setLog((current) => [`Failed to load groups: ${String(error)}`, ...current]);
    }
  }

  async function loadTagDefinitions() {
    try {
      const result = await invoke<TagDefinition[]>("load_tag_definitions");
      setTagDefinitions(result);
    } catch (error) {
      setLog((current) => [`Failed to load tag definitions: ${String(error)}`, ...current]);
    }
  }

  async function createGroup(name: string) {
    try {
      const group = await invoke<StationGroup>("create_station_group", {
        name,
        description: "",
        color: "#3b82f6",
        icon: null,
        stationIds: [],
      });
      setGroups((current) => [...current, group]);
      setLog((current) => [`Group "${name}" created.`, ...current]);
    } catch (error) {
      setLog((current) => [`Failed to create group: ${String(error)}`, ...current]);
    }
  }

  async function updateGroup(group: StationGroup) {
    try {
      const updated = await invoke<StationGroup>("update_station_group", {
        id: group.id,
        name: group.name,
        description: group.description,
        color: group.color ?? "#3b82f6",
        icon: group.icon ?? null,
        stationIds: group.stationIds,
      });
      setGroups((current) =>
        current.map((g) => (g.id === updated.id ? updated : g))
      );
      setEditingGroup(null);
      setLog((current) => [`Group "${updated.name}" updated.`, ...current]);
    } catch (error) {
      setLog((current) => [`Failed to update group: ${String(error)}`, ...current]);
    }
  }

  async function deleteGroup(id: string) {
    try {
      await invoke<string>("delete_station_group", { id });
      setGroups((current) => current.filter((g) => g.id !== id));
      setLog((current) => [`Group deleted.`, ...current]);
    } catch (error) {
      setLog((current) => [`Failed to delete group: ${String(error)}`, ...current]);
    }
  }

  function addStation() {
    const station = emptyStation();
    setStations((current) => [station, ...current]);
    setSelectedId(station.id);
    setActivePage("stations");
    setDetailTab("edit");
    setViewMode("split");
  }

  function removeSelectedStation() {
    if (!selectedStation) {
      return;
    }
    setStations((current) =>
      current.filter((station) => station.id !== selectedStation.id)
    );
    setSelectedId("");
    setDetailTab("overview");
  }

  function patchSelected(patch: Partial<Station>) {
    if (!selectedStation) {
      return;
    }
    updateStation({ ...selectedStation, ...patch });
  }

  function patchPrograms(nextPrograms: StartProgram[]) {
    patchSelected({ startPrograms: nextPrograms });
  }

  async function patchSelectedGroups(nextGroupIds: string[]) {
    if (!selectedStation) {
      return;
    }

    const previousGroupIds = selectedStation.groups ?? [];
    const uniqueGroupIds = Array.from(new Set(nextGroupIds));

    patchSelected({ groups: uniqueGroupIds });

    setGroups((current) =>
      current.map((group) => {
        const stationIds = group.stationIds ?? [];
        const shouldContainStation = uniqueGroupIds.includes(group.id);
        const alreadyContainsStation = stationIds.includes(selectedStation.id);

        if (shouldContainStation && !alreadyContainsStation) {
          return { ...group, stationIds: [...stationIds, selectedStation.id] };
        }

        if (!shouldContainStation && alreadyContainsStation) {
          return {
            ...group,
            stationIds: stationIds.filter((id) => id !== selectedStation.id),
          };
        }

        return group;
      }),
    );

    try {
      const previousSet = new Set(previousGroupIds);
      const nextSet = new Set(uniqueGroupIds);

      for (const groupId of uniqueGroupIds) {
        if (!previousSet.has(groupId)) {
          await invoke("add_station_to_group", {
            group_id: groupId,
            station_id: selectedStation.id,
            groupId,
            stationId: selectedStation.id,
          });
        }
      }

      for (const groupId of previousGroupIds) {
        if (!nextSet.has(groupId)) {
          await invoke("remove_station_from_group", {
            group_id: groupId,
            station_id: selectedStation.id,
            groupId,
            stationId: selectedStation.id,
          });
        }
      }

      await loadGroups();
    } catch (error) {
      setLog((current) => [
        `Failed to sync station-group membership: ${String(error)}`,
        ...current,
      ]);
    }
  }

  async function patchSelectedTagValue(tagKey: string, value: string) {
    if (!selectedStation) {
      return;
    }

    const nextTags = { ...(selectedStation.tags ?? {}) };
    if (!value.trim()) {
      delete nextTags[tagKey];
    } else {
      nextTags[tagKey] = value;
    }

    patchSelected({ tags: nextTags });

    try {
      await invoke("update_station_tags", {
        station_id: selectedStation.id,
        stationId: selectedStation.id,
        tags: nextTags,
      });
    } catch (error) {
      setLog((current) => [
        `Failed to persist station tags: ${String(error)}`,
        ...current,
      ]);
    }
  }

  function updatePendingStationTagValue(tagKey: string, value: string) {
    setPendingStationTagValues((current) => ({ ...current, [tagKey]: value }));
    setDirtyStationTagKeys((current) => ({ ...current, [tagKey]: true }));
  }

  async function savePendingStationTagValue(tagKey: string) {
    if (!selectedStation) {
      return;
    }

    const nextValue = pendingStationTagValues[tagKey] ?? ((selectedStation.tags ?? {})[tagKey] ?? "");
    await patchSelectedTagValue(tagKey, nextValue);
    setDirtyStationTagKeys((current) => ({ ...current, [tagKey]: false }));
  }

  function selectStation(stationId: string) {
    setSelectedId(stationId);
    setDetailTab("overview");
  }

  function editStation(stationId: string) {
    setSelectedId(stationId);
    setDetailTab("edit");
    setViewMode("split");
  }

  function clearStationFilters() {
    setSearch("");
    setGroupFilter("");
    setTagKeyFilter("");
    setTagValueFilter("");
  }

  const stationsWorkspaceColumns = (() => {
    if (isStationsStacked) {
      return undefined;
    }

    const leftTrack =
      stationPanelWidth !== null
        ? `${stationPanelWidth}px`
        : viewMode === "split"
          ? "minmax(0, 0.9fr)"
          : "minmax(0, 0.95fr)";
    const rightTrack = viewMode === "split" ? "minmax(360px, 1.2fr)" : "minmax(360px, 1.3fr)";
    return `${leftTrack} ${STATIONS_PANEL_RESIZER_WIDTH}px ${rightTrack}`;
  })();

  function startPanelResize(event: React.PointerEvent<HTMLDivElement>) {
    if (isStationsStacked || !stationsWorkspaceRef.current) {
      return;
    }

    event.preventDefault();
    const rect = stationsWorkspaceRef.current.getBoundingClientRect();
    const maxLeft = Math.max(
      MIN_BROWSER_PANEL_WIDTH,
      rect.width - MIN_DETAIL_PANEL_WIDTH - STATIONS_PANEL_RESIZER_WIDTH,
    );
    const next = Math.min(Math.max(event.clientX - rect.left, MIN_BROWSER_PANEL_WIDTH), maxLeft);
    setStationPanelWidth(Math.round(next));
    resizingPanelsRef.current = true;
    setIsResizingPanels(true);
  }

  function resizePanelsWithKeyboard(step: number) {
    if (isStationsStacked || !stationsWorkspaceRef.current) {
      return;
    }

    const rect = stationsWorkspaceRef.current.getBoundingClientRect();
    const maxLeft = Math.max(
      MIN_BROWSER_PANEL_WIDTH,
      rect.width - MIN_DETAIL_PANEL_WIDTH - STATIONS_PANEL_RESIZER_WIDTH,
    );
    const baseWidth = stationPanelWidth ?? Math.round(rect.width * (viewMode === "split" ? 0.43 : 0.42));
    const next = Math.min(Math.max(baseWidth + step, MIN_BROWSER_PANEL_WIDTH), maxLeft);
    setStationPanelWidth(Math.round(next));
  }

  async function handleExecuteBatchTask(
    taskId: string,
    targetIds: string[],
    parameters?: Record<string, string>,
  ) {
    return executeTask(taskId, targetIds, parameters);
  }

  return (
    <AlertProvider>
    <div className="shell">
      <header className={`hero ${hasRuntimeData ? "hero--compact" : "hero--expanded"}`}>
        <div
          className={`heroBanner ${hasStationMessage ? "heroBanner--smallVisible" : "heroBanner--largeVisible"}`}
          role="img"
          aria-label="CC-rClient top banner"
        >
          <img
            src={topBanner}
            alt=""
            aria-hidden="true"
            className="heroBannerImage heroBannerImage--large"
          />
          <img
            src={topBannerSmall}
            alt=""
            aria-hidden="true"
            className="heroBannerImage heroBannerImage--small"
          />
        </div>
      </header>

      <section className="toolbar">
        {/* Navigation */}
        <button
          className={activePage === "stations" ? "accent" : ""}
          onClick={() => setActivePage("stations")}
        >
          Devices
        </button>
        <button
          className={activePage === "settings" ? "accent" : ""}
          onClick={() => setActivePage("settings")}
        >
          Settings
        </button>
        <button
          className={activePage === "groups" ? "accent" : ""}
          onClick={() => {
            setActivePage("groups");
            void loadGroups();
          }}
        >
          Groups
        </button>
        <button
          className={activePage === "tags" ? "accent" : ""}
          onClick={() => {
            setActivePage("tags");
            void loadTagDefinitions();
          }}
        >
          Tags
        </button>
        <button
          className={activePage === "scripts" ? "accent" : ""}
          onClick={() => setActivePage("scripts")}
        >
          Scripts
        </button>
        <button
          className={activePage === "batch" ? "accent" : ""}
          onClick={() => setActivePage("batch")}
        >
          Batch
        </button>
        <button
          className={activePage === "alerts" ? "accent" : ""}
          onClick={() => setActivePage("alerts")}
        >
          Alerts
        </button>

        {toolbarBatchTasks.length > 0 && (
          <>
            <div className="toolbar-divider" />
            <div className="toolbar-batch-shortcuts" aria-label="Pinned batch tasks">
              {toolbarBatchTasks.map((task) => (
                <button
                  key={task.id}
                  className="toolbar-batch-shortcut"
                  onClick={() => openRunner(task)}
                  title={`Open batch task: ${task.name}`}
                >
                  {task.name}
                </button>
              ))}
            </div>
          </>
        )}

        <div className="toolbar-divider" />

        {/* Device Actions */}
        <button onClick={addStation}>+ Add</button>
        <button onClick={removeSelectedStation} disabled={!selectedStation}>
          Remove
        </button>

        <div className="toolbar-divider" />

        {/* System */}
        <button className="accent" onClick={() => void saveState()} disabled={saving}>
          {saving ? "Saving..." : "Save"}
        </button>
      </section>

      {activePage === "stations" ? (
        <main
          ref={stationsWorkspaceRef}
          className={[
            "stationsWorkspace",
            `stationsWorkspace--${viewMode}`,
            !isStationsStacked ? "stationsWorkspace--resizable" : "",
            isResizingPanels ? "stationsWorkspace--resizing" : "",
          ].join(" ")}
          style={stationsWorkspaceColumns ? { gridTemplateColumns: stationsWorkspaceColumns } : undefined}
        >
          <StationBrowserPanel
            filteredStations={filteredStations}
            totalStationCount={stations.length}
            groups={groups}
            tagDefinitions={tagDefinitions}
            runtimeByStation={runtimeByStation}
            loading={loading}
            selectedId={selectedId}
            search={search}
            sortBy={sortBy}
            sortDirection={sortDirection}
            groupFilter={groupFilter}
            tagKeyFilter={tagKeyFilter}
            tagValueFilter={tagValueFilter}
            tagValueOptions={tagValueOptions}
            viewMode={viewMode}
            listColumns={listColumns}
            panelMaxHeight={stationPanelsMaxHeight}
            hasActiveFilter={hasActiveFilter}
            filtersHideStations={filtersHideStations}
            selectedStationHiddenByFilters={selectedStationHiddenByFilters}
            onSearchChange={setSearch}
            onSortChange={setSortBy}
            onSortDirectionChange={setSortDirection}
            onGroupFilterChange={setGroupFilter}
            onTagKeyFilterChange={(value) => {
              setTagKeyFilter(value);
              setTagValueFilter("");
            }}
            onTagValueFilterChange={setTagValueFilter}
            onViewModeChange={setViewMode}
            onListColumnsChange={setListColumns}
            gridColumns={gridColumns}
            onGridColumnsChange={setGridColumns}
            onSelectStation={selectStation}
            onEditStation={editStation}
            onClearFilters={clearStationFilters}
          />

          <div
            className={`stationsPanelResizer ${isResizingPanels ? "isDragging" : ""}`}
            role="separator"
            aria-orientation="vertical"
            aria-label="Resize device and detail panels"
            aria-valuemin={MIN_BROWSER_PANEL_WIDTH}
            aria-valuemax={2000}
            aria-valuenow={stationPanelWidth ?? undefined}
            tabIndex={isStationsStacked ? -1 : 0}
            onPointerDown={startPanelResize}
            onKeyDown={(event) => {
              if (event.key === "ArrowLeft") {
                event.preventDefault();
                resizePanelsWithKeyboard(-24);
              } else if (event.key === "ArrowRight") {
                event.preventDefault();
                resizePanelsWithKeyboard(24);
              }
            }}
          />

          <section
            className="panel stationsDetailPanel"
            style={stationPanelsMaxHeight ? { maxHeight: `${stationPanelsMaxHeight}px` } : undefined}
          >
            <div className="panelHeader stationsDetailHeader">
              <div>
                <h2>{detailTab === "edit" ? "Device Editor" : "Runtime & Tools"}</h2>
                {selectedStation ? (
                  <span className="hint">
                    {selectedStation.id}
                    {selectedStation.lastAction ? ` | ${selectedStation.lastAction}` : ""}
                  </span>
                ) : (
                  <span className="hint">Select a device to inspect or edit it.</span>
                )}
              </div>
              <div className="stationsDetailTabs" role="tablist" aria-label="Device detail mode">
                <button
                  type="button"
                  className={detailTab === "overview" ? "accent" : ""}
                  onClick={() => setDetailTab("overview")}
                  aria-pressed={detailTab === "overview"}
                >
                  Overview
                </button>
                <button
                  type="button"
                  className={detailTab === "edit" ? "accent" : ""}
                  onClick={() => setDetailTab("edit")}
                  aria-pressed={detailTab === "edit"}
                  disabled={!selectedStation}
                >
                  Edit
                </button>
              </div>
            </div>

            {selectedStationHiddenByFilters ? (
              <p className="stationsHiddenSelectionNotice stationsHiddenSelectionNotice--panel">
                The selected device is outside the current filter results. Clear filters or pick a visible device.
              </p>
            ) : null}

            {!selectedStation ? (
              <p className="emptyState">Select or create a device to continue.</p>
            ) : detailTab === "edit" ? (
              <div className="detailLayout">

                <label className="field">
                  <span>Name</span>
                  <input
                    value={selectedStation.name}
                    onChange={(event) => patchSelected({ name: event.target.value })}
                  />
                </label>

                <div className="collection">
                  <div className="subHeader">
                    <h3>Network Interfaces</h3>
                    <button
                      onClick={() =>
                        patchSelected({
                          networkInterfaces: [
                            ...selectedStation.networkInterfaces,
                            { mac: "", ips: [""] }
                          ]
                        })
                      }
                    >
                      Add NIC
                    </button>
                  </div>
                  {selectedStation.networkInterfaces.map((ni, index) => (
                    <div key={`${selectedStation.id}-ni-${index}`} className="cardGrid">
                      <label className="field">
                        <span>MAC</span>
                        <input
                          value={ni.mac}
                          onChange={(event) => {
                            const next = [...selectedStation.networkInterfaces];
                            next[index] = { ...ni, mac: event.target.value };
                            patchSelected({ networkInterfaces: next });
                          }}
                        />
                      </label>
                      <label className="field wide">
                        <span>IPs</span>
                        <input
                          value={ni.ips.join(", ")}
                          onChange={(event) => {
                            const next = [...selectedStation.networkInterfaces];
                            next[index] = {
                              ...ni,
                              ips: event.target.value
                                .split(",")
                                .map((value) => value.trim())
                                .filter(Boolean)
                            };
                            patchSelected({ networkInterfaces: next });
                          }}
                        />
                      </label>
                    </div>
                  ))}
                </div>

                <div className="collection">
                  <div className="subHeader">
                    <h3>Startup Programs</h3>
                    <button
                      onClick={() =>
                        patchPrograms([
                          ...selectedStation.startPrograms,
                          {
                            path: "",
                            arguments: "",
                            processName: "",
                            allowMultiInstance: false
                          }
                        ])
                      }
                    >
                      Add Program
                    </button>
                  </div>
                  {selectedStation.startPrograms.length === 0 ? (
                    <p className="emptyInline">No station-specific startup programs.</p>
                  ) : (
                    selectedStation.startPrograms.map((program, index) => (
                      <div key={`${selectedStation.id}-program-${index}`} className="programCard">
                        <label className="field wide">
                          <span>Path</span>
                          <input
                            value={program.path}
                            onChange={(event) => {
                              const next = [...selectedStation.startPrograms];
                              next[index] = { ...program, path: event.target.value };
                              patchPrograms(next);
                            }}
                          />
                        </label>
                        <label className="field">
                          <span>Arguments</span>
                          <input
                            value={program.arguments}
                            onChange={(event) => {
                              const next = [...selectedStation.startPrograms];
                              next[index] = { ...program, arguments: event.target.value };
                              patchPrograms(next);
                            }}
                          />
                        </label>
                        <label className="field">
                          <span>Process Name</span>
                          <input
                            value={program.processName}
                            onChange={(event) => {
                              const next = [...selectedStation.startPrograms];
                              next[index] = { ...program, processName: event.target.value };
                              patchPrograms(next);
                            }}
                          />
                        </label>
                        <label className="checkField">
                          <input
                            type="checkbox"
                            checked={program.allowMultiInstance}
                            onChange={(event) => {
                              const next = [...selectedStation.startPrograms];
                              next[index] = {
                                ...program,
                                allowMultiInstance: event.target.checked
                              };
                              patchPrograms(next);
                            }}
                          />
                          <span>Allow multi-instance</span>
                        </label>
                      </div>
                    ))
                  )}
                </div>

                <label className="field">
                  <span>Monitor Processes</span>
                  <textarea
                    rows={5}
                    value={selectedStation.monitorProcesses.join("\n")}
                    onChange={(event) =>
                      patchSelected({
                        monitorProcesses: event.target.value
                          .split("\n")
                          .map((value) => value.trim())
                          .filter(Boolean)
                      })
                    }
                  />
                </label>

                <div className="collection">
                  <div className="subHeader">
                    <h3>Group Membership</h3>
                  </div>
                  {groups.length === 0 ? (
                    <p className="emptyInline">No groups defined yet. Create groups in the Groups page.</p>
                  ) : (
                    <div className="cardGrid">
                      {groups.map((group) => {
                        const checked = (selectedStation.groups ?? []).includes(group.id);
                        return (
                          <label key={group.id} className="checkField" style={{ alignSelf: "end" }}>
                            <input
                              type="checkbox"
                              checked={checked}
                              onChange={(event) => {
                                if (event.target.checked) {
                                  void patchSelectedGroups([...(selectedStation.groups ?? []), group.id]);
                                } else {
                                  void patchSelectedGroups((selectedStation.groups ?? []).filter((id) => id !== group.id));
                                }
                              }}
                            />
                            <span>{group.name}</span>
                          </label>
                        );
                      })}
                    </div>
                  )}
                </div>

                <div className="collection">
                  <div className="subHeader">
                    <h3>Device Tags</h3>
                  </div>
                  {tagDefinitions.length === 0 ? (
                    <p className="emptyInline">No tag definitions yet. Create tags in the Tags page.</p>
                  ) : (
                    <div className="cardGrid">
                      {tagDefinitions.map((definition) => {
                        const tagKey = definition.key || definition.id;
                        const tagLabel = definition.label || definition.name || tagKey;
                        const currentValue = (selectedStation.tags ?? {})[tagKey] ?? "";

                        if (definition.type === "boolean") {
                          return (
                            <label className="field" key={tagKey}>
                              <span>{tagLabel}</span>
                              <select
                                value={currentValue}
                                onChange={(event) => void patchSelectedTagValue(tagKey, event.target.value)}
                              >
                                <option value="">Unset</option>
                                <option value="true">True</option>
                                <option value="false">False</option>
                              </select>
                            </label>
                          );
                        }

                        if (definition.type === "select" && definition.options && definition.options.length > 0) {
                          return (
                            <label className="field" key={tagKey}>
                              <span>{tagLabel}</span>
                              <select
                                value={currentValue}
                                onChange={(event) => void patchSelectedTagValue(tagKey, event.target.value)}
                              >
                                <option value="">Unset</option>
                                {definition.options.map((option) => (
                                  <option key={option} value={option}>
                                    {option}
                                  </option>
                                ))}
                              </select>
                            </label>
                          );
                        }

                        return (
                          <label className="field" key={tagKey}>
                            <span>{tagLabel}</span>
                            <div className="tagValueInlineEditor">
                              <input
                                type={definition.type === "number" ? "number" : "text"}
                                value={dirtyStationTagKeys[tagKey] ? (pendingStationTagValues[tagKey] ?? "") : currentValue}
                                onChange={(event) => updatePendingStationTagValue(tagKey, event.target.value)}
                                placeholder={`Value for ${tagLabel}`}
                              />
                              {dirtyStationTagKeys[tagKey] ? (
                                <button
                                  type="button"
                                  className="accent tagValueSaveButton"
                                  onClick={() => void savePendingStationTagValue(tagKey)}
                                >
                                  Save
                                </button>
                              ) : null}
                            </div>
                          </label>
                        );
                      })}
                    </div>
                  )}
                </div>

                <div className="collection">
                  <TelemetryProfilesEditor
                    title="Telemetry"
                    subtitle="Per-device MQTT telemetry profiles. Each profile gathers and publishes on its own interval."
                    profiles={stationTelemetryProfiles}
                    supportedIncludes={telemetrySchema?.supportedIncludes ?? []}
                    loading={stationTelemetryLoading}
                    saving={stationTelemetrySaving}
                    error={stationTelemetryError}
                    notice={stationTelemetryNotice}
                    validationErrors={stationTelemetryValidationErrors}
                    validationWarnings={stationTelemetryValidationWarnings}
                    emptyState="No telemetry profiles configured for this device."
                    onChange={(profiles) => {
                      setStationTelemetryNotice(null);
                      setStationTelemetryError(null);
                      setStationTelemetryProfiles(profiles);
                    }}
                    onSave={() => void saveStationTelemetryProfiles()}
                    saveLabel="Save Device Telemetry"
                    savePlacement="footer"
                  />
                </div>
              </div>
            ) : (
              <>
                <div className="collection">
                  <div className="subHeader">
                    <h3>Runtime</h3>
                    <span>{selectedRuntime ? `${selectedRuntime.telemetrySource.toUpperCase()} live feed` : "MQTT/WebSocket only"}</span>
                  </div>
                  {!selectedRuntime ? (
                    <p className="emptyInline">No live MQTT runtime data received yet.</p>
                  ) : (
                    <div className="programCard">
                  <div className="statsGrid">
                    <div className="statTile">
                      <span>Remote Device ID</span>
                      <strong>{selectedRuntime.stationId || "n/a"}</strong>
                    </div>
                    <CpuPie cpu={selectedRuntime.cpu} />
                    <div className="statTile">
                      <span>Memory</span>
                      <strong>{formatRuntimeMemory(selectedRuntime)}</strong>
                    </div>
                    <div className="statTile">
                      <span>Processes</span>
                      <strong>{selectedRuntime.procCount}</strong>
                      <ProcessCountMiniChart history={historyByStation[selectedStation.id] ?? []} />
                    </div>
                    <div className="statTile">
                      <span>Disk Usage</span>
                      <strong>
                        {selectedRuntime.storageStats[0]
                          ? `${selectedRuntime.storageStats[0].usagePercent.toFixed(0)}%`
                          : getRuntimeUnavailableText(selectedRuntime, "Storage")}
                      </strong>
                    </div>
                  </div>
                  <div className="collection">
                    <div className="subHeader">
                      <h3>Performance (30-point history)</h3>
                    </div>
                    <PerformanceCharts
                      history={historyByStation[selectedStation.id] ?? []}
                      totalMemory={selectedRuntime.totalMemory}
                    />
                  </div>
                  <div className="runtimeEndpointRow">
                    <div className="runtimeEndpointRowGroup">
                      <span>Endpoint</span>
                      <strong>{selectedRuntime.endpoint}</strong>
                    </div>
                    <div className="runtimeEndpointRowGroup runtimeEndpointRowGroup--meta">
                      <span>Detail Level</span>
                      <strong>{runtimeHasFullDetails(selectedRuntime) ? "Full runtime" : "Basic runtime"}</strong>
                    </div>
                  </div>
                  <div className="logEntry">{selectedRuntime.message}</div>
                  <div className="logEntry">
                    {selectedRuntime.computerName || "Unknown host"} ·{" "}
                    {selectedRuntime.osName
                      ? `${selectedRuntime.osName} ${selectedRuntime.osVersion}`.trim()
                      : getRuntimeUnavailableText(selectedRuntime, "OS details")}
                  </div>
                  <div className="logEntry">
                    Service version: {selectedRuntime.serviceVersion || getRuntimeUnavailableText(selectedRuntime, "Service version")}
                  </div>
                  <div className="logEntry">
                    Service path: {selectedRuntime.servicePath || getRuntimeUnavailableText(selectedRuntime, "Service path")}
                  </div>
                  <div className="logEntry">
                    Launcher path: {selectedRuntime.appLauncherPath || getRuntimeUnavailableText(selectedRuntime, "Launcher path")}
                  </div>
                  <div className="collection">
                    <div className="subHeader">
                      <h3>Watched Apps</h3>
                    </div>
                    {selectedRuntime.appStates.length === 0 ? (
                      <p className="emptyInline">
                        {getRuntimeUnavailableText(selectedRuntime, "Watched app state")}
                      </p>
                    ) : (
                      selectedRuntime.appStates.map((item) => (
                        <div key={`${item.monitorName}-${item.processId}`} className="logEntry">
                          {item.monitorName || item.processName || "Unknown process"} ·{" "}
                          {item.isRunning ? "Running" : "Stopped"} · CPU {item.cpu.toFixed(1)}% · Mem{" "}
                          {formatBytes(item.currentMemory)}
                        </div>
                      ))
                    )}
                  </div>
                  <div className="collection">
                    <div className="subHeader">
                      <h3>Network</h3>
                    </div>
                    {selectedRuntime.networkStats.length === 0 ? (
                      <p className="emptyInline">
                        {getRuntimeUnavailableText(selectedRuntime, "Network counters")}
                      </p>
                    ) : (
                      selectedRuntime.networkStats.map((item) => (
                        <div key={item.ifName} className="logEntry">
                          <div>
                            {item.ifName} · Total {formatBytes(item.totalBytesPerSec)}/s · RX {formatBytes(item.bytesReceivedPerSec)}/s · TX {formatBytes(item.bytesSentedPerSec)}/s
                          </div>
                          <div>
                            Uni RX {item.unicastPacketReceived} · Uni TX {item.unicastPacketSented} · Multi RX {item.multicastPacketReceived} · Multi TX {item.multicastPacketSented}
                          </div>
                          <div>
                            RX Total {formatBytes(item.bytesReceived)} · TX Total {formatBytes(item.bytesSented)} · Total {formatBytes(item.bytesTotal)}
                          </div>
                        </div>
                      ))
                    )}
                  </div>
                  <div className="collection">
                    <div className="subHeader">
                      <h3>Storage</h3>
                    </div>
                    {selectedRuntime.storageStats.length === 0 ? (
                      <p className="emptyInline">
                        {getRuntimeUnavailableText(selectedRuntime, "Storage metrics")}
                      </p>
                    ) : (
                      selectedRuntime.storageStats.map((item) => (
                        <div key={item.mountPoint} className="logEntry">
                          {item.mountPoint} · Used {formatBytes(item.usedBytes)} / {formatBytes(item.totalBytes)} · Free{" "}
                          {formatBytes(item.availableBytes)} · {item.usagePercent.toFixed(1)}%
                        </div>
                      ))
                    )}
                  </div>
                    </div>
                  )}
                </div>

                <div className="collection">
              <div className="subHeader">
                <h3>Screen Capture</h3>
                <button onClick={() => void captureScreen()} disabled={!selectedStation || remoteBusy === "capture"}>
                  {remoteBusy === "capture" ? "Capturing..." : "Capture"}
                </button>
              </div>
              {!selectedCapture ? (
                <p className="emptyInline">No remote screenshot captured yet.</p>
              ) : (
                <div className="programCard">
                  <div className="logEntry">
                    {selectedCapture.endpoint} · {formatBytes(selectedCapture.byteLen)}
                  </div>
                  <img className="capturePreview" src={selectedCapture.dataUrl} alt="Remote station capture" />
                </div>
              )}
                </div>

                <div className="collection">
              <div className="subHeader">
                <h3>Remote Command</h3>
                <button onClick={() => void executeRemoteCommand()} disabled={!selectedStation || remoteBusy === "command"}>
                  {remoteBusy === "command" ? "Executing..." : "Execute"}
                </button>
              </div>
              <label className="field">
                <span>Command</span>
                <input
                  value={commandInput}
                  onChange={(event) => setCommandInput(event.target.value)}
                  placeholder="e.g. uname -a, df -h, ps aux"
                  onKeyDown={(event) => { if (event.key === "Enter") void executeRemoteCommand(); }}
                />
              </label>
              <label className="field">
                <span>Timeout (seconds)</span>
                <input
                  type="number"
                  min={1}
                  max={600}
                  value={commandTimeout}
                  onChange={(event) => setCommandTimeout(Math.max(1, Math.min(600, Number(event.target.value))))}
                />
              </label>
              {commandOutput ? (
                <div className="programCard">
                  <div className="logEntry">
                    <strong>Exit code:</strong> {commandOutput.exitCode}
                  </div>
                  {commandOutput.stdout ? (
                    <details>
                      <summary style={{ cursor: "pointer", userSelect: "none" }}>stdout ({commandOutput.stdout.split("\n").length} lines)</summary>
                      <pre className="commandOutput">{commandOutput.stdout}</pre>
                    </details>
                  ) : null}
                  {commandOutput.stderr ? (
                    <details>
                      <summary style={{ cursor: "pointer", userSelect: "none" }}>stderr ({commandOutput.stderr.split("\n").length} lines)</summary>
                      <pre className="commandOutput" style={{ color: "#d64545" }}>{commandOutput.stderr}</pre>
                    </details>
                  ) : null}
                </div>
              ) : null}
                </div>

                <div className="collection">
              <div className="subHeader">
                <h3>Action Log</h3>
              </div>
              <div className="logList">
                {log.length === 0 ? (
                  <p className="emptyInline">No actions yet.</p>
                ) : (
                  log.map((entry, index) => (
                    <div key={`${entry}-${index}`} className="logEntry">
                      {entry}
                    </div>
                  ))
                )}
              </div>
                </div>
              </>
            )}
          </section>
        </main>
      ) : activePage === "groups" ? (
        <GroupsProvider>
          <GroupsPage stations={stations} />
        </GroupsProvider>
      ) : activePage === "tags" ? (
        <TagsProvider>
          <TagsPage stations={stations} />
        </TagsProvider>
      ) : activePage === "scripts" ? (
        <ScriptProvider>
          <ScriptsUIProvider>
            <ScriptsPage
              stations={stations}
            />
          </ScriptsUIProvider>
        </ScriptProvider>
      ) : activePage === "batch" ? (
        <BatchPage stations={stations} />
      ) : activePage === "alerts" ? (
        <AlertUIProvider>
          <AlertRulesPage stations={stations} />
        </AlertUIProvider>
      ) : (
        <main className="grid gridSettingsMode">
          <section className="panel detailPanel">
            <div className="panelHeader">
              <h2>Client Options</h2>
              <span className="hint">Opened from the Settings button</span>
            </div>

            <div className="detailLayout">
              <div className="collection">
                <TelemetryProfilesEditor
                  title="Default Telemetry Profiles"
                  subtitle="Defaults for station telemetry. Apply them to devices after editing."
                  profiles={globalTelemetryProfiles}
                  supportedIncludes={SUPPORTED_TELEMETRY_INCLUDES}
                  loading={false}
                  saving={globalTelemetrySaving}
                  error={globalTelemetryError}
                  notice={globalTelemetryNotice}
                  validationErrors={globalTelemetryValidationErrors}
                  validationWarnings={globalTelemetryValidationWarnings}
                  emptyState="No default telemetry profiles configured."
                  onChange={updateDefaultTelemetryProfiles}
                  onSave={() => void applyDefaultTelemetryProfilesToAllStations()}
                  saveLabel="Apply Default Profile to All Devices"
                />
                <div className="timingFieldRow">
                  <label className="field">
                    <span>Device Telemetry Collection Interval (ms)</span>
                    <input
                      type="number"
                      min={1000}
                      step={1000}
                      value={Math.max(1, options.interval) * 1000}
                      onChange={(event) =>
                        setOptions((current) => ({
                          ...current,
                          interval: Math.max(1, Math.round(Number(event.target.value) / 1000)),
                        }))
                      }
                    />
                    <small className="fieldHint">
                      Used as the station-wide gathering interval pushed to devices. Whole seconds only.
                    </small>
                  </label>
                  <button
                    className="accent"
                    onClick={async () => {
                      try {
                        const msg = await invoke<string>(
                          "set_station_gathering_interval_for_ui",
                          { intervalSeconds: options.interval }
                        );
                        setLog((current) => [msg, ...current]);
                      } catch (err) {
                        setLog((current) => [`Failed to set interval: ${String(err)}`, ...current]);
                      }
                    }}
                  >
                    Save Interval to Devices
                  </button>
                </div>
              </div>

              <label className="checkField">
                <input
                  type="checkbox"
                  checked={options.isFirstTimeRun}
                  onChange={(event) =>
                    setOptions((current) => ({
                      ...current,
                      isFirstTimeRun: event.target.checked
                    }))
                  }
                />
                <span>First run flow still enabled</span>
              </label>

              <label className="field">
                <span>Global Monitor Processes</span>
                <textarea
                  rows={8}
                  value={options.monitorProcesses.join("\n")}
                  onChange={(event) =>
                    setOptions((current) => ({
                      ...current,
                      monitorProcesses: event.target.value
                        .split("\n")
                        .map((value) => value.trim())
                        .filter(Boolean)
                    }))
                  }
                />
              </label>

            </div>
          </section>

          <section className="panel sidePanel">
            <div className="panelHeader">
              <h2>Settings Summary</h2>
              <span className="hint">Client-only configuration page</span>
            </div>

            <div className="programCard">
              <div className="statsGrid">
                <div className="statTile">
                  <span>Monitor Interval</span>
                  <strong>{options.interval}s</strong>
                </div>
                <div className="statTile">
                  <span>First Run</span>
                  <strong>{options.isFirstTimeRun ? "Enabled" : "Disabled"}</strong>
                </div>
                <div className="statTile">
                  <span>Global Monitors</span>
                  <strong>{options.monitorProcesses.length}</strong>
                </div>
                <div className="statTile">
                  <span>Default Profiles</span>
                  <strong>{globalTelemetryProfiles.length}</strong>
                </div>
                <div className="statTile">
                  <span>Startup Apps</span>
                  <strong>{options.startApps.length}</strong>
                </div>
              </div>
            </div>

            <div className="collection">
              <div className="subHeader">
                <h3>Action Log</h3>
              </div>
              <div className="logList">
                {log.length === 0 ? (
                  <p className="emptyInline">No actions yet.</p>
                ) : (
                  log.map((entry, index) => (
                    <div key={`${entry}-${index}`} className="logEntry">
                      {entry}
                    </div>
                  ))
                )}
              </div>
            </div>
          </section>
        </main>
      )}

      {runner.open && runner.task && (
        <div className="batch-runner-layer" role="dialog" aria-modal="true">
          <div className="batch-runner-panel">
            <BatchTaskRunner
              task={runner.task}
              targets={batchTargets}
              onExecute={handleExecuteBatchTask}
              onCancel={closeRunner}
            />
          </div>
        </div>
      )}
    </div>
    </AlertProvider>
  );
}

export default function App() {
  return (
    <BatchProvider>
      <BatchUIProvider>
        <AppShell />
      </BatchUIProvider>
    </BatchProvider>
  );
}
