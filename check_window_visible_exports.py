"""
检查窗口可视状态函数是否正确导出
"""
import ctypes
import sys

try:
    dll = ctypes.WinDLL('./examples/Python/emoji_window.dll')
    
    print("检查函数导出...")
    print("=" * 60)
    
    # 检查GetWindowVisible
    try:
        func = dll.GetWindowVisible
        print("✓ GetWindowVisible 已导出")
    except AttributeError:
        print("✗ GetWindowVisible 未导出")
        sys.exit(1)
    
    # 检查ShowEmojiWindow
    try:
        func = dll.ShowEmojiWindow
        print("✓ ShowEmojiWindow 已导出")
    except AttributeError:
        print("✗ ShowEmojiWindow 未导出")
        sys.exit(1)
    
    print("=" * 60)
    print("所有函数都已正确导出！")
    
except OSError as e:
    print(f"错误: 无法加载DLL - {e}")
    sys.exit(1)
