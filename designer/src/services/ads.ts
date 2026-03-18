import { DEFAULT_RUNTIME_CONFIG, type RuntimeConfig, loadRuntimeConfig } from '../config/runtimeConfig';
import { logAdsDebug } from '../utils/adsDebugLog';
import { runtimeFetch } from '../utils/httpClient';

export interface AdItem {
  id: string;
  text: string;
  url: string;
}

export const DEFAULT_ADS: AdItem[] = [
  {
    id: 'default-ad',
    text: '广告位招租(QQ1098901025)',
    url: 'https://qm.qq.com/q/NIZipXNsUS',
  },
];

function normalizeAds(payload: unknown): AdItem[] {
  const list = Array.isArray(payload)
    ? payload
    : Array.isArray((payload as { ads?: unknown[] })?.ads)
      ? (payload as { ads: unknown[] }).ads
      : Array.isArray((payload as { data?: unknown[] })?.data)
        ? (payload as { data: unknown[] }).data
        : Array.isArray((payload as { data?: { items?: unknown[] } })?.data?.items)
          ? (payload as { data: { items: unknown[] } }).data.items
        : [];

  return list
    .map((item, index) => {
      if (
        typeof item === 'object' &&
        item !== null &&
        typeof (item as { url?: unknown }).url === 'string'
      ) {
        const text =
          typeof (item as { text?: unknown }).text === 'string'
            ? (item as { text: string }).text
            : typeof (item as { title?: unknown }).title === 'string'
              ? (item as { title: string }).title
              : typeof (item as { description?: unknown }).description === 'string'
                ? (item as { description: string }).description
                : null;
        if (!text) {
          return null;
        }
        return {
          id: typeof (item as { id?: unknown }).id === 'string'
            ? (item as { id: string }).id
            : typeof (item as { id?: unknown }).id === 'number'
              ? String((item as { id: number }).id)
              : `ad-${index + 1}`,
          text,
          url: (item as { url: string }).url,
        };
      }
      return null;
    })
    .filter((item): item is AdItem => item !== null);
}

export async function fetchAds(config: RuntimeConfig = loadRuntimeConfig()): Promise<AdItem[]> {
  const runtime = { ...DEFAULT_RUNTIME_CONFIG, ...config };
  logAdsDebug(`fetchAds:start url=${runtime.adApiUrl || '<empty>'} timeout=${runtime.requestTimeoutMs}`);
  if (!runtime.adApiUrl) {
    logAdsDebug('fetchAds:fallback no adApiUrl configured');
    return DEFAULT_ADS;
  }

  try {
    const response = await runtimeFetch(runtime.adApiUrl, {
      method: 'GET',
      timeoutMs: runtime.requestTimeoutMs,
    });
    logAdsDebug(`fetchAds:response status=${response.status} ok=${response.ok}`);
    if (!response.ok) {
      logAdsDebug('fetchAds:fallback response not ok');
      return DEFAULT_ADS;
    }
    const data = await response.json();
    const ads = normalizeAds(data);
    logAdsDebug(`fetchAds:normalized count=${ads.length} ids=${ads.map((ad) => ad.id).join(',') || '<none>'}`);
    return ads.length > 0 ? ads : DEFAULT_ADS;
  } catch (error) {
    logAdsDebug(`fetchAds:error ${(error as Error)?.message || 'unknown error'}`);
    return DEFAULT_ADS;
  }
}
