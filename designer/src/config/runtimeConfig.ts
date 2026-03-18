export type AiProvider = 'openai' | 'anthropic' | 'gemini' | 'openai-compatible' | 'deepseek' | 'alibaba';

export interface RuntimeConfig {
  adApiUrl: string;
  statsApiUrl: string;
  reportLaunchApiUrl: string;
  reportActiveApiUrl: string;
  reportAdEventApiUrl: string;
  requestTimeoutMs: number;
  emojiHotkey: string;
  quickInsertHotkey: string;
  quickInsertControl: string;
  aiProvider: AiProvider;
  aiBaseUrl: string;
  aiApiKey: string;
  aiModel: string;
  aiSystemPrompt: string;
  aiPromptTemplateId: string;
  aiRequestTimeoutMs: number;
  aiTemperature: number;
  aiHistoryFileName: string;
  aiEnableEventGeneration: boolean;
  aiEnableAdvancedNaming: boolean;
}

export const RUNTIME_CONFIG_STORAGE_KEY = 'designer_runtime_config';

export const DEFAULT_RUNTIME_CONFIG: RuntimeConfig = {
  adApiUrl: '',
  statsApiUrl: '',
  reportLaunchApiUrl: '',
  reportActiveApiUrl: '',
  reportAdEventApiUrl: '',
  requestTimeoutMs: 5000,
  emojiHotkey: 'Ctrl+Shift+E',
  quickInsertHotkey: 'Ctrl+Shift+I',
  quickInsertControl: 'button',
  aiProvider: 'openai-compatible',
  aiBaseUrl: 'https://api.openai.com/v1',
  aiApiKey: '',
  aiModel: 'gpt-4.1-mini',
  aiSystemPrompt: '',
  aiPromptTemplateId: 'blank',
  aiRequestTimeoutMs: 15000,
  aiTemperature: 0.2,
  aiHistoryFileName: 'ai-designer-history.json',
  aiEnableEventGeneration: true,
  aiEnableAdvancedNaming: true,
};

type RuntimeEnv = Record<string, string | undefined>;

function parsePositiveNumber(value: string | undefined, fallback: number): number {
  const num = Number(value);
  return Number.isFinite(num) && num > 0 ? num : fallback;
}

function parseBoolean(value: string | undefined, fallback: boolean): boolean {
  if (value === undefined) return fallback;
  return ['1', 'true', 'yes', 'on'].includes(value.trim().toLowerCase());
}

export function resolveEnvRuntimeConfig(env: RuntimeEnv): RuntimeConfig {
  return {
    adApiUrl: env.VITE_AD_API_URL || DEFAULT_RUNTIME_CONFIG.adApiUrl,
    statsApiUrl: env.VITE_STATS_API_URL || DEFAULT_RUNTIME_CONFIG.statsApiUrl,
    reportLaunchApiUrl: env.VITE_REPORT_LAUNCH_API_URL || DEFAULT_RUNTIME_CONFIG.reportLaunchApiUrl,
    reportActiveApiUrl: env.VITE_REPORT_ACTIVE_API_URL || DEFAULT_RUNTIME_CONFIG.reportActiveApiUrl,
    reportAdEventApiUrl: env.VITE_REPORT_AD_EVENT_API_URL || DEFAULT_RUNTIME_CONFIG.reportAdEventApiUrl,
    requestTimeoutMs: parsePositiveNumber(env.VITE_REQUEST_TIMEOUT_MS, DEFAULT_RUNTIME_CONFIG.requestTimeoutMs),
    emojiHotkey: env.VITE_EMOJI_HOTKEY || DEFAULT_RUNTIME_CONFIG.emojiHotkey,
    quickInsertHotkey: env.VITE_QUICK_INSERT_HOTKEY || DEFAULT_RUNTIME_CONFIG.quickInsertHotkey,
    quickInsertControl: env.VITE_QUICK_INSERT_CONTROL || DEFAULT_RUNTIME_CONFIG.quickInsertControl,
    aiProvider: (env.VITE_AI_PROVIDER as AiProvider | undefined) || DEFAULT_RUNTIME_CONFIG.aiProvider,
    aiBaseUrl: env.VITE_AI_BASE_URL || DEFAULT_RUNTIME_CONFIG.aiBaseUrl,
    aiApiKey: env.VITE_AI_API_KEY || DEFAULT_RUNTIME_CONFIG.aiApiKey,
    aiModel: env.VITE_AI_MODEL || DEFAULT_RUNTIME_CONFIG.aiModel,
    aiSystemPrompt: env.VITE_AI_SYSTEM_PROMPT || DEFAULT_RUNTIME_CONFIG.aiSystemPrompt,
    aiPromptTemplateId: env.VITE_AI_PROMPT_TEMPLATE_ID || DEFAULT_RUNTIME_CONFIG.aiPromptTemplateId,
    aiRequestTimeoutMs: parsePositiveNumber(env.VITE_AI_REQUEST_TIMEOUT_MS, DEFAULT_RUNTIME_CONFIG.aiRequestTimeoutMs),
    aiTemperature: Number.isFinite(Number(env.VITE_AI_TEMPERATURE))
      ? Number(env.VITE_AI_TEMPERATURE)
      : DEFAULT_RUNTIME_CONFIG.aiTemperature,
    aiHistoryFileName: env.VITE_AI_HISTORY_FILE_NAME || DEFAULT_RUNTIME_CONFIG.aiHistoryFileName,
    aiEnableEventGeneration: parseBoolean(
      env.VITE_AI_ENABLE_EVENT_GENERATION,
      DEFAULT_RUNTIME_CONFIG.aiEnableEventGeneration
    ),
    aiEnableAdvancedNaming: parseBoolean(
      env.VITE_AI_ENABLE_ADVANCED_NAMING,
      DEFAULT_RUNTIME_CONFIG.aiEnableAdvancedNaming
    ),
  };
}

export function loadRuntimeConfig(env?: RuntimeEnv): RuntimeConfig {
  const baseConfig = resolveEnvRuntimeConfig(env ?? (import.meta.env as RuntimeEnv));

  try {
    const raw = localStorage.getItem(RUNTIME_CONFIG_STORAGE_KEY);
    if (!raw) return baseConfig;
    const parsed = JSON.parse(raw) as Partial<RuntimeConfig>;
    const merged = { ...baseConfig } as Record<string, unknown>;
    for (const [key, value] of Object.entries(parsed)) {
      if (value === '' && merged[key] !== '') {
        continue;
      }
      merged[key] = value;
    }
    return merged as unknown as RuntimeConfig;
  } catch {
    return baseConfig;
  }
}

export function saveRuntimeConfig(config: Partial<RuntimeConfig>) {
  const next = {
    ...loadRuntimeConfig(),
    ...config,
  };
  localStorage.setItem(RUNTIME_CONFIG_STORAGE_KEY, JSON.stringify(next));
}
