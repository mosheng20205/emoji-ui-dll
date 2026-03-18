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
    if (['button', 'label', 'editbox', 'checkbox', 'radiobutton', 'groupbox'].includes(c.type)) {
      lines.push(`.局部变量 文本_${c.name}, 字节集`);
    }
  }

  lines.push(``);
  lines.push(`' ===== 创建窗口 =====`);
  const titleFull = `${win.emoji} ${win.title}`.trim();
  const titleBytes = textToUtf8Bytes(titleFull);
  lines.push(`标题字节集 ＝ ${utf8BytesToEplFormat(titleBytes)}`);
  lines.push(`主窗口句柄 ＝ 创建Emoji窗口_字节集_扩展 (取变量数据地址 (标题字节集), 取字节集长度 (标题字节集), ${win.width}, ${win.height}, ${eplColor(win.titlebarColor)})`);
  lines.push(``);

  lines.push(`' ===== 创建控件 =====`);

  for (const c of controls) {
    const p = c.props;
    lines.push(``);
    lines.push(`' ${c.name}`);

    switch (c.type) {
      case 'button': {
        const emoji = (p.emoji as string) || '';
        const text = (p.text as string) || '按钮';
        if (emoji) {
          const emojiBytes = textToUtf8Bytes(emoji);
          lines.push(`.局部变量 emoji_${c.name}, 字节集`);
          lines.push(`emoji_${c.name} ＝ ${utf8BytesToEplFormat(emojiBytes)}`);
        }
        lines.push(`文本_${c.name} ＝ 编码_Ansi到Utf8 ("${text}")`);
        if (emoji) {
          lines.push(`${c.name} ＝ 创建Emoji按钮_字节集 (主窗口句柄, 取变量数据地址 (emoji_${c.name}), 取字节集长度 (emoji_${c.name}), 取变量数据地址 (文本_${c.name}), 取字节集长度 (文本_${c.name}), ${c.x}, ${c.y}, ${c.width}, ${c.height}, ${eplColor((p.bgColor as string) || '#409EFF')})`);
        } else {
          lines.push(`${c.name} ＝ 创建Emoji按钮_辅助 (主窗口句柄, "", "${text}", ${c.x}, ${c.y}, ${c.width}, ${c.height}, ${eplColor((p.bgColor as string) || '#409EFF')})`);
        }
        break;
      }
      case 'label': {
        const text = (p.text as string) || '标签';
        lines.push(`${c.name} ＝ 创建标签_辅助 (主窗口句柄, ${c.x}, ${c.y}, ${c.width}, ${c.height}, "${text}", ${eplColor((p.fgColor as string) || '#303133')}, ${eplColor((p.bgColor as string) || '#FFFFFF')}, "${(p.fontName as string) || 'Microsoft YaHei UI'}", ${(p.fontSize as number) || 13}, ${eplBool(p.bold)}, ${eplBool(p.italic)}, ${eplBool(p.underline)}, ${(p.alignment as number) || 0}, ${eplBool(p.wordWrap)})`);
        break;
      }
      case 'editbox': {
        const text = (p.text as string) || '';
        const fnName = p.emojiSupport ? '创建彩色Emoji编辑框_辅助' : '创建编辑框_辅助';
        lines.push(`${c.name} ＝ ${fnName} (主窗口句柄, ${c.x}, ${c.y}, ${c.width}, ${c.height}, "${text}", ${eplColor((p.fgColor as string) || '#303133')}, ${eplColor((p.bgColor as string) || '#FFFFFF')}, "${(p.fontName as string) || 'Microsoft YaHei UI'}", ${(p.fontSize as number) || 13}, ${eplBool(p.bold)}, ${eplBool(p.italic)}, ${eplBool(p.underline)}, ${(p.alignment as number) || 0}, ${eplBool(p.multiline)}, ${eplBool(p.readOnly)}, ${eplBool(p.password)}, ${eplBool(p.showBorder !== false)}, ${eplBool(p.vertCenter !== false)})`);
        break;
      }
      case 'checkbox': {
        const text = (p.text as string) || '复选框';
        lines.push(`${c.name} ＝ 创建复选框_辅助 (主窗口句柄, ${c.x}, ${c.y}, ${c.width}, ${c.height}, "${text}", ${eplColor((p.fgColor as string) || '#303133')}, ${eplColor((p.bgColor as string) || '#FFFFFF')}, ${eplColor((p.checkColor as string) || '#409EFF')}, "${(p.fontName as string) || 'Microsoft YaHei UI'}", ${(p.fontSize as number) || 13}, ${eplBool(p.bold)}, ${eplBool(p.italic)}, ${eplBool(p.underline)}, ${eplBool(p.checked)})`);
        break;
      }
      case 'radiobutton': {
        const text = (p.text as string) || '单选按钮';
        lines.push(`${c.name} ＝ 创建单选按钮_辅助 (主窗口句柄, ${c.x}, ${c.y}, ${c.width}, ${c.height}, "${text}", ${eplColor((p.fgColor as string) || '#303133')}, ${eplColor((p.bgColor as string) || '#FFFFFF')}, ${eplColor((p.radioColor as string) || '#409EFF')}, "${(p.fontName as string) || 'Microsoft YaHei UI'}", ${(p.fontSize as number) || 13}, ${eplBool(p.bold)}, ${eplBool(p.italic)}, ${eplBool(p.underline)}, ${(p.groupId as number) || 1}, ${eplBool(p.checked)})`);
        break;
      }
      case 'progressbar':
        lines.push(`${c.name} ＝ 创建进度条 (主窗口句柄, ${c.x}, ${c.y}, ${c.width}, ${c.height}, ${(p.value as number) || 50}, ${eplColor((p.fgColor as string) || '#409EFF')}, ${eplColor((p.bgColor as string) || '#EBEEF5')}, ${p.showText !== false ? 1 : 0})`);
        break;
      case 'groupbox': {
        const text = (p.text as string) || '分组框';
        lines.push(`${c.name} ＝ 创建分组框_辅助 (主窗口句柄, ${c.x}, ${c.y}, ${c.width}, ${c.height}, "${text}", ${eplColor((p.fgColor as string) || '#303133')}, ${eplColor((p.bgColor as string) || '#FFFFFF')}, ${eplColor((p.borderColor as string) || '#DCDFE6')}, "${(p.fontName as string) || 'Microsoft YaHei UI'}", ${(p.fontSize as number) || 13}, ${eplBool(p.bold)}, ${eplBool(p.italic)}, ${eplBool(p.underline)})`);
        break;
      }
      case 'picturebox':
        lines.push(`${c.name} ＝ 创建图片框 (主窗口句柄, ${c.x}, ${c.y}, ${c.width}, ${c.height}, ${eplColor((p.bgColor as string) || '#F5F7FA')}, ${eplColor((p.borderColor as string) || '#DCDFE6')}, ${(p.scaleMode as number) || 2})`);
        break;
      case 'combobox':
        lines.push(`${c.name} ＝ 创建组合框_辅助 (主窗口句柄, ${c.x}, ${c.y}, ${c.width}, ${c.height}, ${eplColor((p.fgColor as string) || '#303133')}, ${eplColor((p.bgColor as string) || '#FFFFFF')}, "${(p.fontName as string) || 'Microsoft YaHei UI'}", ${(p.fontSize as number) || 13}, ${eplBool(p.bold)}, ${eplBool(p.italic)}, ${eplBool(p.underline)})`);
        break;
      case 'listbox':
        lines.push(`${c.name} ＝ 创建列表框_辅助 (主窗口句柄, ${c.x}, ${c.y}, ${c.width}, ${c.height}, ${eplColor((p.fgColor as string) || '#303133')}, ${eplColor((p.bgColor as string) || '#FFFFFF')}, "${(p.fontName as string) || 'Microsoft YaHei UI'}", ${(p.fontSize as number) || 13}, ${eplBool(p.bold)}, ${eplBool(p.italic)}, ${eplBool(p.underline)}, ${eplBool(p.multiSelect)})`);
        break;
      case 'tabcontrol':
        lines.push(`${c.name} ＝ 创建选项卡_辅助 (主窗口句柄, ${c.x}, ${c.y}, ${c.width}, ${c.height}, ${eplColor((p.fgColor as string) || '#303133')}, ${eplColor((p.bgColor as string) || '#FFFFFF')}, "${(p.fontName as string) || 'Microsoft YaHei UI'}", ${(p.fontSize as number) || 13}, ${eplBool(p.bold)}, ${eplBool(p.italic)}, ${eplBool(p.underline)})`);
        break;
      case 'datagridview':
        lines.push(`${c.name} ＝ 创建表格 (主窗口句柄, ${c.x}, ${c.y}, ${c.width}, ${c.height}, ${eplColor((p.fgColor as string) || '#303133')}, ${eplColor((p.bgColor as string) || '#FFFFFF')}, ${eplColor((p.headerColor as string) || '#409EFF')}, "${(p.fontName as string) || 'Microsoft YaHei UI'}", ${(p.fontSize as number) || 13})`);
        break;
      case 'treeview':
        lines.push(`${c.name} ＝ 创建树形框_辅助 (主窗口句柄, ${c.x}, ${c.y}, ${c.width}, ${c.height}, ${eplColor((p.fgColor as string) || '#303133')}, ${eplColor((p.bgColor as string) || '#FFFFFF')}, "${(p.fontName as string) || 'Microsoft YaHei UI'}", ${(p.fontSize as number) || 13}, ${eplBool(p.bold)}, ${eplBool(p.italic)}, ${eplBool(p.underline)}, ${eplBool(p.showCheckBoxes)})`);
        break;
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
