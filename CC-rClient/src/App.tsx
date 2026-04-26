import { invoke } from "@tauri-apps/api/core";
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
import type {
  ActionResult,
  AppSnapshot,
  BatchCapture,
  ClientOptions,
  CommandExecutionResult,
  PersistedState,
  RemoteFileBrowserResult,
  StartProgram,
  Station,
  StationAction,
  StationGroup,
  StationRuntimeSnapshot,
  StationScreenCapture
} from "./types";
import { ScriptProvider, ScriptsPage, ScriptsUIProvider } from "./plugin/script";
import { BatchProvider, useBatch } from "./plugin/batch";
import { BatchPage } from "./plugin/batch/BatchPage";
import { BatchUIProvider } from "./plugin/batch/BatchUIContext";
import { LayoutPresetSelector } from "./plugin/components/LayoutManager";
import { getAllLayoutPresets } from "./plugin/components/LayoutConfig";
import { GroupsPage, TagsPage } from "./plugin/groups";
import { GroupsProvider } from "./plugin/groups/GroupsContext";
import { TagsProvider } from "./plugin/groups/TagsContext";
import type { TagDefinition } from "./plugin/groups/types";

const emptyOptions: ClientOptions = {
  interval: 2,
  isFirstTimeRun: true,
  startApps: [],
  monitorProcesses: []
};

const emptyStation = (): Station => ({
  id: crypto.randomUUID(),
  name: "New Station",
  blocked: false,
  networkInterfaces: [{ mac: "", ips: [""] }],
  startPrograms: [],
  monitorProcesses: [],
  lastAction: null,
  groups: [],
  tags: {},
  metadata: {}
});

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

export default function App() {
  const [snapshot, setSnapshot] = useState<AppSnapshot | null>(null);
  const [stations, setStations] = useState<Station[]>([]);
  const [options, setOptions] = useState<ClientOptions>(emptyOptions);
  const [selectedId, setSelectedId] = useState<string>("");
  const [search, setSearch] = useState("");
  const [sortBy, setSortBy] = useState<"name" | "ip">("name");
  const [loading, setLoading] = useState(true);
  const [saving, setSaving] = useState(false);
  const [log, setLog] = useState<string[]>([]);
  const [runtimeByStation, setRuntimeByStation] = useState<Record<string, StationRuntimeSnapshot>>({});
  const [historyByStation, setHistoryByStation] = useState<Record<string, { cpu: number; memory: number; ts: number; rxbps: number; txbps: number }[]>>({});
  const MAX_HISTORY = 30;
  const [browserByStation, setBrowserByStation] = useState<Record<string, RemoteFileBrowserResult>>({});
  const [captureByStation, setCaptureByStation] = useState<Record<string, StationScreenCapture>>({});
  const [batchCaptures, setBatchCaptures] = useState<BatchCapture[]>([]);
  const [runtimeLoadingId, setRuntimeLoadingId] = useState<string>("");
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
  const [tagFilter, setTagFilter] = useState<string>("");
  const [tagDefinitions, setTagDefinitions] = useState<TagDefinition[]>([]);
  const [editingGroup, setEditingGroup] = useState<StationGroup | null>(null);
  const [activePage, setActivePage] = useState<"stations" | "settings" | "groups" | "tags" | "messages" | "scripts" | "batch">("stations");
  const [isEditingStationDetail, setIsEditingStationDetail] = useState(false);
  const [activeLayout, setActiveLayout] = useState<string>("default");

  useEffect(() => {
    void loadSnapshot();
  }, []);

  useEffect(() => {
    if (activePage === "stations") {
      void loadSnapshot();
    }
  }, [activePage]);

  const filteredStations = useMemo(() => {
    const lowered = search.trim().toLowerCase();
    let next = stations.filter((station) => {
      if (!lowered) {
        return true;
      }
      const allIps = station.networkInterfaces.flatMap((ni) => ni.ips);
      return (
        station.name.toLowerCase().includes(lowered) ||
        station.networkInterfaces.some((ni) => ni.mac.toLowerCase().includes(lowered)) ||
        allIps.some((ip) => ip.toLowerCase().includes(lowered))
      );
    });

    // Group filter: show only stations in the selected group
    if (groupFilter) {
      const group = groups.find((g) => g.id === groupFilter);
      if (group) {
        next = next.filter((s) => (group.stationIds || []).includes(s.id));
      }
    }

    // Tag filter: show only stations with the selected tag
    if (tagFilter) {
      next = next.filter((s) => (s.tags || {}).hasOwnProperty(tagFilter));
    }

    return next.sort((left, right) => {
      if (sortBy === "ip") {
        const leftIp = left.networkInterfaces[0]?.ips[0] ?? "";
        const rightIp = right.networkInterfaces[0]?.ips[0] ?? "";
        return leftIp.localeCompare(rightIp, undefined, { numeric: true });
      }
      return left.name.localeCompare(right.name, undefined, { numeric: true });
    });
  }, [search, sortBy, stations, groupFilter, tagFilter, groups]);

  const selectedStation =
    filteredStations.find((station) => station.id === selectedId) ??
    stations.find((station) => station.id === selectedId) ??
    null;
  const selectedRuntime = selectedStation ? runtimeByStation[selectedStation.id] ?? null : null;
  const selectedBrowser = selectedStation ? browserByStation[selectedStation.id] ?? null : null;
  const selectedCapture = selectedStation ? captureByStation[selectedStation.id] ?? null : null;
  const hasRuntimeData = Object.keys(runtimeByStation).length > 0;
  const hasActiveFilter = search.trim().length > 0 || Boolean(groupFilter) || Boolean(tagFilter);
  const filtersHideStations = stations.length > 0 && filteredStations.length === 0;

  async function loadSnapshot() {
    setLoading(true);
    try {
      const next = await invoke<AppSnapshot>("load_state");
      setSnapshot(next);
      setStations(next.stations);
      setOptions(next.options);
      setGroups(next.groups ?? []);
      setTagDefinitions(next.tags ?? []);
      setSelectedId(next.stations[0]?.id ?? "");
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
    if (!selectedStation && filteredStations[0]) {
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
    void refreshRuntime(selectedStation.id, false);
    const timer = window.setInterval(() => {
      void refreshRuntime(selectedStation.id, false);
    }, Math.max(options.interval, 1) * 1000);

    return () => window.clearInterval(timer);
  }, [selectedStation?.id, options.interval]);

  async function saveState() {
    setSaving(true);
    try {
      const payload: PersistedState = { stations, options, groups, tags: tagDefinitions };
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

  async function exportLegacyFiles() {
    try {
      const message = await invoke<string>("export_legacy_files");
      setLog((current) => [message, ...current]);
    } catch (error) {
      setLog((current) => [`Legacy export failed: ${String(error)}`, ...current]);
    }
  }

  async function executeAction(action: StationAction, ids: string[]) {
    if (ids.length === 0) {
      setLog((current) => ["Select at least one station first.", ...current]);
      return;
    }

    try {
      const result = await invoke<ActionResult>("run_station_action", {
        action,
        ids
      });
      if (result.stations) {
        setStations(result.stations);
      }
      setLog((current) => [result.message, ...current]);
    } catch (error) {
      setLog((current) => [`Action failed: ${String(error)}`, ...current]);
    }
  }

  async function refreshRuntime(targetId = selectedStation?.id, logSuccess = false) {
    if (!targetId) {
      return;
    }

    setRuntimeLoadingId(targetId);
    try {
      const runtime = await invoke<StationRuntimeSnapshot>("fetch_station_runtime_for_ui", {
        id: targetId,
        intervalSeconds: Math.max(options.interval, 1)
      });
      setRuntimeByStation((current) => ({ ...current, [targetId]: runtime }));
      setHistoryByStation((current) => {
        const prev = current[targetId] ?? [];
        const totalRx = runtime.networkStats.reduce((s, n) => s + n.bytesReceivedPerSec, 0);
        const totalTx = runtime.networkStats.reduce((s, n) => s + n.bytesSentedPerSec, 0);
        const next = [
          ...prev.slice(-(MAX_HISTORY - 1)),
          { cpu: runtime.cpu, memory: runtime.currentMemory, ts: Date.now(), rxbps: totalRx, txbps: totalTx },
        ];
        return { ...current, [targetId]: next };
      });
      if (logSuccess) {
        setLog((current) => [`Telemetry refreshed from ${runtime.endpoint}.`, ...current]);
      }
    } catch (error) {
      setLog((current) => [`Telemetry refresh failed: ${String(error)}`, ...current]);
    } finally {
      setRuntimeLoadingId((current) => (current === targetId ? "" : current));
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
      const group = await invoke<StationGroup>("create_station_group", { name });
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
        tags: group.tags,
        stationIds: group.stationIds,
        station_ids: group.stationIds,
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
    setIsEditingStationDetail(true);
  }

  function removeSelectedStation() {
    if (!selectedStation) {
      return;
    }
    setStations((current) =>
      current.filter((station) => station.id !== selectedStation.id)
    );
    setSelectedId("");
    setIsEditingStationDetail(false);
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

  function patchSelectedTagValue(tagKey: string, value: string) {
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
  }

  const selectedIds = selectedStation ? [selectedStation.id] : [];

  return (
    <div className="shell">
      <header className={`hero ${hasRuntimeData ? "hero--compact" : "hero--expanded"}`}>
        <div className="heroBanner">
          <img
            src={topBanner}
            alt="CC-rClient top banner"
            className="heroBannerImage"
          />
        </div>
      </header>

      <section className="toolbar">
        {/* Navigation */}
        <button
          className={activePage === "stations" ? "accent" : ""}
          onClick={() => setActivePage("stations")}
        >
          Stations
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

        <div className="toolbar-divider" />

        {/* Layout */}
        <LayoutPresetSelector
          currentPreset={activeLayout}
          onSelectPreset={(preset) => {
            setActiveLayout(preset);
            console.log('[App] Layout changed to:', preset);
          }}
        />

        <div className="toolbar-divider" />

        {/* Station Actions */}
        <button onClick={addStation}>+ Add</button>
        <button onClick={removeSelectedStation} disabled={!selectedStation}>
          Remove
        </button>
        <button onClick={() => void executeAction("power_on", selectedIds)} disabled={!selectedStation}>
          Power On
        </button>
        <button onClick={() => void executeAction("shutdown", selectedIds)} disabled={!selectedStation}>
          Shutdown
        </button>
        <button onClick={() => void executeAction("reboot", selectedIds)} disabled={!selectedStation}>
          Reboot
        </button>

        <div className="toolbar-divider" />

        {/* App Control */}
        <button onClick={() => void executeAction("start_app", selectedIds)} disabled={!selectedStation}>
          Start App
        </button>
        <button onClick={() => void executeAction("restart_app", selectedIds)} disabled={!selectedStation}>
          Restart App
        </button>
        <button onClick={() => void executeAction("exit_app", selectedIds)} disabled={!selectedStation}>
          Exit App
        </button>

        <div className="toolbar-divider" />

        {/* Block Control */}
        <button onClick={() => void executeAction("block", selectedIds)} disabled={!selectedStation}>
          Block
        </button>
        <button onClick={() => void executeAction("unblock", selectedIds)} disabled={!selectedStation}>
          Unblock
        </button>

        <div className="toolbar-divider" />

        {/* Batch Actions */}
        <button onClick={() => void executeAction("batch_power_on", [])} disabled={stations.length === 0}>
          全部开机
        </button>
        <button onClick={() => void executeAction("batch_shutdown", [])} disabled={stations.length === 0}>
          全部关机
        </button>
        <button onClick={() => void executeAction("batch_reboot", [])} disabled={stations.length === 0}>
          全部重启
        </button>

        <div className="toolbar-divider" />

        {/* System */}
        <button className="accent" onClick={() => void saveState()} disabled={saving}>
          {saving ? "Saving..." : "Save"}
        </button>
        <button onClick={() => void exportLegacyFiles()}>Export</button>
        <button onClick={() => void batchCaptureScreen()} disabled={filteredStations.length === 0 || remoteBusy === "batchCapture"}>
          {remoteBusy === "batchCapture" ? "截图中..." : "截图"}
        </button>
      </section>

      {activePage === "stations" ? (
        <main className={`grid ${isEditingStationDetail ? "gridDetailMode" : "gridRuntimeMode"}`}>
          <aside className="panel stationPanel">
            <div className="panelHeader">
              <h2>Stations</h2>
              <div className="panelControls">
                <input
                  value={search}
                  onChange={(event) => setSearch(event.target.value)}
                  placeholder="Filter by name, MAC, or IP"
                />
                <select
                  value={sortBy}
                  onChange={(event) => setSortBy(event.target.value as "name" | "ip")}
                >
                  <option value="name">Sort: Name</option>
                  <option value="ip">Sort: IP</option>
                </select>
                {groups.length > 0 && (
                  <select
                    value={groupFilter}
                    onChange={(event) => setGroupFilter(event.target.value)}
                  >
                    <option value="">All Groups</option>
                    {groups.map((g) => (
                      <option key={g.id} value={g.id}>{g.name}</option>
                    ))}
                  </select>
                )}
                {tagDefinitions.length > 0 && (
                  <select
                    value={tagFilter}
                    onChange={(event) => setTagFilter(event.target.value)}
                  >
                    <option value="">All Tags</option>
                    {tagDefinitions.map((t) => (
                      <option key={t.id} value={t.id}>{t.name}</option>
                    ))}
                  </select>
                )}
              </div>
            </div>

            <div className="stationList">
              {loading ? (
                <p className="emptyState">Loading state…</p>
              ) : filteredStations.length === 0 ? (
                <p className="emptyState">
                  {filtersHideStations
                    ? hasActiveFilter
                      ? "Current search/group/tag filter is hiding all stations. Clear filters to show them."
                      : "Stations exist but are currently hidden by filters."
                    : "No stations configured yet. Add a station to begin."}
                </p>
              ) : (
                filteredStations.map((station) => {
                  const ip = station.networkInterfaces[0]?.ips[0] ?? "No IP";
                  const stationVisualState = getStationVisualState(
                    station,
                    runtimeByStation[station.id] ?? null
                  );
                  return (
                    <div
                      key={station.id}
                      className={`stationCard ${station.id === selectedId ? "selected" : ""}`}
                      onClick={() => {
                        setSelectedId(station.id);
                        setIsEditingStationDetail(false);
                      }}
                    >
                      <div className="stationCardTopRow">
                        <div className="stationCardIdentity">
                          <ComputerStatusIcon state={stationVisualState} />
                          <span className="stationName">{station.name || "Unnamed Station"}</span>
                        </div>
                        <button
                          type="button"
                          className="stationEditButton"
                          onClick={(event) => {
                            event.stopPropagation();
                            setSelectedId(station.id);
                            setIsEditingStationDetail(true);
                          }}
                          aria-label={`Edit ${station.name || "station"}`}
                        >
                          Edit
                        </button>
                      </div>
                      <div className="stationMeta">
                        <span className={`badge ${stationVisualState}`}>
                          {stationVisualState === "ready"
                            ? "Ready"
                            : stationVisualState === "warning"
                              ? "Warning"
                              : stationVisualState === "error"
                                ? "Error"
                                : "Offline"}
                        </span>
                        {(runtimeByStation[station.id]) && (
                          <span className="stationStats">
                            <span className="stationStatItem" title="CPU">
                              <CpuIcon />
                              {runtimeByStation[station.id].cpu.toFixed(0)}%
                            </span>
                            <span className="stationStatItem" title="Memory">
                              <MemoryIcon />
                              {runtimeByStation[station.id].totalMemory > 0
                                ? (runtimeByStation[station.id].currentMemory / runtimeByStation[station.id].totalMemory * 100).toFixed(0)
                                : 0}%
                            </span>
                            <span className="stationStatItem" title="Processes">
                              <ProcessIcon />
                              {runtimeByStation[station.id].procCount}
                            </span>
                          </span>
                        )}
                        <span className="stationIp">{ip}</span>
                      </div>
                    </div>
                  );
                })
              )}
            </div>
          </aside>

          {isEditingStationDetail ? (
            <section className="panel detailPanel detailPanelExpanded">
              <div className="panelHeader">
                <h2>Station Detail</h2>
                {selectedStation ? (
                  <span className="hint">
                    {selectedStation.id}
                    {selectedStation.lastAction ? ` | ${selectedStation.lastAction}` : ""}
                  </span>
                ) : null}
              </div>

              {!selectedStation ? (
                <p className="emptyState">Select or create a station to edit it.</p>
              ) : (
                <div className="detailLayout">
                  <div className="subHeader">
                    <h3>Edit Station</h3>
                    <button type="button" onClick={() => setIsEditingStationDetail(false)}>
                      Close
                    </button>
                  </div>

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
                            <label key={group.id} className="checkField" style={{ alignSelf: 'end' }}>
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
                      <h3>Station Tags</h3>
                    </div>
                    {tagDefinitions.length === 0 ? (
                      <p className="emptyInline">No tag definitions yet. Create tags in the Tags page.</p>
                    ) : (
                      <div className="cardGrid">
                        {tagDefinitions.map((definition) => {
                          const tagKey = definition.key || definition.id;
                          const tagLabel = definition.label || definition.name || tagKey;
                          const currentValue = (selectedStation.tags ?? {})[tagKey] ?? '';

                          if (definition.type === 'boolean') {
                            return (
                              <label className="field" key={tagKey}>
                                <span>{tagLabel}</span>
                                <select
                                  value={currentValue}
                                  onChange={(event) => patchSelectedTagValue(tagKey, event.target.value)}
                                >
                                  <option value="">Unset</option>
                                  <option value="true">True</option>
                                  <option value="false">False</option>
                                </select>
                              </label>
                            );
                          }

                          if (definition.type === 'select' && definition.options && definition.options.length > 0) {
                            return (
                              <label className="field" key={tagKey}>
                                <span>{tagLabel}</span>
                                <select
                                  value={currentValue}
                                  onChange={(event) => patchSelectedTagValue(tagKey, event.target.value)}
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
                              <input
                                type={definition.type === 'number' ? 'number' : 'text'}
                                value={currentValue}
                                onChange={(event) => patchSelectedTagValue(tagKey, event.target.value)}
                                placeholder={`Value for ${tagLabel}`}
                              />
                            </label>
                          );
                        })}
                      </div>
                    )}
                  </div>
                </div>
              )}
            </section>
          ) : null}

          {!isEditingStationDetail ? (
            <section className="panel sidePanel sidePanelExpanded">
            <div className="panelHeader">
              <h2>Runtime & Tools</h2>
              <span className="hint">Station live data, screen capture, and remote files</span>
            </div>

            <div className="collection">
              <div className="subHeader">
                <h3>Runtime</h3>
                <button
                  onClick={() => void refreshRuntime(selectedStation?.id, true)}
                  disabled={!selectedStation || runtimeLoadingId === selectedStation.id}
                >
                  {runtimeLoadingId === selectedStation?.id ? "Refreshing..." : "Refresh"}
                </button>
              </div>
              {!selectedStation ? (
                <p className="emptyInline">Select a station to view live runtime data.</p>
              ) : !selectedRuntime ? (
                <p className="emptyInline">No live runtime data loaded yet.</p>
              ) : (
                <div className="programCard">
                  <div className="statsGrid">
                    <div className="statTile">
                      <span>Endpoint</span>
                      <strong>{selectedRuntime.endpoint}</strong>
                    </div>
                    <div className="statTile">
                      <span>Remote Station ID</span>
                      <strong>{selectedRuntime.stationId || "n/a"}</strong>
                    </div>
                    <CpuPie cpu={selectedRuntime.cpu} />
                    <div className="statTile">
                      <span>Memory</span>
                      <strong>
                        {formatBytes(selectedRuntime.currentMemory)} / {formatBytes(selectedRuntime.totalMemory)}
                      </strong>
                    </div>
                    <div className="statTile">
                      <span>Processes</span>
                      <strong>{selectedRuntime.procCount}</strong>
                    </div>
                    <div className="statTile">
                      <span>Service Version</span>
                      <strong>{selectedRuntime.serviceVersion || "n/a"}</strong>
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
                  <div className="logEntry">{selectedRuntime.message}</div>
                  <div className="logEntry">
                    {selectedRuntime.computerName || "Unknown host"} · {selectedRuntime.osName || "Unknown OS"}{" "}
                    {selectedRuntime.osVersion}
                  </div>
                  <div className="logEntry">Service path: {selectedRuntime.servicePath || "n/a"}</div>
                  <div className="logEntry">Launcher path: {selectedRuntime.appLauncherPath || "n/a"}</div>
                  <div className="collection">
                    <div className="subHeader">
                      <h3>Watched Apps</h3>
                    </div>
                    {selectedRuntime.appStates.length === 0 ? (
                      <p className="emptyInline">No watched app state returned yet.</p>
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
                      <p className="emptyInline">No network counters returned yet.</p>
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
                <h3>Batch Captures ({batchCaptures.length})</h3>
                <button onClick={() => void batchCaptureScreen()} disabled={filteredStations.length === 0 || remoteBusy === "batchCapture"}>
                  {remoteBusy === "batchCapture" ? "Capturing..." : "Batch Capture All"}
                </button>
              </div>
              {batchCaptures.length === 0 ? (
                <p className="emptyInline">Click "Batch Capture All" to capture screenshots from all filtered stations.</p>
              ) : (
                <div className="captureGrid">
                  {batchCaptures.map((capture) => (
                    <div key={capture.stationId} className="captureThumb">
                      {capture.error ? (
                        <div className="captureThumbError">
                          <div className="captureThumbName">{capture.stationName}</div>
                          <div className="captureThumbErrMsg">Failed: {capture.error}</div>
                        </div>
                      ) : (
                        <>
                          <img
                            src={capture.dataUrl}
                            alt={`Screen of ${capture.stationName}`}
                            className="captureThumbImg"
                            onClick={() => {
                              setSelectedId(capture.stationId);
                              setIsEditingStationDetail(false);
                            }}
                            title={`${capture.stationName} · ${capture.endpoint} · ${formatBytes(capture.byteLen)}`}
                          />
                          <div className="captureThumbName">{capture.stationName}</div>
                        </>
                      )}
                    </div>
                  ))}
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
                <h3>Remote Files</h3>
                <button onClick={() => void browseRemote()} disabled={!selectedStation || remoteBusy === "browse"}>
                  {remoteBusy === "browse" ? "Browsing..." : "Browse"}
                </button>
              </div>
              <label className="field">
                <span>Remote Browse Path</span>
                <input value={remotePath} onChange={(event) => setRemotePath(event.target.value)} />
              </label>
              <label className="field">
                <span>Remote Source Path</span>
                <input value={renameSourcePath} onChange={(event) => setRenameSourcePath(event.target.value)} />
              </label>
              <label className="field">
                <span>Remote Target Path</span>
                <input value={renameTargetPath} onChange={(event) => setRenameTargetPath(event.target.value)} />
              </label>
              <label className="field">
                <span>Local Download Path</span>
                <input value={downloadLocalPath} onChange={(event) => setDownloadLocalPath(event.target.value)} />
              </label>
              <label className="field">
                <span>Local Upload Path</span>
                <input value={uploadLocalPath} onChange={(event) => setUploadLocalPath(event.target.value)} />
              </label>
              <div className="toolbar miniToolbar">
                <button onClick={() => void renameRemote()} disabled={!selectedStation || remoteBusy === "rename"}>
                  Rename
                </button>
                <button onClick={() => void downloadRemote()} disabled={!selectedStation || remoteBusy === "download"}>
                  Download
                </button>
                <button onClick={() => void uploadRemote()} disabled={!selectedStation || remoteBusy === "upload"}>
                  Upload
                </button>
                <button onClick={() => void batchDownloadAll()} disabled={stations.length === 0 || remoteBusy === "batchDownload"}>
                  {remoteBusy === "batchDownload" ? "Sending..." : "发送全部"}
                </button>
                <button onClick={() => void batchUploadAll()} disabled={stations.length === 0 || remoteBusy === "batchUpload"}>
                  {remoteBusy === "batchUpload" ? "Receiving..." : "接收全部"}
                </button>
              </div>
              {!selectedBrowser ? (
                <p className="emptyInline">Browse a remote path to inspect files.</p>
              ) : (
                <div className="logList">
                  {selectedBrowser.items.length === 0 ? (
                    <p className="emptyInline">No remote entries returned for this path.</p>
                  ) : (
                    selectedBrowser.items.map((item) => (
                      <button
                        key={item.path}
                        className="stationCard fileEntry"
                        onClick={() => {
                          setRenameSourcePath(item.path);
                          setRenameTargetPath(item.path);
                          if (item.isDirectory) {
                            setRemotePath(item.path);
                            void browseRemote(item.path);
                          }
                        }}
                      >
                        <span className="stationName">{item.isDirectory ? "Directory" : "File"}</span>
                        <span className="stationMeta">{item.path}</span>
                      </button>
                    ))
                  )}
                </div>
              )}
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
          ) : null}
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
        <BatchProvider>
          <BatchPage stations={stations} />
        </BatchProvider>
      ) : (
        <main className="grid gridSettingsMode">
          <section className="panel detailPanel">
            <div className="panelHeader">
              <h2>Client Options</h2>
              <span className="hint">Opened from the Settings button</span>
            </div>

            <div className="detailLayout">
              <div className="collection">
                <div className="subHeader">
                  <h3>Monitor Settings</h3>
                </div>
                <label className="field">
                  <span>Monitor Interval (seconds)</span>
                  <div style={{ display: "flex", gap: "0.5rem", alignItems: "center" }}>
                    <input
                      type="number"
                      min={1}
                      max={60}
                      value={options.interval}
                      onChange={(event) =>
                        setOptions((current) => ({
                          ...current,
                          interval: Number(event.target.value)
                        }))
                      }
                      style={{ width: "80px" }}
                    />
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
                      Save to Stations
                    </button>
                  </div>
                </label>
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
    </div>
  );
}
