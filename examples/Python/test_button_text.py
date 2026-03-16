"""
测试按钮文本属性命令
"""
import ctypes
import os

# 加载DLL
dll_path = os.path.join("Release", "emoji_window.dll")
if not os.path.exists(dll_path):
    print(f"❌ DLL文件不存在: {dll_path}")
    exit(1)

dll = ctypes.CDLL(dll_path)

# 定义函数原型
dll.create_window_bytes.argtypes = [ctypes.c_char_p, ctypes.c_int, ctypes.c_int, ctypes.c_int]
dll.create_window_bytes.restype = ctypes.c_void_p

dll.create_emoji_button_bytes.argtypes = [
    ctypes.c_void_p,  # parent
    ctypes.c_char_p,  # emoji_bytes
    ctypes.c_int,     # emoji_len
    ctypes.c_char_p,  # text_bytes
    ctypes.c_int,     # text_len
    ctypes.c_int,     # x
    ctypes.c_int,     # y
    ctypes.c_int,     # width
    ctypes.c_int,     # height
    ctypes.c_uint32   # bg_color
]
dll.create_emoji_button_bytes.restype = ctypes.c_int

dll.GetButtonText.argtypes = [ctypes.c_int, ctypes.c_char_p, ctypes.c_int]
dll.GetButtonText.restype = ctypes.c_int

dll.SetButtonText.argtypes = [ctypes.c_int, ctypes.c_char_p, ctypes.c_int]
dll.SetButtonText.restype = None

print("=== 按钮文本属性测试 ===\n")

# 创建窗口
title = "按钮属性测试".encode('utf-8')
hwnd = dll.create_window_bytes(title, len(title), 600, 400)
print(f"✅ 窗口创建成功: {hwnd}")

# 创建按钮
emoji = "✅".encode('utf-8')
text = "测试按钮".encode('utf-8')
button_id = dll.create_emoji_button_bytes(
    hwnd, emoji, len(emoji), text, len(text),
    50, 50, 200, 40, 0xFF409EFF
)
print(f"✅ 按钮创建成功: ID = {button_id}\n")

# 测试GetButtonText - 第一次调用获取长度
print("测试 GetButtonText:")
text_len = dll.GetButtonText(button_id, None, 0)
print(f"  文本长度: {text_len} 字节")

if text_len > 0:
    # 第二次调用获取内容
    buffer = ctypes.create_string_buffer(text_len)
    actual_len = dll.GetButtonText(button_id, buffer, text_len)
    button_text = buffer.raw[:actual_len].decode('utf-8')
    print(f"  按钮文本: {button_text}")
    print(f"  ✅ GetButtonText 测试通过!\n")
else:
    print(f"  ❌ GetButtonText 返回长度错误: {text_len}\n")

# 测试SetButtonText
print("测试 SetButtonText:")
new_text = "新文本🎉".encode('utf-8')
dll.SetButtonText(button_id, new_text, len(new_text))
print(f"  已设置新文本: 新文本🎉")

# 再次获取文本验证
text_len = dll.GetButtonText(button_id, None, 0)
if text_len > 0:
    buffer = ctypes.create_string_buffer(text_len)
    actual_len = dll.GetButtonText(button_id, buffer, text_len)
    button_text = buffer.raw[:actual_len].decode('utf-8')
    print(f"  验证新文本: {button_text}")
    
    if button_text == "新文本🎉":
        print(f"  ✅ SetButtonText 测试通过!\n")
    else:
        print(f"  ❌ SetButtonText 测试失败: 期望 '新文本🎉', 实际 '{button_text}'\n")
else:
    print(f"  ❌ 获取新文本失败\n")

# 测试无效按钮ID
print("测试错误处理:")
invalid_len = dll.GetButtonText(99999, None, 0)
if invalid_len == -1:
    print(f"  ✅ 无效按钮ID正确返回-1\n")
else:
    print(f"  ❌ 无效按钮ID应返回-1, 实际返回: {invalid_len}\n")

print("=== 所有测试完成 ===")
print("按Ctrl+C退出...")

# 保持窗口打开
try:
    dll.run_message_loop()
except KeyboardInterrupt:
    print("\n程序退出")
