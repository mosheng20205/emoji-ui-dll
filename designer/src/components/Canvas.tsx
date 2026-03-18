import { useCallback, useEffect, useMemo, useRef, useState } from 'react';
import { useDesignerStore } from '../store/designerStore';
import ControlRenderer from './ControlRenderer';
import { getControlDef } from '../data/controlDefs';
import {
  findDropTarget,
  getAbsoluteRect,
  getChildControls,
  getContentRect,
  getTabTitles,
  isContainerControl,
  rectIntersects,
  type Rect,
} from '../utils/layout';
import { resizeWindowByHandle } from '../utils/designerCommands';
import type { DesignControl } from '../types/controls';

interface CanvasNodeProps {
  control: DesignControl;
  controls: DesignControl[];
  selectedIds: string[];
  onControlMouseDown: (event: React.MouseEvent, control: DesignControl) => void;
  onResizeMouseDown: (event: React.MouseEvent, control: DesignControl) => void;
  onTabSelect: (controlId: string, index: number) => void;
}

function CanvasNode({
  control,
  controls,
  selectedIds,
  onControlMouseDown,
  onResizeMouseDown,
  onTabSelect,
}: CanvasNodeProps) {
  const isSelected = selectedIds.includes(control.id);
  const contentRect = getContentRect(control);
  const activeTab = Number(control.props.activeTab ?? 0);
  const children = useMemo(() => {
    if (control.type === 'tabcontrol') {
      return getChildControls(controls, control.id, activeTab);
    }
    if (isContainerControl(control)) {
      return getChildControls(controls, control.id, null);
    }
    return [];
  }, [activeTab, control, controls]);

  return (
    <div
      className={`canvas-control ${isSelected ? 'selected' : ''}`}
      style={{
        position: 'absolute',
        left: control.x,
        top: control.y,
        width: control.width,
        height: control.height,
      }}
      onMouseDown={(event) => onControlMouseDown(event, control)}
      data-control-id={control.id}
    >
      <ControlRenderer
        control={control}
        onTabSelect={(index) => onTabSelect(control.id, index)}
      />

      {isContainerControl(control) && (
        <div
          className="container-children-layer"
          style={{
            position: 'absolute',
            left: contentRect.x,
            top: contentRect.y,
            width: contentRect.width,
            height: contentRect.height,
            overflow: 'hidden',
          }}
        >
          {control.type === 'tabcontrol' && getTabTitles(control).length === 0 && (
            <div className="empty-container-hint">先添加选项卡页面</div>
          )}
          {children.map((child) => (
            <CanvasNode
              key={child.id}
              control={child}
              controls={controls}
              selectedIds={selectedIds}
              onControlMouseDown={onControlMouseDown}
              onResizeMouseDown={onResizeMouseDown}
              onTabSelect={onTabSelect}
            />
          ))}
        </div>
      )}

      {isSelected && (
        <>
          <div className="selection-border" />
          <div
            className="resize-handle"
            onMouseDown={(event) => onResizeMouseDown(event, control)}
          />
          <div className="control-name-tag">{control.name}</div>
        </>
      )}
    </div>
  );
}

export default function Canvas() {
  const {
    window: win,
    controls,
    selectedIds,
    selectOnly,
    toggleSelect,
    setSelection,
    moveSelectionBy,
    addControl,
    resizeControl,
    updateControlProp,
    toolboxDragging,
    endToolboxDrag,
    setWindow,
  } = useDesignerStore();

  const canvasRef = useRef<HTMLDivElement>(null);
  const [dragging, setDragging] = useState<{ lastX: number; lastY: number } | null>(null);
  const [resizingControl, setResizingControl] = useState<{
    id: string;
    startX: number;
    startY: number;
    startW: number;
    startH: number;
  } | null>(null);
  const [resizingWindow, setResizingWindow] = useState<{
    handle: 'e' | 's' | 'se';
    startX: number;
    startY: number;
    startW: number;
    startH: number;
  } | null>(null);
  const [marquee, setMarquee] = useState<Rect | null>(null);
  const [marqueeStart, setMarqueeStart] = useState<{ x: number; y: number } | null>(null);

  const rootControls = useMemo(
    () => getChildControls(controls, null),
    [controls]
  );

  const getCanvasPos = useCallback((event: MouseEvent | React.MouseEvent) => {
    const rect = canvasRef.current?.getBoundingClientRect();
    if (!rect) {
      return { x: 0, y: 0 };
    }
    return {
      x: event.clientX - rect.left,
      y: event.clientY - rect.top,
    };
  }, []);

  const finalizeToolboxDrop = useCallback((event: MouseEvent | React.MouseEvent) => {
    if (!toolboxDragging) return;
    const position = getCanvasPos(event);
    const definition = getControlDef(toolboxDragging.type);
    if (!definition) {
      endToolboxDrag();
      return;
    }

    const target = findDropTarget(controls, position.x, position.y);
    let localX = position.x - Math.round(definition.defaultWidth / 2);
    let localY = position.y - Math.round(definition.defaultHeight / 2);

    if (target.parentId) {
      const parent = controls.find((control) => control.id === target.parentId);
      if (parent) {
        const parentAbsolute = getAbsoluteRect(parent, controls);
        const contentRect = getContentRect(parent);
        localX = position.x - parentAbsolute.x - contentRect.x - Math.round(definition.defaultWidth / 2);
        localY = position.y - parentAbsolute.y - contentRect.y - Math.round(definition.defaultHeight / 2);
      }
    }

    addControl(
      toolboxDragging.type,
      Math.max(0, localX),
      Math.max(0, localY),
      {
        parentId: target.parentId,
        parentKind: target.parentKind,
        parentTabIndex: target.parentTabIndex,
      }
    );
    endToolboxDrag();
  }, [addControl, controls, endToolboxDrag, getCanvasPos, toolboxDragging]);

  useEffect(() => {
    const handleMove = (event: MouseEvent) => {
      if (dragging) {
        moveSelectionBy(event.clientX - dragging.lastX, event.clientY - dragging.lastY);
        setDragging({ lastX: event.clientX, lastY: event.clientY });
      }

      if (resizingControl) {
        const dx = event.clientX - resizingControl.startX;
        const dy = event.clientY - resizingControl.startY;
        resizeControl(resizingControl.id, resizingControl.startW + dx, resizingControl.startH + dy);
      }

      if (resizingWindow) {
        const resized = resizeWindowByHandle(
          { width: resizingWindow.startW, height: resizingWindow.startH },
          resizingWindow.handle,
          { dx: event.clientX - resizingWindow.startX, dy: event.clientY - resizingWindow.startY },
          { minWidth: 300, minHeight: 200 }
        );
        setWindow(resized);
      }

      if (marqueeStart) {
        const current = getCanvasPos(event);
        setMarquee({
          x: Math.min(marqueeStart.x, current.x),
          y: Math.min(marqueeStart.y, current.y),
          width: Math.abs(current.x - marqueeStart.x),
          height: Math.abs(current.y - marqueeStart.y),
        });
      }
    };

    const handleUp = (event: MouseEvent) => {
      if (toolboxDragging) {
        const target = event.target as HTMLElement;
        if (target.closest('.canvas-inner')) {
          finalizeToolboxDrop(event);
        } else {
          endToolboxDrag();
        }
      }

      if (marquee) {
        const hits = controls
          .filter((control) => rectIntersects(getAbsoluteRect(control, controls), marquee))
          .map((control) => control.id);
        setSelection(hits);
      }

      setDragging(null);
      setResizingControl(null);
      setResizingWindow(null);
      setMarquee(null);
      setMarqueeStart(null);
    };

    if (!dragging && !resizingControl && !resizingWindow && !marqueeStart && !toolboxDragging) {
      return;
    }

    document.addEventListener('mousemove', handleMove);
    document.addEventListener('mouseup', handleUp);
    return () => {
      document.removeEventListener('mousemove', handleMove);
      document.removeEventListener('mouseup', handleUp);
    };
  }, [
    controls,
    dragging,
    endToolboxDrag,
    finalizeToolboxDrop,
    getCanvasPos,
    marquee,
    marqueeStart,
    moveSelectionBy,
    resizeControl,
    resizingControl,
    resizingWindow,
    setSelection,
    setWindow,
    toolboxDragging,
  ]);

  const handleCanvasMouseDown = (event: React.MouseEvent) => {
    const target = event.target as HTMLElement;
    if (target.closest('.canvas-control') || target.closest('.window-resize-handle')) {
      return;
    }
    if (!event.ctrlKey && !event.metaKey) {
      selectOnly(null);
    }
    const position = getCanvasPos(event);
    setMarqueeStart(position);
    setMarquee({ x: position.x, y: position.y, width: 0, height: 0 });
  };

  const handleCanvasClick = (event: React.MouseEvent) => {
    const target = event.target as HTMLElement;
    if (target === canvasRef.current || target.classList.contains('canvas-inner')) {
      selectOnly(null);
    }
  };

  const handleControlMouseDown = (event: React.MouseEvent, control: DesignControl) => {
    event.stopPropagation();
    const append = event.ctrlKey || event.metaKey;

    if (append) {
      toggleSelect(control.id, true);
    } else if (!selectedIds.includes(control.id)) {
      selectOnly(control.id);
    }

    setDragging({
      lastX: event.clientX,
      lastY: event.clientY,
    });
  };

  const handleResizeMouseDown = (event: React.MouseEvent, control: DesignControl) => {
    event.stopPropagation();
    event.preventDefault();
    setResizingControl({
      id: control.id,
      startX: event.clientX,
      startY: event.clientY,
      startW: control.width,
      startH: control.height,
    });
  };

  const handleWindowResizeMouseDown = (
    event: React.MouseEvent,
    handle: 'e' | 's' | 'se'
  ) => {
    event.stopPropagation();
    event.preventDefault();
    setResizingWindow({
      handle,
      startX: event.clientX,
      startY: event.clientY,
      startW: win.width,
      startH: win.height,
    });
  };

  const handleKeyDown = (event: React.KeyboardEvent) => {
    if (selectedIds.length === 0) return;
    if (event.key === 'Delete' || event.key === 'Backspace') {
      event.preventDefault();
      for (const id of selectedIds) {
        useDesignerStore.getState().removeControl(id);
      }
    }
  };

  return (
    <div
      className="canvas-container"
      tabIndex={0}
      onKeyDown={handleKeyDown}
      style={{ width: win.width, height: win.height }}
    >
      <div className="canvas-header">
        <div className="canvas-title-bar" style={{ background: win.titlebarColor }}>
          <span className="canvas-title-text">{win.emoji} {win.title}</span>
          <div className="canvas-title-buttons">
            <span className="title-btn minimize">─</span>
            <span className="title-btn maximize">□</span>
            <span className="title-btn close">✕</span>
          </div>
        </div>
      </div>
      <div
        ref={canvasRef}
        className={`canvas-inner ${toolboxDragging ? 'drop-ready' : ''}`}
        style={{
          width: win.width,
          height: win.height - 32,
          background: win.bgColor,
        }}
        onMouseDown={handleCanvasMouseDown}
        onClick={handleCanvasClick}
      >
        {rootControls.map((control) => (
          <CanvasNode
            key={control.id}
            control={control}
            controls={controls}
            selectedIds={selectedIds}
            onControlMouseDown={handleControlMouseDown}
            onResizeMouseDown={handleResizeMouseDown}
            onTabSelect={(controlId, index) => updateControlProp(controlId, 'activeTab', index)}
          />
        ))}

        {toolboxDragging && (
          <div className="drop-hint">
            松开鼠标放置 {toolboxDragging.icon} {toolboxDragging.label}
          </div>
        )}

        {marquee && (
          <div
            className="selection-marquee"
            style={{
              left: marquee.x,
              top: marquee.y,
              width: marquee.width,
              height: marquee.height,
            }}
          />
        )}
      </div>

      <div
        className="window-resize-handle window-resize-e"
        onMouseDown={(event) => handleWindowResizeMouseDown(event, 'e')}
      />
      <div
        className="window-resize-handle window-resize-s"
        onMouseDown={(event) => handleWindowResizeMouseDown(event, 's')}
      />
      <div
        className="window-resize-handle window-resize-se"
        onMouseDown={(event) => handleWindowResizeMouseDown(event, 'se')}
      />
    </div>
  );
}
