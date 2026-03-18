#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
按钮可视状态属性功能测试
测试GetButtonVisible和ShowButton函数
"""

import ctypes
import sys
import time

# 加载DLL
try:
    dll = ctypes.WinDLL('./Release/emoji_window.dll')
except:
    print("错误: 无法加载 Release/emoji_window.dll")
    sys.exit(1)

# 定义函数原型
create_window_bytes = dll.create_window_bytes
create_window_bytes.argtypes = [ctypes.c_char_p, ctypes.c_int, ctypes.c_int, ctypes.c_int]
create_window_bytes.restype = ctypes.c_void_p

create_emoji_button_bytes = dll.create_emoji_button_bytes
create_emoji_button_bytes.argtypes = [
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
create_emoji_button_bytes.restype = ctypes.c_int

GetButtonVisible = dll.GetButtonVisible
GetButtonVisible.argtypes = [ctypes.c_int]
GetButtonVisible.restype = ctypes.c_int

ShowButton = dll.ShowButton
ShowButton.argtypes = [ctypes.c_int, ctypes.c_int]
ShowButton.restype = None

GetButtonEnabled = dll.GetButtonEnabled
GetButtonEnabled.argtypes = [ctypes.c_int]
GetButtonEnabled.restype = ctypes.c_int

print("=" * 60)
print("按钮可视状态属性功能测试")
print("=" * 60)
print()

# 创建窗口
title = "按钮可视状态测试".encode('utf-8')
hwnd = create_window_bytes(title, len(title), 600, 400)
if not hwnd:
    print("✗ 错误: 无法创建窗口")
    sys.exit(1)
print("✓ 窗口创建成功")

# 创建测试按钮
emoji = "👁️".encode('utf-8')
text = "测试按钮".encode('utf-8')
button_id = create_emoji_button_bytes(
    hwnd, emoji, len(emoji), text, len(text),
    50, 50, 200, 40, 0xFF4CAF50
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

# 测试1: 获取初始可视状态
print("测试1: 获取初始可视状态")
visible = GetButtonVisible(button_id)
test("按钮默认应该可见", visible != 0, True)
print()

# 测试2: 隐藏按钮
print("测试2: 隐藏按钮")
ShowButton(button_id, 0)  # FALSE
visible = GetButtonVisible(button_id)
test("隐藏后应该不可见", visible == 0, True)
print()

# 测试3: 显示按钮
print("测试3: 显示按钮")
ShowButton(button_id, 1)  # TRUE
visible = GetButtonVisible(button_id)
test("显示后应该可见", visible != 0, True)
print()

# 测试4: 测试无效按钮ID
print("测试4: 测试无效按钮ID")
invalid_visible = GetButtonVisible(9999)
test("无效ID应该返回FALSE", invalid_visible == 0, True)
print()

# 测试5: 多次切换可视状态
print("测试5: 多次切换可视状态")
all_passed = True
for i in range(5):
    expected_visible = (i % 2 == 0)
    ShowButton(button_id, 1 if expected_visible else 0)
    actual_visible = GetButtonVisible(button_id)
    if (actual_visible != 0) != expected_visible:
        all_passed = False
        print(f"  ✗ 第{i+1}次切换失败 (期望: {expected_visible}, 实际: {actual_visible != 0})")
    else:
        print(f"  ✓ 第{i+1}次切换成功 (状态: {'可见' if expected_visible else '隐藏'})")
test("所有切换都应该成功", all_passed, True)
print()

# 测试6: 验证可视状态不影响启用状态
print("测试6: 验证可视状态不影响启用状态")
ShowButton(button_id, 0)  # 隐藏
enabled = GetButtonEnabled(button_id)
test("隐藏按钮仍应保持启用状态", enabled != 0, True)
ShowButton(button_id, 1)  # 显示
enabled = GetButtonEnabled(button_id)
test("显示按钮仍应保持启用状态", enabled != 0, True)
print()

# 测试7: 边界测试 - 使用非0/1值
print("测试7: 边界测试")
ShowButton(button_id, 100)  # 任何非0值都应该是TRUE
visible = GetButtonVisible(button_id)
test("非0值应该设置为可见", visible != 0, True)
ShowButton(button_id, -1)  # 任何非0值都应该是TRUE
visible = GetButtonVisible(button_id)
test("负数非0值应该设置为可见", visible != 0, True)
print()

# 输出测试结果
print("=" * 60)
print(f"测试完成: {passed_tests}/{total_tests} 通过")
if passed_tests == total_tests:
    print("✓ 所有测试通过！")
    print()
    print("提示: 窗口将保持打开5秒，您可以手动验证按钮的可视状态")
    print("      按钮应该是可见的（最后一次设置为可见）")
    time.sleep(5)
else:
    print(f"✗ {total_tests - passed_tests} 个测试失败")
    sys.exit(1)

print("=" * 60)
