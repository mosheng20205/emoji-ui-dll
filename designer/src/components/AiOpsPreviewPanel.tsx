import type { AiOpsDraft } from '../ai/schema';

interface AiOpsPreviewPanelProps {
  ops: AiOpsDraft;
  rawResponse: string;
  canRollback: boolean;
  onApply: () => void;
  onRollback: () => void;
}

export default function AiOpsPreviewPanel({
  ops,
  rawResponse,
  canRollback,
  onApply,
  onRollback,
}: AiOpsPreviewPanelProps) {
  const describeOperation = (index: number) => {
    const operation = ops.operations[index];
    switch (operation.type) {
      case 'addControl':
        return `新增 ${operation.control.type}，名称 ${operation.control.name || '未命名控件'}`;
      case 'updateControl':
        return `更新控件 ${operation.target}`;
      case 'removeControl':
        return `移除控件 ${operation.target}`;
      case 'moveControl':
        return `移动到 ${operation.x}, ${operation.y}`;
      case 'resizeControl':
        return `调整为 ${operation.width} × ${operation.height}`;
      case 'reparentControl':
        return `重新挂载到 ${operation.parentKind || 'window'}`;
      case 'updateWindow':
        return '更新窗口属性';
      case 'createEventPlaceholder':
        return `创建事件 ${operation.eventName}`;
      case 'renameControl':
        return `重命名为 ${operation.name}`;
      default:
        return '';
    }
  };

  return (
    <section className="ai-panel ai-preview-panel">
      <div className="ai-panel-header">
        <h2>Ops 预览</h2>
        <span className="settings-status-badge">编辑布局</span>
      </div>

      <section className="ai-preview-section">
        <h3>操作摘要</h3>
        <div className="ai-preview-summary">
          <div className="stats-card">
            <div className="stats-label">摘要</div>
            <div className="stats-value ai-preview-title">{ops.summary || '未提供摘要'}</div>
          </div>
          <div className="stats-card">
            <div className="stats-label">影响控件</div>
            <div className="stats-value">{ops.affectedControls.length}</div>
          </div>
          <div className="stats-card">
            <div className="stats-label">操作数量</div>
            <div className="stats-value">{ops.operations.length}</div>
          </div>
          <div className="stats-card">
            <div className="stats-label">编辑模式</div>
            <div className="stats-value">增量修改</div>
          </div>
        </div>
      </section>

      <section className="ai-preview-section">
        <h3>影响范围</h3>
        <div className="ai-detail-grid">
          {ops.affectedControls.length > 0 ? (
            ops.affectedControls.map((item) => (
              <div key={item} className="ai-detail-card">
                <span>目标控件</span>
                <strong>{item}</strong>
              </div>
            ))
          ) : (
            <div className="ai-detail-card">
              <span>目标控件</span>
              <strong>窗口级更新</strong>
            </div>
          )}
        </div>
      </section>

      {ops.warnings.length > 0 && (
        <section className="ai-preview-section">
          <h3>风险提示</h3>
          <div className="ai-warning-list">
            {ops.warnings.map((warning) => (
              <div key={warning} className="ai-warning-item">{warning}</div>
            ))}
          </div>
        </section>
      )}

      <section className="ai-preview-section">
        <h3>操作清单</h3>
        <div className="ai-ops-list">
          {ops.operations.map((operation, index) => (
            <div key={`${operation.type}-${index}`} className="ai-preview-control-item">
              <div className="ai-preview-control-main">
                <strong>{operation.type}</strong>
                <span className="ai-preview-type-badge">{'target' in operation ? operation.target : 'window'}</span>
              </div>
              <div className="ai-preview-control-meta">{describeOperation(index)}</div>
            </div>
          ))}
        </div>
      </section>

      <div className="settings-actions ai-preview-actions">
        <button className="header-btn settings-primary-btn ai-action-btn-primary" onClick={onApply}>应用操作清单</button>
        {canRollback && <button className="header-btn ai-action-btn-secondary" onClick={onRollback}>回滚最近一次 AI 应用</button>}
      </div>

      <section className="ai-preview-section">
        <h3>原始 JSON</h3>
        <pre className="about-code-block ai-raw-json">{rawResponse}</pre>
      </section>
    </section>
  );
}
