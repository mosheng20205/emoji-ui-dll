import type { DesignControl } from '../types/controls';

export interface Rect {
  x: number;
  y: number;
  width: number;
  height: number;
}

export function isContainerControl(control: DesignControl): boolean {
  return control.type === 'groupbox' || control.type === 'tabcontrol';
}

export function getTabTitles(control: DesignControl): string[] {
  return String(control.props.tabs || '页面1\n页面2\n页面3')
    .split('\n')
    .map((item) => item.trim())
    .filter(Boolean);
}

export function getActiveTab(control: DesignControl): number {
  const index = Number(control.props.activeTab ?? 0);
  return Number.isNaN(index) ? 0 : Math.max(0, index);
}

export function getContentRect(control: DesignControl): Rect {
  if (control.type === 'groupbox') {
    return {
      x: 12,
      y: 24,
      width: Math.max(0, control.width - 24),
      height: Math.max(0, control.height - 34),
    };
  }

  if (control.type === 'tabcontrol') {
    return {
      x: 1,
      y: 30,
      width: Math.max(0, control.width - 2),
      height: Math.max(0, control.height - 31),
    };
  }

  return {
    x: 0,
    y: 0,
    width: control.width,
    height: control.height,
  };
}

export function getChildControls(
  controls: DesignControl[],
  parentId: string | null,
  parentTabIndex: number | null = null
): DesignControl[] {
  return controls.filter((control) => {
    if ((control.parentId ?? null) !== parentId) {
      return false;
    }
    if (parentTabIndex === null) {
      return true;
    }
    return (control.parentTabIndex ?? null) === parentTabIndex;
  });
}

export function getAbsoluteRect(
  control: DesignControl,
  controls: DesignControl[]
): Rect {
  if (!control.parentId) {
    return { x: control.x, y: control.y, width: control.width, height: control.height };
  }

  const parent = controls.find((item) => item.id === control.parentId);
  if (!parent) {
    return { x: control.x, y: control.y, width: control.width, height: control.height };
  }

  const parentRect = getAbsoluteRect(parent, controls);
  const contentRect = getContentRect(parent);
  return {
    x: parentRect.x + contentRect.x + control.x,
    y: parentRect.y + contentRect.y + control.y,
    width: control.width,
    height: control.height,
  };
}

export function pointInRect(x: number, y: number, rect: Rect): boolean {
  return x >= rect.x && y >= rect.y && x <= rect.x + rect.width && y <= rect.y + rect.height;
}

export function rectIntersects(a: Rect, b: Rect): boolean {
  return !(
    a.x + a.width < b.x ||
    b.x + b.width < a.x ||
    a.y + a.height < b.y ||
    b.y + b.height < a.y
  );
}

export function findDropTarget(
  controls: DesignControl[],
  x: number,
  y: number
): { parentId: string | null; parentKind: 'window' | 'groupbox' | 'tabcontrol'; parentTabIndex: number | null } {
  for (let index = controls.length - 1; index >= 0; index -= 1) {
    const control = controls[index];
    if (!isContainerControl(control)) {
      continue;
    }

    const absolute = getAbsoluteRect(control, controls);
    const content = getContentRect(control);
    const contentAbsolute = {
      x: absolute.x + content.x,
      y: absolute.y + content.y,
      width: content.width,
      height: content.height,
    };

    if (!pointInRect(x, y, contentAbsolute)) {
      continue;
    }

    if (control.type === 'tabcontrol') {
      return {
        parentId: control.id,
        parentKind: 'tabcontrol',
        parentTabIndex: getActiveTab(control),
      };
    }

    return {
      parentId: control.id,
      parentKind: 'groupbox',
      parentTabIndex: null,
    };
  }

  return {
    parentId: null,
    parentKind: 'window',
    parentTabIndex: null,
  };
}
