import type { TagDefinition } from "./plugin/groups/types";
import type { Station, StationGroup, StationRuntimeSnapshot } from "./types";

export type StationSortBy = "name" | "ip" | "group" | "tag";
export type StationSortDirection = "asc" | "desc";
export type StationViewMode = "list" | "grid" | "split";

export interface FilterStationsArgs {
  stations: Station[];
  search: string;
  sortBy: StationSortBy;
  sortDirection: StationSortDirection;
  groupId: string;
  tagKey: string;
  tagValue: string;
  groups: StationGroup[];
  tagDefinitions: TagDefinition[];
}

export function getPrimaryIp(station: Station) {
  return station.networkInterfaces[0]?.ips[0] ?? "No IP";
}

export function getTagDefinitionKey(definition: TagDefinition) {
  return definition.key || definition.id;
}

export function getTagDefinitionLabel(definition: TagDefinition) {
  return definition.label || definition.name || getTagDefinitionKey(definition);
}

export function getStationStatusLabel(
  state: "ready" | "warning" | "error" | "offline",
) {
  if (state === "ready") {
    return "Ready";
  }

  if (state === "warning") {
    return "Warning";
  }

  if (state === "error") {
    return "Error";
  }

  return "Offline";
}

export function getStationGroupNames(station: Station, groups: StationGroup[]) {
  const stationGroupIds = new Set(station.groups ?? []);

  return groups
    .filter((group) => stationGroupIds.has(group.id))
    .map((group) => group.name);
}

export function getStationGroupSortValue(station: Station, groups: StationGroup[]) {
  return getStationGroupNames(station, groups)
    .sort((left, right) => left.localeCompare(right, undefined, { numeric: true }))[0] ?? "";
}

export function getStationTagSummary(
  station: Station,
  tagDefinitions: TagDefinition[],
) {
  const labelByKey = new Map(
    tagDefinitions.map((definition) => [
      getTagDefinitionKey(definition),
      getTagDefinitionLabel(definition),
    ]),
  );

  return Object.entries(station.tags ?? {})
    .filter(([, value]) => value.trim().length > 0)
    .map(([key, value]) => `${labelByKey.get(key) ?? key}: ${value}`);
}

export function getStationTagSortValue(
  station: Station,
  tagDefinitions: TagDefinition[],
  selectedTagKey: string,
) {
  if (selectedTagKey) {
    return station.tags?.[selectedTagKey]?.trim() ?? "";
  }

  return (
    getStationTagSummary(station, tagDefinitions)
      .sort((left, right) => left.localeCompare(right, undefined, { numeric: true }))[0] ?? ""
  );
}

export function deriveTagValueOptions(stations: Station[], tagKey: string) {
  if (!tagKey) {
    return [];
  }

  return Array.from(
    new Set(
      stations
        .map((station) => station.tags?.[tagKey]?.trim() ?? "")
        .filter(Boolean),
    ),
  ).sort((left, right) => left.localeCompare(right, undefined, { numeric: true }));
}

export function getStationRuntimeSummary(
  runtime: StationRuntimeSnapshot | null | undefined,
) {
  if (!runtime) {
    return [];
  }

  const memoryPct =
    runtime.totalMemory > 0
      ? `${((runtime.currentMemory / runtime.totalMemory) * 100).toFixed(0)}%`
      : `${Math.round(runtime.currentMemory / 1024 / 1024)} MB`;

  return [`CPU ${runtime.cpu.toFixed(0)}%`, `MEM ${memoryPct}`, `PROC ${runtime.procCount}`];
}

export function filterStations({
  stations,
  search,
  sortBy,
  sortDirection,
  groupId,
  tagKey,
  tagValue,
  groups,
  tagDefinitions,
}: FilterStationsArgs) {
  const loweredSearch = search.trim().toLowerCase();
  const loweredTagValue = tagValue.trim().toLowerCase();

  const nextStations = stations.filter((station) => {
    if (loweredSearch) {
      const allIps = station.networkInterfaces.flatMap((networkInterface) => networkInterface.ips);
      const matchesSearch =
        station.name.toLowerCase().includes(loweredSearch) ||
        station.networkInterfaces.some((networkInterface) =>
          networkInterface.mac.toLowerCase().includes(loweredSearch),
        ) ||
        allIps.some((ip) => ip.toLowerCase().includes(loweredSearch));

      if (!matchesSearch) {
        return false;
      }
    }

    if (groupId) {
      const group = groups.find((candidate) => candidate.id === groupId);
      if (group && !(group.stationIds || []).includes(station.id)) {
        return false;
      }
    }

    if (tagKey) {
      const stationTagValue = station.tags?.[tagKey]?.trim() ?? "";
      if (!stationTagValue) {
        return false;
      }

      if (loweredTagValue && !stationTagValue.toLowerCase().includes(loweredTagValue)) {
        return false;
      }
    }

    return true;
  });

  return nextStations.sort((left, right) => {
    let comparison = 0;

    if (sortBy === "ip") {
      comparison = getPrimaryIp(left).localeCompare(getPrimaryIp(right), undefined, {
        numeric: true,
      });
    } else if (sortBy === "group") {
      comparison = getStationGroupSortValue(left, groups).localeCompare(
        getStationGroupSortValue(right, groups),
        undefined,
        { numeric: true },
      );
    } else if (sortBy === "tag") {
      comparison = getStationTagSortValue(left, tagDefinitions, tagKey).localeCompare(
        getStationTagSortValue(right, tagDefinitions, tagKey),
        undefined,
        { numeric: true },
      );
    } else {
      comparison = left.name.localeCompare(right.name, undefined, { numeric: true });
    }

    if (comparison === 0) {
      comparison = left.name.localeCompare(right.name, undefined, { numeric: true });
    }

    return sortDirection === "desc" ? comparison * -1 : comparison;
  });
}
