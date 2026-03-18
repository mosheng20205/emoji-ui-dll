import { getControlDef } from '../data/controlDefs';
import type { DesignControl, DesignWindow } from '../types/controls';
import type { AiDesignDraft, AiDraftControl } from './schema';
import { buildSemanticControlName, createUniqueControlName } from './nameStrategy';

export function applyDraft(draft: AiDesignDraft): { window: DesignWindow; controls: DesignControl[] } {
  const usedNames = new Set<string>();
  const usedIds = new Set<string>();
  const sourceIdMap = new Map<string, string>();

  const controls = draft.controls.map((control, index): DesignControl => {
    const definition = getControlDef(control.type);
    const baseName = buildSemanticControlName(
      control.type,
      control.name || control.id,
      typeof control.props?.text === 'string' ? control.props.text : undefined
    );
    const name = createUniqueControlName(baseName, usedNames);
    let id = control.id?.trim() || name;
    if (usedIds.has(id)) {
      id = `${name}_${index + 1}`;
    }
    usedIds.add(id);
    if (control.id) {
      sourceIdMap.set(control.id, id);
    }
    return {
      id,
      type: control.type,
      name,
      x: control.x,
      y: control.y,
      width: control.width,
      height: control.height,
      parentId: control.parentId ?? null,
      parentKind: control.parentKind ?? 'window',
      parentTabIndex: control.parentTabIndex ?? null,
      props: {
        ...(definition?.defaultProps ?? {}),
        ...(control.props ?? {}),
      },
    };
  }).map((control) => ({
    ...control,
    parentId: control.parentId ? (sourceIdMap.get(control.parentId) ?? control.parentId) : control.parentId,
  }));

  return {
    window: draft.window,
    controls,
  };
}
