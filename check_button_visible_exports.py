import ctypes
import sys

try:
    # 加载DLL
    dll = ctypes.WinDLL('./emoji_window.dll')
    
    print("=== 检查按钮可视状态函数导出 ===\n")
    
    # 检查GetButtonVisible
    try:
        GetButtonVisible = dll.GetButtonVisible
        GetButtonVisible.argtypes = [ctypes.c_int]
        GetButtonVisible.restype = ctypes.c_int
        print("✓ GetButtonVisible 函数已导出")
    except AttributeError as e:
        print(f"✗ GetButtonVisible 函数未导出: {e}")
        sys.exit(1)
    except Exception as e:
        print(f"✗ GetButtonVisible 检查出错: {e}")
        sys.exit(1)
    
    # 检查ShowButton
    try:
        ShowButton = dll.ShowButton
        ShowButton.argtypes = [ctypes.c_int, ctypes.c_int]
        ShowButton.restype = None
        print("✓ ShowButton 函数已导出")
    except AttributeError:
        print("✗ ShowButton 函数未导出")
        sys.exit(1)
    
    # 检查GetButtonEnabled（作为对比）
    try:
        GetButtonEnabled = dll.GetButtonEnabled
        GetButtonEnabled.argtypes = [ctypes.c_int]
        GetButtonEnabled.restype = ctypes.c_int
        print("✓ GetButtonEnabled 函数已导出")
    except AttributeError:
        print("✗ GetButtonEnabled 函数未导出")
    
    print("\n=== 所有必需函数都已正确导出 ===")
    
except Exception as e:
    print(f"错误: {e}")
    sys.exit(1)
