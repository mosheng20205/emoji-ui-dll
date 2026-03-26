# -*- coding: utf-8 -*-
"""
ctypes 绑定：emoji_window.dll 树形框（含侧栏模式）导出函数。
DLL 导出名为英文；与 C 头文件 treeview_window.h / emoji_window.def 一致。
"""
from __future__ import annotations

import ctypes
import os
from ctypes import (
    WINFUNCTYPE,
    byref,
    c_bool,
    c_float,
    c_int,
    c_uint,
    c_ubyte,
    c_void_p,
)

# ---------------------------------------------------------------------------
# HWND 与 BOOL
# ---------------------------------------------------------------------------
# 与 HWND 一致，用指针宽整型便于 ctypes 传递句柄
HWND = ctypes.c_void_p


def _dll_path() -> str:
    """按当前 Python 位数选择 bin/x64 或 bin/Win32 下的 Release DLL。"""
    root = os.path.abspath(os.path.join(os.path.dirname(__file__), "..", ".."))
    use_x64 = ctypes.sizeof(ctypes.c_void_p) == 8
    sub = "x64" if use_x64 else "Win32"
    rel = os.path.join(root, "bin", sub, "Release", "emoji_window.dll")
    if os.path.isfile(rel):
        return rel
    # 兼容旧路径（仅 x64）
    legacy = os.path.join(root, "bin", "x64", "Release", "emoji_window.dll")
    if os.path.isfile(legacy):
        return legacy
    here = os.path.join(os.path.dirname(__file__), "emoji_window.dll")
    if os.path.isfile(here):
        return here
    return rel


def load_emoji_window(dll_path: str | None = None):
    path = dll_path or _dll_path()
    if not os.path.isfile(path):
        arch = "x64" if ctypes.sizeof(ctypes.c_void_p) == 8 else "Win32"
        raise FileNotFoundError(
            f"未找到 DLL: {path}（请先 MSBuild Release|{arch} 生成 emoji_window.dll）"
        )
    # WinDLL = __stdcall
    return ctypes.WinDLL(path)


def bind_treeview_api(dll):
    """为已加载的 WinDLL 设置树形框相关 argtypes/restype（最小必需集）。"""

    dll.CreateTreeView.argtypes = [HWND, c_int, c_int, c_int, c_int, c_uint, c_uint, c_void_p]
    dll.CreateTreeView.restype = HWND

    dll.DestroyTreeView.argtypes = [HWND]
    dll.DestroyTreeView.restype = c_bool

    dll.ClearTree.argtypes = [HWND]
    dll.ClearTree.restype = c_bool

    dll.AddRootNode.argtypes = [HWND, ctypes.POINTER(c_ubyte), c_int, ctypes.POINTER(c_ubyte), c_int]
    dll.AddRootNode.restype = c_int

    dll.AddChildNode.argtypes = [HWND, c_int, ctypes.POINTER(c_ubyte), c_int, ctypes.POINTER(c_ubyte), c_int]
    dll.AddChildNode.restype = c_int

    dll.ExpandNode.argtypes = [HWND, c_int]
    dll.ExpandNode.restype = c_bool

    dll.SetSelectedNode.argtypes = [HWND, c_int]
    dll.SetSelectedNode.restype = c_bool

    dll.GetSelectedNode.argtypes = [HWND]
    dll.GetSelectedNode.restype = c_int

    dll.GetNodeText.argtypes = [HWND, c_int, ctypes.POINTER(c_ubyte), c_int]
    dll.GetNodeText.restype = c_int

    dll.SetTreeViewSidebarMode.argtypes = [HWND, c_bool]
    dll.SetTreeViewSidebarMode.restype = c_bool

    dll.GetTreeViewSidebarMode.argtypes = [HWND]
    dll.GetTreeViewSidebarMode.restype = c_bool

    dll.SetTreeViewRowHeight.argtypes = [HWND, c_float]
    dll.SetTreeViewRowHeight.restype = c_bool

    dll.GetTreeViewRowHeight.argtypes = [HWND]
    dll.GetTreeViewRowHeight.restype = c_float

    dll.SetTreeViewItemSpacing.argtypes = [HWND, c_float]
    dll.SetTreeViewItemSpacing.restype = c_bool

    dll.GetTreeViewItemSpacing.argtypes = [HWND]
    dll.GetTreeViewItemSpacing.restype = c_float

    for name in (
        "SetTreeViewTextColor",
        "GetTreeViewTextColor",
        "SetTreeViewSelectedBgColor",
        "GetTreeViewSelectedBgColor",
        "SetTreeViewSelectedForeColor",
        "GetTreeViewSelectedForeColor",
        "SetTreeViewHoverBgColor",
        "GetTreeViewHoverBgColor",
    ):
        fn = getattr(dll, name)
        if name.startswith("Set"):
            fn.argtypes = [HWND, c_uint]
            fn.restype = c_bool
        else:
            fn.argtypes = [HWND]
            fn.restype = c_uint

    dll.SetTreeViewFont.argtypes = [HWND, ctypes.POINTER(c_ubyte), c_int, c_float, c_int, c_bool]
    dll.SetTreeViewFont.restype = c_bool

    dll.GetTreeViewFont.argtypes = [
        HWND,
        ctypes.POINTER(c_ubyte),
        c_int,
        ctypes.POINTER(c_float),
        ctypes.POINTER(c_int),
        ctypes.POINTER(c_bool),
    ]
    dll.GetTreeViewFont.restype = c_bool

    dll.MoveTreeViewNode.argtypes = [HWND, c_int, c_int, c_int]
    dll.MoveTreeViewNode.restype = c_bool

    dll.ScrollToNode.argtypes = [HWND, c_int]
    dll.ScrollToNode.restype = c_bool

    # 主导出名为 SetTreeViewScrollPos / GetTreeViewScrollPos；另含别名 SetScrollPos / GetScrollPos
    for pair in (
        ("SetTreeViewScrollPos", "GetTreeViewScrollPos"),
        ("SetScrollPos", "GetScrollPos"),
    ):
        s, g = pair
        getattr(dll, s).argtypes = [HWND, c_int]
        getattr(dll, s).restype = c_bool
        getattr(dll, g).argtypes = [HWND]
        getattr(dll, g).restype = c_int

    dll.EnableTreeViewDragDrop.argtypes = [HWND, c_bool]
    dll.EnableTreeViewDragDrop.restype = c_bool

    # void __stdcall cb(int node_id, void* ctx)
    TreeViewCallback = WINFUNCTYPE(None, c_int, c_void_p)
    dll.SetTreeViewCallback.argtypes = [HWND, c_int, c_void_p]
    dll.SetTreeViewCallback.restype = c_bool

    dll.GetNodeParent.argtypes = [HWND, c_int]
    dll.GetNodeParent.restype = c_int

    dll.GetNodeLevel.argtypes = [HWND, c_int]
    dll.GetNodeLevel.restype = c_int

    dll.GetNodeChildCount.argtypes = [HWND, c_int]
    dll.GetNodeChildCount.restype = c_int

    dll.FindNodeByText.argtypes = [HWND, ctypes.POINTER(c_ubyte), c_int]
    dll.FindNodeByText.restype = c_int

    dll.ExpandAll.argtypes = [HWND]
    dll.ExpandAll.restype = c_bool

    dll.CollapseAll.argtypes = [HWND]
    dll.CollapseAll.restype = c_bool

    dll.RemoveNode.argtypes = [HWND, c_int]
    dll.RemoveNode.restype = c_bool

    return TreeViewCallback


def utf8_bytes(s: str) -> tuple[ctypes.Array, int]:
    b = s.encode("utf-8")
    buf = (c_ubyte * len(b))(*b)
    return buf, len(b)


def create_window(dll):
    dll.create_window.argtypes = [ctypes.c_char_p, c_int, c_int, c_int, c_int]
    dll.create_window.restype = HWND
    dll.destroy_window.argtypes = [HWND]
    dll.destroy_window.restype = None
    title = "Python TreeView Test".encode("utf-8")
    h = dll.create_window(title, -1, -1, 640, 480)
    return ctypes.cast(h, ctypes.c_void_p).value or 0


def destroy_window(dll, hwnd: int) -> None:
    dll.destroy_window(HWND(hwnd))


CALLBACK_NODE_SELECTED = 1
CALLBACK_NODE_EXPANDED = 2
CALLBACK_NODE_COLLAPSED = 3
CALLBACK_NODE_MOVED = 8
