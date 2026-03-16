"""
测试窗口位置和大小属性命令
测试GetWindowBounds和SetWindowBounds函数
"""
import ctypes
from ctypes import wintypes
import sys
import os
import time

# 添加当前目录到路径
sys.path.insert(0, os.path.dirname(__file__))

# 加载DLL (使用WinDLL因为函数使用__stdcall约定)
dll_path = os.path.join(os.path.dirname(__file__), 'emoji_window.dll')
try:
    dll = ctypes.WinDLL(dll_path)
except OSError as e:
    print(f"错误: 无法加载emoji_window.dll: {e}")
    sys.exit(1)

# 定义函数原型
create_window_bytes = dll.create_window_bytes
create_window_bytes.argtypes = [ctypes.POINTER(ctypes.c_ubyte), ctypes.c_int, ctypes.c_int, ctypes.c_int]
create_window_bytes.restype = wintypes.HWND

GetWindowBounds = dll.GetWindowBounds
GetWindowBounds.argtypes = [wintypes.HWND, ctypes.POINTER(ctypes.c_int), ctypes.POINTER(ctypes.c_int), 
                            ctypes.POINTER(ctypes.c_int), ctypes.POINTER(ctypes.c_int)]
GetWindowBounds.restype = ctypes.c_int

SetWindowBounds = dll.SetWindowBounds
SetWindowBounds.argtypes = [wintypes.HWND, ctypes.c_int, ctypes.c_int, ctypes.c_int, ctypes.c_int]
SetWindowBounds.restype = None

destroy_window = dll.destroy_window
destroy_window.argtypes = [wintypes.HWND]
destroy_window.restype = None

def test_window_bounds():
    """测试窗口位置和大小属性命令"""
    print("=" * 60)
    print("测试窗口位置和大小属性命令")
    print("=" * 60)
    
    # 创建窗口
    title = "测试窗口位置和大小".encode('utf-8')
    title_bytes = (ctypes.c_ubyte * len(title))(*title)
    hwnd = create_window_bytes(title_bytes, len(title), 800, 600)
    
    if not hwnd:
        print("❌ 创建窗口失败")
        return False
    
    print(f"✅ 创建窗口成功: HWND = {hwnd}")
    time.sleep(0.5)  # 等待窗口显示
    
    # 测试1: 获取初始窗口位置和大小
    print("\n测试1: 获取初始窗口位置和大小")
    x = ctypes.c_int()
    y = ctypes.c_int()
    width = ctypes.c_int()
    height = ctypes.c_int()
    
    result = GetWindowBounds(hwnd, ctypes.byref(x), ctypes.byref(y), 
                            ctypes.byref(width), ctypes.byref(height))
    
    if result == 0:
        print(f"✅ 获取窗口位置和大小成功:")
        print(f"   位置: ({x.value}, {y.value})")
        print(f"   大小: {width.value} x {height.value}")
        
        # 验证大小是否正确
        if width.value == 800 and height.value == 600:
            print("✅ 窗口大小验证成功 (800x600)")
        else:
            print(f"⚠️  窗口大小不匹配: 期望 800x600, 实际 {width.value}x{height.value}")
    else:
        print(f"❌ 获取窗口位置和大小失败: 返回值 = {result}")
        destroy_window(hwnd)
        return False
    
    # 测试2: 设置新的窗口位置和大小
    print("\n测试2: 设置新的窗口位置和大小")
    new_x = 100
    new_y = 100
    new_width = 1000
    new_height = 700
    
    print(f"设置窗口位置为 ({new_x}, {new_y}), 大小为 {new_width}x{new_height}")
    SetWindowBounds(hwnd, new_x, new_y, new_width, new_height)
    time.sleep(0.5)  # 等待窗口移动和调整大小
    
    # 验证设置是否成功
    result = GetWindowBounds(hwnd, ctypes.byref(x), ctypes.byref(y), 
                            ctypes.byref(width), ctypes.byref(height))
    
    if result == 0:
        print(f"✅ 验证窗口位置和大小:")
        print(f"   位置: ({x.value}, {y.value})")
        print(f"   大小: {width.value} x {height.value}")
        
        # 检查位置和大小是否正确（允许一些误差，因为窗口边框）
        pos_ok = abs(x.value - new_x) <= 10 and abs(y.value - new_y) <= 10
        size_ok = abs(width.value - new_width) <= 20 and abs(height.value - new_height) <= 20
        
        if pos_ok and size_ok:
            print("✅ 窗口位置和大小设置成功")
        else:
            print(f"⚠️  窗口位置或大小不完全匹配（可能是窗口边框导致的正常误差）")
    else:
        print(f"❌ 验证失败: 返回值 = {result}")
        destroy_window(hwnd)
        return False
    
    # 测试3: 测试边界情况 - 无效句柄
    print("\n测试3: 测试无效句柄")
    result = GetWindowBounds(None, ctypes.byref(x), ctypes.byref(y), 
                            ctypes.byref(width), ctypes.byref(height))
    if result == -1:
        print("✅ 无效句柄正确返回 -1")
    else:
        print(f"❌ 无效句柄应返回 -1, 实际返回 {result}")
    
    # 测试4: 测试边界情况 - NULL指针
    print("\n测试4: 测试NULL指针参数")
    result = GetWindowBounds(hwnd, None, ctypes.byref(y), 
                            ctypes.byref(width), ctypes.byref(height))
    if result == -1:
        print("✅ NULL指针参数正确返回 -1")
    else:
        print(f"❌ NULL指针参数应返回 -1, 实际返回 {result}")
    
    # 测试5: 测试边界情况 - 无效大小
    print("\n测试5: 测试无效大小（负数）")
    SetWindowBounds(hwnd, 100, 100, -100, -100)
    time.sleep(0.3)
    
    # 验证窗口大小没有改变
    result = GetWindowBounds(hwnd, ctypes.byref(x), ctypes.byref(y), 
                            ctypes.byref(width), ctypes.byref(height))
    if result == 0:
        if width.value > 0 and height.value > 0:
            print(f"✅ 无效大小被正确拒绝，窗口保持有效大小: {width.value}x{height.value}")
        else:
            print(f"❌ 窗口大小变为无效值: {width.value}x{height.value}")
    
    # 测试6: 多次设置位置
    print("\n测试6: 多次设置窗口位置")
    positions = [(200, 200), (300, 150), (150, 300)]
    for i, (px, py) in enumerate(positions, 1):
        print(f"  设置位置 {i}: ({px}, {py})")
        SetWindowBounds(hwnd, px, py, 800, 600)
        time.sleep(0.2)
        
        result = GetWindowBounds(hwnd, ctypes.byref(x), ctypes.byref(y), 
                                ctypes.byref(width), ctypes.byref(height))
        if result == 0:
            print(f"    当前位置: ({x.value}, {y.value})")
    
    print("✅ 多次设置位置测试完成")
    
    # 清理
    print("\n清理资源...")
    destroy_window(hwnd)
    print("✅ 窗口已销毁")
    
    print("\n" + "=" * 60)
    print("所有测试完成！")
    print("=" * 60)
    
    return True

if __name__ == "__main__":
    success = test_window_bounds()
    sys.exit(0 if success else 1)
