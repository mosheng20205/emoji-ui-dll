import type { RuntimeConfig } from '../config/runtimeConfig';
import { AI_PROMPT_TEMPLATES } from '../ai/promptTemplates';

export type AiViewMode = 'auto' | 'draft' | 'ops';
export type AiPromptTab = 'template' | 'system';

interface AiPromptPanelProps {
  runtimeConfig: RuntimeConfig;
  mode: AiViewMode;
  templateId: string;
  customSystemPrompt: string;
  activeTab: AiPromptTab;
  collapsed: boolean;
  onModeChange: (mode: AiViewMode) => void;
  onTabChange: (tab: AiPromptTab) => void;
  onTemplateChange: (templateId: string) => void;
  onCustomSystemPromptChange: (value: string) => void;
  onToggleCollapse: () => void;
}

export default function AiPromptPanel({
  runtimeConfig,
  mode,
  templateId,
  customSystemPrompt,
  activeTab,
  collapsed,
  onModeChange,
  onTabChange,
  onTemplateChange,
  onCustomSystemPromptChange,
  onToggleCollapse,
}: AiPromptPanelProps) {
  return (
    <section className="ai-panel ai-prompt-panel">
      <div className="ai-panel-header">
        <h2>提示词与配置</h2>
        <button className="header-btn" onClick={onToggleCollapse}>
          {collapsed ? '展开提示词与配置' : '折叠提示词与配置'}
        </button>
      </div>

      <div className="ai-config-summary">
        <div><span>供应商</span><strong>{runtimeConfig.aiProvider}</strong></div>
        <div><span>模型</span><strong>{runtimeConfig.aiModel}</strong></div>
        <div><span>事件生成</span><strong>{runtimeConfig.aiEnableEventGeneration ? '开启' : '关闭'}</strong></div>
        <div><span>高级命名</span><strong>{runtimeConfig.aiEnableAdvancedNaming ? '开启' : '关闭'}</strong></div>
      </div>

      {!collapsed && (
        <>
      <div className="ai-tablist" role="tablist" aria-label="AI 提示词配置标签页">
        <button
          type="button"
          role="tab"
          aria-selected={activeTab === 'template'}
          className={`ai-tab-btn ${activeTab === 'template' ? 'active' : ''}`}
          onClick={() => onTabChange('template')}
        >
          提示词模板
        </button>
        <button
          type="button"
          role="tab"
          aria-selected={activeTab === 'system'}
          className={`ai-tab-btn ${activeTab === 'system' ? 'active' : ''}`}
          onClick={() => onTabChange('system')}
        >
          系统提示词
        </button>
      </div>

      <div className="ai-tab-panel" role="tabpanel">
        <div className="settings-grid">
          <label className="settings-field">
            <span>生成模式</span>
            <select aria-label="生成模式" value={mode} onChange={(event) => onModeChange(event.target.value as AiViewMode)}>
              <option value="auto">自动判断</option>
              <option value="draft">新建 Draft</option>
              <option value="ops">编辑 Ops</option>
            </select>
          </label>

          {activeTab === 'template' ? (
            <label className="settings-field">
              <span>提示词模板</span>
              <select aria-label="提示词模板" value={templateId} onChange={(event) => onTemplateChange(event.target.value)}>
                {AI_PROMPT_TEMPLATES.map((template) => (
                  <option key={template.id} value={template.id}>
                    {template.label}
                  </option>
                ))}
              </select>
            </label>
          ) : (
            <div className="settings-field">
              <span>当前模板</span>
              <div className="ai-template-readonly">
                {AI_PROMPT_TEMPLATES.find((template) => template.id === templateId)?.label ?? templateId}
              </div>
            </div>
          )}
        </div>

        {activeTab === 'system' && (
          <label className="settings-field settings-field-full">
            <span>会话系统提示词</span>
            <textarea
              aria-label="会话系统提示词"
              rows={8}
              value={customSystemPrompt}
              onChange={(event) => onCustomSystemPromptChange(event.target.value)}
              placeholder="这里可以覆盖当前会话的系统提示词。"
            />
          </label>
        )}
      </div>
        </>
      )}
    </section>
  );
}
