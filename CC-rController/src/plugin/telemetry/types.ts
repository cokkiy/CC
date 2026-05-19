export type TelemetryProfile = {
  id: string;
  name: string;
  enabled: boolean;
  collectionIntervalMs: number;
  includes: string[];
};

export type TelemetryProfileDraft = TelemetryProfile;

export type TelemetryIncludeDefinition = {
  key: string;
  label: string;
  description: string;
};

export type TelemetryProfilesResponse = {
  schemaVersion: number;
  profilesVersion: number;
  profiles: TelemetryProfile[];
};

export type TelemetrySchemaResponse = {
  schemaVersion: number;
  supportedIncludes: TelemetryIncludeDefinition[];
};
