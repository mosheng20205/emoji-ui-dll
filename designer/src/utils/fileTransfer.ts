type TauriFileAdapter = {
  save: (options: {
    defaultPath: string;
    filters: { name: string; extensions: string[] }[];
  }) => Promise<string | null>;
  open: (options: {
    multiple: boolean;
    directory: boolean;
    filters: { name: string; extensions: string[] }[];
  }) => Promise<string | string[] | null>;
  writeTextFile: (path: string, content: string) => Promise<void>;
  readTextFile: (path: string) => Promise<string>;
};

function isTauriRuntime(): boolean {
  return typeof window !== 'undefined' && '__TAURI_INTERNALS__' in window;
}

async function loadTauriFileAdapter(): Promise<TauriFileAdapter | null> {
  if (!isTauriRuntime()) {
    return null;
  }

  const [{ save, open }, { writeTextFile, readTextFile }] = await Promise.all([
    import('@tauri-apps/plugin-dialog'),
    import('@tauri-apps/plugin-fs'),
  ]);

  return { save, open, writeTextFile, readTextFile };
}

function downloadTextFileInBrowser(filename: string, content: string, mimeType = 'application/json'): void {
  const blob = new Blob([content], { type: `${mimeType};charset=utf-8` });
  const url = URL.createObjectURL(blob);
  const anchor = document.createElement('a');
  anchor.href = url;
  anchor.download = filename;
  anchor.style.display = 'none';
  document.body.appendChild(anchor);
  anchor.click();
  anchor.remove();
  window.setTimeout(() => {
    URL.revokeObjectURL(url);
  }, 0);
}

export async function downloadTextFileWithAdapter(
  filename: string,
  content: string,
  mimeType = 'application/json',
  adapter?: Pick<TauriFileAdapter, 'save' | 'writeTextFile'> | null
): Promise<void> {
  const tauri = adapter ?? await loadTauriFileAdapter();
  if (tauri) {
    const path = await tauri.save({
      defaultPath: filename,
      filters: [
        { name: 'JSON 文件', extensions: ['json'] },
      ],
    });
    if (!path) return;
    await tauri.writeTextFile(path, content);
    return;
  }

  downloadTextFileInBrowser(filename, content, mimeType);
}

export async function downloadTextFile(
  filename: string,
  content: string,
  mimeType = 'application/json'
): Promise<void> {
  await downloadTextFileWithAdapter(filename, content, mimeType);
}

function promptJsonFileInBrowser(
  onLoaded: (content: string) => void,
  onError?: (error: Error) => void
): void {
  const input = document.createElement('input');
  input.type = 'file';
  input.accept = '.json,application/json';
  input.style.display = 'none';
  input.onchange = async (event) => {
    const file = (event.target as HTMLInputElement).files?.[0];
    if (!file) return;
    try {
      const text = await file.text();
      onLoaded(text);
    } catch (error) {
      onError?.(error instanceof Error ? error : new Error('文件读取失败'));
    } finally {
      input.remove();
    }
  };
  document.body.appendChild(input);
  input.click();
}

export async function promptJsonFile(
  onLoaded: (content: string) => void,
  onError?: (error: Error) => void,
  adapter?: Pick<TauriFileAdapter, 'open' | 'readTextFile'> | null
): Promise<void> {
  const tauri = adapter ?? await loadTauriFileAdapter();
  if (tauri) {
    try {
      const result = await tauri.open({
        multiple: false,
        directory: false,
        filters: [
          { name: 'JSON 文件', extensions: ['json'] },
        ],
      });

      const path = Array.isArray(result) ? result[0] : result;
      if (!path) return;
      const text = await tauri.readTextFile(path);
      onLoaded(text);
    } catch (error) {
      onError?.(error instanceof Error ? error : new Error('文件读取失败'));
    }
    return;
  }

  promptJsonFileInBrowser(onLoaded, onError);
}
