import type { RuntimeConfig } from '../../config/runtimeConfig';
import type { AiChatMessage, AiChatOptions, AiChatResult, AiConnectionTestResult, AiProviderClient } from '../types';
import { joinUrl, normalizeTextContent, parseJsonResponse } from './shared';

function buildAnthropicHeaders(config: RuntimeConfig): Record<string, string> {
  return {
    'Content-Type': 'application/json',
    'x-api-key': config.aiApiKey,
    'anthropic-version': '2023-06-01',
  };
}

function splitAnthropicMessages(messages: AiChatMessage[]): { system: string; messages: Array<{ role: 'user' | 'assistant'; content: string }> } {
  const system = messages
    .filter((message) => message.role === 'system')
    .map((message) => message.content)
    .join('\n\n')
    .trim();

  const chatMessages = messages
    .filter((message): message is AiChatMessage & { role: 'user' | 'assistant' } => message.role !== 'system')
    .map((message) => ({
      role: message.role,
      content: message.content,
    }));

  return { system, messages: chatMessages };
}

export const anthropicClient: AiProviderClient = {
  async testConnection(config: RuntimeConfig): Promise<AiConnectionTestResult> {
    const startedAt = Date.now();
    const payload = splitAnthropicMessages([{ role: 'user', content: 'ping' }]);
    const response = await fetch(joinUrl(config.aiBaseUrl, 'messages'), {
      method: 'POST',
      headers: buildAnthropicHeaders(config),
      body: JSON.stringify({
        model: config.aiModel,
        max_tokens: 8,
        ...payload,
      }),
    });
    await parseJsonResponse(response);
    return {
      ok: true,
      provider: config.aiProvider,
      model: config.aiModel,
      durationMs: Date.now() - startedAt,
      statusCode: response.status,
      message: `连接成功，模型 ${config.aiModel} 已响应`,
    };
  },

  async chat(
    config: RuntimeConfig,
    messages: AiChatMessage[],
    options?: AiChatOptions
  ): Promise<AiChatResult> {
    const payload = splitAnthropicMessages(messages);
    const response = await fetch(joinUrl(config.aiBaseUrl, 'messages'), {
      method: 'POST',
      headers: buildAnthropicHeaders(config),
      body: JSON.stringify({
        model: config.aiModel,
        max_tokens: options?.maxTokens ?? 4096,
        temperature: options?.temperature ?? config.aiTemperature,
        ...payload,
      }),
    });
    const data = await parseJsonResponse(response);
    const content = (data as { content?: unknown[] }).content ?? [];
    return {
      provider: config.aiProvider,
      model: config.aiModel,
      text: normalizeTextContent(content),
      raw: data,
    };
  },
};
