import { useCallback, useEffect, useRef } from 'react';
import { controlDefinitions } from '../data/controlDefs';
import { useDesignerStore } from '../store/designerStore';
import EmojiPicker from './EmojiPicker';

export default function Toolbox() {
  const addControl = useDesignerStore((s) => s.addControl);
  const startToolboxDrag = useDesignerStore((s) => s.startToolboxDrag);
  const activeView = useDesignerStore((s) => s.activeView);
  const setActiveView = useDesignerStore((s) => s.setActiveView);
  const emojiPickerOpen = useDesignerStore((s) => s.emojiPickerOpen);
  const setEmojiPickerOpen = useDesignerStore((s) => s.setEmojiPickerOpen);
  const emojiAreaRef = useRef<HTMLDivElement | null>(null);

  const handleMouseDown = useCallback((e: React.MouseEvent, type: string, icon: string, label: string) => {
    e.preventDefault();
    startToolboxDrag(type, icon, label);
  }, [startToolboxDrag]);

  const handleDoubleClick = useCallback((type: string) => {
    addControl(type, 50 + Math.random() * 100, 50 + Math.random() * 100);
  }, [addControl]);

  useEffect(() => {
    if (!emojiPickerOpen) return;

    const handleDocumentMouseDown = (event: MouseEvent) => {
      const target = event.target as Node | null;
      if (target && emojiAreaRef.current?.contains(target)) {
        return;
      }
      setEmojiPickerOpen(false);
    };

    document.addEventListener('mousedown', handleDocumentMouseDown);
    return () => {
      document.removeEventListener('mousedown', handleDocumentMouseDown);
    };
  }, [emojiPickerOpen]);

  return (
    <div className="toolbox">
      <div className="toolbox-title">控件工具箱</div>
      <div className="toolbox-emoji-entry" ref={emojiAreaRef}>
        <button
          className="toolbox-emoji-btn"
          onClick={() => setEmojiPickerOpen(!emojiPickerOpen)}
          aria-label="Emoji 选择器"
        >
          😀 Emoji 选择器
        </button>
        <EmojiPicker open={emojiPickerOpen} onClose={() => setEmojiPickerOpen(false)} />
      </div>
      <div className="toolbox-grid">
        {controlDefinitions.map((def) => (
          <div
            key={def.type}
            className="toolbox-item"
            onMouseDown={(e) => handleMouseDown(e, def.type, def.icon, def.label)}
            onDoubleClick={() => handleDoubleClick(def.type)}
            title={`拖拽或双击添加${def.label}`}
          >
            <span className="toolbox-icon">{def.icon}</span>
            <span className="toolbox-label">{def.label}</span>
          </div>
        ))}
      </div>
      <div className="toolbox-footer-nav">
        <button
          className={`toolbox-nav-btn ${activeView === 'designer' ? 'active' : ''}`}
          onClick={() => setActiveView('designer')}
        >
          设计器
        </button>
        <button
          className={`toolbox-nav-btn ${activeView === 'about' ? 'active' : ''}`}
          onClick={() => setActiveView('about')}
        >
          关于
        </button>
        <button
          className={`toolbox-nav-btn ${activeView === 'ai-assistant' ? 'active' : ''}`}
          onClick={() => setActiveView('ai-assistant')}
        >
          AI 助手
        </button>
        <button
          className={`toolbox-nav-btn ${activeView === 'settings' ? 'active' : ''}`}
          onClick={() => setActiveView('settings')}
        >
          系统设置
        </button>
      </div>
    </div>
  );
}
