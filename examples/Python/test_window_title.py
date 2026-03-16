"""
测试GetWindowTitle函数
测试窗口标题获取功能
"""
import ctypes
from ctypes import wintypes
import sys
import os

# 添加当前目录到路径
sys.path.insert(0, os.path.dirname(__file__))

# 加载DLL
try:
    dll = ctypes.CDLL('./emoji_window.dll')
except OSError:
    print("错误: 无法加载emoji_window.dll")
    print("请确保emoji_window.dll在当前目录")
    sys.exit(1)

# 定义函数原型
# HWND __stdcall create_window_bytes(const unsigned char* title_bytes, int title_len, int width, int height)
create_window_bytes = dll.create_window_bytes
create_window_bytes.argtypes = [ctypes.POINTER(ctypes.c_ubyte), ctypes.c_int, ctypes.c_int, ctypes.c_int]
create_window_bytes.restype = wintypes.HWND

# int __stdcall GetWindowTitle(HWND hwnd, unsigned char* buffer, int buffer_size)
GetWindowTitle = dll.GetWindowTitle
GetWindowTitle.argtypes = [wintypes.HWND, ctypes.POINTER(ctypes.c_ubyte), ctypes.c_int]
GetWindowTitle.restype = ctypes.c_int

# void __stdcall destroy_window(HWND hwnd)
destroy_window = dll.destroy_window
destroy_window.argtypes = [wintypes.HWND]
destroy_window.restype = None

def test_get_window_title():
    """测试GetWindowTitle函数"""
    print("=" * 60)
    print("测试GetWindowTitle函数")
    print("=" * 60)
    
    # 测试1: 创建窗口并获取标题
    print("\n测试1: 创建窗口并获取标题")
    title = "测试窗口标题 🎉"
    title_bytes = title.encode('utf-8')
    title_array = (ctypes.c_ubyte * len(title_bytes))(*title_bytes)
    
    hwnd = create_window_bytes(title_array, len(title_bytes), 800, 600)
    if not hwnd:
        print("❌ 创建窗口失败")
        return False
    
    print(f"✓ 创建窗口成功: 0x{hwnd:X}")
    
    # 第一次调用: 获取所需长度
    length = GetWindowTitle(hwnd, None, 0)
    print(f"✓ 标题长度: {length} 字节")
    
    if length <= 0:
        print("❌ 获取标题长度失败")
        destroy_window(hwnd)
        return False
    
    # 第二次调用: 获取实际内容
    buffer = (ctypes.c_ubyte * length)()
    result = GetWindowTitle(hwnd, buffer, length)
    
    if result != length:
        print(f"❌ 获取标题失败: 返回值={result}, 期望={length}")
        destroy_window(hwnd)
        return False
    
    # 转换为字符串
    title_result = bytes(buffer).decode('utf-8')
    print(f"✓ 获取到的标题: {title_result}")
    
    if title_result == title:
        print("✓ 标题匹配成功")
    else:
        print(f"❌ 标题不匹配: 期望='{title}', 实际='{title_result}'")
        destroy_window(hwnd)
        return False
    
    destroy_window(hwnd)
    
    # 测试2: 测试空标题
    print("\n测试2: 测试空标题")
    empty_title = ""
    empty_bytes = empty_title.encode('utf-8')
    empty_array = (ctypes.c_ubyte * len(empty_bytes))(*empty_bytes) if empty_bytes else None
    
    hwnd2 = create_window_bytes(empty_array, len(empty_bytes), 800, 600)
    if not hwnd2:
        print("❌ 创建空标题窗口失败")
        return False
    
    length2 = GetWindowTitle(hwnd2, None, 0)
    print(f"✓ 空标题长度: {length2}")
    
    if length2 == 0:
        print("✓ 空标题测试通过")
    else:
        print(f"❌ 空标题长度应该为0, 实际为{length2}")
        destroy_window(hwnd2)
        return False
    
    destroy_window(hwnd2)
    
    # 测试3: 测试中文标题
    print("\n测试3: 测试中文标题")
    chinese_title = "中文窗口标题测试"
    chinese_bytes = chinese_title.encode('utf-8')
    chinese_array = (ctypes.c_ubyte * len(chinese_bytes))(*chinese_bytes)
    
    hwnd3 = create_window_bytes(chinese_array, len(chinese_bytes), 800, 600)
    if not hwnd3:
        print("❌ 创建中文标题窗口失败")
        return False
    
    length3 = GetWindowTitle(hwnd3, None, 0)
    buffer3 = (ctypes.c_ubyte * length3)()
    result3 = GetWindowTitle(hwnd3, buffer3, length3)
    title_result3 = bytes(buffer3).decode('utf-8')
    
    print(f"✓ 中文标题: {title_result3}")
    
    if title_result3 == chinese_title:
        print("✓ 中文标题测试通过")
    else:
        print(f"❌ 中文标题不匹配")
        destroy_window(hwnd3)
        return False
    
    destroy_window(hwnd3)
    
    # 测试4: 测试emoji标题
    print("\n测试4: 测试emoji标题")
    emoji_title = "Emoji测试 😀🎉🚀"
    emoji_bytes = emoji_title.encode('utf-8')
    emoji_array = (ctypes.c_ubyte * len(emoji_bytes))(*emoji_bytes)
    
    hwnd4 = create_window_bytes(emoji_array, len(emoji_bytes), 800, 600)
    if not hwnd4:
        print("❌ 创建emoji标题窗口失败")
        return False
    
    length4 = GetWindowTitle(hwnd4, None, 0)
    buffer4 = (ctypes.c_ubyte * length4)()
    result4 = GetWindowTitle(hwnd4, buffer4, length4)
    title_result4 = bytes(buffer4).decode('utf-8')
    
    print(f"✓ Emoji标题: {title_result4}")
    
    if title_result4 == emoji_title:
        print("✓ Emoji标题测试通过")
    else:
        print(f"❌ Emoji标题不匹配")
        destroy_window(hwnd4)
        return False
    
    destroy_window(hwnd4)
    
    # 测试5: 测试无效窗口句柄
    print("\n测试5: 测试无效窗口句柄")
    result5 = GetWindowTitle(None, None, 0)
    if result5 == -1:
        print("✓ 无效句柄返回-1")
    else:
        print(f"❌ 无效句柄应返回-1, 实际返回{result5}")
        return False
    
    print("\n" + "=" * 60)
    print("✓ 所有测试通过!")
    print("=" * 60)
    return True

if __name__ == "__main__":
    success = test_get_window_title()
    sys.exit(0 if success else 1)
