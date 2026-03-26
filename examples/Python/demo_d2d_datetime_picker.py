# -*- coding: utf-8 -*-
"""
D2D 日期时间选择器 — 可视化演示（阻塞消息循环，关闭主窗口后退出）。

依赖：bin/x64/Release/emoji_window.dll（可先运行 build_x64_dll.ps1）
"""
from __future__ import annotations

import ctypes
import sys
from ctypes import wintypes

import emoji_window_dll as ew


def _fmt_dt(dll, picker) -> str:
    y = ctypes.c_int()
    mo = ctypes.c_int()
    d = ctypes.c_int()
    hh = ctypes.c_int()
    mm = ctypes.c_int()
    ss = ctypes.c_int()
    dll.GetD2DDateTimePickerDateTime(
        picker,
        ctypes.byref(y),
        ctypes.byref(mo),
        ctypes.byref(d),
        ctypes.byref(hh),
        ctypes.byref(mm),
        ctypes.byref(ss),
    )
    pr = dll.GetD2DDateTimePickerPrecision(picker)
    names = {0: "年", 1: "年月日", 2: "到时", 3: "到分", 4: "到秒"}
    pname = names.get(pr, str(pr))
    return (
        f"精度: {pr}（{pname}）\n"
        f"{y.value}-{mo.value:02d}-{d.value:02d}  "
        f"{hh.value:02d}:{mm.value:02d}:{ss.value:02d}"
    )


def main() -> int:
    if sys.platform != "win32":
        print("仅支持 Windows")
        return 1

    dll = ew.load_emoji_window_dll()
    ew.setup_datetime_picker_functions(dll)
    ew.setup_label_functions(dll)
    ew.setup_message_loop_functions(dll)

    font = ew.utf8_bytes("Microsoft YaHei UI")
    fbuf = (ctypes.c_ubyte * len(font))(*font)

    hwnd = ew.create_window_utf8(
        dll, "D2D日期时间选择器 — Python 演示", -1, -1, 720, 520
    )
    if not hwnd:
        print("创建主窗口失败")
        return 1

    # 使用「到秒」精度，便于在面板中验证日期选中、时分秒与确定按钮
    picker = dll.CreateD2DDateTimePicker(
        hwnd,
        24,
        56,
        400,
        38,
        ew.DTP_YMDHMS,
        0xFF303133,
        0xFFFFFFFF,
        0xFFDCDFE6,
        ctypes.cast(fbuf, ctypes.c_void_p),
        len(font),
        14,
        0,
        0,
        0,
    )
    if not ctypes.cast(picker, ctypes.c_void_p).value:
        print("创建日期时间选择器失败")
        dll.destroy_window(hwnd)
        return 1

    hint = ew.utf8_bytes(
        "点击输入区或右侧箭头打开面板；选日后蓝圈保持；时分秒可点选段后用滚轮或上下键调整，左右键切换时/分/秒。\n"
        "点「确定」后写入并关闭面板。下方标签在值改变时刷新。"
    )
    hint_buf = (ctypes.c_ubyte * len(hint))(*hint)
    label = dll.CreateLabel(
        hwnd,
        24,
        120,
        660,
        140,
        ctypes.cast(hint_buf, ctypes.c_void_p),
        len(hint),
        0xFF606266,
        0xFFF5F7FA,
        ctypes.cast(fbuf, ctypes.c_void_p),
        len(font),
        12,
        0,
        0,
        0,
        0,
        1,
    )
    if not ctypes.cast(label, ctypes.c_void_p).value:
        print("创建标签失败")
        dll.destroy_window(hwnd)
        return 1

    def on_changed(_h: wintypes.HWND) -> None:
        s = ew.utf8_bytes(_fmt_dt(dll, picker))
        b = (ctypes.c_ubyte * len(s))(*s)
        dll.SetLabelText(label, ctypes.cast(b, ctypes.c_void_p), len(s))

    cb = dll._ValueChangedCallback(on_changed)
    dll.SetD2DDateTimePickerCallback(picker, cb)
    dll.SetD2DDateTimePickerDateTime(picker, 2024, 6, 15, 14, 30, 45)

    # 防止回调与字符串缓冲区被 GC
    globals()["_dtp_cb_keep"] = cb
    globals()["_dtp_font_buf"] = fbuf

    dll.set_message_loop_main_window(hwnd)
    print("主窗口已打开：请操作日期时间控件；关闭主窗口后程序结束。")
    print("（若从资源管理器双击运行，关闭窗口后控制台会等待按键再退出。）\n")
    dll.run_message_loop()
    print("消息循环已结束。")
    try:
        input("按回车键关闭控制台…")
    except EOFError:
        pass
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
