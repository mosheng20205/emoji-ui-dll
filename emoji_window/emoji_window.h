#pragma once

#include <windows.h>
#include <commctrl.h>
#include <d2d1.h>
#include <dwrite.h>
#include <uxtheme.h>
#include <string>
#include <vector>
#include <map>

#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "dwrite.lib")
#pragma comment(lib, "comctl32.lib")
#pragma comment(lib, "uxtheme.lib")

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
    HBRUSH bg_brush;            // 背景画刷（避免每次创建）
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

    // 创建编辑框
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
        BOOL has_border
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
        int alignment  // 0=左, 1=中, 2=右
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
}

// Internal functions
LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
LRESULT CALLBACK MsgBoxProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
void DrawButton(ID2D1HwndRenderTarget* rt, IDWriteFactory* factory, const EmojiButton& button);
void DrawMsgBox(ID2D1HwndRenderTarget* rt, IDWriteFactory* factory, MsgBoxState* state);
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
