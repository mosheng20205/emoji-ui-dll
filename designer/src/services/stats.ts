import { DEFAULT_RUNTIME_CONFIG, type RuntimeConfig, loadRuntimeConfig } from '../config/runtimeConfig';
import { runtimeFetch } from '../utils/httpClient';

export interface UsageStats {
  launchCount: number | string;
  totalUsers: number | string;
  dailyActive: number | string;
  source: 'fallback' | 'remote';
}

export const DEFAULT_STATS: UsageStats = {
  launchCount: '待接入',
  totalUsers: '待接入',
  dailyActive: '待接入',
  source: 'fallback',
};

function normalizeStats(payload: unknown): UsageStats | null {
  const raw =
    typeof payload === 'object' &&
    payload !== null &&
    typeof (payload as { data?: { summary?: unknown } }).data?.summary === 'object'
      ? (payload as { data: { summary: Record<string, unknown> } }).data.summary
      : typeof payload === 'object' && payload !== null && typeof (payload as { data?: unknown }).data === 'object'
        ? (payload as { data: Record<string, unknown> }).data
      : payload;

  if (typeof raw !== 'object' || raw === null) {
    return null;
  }

  const launchCount = (raw as { launchCount?: unknown }).launchCount;
  const totalUsers = (raw as { totalUsers?: unknown }).totalUsers;
  const dailyActive = (raw as { dailyActive?: unknown }).dailyActive;

  if (
    (typeof launchCount === 'number' || typeof launchCount === 'string') &&
    (typeof totalUsers === 'number' || typeof totalUsers === 'string') &&
    (typeof dailyActive === 'number' || typeof dailyActive === 'string')
  ) {
    return {
      launchCount,
      totalUsers,
      dailyActive,
      source: 'remote',
    };
  }

  return null;
}

export async function fetchStats(config: RuntimeConfig = loadRuntimeConfig()): Promise<UsageStats> {
  const runtime = { ...DEFAULT_RUNTIME_CONFIG, ...config };
  if (!runtime.statsApiUrl) {
    return DEFAULT_STATS;
  }

  try {
    const response = await runtimeFetch(runtime.statsApiUrl, {
      method: 'GET',
      timeoutMs: runtime.requestTimeoutMs,
    });
    if (!response.ok) {
      return DEFAULT_STATS;
    }
    const data = await response.json();
    return normalizeStats(data) ?? DEFAULT_STATS;
  } catch {
    return DEFAULT_STATS;
  }
}
