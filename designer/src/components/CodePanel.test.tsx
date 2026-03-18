import { fireEvent, render, screen } from '@testing-library/react';
import CodePanel from './CodePanel';
import { useDesignerStore } from '../store/designerStore';
import * as fileTransfer from '../utils/fileTransfer';

vi.mock('../utils/fileTransfer', async () => {
  const actual = await vi.importActual<typeof import('../utils/fileTransfer')>('../utils/fileTransfer');
  return {
    ...actual,
    downloadTextFile: vi.fn().mockResolvedValue(undefined),
  };
});

describe('CodePanel resize', () => {
  beforeEach(() => {
    useDesignerStore.setState({
      window: {
        title: '测试窗体',
        emoji: '🎨',
        width: 800,
        height: 600,
        titlebarColor: '#409EFF',
        bgColor: '#F5F7FA',
      },
      controls: [],
      selectedIds: [],
      activeView: 'designer',
      codeLanguage: 'python',
      showCode: true,
      codePanelHeight: 300,
      toolboxDragging: null,
      dragGhostPos: null,
      clipboard: null,
    });
  });

  test('拖动分隔条时应更新代码区高度', () => {
    render(<CodePanel />);

    fireEvent.mouseDown(screen.getByRole('separator', { name: '调整代码区高度' }), {
      clientY: 600,
    });
    fireEvent.mouseMove(window, { clientY: 500 });
    fireEvent.mouseUp(window);

    expect(useDesignerStore.getState().codePanelHeight).toBe(400);
  });

  test('点击下载按钮时应调用文件下载适配层', () => {
    render(<CodePanel />);

    fireEvent.click(screen.getByRole('button', { name: '下载' }));

    expect(fileTransfer.downloadTextFile).toHaveBeenCalledWith(
      'emoji_window_app.py',
      expect.any(String),
      'text/plain'
    );
  });
});
