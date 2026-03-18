import { loadRuntimeConfig, type RuntimeConfig } from '../config/runtimeConfig';
import { runtimeFetch } from '../utils/httpClient';
import type { AdItem } from './ads';

export const STATS_CLIENT_ID_STORAGE_KEY = 'designer_stats_client_id';
export const STATS_ACTIVE_DATE_STORAGE_KEY = 'designer_stats_active_date';

export type AdEventType = 'impression' | 'click';

function createClientId(): string {
  if (typeof crypto !== 'undefined' && typeof crypto.randomUUID === 'function') {
    return crypto.randomUUID();
  }
  return `client-${Date.now()}-${Math.random().toString(36).slice(2, 10)}`;
}

export function getOrCreateClientId(storage: Storage = localStorage): string {
  const existing = storage.getItem(STATS_CLIENT_ID_STORAGE_KEY);
  if (existing) {
    return existing;
  }
  const next = createClientId();
  storage.setItem(STATS_CLIENT_ID_STORAGE_KEY, next);
  return next;
}

function formatLocalDateKey(now: Date): string {
  const year = now.getFullYear();
  const month = String(now.getMonth() + 1).padStart(2, '0');
  const day = String(now.getDate()).padStart(2, '0');
  return `${year}-${month}-${day}`;
}

async function postJson(
  url: string,
  payload: Record<string, unknown>,
  timeoutMs: number,
  fetchImpl?: typeof fetch
): Promise<void> {
  const init: RequestInit = {
    method: 'POST',
    headers: {
      'Content-Type': 'application/json',
    },
    body: JSON.stringify(payload),
  };

  if (fetchImpl) {
    await fetchImpl(url, init);
    return;
  }

  await runtimeFetch(url, {
    ...init,
    timeoutMs,
  });
}

function resolveNumericAdId(id: string): number | null {
  return /^\d+$/.test(id) ? Number(id) : null;
}

export async function reportLaunch(
  config: RuntimeConfig = loadRuntimeConfig(),
  fetchImpl?: typeof fetch
): Promise<void> {
  if (!config.reportLaunchApiUrl) {
    return;
  }

  const clientId = getOrCreateClientId();
  const reportUrl = config.reportLaunchApiUrl;

  try {
    await postJson(
      reportUrl,
      {
        clientId,
        source: 'emoji-window-designer',
        event: 'launch',
        occurredAt: new Date().toISOString(),
      },
      config.requestTimeoutMs,
      fetchImpl
    );
  } catch {
    // 上报失败时静默忽略，不阻塞设计器启动
  }
}

export async function reportActive(
  config: RuntimeConfig = loadRuntimeConfig(),
  fetchImpl?: typeof fetch,
  storage: Storage = localStorage,
  now: Date = new Date()
): Promise<boolean> {
  if (!config.reportActiveApiUrl) {
    return true;
  }

  const currentDate = formatLocalDateKey(now);
  if (storage.getItem(STATS_ACTIVE_DATE_STORAGE_KEY) === currentDate) {
    return true;
  }

  try {
    await postJson(
      config.reportActiveApiUrl,
      {
        clientId: getOrCreateClientId(storage),
        source: 'emoji-window-designer',
        event: 'active',
        occurredAt: now.toISOString(),
      },
      config.requestTimeoutMs,
      fetchImpl
    );
    storage.setItem(STATS_ACTIVE_DATE_STORAGE_KEY, currentDate);
    return true;
  } catch {
    // 上报失败时静默忽略，由下次真实活跃再尝试
    return false;
  }
}

export async function reportAdEvent(
  eventType: AdEventType,
  ad: Pick<AdItem, 'id' | 'text' | 'url'>,
  config: RuntimeConfig = loadRuntimeConfig(),
  fetchImpl?: typeof fetch,
  storage: Storage = localStorage,
  now: Date = new Date()
): Promise<void> {
  if (!config.reportAdEventApiUrl) {
    return;
  }

  const adId = resolveNumericAdId(ad.id);
  if (adId === null) {
    return;
  }

  try {
    await postJson(
      config.reportAdEventApiUrl,
      {
        adId,
        clientId: getOrCreateClientId(storage),
        source: 'emoji-window-designer',
        eventType,
        occurredAt: now.toISOString(),
      },
      config.requestTimeoutMs,
      fetchImpl
    );
  } catch {
    // 广告上报失败时静默忽略，不影响正常使用
  }
}
