#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
按钮属性命令综合测试
测试所有按钮属性API的功能
"""

import ctypes
import sys

# 加载DLL
try:
    dll = ctypes.WinDLL('./Release/emoji_window.dll')
except:
    try:
        dll = ctypes.WinDLL('./emoji_window.dll')
    except:
        print("错误: 无法加载 emoji_window.dll")
        sys.exit(1)

# 定义函数原型
create_window_bytes = dll.create_window_bytes
create_window_bytes.argtypes = [ctypes.c_char_p, ctypes.c_int, ctypes.c_int, ctypes.c_int]
create_window_bytes.restype = ctypes.c_void_p

create_emoji_button_bytes = dll.create_emoji_button_bytes
create_emoji_button_bytes.argtypes = [
    ctypes.c_void_p, ctypes.c_char_p, ctypes.c_int, ctypes.c_char_p, ctypes.c_int,
    ctypes.c_int, ctypes.c_int, ctypes.c_int, ctypes.c_int, ctypes.c_uint32
]
create_emoji_button_bytes.restype = ctypes.c_int

# 按钮文本属性
GetButtonText = dll.GetButtonText
GetButtonText.argtypes = [ctypes.c_int, ctypes.c_char_p, ctypes.c_int]
GetButtonText.restype = ctypes.c_int

SetButtonText = dll.SetButtonText
SetButtonText.argtypes = [ctypes.c_int, ctypes.c_char_p, ctypes.c_int]
SetButtonText.restype = None

# 按钮Emoji属性
GetButtonEmoji = dll.GetButtonEmoji
GetButtonEmoji.argtypes = [ctypes.c_int, ctypes.c_char_p, ctypes.c_int]
GetButtonEmoji.restype = ctypes.c_int

SetButtonEmoji = dll.SetButtonEmoji
SetButtonEmoji.argtypes = [ctypes.c_int, ctypes.c_char_p, ctypes.c_int]
SetButtonEmoji.restype = None

# 按钮位置和大小
GetButtonBounds = dll.GetButtonBounds
GetButtonBounds.argtypes = [
    ctypes.c_int, 
    ctypes.POINTER(ctypes.c_int), ctypes.POINTER(ctypes.c_int),
    ctypes.POINTER(ctypes.c_int), ctypes.POINTER(ctypes.c_int)
]
GetButtonBounds.restype = ctypes.c_int

SetButtonBounds = dll.SetButtonBounds
SetButtonBounds.argtypes = [ctypes.c_int, ctypes.c_int, ctypes.c_int, ctypes.c_int, ctypes.c_int]
SetButtonBounds.restype = None

# 按钮背景色
GetButtonBackgroundColor = dll.GetButtonBackgroundColor
GetButtonBackgroundColor.argtypes = [ctypes.c_int]
GetButtonBackgroundColor.restype = ctypes.c_uint32

SetButtonBackgroundColor = dll.SetButtonBackgroundColor
SetButtonBackgroundColor.argtypes = [ctypes.c_int, ctypes.c_uint32]
SetButtonBackgroundColor.restype = None

# 按钮可视状态
GetButtonVisible = dll.GetButtonVisible
GetButtonVisible.argtypes = [ctypes.c_int]
GetButtonVisible.restype = ctypes.c_int

ShowButton = dll.ShowButton
ShowButton.argtypes = [ctypes.c_int, ctypes.c_int]
ShowButton.restype = None

# 按钮启用状态
GetButtonEnabled = dll.GetButtonEnabled
GetButtonEnabled.argtypes = [ctypes.c_int]
GetButtonEnabled.restype = ctypes.c_int

print("=" * 70)
print("按钮属性命令综合测试")
print("=" * 70)
print()

# 创建窗口
title = "按钮属性综合测试".encode('utf-8')
hwnd = create_window_bytes(title, len(title), 800, 600)
if not hwnd:
    print("✗ 错误: 无法创建窗口")
    sys.exit(1)
print("✓ 窗口创建成功")

# 创建测试按钮
emoji = "🎯".encode('utf-8')
text = "测试按钮".encode('utf-8')
button_id = create_emoji_button_bytes(
    hwnd, emoji, len(emoji), text, len(text),
    100, 100, 200, 50, 0xFF409EFF
)

if button_id < 0:
    print("✗ 错误: 无法创建按钮")
    sys.exit(1)
print(f"✓ 按钮创建成功 (ID: {button_id})")
print()

# 测试计数器
total_tests = 0
passed_tests = 0

def test(name, condition, expected=True):
    global total_tests, passed_tests
    total_tests += 1
    if condition == expected:
        print(f"  ✓ {name}")
        passed_tests += 1
        return True
    else:
        print(f"  ✗ {name} (期望: {expected}, 实际: {condition})")
        return False

# ========== 测试1: 按钮文本属性 ==========
print("【测试1】按钮文本属性")
print("-" * 70)

# 获取初始文本
text_len = GetButtonText(button_id, None, 0)
if text_len > 0:
    buffer = ctypes.create_string_buffer(text_len + 1)
    actual_len = GetButtonText(button_id, buffer, text_len)
    initial_text = buffer.value.decode('utf-8')
    print(f"  初始文本: '{initial_text}'")
    test("获取初始文本成功", actual_len > 0)
else:
    test("获取初始文本失败", False)

# 设置新文本
new_text = "新按钮文本🎉".encode('utf-8')
SetButtonText(button_id, new_text, len(new_text))

# 验证新文本
text_len = GetButtonText(button_id, None, 0)
if text_len > 0:
    buffer = ctypes.create_string_buffer(text_len + 1)
    GetButtonText(button_id, buffer, text_len)
    updated_text = buffer.value.decode('utf-8')
    print(f"  更新后文本: '{updated_text}'")
    test("设置文本成功", updated_text == "新按钮文本🎉")
else:
    test("设置文本失败", False)

print()

# ========== 测试2: 按钮Emoji属性 ==========
print("【测试2】按钮Emoji属性")
print("-" * 70)

# 获取初始Emoji
emoji_len = GetButtonEmoji(button_id, None, 0)
if emoji_len > 0:
    buffer = ctypes.create_string_buffer(emoji_len + 1)
    actual_len = GetButtonEmoji(button_id, buffer, emoji_len)
    initial_emoji = buffer.value.decode('utf-8')
    print(f"  初始Emoji: '{initial_emoji}'")
    test("获取初始Emoji成功", actual_len > 0)
else:
    test("获取初始Emoji失败", False)

# 设置新Emoji
new_emoji = "🚀".encode('utf-8')
SetButtonEmoji(button_id, new_emoji, len(new_emoji))

# 验证新Emoji
emoji_len = GetButtonEmoji(button_id, None, 0)
if emoji_len > 0:
    buffer = ctypes.create_string_buffer(emoji_len + 1)
    GetButtonEmoji(button_id, buffer, emoji_len)
    updated_emoji = buffer.value.decode('utf-8')
    print(f"  更新后Emoji: '{updated_emoji}'")
    test("设置Emoji成功", updated_emoji == "🚀")
else:
    test("设置Emoji失败", False)

print()

# ========== 测试3: 按钮位置和大小 ==========
print("【测试3】按钮位置和大小")
print("-" * 70)

# 获取初始位置和大小
x, y, width, height = ctypes.c_int(), ctypes.c_int(), ctypes.c_int(), ctypes.c_int()
result = GetButtonBounds(button_id, ctypes.byref(x), ctypes.byref(y), 
                        ctypes.byref(width), ctypes.byref(height))
if result == 0:
    print(f"  初始位置: ({x.value}, {y.value})")
    print(f"  初始大小: {width.value} x {height.value}")
    test("获取初始位置和大小成功", True)
else:
    test("获取初始位置和大小失败", False)

# 设置新位置和大小
SetButtonBounds(button_id, 150, 200, 250, 60)

# 验证新位置和大小
result = GetButtonBounds(button_id, ctypes.byref(x), ctypes.byref(y), 
                        ctypes.byref(width), ctypes.byref(height))
if result == 0:
    print(f"  更新后位置: ({x.value}, {y.value})")
    print(f"  更新后大小: {width.value} x {height.value}")
    test("设置位置和大小成功", 
         x.value == 150 and y.value == 200 and width.value == 250 and height.value == 60)
else:
    test("设置位置和大小失败", False)

print()

# ========== 测试4: 按钮背景色 ==========
print("【测试4】按钮背景色")
print("-" * 70)

# 获取初始背景色
initial_color = GetButtonBackgroundColor(button_id)
print(f"  初始背景色: 0x{initial_color:08X}")
test("获取初始背景色成功", initial_color != 0)

# 设置新背景色（红色）
new_color = 0xFFFF0000
SetButtonBackgroundColor(button_id, new_color)

# 验证新背景色
updated_color = GetButtonBackgroundColor(button_id)
print(f"  更新后背景色: 0x{updated_color:08X}")
test("设置背景色成功", updated_color == new_color)

print()

# ========== 测试5: 按钮可视状态 ==========
print("【测试5】按钮可视状态")
print("-" * 70)

# 获取初始可视状态
visible = GetButtonVisible(button_id)
print(f"  初始可视状态: {'可见' if visible else '隐藏'}")
test("按钮默认可见", visible != 0)

# 隐藏按钮
ShowButton(button_id, 0)
visible = GetButtonVisible(button_id)
print(f"  隐藏后状态: {'可见' if visible else '隐藏'}")
test("隐藏按钮成功", visible == 0)

# 显示按钮
ShowButton(button_id, 1)
visible = GetButtonVisible(button_id)
print(f"  显示后状态: {'可见' if visible else '隐藏'}")
test("显示按钮成功", visible != 0)

print()

# ========== 测试6: 按钮启用状态 ==========
print("【测试6】按钮启用状态")
print("-" * 70)

# 获取初始启用状态
enabled = GetButtonEnabled(button_id)
print(f"  初始启用状态: {'启用' if enabled else '禁用'}")
test("按钮默认启用", enabled != 0)

print()

# ========== 测试7: 无效ID测试 ==========
print("【测试7】无效ID测试")
print("-" * 70)

invalid_id = 9999
test("无效ID - GetButtonText", GetButtonText(invalid_id, None, 0) == -1)
test("无效ID - GetButtonEmoji", GetButtonEmoji(invalid_id, None, 0) == -1)
test("无效ID - GetButtonBounds", GetButtonBounds(invalid_id, None, None, None, None) == -1)
test("无效ID - GetButtonBackgroundColor", GetButtonBackgroundColor(invalid_id) == 0)
test("无效ID - GetButtonVisible", GetButtonVisible(invalid_id) == 0)
test("无效ID - GetButtonEnabled", GetButtonEnabled(invalid_id) == 0)

print()

# 输出测试结果
print("=" * 70)
print(f"测试完成: {passed_tests}/{total_tests} 通过")
if passed_tests == total_tests:
    print("✓ 所有测试通过！")
    print()
    print("阶段1（按钮属性命令）的所有功能已验证通过：")
    print("  1. ✓ 按钮文本属性（GetButtonText / SetButtonText）")
    print("  2. ✓ 按钮Emoji属性（GetButtonEmoji / SetButtonEmoji）")
    print("  3. ✓ 按钮位置和大小（GetButtonBounds / SetButtonBounds）")
    print("  4. ✓ 按钮背景色（GetButtonBackgroundColor / SetButtonBackgroundColor）")
    print("  5. ✓ 按钮可视状态（GetButtonVisible / ShowButton）")
    print("  6. ✓ 按钮启用状态（GetButtonEnabled）")
    print("  7. ✓ 无效ID错误处理")
else:
    print(f"✗ {total_tests - passed_tests} 个测试失败")
    sys.exit(1)

print("=" * 70)
