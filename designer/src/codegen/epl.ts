import { DesignControl, DesignWindow } from '../types/controls';
import { textToUtf8Bytes, utf8BytesToEplFormat } from '../utils/color';
import { normalizeControlIdentifier } from '../utils/controlNaming';

function eplColor(hex: string): string {
  const h = hex.replace('#', '');
  const n = (0xFF000000 | parseInt(h, 16)) >>> 0;
  return n.toString();
}

function eplBool(v: unknown): string {
  return v ? '真' : '假';
}

function hasNonGbkChars(text: string): boolean {
  for (const ch of text) {
    const code = ch.codePointAt(0)!;
    if (code > 0xFFFF || (code > 0x7F && code > 0x9FFF)) {
      return true;
    }
  }
  return false;
}

function splitLeadingEmoji(text: string): { emoji: string; rest: string } {
  const emojiRegex = /^(\p{Emoji_Presentation}|\p{Extended_Pictographic})+/u;
  const match = text.match(emojiRegex);
  if (match) {
    return { emoji: match[0], rest: text.slice(match[0].length) };
  }
  return { emoji: '', rest: text };
}

function emitUtf8Text(varName: string, text: string, lines: string[]): void {
  if (hasNonGbkChars(text)) {
    const bytes = textToUtf8Bytes(text);
    lines.push(`${varName} ＝ ${utf8BytesToEplFormat(bytes)}`);
  } else {
    lines.push(`${varName} ＝ 编码_Ansi到Utf8 ("${text}")`);
  }
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

const CONTROLS_NEEDING_TEXT_BYTES = new Set([
  'button', 'label', 'editbox', 'checkbox', 'radiobutton', 'groupbox',
]);

const CONTROLS_NEEDING_FONT_BYTES = new Set([
  'label', 'editbox', 'checkbox', 'radiobutton', 'groupbox', 'combobox', 'datetimepicker',
]);

export function generateEpl(win: DesignWindow, controls: DesignControl[]): string {
  const lines: string[] = [];
  const eventHandlers = collectEventHandlers(controls);

  lines.push(`.版本 2`);
  lines.push(``);
  lines.push(`.程序集 程序集1`);
  lines.push(``);
  lines.push(`.程序集变量 主窗口句柄, 整数型`);

  for (const c of controls) {
    lines.push(`.程序集变量 ${c.name}, 整数型`);
  }

  lines.push(``);
  lines.push(`.子程序 _启动子程序, 整数型`);
  lines.push(`.局部变量 标题字节集, 字节集`);

  for (const c of controls) {
    if (CONTROLS_NEEDING_TEXT_BYTES.has(c.type)) {
      lines.push(`.局部变量 文本_${c.name}, 字节集`);
    }
    if (CONTROLS_NEEDING_FONT_BYTES.has(c.type)) {
      lines.push(`.局部变量 字体_${c.name}, 字节集`);
    }
    if (c.type === 'datagridview') {
      const cols = ((c.props.columns as string) || '').split('\n').map((s) => s.trim()).filter(Boolean);
      for (let i = 0; i < cols.length; i++) {
        lines.push(`.局部变量 列_${c.name}_${i}, 字节集`);
      }
    }
    if (c.type === 'treeview' || c.type === 'treeview_sidebar') {
      const nodesStr = (c.props.nodes as string) || '';
      const nodeLines = nodesStr.split('\n').map((s) => s.trimEnd()).filter((s) => s.length > 0);
      for (let i = 0; i < nodeLines.length; i++) {
        const line = nodeLines[i];
        const trimmed = line.trimStart();
        const { emoji } = splitLeadingEmoji(trimmed || `节点${i + 1}`);
        lines.push(`.局部变量 节点文本_${c.name}_${i}, 字节集`);
        if (emoji) lines.push(`.局部变量 节点图标_${c.name}_${i}, 字节集`);
        lines.push(`.局部变量 节点ID_${c.name}_${i}, 整数型`);
      }
    }
    if (c.type === 'tabcontrol') {
      lines.push(`.局部变量 tab_${c.name}_tmp, 字节集`);
      const tabCount = ((c.props.tabs as string) || '页面1\n页面2\n页面3').split('\n').filter((t) => t.trim()).length;
      for (let i = 0; i < tabCount; i++) {
        lines.push(`.局部变量 ${c.name}_page_${i}, 整数型`);
      }
    }
  }

  lines.push(``);
  lines.push(`' ===== 创建窗口 =====`);
  const titleFull = `${win.emoji} ${win.title}`.trim();
  const titleBytes = textToUtf8Bytes(titleFull);
  lines.push(`标题字节集 ＝ ${utf8BytesToEplFormat(titleBytes)}`);
  const winBg = (win.bgColor ?? (win as { bg_color?: string }).bg_color ?? '#FFFFFF') as string;
  const clientBgEpl = eplColor(winBg);
  const winX = win.x ?? -1;
  const winY = win.y ?? -1;
  lines.push(`主窗口句柄 ＝ 创建Emoji窗口_字节集_扩展 (取变量数据地址 (标题字节集), 取字节集长度 (标题字节集), ${winX}, ${winY}, ${win.width}, ${win.height}, ${eplColor((win.titlebarColor as string) || '#409EFF')}, ${clientBgEpl})`);
  lines.push(``);

  lines.push(`' ===== 创建控件 =====`);

  const getParentExpr = (c: DesignControl): string => {
    if ((c.parentKind === 'tabcontrol' || c.parentKind === 'tabpage') && c.parentId) {
      const idx = c.parentTabIndex ?? 0;
      return `${c.parentId}_page_${idx}`;
    }
    return '主窗口句柄';
  };

  // 创建 Tab 子控件前需先切换到对应 Tab，否则 TreeView 等控件不显示
  const getTabSwitch = (c: DesignControl): { tabId: string; index: number } | null => {
    if ((c.parentKind === 'tabcontrol' || c.parentKind === 'tabpage') && c.parentId) {
      const idx = c.parentTabIndex ?? 0;
      return { tabId: c.parentId, index: idx };
    }
    return null;
  };

  let lastTabSwitch: { tabId: string; index: number } | null = null;

  for (const c of controls) {
    const p = c.props;
    const parentExpr = getParentExpr(c);
    lines.push(``);
    lines.push(`' ${c.name}`);

    // Tab 子控件必须在父 Tab 可见时创建，先切换到对应 Tab
    if (c.type !== 'tabcontrol') {
      const tabSwitch = getTabSwitch(c);
      if (tabSwitch) {
        if (!lastTabSwitch || lastTabSwitch.tabId !== tabSwitch.tabId || lastTabSwitch.index !== tabSwitch.index) {
          lines.push(`切换到Tab (${tabSwitch.tabId}, ${tabSwitch.index})`);
          lastTabSwitch = tabSwitch;
        }
      } else {
        lastTabSwitch = null;
      }
    }

    const fontName = (p.fontName as string) || 'Microsoft YaHei UI';
    const fontSize = (p.fontSize as number) || 13;
    const fgColor = eplColor((p.fgColor as string) || '#303133');
    const bgColor = eplColor((p.bgColor as string) || '#FFFFFF');

    const emitTextBytes = (text: string) => {
      emitUtf8Text(`文本_${c.name}`, text, lines);
    };
    const emitFontBytes = () => {
      emitUtf8Text(`字体_${c.name}`, fontName, lines);
    };
    const textPtr = `取变量数据地址 (文本_${c.name})`;
    const textLen = `取字节集长度 (文本_${c.name})`;
    const fontPtr = `取变量数据地址 (字体_${c.name})`;
    const fontLen = `取字节集长度 (字体_${c.name})`;

    switch (c.type) {
      case 'button': {
        let emoji = (p.emoji as string) || '';
        let text = (p.text as string) || '按钮';
        if (!emoji) {
          const split = splitLeadingEmoji(text);
          emoji = split.emoji;
          text = split.rest || text;
        }
        emitUtf8Text(`文本_${c.name}`, text, lines);
        if (emoji) {
          const emojiBytes = textToUtf8Bytes(emoji);
          lines.push(`.局部变量 emoji_${c.name}, 字节集`);
          lines.push(`emoji_${c.name} ＝ ${utf8BytesToEplFormat(emojiBytes)}`);
          lines.push(`${c.name} ＝ 创建Emoji按钮_字节集 (${parentExpr}, 取变量数据地址 (emoji_${c.name}), 取字节集长度 (emoji_${c.name}), ${textPtr}, ${textLen}, ${c.x}, ${c.y}, ${c.width}, ${c.height}, ${eplColor((p.bgColor as string) || '#409EFF')})`);
        } else {
          lines.push(`${c.name} ＝ 创建Emoji按钮_字节集 (${parentExpr}, 0, 0, ${textPtr}, ${textLen}, ${c.x}, ${c.y}, ${c.width}, ${c.height}, ${eplColor((p.bgColor as string) || '#409EFF')})`);
        }
        break;
      }
      case 'label': {
        const text = (p.text as string) || '标签';
        emitTextBytes(text);
        emitFontBytes();
        lines.push(`${c.name} ＝ 创建标签 (${parentExpr}, ${c.x}, ${c.y}, ${c.width}, ${c.height}, ${textPtr}, ${textLen}, ${fgColor}, ${bgColor}, ${fontPtr}, ${fontLen}, ${fontSize}, ${eplBool(p.bold)}, ${eplBool(p.italic)}, ${eplBool(p.underline)}, ${(p.alignment as number) || 0}, ${eplBool(p.wordWrap)})`);
        break;
      }
      case 'editbox': {
        const text = (p.text as string) || '';
        const fnName = p.emojiSupport ? '创建彩色Emoji编辑框' : '创建编辑框';
        emitTextBytes(text);
        emitFontBytes();
        lines.push(`${c.name} ＝ ${fnName} (${parentExpr}, ${c.x}, ${c.y}, ${c.width}, ${c.height}, ${textPtr}, ${textLen}, ${fgColor}, ${bgColor}, ${fontPtr}, ${fontLen}, ${fontSize}, ${eplBool(p.bold)}, ${eplBool(p.italic)}, ${eplBool(p.underline)}, ${(p.alignment as number) || 0}, ${eplBool(p.multiline)}, ${eplBool(p.readOnly)}, ${eplBool(p.password)}, ${eplBool(p.showBorder !== false)}, ${eplBool(p.vertCenter !== false)})`);
        break;
      }
      case 'checkbox': {
        const text = (p.text as string) || '复选框';
        emitTextBytes(text);
        emitFontBytes();
        lines.push(`${c.name} ＝ 创建复选框 (${parentExpr}, ${c.x}, ${c.y}, ${c.width}, ${c.height}, ${textPtr}, ${textLen}, ${eplBool(p.checked)}, ${fgColor}, ${bgColor}, ${fontPtr}, ${fontLen}, ${fontSize}, ${eplBool(p.bold)}, ${eplBool(p.italic)}, ${eplBool(p.underline)})`);
        break;
      }
      case 'radiobutton': {
        const text = (p.text as string) || '单选按钮';
        emitTextBytes(text);
        emitFontBytes();
        lines.push(`${c.name} ＝ 创建单选按钮 (${parentExpr}, ${c.x}, ${c.y}, ${c.width}, ${c.height}, ${textPtr}, ${textLen}, ${(p.groupId as number) || 1}, ${eplBool(p.checked)}, ${fgColor}, ${bgColor}, ${fontPtr}, ${fontLen}, ${fontSize}, ${eplBool(p.bold)}, ${eplBool(p.italic)}, ${eplBool(p.underline)})`);
        break;
      }
      case 'progressbar':
        lines.push(`${c.name} ＝ 创建进度条 (${parentExpr}, ${c.x}, ${c.y}, ${c.width}, ${c.height}, ${(p.value as number) || 50}, ${eplColor((p.fgColor as string) || '#409EFF')}, ${eplColor((p.bgColor as string) || '#EBEEF5')}, ${p.showText !== false ? '真' : '假'}, ${eplColor((p.textColor as string) || '#303133')})`);
        break;
      case 'groupbox': {
        const text = (p.text as string) || '分组框';
        emitTextBytes(text);
        emitFontBytes();
        lines.push(`${c.name} ＝ 创建分组框 (${parentExpr}, ${c.x}, ${c.y}, ${c.width}, ${c.height}, ${textPtr}, ${textLen}, ${eplColor((p.borderColor as string) || '#DCDFE6')}, ${bgColor}, ${fontPtr}, ${fontLen}, ${fontSize}, ${eplBool(p.bold)}, ${eplBool(p.italic)}, ${eplBool(p.underline)})`);
        break;
      }
      case 'picturebox':
        lines.push(`${c.name} ＝ 创建图片框 (${parentExpr}, ${c.x}, ${c.y}, ${c.width}, ${c.height}, ${(p.scaleMode as number) || 2}, ${eplColor((p.bgColor as string) || '#F5F7FA')})`);
        break;
      case 'combobox': {
        emitFontBytes();
        lines.push(`${c.name} ＝ 创建组合框 (${parentExpr}, ${c.x}, ${c.y}, ${c.width}, ${c.height}, ${eplBool(p.readOnly)}, ${fgColor}, ${bgColor}, ${(p.itemHeight as number) || 35}, ${fontPtr}, ${fontLen}, ${fontSize}, ${eplBool(p.bold)}, ${eplBool(p.italic)}, ${eplBool(p.underline)})`);
        break;
      }
      case 'listbox':
        lines.push(`${c.name} ＝ 创建列表框 (${parentExpr}, ${c.x}, ${c.y}, ${c.width}, ${c.height}, ${eplBool(p.multiSelect)}, ${fgColor}, ${bgColor})`);
        break;
      case 'tabcontrol': {
        lines.push(`${c.name} ＝ 创建TabControl (主窗口句柄, ${c.x}, ${c.y}, ${c.width}, ${c.height})`);
        const tabNames = ((p.tabs as string) || '页面1\n页面2\n页面3')
          .split('\n')
          .map((t) => t.trim())
          .filter(Boolean);
        for (let index = 0; index < tabNames.length; index++) {
          const varName = `tab_${c.name}_tmp`;
          emitUtf8Text(varName, tabNames[index], lines);
          lines.push(`添加Tab页 (${c.name}, 取变量数据地址 (${varName}), 取字节集长度 (${varName}), 0)`);
          lines.push(`${c.name}_page_${index} ＝ 获取Tab内容窗口 (${c.name}, ${index})`);
        }
        // 新增属性：仅在与默认值不同时生成调用代码
        {
          const selBg = (p.selectedBgColor as string) || '#FFFFFF';
          const unselBg = (p.unselectedBgColor as string) || '#F5F7FA';
          const selText = (p.selectedTextColor as string) || '#409EFF';
          const unselText = (p.unselectedTextColor as string) || '#606266';
          if (selBg !== '#FFFFFF' || unselBg !== '#F5F7FA' || selText !== '#409EFF' || unselText !== '#606266') {
            lines.push(`SetTabColors (${c.name}, ${eplColor(selBg)}, ${eplColor(unselBg)}, ${eplColor(selText)}, ${eplColor(unselText)})`);
          }
          const indColor = (p.indicatorColor as string) || '#409EFF';
          if (indColor !== '#409EFF') {
            lines.push(`SetTabIndicatorColor (${c.name}, ${eplColor(indColor)})`);
          }
          const tabW = (p.tabItemWidth as number) ?? 120;
          const tabH = (p.tabItemHeight as number) ?? 34;
          if (tabW !== 120 || tabH !== 34) {
            lines.push(`SetTabItemSize (${c.name}, ${tabW}, ${tabH})`);
          }
          const padH = (p.paddingH as number) ?? 2;
          const padV = (p.paddingV as number) ?? 0;
          if (padH !== 2 || padV !== 0) {
            lines.push(`SetTabPadding (${c.name}, ${padH}, ${padV})`);
          }
          if (p.closable) {
            lines.push(`SetTabClosable (${c.name}, 1)`);
          }
          if (p.draggable) {
            lines.push(`SetTabDraggable (${c.name}, 1)`);
          }
          if (p.scrollable) {
            lines.push(`SetTabScrollable (${c.name}, 1)`);
          }
          const tabPos = (p.tabPosition as number) ?? 0;
          if (tabPos !== 0) {
            lines.push(`SetTabPosition (${c.name}, ${tabPos})`);
          }
          const tabAlign = (p.tabAlignment as number) ?? 0;
          if (tabAlign !== 0) {
            lines.push(`SetTabAlignment (${c.name}, ${tabAlign})`);
          }
        }
        // 延后到所有控件创建后再调用，确保 Tab 子控件在父窗口可见时创建
        lines.push(`' 更新TabControl布局 将在所有控件创建后调用`);
        break;
      }
      case 'datagridview': {
        lines.push(`${c.name} ＝ 创建DataGridView (${parentExpr}, ${c.x}, ${c.y}, ${c.width}, ${c.height}, ${eplBool(p.virtualMode)}, ${eplBool(p.zebraStripe)}, ${fgColor}, ${bgColor})`);
        const columnsStr = (p.columns as string) || '';
        const columns = columnsStr.split('\n').map((s) => s.trim()).filter(Boolean);
        const colWidth = columns.length > 0 ? Math.max(80, Math.floor((c.width - 20) / columns.length)) : 100;
        for (let i = 0; i < columns.length; i++) {
          const colName = columns[i];
          if (!colName) continue;
          const varName = `列_${c.name}_${i}`;
          emitUtf8Text(varName, colName, lines);
          lines.push(`表格_添加文本列 (${c.name}, 取变量数据地址 (${varName}), 取字节集长度 (${varName}), ${colWidth})`);
        }
        break;
      }
      case 'treeview':
      case 'treeview_sidebar': {
        lines.push(`${c.name} ＝ 创建树形框 (${parentExpr}, ${c.x}, ${c.y}, ${c.width}, ${c.height}, ${bgColor}, ${fgColor}, 0)`);
        const nodesStr = (p.nodes as string) || '';
        const nodeLines = nodesStr.split('\n').map((s) => s.trimEnd()).filter((s) => s.length > 0);
        if (nodeLines.length > 0) {
          lines.push(`ClearTree (${c.name})`);
          const stack: { level: number; idVar: string }[] = [];
          for (let i = 0; i < nodeLines.length; i++) {
            const line = nodeLines[i];
            const trimmed = line.trimStart();
            const level = (line.length - trimmed.length) / 2;  // 2 spaces per level
            const fullText = trimmed || `节点${i + 1}`;
            const { emoji, rest } = splitLeadingEmoji(fullText);
            const text = rest || fullText;  // 无 emoji 时用全文
            const textVar = `节点文本_${c.name}_${i}`;
            const iconVar = `节点图标_${c.name}_${i}`;
            const idVar = `节点ID_${c.name}_${i}`;
            emitUtf8Text(textVar, text, lines);
            if (emoji) {
              const emojiBytes = textToUtf8Bytes(emoji);
              lines.push(`${iconVar} ＝ ${utf8BytesToEplFormat(emojiBytes)}`);
            }
            const iconArgs = emoji
              ? `取变量数据地址 (${iconVar}), 取字节集长度 (${iconVar})`
              : '0, 0';
            if (level === 0) {
              lines.push(`${idVar} ＝ AddRootNode (${c.name}, 取变量数据地址 (${textVar}), 取字节集长度 (${textVar}), ${iconArgs})`);
              stack.length = 0;
              stack.push({ level: 0, idVar });
            } else {
              while (stack.length > 0 && stack[stack.length - 1].level >= level) stack.pop();
              if (stack.length === 0) {
                lines.push(`${idVar} ＝ AddRootNode (${c.name}, 取变量数据地址 (${textVar}), 取字节集长度 (${textVar}), ${iconArgs})`);
                stack.push({ level: 0, idVar });
              } else {
                const parentVar = stack[stack.length - 1].idVar;
                lines.push(`${idVar} ＝ AddChildNode (${c.name}, ${parentVar}, 取变量数据地址 (${textVar}), 取字节集长度 (${textVar}), ${iconArgs})`);
                stack.push({ level, idVar });
              }
            }
          }
        }
        if (c.type === 'treeview_sidebar') {
          const rowH = (p.rowHeight as number) ?? 38;
          const rowSp = (p.itemSpacing as number) ?? 2;
          const selBg = eplColor((p.selectedBgColor as string) || '#335EEA');
          const selFg = eplColor((p.selectedForeColor as string) || '#FFFFFF');
          const hoverBg = eplColor((p.hoverBgColor as string) || '#F5F7FA');
          lines.push(`设置树形框行高 (${c.name}, ${rowH})`);
          lines.push(`设置树形框行间距 (${c.name}, ${rowSp})`);
          lines.push(`设置树形框文字色 (${c.name}, ${fgColor})`);
          lines.push(`设置树形框选中背景色 (${c.name}, ${selBg})`);
          lines.push(`设置树形框选中前景色 (${c.name}, ${selFg})`);
          lines.push(`设置树形框悬停背景色 (${c.name}, ${hoverBg})`);
          lines.push(`设置树形框侧栏模式 (${c.name}, 真)`);
        }
        break;
      }
      case 'datetimepicker': {
        emitFontBytes();
        const borderColor = eplColor((p.borderColor as string) || '#DCDFE6');
        const prec = (p.precision as number) ?? 4;
        lines.push(`${c.name} ＝ 创建D2D日期时间选择器 (${parentExpr}, ${c.x}, ${c.y}, ${c.width}, ${c.height}, ${prec}, ${fgColor}, ${bgColor}, ${borderColor}, ${fontPtr}, ${fontLen}, ${fontSize}, ${eplBool(p.bold)}, ${eplBool(p.italic)}, ${eplBool(p.underline)})`);
        lines.push(`设置D2D日期时间选择器日期时间 (${c.name}, ${(p.year as number) ?? 2024}, ${(p.month as number) ?? 6}, ${(p.day as number) ?? 15}, ${(p.hour as number) ?? 0}, ${(p.minute as number) ?? 0}, ${(p.second as number) ?? 0})`);
        if (p.enabled === false) {
          lines.push(`启用D2D日期时间选择器 (${c.name}, 假)`);
        }
        if (p.visible === false) {
          lines.push(`显示D2D日期时间选择器 (${c.name}, 假)`);
        }
        const vh = normalizeControlIdentifier((p.onValueChanged as string) || '');
        if (vh) {
          lines.push(`设置D2D日期时间选择器回调 (${c.name}, &${vh})`);
        }
        break;
      }
    }
  }

  // 所有 Tab 子控件创建完成后，切换回第一页并更新布局
  const tabControls = controls.filter((c) => c.type === 'tabcontrol');
  if (tabControls.length > 0) {
    lines.push(``);
    lines.push(`' ===== 更新 TabControl 布局 =====`);
    for (const tc of tabControls) {
      lines.push(`切换到Tab (${tc.name}, 0)`);
      lines.push(`更新TabControl布局 (${tc.name})`);
    }
  }

  const hasBtns = controls.some(c => c.type === 'button');
  if (hasBtns) {
    lines.push(``);
    lines.push(`' ===== 设置回调 =====`);
    lines.push(`设置按钮点击回调 (&按钮点击回调)`);
  }

  lines.push(``);
  lines.push(`' ===== 运行消息循环 =====`);
  lines.push(`设置消息循环主窗口 (主窗口句柄)`);
  lines.push(`运行消息循环 ()`);
  lines.push(`返回 (0)`);
  lines.push(``);

  if (hasBtns) {
    lines.push(`.子程序 按钮点击回调, , , 按钮点击事件`);
    lines.push(`.参数 按钮ID, 整数型`);
    lines.push(`.参数 父窗口句柄, 整数型`);
    lines.push(``);
    const btns = controls.filter(c => c.type === 'button');
    if (btns.length === 1) {
      lines.push(`.如果真 (按钮ID ＝ ${btns[0].name})`);
      const handlerName = normalizeControlIdentifier((btns[0].props.onClick as string) || '');
      lines.push(`    ${handlerName ? `${handlerName} ()` : `' TODO: ${btns[0].name} 点击处理`}`);
      lines.push(`.如果真结束`);
    } else {
      lines.push(`.判断开始 (按钮ID ＝ ${btns[0].name})`);
      const firstHandler = normalizeControlIdentifier((btns[0].props.onClick as string) || '');
      lines.push(`    ${firstHandler ? `${firstHandler} ()` : `' TODO: ${btns[0].name} 点击处理`}`);
      for (let i = 1; i < btns.length; i++) {
        lines.push(`.判断 (按钮ID ＝ ${btns[i].name})`);
        const handlerName = normalizeControlIdentifier((btns[i].props.onClick as string) || '');
        lines.push(`    ${handlerName ? `${handlerName} ()` : `' TODO: ${btns[i].name} 点击处理`}`);
      }
      lines.push(`.判断结束`);
    }
  }

  if (eventHandlers.length > 0) {
    lines.push(``);
    lines.push(`' ===== AI 事件占位 =====`);
    for (const handler of eventHandlers) {
      lines.push(`.子程序 ${handler.handler}`);
      lines.push(`    ' TODO: ${handler.controlName}.${handler.eventName}`);
      lines.push(``);
    }
  }

  return lines.join('\n');
}
