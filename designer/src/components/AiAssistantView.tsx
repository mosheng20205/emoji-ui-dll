import { useEffect, useMemo, useState } from 'react';
import { applyDraft } from '../ai/applyDraft';
import { applyOps } from '../ai/applyOps';
import { buildPrompt, type AiGenerationMode } from '../ai/buildPrompt';
import { parseStructuredResponse } from '../ai/parseStructuredResponse';
import { chatWithAi } from '../ai/providerRouter';
import { listAiHistorySessions, loadAiHistorySession, saveAiHistorySession, type AiHistoryIndexItem } from '../services/aiHistory';
import AiChatPanel from './AiChatPanel';
import AiOpsPreviewPanel from './AiOpsPreviewPanel';
import AiPreviewPanel from './AiPreviewPanel';
import AiPromptPanel, { type AiPromptTab, type AiViewMode } from './AiPromptPanel';
import { useDesignerStore } from '../store/designerStore';

function resolveGenerationMode(mode: AiViewMode, controlCount: number): AiGenerationMode {
  if (mode === 'draft' || mode === 'ops') {
    return mode;
  }
  return controlCount > 0 ? 'ops' : 'draft';
}

export default function AiAssistantView() {
  const runtimeConfig = useDesignerStore((s) => s.runtimeConfig);
  const windowState = useDesignerStore((s) => s.window);
  const controls = useDesignerStore((s) => s.controls);
  const loadDesign = useDesignerStore((s) => s.loadDesign);
  const aiSessionId = useDesignerStore((s) => s.aiSessionId);
  const aiMessages = useDesignerStore((s) => s.aiMessages);
  const aiDraftPreview = useDesignerStore((s) => s.aiDraftPreview);
  const aiOpsPreview = useDesignerStore((s) => s.aiOpsPreview);
  const aiRawResponse = useDesignerStore((s) => s.aiRawResponse);
  const aiLastAppliedSnapshot = useDesignerStore((s) => s.aiLastAppliedSnapshot);
  const setAiSessionId = useDesignerStore((s) => s.setAiSessionId);
  const setAiMessages = useDesignerStore((s) => s.setAiMessages);
  const clearAiConversation = useDesignerStore((s) => s.clearAiConversation);
  const setAiDraftPreview = useDesignerStore((s) => s.setAiDraftPreview);
  const setAiOpsPreview = useDesignerStore((s) => s.setAiOpsPreview);
  const setAiRawResponse = useDesignerStore((s) => s.setAiRawResponse);
  const snapshotCurrentDesign = useDesignerStore((s) => s.snapshotCurrentDesign);
  const rollbackLastAiApply = useDesignerStore((s) => s.rollbackLastAiApply);

  const [mode, setMode] = useState<AiViewMode>('auto');
  const [promptTab, setPromptTab] = useState<AiPromptTab>('template');
  const [promptCollapsed, setPromptCollapsed] = useState(false);
  const [templateId, setTemplateId] = useState(runtimeConfig.aiPromptTemplateId);
  const [customSystemPrompt, setCustomSystemPrompt] = useState(runtimeConfig.aiSystemPrompt);
  const [busy, setBusy] = useState(false);
  const [errorMessage, setErrorMessage] = useState('');
  const [sessionCreatedAt, setSessionCreatedAt] = useState<string | null>(null);
  const [historyItems, setHistoryItems] = useState<AiHistoryIndexItem[]>([]);

  useEffect(() => {
    if (!aiSessionId) {
      setAiSessionId(`ai-session-${Date.now()}`);
    }
  }, [aiSessionId, setAiSessionId]);

  useEffect(() => {
    setTemplateId(runtimeConfig.aiPromptTemplateId);
    setCustomSystemPrompt(runtimeConfig.aiSystemPrompt);
  }, [runtimeConfig.aiPromptTemplateId, runtimeConfig.aiSystemPrompt]);

  useEffect(() => {
    if (!sessionCreatedAt) {
      setSessionCreatedAt(new Date().toISOString());
    }
  }, [sessionCreatedAt]);

  useEffect(() => {
    let mounted = true;
    void listAiHistorySessions(runtimeConfig).then((items) => {
      if (mounted) {
        setHistoryItems(items);
      }
    });
    return () => {
      mounted = false;
    };
  }, [runtimeConfig]);

  const previewKind = useMemo(() => {
    if (aiDraftPreview) return 'draft';
    if (aiOpsPreview) return 'ops';
    return 'none';
  }, [aiDraftPreview, aiOpsPreview]);

  const handleSend = async (text: string) => {
    const effectiveMode = resolveGenerationMode(mode, controls.length);
    const previousMessages = aiMessages;
    const nextMessages = [...previousMessages, { role: 'user' as const, content: text }];
    setAiMessages(nextMessages);
    setErrorMessage('');
    setBusy(true);

    try {
      const promptMessages = buildPrompt({
        mode: effectiveMode,
        userPrompt: text,
        templateId,
        customSystemPrompt,
        previousMessages,
        window: windowState,
        controls,
        runtimeConfig,
      });
      const result = await chatWithAi(runtimeConfig, promptMessages);
      setAiRawResponse(result.text);
      const parsed = parseStructuredResponse(result.text);
      const finalMessages = [...nextMessages, { role: 'assistant' as const, content: result.text }];
      setAiMessages(finalMessages);
      if (parsed.kind === 'draft') {
        setAiDraftPreview(parsed.data);
      } else {
        setAiOpsPreview(parsed.data);
      }
      const nextSessionId = aiSessionId || `ai-session-${Date.now()}`;
      if (!aiSessionId) {
        setAiSessionId(nextSessionId);
      }
      const now = new Date().toISOString();
      await saveAiHistorySession(runtimeConfig, {
        sessionId: nextSessionId,
        title: parsed.kind === 'draft'
          ? (parsed.data.summary || text.slice(0, 24))
          : (parsed.data.summary || text.slice(0, 24)),
        createdAt: sessionCreatedAt || now,
        updatedAt: now,
        messages: finalMessages,
        rawResponse: result.text,
        draftPreview: parsed.kind === 'draft' ? parsed.data : null,
        opsPreview: parsed.kind === 'ops' ? parsed.data : null,
      });
      setHistoryItems(await listAiHistorySessions(runtimeConfig));
    } catch (error) {
      setErrorMessage(error instanceof Error ? error.message : 'AI 生成失败');
    } finally {
      setBusy(false);
    }
  };

  const handleApplyDraft = () => {
    if (!aiDraftPreview) return;
    snapshotCurrentDesign();
    const applied = applyDraft(aiDraftPreview);
    loadDesign(applied.window, applied.controls);
  };

  const handleApplyOps = () => {
    if (!aiOpsPreview) return;
    snapshotCurrentDesign();
    const applied = applyOps(windowState, controls, aiOpsPreview);
    loadDesign(applied.window, applied.controls);
  };

  const handleRestoreSession = async (sessionId: string) => {
    const session = await loadAiHistorySession(runtimeConfig, sessionId);
    if (!session) {
      setErrorMessage('无法读取历史会话');
      return;
    }
    setAiSessionId(session.sessionId);
    setSessionCreatedAt(session.createdAt);
    setAiMessages(session.messages);
    setAiRawResponse(session.rawResponse);
    setAiDraftPreview(session.draftPreview);
    setAiOpsPreview(session.opsPreview);
    setErrorMessage('');
  };

  const handleClearConversation = () => {
    clearAiConversation();
    setSessionCreatedAt(new Date().toISOString());
    setErrorMessage('');
  };

  return (
    <section className="info-view" aria-label="AI 助手页面">
      <div className="info-view-card ai-assistant-shell">
        <section className="page-intro-card">
          <div className="page-intro-copy">
            <div className="page-intro-kicker">AI Design Copilot</div>
            <h1>AI 助手</h1>
            <p>通过自然语言描述界面需求，AI 会返回结构化 Draft 或 Ops，并在右侧预览后应用到画布。</p>
          </div>
          <div className="page-intro-badges">
            <span className="page-intro-badge">多轮编辑</span>
            <span className="page-intro-badge">结构化预览</span>
            <span className="page-intro-badge">先预览后应用</span>
          </div>
        </section>

        <AiPromptPanel
          runtimeConfig={runtimeConfig}
          mode={mode}
          templateId={templateId}
          customSystemPrompt={customSystemPrompt}
          activeTab={promptTab}
          collapsed={promptCollapsed}
          onModeChange={setMode}
          onTabChange={setPromptTab}
          onTemplateChange={setTemplateId}
          onCustomSystemPromptChange={setCustomSystemPrompt}
          onToggleCollapse={() => setPromptCollapsed((current) => !current)}
        />

        <div className="ai-main-layout">
          <AiChatPanel
            messages={aiMessages}
            busy={busy}
            errorMessage={errorMessage}
            historyItems={historyItems}
            onSend={handleSend}
            onClear={handleClearConversation}
            onRestoreSession={(sessionId) => void handleRestoreSession(sessionId)}
          />

          <div className="ai-preview-column">
            {previewKind === 'draft' && aiDraftPreview ? (
              <AiPreviewPanel
                draft={aiDraftPreview}
                rawResponse={aiRawResponse}
                canRollback={Boolean(aiLastAppliedSnapshot)}
                onApply={handleApplyDraft}
                onRollback={rollbackLastAiApply}
              />
            ) : previewKind === 'ops' && aiOpsPreview ? (
              <AiOpsPreviewPanel
                ops={aiOpsPreview}
                rawResponse={aiRawResponse}
                canRollback={Boolean(aiLastAppliedSnapshot)}
                onApply={handleApplyOps}
                onRollback={rollbackLastAiApply}
              />
            ) : (
              <section className="ai-panel ai-preview-panel">
                <div className="ai-panel-header">
                  <h2>预览区</h2>
                  <span className="settings-status-badge">等待生成</span>
                </div>
                <div className="ai-empty-tip">发送消息后，这里会显示 Draft 或 Ops 预览。</div>
                <section className="ai-preview-section">
                  <h3>操作流程</h3>
                  <div className="ai-empty-flow-grid">
                    <div className="ai-detail-card">
                      <span>步骤 1</span>
                      <strong>发送需求</strong>
                    </div>
                    <div className="ai-detail-card">
                      <span>步骤 2</span>
                      <strong>检查预览</strong>
                    </div>
                    <div className="ai-detail-card">
                      <span>步骤 3</span>
                      <strong>应用或回滚</strong>
                    </div>
                  </div>
                </section>
                {aiLastAppliedSnapshot && (
                  <div className="settings-actions ai-preview-actions">
                    <button className="header-btn ai-action-btn-secondary" onClick={rollbackLastAiApply}>回滚最近一次 AI 应用</button>
                  </div>
                )}
              </section>
            )}
          </div>
        </div>
      </div>
    </section>
  );
}
