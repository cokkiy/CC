import { invoke } from "@tauri-apps/api/core";

import type {
  TelemetryProfile,
  TelemetryProfilesResponse,
  TelemetrySchemaResponse,
} from "./types";

export const telemetryProfilesApi = {
  async getProfiles(stationId: string): Promise<TelemetryProfilesResponse> {
    return invoke<TelemetryProfilesResponse>("get_station_telemetry_profiles_for_ui", {
      id: stationId,
    });
  },

  async replaceProfiles(
    stationId: string,
    profiles: TelemetryProfile[],
  ): Promise<TelemetryProfilesResponse> {
    const payload = profiles.map((profile) => ({
      id: profile.id,
      name: profile.name,
      enabled: profile.enabled,
      collectionIntervalMs: profile.collectionIntervalMs,
      includes: profile.includes,
    }));

    return invoke<TelemetryProfilesResponse>("replace_station_telemetry_profiles_for_ui", {
      id: stationId,
      profiles: payload,
    });
  },

  async getSchema(stationId: string): Promise<TelemetrySchemaResponse> {
    return invoke<TelemetrySchemaResponse>("get_station_telemetry_schema_for_ui", {
      id: stationId,
    });
  },
};
