function isTauriRuntime(): boolean {
  if (typeof window === 'undefined') {
    return false;
  }

  return (
    '__TAURI_INTERNALS__' in window ||
    window.location.protocol === 'tauri:' ||
    window.location.hostname === 'tauri.localhost'
  );
}

export interface RuntimeFetchInit extends RequestInit {
  timeoutMs?: number;
}

export async function runtimeFetch(
  input: URL | Request | string,
  init: RuntimeFetchInit = {}
): Promise<Response> {
  const { timeoutMs, signal, ...requestInit } = init;

  const tauriRequest = async () => {
    const { fetch } = await import('@tauri-apps/plugin-http');
    return fetch(input, {
      ...requestInit,
      signal,
      connectTimeout: timeoutMs,
    });
  };

  if (isTauriRuntime()) {
    return tauriRequest();
  }

  if (!timeoutMs) {
    try {
      return await fetch(input, requestInit);
    } catch {
      return tauriRequest();
    }
  }

  const controller = new AbortController();
  const timer = window.setTimeout(() => controller.abort(), timeoutMs);

  try {
    return await fetch(input, {
      ...requestInit,
      signal: signal ?? controller.signal,
    });
  } catch {
    return tauriRequest();
  } finally {
    window.clearTimeout(timer);
  }
}
