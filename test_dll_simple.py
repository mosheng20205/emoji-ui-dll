import ctypes
import sys
import traceback

print("=== 测试DLL加载 ===\n")

try:
    # 尝试加载DLL
    print("正在加载 Release/emoji_window.dll...")
    dll = ctypes.WinDLL('./Release/emoji_window.dll')
    print("✓ DLL加载成功\n")
    
    # 尝试访问一个已知存在的函数
    print("测试已知函数 create_window...")
    try:
        create_window = dll.create_window
        print("✓ create_window 可访问\n")
    except Exception as e:
        print(f"✗ create_window 不可访问: {e}\n")
    
    # 测试GetButtonVisible
    print("测试 GetButtonVisible...")
    try:
        GetButtonVisible = dll.GetButtonVisible
        print("✓ GetButtonVisible 可访问")
        print(f"  函数地址: {GetButtonVisible}")
    except Exception as e:
        print(f"✗ GetButtonVisible 不可访问")
        print(f"  错误: {e}")
        traceback.print_exc()
    
    print("\n测试 ShowButton...")
    try:
        ShowButton = dll.ShowButton
        print("✓ ShowButton 可访问")
        print(f"  函数地址: {ShowButton}")
    except Exception as e:
        print(f"✗ ShowButton 不可访问")
        print(f"  错误: {e}")
        traceback.print_exc()
        
except Exception as e:
    print(f"严重错误: {e}")
    traceback.print_exc()
    sys.exit(1)
