"""
所有控件属性操作综合示例 (Python版)
对应易语言综合示例，演示 emoji_window.dll 的所有控件属性 API
包括：按钮、窗口、标签、复选框、单选按钮、分组框、TabControl、编辑框、进度条
"""
import ctypes
from ctypes import c_int, c_uint, c_void_p, c_char_p, POINTER, byref, CFUNCTYPE
import sys
import os
import random

# ===== 加载 DLL =====
dll_path = os.path.join(os.path.dirname(os.path.abspath(__file__)), 'emoji_window.dll')
try:
    dll = ctypes.CDLL(dll_path)
except OSError as e:
    print(f"错误: 无法加载 emoji_window.dll: {e}")
    sys.exit(1)

# ===== 辅助函数 =====
def utf8(text):
    """将文本转为 UTF-8 字节串"""
    return text.encode('utf-8')

def argb(a, r, g, b):
    """构造 ARGB 颜色值"""
    return (a << 24) | (r << 16) | (g << 8) | b

def rgb_color(r, g, b):
    """构造 RGB 颜色值（窗口标题栏用）"""
    return (r << 16) | (g << 8) | b

def argb_str(color):
    """将 ARGB 颜色值转为可读字符串"""
    a = (color >> 24) & 0xFF
    r = (color >> 16) & 0xFF
    g = (color >> 8) & 0xFF
    b = color & 0xFF
    return f"ARGB({a},{r},{g},{b})"

def rgb_str(color):
    """将 RGB 颜色值转为可读字符串"""
    r = (color >> 16) & 0xFF
    g = (color >> 8) & 0xFF
    b = color & 0xFF
    return f"RGB({r},{g},{b})"

def get_text_2call(func, handle, *extra_args):
    """两次调用模式获取文本: 第一次获取长度，第二次获取内容"""
    length = func(handle, *extra_args, 0, 0)
    if length <= 0:
        return b"", length
    buf = ctypes.create_string_buffer(length)
    func(handle, *extra_args, buf, length)
    return buf.raw[:length], length

def show_result(text):
    """在结果标签中显示信息"""
    global label_result
    data = utf8(text)
    dll.SetLabelText(label_result, data, len(data))

def create_btn(parent, emoji_bytes, text, x, y, w, h, color):
    """创建 Emoji 按钮的快捷函数"""
    text_bytes = utf8(text)
    return dll.create_emoji_button_bytes(
        parent, emoji_bytes, len(emoji_bytes),
        text_bytes, len(text_bytes), x, y, w, h, color)

def create_section_label(parent, text, x, y, w, h, bg_color):
    """创建区域标题标签"""
    data = utf8(text)
    font = utf8("微软雅黑")
    return dll.CreateLabel(parent, x, y, w, h,
        data, len(data),
        argb(255,255,255,255), bg_color,
        font, len(font), 11, 1, 0, 0, 1, 0)

# ===== 回调函数类型 =====
CB_BUTTON = CFUNCTYPE(None, c_int, c_int)

# ===== DLL 函数原型 =====
# -- 窗口 --
dll.create_window_bytes_ex.restype = c_void_p
dll.create_window_bytes_ex.argtypes = [c_char_p, c_int, c_int, c_int, c_int]
dll.GetWindowTitle.restype = c_int
dll.GetWindowTitle.argtypes = [c_void_p, c_void_p, c_int]
dll.GetWindowBounds.restype = None
dll.GetWindowBounds.argtypes = [c_void_p, POINTER(c_int), POINTER(c_int), POINTER(c_int), POINTER(c_int)]
dll.GetWindowTitlebarColor.restype = c_int
dll.GetWindowTitlebarColor.argtypes = [c_void_p]
dll.destroy_window.restype = None
dll.destroy_window.argtypes = [c_void_p]

# -- 按钮 --
dll.create_emoji_button_bytes.restype = c_int
dll.create_emoji_button_bytes.argtypes = [c_void_p, c_char_p, c_int, c_char_p, c_int, c_int, c_int, c_int, c_int, c_uint]
dll.set_button_click_callback.restype = None
dll.set_button_click_callback.argtypes = [CB_BUTTON]
dll.GetButtonText.restype = c_int
dll.GetButtonText.argtypes = [c_int, c_void_p, c_int]
dll.SetButtonText.restype = None
dll.SetButtonText.argtypes = [c_int, c_char_p, c_int]
dll.GetButtonBounds.restype = None
dll.GetButtonBounds.argtypes = [c_int, POINTER(c_int), POINTER(c_int), POINTER(c_int), POINTER(c_int)]
dll.SetButtonBackgroundColor.restype = None
dll.SetButtonBackgroundColor.argtypes = [c_int, c_uint]
dll.GetButtonVisible.restype = c_int
dll.GetButtonVisible.argtypes = [c_int]
dll.ShowButton.restype = None
dll.ShowButton.argtypes = [c_int, c_int]
dll.GetButtonEnabled.restype = c_int
dll.GetButtonEnabled.argtypes = [c_int]
dll.SetButtonEmoji.restype = None
dll.SetButtonEmoji.argtypes = [c_int, c_char_p, c_int]

# -- 标签 --
dll.CreateLabel.restype = c_int
dll.CreateLabel.argtypes = [c_void_p, c_int, c_int, c_int, c_int, c_char_p, c_int, c_uint, c_uint, c_char_p, c_int, c_int, c_int, c_int, c_int, c_int, c_int]
dll.SetLabelText.restype = None
dll.SetLabelText.argtypes = [c_int, c_char_p, c_int]
dll.GetLabelText.restype = c_int
dll.GetLabelText.argtypes = [c_int, c_void_p, c_int]
dll.GetLabelFont.restype = c_int
dll.GetLabelFont.argtypes = [c_int, c_void_p, c_int, POINTER(c_int), POINTER(c_int), POINTER(c_int), POINTER(c_int)]
dll.GetLabelColor.restype = c_int
dll.GetLabelColor.argtypes = [c_int, POINTER(c_uint), POINTER(c_uint)]
dll.GetLabelBounds.restype = c_int
dll.GetLabelBounds.argtypes = [c_int, POINTER(c_int), POINTER(c_int), POINTER(c_int), POINTER(c_int)]
dll.GetLabelAlignment.restype = c_int
dll.GetLabelAlignment.argtypes = [c_int]

# -- 复选框 --
dll.CreateCheckBox.restype = c_int
dll.CreateCheckBox.argtypes = [c_void_p, c_int, c_int, c_int, c_int, c_char_p, c_int, c_int, c_uint, c_uint, c_char_p, c_int, c_int, c_int, c_int, c_int]
dll.GetCheckBoxText.restype = c_int
dll.GetCheckBoxText.argtypes = [c_int, c_void_p, c_int]

# -- 单选按钮 --
dll.CreateRadioButton.restype = c_int
dll.CreateRadioButton.argtypes = [c_void_p, c_int, c_int, c_int, c_int, c_char_p, c_int, c_int, c_int, c_uint, c_uint, c_char_p, c_int, c_int, c_int, c_int, c_int]
dll.GetRadioButtonText.restype = c_int
dll.GetRadioButtonText.argtypes = [c_int, c_void_p, c_int]

# -- 编辑框 --
dll.CreateEditBox.restype = c_int
dll.CreateEditBox.argtypes = [c_void_p, c_int, c_int, c_int, c_int, c_char_p, c_int, c_uint, c_uint, c_char_p, c_int, c_int, c_int, c_int, c_int, c_int, c_int, c_int, c_int, c_int, c_int]
dll.GetEditBoxFont.restype = c_int
dll.GetEditBoxFont.argtypes = [c_int, c_void_p, c_int, POINTER(c_int), POINTER(c_int), POINTER(c_int), POINTER(c_int)]
dll.GetEditBoxColor.restype = c_int
dll.GetEditBoxColor.argtypes = [c_int, POINTER(c_uint), POINTER(c_uint)]
dll.GetEditBoxBounds.restype = c_int
dll.GetEditBoxBounds.argtypes = [c_int, POINTER(c_int), POINTER(c_int), POINTER(c_int), POINTER(c_int)]

# -- 分组框 --
dll.CreateGroupBox.restype = c_void_p
dll.CreateGroupBox.argtypes = [c_void_p, c_int, c_int, c_int, c_int, c_char_p, c_int, c_uint, c_uint, c_char_p, c_int, c_int, c_int, c_int, c_int]
dll.AddChildToGroup.restype = None
dll.AddChildToGroup.argtypes = [c_void_p, c_void_p]
dll.GetGroupBoxTitle.restype = c_int
dll.GetGroupBoxTitle.argtypes = [c_void_p, c_void_p, c_int]
dll.GetGroupBoxBounds.restype = c_int
dll.GetGroupBoxBounds.argtypes = [c_void_p, POINTER(c_int), POINTER(c_int), POINTER(c_int), POINTER(c_int)]

# -- TabControl --
dll.CreateTabControl.restype = c_void_p
dll.CreateTabControl.argtypes = [c_void_p, c_int, c_int, c_int, c_int]
dll.AddTabItem.restype = c_int
dll.AddTabItem.argtypes = [c_void_p, c_char_p, c_int, c_int]
dll.GetTabCount.restype = c_int
dll.GetTabCount.argtypes = [c_void_p]
dll.GetTabTitle.restype = c_int
dll.GetTabTitle.argtypes = [c_void_p, c_int, c_void_p, c_int]
dll.GetTabControlBounds.restype = c_int
dll.GetTabControlBounds.argtypes = [c_void_p, POINTER(c_int), POINTER(c_int), POINTER(c_int), POINTER(c_int)]

# -- 进度条 --
dll.CreateProgressBar.restype = c_void_p
dll.CreateProgressBar.argtypes = [c_void_p, c_int, c_int, c_int, c_int, c_int, c_uint, c_uint, c_int, c_uint]
dll.GetProgressBarColor.restype = c_int
dll.GetProgressBarColor.argtypes = [c_void_p, POINTER(c_uint), POINTER(c_uint)]
dll.GetProgressBarBounds.restype = c_int
dll.GetProgressBarBounds.argtypes = [c_void_p, POINTER(c_int), POINTER(c_int), POINTER(c_int), POINTER(c_int)]
dll.GetProgressBarShowText.restype = c_int
dll.GetProgressBarShowText.argtypes = [c_void_p]

# -- 消息循环 --
dll.set_message_loop_main_window.restype = None
dll.set_message_loop_main_window.argtypes = [c_void_p]
dll.run_message_loop.restype = c_int
dll.run_message_loop.argtypes = []

# ===== 全局变量 =====
hwnd = None
label_result = 0

# 演示控件
demo_btn1 = 0
demo_btn2 = 0
demo_btn3 = 0
demo_label = 0
demo_checkbox = 0
demo_radio = 0
demo_editbox = 0
demo_groupbox = None
demo_tabctrl = None
demo_progressbar = None

# 操作按钮 ID —— 按钮属性区
btn_get_btn_text = 0
btn_set_btn_text = 0
btn_get_btn_pos = 0
btn_set_btn_color = 0
btn_toggle_btn_vis = 0
btn_get_btn_enabled = 0

# 操作按钮 ID —— 窗口属性区
btn_get_win_title = 0
btn_get_win_pos = 0
btn_get_win_color = 0

# 操作按钮 ID —— 标签属性区
btn_get_lbl_text = 0
btn_get_lbl_font = 0
btn_get_lbl_color = 0
btn_get_lbl_pos = 0
btn_get_lbl_align = 0

# 操作按钮 ID —— 复选框/单选按钮属性区
btn_get_chk_text = 0
btn_get_rdo_text = 0

# 操作按钮 ID —— 分组框属性区
btn_get_grp_title = 0
btn_get_grp_pos = 0

# 操作按钮 ID —— TabControl 属性区
btn_get_tab_title = 0
btn_get_tab_pos = 0

# 操作按钮 ID —— 编辑框属性区
btn_get_edit_font = 0
btn_get_edit_color = 0
btn_get_edit_pos = 0

# 操作按钮 ID —— 进度条属性区
btn_get_prog_color = 0
btn_get_prog_pos = 0
btn_get_prog_showtext = 0

# 状态标志
btn3_visible = True

# ===== 按钮点击回调 =====
def on_button_click(button_id, parent_hwnd):
    """按钮点击回调函数"""
    global btn3_visible

    # ===== 按钮属性操作 =====
    if button_id == btn_get_btn_text:
        # 获取按钮1文本（两次调用）
        text_bytes, length = get_text_2call(dll.GetButtonText, demo_btn1)
        text = text_bytes.decode('utf-8', errors='replace')
        show_result(f"按钮1的文本: {text}\n字节长度: {length}")

    elif button_id == btn_set_btn_text:
        # 修改按钮1文本
        new_text = f"新文本{random.randint(1,100)}"
        data = utf8(new_text)
        dll.SetButtonText(demo_btn1, data, len(data))
        show_result(f"已将按钮1文本修改为: {new_text}")

    elif button_id == btn_get_btn_pos:
        # 获取按钮1位置
        x, y, w, h = c_int(), c_int(), c_int(), c_int()
        dll.GetButtonBounds(demo_btn1, byref(x), byref(y), byref(w), byref(h))
        show_result(f"按钮1的位置:\nX={x.value}, Y={y.value}\n宽度={w.value}, 高度={h.value}")

    elif button_id == btn_set_btn_color:
        # 修改按钮2背景色（随机颜色）
        color = argb(255, random.randint(50,255), random.randint(50,255), random.randint(50,255))
        dll.SetButtonBackgroundColor(demo_btn2, color)
        show_result(f"已将按钮2的背景色修改为: {argb_str(color)}")

    elif button_id == btn_toggle_btn_vis:
        # 切换按钮3可视
        btn3_visible = not btn3_visible
        dll.ShowButton(demo_btn3, 1 if btn3_visible else 0)
        state = "显示" if btn3_visible else "隐藏"
        show_result(f"已{state}按钮3")

    elif button_id == btn_get_btn_enabled:
        # 获取按钮3启用状态
        enabled = dll.GetButtonEnabled(demo_btn3)
        state = "启用" if enabled else "禁用"
        show_result(f"按钮3的启用状态: {state} ({enabled})")

    # ===== 窗口属性操作 =====
    elif button_id == btn_get_win_title:
        # 获取窗口标题（两次调用）
        text_bytes, length = get_text_2call(dll.GetWindowTitle, hwnd)
        text = text_bytes.decode('utf-8', errors='replace')
        show_result(f"窗口标题: {text}\n字节长度: {length}")

    elif button_id == btn_get_win_pos:
        # 获取窗口位置
        x, y, w, h = c_int(), c_int(), c_int(), c_int()
        dll.GetWindowBounds(hwnd, byref(x), byref(y), byref(w), byref(h))
        show_result(f"窗口位置:\nX={x.value}, Y={y.value}\n宽度={w.value}, 高度={h.value}")

    elif button_id == btn_get_win_color:
        # 获取窗口标题栏颜色
        color = dll.GetWindowTitlebarColor(hwnd)
        show_result(f"窗口标题栏颜色: {rgb_str(color)}\n十进制值: {color}")

    # ===== 标签属性操作 =====
    elif button_id == btn_get_lbl_text:
        # 获取标签文本（两次调用）
        text_bytes, length = get_text_2call(dll.GetLabelText, demo_label)
        text = text_bytes.decode('utf-8', errors='replace')
        show_result(f"标签文本: {text}\n字节长度: {length}")

    elif button_id == btn_get_lbl_font:
        # 获取标签字体（两次调用）
        font_size = c_int()
        bold = c_int()
        italic = c_int()
        underline = c_int()
        name_len = dll.GetLabelFont(demo_label, 0, 0,
            byref(font_size), byref(bold), byref(italic), byref(underline))
        if name_len > 0:
            buf = ctypes.create_string_buffer(name_len)
            dll.GetLabelFont(demo_label, buf, name_len,
                byref(font_size), byref(bold), byref(italic), byref(underline))
            font_name = buf.raw[:name_len].decode('utf-8', errors='replace')
        else:
            font_name = "(未知)"
        show_result(f"标签字体:\n字体名: {font_name}\n大小: {font_size.value}\n"
                    f"粗体: {bool(bold.value)}, 斜体: {bool(italic.value)}, 下划线: {bool(underline.value)}")

    elif button_id == btn_get_lbl_color:
        # 获取标签颜色
        fg = c_uint()
        bg = c_uint()
        ret = dll.GetLabelColor(demo_label, byref(fg), byref(bg))
        show_result(f"标签颜色:\n前景色: {argb_str(fg.value)}\n背景色: {argb_str(bg.value)}")

    elif button_id == btn_get_lbl_pos:
        # 获取标签位置
        x, y, w, h = c_int(), c_int(), c_int(), c_int()
        dll.GetLabelBounds(demo_label, byref(x), byref(y), byref(w), byref(h))
        show_result(f"标签位置:\nX={x.value}, Y={y.value}\n宽度={w.value}, 高度={h.value}")

    elif button_id == btn_get_lbl_align:
        # 获取标签对齐方式
        align = dll.GetLabelAlignment(demo_label)
        align_names = {0: "左对齐", 1: "居中", 2: "右对齐"}
        show_result(f"标签对齐方式: {align_names.get(align, '未知')} ({align})")

    # ===== 复选框/单选按钮属性操作 =====
    elif button_id == btn_get_chk_text:
        # 获取复选框文本（两次调用）
        text_bytes, length = get_text_2call(dll.GetCheckBoxText, demo_checkbox)
        text = text_bytes.decode('utf-8', errors='replace')
        show_result(f"复选框文本: {text}\n字节长度: {length}")

    elif button_id == btn_get_rdo_text:
        # 获取单选按钮文本（两次调用）
        text_bytes, length = get_text_2call(dll.GetRadioButtonText, demo_radio)
        text = text_bytes.decode('utf-8', errors='replace')
        show_result(f"单选按钮文本: {text}\n字节长度: {length}")

    # ===== 分组框属性操作 =====
    elif button_id == btn_get_grp_title:
        # 获取分组框标题（两次调用）
        text_bytes, length = get_text_2call(dll.GetGroupBoxTitle, demo_groupbox)
        text = text_bytes.decode('utf-8', errors='replace')
        show_result(f"分组框标题: {text}\n字节长度: {length}")

    elif button_id == btn_get_grp_pos:
        # 获取分组框位置
        x, y, w, h = c_int(), c_int(), c_int(), c_int()
        dll.GetGroupBoxBounds(demo_groupbox, byref(x), byref(y), byref(w), byref(h))
        show_result(f"分组框位置:\nX={x.value}, Y={y.value}\n宽度={w.value}, 高度={h.value}")

    # ===== TabControl 属性操作 =====
    elif button_id == btn_get_tab_title:
        # 获取所有 Tab 页标题
        count = dll.GetTabCount(demo_tabctrl)
        info = f"Tab页数量: {count}\n"
        for i in range(count):
            # 两次调用：GetTabTitle(handle, index, buf, bufsize)
            title_len = dll.GetTabTitle(demo_tabctrl, i, 0, 0)
            if title_len > 0:
                buf = ctypes.create_string_buffer(title_len)
                dll.GetTabTitle(demo_tabctrl, i, buf, title_len)
                title = buf.raw[:title_len].decode('utf-8', errors='replace')
            else:
                title = "(空)"
            info += f"Tab[{i}]: {title}\n"
        show_result(info.strip())

    elif button_id == btn_get_tab_pos:
        # 获取 TabControl 位置
        x, y, w, h = c_int(), c_int(), c_int(), c_int()
        dll.GetTabControlBounds(demo_tabctrl, byref(x), byref(y), byref(w), byref(h))
        show_result(f"TabControl位置:\nX={x.value}, Y={y.value}\n宽度={w.value}, 高度={h.value}")

    # ===== 编辑框属性操作 =====
    elif button_id == btn_get_edit_font:
        # 获取编辑框字体（两次调用）
        font_size = c_int()
        bold = c_int()
        italic = c_int()
        underline = c_int()
        name_len = dll.GetEditBoxFont(demo_editbox, 0, 0,
            byref(font_size), byref(bold), byref(italic), byref(underline))
        if name_len > 0:
            buf = ctypes.create_string_buffer(name_len)
            dll.GetEditBoxFont(demo_editbox, buf, name_len,
                byref(font_size), byref(bold), byref(italic), byref(underline))
            font_name = buf.raw[:name_len].decode('utf-8', errors='replace')
        else:
            font_name = "(未知)"
        show_result(f"编辑框字体:\n字体名: {font_name}\n大小: {font_size.value}\n"
                    f"粗体: {bool(bold.value)}, 斜体: {bool(italic.value)}, 下划线: {bool(underline.value)}")

    elif button_id == btn_get_edit_color:
        # 获取编辑框颜色
        fg = c_uint()
        bg = c_uint()
        dll.GetEditBoxColor(demo_editbox, byref(fg), byref(bg))
        show_result(f"编辑框颜色:\n前景色: {argb_str(fg.value)}\n背景色: {argb_str(bg.value)}")

    elif button_id == btn_get_edit_pos:
        # 获取编辑框位置
        x, y, w, h = c_int(), c_int(), c_int(), c_int()
        dll.GetEditBoxBounds(demo_editbox, byref(x), byref(y), byref(w), byref(h))
        show_result(f"编辑框位置:\nX={x.value}, Y={y.value}\n宽度={w.value}, 高度={h.value}")

    # ===== 进度条属性操作 =====
    elif button_id == btn_get_prog_color:
        # 获取进度条颜色
        fg = c_uint()
        bg = c_uint()
        dll.GetProgressBarColor(demo_progressbar, byref(fg), byref(bg))
        show_result(f"进度条颜色:\n前景色: {argb_str(fg.value)}\n背景色: {argb_str(bg.value)}")

    elif button_id == btn_get_prog_pos:
        # 获取进度条位置
        x, y, w, h = c_int(), c_int(), c_int(), c_int()
        dll.GetProgressBarBounds(demo_progressbar, byref(x), byref(y), byref(w), byref(h))
        show_result(f"进度条位置:\nX={x.value}, Y={y.value}\n宽度={w.value}, 高度={h.value}")

    elif button_id == btn_get_prog_showtext:
        # 获取进度条显示文本状态
        show_text = dll.GetProgressBarShowText(demo_progressbar)
        state = "显示" if show_text == 1 else "不显示"
        show_result(f"进度条显示文本状态: {state} ({show_text})")

# 保持回调引用防止被 GC
_cb_ref = None

# ===== 主函数 =====
def comprehensive_demo():
    """所有控件属性操作综合示例"""
    global hwnd, label_result, _cb_ref
    global demo_btn1, demo_btn2, demo_btn3, demo_label
    global demo_checkbox, demo_radio, demo_editbox
    global demo_groupbox, demo_tabctrl, demo_progressbar
    global btn_get_btn_text, btn_set_btn_text, btn_get_btn_pos
    global btn_set_btn_color, btn_toggle_btn_vis, btn_get_btn_enabled
    global btn_get_win_title, btn_get_win_pos, btn_get_win_color
    global btn_get_lbl_text, btn_get_lbl_font, btn_get_lbl_color
    global btn_get_lbl_pos, btn_get_lbl_align
    global btn_get_chk_text, btn_get_rdo_text
    global btn_get_grp_title, btn_get_grp_pos
    global btn_get_tab_title, btn_get_tab_pos
    global btn_get_edit_font, btn_get_edit_color, btn_get_edit_pos
    global btn_get_prog_color, btn_get_prog_pos, btn_get_prog_showtext
    global btn3_visible

    # ===== 创建主窗口 =====
    title = utf8("所有控件属性操作综合示例")
    hwnd = dll.create_window_bytes_ex(title, len(title), 1100, 820, rgb_color(50, 100, 180))
    if not hwnd:
        print("错误: 创建窗口失败")
        return

    font_msyh = utf8("微软雅黑")

    # ===== 创建结果显示标签 =====
    hint = utf8("点击下方各区域按钮，查看对应控件的属性信息")
    label_result = dll.CreateLabel(hwnd, 10, 10, 1080, 100,
        hint, len(hint),
        argb(255,0,0,0), argb(255,240,248,255),
        font_msyh, len(font_msyh), 12, 0, 0, 0, 0, 1)

    # ===== 创建演示控件 =====

    # --- 演示按钮1: 📝 获取文本演示 ---
    demo_btn1 = create_btn(hwnd, b'\xf0\x9f\x93\x9d', "获取文本演示",
        10, 120, 160, 36, argb(255,100,149,237))

    # --- 演示按钮2: 🎨 颜色演示 ---
    demo_btn2 = create_btn(hwnd, b'\xf0\x9f\x8e\xa8', "颜色演示",
        185, 120, 160, 36, argb(255,255,140,0))

    # --- 演示按钮3: 👁️ 可视演示 ---
    demo_btn3 = create_btn(hwnd, b'\xf0\x9f\x91\x81\xef\xb8\x8f', "可视演示",
        360, 120, 160, 36, argb(255,60,179,113))
    btn3_visible = True

    # --- 演示标签 ---
    lbl_text = utf8("这是演示标签 Hello 你好")
    demo_label = dll.CreateLabel(hwnd, 10, 165, 520, 36,
        lbl_text, len(lbl_text),
        argb(255,255,255,255), argb(255,70,130,180),
        font_msyh, len(font_msyh), 14, 1, 0, 0, 1, 0)

    # --- 演示复选框 ---
    chk_text = utf8("演示复选框")
    demo_checkbox = dll.CreateCheckBox(hwnd, 10, 210, 160, 30,
        chk_text, len(chk_text), 1,
        argb(255,0,0,0), argb(255,255,255,255),
        font_msyh, len(font_msyh), 12, 0, 0, 0)

    # --- 演示单选按钮 ---
    rdo_text = utf8("演示单选按钮")
    demo_radio = dll.CreateRadioButton(hwnd, 185, 210, 160, 30,
        rdo_text, len(rdo_text), 1, 1,
        argb(255,0,0,0), argb(255,255,255,255),
        font_msyh, len(font_msyh), 12, 0, 0, 0)

    # --- 演示编辑框 ---
    edit_text = utf8("这是演示编辑框内容")
    demo_editbox = dll.CreateEditBox(hwnd, 10, 250, 520, 35,
        edit_text, len(edit_text),
        argb(255,0,0,0), argb(255,255,255,255),
        font_msyh, len(font_msyh), 13, 0, 0, 0,
        0, 0, 0, 0, 1, 1)

    # --- 演示分组框 ---
    grp_title = utf8("演示分组框")
    demo_groupbox = dll.CreateGroupBox(hwnd, 10, 620, 520, 100,
        grp_title, len(grp_title),
        argb(255,0,0,0), argb(255,245,245,245),
        font_msyh, len(font_msyh), 12, 0, 0, 0)

    # 分组框内子控件
    # 子标签 "名："
    sub_text = utf8("名：")
    sub_label = dll.CreateLabel(hwnd, 20, 648, 30, 20,
        sub_text, len(sub_text),
        argb(255,0,0,0), argb(255,245,245,245),
        font_msyh, len(font_msyh), 11, 0, 0, 0, 0, 0)
    dll.AddChildToGroup(demo_groupbox, sub_label)

    # 子编辑框 "Kiro"
    sub_edit_text = utf8("Kiro")
    sub_edit = dll.CreateEditBox(hwnd, 55, 646, 100, 24,
        sub_edit_text, len(sub_edit_text),
        argb(255,0,0,0), argb(255,255,255,255),
        font_msyh, len(font_msyh), 11, 0, 0, 0,
        0, 0, 0, 0, 1, 1)
    dll.AddChildToGroup(demo_groupbox, sub_edit)

    # 子按钮 ✅ OK
    sub_btn = create_btn(hwnd, b'\xe2\x9c\x85', "OK",
        165, 646, 65, 24, argb(255,100,149,237))
    dll.AddChildToGroup(demo_groupbox, sub_btn)

    # 子复选框 "记住"
    sub_chk_text = utf8("记住")
    sub_chk = dll.CreateCheckBox(hwnd, 245, 648, 80, 22,
        sub_chk_text, len(sub_chk_text), 0,
        argb(255,0,0,0), argb(255,245,245,245),
        font_msyh, len(font_msyh), 11, 0, 0, 0)
    dll.AddChildToGroup(demo_groupbox, sub_chk)

    # 子单选按钮 "选A"
    sub_rdo_text = utf8("选A")
    sub_rdo = dll.CreateRadioButton(hwnd, 335, 648, 80, 22,
        sub_rdo_text, len(sub_rdo_text), 2, 1,
        argb(255,0,0,0), argb(255,245,245,245),
        font_msyh, len(font_msyh), 11, 0, 0, 0)
    dll.AddChildToGroup(demo_groupbox, sub_rdo)

    # --- 演示 TabControl ---
    demo_tabctrl = dll.CreateTabControl(hwnd, 540, 120, 550, 130)
    tab1 = utf8("Tab页一")
    dll.AddTabItem(demo_tabctrl, tab1, len(tab1), 0)
    tab2 = utf8("Tab页二")
    dll.AddTabItem(demo_tabctrl, tab2, len(tab2), 0)

    # --- 演示进度条 ---
    demo_progressbar = dll.CreateProgressBar(hwnd, 540, 265, 550, 28,
        60, argb(255,70,130,180), argb(255,220,220,220), 1, argb(255,255,255,255))

    # ===== 创建操作按钮 =====
    GREEN = argb(255,100,180,100)
    RED = argb(255,180,100,100)

    # --- 按钮属性区 ---
    create_section_label(hwnd, "【按钮属性操作】", 10, 295, 530, 24, argb(255,70,100,180))

    btn_get_btn_text = create_btn(hwnd, b'\xf0\x9f\x93\x9d', "获取按钮文本", 10, 335, 130, 36, GREEN)
    btn_set_btn_text = create_btn(hwnd, b'\xe2\x9c\x8f', "修改按钮文本", 150, 335, 130, 36, GREEN)
    btn_get_btn_pos = create_btn(hwnd, b'\xf0\x9f\x93\x90', "获取按钮位置", 290, 335, 130, 36, GREEN)
    btn_set_btn_color = create_btn(hwnd, b'\xf0\x9f\x8e\xa8', "修改按钮颜色", 10, 380, 130, 36, RED)
    btn_toggle_btn_vis = create_btn(hwnd, b'\xf0\x9f\x91\x81\xef\xb8\x8f', "切换按钮可视", 150, 380, 130, 36, RED)
    btn_get_btn_enabled = create_btn(hwnd, b'\xe2\x9c\x85', "获取按钮启用", 290, 380, 130, 36, RED)

    # --- 窗口属性区 ---
    create_section_label(hwnd, "【窗口属性操作】", 10, 425, 530, 24, argb(255,100,70,180))

    btn_get_win_title = create_btn(hwnd, b'\xf0\x9f\x8f\xa0', "获取窗口标题", 10, 455, 130, 36, GREEN)
    btn_get_win_pos = create_btn(hwnd, b'\xf0\x9f\x93\x90', "获取窗口位置", 150, 455, 130, 36, GREEN)
    btn_get_win_color = create_btn(hwnd, b'\xf0\x9f\x8e\xa8', "获取标题栏颜色", 290, 455, 130, 36, GREEN)

    # --- 标签属性区 ---
    create_section_label(hwnd, "【标签属性操作】", 10, 500, 530, 24, argb(255,70,150,100))

    btn_get_lbl_text = create_btn(hwnd, b'\xf0\x9f\x93\x9d', "获取标签文本", 10, 530, 130, 36, GREEN)
    btn_get_lbl_font = create_btn(hwnd, b'\xf0\x9f\x94\xa4', "获取标签字体", 150, 530, 130, 36, GREEN)
    btn_get_lbl_color = create_btn(hwnd, b'\xf0\x9f\x8e\xa8', "获取标签颜色", 290, 530, 130, 36, GREEN)
    btn_get_lbl_pos = create_btn(hwnd, b'\xf0\x9f\x93\x90', "获取标签位置", 10, 575, 130, 36, RED)
    btn_get_lbl_align = create_btn(hwnd, b'\xe2\x86\x94', "获取标签对齐", 150, 575, 130, 36, RED)

    # --- 复选框/单选按钮属性区 ---
    create_section_label(hwnd, "【复选框/单选按钮属性】", 540, 300, 550, 24, argb(255,150,80,150))

    btn_get_chk_text = create_btn(hwnd, b'\xf0\x9f\x93\x9d', "获取复选框文本", 540, 330, 140, 36, GREEN)
    btn_get_rdo_text = create_btn(hwnd, b'\xf0\x9f\x93\x9d', "获取单选按钮文本", 695, 330, 140, 36, GREEN)

    # --- 分组框属性区 ---
    create_section_label(hwnd, "【分组框属性操作】", 540, 375, 550, 24, argb(255,180,100,50))

    btn_get_grp_title = create_btn(hwnd, b'\xf0\x9f\x93\x9d', "获取分组框标题", 540, 405, 140, 36, GREEN)
    btn_get_grp_pos = create_btn(hwnd, b'\xf0\x9f\x93\x90', "获取分组框位置", 695, 405, 140, 36, GREEN)

    # --- TabControl 属性区 ---
    create_section_label(hwnd, "【TabControl属性操作】", 540, 450, 550, 24, argb(255,50,130,150))

    btn_get_tab_title = create_btn(hwnd, b'\xf0\x9f\x93\x8b', "获取Tab标题", 540, 480, 140, 36, GREEN)
    btn_get_tab_pos = create_btn(hwnd, b'\xf0\x9f\x93\x90', "获取Tab位置", 695, 480, 140, 36, GREEN)

    # --- 编辑框属性区 ---
    create_section_label(hwnd, "【编辑框属性操作】", 540, 525, 550, 24, argb(255,100,100,180))

    btn_get_edit_font = create_btn(hwnd, b'\xf0\x9f\x94\xa4', "获取编辑框字体", 540, 555, 140, 36, GREEN)
    btn_get_edit_color = create_btn(hwnd, b'\xf0\x9f\x8e\xa8', "获取编辑框颜色", 695, 555, 140, 36, GREEN)
    btn_get_edit_pos = create_btn(hwnd, b'\xf0\x9f\x93\x90', "获取编辑框位置", 540, 600, 140, 36, RED)

    # --- 进度条属性区 ---
    create_section_label(hwnd, "【进度条属性操作】", 540, 645, 550, 24, argb(255,150,130,50))

    btn_get_prog_color = create_btn(hwnd, b'\xf0\x9f\x8e\xa8', "获取进度条颜色", 540, 675, 140, 36, GREEN)
    btn_get_prog_pos = create_btn(hwnd, b'\xf0\x9f\x93\x90', "获取进度条位置", 695, 675, 140, 36, GREEN)
    btn_get_prog_showtext = create_btn(hwnd, b'\xf0\x9f\x93\x8a', "获取显示文本状态", 540, 720, 140, 36, GREEN)

    # ===== 设置回调和消息循环 =====
    _cb_ref = CB_BUTTON(on_button_click)
    dll.set_button_click_callback(_cb_ref)
    dll.set_message_loop_main_window(hwnd)
    print("综合示例窗口已创建，进入消息循环...")
    dll.run_message_loop()


if __name__ == "__main__":
    comprehensive_demo()
