"""
测试窗口标题栏颜色获取功能
测试GetWindowTitlebarColor函数
"""
import ctypes
from ctypes import wintypes
import sys
import os
import time

# 添加当前目录到路径
sys.path.insert(0, os.path.dirname(__file__))

# 加载DLL
try:
    dll = ctypes.CDLL('./emoji_window.dll')
except OSError:
    print("错误: 无法加载emoji_window.dll")
    sys.exit(1)

# 定义函数原型
dll.create_window_bytes_ex.argtypes = [ctypes.c_char_p, ctypes.c_int, ctypes.c_int, ctypes.c_int, ctypes.c_uint32]
dll.create_window_bytes_ex.restype = wintypes.HWND

dll.GetWindowTitlebarColor.argtypes = [wintypes.HWND]
dll.GetWindowTitlebarColor.restype = ctypes.c_uint32

dll.set_window_titlebar_color.argtypes = [wintypes.HWND, ctypes.c_uint32]
dll.set_window_titlebar_color.restype = None

dll.destroy_window.argtypes = [wintypes.HWND]
dll.destroy_window.restype = None

def test_window_titlebar_color():
    """测试窗口标题栏颜色获取"""
    print("=" * 60)
    print("测试窗口标题栏颜色获取功能")
    print("=" * 60)
    
    # 测试1: 创建窗口时指定标题栏颜色
    print("\n测试1: 创建窗口时指定标题栏颜色")
    title = "测试窗口 - 蓝色标题栏".encode('utf-8')
    blue_color = 0x409EFF  # 蓝色 (RGB格式)
    hwnd1 = dll.create_window_bytes_ex(title, len(title), 800, 600, blue_color)
    
    if not hwnd1:
        print("❌ 创建窗口失败")
        return False
    
    print(f"✅ 窗口创建成功: HWND = {hwnd1}")
    
    # 获取标题栏颜色
    color = dll.GetWindowTitlebarColor(hwnd1)
    print(f"   设置的颜色: 0x{blue_color:06X}")
    print(f"   获取的颜色: 0x{color:06X}")
    
    if color == blue_color:
        print("✅ 颜色匹配成功")
    else:
        print(f"❌ 颜色不匹配")
        dll.destroy_window(hwnd1)
        return False
    
    # 测试2: 使用set_window_titlebar_color修改颜色
    print("\n测试2: 修改标题栏颜色")
    green_color = 0x67C23A  # 绿色
    dll.set_window_titlebar_color(hwnd1, green_color)
    
    # 获取修改后的颜色
    color = dll.GetWindowTitlebarColor(hwnd1)
    print(f"   设置的颜色: 0x{green_color:06X}")
    print(f"   获取的颜色: 0x{color:06X}")
    
    if color == green_color:
        print("✅ 颜色修改成功")
    else:
        print(f"❌ 颜色修改失败")
        dll.destroy_window(hwnd1)
        return False
    
    # 测试3: 创建默认颜色窗口 (0表示跟随主题)
    print("\n测试3: 创建默认颜色窗口")
    title2 = "测试窗口 - 默认颜色".encode('utf-8')
    hwnd2 = dll.create_window_bytes_ex(title2, len(title2), 800, 600, 0)
    
    if not hwnd2:
        print("❌ 创建窗口失败")
        dll.destroy_window(hwnd1)
        return False
    
    print(f"✅ 窗口创建成功: HWND = {hwnd2}")
    
    # 获取默认颜色
    color = dll.GetWindowTitlebarColor(hwnd2)
    print(f"   获取的颜色: 0x{color:06X}")
    
    if color == 0:
        print("✅ 默认颜色正确 (0表示跟随主题)")
    else:
        print(f"⚠️  默认颜色为: 0x{color:06X}")
    
    # 测试4: 测试无效句柄
    print("\n测试4: 测试无效句柄")
    invalid_hwnd = wintypes.HWND(0)
    color = dll.GetWindowTitlebarColor(invalid_hwnd)
    
    if color == 0:
        print("✅ 无效句柄返回0")
    else:
        print(f"❌ 无效句柄应返回0，实际返回: 0x{color:06X}")
        dll.destroy_window(hwnd1)
        dll.destroy_window(hwnd2)
        return False
    
    # 测试5: 测试多种颜色
    print("\n测试5: 测试多种颜色")
    test_colors = [
        (0xFF0000, "红色"),
        (0x00FF00, "绿色"),
        (0x0000FF, "蓝色"),
        (0xFFFF00, "黄色"),
        (0xFF00FF, "品红"),
        (0x00FFFF, "青色"),
        (0xFFFFFF, "白色"),
        (0x000000, "黑色"),
    ]
    
    all_passed = True
    for test_color, color_name in test_colors:
        dll.set_window_titlebar_color(hwnd1, test_color)
        result_color = dll.GetWindowTitlebarColor(hwnd1)
        
        if result_color == test_color:
            print(f"   ✅ {color_name} (0x{test_color:06X}): 正确")
        else:
            print(f"   ❌ {color_name} (0x{test_color:06X}): 失败，获取到 0x{result_color:06X}")
            all_passed = False
    
    if all_passed:
        print("✅ 所有颜色测试通过")
    else:
        print("❌ 部分颜色测试失败")
        dll.destroy_window(hwnd1)
        dll.destroy_window(hwnd2)
        return False
    
    # 清理
    print("\n清理资源...")
    dll.destroy_window(hwnd1)
    dll.destroy_window(hwnd2)
    print("✅ 资源清理完成")
    
    print("\n" + "=" * 60)
    print("所有测试通过！")
    print("=" * 60)
    
    return True

if __name__ == "__main__":
    success = test_window_titlebar_color()
    sys.exit(0 if success else 1)
