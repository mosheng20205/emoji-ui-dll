# -*- coding: utf-8 -*-
"""
侧栏 + 分组框（与易语言方案一相近）：创建窗口后长时间泵消息，用于观察是否崩溃。

默认驻留一段时间再退出，不会立刻关闭（可用 --seconds 调整）。

用法（仓库根目录）:
  python examples/Python/test_sidebar_groupbox_stay_alive.py
  python examples/Python/test_sidebar_groupbox_stay_alive.py --seconds 60
  python examples/Python/test_sidebar_groupbox_stay_alive.py --seconds 120 --stress-switch 2
"""
from __future__ import annotations

import argparse
import ctypes
import os
import sys
import time
from ctypes import byref, c_bool, c_int, c_uint, c_ubyte, c_void_p, cast
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


def _pump_once() -> bool:
    """处理当前队列中的消息；若收到 WM_QUIT 返回 False。"""
    msg = wintypes.MSG()
    while user32.PeekMessageW(byref(msg), 0, 0, 0, PM_REMOVE):
        if msg.message == WM_QUIT:
            return False
        user32.TranslateMessage(byref(msg))
        user32.DispatchMessageW(byref(msg))
    return True


def main() -> int:
    ap = argparse.ArgumentParser(description="侧栏+分组框驻留测试（长时间消息泵）")
    ap.add_argument(
        "--seconds",
        type=float,
        default=30.0,
        help="驻留并泵消息的秒数（默认 30）",
    )
    ap.add_argument(
        "--stress-switch",
        type=float,
        default=0.0,
        metavar="SEC",
        help="每隔多少秒切换一次选中节点（0 表示不加压，默认 0）",
    )
    args = ap.parse_args()

    py_bits = ctypes.sizeof(ctypes.c_void_p) * 8
    print(f"[stay_alive] Python {py_bits} 位，加载 DLL …", flush=True)
    try:
        dll = load_emoji_window()
    except FileNotFoundError as e:
        print(e, file=sys.stderr)
        return 1

    TreeViewCallback = bind_treeview_api(dll)
    _setup_groupbox_label(dll)

    selected: list[int] = []
    _cb_keepalive = []

    hwnd = create_window(dll)
    if not hwnd:
        print("create_window 失败", file=sys.stderr)
        return 2
    print(f"[stay_alive] 主窗口 HWND={hwnd}", flush=True)

    r = dll.CreateTreeView(
        _h(hwnd),
        10,
        40,
        320,
        520,
        0xFFFFFFFF,
        0xFF303133,
        None,
    )
    tv = ctypes.cast(r, ctypes.c_void_p).value or 0
    if not tv:
        print("CreateTreeView 失败", file=sys.stderr)
        destroy_window(dll, hwnd)
        return 2
    htv = _h(tv)

    dll.SetTreeViewSidebarMode(htv, True)
    dll.ClearTree(htv)

    font_msyh, ln_font = utf8_bytes("微软雅黑")
    groups: list[int] = []
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
    hints = ["提示 A", "提示 B", "提示 C", "提示 D", "提示 E", "提示 F"]

    for t, hint in zip(titles, hints):
        tb, nb = utf8_bytes(t)
        ptr_t = cast(tb, ctypes.POINTER(c_ubyte))
        gb = dll.CreateGroupBox(
            _h(hwnd),
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
        if not gbh:
            print(f"CreateGroupBox 失败: {t}", file=sys.stderr)
            dll.DestroyTreeView(htv)
            destroy_window(dll, hwnd)
            return 2
        hb, nhb = utf8_bytes(hint)
        ph = cast(hb, ctypes.POINTER(c_ubyte))
        lbl = dll.CreateLabel(
            _h(hwnd),
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
        if not lh:
            dll.DestroyTreeView(htv)
            destroy_window(dll, hwnd)
            return 2
        dll.AddChildToGroup(_h(gbh), _h(lh))
        groups.append(gbh)

    ids: list[int] = []
    for name in ("商户中心", "订单管理", "通道管理", "财务中心", "API", "账号管理"):
        buf, n = utf8_bytes(name)
        p = cast(buf, ctypes.POINTER(c_ubyte))
        nid = dll.AddRootNode(htv, p, n, None, 0)
        if nid <= 0:
            print("AddRootNode 失败", file=sys.stderr)
            dll.DestroyTreeView(htv)
            destroy_window(dll, hwnd)
            return 2
        ids.append(nid)

    id_to_idx = {ids[i]: i for i in range(6)}

    @TreeViewCallback
    def on_sel(node_id, ctx):
        selected.append(int(node_id))
        idx = id_to_idx.get(int(node_id), 0)
        for i, gbh in enumerate(groups):
            dll.ShowGroupBox(_h(gbh), i == idx)

    _cb_keepalive.append(on_sel)
    if not dll.SetTreeViewCallback(htv, CALLBACK_NODE_SELECTED, on_sel):
        print("SetTreeViewCallback 失败", file=sys.stderr)
        dll.DestroyTreeView(htv)
        destroy_window(dll, hwnd)
        return 2

    for gbh in groups:
        dll.ShowGroupBox(_h(gbh), False)
    dll.ShowGroupBox(_h(groups[4]), True)

    # 与易语言启动流程类似：先选中再泵消息（触发投递的选中回调）
    dll.SetSelectedNode(htv, ids[4])
    _pump_once()
    _pump_once()

    print(
        f"[stay_alive] 开始驻留 {args.seconds}s（可观察窗口；Ctrl+C 结束本脚本但窗口可能仍存在）",
        flush=True,
    )

    t_end = time.perf_counter() + args.seconds
    last_hb = 0.0
    last_stress = 0.0
    rot = 0

    while time.perf_counter() < t_end:
        if not user32.IsWindow(_h(hwnd)):
            print("[stay_alive] 主窗口已无效，进程可能已崩溃", flush=True)
            return 3
        if not _pump_once():
            print("[stay_alive] WM_QUIT", flush=True)
            break

        now = time.perf_counter()
        if now - last_hb >= 5.0:
            last_hb = now
            remain = max(0.0, t_end - now)
            print(
                f"[stay_alive] 仍运行 剩余约 {remain:.1f}s  回调次数={len(selected)}",
                flush=True,
            )

        if args.stress_switch > 0 and (now - last_stress) >= args.stress_switch:
            last_stress = now
            nid = ids[rot % len(ids)]
            rot += 1
            dll.SetSelectedNode(htv, nid)
            _pump_once()

        time.sleep(0.002)

    print(
        f"[stay_alive] 驻留结束；选中回调触发次数={len(selected)}  最后选中节点={dll.GetSelectedNode(htv)}",
        flush=True,
    )

    dll.DestroyTreeView(htv)
    destroy_window(dll, hwnd)
    print("[stay_alive] 已销毁窗口，正常退出", flush=True)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
