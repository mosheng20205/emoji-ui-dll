import { runtimeFetch } from './httpClient';

vi.mock('@tauri-apps/plugin-http', () => ({
  fetch: vi.fn(),
}));

describe('runtimeFetch', () => {
  beforeEach(() => {
    vi.restoreAllMocks();
    delete (window as Window & { __TAURI_INTERNALS__?: unknown }).__TAURI_INTERNALS__;
  });

  test('普通 fetch 因跨域失败时应自动回退到 plugin-http', async () => {
    const browserFetch = vi.fn().mockRejectedValue(new TypeError('Failed to fetch'));
    const tauriFetch = vi.mocked((await import('@tauri-apps/plugin-http')).fetch);
    vi.stubGlobal('fetch', browserFetch);
    tauriFetch.mockResolvedValue(
      new Response(JSON.stringify({ ok: true }), {
        status: 200,
        headers: { 'Content-Type': 'application/json' },
      })
    );

    const response = await runtimeFetch('http://8.134.23.223:3003/api/ads.php', {
      method: 'GET',
      timeoutMs: 3000,
    });

    expect(browserFetch).toHaveBeenCalled();
    expect(tauriFetch).toHaveBeenCalledWith(
      'http://8.134.23.223:3003/api/ads.php',
      expect.objectContaining({
        method: 'GET',
        connectTimeout: 3000,
      })
    );
    expect(await response.json()).toEqual({ ok: true });
  });
});
