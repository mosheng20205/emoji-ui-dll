# -*- coding: utf-8 -*-
"""
编译 Release|x64 的 emoji_window.dll 并运行 examples/Python 下的自动化测试。

用法（在仓库根目录）:
  python examples/Python/build_and_test.py

带界面、可查看日志请使用:
  python examples/Python/run_all_tests_gui.py

依赖: Visual Studio 2022（MSBuild）、64 位 Python 3。
"""
from __future__ import annotations

import os
import sys

# 与 run_all_tests_gui / automated_tests 共用逻辑
_SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))
if _SCRIPT_DIR not in sys.path:
    sys.path.insert(0, _SCRIPT_DIR)


def main() -> int:
    from build_x64 import build_release_x64
    from emoji_test_support import repo_root

    root = repo_root()
    dll = build_release_x64(root)
    print("编译成功:", dll)
    from automated_tests import run_all_automated_tests

    code = run_all_automated_tests(lambda s: print(s))
    print("完成，退出码:", code)
    return code


if __name__ == "__main__":
    try:
        raise SystemExit(main())
    except Exception as e:
        print("失败:", e, file=sys.stderr)
        raise SystemExit(1)
