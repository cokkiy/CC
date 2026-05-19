import React, { useEffect, useMemo, useState } from "react";

import type { Station } from "../../types";
import { telemetryProfilesApi } from "./api";
import { TelemetryProfilesEditor } from "./TelemetryProfilesEditor";
import {
  validateEnabledTelemetrySectionConflicts,
  validateTelemetryProfileDrafts,
} from "./profile-utils";
import type {
  TelemetryProfileDraft,
  TelemetryProfilesResponse,
  TelemetrySchemaResponse,
} from "./types";

export interface TelemetryProfilesPageProps {
  stations: Station[];
}

export const TelemetryProfilesPage: React.FC<TelemetryProfilesPageProps> = ({ stations }) => {
  const [selectedStationId, setSelectedStationId] = useState(stations[0]?.id ?? "");
  const [profiles, setProfiles] = useState<TelemetryProfileDraft[]>([]);
  const [schema, setSchema] = useState<TelemetrySchemaResponse | null>(null);
  const [profilesVersion, setProfilesVersion] = useState(0);
  const [loading, setLoading] = useState(false);
  const [saving, setSaving] = useState(false);
  const [notice, setNotice] = useState<string | null>(null);
  const [error, setError] = useState<string | null>(null);

  useEffect(() => {
    if (!stations.some((station) => station.id === selectedStationId)) {
      setSelectedStationId(stations[0]?.id ?? "");
    }
  }, [selectedStationId, stations]);

  useEffect(() => {
    if (!selectedStationId) {
      setProfiles([]);
      setSchema(null);
      setProfilesVersion(0);
      return;
    }

    let cancelled = false;
    setLoading(true);
    setError(null);
    setNotice(null);

    void Promise.all([
      telemetryProfilesApi.getSchema(selectedStationId),
      telemetryProfilesApi.getProfiles(selectedStationId),
    ])
      .then(([nextSchema, nextProfiles]) => {
        if (cancelled) {
          return;
        }
        setSchema(nextSchema);
        setProfiles(nextProfiles.profiles);
        setProfilesVersion(nextProfiles.profilesVersion);
      })
      .catch((nextError) => {
        if (cancelled) {
          return;
        }
        setError(nextError instanceof Error ? nextError.message : String(nextError));
      })
      .finally(() => {
        if (!cancelled) {
          setLoading(false);
        }
      });

    return () => {
      cancelled = true;
    };
  }, [selectedStationId]);

  const validationErrors = useMemo(() => validateTelemetryProfileDrafts(profiles), [profiles]);
  const validationWarnings = useMemo(
    () => validateEnabledTelemetrySectionConflicts(profiles),
    [profiles],
  );

  async function saveProfiles() {
    if (!selectedStationId || validationErrors.length > 0) {
      return;
    }

    if (validationWarnings.length > 0) {
      setNotice(null);
      setError(validationWarnings.join(" "));
      return;
    }

    setSaving(true);
    setError(null);
    setNotice(null);
    try {
      const result: TelemetryProfilesResponse = await telemetryProfilesApi.replaceProfiles(
        selectedStationId,
        profiles,
      );
      setProfiles(result.profiles);
      setProfilesVersion(result.profilesVersion);
      setNotice("Telemetry profiles saved and applied live.");
    } catch (nextError) {
      setError(nextError instanceof Error ? nextError.message : String(nextError));
    } finally {
      setSaving(false);
    }
  }

  return (
    <div className="telemetryProfilesPage">
      <main className="panel telemetryProfilesPanel">
        <div className="telemetryProfilesPageHeader">
          <div>
            <h2>Telemetry Profiles</h2>
            <p>Manage MQTT collection profiles for each station.</p>
          </div>
          <select
            aria-label="Select station telemetry target"
            value={selectedStationId}
            onChange={(event) => setSelectedStationId(event.target.value)}
            disabled={loading || stations.length === 0}
          >
            {stations.length === 0 ? <option value="">No stations configured</option> : null}
            {stations.map((station) => (
              <option key={station.id} value={station.id}>
                {station.name || station.id}
              </option>
            ))}
          </select>
        </div>

        <div className="telemetryProfilesMeta">
          <span>Schema v{schema?.schemaVersion ?? "?"}</span>
          <span>Profiles version {profilesVersion}</span>
        </div>

        <TelemetryProfilesEditor
          title="Station Profiles"
          profiles={profiles}
          supportedIncludes={schema?.supportedIncludes ?? []}
          loading={loading}
          saving={saving}
          error={error}
          notice={notice}
          validationErrors={validationErrors}
          validationWarnings={validationWarnings}
          emptyState={
            selectedStationId
              ? "No telemetry profiles configured for this station."
              : "Select a station to edit telemetry profiles."
          }
          onChange={setProfiles}
          onSave={() => void saveProfiles()}
        />
      </main>

      <style>{`
        .telemetryProfilesPage {
          min-height: calc(100vh - 64px - 48px);
        }

        .telemetryProfilesPanel {
          min-height: calc(100vh - 64px - 48px);
          padding: 20px;
        }

        .telemetryProfilesPageHeader {
          display: flex;
          justify-content: space-between;
          gap: 16px;
          align-items: flex-start;
          margin-bottom: 16px;
        }

        .telemetryProfilesPageHeader h2 {
          margin: 0 0 6px;
        }

        .telemetryProfilesPageHeader p {
          margin: 0;
          color: var(--text-muted);
        }

        .telemetryProfilesMeta {
          display: flex;
          gap: 16px;
          color: var(--text-muted);
          font-size: 0.84rem;
          margin-bottom: 14px;
        }

        @media (max-width: 900px) {
          .telemetryProfilesPageHeader {
            flex-direction: column;
          }
        }
      `}</style>
    </div>
  );
};
