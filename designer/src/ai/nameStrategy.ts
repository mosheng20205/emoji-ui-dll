import type { ControlType } from '../types/controls';
import { normalizeControlIdentifier } from '../utils/controlNaming';

export function buildSemanticControlName(type: ControlType, suggestedName?: string, fallbackText?: string): string {
  const normalized = normalizeControlIdentifier(suggestedName || fallbackText || '');
  return normalized || type;
}

export function createUniqueControlName(baseName: string, usedNames: Set<string>): string {
  const seed = normalizeControlIdentifier(baseName) || 'control';
  if (!usedNames.has(seed)) {
    usedNames.add(seed);
    return seed;
  }

  let index = 2;
  let candidate = `${seed}_${index}`;
  while (usedNames.has(candidate)) {
    index += 1;
    candidate = `${seed}_${index}`;
  }
  usedNames.add(candidate);
  return candidate;
}
