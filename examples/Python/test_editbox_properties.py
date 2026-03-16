"""
测试编辑框扩展属性获取命令
测试 GetEditBoxFont, GetEditBoxColor, GetEditBoxBounds,
     GetEditBoxAlignment, GetEditBoxEnabled, GetEditBoxVisible
需求: 8.1-8.6
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

dll.CreateEditBox.restype = ctypes.c_void_p
dll.CreateEditBox.argtypes = [
    ctypes.c_void_p,
    ctypes.c_int, ctypes.c_int, ctypes.c_int, ctypes.c_int,
    ctypes.c_char_p, ctypes.c_int,
    ctypes.c_uint, ctypes.c_uint,
    ctypes.c_char_p, ctypes.c_int,
    ctypes.c_int,
    ctypes.c_int, ctypes.c_int, ctypes.c_int,
    ctypes.c_int,
    ctypes.c_int, ctypes.c_int, ctypes.c_int, ctypes.c_int, ctypes.c_int
]

dll.GetEditBoxFont.restype = ctypes.c_int
dll.GetEditBoxFont.argtypes = [
    ctypes.c_void_p,
    ctypes.c_char_p, ctypes.c_int,
    ctypes.POINTER(ctypes.c_int),
    ctypes.POINTER(ctypes.c_int),
    ctypes.POINTER(ctypes.c_int),
    ctypes.POINTER(ctypes.c_int)
]

dll.GetEditBoxColor.restype = ctypes.c_int
dll.GetEditBoxColor.argtypes = [
    ctypes.c_void_p,
    ctypes.POINTER(ctypes.c_uint),
    ctypes.POINTER(ctypes.c_uint)
]

dll.GetEditBoxBounds.restype = ctypes.c_int
dll.GetEditBoxBounds.argtypes = [
    ctypes.c_void_p,
    ctypes.POINTER(ctypes.c_int),
    ctypes.POINTER(ctypes.c_int),
    ctypes.POINTER(ctypes.c_int),
    ctypes.POINTER(ctypes.c_int)
]

dll.GetEditBoxAlignment.restype = ctypes.c_int
dll.GetEditBoxAlignment.argtypes = [ctypes.c_void_p]

dll.GetEditBoxEnabled.restype = ctypes.c_int
dll.GetEditBoxEnabled.argtypes = [ctypes.c_void_p]

dll.GetEditBoxVisible.restype = ctypes.c_int
dll.GetEditBoxVisible.argtypes = [ctypes.c_void_p]

dll.EnableEditBox.restype = None
dll.EnableEditBox.argtypes = [ctypes.c_void_p, ctypes.c_int]

dll.ShowEditBox.restype = None
dll.ShowEditBox.argtypes = [ctypes.c_void_p, ctypes.c_int]

dll.destroy_window.restype = None
dll.destroy_window.argtypes = [ctypes.c_void_p]


def test_editbox_properties():
    print("=" * 60)
    print("测试编辑框扩展属性获取命令")
    print("=" * 60)
    passed = 0
    failed = 0

    title = "编辑框属性测试".encode('utf-8')
    hwnd = dll.create_window_bytes_ex(title, len(title), 100, 100, 400)
    if not hwnd:
        print("✗ 创建窗口失败")
        return False

    font_name = "微软雅黑".encode('utf-8')
    text = "测试文本".encode('utf-8')
    fg = 0xFF000000  # 黑色
    bg = 0xFFFFFFFF  # 白色

    # 创建编辑框（左对齐，单行，粗体）
    hEdit = dll.CreateEditBox(
        hwnd, 10, 10, 200, 30,
        text, len(text),
        fg, bg,
        font_name, len(font_name),
        14,
        1, 0, 0,   # bold=True, italic=False, underline=False
        0,         # 左对齐
        0, 0, 0, 1, 0  # multiline=False, readonly=False, password=False, has_border=True, vertical_center=False
    )

    if not hEdit:
        print("✗ 创建编辑框失败")
        dll.destroy_window(hwnd)
        return False
    print(f"✓ 创建编辑框成功")

    # --- 测试 GetEditBoxFont ---
    print("\n[测试 GetEditBoxFont - 需求 8.1]")
    font_size = ctypes.c_int(0)
    bold = ctypes.c_int(0)
    italic = ctypes.c_int(0)
    underline = ctypes.c_int(0)

    name_len = dll.GetEditBoxFont(hEdit, None, 0,
        ctypes.byref(font_size), ctypes.byref(bold),
        ctypes.byref(italic), ctypes.byref(underline))
    if name_len > 0:
        buf = ctypes.create_string_buffer(name_len)
        dll.GetEditBoxFont(hEdit, buf, name_len,
            ctypes.byref(font_size), ctypes.byref(bold),
            ctypes.byref(italic), ctypes.byref(underline))
        got_name = buf.raw.decode('utf-8')
        print(f"  字体名: {got_name}, 大小: {font_size.value}, 粗体: {bold.value}, 斜体: {italic.value}, 下划线: {underline.value}")
        if got_name == "微软雅黑" and font_size.value == 14 and bold.value == 1 and italic.value == 0:
            print("  ✓ GetEditBoxFont 正确")
            passed += 1
        else:
            print("  ✗ GetEditBoxFont 值不匹配")
            failed += 1
    else:
        print(f"  ✗ GetEditBoxFont 返回长度异常: {name_len}")
        failed += 1

    # --- 测试 GetEditBoxColor ---
    print("\n[测试 GetEditBoxColor - 需求 8.2]")
    out_fg = ctypes.c_uint(0)
    out_bg = ctypes.c_uint(0)
    ret = dll.GetEditBoxColor(hEdit, ctypes.byref(out_fg), ctypes.byref(out_bg))
    print(f"  返回值: {ret}, fg: 0x{out_fg.value:08X}, bg: 0x{out_bg.value:08X}")
    if ret == 0 and out_fg.value == fg and out_bg.value == bg:
        print("  ✓ GetEditBoxColor 正确")
        passed += 1
    else:
        print("  ✗ GetEditBoxColor 值不匹配")
        failed += 1

    # --- 测试 GetEditBoxBounds ---
    print("\n[测试 GetEditBoxBounds - 需求 8.3]")
    ox = ctypes.c_int(0)
    oy = ctypes.c_int(0)
    ow = ctypes.c_int(0)
    oh = ctypes.c_int(0)
    ret = dll.GetEditBoxBounds(hEdit, ctypes.byref(ox), ctypes.byref(oy), ctypes.byref(ow), ctypes.byref(oh))
    print(f"  返回值: {ret}, x={ox.value}, y={oy.value}, w={ow.value}, h={oh.value}")
    if ret == 0 and ow.value == 200 and oh.value == 30:
        print("  ✓ GetEditBoxBounds 正确")
        passed += 1
    else:
        print("  ✗ GetEditBoxBounds 值不匹配")
        failed += 1

    # --- 测试 GetEditBoxAlignment ---
    print("\n[测试 GetEditBoxAlignment - 需求 8.4]")
    align = dll.GetEditBoxAlignment(hEdit)
    print(f"  对齐方式: {align} (期望 0=左对齐)")
    if align == 0:
        print("  ✓ GetEditBoxAlignment 正确")
        passed += 1
    else:
        print("  ✗ GetEditBoxAlignment 值不匹配")
        failed += 1

    # --- 测试 GetEditBoxEnabled ---
    print("\n[测试 GetEditBoxEnabled - 需求 8.5]")
    enabled = dll.GetEditBoxEnabled(hEdit)
    print(f"  启用状态: {enabled} (期望 1=启用)")
    if enabled == 1:
        print("  ✓ GetEditBoxEnabled 默认启用正确")
        passed += 1
    else:
        print("  ✗ GetEditBoxEnabled 默认值不匹配")
        failed += 1

    dll.EnableEditBox(hEdit, 0)
    enabled_after = dll.GetEditBoxEnabled(hEdit)
    print(f"  禁用后状态: {enabled_after} (期望 0=禁用)")
    if enabled_after == 0:
        print("  ✓ GetEditBoxEnabled 禁用状态正确")
        passed += 1
    else:
        print("  ✗ GetEditBoxEnabled 禁用状态不匹配")
        failed += 1
    dll.EnableEditBox(hEdit, 1)

    # --- 测试 GetEditBoxVisible ---
    print("\n[测试 GetEditBoxVisible - 需求 8.6]")
    visible = dll.GetEditBoxVisible(hEdit)
    print(f"  可视状态: {visible} (期望 1=可见)")
    if visible == 1:
        print("  ✓ GetEditBoxVisible 默认可见正确")
        passed += 1
    else:
        print("  ✗ GetEditBoxVisible 默认值不匹配")
        failed += 1

    dll.ShowEditBox(hEdit, 0)
    visible_after = dll.GetEditBoxVisible(hEdit)
    print(f"  隐藏后状态: {visible_after} (期望 0=不可见)")
    if visible_after == 0:
        print("  ✓ GetEditBoxVisible 隐藏状态正确")
        passed += 1
    else:
        print("  ✗ GetEditBoxVisible 隐藏状态不匹配")
        failed += 1

    # --- 测试无效句柄 ---
    print("\n[测试无效句柄]")
    r1 = dll.GetEditBoxFont(None, None, 0, None, None, None, None)
    r2 = dll.GetEditBoxColor(None, None, None)
    r3 = dll.GetEditBoxBounds(None, None, None, None, None)
    r4 = dll.GetEditBoxAlignment(None)
    r5 = dll.GetEditBoxEnabled(None)
    r6 = dll.GetEditBoxVisible(None)
    print(f"  无效句柄返回: Font={r1}, Color={r2}, Bounds={r3}, Align={r4}, Enabled={r5}, Visible={r6}")
    if all(v == -1 for v in [r1, r2, r3, r4, r5, r6]):
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
    success = test_editbox_properties()
    sys.exit(0 if success else 1)
