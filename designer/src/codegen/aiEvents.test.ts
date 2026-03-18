import { generatePython } from './python';
import { generateCSharp } from './csharp';
import { generateEpl } from './epl';
import type { DesignControl, DesignWindow } from '../types/controls';

const WINDOW: DesignWindow = {
  title: '事件测试',
  emoji: '🤖',
  width: 640,
  height: 420,
  titlebarColor: '#409EFF',
  bgColor: '#FFFFFF',
};

const CONTROLS: DesignControl[] = [
  {
    id: 'button_1',
    type: 'button',
    name: 'login_button',
    x: 20,
    y: 20,
    width: 120,
    height: 40,
    parentId: null,
    parentKind: 'window',
    parentTabIndex: null,
    props: {
      text: '登录',
      bgColor: '#409EFF',
      fgColor: '#FFFFFF',
      onClick: 'handle_login',
    },
  },
  {
    id: 'checkbox_1',
    type: 'checkbox',
    name: 'remember_me',
    x: 20,
    y: 80,
    width: 120,
    height: 25,
    parentId: null,
    parentKind: 'window',
    parentTabIndex: null,
    props: {
      text: '记住我',
      onChange: 'handle_remember_change',
    },
  },
];

describe('AI 事件占位代码生成', () => {
  test('Python 代码应包含 AI 生成的事件占位函数', () => {
    const code = generatePython(WINDOW, CONTROLS);

    expect(code).toContain('def handle_login():');
    expect(code).toContain('def handle_remember_change():');
  });

  test('C# 代码应包含 AI 生成的事件占位函数', () => {
    const code = generateCSharp(WINDOW, CONTROLS);

    expect(code).toContain('static void handle_login()');
    expect(code).toContain('static void handle_remember_change()');
  });

  test('易语言代码应包含 AI 生成的事件占位子程序', () => {
    const code = generateEpl(WINDOW, CONTROLS);

    expect(code).toContain('.子程序 handle_login');
    expect(code).toContain('.子程序 handle_remember_change');
  });
});
