import { controlDefinitions } from '../data/controlDefs';
import type { RuntimeConfig } from '../config/runtimeConfig';
import type { DesignControl, DesignWindow } from '../types/controls';
import type { AiChatMessage } from './types';
import { getPromptTemplate } from './promptTemplates';

export type AiGenerationMode = 'draft' | 'ops';

interface BuildPromptOptions {
  mode: AiGenerationMode;
  userPrompt: string;
  templateId: string;
  customSystemPrompt: string;
  previousMessages: AiChatMessage[];
  window: DesignWindow;
  controls: DesignControl[];
  runtimeConfig: RuntimeConfig;
}

export function buildPrompt(options: BuildPromptOptions): AiChatMessage[] {
  const template = getPromptTemplate(options.templateId);
  const supportedControls = controlDefinitions.map((control) => `${control.type}(${control.label})`).join(', ');
  const protocolDescription = options.mode === 'draft'
    ? `返回 JSON，格式必须为 {"mode":"draft","summary":"...","warnings":[],"window":{...},"controls":[...] }`
    : `返回 JSON，格式必须为 {"mode":"ops","summary":"...","warnings":[],"affectedControls":[],"operations":[...] }`;

  const systemPrompt = [
    '你是 Windows 可视化设计器的 UI 助手。',
    `支持的控件类型: ${supportedControls}`,
    '输出只能是 JSON，不要输出 Markdown，不要解释，不要带代码块说明文字。',
    '坐标和尺寸必须是数字，颜色使用 #RRGGBB。',
    options.runtimeConfig.aiEnableEventGeneration
      ? '允许为适合的控件生成 onClick、onChange、onSelect 等事件占位。'
      : '不要生成事件占位。',
    options.runtimeConfig.aiEnableAdvancedNaming
      ? '控件命名请尽量语义化、稳定、避免重复。'
      : '控件命名可以使用简单默认名称。',
    `当前任务模式: ${options.mode}`,
    protocolDescription,
    `模板倾向: ${template.prompt}`,
    options.customSystemPrompt.trim(),
  ]
    .filter(Boolean)
    .join('\n');

  const contextPayload = {
    currentWindow: options.window,
    currentControls: options.controls,
    template: {
      id: template.id,
      label: template.label,
      description: template.description,
    },
    userRequest: options.userPrompt,
  };

  return [
    { role: 'system', content: systemPrompt },
    ...options.previousMessages.filter((message) => message.role !== 'system'),
    {
      role: 'user',
      content: JSON.stringify(contextPayload, null, 2),
    },
  ];
}
