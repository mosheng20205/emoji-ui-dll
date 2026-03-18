import type { RuntimeConfig } from '../../config/runtimeConfig';
import type { AiChatMessage, AiChatOptions, AiChatResult, AiConnectionTestResult, AiProviderClient } from '../types';
import { joinUrl, parseJsonResponse } from './shared';

function buildGeminiUrl(config: RuntimeConfig): string {
  return `${joinUrl(config.aiBaseUrl, `models/${config.aiModel}:generateContent`)}?key=${encodeURIComponent(config.aiApiKey)}`;
}

function mapGeminiRole(role: AiChatMessage['role']): 'user' | 'model' {
  return role === 'assistant' ? 'model' : 'user';
}

function extractGeminiText(data: unknown): string {
  const candidates = (data as { candidates?: Array<{ content?: { parts?: Array<{ text?: string }> } }> }).candidates ?? [];
  return candidates
    .flatMap((candidate) => candidate.content?.parts ?? [])
    .map((part) => part.text ?? '')
    .join('\n')
    .trim();
}

export const geminiClient: AiProviderClient = {
  async testConnection(config: RuntimeConfig): Promise<AiConnectionTestResult> {
    const startedAt = Date.now();
    const response = await fetch(buildGeminiUrl(config), {
      method: 'POST',
      headers: {
        'Content-Type': 'application/json',
      },
      body: JSON.stringify({
        contents: [{ role: 'user', parts: [{ text: 'ping' }] }],
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
    const systemInstruction = messages
      .filter((message) => message.role === 'system')
      .map((message) => message.content)
      .join('\n\n')
      .trim();

    const contents = messages
      .filter((message) => message.role !== 'system')
      .map((message) => ({
        role: mapGeminiRole(message.role),
        parts: [{ text: message.content }],
      }));

    const response = await fetch(buildGeminiUrl(config), {
      method: 'POST',
      headers: {
        'Content-Type': 'application/json',
      },
      body: JSON.stringify({
        systemInstruction: systemInstruction ? { parts: [{ text: systemInstruction }] } : undefined,
        generationConfig: {
          temperature: options?.temperature ?? config.aiTemperature,
          maxOutputTokens: options?.maxTokens,
        },
        contents,
      }),
    });
    const data = await parseJsonResponse(response);
    return {
      provider: config.aiProvider,
      model: config.aiModel,
      text: extractGeminiText(data),
      raw: data,
    };
  },
};
