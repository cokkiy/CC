import { render, screen, waitFor } from "@testing-library/react";
import userEvent from "@testing-library/user-event";
import { beforeEach, describe, expect, it, vi } from "vitest";

import App from "../App";

const invokeMock = vi.fn();
const listenMock = vi.fn();

vi.mock("@tauri-apps/api/core", () => ({
  invoke: (...args: unknown[]) => invokeMock(...args),
}));

vi.mock("@tauri-apps/api/event", () => ({
  listen: (...args: unknown[]) => listenMock(...args),
}));

describe("App batch toolbar shortcuts", () => {
  beforeEach(() => {
    invokeMock.mockReset();
    listenMock.mockReset();

    listenMock.mockResolvedValue(() => {});

    invokeMock.mockImplementation(async (command: string) => {
      switch (command) {
        case "load_state":
          return {
            stations: [],
            options: {
              interval: 2,
              isFirstTimeRun: true,
              startApps: [],
              monitorProcesses: [],
            },
            groups: [],
            tags: [],
            legacyImported: false,
          };
        case "load_batch_tasks":
          return {
            tasks: [
              {
                id: "task-1",
                name: "Pinned Reboot",
                description: "Reboot every device",
                taskType: "reboot",
                targetSelector: { selectorType: "all" },
                content: "",
                parameters: [],
                executionPolicy: {
                  mode: "parallel",
                  batchSize: 5,
                  continueOnFailure: true,
                  failureThresholdPercent: 50,
                  timeoutSecs: 300,
                  retryCount: 0,
                  retryDelaySecs: 5,
                },
                createdBy: "tester",
                createdAt: "2026-05-01T00:00:00.000Z",
                updatedAt: "2026-05-01T00:00:00.000Z",
                status: "draft",
                version: 1,
                usageCount: 0,
                isFavorite: false,
                showInToolbar: true,
                tags: [],
              },
              {
                id: "task-2",
                name: "Hidden Task",
                description: "Should not be pinned",
                taskType: "command",
                targetSelector: { selectorType: "all" },
                content: "hostname",
                parameters: [],
                executionPolicy: {
                  mode: "parallel",
                  batchSize: 5,
                  continueOnFailure: true,
                  failureThresholdPercent: 50,
                  timeoutSecs: 300,
                  retryCount: 0,
                  retryDelaySecs: 5,
                },
                createdBy: "tester",
                createdAt: "2026-05-01T00:00:00.000Z",
                updatedAt: "2026-05-01T00:00:00.000Z",
                status: "draft",
                version: 1,
                usageCount: 0,
                isFavorite: false,
                showInToolbar: false,
                tags: [],
              },
            ],
          };
        case "get_batch_targets":
          return { targets: [], total: 0 };
        default:
          return {};
      }
    });
  });

  it("shows pinned batch tasks in the toolbar and opens the runner", async () => {
    const user = userEvent.setup();

    render(<App />);

    const pinnedButton = await screen.findByRole("button", { name: "Pinned Reboot" });
    expect(screen.queryByRole("button", { name: "Hidden Task" })).not.toBeInTheDocument();

    await user.click(pinnedButton);

    await waitFor(() => {
      expect(screen.getByRole("heading", { name: /execute batch task/i })).toBeInTheDocument();
    });
    expect(screen.getByRole("heading", { name: "Pinned Reboot" })).toBeInTheDocument();
  });
});
