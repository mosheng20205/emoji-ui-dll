import type { AiDesignDraft } from '../ai/schema';

interface AiPreviewPanelProps {
  draft: AiDesignDraft;
  rawResponse: string;
  canRollback: boolean;
  onApply: () => void;
  onRollback: () => void;
}

export default function AiPreviewPanel({
  draft,
  rawResponse,
  canRollback,
  onApply,
  onRollback,
}: AiPreviewPanelProps) {
  return (
    <section className="ai-panel ai-preview-panel">
      <div className="ai-panel-header">
        <h2>Draft 预览</h2>
        <span className="settings-status-badge">新建布局</span>
      </div>

      <section className="ai-preview-section">
        <h3>执行摘要</h3>
        <div className="ai-preview-summary">
          <div className="stats-card">
            <div className="stats-label">摘要</div>
            <div className="stats-value ai-preview-title">{draft.summary || '未提供摘要'}</div>
          </div>
          <div className="stats-card">
            <div className="stats-label">窗口标题</div>
            <div className="stats-value ai-preview-title">{draft.window.title}</div>
          </div>
          <div className="stats-card">
            <div className="stats-label">控件数量</div>
            <div className="stats-value">{draft.controls.length}</div>
          </div>
          <div className="stats-card">
            <div className="stats-label">窗口尺寸</div>
            <div className="stats-value">{`${draft.window.width} × ${draft.window.height}`}</div>
          </div>
        </div>
      </section>

      <section className="ai-preview-section">
        <h3>窗口概览</h3>
        <div className="ai-detail-grid">
          <div className="ai-detail-card">
            <span>标题栏颜色</span>
            <strong>{draft.window.titlebarColor}</strong>
          </div>
          <div className="ai-detail-card">
            <span>背景颜色</span>
            <strong>{draft.window.bgColor}</strong>
          </div>
          <div className="ai-detail-card">
            <span>窗口 Emoji</span>
            <strong>{draft.window.emoji || '未设置'}</strong>
          </div>
          <div className="ai-detail-card">
            <span>布局类型</span>
            <strong>{draft.controls.length > 0 ? '含控件布局' : '空白布局'}</strong>
          </div>
        </div>
      </section>

      {draft.warnings.length > 0 && (
        <section className="ai-preview-section">
          <h3>风险提示</h3>
          <div className="ai-warning-list">
            {draft.warnings.map((warning) => (
              <div key={warning} className="ai-warning-item">{warning}</div>
            ))}
          </div>
        </section>
      )}

      <section className="ai-preview-section">
        <h3>控件清单</h3>
        <div className="ai-preview-control-list">
          {draft.controls.map((control, index) => (
            <div key={`${control.type}-${index}`} className="ai-preview-control-item">
              <div className="ai-preview-control-main">
                <strong>{control.name || `${control.type}_${index + 1}`}</strong>
                <span className="ai-preview-type-badge">{control.type}</span>
              </div>
              <div className="ai-preview-control-meta">
                {`位置 ${control.x}, ${control.y} · 尺寸 ${control.width} × ${control.height}`}
              </div>
            </div>
          ))}
        </div>
      </section>

      <div className="settings-actions ai-preview-actions">
        <button className="header-btn settings-primary-btn ai-action-btn-primary" onClick={onApply}>替换当前画布</button>
        {canRollback && <button className="header-btn ai-action-btn-secondary" onClick={onRollback}>回滚最近一次 AI 应用</button>}
      </div>

      <section className="ai-preview-section">
        <h3>原始 JSON</h3>
        <pre className="about-code-block ai-raw-json">{rawResponse}</pre>
      </section>
    </section>
  );
}
