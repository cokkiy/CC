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

export type WeatherImageOption = {
  download: number;
  url: string;
  userName: string;
  password: string;
  lastHours: number;
  interval: number;
  deletePreviousFiles: boolean;
  deleteHowHoursAgo: number;
  subDirectory: string;
  savePathForLinux: string;
  savePathForWindows: string;
};

export type Station = {
  id: string;
  name: string;
  blocked: boolean;
  networkInterfaces: NetworkInterface[];
  startPrograms: StartProgram[];
  monitorProcesses: string[];
  lastAction?: string | null;
};

export type ClientOptions = {
  interval: number;
  isFirstTimeRun: boolean;
  startApps: StartProgram[];
  monitorProcesses: string[];
  weatherImageDownloadOption: WeatherImageOption;
};

export type PersistedState = {
  stations: Station[];
  options: ClientOptions;
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
  | "full_screen"
  | "real_time"
  | "prev_page"
  | "next_page"
  | "clear_page";
