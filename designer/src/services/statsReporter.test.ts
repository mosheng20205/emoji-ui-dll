import { DEFAULT_RUNTIME_CONFIG } from '../config/runtimeConfig';
import {
  getOrCreateClientId,
  reportActive,
  reportAdEvent,
  reportLaunch,
  STATS_ACTIVE_DATE_STORAGE_KEY,
  STATS_CLIENT_ID_STORAGE_KEY,
} from './statsReporter';

describe('statsReporter', () => {
  beforeEach(() => {
    localStorage.clear();
    vi.restoreAllMocks();
  });

  afterEach(() => {
    vi.useRealTimers();
  });

  test('应在本地持久化 client_id', () => {
    const first = getOrCreateClientId();
    const second = getOrCreateClientId();

    expect(first).toBeTruthy();
    expect(second).toBe(first);
    expect(localStorage.getItem(STATS_CLIENT_ID_STORAGE_KEY)).toBe(first);
  });

  test('应使用 runtimeConfig 中的 report_launch 地址上报', async () => {
    const fetchMock = vi.fn().mockResolvedValue({ ok: true });

    await reportLaunch(
      {
        ...DEFAULT_RUNTIME_CONFIG,
        reportLaunchApiUrl: 'http://8.134.23.223:3003/api/report_launch.php',
      },
      fetchMock
    );

    expect(fetchMock).toHaveBeenCalledWith(
      'http://8.134.23.223:3003/api/report_launch.php',
      expect.objectContaining({
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: expect.any(String),
      })
    );

    const [, init] = fetchMock.mock.calls[0];
    expect(JSON.parse(init.body as string)).toEqual(
      expect.objectContaining({
        clientId: expect.any(String),
        source: 'emoji-window-designer',
        event: 'launch',
      })
    );
  });

  test('未配置 report_launch 地址时应跳过上报', async () => {
    const fetchMock = vi.fn();

    await reportLaunch(
      {
        ...DEFAULT_RUNTIME_CONFIG,
        reportLaunchApiUrl: '',
      },
      fetchMock
    );

    expect(fetchMock).not.toHaveBeenCalled();
  });

  test('应在首次真实活跃时使用 report_active 地址上报，并按天去重', async () => {
    vi.useFakeTimers();
    vi.setSystemTime(new Date('2026-03-19T08:00:00.000Z'));
    const fetchMock = vi.fn().mockResolvedValue({ ok: true });

    await reportActive(
      {
        ...DEFAULT_RUNTIME_CONFIG,
        reportActiveApiUrl: 'http://8.134.23.223:3003/api/report_active.php',
      },
      fetchMock
    );

    await reportActive(
      {
        ...DEFAULT_RUNTIME_CONFIG,
        reportActiveApiUrl: 'http://8.134.23.223:3003/api/report_active.php',
      },
      fetchMock
    );

    expect(fetchMock).toHaveBeenCalledTimes(1);
    expect(fetchMock).toHaveBeenCalledWith(
      'http://8.134.23.223:3003/api/report_active.php',
      expect.objectContaining({
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: expect.any(String),
      })
    );
    expect(localStorage.getItem(STATS_ACTIVE_DATE_STORAGE_KEY)).toBe('2026-03-19');

    const [, init] = fetchMock.mock.calls[0];
    expect(JSON.parse(init.body as string)).toEqual(
      expect.objectContaining({
        clientId: expect.any(String),
        source: 'emoji-window-designer',
        event: 'active',
      })
    );
  });

  test('应使用 report_ad_event 地址上报广告曝光和点击事件', async () => {
    const fetchMock = vi.fn().mockResolvedValue({ ok: true });

    await reportAdEvent(
      'impression',
      { id: '7', text: '广告位', url: 'https://example.com/ad' },
      {
        ...DEFAULT_RUNTIME_CONFIG,
        reportAdEventApiUrl: 'http://8.134.23.223:3003/api/report_ad_event.php',
      },
      fetchMock
    );

    await reportAdEvent(
      'click',
      { id: '7', text: '广告位', url: 'https://example.com/ad' },
      {
        ...DEFAULT_RUNTIME_CONFIG,
        reportAdEventApiUrl: 'http://8.134.23.223:3003/api/report_ad_event.php',
      },
      fetchMock
    );

    expect(fetchMock).toHaveBeenNthCalledWith(
      1,
      'http://8.134.23.223:3003/api/report_ad_event.php',
      expect.objectContaining({
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: expect.any(String),
      })
    );
    expect(fetchMock).toHaveBeenNthCalledWith(
      2,
      'http://8.134.23.223:3003/api/report_ad_event.php',
      expect.objectContaining({
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: expect.any(String),
      })
    );

    const [, firstInit] = fetchMock.mock.calls[0];
    const [, secondInit] = fetchMock.mock.calls[1];
    expect(JSON.parse(firstInit.body as string)).toEqual(
      expect.objectContaining({
        adId: 7,
        clientId: expect.any(String),
        source: 'emoji-window-designer',
        eventType: 'impression',
      })
    );
    expect(JSON.parse(secondInit.body as string)).toEqual(
      expect.objectContaining({
        adId: 7,
        clientId: expect.any(String),
        source: 'emoji-window-designer',
        eventType: 'click',
      })
    );
  });

  test('默认兜底广告没有数字 id 时不应上报广告事件', async () => {
    const fetchMock = vi.fn();

    await reportAdEvent(
      'impression',
      { id: 'default-ad', text: '广告位招租', url: 'https://example.com/default' },
      {
        ...DEFAULT_RUNTIME_CONFIG,
        reportAdEventApiUrl: 'http://8.134.23.223:3003/api/report_ad_event.php',
      },
      fetchMock
    );

    expect(fetchMock).not.toHaveBeenCalled();
  });
});
