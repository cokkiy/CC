import { render, screen, waitFor } from "@testing-library/react";
import userEvent from "@testing-library/user-event";
import { describe, expect, it, vi } from "vitest";

import { TelemetryProfilesPage } from "../plugin/telemetry/TelemetryProfilesPage";
import type { Station } from "../types";

const stations: Station[] = [
  {
    id: "station-1",
    name: "Alpha",
    blocked: false,
    networkInterfaces: [{ mac: "AA", ips: ["10.0.0.10"] }],
    startPrograms: [],
    monitorProcesses: [],
    groups: [],
    tags: {},
    metadata: {},
  },
];

const telemetryMocks = vi.hoisted(() => ({
  getSchema: vi.fn().mockResolvedValue({
    schemaVersion: 2,
    supportedIncludes: [
      {
        key: "runtime_basic",
        label: "Runtime Basics",
        description: "CPU and memory",
      },
      {
        key: "runtime_network",
        label: "Network",
        description: "Interface counters",
      },
    ],
  }),
  getProfiles: vi.fn().mockResolvedValue({
    schemaVersion: 2,
    profilesVersion: 3,
    profiles: [
      {
        id: "default",
        name: "Default Runtime",
        enabled: true,
        collectionIntervalMs: 1000,
        includes: ["runtime_basic"],
      },
    ],
  }),
  replaceProfiles: vi.fn().mockResolvedValue({
    schemaVersion: 2,
    profilesVersion: 4,
    profiles: [
      {
        id: "default",
        name: "Updated Runtime",
        enabled: true,
        collectionIntervalMs: 1500,
        includes: ["runtime_basic", "runtime_network"],
      },
    ],
  }),
}));

vi.mock("../plugin/telemetry/api", () => ({
  telemetryProfilesApi: telemetryMocks,
}));

describe("TelemetryProfilesPage", () => {
  it("loads station schema and profiles", async () => {
    render(<TelemetryProfilesPage stations={stations} />);

    expect(await screen.findByDisplayValue("Default Runtime")).toBeInTheDocument();
    expect(telemetryMocks.getSchema).toHaveBeenCalledWith("station-1");
    expect(telemetryMocks.getProfiles).toHaveBeenCalledWith("station-1");
  });

  it("saves edited telemetry profiles", async () => {
    const user = userEvent.setup();
    render(<TelemetryProfilesPage stations={stations} />);

    const nameInput = await screen.findByDisplayValue("Default Runtime");
    await user.clear(nameInput);
    await user.type(nameInput, "Updated Runtime");

    const collectionInterval = screen.getByLabelText("Collection Interval (ms)");
    await user.clear(collectionInterval);
    await user.type(collectionInterval, "1500");

    const networkLabel = screen.getByText("Network").closest("label");
    const networkCheckbox = networkLabel?.querySelector("input");
    expect(networkCheckbox).toBeTruthy();
    await user.click(networkCheckbox as HTMLInputElement);
    await user.click(screen.getByRole("button", { name: /save profiles/i }));

    await waitFor(() => {
      expect(telemetryMocks.replaceProfiles).toHaveBeenCalledWith("station-1", [
        {
          id: "default",
          name: "Updated Runtime",
          enabled: true,
          collectionIntervalMs: 1500,
          includes: ["runtime_basic", "runtime_network"],
        },
      ]);
    });

    expect(await screen.findByText(/saved and applied live/i)).toBeInTheDocument();
  });
});
