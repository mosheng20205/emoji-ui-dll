# -*- coding: utf-8 -*-
"""
带界面的「编译 x64 DLL + 运行自动化测试」。

在仓库根目录执行:
  python examples/Python/run_all_tests_gui.py

或在 examples/Python 下:
  python run_all_tests_gui.py

窗口会保持打开，日志在文本框中；关闭窗口即退出（不会一闪而过）。
"""
from __future__ import annotations

import queue
import sys
import threading
import tkinter as tk
from tkinter import messagebox, scrolledtext, ttk

# 保证可 import 同目录模块
_SCRIPT_DIR = __file__
if _SCRIPT_DIR:
    import os

    _d = os.path.dirname(os.path.abspath(__file__))
    if _d not in sys.path:
        sys.path.insert(0, _d)


def _worker_build(q: queue.Queue, root: Path) -> None:
    try:
        from build_x64 import build_release_x64

        dll = build_release_x64(root)
        q.put(("log", "[编译] 成功: " + str(dll)))
        q.put(("done_build", 0))
    except Exception as e:
        q.put(("log", "[编译] 失败: " + str(e)))
        q.put(("done_build", 1))


def _worker_tests(q: queue.Queue) -> None:
    try:
        from automated_tests import run_all_automated_tests

        def emit(s: str) -> None:
            q.put(("log", s))

        code = run_all_automated_tests(emit)
        q.put(("done_tests", code))
    except Exception as e:
        q.put(("log", "[异常] " + str(e)))
        q.put(("done_tests", 1))


def main() -> None:
    from pathlib import Path

    from emoji_test_support import repo_root

    root = repo_root()
    win = tk.Tk()
    win.title("emoji_window — 编译 x64 DLL 与自动化测试")
    win.geometry("900x640")
    win.minsize(600, 400)

    frm = ttk.Frame(win, padding=8)
    frm.pack(fill=tk.BOTH, expand=True)

    row1 = ttk.Frame(frm)
    row1.pack(fill=tk.X, pady=(0, 6))

    status_var = tk.StringVar(value="就绪（需 64 位 Python）")
    ttk.Label(row1, textvariable=status_var).pack(side=tk.LEFT)

    log_q: queue.Queue = queue.Queue()
    txt = scrolledtext.ScrolledText(frm, wrap=tk.WORD, font=("Consolas", 9))
    txt.pack(fill=tk.BOTH, expand=True, pady=(0, 6))

    def append_log(s: str) -> None:
        txt.insert(tk.END, s + "\n")
        txt.see(tk.END)

    def busy(on: bool) -> None:
        for b in buttons:
            b.configure(state=tk.DISABLED if on else tk.NORMAL)

    buttons: list[ttk.Button] = []

    def poll_queue() -> None:
        try:
            while True:
                kind, payload = log_q.get_nowait()
                if kind == "log":
                    append_log(payload)
                elif kind == "done_build":
                    busy(False)
                    if payload == 0:
                        status_var.set("编译成功")
                    else:
                        status_var.set("编译失败")
                elif kind == "done_tests":
                    busy(False)
                    if payload == 0:
                        status_var.set("全部自动化测试通过")
                        messagebox.showinfo("结果", "全部自动化测试通过。", parent=win)
                    else:
                        status_var.set("部分测试失败")
                        messagebox.showwarning("结果", "存在失败项，请查看日志。", parent=win)
        except queue.Empty:
            pass
        win.after(120, poll_queue)

    def do_build() -> None:
        status_var.set("正在编译 Release|x64 …")
        busy(True)
        append_log("======== 开始编译 MSBuild Release|x64 ========")

        def run() -> None:
            _worker_build(log_q, root)

        threading.Thread(target=run, daemon=True).start()

    def do_tests_only() -> None:
        status_var.set("正在运行测试（复制 DLL + unittest + 脚本）…")
        busy(True)
        append_log("======== 开始自动化测试（不重新编译）========")

        def run() -> None:
            _worker_tests(log_q)

        threading.Thread(target=run, daemon=True).start()

    def do_build_and_test() -> None:
        status_var.set("正在编译并运行测试…")
        busy(True)
        append_log("======== 编译 + 自动化测试 ========")

        def run() -> None:
            try:
                from build_x64 import build_release_x64

                dll = build_release_x64(root)
                log_q.put(("log", "[编译] 成功: " + str(dll)))
            except Exception as e:
                log_q.put(("log", "[编译] 失败: " + str(e)))
                log_q.put(("done_tests", 1))
                return
            _worker_tests(log_q)

        threading.Thread(target=run, daemon=True).start()

    btn_row = ttk.Frame(frm)
    btn_row.pack(fill=tk.X)

    b1 = ttk.Button(btn_row, text="仅编译 x64 DLL", command=do_build)
    b1.pack(side=tk.LEFT, padx=(0, 8))
    buttons.append(b1)

    b2 = ttk.Button(btn_row, text="仅运行自动化测试", command=do_tests_only)
    b2.pack(side=tk.LEFT, padx=(0, 8))
    buttons.append(b2)

    b3 = ttk.Button(btn_row, text="编译并运行全部自动化测试", command=do_build_and_test)
    b3.pack(side=tk.LEFT, padx=(0, 8))
    buttons.append(b3)

    ttk.Label(
        frm,
        text=(
            "说明：脚本测试依赖本目录下的 emoji_window.dll（运行前会自动从 bin/x64/Release 复制）。"
            "含消息循环的交互脚本未纳入批跑，见 automated_tests.INTERACTIVE_TEST_SCRIPTS。"
        ),
        wraplength=860,
    ).pack(fill=tk.X, pady=(4, 0))

    append_log("仓库根目录: " + str(root))
    append_log("点击按钮开始；本窗口可关闭，不会自动退出控制台（若从命令行启动）。")
    win.after(100, poll_queue)
    win.mainloop()


if __name__ == "__main__":
    if sys.platform != "win32":
        print("仅支持 Windows", file=sys.stderr)
        sys.exit(1)
    main()
