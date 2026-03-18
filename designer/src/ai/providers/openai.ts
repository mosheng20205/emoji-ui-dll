import type { RuntimeConfig } from '../../config/runtimeConfig';
import type { AiChatMessage, AiChatOptions, AiChatResult, AiConnectionTestResult, AiProviderClient } from '../types';
import { buildJsonHeaders, joinUrl, normalizeTextContent, parseJsonResponse } from './shared';

function buildConnectionMessage(data: unknown, model: string): string {
  const models = Array.isArray((data as { data?: unknown[] })?.data)
    ? ((data as { data: Array<{ id?: string }> }).data.map((item) => item.id).filter(Boolean) as string[])
    : [];
  const hasModel = models.includes(model);
  return hasModel ? `连接成功，模型 ${model} 可用` : `连接成功，已获取 ${models.length} 个模型`;
}

export const openAiClient: AiProviderClient = {
  async testConnection(config: RuntimeConfig): Promise<AiConnectionTestResult> {
    const startedAt = Date.now();
    const response = await fetch(joinUrl(config.aiBaseUrl, 'models'), {
      method: 'GET',
      headers: buildJsonHeaders(config),
    });
    const data = await parseJsonResponse(response);
    return {
      ok: true,
      provider: config.aiProvider,
      model: config.aiModel,
      durationMs: Date.now() - startedAt,
      statusCode: response.status,
      message: buildConnectionMessage(data, config.aiModel),
      availableModels: Array.isArray((data as { data?: unknown[] })?.data)
        ? ((data as { data: Array<{ id?: string }> }).data.map((item) => item.id).filter(Boolean) as string[])
        : [],
    };
  },

  async chat(
    config: RuntimeConfig,
    messages: AiChatMessage[],
    options?: AiChatOptions
  ): Promise<AiChatResult> {
    const response = await fetch(joinUrl(config.aiBaseUrl, 'chat/completions'), {
      method: 'POST',
      headers: buildJsonHeaders(config),
      body: JSON.stringify({
        model: config.aiModel,
        messages,
        temperature: options?.temperature ?? config.aiTemperature,
        max_tokens: options?.maxTokens,
      }),
    });
    const data = await parseJsonResponse(response);
    const firstChoice = (data as { choices?: Array<{ message?: { content?: unknown } }> }).choices?.[0];
    return {
      provider: config.aiProvider,
      model: config.aiModel,
      text: normalizeTextContent(firstChoice?.message?.content),
      raw: data,
    };
  },
};
