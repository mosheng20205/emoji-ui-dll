"""
测试批量操作函数
测试 RemoveAllTabs、InsertTabItem、MoveTabItem、GetTabIndexByTitle
覆盖清空、插入位置（头部/中间/尾部/越界）、移动、标题查找（中文/emoji/不存在）等场景

测试的功能点:
- RemoveAllTabs: 清空所有标签页
- InsertTabItem: 在指定位置插入标签页
- MoveTabItem: 移动标签页位置
- GetTabIndexByTitle: 根据标题查找标签页索引

预期结果:
- RemoveAllTabs: 有效句柄返回 0，无效句柄返回 -1
- InsertTabItem: 返回实际插入索引，无效参数返回 -1
- MoveTabItem: 成功返回 0，fromIndex==toIndex 返回 0，无效索引返回 -1
- GetTabIndexByTitle: 返回匹配索引，未找到返回 -1
"""
import ctypes
from ctypes import wintypes
import sys
import os

# 添加当前目录到路径
sys.path.insert(0, os.path.dirname(__file__))

if hasattr(sys.stdout, "reconfigure"):
    try:
        sys.stdout.reconfigure(encoding="utf-8", errors="replace")
        sys.stderr.reconfigure(encoding="utf-8", errors="replace")
    except Exception:
        pass

# 加载DLL
try:
    dll = ctypes.CDLL('./emoji_window.dll')
except OSError:
    print("错误: 无法加载 emoji_window.dll")
    sys.exit(1)

# ========== 定义函数原型 ==========

dll.create_window.argtypes = [ctypes.c_char_p, ctypes.c_int, ctypes.c_int]
dll.create_window.restype = wintypes.HWND

dll.destroy_window.argtypes = [wintypes.HWND]
dll.destroy_window.restype = None

dll.CreateTabControl.argtypes = [wintypes.HWND, ctypes.c_int, ctypes.c_int, ctypes.c_int, ctypes.c_int]
dll.CreateTabControl.restype = wintypes.HWND

dll.AddTabItem.argtypes = [wintypes.HWND, ctypes.POINTER(ctypes.c_ubyte), ctypes.c_int, wintypes.HWND]
dll.AddTabItem.restype = ctypes.c_int

dll.GetTabCount.argtypes = [wintypes.HWND]
dll.GetTabCount.restype = ctypes.c_int

dll.GetCurrentTabIndex.argtypes = [wintypes.HWND]
dll.GetCurrentTabIndex.restype = ctypes.c_int

dll.DestroyTabControl.argtypes = [wintypes.HWND]
dll.DestroyTabControl.restype = None

# DLL: int GetTabTitle(HWND, int index, unsigned char* buffer, int bufferSize)
# buffer==NULL 时返回 UTF-8 字节长度；否则写入 buffer（bufferSize 须足够）
dll.GetTabTitle.argtypes = [wintypes.HWND, ctypes.c_int, ctypes.c_void_p, ctypes.c_int]
dll.GetTabTitle.restype = ctypes.c_int

dll.SelectTab.argtypes = [wintypes.HWND, ctypes.c_int]
dll.SelectTab.restype = wintypes.BOOL

# RemoveAllTabs
dll.RemoveAllTabs.argtypes = [wintypes.HWND]
dll.RemoveAllTabs.restype = ctypes.c_int

# InsertTabItem
dll.InsertTabItem.argtypes = [wintypes.HWND, ctypes.c_int, ctypes.POINTER(ctypes.c_ubyte), ctypes.c_int, wintypes.HWND]
dll.InsertTabItem.restype = ctypes.c_int

# MoveTabItem
dll.MoveTabItem.argtypes = [wintypes.HWND, ctypes.c_int, ctypes.c_int]
dll.MoveTabItem.restype = ctypes.c_int

# GetTabIndexByTitle
dll.GetTabIndexByTitle.argtypes = [wintypes.HWND, ctypes.POINTER(ctypes.c_ubyte), ctypes.c_int]
dll.GetTabIndexByTitle.restype = ctypes.c_int


def make_utf8_bytes(text):
    """将 Python 字符串转换为 UTF-8 字节数组指针和长度"""
    encoded = text.encode('utf-8')
    arr = (ctypes.c_ubyte * len(encoded))(*encoded)
    return arr, len(encoded)


def get_tab_title(hTab, index):
    """获取标签页标题（先取长度再取内容，与 DLL 导出一致）"""
    n = dll.GetTabTitle(hTab, index, None, 0)
    if n <= 0:
        return ""
    buf = (ctypes.c_ubyte * n)()
    r = dll.GetTabTitle(hTab, index, buf, n)
    if r < 0:
        return ""
    return bytes(buf[:r]).decode("utf-8")


def add_tabs(hTab, titles):
    """批量添加标签页，返回索引列表"""
    indices = []
    for t in titles:
        b, l = make_utf8_bytes(t)
        idx = dll.AddTabItem(hTab, b, l, None)
        indices.append(idx)
    return indices


def test_tab_batch_ops():
    """测试批量操作函数"""
    print("=" * 60)
    print("测试批量操作函数")
    print("=" * 60)

    all_passed = True
    INVALID_HWND = wintypes.HWND(0x12345678)

    # 创建测试窗口和 TabControl
    print("\n[准备] 创建测试窗口和 TabControl...")
    hwnd = dll.create_window(b"Test Batch Ops", 800, 600)
    if not hwnd:
        print("  错误: 无法创建窗口")
        return False

    hTab = dll.CreateTabControl(hwnd, 10, 10, 780, 560)
    if not hTab:
        print("  错误: 无法创建 TabControl")
        dll.destroy_window(hwnd)
        return False
    print("  创建成功")

    # ========== 测试 RemoveAllTabs ==========
    print("\n--- 测试 RemoveAllTabs ---")

    # 添加几个标签页后清空
    add_tabs(hTab, ["页面A", "页面B", "页面C"])
    count_before = dll.GetTabCount(hTab)
    print(f"  清空前标签页数量: {count_before}")

    ret = dll.RemoveAllTabs(hTab)
    count_after = dll.GetTabCount(hTab)
    current_after = dll.GetCurrentTabIndex(hTab)
    if ret == 0 and count_after == 0 and current_after == -1:
        print(f"  [通过] RemoveAllTabs 返回 0，数量={count_after}，currentIndex={current_after}")
    else:
        print(f"  [失败] ret={ret}，数量={count_after}，currentIndex={current_after}")
        all_passed = False

    # 空 TabControl 再次清空
    ret = dll.RemoveAllTabs(hTab)
    if ret == 0:
        print("  [通过] 空 TabControl 清空返回 0")
    else:
        print(f"  [失败] 空 TabControl 清空返回 {ret}，期望 0")
        all_passed = False

    # 无效句柄
    ret = dll.RemoveAllTabs(INVALID_HWND)
    if ret == -1:
        print("  [通过] 无效句柄返回 -1")
    else:
        print(f"  [失败] 无效句柄返回 {ret}，期望 -1")
        all_passed = False

    # ========== 测试 InsertTabItem ==========
    print("\n--- 测试 InsertTabItem ---")

    # 先清空，从头开始
    dll.RemoveAllTabs(hTab)

    # 插入第一个标签页（空 TabControl，index=0）
    b, l = make_utf8_bytes("第一页")
    ret = dll.InsertTabItem(hTab, 0, b, l, None)
    if ret == 0:
        print(f"  [通过] 插入到空 TabControl index=0，返回 {ret}")
    else:
        print(f"  [失败] 插入到空 TabControl 返回 {ret}，期望 0")
        all_passed = False

    # 追加到末尾（index=1）
    b, l = make_utf8_bytes("第三页")
    ret = dll.InsertTabItem(hTab, 1, b, l, None)
    if ret == 1:
        print(f"  [通过] 追加到末尾 index=1，返回 {ret}")
    else:
        print(f"  [失败] 追加到末尾返回 {ret}，期望 1")
        all_passed = False

    # 在中间插入（index=1，在"第一页"和"第三页"之间）
    b, l = make_utf8_bytes("第二页")
    ret = dll.InsertTabItem(hTab, 1, b, l, None)
    if ret == 1:
        print(f"  [通过] 中间插入 index=1，返回 {ret}")
    else:
        print(f"  [失败] 中间插入返回 {ret}，期望 1")
        all_passed = False

    # 验证顺序
    t0 = get_tab_title(hTab, 0)
    t1 = get_tab_title(hTab, 1)
    t2 = get_tab_title(hTab, 2)
    if t0 == "第一页" and t1 == "第二页" and t2 == "第三页":
        print(f"  [通过] 标签顺序正确: [{t0}, {t1}, {t2}]")
    else:
        print(f"  [失败] 标签顺序错误: [{t0}, {t1}, {t2}]，期望 [第一页, 第二页, 第三页]")
        all_passed = False

    # 在头部插入（index=0）
    b, l = make_utf8_bytes("头部页")
    ret = dll.InsertTabItem(hTab, 0, b, l, None)
    if ret == 0:
        print(f"  [通过] 头部插入 index=0，返回 {ret}")
    else:
        print(f"  [失败] 头部插入返回 {ret}，期望 0")
        all_passed = False

    # index 越界（超出范围，应追加到末尾）
    b, l = make_utf8_bytes("尾部页")
    count = dll.GetTabCount(hTab)
    ret = dll.InsertTabItem(hTab, 999, b, l, None)
    if ret == count:
        print(f"  [通过] 越界 index=999 追加到末尾，返回 {ret}")
    else:
        print(f"  [失败] 越界 index=999 返回 {ret}，期望 {count}")
        all_passed = False

    # index < 0 返回 -1
    b, l = make_utf8_bytes("负数页")
    ret = dll.InsertTabItem(hTab, -1, b, l, None)
    if ret == -1:
        print("  [通过] index=-1 返回 -1")
    else:
        print(f"  [失败] index=-1 返回 {ret}，期望 -1")
        all_passed = False

    # 无效句柄
    b, l = make_utf8_bytes("无效")
    ret = dll.InsertTabItem(INVALID_HWND, 0, b, l, None)
    if ret == -1:
        print("  [通过] 无效句柄返回 -1")
    else:
        print(f"  [失败] 无效句柄返回 {ret}，期望 -1")
        all_passed = False

    # 测试 currentIndex 更新：选中 index=2，在 index=1 插入，currentIndex 应变为 3
    dll.RemoveAllTabs(hTab)
    add_tabs(hTab, ["A", "B", "C", "D"])
    dll.SelectTab(hTab, 2)
    cur_before = dll.GetCurrentTabIndex(hTab)
    b, l = make_utf8_bytes("新页")
    dll.InsertTabItem(hTab, 1, b, l, None)
    cur_after = dll.GetCurrentTabIndex(hTab)
    if cur_before == 2 and cur_after == 3:
        print(f"  [通过] 插入前 currentIndex={cur_before}，插入后={cur_after}")
    else:
        print(f"  [失败] 插入前 currentIndex={cur_before}，插入后={cur_after}，期望 2→3")
        all_passed = False

    # ========== 测试 MoveTabItem ==========
    print("\n--- 测试 MoveTabItem ---")

    dll.RemoveAllTabs(hTab)
    add_tabs(hTab, ["A", "B", "C", "D", "E"])

    # fromIndex == toIndex 返回 0
    ret = dll.MoveTabItem(hTab, 2, 2)
    if ret == 0:
        print("  [通过] fromIndex==toIndex 返回 0")
    else:
        print(f"  [失败] fromIndex==toIndex 返回 {ret}，期望 0")
        all_passed = False

    # 向后移动：将 index=0 移到 index=3
    # 当前: [A, B, C, D, E] → 移动后: [B, C, D, A, E]
    ret = dll.MoveTabItem(hTab, 0, 3)
    t0 = get_tab_title(hTab, 0)
    t3 = get_tab_title(hTab, 3)
    if ret == 0 and t0 == "B" and t3 == "A":
        print(f"  [通过] 向后移动 0→3: [{t0},...,{t3},...] 返回 {ret}")
    else:
        titles = [get_tab_title(hTab, i) for i in range(dll.GetTabCount(hTab))]
        print(f"  [失败] 向后移动 0→3 返回 {ret}，顺序={titles}")
        all_passed = False

    # 向前移动：将 index=3 移到 index=0
    # 当前: [B, C, D, A, E] → 移动后: [A, B, C, D, E]
    ret = dll.MoveTabItem(hTab, 3, 0)
    titles = [get_tab_title(hTab, i) for i in range(dll.GetTabCount(hTab))]
    if ret == 0 and titles == ["A", "B", "C", "D", "E"]:
        print(f"  [通过] 向前移动 3→0: {titles}")
    else:
        print(f"  [失败] 向前移动 3→0 返回 {ret}，顺序={titles}，期望 [A,B,C,D,E]")
        all_passed = False

    # 移动当前选中页：选中 index=1(B)，移到 index=4
    dll.SelectTab(hTab, 1)
    dll.MoveTabItem(hTab, 1, 4)
    cur = dll.GetCurrentTabIndex(hTab)
    if cur == 4:
        print(f"  [通过] 移动选中页 1→4，currentIndex={cur}")
    else:
        print(f"  [失败] 移动选中页 1→4，currentIndex={cur}，期望 4")
        all_passed = False

    # 无效索引
    ret = dll.MoveTabItem(hTab, -1, 2)
    if ret == -1:
        print("  [通过] fromIndex=-1 返回 -1")
    else:
        print(f"  [失败] fromIndex=-1 返回 {ret}，期望 -1")
        all_passed = False

    ret = dll.MoveTabItem(hTab, 0, 100)
    if ret == -1:
        print("  [通过] toIndex=100 返回 -1")
    else:
        print(f"  [失败] toIndex=100 返回 {ret}，期望 -1")
        all_passed = False

    # 无效句柄
    ret = dll.MoveTabItem(INVALID_HWND, 0, 1)
    if ret == -1:
        print("  [通过] 无效句柄返回 -1")
    else:
        print(f"  [失败] 无效句柄返回 {ret}，期望 -1")
        all_passed = False

    # ========== 测试 GetTabIndexByTitle ==========
    print("\n--- 测试 GetTabIndexByTitle ---")

    dll.RemoveAllTabs(hTab)
    add_tabs(hTab, ["首页", "设置", "关于"])

    # 精确匹配
    b, l = make_utf8_bytes("设置")
    ret = dll.GetTabIndexByTitle(hTab, b, l)
    if ret == 1:
        print(f"  [通过] 查找'设置'返回 {ret}")
    else:
        print(f"  [失败] 查找'设置'返回 {ret}，期望 1")
        all_passed = False

    # 查找第一个
    b, l = make_utf8_bytes("首页")
    ret = dll.GetTabIndexByTitle(hTab, b, l)
    if ret == 0:
        print(f"  [通过] 查找'首页'返回 {ret}")
    else:
        print(f"  [失败] 查找'首页'返回 {ret}，期望 0")
        all_passed = False

    # 不存在的标题
    b, l = make_utf8_bytes("不存在")
    ret = dll.GetTabIndexByTitle(hTab, b, l)
    if ret == -1:
        print("  [通过] 查找不存在标题返回 -1")
    else:
        print(f"  [失败] 查找不存在标题返回 {ret}，期望 -1")
        all_passed = False

    # 中文标题
    dll.RemoveAllTabs(hTab)
    add_tabs(hTab, ["中文标签", "测试页面"])
    b, l = make_utf8_bytes("中文标签")
    ret = dll.GetTabIndexByTitle(hTab, b, l)
    if ret == 0:
        print(f"  [通过] 查找中文标题'中文标签'返回 {ret}")
    else:
        print(f"  [失败] 查找中文标题返回 {ret}，期望 0")
        all_passed = False

    # Emoji 标题
    dll.RemoveAllTabs(hTab)
    add_tabs(hTab, ["😀笑脸", "🎨调色板", "普通页"])
    b, l = make_utf8_bytes("🎨调色板")
    ret = dll.GetTabIndexByTitle(hTab, b, l)
    if ret == 1:
        print(f"  [通过] 查找 emoji 标题'🎨调色板'返回 {ret}")
    else:
        print(f"  [失败] 查找 emoji 标题返回 {ret}，期望 1")
        all_passed = False

    # 区分大小写
    dll.RemoveAllTabs(hTab)
    add_tabs(hTab, ["Hello", "hello", "HELLO"])
    b, l = make_utf8_bytes("hello")
    ret = dll.GetTabIndexByTitle(hTab, b, l)
    if ret == 1:
        print(f"  [通过] 区分大小写查找'hello'返回 {ret}（跳过'Hello'）")
    else:
        print(f"  [失败] 区分大小写查找'hello'返回 {ret}，期望 1")
        all_passed = False

    # titleBytes 为 NULL
    ret = dll.GetTabIndexByTitle(hTab, None, 0)
    if ret == -1:
        print("  [通过] titleBytes=NULL 返回 -1")
    else:
        print(f"  [失败] titleBytes=NULL 返回 {ret}，期望 -1")
        all_passed = False

    # 无效句柄
    b, l = make_utf8_bytes("test")
    ret = dll.GetTabIndexByTitle(INVALID_HWND, b, l)
    if ret == -1:
        print("  [通过] 无效句柄返回 -1")
    else:
        print(f"  [失败] 无效句柄返回 {ret}，期望 -1")
        all_passed = False

    # ========== 清理 ==========
    print("\n[清理] 销毁 TabControl 和窗口...")
    dll.DestroyTabControl(hTab)
    dll.destroy_window(hwnd)
    print("  清理完成")

    # ========== 结果 ==========
    print("\n" + "=" * 60)
    if all_passed:
        print("所有测试通过!")
    else:
        print("部分测试失败!")
    print("=" * 60)

    return all_passed


if __name__ == "__main__":
    success = test_tab_batch_ops()
    sys.exit(0 if success else 1)
