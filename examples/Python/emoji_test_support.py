# -*- coding: utf-8 -*-
"""
自动化测试共用：仓库根路径、Release|x64 DLL 路径、复制到本目录供 legacy 脚本加载。
"""
from __future__ import annotations

import shutil
import sys
from pathlib import Path


def repo_root() -> Path:
    return Path(__file__).resolve().parent.parent.parent


def release_dll_path() -> Path:
    return repo_root() / "bin" / "x64" / "Release" / "emoji_window.dll"


def examples_python_dir() -> Path:
    return Path(__file__).resolve().parent


def copy_release_dll_to_examples_python() -> Path:
    """
    将 bin/x64/Release/emoji_window.dll 复制为 examples/Python/emoji_window.dll，
    与旧版 test_*.py 中 CDLL('./emoji_window.dll') 一致。
    """
    src = release_dll_path()
    if not src.is_file():
        raise FileNotFoundError(
            f"未找到 {src}，请先编译 Release|x64（python examples/Python/build_x64.py）"
        )
    dst = examples_python_dir() / "emoji_window.dll"
    shutil.copy2(src, dst)
    return dst


def require_64bit_python() -> None:
    import struct

    if struct.calcsize("P") * 8 != 64:
        print("需要 64 位 Python 加载 x64 DLL", file=sys.stderr)
        sys.exit(2)
