# -*- coding: utf-8 -*-
"""
集中定义「可无人值守运行」的测试（unittest + 无消息循环的脚本）。

含 run_message_loop / 需人工拖窗口 的脚本见 INTERACTIVE_TEST_SCRIPTS，不纳入自动批跑。
"""
from __future__ import annotations

import os
import subprocess
import sys
from pathlib import Path
from typing import Callable


def _child_env() -> dict[str, str]:
    """子进程使用 UTF-8 标准输出，避免 print(emoji) 在 GBK 控制台崩溃。"""
    env = os.environ.copy()
    env.setdefault("PYTHONUTF8", "1")
    env.setdefault("PYTHONIOENCODING", "utf-8")
    return env

# unittest 模块名（不含 .py，相对于 examples/Python）
UNITTEST_MODULES = (
    "test_d2d_datetime_picker",
    "test_treeview_sidebar",
)

# 运行 `python test_xxx.py` 且退出码表示成败（依赖本目录 emoji_window.dll）
SCRIPT_TESTS = (
    "test_tab_layout.py",
    "test_tab_appearance.py",
    "test_tab_content_bg.py",
    "test_tab_item_control.py",
    "test_tab_batch_ops.py",
    "test_tab_interaction.py",
    "test_tab_state_query.py",
    "test_titlebar_style.py",
)

# 需手动运行（阻塞在消息循环或演示）
INTERACTIVE_TEST_SCRIPTS = (
    "test_browser_layout.py",
    "test_dpi_check.py",
    "test_full_browser_layout.py",
    "test_non_dpi_aware.py",
    "test_resize_callback.py",
    "test_resize_exact.py",
    "test_resize_verify.py",
)


def _examples_dir() -> Path:
    return Path(__file__).resolve().parent


def run_unittests(emit: Callable[[str], None], timeout_sec: float = 300) -> bool:
    py_dir = _examples_dir()
    cmd = [sys.executable, "-m", "unittest"] + list(UNITTEST_MODULES) + ["-v"]
    try:
        r = subprocess.run(
            cmd,
            cwd=str(py_dir),
            capture_output=True,
            text=True,
            encoding="utf-8",
            errors="replace",
            timeout=timeout_sec,
            env=_child_env(),
        )
    except subprocess.TimeoutExpired:
        emit(f"[失败] unittest 超时 ({timeout_sec}s)")
        return False
    if r.stdout:
        for line in r.stdout.splitlines():
            emit(line)
    if r.stderr:
        for line in r.stderr.splitlines():
            emit("[stderr] " + line)
    ok = r.returncode == 0
    emit(f"[unittest] {'通过' if ok else '失败'} (退出码 {r.returncode})")
    return ok


def run_script_tests(emit: Callable[[str], None], timeout_sec: float = 180) -> bool:
    """逐个 subprocess 运行脚本测试。"""
    py_dir = _examples_dir()
    py = sys.executable
    all_ok = True
    for name in SCRIPT_TESTS:
        script = py_dir / name
        if not script.is_file():
            emit(f"[跳过] 不存在: {name}")
            all_ok = False
            continue
        emit(f"---- 运行 {name} ----")
        try:
            r = subprocess.run(
                [py, str(script)],
                cwd=str(py_dir),
                capture_output=True,
                text=True,
                encoding="utf-8",
                errors="replace",
                timeout=timeout_sec,
                env=_child_env(),
            )
        except subprocess.TimeoutExpired:
            emit(f"[失败] {name} 超时 ({timeout_sec}s)")
            all_ok = False
            continue
        if r.stdout:
            for line in r.stdout.splitlines():
                emit(line)
        if r.stderr:
            for line in r.stderr.splitlines():
                emit("[stderr] " + line)
        if r.returncode != 0:
            emit(f"[失败] {name} 退出码 {r.returncode}")
            all_ok = False
        else:
            emit(f"[通过] {name}")
    return all_ok


def _configure_stdio_utf8() -> None:
    """避免 Windows 控制台 GBK 下打印测试输出中的 Unicode 时崩溃。"""
    for stream in (sys.stdout, sys.stderr):
        if hasattr(stream, "reconfigure"):
            try:
                stream.reconfigure(encoding="utf-8", errors="replace")
            except Exception:
                pass


def run_all_automated_tests(emit: Callable[[str], None]) -> int:
    """
    复制 DLL → unittest → 脚本测试。
    返回进程退出码 0 表示全部成功。
    """
    _configure_stdio_utf8()
    from emoji_test_support import copy_release_dll_to_examples_python

    emit("复制 emoji_window.dll 到 examples/Python ...")
    try:
        copy_release_dll_to_examples_python()
    except Exception as e:
        emit(f"复制失败: {e}")
        return 1

    u_ok = run_unittests(emit)
    s_ok = run_script_tests(emit)
    if u_ok and s_ok:
        emit("")
        emit("======== 全部自动化测试通过 ========")
        return 0
    emit("")
    emit("======== 存在失败项，请查看上方日志 ========")
    return 1


def main() -> int:
    _configure_stdio_utf8()

    def emit(s: str) -> None:
        print(s)

    return run_all_automated_tests(emit)


if __name__ == "__main__":
    raise SystemExit(main())
