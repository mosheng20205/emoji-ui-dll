import { act, fireEvent, render, screen, waitFor } from '@testing-library/react';
import Header from './Header';
import { useDesignerStore } from '../store/designerStore';
import * as fileTransfer from '../utils/fileTransfer';
import { openExternal } from '../utils/externalLink';
import * as adsService from '../services/ads';
import * as statsReporter from '../services/statsReporter';

vi.mock('../utils/fileTransfer', async () => {
  const actual = await vi.importActual<typeof import('../utils/fileTransfer')>('../utils/fileTransfer');
  return {
    ...actual,
    downloadTextFile: vi.fn().mockResolvedValue(undefined),
    promptJsonFile: vi.fn().mockResolvedValue(undefined),
  };
});

vi.mock('../utils/externalLink', () => ({
  openExternal: vi.fn().mockResolvedValue(undefined),
}));

vi.mock('../services/ads', async () => {
  const actual = await vi.importActual<typeof import('../services/ads')>('../services/ads');
  return {
    ...actual,
    fetchAds: vi.fn().mockResolvedValue(actual.DEFAULT_ADS),
  };
});

vi.mock('../services/statsReporter', () => ({
  reportAdEvent: vi.fn().mockResolvedValue(undefined),
}));

describe('Header import/export', () => {
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
      codeLanguage: 'python',
      showCode: true,
      toolboxDragging: null,
      dragGhostPos: null,
    });
  });

  test('导出按钮应调用导出服务', async () => {
    render(<Header />);

    fireEvent.click(screen.getByRole('button', { name: /导出/i }));

    await waitFor(() => {
      expect(fileTransfer.downloadTextFile).toHaveBeenCalled();
    });
  });

  test('导入按钮应调用导入服务', async () => {
    render(<Header />);

    fireEvent.click(screen.getByRole('button', { name: /导入/i }));
    await waitFor(() => {
      expect(fileTransfer.promptJsonFile).toHaveBeenCalled();
    });
  });

  test('应显示QQ群入口和默认广告文案', () => {
    render(<Header />);

    expect(screen.getByRole('button', { name: 'QQ交流群' })).toBeInTheDocument();
    expect(screen.getByText('广告位招租(QQ1098901025)')).toBeInTheDocument();
  });

  test('点击QQ群入口时应打开外链', () => {
    render(<Header />);

    fireEvent.click(screen.getByRole('button', { name: 'QQ交流群' }));

    expect(openExternal).toHaveBeenCalledWith('https://qm.qq.com/q/NIZipXNsUS');
  });

  test('QQ群入口应显示企鹅图标而不是文字 QQ 徽标', () => {
    render(<Header />);

    expect(screen.getAllByTestId('qq-penguin-icon').length).toBeGreaterThan(0);
    expect(screen.queryByText('QQ', { selector: '.qq-icon-badge' })).not.toBeInTheDocument();
  });

  test('接口返回两个广告时应轮播展示', async () => {
    vi.useFakeTimers();
    vi.mocked(adsService.fetchAds).mockResolvedValue([
      { id: '1', text: '广告1', url: 'https://example.com/1' },
      { id: '2', text: '广告2', url: 'https://example.com/2' },
    ]);

    render(<Header />);

    await act(async () => {
      await Promise.resolve();
      await Promise.resolve();
    });
    expect(screen.getByText('广告1')).toBeInTheDocument();

    await act(async () => {
      await vi.advanceTimersByTimeAsync(3000);
    });
    expect(screen.getByText('广告2')).toBeInTheDocument();

    vi.useRealTimers();
  });

  test('展示真实广告时应上报曝光事件', async () => {
    vi.mocked(adsService.fetchAds).mockResolvedValue([
      { id: '1', text: '广告1', url: 'https://example.com/1' },
    ]);

    render(<Header />);

    await waitFor(() => {
      expect(statsReporter.reportAdEvent).toHaveBeenCalledWith(
        'impression',
        { id: '1', text: '广告1', url: 'https://example.com/1' },
        expect.any(Object)
      );
    });
  });

  test('点击真实广告时应上报点击事件并打开外链', async () => {
    vi.mocked(adsService.fetchAds).mockResolvedValue([
      { id: '2', text: '广告2', url: 'https://example.com/2' },
    ]);

    render(<Header />);

    await screen.findByText('广告2');
    fireEvent.click(screen.getByRole('button', { name: '广告2' }));

    await waitFor(() => {
      expect(statsReporter.reportAdEvent).toHaveBeenCalledWith(
        'click',
        { id: '2', text: '广告2', url: 'https://example.com/2' },
        expect.any(Object)
      );
    });
    expect(openExternal).toHaveBeenCalledWith('https://example.com/2');
  });

  test('运行时配置变化时不应重复上报同一条广告曝光', async () => {
    vi.mocked(adsService.fetchAds).mockResolvedValue([
      { id: '3', text: '广告3', url: 'https://example.com/3' },
    ]);

    render(<Header />);

    await waitFor(() => {
      expect(statsReporter.reportAdEvent).toHaveBeenCalledWith(
        'impression',
        { id: '3', text: '广告3', url: 'https://example.com/3' },
        expect.any(Object)
      );
    });

    act(() => {
      const state = useDesignerStore.getState();
      useDesignerStore.setState({
        runtimeConfig: {
          ...state.runtimeConfig,
          requestTimeoutMs: state.runtimeConfig.requestTimeoutMs + 1,
        },
      });
    });

    await act(async () => {
      await Promise.resolve();
    });

    expect(
      vi.mocked(statsReporter.reportAdEvent).mock.calls.filter(
        ([event, ad]) => event === 'impression' && ad.id === '3'
      )
    ).toHaveLength(1);
  });
});
