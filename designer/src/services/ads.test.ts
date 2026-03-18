import { fetchAds, DEFAULT_ADS } from './ads';
import { DEFAULT_RUNTIME_CONFIG } from '../config/runtimeConfig';

vi.mock('@tauri-apps/plugin-http', () => ({
  fetch: vi.fn(),
}));

describe('ads service', () => {
  beforeEach(() => {
    vi.restoreAllMocks();
    delete (window as Window & { __TAURI_INTERNALS__?: unknown }).__TAURI_INTERNALS__;
  });

  test('未配置 API 地址时应返回默认广告位', async () => {
    const ads = await fetchAds({
      ...DEFAULT_RUNTIME_CONFIG,
      adApiUrl: '',
      requestTimeoutMs: 3000,
    });

    expect(ads).toEqual(DEFAULT_ADS);
  });

  test('应兼容后端 data.items + title/url 的广告结构', async () => {
    vi.stubGlobal('fetch', vi.fn().mockResolvedValue({
      ok: true,
      json: async () => ({
        code: 0,
        message: 'ok',
        data: {
          items: [
            {
              id: 4,
              title: '支持AI和skills',
              description: '支持AI和skills',
              url: 'http://bcloak.com/',
            },
            {
              id: 5,
              title: '广告位招租',
              description: '广告位招租',
              url: 'http://8.134.23.223:3003/public/index.html',
            },
          ],
        },
      }),
    }));

    const ads = await fetchAds({
      ...DEFAULT_RUNTIME_CONFIG,
      adApiUrl: 'http://8.134.23.223:3003/api/ads.php',
      requestTimeoutMs: 3000,
    });

    expect(ads).toEqual([
      { id: '4', text: '支持AI和skills', url: 'http://bcloak.com/' },
      { id: '5', text: '广告位招租', url: 'http://8.134.23.223:3003/public/index.html' },
    ]);
  });

  test('Tauri 打包环境应使用 plugin-http 拉取广告，避免 CORS 回退到默认广告', async () => {
    const tauriFetch = vi.mocked((await import('@tauri-apps/plugin-http')).fetch);
    Object.defineProperty(window, '__TAURI_INTERNALS__', {
      value: {},
      configurable: true,
    });
    vi.stubGlobal('fetch', vi.fn().mockRejectedValue(new Error('cors blocked')));
    tauriFetch.mockResolvedValue(
      new Response(
        JSON.stringify({
          code: 0,
          data: {
            items: [
              { id: 4, title: '广告A', url: 'https://example.com/a' },
              { id: 5, title: '广告B', url: 'https://example.com/b' },
            ],
          },
        }),
        {
          status: 200,
          headers: { 'Content-Type': 'application/json' },
        }
      )
    );

    const ads = await fetchAds({
      ...DEFAULT_RUNTIME_CONFIG,
      adApiUrl: 'http://8.134.23.223:3003/api/ads.php',
      requestTimeoutMs: 3000,
    });

    expect(tauriFetch).toHaveBeenCalledWith(
      'http://8.134.23.223:3003/api/ads.php',
      expect.objectContaining({
        method: 'GET',
        connectTimeout: 3000,
      })
    );
    expect(ads).toEqual([
      { id: '4', text: '广告A', url: 'https://example.com/a' },
      { id: '5', text: '广告B', url: 'https://example.com/b' },
    ]);
  });
});
