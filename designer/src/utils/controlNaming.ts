export function normalizeControlIdentifier(input: string): string {
  const normalized = input
    .normalize('NFKD')
    .replace(/[\u0300-\u036f]/g, '')
    .replace(/[^a-zA-Z0-9_]+/g, '_')
    .replace(/_+/g, '_')
    .replace(/^_+|_+$/g, '')
    .toLowerCase();

  if (!normalized) {
    return '';
  }

  return /^[a-z_]/.test(normalized) ? normalized : `n_${normalized}`;
}
