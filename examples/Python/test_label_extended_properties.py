"""
测试标签扩展属性获取命令
测试 GetLabelFont, GetLabelColor, GetLabelBounds, GetLabelAlignment, GetLabelEnabled, GetLabelVisible
需求: 9.1-9.6
"""
import ctypes
import sys
import os

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

dll.CreateLabel.restype = ctypes.c_void_p
dll.CreateLabel.argtypes = [
    ctypes.c_void_p,
    ctypes.c_int, ctypes.c_int, ctypes.c_int, ctypes.c_int,
    ctypes.c_char_p, ctypes.c_int,
    ctypes.c_uint, ctypes.c_uint,
    ctypes.c_char_p, ctypes.c_int,
    ctypes.c_int,
    ctypes.c_int, ctypes.c_int, ctypes.c_int,
    ctypes.c_int,
    ctypes.c_int,
]

dll.SetLabelFont.restype = None
dll.SetLabelFont.argtypes = [
    ctypes.c_void_p, ctypes.c_char_p, ctypes.c_int,
    ctypes.c_int, ctypes.c_int, ctypes.c_int, ctypes.c_int
]

dll.SetLabelColor.restype = None
dll.SetLabelColor.argtypes = [ctypes.c_void_p, ctypes.c_uint, ctypes.c_uint]

dll.SetLabelBounds.restype = None
dll.SetLabelBounds.argtypes = [ctypes.c_void_p, ctypes.c_int, ctypes.c_int, ctypes.c_int, ctypes.c_int]

dll.EnableLabel.restype = None
dll.EnableLabel.argtypes = [ctypes.c_void_p, ctypes.c_int]

dll.ShowLabel.restype = None
dll.ShowLabel.argtypes = [ctypes.c_void_p, ctypes.c_int]

# 新增扩展属性获取函数
dll.GetLabelFont.restype = ctypes.c_int
dll.GetLabelFont.argtypes = [
    ctypes.c_void_p,
    ctypes.c_char_p, ctypes.c_int,
    ctypes.POINTER(ctypes.c_int),
    ctypes.POINTER(ctypes.c_int),
    ctypes.POINTER(ctypes.c_int),
    ctypes.POINTER(ctypes.c_int)
]

dll.GetLabelColor.restype = ctypes.c_int
dll.GetLabelColor.argtypes = [
    ctypes.c_void_p,
    ctypes.POINTER(ctypes.c_uint),
    ctypes.POINTER(ctypes.c_uint)
]

dll.GetLabelBounds.restype = ctypes.c_int
dll.GetLabelBounds.argtypes = [
    ctypes.c_void_p,
    ctypes.POINTER(ctypes.c_int),
    ctypes.POINTER(ctypes.c_int),
    ctypes.POINTER(ctypes.c_int),
    ctypes.POINTER(ctypes.c_int)
]

dll.GetLabelAlignment.restype = ctypes.c_int
dll.GetLabelAlignment.argtypes = [ctypes.c_void_p]

dll.GetLabelEnabled.restype = ctypes.c_int
dll.GetLabelEnabled.argtypes = [ctypes.c_void_p]

dll.GetLabelVisible.restype = ctypes.c_int
dll.GetLabelVisible.argtypes = [ctypes.c_void_p]

dll.destroy_window.restype = None
dll.destroy_window.argtypes = [ctypes.c_void_p]


def create_test_window():
    title = "标签扩展属性测试".encode('utf-8')
    return dll.create_window_bytes_ex(title, len(title), 100, 100, 400)


def create_label(hwnd, text, alignment=0):
    text_bytes = text.encode('utf-8')
    font_name = "微软雅黑".encode('utf-8')
    return dll.CreateLabel(
        hwnd,
        10, 10, 200, 30,
        text_bytes, len(text_bytes),
        0xFF000000, 0xFFFFFFFF,
        font_name, len(font_name),
        14,
        0, 0, 0,
        alignment,
        0,
    )


def test_label_extended_properties():
    print("=" * 60)
    print("测试标签扩展属性获取命令")
    print("=" * 60)

    hwnd = create_test_window()
    if not hwnd:
        print("✗ 创建窗口失败")
        return False

    print(f"✓ 创建窗口成功")

    hLabel = create_label(hwnd, "测试标签")
    if not hLabel:
        print("✗ 创建标签失败")
        dll.destroy_window(hwnd)
        return False

    print(f"✓ 创建标签成功")

    all_passed = True

    # ===== 测试 GetLabelFont (需求 9.1) =====
    print("\n--- 测试 GetLabelFont (需求 9.1) ---")

    font_name = "微软雅黑".encode('utf-8')
    dll.SetLabelFont(hLabel, font_name, len(font_name), 16, 1, 0, 1)

    # 第一次调用获取长度
    name_len = dll.GetLabelFont(hLabel, None, 0, None, None, None, None)
    if name_len < 0:
        print(f"✗ GetLabelFont 第一次调用失败: {name_len}")
        all_passed = False
    else:
        print(f"✓ 字体名长度: {name_len}")
        buf = ctypes.create_string_buffer(name_len + 1)
        font_size = ctypes.c_int(0)
        bold = ctypes.c_int(0)
        italic = ctypes.c_int(0)
        underline = ctypes.c_int(0)

        ret = dll.GetLabelFont(hLabel, buf, name_len,
                               ctypes.byref(font_size),
                               ctypes.byref(bold),
                               ctypes.byref(italic),
                               ctypes.byref(underline))

        if ret < 0:
            print(f"✗ GetLabelFont 第二次调用失败: {ret}")
            all_passed = False
        else:
            got_name = buf.raw[:ret].decode('utf-8')
            checks = [
                (got_name == "微软雅黑", f"字体名 '{got_name}' == '微软雅黑'"),
                (font_size.value == 16, f"字体大小 {font_size.value} == 16"),
                (bold.value == 1, f"粗体 {bold.value} == 1"),
                (italic.value == 0, f"斜体 {italic.value} == 0"),
                (underline.value == 1, f"下划线 {underline.value} == 1"),
            ]
            for ok, msg in checks:
                print(f"{'✓' if ok else '✗'} {msg}")
                if not ok:
                    all_passed = False

    # ===== 测试 GetLabelColor (需求 9.2) =====
    print("\n--- 测试 GetLabelColor (需求 9.2) ---")

    dll.SetLabelColor(hLabel, 0xFF0000FF, 0xFFFFFF00)

    fg = ctypes.c_uint(0)
    bg = ctypes.c_uint(0)
    ret = dll.GetLabelColor(hLabel, ctypes.byref(fg), ctypes.byref(bg))

    checks = [
        (ret == 0, f"返回值 {ret} == 0"),
        (fg.value == 0xFF0000FF, f"前景色 0x{fg.value:08X} == 0xFF0000FF"),
        (bg.value == 0xFFFFFF00, f"背景色 0x{bg.value:08X} == 0xFFFFFF00"),
    ]
    for ok, msg in checks:
        print(f"{'✓' if ok else '✗'} {msg}")
        if not ok:
            all_passed = False

    # ===== 测试 GetLabelBounds (需求 9.3) =====
    print("\n--- 测试 GetLabelBounds (需求 9.3) ---")

    dll.SetLabelBounds(hLabel, 20, 50, 180, 40)

    x = ctypes.c_int(0)
    y = ctypes.c_int(0)
    w = ctypes.c_int(0)
    h = ctypes.c_int(0)
    ret = dll.GetLabelBounds(hLabel, ctypes.byref(x), ctypes.byref(y),
                             ctypes.byref(w), ctypes.byref(h))

    checks = [
        (ret == 0, f"返回值 {ret} == 0"),
        (x.value == 20, f"x={x.value} == 20"),
        (y.value == 50, f"y={y.value} == 50"),
        (w.value == 180, f"width={w.value} == 180"),
        (h.value == 40, f"height={h.value} == 40"),
    ]
    for ok, msg in checks:
        print(f"{'✓' if ok else '✗'} {msg}")
        if not ok:
            all_passed = False

    # ===== 测试 GetLabelAlignment (需求 9.4) =====
    print("\n--- 测试 GetLabelAlignment (需求 9.4) ---")

    # 创建不同对齐方式的标签
    for expected_align, align_name in [(0, "左对齐"), (1, "居中"), (2, "右对齐")]:
        lbl = create_label(hwnd, "对齐测试", alignment=expected_align)
        if lbl:
            got = dll.GetLabelAlignment(lbl)
            ok = got == expected_align
            print(f"{'✓' if ok else '✗'} {align_name}: GetLabelAlignment={got} (期望 {expected_align})")
            if not ok:
                all_passed = False

    # ===== 测试 GetLabelEnabled (需求 9.5) =====
    print("\n--- 测试 GetLabelEnabled (需求 9.5) ---")

    lbl_en = create_label(hwnd, "启用测试")
    if lbl_en:
        # 默认启用
        enabled = dll.GetLabelEnabled(lbl_en)
        ok = enabled == 1
        print(f"{'✓' if ok else '✗'} 默认启用状态: {enabled} (期望 1)")
        if not ok:
            all_passed = False

        # 禁用
        dll.EnableLabel(lbl_en, 0)
        enabled = dll.GetLabelEnabled(lbl_en)
        ok = enabled == 0
        print(f"{'✓' if ok else '✗'} 禁用后状态: {enabled} (期望 0)")
        if not ok:
            all_passed = False

        # 重新启用
        dll.EnableLabel(lbl_en, 1)
        enabled = dll.GetLabelEnabled(lbl_en)
        ok = enabled == 1
        print(f"{'✓' if ok else '✗'} 重新启用后状态: {enabled} (期望 1)")
        if not ok:
            all_passed = False

    # ===== 测试 GetLabelVisible (需求 9.6) =====
    print("\n--- 测试 GetLabelVisible (需求 9.6) ---")

    lbl_vis = create_label(hwnd, "可视测试")
    if lbl_vis:
        # 默认可见
        visible = dll.GetLabelVisible(lbl_vis)
        ok = visible == 1
        print(f"{'✓' if ok else '✗'} 默认可视状态: {visible} (期望 1)")
        if not ok:
            all_passed = False

        # 隐藏
        dll.ShowLabel(lbl_vis, 0)
        visible = dll.GetLabelVisible(lbl_vis)
        ok = visible == 0
        print(f"{'✓' if ok else '✗'} 隐藏后状态: {visible} (期望 0)")
        if not ok:
            all_passed = False

        # 重新显示
        dll.ShowLabel(lbl_vis, 1)
        visible = dll.GetLabelVisible(lbl_vis)
        ok = visible == 1
        print(f"{'✓' if ok else '✗'} 重新显示后状态: {visible} (期望 1)")
        if not ok:
            all_passed = False

    # ===== 测试无效句柄 =====
    print("\n--- 测试无效句柄 ---")

    invalid = ctypes.c_void_p(0x12345678)
    checks = [
        (dll.GetLabelFont(invalid, None, 0, None, None, None, None) == -1, "GetLabelFont 无效句柄返回 -1"),
        (dll.GetLabelColor(invalid, None, None) == -1, "GetLabelColor 无效句柄返回 -1"),
        (dll.GetLabelBounds(invalid, None, None, None, None) == -1, "GetLabelBounds 无效句柄返回 -1"),
        (dll.GetLabelAlignment(invalid) == -1, "GetLabelAlignment 无效句柄返回 -1"),
        (dll.GetLabelVisible(invalid) == -1, "GetLabelVisible 无效句柄返回 -1"),
    ]
    for ok, msg in checks:
        print(f"{'✓' if ok else '✗'} {msg}")
        if not ok:
            all_passed = False

    dll.destroy_window(hwnd)

    print("\n" + "=" * 60)
    if all_passed:
        print("✓ 所有测试通过")
    else:
        print("✗ 部分测试失败")
    print("=" * 60)

    return all_passed


if __name__ == "__main__":
    success = test_label_extended_properties()
    sys.exit(0 if success else 1)
