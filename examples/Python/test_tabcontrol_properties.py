"""
测试 TabControl 属性命令
测试 GetTabTitle, SetTabTitle, GetTabControlBounds, SetTabControlBounds,
     GetTabControlVisible, ShowTabControl, EnableTabControl
需求: 7.1-7.8
"""
import ctypes
import sys
import os

# 加载DLL
dll_path = os.path.join(os.path.dirname(__file__), '..', '..', 'emoji_window.dll')
dll_path = os.path.abspath(dll_path)
try:
    dll = ctypes.CDLL(dll_path)
except OSError as e:
    print(f"错误: 无法加载emoji_window.dll: {e}")
    sys.exit(1)

# ===== 函数原型定义 =====

dll.create_window_bytes_ex.restype = ctypes.c_void_p
dll.create_window_bytes_ex.argtypes = [ctypes.c_char_p, ctypes.c_int, ctypes.c_int, ctypes.c_int, ctypes.c_int]

dll.CreateTabControl.restype = ctypes.c_void_p
dll.CreateTabControl.argtypes = [ctypes.c_void_p, ctypes.c_int, ctypes.c_int, ctypes.c_int, ctypes.c_int]

dll.AddTabItem.restype = ctypes.c_int
dll.AddTabItem.argtypes = [ctypes.c_void_p, ctypes.c_char_p, ctypes.c_int, ctypes.c_int]

dll.GetTabCount.restype = ctypes.c_int
dll.GetTabCount.argtypes = [ctypes.c_void_p]

dll.GetTabTitle.restype = ctypes.c_int
dll.GetTabTitle.argtypes = [ctypes.c_void_p, ctypes.c_int, ctypes.c_char_p, ctypes.c_int]

dll.SetTabTitle.restype = ctypes.c_int
dll.SetTabTitle.argtypes = [ctypes.c_void_p, ctypes.c_int, ctypes.c_char_p, ctypes.c_int]

dll.GetTabControlBounds.restype = ctypes.c_int
dll.GetTabControlBounds.argtypes = [
    ctypes.c_void_p,
    ctypes.POINTER(ctypes.c_int),
    ctypes.POINTER(ctypes.c_int),
    ctypes.POINTER(ctypes.c_int),
    ctypes.POINTER(ctypes.c_int),
]

dll.SetTabControlBounds.restype = ctypes.c_int
dll.SetTabControlBounds.argtypes = [ctypes.c_void_p, ctypes.c_int, ctypes.c_int, ctypes.c_int, ctypes.c_int]

dll.GetTabControlVisible.restype = ctypes.c_int
dll.GetTabControlVisible.argtypes = [ctypes.c_void_p]

dll.ShowTabControl.restype = ctypes.c_int
dll.ShowTabControl.argtypes = [ctypes.c_void_p, ctypes.c_int]

dll.EnableTabControl.restype = ctypes.c_int
dll.EnableTabControl.argtypes = [ctypes.c_void_p, ctypes.c_int]

dll.DestroyTabControl.restype = None
dll.DestroyTabControl.argtypes = [ctypes.c_void_p]

dll.destroy_window.restype = None
dll.destroy_window.argtypes = [ctypes.c_void_p]


def argb(a, r, g, b):
    return (a << 24) | (r << 16) | (g << 8) | b


def create_window_and_tabcontrol():
    """创建测试窗口和TabControl"""
    title = "测试窗口".encode('utf-8')
    hwnd = dll.create_window_bytes_ex(title, len(title), 800, 600, argb(255, 70, 130, 180))
    assert hwnd, "创建窗口失败"

    htab = dll.CreateTabControl(hwnd, 20, 20, 760, 400)
    assert htab, "创建TabControl失败"

    # 添加3个Tab页
    titles = ["基本信息", "设置选项", "高级功能"]
    for t in titles:
        tb = t.encode('utf-8')
        dll.AddTabItem(htab, tb, len(tb), 0)

    return hwnd, htab


def test_get_tab_title():
    """测试 GetTabTitle - 需求 7.1"""
    print("\n--- 测试 GetTabTitle ---")
    hwnd, htab = create_window_and_tabcontrol()
    try:
        # 第一次调用获取长度
        length = dll.GetTabTitle(htab, 0, None, 0)
        print(f"  Tab[0] 标题字节长度: {length}")
        assert length > 0, f"GetTabTitle 返回长度应 > 0，实际: {length}"

        # 第二次调用获取内容
        buf = ctypes.create_string_buffer(length)
        ret = dll.GetTabTitle(htab, 0, buf, length)
        assert ret > 0, f"第二次调用应返回 > 0，实际: {ret}"
        title = buf.raw[:ret].decode('utf-8', errors='replace')
        print(f"  Tab[0] 标题内容: {title}")
        assert "基本信息" in title, f"标题应包含'基本信息'，实际: {title}"

        # 测试其他Tab页
        length2 = dll.GetTabTitle(htab, 1, None, 0)
        buf2 = ctypes.create_string_buffer(length2)
        dll.GetTabTitle(htab, 1, buf2, length2)
        title2 = buf2.raw[:length2].decode('utf-8', errors='replace')
        print(f"  Tab[1] 标题内容: {title2}")
        assert "设置选项" in title2, f"Tab[1]标题应包含'设置选项'，实际: {title2}"

        # 无效句柄
        ret_invalid = dll.GetTabTitle(None, 0, None, 0)
        assert ret_invalid == -1, f"无效句柄应返回 -1，实际: {ret_invalid}"

        # 无效索引
        ret_invalid2 = dll.GetTabTitle(htab, 99, None, 0)
        assert ret_invalid2 == -1, f"无效索引应返回 -1，实际: {ret_invalid2}"

        print("  ✅ GetTabTitle 测试通过")
        return True
    finally:
        dll.DestroyTabControl(htab)
        dll.destroy_window(hwnd)


def test_set_tab_title():
    """测试 SetTabTitle - 需求 7.2"""
    print("\n--- 测试 SetTabTitle ---")
    hwnd, htab = create_window_and_tabcontrol()
    try:
        new_title = "已修改标题".encode('utf-8')
        ret = dll.SetTabTitle(htab, 0, new_title, len(new_title))
        print(f"  SetTabTitle 返回值: {ret}")
        assert ret == 0, f"SetTabTitle 应返回 0，实际: {ret}"

        # 验证修改后的标题
        length = dll.GetTabTitle(htab, 0, None, 0)
        buf = ctypes.create_string_buffer(length)
        dll.GetTabTitle(htab, 0, buf, length)
        title = buf.raw[:length].decode('utf-8', errors='replace')
        print(f"  修改后 Tab[0] 标题: {title}")
        assert "已修改标题" in title, f"修改后标题应包含'已修改标题'，实际: {title}"

        # 无效句柄
        ret_invalid = dll.SetTabTitle(None, 0, new_title, len(new_title))
        assert ret_invalid == -1, f"无效句柄应返回 -1，实际: {ret_invalid}"

        print("  ✅ SetTabTitle 测试通过")
        return True
    finally:
        dll.DestroyTabControl(htab)
        dll.destroy_window(hwnd)


def test_get_tabcontrol_bounds():
    """测试 GetTabControlBounds - 需求 7.3"""
    print("\n--- 测试 GetTabControlBounds ---")
    hwnd, htab = create_window_and_tabcontrol()
    try:
        x = ctypes.c_int()
        y = ctypes.c_int()
        w = ctypes.c_int()
        h = ctypes.c_int()
        ret = dll.GetTabControlBounds(htab, ctypes.byref(x), ctypes.byref(y), ctypes.byref(w), ctypes.byref(h))
        print(f"  位置: ({x.value}, {y.value})  大小: {w.value}x{h.value}  返回值: {ret}")
        assert ret == 0, f"GetTabControlBounds 应返回 0，实际: {ret}"
        # X坐标应与创建时一致
        assert x.value == 20, f"X 应为 20，实际: {x.value}"
        # Y坐标会加上标题栏偏移(tb_offset)，所以实际Y >= 创建时传入的Y
        assert y.value >= 20, f"Y 应 >= 20，实际: {y.value}"
        assert w.value == 760, f"宽度应为 760，实际: {w.value}"
        assert h.value == 400, f"高度应为 400，实际: {h.value}"

        # 无效句柄
        ret_invalid = dll.GetTabControlBounds(None, ctypes.byref(x), ctypes.byref(y), ctypes.byref(w), ctypes.byref(h))
        assert ret_invalid == -1, f"无效句柄应返回 -1，实际: {ret_invalid}"

        print("  ✅ GetTabControlBounds 测试通过")
        return True
    finally:
        dll.DestroyTabControl(htab)
        dll.destroy_window(hwnd)


def test_set_tabcontrol_bounds():
    """测试 SetTabControlBounds - 需求 7.4"""
    print("\n--- 测试 SetTabControlBounds ---")
    hwnd, htab = create_window_and_tabcontrol()
    try:
        # 先获取当前Y偏移量
        x0 = ctypes.c_int()
        y0 = ctypes.c_int()
        w0 = ctypes.c_int()
        h0 = ctypes.c_int()
        dll.GetTabControlBounds(htab, ctypes.byref(x0), ctypes.byref(y0), ctypes.byref(w0), ctypes.byref(h0))
        tb_offset = y0.value - 20  # 标题栏偏移量

        ret = dll.SetTabControlBounds(htab, 50, 60, 700, 350)
        print(f"  SetTabControlBounds 返回值: {ret}")
        assert ret == 0, f"SetTabControlBounds 应返回 0，实际: {ret}"

        # 验证修改后的位置（Y坐标同样会加上tb_offset）
        x = ctypes.c_int()
        y = ctypes.c_int()
        w = ctypes.c_int()
        h = ctypes.c_int()
        dll.GetTabControlBounds(htab, ctypes.byref(x), ctypes.byref(y), ctypes.byref(w), ctypes.byref(h))
        print(f"  修改后位置: ({x.value}, {y.value})  大小: {w.value}x{h.value}  (tb_offset={tb_offset})")
        assert x.value == 50, f"修改后X应为50，实际: {x.value}"
        assert y.value == 60 + tb_offset, f"修改后Y应为{60 + tb_offset}，实际: {y.value}"
        assert w.value == 700, f"修改后宽度应为700，实际: {w.value}"
        assert h.value == 350, f"修改后高度应为350，实际: {h.value}"

        # 无效句柄
        ret_invalid = dll.SetTabControlBounds(None, 0, 0, 100, 100)
        assert ret_invalid == -1, f"无效句柄应返回 -1，实际: {ret_invalid}"

        print("  ✅ SetTabControlBounds 测试通过")
        return True
    finally:
        dll.DestroyTabControl(htab)
        dll.destroy_window(hwnd)


def test_get_tabcontrol_visible():
    """测试 GetTabControlVisible - 需求 7.5"""
    print("\n--- 测试 GetTabControlVisible ---")
    hwnd, htab = create_window_and_tabcontrol()
    try:
        # 默认可见
        vis = dll.GetTabControlVisible(htab)
        print(f"  默认可视状态: {vis}")
        assert vis == 1, f"默认应可见(1)，实际: {vis}"

        # 隐藏后
        dll.ShowTabControl(htab, 0)
        vis = dll.GetTabControlVisible(htab)
        print(f"  隐藏后可视状态: {vis}")
        assert vis == 0, f"隐藏后应不可见(0)，实际: {vis}"

        # 显示后
        dll.ShowTabControl(htab, 1)
        vis = dll.GetTabControlVisible(htab)
        print(f"  显示后可视状态: {vis}")
        assert vis == 1, f"显示后应可见(1)，实际: {vis}"

        # 无效句柄
        ret_invalid = dll.GetTabControlVisible(None)
        assert ret_invalid == -1, f"无效句柄应返回 -1，实际: {ret_invalid}"

        print("  ✅ GetTabControlVisible 测试通过")
        return True
    finally:
        dll.DestroyTabControl(htab)
        dll.destroy_window(hwnd)


def test_show_tabcontrol():
    """测试 ShowTabControl - 需求 7.6"""
    print("\n--- 测试 ShowTabControl ---")
    hwnd, htab = create_window_and_tabcontrol()
    try:
        # 隐藏
        ret = dll.ShowTabControl(htab, 0)
        print(f"  隐藏返回值: {ret}")
        assert ret == 0, f"ShowTabControl(0) 应返回 0，实际: {ret}"
        assert dll.GetTabControlVisible(htab) == 0, "隐藏后应不可见"

        # 显示
        ret = dll.ShowTabControl(htab, 1)
        print(f"  显示返回值: {ret}")
        assert ret == 0, f"ShowTabControl(1) 应返回 0，实际: {ret}"
        assert dll.GetTabControlVisible(htab) == 1, "显示后应可见"

        # 无效句柄
        ret_invalid = dll.ShowTabControl(None, 1)
        assert ret_invalid == -1, f"无效句柄应返回 -1，实际: {ret_invalid}"

        print("  ✅ ShowTabControl 测试通过")
        return True
    finally:
        dll.DestroyTabControl(htab)
        dll.destroy_window(hwnd)


def test_enable_tabcontrol():
    """测试 EnableTabControl - 需求 7.7, 7.8"""
    print("\n--- 测试 EnableTabControl ---")
    hwnd, htab = create_window_and_tabcontrol()
    try:
        # 禁用
        ret = dll.EnableTabControl(htab, 0)
        print(f"  禁用返回值: {ret}")
        assert ret == 0, f"EnableTabControl(0) 应返回 0，实际: {ret}"

        # 启用
        ret = dll.EnableTabControl(htab, 1)
        print(f"  启用返回值: {ret}")
        assert ret == 0, f"EnableTabControl(1) 应返回 0，实际: {ret}"

        # 无效句柄
        ret_invalid = dll.EnableTabControl(None, 1)
        assert ret_invalid == -1, f"无效句柄应返回 -1，实际: {ret_invalid}"

        print("  ✅ EnableTabControl 测试通过")
        return True
    finally:
        dll.DestroyTabControl(htab)
        dll.destroy_window(hwnd)


def main():
    print("=" * 60)
    print("测试 TabControl 属性命令")
    print("=" * 60)

    tests = [
        test_get_tab_title,
        test_set_tab_title,
        test_get_tabcontrol_bounds,
        test_set_tabcontrol_bounds,
        test_get_tabcontrol_visible,
        test_show_tabcontrol,
        test_enable_tabcontrol,
    ]

    passed = 0
    failed = 0
    for t in tests:
        try:
            if t():
                passed += 1
            else:
                failed += 1
        except AssertionError as e:
            print(f"  ❌ 断言失败: {e}")
            failed += 1
        except Exception as e:
            print(f"  ❌ 异常: {e}")
            import traceback
            traceback.print_exc()
            failed += 1

    print("\n" + "=" * 60)
    print(f"结果: {passed} 通过 / {failed} 失败")
    print("=" * 60)
    return failed == 0


if __name__ == "__main__":
    success = main()
    sys.exit(0 if success else 1)
