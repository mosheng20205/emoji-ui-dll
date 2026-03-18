import { generateCode } from './index';
import type { DesignControl, DesignWindow } from '../types/controls';

const win: DesignWindow = {
  title: '测试窗口',
  emoji: '🎨',
  width: 800,
  height: 600,
  titlebarColor: '#409EFF',
  bgColor: '#FFFFFF',
};

describe('容器控件代码生成', () => {
  test('易语言代码中的控件句柄应生成到程序集变量而不是局部变量', () => {
    const controls = [
      {
        id: 'button_1',
        type: 'button',
        name: 'button_1',
        x: 10,
        y: 10,
        width: 120,
        height: 40,
        props: {
          text: '按钮一',
          emoji: '',
          bgColor: '#409EFF',
          fgColor: '#FFFFFF',
          fontName: 'Microsoft YaHei UI',
          fontSize: 13,
          bold: false,
          italic: false,
          underline: false,
          onClick: '',
        },
      },
      {
        id: 'button_2',
        type: 'button',
        name: 'button_2',
        x: 140,
        y: 10,
        width: 120,
        height: 40,
        props: {
          text: '按钮二',
          emoji: '',
          bgColor: '#67C23A',
          fgColor: '#FFFFFF',
          fontName: 'Microsoft YaHei UI',
          fontSize: 13,
          bold: false,
          italic: false,
          underline: false,
          onClick: '',
        },
      },
    ] as unknown as DesignControl[];

    const eplCode = generateCode('epl', win, controls);
    expect(eplCode).toContain('.程序集变量 button_1, 整数型');
    expect(eplCode).toContain('.程序集变量 button_2, 整数型');
    expect(eplCode).not.toContain('.局部变量 button_1, 整数型');
    expect(eplCode).not.toContain('.局部变量 button_2, 整数型');
    expect(eplCode).toContain('.局部变量 文本_button_1, 字节集');
  });

  test('TabControl 内嵌控件应生成页容器代码', () => {
    const controls = [
      {
        id: 'tab_1',
        type: 'tabcontrol',
        name: 'tab_1',
        x: 10,
        y: 10,
        width: 300,
        height: 200,
        props: {
          tabs: '页面1\n页面2\n页面3',
          fgColor: '#303133',
          bgColor: '#FFFFFF',
          fontName: 'Microsoft YaHei UI',
          fontSize: 13,
          bold: false,
          italic: false,
          underline: false,
        },
      },
      {
        id: 'button_1',
        type: 'button',
        name: 'button_1',
        x: 20,
        y: 20,
        width: 120,
        height: 36,
        parentId: 'tab_1',
        parentKind: 'tabpage',
        parentTabIndex: 1,
        props: {
          text: '页内按钮',
          emoji: '',
          bgColor: '#409EFF',
          fgColor: '#FFFFFF',
          fontName: 'Microsoft YaHei UI',
          fontSize: 13,
          bold: false,
          italic: false,
          underline: false,
          onClick: '',
        },
      },
    ] as unknown as DesignControl[];

    const pythonCode = generateCode('python', win, controls);
    expect(pythonCode).toContain('GetTabContentWindow');
    expect(pythonCode).toMatch(/button_1 = .*tab_1_page_1/);
  });

  test('GroupBox 内嵌控件应生成 AddChildToGroup 挂载代码', () => {
    const controls = [
      {
        id: 'group_1',
        type: 'groupbox',
        name: 'group_1',
        x: 20,
        y: 20,
        width: 240,
        height: 180,
        props: {
          text: '分组一',
          fgColor: '#303133',
          bgColor: '#FFFFFF',
          borderColor: '#DCDFE6',
          fontName: 'Microsoft YaHei UI',
          fontSize: 13,
          bold: false,
          italic: false,
          underline: false,
        },
      },
      {
        id: 'label_1',
        type: 'label',
        name: 'label_1',
        x: 16,
        y: 28,
        width: 100,
        height: 24,
        parentId: 'group_1',
        parentKind: 'groupbox',
        props: {
          text: '组内标签',
          fgColor: '#303133',
          bgColor: '#ffeeaa',
          fontName: 'Microsoft YaHei UI',
          fontSize: 13,
          bold: false,
          italic: false,
          underline: false,
          alignment: 0,
          wordWrap: false,
        },
      },
    ] as unknown as DesignControl[];

    const csharpCode = generateCode('csharp', win, controls);
    expect(csharpCode).toMatch(/label_1 = CreateLabel\(hwnd,/);
    expect(csharpCode).toContain('AddChildToGroup(group_1, label_1);');
  });
});
