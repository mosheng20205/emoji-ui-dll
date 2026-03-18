import { chatWithAi, testAiConnection } from './providerRouter';
import { DEFAULT_RUNTIME_CONFIG, type RuntimeConfig } from '../config/runtimeConfig';

const BASE_CONFIG: RuntimeConfig = {
  ...DEFAULT_RUNTIME_CONFIG,
  aiProvider: 'openai-compatible',
  aiBaseUrl: 'https://example.com/v1',
  aiApiKey: 'sk-test',
  aiModel: 'gpt-4.1-mini',
  aiHistoryFileName: 'history.json',
};

describe('providerRouter', () => {
  afterEach(() => {
    vi.unstubAllGlobals();
  });

  test('OpenAI Compatible 测试连接应请求 models 接口', async () => {
    const fetchMock = vi.fn().mockResolvedValue(
      new Response(JSON.stringify({ data: [{ id: 'gpt-4.1-mini' }] }), {
        status: 200,
        headers: { 'Content-Type': 'application/json' },
      })
    );
    vi.stubGlobal('fetch', fetchMock);

    const result = await testAiConnection(BASE_CONFIG);

    expect(fetchMock).toHaveBeenCalledWith(
      'https://example.com/v1/models',
      expect.objectContaining({
        method: 'GET',
        headers: expect.objectContaining({
          Authorization: 'Bearer sk-test',
        }),
      })
    );
    expect(result.ok).toBe(true);
    expect(result.message).toContain('gpt-4.1-mini');
  });

  test('Anthropic 聊天应发送 messages 请求并提取文本', async () => {
    const fetchMock = vi.fn().mockResolvedValue(
      new Response(
        JSON.stringify({
          content: [{ type: 'text', text: '{"summary":"ok"}' }],
        }),
        {
          status: 200,
          headers: { 'Content-Type': 'application/json' },
        }
      )
    );
    vi.stubGlobal('fetch', fetchMock);

    const result = await chatWithAi(
      {
        ...BASE_CONFIG,
        aiProvider: 'anthropic',
        aiBaseUrl: 'https://api.anthropic.com/v1',
        aiApiKey: 'anthropic-key',
        aiModel: 'claude-3-5-sonnet-latest',
      },
      [
        { role: 'system', content: '你是 UI 助手' },
        { role: 'user', content: '生成一个登录窗口' },
      ]
    );

    expect(fetchMock).toHaveBeenCalledTimes(1);
    const [url, init] = fetchMock.mock.calls[0] as [string, RequestInit];
    expect(url).toBe('https://api.anthropic.com/v1/messages');
    expect(init.method).toBe('POST');
    expect(init.headers).toMatchObject({
      'x-api-key': 'anthropic-key',
      'anthropic-version': '2023-06-01',
    });

    const body = JSON.parse(String(init.body));
    expect(body.system).toBe('你是 UI 助手');
    expect(body.messages).toEqual([{ role: 'user', content: '生成一个登录窗口' }]);
    expect(result.text).toBe('{"summary":"ok"}');
  });

  test('Gemini 聊天应提取 candidates 文本', async () => {
    const fetchMock = vi.fn().mockResolvedValue(
      new Response(
        JSON.stringify({
          candidates: [
            {
              content: {
                parts: [{ text: '{"summary":"gemini"}' }],
              },
            },
          ],
        }),
        {
          status: 200,
          headers: { 'Content-Type': 'application/json' },
        }
      )
    );
    vi.stubGlobal('fetch', fetchMock);

    const result = await chatWithAi(
      {
        ...BASE_CONFIG,
        aiProvider: 'gemini',
        aiBaseUrl: 'https://generativelanguage.googleapis.com/v1beta',
        aiApiKey: 'gemini-key',
        aiModel: 'gemini-2.0-flash',
      },
      [{ role: 'user', content: '生成设置页' }]
    );

    expect(fetchMock).toHaveBeenCalledWith(
      'https://generativelanguage.googleapis.com/v1beta/models/gemini-2.0-flash:generateContent?key=gemini-key',
      expect.objectContaining({
        method: 'POST',
      })
    );
    expect(result.text).toBe('{"summary":"gemini"}');
  });

  test('DeepSeek 测试连接应复用 OpenAI 兼容协议', async () => {
    const fetchMock = vi.fn().mockResolvedValue(
      new Response(JSON.stringify({ data: [{ id: 'deepseek-chat' }] }), {
        status: 200,
        headers: { 'Content-Type': 'application/json' },
      })
    );
    vi.stubGlobal('fetch', fetchMock);

    const result = await testAiConnection({
      ...BASE_CONFIG,
      aiProvider: 'deepseek',
      aiBaseUrl: 'https://api.deepseek.com/v1',
      aiModel: 'deepseek-chat',
    });

    expect(fetchMock).toHaveBeenCalledWith(
      'https://api.deepseek.com/v1/models',
      expect.objectContaining({ method: 'GET' })
    );
    expect(result.ok).toBe(true);
    expect(result.model).toBe('deepseek-chat');
  });

  test('阿里通义测试连接应复用 OpenAI 兼容协议', async () => {
    const fetchMock = vi.fn().mockResolvedValue(
      new Response(JSON.stringify({ data: [{ id: 'qwen-plus' }] }), {
        status: 200,
        headers: { 'Content-Type': 'application/json' },
      })
    );
    vi.stubGlobal('fetch', fetchMock);

    const result = await testAiConnection({
      ...BASE_CONFIG,
      aiProvider: 'alibaba',
      aiBaseUrl: 'https://dashscope.aliyuncs.com/compatible-mode/v1',
      aiModel: 'qwen-plus',
    });

    expect(fetchMock).toHaveBeenCalledWith(
      'https://dashscope.aliyuncs.com/compatible-mode/v1/models',
      expect.objectContaining({ method: 'GET' })
    );
    expect(result.ok).toBe(true);
    expect(result.model).toBe('qwen-plus');
  });
});
