#include "treeview_window.h"
#include "treeview_state.h"
#include <windowsx.h>
#include <algorithm>
#include <cwctype>
#include <commctrl.h>  // 用于子类化功能
#include <richedit.h>  // 用于 RichEdit 控件（支持彩色 emoji）

#pragma comment(lib, "comctl32.lib")  // 链接 comctl32 库

// ============================================================================
// 全局变量
// ============================================================================

// 全局 State 管理 map
static std::unordered_map<HWND, TreeViewState*> g_treeview_states;

// 窗口类名
static const wchar_t* TREEVIEW_CLASS_NAME = L"CustomTreeViewClass";

// 窗口类是否已注册
static bool g_class_registered = false;

// ============================================================================
// 辅助函数实现
// ============================================================================

/**
 * 获取树形框状态
 */
TreeViewState* GetTreeViewState(HWND hwnd) {
    auto it = g_treeview_states.find(hwnd);
    if (it != g_treeview_states.end()) {
        return it->second;
    }
    return nullptr;
}

/**
 * UTF-8 转 UTF-16
 */
std::wstring Utf8ToUtf16(const unsigned char* utf8, int length) {
    if (!utf8 || length <= 0) {
        return std::wstring();
    }
    
    // 计算需要的宽字符数量
    int wchar_count = MultiByteToWideChar(
        CP_UTF8,
        0,
        reinterpret_cast<const char*>(utf8),
        length,
        nullptr,
        0
    );
    
    if (wchar_count <= 0) {
        return std::wstring();
    }
    
    // 分配缓冲区并转换
    std::wstring result(wchar_count, 0);
    MultiByteToWideChar(
        CP_UTF8,
        0,
        reinterpret_cast<const char*>(utf8),
        length,
        &result[0],
        wchar_count
    );
    
    return result;
}

/**
 * UTF-16 转 UTF-8
 */
std::string Utf16ToUtf8(const std::wstring& utf16) {
    if (utf16.empty()) {
        return std::string();
    }
    
    // 计算需要的字节数量
    int byte_count = WideCharToMultiByte(
        CP_UTF8,
        0,
        utf16.c_str(),
        static_cast<int>(utf16.length()),
        nullptr,
        0,
        nullptr,
        nullptr
    );
    
    if (byte_count <= 0) {
        return std::string();
    }
    
    // 分配缓冲区并转换
    std::string result(byte_count, 0);
    WideCharToMultiByte(
        CP_UTF8,
        0,
        utf16.c_str(),
        static_cast<int>(utf16.length()),
        &result[0],
        byte_count,
        nullptr,
        nullptr
    );
    
    return result;
}

/**
 * ARGB 颜色转 D2D1_COLOR_F
 */
D2D1_COLOR_F ArgbToColorF(unsigned int argb) {
    float a = ((argb >> 24) & 0xFF) / 255.0f;
    float r = ((argb >> 16) & 0xFF) / 255.0f;
    float g = ((argb >> 8) & 0xFF) / 255.0f;
    float b = (argb & 0xFF) / 255.0f;
    
    return D2D1::ColorF(r, g, b, a);
}

/**
 * 创建节点
 * 分配 TreeNode 内存，初始化所有字段，设置默认颜色和状态
 */
TreeNode* CreateNode(int id, const std::wstring& text) {
    TreeNode* node = new TreeNode();
    
    // 基本信息
    node->id = id;
    node->text = text;
    node->icon = L"";
    
    // 层级关系
    node->parent = nullptr;
    node->children.clear();
    
    // 状态
    node->expanded = false;
    node->selected = false;
    node->has_checkbox = false;
    node->checked = false;
    node->enabled = true;
    
    // 默认颜色
    node->fore_color = D2D1::ColorF(0.2f, 0.2f, 0.2f);        // 深灰色文本
    node->back_color = D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.0f);  // 透明背景
    
    // 字体
    node->font_family = L"Microsoft YaHei UI";
    node->font_size = 14.0f;
    
    // 布局信息
    node->level = 0;
    node->visible_index = -1;
    node->y_position = 0;
    node->height = 0;
    
    return node;
}

/**
 * 删除节点（递归）
 */
void DeleteNode(TreeNode* node) {
    if (!node) return;
    
    // 递归删除所有子节点
    for (TreeNode* child : node->children) {
        DeleteNode(child);
    }
    
    delete node;
}

/**
 * 删除所有节点
 */
void DeleteAllNodes(TreeViewState* state) {
    if (!state) return;
    
    for (TreeNode* root : state->root_nodes) {
        DeleteNode(root);
    }
    
    state->root_nodes.clear();
    state->node_map.clear();
    state->visible_nodes.clear();
    state->selected_node = nullptr;
    state->hover_node = nullptr;
    state->editing_node = nullptr;
    state->drag_node = nullptr;
    state->drop_target_node = nullptr;
    state->drop_insert_index = -1;
    state->drop_as_child = false;
    state->is_dragging = false;
}

/**
 * 根据 ID 查找节点
 */
TreeNode* FindNodeById(TreeViewState* state, int node_id) {
    if (!state) return nullptr;
    
    auto it = state->node_map.find(node_id);
    if (it != state->node_map.end()) {
        return it->second;
    }
    
    return nullptr;
}

/**
 * 递归搜索节点文本
 */
int FindNodeByTextRecursive(TreeNode* node, const std::wstring& search_text) {
    if (!node) {
        return -1;
    }
    
    // 比较当前节点的文本
    if (node->text == search_text) {
        return node->id;
    }
    
    // 递归搜索子节点
    for (TreeNode* child : node->children) {
        int result = FindNodeByTextRecursive(child, search_text);
        if (result != -1) {
            return result;
        }
    }
    
    return -1; // 未找到
}

/**
 * 获取客户区宽度
 */
int GetClientWidth(HWND hwnd) {
    RECT rect;
    GetClientRect(hwnd, &rect);
    return rect.right - rect.left;
}

/**
 * 获取客户区高度
 */
int GetClientHeight(HWND hwnd) {
    RECT rect;
    GetClientRect(hwnd, &rect);
    return rect.bottom - rect.top;
}

/**
 * 递归添加可见节点
 */
void AddVisibleNodesRecursive(TreeViewState* state, TreeNode* node) {
    if (!state || !node) return;
    
    state->visible_nodes.push_back(node);
    node->visible_index = static_cast<int>(state->visible_nodes.size()) - 1;
    
    // 如果节点展开，递归添加子节点
    if (node->expanded) {
        for (TreeNode* child : node->children) {
            AddVisibleNodesRecursive(state, child);
        }
    }
}

/**
 * 重建可见节点列表
 */
void RebuildVisibleNodes(TreeViewState* state) {
    if (!state) return;
    
    state->visible_nodes.clear();
    
    // 递归遍历所有根节点
    for (TreeNode* root : state->root_nodes) {
        AddVisibleNodesRecursive(state, root);
    }
    
    state->need_rebuild_visible = false;
}

/**
 * 重新计算布局
 */
void RecalculateLayout(TreeViewState* state) {
    if (!state) return;
    
    float y_offset = 0;
    float node_height = state->node_height * state->dpi_scale;
    
    for (TreeNode* node : state->visible_nodes) {
        node->y_position = y_offset;
        node->height = node_height;
        y_offset += node_height;
    }
    
    state->content_height = static_cast<int>(y_offset);
    
    // 计算滚动条
    int client_height = GetClientHeight(state->hwnd);
    state->scroll_max = (std::max)(0, state->content_height - client_height);
    state->show_scrollbar = state->scroll_max > 0;
    
    // 限制滚动位置
    if (state->scroll_pos > state->scroll_max) {
        state->scroll_pos = state->scroll_max;
    }
}

/**
 * 初始化 DPI
 * 使用 GetDpiForWindow 获取当前 DPI，计算 dpi_scale，并缩放所有尺寸参数
 */
void InitializeDPI(TreeViewState* state) {
    if (!state || !state->hwnd) return;
    
    // 获取窗口 DPI
    state->dpi = GetDpiForWindow(state->hwnd);
    
    // 计算 DPI 缩放比例（96 DPI 为基准）
    state->dpi_scale = state->dpi / 96.0f;
    
    // 注意：这里的尺寸参数保持为逻辑单位（不在这里缩放）
    // 在渲染时会根据 dpi_scale 进行动态缩放
    // 这样可以在 DPI 改变时更灵活地调整
    // node_height, indent_width, icon_size, expand_icon_size 保持为逻辑单位
}

/**
 * 重新创建文本格式
 * 使用TextFormatCache获取或创建文本格式，避免重复创建
 * 使用 Segoe UI Emoji 字体以支持彩色emoji显示
 */
void RecreateTextFormat(TreeViewState* state) {
    if (!state || !state->text_format_cache) return;
    
    // 释放旧的文本格式（不需要手动释放，由缓存管理）
    state->text_format = nullptr;
    
    // 从缓存获取或创建新的文本格式
    // 使用 Segoe UI Emoji 字体以支持彩色emoji，中文会自动fallback到合适的字体
    // 基础字体大小为 14.0f 逻辑单位
    state->text_format = state->text_format_cache->GetOrCreate(
        L"Segoe UI Emoji",      // 使用支持彩色emoji的字体
        14.0f,                  // 基础字体大小
        state->dpi_scale        // DPI 缩放比例
    );
}

/**
 * 命中测试
 */
TreeNode* HitTestNode(TreeViewState* state, int x, int y) {
    if (!state) return nullptr;
    
    // 调整坐标（考虑滚动）
    float adjusted_y = y + state->scroll_pos;
    
    // 遍历可见节点
    for (TreeNode* node : state->visible_nodes) {
        float node_top = node->y_position;
        float node_bottom = node_top + node->height;
        
        if (adjusted_y >= node_top && adjusted_y < node_bottom) {
            return node;
        }
    }
    
    return nullptr;
}

/**
 * 获取上一个可见节点
 */
TreeNode* GetPreviousVisibleNode(TreeViewState* state, TreeNode* node) {
    if (!state || !node || node->visible_index <= 0) {
        return nullptr;
    }
    
    int prev_index = node->visible_index - 1;
    if (prev_index >= 0 && prev_index < static_cast<int>(state->visible_nodes.size())) {
        return state->visible_nodes[prev_index];
    }
    
    return nullptr;
}

/**
 * 获取下一个可见节点
 */
TreeNode* GetNextVisibleNode(TreeViewState* state, TreeNode* node) {
    if (!state || !node) {
        return nullptr;
    }
    
    int next_index = node->visible_index + 1;
    if (next_index >= 0 && next_index < static_cast<int>(state->visible_nodes.size())) {
        return state->visible_nodes[next_index];
    }
    
    return nullptr;
}

/**
 * 递归展开所有节点
 */
void ExpandAllNodesRecursive(TreeNode* node) {
    if (!node) return;
    
    node->expanded = true;
    
    for (TreeNode* child : node->children) {
        ExpandAllNodesRecursive(child);
    }
}

/**
 * 递归折叠所有节点
 */
void CollapseAllNodesRecursive(TreeNode* node) {
    if (!node) return;
    
    node->expanded = false;
    
    for (TreeNode* child : node->children) {
        CollapseAllNodesRecursive(child);
    }
}

/**
 * 滚动到节点（内部函数）
 */
void ScrollToNodeInternal(TreeViewState* state, TreeNode* node) {
    if (!state || !node) return;
    
    int client_height = GetClientHeight(state->hwnd);
    float node_top = node->y_position;
    float node_bottom = node_top + node->height;
    
    // 如果节点在可见区域上方
    if (node_top < state->scroll_pos) {
        state->scroll_pos = static_cast<int>(node_top);
    }
    // 如果节点在可见区域下方
    else if (node_bottom > state->scroll_pos + client_height) {
        state->scroll_pos = static_cast<int>(node_bottom - client_height);
    }
    
    // 限制滚动位置
    state->scroll_pos = (std::max)(0, (std::min)(state->scroll_pos, state->scroll_max));
}

/**
 * 进入编辑模式
 * 在双击时创建 Edit 控件覆盖节点文本，设置编辑框位置、大小、字体
 */
/**
 * 进入编辑模式（自定义编辑框，支持彩色 emoji）
 */
bool EnterEditMode(TreeViewState* state, TreeNode* node) {
    if (!state || !node || !node->enabled) {
        return false;
    }
    
    // 如果已经在编辑模式，先退出
    if (state->editing_node || state->custom_editing) {
        ExitEditMode(state, true);
    }
    
    // 计算编辑框的位置和大小
    float x_base = node->level * state->indent_width * state->dpi_scale;
    float x_cursor = x_base;
    
    // 跳过展开图标
    x_cursor += (state->expand_icon_size + 8) * state->dpi_scale;
    
    // 跳过复选框
    if (node->has_checkbox) {
        x_cursor += (16 + 6) * state->dpi_scale;
    }
    
    // 跳过图标
    if (!node->icon.empty()) {
        x_cursor += (state->icon_size + 6) * state->dpi_scale;
    }
    
    // 计算编辑框位置（相对于窗口客户区）
    float edit_x = x_cursor;
    float edit_y = node->y_position - state->scroll_pos;
    float edit_width = GetClientWidth(state->hwnd) - edit_x - 20 * state->dpi_scale;
    float edit_height = node->height;
    
    // 确保编辑框在可见区域内
    if (edit_y < 0 || edit_y + edit_height > GetClientHeight(state->hwnd)) {
        // 滚动到节点位置
        ScrollToNodeInternal(state, node);
        edit_y = node->y_position - state->scroll_pos;
    }
    
    // 设置自定义编辑状态
    state->custom_editing = true;
    state->editing_node = node;
    state->edit_text = node->text;
    state->edit_cursor_pos = static_cast<int>(node->text.length());  // 光标在末尾
    state->edit_selection_start = static_cast<int>(node->text.length());  // 不选中文本
    state->edit_selection_end = static_cast<int>(node->text.length());    // 不选中文本
    state->edit_rect = D2D1::RectF(edit_x, edit_y, edit_x + edit_width, edit_y + edit_height);
    state->edit_cursor_blink_time = GetTickCount();
    state->edit_cursor_visible = true;
    
    // 设置定时器用于光标闪烁
    SetTimer(state->hwnd, 1, 500, NULL);  // 500ms 闪烁一次
    
    // 触发重绘
    InvalidateRect(state->hwnd, NULL, FALSE);
    
    return true;
}

/**
 * 退出编辑模式（自定义编辑框）
 */
bool ExitEditMode(TreeViewState* state, bool save_changes) {
    if (!state || !state->editing_node || !state->custom_editing) {
        return false;
    }
    
    TreeNode* editing_node = state->editing_node;
    
    if (save_changes) {
        // 检查文本是否有变化
        if (state->edit_text != editing_node->text) {
            // 更新节点文本
            editing_node->text = state->edit_text;
            
            // 触发文本改变回调
            if (state->on_node_text_changed) {
                // 将 UTF-16 转换为 UTF-8 用于回调
                std::string new_text_utf8 = Utf16ToUtf8(state->edit_text);
                state->on_node_text_changed(
                    editing_node->id,
                    reinterpret_cast<const unsigned char*>(new_text_utf8.c_str()),
                    static_cast<int>(new_text_utf8.length()),
                    state->callback_context
                );
            }
        }
    }
    
    // 清除编辑状态
    state->custom_editing = false;
    state->editing_node = nullptr;
    state->edit_text.clear();
    state->edit_cursor_pos = 0;
    state->edit_selection_start = 0;
    state->edit_selection_end = 0;
    
    // 停止光标闪烁定时器
    KillTimer(state->hwnd, 1);
    
    // 触发重绘
    InvalidateRect(state->hwnd, NULL, FALSE);
    
    return true;
}

/**
 * 编辑框子类化窗口过程
 * 处理 Enter 键保存编辑、Esc 键取消编辑
 */
LRESULT CALLBACK EditControlSubclassProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData) {
    TreeViewState* state = reinterpret_cast<TreeViewState*>(dwRefData);
    
    switch (msg) {
        case WM_KEYDOWN:
            switch (wParam) {
                case VK_RETURN:
                    // Enter 键：保存编辑并退出
                    if (state) {
                        ExitEditMode(state, true);
                    }
                    return 0;
                    
                case VK_ESCAPE:
                    // Esc 键：取消编辑并退出
                    if (state) {
                        ExitEditMode(state, false);
                    }
                    return 0;
            }
            break;
            
        case WM_KILLFOCUS:
            // 失去焦点：保存编辑并退出
            if (state) {
                ExitEditMode(state, true);
            }
            return 0;
            
        case WM_NCDESTROY:
            // 移除子类化
            RemoveWindowSubclass(hwnd, EditControlSubclassProc, uIdSubclass);
            break;
    }
    
    return DefSubclassProc(hwnd, msg, wParam, lParam);
}

// ============================================================================
// 窗口过程（前向声明）
// ============================================================================

LRESULT CALLBACK TreeViewWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

// ============================================================================
// 渲染函数（前向声明）
// ============================================================================

void RenderTreeView(TreeViewState* state);
void RenderNode(TreeViewState* state, TreeNode* node, float y_offset);
void RenderExpandIcon(TreeViewState* state, float x, float y, bool expanded);
void RenderCheckbox(TreeViewState* state, float x, float y, float size, bool checked);
void RenderEmoji(TreeViewState* state, const wchar_t* emoji, float x, float y, float size);
void RenderScrollbar(TreeViewState* state);
void RenderCustomEditBox(TreeViewState* state);

// ============================================================================
// API 实现（占位符 - 将在后续任务中实现）
// ============================================================================

// 这些函数将在后续任务中实现
// 这里只提供基本的框架

HWND __stdcall CreateTreeView(
    HWND parent,
    int x, int y,
    int width, int height,
    unsigned int bg_color,
    unsigned int text_color,
    void* callback_context
) {
    // 参数验证
    if (!parent) {
        return NULL;
    }
    
    // 注册窗口类（首次调用时）
    if (!g_class_registered) {
        WNDCLASSEXW wc = {};
        wc.cbSize = sizeof(WNDCLASSEXW);
        wc.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
        wc.lpfnWndProc = TreeViewWndProc;
        wc.cbClsExtra = 0;
        wc.cbWndExtra = 0;
        wc.hInstance = GetModuleHandle(NULL);
        wc.hIcon = NULL;
        wc.hCursor = LoadCursor(NULL, IDC_ARROW);
        wc.hbrBackground = NULL;  // 使用 Direct2D 绘制，不需要背景刷
        wc.lpszMenuName = NULL;
        wc.lpszClassName = TREEVIEW_CLASS_NAME;
        wc.hIconSm = NULL;
        
        if (!RegisterClassExW(&wc)) {
            return NULL;
        }
        
        g_class_registered = true;
    }
    
    // 创建子窗口
    HWND hwnd = CreateWindowExW(
        0,
        TREEVIEW_CLASS_NAME,
        L"",
        WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN,
        x, y, width, height,
        parent,
        NULL,
        GetModuleHandle(NULL),
        NULL
    );
    
    if (!hwnd) {
        return NULL;
    }
    
    // 创建 TreeViewState
    TreeViewState* state = new TreeViewState();
    state->hwnd = hwnd;
    state->callback_context = callback_context;
    
    // 初始化 Direct2D 工厂
    HRESULT hr = D2D1CreateFactory(
        D2D1_FACTORY_TYPE_SINGLE_THREADED,
        &state->d2d_factory
    );
    
    if (FAILED(hr)) {
        delete state;
        DestroyWindow(hwnd);
        return NULL;
    }
    
    // 获取客户区大小
    RECT rc;
    GetClientRect(hwnd, &rc);
    
    // 创建 HwndRenderTarget
    hr = state->d2d_factory->CreateHwndRenderTarget(
        D2D1::RenderTargetProperties(),
        D2D1::HwndRenderTargetProperties(
            hwnd,
            D2D1::SizeU(rc.right - rc.left, rc.bottom - rc.top)
        ),
        &state->render_target
    );
    
    if (FAILED(hr)) {
        SafeRelease(&state->d2d_factory);
        delete state;
        DestroyWindow(hwnd);
        return NULL;
    }
    
    // 创建画刷
    hr = state->render_target->CreateSolidColorBrush(
        D2D1::ColorF(D2D1::ColorF::Black),
        &state->brush
    );
    
    if (FAILED(hr)) {
        SafeRelease(&state->render_target);
        SafeRelease(&state->d2d_factory);
        delete state;
        DestroyWindow(hwnd);
        return NULL;
    }
    
    // 初始化 DirectWrite 工厂
    hr = DWriteCreateFactory(
        DWRITE_FACTORY_TYPE_SHARED,
        __uuidof(IDWriteFactory),
        reinterpret_cast<IUnknown**>(&state->dwrite_factory)
    );
    
    if (FAILED(hr)) {
        SafeRelease(&state->brush);
        SafeRelease(&state->render_target);
        SafeRelease(&state->d2d_factory);
        delete state;
        DestroyWindow(hwnd);
        return NULL;
    }
    
    // 获取窗口 DPI 并初始化 DPI 缩放
    InitializeDPI(state);
    
    // 初始化文本格式缓存
    state->text_format_cache = new TextFormatCache(state->dwrite_factory);
    
    // 创建文本格式
    RecreateTextFormat(state);
    
    if (!state->text_format) {
        if (state->text_format_cache) {
            delete state->text_format_cache;
            state->text_format_cache = nullptr;
        }
        SafeRelease(&state->dwrite_factory);
        SafeRelease(&state->brush);
        SafeRelease(&state->render_target);
        SafeRelease(&state->d2d_factory);
        delete state;
        DestroyWindow(hwnd);
        return NULL;
    }
    
    // 设置默认样式
    state->background_color = ArgbToColorF(bg_color);
    state->text_color = ArgbToColorF(text_color);
    state->selected_color = D2D1::ColorF(0.26f, 0.59f, 0.98f, 0.2f);  // Element UI 蓝色半透明
    state->hover_color = D2D1::ColorF(0.96f, 0.96f, 0.96f);           // 浅灰色
    state->border_color = D2D1::ColorF(0.9f, 0.9f, 0.9f);
    
    // 初始化尺寸参数（逻辑单位）
    state->node_height = 28.0f;
    state->indent_width = 20.0f;
    state->icon_size = 16.0f;
    state->expand_icon_size = 12.0f;
    
    // 初始化节点数据
    state->next_node_id = 1;
    state->selected_node = nullptr;
    state->hover_node = nullptr;
    state->editing_node = nullptr;
    state->edit_control = NULL;
    state->drag_node = nullptr;
    state->drag_enabled = false;
    state->is_dragging = false;
    state->drop_target_node = nullptr;
    state->drop_insert_index = -1;
    state->drop_as_child = false;
    state->need_rebuild_visible = false;
    
    // 初始化滚动状态
    state->scroll_pos = 0;
    state->scroll_max = 0;
    state->content_height = 0;
    state->show_scrollbar = false;
    
    // 初始化回调函数
    state->on_node_selected = nullptr;
    state->on_node_expanded = nullptr;
    state->on_node_collapsed = nullptr;
    state->on_node_double_click = nullptr;
    state->on_node_right_click = nullptr;
    state->on_node_text_changed = nullptr;
    state->on_node_checked = nullptr;
    state->on_node_moved = nullptr;
    
    // 存入全局 map
    g_treeview_states[hwnd] = state;
    
    return hwnd;
}

bool __stdcall DestroyTreeView(HWND hwnd) {
    // 参数验证
    if (!hwnd) {
        return false;
    }
    
    // 获取 State
    TreeViewState* state = GetTreeViewState(hwnd);
    if (!state) {
        return false;
    }
    
    // 释放所有节点内存（递归删除）
    DeleteAllNodes(state);
    
    // 如果正在编辑，退出编辑模式
    if (state->editing_node || state->edit_control) {
        ExitEditMode(state, false); // 不保存更改
    }
    
    // 释放 Direct2D 资源
    SafeRelease(&state->brush);
    SafeRelease(&state->text_format);
    SafeRelease(&state->render_target);
    SafeRelease(&state->d2d_factory);
    
    // 释放文本格式缓存
    if (state->text_format_cache) {
        delete state->text_format_cache;
        state->text_format_cache = nullptr;
    }
    
    // 释放 DirectWrite 资源
    SafeRelease(&state->dwrite_factory);
    
    // 从全局 map 移除 State
    g_treeview_states.erase(hwnd);
    
    // 释放 State 内存
    delete state;
    
    // 销毁窗口
    DestroyWindow(hwnd);
    
    return true;
}

int __stdcall AddRootNode(
    HWND hwnd,
    const unsigned char* text,
    int text_len,
    const unsigned char* icon,
    int icon_len
) {
    // 参数验证
    if (!hwnd || !text || text_len <= 0) {
        return -1;
    }
    
    // 获取 State
    TreeViewState* state = GetTreeViewState(hwnd);
    if (!state) {
        return -1;
    }
    
    // 将 UTF-8 文本转换为 UTF-16
    std::wstring text_utf16 = Utf8ToUtf16(text, text_len);
    if (text_utf16.empty()) {
        return -1;
    }
    
    // 创建新节点
    int node_id = state->next_node_id++;
    TreeNode* node = CreateNode(node_id, text_utf16);
    if (!node) {
        return -1;
    }
    
    // 设置图标（如果提供）
    if (icon && icon_len > 0) {
        std::wstring icon_utf16 = Utf8ToUtf16(icon, icon_len);
        node->icon = icon_utf16;
    }
    
    // 设置 level = 0（根节点）
    node->level = 0;
    
    // 添加到 state->root_nodes
    state->root_nodes.push_back(node);
    
    // 添加到 state->node_map
    state->node_map[node_id] = node;
    
    // 标记需要重建可见列表
    state->need_rebuild_visible = true;
    RebuildVisibleNodes(state);
    RecalculateLayout(state);
    
    // 触发重绘
    InvalidateRect(hwnd, NULL, FALSE);
    
    // 返回节点 ID
    return node_id;
}

int __stdcall AddChildNode(
    HWND hwnd,
    int parent_id,
    const unsigned char* text,
    int text_len,
    const unsigned char* icon,
    int icon_len
) {
    // 参数验证
    if (!hwnd || !text || text_len <= 0) {
        return -1;
    }
    
    // 获取 State
    TreeViewState* state = GetTreeViewState(hwnd);
    if (!state) {
        return -1;
    }
    
    // 验证父节点存在性
    TreeNode* parent = FindNodeById(state, parent_id);
    if (!parent) {
        return -1;
    }
    
    // 将 UTF-8 文本转换为 UTF-16
    std::wstring text_utf16 = Utf8ToUtf16(text, text_len);
    if (text_utf16.empty()) {
        return -1;
    }
    
    // 创建新节点
    int node_id = state->next_node_id++;
    TreeNode* node = CreateNode(node_id, text_utf16);
    if (!node) {
        return -1;
    }
    
    // 设置图标（如果提供）
    if (icon && icon_len > 0) {
        std::wstring icon_utf16 = Utf8ToUtf16(icon, icon_len);
        node->icon = icon_utf16;
    }
    
    // 设置 parent 指针和 level
    node->parent = parent;
    node->level = parent->level + 1;
    
    // 添加到父节点的 children 列表
    parent->children.push_back(node);
    
    // 添加到 state->node_map
    state->node_map[node_id] = node;
    
    // 标记需要重建可见列表
    state->need_rebuild_visible = true;
    RebuildVisibleNodes(state);
    RecalculateLayout(state);
    
    // 触发重绘
    InvalidateRect(hwnd, NULL, FALSE);
    
    // 返回节点 ID
    return node_id;
}

bool __stdcall RemoveNode(HWND hwnd, int node_id) {
    // 参数验证
    if (!hwnd) {
        return false;
    }
    
    // 获取 State
    TreeViewState* state = GetTreeViewState(hwnd);
    if (!state) {
        return false;
    }
    
    // 验证节点存在性
    TreeNode* node = FindNodeById(state, node_id);
    if (!node) {
        return false;
    }
    
    // 从父节点的 children 列表移除
    if (node->parent) {
        auto& siblings = node->parent->children;
        auto it = std::find(siblings.begin(), siblings.end(), node);
        if (it != siblings.end()) {
            siblings.erase(it);
        }
    } else {
        // 如果是根节点，从 root_nodes 移除
        auto& roots = state->root_nodes;
        auto it = std::find(roots.begin(), roots.end(), node);
        if (it != roots.end()) {
            roots.erase(it);
        }
    }
    
    // 从 state->node_map 移除（递归移除所有子节点）
    std::vector<TreeNode*> nodes_to_remove;
    nodes_to_remove.push_back(node);
    
    // 收集所有需要移除的节点（包括子节点）
    for (size_t i = 0; i < nodes_to_remove.size(); ++i) {
        TreeNode* current = nodes_to_remove[i];
        for (TreeNode* child : current->children) {
            nodes_to_remove.push_back(child);
        }
    }
    
    // 从 node_map 移除所有节点
    for (TreeNode* n : nodes_to_remove) {
        state->node_map.erase(n->id);
        
        // 如果是选中节点，清除选中状态
        if (state->selected_node == n) {
            state->selected_node = nullptr;
        }
        
        // 如果是悬停节点，清除悬停状态
        if (state->hover_node == n) {
            state->hover_node = nullptr;
        }
        
        // 如果是编辑节点，清除编辑状态
        if (state->editing_node == n) {
            state->editing_node = nullptr;
        }
        
        // 如果是拖动节点，清除拖动状态
        if (state->drag_node == n) {
            state->drag_node = nullptr;
        }
    }
    
    // 调用 DeleteNode 递归删除
    DeleteNode(node);
    
    // 标记需要重建可见列表
    state->need_rebuild_visible = true;
    RebuildVisibleNodes(state);
    RecalculateLayout(state);
    
    // 触发重绘
    InvalidateRect(hwnd, NULL, FALSE);
    
    return true;
}

bool __stdcall ClearTree(HWND hwnd) {
    // 参数验证
    if (!hwnd) {
        return false;
    }
    
    // 获取 State
    TreeViewState* state = GetTreeViewState(hwnd);
    if (!state) {
        return false;
    }
    
    // 遍历所有根节点调用 DeleteNode
    for (TreeNode* root : state->root_nodes) {
        DeleteNode(root);
    }
    
    // 清空 root_nodes, node_map, visible_nodes
    state->root_nodes.clear();
    state->node_map.clear();
    state->visible_nodes.clear();
    
    // 重置 selected_node, hover_node
    state->selected_node = nullptr;
    state->hover_node = nullptr;
    state->editing_node = nullptr;
    state->drag_node = nullptr;
    
    // 重置滚动状态
    state->scroll_pos = 0;
    state->scroll_max = 0;
    state->content_height = 0;
    state->show_scrollbar = false;
    
    // 标记需要重建可见列表
    state->need_rebuild_visible = false;  // 已经清空，不需要重建
    
    // 触发重绘
    InvalidateRect(hwnd, NULL, FALSE);
    
    return true;
}

bool __stdcall ExpandNode(HWND hwnd, int node_id) {
    // 参数验证
    if (!hwnd) {
        return false;
    }
    
    // 获取 State
    TreeViewState* state = GetTreeViewState(hwnd);
    if (!state) {
        return false;
    }
    
    // 验证节点存在性
    TreeNode* node = FindNodeById(state, node_id);
    if (!node) {
        return false;
    }
    
    // 如果节点已经展开，直接返回成功
    if (node->expanded) {
        return true;
    }
    
    // 设置 node->expanded = true
    node->expanded = true;
    
    // 调用 RebuildVisibleNodes
    RebuildVisibleNodes(state);
    
    // 调用 RecalculateLayout
    RecalculateLayout(state);
    
    // 触发 on_node_expanded 回调
    if (state->on_node_expanded) {
        state->on_node_expanded(node_id, state->callback_context);
    }
    
    // 触发重绘
    InvalidateRect(hwnd, NULL, FALSE);
    
    return true;
}

bool __stdcall CollapseNode(HWND hwnd, int node_id) {
    // 参数验证
    if (!hwnd) {
        return false;
    }
    
    // 获取 State
    TreeViewState* state = GetTreeViewState(hwnd);
    if (!state) {
        return false;
    }
    
    // 验证节点存在性
    TreeNode* node = FindNodeById(state, node_id);
    if (!node) {
        return false;
    }
    
    // 如果节点已经折叠，直接返回成功
    if (!node->expanded) {
        return true;
    }
    
    // 设置 node->expanded = false
    node->expanded = false;
    
    // 调用 RebuildVisibleNodes
    RebuildVisibleNodes(state);
    
    // 调用 RecalculateLayout
    RecalculateLayout(state);
    
    // 触发 on_node_collapsed 回调
    if (state->on_node_collapsed) {
        state->on_node_collapsed(node_id, state->callback_context);
    }
    
    // 触发重绘
    InvalidateRect(hwnd, NULL, FALSE);
    
    return true;
}

bool __stdcall ExpandAll(HWND hwnd) {
    // 参数验证
    if (!hwnd) {
        return false;
    }
    
    // 获取 State
    TreeViewState* state = GetTreeViewState(hwnd);
    if (!state) {
        return false;
    }
    
    // 递归遍历所有节点设置 expanded 状态
    for (TreeNode* root : state->root_nodes) {
        ExpandAllNodesRecursive(root);
    }
    
    // 调用 RebuildVisibleNodes 和 RecalculateLayout
    RebuildVisibleNodes(state);
    RecalculateLayout(state);
    
    // 触发重绘
    InvalidateRect(hwnd, NULL, FALSE);
    
    return true;
}

bool __stdcall CollapseAll(HWND hwnd) {
    // 参数验证
    if (!hwnd) {
        return false;
    }
    
    // 获取 State
    TreeViewState* state = GetTreeViewState(hwnd);
    if (!state) {
        return false;
    }
    
    // 递归遍历所有节点设置 expanded 状态
    for (TreeNode* root : state->root_nodes) {
        CollapseAllNodesRecursive(root);
    }
    
    // 调用 RebuildVisibleNodes 和 RecalculateLayout
    RebuildVisibleNodes(state);
    RecalculateLayout(state);
    
    // 触发重绘
    InvalidateRect(hwnd, NULL, FALSE);
    
    return true;
}

bool __stdcall IsNodeExpanded(HWND hwnd, int node_id) {
    // 参数验证
    if (!hwnd) {
        return false;
    }
    
    // 获取 State
    TreeViewState* state = GetTreeViewState(hwnd);
    if (!state) {
        return false;
    }
    
    // 验证节点存在性
    TreeNode* node = FindNodeById(state, node_id);
    if (!node) {
        return false;
    }
    
    // 返回 node->expanded 状态
    return node->expanded;
}

bool __stdcall SetSelectedNode(HWND hwnd, int node_id) {
    // 参数验证
    if (!hwnd) {
        return false;
    }
    
    // 获取 State
    TreeViewState* state = GetTreeViewState(hwnd);
    if (!state) {
        return false;
    }
    
    // 验证节点存在性
    TreeNode* node = FindNodeById(state, node_id);
    if (!node) {
        return false;
    }
    
    // 如果节点已经被选中，直接返回成功
    if (state->selected_node == node) {
        return true;
    }
    
    // 取消之前选中节点的高亮
    if (state->selected_node) {
        state->selected_node->selected = false;
    }
    
    // 设置 state->selected_node
    state->selected_node = node;
    node->selected = true;
    
    // 触发 on_node_selected 回调
    if (state->on_node_selected) {
        state->on_node_selected(node_id, state->callback_context);
    }
    
    // 触发重绘
    InvalidateRect(hwnd, NULL, FALSE);
    
    return true;
}

int __stdcall GetSelectedNode(HWND hwnd) {
    // 参数验证
    if (!hwnd) {
        return -1;
    }
    
    // 获取 State
    TreeViewState* state = GetTreeViewState(hwnd);
    if (!state) {
        return -1;
    }
    
    // 返回 state->selected_node 的 ID
    if (state->selected_node) {
        return state->selected_node->id;
    }
    
    // 如果没有选中返回 -1
    return -1;
}

bool __stdcall SetNodeText(
    HWND hwnd,
    int node_id,
    const unsigned char* text,
    int text_len
) {
    // 参数验证
    if (!hwnd || !text || text_len <= 0) {
        return false;
    }
    
    // 获取 State
    TreeViewState* state = GetTreeViewState(hwnd);
    if (!state) {
        return false;
    }
    
    // 验证节点存在性
    TreeNode* node = FindNodeById(state, node_id);
    if (!node) {
        return false;
    }
    
    // UTF-8 转 UTF-16
    std::wstring new_text = Utf8ToUtf16(text, text_len);
    if (new_text.empty()) {
        return false;
    }
    
    // 保存旧文本用于回调
    std::wstring old_text = node->text;
    
    // 设置 node->text
    node->text = new_text;
    
    // 触发 on_node_text_changed 回调
    if (state->on_node_text_changed) {
        state->on_node_text_changed(node_id, text, text_len, state->callback_context);
    }
    
    // 触发重绘
    InvalidateRect(hwnd, NULL, FALSE);
    
    return true;
}

int __stdcall GetNodeText(
    HWND hwnd,
    int node_id,
    unsigned char* buffer,
    int buffer_size
) {
    // 参数验证
    if (!hwnd) {
        return -1;
    }
    
    // 获取 State
    TreeViewState* state = GetTreeViewState(hwnd);
    if (!state) {
        return -1;
    }
    
    // 验证节点存在性
    TreeNode* node = FindNodeById(state, node_id);
    if (!node) {
        return -1;
    }
    
    // UTF-16 转 UTF-8
    std::string text_utf8 = Utf16ToUtf8(node->text);
    int text_byte_len = static_cast<int>(text_utf8.length());
    
    // 如果只是查询长度，直接返回
    if (!buffer || buffer_size <= 0) {
        return text_byte_len;
    }
    
    // 复制到缓冲区
    int copy_len = (std::min)(text_byte_len, buffer_size - 1); // 预留一个字节给 null 终止符
    if (copy_len > 0) {
        memcpy(buffer, text_utf8.c_str(), copy_len);
    }
    
    // 添加 null 终止符
    if (buffer_size > 0) {
        buffer[copy_len] = 0;
    }
    
    // 返回字节长度
    return text_byte_len;
}

bool __stdcall SetNodeIcon(
    HWND hwnd,
    int node_id,
    const unsigned char* icon,
    int icon_len
) {
    // 参数验证
    if (!hwnd) {
        return false;
    }
    
    // 获取 State
    TreeViewState* state = GetTreeViewState(hwnd);
    if (!state) {
        return false;
    }
    
    // 验证节点存在性
    TreeNode* node = FindNodeById(state, node_id);
    if (!node) {
        return false;
    }
    
    // 将 UTF-8 图标转换为 UTF-16
    if (icon && icon_len > 0) {
        std::wstring icon_utf16 = Utf8ToUtf16(icon, icon_len);
        node->icon = icon_utf16;
    } else {
        // 如果图标为空，清除图标
        node->icon.clear();
    }
    
    // 触发重绘
    InvalidateRect(hwnd, NULL, FALSE);
    
    return true;
}

int __stdcall GetNodeIcon(
    HWND hwnd,
    int node_id,
    unsigned char* buffer,
    int buffer_size
) {
    // 参数验证
    if (!hwnd) {
        return -1;
    }
    
    // 获取 State
    TreeViewState* state = GetTreeViewState(hwnd);
    if (!state) {
        return -1;
    }
    
    // 验证节点存在性
    TreeNode* node = FindNodeById(state, node_id);
    if (!node) {
        return -1;
    }
    
    // 将 UTF-16 图标转换为 UTF-8
    std::string icon_utf8 = Utf16ToUtf8(node->icon);
    int icon_byte_len = static_cast<int>(icon_utf8.length());
    
    // 如果只是查询长度，直接返回
    if (!buffer || buffer_size <= 0) {
        return icon_byte_len;
    }
    
    // 复制到缓冲区
    int copy_len = (std::min)(icon_byte_len, buffer_size - 1); // 预留一个字节给 null 终止符
    if (copy_len > 0) {
        memcpy(buffer, icon_utf8.c_str(), copy_len);
    }
    
    // 添加 null 终止符
    if (buffer_size > 0) {
        buffer[copy_len] = 0;
    }
    
    // 返回字节长度
    return icon_byte_len;
}

bool __stdcall SetNodeForeColor(HWND hwnd, int node_id, unsigned int color) {
    // 参数验证
    if (!hwnd) {
        return false;
    }
    
    // 获取 State
    TreeViewState* state = GetTreeViewState(hwnd);
    if (!state) {
        return false;
    }
    
    // 验证节点存在性
    TreeNode* node = FindNodeById(state, node_id);
    if (!node) {
        return false;
    }
    
    // 将 ARGB 转换为 D2D1_COLOR_F
    node->fore_color = ArgbToColorF(color);
    
    // 触发重绘
    InvalidateRect(hwnd, NULL, FALSE);
    
    return true;
}

bool __stdcall SetNodeBackColor(HWND hwnd, int node_id, unsigned int color) {
    // 参数验证
    if (!hwnd) {
        return false;
    }
    
    // 获取 State
    TreeViewState* state = GetTreeViewState(hwnd);
    if (!state) {
        return false;
    }
    
    // 验证节点存在性
    TreeNode* node = FindNodeById(state, node_id);
    if (!node) {
        return false;
    }
    
    // 将 ARGB 转换为 D2D1_COLOR_F
    node->back_color = ArgbToColorF(color);
    
    // 触发重绘
    InvalidateRect(hwnd, NULL, FALSE);
    
    return true;
}

bool __stdcall SetNodeEnabled(HWND hwnd, int node_id, bool enabled) {
    // 参数验证
    if (!hwnd) {
        return false;
    }
    
    // 获取 State
    TreeViewState* state = GetTreeViewState(hwnd);
    if (!state) {
        return false;
    }
    
    // 验证节点存在性
    TreeNode* node = FindNodeById(state, node_id);
    if (!node) {
        return false;
    }
    
    // 设置 node->enabled
    node->enabled = enabled;
    
    // 触发重绘（灰色样式）
    InvalidateRect(hwnd, NULL, FALSE);
    
    return true;
}

bool __stdcall IsNodeEnabled(HWND hwnd, int node_id) {
    // 参数验证
    if (!hwnd) {
        return false;
    }
    
    // 获取 State
    TreeViewState* state = GetTreeViewState(hwnd);
    if (!state) {
        return false;
    }
    
    // 验证节点存在性
    TreeNode* node = FindNodeById(state, node_id);
    if (!node) {
        return false;
    }
    
    // 获取 node->enabled
    return node->enabled;
}

bool __stdcall SetNodeCheckBox(HWND hwnd, int node_id, bool show) {
    // 参数验证
    if (!hwnd) {
        return false;
    }
    
    // 获取 State
    TreeViewState* state = GetTreeViewState(hwnd);
    if (!state) {
        return false;
    }
    
    // 验证节点存在性
    TreeNode* node = FindNodeById(state, node_id);
    if (!node) {
        return false;
    }
    
    // 调试输出：显示设置前的状态
    OutputDebugStringA(("SetNodeCheckBox: node_id=" + std::to_string(node_id) + 
                        ", show=" + std::to_string(show) + 
                        ", checked_before=" + std::to_string(node->checked) + "\n").c_str());
    
    // 设置 node->has_checkbox
    node->has_checkbox = show;
    
    // 当显示复选框时，确保 checked 状态为 false（未选中）
    if (show) {
        node->checked = false;
    }
    
    // 调试输出：显示设置后的状态
    OutputDebugStringA(("SetNodeCheckBox: checked_after=" + std::to_string(node->checked) + "\n").c_str());
    
    // 触发重绘
    InvalidateRect(hwnd, NULL, FALSE);
    
    return true;
}

bool __stdcall SetNodeChecked(HWND hwnd, int node_id, bool checked) {
    // 参数验证
    if (!hwnd) {
        return false;
    }
    
    // 获取 State
    TreeViewState* state = GetTreeViewState(hwnd);
    if (!state) {
        return false;
    }
    
    // 验证节点存在性
    TreeNode* node = FindNodeById(state, node_id);
    if (!node) {
        return false;
    }
    
    // 设置 node->checked
    node->checked = checked;
    
    // 触发 on_node_checked 回调
    if (state->on_node_checked) {
        state->on_node_checked(node_id, checked, state->callback_context);
    }
    
    // 触发重绘
    InvalidateRect(hwnd, NULL, FALSE);
    
    return true;
}

BOOL __stdcall GetNodeChecked(HWND hwnd, int node_id) {
    // 参数验证
    if (!hwnd) {
        OutputDebugStringA("GetNodeChecked: hwnd is NULL\n");
        return FALSE;
    }
    
    // 获取 State
    TreeViewState* state = GetTreeViewState(hwnd);
    if (!state) {
        OutputDebugStringA("GetNodeChecked: state is NULL\n");
        return FALSE;
    }
    
    // 验证节点存在性
    TreeNode* node = FindNodeById(state, node_id);
    if (!node) {
        OutputDebugStringA(("GetNodeChecked: node not found, node_id=" + std::to_string(node_id) + "\n").c_str());
        return FALSE;
    }
    
    // 调试输出
    OutputDebugStringA(("GetNodeChecked: node_id=" + std::to_string(node_id) + 
                        ", has_checkbox=" + std::to_string(node->has_checkbox) + 
                        ", checked=" + std::to_string(node->checked) + "\n").c_str());
    
    // 获取 node->checked，返回 BOOL 类型（4字节整数）
    return node->checked ? TRUE : FALSE;
}

int __stdcall FindNodeByText(
    HWND hwnd,
    const unsigned char* text,
    int text_len
) {
    // 参数验证
    if (!hwnd || !text || text_len <= 0) {
        return -1;
    }
    
    TreeViewState* state = GetTreeViewState(hwnd);
    if (!state) {
        return -1;
    }
    
    // 将 UTF-8 文本转换为 UTF-16
    std::wstring search_text = Utf8ToUtf16(text, text_len);
    if (search_text.empty()) {
        return -1;
    }
    
    // 递归搜索所有节点
    for (TreeNode* root : state->root_nodes) {
        int result = FindNodeByTextRecursive(root, search_text);
        if (result != -1) {
            return result;
        }
    }
    
    return -1; // 未找到
}

int __stdcall GetNodeParent(HWND hwnd, int node_id) {
    // 参数验证
    if (!hwnd) {
        return -1;
    }
    
    TreeViewState* state = GetTreeViewState(hwnd);
    if (!state) {
        return -1;
    }
    
    // 验证节点存在性
    TreeNode* node = FindNodeById(state, node_id);
    if (!node) {
        return -1;
    }
    
    // 返回父节点 ID，如果是根节点则返回 -1
    if (node->parent) {
        return node->parent->id;
    }
    
    return -1; // 根节点没有父节点
}

int __stdcall GetNodeChildCount(HWND hwnd, int node_id) {
    // 参数验证
    if (!hwnd) {
        return -1;
    }
    
    TreeViewState* state = GetTreeViewState(hwnd);
    if (!state) {
        return -1;
    }
    
    // 验证节点存在性
    TreeNode* node = FindNodeById(state, node_id);
    if (!node) {
        return -1;
    }
    
    // 返回子节点数量
    return static_cast<int>(node->children.size());
}

int __stdcall GetNodeChildren(
    HWND hwnd,
    int node_id,
    int* buffer,
    int buffer_size
) {
    // 参数验证
    if (!hwnd || !buffer || buffer_size <= 0) {
        return -1;
    }
    
    TreeViewState* state = GetTreeViewState(hwnd);
    if (!state) {
        return -1;
    }
    
    // 验证节点存在性
    TreeNode* node = FindNodeById(state, node_id);
    if (!node) {
        return -1;
    }
    
    // 获取子节点数量
    int child_count = static_cast<int>(node->children.size());
    
    // 复制子节点 ID 到缓冲区
    int copy_count = min(child_count, buffer_size);
    for (int i = 0; i < copy_count; i++) {
        buffer[i] = node->children[i]->id;
    }
    
    // 返回实际子节点数量
    return child_count;
}

int __stdcall GetNodeLevel(HWND hwnd, int node_id) {
    // 参数验证
    if (!hwnd) {
        return -1;
    }
    
    TreeViewState* state = GetTreeViewState(hwnd);
    if (!state) {
        return -1;
    }
    
    // 验证节点存在性
    TreeNode* node = FindNodeById(state, node_id);
    if (!node) {
        return -1;
    }
    
    // 返回节点层级深度（根节点为 0）
    return node->level;
}

bool __stdcall ScrollToNode(HWND hwnd, int node_id) {
    // 参数验证
    if (!hwnd) {
        return false;
    }
    
    // 获取 State
    TreeViewState* state = GetTreeViewState(hwnd);
    if (!state) {
        return false;
    }
    
    // 验证节点存在性
    TreeNode* node = FindNodeById(state, node_id);
    if (!node) {
        return false;
    }
    
    // 计算节点的 y_position（确保布局是最新的）
    if (state->need_rebuild_visible) {
        RebuildVisibleNodes(state);
        RecalculateLayout(state);
    }
    
    // 调整 scroll_pos 使节点可见
    ScrollToNodeInternal(state, node);
    
    // 触发重绘
    InvalidateRect(hwnd, NULL, FALSE);
    
    return true;
}

bool __stdcall SetTreeViewScrollPos(HWND hwnd, int pos) {
    // 参数验证
    if (!hwnd) {
        return false;
    }
    
    // 获取 State
    TreeViewState* state = GetTreeViewState(hwnd);
    if (!state) {
        return false;
    }
    
    // 设置 scroll_pos
    state->scroll_pos = pos;
    
    // 限制范围
    state->scroll_pos = (std::max)(0, (std::min)(state->scroll_pos, state->scroll_max));
    
    // 触发重绘
    InvalidateRect(hwnd, NULL, FALSE);
    
    return true;
}

int __stdcall GetTreeViewScrollPos(HWND hwnd) {
    // 参数验证
    if (!hwnd) {
        return -1;
    }
    
    // 获取 State
    TreeViewState* state = GetTreeViewState(hwnd);
    if (!state) {
        return -1;
    }
    
    // 获取 scroll_pos
    return state->scroll_pos;
}

bool __stdcall SetTreeViewCallback(
    HWND hwnd,
    int callback_type,
    void* callback_func
) {
    // 参数验证
    if (!hwnd) {
        return false;
    }
    
    // 获取 State
    TreeViewState* state = GetTreeViewState(hwnd);
    if (!state) {
        return false;
    }
    
    // 验证 callback_type 并设置对应的回调函数指针
    switch (callback_type) {
        case CALLBACK_NODE_SELECTED:
            state->on_node_selected = reinterpret_cast<TreeViewCallback>(callback_func);
            break;
            
        case CALLBACK_NODE_EXPANDED:
            state->on_node_expanded = reinterpret_cast<TreeViewCallback>(callback_func);
            break;
            
        case CALLBACK_NODE_COLLAPSED:
            state->on_node_collapsed = reinterpret_cast<TreeViewCallback>(callback_func);
            break;
            
        case CALLBACK_NODE_DOUBLE_CLICK:
            state->on_node_double_click = reinterpret_cast<TreeViewCallback>(callback_func);
            break;
            
        case CALLBACK_NODE_RIGHT_CLICK:
            state->on_node_right_click = reinterpret_cast<TreeViewRightClickCallback>(callback_func);
            break;
            
        case CALLBACK_NODE_TEXT_CHANGED:
            state->on_node_text_changed = reinterpret_cast<TreeViewTextCallback>(callback_func);
            break;
            
        case CALLBACK_NODE_CHECKED:
            state->on_node_checked = reinterpret_cast<TreeViewCheckCallback>(callback_func);
            break;
            
        case CALLBACK_NODE_MOVED:
            state->on_node_moved = reinterpret_cast<TreeViewMoveCallback>(callback_func);
            break;
            
        default:
            // 不支持的回调类型
            return false;
    }
    
    return true;
}

bool __stdcall EnableTreeViewDragDrop(HWND hwnd, bool enable) {
    // 参数验证
    if (!hwnd) {
        return false;
    }
    
    // 获取 State
    TreeViewState* state = GetTreeViewState(hwnd);
    if (!state) {
        return false;
    }
    
    // 设置 state->drag_enabled
    state->drag_enabled = enable;
    
    return true;
}

// ============================================================================
// 窗口过程（基本框架）
// ============================================================================

LRESULT CALLBACK TreeViewWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    TreeViewState* state = GetTreeViewState(hwnd);
    
    switch (msg) {
        case WM_CREATE:
            // 窗口创建时的初始化已在 CreateTreeView 中完成
            return 0;
            
        case WM_DESTROY:
            // 清理工作已在 DestroyTreeView 中完成
            // 这里只需要确保从 map 中移除（如果还存在）
            if (state) {
                g_treeview_states.erase(hwnd);
            }
            return 0;
            
        case WM_PAINT:
            // 调用 RenderTreeView
            if (state) {
                PAINTSTRUCT ps;
                BeginPaint(hwnd, &ps);
                RenderTreeView(state);
                EndPaint(hwnd, &ps);
            }
            return 0;
            
        case WM_SIZE:
            // 调整渲染目标大小
            if (state && state->render_target) {
                UINT width = LOWORD(lParam);
                UINT height = HIWORD(lParam);
                
                state->render_target->Resize(D2D1::SizeU(width, height));
                
                // 重新计算布局
                RecalculateLayout(state);
                
                // 触发重绘
                InvalidateRect(hwnd, NULL, FALSE);
            }
            return 0;
            
        case WM_DPICHANGED:
            // DPI 改变时的处理
            if (state) {
                // 更新 DPI 信息
                state->dpi = HIWORD(wParam);
                state->dpi_scale = state->dpi / 96.0f;
                
                // 重新缩放尺寸（调用 InitializeDPI）
                InitializeDPI(state);
                
                // 重新创建文本格式（字体大小需要缩放）
                RecreateTextFormat(state);
                
                // 重新构建可见节点列表
                RebuildVisibleNodes(state);
                
                // 重新计算布局
                RecalculateLayout(state);
                
                // 调整窗口大小（使用系统建议的新矩形）
                RECT* new_rect = (RECT*)lParam;
                SetWindowPos(hwnd, NULL,
                    new_rect->left, new_rect->top,
                    new_rect->right - new_rect->left,
                    new_rect->bottom - new_rect->top,
                    SWP_NOZORDER | SWP_NOACTIVATE);
                
                // 触发重绘
                InvalidateRect(hwnd, NULL, FALSE);
            }
            return 0;
            
        case WM_LBUTTONDOWN:
            // 鼠标左键按下处理
            if (state) {
                // 设置焦点以接收键盘消息
                SetFocus(hwnd);
                
                int x = GET_X_LPARAM(lParam);
                int y = GET_Y_LPARAM(lParam);
                
                // 如果正在自定义编辑模式，检查是否点击在编辑框外部
                if (state->custom_editing && state->editing_node) {
                    // 检查点击位置是否在编辑框内
                    if (x < state->edit_rect.left || x > state->edit_rect.right ||
                        y < state->edit_rect.top || y > state->edit_rect.bottom) {
                        // 点击在编辑框外部，保存并退出编辑模式
                        ExitEditMode(state, true);
                    }
                    return 0;
                }
                
                // 调用 HitTestNode 查找点击的节点
                TreeNode* clicked_node = HitTestNode(state, x, y);
                
                if (clicked_node && clicked_node->enabled) {
                    // 计算节点的基础位置
                    float x_base = clicked_node->level * state->indent_width * state->dpi_scale;
                    float x_cursor = x_base;
                    
                    // 检测是否点击展开图标（切换展开/折叠）
                    if (!clicked_node->children.empty()) {
                        float expand_icon_x = x_cursor + 4 * state->dpi_scale;
                        float expand_icon_width = (state->expand_icon_size + 8) * state->dpi_scale;
                        
                        if (x >= expand_icon_x && x < expand_icon_x + expand_icon_width) {
                            // 切换展开/折叠状态
                            if (clicked_node->expanded) {
                                CollapseNode(hwnd, clicked_node->id);
                            } else {
                                ExpandNode(hwnd, clicked_node->id);
                            }
                            return 0;
                        }
                    }
                    
                    // 更新 x_cursor 位置
                    x_cursor += (state->expand_icon_size + 8) * state->dpi_scale;
                    
                    // 检测是否点击复选框（切换选中状态）
                    if (clicked_node->has_checkbox) {
                        float checkbox_size = 16 * state->dpi_scale;
                        float checkbox_x = x_cursor;
                        
                        if (x >= checkbox_x && x < checkbox_x + checkbox_size) {
                            // 切换复选框状态
                            clicked_node->checked = !clicked_node->checked;
                            
                            // 触发 on_node_checked 回调
                            if (state->on_node_checked) {
                                state->on_node_checked(clicked_node->id, clicked_node->checked, state->callback_context);
                            }
                            
                            InvalidateRect(hwnd, NULL, FALSE);
                            return 0;
                        }
                    }
                    
                    // 否则选中节点
                    if (state->selected_node != clicked_node) {
                        SetSelectedNode(hwnd, clicked_node->id);
                    }
                    
                    // 如果启用拖放则开始拖放操作
                    if (state->drag_enabled) {
                        state->drag_node = clicked_node;
                        state->drag_start_pos.x = x;
                        state->drag_start_pos.y = y;
                        state->is_dragging = false;  // 还没开始拖动，只是记录起始位置
                        SetCapture(hwnd);
                    }
                }
            }
            return 0;
            
        case WM_LBUTTONUP:
            // 鼠标左键释放处理
            if (state) {
                // 如果正在拖放，结束拖放操作
                if (state->drag_node && GetCapture() == hwnd) {
                    ReleaseCapture();
                    
                    if (state->is_dragging && state->drop_target_node != nullptr) {
                        // 执行节点移动
                        TreeNode* drag_node = state->drag_node;
                        TreeNode* target_node = state->drop_target_node;
                        int insert_index = state->drop_insert_index;
                        bool as_child = state->drop_as_child;
                        
                        // 计算新的父节点和父节点ID（用于回调）
                        TreeNode* new_parent = nullptr;
                        int new_parent_id = -1;
                        
                        if (as_child) {
                            // 作为子节点插入
                            new_parent = target_node;
                            new_parent_id = target_node->id;
                        } else {
                            // 作为兄弟节点插入
                            new_parent = target_node; // target_node实际上是父节点
                            new_parent_id = target_node ? target_node->id : -1;
                        }
                        
                        // 移动节点
                        if (MoveNodeToPosition(state, drag_node, new_parent, insert_index)) {
                            // 重建可见节点列表和重新计算布局
                            RebuildVisibleNodes(state);
                            RecalculateLayout(state);
                            
                            // 触发 on_node_moved 回调
                            if (state->on_node_moved) {
                                state->on_node_moved(
                                    drag_node->id,
                                    new_parent_id,
                                    insert_index,
                                    state->callback_context
                                );
                            }
                        }
                    }
                    
                    // 清理拖放状态
                    state->is_dragging = false;
                    state->drag_node = nullptr;
                    state->drop_target_node = nullptr;
                    state->drop_insert_index = -1;
                    state->drop_as_child = false;
                    
                    // 触发重绘
                    InvalidateRect(hwnd, NULL, FALSE);
                }
            }
            return 0;
            
        case WM_LBUTTONDBLCLK:
            // 鼠标左键双击处理
            if (state) {
                int x = GET_X_LPARAM(lParam);
                int y = GET_Y_LPARAM(lParam);
                
                // 调用 HitTestNode 查找双击的节点
                TreeNode* double_clicked_node = HitTestNode(state, x, y);
                
                if (double_clicked_node && double_clicked_node->enabled) {
                    // 触发 on_node_double_click 回调
                    if (state->on_node_double_click) {
                        state->on_node_double_click(double_clicked_node->id, state->callback_context);
                    }
                    
                    // 进入编辑模式
                    EnterEditMode(state, double_clicked_node);
                }
            }
            return 0;
            
        case WM_RBUTTONDOWN:
            // 鼠标右键按下处理
            if (state) {
                int x = GET_X_LPARAM(lParam);
                int y = GET_Y_LPARAM(lParam);
                
                // 调用 HitTestNode 查找右键点击的节点
                TreeNode* right_clicked_node = HitTestNode(state, x, y);
                
                if (right_clicked_node) {
                    // 触发 on_node_right_click 回调（传递坐标）
                    if (state->on_node_right_click) {
                        state->on_node_right_click(right_clicked_node->id, x, y, state->callback_context);
                    }
                }
            }
            return 0;
            
        case WM_MOUSEMOVE:
            // 鼠标移动处理
            if (state) {
                int x = GET_X_LPARAM(lParam);
                int y = GET_Y_LPARAM(lParam);
                
                // 调用 HitTestNode 查找悬停的节点
                TreeNode* hover_node = HitTestNode(state, x, y);
                
                // 更新 state->hover_node
                if (state->hover_node != hover_node) {
                    state->hover_node = hover_node;
                    
                    // 触发重绘（悬停效果）
                    InvalidateRect(hwnd, NULL, FALSE);
                }
                
                // 如果正在拖放则更新拖放预览
                if (state->drag_node && GetCapture() == hwnd) {
                    // 检查是否开始拖动（鼠标移动超过一定距离）
                    if (!state->is_dragging) {
                        int dx = x - state->drag_start_pos.x;
                        int dy = y - state->drag_start_pos.y;
                        int drag_threshold = 5; // 拖动阈值（像素）
                        
                        if (abs(dx) > drag_threshold || abs(dy) > drag_threshold) {
                            state->is_dragging = true;
                        }
                    }
                    
                    if (state->is_dragging) {
                        // 计算拖放目标
                        TreeNode* target_node = nullptr;
                        int insert_index = -1;
                        bool as_child = false;
                        
                        if (CalculateDropTarget(state, x, y, &target_node, &insert_index, &as_child)) {
                            // 更新拖放目标信息
                            state->drop_target_node = target_node;
                            state->drop_insert_index = insert_index;
                            state->drop_as_child = as_child;
                        } else {
                            // 清除拖放目标
                            state->drop_target_node = nullptr;
                            state->drop_insert_index = -1;
                            state->drop_as_child = false;
                        }
                        
                        // 触发重绘以显示拖放预览和指示线
                        InvalidateRect(hwnd, NULL, FALSE);
                    }
                }
                
                // 确保鼠标离开消息能够被接收
                TRACKMOUSEEVENT tme = {};
                tme.cbSize = sizeof(TRACKMOUSEEVENT);
                tme.dwFlags = TME_LEAVE;
                tme.hwndTrack = hwnd;
                TrackMouseEvent(&tme);
            }
            return 0;
            
        case WM_MOUSELEAVE:
            // 鼠标离开处理
            if (state) {
                // 清除 hover_node
                if (state->hover_node) {
                    state->hover_node = nullptr;
                    
                    // 触发重绘
                    InvalidateRect(hwnd, NULL, FALSE);
                }
            }
            return 0;
            
        case WM_MOUSEWHEEL:
            // 鼠标滚轮处理
            if (state) {
                // 获取滚轮增量
                int delta = GET_WHEEL_DELTA_WPARAM(wParam);
                
                // 计算滚动步长（每次滚动 3 行）
                float scroll_step = (state->node_height * state->dpi_scale) * 3;
                
                // 根据滚轮增量调整 scroll_pos
                if (delta > 0) {
                    // 向上滚动
                    state->scroll_pos -= static_cast<int>(scroll_step);
                } else {
                    // 向下滚动
                    state->scroll_pos += static_cast<int>(scroll_step);
                }
                
                // 限制 scroll_pos 在 [0, scroll_max] 范围内
                state->scroll_pos = (std::max)(0, (std::min)(state->scroll_pos, state->scroll_max));
                
                // 触发重绘
                InvalidateRect(hwnd, NULL, FALSE);
            }
            return 0;
            
        case WM_COMMAND:
            // 处理子控件消息
            if (state) {
                HWND control = (HWND)lParam;
                WORD notification = HIWORD(wParam);
                
                // 检查是否是编辑框的消息
                if (control == state->edit_control) {
                    switch (notification) {
                        case EN_KILLFOCUS:
                            // 编辑框失去焦点，保存编辑
                            ExitEditMode(state, true);
                            break;
                    }
                }
            }
            return 0;
            
        case WM_KEYDOWN:
            // 键盘导航处理
            if (state) {
                // 如果正在自定义编辑模式，处理编辑相关的按键
                if (state->custom_editing && state->editing_node) {
                    switch (wParam) {
                        case VK_RETURN:
                            // Enter 键：保存编辑并退出
                            ExitEditMode(state, true);
                            return 0;
                            
                        case VK_ESCAPE:
                            // Esc 键：取消编辑并退出
                            ExitEditMode(state, false);
                            return 0;
                            
                        case VK_BACK:
                            // 退格键：删除选中文本或光标前的字符
                            if (state->edit_selection_start != state->edit_selection_end) {
                                // 删除选中文本
                                int start = (std::min)(state->edit_selection_start, state->edit_selection_end);
                                int end = (std::max)(state->edit_selection_start, state->edit_selection_end);
                                state->edit_text.erase(start, end - start);
                                state->edit_cursor_pos = start;
                                state->edit_selection_start = start;
                                state->edit_selection_end = start;
                            } else if (state->edit_cursor_pos > 0) {
                                // 删除光标前的字符
                                state->edit_text.erase(state->edit_cursor_pos - 1, 1);
                                state->edit_cursor_pos--;
                                state->edit_selection_start = state->edit_cursor_pos;
                                state->edit_selection_end = state->edit_cursor_pos;
                            }
                            InvalidateRect(hwnd, NULL, FALSE);
                            return 0;
                            
                        case VK_DELETE:
                            // Delete 键：删除选中文本或光标后的字符
                            if (state->edit_selection_start != state->edit_selection_end) {
                                // 删除选中文本
                                int start = (std::min)(state->edit_selection_start, state->edit_selection_end);
                                int end = (std::max)(state->edit_selection_start, state->edit_selection_end);
                                state->edit_text.erase(start, end - start);
                                state->edit_cursor_pos = start;
                                state->edit_selection_start = start;
                                state->edit_selection_end = start;
                            } else if (state->edit_cursor_pos < static_cast<int>(state->edit_text.length())) {
                                // 删除光标后的字符
                                state->edit_text.erase(state->edit_cursor_pos, 1);
                            }
                            InvalidateRect(hwnd, NULL, FALSE);
                            return 0;
                            
                        case VK_LEFT:
                            // 左箭头：移动光标
                            if (state->edit_selection_start != state->edit_selection_end) {
                                // 如果有选中文本，将光标移动到选中区域的开始位置
                                state->edit_cursor_pos = (std::min)(state->edit_selection_start, state->edit_selection_end);
                                state->edit_selection_start = state->edit_cursor_pos;
                                state->edit_selection_end = state->edit_cursor_pos;
                                InvalidateRect(hwnd, NULL, FALSE);
                            } else if (state->edit_cursor_pos > 0) {
                                // 没有选中文本，正常向左移动光标
                                state->edit_cursor_pos--;
                                state->edit_selection_start = state->edit_cursor_pos;
                                state->edit_selection_end = state->edit_cursor_pos;
                                InvalidateRect(hwnd, NULL, FALSE);
                            }
                            return 0;
                            
                        case VK_RIGHT:
                            // 右箭头：移动光标
                            if (state->edit_selection_start != state->edit_selection_end) {
                                // 如果有选中文本，将光标移动到选中区域的结束位置
                                state->edit_cursor_pos = (std::max)(state->edit_selection_start, state->edit_selection_end);
                                state->edit_selection_start = state->edit_cursor_pos;
                                state->edit_selection_end = state->edit_cursor_pos;
                                InvalidateRect(hwnd, NULL, FALSE);
                            } else if (state->edit_cursor_pos < static_cast<int>(state->edit_text.length())) {
                                // 没有选中文本，正常向右移动光标
                                state->edit_cursor_pos++;
                                state->edit_selection_start = state->edit_cursor_pos;
                                state->edit_selection_end = state->edit_cursor_pos;
                                InvalidateRect(hwnd, NULL, FALSE);
                            }
                            return 0;
                            
                        case VK_HOME:
                            // Home 键：移动到开头
                            state->edit_cursor_pos = 0;
                            state->edit_selection_start = 0;
                            state->edit_selection_end = 0;
                            InvalidateRect(hwnd, NULL, FALSE);
                            return 0;
                            
                        case VK_END:
                            // End 键：移动到末尾
                            state->edit_cursor_pos = static_cast<int>(state->edit_text.length());
                            state->edit_selection_start = state->edit_cursor_pos;
                            state->edit_selection_end = state->edit_cursor_pos;
                            InvalidateRect(hwnd, NULL, FALSE);
                            return 0;
                            
                        case 'A':
                            // Ctrl+A：全选
                            if (GetKeyState(VK_CONTROL) & 0x8000) {
                                state->edit_selection_start = 0;
                                state->edit_selection_end = static_cast<int>(state->edit_text.length());
                                state->edit_cursor_pos = state->edit_selection_end;
                                InvalidateRect(hwnd, NULL, FALSE);
                                return 0;
                            }
                            break;
                    }
                    return 0;
                }
                
                // 如果正在使用旧的编辑控件，不处理键盘导航
                if (state->edit_control) {
                    return DefWindowProc(hwnd, msg, wParam, lParam);
                }
                
                TreeNode* new_selection = nullptr;
                bool handled = true;
                
                switch (wParam) {
                    case VK_UP:
                        // 选中上一个可见节点
                        if (state->selected_node) {
                            new_selection = GetPreviousVisibleNode(state, state->selected_node);
                        } else if (!state->visible_nodes.empty()) {
                            // 如果没有选中节点，选中最后一个可见节点
                            new_selection = state->visible_nodes.back();
                        }
                        break;
                        
                    case VK_DOWN:
                        // 选中下一个可见节点
                        if (state->selected_node) {
                            new_selection = GetNextVisibleNode(state, state->selected_node);
                        } else if (!state->visible_nodes.empty()) {
                            // 如果没有选中节点，选中第一个可见节点
                            new_selection = state->visible_nodes[0];
                        }
                        break;
                        
                    case VK_LEFT:
                        // 折叠节点或选中父节点
                        if (state->selected_node) {
                            if (state->selected_node->expanded && !state->selected_node->children.empty()) {
                                // 如果节点展开且有子节点，折叠节点
                                CollapseNode(hwnd, state->selected_node->id);
                            } else if (state->selected_node->parent) {
                                // 否则选中父节点
                                new_selection = state->selected_node->parent;
                            }
                        }
                        break;
                        
                    case VK_RIGHT:
                        // 展开节点
                        if (state->selected_node && !state->selected_node->children.empty()) {
                            if (!state->selected_node->expanded) {
                                // 如果节点未展开，展开节点
                                ExpandNode(hwnd, state->selected_node->id);
                            } else {
                                // 如果节点已展开，选中第一个子节点
                                if (!state->selected_node->children.empty()) {
                                    new_selection = state->selected_node->children[0];
                                }
                            }
                        }
                        break;
                        
                    case VK_HOME:
                        // 选中第一个节点
                        if (!state->visible_nodes.empty()) {
                            new_selection = state->visible_nodes[0];
                        }
                        break;
                        
                    case VK_END:
                        // 选中最后一个节点
                        if (!state->visible_nodes.empty()) {
                            new_selection = state->visible_nodes.back();
                        }
                        break;
                        
                    case VK_RETURN:
                        // Enter 键：展开/折叠选中节点
                        if (state->selected_node && !state->selected_node->children.empty()) {
                            if (state->selected_node->expanded) {
                                // 如果节点已展开，折叠节点
                                CollapseNode(hwnd, state->selected_node->id);
                            } else {
                                // 如果节点未展开，展开节点
                                ExpandNode(hwnd, state->selected_node->id);
                            }
                        }
                        break;
                        
                    case VK_SPACE:
                        // 空格键：切换复选框状态（如果有）
                        if (state->selected_node && state->selected_node->has_checkbox) {
                            SetNodeChecked(hwnd, state->selected_node->id, !state->selected_node->checked);
                        }
                        break;
                        
                    default:
                        // 字母键: 查找以该字母开头的节点
                        if (wParam >= 'A' && wParam <= 'Z') {
                            wchar_t search_char = static_cast<wchar_t>(wParam);
                            
                            // 从当前选中节点的下一个节点开始查找
                            int start_index = 0;
                            if (state->selected_node && state->selected_node->visible_index >= 0) {
                                start_index = state->selected_node->visible_index + 1;
                            }
                            
                            // 查找匹配的节点
                            TreeNode* found_node = nullptr;
                            
                            // 先从当前位置往后查找
                            for (int i = start_index; i < static_cast<int>(state->visible_nodes.size()); ++i) {
                                TreeNode* node = state->visible_nodes[i];
                                if (node && !node->text.empty()) {
                                    wchar_t first_char = towupper(node->text[0]);
                                    if (first_char == search_char) {
                                        found_node = node;
                                        break;
                                    }
                                }
                            }
                            
                            // 如果没找到，从头开始查找到当前位置
                            if (!found_node) {
                                int end_index = (state->selected_node && state->selected_node->visible_index >= 0) 
                                    ? state->selected_node->visible_index : 0;
                                    
                                for (int i = 0; i < end_index; ++i) {
                                    TreeNode* node = state->visible_nodes[i];
                                    if (node && !node->text.empty()) {
                                        wchar_t first_char = towupper(node->text[0]);
                                        if (first_char == search_char) {
                                            found_node = node;
                                            break;
                                        }
                                    }
                                }
                            }
                            
                            if (found_node) {
                                new_selection = found_node;
                            } else {
                                handled = false;
                            }
                        } else {
                            handled = false;
                        }
                        break;
                }
                
                // 如果有新的选中节点，更新选择
                if (new_selection && new_selection != state->selected_node) {
                    SetSelectedNode(hwnd, new_selection->id);
                    
                    // 自动滚动到选中节点
                    ScrollToNodeInternal(state, new_selection);
                    
                    // 触发重绘
                    InvalidateRect(hwnd, NULL, FALSE);
                }
                
                if (handled) {
                    return 0;
                }
            }
            
            // 如果没有处理，传递给默认窗口过程
            return DefWindowProc(hwnd, msg, wParam, lParam);
            
        case WM_CHAR:
            // 字符输入处理（用于自定义编辑框）
            if (state && state->custom_editing && state->editing_node) {
                wchar_t ch = static_cast<wchar_t>(wParam);
                
                // 忽略控制字符（除了可打印字符）
                if (ch >= 32 || ch == '\t') {
                    // 如果有选中文本，先删除
                    if (state->edit_selection_start != state->edit_selection_end) {
                        int start = (std::min)(state->edit_selection_start, state->edit_selection_end);
                        int end = (std::max)(state->edit_selection_start, state->edit_selection_end);
                        state->edit_text.erase(start, end - start);
                        state->edit_cursor_pos = start;
                    }
                    
                    // 插入字符
                    state->edit_text.insert(state->edit_cursor_pos, 1, ch);
                    state->edit_cursor_pos++;
                    state->edit_selection_start = state->edit_cursor_pos;
                    state->edit_selection_end = state->edit_cursor_pos;
                    
                    // 触发重绘
                    InvalidateRect(hwnd, NULL, FALSE);
                }
                return 0;
            }
            return DefWindowProc(hwnd, msg, wParam, lParam);
            
        case WM_TIMER:
            // 定时器处理（用于光标闪烁）
            if (state && state->custom_editing && wParam == 1) {
                state->edit_cursor_visible = !state->edit_cursor_visible;
                InvalidateRect(hwnd, NULL, FALSE);
                return 0;
            }
            return DefWindowProc(hwnd, msg, wParam, lParam);
            
        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }
}

// ============================================================================
// 渲染函数实现
// ============================================================================

/**
 * 主渲染函数（优化版）
 * 调用 BeginDraw，清空背景色，计算可见区域，遍历 visible_nodes 调用 RenderNode，
 * 如果需要则渲染滚动条，调用 EndDraw
 * 优化：使用二分查找快速定位可见节点范围，支持10000+节点的高性能渲染
 */
void RenderTreeView(TreeViewState* state) {
    if (!state || !state->render_target) {
        return;
    }
    
    ID2D1HwndRenderTarget* rt = state->render_target;
    
    // 开始绘制
    rt->BeginDraw();
    
    // 清空背景色
    rt->Clear(state->background_color);
    
    // 如果没有可见节点，直接结束
    if (state->visible_nodes.empty()) {
        rt->EndDraw();
        return;
    }
    
    // 计算可见区域（考虑滚动位置）
    RECT client_rect;
    GetClientRect(state->hwnd, &client_rect);
    float visible_top = static_cast<float>(state->scroll_pos);
    float visible_bottom = visible_top + (client_rect.bottom - client_rect.top);
    
    // 使用二分查找快速定位第一个可见节点
    int start_index = FindFirstVisibleNodeIndex(state, visible_top);
    int end_index = FindLastVisibleNodeIndex(state, visible_bottom);
    
    // 确保索引在有效范围内
    start_index = max(0, min(start_index, static_cast<int>(state->visible_nodes.size()) - 1));
    end_index = max(start_index, min(end_index, static_cast<int>(state->visible_nodes.size()) - 1));
    
    // 只渲染可见范围内的节点（性能优化）
    for (int i = start_index; i <= end_index; ++i) {
        TreeNode* node = state->visible_nodes[i];
        if (!node) continue;
        
        float node_top = node->y_position - state->scroll_pos;
        
        // 双重检查确保节点在可见区域内
        if (node_top + node->height < 0 || node_top > visible_bottom) {
            continue;
        }
        
        // 渲染节点
        RenderNode(state, node, node_top);
    }
    
    // 如果需要则渲染滚动条
    if (state->show_scrollbar) {
        RenderScrollbar(state);
    }
    
    // 如果正在拖放，渲染拖放指示线
    if (state->is_dragging && state->drag_node) {
        RenderDropIndicator(state);
    }
    
    // 如果在自定义编辑模式，渲染编辑框
    if (state->custom_editing && state->editing_node) {
        RenderCustomEditBox(state);
    }
    
    // 结束绘制
    HRESULT hr = rt->EndDraw();
    
    // 如果设备丢失，需要重新创建资源
    if (hr == D2DERR_RECREATE_TARGET) {
        // 这里可以添加重新创建渲染目标的逻辑
        // 暂时忽略，在后续版本中完善
    }
}

/**
 * 渲染单个节点
 * 绘制节点背景（选中/悬停状态），计算缩进位置，绘制展开/折叠图标，
 * 绘制复选框，绘制 emoji 图标，绘制节点文本
 */
void RenderNode(TreeViewState* state, TreeNode* node, float y_offset) {
    if (!state || !node || !state->render_target || !state->brush) {
        return;
    }
    
    ID2D1HwndRenderTarget* rt = state->render_target;
    ID2D1SolidColorBrush* brush = state->brush;
    
    // 计算基础位置和尺寸
    float x_base = node->level * state->indent_width * state->dpi_scale;
    float node_height = state->node_height * state->dpi_scale;
    int client_width = GetClientWidth(state->hwnd);
    
    // 1. 绘制节点背景
    D2D1_RECT_F bg_rect = D2D1::RectF(0, y_offset, 
                                      static_cast<float>(client_width), 
                                      y_offset + node_height);
    
    // 首先绘制节点自定义背景色（如果不透明）
    if (node->back_color.a > 0.0f) {
        brush->SetColor(node->back_color);
        rt->FillRectangle(bg_rect, brush);
    }
    
    // 然后绘制选中/悬停状态（会覆盖自定义背景色）
    if (node->selected) {
        brush->SetColor(state->selected_color);
        rt->FillRectangle(bg_rect, brush);
    } else if (node == state->hover_node) {
        brush->SetColor(state->hover_color);
        rt->FillRectangle(bg_rect, brush);
    }
    
    float x_cursor = x_base;
    
    // 2. 如果有子节点则绘制展开/折叠图标
    if (!node->children.empty()) {
        float icon_x = x_cursor + 4 * state->dpi_scale;
        float icon_y = y_offset + (node_height - state->expand_icon_size * state->dpi_scale) / 2;
        
        RenderExpandIcon(state, icon_x, icon_y, node->expanded);
        x_cursor += (state->expand_icon_size + 8) * state->dpi_scale;
    } else {
        // 即使没有子节点，也要预留空间保持对齐
        x_cursor += (state->expand_icon_size + 8) * state->dpi_scale;
    }
    
    // 3. 如果有复选框则绘制复选框
    if (node->has_checkbox) {
        float checkbox_size = 16 * state->dpi_scale;
        float checkbox_x = x_cursor;
        float checkbox_y = y_offset + (node_height - checkbox_size) / 2;
        
        RenderCheckbox(state, checkbox_x, checkbox_y, checkbox_size, node->checked);
        x_cursor += checkbox_size + 6 * state->dpi_scale;
    }
    
    // 4. 如果有图标则绘制 emoji 图标
    if (!node->icon.empty()) {
        float icon_x = x_cursor;
        float icon_y = y_offset + (node_height - state->icon_size * state->dpi_scale) / 2;
        
        RenderEmoji(state, node->icon.c_str(), icon_x, icon_y, state->icon_size * state->dpi_scale);
        x_cursor += (state->icon_size + 6) * state->dpi_scale;
    }
    
    // 5. 绘制节点文本（使用 DirectWrite，支持彩色emoji）
    if (!node->text.empty() && state->text_format && state->dwrite_factory) {
        // 设置文本颜色（考虑启用状态）
        D2D1_COLOR_F text_color;
        if (node->enabled) {
            text_color = node->fore_color;
        } else {
            text_color = D2D1::ColorF(0.6f, 0.6f, 0.6f); // 禁用状态为灰色
        }
        
        // 创建文本布局以支持彩色emoji
        IDWriteTextLayout* text_layout = nullptr;
        float max_width = static_cast<float>(client_width) - x_cursor - 20 * state->dpi_scale;
        
        HRESULT hr = state->dwrite_factory->CreateTextLayout(
            node->text.c_str(),
            static_cast<UINT32>(node->text.length()),
            state->text_format,
            max_width,
            node_height,
            &text_layout
        );
        
        if (SUCCEEDED(hr) && text_layout) {
            // 设置文本颜色
            brush->SetColor(text_color);
            
            // 使用 DrawTextLayout 支持彩色emoji
            rt->DrawTextLayout(
                D2D1::Point2F(x_cursor, y_offset),
                text_layout,
                brush,
                D2D1_DRAW_TEXT_OPTIONS_ENABLE_COLOR_FONT  // 启用彩色字体
            );
            
            text_layout->Release();
        }
    }
}

/**
 * 渲染展开/折叠图标
 * 使用 ID2D1PathGeometry 绘制三角形，展开状态绘制向下箭头，折叠状态绘制向右箭头，应用 DPI 缩放
 */
void RenderExpandIcon(TreeViewState* state, float x, float y, bool expanded) {
    if (!state || !state->render_target || !state->brush || !state->d2d_factory) {
        return;
    }
    
    ID2D1HwndRenderTarget* rt = state->render_target;
    ID2D1SolidColorBrush* brush = state->brush;
    
    // 设置图标颜色
    brush->SetColor(D2D1::ColorF(0.4f, 0.4f, 0.4f));
    
    float size = state->expand_icon_size * state->dpi_scale;
    float center_x = x + size / 2;
    float center_y = y + size / 2;
    
    // 创建路径几何体
    ID2D1PathGeometry* path = nullptr;
    HRESULT hr = state->d2d_factory->CreatePathGeometry(&path);
    
    if (SUCCEEDED(hr) && path) {
        ID2D1GeometrySink* sink = nullptr;
        hr = path->Open(&sink);
        
        if (SUCCEEDED(hr) && sink) {
            if (expanded) {
                // 绘制向下箭头 ▼
                D2D1_POINT_2F points[3] = {
                    D2D1::Point2F(center_x - size * 0.3f, center_y - size * 0.15f),
                    D2D1::Point2F(center_x + size * 0.3f, center_y - size * 0.15f),
                    D2D1::Point2F(center_x, center_y + size * 0.25f)
                };
                
                sink->BeginFigure(points[0], D2D1_FIGURE_BEGIN_FILLED);
                sink->AddLine(points[1]);
                sink->AddLine(points[2]);
                sink->EndFigure(D2D1_FIGURE_END_CLOSED);
            } else {
                // 绘制向右箭头 ▶
                D2D1_POINT_2F points[3] = {
                    D2D1::Point2F(center_x - size * 0.15f, center_y - size * 0.3f),
                    D2D1::Point2F(center_x + size * 0.25f, center_y),
                    D2D1::Point2F(center_x - size * 0.15f, center_y + size * 0.3f)
                };
                
                sink->BeginFigure(points[0], D2D1_FIGURE_BEGIN_FILLED);
                sink->AddLine(points[1]);
                sink->AddLine(points[2]);
                sink->EndFigure(D2D1_FIGURE_END_CLOSED);
            }
            
            sink->Close();
            
            // 填充路径
            rt->FillGeometry(path, brush);
            
            SafeRelease(&sink);
        }
        
        SafeRelease(&path);
    }
}

/**
 * 渲染复选框
 * 绘制复选框边框和背景，如果选中则绘制勾选标记，使用 Element UI 蓝色，应用 DPI 缩放
 */
void RenderCheckbox(TreeViewState* state, float x, float y, float size, bool checked) {
    if (!state || !state->render_target || !state->brush) {
        return;
    }
    
    ID2D1HwndRenderTarget* rt = state->render_target;
    ID2D1SolidColorBrush* brush = state->brush;
    
    D2D1_RECT_F rect = D2D1::RectF(x, y, x + size, y + size);
    
    // 绘制背景
    brush->SetColor(D2D1::ColorF(1.0f, 1.0f, 1.0f)); // 白色背景
    rt->FillRectangle(rect, brush);
    
    // 绘制边框
    brush->SetColor(D2D1::ColorF(0.8f, 0.8f, 0.8f)); // 浅灰色边框
    rt->DrawRectangle(rect, brush, 1.0f * state->dpi_scale);
    
    // 绘制勾选标记
    if (checked) {
        brush->SetColor(D2D1::ColorF(0.26f, 0.59f, 0.98f)); // Element UI 蓝色
        
        float padding = size * 0.25f;
        float stroke_width = 2.0f * state->dpi_scale;
        
        // 绘制勾选标记（两条线段组成的勾）
        D2D1_POINT_2F p1 = D2D1::Point2F(x + padding, y + size / 2);
        D2D1_POINT_2F p2 = D2D1::Point2F(x + size * 0.45f, y + size - padding);
        D2D1_POINT_2F p3 = D2D1::Point2F(x + size - padding, y + padding);
        
        rt->DrawLine(p1, p2, brush, stroke_width);
        rt->DrawLine(p2, p3, brush, stroke_width);
    }
}

/**
 * 渲染 Emoji 图标
 * 使用TextFormatCache获取Segoe UI Emoji字体格式，绘制彩色emoji，应用DPI缩放
 */
void RenderEmoji(TreeViewState* state, const wchar_t* emoji, float x, float y, float size) {
    if (!state || !emoji || !state->render_target || !state->brush || !state->text_format_cache) {
        return;
    }
    
    ID2D1HwndRenderTarget* rt = state->render_target;
    
    // 从缓存获取或创建彩色字体文本格式
    // 注意：size参数已经是DPI缩放后的值，所以dpi_scale传1.0
    IDWriteTextFormat* emoji_format = state->text_format_cache->GetOrCreate(
        L"Segoe UI Emoji",  // 支持彩色 emoji 的字体
        size,               // 字体大小（已经过 DPI 缩放）
        1.0f                // 不再次缩放
    );
    
    if (emoji_format) {
        // 绘制 emoji（颜色会自动使用 emoji 的彩色）
        D2D1_RECT_F rect = D2D1::RectF(x, y, x + size * 1.5f, y + size * 1.5f);
        
        // 设置画刷颜色（emoji 会自动使用彩色，这里的颜色主要用于非彩色字符）
        state->brush->SetColor(D2D1::ColorF(0, 0, 0));
        
        rt->DrawText(
            emoji,
            static_cast<UINT32>(wcslen(emoji)),
            emoji_format,
            rect,
            state->brush
        );
    }
}

/**
 * 渲染滚动条
 * 绘制滚动条背景，计算滑块位置和大小，绘制滚动条滑块（圆角矩形），应用 DPI 缩放
 */
void RenderScrollbar(TreeViewState* state) {
    if (!state || !state->render_target || !state->brush) {
        return;
    }
    
    ID2D1HwndRenderTarget* rt = state->render_target;
    ID2D1SolidColorBrush* brush = state->brush;
    
    RECT client_rect;
    GetClientRect(state->hwnd, &client_rect);
    
    float scrollbar_width = state->scrollbar_width * state->dpi_scale;
    float scrollbar_x = client_rect.right - scrollbar_width;
    float client_height = static_cast<float>(client_rect.bottom - client_rect.top);
    
    // 绘制滚动条背景
    brush->SetColor(state->scrollbar_bg_color);
    D2D1_RECT_F bg_rect = D2D1::RectF(
        scrollbar_x, 0,
        static_cast<float>(client_rect.right), client_height
    );
    rt->FillRectangle(bg_rect, brush);
    
    // 计算滚动条滑块位置和大小
    if (state->content_height > 0 && state->scroll_max > 0) {
        float visible_height = client_height;
        float content_height = static_cast<float>(state->content_height);
        
        // 计算滑块高度（按比例）
        float thumb_height = (visible_height / content_height) * visible_height;
        thumb_height = (std::max)(20.0f * state->dpi_scale, thumb_height); // 最小高度
        
        // 计算滑块位置
        float thumb_y = (static_cast<float>(state->scroll_pos) / static_cast<float>(state->scroll_max)) * (visible_height - thumb_height);
        
        // 绘制滚动条滑块（圆角矩形）
        brush->SetColor(state->scrollbar_thumb_color);
        
        float padding = 2 * state->dpi_scale;
        float radius = 4 * state->dpi_scale;
        
        D2D1_ROUNDED_RECT thumb_rect = D2D1::RoundedRect(
            D2D1::RectF(
                scrollbar_x + padding,
                thumb_y,
                static_cast<float>(client_rect.right) - padding,
                thumb_y + thumb_height
            ),
            radius,
            radius
        );
        rt->FillRoundedRectangle(thumb_rect, brush);
    }
}

/**
 * 渲染自定义编辑框
 * 绘制编辑框背景、边框、文本、选中区域和光标
 */
void RenderCustomEditBox(TreeViewState* state) {
    if (!state || !state->render_target || !state->brush || !state->editing_node) {
        return;
    }
    
    ID2D1HwndRenderTarget* rt = state->render_target;
    ID2D1SolidColorBrush* brush = state->brush;
    
    // 绘制编辑框背景
    brush->SetColor(D2D1::ColorF(1.0f, 1.0f, 1.0f)); // 白色背景
    rt->FillRectangle(state->edit_rect, brush);
    
    // 绘制编辑框边框
    brush->SetColor(D2D1::ColorF(0.26f, 0.59f, 0.98f)); // Element UI 蓝色边框
    rt->DrawRectangle(state->edit_rect, brush, 2.0f * state->dpi_scale);
    
    // 如果有文本，绘制文本
    if (!state->edit_text.empty() && state->text_format && state->dwrite_factory) {
        // 创建文本布局
        IDWriteTextLayout* text_layout = nullptr;
        float max_width = state->edit_rect.right - state->edit_rect.left - 8 * state->dpi_scale;
        float max_height = state->edit_rect.bottom - state->edit_rect.top;
        
        HRESULT hr = state->dwrite_factory->CreateTextLayout(
            state->edit_text.c_str(),
            static_cast<UINT32>(state->edit_text.length()),
            state->text_format,
            max_width,
            max_height,
            &text_layout
        );
        
        if (SUCCEEDED(hr) && text_layout) {
            // 如果有选中文本，绘制选中背景
            if (state->edit_selection_start != state->edit_selection_end) {
                int start = (std::min)(state->edit_selection_start, state->edit_selection_end);
                int end = (std::max)(state->edit_selection_start, state->edit_selection_end);
                
                // 获取选中文本的位置
                DWRITE_HIT_TEST_METRICS hit_metrics_start = {};
                DWRITE_HIT_TEST_METRICS hit_metrics_end = {};
                float x_start = 0, y_start = 0;
                float x_end = 0, y_end = 0;
                
                text_layout->HitTestTextPosition(start, FALSE, &x_start, &y_start, &hit_metrics_start);
                text_layout->HitTestTextPosition(end, FALSE, &x_end, &y_end, &hit_metrics_end);
                
                // 绘制选中背景
                brush->SetColor(D2D1::ColorF(0.26f, 0.59f, 0.98f, 0.3f)); // 半透明蓝色
                D2D1_RECT_F selection_rect = D2D1::RectF(
                    state->edit_rect.left + 4 * state->dpi_scale + x_start,
                    state->edit_rect.top + y_start,
                    state->edit_rect.left + 4 * state->dpi_scale + x_end,
                    state->edit_rect.top + y_end + hit_metrics_end.height
                );
                rt->FillRectangle(selection_rect, brush);
            }
            
            // 绘制文本
            brush->SetColor(D2D1::ColorF(0.2f, 0.2f, 0.2f)); // 深灰色文本
            rt->DrawTextLayout(
                D2D1::Point2F(
                    state->edit_rect.left + 4 * state->dpi_scale,
                    state->edit_rect.top + (state->edit_rect.bottom - state->edit_rect.top - state->node_height * state->dpi_scale) / 2
                ),
                text_layout,
                brush,
                D2D1_DRAW_TEXT_OPTIONS_ENABLE_COLOR_FONT
            );
            
            // 绘制光标（如果可见）
            if (state->edit_cursor_visible) {
                DWRITE_HIT_TEST_METRICS hit_metrics = {};
                float cursor_x = 0, cursor_y = 0;
                
                if (state->edit_cursor_pos > 0 && state->edit_cursor_pos <= static_cast<int>(state->edit_text.length())) {
                    text_layout->HitTestTextPosition(state->edit_cursor_pos, FALSE, &cursor_x, &cursor_y, &hit_metrics);
                } else {
                    cursor_y = 0;
                    hit_metrics.height = state->node_height * state->dpi_scale;
                }
                
                // 绘制光标线
                brush->SetColor(D2D1::ColorF(0.2f, 0.2f, 0.2f)); // 深灰色光标
                D2D1_POINT_2F p1 = D2D1::Point2F(
                    state->edit_rect.left + 4 * state->dpi_scale + cursor_x,
                    state->edit_rect.top + cursor_y + 2 * state->dpi_scale
                );
                D2D1_POINT_2F p2 = D2D1::Point2F(
                    state->edit_rect.left + 4 * state->dpi_scale + cursor_x,
                    state->edit_rect.top + cursor_y + hit_metrics.height - 2 * state->dpi_scale
                );
                rt->DrawLine(p1, p2, brush, 1.5f * state->dpi_scale);
            }
            
            text_layout->Release();
        }
    } else {
        // 没有文本时，只绘制光标
        if (state->edit_cursor_visible) {
            brush->SetColor(D2D1::ColorF(0.2f, 0.2f, 0.2f)); // 深灰色光标
            D2D1_POINT_2F p1 = D2D1::Point2F(
                state->edit_rect.left + 4 * state->dpi_scale,
                state->edit_rect.top + 2 * state->dpi_scale
            );
            D2D1_POINT_2F p2 = D2D1::Point2F(
                state->edit_rect.left + 4 * state->dpi_scale,
                state->edit_rect.bottom - 2 * state->dpi_scale
            );
            rt->DrawLine(p1, p2, brush, 1.5f * state->dpi_scale);
        }
    }
}

// ============================================================================
// 拖放辅助函数实现
// ============================================================================

/**
 * 计算拖放目标
 * 根据鼠标位置计算拖放的目标节点和插入位置
 */
bool CalculateDropTarget(TreeViewState* state, int x, int y, TreeNode** target_node, int* insert_index, bool* as_child) {
    if (!state || !target_node || !insert_index || !as_child) {
        return false;
    }
    
    *target_node = nullptr;
    *insert_index = -1;
    *as_child = false;
    
    // 调整坐标（考虑滚动）
    float adjusted_y = y + state->scroll_pos;
    
    // 查找鼠标位置的节点
    TreeNode* hover_node = nullptr;
    int hover_index = -1;
    
    for (int i = 0; i < static_cast<int>(state->visible_nodes.size()); ++i) {
        TreeNode* node = state->visible_nodes[i];
        if (!node) continue;
        
        float node_top = node->y_position;
        float node_bottom = node_top + node->height;
        
        if (adjusted_y >= node_top && adjusted_y < node_bottom) {
            hover_node = node;
            hover_index = i;
            break;
        }
    }
    
    if (!hover_node) {
        return false;
    }
    
    // 不能拖放到自己或自己的子节点上
    if (hover_node == state->drag_node) {
        return false;
    }
    
    // 检查是否是拖动节点的子节点（递归检查）
    TreeNode* check_parent = hover_node->parent;
    while (check_parent) {
        if (check_parent == state->drag_node) {
            return false; // 不能拖放到自己的子节点上
        }
        check_parent = check_parent->parent;
    }
    
    // 计算在节点内的相对位置
    float node_top = hover_node->y_position;
    float relative_y = adjusted_y - node_top;
    float node_height = hover_node->height;
    
    // 根据鼠标在节点内的位置决定插入方式
    if (relative_y < node_height * 0.25f) {
        // 上部1/4：插入到节点前面（作为兄弟节点）
        *target_node = hover_node->parent;
        *as_child = false;
        
        if (hover_node->parent) {
            // 找到hover_node在父节点children中的索引
            auto& siblings = hover_node->parent->children;
            auto it = std::find(siblings.begin(), siblings.end(), hover_node);
            if (it != siblings.end()) {
                *insert_index = static_cast<int>(it - siblings.begin());
            }
        } else {
            // 根节点：插入到根节点列表中
            auto& roots = state->root_nodes;
            auto it = std::find(roots.begin(), roots.end(), hover_node);
            if (it != roots.end()) {
                *insert_index = static_cast<int>(it - roots.begin());
            }
        }
    } else if (relative_y > node_height * 0.75f) {
        // 下部1/4：插入到节点后面（作为兄弟节点）
        *target_node = hover_node->parent;
        *as_child = false;
        
        if (hover_node->parent) {
            // 找到hover_node在父节点children中的索引
            auto& siblings = hover_node->parent->children;
            auto it = std::find(siblings.begin(), siblings.end(), hover_node);
            if (it != siblings.end()) {
                *insert_index = static_cast<int>(it - siblings.begin()) + 1;
            }
        } else {
            // 根节点：插入到根节点列表中
            auto& roots = state->root_nodes;
            auto it = std::find(roots.begin(), roots.end(), hover_node);
            if (it != roots.end()) {
                *insert_index = static_cast<int>(it - roots.begin()) + 1;
            }
        }
    } else {
        // 中间1/2：作为子节点插入
        *target_node = hover_node;
        *as_child = true;
        *insert_index = static_cast<int>(hover_node->children.size()); // 插入到末尾
    }
    
    return true;
}

/**
 * 渲染拖放指示线
 * 在拖放过程中显示插入位置的指示线
 */
void RenderDropIndicator(TreeViewState* state) {
    if (!state || !state->render_target || !state->brush || !state->is_dragging || !state->drop_target_node) {
        return;
    }
    
    ID2D1HwndRenderTarget* rt = state->render_target;
    ID2D1SolidColorBrush* brush = state->brush;
    
    // 设置指示线颜色（Element UI 蓝色）
    brush->SetColor(D2D1::ColorF(0.26f, 0.59f, 0.98f));
    
    float line_thickness = 2.0f * state->dpi_scale;
    int client_width = GetClientWidth(state->hwnd);
    
    if (state->drop_as_child) {
        // 作为子节点：在目标节点周围绘制边框
        for (TreeNode* node : state->visible_nodes) {
            if (node == state->drop_target_node) {
                float node_top = node->y_position - state->scroll_pos;
                float node_height = node->height;
                
                if (node_top >= 0 && node_top < GetClientHeight(state->hwnd)) {
                    D2D1_RECT_F rect = D2D1::RectF(
                        0, node_top,
                        static_cast<float>(client_width), node_top + node_height
                    );
                    rt->DrawRectangle(rect, brush, line_thickness);
                }
                break;
            }
        }
    } else {
        // 作为兄弟节点：绘制水平指示线
        float line_y = 0;
        float indent_x = 0;
        
        if (state->drop_target_node) {
            // 有父节点：根据父节点的层级计算缩进
            indent_x = (state->drop_target_node->level + 1) * state->indent_width * state->dpi_scale;
        } else {
            // 根节点：无缩进
            indent_x = 0;
        }
        
        // 计算指示线的Y位置
        if (state->drop_insert_index == 0) {
            // 插入到第一个位置
            if (state->drop_target_node && !state->drop_target_node->children.empty()) {
                TreeNode* first_child = state->drop_target_node->children[0];
                line_y = first_child->y_position - state->scroll_pos;
            } else if (!state->root_nodes.empty()) {
                TreeNode* first_root = state->root_nodes[0];
                line_y = first_root->y_position - state->scroll_pos;
            }
        } else {
            // 插入到指定位置
            std::vector<TreeNode*>* siblings = nullptr;
            if (state->drop_target_node) {
                siblings = &state->drop_target_node->children;
            } else {
                siblings = &state->root_nodes;
            }
            
            if (siblings && state->drop_insert_index > 0 && state->drop_insert_index <= static_cast<int>(siblings->size())) {
                TreeNode* prev_node = (*siblings)[state->drop_insert_index - 1];
                line_y = prev_node->y_position + prev_node->height - state->scroll_pos;
            }
        }
        
        // 绘制水平指示线
        if (line_y >= 0 && line_y < GetClientHeight(state->hwnd)) {
            D2D1_POINT_2F start = D2D1::Point2F(indent_x, line_y);
            D2D1_POINT_2F end = D2D1::Point2F(static_cast<float>(client_width) - 20 * state->dpi_scale, line_y);
            rt->DrawLine(start, end, brush, line_thickness);
            
            // 在线的左端绘制一个小圆点
            D2D1_ELLIPSE circle = D2D1::Ellipse(start, 3 * state->dpi_scale, 3 * state->dpi_scale);
            rt->FillEllipse(circle, brush);
        }
    }
}

/**
 * 移动节点到新位置
 * @param state 树形框状态
 * @param node 要移动的节点
 * @param new_parent 新的父节点（nullptr表示根节点）
 * @param insert_index 插入位置索引
 * @return 成功返回true，失败返回false
 */
bool MoveNodeToPosition(TreeViewState* state, TreeNode* node, TreeNode* new_parent, int insert_index) {
    if (!state || !node) {
        return false;
    }
    
    // 从原位置移除节点
    if (node->parent) {
        // 从父节点的children列表中移除
        auto& siblings = node->parent->children;
        auto it = std::find(siblings.begin(), siblings.end(), node);
        if (it != siblings.end()) {
            siblings.erase(it);
        }
    } else {
        // 从根节点列表中移除
        auto& roots = state->root_nodes;
        auto it = std::find(roots.begin(), roots.end(), node);
        if (it != roots.end()) {
            roots.erase(it);
        }
    }
    
    // 更新节点的父节点指针
    node->parent = new_parent;
    
    // 重新计算节点层级
    if (new_parent) {
        node->level = new_parent->level + 1;
    } else {
        node->level = 0;
    }
    
    // 递归更新所有子节点的层级
    std::function<void(TreeNode*, int)> update_levels = [&](TreeNode* n, int level) {
        n->level = level;
        for (TreeNode* child : n->children) {
            update_levels(child, level + 1);
        }
    };
    update_levels(node, node->level);
    
    // 插入到新位置
    if (new_parent) {
        // 插入到父节点的children列表中
        auto& siblings = new_parent->children;
        int safe_index = (std::max)(0, (std::min)(insert_index, static_cast<int>(siblings.size())));
        siblings.insert(siblings.begin() + safe_index, node);
    } else {
        // 插入到根节点列表中
        auto& roots = state->root_nodes;
        int safe_index = (std::max)(0, (std::min)(insert_index, static_cast<int>(roots.size())));
        roots.insert(roots.begin() + safe_index, node);
    }
    
    return true;
}

// ============================================================================
// 性能优化函数实现
// ============================================================================

/**
 * 只重绘单个节点的矩形区域
 * 减少不必要的全窗口重绘，提高性能
 */
void InvalidateNode(TreeViewState* state, TreeNode* node) {
    if (!state || !node || !state->hwnd) {
        return;
    }
    
    // 计算节点在屏幕上的矩形区域
    RECT node_rect;
    node_rect.left = 0;
    node_rect.top = static_cast<LONG>(node->y_position - state->scroll_pos);
    node_rect.right = GetClientWidth(state->hwnd);
    node_rect.bottom = node_rect.top + static_cast<LONG>(node->height);
    
    // 只有当节点在可见区域内时才重绘
    int client_height = GetClientHeight(state->hwnd);
    if (node_rect.bottom >= 0 && node_rect.top < client_height) {
        // 确保矩形在客户区范围内
        node_rect.top = max(0, node_rect.top);
        node_rect.bottom = min(client_height, node_rect.bottom);
        
        // 只重绘这个节点的矩形区域
        InvalidateRect(state->hwnd, &node_rect, FALSE);
    }
}

/**
 * 使用二分查找快速定位第一个可见节点的索引
 * @param state 树形框状态
 * @param visible_top 可见区域顶部Y坐标
 * @return 第一个可见节点的索引
 */
int FindFirstVisibleNodeIndex(TreeViewState* state, float visible_top) {
    if (!state || state->visible_nodes.empty()) {
        return 0;
    }
    
    int left = 0;
    int right = static_cast<int>(state->visible_nodes.size()) - 1;
    int result = 0;
    
    while (left <= right) {
        int mid = left + (right - left) / 2;
        TreeNode* node = state->visible_nodes[mid];
        
        if (!node) {
            left = mid + 1;
            continue;
        }
        
        float node_bottom = node->y_position + node->height;
        
        if (node_bottom >= visible_top) {
            result = mid;
            right = mid - 1;
        } else {
            left = mid + 1;
        }
    }
    
    return result;
}

/**
 * 使用二分查找快速定位最后一个可见节点的索引
 * @param state 树形框状态
 * @param visible_bottom 可见区域底部Y坐标
 * @return 最后一个可见节点的索引
 */
int FindLastVisibleNodeIndex(TreeViewState* state, float visible_bottom) {
    if (!state || state->visible_nodes.empty()) {
        return 0;
    }
    
    int left = 0;
    int right = static_cast<int>(state->visible_nodes.size()) - 1;
    int result = right;
    
    while (left <= right) {
        int mid = left + (right - left) / 2;
        TreeNode* node = state->visible_nodes[mid];
        
        if (!node) {
            right = mid - 1;
            continue;
        }
        
        if (node->y_position <= visible_bottom) {
            result = mid;
            left = mid + 1;
        } else {
            right = mid - 1;
        }
    }
    
    return result;
}

/**
 * 文本格式缓存类实现
 * 缓存不同字体大小的 IDWriteTextFormat，避免重复创建和销毁
 */

TextFormatCache::TextFormatCache(IDWriteFactory* factory) 
    : dwrite_factory(factory) {
    if (dwrite_factory) {
        dwrite_factory->AddRef();
    }
}

TextFormatCache::~TextFormatCache() {
    Clear();
    SafeRelease(&dwrite_factory);
}

std::wstring TextFormatCache::GenerateKey(const wchar_t* font_family, float font_size, float dpi_scale) {
    // 生成唯一的缓存键：字体名称_字体大小_DPI缩放
    wchar_t buffer[256];
    swprintf_s(buffer, L"%s_%.1f_%.2f", font_family, font_size, dpi_scale);
    return std::wstring(buffer);
}

IDWriteTextFormat* TextFormatCache::GetOrCreate(
    const wchar_t* font_family,
    float font_size,
    float dpi_scale
) {
    if (!dwrite_factory || !font_family) {
        return nullptr;
    }
    
    // 生成缓存键
    std::wstring key = GenerateKey(font_family, font_size, dpi_scale);
    
    // 查找缓存
    auto it = formats.find(key);
    if (it != formats.end()) {
        return it->second;
    }
    
    // 创建新的文本格式
    IDWriteTextFormat* text_format = nullptr;
    float scaled_font_size = font_size * dpi_scale;
    
    HRESULT hr = dwrite_factory->CreateTextFormat(
        font_family,
        nullptr,                // 字体集合（使用系统默认）
        DWRITE_FONT_WEIGHT_NORMAL,
        DWRITE_FONT_STYLE_NORMAL,
        DWRITE_FONT_STRETCH_NORMAL,
        scaled_font_size,       // DPI 缩放后的字体大小
        L"zh-CN",              // 本地化设置
        &text_format
    );
    
    if (SUCCEEDED(hr) && text_format) {
        // 设置文本对齐方式
        text_format->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
        text_format->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
        
        // 添加到缓存
        formats[key] = text_format;
        
        return text_format;
    }
    
    return nullptr;
}

void TextFormatCache::Clear() {
    for (auto& pair : formats) {
        SafeRelease(&pair.second);
    }
    formats.clear();
}

size_t TextFormatCache::GetCacheSize() const {
    return formats.size();
}