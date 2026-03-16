"""
测试窗口可视状态属性命令
测试GetWindowVisible和ShowEmojiWindow函数
"""
import ctypes
from ctypes import wintypes
import sys
import os
import time

# 添加当前目录到路径
sys.path.insert(0, os.path.dirname(__file__))

# 加载DLL (使用WinDLL因为函数使用__stdcall约定)
try:
    dll = ctypes.WinDLL('./emoji_window.dll')
except OSError:
    print("错误: 无法加载emoji_window.dll")
    sys.exit(1)

# 定义函数原型
# HWND create_window_bytes(const unsigned char* title_bytes, int title_len, int width, int height)
create_window_bytes = dll.create_window_bytes
create_window_bytes.argtypes = [ctypes.POINTER(ctypes.c_ubyte), ctypes.c_int, ctypes.c_int, ctypes.c_int]
create_window_bytes.restype = wintypes.HWND

# int GetWindowVisible(HWND hwnd)
GetWindowVisible = dll.GetWindowVisible
GetWindowVisible.argtypes = [wintypes.HWND]
GetWindowVisible.restype = ctypes.c_int

# void ShowEmojiWindow(HWND hwnd, int visible)
ShowEmojiWindow = dll.ShowEmojiWindow
ShowEmojiWindow.argtypes = [wintypes.HWND, ctypes.c_int]
ShowEmojiWindow.restype = None

# void destroy_window(HWND hwnd)
destroy_window = dll.destroy_window
destroy_window.argtypes = [wintypes.HWND]
destroy_window.restype = None

def test_window_visible():
    """测试窗口可视状态属性"""
    print("=" * 60)
    print("测试窗口可视状态属性命令")
    print("=" * 60)
    
    # 创建测试窗口
    title = "测试窗口可视状态"
    title_bytes = title.encode('utf-8')
    title_array = (ctypes.c_ubyte * len(title_bytes))(*title_bytes)
    
    hwnd = create_window_bytes(title_array, len(title_bytes), 400, 300)
    if not hwnd:
        print("✗ 创建窗口失败")
        return False
    
    print(f"✓ 创建窗口成功 (HWND: {hwnd})")
    time.sleep(0.5)
    
    try:
        # 测试1: 获取初始可视状态（窗口创建后应该是可见的）
        print("\n测试1: 获取初始可视状态")
        visible = GetWindowVisible(hwnd)
        if visible == 1:
            print(f"✓ 窗口初始状态: 可见 (返回值: {visible})")
        else:
            print(f"✗ 窗口初始状态异常 (返回值: {visible})")
            return False
        
        # 测试2: 隐藏窗口
        print("\n测试2: 隐藏窗口")
        ShowEmojiWindow(hwnd, 0)
        time.sleep(0.3)
        visible = GetWindowVisible(hwnd)
        if visible == 0:
            print(f"✓ 窗口已隐藏 (返回值: {visible})")
        else:
            print(f"✗ 隐藏窗口失败 (返回值: {visible})")
            return False
        
        # 测试3: 显示窗口
        print("\n测试3: 显示窗口")
        ShowEmojiWindow(hwnd, 1)
        time.sleep(0.3)
        visible = GetWindowVisible(hwnd)
        if visible == 1:
            print(f"✓ 窗口已显示 (返回值: {visible})")
        else:
            print(f"✗ 显示窗口失败 (返回值: {visible})")
            return False
        
        # 测试4: 多次切换可视状态
        print("\n测试4: 多次切换可视状态")
        for i in range(3):
            ShowEmojiWindow(hwnd, 0)
            time.sleep(0.2)
            visible = GetWindowVisible(hwnd)
            if visible != 0:
                print(f"✗ 第{i+1}次隐藏失败")
                return False
            
            ShowEmojiWindow(hwnd, 1)
            time.sleep(0.2)
            visible = GetWindowVisible(hwnd)
            if visible != 1:
                print(f"✗ 第{i+1}次显示失败")
                return False
        print("✓ 多次切换可视状态成功")
        
        # 测试5: 测试无效句柄
        print("\n测试5: 测试无效句柄")
        result = GetWindowVisible(None)
        if result == -1:
            print(f"✓ 无效句柄返回-1 (返回值: {result})")
        else:
            print(f"✗ 无效句柄应返回-1 (返回值: {result})")
            return False
        
        # 测试6: ShowEmojiWindow对无效句柄的处理
        print("\n测试6: ShowEmojiWindow对无效句柄的处理")
        ShowEmojiWindow(None, 1)  # 应该不崩溃
        print("✓ ShowEmojiWindow正确处理无效句柄")
        
        print("\n" + "=" * 60)
        print("所有测试通过！")
        print("=" * 60)
        return True
        
    finally:
        # 清理资源
        time.sleep(0.5)
        destroy_window(hwnd)
        print("\n✓ 窗口已销毁")

if __name__ == "__main__":
    try:
        success = test_window_visible()
        sys.exit(0 if success else 1)
    except Exception as e:
        print(f"\n✗ 测试过程中发生异常: {e}")
        import traceback
        traceback.print_exc()
        sys.exit(1)
