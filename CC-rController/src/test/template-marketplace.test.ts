import { describe, expect, it } from "vitest";
import { readTemplateFromFile } from "../plugin/components/TemplateMarketplace";

const templatePayload = {
  metadata: {
    id: "local-template",
    name: "Local Template",
    version: "1.0.0",
    category: "custom",
  },
  content: {
    layout: {
      version: "1.0.0",
      name: "Local Layout",
      panels: [],
    },
  },
};

function encodeText(value: string) {
  return new TextEncoder().encode(value);
}

function pushUInt16(target: number[], value: number) {
  target.push(value & 0xff, (value >> 8) & 0xff);
}

function pushUInt32(target: number[], value: number) {
  target.push(value & 0xff, (value >> 8) & 0xff, (value >> 16) & 0xff, (value >> 24) & 0xff);
}

function createStoredZip(entries: Array<{ name: string; data: Uint8Array }>) {
  const localParts: number[] = [];
  const centralParts: number[] = [];
  let offset = 0;

  for (const entry of entries) {
    const nameBytes = encodeText(entry.name);
    const localHeader: number[] = [];
    pushUInt32(localHeader, 0x04034b50);
    pushUInt16(localHeader, 20);
    pushUInt16(localHeader, 0);
    pushUInt16(localHeader, 0);
    pushUInt16(localHeader, 0);
    pushUInt16(localHeader, 0);
    pushUInt32(localHeader, 0);
    pushUInt32(localHeader, entry.data.length);
    pushUInt32(localHeader, entry.data.length);
    pushUInt16(localHeader, nameBytes.length);
    pushUInt16(localHeader, 0);
    localHeader.push(...nameBytes);

    localParts.push(...localHeader, ...entry.data);

    const centralHeader: number[] = [];
    pushUInt32(centralHeader, 0x02014b50);
    pushUInt16(centralHeader, 20);
    pushUInt16(centralHeader, 20);
    pushUInt16(centralHeader, 0);
    pushUInt16(centralHeader, 0);
    pushUInt16(centralHeader, 0);
    pushUInt16(centralHeader, 0);
    pushUInt32(centralHeader, 0);
    pushUInt32(centralHeader, entry.data.length);
    pushUInt32(centralHeader, entry.data.length);
    pushUInt16(centralHeader, nameBytes.length);
    pushUInt16(centralHeader, 0);
    pushUInt16(centralHeader, 0);
    pushUInt16(centralHeader, 0);
    pushUInt16(centralHeader, 0);
    pushUInt32(centralHeader, 0);
    pushUInt32(centralHeader, offset);
    centralHeader.push(...nameBytes);
    centralParts.push(...centralHeader);

    offset += localHeader.length + entry.data.length;
  }

  const eocd: number[] = [];
  pushUInt32(eocd, 0x06054b50);
  pushUInt16(eocd, 0);
  pushUInt16(eocd, 0);
  pushUInt16(eocd, entries.length);
  pushUInt16(eocd, entries.length);
  pushUInt32(eocd, centralParts.length);
  pushUInt32(eocd, localParts.length);
  pushUInt16(eocd, 0);

  return new Uint8Array([...localParts, ...centralParts, ...eocd]);
}

describe("readTemplateFromFile", () => {
  it("reads a JSON template file", async () => {
    const file = new File([JSON.stringify(templatePayload)], "template.json", {
      type: "application/json",
    });

    await expect(readTemplateFromFile(file)).resolves.toMatchObject({
      metadata: { id: "local-template" },
      content: { layout: { name: "Local Layout" } },
    });
  });

  it("reads a ZIP template package with local previews", async () => {
    const zipBytes = createStoredZip([
      {
        name: "bundle/template.json",
        data: encodeText(JSON.stringify(templatePayload)),
      },
      {
        name: "bundle/preview/thumb.png",
        data: new Uint8Array([137, 80, 78, 71]),
      },
    ]);

    const file = new File([zipBytes], "template.zip", {
      type: "application/zip",
    });

    await expect(readTemplateFromFile(file)).resolves.toMatchObject({
      bundle: { format: "zip", needsExtraction: false },
      preview: [{ type: "thumbnail" }],
    });
  });

  it("rejects ZIP packages without template.json", async () => {
    const zipBytes = createStoredZip([
      {
        name: "bundle/preview/thumb.png",
        data: new Uint8Array([137, 80, 78, 71]),
      },
    ]);

    const file = new File([zipBytes], "broken.zip", {
      type: "application/zip",
    });

    await expect(readTemplateFromFile(file)).rejects.toThrow(/template\.json/i);
  });
});
