export function hexToArgb(hex: string): string {
  const h = hex.replace('#', '');
  const r = parseInt(h.substring(0, 2), 16);
  const g = parseInt(h.substring(2, 4), 16);
  const b = parseInt(h.substring(4, 6), 16);
  return `0xFF${h.toUpperCase()}`;
}

export function hexToArgbNum(hex: string): number {
  const h = hex.replace('#', '');
  return (0xFF000000 | parseInt(h, 16)) >>> 0;
}

export function hexToRgbComponents(hex: string): { r: number; g: number; b: number } {
  const h = hex.replace('#', '');
  return {
    r: parseInt(h.substring(0, 2), 16),
    g: parseInt(h.substring(2, 4), 16),
    b: parseInt(h.substring(4, 6), 16),
  };
}

export function textToUtf8Bytes(text: string): number[] {
  const encoder = new TextEncoder();
  return Array.from(encoder.encode(text));
}

export function utf8BytesToEplFormat(bytes: number[]): string {
  return `{ ${bytes.join(', ')} }`;
}
