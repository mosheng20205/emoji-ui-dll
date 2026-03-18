import type { RuntimeConfig } from '../../config/runtimeConfig';

export class AiHttpError extends Error {
  statusCode: number;

  constructor(statusCode: number, message: string) {
    super(message);
    this.statusCode = statusCode;
  }
}

export function joinUrl(baseUrl: string, path: string): string {
  return `${baseUrl.replace(/\/+$/, '')}/${path.replace(/^\/+/, '')}`;
}

export function buildJsonHeaders(config: RuntimeConfig, extra?: Record<string, string>): Record<string, string> {
  return {
    'Content-Type': 'application/json',
    ...extra,
    ...(config.aiApiKey ? { Authorization: `Bearer ${config.aiApiKey}` } : {}),
  };
}

export async function parseJsonResponse(response: Response): Promise<unknown> {
  const text = await response.text();
  let data: unknown = {};

  if (text) {
    try {
      data = JSON.parse(text);
    } catch {
      data = { error: { message: text } };
    }
  }

  if (!response.ok) {
    const message = extractErrorMessage(data) || `AI 请求失败 (${response.status})`;
    throw new AiHttpError(response.status, message);
  }

  return data;
}

export function extractErrorMessage(data: unknown): string {
  if (!data || typeof data !== 'object') return '';
  if ('error' in data) {
    const errorValue = (data as { error?: unknown }).error;
    if (typeof errorValue === 'string') return errorValue;
    if (errorValue && typeof errorValue === 'object' && 'message' in errorValue) {
      const message = (errorValue as { message?: unknown }).message;
      if (typeof message === 'string') return message;
    }
  }
  if ('message' in data && typeof (data as { message?: unknown }).message === 'string') {
    return (data as { message: string }).message;
  }
  return '';
}

export function normalizeTextContent(content: unknown): string {
  if (typeof content === 'string') {
    return content;
  }
  if (Array.isArray(content)) {
    return content
      .map((item) => {
        if (typeof item === 'string') return item;
        if (item && typeof item === 'object' && 'text' in item) {
          const text = (item as { text?: unknown }).text;
          return typeof text === 'string' ? text : '';
        }
        return '';
      })
      .join('\n')
      .trim();
  }
  return '';
}
