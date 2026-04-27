"""
emoji_window.dll 的 Python 封装
使用 ctypes 调用 C++ DLL
"""

import ctypes
from ctypes import c_int, c_uint, c_bool, c_void_p, c_char_p, POINTER, WINFUNCTYPE
import sys
import os


class EmojiWindow:
    """emoji_window.dll 的 Python 封装类"""
    
    def __init__(self, dll_path=None):
        """
        初始化 DLL
        
        Args:
            dll_path: DLL 文件路径，默认自动搜索
        """
        if dll_path is None:
            dll_path = self._find_dll()
        
        if not dll_path:
            print("❌ 找不到可用的 emoji_window.dll")
            print("请将 DLL 文件放在以下位置之一：")
            print("  1. 当前目录")
            print("  2. ../../x64/Release/emoji_window.dll")
            print("  3. ../../Release/emoji_window.dll")
            sys.exit(1)
        
        try:
            # 使用绝对路径加载 DLL
            abs_path = os.path.abspath(dll_path)
            self.dll = ctypes.WinDLL(abs_path)
            self._setup_functions()
            print(f"✅ 成功加载 DLL: {dll_path}")
            print(f"   绝对路径: {abs_path}")
        except Exception as e:
            print(f"❌ 加载 DLL 失败: {e}")
            print(f"   DLL 路径: {dll_path}")
            print()
            print("可能的原因：")
            print("1. Python 位数与 DLL 位数不匹配")
            print("   - 检查: python -c \"import struct; print(struct.calcsize('P') * 8, 'bit')\"")
            print("2. 缺少 Visual C++ 运行库")
            print("   - 下载: https://aka.ms/vs/17/release/vc_redist.x64.exe")
            sys.exit(1)
    
    def _find_dll(self):
        """查找可用的 DLL 文件"""
        # 可能的 DLL 路径
        paths = [
            "emoji_window.dll",
            "../../x64/Release/emoji_window.dll",
            "../../Release/emoji_window.dll",
            "../../../x64/Release/emoji_window.dll",
            "../../../Release/emoji_window.dll",
        ]
        
        for path in paths:
            if os.path.exists(path):
                # 尝试加载
                try:
                    abs_path = os.path.abspath(path)
                    test_dll = ctypes.WinDLL(abs_path)
                    # 加载成功，返回这个路径
                    return path
                except:
                    # 加载失败，继续尝试下一个
                    continue
        
        return None
    
    def _setup_functions(self):
        """设置 DLL 函数签名"""
        
        # 窗口管理
        self.dll.create_window_bytes.argtypes = [c_char_p, c_int, c_int, c_int]
        self.dll.create_window_bytes.restype = c_void_p
        
        self.dll.set_message_loop_main_window.argtypes = [c_void_p]
        self.dll.set_message_loop_main_window.restype = None
        
        self.dll.run_message_loop.argtypes = []
        self.dll.run_message_loop.restype = c_int
        
        self.dll.destroy_window.argtypes = [c_void_p]
        self.dll.destroy_window.restype = None
        
        # 按钮
        self.dll.create_emoji_button_bytes.argtypes = [
            c_void_p, c_char_p, c_int, c_char_p, c_int,
            c_int, c_int, c_int, c_int, c_uint
        ]
        self.dll.create_emoji_button_bytes.restype = c_int
        
        # 按钮回调类型
        self.ButtonClickCallback = WINFUNCTYPE(None, c_int, c_void_p)
        self.dll.set_button_click_callback.argtypes = [self.ButtonClickCallback]
        self.dll.set_button_click_callback.restype = None
        
        # 标签
        self.dll.CreateLabel.argtypes = [
            c_void_p, c_int, c_int, c_int, c_int,
            c_char_p, c_int, c_uint, c_uint,
            c_char_p, c_int, c_int, c_bool, c_bool, c_bool,
            c_int, c_bool
        ]
        self.dll.CreateLabel.restype = c_void_p
        
        self.dll.SetLabelText.argtypes = [c_void_p, c_char_p, c_int]
        self.dll.SetLabelText.restype = None
        
        # 分组框
        self.dll.CreateGroupBox.argtypes = [
            c_void_p, c_int, c_int, c_int, c_int,
            c_char_p, c_int, c_uint, c_uint
        ]
        self.dll.CreateGroupBox.restype = c_void_p
        
        # 复选框
        self.dll.CreateCheckBox.argtypes = [
            c_void_p, c_int, c_int, c_int, c_int,
            c_char_p, c_int, c_bool, c_uint, c_uint
        ]
        self.dll.CreateCheckBox.restype = c_void_p
        
        self.dll.GetCheckBoxState.argtypes = [c_void_p]
        self.dll.GetCheckBoxState.restype = c_bool
        
        # 复选框回调类型
        self.CheckBoxCallback = WINFUNCTYPE(None, c_void_p, c_bool)
        self.dll.SetCheckBoxCallback.argtypes = [c_void_p, self.CheckBoxCallback]
        self.dll.SetCheckBoxCallback.restype = None
        
        # 信息框
        self.dll.show_message_box_bytes.argtypes = [
            c_void_p, c_char_p, c_int, c_char_p, c_int, c_char_p, c_int
        ]
        self.dll.show_message_box_bytes.restype = None

        # 通用对话框 / 文件拖拽 / Excel读取
        self.FileDropCallback = WINFUNCTYPE(
            None, c_void_p, c_void_p, c_int, c_int, c_int, c_int, c_int
        )
        self.dll.ShowOpenFileDialog.argtypes = [
            c_void_p, c_char_p, c_int, c_char_p, c_int,
            c_char_p, c_int, c_int, c_void_p, c_int
        ]
        self.dll.ShowOpenFileDialog.restype = c_int
        self.dll.ShowSaveFileDialog.argtypes = [
            c_void_p, c_char_p, c_int, c_char_p, c_int,
            c_char_p, c_int, c_char_p, c_int, c_void_p, c_int
        ]
        self.dll.ShowSaveFileDialog.restype = c_int
        self.dll.ShowSelectFolderDialog.argtypes = [
            c_void_p, c_char_p, c_int, c_char_p, c_int, c_void_p, c_int
        ]
        self.dll.ShowSelectFolderDialog.restype = c_int
        self.dll.ShowColorDialog.argtypes = [c_void_p, c_uint, POINTER(c_uint)]
        self.dll.ShowColorDialog.restype = c_int
        self.dll.EnableFileDrop.argtypes = [c_void_p, c_int]
        self.dll.EnableFileDrop.restype = c_int
        self.dll.SetFileDropCallback.argtypes = [c_void_p, self.FileDropCallback]
        self.dll.SetFileDropCallback.restype = None

        self.dll.Excel_OpenWorkbook.argtypes = [c_char_p, c_int, c_int]
        self.dll.Excel_OpenWorkbook.restype = c_int
        self.dll.Excel_CloseWorkbook.argtypes = [c_int]
        self.dll.Excel_CloseWorkbook.restype = None
        self.dll.Excel_CloseAllWorkbooks.argtypes = []
        self.dll.Excel_CloseAllWorkbooks.restype = None
        self.dll.Excel_GetRowCount.argtypes = [c_int]
        self.dll.Excel_GetRowCount.restype = c_int
        self.dll.Excel_GetColumnCount.argtypes = [c_int]
        self.dll.Excel_GetColumnCount.restype = c_int
        self.dll.Excel_GetCellText.argtypes = [c_int, c_int, c_int, c_void_p, c_int]
        self.dll.Excel_GetCellText.restype = c_int
        self.dll.Excel_LoadWorkbookToDataGrid.argtypes = [c_int, c_void_p, c_int]
        self.dll.Excel_LoadWorkbookToDataGrid.restype = c_int
        self.dll.Excel_ReadFileToDataGrid.argtypes = [c_char_p, c_int, c_void_p, c_int, c_int]
        self.dll.Excel_ReadFileToDataGrid.restype = c_int
    
    @staticmethod
    def str_to_utf8(text):
        """将字符串转换为 UTF-8 字节"""
        if isinstance(text, str):
            return text.encode('utf-8')
        return text
    
    @staticmethod
    def argb(a, r, g, b):
        """创建 ARGB 颜色值"""
        return (a << 24) | (r << 16) | (g << 8) | b
    
    def create_window(self, title, width, height):
        """
        创建窗口
        
        Args:
            title: 窗口标题（支持 Unicode 表情）
            width: 窗口宽度
            height: 窗口高度
            
        Returns:
            窗口句柄
        """
        title_bytes = self.str_to_utf8(title)
        hwnd = self.dll.create_window_bytes(title_bytes, len(title_bytes), width, height)
        return hwnd
    
    def create_button(self, parent, emoji, text, x, y, width, height, bg_color):
        """
        创建按钮
        
        Args:
            parent: 父窗口句柄
            emoji: Emoji 图标
            text: 按钮文本
            x, y: 位置
            width, height: 尺寸
            bg_color: 背景色（ARGB）
            
        Returns:
            按钮 ID
        """
        emoji_bytes = self.str_to_utf8(emoji)
        text_bytes = self.str_to_utf8(text)
        return self.dll.create_emoji_button_bytes(
            parent, emoji_bytes, len(emoji_bytes),
            text_bytes, len(text_bytes),
            x, y, width, height, bg_color
        )
    
    def create_label(self, parent, text, x, y, width, height,
                    fg_color, bg_color, font_name="Microsoft YaHei UI",
                    font_size=14, bold=False, italic=False, underline=False,
                    alignment=0, word_wrap=False):
        """
        创建标签
        
        Args:
            parent: 父窗口句柄
            text: 标签文本（支持 Unicode 表情）
            x, y: 位置
            width, height: 尺寸
            fg_color: 前景色（ARGB）
            bg_color: 背景色（ARGB）
            font_name: 字体名称
            font_size: 字体大小
            bold: 是否粗体
            italic: 是否斜体
            underline: 是否下划线
            alignment: 对齐方式（0=左，1=中，2=右）
            word_wrap: 是否自动换行
            
        Returns:
            标签句柄
        """
        text_bytes = self.str_to_utf8(text)
        font_bytes = self.str_to_utf8(font_name)
        return self.dll.CreateLabel(
            parent, x, y, width, height,
            text_bytes, len(text_bytes),
            fg_color, bg_color,
            font_bytes, len(font_bytes),
            font_size, bold, italic, underline,
            alignment, word_wrap
        )
    
    def create_group_box(self, parent, title, x, y, width, height,
                        border_color, bg_color):
        """
        创建分组框
        
        Args:
            parent: 父窗口句柄
            title: 分组框标题（支持 Unicode 表情）
            x, y: 位置
            width, height: 尺寸
            border_color: 边框颜色（ARGB）
            bg_color: 背景色（ARGB）
            
        Returns:
            分组框句柄
        """
        title_bytes = self.str_to_utf8(title)
        return self.dll.CreateGroupBox(
            parent, x, y, width, height,
            title_bytes, len(title_bytes),
            border_color, bg_color
        )
    
    def create_checkbox(self, parent, text, x, y, width, height,
                       checked=False, fg_color=None, bg_color=None):
        """
        创建复选框
        
        Args:
            parent: 父窗口句柄
            text: 复选框文本（支持 Unicode 表情）
            x, y: 位置
            width, height: 尺寸
            checked: 是否选中
            fg_color: 前景色（ARGB）
            bg_color: 背景色（ARGB）
            
        Returns:
            复选框句柄
        """
        if fg_color is None:
            fg_color = self.argb(255, 50, 50, 50)
        if bg_color is None:
            bg_color = self.argb(0, 0, 0, 0)
        
        text_bytes = self.str_to_utf8(text)
        return self.dll.CreateCheckBox(
            parent, x, y, width, height,
            text_bytes, len(text_bytes),
            checked, fg_color, bg_color
        )
    
    def show_message_box(self, parent, title, message, icon="ℹ️"):
        """
        显示信息框
        
        Args:
            parent: 父窗口句柄
            title: 标题（支持 Unicode 表情）
            message: 消息内容（支持 Unicode 表情）
            icon: 图标（Emoji）
        """
        title_bytes = self.str_to_utf8(title)
        message_bytes = self.str_to_utf8(message)
        icon_bytes = self.str_to_utf8(icon)
        self.dll.show_message_box_bytes(
            parent,
            title_bytes, len(title_bytes),
            message_bytes, len(message_bytes),
            icon_bytes, len(icon_bytes)
        )
    
    def set_button_callback(self, callback):
        """
        设置按钮点击回调
        
        Args:
            callback: 回调函数，签名为 callback(button_id, parent_hwnd)
        """
        self._button_callback = self.ButtonClickCallback(callback)
        self.dll.set_button_click_callback(self._button_callback)
    
    def set_checkbox_callback(self, checkbox_hwnd, callback):
        """
        设置复选框回调
        
        Args:
            checkbox_hwnd: 复选框句柄
            callback: 回调函数，签名为 callback(hwnd, checked)
        """
        cb = self.CheckBoxCallback(callback)
        # 保存引用防止被垃圾回收
        if not hasattr(self, '_checkbox_callbacks'):
            self._checkbox_callbacks = {}
        self._checkbox_callbacks[checkbox_hwnd] = cb
        self.dll.SetCheckBoxCallback(checkbox_hwnd, cb)
    
    def set_label_text(self, label_hwnd, text):
        """
        设置标签文本
        
        Args:
            label_hwnd: 标签句柄
            text: 新文本（支持 Unicode 表情）
        """
        text_bytes = self.str_to_utf8(text)
        self.dll.SetLabelText(label_hwnd, text_bytes, len(text_bytes))
    
    def set_main_window(self, hwnd):
        """设置消息循环主窗口"""
        self.dll.set_message_loop_main_window(hwnd)
    
    def run_message_loop(self):
        """运行消息循环"""
        return self.dll.run_message_loop()


# 颜色常量（Element UI 标准配色）
class Colors:
    """Element UI 标准配色"""
    PRIMARY = EmojiWindow.argb(255, 64, 158, 255)    # #409EFF 主题蓝
    SUCCESS = EmojiWindow.argb(255, 103, 194, 58)    # #67C23A 成功绿
    WARNING = EmojiWindow.argb(255, 230, 162, 60)    # #E6A23C 警告橙
    DANGER = EmojiWindow.argb(255, 245, 108, 108)    # #F56C6C 危险红
    INFO = EmojiWindow.argb(255, 144, 147, 153)      # #909399 信息灰
    
    TEXT_PRIMARY = EmojiWindow.argb(255, 48, 49, 51)      # #303133 主要文本
    TEXT_REGULAR = EmojiWindow.argb(255, 96, 98, 102)     # #606266 常规文本
    TEXT_SECONDARY = EmojiWindow.argb(255, 144, 147, 153) # #909399 次要文本
    
    BORDER_BASE = EmojiWindow.argb(255, 220, 223, 230)    # #DCDFE6 基础边框
    BORDER_LIGHT = EmojiWindow.argb(255, 228, 231, 237)   # #E4E7ED 浅色边框
    
    BG_WHITE = EmojiWindow.argb(255, 255, 255, 255)       # #FFFFFF 白色背景
    BG_LIGHT = EmojiWindow.argb(255, 245, 247, 250)       # #F5F7FA 浅色背景
    TRANSPARENT = EmojiWindow.argb(0, 0, 0, 0)            # 透明
