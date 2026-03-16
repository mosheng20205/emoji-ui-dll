"""
测试GetLabelText函数
验证标签文本获取功能，包括UTF-8编码、Unicode字符和空文本处理
"""
import ctypes
import sys
import os
import time

# 加载DLL
dll_path = os.path.join(os.path.dirname(__file__), 'emoji_window.dll')
try:
    dll = ctypes.CDLL(dll_path)
except OSError:
    print("错误: 无法加载emoji_window.dll")
    sys.exit(1)

# 定义函数原型
dll.create_window_bytes_ex.restype = ctypes.c_void_p
dll.create_window_bytes_ex.argtypes = [ctypes.c_char_p, ctypes.c_int, ctypes.c_int, ctypes.c_int, ctypes.c_int]

dll.CreateLabel.restype = ctypes.c_void_p
dll.CreateLabel.argtypes = [
    ctypes.c_void_p,  # hParent
    ctypes.c_int, ctypes.c_int, ctypes.c_int, ctypes.c_int,  # x, y, w, h
    ctypes.c_char_p, ctypes.c_int,  # text_bytes, text_len
    ctypes.c_uint,  # fg_color
    ctypes.c_uint,  # bg_color
    ctypes.c_char_p, ctypes.c_int,  # font_name, font_name_len
    ctypes.c_int,  # font_size
    ctypes.c_int, ctypes.c_int, ctypes.c_int,  # bold, italic, underline
    ctypes.c_int,  # alignment
    ctypes.c_int,  # word_wrap
]

dll.GetLabelText.restype = ctypes.c_int
dll.GetLabelText.argtypes = [ctypes.c_void_p, ctypes.c_char_p, ctypes.c_int]

dll.SetLabelText.restype = None
dll.SetLabelText.argtypes = [ctypes.c_void_p, ctypes.c_char_p, ctypes.c_int]

dll.destroy_window.restype = None
dll.destroy_window.argtypes = [ctypes.c_void_p]


def create_test_window():
    title = "测试窗口".encode('utf-8')
    hwnd = dll.create_window_bytes_ex(title, len(title), 100, 100, 400)
    return hwnd


def create_label(hwnd, text):
    text_bytes = text.encode('utf-8')
    font_name = "微软雅黑".encode('utf-8')
    label = dll.CreateLabel(
        hwnd,
        10, 10, 200, 30,
        text_bytes, len(text_bytes),
        0xFF000000,  # fg_color (黑色)
        0xFFFFFFFF,  # bg_color (白色)
        font_name, len(font_name),
        14,
        0, 0, 0,  # bold, italic, underline
        0,  # alignment (左对齐)
        0,  # word_wrap
    )
    return label


def get_label_text(label_hwnd):
    """使用两次调用模式获取标签文本"""
    # 第一次调用：获取长度
    length = dll.GetLabelText(label_hwnd, None, 0)
    if length < 0:
        return None
    if length == 0:
        return ""
    # 第二次调用：获取内容
    buf = ctypes.create_string_buffer(length)
    actual_len = dll.GetLabelText(label_hwnd, buf, length)
    if actual_len < 0:
        return None
    return buf.raw[:actual_len].decode('utf-8')


def test_label_text():
    print("=" * 60)
    print("测试 GetLabelText 函数")
    print("=" * 60)

    hwnd = create_test_window()
    if not hwnd:
        print("✗ 创建窗口失败")
        return False

    all_passed = True

    # 测试1: 基本ASCII文本
    print("\n--- 测试1: 基本ASCII文本 ---")
    label = create_label(hwnd, "Hello World")
    if not label:
        print("✗ 创建标签失败")
        all_passed = False
    else:
        text = get_label_text(label)
        if text == "Hello World":
            print(f"✓ 获取文本成功: '{text}'")
        else:
            print(f"✗ 文本不匹配，期望 'Hello World'，实际 '{text}'")
            all_passed = False

    # 测试2: 中文Unicode文本
    print("\n--- 测试2: 中文Unicode文本 ---")
    label2 = create_label(hwnd, "你好世界")
    if not label2:
        print("✗ 创建标签失败")
        all_passed = False
    else:
        text = get_label_text(label2)
        if text == "你好世界":
            print(f"✓ 获取中文文本成功: '{text}'")
        else:
            print(f"✗ 中文文本不匹配，期望 '你好世界'，实际 '{text}'")
            all_passed = False

    # 测试3: 空文本
    print("\n--- 测试3: 空文本 ---")
    label3 = create_label(hwnd, "")
    if not label3:
        print("✗ 创建标签失败")
        all_passed = False
    else:
        length = dll.GetLabelText(label3, None, 0)
        if length == 0:
            print(f"✓ 空文本返回0字节: length={length}")
        else:
            print(f"✗ 空文本应返回0，实际返回 {length}")
            all_passed = False

    # 测试4: 无效句柄
    print("\n--- 测试4: 无效句柄 ---")
    result = dll.GetLabelText(ctypes.c_void_p(0), None, 0)
    if result == -1:
        print(f"✓ 无效句柄返回-1: result={result}")
    else:
        print(f"✗ 无效句柄应返回-1，实际返回 {result}")
        all_passed = False

    # 测试5: SetLabelText后再GetLabelText（验证修改后能正确获取）
    print("\n--- 测试5: 修改文本后获取 ---")
    label5 = create_label(hwnd, "原始文本")
    if not label5:
        print("✗ 创建标签失败")
        all_passed = False
    else:
        new_text = "修改后的文本"
        new_text_bytes = new_text.encode('utf-8')
        dll.SetLabelText(label5, new_text_bytes, len(new_text_bytes))
        text = get_label_text(label5)
        if text == new_text:
            print(f"✓ 修改后获取文本成功: '{text}'")
        else:
            print(f"✗ 修改后文本不匹配，期望 '{new_text}'，实际 '{text}'")
            all_passed = False

    # 测试6: 含Emoji的文本
    print("\n--- 测试6: 含Emoji的文本 ---")
    emoji_text = "Hello 😀 World"
    label6 = create_label(hwnd, emoji_text)
    if not label6:
        print("✗ 创建标签失败")
        all_passed = False
    else:
        text = get_label_text(label6)
        if text == emoji_text:
            print(f"✓ 含Emoji文本获取成功: '{text}'")
        else:
            print(f"✗ Emoji文本不匹配，期望 '{emoji_text}'，实际 '{text}'")
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
    success = test_label_text()
    sys.exit(0 if success else 1)
