import type { AppControlAction, BatchTaskType, FileTransferDirection, FileTransferEntryType } from './types';

export const APP_CONTROL_ACTIONS: { value: AppControlAction; label: string }[] = [
  { value: 'start', label: 'Start App' },
  { value: 'stop', label: 'Stop App' },
  { value: 'restart', label: 'Restart App' },
];

export const FILE_TRANSFER_DIRECTIONS: { value: FileTransferDirection; label: string }[] = [
  { value: 'push', label: 'Push to Device' },
  { value: 'pull', label: 'Pull from Device' },
];

export const FILE_TRANSFER_ENTRY_TYPES: { value: FileTransferEntryType; label: string }[] = [
  { value: 'file', label: 'File' },
  { value: 'folder', label: 'Folder' },
];

type TaskMeta = {
  icon: string;
  label: string;
  summary: string;
};

export const BATCH_TASK_META: Record<BatchTaskType, TaskMeta> = {
  power_on: {
    icon: '⚡',
    label: 'Power On',
    summary: 'Wake devices with Wake-on-LAN.',
  },
  shutdown: {
    icon: '⏻',
    label: 'Shutdown',
    summary: 'Request a graceful shutdown over station control RPC.',
  },
  reboot: {
    icon: '🔄',
    label: 'Reboot',
    summary: 'Request a reboot over station control RPC.',
  },
  start_app: {
    icon: '🚀',
    label: 'Start App (Legacy)',
    summary: 'Legacy task type that starts each device’s configured startup programs.',
  },
  command: {
    icon: '💻',
    label: 'Command',
    summary: 'Run a direct shell command on the target device.',
  },
  watch_processes: {
    icon: '👁',
    label: 'Watch Processes',
    summary: 'Update the device-side watched process list.',
  },
  script: {
    icon: '📜',
    label: 'Script',
    summary: 'Run a saved script on the target device.',
  },
  app_control: {
    icon: '🧩',
    label: 'App Control',
    summary: 'Start, stop, or restart each device’s configured startup apps.',
  },
  file_transfer: {
    icon: '📦',
    label: 'File Transfer',
    summary: 'Push or pull files and folders between the client and target devices.',
  },
};

export function getBatchTaskMeta(type: string): TaskMeta {
  return BATCH_TASK_META[type as BatchTaskType] ?? {
    icon: '📄',
    label: type,
    summary: 'Custom batch task type.',
  };
}
