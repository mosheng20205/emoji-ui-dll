# -*- coding: utf-8 -*-
"""
复现《侧栏折叠菜单升级版_方案一分组框》流程：树 + 6 分组框 + ShowGroupBox 切换。

用于验证 DLL 逻辑；64 位 Python 用 bin/x64/Release，32 位 Python 用 bin/Win32/Release。

长时间驻留：python examples/Python/test_sidebar_groupbox_stay_alive.py --seconds 60

运行（仓库根目录）:
  python examples/Python/test_sidebar_groupbox_scheme1.py
"""
from __future__ import annotations

import ctypes
import os
import sys
import time
import unittest
from ctypes import WINFUNCTYPE, byref, c_bool, c_float, c_int, c_uint, c_ubyte, c_void_p, cast
from ctypes import wintypes

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

from emoji_window_treeview import (
    CALLBACK_NODE_SELECTED,
    bind_treeview_api,
    create_window,
    destroy_window,
    load_emoji_window,
    utf8_bytes,
)

user32 = ctypes.windll.user32
PM_REMOVE = 0x0001
WM_QUIT = 0x0012


def _pump_ms(duration_ms: int) -> None:
    msg = wintypes.MSG()
    end = time.perf_counter() + duration_ms / 1000.0
    while time.perf_counter() < end:
        while user32.PeekMessageW(byref(msg), 0, 0, 0, PM_REMOVE):
            if msg.message == WM_QUIT:
                return
            user32.TranslateMessage(byref(msg))
            user32.DispatchMessageW(byref(msg))
        time.sleep(0.005)


def _h(v: int) -> c_void_p:
    return c_void_p(v if v else 0)


def argb(a: int, r: int, g: int, b: int) -> int:
    return (a << 24) | (r << 16) | (g << 8) | b


def _setup_groupbox_label(dll: ctypes.WinDLL) -> None:
    HWND = c_void_p
    dll.CreateGroupBox.argtypes = [
        HWND,
        c_int,
        c_int,
        c_int,
        c_int,
        ctypes.c_void_p,
        c_int,
        c_uint,
        c_uint,
        ctypes.c_void_p,
        c_int,
        c_int,
        c_bool,
        c_bool,
        c_bool,
    ]
    dll.CreateGroupBox.restype = HWND

    dll.CreateLabel.argtypes = [
        HWND,
        c_int,
        c_int,
        c_int,
        c_int,
        ctypes.c_void_p,
        c_int,
        c_uint,
        c_uint,
        ctypes.c_void_p,
        c_int,
        c_int,
        c_bool,
        c_bool,
        c_bool,
        c_int,
        c_bool,
    ]
    dll.CreateLabel.restype = HWND

    dll.AddChildToGroup.argtypes = [HWND, HWND]
    dll.AddChildToGroup.restype = None

    dll.ShowGroupBox.argtypes = [HWND, c_bool]
    dll.ShowGroupBox.restype = None


class SidebarGroupBoxScheme1Test(unittest.TestCase):
    dll = None
    TreeViewCallback = None
    _cb_keepalive = []

    @classmethod
    def setUpClass(cls):
        try:
            cls.dll = load_emoji_window()
        except FileNotFoundError as e:
            raise unittest.SkipTest(str(e)) from e
        cls.TreeViewCallback = bind_treeview_api(cls.dll)
        _setup_groupbox_label(cls.dll)

    def setUp(self):
        self._cb_keepalive = []

    def tearDown(self):
        tv = getattr(self, "tv", 0)
        if tv:
            self.dll.DestroyTreeView(_h(tv))
        if getattr(self, "hwnd", 0):
            destroy_window(self.dll, self.hwnd)
        self.tv = 0
        self.hwnd = 0

    def test_tree_groupbox_switch_no_crash(self):
        """创建主窗口、树、6 个分组框与标签（父=主窗口 + AddChildToGroup），隐藏全部再显示其一；回调切换。"""
        d = self.dll
        self.hwnd = create_window(d)
        self.assertNotEqual(self.hwnd, 0)

        r = d.CreateTreeView(
            _h(self.hwnd),
            10,
            40,
            320,
            520,
            0xFFFFFFFF,
            0xFF303133,
            None,
        )
        self.tv = ctypes.cast(r, ctypes.c_void_p).value or 0
        self.assertNotEqual(self.tv, 0)

        htv = _h(self.tv)
        d.SetTreeViewSidebarMode(htv, True)
        d.ClearTree(htv)

        font_msyh, ln_font = utf8_bytes("微软雅黑")

        groups = []
        # 与易语言文档相同区域
        gx, gy, gw, gh = 340, 40, 540, 500
        border = argb(255, 200, 200, 200)
        bg = argb(255, 255, 255, 255)

        titles = [
            "商户中心 · 内容区",
            "订单管理 · 内容区",
            "通道管理 · 内容区",
            "财务中心 · 内容区",
            "API · 内容区",
            "账号管理 · 内容区",
        ]
        hints = [
            "欢迎使用商户中心。",
            "订单相关：列表、筛选、导出等。",
            "通道、轮询、支付配置。",
            "对账、结算、报表。",
            "接口文档、密钥、调试工具。",
            "个人资料、安全、套餐等。",
        ]

        for t, hint in zip(titles, hints):
            tb, nb = utf8_bytes(t)
            ptr_t = cast(tb, ctypes.POINTER(c_ubyte))
            gb = d.CreateGroupBox(
                _h(self.hwnd),
                gx,
                gy,
                gw,
                gh,
                ptr_t,
                nb,
                border,
                bg,
                cast(font_msyh, ctypes.POINTER(c_ubyte)),
                ln_font,
                14,
                False,
                False,
                False,
            )
            gbh = ctypes.cast(gb, ctypes.c_void_p).value or 0
            self.assertNotEqual(gbh, 0, f"CreateGroupBox failed for {t}")
            hb, nhb = utf8_bytes(hint)
            ph = cast(hb, ctypes.POINTER(c_ubyte))
            # 与 comprehensive_demo：主窗口为父，使用客户区绝对坐标（分组框内容区约：左 350、上 65）
            lbl = d.CreateLabel(
                _h(self.hwnd),
                370,
                105,
                500,
                60,
                ph,
                nhb,
                argb(255, 48, 49, 51),
                argb(255, 255, 255, 255),
                cast(font_msyh, ctypes.POINTER(c_ubyte)),
                ln_font,
                14,
                False,
                False,
                False,
                0,
                True,
            )
            lh = ctypes.cast(lbl, ctypes.c_void_p).value or 0
            self.assertNotEqual(lh, 0)
            d.AddChildToGroup(_h(gbh), _h(lh))
            groups.append(gbh)

        # 树节点（简化：6 根节点，与分组一一对应）
        ids = []
        for name in ("商户中心", "订单管理", "通道管理", "财务中心", "API", "账号管理"):
            buf, n = utf8_bytes(name)
            p = cast(buf, ctypes.POINTER(c_ubyte))
            nid = d.AddRootNode(htv, p, n, None, 0)
            self.assertGreater(nid, 0)
            ids.append(nid)

        node_api = ids[4]
        id_to_idx = {ids[i]: i for i in range(6)}

        selected = []

        @self.TreeViewCallback
        def on_sel(node_id, ctx):
            selected.append(int(node_id))
            idx = id_to_idx.get(int(node_id), 0)
            for i, gbh in enumerate(groups):
                d.ShowGroupBox(_h(gbh), i == idx)

        self._cb_keepalive.append(on_sel)
        self.assertTrue(d.SetTreeViewCallback(htv, CALLBACK_NODE_SELECTED, on_sel))

        for gbh in groups:
            d.ShowGroupBox(_h(gbh), False)
        d.ShowGroupBox(_h(groups[4]), True)

        self.assertTrue(d.SetSelectedNode(htv, node_api))
        _pump_ms(80)
        self.assertTrue(len(selected) >= 1)

        self.assertTrue(d.SetSelectedNode(htv, ids[0]))
        self.assertTrue(d.SetSelectedNode(htv, ids[2]))
        _pump_ms(80)


def main():
    unittest.main(verbosity=2)


if __name__ == "__main__":
    main()
