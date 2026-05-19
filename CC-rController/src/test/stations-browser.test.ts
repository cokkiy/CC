import { describe, expect, it } from "vitest";
import type { TagDefinition } from "../plugin/groups/types";
import { deriveTagValueOptions, filterStations, getStationTagSummary } from "../stations-browser";
import type { Station, StationGroup } from "../types";

const groups: StationGroup[] = [
  {
    id: "ops",
    name: "Operations",
    description: "",
    color: "#3b82f6",
    stationIds: ["station-1", "station-2"],
  },
  {
    id: "lab",
    name: "Lab",
    description: "",
    color: "#10b981",
    stationIds: ["station-3"],
  },
];

const tagDefinitions: TagDefinition[] = [
  { id: "region", name: "Region", description: "", color: "#0ea5e9" },
  { id: "tier", name: "Tier", description: "", color: "#8b5cf6" },
];

const stations: Station[] = [
  {
    id: "station-1",
    name: "Alpha",
    blocked: false,
    networkInterfaces: [{ mac: "AA:BB", ips: ["10.0.0.12"] }],
    startPrograms: [],
    monitorProcesses: [],
    groups: ["ops"],
    tags: { region: "west", tier: "gold" },
    metadata: {},
  },
  {
    id: "station-2",
    name: "Bravo",
    blocked: false,
    networkInterfaces: [{ mac: "CC:DD", ips: ["10.0.0.2"] }],
    startPrograms: [],
    monitorProcesses: [],
    groups: ["ops"],
    tags: { region: "east", tier: "silver" },
    metadata: {},
  },
  {
    id: "station-3",
    name: "Charlie",
    blocked: false,
    networkInterfaces: [{ mac: "EE:FF", ips: ["10.0.0.30"] }],
    startPrograms: [],
    monitorProcesses: [],
    groups: ["lab"],
    tags: { region: "west" },
    metadata: {},
  },
];

describe("stations-browser helpers", () => {
  it("filters by group and tag value together", () => {
    const result = filterStations({
      stations,
      search: "",
      sortBy: "name",
      sortDirection: "asc",
      groupId: "ops",
      tagKey: "region",
      tagValue: "west",
      groups,
      tagDefinitions,
    });

    expect(result.map((station) => station.id)).toEqual(["station-1"]);
  });

  it("sorts IPs numerically", () => {
    const result = filterStations({
      stations,
      search: "",
      sortBy: "ip",
      sortDirection: "asc",
      groupId: "",
      tagKey: "",
      tagValue: "",
      groups,
      tagDefinitions,
    });

    expect(result.map((station) => station.id)).toEqual(["station-2", "station-1", "station-3"]);
  });

  it("sorts by group name", () => {
    const result = filterStations({
      stations,
      search: "",
      sortBy: "group",
      sortDirection: "asc",
      groupId: "",
      tagKey: "",
      tagValue: "",
      groups,
      tagDefinitions,
    });

    expect(result.map((station) => station.id)).toEqual(["station-3", "station-1", "station-2"]);
  });

  it("sorts by selected tag value descending", () => {
    const result = filterStations({
      stations,
      search: "",
      sortBy: "tag",
      sortDirection: "desc",
      groupId: "",
      tagKey: "region",
      tagValue: "",
      groups,
      tagDefinitions,
    });

    expect(result.map((station) => station.id)).toEqual(["station-3", "station-1", "station-2"]);
  });

  it("derives unique tag values for the selected key", () => {
    expect(deriveTagValueOptions(stations, "region")).toEqual(["east", "west"]);
  });

  it("builds tag summaries with definition labels", () => {
    expect(getStationTagSummary(stations[0], tagDefinitions)).toEqual(["Region: west", "Tier: gold"]);
  });
});
