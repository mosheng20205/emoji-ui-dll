import { getControlDef } from '../data/controlDefs';
import type { DesignControl, DesignWindow } from '../types/controls';
import type { AiDraftControl, AiOpsDraft } from './schema';
import { buildSemanticControlName, createUniqueControlName } from './nameStrategy';

function collectDescendantIds(controls: DesignControl[], targetId: string): string[] {
  const ids = [targetId];
  for (const control of controls) {
    if (control.parentId === targetId) {
      ids.push(...collectDescendantIds(controls, control.id));
    }
  }
  return ids;
}

function toDesignControl(control: AiDraftControl, existingControls: DesignControl[]): DesignControl {
  const definition = getControlDef(control.type);
  const usedNames = new Set(existingControls.map((item) => item.name));
  const usedIds = new Set(existingControls.map((item) => item.id));
  const baseName = buildSemanticControlName(
    control.type,
    control.name || control.id,
    typeof control.props?.text === 'string' ? control.props.text : undefined
  );
  const name = createUniqueControlName(baseName, usedNames);
  let id = control.id?.trim() || name;
  if (usedIds.has(id)) {
    id = createUniqueControlName(id, usedIds);
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
}

export function applyOps(
  window: DesignWindow,
  controls: DesignControl[],
  ops: AiOpsDraft
): { window: DesignWindow; controls: DesignControl[] } {
  let nextWindow = { ...window };
  let nextControls = structuredClone(controls);

  for (const operation of ops.operations) {
    switch (operation.type) {
      case 'updateWindow':
        nextWindow = { ...nextWindow, ...operation.updates };
        break;
      case 'addControl':
        nextControls.push(toDesignControl(operation.control, nextControls));
        break;
      case 'updateControl':
        nextControls = nextControls.map((control) =>
          control.id === operation.target
            ? {
                ...control,
                ...operation.updates,
                props: {
                  ...control.props,
                  ...(operation.updates.props ?? {}),
                },
              }
            : control
        );
        break;
      case 'removeControl': {
        const removeIds = new Set(collectDescendantIds(nextControls, operation.target));
        nextControls = nextControls.filter((control) => !removeIds.has(control.id));
        break;
      }
      case 'moveControl':
        nextControls = nextControls.map((control) =>
          control.id === operation.target ? { ...control, x: operation.x, y: operation.y } : control
        );
        break;
      case 'resizeControl':
        nextControls = nextControls.map((control) =>
          control.id === operation.target
            ? { ...control, width: operation.width, height: operation.height }
            : control
        );
        break;
      case 'reparentControl':
        nextControls = nextControls.map((control) =>
          control.id === operation.target
            ? {
                ...control,
                parentId: operation.parentId ?? null,
                parentKind: operation.parentKind ?? 'window',
                parentTabIndex: operation.parentTabIndex ?? null,
              }
            : control
        );
        break;
      case 'createEventPlaceholder':
        nextControls = nextControls.map((control) =>
          control.id === operation.target
            ? {
                ...control,
                props: {
                  ...control.props,
                  [operation.eventName]: operation.handlerName,
                },
              }
            : control
        );
        break;
      case 'renameControl':
        {
          const usedNames = new Set(nextControls.filter((control) => control.id !== operation.target).map((control) => control.name));
        nextControls = nextControls.map((control) =>
          control.id === operation.target
            ? {
                ...control,
                name: createUniqueControlName(
                  buildSemanticControlName(control.type, operation.name),
                  usedNames
                ),
              }
            : control
        );
        }
        break;
    }
  }

  return {
    window: nextWindow,
    controls: nextControls,
  };
}
