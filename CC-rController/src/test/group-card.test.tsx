import { render, screen, waitFor } from "@testing-library/react";
import userEvent from "@testing-library/user-event";
import { describe, expect, it, vi } from "vitest";
import { GroupCard } from "../plugin/groups/GroupCard";
import type { Station } from "../types";
import type { StationGroup } from "../plugin/groups/types";

const stations: Station[] = [
  {
    id: "station-1",
    name: "Alpha",
    blocked: false,
    networkInterfaces: [{ mac: "AA", ips: ["10.0.0.11"] }],
    startPrograms: [],
    monitorProcesses: [],
    groups: [],
    tags: {},
    metadata: {},
  },
  {
    id: "station-2",
    name: "Bravo",
    blocked: false,
    networkInterfaces: [{ mac: "BB", ips: ["10.0.0.22"] }],
    startPrograms: [],
    monitorProcesses: [],
    groups: [],
    tags: {},
    metadata: {},
  },
];

const group: StationGroup = {
  id: "ops",
  name: "Operations",
  description: "Ops devices",
  color: "#3b82f6",
  station_ids: [],
};

describe("GroupCard", () => {
  it("filters available devices by IP and batch adds the filtered selection", async () => {
    const user = userEvent.setup();
    const onBatchAddStations = vi.fn().mockResolvedValue(undefined);

    render(
      <GroupCard
        group={group}
        stations={stations}
        isSelected={false}
        onSelect={() => undefined}
        onEdit={() => undefined}
        onDelete={() => undefined}
        onAddStation={vi.fn().mockResolvedValue(undefined)}
        onRemoveStation={vi.fn()}
        onBatchAddStations={onBatchAddStations}
      />,
    );

    await user.click(screen.getByRole("button", { name: /\+ add devices/i }));
    await user.type(screen.getByPlaceholderText(/filter by name or ip/i), "0.22");

    expect(screen.queryByText("Alpha")).not.toBeInTheDocument();
    expect(screen.getByText("Bravo")).toBeInTheDocument();

    await user.click(screen.getByRole("checkbox", { name: /select bravo/i }));
    await user.click(screen.getByRole("button", { name: /add selected/i }));

    await waitFor(() => {
      expect(onBatchAddStations).toHaveBeenCalledWith(["station-2"]);
    });
  });
});
