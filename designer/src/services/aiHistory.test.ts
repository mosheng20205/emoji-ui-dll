import { listAiHistorySessions, loadAiHistorySession, saveAiHistorySession } from './aiHistory';
import { DEFAULT_RUNTIME_CONFIG, type RuntimeConfig } from '../config/runtimeConfig';

function createMemoryAdapter() {
  const files = new Map<string, string>();
  return {
    baseDir: 0,
    mkdir: vi.fn().mockResolvedValue(undefined),
    writeTextFile: vi.fn().mockImplementation(async (path: string, content: string) => {
      files.set(path, content);
    }),
    readTextFile: vi.fn().mockImplementation(async (path: string) => {
      const content = files.get(path);
      if (!content) {
        throw new Error('file not found');
      }
      return content;
    }),
    exists: vi.fn().mockImplementation(async (path: string) => files.has(path)),
  };
}

const CONFIG: RuntimeConfig = {
  ...DEFAULT_RUNTIME_CONFIG,
  aiBaseUrl: 'https://example.com/v1',
  aiApiKey: 'sk-test',
  aiHistoryFileName: 'ai-history.json',
};

describe('aiHistory service', () => {
  test('应保存会话文件并可恢复索引与内容', async () => {
    const adapter = createMemoryAdapter();

    await saveAiHistorySession(
      CONFIG,
      {
        sessionId: 'session-1',
        title: '登录窗体',
        createdAt: '2026-03-18T00:00:00.000Z',
        updatedAt: '2026-03-18T00:10:00.000Z',
        messages: [{ role: 'user', content: '生成登录窗体' }],
        rawResponse: '{"mode":"draft"}',
        draftPreview: null,
        opsPreview: null,
      },
      adapter as never
    );

    const list = await listAiHistorySessions(CONFIG, adapter as never);
    const session = await loadAiHistorySession(CONFIG, 'session-1', adapter as never);

    expect(list).toHaveLength(1);
    expect(list[0].title).toBe('登录窗体');
    expect(session?.messages[0].content).toBe('生成登录窗体');
  });
});
