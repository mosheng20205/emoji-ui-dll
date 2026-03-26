#pragma once

#ifndef TREEVIEW_STATE_H
#define TREEVIEW_STATE_H

#include <windows.h>
#include <d2d1.h>
#include <dwrite.h>
#include <vector>
#include <string>
#include <unordered_map>
#include <functional>
#include "treeview_window.h"

// 前向声明
struct TreeNode;
struct TreeViewState;
class TextFormatCache;

// ============================================================================
// 文本格式缓存类（前向声明在前面，完整定义在这里）
// ============================================================================

/**
 * 文本格式缓存类
 * 缓存不同字体大小的 IDWriteTextFormat，避免重复创建和销毁
 */
class TextFormatCache {
private:
    IDWriteFactory* dwrite_factory;
    std::unordered_map<std::wstring, IDWriteTextFormat*> formats;
    
    // 生成缓存键
    std::wstring GenerateKey(
        const wchar_t* font_family,
        float font_size,
        float dpi_scale,
        DWRITE_FONT_WEIGHT font_weight,
        DWRITE_FONT_STYLE font_style
    );
    
public:
    TextFormatCache(IDWriteFactory* factory);
    ~TextFormatCache();
    
    // 获取或创建文本格式
    IDWriteTextFormat* GetOrCreate(
        const wchar_t* font_family,
        float font_size,
        float dpi_scale,
        DWRITE_FONT_WEIGHT font_weight = DWRITE_FONT_WEIGHT_NORMAL,
        DWRITE_FONT_STYLE font_style = DWRITE_FONT_STYLE_NORMAL
    );
    
    // 清除所有缓存
    void Clear();
    
    // 获取缓存大小
    size_t GetCacheSize() const;
};

// ============================================================================
// TreeNode 结构体 - 树形节点数据
// ============================================================================

struct TreeNode {
    int id;                              // 节点唯一 ID
    std::wstring text;                   // 节点文本（UTF-16，内部使用）
    std::wstring icon;                   // 节点图标（emoji）
    
    TreeNode* parent;                    // 父节点指针
    std::vector<TreeNode*> children;     // 子节点列表
    
    bool expanded;                       // 是否展开
    bool selected;                       // 是否选中
    bool has_checkbox;                   // 是否显示复选框
    bool checked;                        // 复选框状态
    bool enabled;                        // 是否启用
    
    D2D1_COLOR_F fore_color;             // 文本颜色
    D2D1_COLOR_F back_color;             // 背景颜色
    std::wstring font_family;            // 字体名称
    float font_size;                     // 字体大小（逻辑单位）
    
    int level;                           // 层级深度（根节点为 0）
    int visible_index;                   // 可见索引（用于渲染）
    
    // 布局信息（像素单位，已 DPI 缩放）
    float y_position;                    // Y 坐标
    float height;                        // 节点高度
    
    // 构造函数
    TreeNode() :
        id(0),
        parent(nullptr),
        expanded(false),
        selected(false),
        has_checkbox(false),
        checked(false),
        enabled(true),
        font_size(14.0f),
        level(0),
        visible_index(-1),
        y_position(0),
        height(0)
    {
        fore_color = D2D1::ColorF(0.2f, 0.2f, 0.2f);
        back_color = D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.0f);
        font_family = L"Microsoft YaHei UI";
    }
};

// ============================================================================
// TreeViewState 结构体 - 树形框控件状态
// ============================================================================

struct TreeViewState {
    HWND hwnd;                           // 窗口句柄
    
    // Direct2D 资源
    ID2D1Factory* d2d_factory;           // D2D 工厂
    ID2D1HwndRenderTarget* render_target; // 渲染目标
    ID2D1SolidColorBrush* brush;         // 画刷
    IDWriteFactory* dwrite_factory;      // DirectWrite 工厂
    IDWriteTextFormat* text_format;      // 文本格式
    TextFormatCache* text_format_cache;  // 文本格式缓存
    
    // 节点数据
    std::vector<TreeNode*> root_nodes;               // 根节点列表
    std::unordered_map<int, TreeNode*> node_map;     // ID -> 节点映射
    int next_node_id;                                // 下一个节点 ID
    TreeNode* selected_node;                         // 当前选中节点
    
    // 可见节点缓存（用于渲染优化）
    std::vector<TreeNode*> visible_nodes;
    bool need_rebuild_visible;                       // 是否需要重建可见列表
    
    // 滚动状态
    int scroll_pos;                      // 当前滚动位置（像素）
    int scroll_max;                      // 最大滚动位置
    int content_height;                  // 内容总高度
    bool show_scrollbar;                 // 是否显示滚动条
    int scrollbar_width;                 // 滚动条宽度（像素）
    bool scrollbar_dragging;             // 是否正在拖动滚动条
    int scrollbar_drag_start_y;          // 滚动条拖动起始 Y 坐标
    int scrollbar_drag_start_pos;        // 滚动条拖动起始滚动位置
    
    // DPI 信息
    float dpi_scale;                     // DPI 缩放比例（1.0 = 96 DPI）
    UINT dpi;                            // 当前 DPI 值
    
    // 样式配置（逻辑单位，会根据 DPI 缩放）
    float node_height;                   // 节点高度（默认 28）
    float indent_width;                  // 缩进宽度（默认 20）
    float icon_size;                     // 图标大小（默认 16）
    float expand_icon_size;              // 展开图标大小（默认 12）
    float item_spacing;                  // 行间距（逻辑单位，行间额外空隙）
    float sidebar_pad_left;              // 侧栏模式左侧内容内边距（逻辑单位）
    float sidebar_pad_right;             // 侧栏模式右侧（箭头区）内边距（逻辑单位）
    bool sidebar_mode;                   // 侧栏折叠菜单模式（右侧箭头、交互见文档）
    
    D2D1_COLOR_F background_color;       // 背景色
    D2D1_COLOR_F text_color;             // 文本颜色（全局默认，新节点前景）
    D2D1_COLOR_F selected_color;         // 选中背景色
    D2D1_COLOR_F selected_foreground;    // 选中前景色（字、侧栏图标与箭头）
    D2D1_COLOR_F hover_color;            // 悬停背景色
    D2D1_COLOR_F border_color;           // 边框颜色
    D2D1_COLOR_F scrollbar_bg_color;     // 滚动条背景色
    D2D1_COLOR_F scrollbar_thumb_color;  // 滚动条滑块颜色
    
    // 交互状态
    TreeNode* hover_node;                // 鼠标悬停节点
    TreeNode* editing_node;              // 正在编辑的节点
    HWND edit_control;                   // 编辑框句柄（已废弃，使用自定义编辑）
    
    // 自定义编辑框状态（支持彩色 emoji）
    bool custom_editing;                 // 是否在自定义编辑模式
    std::wstring edit_text;              // 编辑中的文本
    int edit_cursor_pos;                 // 光标位置（字符索引）
    int edit_selection_start;            // 选择起始位置
    int edit_selection_end;              // 选择结束位置
    D2D1_RECT_F edit_rect;               // 编辑框矩形区域
    DWORD edit_cursor_blink_time;        // 光标闪烁时间
    bool edit_cursor_visible;            // 光标是否可见
    
    bool drag_enabled;                   // 是否启用拖放
    TreeNode* drag_node;                 // 正在拖动的节点
    POINT drag_start_pos;                // 拖动起始位置
    bool is_dragging;                    // 是否正在拖动
    TreeNode* drop_target_node;          // 拖放目标节点
    int drop_insert_index;               // 插入位置索引（-1表示作为子节点，>=0表示作为兄弟节点的索引）
    bool drop_as_child;                  // 是否作为子节点插入
    
    // 回调函数
    void* callback_context;              // 回调上下文
    TreeViewCallback on_node_selected;
    TreeViewCallback on_node_expanded;
    TreeViewCallback on_node_collapsed;
    TreeViewCallback on_node_double_click;
    TreeViewRightClickCallback on_node_right_click;
    TreeViewTextCallback on_node_text_changed;
    TreeViewCheckCallback on_node_checked;
    TreeViewMoveCallback on_node_moved;
    
    // 全局字体（树控件主字体，逻辑单位）
    std::wstring font_family_name;
    float font_size_logical;
    DWRITE_FONT_WEIGHT font_weight;
    DWRITE_FONT_STYLE font_style;
    
    // 构造函数
    TreeViewState() :
        hwnd(NULL),
        d2d_factory(nullptr),
        render_target(nullptr),
        brush(nullptr),
        dwrite_factory(nullptr),
        text_format(nullptr),
        text_format_cache(nullptr),
        next_node_id(1),
        selected_node(nullptr),
        need_rebuild_visible(true),
        scroll_pos(0),
        scroll_max(0),
        content_height(0),
        show_scrollbar(false),
        scrollbar_width(12),
        scrollbar_dragging(false),
        scrollbar_drag_start_y(0),
        scrollbar_drag_start_pos(0),
        dpi_scale(1.0f),
        dpi(96),
        node_height(28.0f),
        indent_width(20.0f),
        icon_size(16.0f),
        expand_icon_size(12.0f),
        item_spacing(0.0f),
        sidebar_pad_left(16.0f),
        sidebar_pad_right(16.0f),
        sidebar_mode(false),
        hover_node(nullptr),
        editing_node(nullptr),
        edit_control(NULL),
        custom_editing(false),
        edit_cursor_pos(0),
        edit_selection_start(0),
        edit_selection_end(0),
        edit_cursor_blink_time(0),
        edit_cursor_visible(true),
        drag_enabled(false),
        drag_node(nullptr),
        is_dragging(false),
        drop_target_node(nullptr),
        drop_insert_index(-1),
        drop_as_child(false),
        callback_context(nullptr),
        on_node_selected(nullptr),
        on_node_expanded(nullptr),
        on_node_collapsed(nullptr),
        on_node_double_click(nullptr),
        on_node_right_click(nullptr),
        on_node_text_changed(nullptr),
        on_node_checked(nullptr),
        on_node_moved(nullptr),
        font_family_name(L"Segoe UI Emoji"),
        font_size_logical(14.0f),
        font_weight(DWRITE_FONT_WEIGHT_NORMAL),
        font_style(DWRITE_FONT_STYLE_NORMAL)
    {
        // 默认颜色（Element UI 风格）
        background_color = D2D1::ColorF(1.0f, 1.0f, 1.0f);           // 白色
        text_color = D2D1::ColorF(0.2f, 0.2f, 0.2f);                 // 深灰色
        selected_color = D2D1::ColorF(0.95f, 0.97f, 1.0f);           // 浅蓝色
        selected_foreground = D2D1::ColorF(0.26f, 0.59f, 0.98f);     // Element 主色字 #409EFF 近似
        hover_color = D2D1::ColorF(0.98f, 0.98f, 0.98f);             // 浅灰色
        border_color = D2D1::ColorF(0.9f, 0.9f, 0.9f);               // 边框灰色
        scrollbar_bg_color = D2D1::ColorF(0.95f, 0.95f, 0.95f);      // 滚动条背景
        scrollbar_thumb_color = D2D1::ColorF(0.7f, 0.7f, 0.7f);      // 滚动条滑块
        
        drag_start_pos.x = 0;
        drag_start_pos.y = 0;
    }
};

// ============================================================================
// 辅助函数声明
// ============================================================================

/**
 * UTF-8 转 UTF-16
 * @param utf8 UTF-8 字符串
 * @param length UTF-8 字符串字节长度
 * @return UTF-16 宽字符串
 */
std::wstring Utf8ToUtf16(const unsigned char* utf8, int length);

/**
 * UTF-16 转 UTF-8
 * @param utf16 UTF-16 宽字符串
 * @return UTF-8 字符串（std::string）
 */
std::string Utf16ToUtf8(const std::wstring& utf16);

/**
 * ARGB 颜色转 D2D1_COLOR_F
 * @param argb ARGB 颜色值（0xAARRGGBB）
 * @return D2D1_COLOR_F 颜色
 */
D2D1_COLOR_F ArgbToColorF(unsigned int argb);

/**
 * 逻辑单位转物理像素
 * @param logical 逻辑单位值
 * @param dpi_scale DPI 缩放比例
 * @return 物理像素值
 */
inline float LogicalToPhysical(float logical, float dpi_scale) {
    return logical * dpi_scale;
}

/**
 * 物理像素转逻辑单位
 * @param physical 物理像素值
 * @param dpi_scale DPI 缩放比例
 * @return 逻辑单位值
 */
inline float PhysicalToLogical(float physical, float dpi_scale) {
    return physical / dpi_scale;
}

/**
 * 安全释放 COM 对象
 */
template<typename T>
void SafeRelease(T** ptr) {
    if (*ptr) {
        (*ptr)->Release();
        *ptr = nullptr;
    }
}

/**
 * 创建节点
 * @param id 节点 ID
 * @param text 节点文本
 * @return 新创建的节点指针
 */
TreeNode* CreateNode(int id, const std::wstring& text);

/**
 * 删除节点（递归删除所有子节点）
 * @param node 要删除的节点
 */
void DeleteNode(TreeNode* node);

/**
 * 删除所有节点
 * @param state 树形框状态
 */
void DeleteAllNodes(TreeViewState* state);

/**
 * 重建可见节点列表
 * @param state 树形框状态
 */
void RebuildVisibleNodes(TreeViewState* state);

/**
 * 递归添加可见节点
 * @param state 树形框状态
 * @param node 当前节点
 */
void AddVisibleNodesRecursive(TreeViewState* state, TreeNode* node);

/**
 * 重新计算布局
 * @param state 树形框状态
 */
void RecalculateLayout(TreeViewState* state);

/**
 * 初始化 DPI 缩放
 * @param state 树形框状态
 */
void InitializeDPI(TreeViewState* state);

/**
 * 重新创建文本格式（DPI 改变时）
 * @param state 树形框状态
 */
void RecreateTextFormat(TreeViewState* state);

/**
 * 根据 ID 查找节点
 * @param state 树形框状态
 * @param node_id 节点 ID
 * @return 节点指针，未找到返回 nullptr
 */
TreeNode* FindNodeById(TreeViewState* state, int node_id);

/**
 * 递归搜索节点文本
 * @param node 当前节点
 * @param search_text 搜索文本
 * @return 匹配节点的 ID，未找到返回 -1
 */
int FindNodeByTextRecursive(TreeNode* node, const std::wstring& search_text);

/**
 * 命中测试 - 根据坐标查找节点
 * @param state 树形框状态
 * @param x X 坐标
 * @param y Y 坐标
 * @return 节点指针，未找到返回 nullptr
 */
TreeNode* HitTestNode(TreeViewState* state, int x, int y);

/**
 * 获取上一个可见节点
 * @param state 树形框状态
 * @param node 当前节点
 * @return 上一个可见节点，没有返回 nullptr
 */
TreeNode* GetPreviousVisibleNode(TreeViewState* state, TreeNode* node);

/**
 * 获取下一个可见节点
 * @param state 树形框状态
 * @param node 当前节点
 * @return 下一个可见节点，没有返回 nullptr
 */
TreeNode* GetNextVisibleNode(TreeViewState* state, TreeNode* node);

/**
 * 滚动到节点
 * @param state 树形框状态
 * @param node 目标节点
 */
void ScrollToNodeInternal(TreeViewState* state, TreeNode* node);

/**
 * 移动节点到新位置
 * @param state 树形框状态
 * @param node 要移动的节点
 * @param new_parent 新的父节点（nullptr表示根节点）
 * @param insert_index 插入位置索引
 * @return 成功返回true，失败返回false
 */
bool MoveNodeToPosition(TreeViewState* state, TreeNode* node, TreeNode* new_parent, int insert_index);

/**
 * 退出编辑模式（前向声明）
 * @param state 树形框状态
 * @param save_changes 是否保存更改
 * @return 成功返回 true，失败返回 false
 */
bool ExitEditMode(TreeViewState* state, bool save_changes);

/**
 * 进入编辑模式
 * @param state 树形框状态
 * @param node 要编辑的节点
 * @return 成功返回 true，失败返回 false
 */
bool EnterEditMode(TreeViewState* state, TreeNode* node);

/**
 * 编辑框子类化窗口过程
 */
LRESULT CALLBACK EditControlSubclassProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData);

/**
 * 获取客户区宽度
 * @param hwnd 窗口句柄
 * @return 客户区宽度
 */
int GetClientWidth(HWND hwnd);

/**
 * 获取客户区高度
 * @param hwnd 窗口句柄
 * @return 客户区高度
 */
int GetClientHeight(HWND hwnd);

// ============================================================================
// 渲染函数声明
// ============================================================================

/**
 * 主渲染函数
 * @param state 树形框状态
 */
void RenderTreeView(TreeViewState* state);

/**
 * 渲染单个节点
 * @param state 树形框状态
 * @param node 要渲染的节点
 * @param y_offset Y 偏移量
 */
void RenderNode(TreeViewState* state, TreeNode* node, float y_offset);

/**
 * 侧栏模式右侧折叠箭头（∨ / ∧）
 */
void RenderSidebarChevron(TreeViewState* state, float center_x, float center_y, float size, bool expanded);

/**
 * 渲染展开/折叠图标
 * @param state 树形框状态
 * @param x X 坐标
 * @param y Y 坐标
 * @param expanded 是否展开
 */
void RenderExpandIcon(TreeViewState* state, float x, float y, bool expanded);

/**
 * 渲染复选框
 * @param state 树形框状态
 * @param x X 坐标
 * @param y Y 坐标
 * @param size 复选框大小
 * @param checked 是否选中
 */
void RenderCheckbox(TreeViewState* state, float x, float y, float size, bool checked);

/**
 * 渲染 Emoji 图标
 * @param state 树形框状态
 * @param emoji Emoji 字符串
 * @param x X 坐标
 * @param y Y 坐标
 * @param size 图标大小
 */
void RenderEmoji(TreeViewState* state, const wchar_t* emoji, float x, float y, float size);

/**
 * 渲染滚动条
 * @param state 树形框状态
 */
void RenderScrollbar(TreeViewState* state);

/**
 * 计算拖放目标
 * @param state 树形框状态
 * @param x 鼠标X坐标
 * @param y 鼠标Y坐标
 * @param target_node 输出：目标节点
 * @param insert_index 输出：插入索引
 * @param as_child 输出：是否作为子节点
 * @return 是否找到有效的拖放目标
 */
bool CalculateDropTarget(TreeViewState* state, int x, int y, TreeNode** target_node, int* insert_index, bool* as_child);

/**
 * 渲染拖放指示线
 * @param state 树形框状态
 */
void RenderDropIndicator(TreeViewState* state);

// ============================================================================
// 性能优化函数声明
// ============================================================================

/**
 * 只重绘单个节点的矩形区域
 * @param state 树形框状态
 * @param node 要重绘的节点
 */
void InvalidateNode(TreeViewState* state, TreeNode* node);

/**
 * 使用二分查找快速定位第一个可见节点的索引
 * @param state 树形框状态
 * @param visible_top 可见区域顶部Y坐标
 * @return 第一个可见节点的索引
 */
int FindFirstVisibleNodeIndex(TreeViewState* state, float visible_top);

/**
 * 使用二分查找快速定位最后一个可见节点的索引
 * @param state 树形框状态
 * @param visible_bottom 可见区域底部Y坐标
 * @return 最后一个可见节点的索引
 */
int FindLastVisibleNodeIndex(TreeViewState* state, float visible_bottom);

#endif // TREEVIEW_STATE_H
