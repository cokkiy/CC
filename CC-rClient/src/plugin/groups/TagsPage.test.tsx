import { cleanup, render, screen, waitFor } from '@testing-library/react';
import userEvent from '@testing-library/user-event';
import { afterEach, beforeEach, describe, expect, it, vi } from 'vitest';
import { TagsPage } from './TagsPage';
import type { Station } from '../../types';
import type { TagDefinition } from './types';

const mockUseTags = vi.fn();

vi.mock('./TagsContext', () => ({
  useTags: () => mockUseTags(),
}));

describe('TagsPage station tag actions', () => {
  const getEnabledByTestId = (testId: string) => {
    const candidates = screen.getAllByTestId(testId);
    const enabled = candidates.find((element) => !(element as HTMLButtonElement).disabled);
    if (!enabled) {
      throw new Error(`No enabled element found for testId ${testId}`);
    }
    return enabled;
  };

  const stations: Station[] = [
    {
      id: 's1',
      name: 'Station 1',
      blocked: false,
      networkInterfaces: [],
      startPrograms: [],
      monitorProcesses: [],
      lastAction: null,
      groups: [],
      tags: {},
      metadata: {},
    },
    {
      id: 's2',
      name: 'Station 2',
      blocked: false,
      networkInterfaces: [],
      startPrograms: [],
      monitorProcesses: [],
      lastAction: null,
      groups: [],
      tags: {},
      metadata: {},
    },
  ];

  const tagDefinitions: TagDefinition[] = [
    {
      id: 'env',
      key: 'env',
      name: 'Environment',
      label: 'Environment',
      description: '',
      color: '#3B82F6',
      type: 'string',
    },
  ];

  const makeTagsContext = (overrides?: Partial<Record<string, unknown>>) => ({
    tagDefinitions,
    selectedTag: null,
    createTag: vi.fn(),
    updateTag: vi.fn(),
    deleteTag: vi.fn(),
    selectTag: vi.fn(),
    importTagDefinitions: vi.fn(),
    exportTagDefinitions: vi.fn(),
    getTagStats: vi.fn(async () => []),
    getStationTags: vi.fn(async () => ({})),
    updateStationTags: vi.fn(async (_stationId: string, tags: Record<string, string>) => tags),
    batchUpdateStationTags: vi.fn(async () => undefined),
    ...overrides,
  });

  beforeEach(() => {
    vi.clearAllMocks();
    vi.stubGlobal('confirm', vi.fn(() => true));

    mockUseTags.mockReturnValue(makeTagsContext());
  });

  afterEach(() => {
    cleanup();
  });

  it('saves station tags and shows success notice', async () => {
    const user = userEvent.setup();
    render(<TagsPage stations={stations} />);

    await user.selectOptions(screen.getByLabelText('Station'), 's1');

    const inputs = await screen.findAllByPlaceholderText('Value for Environment');
    for (const input of inputs) {
      await user.clear(input);
      await user.type(input, 'prod');
    }

    await user.click(getEnabledByTestId('station-tags-save-btn'));

    const updateStationTags = mockUseTags.mock.results[0].value.updateStationTags as ReturnType<typeof vi.fn>;
    await waitFor(() => {
      expect(updateStationTags).toHaveBeenCalledWith('s1', { env: 'prod' });
    });

    expect(await screen.findByRole('status')).toHaveTextContent('Station tag values saved successfully.');
  });

  it('applies current station tags to all stations', async () => {
    const batchSpy = vi.fn(async () => undefined);
    mockUseTags.mockReturnValue(
      makeTagsContext({
        getStationTags: vi.fn(async () => ({ env: 'staging' })),
        batchUpdateStationTags: batchSpy,
      }),
    );

    const user = userEvent.setup();
    render(<TagsPage stations={stations} />);

    await user.selectOptions(screen.getByLabelText('Station'), 's1');

    await user.click(getEnabledByTestId('station-tags-apply-all-btn'));

    await waitFor(() => {
      expect(batchSpy).toHaveBeenCalledWith(['s1', 's2'], { env: 'staging' });
    });

    expect(await screen.findByRole('status')).toHaveTextContent('Tag values applied to all stations.');
  });
});
