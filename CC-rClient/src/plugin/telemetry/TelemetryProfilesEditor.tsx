import React from "react";

import type { TelemetryIncludeDefinition, TelemetryProfileDraft } from "./types";
import { createTelemetryProfileDraft } from "./profile-utils";

export interface TelemetryProfilesEditorProps {
  title: string;
  subtitle?: string;
  profiles: TelemetryProfileDraft[];
  supportedIncludes: TelemetryIncludeDefinition[];
  loading?: boolean;
  saving?: boolean;
  error?: string | null;
  notice?: string | null;
  validationErrors?: string[];
  validationWarnings?: string[];
  emptyState?: string;
  onChange: (profiles: TelemetryProfileDraft[]) => void;
  onSave?: () => void;
  saveLabel?: string;
  savePlacement?: "header" | "footer";
}

export const TelemetryProfilesEditor: React.FC<TelemetryProfilesEditorProps> = ({
  title,
  subtitle,
  profiles,
  supportedIncludes,
  loading = false,
  saving = false,
  error,
  notice,
  validationErrors = [],
  validationWarnings = [],
  emptyState = "No telemetry profiles configured.",
  onChange,
  onSave,
  saveLabel = "Save Profiles",
  savePlacement = "header",
}) => {
  const enabledSectionCounts = profiles.reduce<Record<string, number>>((counts, profile) => {
    if (!profile.enabled) {
      return counts;
    }

    Array.from(new Set(profile.includes)).forEach((section) => {
      counts[section] = (counts[section] ?? 0) + 1;
    });
    return counts;
  }, {});

  const statusMessages = (
    <>
      {notice ? <p className="telemetryProfilesNotice success">{notice}</p> : null}
      {error ? <p className="telemetryProfilesNotice error">{error}</p> : null}
      {validationErrors.length > 0 ? (
        <div className="telemetryProfilesValidation">
          <strong>Validation</strong>
          <ul>
            {validationErrors.map((item) => (
              <li key={item}>{item}</li>
            ))}
          </ul>
        </div>
      ) : null}
      {validationWarnings.length > 0 ? (
        <div className="telemetryProfilesValidation warning">
          <strong>Warning</strong>
          <ul>
            {validationWarnings.map((item) => (
              <li key={item}>{item}</li>
            ))}
          </ul>
        </div>
      ) : null}
    </>
  );

  function patchProfile(index: number, patch: Partial<TelemetryProfileDraft>) {
    onChange(
      profiles.map((profile, profileIndex) =>
        profileIndex === index ? { ...profile, ...patch } : profile,
      ),
    );
  }

  function toggleInclude(index: number, includeKey: string) {
    onChange(
      profiles.map((profile, profileIndex) => {
        if (profileIndex !== index) {
          return profile;
        }

        const includes = profile.includes.includes(includeKey)
          ? profile.includes.filter((value) => value !== includeKey)
          : [...profile.includes, includeKey];

        return { ...profile, includes };
      }),
    );
  }

  function addProfile() {
    onChange([...profiles, createTelemetryProfileDraft(profiles.length + 1)]);
  }

  function removeProfile(index: number) {
    onChange(profiles.filter((_, profileIndex) => profileIndex !== index));
  }

  return (
    <div className="telemetryProfilesEditor">
      <div className="telemetryProfilesHeader">
        <div>
          <h3>{title}</h3>
          {subtitle ? <p>{subtitle}</p> : null}
        </div>
        <div className="telemetryProfilesHeaderActions">
          <button type="button" onClick={addProfile} disabled={loading || saving}>
            Add Profile
          </button>
          {onSave && savePlacement === "header" ? (
            <button
              type="button"
              className="accent"
              onClick={onSave}
              disabled={loading || saving || validationErrors.length > 0}
            >
              {saving ? "Saving..." : saveLabel}
            </button>
          ) : null}
        </div>
      </div>

      {savePlacement === "footer" ? null : statusMessages}

      {loading ? (
        <p className="emptyInline">Loading telemetry profile data…</p>
      ) : profiles.length === 0 ? (
        <p className="emptyInline">{emptyState}</p>
      ) : (
        <div className="telemetryProfilesList">
          {profiles.map((profile, index) => (
            <section key={index} className="telemetryProfileCard">
              <div className="telemetryProfileCardHeader">
                <strong>{profile.name || `Profile ${index + 1}`}</strong>
                <button
                  type="button"
                  onClick={() => removeProfile(index)}
                  disabled={saving}
                >
                  Remove
                </button>
              </div>

              <div className="telemetryProfileGrid">
                <label className="field">
                  <span>Profile ID</span>
                  <input
                    value={profile.id}
                    onChange={(event) => patchProfile(index, { id: event.target.value })}
                    disabled={saving}
                  />
                </label>
                <label className="field">
                  <span>Name</span>
                  <input
                    value={profile.name}
                    onChange={(event) => patchProfile(index, { name: event.target.value })}
                    disabled={saving}
                  />
                </label>
                <label className="field">
                  <span>Collection Interval (ms)</span>
                  <input
                    type="number"
                    min={500}
                    step={500}
                    value={profile.collectionIntervalMs}
                    onChange={(event) =>
                      patchProfile(index, {
                        collectionIntervalMs: Number(event.target.value),
                      })
                    }
                    disabled={saving}
                  />
                </label>
              </div>

              <label className="checkField telemetryProfileToggle">
                <input
                  type="checkbox"
                  checked={profile.enabled}
                  onChange={(event) =>
                    patchProfile(index, { enabled: event.target.checked })
                  }
                  disabled={saving}
                />
                <span>Enabled</span>
              </label>

              <div className="telemetryProfileIncludes">
                <span className="telemetryProfileIncludesLabel">Included sections</span>
                <div className="telemetryProfileIncludeGrid">
                  {supportedIncludes.map((item) => (
                    <label
                      key={item.key}
                      className={[
                        "telemetryIncludeOption",
                        profile.enabled &&
                        profile.includes.includes(item.key) &&
                        enabledSectionCounts[item.key] > 1
                          ? "hasConflict"
                          : "",
                      ].join(" ")}
                    >
                      <input
                        type="checkbox"
                        checked={profile.includes.includes(item.key)}
                        onChange={() => toggleInclude(index, item.key)}
                        disabled={saving}
                      />
                      <span className="telemetryIncludeLabel">
                        {item.label}
                        {profile.enabled &&
                        profile.includes.includes(item.key) &&
                        enabledSectionCounts[item.key] > 1 ? (
                          <span
                            className="telemetryConflictIcon"
                            title="This section is already enabled in another profile."
                            aria-label="Duplicate enabled section"
                          >
                            !
                          </span>
                        ) : null}
                      </span>
                      <small>{item.description}</small>
                    </label>
                  ))}
                </div>
              </div>
            </section>
          ))}
        </div>
      )}

      {onSave && savePlacement === "footer" ? (
        <>
          {statusMessages}
          <div className="telemetryProfilesFooterActions">
            <button
              type="button"
              className="accent"
              onClick={onSave}
              disabled={loading || saving || validationErrors.length > 0}
            >
              {saving ? "Saving..." : saveLabel}
            </button>
          </div>
        </>
      ) : null}

      <style>{`
        .telemetryProfilesEditor {
          display: grid;
          gap: 14px;
        }

        .telemetryProfilesHeader {
          display: flex;
          justify-content: space-between;
          gap: 16px;
          align-items: flex-end;
        }

        .telemetryProfilesHeader h3 {
          margin: 0 0 6px;
        }

        .telemetryProfilesHeader p {
          margin: 0;
          color: var(--text-muted);
        }

        .telemetryProfilesHeaderActions {
          display: flex;
          gap: 10px;
          flex-wrap: wrap;
          align-items: flex-end;
        }

        .telemetryProfilesFooterActions {
          display: flex;
          justify-content: flex-end;
          gap: 10px;
          padding-top: 2px;
        }

        .fieldHint {
          color: var(--text-muted);
          font-size: 0.78rem;
        }

        .telemetryProfilesNotice,
        .telemetryProfilesValidation {
          border-radius: 10px;
          padding: 10px 12px;
          border: 1px solid var(--border-color);
        }

        .telemetryProfilesNotice.success {
          color: #0f5132;
          background: #d1e7dd;
          border-color: #badbcc;
        }

        .telemetryProfilesNotice.error {
          color: #842029;
          background: #f8d7da;
          border-color: #f5c2c7;
        }

        .telemetryProfilesValidation.warning {
          color: #664d03;
          background: #fff3cd;
          border-color: #ffecb5;
        }

        .telemetryProfilesValidation ul {
          margin: 8px 0 0;
          padding-left: 18px;
        }

        .telemetryProfilesList {
          display: grid;
          gap: 16px;
        }

        .telemetryProfileCard {
          border: 1px solid var(--border-color);
          border-radius: 14px;
          padding: 16px;
          background: var(--bg-main);
        }

        .telemetryProfileCardHeader {
          display: flex;
          justify-content: space-between;
          gap: 12px;
          align-items: center;
          margin-bottom: 14px;
        }

        .telemetryProfileGrid {
          display: grid;
          grid-template-columns: repeat(auto-fit, minmax(220px, 1fr));
          gap: 12px;
        }

        .telemetryProfileToggle {
          margin: 14px 0;
        }

        .telemetryProfileIncludes {
          display: grid;
          gap: 10px;
        }

        .telemetryProfileIncludesLabel {
          font-size: 0.8rem;
          text-transform: uppercase;
          letter-spacing: 0.04em;
          color: var(--text-muted);
        }

        .telemetryProfileIncludeGrid {
          display: grid;
          grid-template-columns: repeat(auto-fit, minmax(220px, 1fr));
          gap: 10px;
        }

        .telemetryIncludeOption {
          display: grid;
          gap: 4px;
          padding: 10px 12px;
          border-radius: 10px;
          border: 1px solid var(--border-color);
          background: var(--bg-card);
        }

        .telemetryIncludeOption.hasConflict {
          border-color: #ffecb5;
          background: #fff8e1;
        }

        .telemetryIncludeLabel {
          display: inline-flex;
          align-items: center;
          gap: 6px;
          font-weight: 600;
        }

        .telemetryConflictIcon {
          display: inline-flex;
          align-items: center;
          justify-content: center;
          width: 16px;
          height: 16px;
          border-radius: 999px;
          background: #ffc107;
          color: #3f3000;
          font-size: 0.72rem;
          font-weight: 800;
          line-height: 1;
        }

        .telemetryIncludeOption small {
          color: var(--text-muted);
        }

        @media (max-width: 900px) {
          .telemetryProfilesHeader {
            flex-direction: column;
          }

          .telemetryProfilesHeaderActions {
            width: 100%;
          }
        }
      `}</style>
    </div>
  );
};
