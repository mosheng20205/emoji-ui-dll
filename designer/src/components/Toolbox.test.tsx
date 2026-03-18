import { fireEvent, render, screen, waitFor } from '@testing-library/react';
import Toolbox from './Toolbox';
import { useDesignerStore } from '../store/designerStore';
import { DEFAULT_RUNTIME_CONFIG } from '../config/runtimeConfig';

describe('Toolbox emoji picker', () => {
  beforeEach(() => {
    localStorage.clear();
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
      runtimeConfig: {
        ...DEFAULT_RUNTIME_CONFIG,
      },
      codeLanguage: 'python',
      showCode: true,
      codePanelHeight: 300,
      emojiPickerOpen: false,
      toolboxDragging: null,
      dragGhostPos: null,
      clipboard: null,
    });

    Object.assign(navigator, {
      clipboard: {
        writeText: vi.fn().mockResolvedValue(undefined),
      },
    });
  });

  test('应显示分类 Emoji 面板并支持点击复制', async () => {
    render(<Toolbox />);

    fireEvent.click(screen.getByRole('button', { name: /^Emoji 选择器$/i }));

    expect(screen.getByText('最近使用')).toBeInTheDocument();
    expect(screen.getByText('笑脸')).toBeInTheDocument();
    expect(screen.getByText('动物')).toBeInTheDocument();

    fireEvent.click(screen.getByRole('button', { name: /😀/i }));

    await waitFor(() => {
      expect(navigator.clipboard.writeText).toHaveBeenCalledWith('😀');
    });
  });

  test('应支持搜索并跨分类过滤 Emoji', async () => {
    render(<Toolbox />);

    fireEvent.click(screen.getByRole('button', { name: /^Emoji 选择器$/i }));
    fireEvent.change(screen.getByPlaceholderText('搜索 Emoji 或分类'), {
      target: { value: '动物' },
    });

    expect(screen.getByRole('button', { name: /🐶/i })).toBeInTheDocument();
    expect(screen.queryByRole('button', { name: /😀/i })).not.toBeInTheDocument();
  });

  test('应从本地持久化加载最近使用并在复制后更新', async () => {
    localStorage.setItem('designer_recent_emojis', JSON.stringify(['🎉', '🔥']));

    render(<Toolbox />);

    fireEvent.click(screen.getByRole('button', { name: /^Emoji 选择器$/i }));

    expect(screen.getByRole('button', { name: /🎉/i })).toBeInTheDocument();
    expect(screen.getByRole('button', { name: /🔥/i })).toBeInTheDocument();

    fireEvent.click(screen.getByRole('button', { name: /^笑脸$/i }));
    fireEvent.click(screen.getByRole('button', { name: /😀/i }));

    await waitFor(() => {
      expect(JSON.parse(localStorage.getItem('designer_recent_emojis') ?? '[]')).toEqual(['😀', '🎉', '🔥']);
    });
  });

  test('点击空白区域时应关闭 Emoji 面板', () => {
    render(<Toolbox />);

    fireEvent.click(screen.getByRole('button', { name: /^Emoji 选择器$/i }));
    expect(screen.getByRole('dialog', { name: 'Emoji 选择器' })).toBeInTheDocument();

    fireEvent.mouseDown(document.body);

    expect(screen.queryByRole('dialog', { name: 'Emoji 选择器' })).not.toBeInTheDocument();
  });
});
