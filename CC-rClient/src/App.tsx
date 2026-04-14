import { invoke } from "@tauri-apps/api/core";
import { useEffect, useMemo, useState } from "react";
import type {
  ActionResult,
  AppSnapshot,
  ClientOptions,
  PersistedState,
  RemoteFileBrowserResult,
  StartProgram,
  Station,
  StationAction,
  StationRuntimeSnapshot,
  StationScreenCapture
} from "./types";

const emptyWeatherOption = {
  download: 2,
  url: "",
  userName: "",
  password: "",
  lastHours: 0,
  interval: 2,
  deletePreviousFiles: false,
  deleteHowHoursAgo: 0,
  subDirectory: "",
  savePathForLinux: "",
  savePathForWindows: ""
};

const emptyOptions: ClientOptions = {
  interval: 2,
  isFirstTimeRun: true,
  startApps: [],
  monitorProcesses: [],
  weatherImageDownloadOption: emptyWeatherOption
};

const emptyStation = (): Station => ({
  id: crypto.randomUUID(),
  name: "New Station",
  blocked: false,
  networkInterfaces: [{ mac: "", ips: [""] }],
  startPrograms: [],
  monitorProcesses: [],
  lastAction: null
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
  const lastAction = (station.lastAction ?? "").toLowerCase();
  const hasErrorSignal =
    runtimeMessage.includes("error") ||
    runtimeMessage.includes("failed") ||
    runtimeMessage.includes("unavailable") ||
    runtimeMessage.includes("timeout") ||
    lastAction.includes("error") ||
    lastAction.includes("failed");

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
  const [browserByStation, setBrowserByStation] = useState<Record<string, RemoteFileBrowserResult>>({});
  const [captureByStation, setCaptureByStation] = useState<Record<string, StationScreenCapture>>({});
  const [runtimeLoadingId, setRuntimeLoadingId] = useState<string>("");
  const [remoteBusy, setRemoteBusy] = useState<string>("");
  const [remotePath, setRemotePath] = useState("");
  const [downloadLocalPath, setDownloadLocalPath] = useState("");
  const [uploadLocalPath, setUploadLocalPath] = useState("");
  const [renameSourcePath, setRenameSourcePath] = useState("");
  const [renameTargetPath, setRenameTargetPath] = useState("");
  const [activePage, setActivePage] = useState<"stations" | "settings">("stations");
  const [isEditingStationDetail, setIsEditingStationDetail] = useState(false);

  useEffect(() => {
    void loadSnapshot();
  }, []);

  const filteredStations = useMemo(() => {
    const lowered = search.trim().toLowerCase();
    const next = stations.filter((station) => {
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

    return next.sort((left, right) => {
      if (sortBy === "ip") {
        const leftIp = left.networkInterfaces[0]?.ips[0] ?? "";
        const rightIp = right.networkInterfaces[0]?.ips[0] ?? "";
        return leftIp.localeCompare(rightIp, undefined, { numeric: true });
      }
      return left.name.localeCompare(right.name, undefined, { numeric: true });
    });
  }, [search, sortBy, stations]);

  const selectedStation =
    filteredStations.find((station) => station.id === selectedId) ??
    stations.find((station) => station.id === selectedId) ??
    null;
  const selectedRuntime = selectedStation ? runtimeByStation[selectedStation.id] ?? null : null;
  const selectedBrowser = selectedStation ? browserByStation[selectedStation.id] ?? null : null;
  const selectedCapture = selectedStation ? captureByStation[selectedStation.id] ?? null : null;

  async function loadSnapshot() {
    setLoading(true);
    try {
      const next = await invoke<AppSnapshot>("load_state");
      setSnapshot(next);
      setStations(next.stations);
      setOptions(next.options);
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
      const payload: PersistedState = { stations, options };
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

  const selectedIds = selectedStation ? [selectedStation.id] : [];

  return (
    <div className="shell">
      <header className="hero">
        <div>
          <p className="eyebrow">CC Client Migration</p>
          <h1>CC-rClient</h1>
          <p className="lede">
            Rust/Tauri replacement for the legacy Qt workstation console with
            Rust-native state ownership, legacy import/export, Wake-on-LAN, and
            direct gRPC control for the completed `CC-rStationService` actions.
          </p>
        </div>
        <div className="heroCard">
          <span>Stations</span>
          <strong>{stations.length}</strong>
          <span>Storage</span>
          <code>{snapshot?.storagePath ?? "loading..."}</code>
        </div>
      </header>

      <section className="toolbar">
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
        <button onClick={addStation}>Add Station</button>
        <button onClick={removeSelectedStation} disabled={!selectedStation}>
          Remove
        </button>
        <button onClick={() => void executeAction("power_on", selectedIds)} disabled={!selectedStation}>
          Power On
        </button>
        <button onClick={() => void executeAction("block", selectedIds)} disabled={!selectedStation}>
          Block
        </button>
        <button onClick={() => void executeAction("unblock", selectedIds)} disabled={!selectedStation}>
          Unblock
        </button>
        <button onClick={() => void executeAction("start_app", selectedIds)} disabled={!selectedStation}>
          Start App
        </button>
        <button onClick={() => void executeAction("restart_app", selectedIds)} disabled={!selectedStation}>
          Restart App
        </button>
        <button onClick={() => void executeAction("exit_app", selectedIds)} disabled={!selectedStation}>
          Exit App
        </button>
        <button onClick={() => void executeAction("shutdown", selectedIds)} disabled={!selectedStation}>
          Shutdown
        </button>
        <button onClick={() => void executeAction("reboot", selectedIds)} disabled={!selectedStation}>
          Reboot
        </button>
        <button className="accent" onClick={() => void saveState()} disabled={saving}>
          {saving ? "Saving..." : "Save"}
        </button>
        <button onClick={() => void exportLegacyFiles()}>Export Legacy</button>
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
              </div>
            </div>

            <div className="stationList">
              {loading ? (
                <p className="emptyState">Loading state…</p>
              ) : filteredStations.length === 0 ? (
                <p className="emptyState">No stations match the current filter.</p>
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
                      <span className="stationMeta">{ip}</span>
                      <span className={`badge ${stationVisualState}`}>
                        {stationVisualState === "ready"
                          ? "Ready"
                          : stationVisualState === "warning"
                            ? "Warning"
                            : stationVisualState === "error"
                              ? "Error"
                              : "Offline"}
                      </span>
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
                      selectedRuntime.networkStats.map((item) => {
                        console.log(`UI: Displaying ${selectedRuntime.networkStats.length} network interfaces`);
                        selectedRuntime.networkStats.forEach((item, index) => {
                          console.log(`UI: Interface ${index}: ${item.ifName}, RX=${item.bytesReceivedPerSec}, TX=${item.bytesSentedPerSec}`);
                        });
                        return (
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
                        );
                      })
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
      ) : (
        <main className="grid">
          <section className="panel detailPanel">
            <div className="panelHeader">
              <h2>Client Options</h2>
              <span className="hint">Opened from the Settings button</span>
            </div>

            <div className="detailLayout">
              <label className="field">
                <span>Monitor Interval</span>
                <input
                  type="number"
                  min={1}
                  value={options.interval}
                  onChange={(event) =>
                    setOptions((current) => ({
                      ...current,
                      interval: Number(event.target.value)
                    }))
                  }
                />
              </label>

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

              <div className="collection">
                <div className="subHeader">
                  <h3>Weather Download Options</h3>
                </div>
                <p className="emptyInline">
                  Weather Source URL input has been removed from the UI.
                </p>
              </div>
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
