"""
测试进度条扩展属性获取命令
测试 GetProgressBarColor, GetProgressBarBounds,
     GetProgressBarEnabled, GetProgressBarVisible, GetProgressBarShowText
需求: 10.1-10.5
"""
import ctypes
import sys
import os

dll_path = os.path.join(os.path.dirname(__file__), 'emoji_window.dll')
try:
    dll = ctypes.CDLL(dll_path)
except OSError as e:
    print(f"错误: 无法加载emoji_window.dll: {e}")
    sys.exit(1)

# ===== 函数原型定义 =====

dll.create_window_bytes_ex.restype = ctypes.c_void_p
dll.create_window_bytes_ex.argtypes = [ctypes.c_char_p, ctypes.c_int, ctypes.c_int, ctypes.c_int, ctypes.c_int]

dll.CreateProgressBar.restype = ctypes.c_void_p
dll.CreateProgressBar.argtypes = [
    ctypes.c_void_p,
    ctypes.c_int, ctypes.c_int, ctypes.c_int, ctypes.c_int,
    ctypes.c_int,
    ctypes.c_uint, ctypes.c_uint,
    ctypes.c_int,
    ctypes.c_uint
]

dll.GetProgressBarColor.restype = ctypes.c_int
dll.GetProgressBarColor.argtypes = [
    ctypes.c_void_p,
    ctypes.POINTER(ctypes.c_uint),
    ctypes.POINTER(ctypes.c_uint)
]

dll.GetProgressBarBounds.restype = ctypes.c_int
dll.GetProgressBarBounds.argtypes = [
    ctypes.c_void_p,
    ctypes.POINTER(ctypes.c_int),
    ctypes.POINTER(ctypes.c_int),
    ctypes.POINTER(ctypes.c_int),
    ctypes.POINTER(ctypes.c_int)
]

dll.GetProgressBarEnabled.restype = ctypes.c_int
dll.GetProgressBarEnabled.argtypes = [ctypes.c_void_p]

dll.GetProgressBarVisible.restype = ctypes.c_int
dll.GetProgressBarVisible.argtypes = [ctypes.c_void_p]

dll.GetProgressBarShowText.restype = ctypes.c_int
dll.GetProgressBarShowText.argtypes = [ctypes.c_void_p]

dll.EnableProgressBar.restype = None
dll.EnableProgressBar.argtypes = [ctypes.c_void_p, ctypes.c_int]

dll.ShowProgressBar.restype = None
dll.ShowProgressBar.argtypes = [ctypes.c_void_p, ctypes.c_int]

dll.SetProgressBarShowText.restype = None
dll.SetProgressBarShowText.argtypes = [ctypes.c_void_p, ctypes.c_int]

dll.destroy_window.restype = None
dll.destroy_window.argtypes = [ctypes.c_void_p]


def test_progressbar_properties():
    print("=" * 60)
    print("测试进度条扩展属性获取命令")
    print("=" * 60)
    passed = 0
    failed = 0

    title = "进度条属性测试".encode('utf-8')
    hwnd = dll.create_window_bytes_ex(title, len(title), 100, 100, 400)
    if not hwnd:
        print("✗ 创建窗口失败")
        return False

    fg = 0xFF409EFF  # 蓝色
    bg = 0xFFDCDCDC  # 灰色
    text_color = 0xFFFFFFFF  # 白色

    hPB = dll.CreateProgressBar(hwnd, 10, 10, 300, 30, 50, fg, bg, 1, text_color)
    if not hPB:
        print("✗ 创建进度条失败")
        dll.destroy_window(hwnd)
        return False
    print("✓ 创建进度条成功")

    # --- 测试 GetProgressBarColor ---
    print("\n[测试 GetProgressBarColor - 需求 10.1]")
    out_fg = ctypes.c_uint(0)
    out_bg = ctypes.c_uint(0)
    ret = dll.GetProgressBarColor(hPB, ctypes.byref(out_fg), ctypes.byref(out_bg))
    print(f"  返回值: {ret}, fg: 0x{out_fg.value:08X}, bg: 0x{out_bg.value:08X}")
    if ret == 0 and out_fg.value == fg and out_bg.value == bg:
        print("  ✓ GetProgressBarColor 正确")
        passed += 1
    else:
        print("  ✗ GetProgressBarColor 值不匹配")
        failed += 1

    # --- 测试 GetProgressBarBounds ---
    print("\n[测试 GetProgressBarBounds - 需求 10.2]")
    ox = ctypes.c_int(0)
    oy = ctypes.c_int(0)
    ow = ctypes.c_int(0)
    oh = ctypes.c_int(0)
    ret = dll.GetProgressBarBounds(hPB, ctypes.byref(ox), ctypes.byref(oy), ctypes.byref(ow), ctypes.byref(oh))
    print(f"  返回值: {ret}, x={ox.value}, y={oy.value}, w={ow.value}, h={oh.value}")
    if ret == 0 and ow.value == 300 and oh.value == 30:
        print("  ✓ GetProgressBarBounds 正确")
        passed += 1
    else:
        print("  ✗ GetProgressBarBounds 值不匹配")
        failed += 1

    # --- 测试 GetProgressBarEnabled ---
    print("\n[测试 GetProgressBarEnabled - 需求 10.3]")
    enabled = dll.GetProgressBarEnabled(hPB)
    print(f"  启用状态: {enabled} (期望 1=启用)")
    if enabled == 1:
        print("  ✓ GetProgressBarEnabled 默认启用正确")
        passed += 1
    else:
        print("  ✗ GetProgressBarEnabled 默认值不匹配")
        failed += 1

    dll.EnableProgressBar(hPB, 0)
    enabled_after = dll.GetProgressBarEnabled(hPB)
    print(f"  禁用后状态: {enabled_after} (期望 0=禁用)")
    if enabled_after == 0:
        print("  ✓ GetProgressBarEnabled 禁用状态正确")
        passed += 1
    else:
        print("  ✗ GetProgressBarEnabled 禁用状态不匹配")
        failed += 1
    dll.EnableProgressBar(hPB, 1)

    # --- 测试 GetProgressBarVisible ---
    print("\n[测试 GetProgressBarVisible - 需求 10.4]")
    visible = dll.GetProgressBarVisible(hPB)
    print(f"  可视状态: {visible} (期望 1=可见)")
    if visible == 1:
        print("  ✓ GetProgressBarVisible 默认可见正确")
        passed += 1
    else:
        print("  ✗ GetProgressBarVisible 默认值不匹配")
        failed += 1

    dll.ShowProgressBar(hPB, 0)
    visible_after = dll.GetProgressBarVisible(hPB)
    print(f"  隐藏后状态: {visible_after} (期望 0=不可见)")
    if visible_after == 0:
        print("  ✓ GetProgressBarVisible 隐藏状态正确")
        passed += 1
    else:
        print("  ✗ GetProgressBarVisible 隐藏状态不匹配")
        failed += 1

    # --- 测试 GetProgressBarShowText ---
    print("\n[测试 GetProgressBarShowText - 需求 10.5]")
    show_text = dll.GetProgressBarShowText(hPB)
    print(f"  显示文本状态: {show_text} (期望 1=显示)")
    if show_text == 1:
        print("  ✓ GetProgressBarShowText 默认显示正确")
        passed += 1
    else:
        print("  ✗ GetProgressBarShowText 默认值不匹配")
        failed += 1

    dll.SetProgressBarShowText(hPB, 0)
    show_text_after = dll.GetProgressBarShowText(hPB)
    print(f"  关闭后状态: {show_text_after} (期望 0=不显示)")
    if show_text_after == 0:
        print("  ✓ GetProgressBarShowText 关闭状态正确")
        passed += 1
    else:
        print("  ✗ GetProgressBarShowText 关闭状态不匹配")
        failed += 1

    # --- 测试无效句柄 ---
    print("\n[测试无效句柄]")
    r1 = dll.GetProgressBarColor(None, None, None)
    r2 = dll.GetProgressBarBounds(None, None, None, None, None)
    r3 = dll.GetProgressBarEnabled(None)
    r4 = dll.GetProgressBarVisible(None)
    r5 = dll.GetProgressBarShowText(None)
    print(f"  无效句柄返回: Color={r1}, Bounds={r2}, Enabled={r3}, Visible={r4}, ShowText={r5}")
    if all(v == -1 for v in [r1, r2, r3, r4, r5]):
        print("  ✓ 无效句柄均返回 -1")
        passed += 1
    else:
        print("  ✗ 部分无效句柄未返回 -1")
        failed += 1

    dll.destroy_window(hwnd)

    print("\n" + "=" * 60)
    print(f"测试结果: {passed} 通过, {failed} 失败")
    print("=" * 60)
    return failed == 0


if __name__ == "__main__":
    success = test_progressbar_properties()
    sys.exit(0 if success else 1)
