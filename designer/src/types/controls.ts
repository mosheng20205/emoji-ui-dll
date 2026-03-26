export type ControlType =
  | 'window' | 'button' | 'label' | 'editbox' | 'checkbox'
  | 'progressbar' | 'picturebox' | 'radiobutton' | 'listbox'
  | 'combobox' | 'hotkey' | 'groupbox'   | 'tabcontrol' | 'datagridview'
  | 'treeview' | 'treeview_sidebar' | 'datetimepicker';

export interface FontStyle {
  name: string;
  size: number;
  bold: boolean;
  italic: boolean;
  underline: boolean;
}

export interface ControlProperty {
  key: string;
  label: string;
  type: 'string' | 'number' | 'boolean' | 'color' | 'select' | 'emoji';
  options?: { label: string; value: string | number }[];
  min?: number;
  max?: number;
}

export interface DesignControl {
  id: string;
  type: ControlType;
  name: string;
  x: number;
  y: number;
  width: number;
  height: number;
  parentId?: string | null;
  parentKind?: 'window' | 'groupbox' | 'tabcontrol' | 'tabpage';
  parentTabIndex?: number | null;
  props: Record<string, unknown>;
}

export interface DesignWindow {
  title: string;
  emoji: string;
  x?: number;
  y?: number;
  width: number;
  height: number;
  titlebarColor: string;
  bgColor: string;
}

export interface ControlDefinition {
  type: ControlType;
  label: string;
  icon: string;
  defaultWidth: number;
  defaultHeight: number;
  defaultProps: Record<string, unknown>;
  properties: ControlProperty[];
}

export type CodeLanguage = 'python' | 'csharp' | 'epl';
export type AppView = 'designer' | 'about' | 'settings' | 'ai-assistant';
