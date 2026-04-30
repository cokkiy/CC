import { render, screen, waitFor } from "@testing-library/react";
import userEvent from "@testing-library/user-event";
import { describe, expect, it, vi } from "vitest";
import { TagsPage } from "../plugin/groups/TagsPage";
import type { Station } from "../types";
import type { TagDefinition } from "../plugin/groups/types";

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

const tagMocks = vi.hoisted(() => ({
  tagDefinition: {
    id: "region",
    name: "Region",
    description: "",
    color: "#0ea5e9",
  },
  batchUpdateStationTags: vi.fn().mockResolvedValue(undefined),
  updateStationTags: vi.fn().mockResolvedValue({}),
  getStationTags: vi.fn().mockResolvedValue({}),
  getTagStats: vi.fn().mockResolvedValue([]),
  createTag: vi.fn(),
  updateTag: vi.fn(),
  deleteTag: vi.fn().mockResolvedValue(undefined),
  selectTag: vi.fn(),
  importTagDefinitions: vi.fn().mockResolvedValue(undefined),
  exportTagDefinitions: vi.fn().mockResolvedValue([]),
}));

const tagDefinition = tagMocks.tagDefinition as TagDefinition;
tagMocks.createTag.mockResolvedValue(tagDefinition);
tagMocks.updateTag.mockResolvedValue(tagDefinition);

vi.mock("../plugin/groups/TagsContext", () => ({
  useTags: () => ({
    tagDefinitions: [tagDefinition],
    selectedTag: null,
    createTag: tagMocks.createTag,
    updateTag: tagMocks.updateTag,
    deleteTag: tagMocks.deleteTag,
    selectTag: tagMocks.selectTag,
    importTagDefinitions: tagMocks.importTagDefinitions,
    exportTagDefinitions: tagMocks.exportTagDefinitions,
    getTagStats: tagMocks.getTagStats,
    getStationTags: tagMocks.getStationTags,
    updateStationTags: tagMocks.updateStationTags,
    batchUpdateStationTags: tagMocks.batchUpdateStationTags,
  }),
}));

vi.mock("../plugin/groups/TagEditor", () => ({
  TagDefinitionList: () => <div data-testid="tag-definition-list" />,
  TagEditor: () => <div data-testid="tag-editor" />,
}));

describe("TagsPage", () => {
  it("filters devices by IP in the multi-device picker", async () => {
    const user = userEvent.setup();

    render(<TagsPage stations={stations} />);

    await user.click(screen.getByRole("button", { name: /select devices/i }));
    await user.type(screen.getByPlaceholderText(/filter by name or ip/i), "0.22");

    expect(screen.queryByText("Alpha")).not.toBeInTheDocument();
    expect(screen.getByText("Bravo")).toBeInTheDocument();
  });

  it("batch applies tag values to the selected devices", async () => {
    const user = userEvent.setup();

    render(<TagsPage stations={stations} />);

    await user.click(screen.getByRole("button", { name: /select devices/i }));
    await user.click(screen.getByRole("checkbox", { name: /select alpha/i }));
    await user.click(screen.getByRole("checkbox", { name: /select bravo/i }));
    await user.click(screen.getByRole("button", { name: /apply selection/i }));
    await user.type(screen.getByPlaceholderText(/value for region/i), "west");
    await user.click(screen.getByRole("button", { name: /save to 2 devices/i }));

    await waitFor(() => {
      expect(tagMocks.batchUpdateStationTags).toHaveBeenCalledWith(
        ["station-1", "station-2"],
        { region: "west" },
      );
    });
  });
});
