import { render, screen, waitFor } from "@testing-library/react";
import userEvent from "@testing-library/user-event";
import { describe, expect, it, vi } from "vitest";

import { BatchTaskEditor } from "../plugin/batch/BatchEditor";
import { BatchTaskList } from "../plugin/batch/BatchList";
import type { BatchTask, BatchTarget } from "../plugin/batch/types";

const targets: BatchTarget[] = [
  {
    id: "station-1",
    name: "Alpha",
    status: "online",
    groups: [],
    tags: {},
  },
];

const baseTask: BatchTask = {
  id: "task-1",
  name: "Pinned Reboot",
  description: "Reboot devices after deploy",
  taskType: "reboot",
  targetSelector: { selectorType: "all" },
  content: "",
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
};

describe("Batch task toolbar controls", () => {
  it("toggles toolbar pinning from the task list", async () => {
    const user = userEvent.setup();
    const onToggleToolbarPin = vi.fn();

    render(
      <BatchTaskList
        tasks={[baseTask]}
        onEditTask={vi.fn()}
        onExecuteTask={vi.fn()}
        onDeleteTask={vi.fn()}
        onToggleToolbarPin={onToggleToolbarPin}
      />,
    );

    await user.click(screen.getByRole("button", { name: /add to toolbar/i }));

    expect(onToggleToolbarPin).toHaveBeenCalledWith(baseTask);
  });

  it("includes showInToolbar in the saved task payload", async () => {
    const user = userEvent.setup();
    const onSave = vi.fn();

    render(
      <BatchTaskEditor
        targets={targets}
        groups={[]}
        scripts={[]}
        onSave={onSave}
        onCancel={vi.fn()}
      />,
    );

    await user.type(screen.getByPlaceholderText(/enter task name/i), "Toolbar command");
    await user.type(screen.getByPlaceholderText(/enter command to execute/i), "hostname");
    await user.click(screen.getByRole("checkbox", { name: /show this task in the main toolbar/i }));

    await waitFor(() => {
      expect(screen.getByRole("button", { name: /create task/i })).toBeEnabled();
    });

    await user.click(screen.getByRole("button", { name: /create task/i }));

    expect(onSave).toHaveBeenCalledWith(
      expect.objectContaining({
        name: "Toolbar command",
        content: "hostname",
        showInToolbar: true,
      }),
    );
  });

  it("saves app control tasks with the selected subtype parameter", async () => {
    const user = userEvent.setup();
    const onSave = vi.fn();

    render(
      <BatchTaskEditor
        targets={targets}
        groups={[]}
        scripts={[]}
        onSave={onSave}
        onCancel={vi.fn()}
      />,
    );

    await user.type(screen.getByPlaceholderText(/enter task name/i), "Restart apps");
    await user.click(screen.getByRole("button", { name: /app control/i }));
    await user.selectOptions(screen.getByRole("combobox", { name: /action/i }), "restart");

    await waitFor(() => {
      expect(screen.getByRole("button", { name: /create task/i })).toBeEnabled();
    });

    await user.click(screen.getByRole("button", { name: /create task/i }));

    expect(onSave).toHaveBeenCalledWith(
      expect.objectContaining({
        taskType: "app_control",
        parameters: expect.arrayContaining([
          expect.objectContaining({
            name: "appControlAction",
            defaultValue: "restart",
          }),
        ]),
      }),
    );
  });

  it("saves file transfer tasks with structured source and target paths", async () => {
    const user = userEvent.setup();
    const onSave = vi.fn();

    render(
      <BatchTaskEditor
        targets={targets}
        groups={[]}
        scripts={[]}
        onSave={onSave}
        onCancel={vi.fn()}
      />,
    );

    await user.type(screen.getByPlaceholderText(/enter task name/i), "Pull configs");
    await user.click(screen.getByRole("button", { name: /file transfer/i }));
    await user.selectOptions(screen.getByRole("combobox", { name: /direction/i }), "pull");
    await user.selectOptions(screen.getByRole("combobox", { name: /entry type/i }), "folder");
    await user.type(screen.getByPlaceholderText("/path/on/device"), "/opt/config");
    await user.type(screen.getByPlaceholderText("/path/on/client"), "/tmp/config");

    await waitFor(() => {
      expect(screen.getByRole("button", { name: /create task/i })).toBeEnabled();
    });

    await user.click(screen.getByRole("button", { name: /create task/i }));

    expect(onSave).toHaveBeenCalledWith(
      expect.objectContaining({
        taskType: "file_transfer",
        content: "",
        parameters: expect.arrayContaining([
          expect.objectContaining({ name: "transferDirection", defaultValue: "pull" }),
          expect.objectContaining({ name: "transferEntryType", defaultValue: "folder" }),
          expect.objectContaining({ name: "sourcePath", defaultValue: "/opt/config" }),
          expect.objectContaining({ name: "targetPath", defaultValue: "/tmp/config" }),
        ]),
      }),
    );
  });
});
