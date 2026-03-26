# -*- coding: utf-8 -*-
"""
树形框 + 侧栏模式 API 自动化测试（64 位 Python + 64 位 emoji_window.dll）。

运行（仓库根目录）:
  python examples/Python/test_treeview_sidebar.py

或先编译再测:
  python examples/Python/build_and_test.py
"""
from __future__ import annotations

import ctypes
import os
import sys
import unittest
from ctypes import WINFUNCTYPE, byref, c_bool, c_float, c_int, c_ubyte, c_void_p, cast

# 保证可导入同目录模块
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

from emoji_window_treeview import (
    CALLBACK_NODE_MOVED,
    CALLBACK_NODE_SELECTED,
    bind_treeview_api,
    create_window,
    destroy_window,
    load_emoji_window,
)

HWND = c_void_p


def _h(v: int) -> HWND:
    return HWND(v if v else 0)


def _utf8_buf(s: str):
    raw = s.encode("utf-8")
    buf = (c_ubyte * len(raw))(*raw)
    ptr = cast(buf, ctypes.POINTER(c_ubyte))
    return buf, ptr, len(raw)


class TreeViewSidebarTests(unittest.TestCase):
    dll = None
    TreeViewCallback = None
    TreeViewMoveCallback = None
    _cb_keepalive = []

    @classmethod
    def setUpClass(cls):
        if ctypes.sizeof(ctypes.c_void_p) != 8:
            raise unittest.SkipTest("需要 64 位 Python")
        cls.dll = load_emoji_window()
        cls.TreeViewCallback = bind_treeview_api(cls.dll)
        cls.TreeViewMoveCallback = WINFUNCTYPE(None, c_int, c_int, c_int, c_void_p)
        # 移动回调
        @cls.TreeViewMoveCallback
        def _on_move(node_id, new_parent, idx, ctx):
            pass

        cls._cb_keepalive.append(_on_move)

    def setUp(self):
        self.hwnd = create_window(self.dll)
        self.assertNotEqual(self.hwnd, 0, "create_window 失败")
        r = self.dll.CreateTreeView(
            _h(self.hwnd),
            10,
            40,
            300,
            400,
            0xFFFFFFFF,
            0xFF303133,
            None,
        )
        self.tv = r.value if hasattr(r, "value") else int(r or 0)
        self.assertNotEqual(self.tv, 0, "CreateTreeView 失败")

    def tearDown(self):
        tv = getattr(self, "tv", 0)
        if tv:
            self.assertTrue(self.dll.DestroyTreeView(_h(tv)))
        if getattr(self, "hwnd", 0):
            destroy_window(self.dll, self.hwnd)
        self.tv = 0
        self.hwnd = 0

    def test_sidebar_and_colors_font(self):
        d = self.dll
        h = _h(self.tv)

        self.assertTrue(d.SetTreeViewSidebarMode(h, True))
        self.assertTrue(d.GetTreeViewSidebarMode(h))

        self.assertTrue(d.SetTreeViewRowHeight(h, ctypes.c_float(40.0)))
        self.assertAlmostEqual(d.GetTreeViewRowHeight(h), 40.0, places=3)

        self.assertTrue(d.SetTreeViewItemSpacing(h, ctypes.c_float(3.0)))
        self.assertAlmostEqual(d.GetTreeViewItemSpacing(h), 3.0, places=3)

        self.assertTrue(d.SetTreeViewTextColor(h, 0xFF303133))
        self.assertEqual(d.GetTreeViewTextColor(h), 0xFF303133)

        self.assertTrue(d.SetTreeViewSelectedBgColor(h, 0x334096E6))
        self.assertEqual(d.GetTreeViewSelectedBgColor(h), 0x334096E6)

        self.assertTrue(d.SetTreeViewSelectedForeColor(h, 0xFF4096E6))
        self.assertEqual(d.GetTreeViewSelectedForeColor(h), 0xFF4096E6)

        self.assertTrue(d.SetTreeViewHoverBgColor(h, 0xFFF5F7FA))
        self.assertEqual(d.GetTreeViewHoverBgColor(h), 0xFFF5F7FA)

        fam, p, ln = _utf8_buf("Segoe UI")
        self.assertTrue(d.SetTreeViewFont(h, p, ln, ctypes.c_float(14.0), 400, False))

        out_sz = c_float(0.0)
        out_w = c_int(0)
        out_i = c_bool(False)
        buf = (c_ubyte * 256)()
        self.assertTrue(
            d.GetTreeViewFont(
                h,
                cast(buf, ctypes.POINTER(c_ubyte)),
                256,
                byref(out_sz),
                byref(out_w),
                byref(out_i),
            )
        )
        self.assertGreater(out_sz.value, 0)

    def test_nodes_expand_select_text_scroll(self):
        d = self.dll
        h = _h(self.tv)

        d.ClearTree(h)

        t1, p1, n1 = _utf8_buf("root")
        r = d.AddRootNode(h, p1, n1, None, 0)
        self.assertGreater(r, 0)

        ic, pic, nic = _utf8_buf("\U0001F4E6")  # 📦
        r2 = d.AddRootNode(h, p1, n1, pic, nic)
        self.assertGreater(r2, 0)

        t3, p3, n3 = _utf8_buf("child")
        c = d.AddChildNode(h, r, p3, n3, None, 0)
        self.assertGreater(c, 0)

        self.assertEqual(d.GetNodeLevel(h, r), 0)
        self.assertEqual(d.GetNodeLevel(h, c), 1)
        self.assertEqual(d.GetNodeParent(h, c), r)

        self.assertTrue(d.ExpandNode(h, r))
        self.assertTrue(d.SetSelectedNode(h, c))
        self.assertEqual(d.GetSelectedNode(h), c)

        buf = (c_ubyte * 512)()
        ln = d.GetNodeText(h, c, cast(buf, ctypes.POINTER(c_ubyte)), 512)
        self.assertGreater(ln, 0)
        text = bytes(buf[:ln]).decode("utf-8")
        self.assertEqual(text, "child")

        self.assertTrue(d.ScrollToNode(h, c))

        self.assertTrue(d.SetTreeViewScrollPos(h, 0))
        self.assertGreaterEqual(d.GetTreeViewScrollPos(h), 0)

    def test_find_remove_collapse_expand_all(self):
        d = self.dll
        h = _h(self.tv)
        d.ClearTree(h)
        tx, px, nx = _utf8_buf("unique_xyz")
        nid = d.AddRootNode(h, px, nx, None, 0)
        self.assertGreater(nid, 0)
        fx, pfx, nfx = _utf8_buf("unique_xyz")
        found = d.FindNodeByText(h, pfx, nfx)
        self.assertEqual(found, nid)

        self.assertTrue(d.EnableTreeViewDragDrop(h, True))

        # 回调注册（防 GC：保留引用）
        @self.TreeViewCallback
        def on_sel(node_id, ctx):
            pass

        self._cb_keepalive.append(on_sel)
        self.assertTrue(d.SetTreeViewCallback(h, CALLBACK_NODE_SELECTED, on_sel))

        self.assertTrue(d.ExpandAll(h))
        self.assertTrue(d.CollapseAll(h))

    def test_move_node(self):
        d = self.dll
        h = _h(self.tv)
        d.SetTreeViewSidebarMode(h, True)
        d.ClearTree(h)

        a, pa, na = _utf8_buf("A")
        b, pb, nb = _utf8_buf("B")
        c, pc, nc = _utf8_buf("C")

        id_a = d.AddRootNode(h, pa, na, None, 0)
        id_b = d.AddChildNode(h, id_a, pb, nb, None, 0)
        id_c = d.AddChildNode(h, id_b, pc, nc, None, 0)

        moved = []

        @self.TreeViewMoveCallback
        def _on_move(nid, pid, idx, ctx):
            moved.append((nid, pid, idx))

        self._cb_keepalive.append(_on_move)
        self.assertTrue(d.SetTreeViewCallback(h, CALLBACK_NODE_MOVED, _on_move))

        # 将 C 移到根下
        self.assertTrue(d.MoveTreeViewNode(h, id_c, -1, 0))
        self.assertEqual(d.GetNodeParent(h, id_c), -1)
        self.assertEqual(len(moved), 1)

    def test_selection_callback_runs_synchronously_in_set_selected(self):
        """选中回调在 SetSelectedNode 内同步调用（当前实现无 PostMessage 延后）。"""
        d = self.dll
        h = _h(self.tv)
        d.ClearTree(h)
        a, pa, na = _utf8_buf("R")
        b, pb, nb = _utf8_buf("X")
        id_r = d.AddRootNode(h, pa, na, None, 0)
        id_x = d.AddChildNode(h, id_r, pb, nb, None, 0)
        d.ExpandNode(h, id_r)

        called = []

        @self.TreeViewCallback
        def on_sel(node_id, ctx):
            called.append(int(node_id))

        self._cb_keepalive.append(on_sel)
        self.assertTrue(d.SetTreeViewCallback(h, CALLBACK_NODE_SELECTED, on_sel))

        self.assertTrue(d.SetSelectedNode(h, id_x))
        self.assertEqual(called, [id_x])


def main():
    unittest.main(verbosity=2)


if __name__ == "__main__":
    main()
