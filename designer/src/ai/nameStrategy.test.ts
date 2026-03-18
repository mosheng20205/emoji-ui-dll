import { buildSemanticControlName, createUniqueControlName } from './nameStrategy';

describe('AI name strategy', () => {
  test('应将语义化名称规范成合法标识符', () => {
    expect(buildSemanticControlName('button', 'Login Button')).toBe('login_button');
    expect(buildSemanticControlName('label', '###')).toBe('label');
  });

  test('应自动去重并保持稳定后缀', () => {
    const used = new Set<string>();

    expect(createUniqueControlName('login_button', used)).toBe('login_button');
    expect(createUniqueControlName('login_button', used)).toBe('login_button_2');
    expect(createUniqueControlName('login_button', used)).toBe('login_button_3');
  });
});
