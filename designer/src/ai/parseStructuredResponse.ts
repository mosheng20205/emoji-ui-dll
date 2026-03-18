import type { AiDesignDraft, AiOpsDraft } from './schema';
import { validateDraft } from './validateDraft';
import { validateOps } from './validateOps';

export type ParsedStructuredResponse =
  | { kind: 'draft'; data: AiDesignDraft }
  | { kind: 'ops'; data: AiOpsDraft };

function extractJsonText(text: string): string {
  const fenced = text.match(/```(?:json)?\s*([\s\S]*?)```/i);
  if (fenced?.[1]) {
    return fenced[1].trim();
  }

  const firstBrace = text.indexOf('{');
  const lastBrace = text.lastIndexOf('}');
  if (firstBrace >= 0 && lastBrace > firstBrace) {
    return text.slice(firstBrace, lastBrace + 1);
  }
  return text.trim();
}

export function parseStructuredResponse(text: string): ParsedStructuredResponse {
  const jsonText = extractJsonText(text);
  let parsed: unknown;
  try {
    parsed = JSON.parse(jsonText);
  } catch {
    throw new Error('AI 返回内容不是有效 JSON');
  }

  if (parsed && typeof parsed === 'object' && 'mode' in parsed) {
    const mode = (parsed as { mode?: unknown }).mode;
    const { mode: _mode, ...payload } = parsed as Record<string, unknown>;
    if (mode === 'draft') {
      const result = validateDraft(payload);
      if (!result.ok) {
        throw new Error(result.errors.join('；'));
      }
      return { kind: 'draft', data: result.data };
    }
    if (mode === 'ops') {
      const result = validateOps(payload);
      if (!result.ok) {
        throw new Error(result.errors.join('；'));
      }
      return { kind: 'ops', data: result.data };
    }
  }

  const draftResult = validateDraft(parsed);
  if (draftResult.ok) {
    return { kind: 'draft', data: draftResult.data };
  }

  const opsResult = validateOps(parsed);
  if (opsResult.ok) {
    return { kind: 'ops', data: opsResult.data };
  }

  throw new Error('AI 返回 JSON 不符合 Draft/Ops 协议');
}
