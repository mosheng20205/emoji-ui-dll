#pragma once

#include <windows.h>
#include <commctrl.h>
#include <d2d1.h>
#include <dwrite.h>
#include <uxtheme.h>
#include <wincodec.h>  // WIC (Windows Imaging Component)
#include <richedit.h>  // RichEdit控件（支持彩色emoji）
#include <dwmapi.h>    // DWM (Desktop Window Manager) - 自定义标题栏
#include <string>
#include <vector>
#include <map>

#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "dwrite.lib")
#pragma comment(lib, "comctl32.lib")
#pragma comment(lib, "uxtheme.lib")
#pragma comment(lib, "windowscodecs.lib")  // WIC库
#pragma comment(lib, "dwmapi.lib")         // DWM库

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

// 菜单项点击回调函数类型 (stdcall 调用约定)
// menu_id: 顶级菜单ID（如 #菜单_主题设置 / #菜单_测试菜单）
// item_id: 被点击的菜单项ID（子项ID或顶级ID）
typedef void (__stdcall *MenuItemClickCallback)(int menu_id, int item_id);

// 前向声明子菜单窗口类
class SubMenuWindow;

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

// ========== 通用事件回调类型 (需求 8.1-8.10) ==========

// 鼠标进入/离开回调 (hwnd)
typedef void (__stdcall *MouseEnterCallback)(HWND hwnd);
typedef void (__stdcall *MouseLeaveCallback)(HWND hwnd);

// 双击回调 (hwnd, x, y)
typedef void (__stdcall *DoubleClickCallback)(HWND hwnd, int x, int y);

// 右键点击回调 (hwnd, x, y)
typedef void (__stdcall *RightClickCallback)(HWND hwnd, int x, int y);

// 焦点回调 (hwnd)
typedef void (__stdcall *FocusCallback)(HWND hwnd);
typedef void (__stdcall *BlurCallback)(HWND hwnd);

// 键盘回调 (hwnd, vk_code, shift, ctrl, alt)
typedef void (__stdcall *KeyDownCallback)(HWND hwnd, int vk_code, int shift, int ctrl, int alt);
typedef void (__stdcall *KeyUpCallback)(HWND hwnd, int vk_code, int shift, int ctrl, int alt);

// 字符输入回调 (hwnd, char_code) - Unicode字符
typedef void (__stdcall *CharCallback)(HWND hwnd, int char_code);

// 值改变回调 (hwnd)
typedef void (__stdcall *ValueChangedCallback)(HWND hwnd);

// 通用事件回调集合
struct EventCallbacks {
    MouseEnterCallback on_mouse_enter;
    MouseLeaveCallback on_mouse_leave;
    DoubleClickCallback on_double_click;
    RightClickCallback on_right_click;
    FocusCallback on_focus;
    BlurCallback on_blur;
    KeyDownCallback on_key_down;
    KeyUpCallback on_key_up;
    CharCallback on_char;
    ValueChangedCallback on_value_changed;

    EventCallbacks() : on_mouse_enter(nullptr), on_mouse_leave(nullptr),
        on_double_click(nullptr), on_right_click(nullptr),
        on_focus(nullptr), on_blur(nullptr),
        on_key_down(nullptr), on_key_up(nullptr),
        on_char(nullptr), on_value_changed(nullptr) {}
};

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
    EventCallbacks events;      // 通用事件回调
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
    EventCallbacks events;                // 通用事件回调
};

// 标签状态
struct LabelState {
    HWND hwnd;                  // 标签句柄
    HWND parent;                // 父窗口句柄
    int id;                     // 控件ID
    int x, y, width, height;    // 位置和尺寸
    std::wstring text;          // 文本内容
    UINT32 fg_color;            // 前景色 (ARGB)
    UINT32 bg_color;            // 背景色 (ARGB)
    FontStyle font;             // 字体样式
    TextAlignment alignment;    // 文字对齐
    HBRUSH bg_brush;            // 背景画刷（避免每次创建）
    bool word_wrap;             // 是否换行显示
    bool parent_drawn = false;  // 是否改为父窗口统一绘制
    bool visible = true;        // 父绘制模式下的可见状态
    EventCallbacks events;      // 通用事件回调
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
    EventCallbacks events;      // 通用事件回调
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
    EventCallbacks events;      // 通用事件回调
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
    EventCallbacks events;      // 通用事件回调
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
    EventCallbacks events;      // 通用事件回调
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
    EventCallbacks events;      // 通用事件回调
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
    EventCallbacks events;      // 通用事件回调
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
    EventCallbacks events;          // 通用事件回调

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
    EventCallbacks events;      // 通用事件回调
};

// 分组框回调函数类型 (stdcall 调用约定)
typedef void (__stdcall *GroupBoxCallback)(HWND hGroupBox);

// ========== DataGridView 回调函数类型 ==========

// 单元格点击回调 (hGrid, row, col)
typedef void (__stdcall *DataGridCellClickCallback)(HWND hGrid, int row, int col);

// 单元格双击回调 (hGrid, row, col)
typedef void (__stdcall *DataGridCellDoubleClickCallback)(HWND hGrid, int row, int col);

// 单元格值改变回调 (hGrid, row, col)
typedef void (__stdcall *DataGridCellValueChangedCallback)(HWND hGrid, int row, int col);

// 列头点击回调 (hGrid, col)
typedef void (__stdcall *DataGridColumnHeaderClickCallback)(HWND hGrid, int col);

// 选择改变回调 (hGrid, row, col)
typedef void (__stdcall *DataGridSelectionChangedCallback)(HWND hGrid, int row, int col);

// 虚拟模式数据请求回调 (hGrid, row, col, buffer, buffer_size) -> 返回写入的字节数
typedef int (__stdcall *DataGridVirtualDataCallback)(HWND hGrid, int row, int col, unsigned char* buffer, int buffer_size);

// ========== DataGridView 枚举和结构 ==========

// 列类型
enum DataGridColumnType {
    DGCOL_TEXT = 0,         // 文本列
    DGCOL_CHECKBOX = 1,     // 复选框列
    DGCOL_BUTTON = 2,       // 按钮列
    DGCOL_LINK = 3,         // 链接列
    DGCOL_IMAGE = 4         // 图片列
};

// 排序方向
enum DataGridSortOrder {
    DGSORT_NONE = 0,        // 无排序
    DGSORT_ASC = 1,         // 升序
    DGSORT_DESC = 2         // 降序
};

// 选择模式
enum DataGridSelectionMode {
    DGSEL_CELL = 0,         // 单元格选择
    DGSEL_ROW = 1           // 整行选择
};

// 单元格样式
struct DataGridCellStyle {
    UINT32 fg_color;        // 前景色 (0=使用默认)
    UINT32 bg_color;        // 背景色 (0=使用默认)
    bool bold;              // 粗体
    bool italic;            // 斜体
};

// 列定义
struct DataGridColumn {
    std::wstring header_text;   // 列头文本
    int width;                  // 列宽（像素）
    int min_width;              // 最小列宽
    DataGridColumnType type;    // 列类型
    bool resizable;             // 是否可调整宽度
    bool sortable;              // 是否可排序
    DataGridSortOrder sort_order; // 当前排序方向
    DWRITE_TEXT_ALIGNMENT header_alignment;  // 列头对齐方式
    DWRITE_TEXT_ALIGNMENT cell_alignment;    // 单元格对齐方式
    
    // 构造函数，设置默认值
    DataGridColumn() : 
        width(100), min_width(30), type(DGCOL_TEXT), 
        resizable(true), sortable(true), sort_order(DGSORT_NONE),
        header_alignment(DWRITE_TEXT_ALIGNMENT_LEADING),
        cell_alignment(DWRITE_TEXT_ALIGNMENT_LEADING) {}
};

// 单元格数据
struct DataGridCell {
    std::wstring text;          // 文本内容
    bool checked;               // 复选框状态 (DGCOL_CHECKBOX)
    DataGridCellStyle style;    // 单元格样式
};

// 行数据
struct DataGridRow {
    std::vector<DataGridCell> cells; // 单元格列表
    int height;                      // 行高（0=使用默认）
};

// DataGridView 状态
struct DataGridViewState {
    HWND hwnd;                  // 控件句柄
    HWND parent;                // 父窗口句柄
    int id;                     // 控件ID
    int x, y, width, height;   // 位置和尺寸

    // 列和行数据
    std::vector<DataGridColumn> columns;
    std::vector<DataGridRow> rows;

    // 虚拟模式
    bool virtual_mode;          // 是否虚拟模式
    int virtual_row_count;      // 虚拟模式总行数

    // 选择状态
    int selected_row;           // 选中行 (-1=无)
    int selected_col;           // 选中列 (-1=无)
    DataGridSelectionMode selection_mode; // 选择模式

    // 编辑状态
    bool editing;               // 是否正在编辑
    int edit_row;               // 编辑行
    int edit_col;               // 编辑列
    HWND edit_hwnd;             // 编辑框句柄

    // 悬停状态
    int hovered_row;            // 悬停行
    int hovered_col;            // 悬停列

    // 滚动
    int scroll_x;               // 水平滚动偏移
    int scroll_y;               // 垂直滚动偏移

    // 列宽调整
    bool resizing_col;          // 是否正在调整列宽
    int resize_col_index;       // 正在调整的列索引
    int resize_start_x;         // 调整起始X坐标
    int resize_start_width;     // 调整起始列宽

    // 滚动条拖拽
    bool scrollbar_v_dragging;  // 是否正在拖拽纵向滚动条
    bool scrollbar_h_dragging;  // 是否正在拖拽横向滚动条
    float scrollbar_drag_offset; // 拖拽起始偏移（鼠标相对thumb顶部）

    // 排序
    int sort_col;               // 排序列 (-1=无)
    DataGridSortOrder sort_order; // 排序方向

    // 冻结
    bool freeze_header;         // 冻结首行（列头）
    bool freeze_first_col;      // 冻结首列

    // 外观
    int header_height;          // 列头高度
    int default_row_height;     // 默认行高
    bool zebra_stripe;          // 隔行变色
    bool show_grid_lines;       // 显示网格线
    bool enabled;               // 启用状态
    UINT32 fg_color;            // 前景色
    UINT32 bg_color;            // 背景色
    UINT32 header_bg_color;     // 列头背景色
    UINT32 header_fg_color;     // 列头前景色
    UINT32 grid_line_color;     // 网格线颜色
    UINT32 select_color;        // 选中背景色
    UINT32 hover_color;         // 悬停背景色
    UINT32 zebra_color;         // 隔行变色背景色
    FontStyle font;             // 字体样式

    // 回调
    DataGridCellClickCallback cell_click_cb;
    DataGridCellDoubleClickCallback cell_dblclick_cb;
    DataGridCellValueChangedCallback cell_value_changed_cb;
    DataGridColumnHeaderClickCallback col_header_click_cb;
    DataGridSelectionChangedCallback selection_changed_cb;
    DataGridVirtualDataCallback virtual_data_cb;
    EventCallbacks events;      // 通用事件回调
};

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
    EventCallbacks events;      // 通用事件回调
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
    // 自定义标题栏支持
    std::wstring title;           // 窗口标题（用于D2D彩色emoji绘制）
    UINT32 titlebar_color = 0;    // 标题栏背景色（0=跟随主题）
    int titlebar_height = 30;     // 标题栏高度（像素）
    bool custom_titlebar = true;  // 是否启用自定义标题栏
    int hovered_titlebar_button = 0; // 0=无 1=最小化 2=最大化 3=关闭
    bool titlebar_mouse_tracking = false;
};

// 菜单项状态
struct MenuItem {
    int id;                     // 菜单项ID
    std::wstring text;          // 显示文本
    std::wstring shortcut;      // 快捷键文本
    bool enabled = true;        // 是否启用
    bool checked = false;       // 是否勾选
    bool separator = false;     // 是否分隔线
    std::vector<MenuItem> sub_items; // 子菜单项
    D2D1_RECT_F bounds = {};    // 菜单项边界（用于命中测试）
};

// 菜单栏状态
struct MenuBarState {
    HWND hwnd;                      // 所属窗口句柄
    std::vector<MenuItem> items;    // 菜单项列表
    int hovered_index = -1;         // 当前悬停的菜单索引
    int opened_index = -1;          // 当前展开的菜单索引
    int opened_menu_id = 0;         // 当前展开的顶级菜单ID（用于回调传参）
    bool visible = true;            // 是否可见
    UINT32 bg_color = 0;            // 背景色（0=默认）
    UINT32 fg_color = 0;            // 前景色（0=默认）
    FontStyle font;                 // 字体样式
    MenuItemClickCallback callback; // 菜单项点击回调
    SubMenuWindow* submenu = nullptr; // 子菜单窗口
};

// 弹出菜单状态
struct PopupMenuState {
    HWND hwnd = nullptr;            // 弹出菜单窗口句柄
    HWND owner_hwnd;                // 关联窗口句柄
    ID2D1HwndRenderTarget* render_target = nullptr; // D2D渲染目标
    IDWriteFactory* dwrite_factory = nullptr;       // DirectWrite工厂
    std::vector<MenuItem> items;    // 菜单项列表
    int hovered_index = -1;         // 当前悬停的菜单索引
    bool visible = false;           // 是否可见
    int x = 0;                      // 显示位置X
    int y = 0;                      // 显示位置Y
    int width = 0;                  // 菜单宽度
    int height = 0;                 // 菜单高度
    int item_height = 0;            // 菜单项高度
    UINT32 bg_color = 0;            // 背景色（0=默认）
    UINT32 fg_color = 0;            // 前景色（0=默认）
    UINT32 hover_color = 0;         // 悬停背景色
    FontStyle font;                 // 字体样式
    MenuItemClickCallback callback; // 菜单项点击回调
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

// ========== 主题系统 (需求 11.1-11.10) ==========

// 主题颜色
struct ThemeColors {
    UINT32 primary;             // 主色 (默认 #409EFF)
    UINT32 success;             // 成功色 (默认 #67C23A)
    UINT32 warning;             // 警告色 (默认 #E6A23C)
    UINT32 danger;              // 危险色 (默认 #F56C6C)
    UINT32 info;                // 信息色 (默认 #909399)
    UINT32 text_primary;        // 主要文本色 (默认 #303133)
    UINT32 text_regular;        // 常规文本色 (默认 #606266)
    UINT32 text_secondary;      // 次要文本色 (默认 #909399)
    UINT32 text_placeholder;    // 占位文本色 (默认 #C0C4CC)
    UINT32 border_base;         // 基础边框色 (默认 #DCDFE6)
    UINT32 border_light;        // 浅色边框色 (默认 #E4E7ED)
    UINT32 border_lighter;      // 更浅边框色 (默认 #EBEEF5)
    UINT32 border_extra_light;  // 极浅边框色 (默认 #F2F6FC)
    UINT32 background;          // 背景色 (默认 #FFFFFF)
    UINT32 background_light;    // 浅色背景 (默认 #F5F7FA)
};

// 主题字体
struct ThemeFonts {
    std::wstring title_font;    // 标题字体 (默认 "Microsoft YaHei UI")
    std::wstring body_font;     // 正文字体 (默认 "Microsoft YaHei UI")
    std::wstring mono_font;     // 等宽字体 (默认 "Consolas")
    int title_size;             // 标题字号 (默认 16)
    int body_size;              // 正文字号 (默认 14)
    int small_size;             // 小号字号 (默认 12)
};

// 主题尺寸
struct ThemeSizes {
    float border_radius;        // 圆角半径 (默认 4.0f)
    float border_width;         // 边框宽度 (默认 1.0f)
    int control_height;         // 控件高度 (默认 32)
    int spacing_small;          // 小间距 (默认 8)
    int spacing_medium;         // 中间距 (默认 16)
    int spacing_large;          // 大间距 (默认 24)
};

// 主题结构
struct Theme {
    std::wstring name;          // 主题名称
    bool dark_mode;             // 是否暗色模式
    ThemeColors colors;         // 颜色
    ThemeFonts fonts;           // 字体
    ThemeSizes sizes;           // 尺寸
};

// 主题回调函数类型 (stdcall 调用约定)
typedef void (__stdcall *ThemeChangedCallback)(const char* theme_name);

extern Theme* g_current_theme;
extern Theme g_light_theme;
extern Theme g_dark_theme;
extern ThemeChangedCallback g_theme_changed_callback;

// 主题辅助函数（C++内部使用，不导出）
// 注意：这些函数不在 extern "C" 块中声明，在 cpp 中直接定义

// ========== 布局管理器 ==========

enum LayoutType {
    LAYOUT_NONE = 0,
    LAYOUT_FLOW_HORIZONTAL = 1,     // 水平流式布局
    LAYOUT_FLOW_VERTICAL = 2,       // 垂直流式布局
    LAYOUT_GRID = 3,                // 网格布局
    LAYOUT_DOCK = 4                 // 停靠布局
};

enum DockPosition {
    DOCK_NONE = 0,
    DOCK_TOP = 1,
    DOCK_BOTTOM = 2,
    DOCK_LEFT = 3,
    DOCK_RIGHT = 4,
    DOCK_FILL = 5
};

struct LayoutProperties {
    int margin_left = 0;
    int margin_top = 0;
    int margin_right = 0;
    int margin_bottom = 0;
    DockPosition dock = DOCK_NONE;
    bool stretch_horizontal = false;
    bool stretch_vertical = false;
    int order = 0;  // 控件在布局中的顺序
};

// 布局控件项：可以是HWND控件或Emoji按钮ID
struct LayoutItem {
    HWND hwnd = nullptr;       // HWND控件句柄（如果是HWND控件）
    int button_id = 0;         // Emoji按钮ID（如果是Emoji按钮）
    bool is_button = false;    // true=Emoji按钮, false=HWND控件

    bool operator==(const LayoutItem& other) const {
        if (is_button != other.is_button) return false;
        return is_button ? (button_id == other.button_id) : (hwnd == other.hwnd);
    }
};

struct LayoutManager {
    HWND parent_hwnd = nullptr;
    LayoutType type = LAYOUT_NONE;
    int rows = 0;
    int columns = 0;
    int spacing = 0;
    int padding_left = 0;
    int padding_top = 0;
    int padding_right = 0;
    int padding_bottom = 0;
    std::map<HWND, LayoutProperties> control_props;
    std::vector<HWND> control_order;  // 按添加顺序排列的HWND控件列表
    // Emoji按钮布局支持
    std::vector<LayoutItem> item_order;  // 统一顺序列表（HWND + Emoji按钮）
    std::map<int, LayoutProperties> button_props;  // Emoji按钮ID -> 布局属性
};

extern std::map<HWND, LayoutManager*> g_layout_managers;

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
extern std::map<HWND, DataGridViewState*> g_datagrids;
extern std::map<HWND, MenuBarState*> g_menu_bars;
extern std::map<HWND, PopupMenuState*> g_popup_menus;
extern std::map<HWND, HWND> g_control_menu_bindings;
extern ButtonClickCallback g_button_callback;
extern WindowResizeCallback g_window_resize_callback;
extern WindowCloseCallback g_window_close_callback;

// Export functions (stdcall calling convention)
extern "C" {
    __declspec(dllexport) HWND __stdcall create_window(const char* title, int width, int height);
    __declspec(dllexport) HWND __stdcall create_window_bytes(const unsigned char* title_bytes, int title_len, int width, int height);
    __declspec(dllexport) HWND __stdcall create_window_bytes_ex(const unsigned char* title_bytes, int title_len, int width, int height, UINT32 titlebar_color);
    __declspec(dllexport) void __stdcall set_window_titlebar_color(HWND hwnd, UINT32 color);

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
    __declspec(dllexport) void __stdcall set_message_loop_main_window(HWND hwnd);
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

    // ========== 菜单栏 / 右键菜单 ==========

    // 创建菜单栏（绑定到窗口）
    __declspec(dllexport) HWND __stdcall CreateMenuBar(
        HWND hWindow
    );

    // 销毁菜单栏
    __declspec(dllexport) void __stdcall DestroyMenuBar(
        HWND hMenuBar
    );

    // 添加菜单栏项
    __declspec(dllexport) int __stdcall MenuBarAddItem(
        HWND hMenuBar,
        const unsigned char* text_bytes,
        int text_len,
        int item_id
    );

    // 添加菜单栏子项
    __declspec(dllexport) int __stdcall MenuBarAddSubItem(
        HWND hMenuBar,
        int parent_item_id,
        const unsigned char* text_bytes,
        int text_len,
        int item_id
    );

    // 设置菜单栏位置和大小
    __declspec(dllexport) void __stdcall SetMenuBarPlacement(
        HWND hMenuBar,
        int x, int y, int width, int height
    );

    // 设置菜单栏回调
    __declspec(dllexport) void __stdcall SetMenuBarCallback(
        HWND hMenuBar,
        MenuItemClickCallback callback
    );

    // 更新菜单栏子项文本
    __declspec(dllexport) BOOL __stdcall MenuBarUpdateSubItemText(
        HWND hMenuBar,
        int parent_item_id,
        int item_id,
        const unsigned char* text_bytes,
        int text_len
    );

    // 创建弹出菜单（右键菜单）
    __declspec(dllexport) HWND __stdcall CreateEmojiPopupMenu(
        HWND hOwner
    );

    // 销毁弹出菜单
    __declspec(dllexport) void __stdcall DestroyEmojiPopupMenu(
        HWND hPopupMenu
    );

    // 添加弹出菜单项
    __declspec(dllexport) int __stdcall PopupMenuAddItem(
        HWND hPopupMenu,
        const unsigned char* text_bytes,
        int text_len,
        int item_id
    );

    // 添加弹出菜单子项
    __declspec(dllexport) int __stdcall PopupMenuAddSubItem(
        HWND hPopupMenu,
        int parent_item_id,
        const unsigned char* text_bytes,
        int text_len,
        int item_id
    );

    // 绑定控件与弹出菜单
    __declspec(dllexport) void __stdcall BindControlMenu(
        HWND hControl,
        HWND hPopupMenu
    );

    // 显示右键菜单
    __declspec(dllexport) void __stdcall ShowContextMenu(
        HWND hPopupMenu,
        int x,
        int y
    );

    // 设置弹出菜单回调
    __declspec(dllexport) void __stdcall SetPopupMenuCallback(
        HWND hPopupMenu,
        MenuItemClickCallback callback
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

    // 获取D2D组合框选中项文本
    __declspec(dllexport) int __stdcall GetD2DComboSelectedText(
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

    // 清除热键
    __declspec(dllexport) void __stdcall ClearHotKey(
        HWND hHotKey
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

    // ========== DataGridView 功能 ==========

    // 创建DataGridView
    __declspec(dllexport) HWND __stdcall CreateDataGridView(
        HWND hParent,
        int x, int y, int width, int height,
        BOOL virtual_mode,
        BOOL zebra_stripe,
        UINT32 fg_color,
        UINT32 bg_color
    );

    // --- 列管理 ---

    // 添加文本列
    __declspec(dllexport) int __stdcall DataGrid_AddTextColumn(
        HWND hGrid,
        const unsigned char* header_bytes,
        int header_len,
        int width
    );

    // 添加复选框列
    __declspec(dllexport) int __stdcall DataGrid_AddCheckBoxColumn(
        HWND hGrid,
        const unsigned char* header_bytes,
        int header_len,
        int width
    );

    // 添加按钮列
    __declspec(dllexport) int __stdcall DataGrid_AddButtonColumn(
        HWND hGrid,
        const unsigned char* header_bytes,
        int header_len,
        int width
    );

    // 添加链接列
    __declspec(dllexport) int __stdcall DataGrid_AddLinkColumn(
        HWND hGrid,
        const unsigned char* header_bytes,
        int header_len,
        int width
    );

    // 添加图片列
    __declspec(dllexport) int __stdcall DataGrid_AddImageColumn(
        HWND hGrid,
        const unsigned char* header_bytes,
        int header_len,
        int width
    );

    // 移除列
    __declspec(dllexport) void __stdcall DataGrid_RemoveColumn(
        HWND hGrid,
        int col_index
    );

    // 清空所有列
    __declspec(dllexport) void __stdcall DataGrid_ClearColumns(
        HWND hGrid
    );

    // 获取列数
    __declspec(dllexport) int __stdcall DataGrid_GetColumnCount(
        HWND hGrid
    );

    // 设置列宽
    __declspec(dllexport) void __stdcall DataGrid_SetColumnWidth(
        HWND hGrid,
        int col_index,
        int width
    );

    // --- 行管理 ---

    // 添加行
    __declspec(dllexport) int __stdcall DataGrid_AddRow(
        HWND hGrid
    );

    // 移除行
    __declspec(dllexport) void __stdcall DataGrid_RemoveRow(
        HWND hGrid,
        int row_index
    );

    // 清空所有行
    __declspec(dllexport) void __stdcall DataGrid_ClearRows(
        HWND hGrid
    );

    // 获取行数
    __declspec(dllexport) int __stdcall DataGrid_GetRowCount(
        HWND hGrid
    );

    // --- 单元格操作 ---

    // 设置单元格文本
    __declspec(dllexport) void __stdcall DataGrid_SetCellText(
        HWND hGrid,
        int row, int col,
        const unsigned char* text_bytes,
        int text_len
    );

    // 获取单元格文本
    __declspec(dllexport) int __stdcall DataGrid_GetCellText(
        HWND hGrid,
        int row, int col,
        unsigned char* buffer,
        int buffer_size
    );

    // 设置单元格复选框状态
    __declspec(dllexport) void __stdcall DataGrid_SetCellChecked(
        HWND hGrid,
        int row, int col,
        BOOL checked
    );

    // 获取单元格复选框状态
    __declspec(dllexport) BOOL __stdcall DataGrid_GetCellChecked(
        HWND hGrid,
        int row, int col
    );

    // 设置单元格样式
    __declspec(dllexport) void __stdcall DataGrid_SetCellStyle(
        HWND hGrid,
        int row, int col,
        UINT32 fg_color,
        UINT32 bg_color,
        BOOL bold,
        BOOL italic
    );

    // --- 选择和导航 ---

    // 获取选中行
    __declspec(dllexport) int __stdcall DataGrid_GetSelectedRow(
        HWND hGrid
    );

    // 获取选中列
    __declspec(dllexport) int __stdcall DataGrid_GetSelectedCol(
        HWND hGrid
    );

    // 设置选中单元格
    __declspec(dllexport) void __stdcall DataGrid_SetSelectedCell(
        HWND hGrid,
        int row, int col
    );

    // 设置选择模式
    __declspec(dllexport) void __stdcall DataGrid_SetSelectionMode(
        HWND hGrid,
        int mode  // 0=单元格, 1=整行
    );

    // --- 排序 ---

    // 按列排序
    __declspec(dllexport) void __stdcall DataGrid_SortByColumn(
        HWND hGrid,
        int col_index,
        int sort_order  // 0=无, 1=升序, 2=降序
    );

    // --- 冻结 ---

    // 设置冻结首行
    __declspec(dllexport) void __stdcall DataGrid_SetFreezeHeader(
        HWND hGrid,
        BOOL freeze
    );

    // 设置冻结首列
    __declspec(dllexport) void __stdcall DataGrid_SetFreezeFirstColumn(
        HWND hGrid,
        BOOL freeze
    );

    // --- 虚拟模式 ---

    // 设置虚拟模式行数
    __declspec(dllexport) void __stdcall DataGrid_SetVirtualRowCount(
        HWND hGrid,
        int row_count
    );

    // 设置虚拟模式数据回调
    __declspec(dllexport) void __stdcall DataGrid_SetVirtualDataCallback(
        HWND hGrid,
        DataGridVirtualDataCallback callback
    );

    // --- 外观 ---

    // 设置显示网格线
    __declspec(dllexport) void __stdcall DataGrid_SetShowGridLines(
        HWND hGrid,
        BOOL show
    );

    // 设置默认行高
    __declspec(dllexport) void __stdcall DataGrid_SetDefaultRowHeight(
        HWND hGrid,
        int height
    );

    // 设置列头高度
    __declspec(dllexport) void __stdcall DataGrid_SetHeaderHeight(
        HWND hGrid,
        int height
    );

    // --- 事件回调 ---

    // 设置单元格点击回调
    __declspec(dllexport) void __stdcall DataGrid_SetCellClickCallback(
        HWND hGrid,
        DataGridCellClickCallback callback
    );

    // 设置单元格双击回调
    __declspec(dllexport) void __stdcall DataGrid_SetCellDoubleClickCallback(
        HWND hGrid,
        DataGridCellDoubleClickCallback callback
    );

    // 设置单元格值改变回调
    __declspec(dllexport) void __stdcall DataGrid_SetCellValueChangedCallback(
        HWND hGrid,
        DataGridCellValueChangedCallback callback
    );

    // 设置列头点击回调
    __declspec(dllexport) void __stdcall DataGrid_SetColumnHeaderClickCallback(
        HWND hGrid,
        DataGridColumnHeaderClickCallback callback
    );

    // 设置选择改变回调
    __declspec(dllexport) void __stdcall DataGrid_SetSelectionChangedCallback(
        HWND hGrid,
        DataGridSelectionChangedCallback callback
    );

    // --- 其他 ---

    // 启用/禁用DataGridView
    __declspec(dllexport) void __stdcall DataGrid_Enable(
        HWND hGrid,
        BOOL enable
    );

    // 显示/隐藏DataGridView
    __declspec(dllexport) void __stdcall DataGrid_Show(
        HWND hGrid,
        BOOL show
    );

    // 设置DataGridView位置和大小
    __declspec(dllexport) void __stdcall DataGrid_SetBounds(
        HWND hGrid,
        int x, int y, int width, int height
    );

    // 刷新DataGridView
    __declspec(dllexport) void __stdcall DataGrid_Refresh(
        HWND hGrid
    );

    // 设置列头对齐方式 (0=左对齐, 1=居中, 2=右对齐)
    __declspec(dllexport) void __stdcall DataGrid_SetColumnHeaderAlignment(
        HWND hGrid,
        int col_index,
        int alignment
    );

    // 设置列单元格对齐方式 (0=左对齐, 1=居中, 2=右对齐)
    __declspec(dllexport) void __stdcall DataGrid_SetColumnCellAlignment(
        HWND hGrid,
        int col_index,
        int alignment
    );

    // 导出CSV
    __declspec(dllexport) BOOL __stdcall DataGrid_ExportCSV(
        HWND hGrid,
        const unsigned char* file_path_bytes,
        int path_len
    );

    // ========== 通用事件回调设置 (需求 8.1-8.10) ==========

    // 设置鼠标进入回调
    __declspec(dllexport) void __stdcall SetMouseEnterCallback(HWND hControl, MouseEnterCallback callback);

    // 设置鼠标离开回调
    __declspec(dllexport) void __stdcall SetMouseLeaveCallback(HWND hControl, MouseLeaveCallback callback);

    // 设置双击回调
    __declspec(dllexport) void __stdcall SetDoubleClickCallback(HWND hControl, DoubleClickCallback callback);

    // 设置右键点击回调
    __declspec(dllexport) void __stdcall SetRightClickCallback(HWND hControl, RightClickCallback callback);

    // 设置获得焦点回调
    __declspec(dllexport) void __stdcall SetFocusCallback(HWND hControl, FocusCallback callback);

    // 设置失去焦点回调
    __declspec(dllexport) void __stdcall SetBlurCallback(HWND hControl, BlurCallback callback);

    // 设置键盘按下回调
    __declspec(dllexport) void __stdcall SetKeyDownCallback(HWND hControl, KeyDownCallback callback);

    // 设置键盘松开回调
    __declspec(dllexport) void __stdcall SetKeyUpCallback(HWND hControl, KeyUpCallback callback);

    // 设置字符输入回调
    __declspec(dllexport) void __stdcall SetCharCallback(HWND hControl, CharCallback callback);

    // 设置值改变回调
    __declspec(dllexport) void __stdcall SetValueChangedCallback(HWND hControl, ValueChangedCallback callback);

    // ========== 布局管理器功能 ==========

    // 设置窗口的布局管理器
    // layout_type: 0=无, 1=水平流式, 2=垂直流式, 3=网格, 4=停靠
    __declspec(dllexport) void __stdcall SetLayoutManager(
        HWND hParent,
        int layout_type,
        int rows,
        int columns,
        int spacing
    );

    // 设置布局管理器的内边距
    __declspec(dllexport) void __stdcall SetLayoutPadding(
        HWND hParent,
        int padding_left,
        int padding_top,
        int padding_right,
        int padding_bottom
    );

    // 设置控件的布局属性
    __declspec(dllexport) void __stdcall SetControlLayoutProps(
        HWND hControl,
        int margin_left,
        int margin_top,
        int margin_right,
        int margin_bottom,
        int dock_position,
        BOOL stretch_horizontal,
        BOOL stretch_vertical
    );

    // 将控件添加到布局管理器
    __declspec(dllexport) void __stdcall AddControlToLayout(
        HWND hParent,
        HWND hControl
    );

    // 从布局管理器移除控件
    __declspec(dllexport) void __stdcall RemoveControlFromLayout(
        HWND hParent,
        HWND hControl
    );

    // 立即重新计算布局
    __declspec(dllexport) void __stdcall UpdateLayout(HWND hParent);

    // 移除窗口的布局管理器
    __declspec(dllexport) void __stdcall RemoveLayoutManager(HWND hParent);

    // ========== 主题系统 API ==========

    // 从JSON字符串加载主题
    __declspec(dllexport) BOOL __stdcall LoadThemeFromJSON(
        const unsigned char* json_bytes,
        int json_len
    );

    // 从文件加载主题
    __declspec(dllexport) BOOL __stdcall LoadThemeFromFile(
        const unsigned char* file_path_bytes,
        int path_len
    );

    // 设置当前主题 (theme_name: "light" 或 "dark" 或自定义名称)
    __declspec(dllexport) void __stdcall SetTheme(
        const unsigned char* theme_name_bytes,
        int name_len
    );

    // 设置暗色模式 (切换亮色/暗色主题)
    __declspec(dllexport) void __stdcall SetDarkMode(BOOL dark_mode);

    // 获取主题颜色 (color_name: "primary", "success", "warning", "danger", "info",
    //   "text_primary", "text_regular", "text_secondary", "text_placeholder",
    //   "border_base", "border_light", "border_lighter", "border_extra_light",
    //   "background", "background_light")
    __declspec(dllexport) UINT32 __stdcall EW_GetThemeColor(
        const unsigned char* color_name_bytes,
        int name_len
    );

    // 获取主题字体名称 (font_type: 0=标题, 1=正文, 2=等宽)
    // 返回UTF-8字节数，写入buffer
    __declspec(dllexport) int __stdcall EW_GetThemeFont(
        int font_type,
        unsigned char* buffer,
        int buffer_size
    );

    // 获取主题字号 (font_type: 0=标题, 1=正文, 2=小号)
    __declspec(dllexport) int __stdcall GetThemeFontSize(int font_type);

    // 获取主题尺寸 (size_type: 0=圆角半径, 1=边框宽度, 2=控件高度,
    //   3=小间距, 4=中间距, 5=大间距)
    __declspec(dllexport) int __stdcall GetThemeSize(int size_type);

    // 获取当前是否暗色模式
    __declspec(dllexport) BOOL __stdcall IsDarkMode();

    // 获取当前主题名称 (返回UTF-8字节数，写入buffer)
    __declspec(dllexport) int __stdcall EW_GetCurrentThemeName(
        unsigned char* buffer,
        int buffer_size
    );

    // 设置主题切换回调
    __declspec(dllexport) void __stdcall SetThemeChangedCallback(ThemeChangedCallback callback);
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
LRESULT CALLBACK DataGridViewProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData);
LRESULT CALLBACK PopupMenuProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
void DrawButton(ID2D1HwndRenderTarget* rt, IDWriteFactory* factory, const EmojiButton& button);
void DrawPopupMenu(ID2D1HwndRenderTarget* rt, IDWriteFactory* factory, PopupMenuState* popup);
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
void DrawDataGridView(ID2D1HwndRenderTarget* rt, IDWriteFactory* factory, DataGridViewState* state);
std::wstring Utf8ToWide(const unsigned char* bytes, int len);
D2D1_COLOR_F ColorFromUInt32(UINT32 color);
UINT32 LightenColor(UINT32 color, float factor);
UINT32 DarkenColor(UINT32 color, float factor);
EventCallbacks* FindEventCallbacks(HWND hwnd);
HWND CreateMessageBoxWindow(HWND parent, const std::wstring& title, const std::wstring& message,
                            const std::wstring& icon, MsgBoxButtonType type, MessageBoxCallback callback);
void CloseMessageBox(HWND hwnd, bool result);

// TabControl 内部辅助函数
void UpdateTabLayout(TabControlState* state);
LRESULT CALLBACK TabControlSubclassProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData);

// 布局管理器内部函数
void CalculateFlowLayout(LayoutManager* lm, int client_width, int client_height, HDWP* hdwp);
void CalculateGridLayout(LayoutManager* lm, int client_width, int client_height, HDWP* hdwp);
void CalculateDockLayout(LayoutManager* lm, int client_width, int client_height, HDWP* hdwp);
