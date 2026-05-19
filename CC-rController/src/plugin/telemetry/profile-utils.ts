import type {
  TelemetryIncludeDefinition,
  TelemetryProfile,
  TelemetryProfileDraft,
} from "./types";

export const SUPPORTED_TELEMETRY_INCLUDES: TelemetryIncludeDefinition[] = [
  {
    key: "runtime_basic",
    label: "Runtime Basics",
    description: "CPU, memory, and process counts.",
  },
  {
    key: "runtime_system",
    label: "System Details",
    description: "OS, service version, and runtime paths.",
  },
  {
    key: "runtime_apps",
    label: "Watched Apps",
    description: "Process state and resource usage for monitored apps.",
  },
  {
    key: "runtime_network",
    label: "Network",
    description: "Interface counters and connection totals.",
  },
  {
    key: "runtime_storage",
    label: "Storage",
    description: "Volume capacity and usage snapshots.",
  },
];

export function createTelemetryProfileDraft(seed: number): TelemetryProfileDraft {
  return {
    id: `profile-${seed}`,
    name: `Profile ${seed}`,
    enabled: true,
    collectionIntervalMs: 1000,
    includes: ["runtime_basic"],
  };
}

export function validateTelemetryProfileDrafts(profiles: TelemetryProfileDraft[]) {
  const errors: string[] = [];
  const ids = new Set<string>();

  profiles.forEach((profile, index) => {
    const label = profile.name.trim() || `Profile ${index + 1}`;
    const trimmedId = profile.id.trim();

    if (!trimmedId) {
      errors.push(`${label}: id is required.`);
    } else if (ids.has(trimmedId)) {
      errors.push(`${label}: duplicate id '${trimmedId}'.`);
    } else {
      ids.add(trimmedId);
    }

    if (!profile.name.trim()) {
      errors.push(`Profile ${index + 1}: name is required.`);
    }

    if (profile.collectionIntervalMs <= 0) {
      errors.push(`${label}: collection interval must be positive.`);
    }

    if (profile.includes.length === 0) {
      errors.push(`${label}: select at least one section.`);
    }
  });

  return errors;
}

export function validateEnabledTelemetrySectionConflicts(
  profiles: TelemetryProfileDraft[] = [],
) {
  const warnings: string[] = [];
  const sectionOwners = new Map<string, string>();

  profiles.forEach((profile, index) => {
    if (!profile.enabled) {
      return;
    }

    const label = profile.name.trim() || profile.id.trim() || `Profile ${index + 1}`;
    Array.from(new Set(profile.includes)).forEach((section) => {
      const owner = sectionOwners.get(section);
      if (owner) {
        warnings.push(
          `Section '${section}' is enabled in both '${owner}' and '${label}'. Disable one profile or remove the duplicate section before applying.`,
        );
        return;
      }

      sectionOwners.set(section, label);
    });
  });

  return warnings;
}
