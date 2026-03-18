import type { DesignControl } from '../types/controls';

export type AlignMode = 'left' | 'right' | 'top' | 'bottom' | 'hcenter' | 'vcenter';
export type ResizeHandle = 'e' | 's' | 'se';

function uniqueId(baseId: string, usedIds: Set<string>): string {
  let index = 1;
  let nextId = `${baseId}_copy_${index}`;
  while (usedIds.has(nextId)) {
    index += 1;
    nextId = `${baseId}_copy_${index}`;
  }
  usedIds.add(nextId);
  return nextId;
}

function collectDescendants(controls: DesignControl[], selectedIds: Set<string>): DesignControl[] {
  const queue = [...selectedIds];
  const allIds = new Set(selectedIds);

  while (queue.length > 0) {
    const current = queue.shift()!;
    for (const control of controls) {
      if (control.parentId === current && !allIds.has(control.id)) {
        allIds.add(control.id);
        queue.push(control.id);
      }
    }
  }

  return controls.filter((control) => allIds.has(control.id));
}

export function toggleSelection(current: string[], id: string, append: boolean): string[] {
  if (!append) {
    return [id];
  }
  return current.includes(id)
    ? current.filter((item) => item !== id)
    : [...current, id];
}

export function alignControls(
  controls: DesignControl[],
  selectedIds: string[],
  mode: AlignMode
): DesignControl[] {
  const selected = controls.filter((control) => selectedIds.includes(control.id));
  if (selected.length < 2) {
    return controls;
  }

  const left = Math.min(...selected.map((item) => item.x));
  const right = Math.max(...selected.map((item) => item.x + item.width));
  const top = Math.min(...selected.map((item) => item.y));
  const bottom = Math.max(...selected.map((item) => item.y + item.height));
  const centerX = Math.round((left + right) / 2);
  const centerY = Math.round((top + bottom) / 2);

  return controls.map((control) => {
    if (!selectedIds.includes(control.id)) {
      return control;
    }

    switch (mode) {
      case 'left':
        return { ...control, x: left };
      case 'right':
        return { ...control, x: right - control.width };
      case 'top':
        return { ...control, y: top };
      case 'bottom':
        return { ...control, y: bottom - control.height };
      case 'hcenter':
        return { ...control, x: centerX - Math.round(control.width / 2) };
      case 'vcenter':
        return { ...control, y: centerY - Math.round(control.height / 2) };
    }
  });
}

export function duplicateControls(
  controls: DesignControl[],
  selectedIds: string[]
): { all: DesignControl[]; created: DesignControl[] } {
  const usedIds = new Set(controls.map((control) => control.id));
  const selected = collectDescendants(controls, new Set(selectedIds));
  const selectedMap = new Map(selected.map((control) => [control.id, control]));
  const idMap = new Map<string, string>();

  const created = selected.map((control) => {
    const newId = uniqueId(control.id, usedIds);
    idMap.set(control.id, newId);
    return {
      ...control,
      id: newId,
      name: uniqueId(control.name, usedIds),
      x: control.x + 20,
      y: control.y + 20,
    };
  });

  const normalized = created.map((control) => ({
    ...control,
    parentId: control.parentId && selectedMap.has(control.parentId)
      ? idMap.get(control.parentId) ?? control.parentId
      : control.parentId,
  }));

  return {
    all: [...controls, ...normalized],
    created: normalized.filter((control) => selectedIds.includes(
      [...idMap.entries()].find(([, nextId]) => nextId === control.id)?.[0] ?? ''
    )),
  };
}

export function resizeWindowByHandle(
  current: { width: number; height: number },
  handle: ResizeHandle,
  delta: { dx: number; dy: number },
  limits: { minWidth: number; minHeight: number }
): { width: number; height: number } {
  const next = { ...current };

  if (handle === 'e' || handle === 'se') {
    next.width = Math.max(limits.minWidth, current.width + delta.dx);
  }
  if (handle === 's' || handle === 'se') {
    next.height = Math.max(limits.minHeight, current.height + delta.dy);
  }

  return next;
}
