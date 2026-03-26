# -*- coding: utf-8 -*-
"""
编译 Release|x64 的 emoji_window.dll（供 64 位 Python 使用）。

可在仓库根目录执行:
  python examples/Python/build_x64.py

也可被其它脚本 import: build_release_x64(repo_root)
"""
from __future__ import annotations

import os
import subprocess
import sys
from pathlib import Path


def find_msbuild() -> str:
    vswhere = os.path.expandvars(
        r"%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"
    )
    if not os.path.isfile(vswhere):
        raise FileNotFoundError("未找到 vswhere.exe，请安装 Visual Studio")
    r = subprocess.run(
        [
            vswhere,
            "-latest",
            "-requires",
            "Microsoft.Component.MSBuild",
            "-find",
            r"MSBuild\**\Bin\MSBuild.exe",
        ],
        capture_output=True,
        text=True,
        check=False,
    )
    lines = [x.strip() for x in r.stdout.splitlines() if x.strip()]
    if not lines:
        raise FileNotFoundError("vswhere 未返回 MSBuild 路径")
    return lines[0]


def repo_root_from_this_file() -> Path:
    return Path(__file__).resolve().parent.parent.parent


def build_release_x64(root: Path | None = None) -> Path:
    """
    使用 MSBuild 编译 emoji_window.sln（Release|x64）。
    返回生成的 emoji_window.dll 路径；失败则 raise CalledProcessError。
    """
    root = root or repo_root_from_this_file()
    sln = root / "emoji_window.sln"
    if not sln.is_file():
        raise FileNotFoundError(f"未找到解决方案: {sln}")
    msbuild = find_msbuild()
    subprocess.run(
        [
            msbuild,
            str(sln),
            "/p:Configuration=Release",
            "/p:Platform=x64",
            "/m",
            "/v:minimal",
        ],
        cwd=str(root),
        check=True,
    )
    dll = root / "bin" / "x64" / "Release" / "emoji_window.dll"
    if not dll.is_file():
        raise FileNotFoundError(f"编译后未找到: {dll}")
    return dll


def main() -> int:
    try:
        dll = build_release_x64()
        print("OK:", dll)
        return 0
    except Exception as e:
        print("失败:", e, file=sys.stderr)
        return 1


if __name__ == "__main__":
    raise SystemExit(main())
