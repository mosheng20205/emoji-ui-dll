import { DesignControl, DesignWindow } from '../types/controls';
import { hexToArgbNum } from '../utils/color';
import { normalizeControlIdentifier } from '../utils/controlNaming';

function csColor(hex: string): string {
  const n = hexToArgbNum(hex);
  return `0x${n.toString(16).toUpperCase().padStart(8, '0')}`;
}

function escapeCs(text: string): string {
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

export function generateCSharp(win: DesignWindow, controls: DesignControl[]): string {
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

  lines.push(`using System;`);
  lines.push(`using System.Runtime.InteropServices;`);
  lines.push(`using System.Text;`);
  lines.push(``);
  lines.push(`class Program`);
  lines.push(`{`);
  lines.push(`    const string DLL = "emoji_window.dll";`);
  lines.push(``);
  lines.push(`    static byte[] ToUtf8(string s) => Encoding.UTF8.GetBytes(s);`);
  lines.push(``);

  lines.push(`    // ===== DLL 导入声明 =====`);
  lines.push(`    [DllImport(DLL, CallingConvention = CallingConvention.StdCall)]`);
  lines.push(`    static extern IntPtr create_window_bytes_ex(byte[] title, int titleLen, int x, int y, int w, int h, uint titlebarColor, uint clientBgColor);`);
  lines.push(``);
  lines.push(`    [DllImport(DLL, CallingConvention = CallingConvention.StdCall)]`);
  lines.push(`    static extern void set_message_loop_main_window(IntPtr hwnd);`);
  lines.push(``);
  lines.push(`    [DllImport(DLL, CallingConvention = CallingConvention.StdCall)]`);
  lines.push(`    static extern void run_message_loop();`);
  lines.push(``);

  const types = new Set(controls.map(c => c.type));
  const hasGroupChildren = controls.some(c => c.parentKind === 'groupbox');

  if (types.has('button')) {
    lines.push(`    [UnmanagedFunctionPointer(CallingConvention.StdCall)]`);
    lines.push(`    delegate void ButtonClickCallback(int buttonId, IntPtr parentHwnd);`);
    lines.push(``);
    lines.push(`    [DllImport(DLL, CallingConvention = CallingConvention.StdCall)]`);
    lines.push(`    static extern int create_emoji_button_bytes(IntPtr parent, byte[] emoji, int emojiLen, byte[] text, int textLen, int x, int y, int w, int h, uint bgColor);`);
    lines.push(``);
    lines.push(`    [DllImport(DLL, CallingConvention = CallingConvention.StdCall)]`);
    lines.push(`    static extern void set_button_click_callback(ButtonClickCallback cb);`);
    lines.push(``);
  }
  if (types.has('label')) {
    lines.push(`    [DllImport(DLL, CallingConvention = CallingConvention.StdCall)]`);
    lines.push(`    static extern IntPtr CreateLabel(IntPtr parent, int x, int y, int w, int h, byte[] text, int textLen, uint fg, uint bg, byte[] font, int fontLen, int fontSize, int bold, int italic, int underline, int align, int wordWrap);`);
    lines.push(``);
  }
  if (types.has('editbox')) {
    lines.push(`    [DllImport(DLL, CallingConvention = CallingConvention.StdCall)]`);
    lines.push(`    static extern IntPtr CreateEditBox(IntPtr parent, int x, int y, int w, int h, byte[] text, int textLen, uint fg, uint bg, byte[] font, int fontLen, int fontSize, int bold, int italic, int underline, int align, int multiline, int readOnly, int password, int showBorder, int vertCenter);`);
    lines.push(``);
    lines.push(`    [DllImport(DLL, CallingConvention = CallingConvention.StdCall)]`);
    lines.push(`    static extern IntPtr CreateColorEmojiEditBox(IntPtr parent, int x, int y, int w, int h, byte[] text, int textLen, uint fg, uint bg, byte[] font, int fontLen, int fontSize, int bold, int italic, int underline, int align, int multiline, int readOnly, int password, int showBorder, int vertCenter);`);
    lines.push(``);
  }
  if (types.has('checkbox')) {
    lines.push(`    [DllImport(DLL, CallingConvention = CallingConvention.StdCall)]`);
    lines.push(`    static extern IntPtr CreateCheckBox(IntPtr parent, int x, int y, int w, int h, byte[] text, int textLen, uint fg, uint bg, uint checkColor, byte[] font, int fontLen, int fontSize, int bold, int italic, int underline, int defaultChecked);`);
    lines.push(``);
  }
  if (types.has('radiobutton')) {
    lines.push(`    [DllImport(DLL, CallingConvention = CallingConvention.StdCall)]`);
    lines.push(`    static extern IntPtr CreateRadioButton(IntPtr parent, int x, int y, int w, int h, byte[] text, int textLen, uint fg, uint bg, uint radioColor, byte[] font, int fontLen, int fontSize, int bold, int italic, int underline, int groupId, int defaultChecked);`);
    lines.push(``);
  }
  if (types.has('progressbar')) {
    lines.push(`    [DllImport(DLL, CallingConvention = CallingConvention.StdCall)]`);
    lines.push(`    static extern IntPtr CreateProgressBar(IntPtr parent, int x, int y, int w, int h, int value, uint fg, uint bg, int showText);`);
    lines.push(``);
  }
  if (types.has('combobox')) {
    lines.push(`    [DllImport(DLL, CallingConvention = CallingConvention.StdCall)]`);
    lines.push(`    static extern IntPtr CreateComboBox(IntPtr parent, int x, int y, int w, int h, uint fg, uint bg, byte[] font, int fontLen, int fontSize, int bold, int italic, int underline);`);
    lines.push(``);
  }
  if (types.has('listbox')) {
    lines.push(`    [DllImport(DLL, CallingConvention = CallingConvention.StdCall)]`);
    lines.push(`    static extern IntPtr CreateListBox(IntPtr parent, int x, int y, int w, int h, uint fg, uint bg, byte[] font, int fontLen, int fontSize, int bold, int italic, int underline, int multiSelect);`);
    lines.push(``);
  }
  if (types.has('groupbox')) {
    lines.push(`    [DllImport(DLL, CallingConvention = CallingConvention.StdCall)]`);
    lines.push(`    static extern IntPtr CreateGroupBox(IntPtr parent, int x, int y, int w, int h, byte[] title, int titleLen, uint borderColor, uint bg, byte[] font, int fontLen, int fontSize, int bold, int italic, int underline);`);
    lines.push(``);
  }
  if (hasGroupChildren) {
    lines.push(`    [DllImport(DLL, CallingConvention = CallingConvention.StdCall)]`);
    lines.push(`    static extern void AddChildToGroup(IntPtr group, IntPtr child);`);
    lines.push(``);
  }
  if (types.has('picturebox')) {
    lines.push(`    [DllImport(DLL, CallingConvention = CallingConvention.StdCall)]`);
    lines.push(`    static extern IntPtr CreatePictureBox(IntPtr parent, int x, int y, int w, int h, uint bg, uint borderColor, int scaleMode);`);
    lines.push(``);
  }
  if (types.has('tabcontrol')) {
    lines.push(`    [DllImport(DLL, CallingConvention = CallingConvention.StdCall)]`);
    lines.push(`    static extern IntPtr CreateTabControl(IntPtr parent, int x, int y, int w, int h);`);
    lines.push(``);
    lines.push(`    [DllImport(DLL, CallingConvention = CallingConvention.StdCall)]`);
    lines.push(`    static extern int AddTabItem(IntPtr tabCtrl, byte[] title, int titleLen, IntPtr contentHwnd);`);
    lines.push(``);
    lines.push(`    [DllImport(DLL, CallingConvention = CallingConvention.StdCall)]`);
    lines.push(`    static extern IntPtr GetTabContentWindow(IntPtr tabCtrl, int index);`);
    lines.push(``);
    lines.push(`    [DllImport(DLL, CallingConvention = CallingConvention.StdCall)]`);
    lines.push(`    static extern int SetTabColors(IntPtr hTab, uint selectedBg, uint unselectedBg, uint selectedText, uint unselectedText);`);
    lines.push(``);
    lines.push(`    [DllImport(DLL, CallingConvention = CallingConvention.StdCall)]`);
    lines.push(`    static extern int SetTabIndicatorColor(IntPtr hTab, uint color);`);
    lines.push(``);
    lines.push(`    [DllImport(DLL, CallingConvention = CallingConvention.StdCall)]`);
    lines.push(`    static extern int SetTabItemSize(IntPtr hTab, int width, int height);`);
    lines.push(``);
    lines.push(`    [DllImport(DLL, CallingConvention = CallingConvention.StdCall)]`);
    lines.push(`    static extern int SetTabPadding(IntPtr hTab, int horizontal, int vertical);`);
    lines.push(``);
    lines.push(`    [DllImport(DLL, CallingConvention = CallingConvention.StdCall)]`);
    lines.push(`    static extern int SetTabClosable(IntPtr hTab, int closable);`);
    lines.push(``);
    lines.push(`    [DllImport(DLL, CallingConvention = CallingConvention.StdCall)]`);
    lines.push(`    static extern int SetTabDraggable(IntPtr hTab, int draggable);`);
    lines.push(``);
    lines.push(`    [DllImport(DLL, CallingConvention = CallingConvention.StdCall)]`);
    lines.push(`    static extern int SetTabScrollable(IntPtr hTab, int scrollable);`);
    lines.push(``);
    lines.push(`    [DllImport(DLL, CallingConvention = CallingConvention.StdCall)]`);
    lines.push(`    static extern int SetTabPosition(IntPtr hTab, int position);`);
    lines.push(``);
    lines.push(`    [DllImport(DLL, CallingConvention = CallingConvention.StdCall)]`);
    lines.push(`    static extern int SetTabAlignment(IntPtr hTab, int align);`);
    lines.push(``);
  }
  if (types.has('datagridview')) {
    lines.push(`    [DllImport(DLL, CallingConvention = CallingConvention.StdCall)]`);
    lines.push(`    static extern IntPtr CreateDataGridView(IntPtr parent, int x, int y, int w, int h, uint fg, uint bg, uint headerColor, byte[] font, int fontLen, int fontSize);`);
    lines.push(``);
  }
  if (types.has('treeview') || types.has('treeview_sidebar')) {
    lines.push(`    [DllImport(DLL, CallingConvention = CallingConvention.StdCall)]`);
    lines.push(`    static extern IntPtr CreateTreeView(IntPtr parent, int x, int y, int w, int h, uint fg, uint bg, byte[] font, int fontLen, int fontSize, int bold, int italic, int underline, int showCheckBoxes);`);
    lines.push(``);
  }
  if (types.has('treeview_sidebar')) {
    lines.push(`    [DllImport(DLL, CallingConvention = CallingConvention.StdCall)]`);
    lines.push(`    [return: MarshalAs(UnmanagedType.Bool)]`);
    lines.push(`    static extern bool SetTreeViewSidebarMode(IntPtr hwnd, [MarshalAs(UnmanagedType.Bool)] bool enable);`);
    lines.push(``);
    lines.push(`    [DllImport(DLL, CallingConvention = CallingConvention.StdCall)]`);
    lines.push(`    [return: MarshalAs(UnmanagedType.Bool)]`);
    lines.push(`    static extern bool SetTreeViewRowHeight(IntPtr hwnd, float height);`);
    lines.push(``);
    lines.push(`    [DllImport(DLL, CallingConvention = CallingConvention.StdCall)]`);
    lines.push(`    [return: MarshalAs(UnmanagedType.Bool)]`);
    lines.push(`    static extern bool SetTreeViewItemSpacing(IntPtr hwnd, float spacing);`);
    lines.push(``);
    lines.push(`    [DllImport(DLL, CallingConvention = CallingConvention.StdCall)]`);
    lines.push(`    [return: MarshalAs(UnmanagedType.Bool)]`);
    lines.push(`    static extern bool SetTreeViewTextColor(IntPtr hwnd, uint color);`);
    lines.push(``);
    lines.push(`    [DllImport(DLL, CallingConvention = CallingConvention.StdCall)]`);
    lines.push(`    [return: MarshalAs(UnmanagedType.Bool)]`);
    lines.push(`    static extern bool SetTreeViewSelectedBgColor(IntPtr hwnd, uint color);`);
    lines.push(``);
    lines.push(`    [DllImport(DLL, CallingConvention = CallingConvention.StdCall)]`);
    lines.push(`    [return: MarshalAs(UnmanagedType.Bool)]`);
    lines.push(`    static extern bool SetTreeViewSelectedForeColor(IntPtr hwnd, uint color);`);
    lines.push(``);
    lines.push(`    [DllImport(DLL, CallingConvention = CallingConvention.StdCall)]`);
    lines.push(`    [return: MarshalAs(UnmanagedType.Bool)]`);
    lines.push(`    static extern bool SetTreeViewHoverBgColor(IntPtr hwnd, uint color);`);
    lines.push(``);
  }
  if (types.has('datetimepicker')) {
    lines.push(`    [UnmanagedFunctionPointer(CallingConvention.StdCall)]`);
    lines.push(`    delegate void ValueChangedCallback(IntPtr hwnd);`);
    lines.push(``);
    lines.push(`    [DllImport(DLL, CallingConvention = CallingConvention.StdCall)]`);
    lines.push(`    static extern IntPtr CreateD2DDateTimePicker(IntPtr parent, int x, int y, int w, int h, int initialPrecision, uint fg, uint bg, uint borderColor, byte[] font, int fontLen, int fontSize, int bold, int italic, int underline);`);
    lines.push(``);
    lines.push(`    [DllImport(DLL, CallingConvention = CallingConvention.StdCall)]`);
    lines.push(`    static extern void SetD2DDateTimePickerDateTime(IntPtr hPicker, int year, int month, int day, int hour, int minute, int second);`);
    lines.push(``);
    lines.push(`    [DllImport(DLL, CallingConvention = CallingConvention.StdCall)]`);
    lines.push(`    static extern void SetD2DDateTimePickerCallback(IntPtr hPicker, ValueChangedCallback cb);`);
    lines.push(``);
    lines.push(`    [DllImport(DLL, CallingConvention = CallingConvention.StdCall)]`);
    lines.push(`    static extern void EnableD2DDateTimePicker(IntPtr hPicker, int enable);`);
    lines.push(``);
    lines.push(`    [DllImport(DLL, CallingConvention = CallingConvention.StdCall)]`);
    lines.push(`    static extern void ShowD2DDateTimePicker(IntPtr hPicker, int show);`);
    lines.push(``);
  }

  lines.push(`    // ===== 控件句柄 =====`);
  for (const c of controls) {
    if (c.type === 'button') {
      lines.push(`    static int ${c.name};`);
    } else {
      lines.push(`    static IntPtr ${c.name};`);
    }
  }
  lines.push(``);

  if (types.has('button')) {
    lines.push(`    static ButtonClickCallback _btnCb;`);
    lines.push(``);
  }

  for (const c of controls) {
    if (c.type !== 'datetimepicker') continue;
    const ov = c.props.onValueChanged;
    if (typeof ov === 'string' && ov.trim()) {
      lines.push(`    static ValueChangedCallback _dtpCb_${c.name};`);
    }
  }

  lines.push(`    static void Main()`);
  lines.push(`    {`);
  const titleFull = `${win.emoji} ${win.title}`.trim();
  lines.push(`        byte[] title = ToUtf8("${titleFull}");`);
        const winBg = (win.bgColor as string) || '#FFFFFF';
        const winX = win.x ?? -1;
        const winY = win.y ?? -1;
        lines.push(`        IntPtr hwnd = create_window_bytes_ex(title, title.Length, ${winX}, ${winY}, ${win.width}, ${win.height}, ${csColor(win.titlebarColor)}, ${csColor(winBg)});`);
  lines.push(``);
  lines.push(`        byte[] font;`);

  for (const c of sortedControls) {
    const p = c.props;
    const fn = (p.fontName as string) || 'Microsoft YaHei UI';
    const parentExpr = (c.parentKind === 'tabcontrol' || c.parentKind === 'tabpage') && c.parentId
      ? `${c.parentId}_page_${c.parentTabIndex ?? 0}`
      : 'hwnd';
    const attachToGroup = c.parentKind === 'groupbox' && c.parentId;
    lines.push(``);
    lines.push(`        // ${c.name}`);

    switch (c.type) {
      case 'button': {
        const emoji = (p.emoji as string) || '';
        const text = (p.text as string) || '按钮';
        lines.push(`        byte[] emoji_${c.name} = ${emoji ? `ToUtf8("${escapeCs(emoji)}")` : 'new byte[0]'};`);
        lines.push(`        byte[] text_${c.name} = ToUtf8("${escapeCs(text)}");`);
        lines.push(`        ${c.name} = create_emoji_button_bytes(${parentExpr}, emoji_${c.name}, emoji_${c.name}.Length, text_${c.name}, text_${c.name}.Length, ${c.x}, ${c.y}, ${c.width}, ${c.height}, ${csColor((p.bgColor as string) || '#409EFF')});`);
        if (attachToGroup) lines.push(`        AddChildToGroup(${c.parentId}, ${c.name});`);
        break;
      }
      case 'label': {
        const text = (p.text as string) || '标签';
        lines.push(`        font = ToUtf8("${fn}");`);
        lines.push(`        ${c.name} = CreateLabel(${parentExpr}, ${c.x}, ${c.y}, ${c.width}, ${c.height}, ToUtf8("${escapeCs(text)}"), ${new TextEncoder().encode(text).length}, ${csColor((p.fgColor as string) || '#303133')}, ${csColor((p.bgColor as string) || '#FFFFFF')}, font, font.Length, ${(p.fontSize as number) || 13}, ${p.bold ? 1 : 0}, ${p.italic ? 1 : 0}, ${p.underline ? 1 : 0}, ${(p.alignment as number) || 0}, ${p.wordWrap ? 1 : 0});`);
        if (attachToGroup) lines.push(`        AddChildToGroup(${c.parentId}, ${c.name});`);
        break;
      }
      case 'editbox': {
        const text = (p.text as string) || '';
        const fnCreate = p.emojiSupport ? 'CreateColorEmojiEditBox' : 'CreateEditBox';
        lines.push(`        font = ToUtf8("${fn}");`);
        lines.push(`        ${c.name} = ${fnCreate}(${parentExpr}, ${c.x}, ${c.y}, ${c.width}, ${c.height}, ToUtf8("${escapeCs(text)}"), ${new TextEncoder().encode(text).length}, ${csColor((p.fgColor as string) || '#303133')}, ${csColor((p.bgColor as string) || '#FFFFFF')}, font, font.Length, ${(p.fontSize as number) || 13}, ${p.bold ? 1 : 0}, ${p.italic ? 1 : 0}, ${p.underline ? 1 : 0}, ${(p.alignment as number) || 0}, ${p.multiline ? 1 : 0}, ${p.readOnly ? 1 : 0}, ${p.password ? 1 : 0}, ${p.showBorder !== false ? 1 : 0}, ${p.vertCenter !== false ? 1 : 0});`);
        if (attachToGroup) lines.push(`        AddChildToGroup(${c.parentId}, ${c.name});`);
        break;
      }
      case 'checkbox': {
        const text = (p.text as string) || '复选框';
        lines.push(`        font = ToUtf8("${fn}");`);
        lines.push(`        ${c.name} = CreateCheckBox(${parentExpr}, ${c.x}, ${c.y}, ${c.width}, ${c.height}, ToUtf8("${escapeCs(text)}"), ${new TextEncoder().encode(text).length}, ${csColor((p.fgColor as string) || '#303133')}, ${csColor((p.bgColor as string) || '#FFFFFF')}, ${csColor((p.checkColor as string) || '#409EFF')}, font, font.Length, ${(p.fontSize as number) || 13}, ${p.bold ? 1 : 0}, ${p.italic ? 1 : 0}, ${p.underline ? 1 : 0}, ${p.checked ? 1 : 0});`);
        if (attachToGroup) lines.push(`        AddChildToGroup(${c.parentId}, ${c.name});`);
        break;
      }
      case 'radiobutton': {
        const text = (p.text as string) || '单选按钮';
        lines.push(`        font = ToUtf8("${fn}");`);
        lines.push(`        ${c.name} = CreateRadioButton(${parentExpr}, ${c.x}, ${c.y}, ${c.width}, ${c.height}, ToUtf8("${escapeCs(text)}"), ${new TextEncoder().encode(text).length}, ${csColor((p.fgColor as string) || '#303133')}, ${csColor((p.bgColor as string) || '#FFFFFF')}, ${csColor((p.radioColor as string) || '#409EFF')}, font, font.Length, ${(p.fontSize as number) || 13}, ${p.bold ? 1 : 0}, ${p.italic ? 1 : 0}, ${p.underline ? 1 : 0}, ${(p.groupId as number) || 1}, ${p.checked ? 1 : 0});`);
        if (attachToGroup) lines.push(`        AddChildToGroup(${c.parentId}, ${c.name});`);
        break;
      }
      case 'progressbar':
        lines.push(`        ${c.name} = CreateProgressBar(${parentExpr}, ${c.x}, ${c.y}, ${c.width}, ${c.height}, ${(p.value as number) || 50}, ${csColor((p.fgColor as string) || '#409EFF')}, ${csColor((p.bgColor as string) || '#EBEEF5')}, ${p.showText !== false ? 1 : 0});`);
        if (attachToGroup) lines.push(`        AddChildToGroup(${c.parentId}, ${c.name});`);
        break;
      case 'groupbox': {
        const text = (p.text as string) || '分组框';
        lines.push(`        font = ToUtf8("${fn}");`);
        lines.push(`        ${c.name} = CreateGroupBox(hwnd, ${c.x}, ${c.y}, ${c.width}, ${c.height}, ToUtf8("${escapeCs(text)}"), ${new TextEncoder().encode(text).length}, ${csColor((p.borderColor as string) || '#DCDFE6')}, ${csColor((p.bgColor as string) || '#FFFFFF')}, font, font.Length, ${(p.fontSize as number) || 13}, ${p.bold ? 1 : 0}, ${p.italic ? 1 : 0}, ${p.underline ? 1 : 0});`);
        break;
      }
      case 'picturebox':
        lines.push(`        ${c.name} = CreatePictureBox(${parentExpr}, ${c.x}, ${c.y}, ${c.width}, ${c.height}, ${csColor((p.bgColor as string) || '#F5F7FA')}, ${csColor((p.borderColor as string) || '#DCDFE6')}, ${(p.scaleMode as number) || 2});`);
        if (attachToGroup) lines.push(`        AddChildToGroup(${c.parentId}, ${c.name});`);
        break;
      case 'combobox': {
        lines.push(`        font = ToUtf8("${fn}");`);
        lines.push(`        ${c.name} = CreateComboBox(${parentExpr}, ${c.x}, ${c.y}, ${c.width}, ${c.height}, ${csColor((p.fgColor as string) || '#303133')}, ${csColor((p.bgColor as string) || '#FFFFFF')}, font, font.Length, ${(p.fontSize as number) || 13}, ${p.bold ? 1 : 0}, ${p.italic ? 1 : 0}, ${p.underline ? 1 : 0});`);
        if (attachToGroup) lines.push(`        AddChildToGroup(${c.parentId}, ${c.name});`);
        break;
      }
      case 'listbox': {
        lines.push(`        font = ToUtf8("${fn}");`);
        lines.push(`        ${c.name} = CreateListBox(${parentExpr}, ${c.x}, ${c.y}, ${c.width}, ${c.height}, ${csColor((p.fgColor as string) || '#303133')}, ${csColor((p.bgColor as string) || '#FFFFFF')}, font, font.Length, ${(p.fontSize as number) || 13}, ${p.bold ? 1 : 0}, ${p.italic ? 1 : 0}, ${p.underline ? 1 : 0}, ${p.multiSelect ? 1 : 0});`);
        if (attachToGroup) lines.push(`        AddChildToGroup(${c.parentId}, ${c.name});`);
        break;
      }
      case 'tabcontrol': {
        lines.push(`        ${c.name} = CreateTabControl(hwnd, ${c.x}, ${c.y}, ${c.width}, ${c.height});`);
        const tabs = ((p.tabs as string) || '').split('\n').filter(Boolean);
        for (let index = 0; index < tabs.length; index += 1) {
          lines.push(`        { byte[] t = ToUtf8("${escapeCs(tabs[index])}"); AddTabItem(${c.name}, t, t.Length, IntPtr.Zero); }`);
          lines.push(`        IntPtr ${c.name}_page_${index} = GetTabContentWindow(${c.name}, ${index});`);
        }
        // 新增属性：仅在与默认值不同时生成调用代码
        {
          const selBg = (p.selectedBgColor as string) || '#FFFFFF';
          const unselBg = (p.unselectedBgColor as string) || '#F5F7FA';
          const selText = (p.selectedTextColor as string) || '#409EFF';
          const unselText = (p.unselectedTextColor as string) || '#606266';
          if (selBg !== '#FFFFFF' || unselBg !== '#F5F7FA' || selText !== '#409EFF' || unselText !== '#606266') {
            lines.push(`        SetTabColors(${c.name}, ${csColor(selBg)}, ${csColor(unselBg)}, ${csColor(selText)}, ${csColor(unselText)});`);
          }
          const indColor = (p.indicatorColor as string) || '#409EFF';
          if (indColor !== '#409EFF') {
            lines.push(`        SetTabIndicatorColor(${c.name}, ${csColor(indColor)});`);
          }
          const tabW = (p.tabItemWidth as number) ?? 120;
          const tabH = (p.tabItemHeight as number) ?? 34;
          if (tabW !== 120 || tabH !== 34) {
            lines.push(`        SetTabItemSize(${c.name}, ${tabW}, ${tabH});`);
          }
          const padH = (p.paddingH as number) ?? 2;
          const padV = (p.paddingV as number) ?? 0;
          if (padH !== 2 || padV !== 0) {
            lines.push(`        SetTabPadding(${c.name}, ${padH}, ${padV});`);
          }
          if (p.closable) {
            lines.push(`        SetTabClosable(${c.name}, 1);`);
          }
          if (p.draggable) {
            lines.push(`        SetTabDraggable(${c.name}, 1);`);
          }
          if (p.scrollable) {
            lines.push(`        SetTabScrollable(${c.name}, 1);`);
          }
          const tabPos = (p.tabPosition as number) ?? 0;
          if (tabPos !== 0) {
            lines.push(`        SetTabPosition(${c.name}, ${tabPos});`);
          }
          const tabAlign = (p.tabAlignment as number) ?? 0;
          if (tabAlign !== 0) {
            lines.push(`        SetTabAlignment(${c.name}, ${tabAlign});`);
          }
        }
        break;
      }
      case 'datagridview': {
        lines.push(`        font = ToUtf8("${fn}");`);
        lines.push(`        ${c.name} = CreateDataGridView(${parentExpr}, ${c.x}, ${c.y}, ${c.width}, ${c.height}, ${csColor((p.fgColor as string) || '#303133')}, ${csColor((p.bgColor as string) || '#FFFFFF')}, ${csColor((p.headerColor as string) || '#409EFF')}, font, font.Length, ${(p.fontSize as number) || 13});`);
        if (attachToGroup) lines.push(`        AddChildToGroup(${c.parentId}, ${c.name});`);
        break;
      }
      case 'treeview':
      case 'treeview_sidebar': {
        lines.push(`        font = ToUtf8("${fn}");`);
        lines.push(`        ${c.name} = CreateTreeView(${parentExpr}, ${c.x}, ${c.y}, ${c.width}, ${c.height}, ${csColor((p.fgColor as string) || '#303133')}, ${csColor((p.bgColor as string) || '#FFFFFF')}, font, font.Length, ${(p.fontSize as number) || 13}, ${p.bold ? 1 : 0}, ${p.italic ? 1 : 0}, ${p.underline ? 1 : 0}, ${p.showCheckBoxes ? 1 : 0});`);
        if (attachToGroup) lines.push(`        AddChildToGroup(${c.parentId}, ${c.name});`);
        if (c.type === 'treeview_sidebar') {
          const rowH = (p.rowHeight as number) ?? 38;
          const rowSp = (p.itemSpacing as number) ?? 2;
          lines.push(`        SetTreeViewRowHeight(${c.name}, ${rowH}f);`);
          lines.push(`        SetTreeViewItemSpacing(${c.name}, ${rowSp}f);`);
          lines.push(`        SetTreeViewTextColor(${c.name}, ${csColor((p.fgColor as string) || '#303133')});`);
          lines.push(`        SetTreeViewSelectedBgColor(${c.name}, ${csColor((p.selectedBgColor as string) || '#335EEA')});`);
          lines.push(`        SetTreeViewSelectedForeColor(${c.name}, ${csColor((p.selectedForeColor as string) || '#FFFFFF')});`);
          lines.push(`        SetTreeViewHoverBgColor(${c.name}, ${csColor((p.hoverBgColor as string) || '#F5F7FA')});`);
          lines.push(`        SetTreeViewSidebarMode(${c.name}, true);`);
        }
        break;
      }
      case 'datetimepicker': {
        lines.push(`        font = ToUtf8("${fn}");`);
        lines.push(`        ${c.name} = CreateD2DDateTimePicker(${parentExpr}, ${c.x}, ${c.y}, ${c.width}, ${c.height}, ${(p.precision as number) ?? 4}, ${csColor((p.fgColor as string) || '#606266')}, ${csColor((p.bgColor as string) || '#FFFFFF')}, ${csColor((p.borderColor as string) || '#DCDFE6')}, font, font.Length, ${(p.fontSize as number) || 14}, ${p.bold ? 1 : 0}, ${p.italic ? 1 : 0}, ${p.underline ? 1 : 0});`);
        lines.push(`        SetD2DDateTimePickerDateTime(${c.name}, ${(p.year as number) ?? 2024}, ${(p.month as number) ?? 6}, ${(p.day as number) ?? 15}, ${(p.hour as number) ?? 0}, ${(p.minute as number) ?? 0}, ${(p.second as number) ?? 0});`);
        if (p.enabled === false) {
          lines.push(`        EnableD2DDateTimePicker(${c.name}, 0);`);
        }
        if (p.visible === false) {
          lines.push(`        ShowD2DDateTimePicker(${c.name}, 0);`);
        }
        const ov = (p.onValueChanged as string) || '';
        const hname = normalizeControlIdentifier(ov);
        if (hname) {
          lines.push(`        _dtpCb_${c.name} = DtpVc_${c.name};`);
          lines.push(`        SetD2DDateTimePickerCallback(${c.name}, _dtpCb_${c.name});`);
        }
        if (attachToGroup) lines.push(`        AddChildToGroup(${c.parentId}, ${c.name});`);
        break;
      }
    }
  }

  if (types.has('button')) {
    lines.push(``);
    lines.push(`        // 按钮回调`);
    lines.push(`        _btnCb = OnButtonClick;`);
    lines.push(`        set_button_click_callback(_btnCb);`);
  }

  lines.push(``);
  lines.push(`        set_message_loop_main_window(hwnd);`);
  lines.push(`        run_message_loop();`);
  lines.push(`    }`);

  if (types.has('button')) {
    lines.push(``);
    lines.push(`    static void OnButtonClick(int buttonId, IntPtr parentHwnd)`);
    lines.push(`    {`);
    const btns = controls.filter(c => c.type === 'button');
    for (const b of btns) {
      const handlerName = normalizeControlIdentifier((b.props.onClick as string) || '');
      lines.push(`        if (buttonId == ${b.name}) { ${handlerName ? `${handlerName}();` : `/* TODO: ${b.name} 点击处理 */`} }`);
    }
    lines.push(`    }`);
  }

  for (const c of controls) {
    if (c.type !== 'datetimepicker') continue;
    const ov = (c.props.onValueChanged as string) || '';
    const hname = normalizeControlIdentifier(ov);
    if (!hname) continue;
    lines.push(``);
    lines.push(`    static void DtpVc_${c.name}(IntPtr hwnd)`);
    lines.push(`    {`);
    lines.push(`        ${hname}();`);
    lines.push(`    }`);
  }

  if (eventHandlers.length > 0) {
    lines.push(``);
    lines.push(`    // ===== AI 事件占位 =====`);
    for (const handler of eventHandlers) {
      lines.push(`    static void ${handler.handler}()`);
      lines.push(`    {`);
      lines.push(`        // TODO: ${handler.controlName}.${handler.eventName}`);
      lines.push(`    }`);
      lines.push(``);
    }
  }

  lines.push(`}`);
  return lines.join('\n');
}
