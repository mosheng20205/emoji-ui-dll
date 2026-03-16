#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
测试按钮Emoji属性命令
"""

import ctypes
import sys
import os

# 加载DLL
try:
    dll = ctypes.CDLL('./emoji_window.dll')
except:
    print("错误: 无法加载 emoji_window.dll")
    sys.exit(1)

# 定义函数原型
dll.create_window_bytes.argtypes = [ctypes.c_char_p, ctypes.c_int, ctypes.c_int, ctypes.c_int]
dll.create_window_bytes.restype = ctypes.c_void_p

dll.create_emoji_button_bytes.argtypes = [
    ctypes.c_void_p, ctypes.c_char_p, ctypes.c_int,
    ctypes.c_char_p, ctypes.c_int,
    ctypes.c_int, ctypes.c_int, ctypes.c_int, ctypes.c_int, ctypes.c_uint32
]
dll.create_emoji_button_bytes.restype = ctypes.c_int

dll.GetButtonEmoji.argtypes = [ctypes.c_int, ctypes.c_char_p, ctypes.c_int]
dll.GetButtonEmoji.restype = ctypes.c_int

dll.SetButtonEmoji.argtypes = [ctypes.c_int, ctypes.c_char_p, ctypes.c_int]
dll.SetButtonEmoji.restype = None

dll.GetButtonText.argtypes = [ctypes.c_int, ctypes.c_char_p, ctypes.c_int]
dll.GetButtonText.restype = ctypes.c_int

def get_button_emoji(button_id):
    """获取按钮Emoji"""
    # 第一次调用获取长度
    length = dll.GetButtonEmoji(button_id, None, 0)
    if length < 0:
        return None
    if length == 0:
        return ""
    
    # 第二次调用获取内容
    buffer = ctypes.create_string_buffer(length)
    actual_len = dll.GetButtonEmoji(button_id, buffer, length)
    if actual_len < 0:
        return None
    
    return buffer.raw[:actual_len].decode('utf-8')

def set_button_emoji(button_id, emoji):
    """设置按钮Emoji"""
    emoji_bytes = emoji.encode('utf-8')
    dll.SetButtonEmoji(button_id, emoji_bytes, len(emoji_bytes))

def get_button_text(button_id):
    """获取按钮文本"""
    length = dll.GetButtonText(button_id, None, 0)
    if length < 0:
        return None
    if length == 0:
        return ""
    
    buffer = ctypes.create_string_buffer(length)
    actual_len = dll.GetButtonText(button_id, buffer, length)
    if actual_len < 0:
        return None
    
    return buffer.raw[:actual_len].decode('utf-8')

def main():
    print("=== 按钮Emoji属性命令测试 ===\n")
    
    # 创建窗口
    title = "测试窗口".encode('utf-8')
    hwnd = dll.create_window_bytes(title, len(title), 800, 600)
    if not hwnd:
        print("错误: 无法创建窗口")
        return 1
    print("✓ 窗口创建成功")
    
    # 创建按钮
    emoji = "😀".encode('utf-8')
    text = "测试按钮".encode('utf-8')
    button_id = dll.create_emoji_button_bytes(
        hwnd, emoji, len(emoji), text, len(text),
        100, 100, 200, 50, 0xFF4CAF50
    )
    if button_id <= 0:
        print("错误: 无法创建按钮")
        return 1
    print(f"✓ 按钮创建成功 (ID: {button_id})\n")
    
    # 测试1: 获取初始Emoji
    print("测试1: 获取初始Emoji")
    initial_emoji = get_button_emoji(button_id)
    if initial_emoji == "😀":
        print(f"✓ Emoji内容正确: {initial_emoji}")
    else:
        print(f"✗ Emoji内容不匹配")
        print(f"  期望: 😀")
        print(f"  实际: {initial_emoji}")
    
    # 测试2: 设置新的Emoji
    print("\n测试2: 设置新的Emoji")
    new_emoji = "🎉"
    set_button_emoji(button_id, new_emoji)
    print(f"  已设置新Emoji: {new_emoji}")
    
    # 验证设置
    current_emoji = get_button_emoji(button_id)
    if current_emoji == new_emoji:
        print(f"✓ Emoji设置成功: {current_emoji}")
    else:
        print(f"✗ Emoji设置失败")
        print(f"  期望: {new_emoji}")
        print(f"  实际: {current_emoji}")
    
    # 测试3: 设置空Emoji
    print("\n测试3: 设置空Emoji")
    set_button_emoji(button_id, "")
    current_emoji = get_button_emoji(button_id)
    if current_emoji == "":
        print("✓ 空Emoji设置成功")
    else:
        print(f"✗ 空Emoji设置失败: {current_emoji}")
    
    # 测试4: 设置包含多个Emoji的字符串
    print("\n测试4: 设置多个Emoji")
    multi_emoji = "😀🎉🚀"
    set_button_emoji(button_id, multi_emoji)
    current_emoji = get_button_emoji(button_id)
    if current_emoji == multi_emoji:
        print(f"✓ 多个Emoji设置成功: {current_emoji}")
    else:
        print(f"✗ 多个Emoji设置失败")
        print(f"  期望: {multi_emoji}")
        print(f"  实际: {current_emoji}")
    
    # 测试5: 测试无效按钮ID
    print("\n测试5: 测试无效按钮ID")
    invalid_emoji = get_button_emoji(9999)
    if invalid_emoji is None:
        print("✓ 无效ID正确返回None")
    else:
        print(f"✗ 无效ID应返回None，实际返回: {invalid_emoji}")
    
    # 测试6: 验证文本不受影响
    print("\n测试6: 验证文本不受Emoji修改影响")
    current_text = get_button_text(button_id)
    if current_text == "测试按钮":
        print(f"✓ 文本未受影响: {current_text}")
    else:
        print(f"✗ 文本被意外修改: {current_text}")
    
    # 测试7: 测试中文字符作为Emoji
    print("\n测试7: 测试中文字符作为Emoji")
    chinese_emoji = "中文"
    set_button_emoji(button_id, chinese_emoji)
    current_emoji = get_button_emoji(button_id)
    if current_emoji == chinese_emoji:
        print(f"✓ 中文字符设置成功: {current_emoji}")
    else:
        print(f"✗ 中文字符设置失败")
        print(f"  期望: {chinese_emoji}")
        print(f"  实际: {current_emoji}")
    
    # 测试8: 测试特殊Unicode字符
    print("\n测试8: 测试特殊Unicode字符")
    special_chars = "★♥♦♣"
    set_button_emoji(button_id, special_chars)
    current_emoji = get_button_emoji(button_id)
    if current_emoji == special_chars:
        print(f"✓ 特殊字符设置成功: {current_emoji}")
    else:
        print(f"✗ 特殊字符设置失败")
    
    print("\n=== 测试完成 ===")
    return 0

if __name__ == '__main__':
    sys.exit(main())
