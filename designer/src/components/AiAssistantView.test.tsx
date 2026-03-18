import { fireEvent, render, screen, waitFor } from '@testing-library/react';
import AiAssistantView from './AiAssistantView';
import { useDesignerStore } from '../store/designerStore';
import { chatWithAi } from '../ai/providerRouter';
import { listAiHistorySessions, loadAiHistorySession } from '../services/aiHistory';

vi.mock('../ai/providerRouter', () => ({
  chatWithAi: vi.fn(),
}));

vi.mock('../services/aiHistory', () => ({
  listAiHistorySessions: vi.fn().mockResolvedValue([]),
  loadAiHistorySession: vi.fn().mockResolvedValue(null),
  saveAiHistorySession: vi.fn().mockResolvedValue(undefined),
}));

describe('AiAssistantView', () => {
  beforeEach(() => {
    useDesignerStore.setState({
      window: {
        title: '原始窗口',
        emoji: '🎨',
        width: 800,
        height: 600,
        titlebarColor: '#409EFF',
        bgColor: '#F5F7FA',
      },
      controls: [],
      aiSessionId: null,
      aiMessages: [],
      aiDraftPreview: null,
      aiOpsPreview: null,
      aiRawResponse: '',
      aiLastAppliedSnapshot: null,
      runtimeConfig: {
        adApiUrl: '',
        statsApiUrl: '',
        requestTimeoutMs: 5000,
        emojiHotkey: 'Ctrl+Shift+E',
        quickInsertHotkey: 'Ctrl+Shift+I',
        quickInsertControl: 'button',
        aiProvider: 'openai-compatible',
        aiBaseUrl: 'https://example.com/v1',
        aiApiKey: 'sk-test',
        aiModel: 'gpt-4.1-mini',
        aiSystemPrompt: '',
        aiPromptTemplateId: 'blank',
        aiRequestTimeoutMs: 15000,
        aiTemperature: 0.2,
        aiHistoryFileName: 'history.json',
        aiEnableEventGeneration: true,
        aiEnableAdvancedNaming: true,
      },
    } as never);
  });

  test('发送请求后应展示 draft 预览并可替换当前画布', async () => {
    vi.mocked(chatWithAi).mockResolvedValue({
      provider: 'openai-compatible',
      model: 'gpt-4.1-mini',
      text: JSON.stringify({
        mode: 'draft',
        summary: '生成登录窗体',
        warnings: [],
        window: {
          title: 'AI 登录窗体',
          emoji: '🤖',
          width: 640,
          height: 420,
          titlebarColor: '#6C5CE7',
          bgColor: '#FFFFFF',
        },
        controls: [
          {
            type: 'button',
            name: 'login_button',
            x: 100,
            y: 280,
            width: 140,
            height: 42,
            props: {
              text: '登录',
            },
          },
        ],
      }),
      raw: {},
    });

    render(<AiAssistantView />);

    fireEvent.change(screen.getByLabelText('AI 输入框'), {
      target: { value: '帮我生成一个登录窗体' },
    });
    fireEvent.click(screen.getByRole('button', { name: '发送到 AI' }));

    await waitFor(() => {
      expect(screen.getByText('生成登录窗体')).toBeInTheDocument();
      expect(screen.getByText('执行摘要')).toBeInTheDocument();
      expect(screen.getByText('窗口概览')).toBeInTheDocument();
      expect(screen.getByText('控件清单')).toBeInTheDocument();
      expect(screen.getByText('原始 JSON')).toBeInTheDocument();
      expect(screen.getByRole('button', { name: '替换当前画布' })).toBeInTheDocument();
    });

    fireEvent.click(screen.getByRole('button', { name: '替换当前画布' }));

    expect(useDesignerStore.getState().window.title).toBe('AI 登录窗体');
    expect(useDesignerStore.getState().controls).toHaveLength(1);
  });

  test('应将提示词配置显示为顶部 tab，聊天区显示历史会话入口', async () => {
    vi.mocked(listAiHistorySessions).mockResolvedValue([
      {
        sessionId: 'session-top',
        title: '顶部历史会话',
        updatedAt: '2026-03-18T00:00:00.000Z',
        fileName: 'session-top.json',
      },
    ]);

    render(<AiAssistantView />);

    expect(screen.getByRole('tab', { name: '提示词模板' })).toBeInTheDocument();
    expect(screen.getByRole('tab', { name: '系统提示词' })).toBeInTheDocument();

    await waitFor(() => {
      expect(screen.getByRole('button', { name: '历史会话' })).toBeInTheDocument();
    });
  });

  test('提示词与配置应支持折叠和展开', () => {
    render(<AiAssistantView />);

    expect(screen.getByText('提示词与配置')).toBeInTheDocument();
    expect(screen.getByLabelText('生成模式')).toBeInTheDocument();

    fireEvent.click(screen.getByRole('button', { name: '折叠提示词与配置' }));
    expect(screen.queryByLabelText('生成模式')).not.toBeInTheDocument();

    fireEvent.click(screen.getByRole('button', { name: '展开提示词与配置' }));
    expect(screen.getByLabelText('生成模式')).toBeInTheDocument();
  });

  test('已有画布时应支持预览并应用 ops 修改', async () => {
    useDesignerStore.setState({
      controls: [
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
            text: '旧按钮',
            bgColor: '#409EFF',
          },
        },
      ],
    } as never);

    vi.mocked(chatWithAi).mockResolvedValue({
      provider: 'openai-compatible',
      model: 'gpt-4.1-mini',
      text: JSON.stringify({
        mode: 'ops',
        summary: '修改现有布局',
        warnings: [],
        affectedControls: ['button_1'],
        operations: [
          {
            type: 'updateControl',
            target: 'button_1',
            updates: {
              props: {
                text: '新按钮',
              },
            },
          },
        ],
      }),
      raw: {},
    });

    render(<AiAssistantView />);

    fireEvent.change(screen.getByLabelText('AI 输入框'), {
      target: { value: '把当前按钮改成新按钮' },
    });
    fireEvent.click(screen.getByRole('button', { name: '发送到 AI' }));

    await waitFor(() => {
      expect(screen.getByText('修改现有布局')).toBeInTheDocument();
      expect(screen.getByText('操作摘要')).toBeInTheDocument();
      expect(screen.getByText('操作清单')).toBeInTheDocument();
      expect(screen.getByRole('button', { name: '应用操作清单' })).toBeInTheDocument();
    });

    fireEvent.click(screen.getByRole('button', { name: '应用操作清单' }));

    expect(useDesignerStore.getState().controls[0]?.props.text).toBe('新按钮');
  });

  test('应通过聊天区顶部历史入口恢复会话', async () => {
    vi.mocked(listAiHistorySessions).mockResolvedValue([
      {
        sessionId: 'session-old',
        title: '历史登录窗体',
        updatedAt: '2026-03-18T00:00:00.000Z',
        fileName: 'session-old.json',
      },
    ]);
    vi.mocked(loadAiHistorySession).mockResolvedValue({
      sessionId: 'session-old',
      title: '历史登录窗体',
      createdAt: '2026-03-18T00:00:00.000Z',
      updatedAt: '2026-03-18T00:00:00.000Z',
      messages: [{ role: 'user', content: '帮我恢复这个登录窗体' }],
      rawResponse: '',
      draftPreview: null,
      opsPreview: null,
    });

    render(<AiAssistantView />);

    await waitFor(() => {
      expect(screen.getByRole('button', { name: '历史会话' })).toBeInTheDocument();
    });

    fireEvent.click(screen.getByRole('button', { name: '历史会话' }));
    fireEvent.click(screen.getByRole('button', { name: /恢复：历史登录窗体/i }));

    await waitFor(() => {
      expect(screen.getByText('帮我恢复这个登录窗体')).toBeInTheDocument();
    });
  });

  test('发送按钮应为右下角大按钮样式', () => {
    render(<AiAssistantView />);

    const sendButton = screen.getByRole('button', { name: '发送到 AI' });
    expect(sendButton).toHaveClass('settings-primary-btn');
    expect(sendButton).toHaveClass('ai-send-btn');
  });

  test('空预览区应展示操作流程提示卡', () => {
    render(<AiAssistantView />);

    expect(screen.getByText('AI Design Copilot')).toBeInTheDocument();
    expect(screen.getByText('建议提示')).toBeInTheDocument();
    expect(screen.getByText('创建一个登录窗口')).toBeInTheDocument();
    expect(screen.getByText('操作流程')).toBeInTheDocument();
    expect(screen.getByText('发送需求')).toBeInTheDocument();
    expect(screen.getByText('检查预览')).toBeInTheDocument();
    expect(screen.getByText('应用或回滚')).toBeInTheDocument();
  });
});
