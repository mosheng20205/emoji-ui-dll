import type { RuntimeConfig } from '../config/runtimeConfig';
import { anthropicClient } from './providers/anthropic';
import { geminiClient } from './providers/gemini';
import { openAiClient } from './providers/openai';
import { openAiCompatibleClient } from './providers/openaiCompatible';
import type { AiChatMessage, AiChatOptions, AiChatResult, AiConnectionTestResult, AiProviderClient } from './types';

function getProviderClient(config: RuntimeConfig): AiProviderClient {
  switch (config.aiProvider) {
    case 'openai':
      return openAiClient;
    case 'openai-compatible':
      return openAiCompatibleClient;
    case 'deepseek':
      return openAiCompatibleClient;
    case 'alibaba':
      return openAiCompatibleClient;
    case 'anthropic':
      return anthropicClient;
    case 'gemini':
      return geminiClient;
    default:
      return openAiCompatibleClient;
  }
}

export async function testAiConnection(config: RuntimeConfig): Promise<AiConnectionTestResult> {
  return getProviderClient(config).testConnection(config);
}

export async function chatWithAi(
  config: RuntimeConfig,
  messages: AiChatMessage[],
  options?: AiChatOptions
): Promise<AiChatResult> {
  return getProviderClient(config).chat(config, messages, options);
}
