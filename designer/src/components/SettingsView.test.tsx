import { fireEvent, render, screen, waitFor } from '@testing-library/react';
import SettingsView from './SettingsView';
import { useDesignerStore } from '../store/designerStore';
import { testAiConnection } from '../ai/testConnection';

vi.mock('../ai/testConnection', () => ({
  testAiConnection: vi.fn(),
}));

describe('SettingsView AI 配置', () => {
  beforeEach(() => {
    useDesignerStore.setState({
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
        aiSystemPrompt: '你是 UI 助手',
        aiPromptTemplateId: 'blank',
        aiRequestTimeoutMs: 15000,
        aiTemperature: 0.2,
        aiHistoryFileName: 'history.json',
        aiEnableEventGeneration: true,
        aiEnableAdvancedNaming: true,
      },
    } as never);
  });

  test('应显示 AI 配置与本地保存提示', () => {
    render(<SettingsView />);

    expect(screen.getByText('所有 AI 配置仅保存在本地，不同步云端')).toBeInTheDocument();
    expect(screen.getByLabelText('AI 提供商')).toBeInTheDocument();
    expect(screen.getByLabelText('AI BaseURL')).toBeInTheDocument();
    expect(screen.getByLabelText('AI API Key')).toBeInTheDocument();
    expect(screen.getByLabelText('AI 模型')).toBeInTheDocument();
    expect(screen.getByRole('button', { name: '测试连接' })).toBeInTheDocument();
  });

  test('切换 AI 提供商后应更新表单值', () => {
    render(<SettingsView />);

    fireEvent.change(screen.getByLabelText('AI 提供商'), {
      target: { value: 'deepseek' },
    });

    expect(screen.getByRole('option', { name: 'DeepSeek' })).toBeInTheDocument();
    expect(screen.getByRole('option', { name: '阿里通义' })).toBeInTheDocument();
    expect((screen.getByLabelText('AI 提供商') as HTMLSelectElement).value).toBe('deepseek');
  });

  test('点击测试连接后应显示连接成功提示', async () => {
    vi.mocked(testAiConnection).mockResolvedValue({
      ok: true,
      provider: 'openai-compatible',
      model: 'gpt-4.1-mini',
      durationMs: 123,
      statusCode: 200,
      message: '连接成功，模型 gpt-4.1-mini 可用',
    });

    render(<SettingsView />);

    fireEvent.click(screen.getByRole('button', { name: '测试连接' }));

    await waitFor(() => {
      expect(screen.getByText('连接成功，模型 gpt-4.1-mini 可用')).toBeInTheDocument();
    });
  });

  test('测试连接和保存设置按钮应使用更大的主按钮样式', () => {
    render(<SettingsView />);

    expect(screen.getByRole('button', { name: '测试连接' })).toHaveClass('settings-primary-btn');
    expect(screen.getByRole('button', { name: '保存设置' })).toHaveClass('settings-primary-btn');
  });

  test('应使用新版分组卡片布局', () => {
    render(<SettingsView />);

    expect(screen.getByText('本地配置中心')).toBeInTheDocument();
    expect(screen.getByText('快捷键设置')).toBeInTheDocument();
    expect(screen.getByText('AI 连接与模型')).toBeInTheDocument();
    expect(screen.getByText('本地保存说明')).toBeInTheDocument();
  });
});
