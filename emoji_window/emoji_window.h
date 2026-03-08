#pragma once

#include <windows.h>
#include <commctrl.h>
#include <d2d1.h>
#include <dwrite.h>
#include <uxtheme.h>
#include <wincodec.h>  // WIC (Windows Imaging Component)
#include <richedit.h>  // RichEdit控件（支持彩色emoji）
#include <string>
#include <vector>
#include <map>

#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "dwrite.lib")
#pragma comment(lib, "comctl32.lib")
#pragma comment(lib, "uxtheme.lib")
#pragma comment(lib, "windowscodecs.lib")  // WIC库

// Button click callback type (stdcall)
typedef void (__stdcall *ButtonClickCallback)(int button_id);

// Message box callback type (confirmed: 1=OK, 0=Cancel)
typedef void (__stdcall *MessageBoxCallback)(int confirmed);

// Tab 切换回调函数类型 (stdcall 调用约定)
typedef void (__stdcall *TAB_CALLBACK)(HWND hTabControl, int selectedIndex);

// 窗口大小改变回调函数类型 (stdcall 调用约定)
typedef void (__stdcall *WindowResizeCallback)(HWND hwnd, int width, int height);

// 自绘窗口关闭回调函数类型 (stdcall 调用约定)
// hwnd: 被关闭的窗口句柄（此时 HWND 已失效，仅用于识别是哪个窗口）
typedef void (__stdcall *WindowCloseCallback)(HWND hwnd);

// 编辑框按键回调 (stdcall)：hEdit 句柄, key_code 虚拟键码, key_down 1=按下 0=松开, shift/ctrl/alt 修饰键是否按下(0/1)
typedef void (__stdcall *EditBoxKeyCallback)(HWND hEdit, int key_code, int key_down, int shift, int ctrl, int alt);

// 复选框回调函数类型 (stdcall 调用约定)
typedef void (__stdcall *CheckBoxCallback)(HWND hCheckBox, BOOL checked);

// 进度条回调函数类型 (stdcall 调用约定)
typedef void (__stdcall *ProgressBarCallback)(HWND hProgressBar, int value);

// 图片缩放模式
enum ImageScaleMode {
    SCALE_NONE = 0,         // 不缩放
    SCALE_STRETCH = 1,      // 拉伸填充
    SCALE_FIT = 2,          // 等比缩放适应
    SCALE_CENTER = 3        // 居中显示
};

// 图片框回调函数类型 (stdcall 调用约定)
typedef void (__stdcall *PictureBoxCallback)(HWND hPictureBox);

// 单选按钮回调函数类型 (stdcall 调用约定)
typedef void (__stdcall *RadioButtonCallback)(HWND hRadioButton, int group_id, BOOL checked);

// 列表框回调函数类型 (stdcall 调用约定)
typedef void (__stdcall *ListBoxCallback)(HWND hListBox, int index);

// 组合框回调函数类型 (stdcall 调用约定)
typedef void (__stdcall *ComboBoxCallback)(HWND hComboBox, int index);

// 热键控件回调函数类型 (stdcall 调用约定)
typedef void (__stdcall *HotKeyCallback)(HWND hHotKey, int vk_code, int modifiers);

// 文本对齐方式
enum TextAlignment {
    ALIGN_LEFT = 0,
    ALIGN_CENTER = 1,
    ALIGN_RIGHT = 2
};

// 字体样式结构
struct FontStyle {
    std::wstring font_name;     // 字体名称
    int font_size;              // 字体大小
    bool bold;                  // 粗体
    bool italic;                // 斜体
    bool underline;             // 下划线
};

// 编辑框状态
struct EditBoxState {
    HWND hwnd;                  // 编辑框句柄
    HWND parent;                // 父窗口句柄
    int id;                     // 控件ID
    UINT32 fg_color;            // 前景色 (ARGB)
    UINT32 bg_color;            // 背景色 (ARGB)
    FontStyle font;             // 字体样式
    TextAlignment alignment;    // 文字对齐
    bool multiline;             // 多行模式
    bool readonly;              // 只读模式
    bool password;              // 密码框
    bool has_border;            // 是否有边框
    bool vertical_center;       // 文本垂直居中（仅单行有效）
    HBRUSH bg_brush;            // 背景画刷（避免每次创建）
    EditBoxKeyCallback key_callback;  // 按键按下/松开回调，可为 NULL
};

// D2D自定义绘制编辑框状态（支持彩色emoji）
struct D2DEditBoxState {
    HWND hwnd;                  // 控件句柄
    HWND parent;                // 父窗口句柄
    int id;                     // 控件ID
    int x, y, width, height;    // 位置和尺寸
    std::wstring text;          // 文本内容
    int cursor_pos;             // 光标位置（字符索引）
    int selection_start;        // 选择起始位置（-1表示无选择）
    int selection_end;          // 选择结束位置
    bool has_focus;             // 是否有焦点
    bool cursor_visible;        // 光标是否可见（闪烁）
    UINT_PTR cursor_timer;      // 光标闪烁定时器
    int scroll_offset_x;        // 水平滚动偏移（单行）
    int scroll_offset_y;        // 垂直滚动偏移（多行）
    bool is_composing;          // 是否正在输入法组合
    std::wstring composition_text; // 输入法组合文本
    UINT32 fg_color;            // 前景色
    UINT32 bg_color;            // 背景色
    UINT32 selection_color;     // 选择背景色
    UINT32 border_color;        // 边框颜色
    FontStyle font;             // 字体样式
    TextAlignment alignment;    // 文字对齐
    bool multiline;             // 多行模式
    bool readonly;              // 只读模式
    bool password;              // 密码框
    bool has_border;            // 是否有边框
    bool vertical_center;       // 文本垂直居中（仅单行有效）
    bool enabled;               // 启用状态
    ID2D1HwndRenderTarget* render_target; // D2D渲染目标
    IDWriteFactory* dwrite_factory;       // DirectWrite工厂
    EditBoxKeyCallback key_callback;      // 按键回调
};

// 标签状态
struct LabelState {
    HWND hwnd;                  // 标签句柄
    HWND parent;                // 父窗口句柄
    int id;                     // 控件ID
    std::wstring text;          // 文本内容
    UINT32 fg_color;            // 前景色 (ARGB)
    UINT32 bg_color;            // 背景色 (ARGB)
    FontStyle font;             // 字体样式
    TextAlignment alignment;    // 文字对齐
    HBRUSH bg_brush;            // 背景画刷（避免每次创建）
    bool word_wrap;             // 是否换行显示
};

// 复选框状态
struct CheckBoxState {
    HWND hwnd;                  // 控件句柄
    HWND parent;                // 父窗口句柄
    int id;                     // 控件ID
    int x, y, width, height;    // 位置和尺寸
    std::wstring text;          // 显示文本
    bool checked;               // 选中状态
    bool enabled;               // 启用状态
    bool hovered;               // 悬停状态
    bool pressed;               // 按下状态
    UINT32 fg_color;            // 前景色
    UINT32 bg_color;            // 背景色
    UINT32 check_color;         // 勾选标记颜色
    FontStyle font;             // 字体样式
    CheckBoxCallback callback;  // 回调函数
};

// 进度条状态
struct ProgressBarState {
    HWND hwnd;                  // 控件句柄
    HWND parent;                // 父窗口句柄
    int id;                     // 控件ID
    int x, y, width, height;    // 位置和尺寸
    int current_value;          // 当前值 (0-100)
    int target_value;           // 目标值 (0-100)
    float animation_value;      // 动画中间值 (用于平滑过渡)
    bool indeterminate;         // 不确定模式
    float indeterminate_pos;    // 不确定模式动画位置 (0.0-1.0)
    bool enabled;               // 启用状态
    UINT32 fg_color;            // 前景色 (进度条颜色)
    UINT32 bg_color;            // 背景色
    UINT32 border_color;        // 边框颜色
    UINT32 text_color;          // 文本颜色
    bool show_text;             // 是否显示百分比文本
    FontStyle font;             // 字体样式
    ProgressBarCallback callback; // 回调函数
    UINT_PTR timer_id;          // 动画定时器ID
};

// 图片框状态
struct PictureBoxState {
    HWND hwnd;                  // 控件句柄
    HWND parent;                // 父窗口句柄
    int id;                     // 控件ID
    int x, y, width, height;    // 位置和尺寸
    ID2D1Bitmap* bitmap;        // D2D1位图
    IWICBitmapSource* wic_source; // WIC位图源
    ImageScaleMode scale_mode;  // 缩放模式
    float opacity;              // 透明度 (0.0 - 1.0)
    UINT32 bg_color;            // 背景色
    bool enabled;               // 启用状态
    PictureBoxCallback callback; // 回调函数
};

// 单选按钮状态
struct RadioButtonState {
    HWND hwnd;                  // 控件句柄
    HWND parent;                // 父窗口句柄
    int id;                     // 控件ID
    int group_id;               // 分组ID，同组互斥
    int x, y, width, height;    // 位置和尺寸
    std::wstring text;          // 显示文本
    bool checked;               // 选中状态
    bool enabled;               // 启用状态
    bool hovered;               // 悬停状态
    bool pressed;               // 按下状态
    UINT32 fg_color;            // 前景色
    UINT32 bg_color;            // 背景色
    UINT32 dot_color;           // 圆点颜色
    FontStyle font;             // 字体样式
    RadioButtonCallback callback; // 回调函数
};

// 列表框项目
struct ListBoxItem {
    std::wstring text;          // 项目文本
    int id;                     // 项目ID
    void* user_data;            // 用户自定义数据
};

// 列表框状态
struct ListBoxState {
    HWND hwnd;                  // 控件句柄
    HWND parent;                // 父窗口句柄
    int id;                     // 控件ID
    int x, y, width, height;    // 位置和尺寸
    std::vector<ListBoxItem> items; // 所有项目
    int selected_index;         // 当前选中项 (-1表示无选中)
    int hovered_index;          // 悬停项 (-1表示无悬停)
    int scroll_offset;          // 滚动偏移量（像素）
    int item_height;            // 项目高度
    bool multi_select;          // 多选模式
    std::vector<int> selected_indices; // 多选时的选中项
    bool enabled;               // 启用状态
    UINT32 fg_color;            // 前景色
    UINT32 bg_color;            // 背景色
    UINT32 select_color;        // 选中背景色
    UINT32 hover_color;         // 悬停背景色
    FontStyle font;             // 字体样式
    ListBoxCallback callback;   // 回调函数
};

// 组合框状态
struct ComboBoxState {
    HWND hwnd;                  // 主控件句柄
    HWND parent;                // 父窗口句柄
    HWND edit_hwnd;             // 编辑框句柄
    HWND dropdown_hwnd;         // 下拉列表窗口句柄
    int id;                     // 控件ID
    int x, y, width, height;    // 位置和尺寸
    std::vector<std::wstring> items; // 下拉项目
    int selected_index;         // 当前选中项 (-1表示无选中)
    int hovered_index;          // 悬停项 (-1表示无悬停)
    int scroll_offset;          // 滚动偏移量（像素）
    bool dropdown_visible;      // 下拉列表是否可见
    bool readonly;              // 只读模式
    bool enabled;               // 启用状态
    bool button_hovered;        // 下拉按钮悬停状态
    bool button_pressed;        // 下拉按钮按下状态
    int item_height;            // 表项高度
    UINT32 fg_color;            // 前景色
    UINT32 bg_color;            // 背景色
    UINT32 select_color;        // 选中背景色
    UINT32 hover_color;         // 悬停背景色
    FontStyle font;             // 字体样式
    ComboBoxCallback callback;  // 回调函数
};


// D2D组合框状态（完全自定义，支持彩色emoji）
struct D2DComboBoxState {
    HWND hwnd;                      // 主控件句柄
    HWND parent;                    // 父窗口句柄
    HWND edit_hwnd;                 // D2D编辑框句柄
    HWND dropdown_hwnd;             // 下拉列表窗口句柄
    int id;                         // 控件ID
    int x, y, width, height;        // 位置和尺寸
    
    std::vector<std::wstring> items; // 下拉项目
    int selected_index;             // 当前选中项 (-1表示无选中)
    int hovered_index;              // 悬停项 (-1表示无悬停)
    int scroll_offset;              // 滚动偏移量（像素）
    
    bool dropdown_visible;          // 下拉列表是否可见
    bool readonly;                  // 只读模式
    bool enabled;                   // 启用状态
    bool button_hovered;            // 下拉按钮悬停状态
    bool button_pressed;            // 下拉按钮按下状态
    
    int item_height;                // 表项高度
    int button_width;               // 下拉按钮宽度（默认30）
    int max_dropdown_items;         // 下拉列表最多显示项数（默认10）
    
    UINT32 fg_color;                // 前景色
    UINT32 bg_color;                // 背景色
    UINT32 select_color;            // 选中背景色
    UINT32 hover_color;             // 悬停背景色
    UINT32 border_color;            // 边框颜色
    UINT32 button_color;            // 按钮颜色
    
    FontStyle font;                 // 字体样式
    ComboBoxCallback callback;      // 回调函数
    
    ID2D1HwndRenderTarget* render_target; // D2D渲染目标（用于下拉列表）
    IDWriteFactory* dwrite_factory;       // DirectWrite工厂
};

// 热键控件状态
struct HotKeyState {
    HWND hwnd;                  // 控件句柄
    HWND parent;                // 父窗口句柄
    int id;                     // 控件ID
    int x, y, width, height;    // 位置和尺寸
    int vk_code;                // 虚拟键码
    int modifiers;              // 修饰键 (Ctrl=1, Shift=2, Alt=4)
    std::wstring display_text;  // 显示文本
    bool capturing;             // 是否正在捕获
    bool has_focus;             // 是否有焦点
    bool enabled;               // 启用状态
    UINT32 fg_color;            // 前景色
    UINT32 bg_color;            // 背景色
    UINT32 border_color;        // 边框颜色
    FontStyle font;             // 字体样式
    HotKeyCallback callback;    // 回调函数
};

// 分组框回调函数类型 (stdcall 调用约定)
typedef void (__stdcall *GroupBoxCallback)(HWND hGroupBox);

// 分组框状态
struct GroupBoxState {
    HWND hwnd;                  // 控件句柄
    HWND parent;                // 父窗口句柄
    int id;                     // 控件ID
    int x, y, width, height;    // 位置和尺寸
    std::wstring title;         // 标题文本
    std::vector<HWND> children; // 子控件列表
    bool enabled;               // 启用状态
    bool visible;               // 可见状态
    UINT32 border_color;        // 边框颜色
    UINT32 title_color;         // 标题颜色
    UINT32 bg_color;            // 背景色
    FontStyle font;             // 字体样式
    GroupBoxCallback callback;  // 回调函数
};

// Button structure
struct EmojiButton {
    int id;
    std::wstring emoji;
    std::wstring text;
    int x, y, width, height;
    UINT32 bg_color;
    bool is_hovered;
    bool is_pressed;

    bool ContainsPoint(int px, int py) const {
        return px >= x && px <= x + width && py >= y && py <= y + height;
    }
};

// Window state
struct WindowState {
    HWND hwnd;
    ID2D1HwndRenderTarget* render_target;
    IDWriteFactory* dwrite_factory;
    std::vector<EmojiButton> buttons;
};

// Message box button type
enum MsgBoxButtonType {
    MSGBOX_OK = 0,
    MSGBOX_OKCANCEL = 1
};

// Message box state
struct MsgBoxState {
    HWND hwnd;
    HWND parent_hwnd;
    ID2D1HwndRenderTarget* render_target;
    IDWriteFactory* dwrite_factory;
    std::wstring title;
    std::wstring message;
    std::wstring icon_emoji;
    MsgBoxButtonType button_type;
    MessageBoxCallback callback;
    EmojiButton ok_button;
    EmojiButton cancel_button;
    bool result;
};

// Tab 页信息结构
struct TabPageInfo {
    int index;                      // Tab 索引
    std::wstring title;             // Tab 标题
    HWND hContentWindow;            // 内容容器窗口句柄
    bool visible;                   // 是否可见
};

// TabControl 状态管理
struct TabControlState {
    HWND hTabControl;               // Tab Control 句柄
    HWND hParent;                   // 父窗口句柄
    std::vector<TabPageInfo> pages; // 所有 Tab 页信息
    int currentIndex;               // 当前选中的 Tab 索引
    TAB_CALLBACK callback;          // 切换回调函数
};

// Global variables
extern std::map<HWND, WindowState*> g_windows;
extern std::map<HWND, MsgBoxState*> g_msgboxes;
extern std::map<HWND, TabControlState*> g_tab_controls;
extern std::map<HWND, EditBoxState*> g_editboxes;
extern std::map<HWND, LabelState*> g_labels;
extern std::map<HWND, CheckBoxState*> g_checkboxes;
extern std::map<HWND, ProgressBarState*> g_progressbars;
extern std::map<HWND, PictureBoxState*> g_pictureboxes;
extern std::map<HWND, RadioButtonState*> g_radiobuttons;
extern std::map<int, std::vector<HWND>> g_radio_groups;  // 分组管理
extern std::map<HWND, ListBoxState*> g_listboxes;
extern std::map<HWND, ComboBoxState*> g_comboboxes;
extern std::map<HWND, D2DComboBoxState*> g_d2d_comboboxes;
extern std::map<HWND, HotKeyState*> g_hotkeys;
extern std::map<HWND, GroupBoxState*> g_groupboxes;
extern ButtonClickCallback g_button_callback;
extern WindowResizeCallback g_window_resize_callback;
extern WindowCloseCallback g_window_close_callback;

// Export functions (stdcall calling convention)
extern "C" {
    __declspec(dllexport) HWND __stdcall create_window(const char* title, int width, int height);

    __declspec(dllexport) int __stdcall create_emoji_button_bytes(
        HWND parent,
        const unsigned char* emoji_bytes,
        int emoji_len,
        const unsigned char* text_bytes,
        int text_len,
        int x, int y, int width, int height,
        UINT32 bg_color
    );

    __declspec(dllexport) void __stdcall set_button_click_callback(ButtonClickCallback callback);
    __declspec(dllexport) int __stdcall run_message_loop();
    __declspec(dllexport) void __stdcall destroy_window(HWND hwnd);
    __declspec(dllexport) void __stdcall set_window_icon(HWND hwnd, const char* icon_path);
    __declspec(dllexport) void __stdcall set_window_title(HWND hwnd, const char* title_utf8, int title_len);

    // Message box (OK button only, supports Unicode and Emoji)
    __declspec(dllexport) void __stdcall show_message_box_bytes(
        HWND parent,
        const unsigned char* title_bytes,
        int title_len,
        const unsigned char* message_bytes,
        int message_len,
        const unsigned char* icon_bytes,
        int icon_len
    );

    // Confirm box (with callback, supports Unicode and Emoji)
    __declspec(dllexport) void __stdcall show_confirm_box_bytes(
        HWND parent,
        const unsigned char* title_bytes,
        int title_len,
        const unsigned char* message_bytes,
        int message_len,
        const unsigned char* icon_bytes,
        int icon_len,
        MessageBoxCallback callback
    );

    // ========== TabControl 功能 ==========

    // 创建 TabControl
    __declspec(dllexport) HWND __stdcall CreateTabControl(
        HWND hParent,
        int x,
        int y,
        int width,
        int height
    );

    // 添加 Tab 页
    __declspec(dllexport) int __stdcall AddTabItem(
        HWND hTabControl,
        const unsigned char* title_bytes,
        int title_len,
        HWND hContentWindow  // 可选，若为 NULL 则自动创建
    );

    // 移除 Tab 页
    __declspec(dllexport) BOOL __stdcall RemoveTabItem(
        HWND hTabControl,
        int index
    );

    // 设置 Tab 切换回调
    __declspec(dllexport) void __stdcall SetTabCallback(
        HWND hTabControl,
        TAB_CALLBACK pCallback
    );

    // 获取当前选中的 Tab 索引
    __declspec(dllexport) int __stdcall GetCurrentTabIndex(
        HWND hTabControl
    );

    // 切换到指定 Tab
    __declspec(dllexport) BOOL __stdcall SelectTab(
        HWND hTabControl,
        int index
    );

    // 获取 Tab 数量
    __declspec(dllexport) int __stdcall GetTabCount(
        HWND hTabControl
    );

    // 获取指定 Tab 的内容窗口句柄
    __declspec(dllexport) HWND __stdcall GetTabContentWindow(
        HWND hTabControl,
        int index
    );

    // 销毁 TabControl（清理资源）
    __declspec(dllexport) void __stdcall DestroyTabControl(
        HWND hTabControl
    );

    // 手动更新 TabControl 布局（窗口大小改变后调用）
    __declspec(dllexport) void __stdcall UpdateTabControlLayout(
        HWND hTabControl
    );

    // ========== 窗口大小改变回调 ==========

    // 设置窗口大小改变回调
    __declspec(dllexport) void __stdcall SetWindowResizeCallback(
        WindowResizeCallback callback
    );

    // 设置自绘窗口关闭回调
    // 当自绘窗口被关闭（用户点 X 或代码调用 destroy_window）时触发
    __declspec(dllexport) void __stdcall SetWindowCloseCallback(
        WindowCloseCallback callback
    );

    // ========== 编辑框功能 ==========

    // 创建编辑框（vertical_center 仅单行有效）
    __declspec(dllexport) HWND __stdcall CreateEditBox(
        HWND hParent,
        int x, int y, int width, int height,
        const unsigned char* text_bytes,
        int text_len,
        UINT32 fg_color,
        UINT32 bg_color,
        const unsigned char* font_name_bytes,
        int font_name_len,
        int font_size,
        BOOL bold,
        BOOL italic,
        BOOL underline,
        int alignment,  // 0=左, 1=中, 2=右
        BOOL multiline,
        BOOL readonly,
        BOOL password,
        BOOL has_border,
        BOOL vertical_center  // 文本垂直居中（仅单行有效）
    );

    // 获取编辑框文本
    __declspec(dllexport) int __stdcall GetEditBoxText(
        HWND hEdit,
        unsigned char* buffer,
        int buffer_size
    );

    // 设置编辑框文本
    __declspec(dllexport) void __stdcall SetEditBoxText(
        HWND hEdit,
        const unsigned char* text_bytes,
        int text_len
    );

    // 设置编辑框字体
    __declspec(dllexport) void __stdcall SetEditBoxFont(
        HWND hEdit,
        const unsigned char* font_name_bytes,
        int font_name_len,
        int font_size,
        BOOL bold,
        BOOL italic,
        BOOL underline
    );

    // 设置编辑框颜色
    __declspec(dllexport) void __stdcall SetEditBoxColor(
        HWND hEdit,
        UINT32 fg_color,
        UINT32 bg_color
    );

    // 设置编辑框位置和大小
    __declspec(dllexport) void __stdcall SetEditBoxBounds(
        HWND hEdit,
        int x, int y, int width, int height
    );

    // 启用/禁用编辑框
    __declspec(dllexport) void __stdcall EnableEditBox(
        HWND hEdit,
        BOOL enable
    );

    // 显示/隐藏编辑框
    __declspec(dllexport) void __stdcall ShowEditBox(
        HWND hEdit,
        BOOL show
    );

    // 设置编辑框文本是否垂直居中（仅单行编辑框有效）
    __declspec(dllexport) void __stdcall SetEditBoxVerticalCenter(
        HWND hEdit,
        BOOL vertical_center
    );

    // 设置编辑框按键回调（key_down: 1=按下 0=松开; shift/ctrl/alt: 0=未按 1=按下）
    __declspec(dllexport) void __stdcall SetEditBoxKeyCallback(
        HWND hEdit,
        EditBoxKeyCallback callback
    );

    // 创建彩色Emoji编辑框（使用RichEdit控件，支持彩色emoji显示）
    __declspec(dllexport) HWND __stdcall CreateColorEmojiEditBox(
        HWND hParent,
        int x, int y, int width, int height,
        const unsigned char* text_bytes,
        int text_len,
        UINT32 fg_color,
        UINT32 bg_color,
        const unsigned char* font_name_bytes,
        int font_name_len,
        int font_size,
        BOOL bold,
        BOOL italic,
        BOOL underline,
        int alignment,  // 0=左, 1=中, 2=右
        BOOL multiline,
        BOOL readonly,
        BOOL password,
        BOOL has_border,
        BOOL vertical_center  // 文本垂直居中（仅单行有效）
    );

    // ========== 标签功能 ==========

    // 创建标签
    __declspec(dllexport) HWND __stdcall CreateLabel(
        HWND hParent,
        int x, int y, int width, int height,
        const unsigned char* text_bytes,
        int text_len,
        UINT32 fg_color,
        UINT32 bg_color,
        const unsigned char* font_name_bytes,
        int font_name_len,
        int font_size,
        BOOL bold,
        BOOL italic,
        BOOL underline,
        int alignment,  // 0=左, 1=中, 2=右
        BOOL word_wrap  // 是否换行显示
    );

    // 设置标签文本
    __declspec(dllexport) void __stdcall SetLabelText(
        HWND hLabel,
        const unsigned char* text_bytes,
        int text_len
    );

    // 设置标签字体
    __declspec(dllexport) void __stdcall SetLabelFont(
        HWND hLabel,
        const unsigned char* font_name_bytes,
        int font_name_len,
        int font_size,
        BOOL bold,
        BOOL italic,
        BOOL underline
    );

    // 设置标签颜色
    __declspec(dllexport) void __stdcall SetLabelColor(
        HWND hLabel,
        UINT32 fg_color,
        UINT32 bg_color
    );

    // 设置标签位置和大小
    __declspec(dllexport) void __stdcall SetLabelBounds(
        HWND hLabel,
        int x, int y, int width, int height
    );

    // 启用/禁用标签
    __declspec(dllexport) void __stdcall EnableLabel(
        HWND hLabel,
        BOOL enable
    );

    // 显示/隐藏标签
    __declspec(dllexport) void __stdcall ShowLabel(
        HWND hLabel,
        BOOL show
    );

    // ========== 复选框功能 ==========

    // 创建复选框
    __declspec(dllexport) HWND __stdcall CreateCheckBox(
        HWND hParent,
        int x, int y, int width, int height,
        const unsigned char* text_bytes,
        int text_len,
        BOOL checked,
        UINT32 fg_color,
        UINT32 bg_color
    );

    // 获取复选框选中状态
    __declspec(dllexport) BOOL __stdcall GetCheckBoxState(
        HWND hCheckBox
    );

    // 设置复选框选中状态
    __declspec(dllexport) void __stdcall SetCheckBoxState(
        HWND hCheckBox,
        BOOL checked
    );

    // 设置复选框回调
    __declspec(dllexport) void __stdcall SetCheckBoxCallback(
        HWND hCheckBox,
        CheckBoxCallback callback
    );

    // 启用/禁用复选框
    __declspec(dllexport) void __stdcall EnableCheckBox(
        HWND hCheckBox,
        BOOL enable
    );

    // 显示/隐藏复选框
    __declspec(dllexport) void __stdcall ShowCheckBox(
        HWND hCheckBox,
        BOOL show
    );

    // 设置复选框文本
    __declspec(dllexport) void __stdcall SetCheckBoxText(
        HWND hCheckBox,
        const unsigned char* text_bytes,
        int text_len
    );

    // 设置复选框位置和大小
    __declspec(dllexport) void __stdcall SetCheckBoxBounds(
        HWND hCheckBox,
        int x, int y, int width, int height
    );

    // ========== 进度条功能 ==========

    // 创建进度条
    __declspec(dllexport) HWND __stdcall CreateProgressBar(
        HWND hParent,
        int x, int y, int width, int height,
        int initial_value,  // 初始值 (0-100)
        UINT32 fg_color,
        UINT32 bg_color,
        BOOL show_text,     // 是否显示百分比文本
        UINT32 text_color   // 文本颜色
    );

    // 设置进度条值 (0-100)
    __declspec(dllexport) void __stdcall SetProgressValue(
        HWND hProgressBar,
        int value
    );

    // 获取进度条值
    __declspec(dllexport) int __stdcall GetProgressValue(
        HWND hProgressBar
    );

    // 设置进度条不确定模式
    __declspec(dllexport) void __stdcall SetProgressIndeterminate(
        HWND hProgressBar,
        BOOL indeterminate
    );

    // 设置进度条颜色
    __declspec(dllexport) void __stdcall SetProgressBarColor(
        HWND hProgressBar,
        UINT32 fg_color,
        UINT32 bg_color
    );

    // 设置进度条回调
    __declspec(dllexport) void __stdcall SetProgressBarCallback(
        HWND hProgressBar,
        ProgressBarCallback callback
    );

    // 启用/禁用进度条
    __declspec(dllexport) void __stdcall EnableProgressBar(
        HWND hProgressBar,
        BOOL enable
    );

    // 显示/隐藏进度条
    __declspec(dllexport) void __stdcall ShowProgressBar(
        HWND hProgressBar,
        BOOL show
    );

    // 设置进度条位置和大小
    __declspec(dllexport) void __stdcall SetProgressBarBounds(
        HWND hProgressBar,
        int x, int y, int width, int height
    );

    // 设置是否显示百分比文本
    __declspec(dllexport) void __stdcall SetProgressBarShowText(
        HWND hProgressBar,
        BOOL show_text
    );

    // ========== 图片框功能 ==========

    // 创建图片框
    __declspec(dllexport) HWND __stdcall CreatePictureBox(
        HWND hParent,
        int x, int y, int width, int height,
        int scale_mode,     // 缩放模式: 0=不缩放, 1=拉伸, 2=等比缩放, 3=居中
        UINT32 bg_color
    );

    // 从文件加载图片
    __declspec(dllexport) BOOL __stdcall LoadImageFromFile(
        HWND hPictureBox,
        const unsigned char* file_path_bytes,
        int path_len
    );

    // 从内存加载图片
    __declspec(dllexport) BOOL __stdcall LoadImageFromMemory(
        HWND hPictureBox,
        const unsigned char* image_data,
        int data_len
    );

    // 清除图片
    __declspec(dllexport) void __stdcall ClearImage(
        HWND hPictureBox
    );

    // 设置图片透明度
    __declspec(dllexport) void __stdcall SetImageOpacity(
        HWND hPictureBox,
        float opacity  // 0.0 - 1.0
    );

    // 设置图片框回调
    __declspec(dllexport) void __stdcall SetPictureBoxCallback(
        HWND hPictureBox,
        PictureBoxCallback callback
    );

    // 启用/禁用图片框
    __declspec(dllexport) void __stdcall EnablePictureBox(
        HWND hPictureBox,
        BOOL enable
    );

    // 显示/隐藏图片框
    __declspec(dllexport) void __stdcall ShowPictureBox(
        HWND hPictureBox,
        BOOL show
    );

    // 设置图片框位置和大小
    __declspec(dllexport) void __stdcall SetPictureBoxBounds(
        HWND hPictureBox,
        int x, int y, int width, int height
    );

    // 设置图片框缩放模式
    __declspec(dllexport) void __stdcall SetPictureBoxScaleMode(
        HWND hPictureBox,
        int scale_mode
    );

    // 设置图片框背景色
    __declspec(dllexport) void __stdcall SetPictureBoxBackgroundColor(
        HWND hPictureBox,
        UINT32 bg_color
    );

    // ========== 单选按钮功能 ==========

    // 创建单选按钮
    __declspec(dllexport) HWND __stdcall CreateRadioButton(
        HWND hParent,
        int x, int y, int width, int height,
        const unsigned char* text_bytes,
        int text_len,
        int group_id,
        BOOL checked,
        UINT32 fg_color,
        UINT32 bg_color
    );

    // 获取单选按钮选中状态
    __declspec(dllexport) BOOL __stdcall GetRadioButtonState(
        HWND hRadioButton
    );

    // 设置单选按钮选中状态
    __declspec(dllexport) void __stdcall SetRadioButtonState(
        HWND hRadioButton,
        BOOL checked
    );

    // 设置单选按钮回调
    __declspec(dllexport) void __stdcall SetRadioButtonCallback(
        HWND hRadioButton,
        RadioButtonCallback callback
    );

    // 启用/禁用单选按钮
    __declspec(dllexport) void __stdcall EnableRadioButton(
        HWND hRadioButton,
        BOOL enable
    );

    // 显示/隐藏单选按钮
    __declspec(dllexport) void __stdcall ShowRadioButton(
        HWND hRadioButton,
        BOOL show
    );

    // 设置单选按钮文本
    __declspec(dllexport) void __stdcall SetRadioButtonText(
        HWND hRadioButton,
        const unsigned char* text_bytes,
        int text_len
    );

    // 设置单选按钮位置和大小
    __declspec(dllexport) void __stdcall SetRadioButtonBounds(
        HWND hRadioButton,
        int x, int y, int width, int height
    );

    // ========== 列表框功能 ==========

    // 创建列表框
    __declspec(dllexport) HWND __stdcall CreateListBox(
        HWND hParent,
        int x, int y, int width, int height,
        BOOL multi_select,
        UINT32 fg_color,
        UINT32 bg_color
    );

    // 添加列表项
    __declspec(dllexport) int __stdcall AddListItem(
        HWND hListBox,
        const unsigned char* text_bytes,
        int text_len
    );

    // 移除列表项
    __declspec(dllexport) void __stdcall RemoveListItem(
        HWND hListBox,
        int index
    );

    // 清空列表框
    __declspec(dllexport) void __stdcall ClearListBox(
        HWND hListBox
    );

    // 获取选中项索引
    __declspec(dllexport) int __stdcall GetSelectedIndex(
        HWND hListBox
    );

    // 设置选中项索引
    __declspec(dllexport) void __stdcall SetSelectedIndex(
        HWND hListBox,
        int index
    );

    // 获取列表项数量
    __declspec(dllexport) int __stdcall GetListItemCount(
        HWND hListBox
    );

    // 获取列表项文本
    __declspec(dllexport) int __stdcall GetListItemText(
        HWND hListBox,
        int index,
        unsigned char* buffer,
        int buffer_size
    );

    // 设置列表框回调
    __declspec(dllexport) void __stdcall SetListBoxCallback(
        HWND hListBox,
        ListBoxCallback callback
    );

    // 启用/禁用列表框
    __declspec(dllexport) void __stdcall EnableListBox(
        HWND hListBox,
        BOOL enable
    );

    // 显示/隐藏列表框
    __declspec(dllexport) void __stdcall ShowListBox(
        HWND hListBox,
        BOOL show
    );

    // 设置列表框位置和大小
    __declspec(dllexport) void __stdcall SetListBoxBounds(
        HWND hListBox,
        int x, int y, int width, int height
    );

    // ========== 组合框功能 ==========

    // 创建组合框（增强版）
    __declspec(dllexport) HWND __stdcall CreateComboBox(
        HWND hParent,
        int x, int y, int width, int height,
        BOOL readonly,
        UINT32 fg_color,
        UINT32 bg_color,
        int item_height,                        // 表项高度
        const unsigned char* font_name_bytes,  // 字体名称
        int font_name_len,
        int font_size,                          // 字体大小
        BOOL bold,                              // 粗体
        BOOL italic,                            // 斜体
        BOOL underline                          // 下划线
    );

    // 添加组合框项目
    __declspec(dllexport) int __stdcall AddComboItem(
        HWND hComboBox,
        const unsigned char* text_bytes,
        int text_len
    );

    // 移除组合框项目
    __declspec(dllexport) void __stdcall RemoveComboItem(
        HWND hComboBox,
        int index
    );

    // 清空组合框
    __declspec(dllexport) void __stdcall ClearComboBox(
        HWND hComboBox
    );

    // 获取组合框选中项索引
    __declspec(dllexport) int __stdcall GetComboSelectedIndex(
        HWND hComboBox
    );

    // 设置组合框选中项索引
    __declspec(dllexport) void __stdcall SetComboSelectedIndex(
        HWND hComboBox,
        int index
    );

    // 获取组合框项目数量
    __declspec(dllexport) int __stdcall GetComboItemCount(
        HWND hComboBox
    );

    // 获取组合框项目文本
    __declspec(dllexport) int __stdcall GetComboItemText(
        HWND hComboBox,
        int index,
        unsigned char* buffer,
        int buffer_size
    );

    // 设置组合框回调
    __declspec(dllexport) void __stdcall SetComboBoxCallback(
        HWND hComboBox,
        ComboBoxCallback callback
    );

    // 启用/禁用组合框
    __declspec(dllexport) void __stdcall EnableComboBox(
        HWND hComboBox,
        BOOL enable
    );

    // 显示/隐藏组合框
    __declspec(dllexport) void __stdcall ShowComboBox(
        HWND hComboBox,
        BOOL show
    );

    // 设置组合框位置和大小
    __declspec(dllexport) void __stdcall SetComboBoxBounds(
        HWND hComboBox,
        int x, int y, int width, int height
    );

    // 获取组合框文本
    __declspec(dllexport) int __stdcall GetComboBoxText(
        HWND hComboBox,
        unsigned char* buffer,
        int buffer_size
    );

    // 设置组合框文本
    __declspec(dllexport) void __stdcall SetComboBoxText(
        HWND hComboBox,
        const unsigned char* text_bytes,
        int text_len
    );

    // ========== D2D组合框功能（支持彩色emoji） ==========

    // 创建D2D组合框
    __declspec(dllexport) HWND __stdcall CreateD2DComboBox(
        HWND hParent,
        int x, int y, int width, int height,
        BOOL readonly,
        UINT32 fg_color,
        UINT32 bg_color,
        int item_height,
        const unsigned char* font_name_bytes,
        int font_name_len,
        int font_size,
        BOOL bold,
        BOOL italic,
        BOOL underline
    );

    // 添加D2D组合框项目
    __declspec(dllexport) int __stdcall AddD2DComboItem(
        HWND hComboBox,
        const unsigned char* text_bytes,
        int text_len
    );

    // 移除D2D组合框项目
    __declspec(dllexport) void __stdcall RemoveD2DComboItem(
        HWND hComboBox,
        int index
    );

    // 清空D2D组合框
    __declspec(dllexport) void __stdcall ClearD2DComboBox(
        HWND hComboBox
    );

    // 获取D2D组合框选中项索引
    __declspec(dllexport) int __stdcall GetD2DComboSelectedIndex(
        HWND hComboBox
    );

    // 设置D2D组合框选中项索引
    __declspec(dllexport) void __stdcall SetD2DComboSelectedIndex(
        HWND hComboBox,
        int index
    );

    // 获取D2D组合框项目数量
    __declspec(dllexport) int __stdcall GetD2DComboItemCount(
        HWND hComboBox
    );

    // 获取D2D组合框项目文本
    __declspec(dllexport) int __stdcall GetD2DComboItemText(
        HWND hComboBox,
        int index,
        unsigned char* buffer,
        int buffer_size
    );

    // 获取D2D组合框编辑框文本
    __declspec(dllexport) int __stdcall GetD2DComboText(
        HWND hComboBox,
        unsigned char* buffer,
        int buffer_size
    );

    // 设置D2D组合框编辑框文本
    __declspec(dllexport) void __stdcall SetD2DComboText(
        HWND hComboBox,
        const unsigned char* text_bytes,
        int text_len
    );

    // 设置D2D组合框回调
    __declspec(dllexport) void __stdcall SetD2DComboBoxCallback(
        HWND hComboBox,
        ComboBoxCallback callback
    );

    // 启用/禁用D2D组合框
    __declspec(dllexport) void __stdcall EnableD2DComboBox(
        HWND hComboBox,
        BOOL enable
    );

    // 显示/隐藏D2D组合框
    __declspec(dllexport) void __stdcall ShowD2DComboBox(
        HWND hComboBox,
        BOOL show
    );

    // 设置D2D组合框位置和大小
    __declspec(dllexport) void __stdcall SetD2DComboBoxBounds(
        HWND hComboBox,
        int x, int y, int width, int height
    );


    // ========== 热键控件功能 ==========

    // 创建热键控件
    __declspec(dllexport) HWND __stdcall CreateHotKeyControl(
        HWND hParent,
        int x, int y, int width, int height,
        UINT32 fg_color,
        UINT32 bg_color
    );

    // 获取热键
    __declspec(dllexport) void __stdcall GetHotKey(
        HWND hHotKey,
        int* vk_code,
        int* modifiers
    );

    // 设置热键
    __declspec(dllexport) void __stdcall SetHotKey(
        HWND hHotKey,
        int vk_code,
        int modifiers
    );

    // 设置热键回调
    __declspec(dllexport) void __stdcall SetHotKeyCallback(
        HWND hHotKey,
        HotKeyCallback callback
    );

    // 启用/禁用热键控件
    __declspec(dllexport) void __stdcall EnableHotKeyControl(
        HWND hHotKey,
        BOOL enable
    );

    // 显示/隐藏热键控件
    __declspec(dllexport) void __stdcall ShowHotKeyControl(
        HWND hHotKey,
        BOOL show
    );

    // 设置热键控件位置和大小
    __declspec(dllexport) void __stdcall SetHotKeyControlBounds(
        HWND hHotKey,
        int x, int y, int width, int height
    );

    // ========== 分组框功能 ==========

    // 创建分组框
    __declspec(dllexport) HWND __stdcall CreateGroupBox(
        HWND hParent,
        int x, int y, int width, int height,
        const unsigned char* title_bytes,
        int title_len,
        UINT32 border_color,
        UINT32 bg_color
    );

    // 添加子控件到分组框
    __declspec(dllexport) void __stdcall AddChildToGroup(
        HWND hGroupBox,
        HWND hChild
    );

    // 从分组框移除子控件
    __declspec(dllexport) void __stdcall RemoveChildFromGroup(
        HWND hGroupBox,
        HWND hChild
    );

    // 设置分组框标题
    __declspec(dllexport) void __stdcall SetGroupBoxTitle(
        HWND hGroupBox,
        const unsigned char* title_bytes,
        int title_len
    );

    // 启用/禁用分组框
    __declspec(dllexport) void __stdcall EnableGroupBox(
        HWND hGroupBox,
        BOOL enable
    );

    // 显示/隐藏分组框
    __declspec(dllexport) void __stdcall ShowGroupBox(
        HWND hGroupBox,
        BOOL show
    );

    // 设置分组框位置和大小
    __declspec(dllexport) void __stdcall SetGroupBoxBounds(
        HWND hGroupBox,
        int x, int y, int width, int height
    );

    // 设置分组框回调
    __declspec(dllexport) void __stdcall SetGroupBoxCallback(
        HWND hGroupBox,
        GroupBoxCallback callback
    );
}

// Internal functions
LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
LRESULT CALLBACK MsgBoxProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
LRESULT CALLBACK CheckBoxProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData);
LRESULT CALLBACK ProgressBarProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData);
LRESULT CALLBACK PictureBoxProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData);
LRESULT CALLBACK RadioButtonProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData);
LRESULT CALLBACK ListBoxProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData);
LRESULT CALLBACK ComboBoxProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData);
LRESULT CALLBACK ComboDropDownProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
LRESULT CALLBACK HotKeyProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData);
LRESULT CALLBACK GroupBoxProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData);
void DrawButton(ID2D1HwndRenderTarget* rt, IDWriteFactory* factory, const EmojiButton& button);
void DrawMsgBox(ID2D1HwndRenderTarget* rt, IDWriteFactory* factory, MsgBoxState* state);
void DrawCheckBox(ID2D1HwndRenderTarget* rt, IDWriteFactory* factory, CheckBoxState* state);
void DrawProgressBar(ID2D1HwndRenderTarget* rt, IDWriteFactory* factory, ProgressBarState* state);
void DrawPictureBox(ID2D1HwndRenderTarget* rt, IDWriteFactory* factory, PictureBoxState* state);
void DrawRadioButton(ID2D1HwndRenderTarget* rt, IDWriteFactory* factory, RadioButtonState* state);
void DrawListBox(ID2D1HwndRenderTarget* rt, IDWriteFactory* factory, ListBoxState* state);
void DrawComboBox(ID2D1HwndRenderTarget* rt, IDWriteFactory* factory, ComboBoxState* state);
void DrawComboDropDown(ID2D1HwndRenderTarget* rt, IDWriteFactory* factory, ComboBoxState* state);
void DrawHotKey(ID2D1HwndRenderTarget* rt, IDWriteFactory* factory, HotKeyState* state);
void DrawGroupBox(ID2D1HwndRenderTarget* rt, IDWriteFactory* factory, GroupBoxState* state);
std::wstring Utf8ToWide(const unsigned char* bytes, int len);
D2D1_COLOR_F ColorFromUInt32(UINT32 color);
UINT32 LightenColor(UINT32 color, float factor);
UINT32 DarkenColor(UINT32 color, float factor);
HWND CreateMessageBoxWindow(HWND parent, const std::wstring& title, const std::wstring& message,
                            const std::wstring& icon, MsgBoxButtonType type, MessageBoxCallback callback);
void CloseMessageBox(HWND hwnd, bool result);

// TabControl 内部辅助函数
void UpdateTabLayout(TabControlState* state);
LRESULT CALLBACK TabControlSubclassProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData);
