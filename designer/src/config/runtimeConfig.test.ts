import {
  DEFAULT_RUNTIME_CONFIG,
  RUNTIME_CONFIG_STORAGE_KEY,
  loadRuntimeConfig,
  resolveEnvRuntimeConfig,
} from './runtimeConfig';

describe('runtime config', () => {
  beforeEach(() => {
    localStorage.clear();
  });

  test('.env 配置应作为默认运行时配置来源', () => {
    const config = resolveEnvRuntimeConfig({
      VITE_AD_API_URL: 'https://example.com/ads',
      VITE_STATS_API_URL: 'https://example.com/stats',
      VITE_REPORT_LAUNCH_API_URL: 'https://example.com/report-launch',
      VITE_REPORT_ACTIVE_API_URL: 'https://example.com/report-active',
      VITE_REPORT_AD_EVENT_API_URL: 'https://example.com/report-ad-event',
      VITE_REQUEST_TIMEOUT_MS: '8000',
      VITE_EMOJI_HOTKEY: 'Ctrl+Alt+E',
      VITE_QUICK_INSERT_HOTKEY: 'Ctrl+Alt+I',
      VITE_QUICK_INSERT_CONTROL: 'label',
      VITE_AI_PROVIDER: 'openai-compatible',
      VITE_AI_BASE_URL: 'https://example.com/v1',
      VITE_AI_API_KEY: 'sk-test',
      VITE_AI_MODEL: 'gpt-4.1-mini',
      VITE_AI_SYSTEM_PROMPT: '你是 UI 助手',
      VITE_AI_PROMPT_TEMPLATE_ID: 'login-form',
      VITE_AI_REQUEST_TIMEOUT_MS: '12000',
      VITE_AI_TEMPERATURE: '0.3',
      VITE_AI_HISTORY_FILE_NAME: 'history.json',
      VITE_AI_ENABLE_EVENT_GENERATION: 'true',
      VITE_AI_ENABLE_ADVANCED_NAMING: 'true',
    });

    expect(config).toEqual({
      adApiUrl: 'https://example.com/ads',
      statsApiUrl: 'https://example.com/stats',
      reportLaunchApiUrl: 'https://example.com/report-launch',
      reportActiveApiUrl: 'https://example.com/report-active',
      reportAdEventApiUrl: 'https://example.com/report-ad-event',
      requestTimeoutMs: 8000,
      emojiHotkey: 'Ctrl+Alt+E',
      quickInsertHotkey: 'Ctrl+Alt+I',
      quickInsertControl: 'label',
      aiProvider: 'openai-compatible',
      aiBaseUrl: 'https://example.com/v1',
      aiApiKey: 'sk-test',
      aiModel: 'gpt-4.1-mini',
      aiSystemPrompt: '你是 UI 助手',
      aiPromptTemplateId: 'login-form',
      aiRequestTimeoutMs: 12000,
      aiTemperature: 0.3,
      aiHistoryFileName: 'history.json',
      aiEnableEventGeneration: true,
      aiEnableAdvancedNaming: true,
    });
  });

  test('本地设置应覆盖 .env 默认配置', () => {
    localStorage.setItem(
      RUNTIME_CONFIG_STORAGE_KEY,
      JSON.stringify({
        adApiUrl: 'https://local.test/ads',
        reportLaunchApiUrl: 'https://local.test/report-launch',
        quickInsertControl: 'checkbox',
        aiModel: 'claude-test',
      })
    );

    const config = loadRuntimeConfig({
      VITE_AD_API_URL: 'https://env.test/ads',
      VITE_STATS_API_URL: 'https://env.test/stats',
      VITE_REPORT_ACTIVE_API_URL: 'https://env.test/report-active',
    });

    expect(config.adApiUrl).toBe('https://local.test/ads');
    expect(config.statsApiUrl).toBe('https://env.test/stats');
    expect(config.reportLaunchApiUrl).toBe('https://local.test/report-launch');
    expect(config.reportActiveApiUrl).toBe('https://env.test/report-active');
    expect(config.quickInsertControl).toBe('checkbox');
    expect(config.emojiHotkey).toBe(DEFAULT_RUNTIME_CONFIG.emojiHotkey);
    expect(config.aiModel).toBe('claude-test');
    expect(config.aiProvider).toBe(DEFAULT_RUNTIME_CONFIG.aiProvider);
  });

  test('localStorage 中的空字符串不应覆盖 .env 非空默认值', () => {
    localStorage.setItem(
      RUNTIME_CONFIG_STORAGE_KEY,
      JSON.stringify({
        adApiUrl: '',
        statsApiUrl: '',
        reportLaunchApiUrl: '',
        aiModel: 'custom-model',
      })
    );

    const config = loadRuntimeConfig({
      VITE_AD_API_URL: 'https://env.test/ads',
      VITE_STATS_API_URL: 'https://env.test/stats',
      VITE_REPORT_LAUNCH_API_URL: 'https://env.test/report-launch',
    });

    expect(config.adApiUrl).toBe('https://env.test/ads');
    expect(config.statsApiUrl).toBe('https://env.test/stats');
    expect(config.reportLaunchApiUrl).toBe('https://env.test/report-launch');
    expect(config.aiModel).toBe('custom-model');
  });
});
