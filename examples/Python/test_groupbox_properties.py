"""
测试分组框属性获取命令
测试 GetGroupBoxTitle, GetGroupBoxBounds, GetGroupBoxVisible,
     GetGroupBoxEnabled, GetGroupBoxColor
需求: 6.1-6.5
"""
import ctypes
import sys
import os
import time

# 加载DLL
dll_path = os.path.join(os.path.dirname(__file__), 'emoji_window.dll')
try:
    dll = ctypes.CDLL(dll_path)
except OSError as e:
    print(f"错误: 无法加载emoji_window.dll: {e}")
    sys.exit(1)

# ===== 函数原型定义 =====

dll.create_window_bytes_ex.restype = ctypes.c_void_p
dll.create_window_bytes_ex.argtypes = [ctypes.c_char_p, ctypes.c_int, ctypes.c_int, ctypes.c_int, ctypes.c_int]

dll.CreateGroupBox.restype = ctypes.c_void_p
dll.CreateGroupBox.argtypes = [
    ctypes.c_void_p,
    ctypes.c_int, ctypes.c_int, ctypes.c_int, ctypes.c_int,
    ctypes.c_char_p, ctypes.c_int,
    ctypes.c_uint, ctypes.c_uint,
]

dll.GetGroupBoxTitle.restype = ctypes.c_int
dll.GetGroupBoxTitle.argtypes = [ctypes.c_void_p, ctypes.c_char_p, ctypes.c_int]

dll.GetGroupBoxBounds.restype = ctypes.c_int
dll.GetGroupBoxBounds.argtypes = [
    ctypes.c_void_p,
    ctypes.POINTER(ctypes.c_int),
    ctypes.POINTER(ctypes.c_int),
    ctypes.POINTER(ctypes.c_int),
    ctypes.POINTER(ctypes.c_int),
]

dll.GetGroupBoxVisible.restype = ctypes.c_int
dll.GetGroupBoxVisible.argtypes = [ctypes.c_void_p]

dll.GetGroupBoxEnabled.restype = ctypes.c_int
dll.GetGroupBoxEnabled.argtypes = [ctypes.c_void_p]

dll.GetGroupBoxColor.restype = ctypes.c_int
dll.GetGroupBoxColor.argtypes = [
    ctypes.c_void_p,
    ctypes.POINTER(ctypes.c_uint),
    ctypes.POINTER(ctypes.c_uint),
]

dll.ShowGroupBox.restype = None
dll.ShowGroupBox.argtypes = [ctypes.c_void_p, ctypes.c_int]

dll.EnableGroupBox.restype = None
dll.EnableGroupBox.argtypes = [ctypes.c_void_p, ctypes.c_int]

dll.destroy_window.restype = None
dll.destroy_window.argtypes = [ctypes.c_void_p]


def argb(a, r, g, b):
    return (a << 24) | (r << 16) | (g << 8) | b


def create_window_and_groupbox():
    title = "测试窗口".encode('utf-8')
    hwnd = dll.create_window_bytes_ex(title, len(title), 600, 400, argb(255, 70, 130, 180))
    assert hwnd, "创建窗口失败"

    gb_title = "性别选择".encode('utf-8')
    hgb = dll.CreateGroupBox(
        hwnd,
        20, 30, 260, 150,
        gb_title, len(gb_title),
        argb(255, 100, 149, 237),
        argb(255, 245, 248, 255),
    )
    assert hgb, "创建分组框失败"
    return hwnd, hgb


def test_get_title():
    print("\n--- 测试 GetGroupBoxTitle ---")
    hwnd, hgb = create_window_and_groupbox()
    try:
        # 第一次调用获取长度
        length = dll.GetGroupBoxTitle(hgb, None, 0)
        print(f"  标题字节长度: {length}")
        assert length > 0, f"GetGroupBoxTitle 返回长度应 > 0，实际: {length}"

        # 第二次调用获取内容
        buf = ctypes.create_string_buffer(length)
        ret = dll.GetGroupBoxTitle(hgb, buf, length)
        assert ret > 0, f"第二次调用应返回 > 0，实际: {ret}"
        title = buf.raw[:ret].decode('utf-8', errors='replace')
        print(f"  标题内容: {title}")
        assert "性别选择" in title, f"标题应包含'性别选择'，实际: {title}"

        # 无效句柄
        ret_invalid = dll.GetGroupBoxTitle(None, None, 0)
        assert ret_invalid == -1, f"无效句柄应返回 -1，实际: {ret_invalid}"

        print("  ✅ GetGroupBoxTitle 测试通过")
        return True
    finally:
        dll.destroy_window(hwnd)


def test_get_bounds():
    print("\n--- 测试 GetGroupBoxBounds ---")
    hwnd, hgb = create_window_and_groupbox()
    try:
        x = ctypes.c_int()
        y = ctypes.c_int()
        w = ctypes.c_int()
        h = ctypes.c_int()
        ret = dll.GetGroupBoxBounds(hgb, ctypes.byref(x), ctypes.byref(y), ctypes.byref(w), ctypes.byref(h))
        print(f"  位置: ({x.value}, {y.value})  大小: {w.value}x{h.value}  返回值: {ret}")
        assert ret == 0, f"GetGroupBoxBounds 应返回 0，实际: {ret}"
        assert x.value == 20, f"X 应为 20，实际: {x.value}"
        assert y.value == 30, f"Y 应为 30，实际: {y.value}"
        assert w.value == 260, f"宽度应为 260，实际: {w.value}"
        assert h.value == 150, f"高度应为 150，实际: {h.value}"

        # 无效句柄
        ret_invalid = dll.GetGroupBoxBounds(None, ctypes.byref(x), ctypes.byref(y), ctypes.byref(w), ctypes.byref(h))
        assert ret_invalid == -1, f"无效句柄应返回 -1，实际: {ret_invalid}"

        print("  ✅ GetGroupBoxBounds 测试通过")
        return True
    finally:
        dll.destroy_window(hwnd)


def test_get_visible():
    print("\n--- 测试 GetGroupBoxVisible ---")
    hwnd, hgb = create_window_and_groupbox()
    try:
        # 默认可见
        vis = dll.GetGroupBoxVisible(hgb)
        print(f"  默认可视状态: {vis}")
        assert vis == 1, f"默认应可见(1)，实际: {vis}"

        # 隐藏后
        dll.ShowGroupBox(hgb, 0)
        vis = dll.GetGroupBoxVisible(hgb)
        print(f"  隐藏后可视状态: {vis}")
        assert vis == 0, f"隐藏后应不可见(0)，实际: {vis}"

        # 显示后
        dll.ShowGroupBox(hgb, 1)
        vis = dll.GetGroupBoxVisible(hgb)
        print(f"  显示后可视状态: {vis}")
        assert vis == 1, f"显示后应可见(1)，实际: {vis}"

        # 无效句柄
        ret_invalid = dll.GetGroupBoxVisible(None)
        assert ret_invalid == -1, f"无效句柄应返回 -1，实际: {ret_invalid}"

        print("  ✅ GetGroupBoxVisible 测试通过")
        return True
    finally:
        dll.destroy_window(hwnd)


def test_get_enabled():
    print("\n--- 测试 GetGroupBoxEnabled ---")
    hwnd, hgb = create_window_and_groupbox()
    try:
        # 默认启用
        enabled = dll.GetGroupBoxEnabled(hgb)
        print(f"  默认启用状态: {enabled}")
        assert enabled == 1, f"默认应启用(1)，实际: {enabled}"

        # 禁用后
        dll.EnableGroupBox(hgb, 0)
        enabled = dll.GetGroupBoxEnabled(hgb)
        print(f"  禁用后启用状态: {enabled}")
        assert enabled == 0, f"禁用后应为0，实际: {enabled}"

        # 启用后
        dll.EnableGroupBox(hgb, 1)
        enabled = dll.GetGroupBoxEnabled(hgb)
        print(f"  启用后启用状态: {enabled}")
        assert enabled == 1, f"启用后应为1，实际: {enabled}"

        # 无效句柄
        ret_invalid = dll.GetGroupBoxEnabled(None)
        assert ret_invalid == -1, f"无效句柄应返回 -1，实际: {ret_invalid}"

        print("  ✅ GetGroupBoxEnabled 测试通过")
        return True
    finally:
        dll.destroy_window(hwnd)


def test_get_color():
    print("\n--- 测试 GetGroupBoxColor ---")
    hwnd, hgb = create_window_and_groupbox()
    try:
        border_color = ctypes.c_uint()
        bg_color = ctypes.c_uint()
        ret = dll.GetGroupBoxColor(hgb, ctypes.byref(border_color), ctypes.byref(bg_color))
        print(f"  返回值: {ret}  边框色: 0x{border_color.value:08X}  背景色: 0x{bg_color.value:08X}")
        assert ret == 0, f"GetGroupBoxColor 应返回 0，实际: {ret}"

        expected_border = argb(255, 100, 149, 237)
        expected_bg = argb(255, 245, 248, 255)
        assert border_color.value == expected_border, \
            f"边框色应为 0x{expected_border:08X}，实际: 0x{border_color.value:08X}"
        assert bg_color.value == expected_bg, \
            f"背景色应为 0x{expected_bg:08X}，实际: 0x{bg_color.value:08X}"

        # 无效句柄
        ret_invalid = dll.GetGroupBoxColor(None, ctypes.byref(border_color), ctypes.byref(bg_color))
        assert ret_invalid == -1, f"无效句柄应返回 -1，实际: {ret_invalid}"

        print("  ✅ GetGroupBoxColor 测试通过")
        return True
    finally:
        dll.destroy_window(hwnd)


def main():
    print("=" * 60)
    print("测试分组框属性获取命令")
    print("=" * 60)

    tests = [
        test_get_title,
        test_get_bounds,
        test_get_visible,
        test_get_enabled,
        test_get_color,
    ]

    passed = 0
    failed = 0
    for t in tests:
        try:
            if t():
                passed += 1
            else:
                failed += 1
        except AssertionError as e:
            print(f"  ❌ 断言失败: {e}")
            failed += 1
        except Exception as e:
            print(f"  ❌ 异常: {e}")
            failed += 1

    print("\n" + "=" * 60)
    print(f"结果: {passed} 通过 / {failed} 失败")
    print("=" * 60)
    return failed == 0


if __name__ == "__main__":
    success = main()
    sys.exit(0 if success else 1)
