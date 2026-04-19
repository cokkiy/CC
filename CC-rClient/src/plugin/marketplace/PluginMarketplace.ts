/**
 * PluginMarketplace - Plugin distribution, installation, and management
 * Handles plugin marketplace interactions, downloads, and lifecycle
 */

import type {
  MarketplacePlugin,
  MarketplaceListing,
  MarketplaceConfig,
  MarketplaceError,
  MarketplaceErrorCode,
  MarketplaceSearchOptions,
  InstallResult,
  UninstallResult,
  UpdateInfo,
  DownloadProgressCallback,
  DownloadProgress,
  PluginSignature,
  SignedPluginPackage,
  InstalledPluginInfo,
} from './types';

import { PluginSigner } from './PluginSigner';

// Error factory
function createError(
  code: MarketplaceErrorCode,
  message: string,
  details?: unknown,
  pluginId?: string
): MarketplaceError {
  return { code, message, details, pluginId };
}

// Store for installed plugins (would typically use localStorage or IndexedDB in browser)
const INSTALLED_PLUGINS_KEY = 'cc_installed_plugins';

/**
 * PluginMarketplace handles plugin marketplace operations including:
 * - Browsing and searching the marketplace
 * - Downloading and verifying plugin packages
 * - Installing and uninstalling plugins
 * - Managing plugin updates
 */
export class PluginMarketplace {
  private signer: PluginSigner;
  private downloadProgressCallbacks: Map<string, Set<DownloadProgressCallback>> = new Map();

  constructor(private config: MarketplaceConfig) {
    this.signer = new PluginSigner(config);
  }

  /**
   * Initialize the marketplace - load public key and installed plugins
   */
  async initialize(): Promise<void> {
    if (this.config.publicKey) {
      await this.signer.importPublicKey(this.config.publicKey, 'marketplace');
    } else if (this.config.publicKeyUrl) {
      await this.loadPublicKeyFromUrl(this.config.publicKeyUrl);
    }
  }

  /**
   * Load public key from a URL
   */
  private async loadPublicKeyFromUrl(url: string): Promise<void> {
    try {
      const response = await fetch(url, { signal: AbortSignal.timeout(this.config.timeout) });
      if (!response.ok) {
        throw new Error(`HTTP ${response.status}: ${response.statusText}`);
      }
      const data = await response.json() as { publicKey: string; keyId: string };
      await this.signer.importPublicKey(data.publicKey, data.keyId);
    } catch (error) {
      console.error('[PluginMarketplace] Failed to load public key from URL:', error);
      throw createError(
        'NETWORK_ERROR',
        `Failed to load marketplace public key: ${error instanceof Error ? error.message : String(error)}`
      );
    }
  }

  // ==================== Marketplace Browsing ====================

  /**
   * Get the marketplace listing with optional search and filters
   */
  async getListing(options: MarketplaceSearchOptions): Promise<MarketplaceListing> {
    try {
      const params = new URLSearchParams({
        page: options.page.toString(),
        pageSize: options.pageSize.toString(),
        sortBy: options.sortBy,
        sortOrder: options.sortOrder,
      });

      if (options.query) params.set('q', options.query);
      if (options.category) params.set('category', options.category);
      if (options.author) params.set('author', options.author);
      if (options.minRating) params.set('minRating', options.minRating.toString());

      const response = await fetch(
        `${this.config.marketplaceUrl}/plugins?${params}`,
        { signal: AbortSignal.timeout(this.config.timeout) }
      );

      if (!response.ok) {
        throw new Error(`HTTP ${response.status}: ${response.statusText}`);
      }

      return await response.json() as MarketplaceListing;
    } catch (error) {
      if ((error as MarketplaceError).code) {
        throw error;
      }
      throw createError(
        'NETWORK_ERROR',
        `Failed to fetch marketplace listing: ${error instanceof Error ? error.message : String(error)}`
      );
    }
  }

  /**
   * Get a specific plugin from the marketplace
   */
  async getPlugin(pluginId: string): Promise<MarketplacePlugin | null> {
    try {
      const response = await fetch(
        `${this.config.marketplaceUrl}/plugins/${pluginId}`,
        { signal: AbortSignal.timeout(this.config.timeout) }
      );

      if (response.status === 404) {
        return null;
      }

      if (!response.ok) {
        throw new Error(`HTTP ${response.status}: ${response.statusText}`);
      }

      return await response.json() as MarketplacePlugin;
    } catch (error) {
      if ((error as MarketplaceError).code) {
        throw error;
      }
      throw createError(
        'NETWORK_ERROR',
        `Failed to fetch plugin: ${error instanceof Error ? error.message : String(error)}`
      );
    }
  }

  /**
   * Get available categories
   */
  async getCategories(): Promise<string[]> {
    try {
      const response = await fetch(
        `${this.config.marketplaceUrl}/categories`,
        { signal: AbortSignal.timeout(this.config.timeout) }
      );

      if (!response.ok) {
        throw new Error(`HTTP ${response.status}: ${response.statusText}`);
      }

      const data = await response.json() as { categories: string[] };
      return data.categories;
    } catch (error) {
      console.warn('[PluginMarketplace] Failed to fetch categories:', error);
      return [];
    }
  }

  // ==================== Download & Verification ====================

  /**
   * Download a plugin package
   */
  async downloadPlugin(
    plugin: MarketplacePlugin,
    onProgress?: DownloadProgressCallback
  ): Promise<ArrayBuffer> {
    if (onProgress) {
      if (!this.downloadProgressCallbacks.has(plugin.metadata.id)) {
        this.downloadProgressCallbacks.set(plugin.metadata.id, new Set());
      }
      this.downloadProgressCallbacks.get(plugin.metadata.id)!.add(onProgress);
    }

    try {
      const response = await fetch(plugin.packageUrl, {
        signal: AbortSignal.timeout(this.config.timeout),
      });

      if (!response.ok) {
        throw createError(
          'NETWORK_ERROR',
          `Failed to download plugin: HTTP ${response.status}`,
          undefined,
          plugin.metadata.id
        );
      }

      const contentLength = parseInt(response.headers.get('content-length') || '0', 10);
      const reader = response.body?.getReader();
      
      if (!reader) {
        throw createError('NETWORK_ERROR', 'Response body is null', undefined, plugin.metadata.id);
      }

      const chunks: Uint8Array[] = [];
      let bytesDownloaded = 0;

      while (true) {
        const { done, value } = await reader.read();
        if (done) break;

        chunks.push(value);
        bytesDownloaded += value.length;

        this.emitProgress(plugin.metadata.id, {
          pluginId: plugin.metadata.id,
          bytesDownloaded,
          totalBytes: contentLength,
          percentage: contentLength > 0 ? (bytesDownloaded / contentLength) * 100 : 0,
        });
      }

      // Combine all chunks
      const totalLength = chunks.reduce((acc, chunk) => acc + chunk.length, 0);
      const result = new Uint8Array(totalLength);
      let offset = 0;
      for (const chunk of chunks) {
        result.set(chunk, offset);
        offset += chunk.length;
      }

      return result.buffer;
    } catch (error) {
      if ((error as MarketplaceError).code) {
        throw error;
      }
      throw createError(
        'NETWORK_ERROR',
        `Download failed: ${error instanceof Error ? error.message : String(error)}`,
        undefined,
        plugin.metadata.id
      );
    } finally {
      if (onProgress) {
        this.downloadProgressCallbacks.get(plugin.metadata.id)?.delete(onProgress);
      }
    }
  }

  /**
   * Download and verify a plugin package
   */
  async downloadAndVerify(
    plugin: MarketplacePlugin,
    onProgress?: DownloadProgressCallback
  ): Promise<{ packageData: ArrayBuffer; verified: boolean; error?: MarketplaceError }> {
    const packageData = await this.downloadPlugin(plugin, onProgress);

    if (this.config.autoVerify) {
      return this.verifyPackage(plugin, packageData);
    }

    return { packageData, verified: false };
  }

  /**
   * Verify a downloaded package against marketplace data
   */
  async verifyPackage(
    plugin: MarketplacePlugin,
    packageData: ArrayBuffer
  ): Promise<{ packageData: ArrayBuffer; verified: boolean; error?: MarketplaceError }> {
    // Calculate checksum of downloaded package
    const checksum = await this.signer.calculateChecksum(packageData);
    
    // Compare with expected checksum
    const expectedChecksum = plugin.checksums[plugin.packageUrl];
    if (!expectedChecksum || checksum !== expectedChecksum) {
      return {
        packageData,
        verified: false,
        error: createError(
          'PACKAGE_CORRUPTED',
          'Downloaded package checksum does not match expected value',
          { expected: expectedChecksum, calculated: checksum },
          plugin.metadata.id
        ),
      };
    }

    // Verify signature if present
    if (plugin.signature && this.config.autoVerify) {
      const manifest = {
        format: plugin.format,
        entries: [],
        totalSize: packageData.byteLength,
        checksums: plugin.checksums,
      };

      const signatureResult = await this.signer.verifySignature(manifest, plugin.signature);
      if (!signatureResult) {
        return {
          packageData,
          verified: false,
          error: createError(
            'SIGNATURE_INVALID',
            'Plugin signature verification failed',
            { publicKeyId: plugin.signature.publicKeyId },
            plugin.metadata.id
          ),
        };
      }
    }

    return { packageData, verified: true };
  }

  private emitProgress(pluginId: string, progress: DownloadProgress): void {
    this.downloadProgressCallbacks.get(pluginId)?.forEach(callback => {
      try {
        callback(progress);
      } catch (error) {
        console.error('[PluginMarketplace] Progress callback error:', error);
      }
    });
  }

  // ==================== Install/Uninstall ====================

  /**
   * Install a plugin from the marketplace
   */
  async install(
    plugin: MarketplacePlugin,
    onProgress?: DownloadProgressCallback
  ): Promise<InstallResult> {
    try {
      // Check if already installed
      const installed = this.getInstalledPlugins();
      const existing = installed.find(p => p.metadata.id === plugin.metadata.id);
      
      if (existing) {
        return {
          success: false,
          pluginId: plugin.metadata.id,
          error: createError(
            'ALREADY_INSTALLED',
            `Plugin ${plugin.metadata.id} is already installed`,
            undefined,
            plugin.metadata.id
          ).message,
        };
      }

      // Download and verify
      const downloadResult = await this.downloadAndVerify(plugin, onProgress);
      
      if (!downloadResult.verified && this.config.autoVerify) {
        return {
          success: false,
          pluginId: plugin.metadata.id,
          error: downloadResult.error?.message || 'Verification failed',
        };
      }

      // Extract package (placeholder - would use JSZip or similar)
      const files = await this.extractPackage(downloadResult.packageData, plugin.format);

      // Calculate and verify checksums
      const checksumsResult = await this.signer.verifyChecksums(files, plugin.checksums);
      if (!checksumsResult.valid) {
        return {
          success: false,
          pluginId: plugin.metadata.id,
          error: createError(
            'PACKAGE_CORRUPTED',
            `Checksum verification failed for files: ${checksumsResult.mismatches.join(', ')}`,
            { mismatches: checksumsResult.mismatches },
            plugin.metadata.id
          ).message,
        };
      }

      // Determine install path (would use Tauri's path API in real implementation)
      const installPath = this.getInstallPath(plugin.metadata.id);

      // Create plugin info
      const pluginInfo: InstalledPluginInfo = {
        metadata: plugin.metadata,
        installedAt: Date.now(),
        updatedAt: Date.now(),
        installPath,
        config: {
          enabled: true,
          priority: 100,
        },
        signatureVerified: downloadResult.verified,
        source: 'marketplace',
      };

      // Store plugin info
      this.saveInstalledPlugin(pluginInfo);

      console.log(`[PluginMarketplace] Installed plugin: ${plugin.metadata.id} v${plugin.metadata.version}`);

      return {
        success: true,
        pluginId: plugin.metadata.id,
        version: plugin.metadata.version,
        installPath,
      };
    } catch (error) {
      console.error(`[PluginMarketplace] Failed to install plugin ${plugin.metadata.id}:`, error);
      return {
        success: false,
        pluginId: plugin.metadata.id,
        error: createError(
          'INSTALL_FAILED',
          `Installation failed: ${error instanceof Error ? error.message : String(error)}`,
          error,
          plugin.metadata.id
        ).message,
      };
    }
  }

  /**
   * Uninstall a plugin
   */
  async uninstall(pluginId: string, force: boolean = false): Promise<UninstallResult> {
    try {
      const installed = this.getInstalledPlugins();
      const pluginInfo = installed.find(p => p.metadata.id === pluginId);

      if (!pluginInfo) {
        return {
          success: false,
          pluginId,
          error: createError(
            'PLUGIN_NOT_FOUND',
            `Plugin ${pluginId} is not installed`,
            undefined,
            pluginId
          ).message,
        };
      }

      if (pluginInfo.source === 'builtin') {
        return {
          success: false,
          pluginId,
          error: createError(
            'PERMISSION_DENIED',
            'Cannot uninstall built-in plugins',
            undefined,
            pluginId
          ).message,
        };
      }

      // Remove files (placeholder - would use Tauri fs API)
      await this.removePluginFiles(pluginInfo.installPath);

      // Remove from storage
      this.removeInstalledPlugin(pluginId);

      console.log(`[PluginMarketplace] Uninstalled plugin: ${pluginId}`);

      return {
        success: true,
        pluginId,
        message: `Plugin ${pluginId} has been uninstalled`,
      };
    } catch (error) {
      console.error(`[PluginMarketplace] Failed to uninstall plugin ${pluginId}:`, error);
      return {
        success: false,
        pluginId,
        error: createError(
          'UNINSTALL_FAILED',
          `Uninstallation failed: ${error instanceof Error ? error.message : String(error)}`,
          error,
          pluginId
        ).message,
      };
    }
  }

  /**
   * Update a plugin to a new version
   */
  async update(
    pluginId: string,
    onProgress?: DownloadProgressCallback
  ): Promise<InstallResult> {
    const currentPlugin = this.getInstalledPlugins().find(p => p.metadata.id === pluginId);
    
    if (!currentPlugin) {
      return {
        success: false,
        pluginId,
        error: createError('PLUGIN_NOT_FOUND', `Plugin ${pluginId} is not installed`, undefined, pluginId).message,
      };
    }

    // Fetch latest version from marketplace
    const marketplacePlugin = await this.getPlugin(pluginId);
    
    if (!marketplacePlugin) {
      return {
        success: false,
        pluginId,
        error: createError('PLUGIN_NOT_FOUND', `Plugin ${pluginId} not found in marketplace`, undefined, pluginId).message,
      };
    }

    // Check if update is available
    if (currentPlugin.metadata.version === marketplacePlugin.metadata.version) {
      return {
        success: false,
        pluginId,
        error: 'Plugin is already at the latest version',
      };
    }

    // Uninstall old version
    await this.uninstall(pluginId, true);

    // Install new version
    return this.install(marketplacePlugin, onProgress);
  }

  // ==================== Installed Plugins Management ====================

  /**
   * Get all installed plugins
   */
  getInstalledPlugins(): InstalledPluginInfo[] {
    try {
      const stored = localStorage.getItem(INSTALLED_PLUGINS_KEY);
      if (!stored) return [];
      return JSON.parse(stored) as InstalledPluginInfo[];
    } catch (error) {
      console.error('[PluginMarketplace] Failed to load installed plugins:', error);
      return [];
    }
  }

  /**
   * Get a specific installed plugin
   */
  getInstalledPlugin(pluginId: string): InstalledPluginInfo | null {
    const installed = this.getInstalledPlugins();
    return installed.find(p => p.metadata.id === pluginId) || null;
  }

  /**
   * Check for updates for all installed marketplace plugins
   */
  async checkForUpdates(): Promise<UpdateInfo[]> {
    const installed = this.getInstalledPlugins();
    const updates: UpdateInfo[] = [];

    for (const plugin of installed) {
      if (plugin.source === 'builtin') continue;

      try {
        const marketplacePlugin = await this.getPlugin(plugin.metadata.id);
        if (!marketplacePlugin) continue;

        const currentVersion = plugin.metadata.version;
        const latestVersion = marketplacePlugin.metadata.version;

        if (this.compareVersions(latestVersion, currentVersion) > 0) {
          updates.push({
            pluginId: plugin.metadata.id,
            currentVersion,
            latestVersion,
            updateAvailable: true,
            changelog: undefined,
            downloadUrl: marketplacePlugin.packageUrl,
            signature: marketplacePlugin.signature,
          });
        }
      } catch (error) {
        console.warn(`[PluginMarketplace] Failed to check update for ${plugin.metadata.id}:`, error);
      }
    }

    return updates;
  }

  /**
   * Get update info for a specific plugin
   */
  async getUpdateInfo(pluginId: string): Promise<UpdateInfo | null> {
    const installed = this.getInstalledPlugins().find(p => p.metadata.id === pluginId);
    if (!installed) return null;

    const marketplacePlugin = await this.getPlugin(pluginId);
    if (!marketplacePlugin) return null;

    const currentVersion = installed.metadata.version;
    const latestVersion = marketplacePlugin.metadata.version;

    return {
      pluginId,
      currentVersion,
      latestVersion,
      updateAvailable: this.compareVersions(latestVersion, currentVersion) > 0,
      changelog: undefined,
      downloadUrl: marketplacePlugin.packageUrl,
      signature: marketplacePlugin.signature,
    };
  }

  // ==================== Private Helpers ====================

  /**
   * Extract plugin package (placeholder implementation)
   */
  private async extractPackage(
    _data: ArrayBuffer,
    _format: string
  ): Promise<Array<{ path: string; content: ArrayBuffer }>> {
    // This is a placeholder. In a real implementation, you would:
    // - For tarball: use a library like Tar.js or pako
    // - For zip: use JSZip
    // - For directory: just return the file list
    return [];
  }

  /**
   * Remove plugin files from install path (placeholder)
   */
  private async removePluginFiles(_path: string): Promise<void> {
    // This is a placeholder. In a real implementation, you would:
    // - Use Tauri fs API to recursively delete files
    console.log('[PluginMarketplace] Placeholder: removePluginFiles');
  }

  /**
   * Get the install path for a plugin
   */
  private getInstallPath(pluginId: string): string {
    // This would typically use Tauri path API
    // For now, return a placeholder path
    return `plugins/${pluginId}`;
  }

  /**
   * Save installed plugin to storage
   */
  private saveInstalledPlugin(pluginInfo: InstalledPluginInfo): void {
    try {
      const installed = this.getInstalledPlugins();
      const existingIndex = installed.findIndex(p => p.metadata.id === pluginInfo.metadata.id);
      
      if (existingIndex >= 0) {
        installed[existingIndex] = pluginInfo;
      } else {
        installed.push(pluginInfo);
      }

      localStorage.setItem(INSTALLED_PLUGINS_KEY, JSON.stringify(installed));
    } catch (error) {
      console.error('[PluginMarketplace] Failed to save installed plugin:', error);
    }
  }

  /**
   * Remove installed plugin from storage
   */
  private removeInstalledPlugin(pluginId: string): void {
    try {
      const installed = this.getInstalledPlugins().filter(p => p.metadata.id !== pluginId);
      localStorage.setItem(INSTALLED_PLUGINS_KEY, JSON.stringify(installed));
    } catch (error) {
      console.error('[PluginMarketplace] Failed to remove installed plugin:', error);
    }
  }

  /**
   * Compare semantic versions
   */
  private compareVersions(a: string, b: string): number {
    const partsA = a.split('.').map(Number);
    const partsB = b.split('.').map(Number);

    for (let i = 0; i < Math.max(partsA.length, partsB.length); i++) {
      const partA = partsA[i] || 0;
      const partB = partsB[i] || 0;
      
      if (partA > partB) return 1;
      if (partA < partB) return -1;
    }

    return 0;
  }

  /**
   * Get the signer instance for direct use
   */
  getSigner(): PluginSigner {
    return this.signer;
  }
}

// ==================== Default Configuration ====================

export const DEFAULT_MARKETPLACE_CONFIG: MarketplaceConfig = {
  marketplaceUrl: 'https://plugins.cc Marketplace',
  signatureAlgorithm: 'RSA-SHA256',
  autoVerify: true,
  allowUnsigned: false,
  timeout: 30000,
  retryCount: 3,
};

// ==================== Factory Function ====================

export function createPluginMarketplace(config: Partial<MarketplaceConfig>): PluginMarketplace {
  return new PluginMarketplace({ ...DEFAULT_MARKETPLACE_CONFIG, ...config });
}

// ==================== Singleton Instance ====================

let marketplaceInstance: PluginMarketplace | null = null;

export function getPluginMarketplace(): PluginMarketplace {
  if (!marketplaceInstance) {
    marketplaceInstance = createPluginMarketplace(DEFAULT_MARKETPLACE_CONFIG);
  }
  return marketplaceInstance;
}
