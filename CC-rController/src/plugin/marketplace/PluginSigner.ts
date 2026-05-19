/**
 * PluginSigner - Cryptographic signature verification for plugins
 * Handles plugin package signing and signature verification
 */

import type {
  PluginSignature,
  SignatureAlgorithm,
  SignedPluginPackage,
  MarketplaceConfig,
  MarketplaceError,
  MarketplaceErrorCode,
  PluginFileEntry,
  PluginPackageManifest,
} from './types';

// Error factory helper
function createError(code: MarketplaceErrorCode, message: string, details?: unknown): MarketplaceError {
  return { code, message, details };
}

/**
 * PluginSigner handles cryptographic signing and verification of plugin packages
 */
export class PluginSigner {
  private publicKey: CryptoKey | null = null;
  private publicKeyId: string = '';
  private algorithm: SignatureAlgorithm = 'RSA-SHA256';

  constructor(private config: MarketplaceConfig) {
    this.algorithm = config.signatureAlgorithm;
  }

  /**
   * Import a public key from PEM format for signature verification
   */
  async importPublicKey(pemKey: string, keyId: string): Promise<void> {
    try {
      // Remove PEM headers and decode base64
      const pemContents = pemKey
        .replace(/-----BEGIN PUBLIC KEY-----/, '')
        .replace(/-----END PUBLIC KEY-----/, '')
        .replace(/\s/g, '');
      
      const keyData = Uint8Array.from(atob(pemContents), c => c.charCodeAt(0));
      
      this.publicKey = await crypto.subtle.importKey(
        'spki',
        keyData.buffer,
        {
          name: 'RSA-PKCS1-v1_5',
          hash: this.getHashAlgorithm(),
        },
        true,
        ['verify']
      );
      
      this.publicKeyId = keyId;
    } catch (error) {
      throw createError(
        'VERIFICATION_FAILED',
        `Failed to import public key: ${error instanceof Error ? error.message : String(error)}`,
        error
      );
    }
  }

  /**
   * Generate a key pair for signing (used by plugin developers)
   */
  async generateKeyPair(): Promise<{ publicKey: string; privateKey: string }> {
    const keyPair = await crypto.subtle.generateKey(
      {
        name: 'RSA-PKCS1-v1_5',
        modulusLength: 4096,
        publicExponent: new Uint8Array([1, 0, 1]),
        hash: this.getHashAlgorithm(),
      },
      true,
      ['sign', 'verify']
    );

    const publicKeyExported = await crypto.subtle.exportKey('spki', keyPair.publicKey);
    const privateKeyExported = await crypto.subtle.exportKey('pkcs8', keyPair.privateKey);

    const publicKeyPem = this.arrayBufferToPem(
      publicKeyExported,
      'PUBLIC KEY'
    );
    const privateKeyPem = this.arrayBufferToPem(
      privateKeyExported,
      'PRIVATE KEY'
    );

    return {
      publicKey: publicKeyPem,
      privateKey: privateKeyPem,
    };
  }

  /**
   * Sign a plugin package manifest
   */
  async signManifest(
    manifest: PluginPackageManifest,
    privateKeyPem: string
  ): Promise<PluginSignature> {
    try {
      const privateKey = await this.importPrivateKey(privateKeyPem);
      const manifestData = JSON.stringify(manifest);
      const encoder = new TextEncoder();
      const data = encoder.encode(manifestData);

      const signature = await crypto.subtle.sign(
        'RSA-PKCS1-v1_5',
        privateKey,
        data
      );

      return {
        algorithm: this.algorithm,
        signature: this.arrayBufferToBase64(signature),
        publicKeyId: this.publicKeyId || 'default',
        timestamp: Date.now(),
      };
    } catch (error) {
      throw createError(
        'VERIFICATION_FAILED',
        `Failed to sign manifest: ${error instanceof Error ? error.message : String(error)}`,
        error
      );
    }
  }

  /**
   * Verify a signature against a manifest
   */
  async verifySignature(
    manifest: PluginPackageManifest,
    signature: PluginSignature
  ): Promise<boolean> {
    if (!this.publicKey && this.config.publicKey) {
      await this.importPublicKey(this.config.publicKey, 'marketplace');
    }

    if (!this.publicKey) {
      throw createError('VERIFICATION_FAILED', 'No public key available for verification');
    }

    try {
      const manifestData = JSON.stringify(manifest);
      const encoder = new TextEncoder();
      const data = encoder.encode(manifestData);
      const signatureBuffer = this.base64ToArrayBuffer(signature.signature);

      const isValid = await crypto.subtle.verify(
        'RSA-PKCS1-v1_5',
        this.publicKey,
        signatureBuffer,
        data
      );

      if (!isValid) {
        console.warn('[PluginSigner] Signature verification failed for key:', signature.publicKeyId);
      }

      return isValid;
    } catch (error) {
      console.error('[PluginSigner] Signature verification error:', error);
      return false;
    }
  }

  /**
   * Verify a complete signed plugin package
   */
  async verifyPackage(pkg: SignedPluginPackage): Promise<{ valid: boolean; error?: MarketplaceError }> {
    try {
      // Verify the signature
      const isValid = await this.verifySignature(pkg.manifest, pkg.signature);
      
      if (!isValid) {
        return {
          valid: false,
          error: createError(
            'SIGNATURE_INVALID',
            'Plugin package signature verification failed',
            { publicKeyId: pkg.signature.publicKeyId }
          ),
        };
      }

      // Verify timestamp (optional: check if signature is not too old)
      const maxAge = 30 * 24 * 60 * 60 * 1000; // 30 days
      const age = Date.now() - pkg.signature.timestamp;
      
      if (age > maxAge) {
        return {
          valid: false,
          error: createError(
            'SIGNATURE_EXPIRED',
            'Plugin signature has expired',
            { age: Math.floor(age / (24 * 60 * 60 * 1000)) + ' days' }
          ),
        };
      }

      return { valid: true };
    } catch (error) {
      return {
        valid: false,
        error: createError(
          'VERIFICATION_FAILED',
          `Package verification error: ${error instanceof Error ? error.message : String(error)}`,
          error
        ),
      };
    }
  }

  /**
   * Calculate SHA-256 checksum for file content
   */
  async calculateChecksum(content: ArrayBuffer): Promise<string> {
    const hashBuffer = await crypto.subtle.digest('SHA-256', content);
    return this.arrayBufferToHex(hashBuffer);
  }

  /**
   * Calculate checksums for multiple files
   */
  async calculateChecksums(
    files: Array<{ path: string; content: ArrayBuffer }>
  ): Promise<Record<string, string>> {
    const checksums: Record<string, string> = {};
    
    await Promise.all(
      files.map(async (file) => {
        checksums[file.path] = await this.calculateChecksum(file.content);
      })
    );
    
    return checksums;
  }

  /**
   * Verify file checksums against a manifest
   */
  async verifyChecksums(
    files: Array<{ path: string; content: ArrayBuffer }>,
    expectedChecksums: Record<string, string>
  ): Promise<{ valid: boolean; mismatches: string[] }> {
    const mismatches: string[] = [];
    
    for (const file of files) {
      const calculated = await this.calculateChecksum(file.content);
      const expected = expectedChecksums[file.path];
      
      if (!expected) {
        mismatches.push(`${file.path}: no expected checksum`);
      } else if (calculated !== expected) {
        mismatches.push(`${file.path}: checksum mismatch`);
      }
    }
    
    return {
      valid: mismatches.length === 0,
      mismatches,
    };
  }

  /**
   * Create a plugin package manifest from files
   */
  async createManifest(
    files: Array<{ path: string; content: ArrayBuffer }>
  ): Promise<PluginPackageManifest> {
    const checksums: Record<string, string> = {};
    let totalSize = 0;
    
    for (const file of files) {
      checksums[file.path] = await this.calculateChecksum(file.content);
      totalSize += file.content.byteLength;
    }
    
    return {
      format: 'tarball',
      entries: files.map(f => f.path),
      totalSize,
      checksums,
    };
  }

  /**
   * Get the appropriate hash algorithm for the signature algorithm
   */
  private getHashAlgorithm(): AlgorithmIdentifier {
    switch (this.algorithm) {
      case 'RSA-SHA256':
        return 'SHA-256';
      case 'RSA-SHA384':
        return 'SHA-384';
      case 'RSA-SHA512':
        return 'SHA-512';
      case 'Ed25519':
        return 'Ed25519';
      default:
        return 'SHA-256';
    }
  }

  /**
   * Import a private key from PEM format for signing
   */
  private async importPrivateKey(pemKey: string): Promise<CryptoKey> {
    const pemContents = pemKey
      .replace(/-----BEGIN PRIVATE KEY-----/, '')
      .replace(/-----END PRIVATE KEY-----/, '')
      .replace(/\s/g, '');
    
    const keyData = Uint8Array.from(atob(pemContents), c => c.charCodeAt(0));
    
    return crypto.subtle.importKey(
      'pkcs8',
      keyData.buffer,
      {
        name: 'RSA-PKCS1-v1_5',
        hash: this.getHashAlgorithm(),
      },
      true,
      ['sign']
    );
  }

  /**
   * Convert ArrayBuffer to hex string
   */
  private arrayBufferToHex(buffer: ArrayBuffer): string {
    return Array.from(new Uint8Array(buffer))
      .map(b => b.toString(16).padStart(2, '0'))
      .join('');
  }

  /**
   * Convert ArrayBuffer to base64 string
   */
  private arrayBufferToBase64(buffer: ArrayBuffer): string {
    const bytes = new Uint8Array(buffer);
    let binary = '';
    for (let i = 0; i < bytes.length; i++) {
      binary += String.fromCharCode(bytes[i]);
    }
    return btoa(binary);
  }

  /**
   * Convert ArrayBuffer to PEM format
   */
  private arrayBufferToPem(buffer: ArrayBuffer, type: string): string {
    const base64 = this.arrayBufferToBase64(buffer);
    const lines = base64.match(/.{1,64}/g) || [];
    return `-----BEGIN ${type}-----\n${lines.join('\n')}\n-----END ${type}-----`;
  }

  /**
   * Convert base64 string to ArrayBuffer
   */
  private base64ToArrayBuffer(base64: string): ArrayBuffer {
    const binary = atob(base64);
    const bytes = new Uint8Array(binary.length);
    for (let i = 0; i < binary.length; i++) {
      bytes[i] = binary.charCodeAt(i);
    }
    return bytes.buffer;
  }
}

// Export a factory function for creating signers
export function createPluginSigner(config: MarketplaceConfig): PluginSigner {
  return new PluginSigner(config);
}
