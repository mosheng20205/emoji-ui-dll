export interface AiPromptTemplate {
  id: string;
  label: string;
  description: string;
  prompt: string;
}

export const AI_PROMPT_TEMPLATES: AiPromptTemplate[] = [
  {
    id: 'blank',
    label: '通用空白界面',
    description: '从零开始创建普通业务窗体',
    prompt: '创建一个结构清晰、适合 Windows 桌面软件的业务界面，优先保持控件对齐和留白合理。',
  },
  {
    id: 'login-form',
    label: '登录窗口',
    description: '适合账号密码登录场景',
    prompt: '优先生成登录窗体，包含标题、用户名输入框、密码输入框、登录按钮和辅助说明。',
  },
  {
    id: 'settings-form',
    label: '设置页',
    description: '适合配置项编辑',
    prompt: '优先生成设置页，分组清晰，适合展示多个配置项和保存按钮。',
  },
  {
    id: 'data-entry',
    label: '数据录入窗体',
    description: '适合录入表单或后台工具',
    prompt: '优先生成录入型窗体，强调标签与输入控件对齐、主次按钮明显。',
  },
];

export function getPromptTemplate(templateId: string): AiPromptTemplate {
  return AI_PROMPT_TEMPLATES.find((template) => template.id === templateId) ?? AI_PROMPT_TEMPLATES[0];
}
