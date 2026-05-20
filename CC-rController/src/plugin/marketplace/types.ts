/**
 * Plugin Marketplace Type Definitions
 * Types for plugin distribution, signing, and marketplace operations
 */

import type { PluginMetadata, PluginConfig } from '../types';

// Plugin package types
export type PackageFormat = 'tarball' | 'zip' | 'directory';

// Signature algorithm types
export type SignatureAlgorithm = 'RSA-SHA256' | 'RSA-SHA384' | 'RSA-SHA512' | 'Ed25519';

// Plugin package manifest
export interface PluginPackageManifest {
  format: PackageFormat;
  entries: string[];               // List of files in the package
  totalSize: number;                // Total size in bytes
  checksums: Record<string, string>; // SHA-256 checksums for each file
}

// Signature structure
export interface PluginSignature {
  algorithm: SignatureAlgorithm;
  signature: string;               // Base64-encoded signature
  publicKeyId: string;             // Identifier for the public key used
  timestamp: number;               // Signature creation timestamp
}

// Signed plugin package
export interface SignedPluginPackage {
  manifest: PluginPackageManifest;
  signature: PluginSignature;
}

// Plugin package in marketplace
export interface MarketplacePlugin {
  metadata: PluginMetadata;
  packageUrl: string;              // URL to download the package
  signature: PluginSignature;      // Signature for verification
  checksums: Record<string, string>; // SHA-256 checksums
  format: PackageFormat;
  downloadCount: number;
  ratings: number[];
  verified: boolean;               // Whether signature is verified by marketplace
  categories: string[];
  minAppVersion?: string;           // Minimum compatible app version
  createdAt: number;
  updatedAt: number;
}

// Local installed plugin info
export interface InstalledPluginInfo {
  metadata: PluginMetadata;
  installedAt: number;
  updatedAt: number;
  installPath: string;
  config: PluginConfig;
  signatureVerified: boolean;
  source: 'marketplace' | 'local' | 'builtin';
}

// Marketplace listing response
export interface MarketplaceListing {
  plugins: MarketplacePlugin[];
  total: number;
  page: number;
  pageSize: number;
  categories: string[];
}

// Install result
export interface InstallResult {
  success: boolean;
  pluginId?: string;
  version?: string;
  installPath?: string;
  message?: string;
  error?: string;
}

// Uninstall result
export interface UninstallResult {
  success: boolean;
  pluginId: string;
  message?: string;
  error?: string;
}

// Update check result
export interface UpdateInfo {
  pluginId: string;
  currentVersion: string;
  latestVersion: string;
  updateAvailable: boolean;
  changelog?: string;
  downloadUrl?: string;
  signature?: PluginSignature;
}

// Marketplace configuration
export interface MarketplaceConfig {
  marketplaceUrl: string;          // Base URL of the marketplace API
  publicKeyUrl?: string;            // URL to fetch the marketplace public key
  publicKey?: string;              // Embedded public key for verification
  signatureAlgorithm: SignatureAlgorithm;
  autoVerify: boolean;              // Auto-verify signatures on install
  allowUnsigned: boolean;           // Allow installation of unsigned plugins
  proxyUrl?: string;               // Optional proxy for downloads
  timeout: number;                 // Download timeout in ms
  retryCount: number;               // Number of retries for failed downloads
}

// Marketplace error codes - using string literal union type for better TypeScript compatibility
export type MarketplaceErrorCode =
  | 'NETWORK_ERROR'
  | 'VERIFICATION_FAILED'
  | 'SIGNATURE_INVALID'
  | 'SIGNATURE_EXPIRED'
  | 'PACKAGE_CORRUPTED'
  | 'INSTALL_FAILED'
  | 'UNINSTALL_FAILED'
  | 'PLUGIN_NOT_FOUND'
  | 'INCOMPATIBLE_VERSION'
  | 'DEPENDENCY_ERROR'
  | 'PERMISSION_DENIED'
  | 'ALREADY_INSTALLED';

// Marketplace error codes constant (for convenience and consistency)
export const MarketplaceErrorCode = {
  NETWORK_ERROR: 'NETWORK_ERROR',
  VERIFICATION_FAILED: 'VERIFICATION_FAILED',
  SIGNATURE_INVALID: 'SIGNATURE_INVALID',
  SIGNATURE_EXPIRED: 'SIGNATURE_EXPIRED',
  PACKAGE_CORRUPTED: 'PACKAGE_CORRUPTED',
  INSTALL_FAILED: 'INSTALL_FAILED',
  UNINSTALL_FAILED: 'UNINSTALL_FAILED',
  PLUGIN_NOT_FOUND: 'PLUGIN_NOT_FOUND',
  INCOMPATIBLE_VERSION: 'INCOMPATIBLE_VERSION',
  DEPENDENCY_ERROR: 'DEPENDENCY_ERROR',
  PERMISSION_DENIED: 'PERMISSION_DENIED',
  ALREADY_INSTALLED: 'ALREADY_INSTALLED',
} as const;

// Marketplace error
export interface MarketplaceError {
  code: MarketplaceErrorCode;
  message: string;
  details?: unknown;
  pluginId?: string;
}

// Search/filter options
export interface MarketplaceSearchOptions {
  query?: string;
  category?: string;
  author?: string;
  minRating?: number;
  sortBy: 'downloads' | 'rating' | 'recent' | 'name';
  sortOrder: 'asc' | 'desc';
  page: number;
  pageSize: number;
}

// Download progress callback
export type DownloadProgressCallback = (progress: DownloadProgress) => void;

export interface DownloadProgress {
  pluginId: string;
  bytesDownloaded: number;
  totalBytes: number;
  percentage: number;
}

// Plugin file entry for verification
export interface PluginFileEntry {
  path: string;
  checksum: string;
  size: number;
}
