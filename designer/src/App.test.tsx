import { fireEvent, render, screen, waitFor } from '@testing-library/react';
import App from './App';
import { DEFAULT_RUNTIME_CONFIG } from './config/runtimeConfig';
import { useDesignerStore } from './store/designerStore';
import { reportActive, reportLaunch } from './services/statsReporter';

vi.mock('./services/statsReporter', () => ({
  reportLaunch: vi.fn().mockResolvedValue(undefined),
  reportActive: vi.fn().mockResolvedValue(undefined),
  reportAdEvent: vi.fn().mockResolvedValue(undefined),
}));

describe('App shell views', () => {
  beforeEach(() => {
    vi.clearAllMocks();
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
        aiBaseUrl: 'https://example.com/v1',
        aiApiKey: 'sk-test',
        aiHistoryFileName: 'history.json',
      },
      codeLanguage: 'python',
      showCode: true,
      codePanelHeight: 300,
      emojiPickerOpen: false,
      toolboxDragging: null,
      dragGhostPos: null,
      clipboard: null,
    });
  });

  test('切换到关于视图时应显示关于页面', () => {
    useDesignerStore.setState({ activeView: 'about' } as never);

    render(<App />);

    expect(screen.getByRole('heading', { name: '关于' })).toBeInTheDocument();
    expect(screen.queryByText('生成代码')).not.toBeInTheDocument();
  });

  test('切换到系统设置视图时应显示设置页面', () => {
    useDesignerStore.setState({ activeView: 'settings' } as never);

    render(<App />);

    expect(screen.getByRole('heading', { name: '系统设置' })).toBeInTheDocument();
    expect(screen.queryByText('生成代码')).not.toBeInTheDocument();
  });

  test('切换到 AI 助手视图时应显示 AI 页面', () => {
    useDesignerStore.setState({ activeView: 'ai-assistant' } as never);

    render(<App />);

    expect(screen.getByRole('heading', { name: 'AI 助手' })).toBeInTheDocument();
  });

  test('点击左下角关于按钮时应切换到关于视图', () => {
    render(<App />);

    fireEvent.click(screen.getByRole('button', { name: '关于' }));

    expect(screen.getByRole('heading', { name: '关于' })).toBeInTheDocument();
  });

  test('点击左下角 AI 助手按钮时应切换到 AI 视图', () => {
    render(<App />);

    fireEvent.click(screen.getByRole('button', { name: 'AI 助手' }));

    expect(screen.getByRole('heading', { name: 'AI 助手' })).toBeInTheDocument();
  });

  test('按下快捷键时应打开 Emoji 面板', () => {
    render(<App />);

    fireEvent.keyDown(window, { key: 'E', ctrlKey: true, shiftKey: true });

    expect(screen.getByRole('dialog', { name: 'Emoji 选择器' })).toBeInTheDocument();
  });

  test('按下快速插入控件快捷键时应添加控件', () => {
    render(<App />);

    fireEvent.keyDown(window, { key: 'I', ctrlKey: true, shiftKey: true });

    expect(useDesignerStore.getState().controls).toHaveLength(1);
    expect(useDesignerStore.getState().controls[0]?.type).toBe('button');
  });

  test('启动时应触发统计上报', () => {
    render(<App />);

    expect(reportLaunch).toHaveBeenCalled();
  });

  test('首次真实交互时应触发活跃上报', () => {
    render(<App />);

    fireEvent.pointerDown(window);

    expect(reportActive).toHaveBeenCalled();
  });

  test('首次活跃上报失败后再次交互应重试', async () => {
    vi.mocked(reportActive)
      .mockResolvedValueOnce(false as never)
      .mockResolvedValueOnce(true as never);

    render(<App />);

    fireEvent.pointerDown(window);
    await waitFor(() => {
      expect(reportActive).toHaveBeenCalledTimes(1);
    });
    fireEvent.keyDown(window, { key: 'A' });

    await waitFor(() => {
      expect(reportActive).toHaveBeenCalledTimes(2);
    });
  });
});
