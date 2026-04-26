export type NetworkInterface = {
  mac: string;
  ips: string[];
};

export type StartProgram = {
  path: string;
  arguments: string;
  processName: string;
  allowMultiInstance: boolean;
};

export type Station = {
  id: string;
  name: string;
  blocked: boolean;
  networkInterfaces: NetworkInterface[];
  startPrograms: StartProgram[];
  monitorProcesses: string[];
  lastAction?: string | null;
  groups: string[];
  tags: Record<string, string>;
  metadata: Record<string, string>;
  location?: Location;
};

export type ClientOptions = {
  interval: number;
  isFirstTimeRun: boolean;
  startApps: StartProgram[];
  monitorProcesses: string[];
};

export type PersistedState = {
  stations: Station[];
  options: ClientOptions;
  groups: StationGroup[];
  tags: TagDefinition[];
};

export type AppSnapshot = PersistedState & {
  storagePath: string;
  legacyImported: boolean;
};

export type ActionResult = {
  ok: boolean;
  message: string;
  stations?: Station[];
  implemented: boolean;
};

export type RemoteAppState = {
  monitorName: string;
  processName: string;
  processId: number;
  isRunning: boolean;
  cpu: number;
  procCount: number;
  threadCount: number;
  currentMemory: number;
  appVersion: string;
  startTime: number;
};

export type RemoteInterfaceStat = {
  ifName: string;
  bytesReceivedPerSec: number;
  bytesSentedPerSec: number;
  totalBytesPerSec: number;
  bytesReceived: number;
  bytesSented: number;
  bytesTotal: number;
  unicastPacketReceived: number;
  unicastPacketSented: number;
  multicastPacketReceived: number;
  multicastPacketSented: number;
};

export type StationRuntimeSnapshot = {
  endpoint: string;
  stationId: string;
  computerName: string;
  osName: string;
  osVersion: string;
  totalMemory: number;
  currentMemory: number;
  cpu: number;
  procCount: number;
  serviceVersion: string;
  appLauncherVersion: string;
  servicePath: string;
  appLauncherPath: string;
  appStates: RemoteAppState[];
  networkStats: RemoteInterfaceStat[];
  message: string;
};

export type RemoteFileEntry = {
  parent: string;
  path: string;
  isDirectory: boolean;
  creationTime: string;
  lastModify: string;
};

export type RemoteFileBrowserResult = {
  endpoint: string;
  requestedPath: string;
  items: RemoteFileEntry[];
};

export type StationScreenCapture = {
  endpoint: string;
  byteLen: number;
  dataUrl: string;
};

export type StationAction =
  | "power_on"
  | "block"
  | "unblock"
  | "start_app"
  | "restart_app"
  | "exit_app"
  | "shutdown"
  | "reboot"
  | "batch_power_on"
  | "batch_shutdown"
  | "batch_reboot";

export type CommandExecutionResult = {
  exitCode: number;
  stdout: string;
  stderr: string;
};

export type Location = {
  latitude?: number;
  longitude?: number;
  altitude?: number;
  accuracy?: number;
  address?: string;
};

export type TagDefinition = {
  id: string;
  name: string;
  description: string;
  color: string;
  type?: 'string' | 'number' | 'boolean' | 'select';
  options?: string[];
  required?: boolean;
  defaultValue?: string;
  created_at?: string;
  updated_at?: string;
  // UI aliases
  label?: string;
  key?: string;
};

export type StationGroup = {
  id: string;
  name: string;
  description: string;
  tags: string[];
  stationIds: string[];
  // Optional UI fields
  color?: string;
  createdAt?: string;
  updatedAt?: string;
};

export type BatchCapture = {
  stationId: string;
  stationName: string;
  endpoint: string;
  byteLen: number;
  dataUrl: string;
  error?: string;
};
