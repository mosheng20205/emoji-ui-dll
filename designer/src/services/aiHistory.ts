import type { RuntimeConfig } from '../config/runtimeConfig';
import type { AiDesignDraft, AiOpsDraft } from '../ai/schema';
import type { AiChatMessage } from '../ai/types';

export interface AiHistorySession {
  sessionId: string;
  title: string;
  createdAt: string;
  updatedAt: string;
  messages: AiChatMessage[];
  rawResponse: string;
  draftPreview: AiDesignDraft | null;
  opsPreview: AiOpsDraft | null;
}

export interface AiHistoryIndexItem {
  sessionId: string;
  title: string;
  updatedAt: string;
  fileName: string;
}

interface HistoryFsAdapter {
  mkdir: (path: string, options: { baseDir: number; recursive?: boolean }) => Promise<void>;
  writeTextFile: (path: string, content: string, options: { baseDir: number }) => Promise<void>;
  readTextFile: (path: string, options: { baseDir: number }) => Promise<string>;
  exists: (path: string, options: { baseDir: number }) => Promise<boolean>;
  baseDir: number;
}

const STORAGE_PREFIX = 'designer_ai_history';

function getHistoryBaseName(fileName: string): string {
  return (fileName || 'ai-designer-history.json').replace(/\.json$/i, '');
}

function buildHistoryPaths(fileName: string, sessionId?: string) {
  const baseName = getHistoryBaseName(fileName);
  const dir = `${baseName}-sessions`;
  return {
    dir,
    index: `${baseName}.json`,
    session: sessionId ? `${dir}/${sessionId}.json` : '',
  };
}

async function loadFsAdapter(): Promise<HistoryFsAdapter | null> {
  if (typeof window === 'undefined' || !('__TAURI_INTERNALS__' in window)) {
    return null;
  }

  const [{ mkdir, writeTextFile, readTextFile, exists, BaseDirectory }] = await Promise.all([
    import('@tauri-apps/plugin-fs'),
  ]);

  return {
    mkdir,
    writeTextFile,
    readTextFile,
    exists,
    baseDir: BaseDirectory.AppLocalData,
  };
}

function localStorageKey(config: RuntimeConfig, suffix: string): string {
  return `${STORAGE_PREFIX}:${getHistoryBaseName(config.aiHistoryFileName)}:${suffix}`;
}

async function readIndex(config: RuntimeConfig, adapter?: HistoryFsAdapter | null): Promise<AiHistoryIndexItem[]> {
  const fs = adapter ?? await loadFsAdapter();
  const { index } = buildHistoryPaths(config.aiHistoryFileName);
  if (fs) {
    if (!(await fs.exists(index, { baseDir: fs.baseDir }))) {
      return [];
    }
    const text = await fs.readTextFile(index, { baseDir: fs.baseDir });
    return JSON.parse(text) as AiHistoryIndexItem[];
  }

  const raw = localStorage.getItem(localStorageKey(config, 'index'));
  return raw ? (JSON.parse(raw) as AiHistoryIndexItem[]) : [];
}

async function writeIndex(config: RuntimeConfig, items: AiHistoryIndexItem[], adapter?: HistoryFsAdapter | null): Promise<void> {
  const fs = adapter ?? await loadFsAdapter();
  const { index } = buildHistoryPaths(config.aiHistoryFileName);
  if (fs) {
    await fs.writeTextFile(index, JSON.stringify(items, null, 2), { baseDir: fs.baseDir });
    return;
  }
  localStorage.setItem(localStorageKey(config, 'index'), JSON.stringify(items));
}

export async function saveAiHistorySession(
  config: RuntimeConfig,
  session: AiHistorySession,
  adapter?: HistoryFsAdapter | null
): Promise<void> {
  const fs = adapter ?? await loadFsAdapter();
  const paths = buildHistoryPaths(config.aiHistoryFileName, session.sessionId);
  if (fs) {
    await fs.mkdir(paths.dir, { baseDir: fs.baseDir, recursive: true });
    await fs.writeTextFile(paths.session, JSON.stringify(session, null, 2), { baseDir: fs.baseDir });
  } else {
    localStorage.setItem(localStorageKey(config, session.sessionId), JSON.stringify(session));
  }

  const index = await readIndex(config, fs);
  const nextItem: AiHistoryIndexItem = {
    sessionId: session.sessionId,
    title: session.title,
    updatedAt: session.updatedAt,
    fileName: `${session.sessionId}.json`,
  };
  const nextIndex = [nextItem, ...index.filter((item) => item.sessionId !== session.sessionId)]
    .sort((a, b) => b.updatedAt.localeCompare(a.updatedAt));
  await writeIndex(config, nextIndex, fs);
}

export async function listAiHistorySessions(
  config: RuntimeConfig,
  adapter?: HistoryFsAdapter | null
): Promise<AiHistoryIndexItem[]> {
  return readIndex(config, adapter);
}

export async function loadAiHistorySession(
  config: RuntimeConfig,
  sessionId: string,
  adapter?: HistoryFsAdapter | null
): Promise<AiHistorySession | null> {
  const fs = adapter ?? await loadFsAdapter();
  const paths = buildHistoryPaths(config.aiHistoryFileName, sessionId);
  if (fs) {
    if (!(await fs.exists(paths.session, { baseDir: fs.baseDir }))) {
      return null;
    }
    const text = await fs.readTextFile(paths.session, { baseDir: fs.baseDir });
    return JSON.parse(text) as AiHistorySession;
  }

  const raw = localStorage.getItem(localStorageKey(config, sessionId));
  return raw ? (JSON.parse(raw) as AiHistorySession) : null;
}
