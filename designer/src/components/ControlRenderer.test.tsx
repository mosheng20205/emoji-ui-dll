import { render, screen } from '@testing-library/react';
import ControlRenderer from './ControlRenderer';
import type { DesignControl } from '../types/controls';

describe('ControlRenderer', () => {
  test('标签控件应显示配置的背景色', () => {
    const control: DesignControl = {
      id: 'label_1',
      type: 'label',
      name: 'label_1',
      x: 0,
      y: 0,
      width: 120,
      height: 30,
      props: {
        text: '标签文本',
        fgColor: '#111111',
        bgColor: '#ff0000',
        fontName: 'Microsoft YaHei UI',
        fontSize: 13,
        bold: false,
        italic: false,
        underline: false,
        alignment: 0,
        wordWrap: false,
      },
    };

    render(<ControlRenderer control={control} />);

    const label = screen.getByText('标签文本');
    expect(label).toHaveStyle('background-color: rgb(255, 0, 0)');
  });
});
