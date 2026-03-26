# -*- coding: utf-8 -*-
"""加载 bin/x64/Release/emoji_window.dll 并封装 D2D 日期时间选择器相关 API（stdcall，64 位）。"""
from __future__ import annotations

import ctypes
import platform
import sys
from ctypes import wintypes
from pathlib import Path


def _repo_root() -> Path:
    return Path(__file__).resolve().parent.parent.parent


def load_emoji_window_dll() -> ctypes.CDLL:
    if sys.platform != "win32":
        raise OSError("仅支持 Windows")
    if platform.architecture()[0] != "64bit":
        raise OSError("请使用 64 位 Python 加载 x64 DLL")
    dll_path = _repo_root() / "bin" / "x64" / "Release" / "emoji_window.dll"
    if not dll_path.is_file():
        raise FileNotFoundError(
            f"未找到 {dll_path}，请先运行 build_x64_dll.ps1 或 MSBuild Release|x64"
        )
    return ctypes.CDLL(str(dll_path))


# 与 emoji_window.h 中 DateTimePickerPrecision 一致
DTP_YEAR = 0
DTP_YMD = 1
DTP_YMDH = 2
DTP_YMDHM = 3
DTP_YMDHMS = 4


def setup_datetime_picker_functions(dll: ctypes.CDLL) -> None:
    HWND = wintypes.HWND
    BOOL = wintypes.BOOL
    UINT32 = wintypes.UINT

    # 与其它 test_*.py 共用同一 DLL 映像时需反复设置正确签名
    dll.create_window_bytes.argtypes = [
        ctypes.c_void_p,
        ctypes.c_int,
        ctypes.c_int,
        ctypes.c_int,
        ctypes.c_int,
        ctypes.c_int,
    ]
    dll.create_window_bytes.restype = ctypes.c_void_p

    dll.destroy_window.argtypes = [HWND]
    dll.destroy_window.restype = None

    dll.GetWindowBounds.argtypes = [
        HWND,
        ctypes.POINTER(ctypes.c_int),
        ctypes.POINTER(ctypes.c_int),
        ctypes.POINTER(ctypes.c_int),
        ctypes.POINTER(ctypes.c_int),
    ]
    dll.GetWindowBounds.restype = ctypes.c_int

    dll.CreateD2DDateTimePicker.argtypes = [
        HWND,
        ctypes.c_int,
        ctypes.c_int,
        ctypes.c_int,
        ctypes.c_int,
        ctypes.c_int,
        UINT32,
        UINT32,
        UINT32,
        ctypes.c_void_p,
        ctypes.c_int,
        ctypes.c_int,
        BOOL,
        BOOL,
        BOOL,
    ]
    dll.CreateD2DDateTimePicker.restype = HWND

    dll.GetD2DDateTimePickerPrecision.argtypes = [HWND]
    dll.GetD2DDateTimePickerPrecision.restype = ctypes.c_int

    dll.SetD2DDateTimePickerPrecision.argtypes = [HWND, ctypes.c_int]
    dll.SetD2DDateTimePickerPrecision.restype = None

    dll.GetD2DDateTimePickerDateTime.argtypes = [
        HWND,
        ctypes.POINTER(ctypes.c_int),
        ctypes.POINTER(ctypes.c_int),
        ctypes.POINTER(ctypes.c_int),
        ctypes.POINTER(ctypes.c_int),
        ctypes.POINTER(ctypes.c_int),
        ctypes.POINTER(ctypes.c_int),
    ]
    dll.GetD2DDateTimePickerDateTime.restype = None

    dll.SetD2DDateTimePickerDateTime.argtypes = [
        HWND,
        ctypes.c_int,
        ctypes.c_int,
        ctypes.c_int,
        ctypes.c_int,
        ctypes.c_int,
        ctypes.c_int,
    ]
    dll.SetD2DDateTimePickerDateTime.restype = None

    ValueChangedCallback = ctypes.WINFUNCTYPE(None, HWND)
    dll.SetD2DDateTimePickerCallback.argtypes = [HWND, ValueChangedCallback]
    dll.SetD2DDateTimePickerCallback.restype = None
    # 供测试保存引用，避免被 GC
    dll._ValueChangedCallback = ValueChangedCallback

    dll.EnableD2DDateTimePicker.argtypes = [HWND, BOOL]
    dll.EnableD2DDateTimePicker.restype = None

    dll.ShowD2DDateTimePicker.argtypes = [HWND, BOOL]
    dll.ShowD2DDateTimePicker.restype = None

    dll.SetD2DDateTimePickerBounds.argtypes = [
        HWND,
        ctypes.c_int,
        ctypes.c_int,
        ctypes.c_int,
        ctypes.c_int,
    ]
    dll.SetD2DDateTimePickerBounds.restype = None


def setup_label_functions(dll: ctypes.CDLL) -> None:
    HWND = wintypes.HWND
    BOOL = wintypes.BOOL
    UINT32 = wintypes.UINT

    dll.CreateLabel.argtypes = [
        HWND,
        ctypes.c_int,
        ctypes.c_int,
        ctypes.c_int,
        ctypes.c_int,
        ctypes.c_void_p,
        ctypes.c_int,
        UINT32,
        UINT32,
        ctypes.c_void_p,
        ctypes.c_int,
        ctypes.c_int,
        BOOL,
        BOOL,
        BOOL,
        ctypes.c_int,
        BOOL,
    ]
    dll.CreateLabel.restype = ctypes.c_void_p

    dll.SetLabelText.argtypes = [HWND, ctypes.c_void_p, ctypes.c_int]
    dll.SetLabelText.restype = None


def setup_message_loop_functions(dll: ctypes.CDLL) -> None:
    HWND = wintypes.HWND
    dll.set_message_loop_main_window.argtypes = [HWND]
    dll.set_message_loop_main_window.restype = None
    dll.run_message_loop.argtypes = []
    dll.run_message_loop.restype = ctypes.c_int


def utf8_bytes(s: str) -> bytes:
    return s.encode("utf-8")


def create_window_utf8(dll: ctypes.CDLL, title: str, x: int, y: int, w: int, h: int):
    """create_window_bytes：标题为 UTF-8 字节，不含 \\0 结尾。调用前请先 setup_datetime_picker_functions。"""
    raw = utf8_bytes(title)
    buf = (ctypes.c_ubyte * len(raw))(*raw)
    hwnd = dll.create_window_bytes(
        ctypes.cast(buf, ctypes.c_void_p),
        len(raw),
        x,
        y,
        w,
        h,
    )
    addr = ctypes.cast(hwnd, ctypes.c_void_p).value
    if not addr:
        return None
    return hwnd

