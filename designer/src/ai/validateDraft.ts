import type { ControlType, DesignWindow } from '../types/controls';
import type { AiDesignDraft, AiDraftControl, ValidationResult } from './schema';

const VALID_CONTROL_TYPES = new Set<ControlType>([
  'window',
  'button',
  'label',
  'editbox',
  'checkbox',
  'progressbar',
  'picturebox',
  'radiobutton',
  'listbox',
  'combobox',
  'hotkey',
  'groupbox',
  'tabcontrol',
  'datagridview',
  'treeview',
  'treeview_sidebar',
  'datetimepicker',
]);

function isObject(value: unknown): value is Record<string, unknown> {
  return Boolean(value) && typeof value === 'object' && !Array.isArray(value);
}

function isValidWindow(value: unknown): value is DesignWindow {
  return isObject(value)
    && typeof value.title === 'string'
    && typeof value.emoji === 'string'
    && typeof value.width === 'number'
    && typeof value.height === 'number'
    && typeof value.titlebarColor === 'string'
    && typeof value.bgColor === 'string';
}

function isValidDraftControl(value: unknown): value is AiDraftControl {
  return isObject(value)
    && typeof value.type === 'string'
    && VALID_CONTROL_TYPES.has(value.type as ControlType)
    && typeof value.x === 'number'
    && typeof value.y === 'number'
    && typeof value.width === 'number'
    && typeof value.height === 'number';
}

export function validateDraft(input: unknown): ValidationResult<AiDesignDraft> {
  if (!isObject(input)) {
    return { ok: false, errors: ['Draft 必须是对象'] };
  }
  if (!isValidWindow(input.window)) {
    return { ok: false, errors: ['Draft.window 缺少必要字段'] };
  }
  if (!Array.isArray(input.controls) || input.controls.some((control) => !isValidDraftControl(control))) {
    return { ok: false, errors: ['Draft.controls 格式不正确'] };
  }

  return {
    ok: true,
    data: {
      window: input.window,
      controls: input.controls,
      summary: typeof input.summary === 'string' ? input.summary : '',
      warnings: Array.isArray(input.warnings) ? input.warnings.filter((item): item is string => typeof item === 'string') : [],
    },
  };
}
