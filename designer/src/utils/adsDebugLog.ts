async function appendTauriLog(message: string): Promise<void> {
  if (typeof window === 'undefined') {
    return;
  }

  const isTauri =
    '__TAURI_INTERNALS__' in window ||
    window.location.protocol === 'tauri:' ||
    window.location.hostname === 'tauri.localhost';

  if (!isTauri) {
    return;
  }

  try {
    const [{ writeTextFile, readTextFile, exists, BaseDirectory }] = await Promise.all([
      import('@tauri-apps/plugin-fs'),
    ]);
    const path = 'ads-debug.log';
    const prefix = new Date().toISOString();
    const nextLine = `[${prefix}] ${message}\n`;
    const hasFile = await exists(path, { baseDir: BaseDirectory.AppLocalData });
    const current = hasFile ? await readTextFile(path, { baseDir: BaseDirectory.AppLocalData }) : '';
    await writeTextFile(path, `${current}${nextLine}`, { baseDir: BaseDirectory.AppLocalData });
  } catch {
    // 调试日志失败时静默忽略
  }
}

export function logAdsDebug(message: string): void {
  void appendTauriLog(message);
}
