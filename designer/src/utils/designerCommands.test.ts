import type { DesignControl } from '../types/controls';
import {
  alignControls,
  duplicateControls,
  resizeWindowByHandle,
  toggleSelection,
} from './designerCommands';

const controls: DesignControl[] = [
  {
    id: 'a',
    type: 'button',
    name: 'a',
    x: 100,
    y: 100,
    width: 100,
    height: 40,
    props: {},
  },
  {
    id: 'b',
    type: 'label',
    name: 'b',
    x: 220,
    y: 140,
    width: 120,
    height: 30,
    props: {},
  },
];

describe('designerCommands', () => {
  test('toggleSelection 支持单选和追加选择', () => {
    expect(toggleSelection([], 'a', false)).toEqual(['a']);
    expect(toggleSelection(['a'], 'b', true)).toEqual(['a', 'b']);
    expect(toggleSelection(['a', 'b'], 'a', true)).toEqual(['b']);
  });

  test('alignControls 支持左对齐', () => {
    const result = alignControls(controls, ['a', 'b'], 'left');
    const a = result.find((item) => item.id === 'a');
    const b = result.find((item) => item.id === 'b');
    expect(a?.x).toBe(100);
    expect(b?.x).toBe(100);
  });

  test('duplicateControls 会生成偏移副本', () => {
    const result = duplicateControls(controls, ['a']);
    expect(result.created).toHaveLength(1);
    expect(result.created[0].id).not.toBe('a');
    expect(result.created[0].x).toBe(120);
    expect(result.created[0].y).toBe(120);
  });

  test('resizeWindowByHandle 支持右下角缩放', () => {
    const resized = resizeWindowByHandle(
      { width: 800, height: 600 },
      'se',
      { dx: 120, dy: 80 },
      { minWidth: 200, minHeight: 150 }
    );
    expect(resized.width).toBe(920);
    expect(resized.height).toBe(680);
  });
});
