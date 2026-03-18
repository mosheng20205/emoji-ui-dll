import { useEffect, useState } from 'react';
import { controlDefinitions } from '../data/controlDefs';
import { saveRuntimeConfig, type RuntimeConfig } from '../config/runtimeConfig';
import { useDesignerStore } from '../store/designerStore';
import { formatShortcutFromEvent } from '../utils/shortcut';
import { testAiConnection } from '../ai/testConnection';
import { AI_PROVIDER_PRESETS, getAiProviderPreset } from '../ai/providerPresets';

export default function SettingsView() {
  const runtimeConfig = useDesignerStore((s) => s.runtimeConfig);
  const setRuntimeConfig = useDesignerStore((s) => s.setRuntimeConfig);
  const [draft, setDraft] = useState<RuntimeConfig>(runtimeConfig);
  const [saved, setSaved] = useState(false);
  const [connectionMessage, setConnectionMessage] = useState('');
  const [testingConnection, setTestingConnection] = useState(false);

  useEffect(() => {
    setDraft(runtimeConfig);
  }, [runtimeConfig]);

  const captureShortcut = (
    event: React.KeyboardEvent<HTMLInputElement>,
    key: 'emojiHotkey' | 'quickInsertHotkey'
  ) => {
    event.preventDefault();
    const shortcut = formatShortcutFromEvent(event.nativeEvent);
    if (shortcut) {
      setDraft((current) => ({ ...current, [key]: shortcut }));
    }
  };

  const handleSave = () => {
    saveRuntimeConfig(draft);
    setRuntimeConfig(draft);
    setSaved(true);
    window.setTimeout(() => setSaved(false), 1500);
  };

  const handleAiConnectionTest = async () => {
    setTestingConnection(true);
    setConnectionMessage('');
    try {
      const result = await testAiConnection(draft);
      setConnectionMessage(result.message);
    } catch (error) {
      setConnectionMessage(error instanceof Error ? error.message : '连接测试失败');
    } finally {
      setTestingConnection(false);
    }
  };

  const handleProviderChange = (nextProvider: RuntimeConfig['aiProvider']) => {
    const previousPreset = getAiProviderPreset(draft.aiProvider);
    const nextPreset = getAiProviderPreset(nextProvider);

    setDraft((current) => ({
      ...current,
      aiProvider: nextProvider,
      aiBaseUrl: !current.aiBaseUrl || current.aiBaseUrl === previousPreset.defaultBaseUrl
        ? nextPreset.defaultBaseUrl
        : current.aiBaseUrl,
      aiModel: !current.aiModel || current.aiModel === previousPreset.defaultModel
        ? nextPreset.defaultModel
        : current.aiModel,
    }));
  };

  return (
    <section className="info-view" aria-label="系统设置页面">
      <div className="info-view-card settings-redesign-shell">
        <section className="page-intro-card">
          <div className="page-intro-copy">
            <div className="page-intro-kicker">本地配置中心</div>
            <h1>系统设置</h1>
            <p>快捷键仅在软件窗口激活时生效。新版排版将配置分为快捷操作、AI 连接与本地保存说明三组，更适合日常维护。</p>
          </div>
          <div className="page-intro-badges">
            <span className="page-intro-badge">快捷键</span>
            <span className="page-intro-badge">AI 连接</span>
            <span className="page-intro-badge">本地保存</span>
          </div>
        </section>

        <div className="settings-card-grid">
          <section className="settings-panel-card">
            <div className="settings-card-header">
              <div>
                <div className="settings-card-kicker">快捷操作</div>
                <h2>快捷键设置</h2>
              </div>
              <span className="settings-status-badge">仅设计器内生效</span>
            </div>
            <div className="settings-grid">
              <label className="settings-field">
                <span>打开 Emoji 选择器</span>
                <input
                  aria-label="打开 Emoji 选择器"
                  value={draft.emojiHotkey}
                  onKeyDown={(event) => captureShortcut(event, 'emojiHotkey')}
                  onChange={() => undefined}
                />
              </label>
              <label className="settings-field">
                <span>快速插入控件</span>
                <input
                  aria-label="快速插入控件"
                  value={draft.quickInsertHotkey}
                  onKeyDown={(event) => captureShortcut(event, 'quickInsertHotkey')}
                  onChange={() => undefined}
                />
              </label>
              <label className="settings-field">
                <span>快速插入控件类型</span>
                <select
                  aria-label="快速插入控件类型"
                  value={draft.quickInsertControl}
                  onChange={(event) => setDraft((current) => ({ ...current, quickInsertControl: event.target.value }))}
                >
                  {controlDefinitions.map((control) => (
                    <option key={control.type} value={control.type}>
                      {control.label}
                    </option>
                  ))}
                </select>
              </label>
            </div>
          </section>

          <section className="settings-panel-card settings-panel-card-wide">
            <div className="settings-card-header">
              <div>
                <div className="settings-card-kicker">AI Assistant</div>
                <h2>AI 连接与模型</h2>
              </div>
              <span className="settings-status-badge">本地保存</span>
            </div>
            <p className="settings-note">所有 AI 配置仅保存在本地，不同步云端</p>
            <p className="settings-note">项目代码已开源，你可以自行审查配置保存和网络请求逻辑。</p>
            <div className="settings-grid">
              <label className="settings-field">
                <span>AI 提供商</span>
                <select
                  aria-label="AI 提供商"
                  value={draft.aiProvider}
                  onChange={(event) => handleProviderChange(event.target.value as RuntimeConfig['aiProvider'])}
                >
                  {AI_PROVIDER_PRESETS.map((provider) => (
                    <option key={provider.id} value={provider.id}>
                      {provider.label}
                    </option>
                  ))}
                </select>
              </label>
              <label className="settings-field">
                <span>AI BaseURL</span>
                <input
                  aria-label="AI BaseURL"
                  value={draft.aiBaseUrl}
                  onChange={(event) => setDraft((current) => ({ ...current, aiBaseUrl: event.target.value }))}
                  placeholder="https://api.openai.com/v1"
                />
              </label>
              <label className="settings-field">
                <span>AI API Key</span>
                <input
                  aria-label="AI API Key"
                  type="password"
                  value={draft.aiApiKey}
                  onChange={(event) => setDraft((current) => ({ ...current, aiApiKey: event.target.value }))}
                  placeholder="sk-..."
                />
              </label>
              <label className="settings-field">
                <span>AI 模型</span>
                <input
                  aria-label="AI 模型"
                  value={draft.aiModel}
                  onChange={(event) => setDraft((current) => ({ ...current, aiModel: event.target.value }))}
                  placeholder="gpt-4.1-mini"
                />
              </label>
              <label className="settings-field">
                <span>内置模板</span>
                <select
                  aria-label="内置模板"
                  value={draft.aiPromptTemplateId}
                  onChange={(event) => setDraft((current) => ({ ...current, aiPromptTemplateId: event.target.value }))}
                >
                  <option value="blank">通用空白界面</option>
                  <option value="login-form">登录窗口</option>
                  <option value="settings-form">设置页</option>
                  <option value="data-entry">数据录入窗体</option>
                </select>
              </label>
              <label className="settings-field">
                <span>AI 请求超时 (ms)</span>
                <input
                  aria-label="AI 请求超时 (ms)"
                  type="number"
                  value={draft.aiRequestTimeoutMs}
                  onChange={(event) =>
                    setDraft((current) => ({ ...current, aiRequestTimeoutMs: Number(event.target.value) || 1000 }))
                  }
                />
              </label>
              <label className="settings-field">
                <span>Temperature</span>
                <input
                  aria-label="Temperature"
                  type="number"
                  min="0"
                  max="2"
                  step="0.1"
                  value={draft.aiTemperature}
                  onChange={(event) =>
                    setDraft((current) => ({ ...current, aiTemperature: Number(event.target.value) || 0 }))
                  }
                />
              </label>
              <label className="settings-field">
                <span>历史文件名</span>
                <input
                  aria-label="历史文件名"
                  value={draft.aiHistoryFileName}
                  onChange={(event) => setDraft((current) => ({ ...current, aiHistoryFileName: event.target.value }))}
                />
              </label>
              <label className="settings-field settings-field-full">
                <span>系统提示词</span>
                <textarea
                  aria-label="系统提示词"
                  value={draft.aiSystemPrompt}
                  rows={5}
                  onChange={(event) => setDraft((current) => ({ ...current, aiSystemPrompt: event.target.value }))}
                  placeholder="可以在这里覆盖内置系统提示词。"
                />
              </label>
              <label className="settings-checkbox">
                <input
                  type="checkbox"
                  checked={draft.aiEnableEventGeneration}
                  onChange={(event) =>
                    setDraft((current) => ({ ...current, aiEnableEventGeneration: event.target.checked }))
                  }
                />
                <span>启用事件占位生成</span>
              </label>
              <label className="settings-checkbox">
                <input
                  type="checkbox"
                  checked={draft.aiEnableAdvancedNaming}
                  onChange={(event) =>
                    setDraft((current) => ({ ...current, aiEnableAdvancedNaming: event.target.checked }))
                  }
                />
                <span>启用高级自动命名策略</span>
              </label>
            </div>
            <div className="settings-actions">
              <button className="header-btn settings-primary-btn" onClick={() => void handleAiConnectionTest()} disabled={testingConnection}>
                {testingConnection ? '测试中...' : '测试连接'}
              </button>
              {connectionMessage && <span className="settings-saved-tip">{connectionMessage}</span>}
            </div>
          </section>

          <section className="settings-panel-card">
            <div className="settings-card-header">
              <div>
                <div className="settings-card-kicker">安全与数据</div>
                <h2>本地保存说明</h2>
              </div>
            </div>
            <div className="settings-info-list">
              <p>AI 配置、历史会话和设计器偏好均保存在本机，不会自动上传。</p>
              <p>如需更换模型供应商，只需调整 BaseURL、API Key 和模型名称即可。</p>
              <p>建议在多环境打包时通过 `.env` 预设默认值，再由本地设置进行覆盖。</p>
            </div>
          </section>
        </div>

        <div className="settings-actions">
          <button className="header-btn settings-primary-btn" onClick={handleSave}>保存设置</button>
          {saved && <span className="settings-saved-tip">已保存</span>}
        </div>
      </div>
    </section>
  );
}
