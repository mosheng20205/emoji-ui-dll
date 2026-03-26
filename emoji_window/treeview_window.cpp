#define _CRT_SECURE_NO_WARNINGS
#include "treeview_window.h"
#include "treeview_state.h"
#include "log.h"
#include <windowsx.h>
#include <algorithm>
#include <cwctype>
#include <cstring>
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

void TreeViewOnNodeSelectedDeferred(HWND hTree, int node_id) {
    TreeViewState* st = GetTreeViewState(hTree);
    WriteLog("TreeViewOnNodeSelectedDeferred: hTree=%p, node_id=%d, state=%p, callback=%p, context=%p",
        hTree,
        node_id,
        st,
        st ? reinterpret_cast<void*>(st->on_node_selected) : nullptr,
        st ? reinterpret_cast<void*>(st->callback_context) : nullptr);
    if (st && st->on_node_selected) {
        st->on_node_selected(node_id, st->callback_context);
        WriteLog("TreeViewOnNodeSelectedDeferred: callback returned, hTree=%p, node_id=%d", hTree, node_id);
    }
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

static unsigned int ColorFToArgb(const D2D1_COLOR_F& c) {
    auto ch = [](float v) -> unsigned int {
        int n = static_cast<int>(v * 255.0f + 0.5f);
        if (n < 0) n = 0;
        if (n > 255) n = 255;
        return static_cast<unsigned int>(n);
    };
    return (ch(c.a) << 24) | (ch(c.r) << 16) | (ch(c.g) << 8) | ch(c.b);
}

static bool IsNodeInSubtree(TreeNode* root, TreeNode* target) {
    if (!root || !target) {
        return false;
    }
    if (root == target) {
        return true;
    }
    for (TreeNode* ch : root->children) {
        if (IsNodeInSubtree(ch, target)) {
            return true;
        }
    }
    return false;
}

static void TreeViewInvalidateLayout(HWND hwnd) {
    TreeViewState* state = GetTreeViewState(hwnd);
    if (!state || !state->hwnd) {
        return;
    }
    RebuildVisibleNodes(state);
    RecalculateLayout(state);
    InvalidateRect(hwnd, NULL, TRUE);
}

static void TreeViewApplySelection(HWND hwnd, TreeViewState* state, TreeNode* node, bool notify, bool always_notify_if_same) {
    if (!state || !node || !state->hwnd) {
        return;
    }
    bool same = (state->selected_node == node);
    if (same && !always_notify_if_same) {
        return;
    }
    if (state->selected_node && state->selected_node != node) {
        state->selected_node->selected = false;
    }
    state->selected_node = node;
    node->selected = true;
    const bool need_cb =
        notify && state->on_node_selected && (!same || always_notify_if_same);
    if (need_cb) {
        HWND root = GetAncestor(hwnd, GA_ROOT);
        WriteLog("TreeViewApplySelection: hTree=%p, root=%p, node_id=%d, same=%d, always_notify_if_same=%d, callback=%p",
            hwnd,
            root,
            node->id,
            same ? 1 : 0,
            always_notify_if_same ? 1 : 0,
            reinterpret_cast<void*>(state->on_node_selected));
        if (root && PostMessageW(root, WM_EW_TV_NODE_SELECTED_DEFERRED, (WPARAM)hwnd, (LPARAM)(INT_PTR)node->id)) {
            WriteLog("TreeViewApplySelection: posted WM_EW_TV_NODE_SELECTED_DEFERRED, hTree=%p, root=%p, node_id=%d",
                hwnd, root, node->id);
            // 下一轮在主窗口过程里再调易语言回调，避免 DLL 仍在 TreeViewApplySelection 栈上时同步进入易语言崩溃
        } else {
            WriteLog("TreeViewApplySelection: PostMessage failed, invoking callback inline, hTree=%p, root=%p, node_id=%d",
                hwnd, root, node->id);
            state->on_node_selected(node->id, state->callback_context);
            WriteLog("TreeViewApplySelection: inline callback returned, hTree=%p, node_id=%d", hwnd, node->id);
            if (root) {
                PostMessageW(root, WM_EW_TV_INVALIDATE_DEFERRED, (WPARAM)hwnd, 0);
            } else {
                InvalidateRect(hwnd, NULL, FALSE);
            }
        }
    } else {
        InvalidateRect(hwnd, NULL, FALSE);
    }
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
    float spacing = state->item_spacing * state->dpi_scale;
    
    for (size_t i = 0; i < state->visible_nodes.size(); ++i) {
        TreeNode* node = state->visible_nodes[i];
        node->y_position = y_offset;
        node->height = node_height;
        y_offset += node_height;
        if (i + 1 < state->visible_nodes.size()) {
            y_offset += spacing;
        }
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
    
    const wchar_t* face = state->font_family_name.empty() ? L"Segoe UI Emoji" : state->font_family_name.c_str();
    state->text_format = state->text_format_cache->GetOrCreate(
        face,
        state->font_size_logical > 0.1f ? state->font_size_logical : 14.0f,
        state->dpi_scale,
        state->font_weight,
        state->font_style
    );
}

/**
 * 命中测试
 */
TreeNode* HitTestNode(TreeViewState* state, int x, int y) {
    if (!state) return nullptr;
    
    // 调整坐标（考虑滚动）
    float adjusted_y = static_cast<float>(y) + static_cast<float>(state->scroll_pos);
    
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

// 重建 D2D 渲染目标（窗口隐藏后显示时可能失效）
static bool RecreateTreeViewRenderTarget(TreeViewState* state) {
    if (!state || !state->d2d_factory) return false;
    SafeRelease(&state->brush);
    SafeRelease(&state->render_target);
    RECT rc;
    GetClientRect(state->hwnd, &rc);
    int w = rc.right - rc.left, h = rc.bottom - rc.top;
    if (w < 1) w = 1;
    if (h < 1) h = 1;
    HRESULT hr = state->d2d_factory->CreateHwndRenderTarget(
        D2D1::RenderTargetProperties(),
        D2D1::HwndRenderTargetProperties(state->hwnd, D2D1::SizeU((UINT)w, (UINT)h)),
        &state->render_target);
    if (FAILED(hr) || !state->render_target) return false;
    hr = state->render_target->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Black), &state->brush);
    return SUCCEEDED(hr) && state->brush;
}
void RenderExpandIcon(TreeViewState* state, float x, float y, bool expanded);
void RenderCheckbox(TreeViewState* state, float x, float y, float size, bool checked);
void RenderEmoji(TreeViewState* state, const wchar_t* emoji, float x, float y, float size);
void RenderScrollbar(TreeViewState* state);
void RenderCustomEditBox(TreeViewState* state);

// ============================================================================
// CreateSimpleTreeView - 标准 Windows SysTreeView32 控件（用于排查 Tab 内显示问题）
// ============================================================================

HWND __stdcall CreateSimpleTreeView(HWND parent, int x, int y, int width, int height) {
    if (!parent) return NULL;

    // 确保 Common Controls 已初始化（含 TreeView）
    static bool treeview_comctl_init = false;
    if (!treeview_comctl_init) {
        INITCOMMONCONTROLSEX icex = {};
        icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
        icex.dwICC = ICC_TREEVIEW_CLASSES;
        if (InitCommonControlsEx(&icex)) {
            treeview_comctl_init = true;
        }
    }

    DWORD style = WS_CHILD | WS_VISIBLE | WS_BORDER
        | TVS_HASLINES | TVS_HASBUTTONS | TVS_LINESATROOT | TVS_SHOWSELALWAYS;

    HWND hTree = CreateWindowExW(
        0,
        WC_TREEVIEWW,
        L"",
        style,
        x, y, width, height,
        parent,
        NULL,
        GetModuleHandle(NULL),
        NULL
    );

    if (!hTree) return NULL;

    // 添加默认根节点便于验证显示
    wchar_t rootText[] = L"\u6839\u8282\u70B9";  // "根节点"
    TVINSERTSTRUCTW tvis = {};
    tvis.hParent = TVI_ROOT;
    tvis.hInsertAfter = TVI_LAST;
    tvis.item.mask = TVIF_TEXT;
    tvis.item.pszText = rootText;
    TreeView_InsertItem(hTree, &tvis);

    return hTree;
}

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
    state->selected_color = D2D1::ColorF(0.26f, 0.59f, 0.98f, 0.2f);  // Element UI 蓝色半透明（选中背景）
    state->selected_foreground = D2D1::ColorF(0.26f, 0.59f, 0.98f, 1.0f); // 选中前景（字/图标）
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

    // 添加默认根节点，确保空树也有可见内容（便于 Tab 内显示验证）
    AddRootNode(hwnd, reinterpret_cast<const unsigned char*>("\xe6\xa0\xb9\xe8\x8a\x82\xe7\x82\xb9"), 9, nullptr, 0);

    return hwnd;
}

/** 仅释放 TreeViewState（不销毁 HWND）；供 WM_DESTROY 与 DestroyTreeView 共用 */
static bool FreeTreeViewState(HWND hwnd) {
    if (!hwnd)
        return false;
    TreeViewState* state = GetTreeViewState(hwnd);
    if (!state)
        return false;

    // 必须在 DeleteAllNodes 之前退出编辑：否则 editing_node 被清空后 ExitEditMode 无法 KillTimer，
    // 且 state->text_format 由 TextFormatCache 持有 refcount，不得对 text_format 单独 Release（否则缓存 Clear 时 UAF/double-free）
    if (state->custom_editing && state->editing_node) {
        ExitEditMode(state, false);
    } else if (state->custom_editing) {
        state->custom_editing = false;
        KillTimer(state->hwnd, 1);
    }
    if (state->edit_control) {
        HWND ec = state->edit_control;
        state->edit_control = nullptr;
        DestroyWindow(ec);
    }
    DeleteAllNodes(state);

    SafeRelease(&state->brush);
    // text_format 来自 text_format_cache->GetOrCreate，无独立 AddRef，由缓存 Clear 统一 Release
    state->text_format = nullptr;
    SafeRelease(&state->render_target);
    SafeRelease(&state->d2d_factory);
    if (state->text_format_cache) {
        delete state->text_format_cache;
        state->text_format_cache = nullptr;
    }
    SafeRelease(&state->dwrite_factory);
    g_treeview_states.erase(hwnd);
    delete state;
    return true;
}

bool __stdcall DestroyTreeView(HWND hwnd) {
    if (!hwnd)
        return false;
    if (!FreeTreeViewState(hwnd))
        return false;
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
        WriteLog("GetNodeParent: state not found, hwnd=%p", hwnd);
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
    
    node->fore_color = state->text_color;
    
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
    
    node->fore_color = state->text_color;
    
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
    
    if (state->selected_node == node) {
        return true;
    }
    
    TreeViewApplySelection(hwnd, state, node, true, false);
    return true;
}

int __stdcall GetSelectedNode(HWND hwnd) {
    // 参数验证
    if (!hwnd) {
        WriteLog("GetNodeParent: hwnd is null");
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
        WriteLog("GetNodeParent: node not found, hwnd=%p, node_id=%d", hwnd, node_id);
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
    WriteLog("GetNodeParent: enter hwnd=%p, node_id=%d", hwnd, node_id);
    if (!hwnd) {
        WriteLog("GetNodeParent: hwnd is null");
        return -1;
    }
    
    TreeViewState* state = GetTreeViewState(hwnd);
    if (!state) {
        WriteLog("GetNodeParent: state not found, hwnd=%p", hwnd);
        return -1;
    }

    if (node_id <= 0) {
        WriteLog("GetNodeParent: invalid non-positive node_id=%d, hwnd=%p", node_id, hwnd);
        return -1;
    }

    if (state->next_node_id > 1 && node_id >= state->next_node_id) {
        WriteLog("GetNodeParent: node_id out of range, hwnd=%p, node_id=%d, next_node_id=%d",
            hwnd,
            node_id,
            state->next_node_id);
        return -1;
    }
    
    TreeNode* node = FindNodeById(state, node_id);
    if (!node) {
        WriteLog("GetNodeParent: node not found, hwnd=%p, node_id=%d", hwnd, node_id);
        return -1;
    }
    
    if (node->parent) {
        WriteLog("GetNodeParent: return parent_id=%d for node_id=%d", node->parent->id, node_id);
        return node->parent->id;
    }
    
    WriteLog("GetNodeParent: node_id=%d is root, return -1", node_id);
    return -1;
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

bool __stdcall SetTreeViewSidebarMode(HWND hwnd, bool enable) {
    TreeViewState* state = GetTreeViewState(hwnd);
    if (!state) {
        return false;
    }
    state->sidebar_mode = enable;
    InvalidateRect(hwnd, NULL, TRUE);
    return true;
}

bool __stdcall GetTreeViewSidebarMode(HWND hwnd) {
    TreeViewState* state = GetTreeViewState(hwnd);
    if (!state) {
        return false;
    }
    return state->sidebar_mode;
}

bool __stdcall SetTreeViewRowHeight(HWND hwnd, float height) {
    TreeViewState* state = GetTreeViewState(hwnd);
    if (!state || height < 4.0f) {
        return false;
    }
    state->node_height = height;
    TreeViewInvalidateLayout(hwnd);
    return true;
}

float __stdcall GetTreeViewRowHeight(HWND hwnd) {
    TreeViewState* state = GetTreeViewState(hwnd);
    if (!state) {
        return 0.0f;
    }
    return state->node_height;
}

bool __stdcall SetTreeViewItemSpacing(HWND hwnd, float spacing) {
    TreeViewState* state = GetTreeViewState(hwnd);
    if (!state || spacing < 0.0f) {
        return false;
    }
    state->item_spacing = spacing;
    TreeViewInvalidateLayout(hwnd);
    return true;
}

float __stdcall GetTreeViewItemSpacing(HWND hwnd) {
    TreeViewState* state = GetTreeViewState(hwnd);
    if (!state) {
        return 0.0f;
    }
    return state->item_spacing;
}

bool __stdcall SetTreeViewTextColor(HWND hwnd, unsigned int argb) {
    TreeViewState* state = GetTreeViewState(hwnd);
    if (!state) {
        return false;
    }
    state->text_color = ArgbToColorF(argb);
    for (auto& kv : state->node_map) {
        if (kv.second) {
            kv.second->fore_color = state->text_color;
        }
    }
    InvalidateRect(hwnd, NULL, TRUE);
    return true;
}

unsigned int __stdcall GetTreeViewTextColor(HWND hwnd) {
    TreeViewState* state = GetTreeViewState(hwnd);
    if (!state) {
        return 0;
    }
    return ColorFToArgb(state->text_color);
}

bool __stdcall SetTreeViewSelectedBgColor(HWND hwnd, unsigned int argb) {
    TreeViewState* state = GetTreeViewState(hwnd);
    if (!state) {
        return false;
    }
    state->selected_color = ArgbToColorF(argb);
    InvalidateRect(hwnd, NULL, TRUE);
    return true;
}

unsigned int __stdcall GetTreeViewSelectedBgColor(HWND hwnd) {
    TreeViewState* state = GetTreeViewState(hwnd);
    if (!state) {
        return 0;
    }
    return ColorFToArgb(state->selected_color);
}

bool __stdcall SetTreeViewSelectedForeColor(HWND hwnd, unsigned int argb) {
    TreeViewState* state = GetTreeViewState(hwnd);
    if (!state) {
        return false;
    }
    state->selected_foreground = ArgbToColorF(argb);
    InvalidateRect(hwnd, NULL, TRUE);
    return true;
}

unsigned int __stdcall GetTreeViewSelectedForeColor(HWND hwnd) {
    TreeViewState* state = GetTreeViewState(hwnd);
    if (!state) {
        return 0;
    }
    return ColorFToArgb(state->selected_foreground);
}

bool __stdcall SetTreeViewHoverBgColor(HWND hwnd, unsigned int argb) {
    TreeViewState* state = GetTreeViewState(hwnd);
    if (!state) {
        return false;
    }
    state->hover_color = ArgbToColorF(argb);
    InvalidateRect(hwnd, NULL, TRUE);
    return true;
}

unsigned int __stdcall GetTreeViewHoverBgColor(HWND hwnd) {
    TreeViewState* state = GetTreeViewState(hwnd);
    if (!state) {
        return 0;
    }
    return ColorFToArgb(state->hover_color);
}

bool __stdcall SetTreeViewFont(
    HWND hwnd,
    const unsigned char* family_utf8,
    int family_len,
    float font_size,
    int font_weight,
    bool italic
) {
    TreeViewState* state = GetTreeViewState(hwnd);
    if (!state || !family_utf8 || family_len <= 0 || font_size < 4.0f) {
        return false;
    }
    std::wstring fam = Utf8ToUtf16(family_utf8, family_len);
    if (fam.empty()) {
        return false;
    }
    state->font_family_name = fam;
    state->font_size_logical = font_size;
    if (font_weight < 0) {
        font_weight = static_cast<int>(DWRITE_FONT_WEIGHT_NORMAL);
    }
    if (font_weight > 1000) {
        font_weight = 1000;
    }
    state->font_weight = static_cast<DWRITE_FONT_WEIGHT>(font_weight);
    state->font_style = italic ? DWRITE_FONT_STYLE_ITALIC : DWRITE_FONT_STYLE_NORMAL;
    if (state->text_format_cache) {
        state->text_format_cache->Clear();
    }
    RecreateTextFormat(state);
    TreeViewInvalidateLayout(hwnd);
    return true;
}

bool __stdcall GetTreeViewFont(
    HWND hwnd,
    unsigned char* family_buf,
    int family_buf_size,
    float* out_font_size,
    int* out_font_weight,
    bool* out_italic
) {
    TreeViewState* state = GetTreeViewState(hwnd);
    if (!state) {
        return false;
    }
    if (out_font_size) {
        *out_font_size = state->font_size_logical;
    }
    if (out_font_weight) {
        *out_font_weight = static_cast<int>(state->font_weight);
    }
    if (out_italic) {
        *out_italic = (state->font_style == DWRITE_FONT_STYLE_ITALIC);
    }
    if (!family_buf || family_buf_size <= 0) {
        return true;
    }
    std::string u8 = Utf16ToUtf8(state->font_family_name);
    int n = static_cast<int>(u8.size());
    if (n >= family_buf_size) {
        n = family_buf_size - 1;
    }
    memcpy(family_buf, u8.data(), static_cast<size_t>(n));
    family_buf[n] = 0;
    return true;
}

bool __stdcall MoveTreeViewNode(HWND hwnd, int node_id, int new_parent_id, int insert_index) {
    TreeViewState* state = GetTreeViewState(hwnd);
    if (!state) {
        return false;
    }
    TreeNode* node = FindNodeById(state, node_id);
    if (!node) {
        return false;
    }
    TreeNode* new_parent = nullptr;
    if (new_parent_id >= 0) {
        new_parent = FindNodeById(state, new_parent_id);
        if (!new_parent) {
            return false;
        }
        if (new_parent == node || IsNodeInSubtree(node, new_parent)) {
            return false;
        }
    }
    if (!MoveNodeToPosition(state, node, new_parent, insert_index)) {
        return false;
    }
    TreeViewInvalidateLayout(hwnd);
    if (state->on_node_moved) {
        state->on_node_moved(node_id, new_parent_id, insert_index, state->callback_context);
    }
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

        case WM_EW_TABPAGE_VISIBLE:
            // Tab 页变为可见时由父窗口发送，重建 D2D 渲染目标
            if (state) {
                RecreateTreeViewRenderTarget(state);
                InvalidateRect(hwnd, nullptr, TRUE);
            }
            return 0;

        case WM_SHOWWINDOW:
            // 从隐藏变为可见时（如 Tab 切换），重建 D2D 渲染目标并强制重绘
            if (wParam == TRUE && state) {
                RecreateTreeViewRenderTarget(state);
                InvalidateRect(hwnd, nullptr, TRUE);
            }
            return DefWindowProc(hwnd, msg, wParam, lParam);
            
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
                    float x_base = clicked_node->level * state->indent_width * state->dpi_scale;
                    float pad_l = state->sidebar_pad_left * state->dpi_scale;
                    float x_cursor = x_base;
                    
                    if (state->sidebar_mode) {
                        x_cursor = x_base + pad_l;
                    } else if (!clicked_node->children.empty()) {
                        float expand_icon_x = x_cursor + 4 * state->dpi_scale;
                        float expand_icon_width = (state->expand_icon_size + 8) * state->dpi_scale;
                        if (x >= expand_icon_x && x < expand_icon_x + expand_icon_width) {
                            if (clicked_node->expanded) {
                                CollapseNode(hwnd, clicked_node->id);
                            } else {
                                ExpandNode(hwnd, clicked_node->id);
                            }
                            return 0;
                        }
                        x_cursor += (state->expand_icon_size + 8) * state->dpi_scale;
                    } else {
                        x_cursor += (state->expand_icon_size + 8) * state->dpi_scale;
                    }
                    
                    if (clicked_node->has_checkbox) {
                        float checkbox_size = 16 * state->dpi_scale;
                        float checkbox_x = x_cursor;
                        if (x >= checkbox_x && x < checkbox_x + checkbox_size) {
                            clicked_node->checked = !clicked_node->checked;
                            if (state->on_node_checked) {
                                state->on_node_checked(clicked_node->id, clicked_node->checked, state->callback_context);
                            }
                            InvalidateRect(hwnd, NULL, FALSE);
                            return 0;
                        }
                    }
                    
                    if (state->sidebar_mode) {
                        if (!clicked_node->children.empty()) {
                            if (!clicked_node->expanded) {
                                ExpandNode(hwnd, clicked_node->id);
                                TreeViewApplySelection(hwnd, state, clicked_node, true, false);
                            } else {
                                CollapseNode(hwnd, clicked_node->id);
                            }
                        } else {
                            TreeViewApplySelection(hwnd, state, clicked_node, true, true);
                        }
                        if (state->drag_enabled) {
                            state->drag_node = clicked_node;
                            state->drag_start_pos.x = x;
                            state->drag_start_pos.y = y;
                            state->is_dragging = false;
                            SetCapture(hwnd);
                        }
                    } else {
                        if (state->selected_node != clicked_node) {
                            SetSelectedNode(hwnd, clicked_node->id);
                        }
                        if (state->drag_enabled) {
                            state->drag_node = clicked_node;
                            state->drag_start_pos.x = x;
                            state->drag_start_pos.y = y;
                            state->is_dragging = false;
                            SetCapture(hwnd);
                        }
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
    if (!state) return;
    if (!state->render_target && !RecreateTreeViewRenderTarget(state)) return;
    
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
    
    // 窗口隐藏后显示时渲染目标可能失效，需重建
    if (hr == D2DERR_RECREATE_TARGET) {
        if (RecreateTreeViewRenderTarget(state)) {
            InvalidateRect(state->hwnd, nullptr, TRUE);
        }
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
    float pad_l = state->sidebar_pad_left * state->dpi_scale;
    float pad_r = state->sidebar_pad_right * state->dpi_scale;
    
    if (!state->sidebar_mode) {
        // 2. 经典树：左侧展开/折叠图标
        if (!node->children.empty()) {
            float icon_x = x_cursor + 4 * state->dpi_scale;
            float icon_y = y_offset + (node_height - state->expand_icon_size * state->dpi_scale) / 2;
            
            RenderExpandIcon(state, icon_x, icon_y, node->expanded);
            x_cursor += (state->expand_icon_size + 8) * state->dpi_scale;
        } else {
            x_cursor += (state->expand_icon_size + 8) * state->dpi_scale;
        }
    } else {
        // 侧栏：左侧不绘制树形三角，仅内容左内边距
        x_cursor = x_base + pad_l;
    }
    
    // 3. 复选框
    if (node->has_checkbox) {
        float checkbox_size = 16 * state->dpi_scale;
        float checkbox_x = x_cursor;
        float checkbox_y = y_offset + (node_height - checkbox_size) / 2;
        
        RenderCheckbox(state, checkbox_x, checkbox_y, checkbox_size, node->checked);
        x_cursor += checkbox_size + 6 * state->dpi_scale;
    }
    
    // 4. emoji 图标（行内垂直居中；RenderEmoji 布局高度=size，与此处计算的 icon_y 一致）
    if (!node->icon.empty()) {
        float icon_x = x_cursor;
        float text_font_size = state->font_size_logical * state->dpi_scale;
        float icon_size = state->icon_size * state->dpi_scale;
        if (icon_size > text_font_size) icon_size = text_font_size;
        float icon_y = y_offset + (node_height - icon_size) * 0.5f;
        if (icon_y < y_offset) {
            icon_y = y_offset;
        }
        
        if (node->enabled && node->selected) {
            brush->SetColor(state->selected_foreground);
        } else {
            brush->SetColor(D2D1::ColorF(0, 0, 0));
        }
        RenderEmoji(state, node->icon.c_str(), icon_x, icon_y, icon_size);
        x_cursor += (state->icon_size + 6) * state->dpi_scale;
    }
    
    // 5. 文本
    if (!node->text.empty() && state->text_format && state->dwrite_factory) {
        D2D1_COLOR_F text_draw_color;
        if (!node->enabled) {
            text_draw_color = D2D1::ColorF(0.6f, 0.6f, 0.6f);
        } else if (node->selected) {
            text_draw_color = state->selected_foreground;
        } else {
            text_draw_color = node->fore_color;
        }
        
        float right_margin = 20.0f * state->dpi_scale;
        if (state->sidebar_mode && !node->children.empty()) {
            right_margin = pad_r + (state->expand_icon_size + 8) * state->dpi_scale;
        } else if (state->sidebar_mode) {
            right_margin = pad_r;
        }
        
        float max_width = static_cast<float>(client_width) - x_cursor - right_margin;
        if (max_width < 8.0f) {
            max_width = 8.0f;
        }
        
        IDWriteTextLayout* text_layout = nullptr;
        HRESULT hr = state->dwrite_factory->CreateTextLayout(
            node->text.c_str(),
            static_cast<UINT32>(node->text.length()),
            state->text_format,
            max_width,
            node_height,
            &text_layout
        );
        
        if (SUCCEEDED(hr) && text_layout) {
            brush->SetColor(text_draw_color);
            rt->DrawTextLayout(
                D2D1::Point2F(x_cursor, y_offset),
                text_layout,
                brush,
                D2D1_DRAW_TEXT_OPTIONS_ENABLE_COLOR_FONT
            );
            text_layout->Release();
        }
    }
    
    // 侧栏：右侧 ∨ / ∧
    if (state->sidebar_mode && !node->children.empty()) {
        float chev = state->expand_icon_size * state->dpi_scale;
        float cx = static_cast<float>(client_width) - pad_r - chev * 0.5f;
        float cy = y_offset + node_height * 0.5f;
        if (node->enabled && node->selected) {
            brush->SetColor(state->selected_foreground);
        } else {
            brush->SetColor(D2D1::ColorF(0.4f, 0.4f, 0.4f));
        }
        RenderSidebarChevron(state, cx, cy, chev, node->expanded);
    }
}

/**
 * 侧栏模式右侧折叠箭头：未展开时 ∨（向下），展开后 ∧（向上）
 */
void RenderSidebarChevron(TreeViewState* state, float center_x, float center_y, float size, bool expanded) {
    if (!state || !state->render_target || !state->brush || !state->d2d_factory) {
        return;
    }
    
    ID2D1HwndRenderTarget* rt = state->render_target;
    ID2D1SolidColorBrush* brush = state->brush;
    
    ID2D1PathGeometry* path = nullptr;
    HRESULT hr = state->d2d_factory->CreatePathGeometry(&path);
    if (FAILED(hr) || !path) {
        return;
    }
    
    ID2D1GeometrySink* sink = nullptr;
    hr = path->Open(&sink);
    if (FAILED(hr) || !sink) {
        SafeRelease(&path);
        return;
    }
    
    if (!expanded) {
        // 向下 ∨
        D2D1_POINT_2F points[3] = {
            D2D1::Point2F(center_x - size * 0.35f, center_y - size * 0.15f),
            D2D1::Point2F(center_x + size * 0.35f, center_y - size * 0.15f),
            D2D1::Point2F(center_x, center_y + size * 0.25f)
        };
        sink->BeginFigure(points[0], D2D1_FIGURE_BEGIN_FILLED);
        sink->AddLine(points[1]);
        sink->AddLine(points[2]);
        sink->EndFigure(D2D1_FIGURE_END_CLOSED);
    } else {
        // 向上 ∧
        D2D1_POINT_2F points[3] = {
            D2D1::Point2F(center_x - size * 0.35f, center_y + size * 0.15f),
            D2D1::Point2F(center_x + size * 0.35f, center_y + size * 0.15f),
            D2D1::Point2F(center_x, center_y - size * 0.25f)
        };
        sink->BeginFigure(points[0], D2D1_FIGURE_BEGIN_FILLED);
        sink->AddLine(points[1]);
        sink->AddLine(points[2]);
        sink->EndFigure(D2D1_FIGURE_END_CLOSED);
    }
    
    sink->Close();
    rt->FillGeometry(path, brush);
    SafeRelease(&sink);
    SafeRelease(&path);
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
 * 使用 CreateTextLayout + DrawTextLayout 并启用 ENABLE_COLOR_FONT 以显示彩色 emoji
 *
 * 布局高度必须与 RenderNode 传入的 size 一致：此前使用 2size×2size 而外侧按 size 做行内垂直居中，
 * 实际墨稿中心与行中心相差约 size/2，导致与右侧段落垂直居中的文字对不齐。
 */
void RenderEmoji(TreeViewState* state, const wchar_t* emoji, float x, float y, float size) {
    if (!state || !emoji || !state->render_target || !state->brush || !state->dwrite_factory || !state->text_format_cache) {
        return;
    }

    ID2D1HwndRenderTarget* rt = state->render_target;
    size_t emoji_len = wcslen(emoji);
    if (emoji_len == 0) return;

    IDWriteTextFormat* emoji_format = state->text_format_cache->GetOrCreate(
        L"Segoe UI Emoji",
        size,
        1.0f
    );
    if (!emoji_format) return;

    IDWriteTextLayout* text_layout = nullptr;
    const float box_h = size;
    const float box_w = (std::max)(size * 1.25f, size + 4.0f);
    HRESULT hr = state->dwrite_factory->CreateTextLayout(
        emoji,
        static_cast<UINT32>(emoji_len),
        emoji_format,
        box_w,
        box_h,
        &text_layout
    );
    if (FAILED(hr) || !text_layout) return;

    text_layout->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
    text_layout->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);

    state->brush->SetColor(D2D1::ColorF(0, 0, 0));
    rt->DrawTextLayout(
        D2D1::Point2F(x, y),
        text_layout,
        state->brush,
        D2D1_DRAW_TEXT_OPTIONS_ENABLE_COLOR_FONT
    );
    text_layout->Release();
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
    float adjusted_y = static_cast<float>(y) + static_cast<float>(state->scroll_pos);
    
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

std::wstring TextFormatCache::GenerateKey(
    const wchar_t* font_family,
    float font_size,
    float dpi_scale,
    DWRITE_FONT_WEIGHT font_weight,
    DWRITE_FONT_STYLE font_style
) {
    wchar_t buffer[320];
    swprintf_s(buffer, L"%s_%.1f_%.2f_%d_%d", font_family, font_size, dpi_scale,
        static_cast<int>(font_weight), static_cast<int>(font_style));
    return std::wstring(buffer);
}

IDWriteTextFormat* TextFormatCache::GetOrCreate(
    const wchar_t* font_family,
    float font_size,
    float dpi_scale,
    DWRITE_FONT_WEIGHT font_weight,
    DWRITE_FONT_STYLE font_style
) {
    if (!dwrite_factory || !font_family) {
        return nullptr;
    }
    
    // 生成缓存键
    std::wstring key = GenerateKey(font_family, font_size, dpi_scale, font_weight, font_style);
    
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
        font_weight,
        font_style,
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
