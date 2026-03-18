import { fetchStats, DEFAULT_STATS } from './stats';
import { DEFAULT_RUNTIME_CONFIG } from '../config/runtimeConfig';

describe('stats service', () => {
  test('未配置 API 地址时应返回默认统计占位数据', async () => {
    const stats = await fetchStats({
      ...DEFAULT_RUNTIME_CONFIG,
      statsApiUrl: '',
      requestTimeoutMs: 3000,
    });

    expect(stats).toEqual(DEFAULT_STATS);
  });

  test('应兼容后端 data.summary 统计结构', async () => {
    vi.stubGlobal('fetch', vi.fn().mockResolvedValue({
      ok: true,
      json: async () => ({
        code: 0,
        message: 'ok',
        data: {
          summary: {
            metricDate: '2026-03-19',
            dailyActive: 2910,
            launchCount: 4720,
            totalUsers: 20480,
          },
          trend: [],
        },
      }),
    }));

    const stats = await fetchStats({
      ...DEFAULT_RUNTIME_CONFIG,
      statsApiUrl: 'http://8.134.23.223:3003/api/stats.php',
      requestTimeoutMs: 3000,
    });

    expect(stats).toEqual({
      dailyActive: 2910,
      launchCount: 4720,
      totalUsers: 20480,
      source: 'remote',
    });
  });
});
