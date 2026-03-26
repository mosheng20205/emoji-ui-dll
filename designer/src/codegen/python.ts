import { DesignControl, DesignWindow } from '../types/controls';
import { hexToArgbNum } from '../utils/color';
import { normalizeControlIdentifier } from '../utils/controlNaming';

function pyColor(hex: string): string {
  const n = hexToArgbNum(hex);
  return `0x${n.toString(16).toUpperCase().padStart(8, '0')}`;
}

function pyBool(v: unknown): string {
  return v ? 'True' : 'False';
}

function escapePy(text: string): string {
  return text.replace(/\\/g, '\\\\').replace(/"/g, '\\"');
}

interface EventHandlerMeta {
  handler: string;
  controlName: string;
  eventName: string;
}

function collectEventHandlers(controls: DesignControl[]): EventHandlerMeta[] {
  const seen = new Set<string>();
  const handlers: EventHandlerMeta[] = [];
  for (const control of controls) {
    for (const [key, value] of Object.entries(control.props)) {
      if (!/^on[A-Z]/.test(key) || typeof value !== 'string' || !value.trim()) {
        continue;
      }
      const handler = normalizeControlIdentifier(value) || `${control.name}_${key.toLowerCase()}`;
      if (seen.has(handler)) {
        continue;
      }
      seen.add(handler);
      handlers.push({
        handler,
        controlName: control.name,
        eventName: key,
      });
    }
  }
  return handlers;
}

export function generatePython(win: DesignWindow, controls: DesignControl[]): string {
  const lines: string[] = [];
  const eventHandlers = collectEventHandlers(controls);
  const byId = new Map(controls.map((control) => [control.id, control]));
  const depthOf = (control: DesignControl): number => {
    let depth = 0;
    let parentId = control.parentId ?? null;
    while (parentId) {
      depth += 1;
      parentId = byId.get(parentId)?.parentId ?? null;
    }
    return depth;
  };
  const sortedControls = [...controls].sort((a, b) => depthOf(a) - depthOf(b));

  lines.push(`import ctypes`);
  lines.push(`from ctypes import c_int, c_uint, c_void_p, c_bool, c_float, WINFUNCTYPE`);
  lines.push(``);
  lines.push(`dll = ctypes.WinDLL("emoji_window.dll")`);
  lines.push(``);

  lines.push(`# ===== DLL 函数声明 =====`);
  lines.push(`dll.create_window_bytes_ex.argtypes = [ctypes.c_char_p, c_int, c_int, c_int, c_int, c_int, c_uint, c_uint]`);
  lines.push(`dll.create_window_bytes_ex.restype = c_void_p`);
  lines.push(`dll.set_button_click_callback.argtypes = [ctypes.CFUNCTYPE(None, c_int, c_void_p)]`);
  lines.push(`dll.set_message_loop_main_window.argtypes = [c_void_p]`);
  lines.push(`dll.run_message_loop.argtypes = []`);
  lines.push(``);

  const hasBtns = controls.some(c => c.type === 'button');
  const hasLabels = controls.some(c => c.type === 'label');
  const hasEdits = controls.some(c => c.type === 'editbox');
  const hasChecks = controls.some(c => c.type === 'checkbox');
  const hasRadios = controls.some(c => c.type === 'radiobutton');
  const hasProgress = controls.some(c => c.type === 'progressbar');
  const hasCombo = controls.some(c => c.type === 'combobox');
  const hasList = controls.some(c => c.type === 'listbox');
  const hasGroup = controls.some(c => c.type === 'groupbox');
  const hasGroupChildren = controls.some(c => c.parentKind === 'groupbox');
  const hasPicture = controls.some(c => c.type === 'picturebox');
  const hasTab = controls.some(c => c.type === 'tabcontrol');
  const hasGrid = controls.some(c => c.type === 'datagridview');
  const hasTree = controls.some((c) => c.type === 'treeview' || c.type === 'treeview_sidebar');
  const hasTreeSidebar = controls.some((c) => c.type === 'treeview_sidebar');
  const hasDtp = controls.some((c) => c.type === 'datetimepicker');

  if (hasBtns) {
    lines.push(`dll.create_emoji_button_bytes.argtypes = [c_void_p, ctypes.c_char_p, c_int, ctypes.c_char_p, c_int, c_int, c_int, c_int, c_int, c_uint]`);
    lines.push(`dll.create_emoji_button_bytes.restype = c_int`);
  }
  if (hasLabels) {
    lines.push(`dll.CreateLabel.argtypes = [c_void_p, c_int, c_int, c_int, c_int, ctypes.c_char_p, c_int, c_uint, c_uint, ctypes.c_char_p, c_int, c_int, c_int, c_int, c_int, c_int, c_int]`);
    lines.push(`dll.CreateLabel.restype = c_void_p`);
  }
  if (hasEdits) {
    lines.push(`dll.CreateEditBox.argtypes = [c_void_p, c_int, c_int, c_int, c_int, ctypes.c_char_p, c_int, c_uint, c_uint, ctypes.c_char_p, c_int, c_int, c_int, c_int, c_int, c_int, c_int, c_int, c_int, c_int, c_int]`);
    lines.push(`dll.CreateEditBox.restype = c_void_p`);
    lines.push(`dll.CreateColorEmojiEditBox.argtypes = [c_void_p, c_int, c_int, c_int, c_int, ctypes.c_char_p, c_int, c_uint, c_uint, ctypes.c_char_p, c_int, c_int, c_int, c_int, c_int, c_int, c_int, c_int, c_int, c_int, c_int]`);
    lines.push(`dll.CreateColorEmojiEditBox.restype = c_void_p`);
  }
  if (hasChecks) {
    lines.push(`dll.CreateCheckBox.argtypes = [c_void_p, c_int, c_int, c_int, c_int, ctypes.c_char_p, c_int, c_uint, c_uint, c_uint, ctypes.c_char_p, c_int, c_int, c_int, c_int, c_int, c_int]`);
    lines.push(`dll.CreateCheckBox.restype = c_void_p`);
  }
  if (hasRadios) {
    lines.push(`dll.CreateRadioButton.argtypes = [c_void_p, c_int, c_int, c_int, c_int, ctypes.c_char_p, c_int, c_uint, c_uint, c_uint, ctypes.c_char_p, c_int, c_int, c_int, c_int, c_int, c_int, c_int]`);
    lines.push(`dll.CreateRadioButton.restype = c_void_p`);
  }
  if (hasProgress) {
    lines.push(`dll.CreateProgressBar.argtypes = [c_void_p, c_int, c_int, c_int, c_int, c_int, c_uint, c_uint, c_int]`);
    lines.push(`dll.CreateProgressBar.restype = c_void_p`);
  }
  if (hasCombo) {
    lines.push(`dll.CreateComboBox.argtypes = [c_void_p, c_int, c_int, c_int, c_int, c_uint, c_uint, ctypes.c_char_p, c_int, c_int, c_int, c_int, c_int]`);
    lines.push(`dll.CreateComboBox.restype = c_void_p`);
  }
  if (hasList) {
    lines.push(`dll.CreateListBox.argtypes = [c_void_p, c_int, c_int, c_int, c_int, c_uint, c_uint, ctypes.c_char_p, c_int, c_int, c_int, c_int, c_int, c_int]`);
    lines.push(`dll.CreateListBox.restype = c_void_p`);
  }
  if (hasGroup) {
    lines.push(`dll.CreateGroupBox.argtypes = [c_void_p, c_int, c_int, c_int, c_int, ctypes.c_char_p, c_int, c_uint, c_uint, ctypes.c_char_p, c_int, c_int, c_int, c_int, c_int]`);
    lines.push(`dll.CreateGroupBox.restype = c_void_p`);
  }
  if (hasGroupChildren) {
    lines.push(`dll.AddChildToGroup.argtypes = [c_void_p, c_void_p]`);
  }
  if (hasPicture) {
    lines.push(`dll.CreatePictureBox.argtypes = [c_void_p, c_int, c_int, c_int, c_int, c_uint, c_uint, c_int]`);
    lines.push(`dll.CreatePictureBox.restype = c_void_p`);
  }
  if (hasTab) {
    lines.push(`dll.CreateTabControl.argtypes = [c_void_p, c_int, c_int, c_int, c_int]`);
    lines.push(`dll.CreateTabControl.restype = c_void_p`);
    lines.push(`dll.AddTabItem.argtypes = [c_void_p, ctypes.c_char_p, c_int, c_void_p]`);
    lines.push(`dll.AddTabItem.restype = c_int`);
    lines.push(`dll.GetTabContentWindow.argtypes = [c_void_p, c_int]`);
    lines.push(`dll.GetTabContentWindow.restype = c_void_p`);
    lines.push(`dll.SetTabColors.argtypes = [c_void_p, c_uint, c_uint, c_uint, c_uint]`);
    lines.push(`dll.SetTabColors.restype = c_int`);
    lines.push(`dll.SetTabIndicatorColor.argtypes = [c_void_p, c_uint]`);
    lines.push(`dll.SetTabIndicatorColor.restype = c_int`);
    lines.push(`dll.SetTabItemSize.argtypes = [c_void_p, c_int, c_int]`);
    lines.push(`dll.SetTabItemSize.restype = c_int`);
    lines.push(`dll.SetTabPadding.argtypes = [c_void_p, c_int, c_int]`);
    lines.push(`dll.SetTabPadding.restype = c_int`);
    lines.push(`dll.SetTabClosable.argtypes = [c_void_p, c_int]`);
    lines.push(`dll.SetTabClosable.restype = c_int`);
    lines.push(`dll.SetTabDraggable.argtypes = [c_void_p, c_int]`);
    lines.push(`dll.SetTabDraggable.restype = c_int`);
    lines.push(`dll.SetTabScrollable.argtypes = [c_void_p, c_int]`);
    lines.push(`dll.SetTabScrollable.restype = c_int`);
    lines.push(`dll.SetTabPosition.argtypes = [c_void_p, c_int]`);
    lines.push(`dll.SetTabPosition.restype = c_int`);
    lines.push(`dll.SetTabAlignment.argtypes = [c_void_p, c_int]`);
    lines.push(`dll.SetTabAlignment.restype = c_int`);
  }
  if (hasGrid) {
    lines.push(`dll.CreateDataGridView.argtypes = [c_void_p, c_int, c_int, c_int, c_int, c_uint, c_uint, c_uint, ctypes.c_char_p, c_int, c_int]`);
    lines.push(`dll.CreateDataGridView.restype = c_void_p`);
  }
  if (hasTree) {
    lines.push(`dll.CreateTreeView.argtypes = [c_void_p, c_int, c_int, c_int, c_int, c_uint, c_uint, ctypes.c_char_p, c_int, c_int, c_int, c_int, c_int, c_int]`);
    lines.push(`dll.CreateTreeView.restype = c_void_p`);
  }
  if (hasTreeSidebar) {
    lines.push(`dll.SetTreeViewSidebarMode.argtypes = [c_void_p, c_bool]`);
    lines.push(`dll.SetTreeViewSidebarMode.restype = c_bool`);
    lines.push(`dll.SetTreeViewRowHeight.argtypes = [c_void_p, c_float]`);
    lines.push(`dll.SetTreeViewRowHeight.restype = c_bool`);
    lines.push(`dll.SetTreeViewItemSpacing.argtypes = [c_void_p, c_float]`);
    lines.push(`dll.SetTreeViewItemSpacing.restype = c_bool`);
    lines.push(`dll.SetTreeViewTextColor.argtypes = [c_void_p, c_uint]`);
    lines.push(`dll.SetTreeViewTextColor.restype = c_bool`);
    lines.push(`dll.SetTreeViewSelectedBgColor.argtypes = [c_void_p, c_uint]`);
    lines.push(`dll.SetTreeViewSelectedBgColor.restype = c_bool`);
    lines.push(`dll.SetTreeViewSelectedForeColor.argtypes = [c_void_p, c_uint]`);
    lines.push(`dll.SetTreeViewSelectedForeColor.restype = c_bool`);
    lines.push(`dll.SetTreeViewHoverBgColor.argtypes = [c_void_p, c_uint]`);
    lines.push(`dll.SetTreeViewHoverBgColor.restype = c_bool`);
  }
  if (hasDtp) {
    lines.push(`dll.CreateD2DDateTimePicker.argtypes = [c_void_p, c_int, c_int, c_int, c_int, c_int, c_uint, c_uint, c_uint, ctypes.c_char_p, c_int, c_int, c_int, c_int, c_int]`);
    lines.push(`dll.CreateD2DDateTimePicker.restype = c_void_p`);
    lines.push(`dll.SetD2DDateTimePickerDateTime.argtypes = [c_void_p, c_int, c_int, c_int, c_int, c_int, c_int]`);
    lines.push(`dll.SetD2DDateTimePickerDateTime.restype = None`);
    lines.push(`dll.EnableD2DDateTimePicker.argtypes = [c_void_p, c_int]`);
    lines.push(`dll.EnableD2DDateTimePicker.restype = None`);
    lines.push(`dll.ShowD2DDateTimePicker.argtypes = [c_void_p, c_int]`);
    lines.push(`dll.ShowD2DDateTimePicker.restype = None`);
  }

  lines.push(``);
  lines.push(`# ===== 创建窗口 =====`);
  const titleFull = `${win.emoji} ${win.title}`.trim();
  lines.push(`title = "${titleFull}".encode("utf-8")`);
  const winBg = (win.bgColor as string) || '#FFFFFF';
  const winX = win.x ?? -1;
  const winY = win.y ?? -1;
  lines.push(`hwnd = dll.create_window_bytes_ex(title, len(title), ${winX}, ${winY}, ${win.width}, ${win.height}, ${pyColor(win.titlebarColor)}, ${pyColor(winBg)})`);
  lines.push(``);

  lines.push(`# ===== 创建控件 =====`);
  const fontCache = new Set<string>();
  for (const c of sortedControls) {
    const p = c.props;
    const fn = (p.fontName as string) || 'Microsoft YaHei UI';
    if (!fontCache.has(fn)) {
      lines.push(`font_${fn.replace(/\s/g, '_')} = "${fn}".encode("utf-8")`);
      fontCache.add(fn);
    }
    const fontVar = `font_${fn.replace(/\s/g, '_')}`;
    const parentExpr = (c.parentKind === 'tabcontrol' || c.parentKind === 'tabpage') && c.parentId
      ? `${c.parentId}_page_${c.parentTabIndex ?? 0}`
      : 'hwnd';
    const attachToGroup = c.parentKind === 'groupbox' && c.parentId;

    switch (c.type) {
      case 'button': {
        const emoji = (p.emoji as string) || '';
        const text = (p.text as string) || '按钮';
        lines.push(``);
        lines.push(`# ${c.name}`);
        if (emoji) {
          lines.push(`emoji_${c.name} = "${escapePy(emoji)}".encode("utf-8")`);
        }
        lines.push(`text_${c.name} = "${escapePy(text)}".encode("utf-8")`);
        lines.push(`${c.name} = dll.create_emoji_button_bytes(${parentExpr}, ${emoji ? `emoji_${c.name}, len(emoji_${c.name})` : 'b"", 0'}, text_${c.name}, len(text_${c.name}), ${c.x}, ${c.y}, ${c.width}, ${c.height}, ${pyColor((p.bgColor as string) || '#409EFF')})`);
        if (attachToGroup) {
          lines.push(`dll.AddChildToGroup(${c.parentId}, ${c.name})`);
        }
        break;
      }
      case 'label': {
        const text = (p.text as string) || '标签';
        lines.push(``);
        lines.push(`# ${c.name}`);
        lines.push(`text_${c.name} = "${escapePy(text)}".encode("utf-8")`);
        lines.push(`${c.name} = dll.CreateLabel(${parentExpr}, ${c.x}, ${c.y}, ${c.width}, ${c.height}, text_${c.name}, len(text_${c.name}), ${pyColor((p.fgColor as string) || '#303133')}, ${pyColor((p.bgColor as string) || '#FFFFFF')}, ${fontVar}, len(${fontVar}), ${(p.fontSize as number) || 13}, ${p.bold ? 1 : 0}, ${p.italic ? 1 : 0}, ${p.underline ? 1 : 0}, ${(p.alignment as number) || 0}, ${p.wordWrap ? 1 : 0})`);
        if (attachToGroup) {
          lines.push(`dll.AddChildToGroup(${c.parentId}, ${c.name})`);
        }
        break;
      }
      case 'editbox': {
        const text = (p.text as string) || '';
        const fn_create = p.emojiSupport ? 'CreateColorEmojiEditBox' : 'CreateEditBox';
        lines.push(``);
        lines.push(`# ${c.name}`);
        lines.push(`text_${c.name} = "${escapePy(text)}".encode("utf-8")`);
        lines.push(`${c.name} = dll.${fn_create}(${parentExpr}, ${c.x}, ${c.y}, ${c.width}, ${c.height}, text_${c.name}, len(text_${c.name}), ${pyColor((p.fgColor as string) || '#303133')}, ${pyColor((p.bgColor as string) || '#FFFFFF')}, ${fontVar}, len(${fontVar}), ${(p.fontSize as number) || 13}, ${p.bold ? 1 : 0}, ${p.italic ? 1 : 0}, ${p.underline ? 1 : 0}, ${(p.alignment as number) || 0}, ${p.multiline ? 1 : 0}, ${p.readOnly ? 1 : 0}, ${p.password ? 1 : 0}, ${p.showBorder !== false ? 1 : 0}, ${p.vertCenter !== false ? 1 : 0})`);
        if (attachToGroup) {
          lines.push(`dll.AddChildToGroup(${c.parentId}, ${c.name})`);
        }
        break;
      }
      case 'checkbox': {
        const text = (p.text as string) || '复选框';
        lines.push(``);
        lines.push(`# ${c.name}`);
        lines.push(`text_${c.name} = "${escapePy(text)}".encode("utf-8")`);
        lines.push(`${c.name} = dll.CreateCheckBox(${parentExpr}, ${c.x}, ${c.y}, ${c.width}, ${c.height}, text_${c.name}, len(text_${c.name}), ${pyColor((p.fgColor as string) || '#303133')}, ${pyColor((p.bgColor as string) || '#FFFFFF')}, ${pyColor((p.checkColor as string) || '#409EFF')}, ${fontVar}, len(${fontVar}), ${(p.fontSize as number) || 13}, ${p.bold ? 1 : 0}, ${p.italic ? 1 : 0}, ${p.underline ? 1 : 0}, ${p.checked ? 1 : 0})`);
        if (attachToGroup) {
          lines.push(`dll.AddChildToGroup(${c.parentId}, ${c.name})`);
        }
        break;
      }
      case 'radiobutton': {
        const text = (p.text as string) || '单选按钮';
        lines.push(``);
        lines.push(`# ${c.name}`);
        lines.push(`text_${c.name} = "${escapePy(text)}".encode("utf-8")`);
        lines.push(`${c.name} = dll.CreateRadioButton(${parentExpr}, ${c.x}, ${c.y}, ${c.width}, ${c.height}, text_${c.name}, len(text_${c.name}), ${pyColor((p.fgColor as string) || '#303133')}, ${pyColor((p.bgColor as string) || '#FFFFFF')}, ${pyColor((p.radioColor as string) || '#409EFF')}, ${fontVar}, len(${fontVar}), ${(p.fontSize as number) || 13}, ${p.bold ? 1 : 0}, ${p.italic ? 1 : 0}, ${p.underline ? 1 : 0}, ${(p.groupId as number) || 1}, ${p.checked ? 1 : 0})`);
        if (attachToGroup) {
          lines.push(`dll.AddChildToGroup(${c.parentId}, ${c.name})`);
        }
        break;
      }
      case 'progressbar': {
        lines.push(``);
        lines.push(`# ${c.name}`);
        lines.push(`${c.name} = dll.CreateProgressBar(${parentExpr}, ${c.x}, ${c.y}, ${c.width}, ${c.height}, ${(p.value as number) || 50}, ${pyColor((p.fgColor as string) || '#409EFF')}, ${pyColor((p.bgColor as string) || '#EBEEF5')}, ${p.showText !== false ? 1 : 0})`);
        if (attachToGroup) {
          lines.push(`dll.AddChildToGroup(${c.parentId}, ${c.name})`);
        }
        break;
      }
      case 'combobox': {
        lines.push(``);
        lines.push(`# ${c.name}`);
        lines.push(`${c.name} = dll.CreateComboBox(${parentExpr}, ${c.x}, ${c.y}, ${c.width}, ${c.height}, ${pyColor((p.fgColor as string) || '#303133')}, ${pyColor((p.bgColor as string) || '#FFFFFF')}, ${fontVar}, len(${fontVar}), ${(p.fontSize as number) || 13}, ${p.bold ? 1 : 0}, ${p.italic ? 1 : 0}, ${p.underline ? 1 : 0})`);
        const items = ((p.items as string) || '').split('\n').filter(Boolean);
        for (const item of items) {
          lines.push(`dll.ComboBoxAddItem(${c.name}, "${escapePy(item)}".encode("utf-8"), len("${escapePy(item)}".encode("utf-8")))`);
        }
        if (attachToGroup) {
          lines.push(`dll.AddChildToGroup(${c.parentId}, ${c.name})`);
        }
        break;
      }
      case 'listbox': {
        lines.push(``);
        lines.push(`# ${c.name}`);
        lines.push(`${c.name} = dll.CreateListBox(${parentExpr}, ${c.x}, ${c.y}, ${c.width}, ${c.height}, ${pyColor((p.fgColor as string) || '#303133')}, ${pyColor((p.bgColor as string) || '#FFFFFF')}, ${fontVar}, len(${fontVar}), ${(p.fontSize as number) || 13}, ${p.bold ? 1 : 0}, ${p.italic ? 1 : 0}, ${p.underline ? 1 : 0}, ${p.multiSelect ? 1 : 0})`);
        const items = ((p.items as string) || '').split('\n').filter(Boolean);
        for (const item of items) {
          lines.push(`dll.ListBoxAddItem(${c.name}, "${escapePy(item)}".encode("utf-8"), len("${escapePy(item)}".encode("utf-8")))`);
        }
        if (attachToGroup) {
          lines.push(`dll.AddChildToGroup(${c.parentId}, ${c.name})`);
        }
        break;
      }
      case 'groupbox': {
        const text = (p.text as string) || '分组框';
        lines.push(``);
        lines.push(`# ${c.name}`);
        lines.push(`text_${c.name} = "${escapePy(text)}".encode("utf-8")`);
        lines.push(`${c.name} = dll.CreateGroupBox(hwnd, ${c.x}, ${c.y}, ${c.width}, ${c.height}, text_${c.name}, len(text_${c.name}), ${pyColor((p.borderColor as string) || '#DCDFE6')}, ${pyColor((p.bgColor as string) || '#FFFFFF')}, ${fontVar}, len(${fontVar}), ${(p.fontSize as number) || 13}, ${p.bold ? 1 : 0}, ${p.italic ? 1 : 0}, ${p.underline ? 1 : 0})`);
        break;
      }
      case 'picturebox': {
        lines.push(``);
        lines.push(`# ${c.name}`);
        lines.push(`${c.name} = dll.CreatePictureBox(${parentExpr}, ${c.x}, ${c.y}, ${c.width}, ${c.height}, ${pyColor((p.bgColor as string) || '#F5F7FA')}, ${pyColor((p.borderColor as string) || '#DCDFE6')}, ${(p.scaleMode as number) || 2})`);
        if (attachToGroup) {
          lines.push(`dll.AddChildToGroup(${c.parentId}, ${c.name})`);
        }
        break;
      }
      case 'tabcontrol': {
        lines.push(``);
        lines.push(`# ${c.name}`);
        lines.push(`${c.name} = dll.CreateTabControl(hwnd, ${c.x}, ${c.y}, ${c.width}, ${c.height})`);
        const tabs = ((p.tabs as string) || '').split('\n').filter(Boolean);
        for (let index = 0; index < tabs.length; index += 1) {
          lines.push(`tab_text = "${escapePy(tabs[index])}".encode("utf-8")`);
          lines.push(`dll.AddTabItem(${c.name}, tab_text, len(tab_text), 0)`);
          lines.push(`${c.name}_page_${index} = dll.GetTabContentWindow(${c.name}, ${index})`);
        }
        // 新增属性：仅在与默认值不同时生成调用代码
        {
          const selBg = (p.selectedBgColor as string) || '#FFFFFF';
          const unselBg = (p.unselectedBgColor as string) || '#F5F7FA';
          const selText = (p.selectedTextColor as string) || '#409EFF';
          const unselText = (p.unselectedTextColor as string) || '#606266';
          if (selBg !== '#FFFFFF' || unselBg !== '#F5F7FA' || selText !== '#409EFF' || unselText !== '#606266') {
            lines.push(`dll.SetTabColors(${c.name}, ${pyColor(selBg)}, ${pyColor(unselBg)}, ${pyColor(selText)}, ${pyColor(unselText)})`);
          }
          const indColor = (p.indicatorColor as string) || '#409EFF';
          if (indColor !== '#409EFF') {
            lines.push(`dll.SetTabIndicatorColor(${c.name}, ${pyColor(indColor)})`);
          }
          const tabW = (p.tabItemWidth as number) ?? 120;
          const tabH = (p.tabItemHeight as number) ?? 34;
          if (tabW !== 120 || tabH !== 34) {
            lines.push(`dll.SetTabItemSize(${c.name}, ${tabW}, ${tabH})`);
          }
          const padH = (p.paddingH as number) ?? 2;
          const padV = (p.paddingV as number) ?? 0;
          if (padH !== 2 || padV !== 0) {
            lines.push(`dll.SetTabPadding(${c.name}, ${padH}, ${padV})`);
          }
          if (p.closable) {
            lines.push(`dll.SetTabClosable(${c.name}, 1)`);
          }
          if (p.draggable) {
            lines.push(`dll.SetTabDraggable(${c.name}, 1)`);
          }
          if (p.scrollable) {
            lines.push(`dll.SetTabScrollable(${c.name}, 1)`);
          }
          const tabPos = (p.tabPosition as number) ?? 0;
          if (tabPos !== 0) {
            lines.push(`dll.SetTabPosition(${c.name}, ${tabPos})`);
          }
          const tabAlign = (p.tabAlignment as number) ?? 0;
          if (tabAlign !== 0) {
            lines.push(`dll.SetTabAlignment(${c.name}, ${tabAlign})`);
          }
        }
        break;
      }
      case 'datagridview': {
        lines.push(``);
        lines.push(`# ${c.name}`);
        lines.push(`${c.name} = dll.CreateDataGridView(${parentExpr}, ${c.x}, ${c.y}, ${c.width}, ${c.height}, ${pyColor((p.fgColor as string) || '#303133')}, ${pyColor((p.bgColor as string) || '#FFFFFF')}, ${pyColor((p.headerColor as string) || '#409EFF')}, ${fontVar}, len(${fontVar}), ${(p.fontSize as number) || 13})`);
        if (attachToGroup) {
          lines.push(`dll.AddChildToGroup(${c.parentId}, ${c.name})`);
        }
        break;
      }
      case 'treeview':
      case 'treeview_sidebar': {
        lines.push(``);
        lines.push(`# ${c.name}`);
        lines.push(`${c.name} = dll.CreateTreeView(${parentExpr}, ${c.x}, ${c.y}, ${c.width}, ${c.height}, ${pyColor((p.fgColor as string) || '#303133')}, ${pyColor((p.bgColor as string) || '#FFFFFF')}, ${fontVar}, len(${fontVar}), ${(p.fontSize as number) || 13}, ${p.bold ? 1 : 0}, ${p.italic ? 1 : 0}, ${p.underline ? 1 : 0}, ${p.showCheckBoxes ? 1 : 0})`);
        if (attachToGroup) {
          lines.push(`dll.AddChildToGroup(${c.parentId}, ${c.name})`);
        }
        if (c.type === 'treeview_sidebar') {
          const rowH = (p.rowHeight as number) ?? 38;
          const rowSp = (p.itemSpacing as number) ?? 2;
          lines.push(`dll.SetTreeViewRowHeight(${c.name}, float(${rowH}))`);
          lines.push(`dll.SetTreeViewItemSpacing(${c.name}, float(${rowSp}))`);
          lines.push(`dll.SetTreeViewTextColor(${c.name}, ${pyColor((p.fgColor as string) || '#303133')})`);
          lines.push(`dll.SetTreeViewSelectedBgColor(${c.name}, ${pyColor((p.selectedBgColor as string) || '#335EEA')})`);
          lines.push(`dll.SetTreeViewSelectedForeColor(${c.name}, ${pyColor((p.selectedForeColor as string) || '#FFFFFF')})`);
          lines.push(`dll.SetTreeViewHoverBgColor(${c.name}, ${pyColor((p.hoverBgColor as string) || '#F5F7FA')})`);
          lines.push(`dll.SetTreeViewSidebarMode(${c.name}, True)`);
        }
        break;
      }
      case 'datetimepicker': {
        lines.push(``);
        lines.push(`# ${c.name}`);
        lines.push(`${c.name} = dll.CreateD2DDateTimePicker(${parentExpr}, ${c.x}, ${c.y}, ${c.width}, ${c.height}, ${(p.precision as number) ?? 4}, ${pyColor((p.fgColor as string) || '#606266')}, ${pyColor((p.bgColor as string) || '#FFFFFF')}, ${pyColor((p.borderColor as string) || '#DCDFE6')}, ${fontVar}, len(${fontVar}), ${(p.fontSize as number) || 14}, ${p.bold ? 1 : 0}, ${p.italic ? 1 : 0}, ${p.underline ? 1 : 0})`);
        lines.push(`dll.SetD2DDateTimePickerDateTime(${c.name}, ${(p.year as number) ?? 2024}, ${(p.month as number) ?? 6}, ${(p.day as number) ?? 15}, ${(p.hour as number) ?? 0}, ${(p.minute as number) ?? 0}, ${(p.second as number) ?? 0})`);
        if (p.enabled === false) {
          lines.push(`dll.EnableD2DDateTimePicker(${c.name}, 0)`);
        }
        if (p.visible === false) {
          lines.push(`dll.ShowD2DDateTimePicker(${c.name}, 0)`);
        }
        if (attachToGroup) {
          lines.push(`dll.AddChildToGroup(${c.parentId}, ${c.name})`);
        }
        break;
      }
    }
  }

  lines.push(``);
  if (eventHandlers.length > 0) {
    lines.push(`# ===== AI 事件占位 =====`);
    for (const handler of eventHandlers) {
      lines.push(`def ${handler.handler}():`);
      lines.push(`    # TODO: ${handler.controlName}.${handler.eventName}`);
      lines.push(`    pass`);
      lines.push(``);
    }
  }

  lines.push(``);
  lines.push(`# ===== 回调函数 =====`);
  if (hasBtns) {
    lines.push(`ButtonClickCB = WINFUNCTYPE(None, c_int, c_void_p)`);
    lines.push(``);
    lines.push(`def on_button_click(button_id, parent_hwnd):`);
    const btns = controls.filter(c => c.type === 'button');
    for (const b of btns) {
      const handler = (b.props.onClick as string) || '';
      const handlerName = normalizeControlIdentifier(handler);
      lines.push(`    if button_id == ${b.name}:`);
      lines.push(`        ${handlerName ? `${handlerName}()` : `print("${b.name} clicked")`}`);
    }
    lines.push(``);
    lines.push(`_btn_cb = ButtonClickCB(on_button_click)`);
    lines.push(`dll.set_button_click_callback(_btn_cb)`);
  }

  lines.push(``);
  lines.push(`# ===== 运行消息循环 =====`);
  lines.push(`dll.set_message_loop_main_window(hwnd)`);
  lines.push(`dll.run_message_loop()`);

  return lines.join('\n');
}
