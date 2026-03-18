import { useState } from 'react';
import type { AiChatMessage } from '../ai/types';
import type { AiHistoryIndexItem } from '../services/aiHistory';

interface AiChatPanelProps {
  messages: AiChatMessage[];
  busy: boolean;
  errorMessage: string;
  historyItems: AiHistoryIndexItem[];
  onSend: (text: string) => Promise<void> | void;
  onClear: () => void;
  onRestoreSession: (sessionId: string) => void;
}

export default function AiChatPanel({
  messages,
  busy,
  errorMessage,
  historyItems,
  onSend,
  onClear,
  onRestoreSession,
}: AiChatPanelProps) {
  const [input, setInput] = useState('');
  const [historyOpen, setHistoryOpen] = useState(false);
  const suggestionPrompts = [
    '创建一个登录窗口',
    '生成一个设置页',
    '把当前界面改成深色风格',
  ];

  const handleSend = async () => {
    const value = input.trim();
    if (!value || busy) return;
    setInput('');
    await onSend(value);
  };

  return (
    <section className="ai-panel ai-chat-panel">
      <div className="ai-panel-header">
        <h2>聊天</h2>
        <div className="ai-chat-actions">
          <button className="header-btn" onClick={() => setHistoryOpen((open) => !open)}>历史会话</button>
          <button className="header-btn" onClick={onClear}>清空会话</button>
        </div>
      </div>

      {historyOpen && (
        <div className="ai-history-popover">
          {historyItems.length === 0 ? (
            <div className="ai-history-empty">暂无历史会话</div>
          ) : (
            historyItems.map((item) => (
              <button
                key={item.sessionId}
                className="toolbox-nav-btn"
                onClick={() => {
                  onRestoreSession(item.sessionId);
                  setHistoryOpen(false);
                }}
              >
                {`恢复：${item.title}`}
              </button>
            ))
          )}
        </div>
      )}

      <div className="ai-chat-messages">
        {messages.length === 0 ? (
          <div className="ai-chat-empty-state">
            <div className="ai-empty-tip">描述你想要的界面，AI 会返回结构化布局或操作清单。</div>
            <div className="ai-suggestion-block">
              <div className="ai-suggestion-title">建议提示</div>
              <div className="ai-suggestion-list">
                {suggestionPrompts.map((prompt) => (
                  <button
                    key={prompt}
                    type="button"
                    className="ai-suggestion-chip"
                    onClick={() => setInput(prompt)}
                  >
                    {prompt}
                  </button>
                ))}
              </div>
            </div>
          </div>
        ) : (
          messages.map((message, index) => (
            <div key={`${message.role}-${index}`} className={`ai-chat-message ai-chat-${message.role}`}>
              <div className="ai-chat-role">
                {message.role === 'user' ? '你' : message.role === 'assistant' ? 'AI' : '系统'}
              </div>
              <pre className="ai-chat-content">{message.content}</pre>
            </div>
          ))
        )}
      </div>

      <div className="ai-compose-card">
        <label className="settings-field settings-field-full">
          <span>输入需求</span>
          <textarea
            aria-label="AI 输入框"
            rows={7}
            value={input}
            onChange={(event) => setInput(event.target.value)}
            placeholder="例如：帮我生成一个带用户名、密码、登录按钮的登录窗体。"
          />
        </label>

        {errorMessage && <div className="ai-error-banner">{errorMessage}</div>}

        <div className="ai-compose-actions">
          <button
            className="header-btn settings-primary-btn ai-send-btn"
            onClick={() => void handleSend()}
            disabled={busy}
          >
            {busy ? '生成中...' : '发送到 AI'}
          </button>
        </div>
      </div>
    </section>
  );
}
