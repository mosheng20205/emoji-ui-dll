import { applyDraft } from './applyDraft';
import { applyOps } from './applyOps';
import { validateDraft } from './validateDraft';
import { validateOps } from './validateOps';
import type { DesignControl, DesignWindow } from '../types/controls';

const BASE_WINDOW: DesignWindow = {
  title: '测试窗口',
  emoji: '🎨',
  width: 800,
  height: 600,
  titlebarColor: '#409EFF',
  bgColor: '#F5F7FA',
};

const BASE_CONTROLS: DesignControl[] = [
  {
    id: 'button_1',
    type: 'button',
    name: 'button_1',
    x: 20,
    y: 20,
    width: 120,
    height: 40,
    parentId: null,
    parentKind: 'window',
    parentTabIndex: null,
    props: {
      text: '按钮',
      bgColor: '#409EFF',
      fgColor: '#FFFFFF',
    },
  },
];

describe('AI structured protocol', () => {
  test('validateDraft + applyDraft 应生成完整画布结构', () => {
    const result = validateDraft({
      window: {
        title: 'AI 登录窗口',
        emoji: '🤖',
        width: 640,
        height: 420,
        titlebarColor: '#6C5CE7',
        bgColor: '#FFFFFF',
      },
      controls: [
        {
          type: 'button',
          name: '登录按钮',
          x: 100,
          y: 280,
          width: 140,
          height: 42,
          props: {
            text: '立即登录',
          },
        },
      ],
      summary: '生成一个登录按钮',
      warnings: [],
    });

    expect(result.ok).toBe(true);
    if (!result.ok) return;

    const applied = applyDraft(result.data);

    expect(applied.window.title).toBe('AI 登录窗口');
    expect(applied.controls).toHaveLength(1);
    expect(applied.controls[0].type).toBe('button');
    expect(applied.controls[0].props.text).toBe('立即登录');
    expect(applied.controls[0].props.bgColor).toBe('#409EFF');
  });

  test('validateOps + applyOps 应支持更新窗口并增改控件', () => {
    const result = validateOps({
      operations: [
        {
          type: 'updateWindow',
          updates: {
            title: 'AI 修改后的窗口',
          },
        },
        {
          type: 'updateControl',
          target: 'button_1',
          updates: {
            props: {
              text: '提交',
            },
          },
        },
        {
          type: 'addControl',
          control: {
            type: 'label',
            name: '说明文本',
            x: 20,
            y: 80,
            width: 240,
            height: 28,
            props: {
              text: '请输入用户名和密码',
            },
          },
        },
      ],
      affectedControls: ['button_1'],
      summary: '添加说明并修改按钮文本',
      warnings: [],
    });

    expect(result.ok).toBe(true);
    if (!result.ok) return;

    const applied = applyOps(BASE_WINDOW, BASE_CONTROLS, result.data);

    expect(applied.window.title).toBe('AI 修改后的窗口');
    expect(applied.controls.find((control) => control.id === 'button_1')?.props.text).toBe('提交');
    expect(applied.controls.some((control) => control.type === 'label')).toBe(true);
  });
});
