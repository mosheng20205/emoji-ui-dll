import type { ControlType, DesignWindow } from '../types/controls';

export type AiParentKind = 'window' | 'groupbox' | 'tabcontrol' | 'tabpage';

export interface AiDraftControl {
  id?: string;
  type: ControlType;
  name?: string;
  x: number;
  y: number;
  width: number;
  height: number;
  parentId?: string | null;
  parentKind?: AiParentKind;
  parentTabIndex?: number | null;
  props?: Record<string, unknown>;
}

export interface AiDesignDraft {
  window: DesignWindow;
  controls: AiDraftControl[];
  summary: string;
  warnings: string[];
}

export interface AddControlOperation {
  type: 'addControl';
  control: AiDraftControl;
}

export interface UpdateControlOperation {
  type: 'updateControl';
  target: string;
  updates: Partial<Omit<AiDraftControl, 'type'>> & { props?: Record<string, unknown> };
}

export interface RemoveControlOperation {
  type: 'removeControl';
  target: string;
}

export interface MoveControlOperation {
  type: 'moveControl';
  target: string;
  x: number;
  y: number;
}

export interface ResizeControlOperation {
  type: 'resizeControl';
  target: string;
  width: number;
  height: number;
}

export interface ReparentControlOperation {
  type: 'reparentControl';
  target: string;
  parentId?: string | null;
  parentKind?: AiParentKind;
  parentTabIndex?: number | null;
}

export interface UpdateWindowOperation {
  type: 'updateWindow';
  updates: Partial<DesignWindow>;
}

export interface CreateEventPlaceholderOperation {
  type: 'createEventPlaceholder';
  target: string;
  eventName: string;
  handlerName: string;
}

export interface RenameControlOperation {
  type: 'renameControl';
  target: string;
  name: string;
}

export type AiOperation =
  | AddControlOperation
  | UpdateControlOperation
  | RemoveControlOperation
  | MoveControlOperation
  | ResizeControlOperation
  | ReparentControlOperation
  | UpdateWindowOperation
  | CreateEventPlaceholderOperation
  | RenameControlOperation;

export interface AiOpsDraft {
  operations: AiOperation[];
  affectedControls: string[];
  summary: string;
  warnings: string[];
}

export type ValidationResult<T> =
  | { ok: true; data: T }
  | { ok: false; errors: string[] };
