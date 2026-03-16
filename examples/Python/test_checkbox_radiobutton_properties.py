"""
测试复选框和单选按钮属性命令
测试 GetCheckBoxText, SetCheckBoxFont, GetCheckBoxFont, SetCheckBoxColor, GetCheckBoxColor
测试 GetRadioButtonText, SetRadioButtonFont, GetRadioButtonFont, SetRadioButtonColor, GetRadioButtonColor
需求: 4.1-4.5, 5.1-5.5
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

# 复选框
dll.CreateCheckBox.restype = ctypes.c_void_p
dll.CreateCheckBox.argtypes = [
    ctypes.c_void_p,
    ctypes.c_int, ctypes.c_int, ctypes.c_int, ctypes.c_int,
    ctypes.c_char_p, ctypes.c_int,
    ctypes.c_int,
    ctypes.c_uint, ctypes.c_uint,
]

dll.SetCheckBoxText.restype = None
dll.SetCheckBoxText.argtypes = [ctypes.c_void_p, ctypes.c_char_p, ctypes.c_int]

dll.GetCheckBoxText.restype = ctypes.c_int
dll.GetCheckBoxText.argtypes = [ctypes.c_void_p, ctypes.c_char_p, ctypes.c_int]

dll.SetCheckBoxFont.restype = None
dll.SetCheckBoxFont.argtypes = [
    ctypes.c_void_p, ctypes.c_char_p, ctypes.c_int,
    ctypes.c_int, ctypes.c_int, ctypes.c_int, ctypes.c_int
]

dll.GetCheckBoxFont.restype = ctypes.c_int
dll.GetCheckBoxFont.argtypes = [
    ctypes.c_void_p,
    ctypes.c_char_p, ctypes.c_int,
    ctypes.POINTER(ctypes.c_int),
    ctypes.POINTER(ctypes.c_int),
    ctypes.POINTER(ctypes.c_int),
    ctypes.POINTER(ctypes.c_int)
]

dll.SetCheckBoxColor.restype = None
dll.SetCheckBoxColor.argtypes = [ctypes.c_void_p, ctypes.c_uint, ctypes.c_uint]

dll.GetCheckBoxColor.restype = ctypes.c_int
dll.GetCheckBoxColor.argtypes = [
    ctypes.c_void_p,
    ctypes.POINTER(ctypes.c_uint),
    ctypes.POINTER(ctypes.c_uint)
]

# 单选按钮
dll.CreateRadioButton.restype = ctypes.c_void_p
dll.CreateRadioButton.argtypes = [
    ctypes.c_void_p,
    ctypes.c_int, ctypes.c_int, ctypes.c_int, ctypes.c_int,
    ctypes.c_char_p, ctypes.c_int,
    ctypes.c_int,
    ctypes.c_uint, ctypes.c_uint,
]

dll.SetRadioButtonText.restype = None
dll.SetRadioButtonText.argtypes = [ctypes.c_void_p, ctypes.c_char_p, ctypes.c_int]

dll.GetRadioButtonText.restype = ctypes.c_int
dll.GetRadioButtonText.argtypes = [ctypes.c_void_p, ctypes.c_char_p, ctypes.c_int]

dll.SetRadioButtonFont.restype = None
dll.SetRadioButtonFont.argtypes = [
    ctypes.c_void_p, ctypes.c_char_p, ctypes.c_int,
    ctypes.c_int, ctypes.c_int, ctypes.c_int, ctypes.c_int
]

dll.GetRadioButtonFont.restype = ctypes.c_int
dll.GetRadioButtonFont.argtypes = [
    ctypes.c_void_p,
    ctypes.c_char_p, ctypes.c_int,
    ctypes.POINTER(ctypes.c_int),
    ctypes.POINTER(ctypes.c_int),
    ctypes.POINTER(ctypes.c_int),
    ctypes.POINTER(ctypes.c_int)
]

dll.SetRadioButtonColor.restype = None
dll.SetRadioButtonColor.argtypes = [ctypes.c_void_p, ctypes.c_uint, ctypes.c_uint]

dll.GetRadioButtonColor.restype = ctypes.c_int
dll.GetRadioButtonColor.argtypes = [
    ctypes.c_void_p,
    ctypes.POINTER(ctypes.c_uint),
    ctypes.POINTER(ctypes.c_uint)
]

dll.destroy_window.restype = None
dll.destroy_window.argtypes = [ctypes.c_void_p]


def create_test_window():
    title = "复选框单选按钮属性测试".encode('utf-8')
    return dll.create_window_bytes_ex(title, len(title), 100, 100, 400)


def create_checkbox(hwnd, text, x=10, y=10):
    text_bytes = text.encode('utf-8')
    return dll.CreateCheckBox(
        hwnd, x, y, 200, 30,
        text_bytes, len(text_bytes),
        0,
        0xFF303133, 0xFFFFFFFF,
    )


def create_radiobutton(hwnd, text, x=10, y=50):
    text_bytes = text.encode('utf-8')
    return dll.CreateRadioButton(
        hwnd, x, y, 200, 30,
        text_bytes, len(text_bytes),
        0,
        0xFF303133, 0xFFFFFFFF,
    )


def test_checkbox_properties():
    """测试复选框属性命令 (需求 4.1-4.5)"""
    print("\n" + "=" * 60)
    print("测试复选框属性命令 (需求 4.1-4.5)")
    print("=" * 60)

    hwnd = create_test_window()
    if not hwnd:
        print("✗ 创建窗口失败")
        return False

    hCB = create_checkbox(hwnd, "初始文本")
    if not hCB:
        print("✗ 创建复选框失败")
        dll.destroy_window(hwnd)
        return False

    print("✓ 创建复选框成功")
    all_passed = True

    # ===== 测试 GetCheckBoxText (需求 4.1) =====
    print("\n--- 测试 GetCheckBoxText (需求 4.1) ---")

    # 第一次调用获取长度
    text_len = dll.GetCheckBoxText(hCB, None, 0)
    if text_len < 0:
        print(f"✗ GetCheckBoxText 第一次调用失败: {text_len}")
        all_passed = False
    else:
        buf = ctypes.create_string_buffer(text_len + 1)
        ret = dll.GetCheckBoxText(hCB, buf, text_len)
        got_text = buf.raw[:ret].decode('utf-8')
        ok = got_text == "初始文本"
        print(f"{'✓' if ok else '✗'} 初始文本: '{got_text}' (期望 '初始文本')")
        if not ok:
            all_passed = False

    # 修改文本后再获取
    new_text = "修改后的文本"
    new_text_bytes = new_text.encode('utf-8')
    dll.SetCheckBoxText(hCB, new_text_bytes, len(new_text_bytes))

    text_len = dll.GetCheckBoxText(hCB, None, 0)
    if text_len >= 0:
        buf = ctypes.create_string_buffer(text_len + 1)
        ret = dll.GetCheckBoxText(hCB, buf, text_len)
        got_text = buf.raw[:ret].decode('utf-8')
        ok = got_text == new_text
        print(f"{'✓' if ok else '✗'} 修改后文本: '{got_text}' (期望 '{new_text}')")
        if not ok:
            all_passed = False

    # ===== 测试 SetCheckBoxFont / GetCheckBoxFont (需求 4.2, 4.4) =====
    print("\n--- 测试 SetCheckBoxFont / GetCheckBoxFont (需求 4.2, 4.4) ---")

    font_name = "微软雅黑"
    font_bytes = font_name.encode('utf-8')
    dll.SetCheckBoxFont(hCB, font_bytes, len(font_bytes), 18, 1, 1, 0)

    name_len = dll.GetCheckBoxFont(hCB, None, 0, None, None, None, None)
    if name_len < 0:
        print(f"✗ GetCheckBoxFont 第一次调用失败: {name_len}")
        all_passed = False
    else:
        buf = ctypes.create_string_buffer(name_len + 1)
        font_size = ctypes.c_int(0)
        bold = ctypes.c_int(0)
        italic = ctypes.c_int(0)
        underline = ctypes.c_int(0)

        ret = dll.GetCheckBoxFont(hCB, buf, name_len,
                                  ctypes.byref(font_size),
                                  ctypes.byref(bold),
                                  ctypes.byref(italic),
                                  ctypes.byref(underline))
        if ret < 0:
            print(f"✗ GetCheckBoxFont 第二次调用失败: {ret}")
            all_passed = False
        else:
            got_name = buf.raw[:ret].decode('utf-8')
            checks = [
                (got_name == font_name, f"字体名 '{got_name}' == '{font_name}'"),
                (font_size.value == 18, f"字体大小 {font_size.value} == 18"),
                (bold.value == 1, f"粗体 {bold.value} == 1"),
                (italic.value == 1, f"斜体 {italic.value} == 1"),
                (underline.value == 0, f"下划线 {underline.value} == 0"),
            ]
            for ok, msg in checks:
                print(f"{'✓' if ok else '✗'} {msg}")
                if not ok:
                    all_passed = False

    # ===== 测试 SetCheckBoxColor / GetCheckBoxColor (需求 4.3, 4.5) =====
    print("\n--- 测试 SetCheckBoxColor / GetCheckBoxColor (需求 4.3, 4.5) ---")

    dll.SetCheckBoxColor(hCB, 0xFF0000FF, 0xFFFFFF00)

    fg = ctypes.c_uint(0)
    bg = ctypes.c_uint(0)
    ret = dll.GetCheckBoxColor(hCB, ctypes.byref(fg), ctypes.byref(bg))

    checks = [
        (ret == 0, f"返回值 {ret} == 0"),
        (fg.value == 0xFF0000FF, f"前景色 0x{fg.value:08X} == 0xFF0000FF"),
        (bg.value == 0xFFFFFF00, f"背景色 0x{bg.value:08X} == 0xFFFFFF00"),
    ]
    for ok, msg in checks:
        print(f"{'✓' if ok else '✗'} {msg}")
        if not ok:
            all_passed = False

    # ===== 测试无效句柄 =====
    print("\n--- 测试无效句柄 ---")
    invalid = ctypes.c_void_p(0x12345678)
    checks = [
        (dll.GetCheckBoxText(invalid, None, 0) == -1, "GetCheckBoxText 无效句柄返回 -1"),
        (dll.GetCheckBoxFont(invalid, None, 0, None, None, None, None) == -1, "GetCheckBoxFont 无效句柄返回 -1"),
        (dll.GetCheckBoxColor(invalid, None, None) == -1, "GetCheckBoxColor 无效句柄返回 -1"),
    ]
    for ok, msg in checks:
        print(f"{'✓' if ok else '✗'} {msg}")
        if not ok:
            all_passed = False

    dll.destroy_window(hwnd)
    return all_passed


def test_radiobutton_properties():
    """测试单选按钮属性命令 (需求 5.1-5.5)"""
    print("\n" + "=" * 60)
    print("测试单选按钮属性命令 (需求 5.1-5.5)")
    print("=" * 60)

    hwnd = create_test_window()
    if not hwnd:
        print("✗ 创建窗口失败")
        return False

    hRB = create_radiobutton(hwnd, "选项A")
    if not hRB:
        print("✗ 创建单选按钮失败")
        dll.destroy_window(hwnd)
        return False

    print("✓ 创建单选按钮成功")
    all_passed = True

    # ===== 测试 GetRadioButtonText (需求 5.1) =====
    print("\n--- 测试 GetRadioButtonText (需求 5.1) ---")

    text_len = dll.GetRadioButtonText(hRB, None, 0)
    if text_len < 0:
        print(f"✗ GetRadioButtonText 第一次调用失败: {text_len}")
        all_passed = False
    else:
        buf = ctypes.create_string_buffer(text_len + 1)
        ret = dll.GetRadioButtonText(hRB, buf, text_len)
        got_text = buf.raw[:ret].decode('utf-8')
        ok = got_text == "选项A"
        print(f"{'✓' if ok else '✗'} 初始文本: '{got_text}' (期望 '选项A')")
        if not ok:
            all_passed = False

    # 修改文本后再获取
    new_text = "选项B（已修改）"
    new_text_bytes = new_text.encode('utf-8')
    dll.SetRadioButtonText(hRB, new_text_bytes, len(new_text_bytes))

    text_len = dll.GetRadioButtonText(hRB, None, 0)
    if text_len >= 0:
        buf = ctypes.create_string_buffer(text_len + 1)
        ret = dll.GetRadioButtonText(hRB, buf, text_len)
        got_text = buf.raw[:ret].decode('utf-8')
        ok = got_text == new_text
        print(f"{'✓' if ok else '✗'} 修改后文本: '{got_text}' (期望 '{new_text}')")
        if not ok:
            all_passed = False

    # ===== 测试 SetRadioButtonFont / GetRadioButtonFont (需求 5.2, 5.4) =====
    print("\n--- 测试 SetRadioButtonFont / GetRadioButtonFont (需求 5.2, 5.4) ---")

    font_name = "宋体"
    font_bytes = font_name.encode('utf-8')
    dll.SetRadioButtonFont(hRB, font_bytes, len(font_bytes), 20, 0, 1, 1)

    name_len = dll.GetRadioButtonFont(hRB, None, 0, None, None, None, None)
    if name_len < 0:
        print(f"✗ GetRadioButtonFont 第一次调用失败: {name_len}")
        all_passed = False
    else:
        buf = ctypes.create_string_buffer(name_len + 1)
        font_size = ctypes.c_int(0)
        bold = ctypes.c_int(0)
        italic = ctypes.c_int(0)
        underline = ctypes.c_int(0)

        ret = dll.GetRadioButtonFont(hRB, buf, name_len,
                                     ctypes.byref(font_size),
                                     ctypes.byref(bold),
                                     ctypes.byref(italic),
                                     ctypes.byref(underline))
        if ret < 0:
            print(f"✗ GetRadioButtonFont 第二次调用失败: {ret}")
            all_passed = False
        else:
            got_name = buf.raw[:ret].decode('utf-8')
            checks = [
                (got_name == font_name, f"字体名 '{got_name}' == '{font_name}'"),
                (font_size.value == 20, f"字体大小 {font_size.value} == 20"),
                (bold.value == 0, f"粗体 {bold.value} == 0"),
                (italic.value == 1, f"斜体 {italic.value} == 1"),
                (underline.value == 1, f"下划线 {underline.value} == 1"),
            ]
            for ok, msg in checks:
                print(f"{'✓' if ok else '✗'} {msg}")
                if not ok:
                    all_passed = False

    # ===== 测试 SetRadioButtonColor / GetRadioButtonColor (需求 5.3, 5.5) =====
    print("\n--- 测试 SetRadioButtonColor / GetRadioButtonColor (需求 5.3, 5.5) ---")

    dll.SetRadioButtonColor(hRB, 0xFFFF0000, 0xFF00FF00)

    fg = ctypes.c_uint(0)
    bg = ctypes.c_uint(0)
    ret = dll.GetRadioButtonColor(hRB, ctypes.byref(fg), ctypes.byref(bg))

    checks = [
        (ret == 0, f"返回值 {ret} == 0"),
        (fg.value == 0xFFFF0000, f"前景色 0x{fg.value:08X} == 0xFFFF0000"),
        (bg.value == 0xFF00FF00, f"背景色 0x{bg.value:08X} == 0xFF00FF00"),
    ]
    for ok, msg in checks:
        print(f"{'✓' if ok else '✗'} {msg}")
        if not ok:
            all_passed = False

    # ===== 测试无效句柄 =====
    print("\n--- 测试无效句柄 ---")
    invalid = ctypes.c_void_p(0x12345678)
    checks = [
        (dll.GetRadioButtonText(invalid, None, 0) == -1, "GetRadioButtonText 无效句柄返回 -1"),
        (dll.GetRadioButtonFont(invalid, None, 0, None, None, None, None) == -1, "GetRadioButtonFont 无效句柄返回 -1"),
        (dll.GetRadioButtonColor(invalid, None, None) == -1, "GetRadioButtonColor 无效句柄返回 -1"),
    ]
    for ok, msg in checks:
        print(f"{'✓' if ok else '✗'} {msg}")
        if not ok:
            all_passed = False

    dll.destroy_window(hwnd)
    return all_passed


if __name__ == "__main__":
    cb_passed = test_checkbox_properties()
    rb_passed = test_radiobutton_properties()

    print("\n" + "=" * 60)
    print("测试汇总")
    print("=" * 60)
    print(f"复选框属性测试: {'✓ 通过' if cb_passed else '✗ 失败'}")
    print(f"单选按钮属性测试: {'✓ 通过' if rb_passed else '✗ 失败'}")

    all_ok = cb_passed and rb_passed
    print(f"\n总体结果: {'✓ 全部通过' if all_ok else '✗ 存在失败'}")
    sys.exit(0 if all_ok else 1)
