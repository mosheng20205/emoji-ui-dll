import type { AiProvider, RuntimeConfig } from '../config/runtimeConfig';

export type AiChatRole = 'system' | 'user' | 'assistant';

export interface AiChatMessage {
  role: AiChatRole;
  content: string;
}

export interface AiChatOptions {
  temperature?: number;
  maxTokens?: number;
}

export interface AiChatResult {
  provider: AiProvider;
  model: string;
  text: string;
  raw: unknown;
}

export interface AiConnectionTestResult {
  ok: boolean;
  provider: AiProvider;
  model: string;
  durationMs: number;
  statusCode?: number;
  message: string;
  availableModels?: string[];
}

export interface AiProviderClient {
  testConnection(config: RuntimeConfig): Promise<AiConnectionTestResult>;
  chat(config: RuntimeConfig, messages: AiChatMessage[], options?: AiChatOptions): Promise<AiChatResult>;
}
