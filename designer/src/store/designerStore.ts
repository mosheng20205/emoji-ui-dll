import { create } from 'zustand';
import type { AppView, DesignControl, DesignWindow, CodeLanguage } from '../types/controls';
import { getControlDef } from '../data/controlDefs';
import { loadRuntimeConfig, type RuntimeConfig } from '../config/runtimeConfig';
import type { AiChatMessage } from '../ai/types';
import type { AiDesignDraft, AiOpsDraft } from '../ai/schema';
import {
  alignControls,
  duplicateControls,
  toggleSelection,
  type AlignMode,
} from '../utils/designerCommands';

let nextId = 1;

interface DesignerState {
  window: DesignWindow;
  controls: DesignControl[];
  selectedIds: string[];
  activeView: AppView;
  runtimeConfig: RuntimeConfig;
  codeLanguage: CodeLanguage;
  showCode: boolean;
  codePanelHeight: number;
  emojiPickerOpen: boolean;
  toolboxDragging: { type: string; icon: string; label: string } | null;
  dragGhostPos: { x: number; y: number } | null;
  clipboard: { controls: DesignControl[]; rootIds: string[] } | null;
  aiSessionId: string | null;
  aiMessages: AiChatMessage[];
  aiDraftPreview: AiDesignDraft | null;
  aiOpsPreview: AiOpsDraft | null;
  aiRawResponse: string;
  aiLastAppliedSnapshot: { window: DesignWindow; controls: DesignControl[] } | null;

  setWindow: (w: Partial<DesignWindow>) => void;
  setActiveView: (view: AppView) => void;
  setRuntimeConfig: (config: RuntimeConfig) => void;
  loadDesign: (window: DesignWindow, controls: DesignControl[]) => void;
  addControl: (
    type: string,
    x: number,
    y: number,
    parentMeta?: Pick<DesignControl, 'parentId' | 'parentKind' | 'parentTabIndex'>
  ) => string | null;
  updateControl: (id: string, updates: Partial<DesignControl>) => void;
  updateControlProp: (id: string, key: string, value: unknown) => void;
  removeControl: (id: string) => void;
  selectOnly: (id: string | null) => void;
  toggleSelect: (id: string, append: boolean) => void;
  setSelection: (ids: string[]) => void;
  moveControl: (id: string, x: number, y: number) => void;
  moveSelectionBy: (dx: number, dy: number) => void;
  resizeControl: (id: string, w: number, h: number) => void;
  setCodeLanguage: (lang: CodeLanguage) => void;
  setShowCode: (show: boolean) => void;
  setCodePanelHeight: (height: number) => void;
  setEmojiPickerOpen: (open: boolean) => void;
  duplicateControl: (id: string) => void;
  duplicateSelection: () => void;
  copySelection: () => void;
  pasteClipboard: () => void;
  alignSelection: (mode: AlignMode) => void;
  clearAll: () => void;
  startToolboxDrag: (type: string, icon: string, label: string) => void;
  updateDragGhost: (x: number, y: number) => void;
  endToolboxDrag: () => void;
  setAiSessionId: (sessionId: string | null) => void;
  setAiMessages: (messages: AiChatMessage[]) => void;
  appendAiMessage: (message: AiChatMessage) => void;
  clearAiConversation: () => void;
  setAiDraftPreview: (draft: AiDesignDraft | null) => void;
  setAiOpsPreview: (ops: AiOpsDraft | null) => void;
  setAiRawResponse: (text: string) => void;
  snapshotCurrentDesign: () => void;
  rollbackLastAiApply: () => void;
}

function collectDescendantIds(controls: DesignControl[], targetId: string): string[] {
  const ids = [targetId];
  for (const control of controls) {
    if (control.parentId === targetId) {
      ids.push(...collectDescendantIds(controls, control.id));
    }
  }
  return ids;
}

function getRootSelectedIds(controls: DesignControl[], selectedIds: string[]): string[] {
  return selectedIds.filter((candidate) =>
    !selectedIds.some((other) => other !== candidate && collectDescendantIds(controls, other).includes(candidate))
  );
}

function cloneControls(controls: DesignControl[]): DesignControl[] {
  return structuredClone(controls);
}

function updateNextIdSeed(controls: DesignControl[]): void {
  const maxNumericId = controls.reduce((max, control) => {
    const match = control.id.match(/_(\d+)$/);
    return match ? Math.max(max, Number(match[1])) : max;
  }, 0);
  nextId = Math.max(nextId, maxNumericId + 1);
}

export const useDesignerStore = create<DesignerState>((set) => ({
  window: {
    title: '我的应用',
    emoji: '🎨',
    width: 800,
    height: 600,
    titlebarColor: '#409EFF',
    bgColor: '#F5F7FA',
  },
  controls: [],
  selectedIds: [],
  activeView: 'designer',
  runtimeConfig: loadRuntimeConfig(),
  codeLanguage: 'python',
  showCode: true,
  codePanelHeight: 300,
  emojiPickerOpen: false,
  toolboxDragging: null,
  dragGhostPos: null,
  clipboard: null,
  aiSessionId: null,
  aiMessages: [],
  aiDraftPreview: null,
  aiOpsPreview: null,
  aiRawResponse: '',
  aiLastAppliedSnapshot: null,

  setWindow: (w) => set((s) => ({ window: { ...s.window, ...w } })),
  setActiveView: (activeView) => set({ activeView }),
  setRuntimeConfig: (runtimeConfig) => set({ runtimeConfig }),

  loadDesign: (window, controls) => {
    updateNextIdSeed(controls);
    set({
      window,
      controls,
      selectedIds: [],
      clipboard: null,
    });
  },

  addControl: (type, x, y, parentMeta) => {
    const def = getControlDef(type);
    if (!def) return null;
    const id = `${type}_${nextId++}`;
    const control: DesignControl = {
      id,
      type: def.type,
      name: id,
      x,
      y,
      width: def.defaultWidth,
      height: def.defaultHeight,
      parentId: parentMeta?.parentId ?? null,
      parentKind: parentMeta?.parentKind ?? 'window',
      parentTabIndex: parentMeta?.parentTabIndex ?? null,
      props: { ...def.defaultProps },
    };
    if (control.type === 'tabcontrol' && control.props.activeTab === undefined) {
      control.props.activeTab = 0;
    }
    set((s) => ({
      controls: [...s.controls, control],
      selectedIds: [id],
    }));
    return id;
  },

  updateControl: (id, updates) =>
    set((s) => ({
      controls: s.controls.map((c) =>
        c.id === id ? { ...c, ...updates } : c
      ),
    })),

  updateControlProp: (id, key, value) =>
    set((s) => ({
      controls: s.controls.map((c) =>
        c.id === id ? { ...c, props: { ...c.props, [key]: value } } : c
      ),
    })),

  removeControl: (id) =>
    set((s) => ({
      controls: s.controls.filter((c) => !collectDescendantIds(s.controls, id).includes(c.id)),
      selectedIds: s.selectedIds.filter((item) => item !== id),
    })),

  selectOnly: (id) => set({ selectedIds: id ? [id] : [] }),

  toggleSelect: (id, append) =>
    set((s) => ({
      selectedIds: id ? toggleSelection(s.selectedIds, id, append) : [],
    })),

  setSelection: (ids) => set({ selectedIds: ids }),

  moveControl: (id, x, y) =>
    set((s) => ({
      controls: s.controls.map((c) =>
        c.id === id ? { ...c, x: Math.max(0, x), y: Math.max(0, y) } : c
      ),
    })),

  moveSelectionBy: (dx, dy) =>
    set((s) => {
      const movableIds = new Set(getRootSelectedIds(s.controls, s.selectedIds));
      return {
        controls: s.controls.map((c) =>
          movableIds.has(c.id)
            ? { ...c, x: Math.max(0, c.x + dx), y: Math.max(0, c.y + dy) }
            : c
        ),
      };
    }),

  resizeControl: (id, w, h) =>
    set((s) => ({
      controls: s.controls.map((c) =>
        c.id === id ? { ...c, width: Math.max(20, w), height: Math.max(10, h) } : c
      ),
    })),

  setCodeLanguage: (lang) => set({ codeLanguage: lang }),
  setShowCode: (show) => set({ showCode: show }),
  setCodePanelHeight: (height) => set({ codePanelHeight: Math.max(160, Math.min(height, 700)) }),
  setEmojiPickerOpen: (emojiPickerOpen) => set({ emojiPickerOpen }),

  duplicateControl: (id) =>
    set((s) => {
      const src = s.controls.find((c) => c.id === id);
      if (!src) return s;
      const newId = `${src.type}_${nextId++}`;
      return {
        controls: [
          ...s.controls,
          { ...src, id: newId, name: newId, x: src.x + 20, y: src.y + 20 },
        ],
        selectedIds: [newId],
      };
    }),

  duplicateSelection: () =>
    set((s) => {
      if (s.selectedIds.length === 0) return s;
      const duplicated = duplicateControls(s.controls, getRootSelectedIds(s.controls, s.selectedIds));
      return {
        controls: duplicated.all,
        selectedIds: duplicated.created.map((control) => control.id),
      };
    }),

  copySelection: () =>
    set((s) => {
      if (s.selectedIds.length === 0) return s;
      const rootIds = getRootSelectedIds(s.controls, s.selectedIds);
      const controls = cloneControls(
        s.controls.filter((control) =>
          rootIds.includes(control.id) ||
          rootIds.some((id) => collectDescendantIds(s.controls, id).includes(control.id))
        )
      );
      return {
        clipboard: {
          controls,
          rootIds,
        },
      };
    }),

  pasteClipboard: () =>
    set((s) => {
      if (!s.clipboard) return s;
      const usedIds = new Set(s.controls.map((control) => control.id));
      const usedNames = new Set(s.controls.map((control) => control.name));
      const idMap = new Map<string, string>();

      const nextIdFor = (baseId: string): string => {
        let index = 1;
        let candidate = `${baseId}_paste_${index}`;
        while (usedIds.has(candidate)) {
          index += 1;
          candidate = `${baseId}_paste_${index}`;
        }
        usedIds.add(candidate);
        return candidate;
      };

      const nextNameFor = (baseName: string): string => {
        let index = 1;
        let candidate = `${baseName}_copy_${index}`;
        while (usedNames.has(candidate)) {
          index += 1;
          candidate = `${baseName}_copy_${index}`;
        }
        usedNames.add(candidate);
        return candidate;
      };

      const rootSourceIds = new Set(s.clipboard.rootIds);
      const rootCreatedIds: string[] = [];
      const created = s.clipboard.controls.map((control) => {
        const newId = nextIdFor(control.id);
        idMap.set(control.id, newId);
        if (rootSourceIds.has(control.id)) {
          rootCreatedIds.push(newId);
        }
        return {
          ...control,
          id: newId,
          name: nextNameFor(control.name),
          x: control.x + 20,
          y: control.y + 20,
        };
      }).map((control) => ({
        ...control,
        parentId: control.parentId ? (idMap.get(control.parentId) ?? control.parentId) : control.parentId,
      }));

      return {
        controls: [...s.controls, ...created],
        selectedIds: rootCreatedIds,
      };
    }),

  alignSelection: (mode) =>
    set((s) => ({
      controls: alignControls(s.controls, getRootSelectedIds(s.controls, s.selectedIds), mode),
    })),

  clearAll: () => set({ controls: [], selectedIds: [], clipboard: null }),

  startToolboxDrag: (type, icon, label) =>
    set({ toolboxDragging: { type, icon, label } }),

  updateDragGhost: (x, y) =>
    set({ dragGhostPos: { x, y } }),

  endToolboxDrag: () =>
    set({ toolboxDragging: null, dragGhostPos: null }),

  setAiSessionId: (aiSessionId) => set({ aiSessionId }),
  setAiMessages: (aiMessages) => set({ aiMessages }),
  appendAiMessage: (message) => set((s) => ({ aiMessages: [...s.aiMessages, message] })),
  clearAiConversation: () =>
    set({
      aiSessionId: null,
      aiMessages: [],
      aiDraftPreview: null,
      aiOpsPreview: null,
      aiRawResponse: '',
    }),
  setAiDraftPreview: (aiDraftPreview) => set({ aiDraftPreview, aiOpsPreview: null }),
  setAiOpsPreview: (aiOpsPreview) => set({ aiOpsPreview, aiDraftPreview: null }),
  setAiRawResponse: (aiRawResponse) => set({ aiRawResponse }),
  snapshotCurrentDesign: () =>
    set((s) => ({
      aiLastAppliedSnapshot: {
        window: structuredClone(s.window),
        controls: cloneControls(s.controls),
      },
    })),
  rollbackLastAiApply: () =>
    set((s) => {
      if (!s.aiLastAppliedSnapshot) {
        return s;
      }
      updateNextIdSeed(s.aiLastAppliedSnapshot.controls);
      return {
        window: structuredClone(s.aiLastAppliedSnapshot.window),
        controls: cloneControls(s.aiLastAppliedSnapshot.controls),
        selectedIds: [],
      };
    }),
}));
