function normalizeKey(key: string): string {
  const lowered = key.toLowerCase();
  if (lowered === ' ') return 'Space';
  if (lowered === 'escape') return 'Esc';
  if (lowered === 'control' || lowered === 'shift' || lowered === 'alt' || lowered === 'meta') {
    return '';
  }
  return key.length === 1 ? key.toUpperCase() : key[0].toUpperCase() + key.slice(1);
}

export function formatShortcutFromEvent(
  event: Pick<KeyboardEvent, 'ctrlKey' | 'shiftKey' | 'altKey' | 'metaKey' | 'key'>
): string {
  const parts: string[] = [];
  if (event.ctrlKey) parts.push('Ctrl');
  if (event.shiftKey) parts.push('Shift');
  if (event.altKey) parts.push('Alt');
  if (event.metaKey) parts.push('Meta');
  const key = normalizeKey(event.key);
  if (key) parts.push(key);
  return parts.join('+');
}

export function isShortcutMatch(
  event: Pick<KeyboardEvent, 'ctrlKey' | 'shiftKey' | 'altKey' | 'metaKey' | 'key'>,
  shortcut: string
): boolean {
  return formatShortcutFromEvent(event) === shortcut;
}
