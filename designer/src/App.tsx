import { useEffect, useRef } from 'react';
import Header from './components/Header';
import Toolbox from './components/Toolbox';
import Canvas from './components/Canvas';
import PropertyPanel from './components/PropertyPanel';
import CodePanel from './components/CodePanel';
import AboutView from './components/AboutView';
import SettingsView from './components/SettingsView';
import AiAssistantView from './components/AiAssistantView';
import { useDesignerStore } from './store/designerStore';
import { reportActive, reportLaunch } from './services/statsReporter';
import { isShortcutMatch } from './utils/shortcut';
import './App.css';

export default function App() {
  const hasCompletedActiveReport = useRef(false);
  const isActiveReportPending = useRef(false);
  const activeView = useDesignerStore((s) => s.activeView);
  const runtimeConfig = useDesignerStore((s) => s.runtimeConfig);
  const toolboxDragging = useDesignerStore((s) => s.toolboxDragging);
  const dragGhostPos = useDesignerStore((s) => s.dragGhostPos);
  const updateDragGhost = useDesignerStore((s) => s.updateDragGhost);
  const endToolboxDrag = useDesignerStore((s) => s.endToolboxDrag);
  const addControl = useDesignerStore((s) => s.addControl);
  const copySelection = useDesignerStore((s) => s.copySelection);
  const pasteClipboard = useDesignerStore((s) => s.pasteClipboard);
  const duplicateSelection = useDesignerStore((s) => s.duplicateSelection);
  const setEmojiPickerOpen = useDesignerStore((s) => s.setEmojiPickerOpen);

  useEffect(() => {
    void reportLaunch(useDesignerStore.getState().runtimeConfig);
  }, []);

  useEffect(() => {
    const triggerActiveReport = () => {
      if (hasCompletedActiveReport.current || isActiveReportPending.current) {
        return;
      }
      isActiveReportPending.current = true;
      void reportActive(useDesignerStore.getState().runtimeConfig)
        .then((completed) => {
          if (completed) {
            hasCompletedActiveReport.current = true;
          }
        })
        .finally(() => {
          isActiveReportPending.current = false;
        });
    };

    window.addEventListener('pointerdown', triggerActiveReport);
    window.addEventListener('keydown', triggerActiveReport);
    return () => {
      window.removeEventListener('pointerdown', triggerActiveReport);
      window.removeEventListener('keydown', triggerActiveReport);
    };
  }, []);

  useEffect(() => {
    if (!toolboxDragging) return;

    const onMouseMove = (e: MouseEvent) => {
      updateDragGhost(e.clientX, e.clientY);
    };

    const onMouseUp = (e: MouseEvent) => {
      const target = e.target as HTMLElement;
      const isCanvas = target.closest('.canvas-inner');
      if (!isCanvas) {
        endToolboxDrag();
      }
    };

    document.addEventListener('mousemove', onMouseMove);
    document.addEventListener('mouseup', onMouseUp);
    return () => {
      document.removeEventListener('mousemove', onMouseMove);
      document.removeEventListener('mouseup', onMouseUp);
    };
  }, [toolboxDragging, updateDragGhost, endToolboxDrag]);

  useEffect(() => {
    const onKeyDown = (event: KeyboardEvent) => {
      const active = document.activeElement as HTMLElement | null;
      const editing = active?.tagName === 'INPUT' || active?.tagName === 'TEXTAREA' || active?.tagName === 'SELECT';
      if (editing) return;

      if (activeView !== 'designer') return;

      if ((event.ctrlKey || event.metaKey) && event.key.toLowerCase() === 'c') {
        event.preventDefault();
        copySelection();
      }
      if ((event.ctrlKey || event.metaKey) && event.key.toLowerCase() === 'v') {
        event.preventDefault();
        pasteClipboard();
      }
      if ((event.ctrlKey || event.metaKey) && event.key.toLowerCase() === 'd') {
        event.preventDefault();
        duplicateSelection();
      }

      if (isShortcutMatch(event, runtimeConfig.emojiHotkey)) {
        event.preventDefault();
        setEmojiPickerOpen(true);
      }

      if (isShortcutMatch(event, runtimeConfig.quickInsertHotkey)) {
        event.preventDefault();
        addControl(runtimeConfig.quickInsertControl, 80, 80);
      }
    };

    window.addEventListener('keydown', onKeyDown);
    return () => {
      window.removeEventListener('keydown', onKeyDown);
    };
  }, [activeView, addControl, copySelection, duplicateSelection, pasteClipboard, runtimeConfig, setEmojiPickerOpen]);

  return (
    <div className={`app ${toolboxDragging ? 'is-dragging' : ''}`}>
      <Header />
      <div className="app-body">
        <Toolbox />
        <div className="app-center">
          {activeView === 'designer' ? (
            <>
              <div className="canvas-wrapper">
                <Canvas />
              </div>
              <CodePanel />
            </>
          ) : activeView === 'about' ? (
            <AboutView />
          ) : activeView === 'ai-assistant' ? (
            <AiAssistantView />
          ) : (
            <SettingsView />
          )}
        </div>
        {activeView === 'designer' && <PropertyPanel />}
      </div>

      {toolboxDragging && dragGhostPos && (
        <div
          className="drag-ghost"
          style={{
            left: dragGhostPos.x + 12,
            top: dragGhostPos.y + 12,
          }}
        >
          <span className="drag-ghost-icon">{toolboxDragging.icon}</span>
          <span>{toolboxDragging.label}</span>
        </div>
      )}
    </div>
  );
}
