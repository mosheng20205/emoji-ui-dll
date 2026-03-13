#pragma once

#ifndef TREEVIEW_WINDOW_H
#define TREEVIEW_WINDOW_H

#include <windows.h>

// 回调类型枚举
enum TreeViewCallbackType {
    CALLBACK_NODE_SELECTED = 1,      // 节点被选中
    CALLBACK_NODE_EXPANDED = 2,      // 节点被展开
    CALLBACK_NODE_COLLAPSED = 3,     // 节点被折叠
    CALLBACK_NODE_DOUBLE_CLICK = 4,  // 节点被双击
    CALLBACK_NODE_RIGHT_CLICK = 5,   // 节点被右键点击
    CALLBACK_NODE_TEXT_CHANGED = 6,  // 节点文本改变
    CALLBACK_NODE_CHECKED = 7,       // 节点复选框改变
    CALLBACK_NODE_MOVED = 8          // 节点被移动
};

// 回调函数类型定义

// 基础回调：void callback(int node_id, void* context)
typedef void(__stdcall* TreeViewCallback)(int node_id, void* context);

// 文本改变回调：void callback(int node_id, const char* new_text, int text_len, void* context)
typedef void(__stdcall* TreeViewTextCallback)(int node_id, const unsigned char* new_text, int text_len, void* context);

// 复选框回调：void callback(int node_id, bool checked, void* context)
typedef void(__stdcall* TreeViewCheckCallback)(int node_id, bool checked, void* context);

// 移动回调：void callback(int node_id, int new_parent_id, int new_index, void* context)
typedef void(__stdcall* TreeViewMoveCallback)(int node_id, int new_parent_id, int new_index, void* context);

// 右键点击回调：void callback(int node_id, int x, int y, void* context)
typedef void(__stdcall* TreeViewRightClickCallback)(int node_id, int x, int y, void* context);

#ifdef __cplusplus
extern "C" {
#endif

// ============================================================================
// 控件生命周期管理
// ============================================================================

/**
 * 创建树形框控件
 * @param parent 父窗口句柄
 * @param x X坐标（逻辑单位）
 * @param y Y坐标（逻辑单位）
 * @param width 宽度（逻辑单位）
 * @param height 高度（逻辑单位）
 * @param bg_color 背景色 (ARGB)
 * @param text_color 文本颜色 (ARGB)
 * @param callback_context 回调上下文指针
 * @return 树形框窗口句柄，失败返回 NULL
 */
HWND __stdcall CreateTreeView(
    HWND parent,
    int x, int y,
    int width, int height,
    unsigned int bg_color,
    unsigned int text_color,
    void* callback_context
);

/**
 * 销毁树形框控件
 * @param hwnd 树形框窗口句柄
 * @return 成功返回 true，失败返回 false
 */
bool __stdcall DestroyTreeView(HWND hwnd);

// ============================================================================
// 节点管理
// ============================================================================

/**
 * 添加根节点
 * @param hwnd 树形框窗口句柄
 * @param text UTF-8 编码的节点文本
 * @param text_len 文本字节长度
 * @param icon UTF-8 编码的图标（emoji）
 * @param icon_len 图标字节长度
 * @return 节点 ID（>0），失败返回 -1
 */
int __stdcall AddRootNode(
    HWND hwnd,
    const unsigned char* text,
    int text_len,
    const unsigned char* icon,
    int icon_len
);

/**
 * 添加子节点
 * @param hwnd 树形框窗口句柄
 * @param parent_id 父节点 ID
 * @param text UTF-8 编码的节点文本
 * @param text_len 文本字节长度
 * @param icon UTF-8 编码的图标（emoji）
 * @param icon_len 图标字节长度
 * @return 节点 ID（>0），失败返回 -1
 */
int __stdcall AddChildNode(
    HWND hwnd,
    int parent_id,
    const unsigned char* text,
    int text_len,
    const unsigned char* icon,
    int icon_len
);

/**
 * 删除节点（包括所有子节点）
 * @param hwnd 树形框窗口句柄
 * @param node_id 节点 ID
 * @return 成功返回 true，失败返回 false
 */
bool __stdcall RemoveNode(HWND hwnd, int node_id);

/**
 * 清空树形框（删除所有节点）
 * @param hwnd 树形框窗口句柄
 * @return 成功返回 true，失败返回 false
 */
bool __stdcall ClearTree(HWND hwnd);

// ============================================================================
// 节点展开/折叠
// ============================================================================

/**
 * 展开节点
 * @param hwnd 树形框窗口句柄
 * @param node_id 节点 ID
 * @return 成功返回 true，失败返回 false
 */
bool __stdcall ExpandNode(HWND hwnd, int node_id);

/**
 * 折叠节点
 * @param hwnd 树形框窗口句柄
 * @param node_id 节点 ID
 * @return 成功返回 true，失败返回 false
 */
bool __stdcall CollapseNode(HWND hwnd, int node_id);

/**
 * 展开所有节点
 * @param hwnd 树形框窗口句柄
 * @return 成功返回 true，失败返回 false
 */
bool __stdcall ExpandAll(HWND hwnd);

/**
 * 折叠所有节点
 * @param hwnd 树形框窗口句柄
 * @return 成功返回 true，失败返回 false
 */
bool __stdcall CollapseAll(HWND hwnd);

/**
 * 检查节点是否展开
 * @param hwnd 树形框窗口句柄
 * @param node_id 节点 ID
 * @return 展开返回 true，折叠或失败返回 false
 */
bool __stdcall IsNodeExpanded(HWND hwnd, int node_id);

// ============================================================================
// 节点选择
// ============================================================================

/**
 * 设置选中节点
 * @param hwnd 树形框窗口句柄
 * @param node_id 节点 ID
 * @return 成功返回 true，失败返回 false
 */
bool __stdcall SetSelectedNode(HWND hwnd, int node_id);

/**
 * 获取选中节点
 * @param hwnd 树形框窗口句柄
 * @return 选中节点的 ID，没有选中返回 -1
 */
int __stdcall GetSelectedNode(HWND hwnd);

// ============================================================================
// 节点属性
// ============================================================================

/**
 * 设置节点文本
 * @param hwnd 树形框窗口句柄
 * @param node_id 节点 ID
 * @param text UTF-8 编码的文本
 * @param text_len 文本字节长度
 * @return 成功返回 true，失败返回 false
 */
bool __stdcall SetNodeText(
    HWND hwnd,
    int node_id,
    const unsigned char* text,
    int text_len
);

/**
 * 获取节点文本
 * @param hwnd 树形框窗口句柄
 * @param node_id 节点 ID
 * @param buffer 接收文本的缓冲区
 * @param buffer_size 缓冲区大小
 * @return 实际文本字节长度，失败返回 -1
 */
int __stdcall GetNodeText(
    HWND hwnd,
    int node_id,
    unsigned char* buffer,
    int buffer_size
);

/**
 * 设置节点图标
 * @param hwnd 树形框窗口句柄
 * @param node_id 节点 ID
 * @param icon UTF-8 编码的图标（emoji）
 * @param icon_len 图标字节长度
 * @return 成功返回 true，失败返回 false
 */
bool __stdcall SetNodeIcon(
    HWND hwnd,
    int node_id,
    const unsigned char* icon,
    int icon_len
);

/**
 * 获取节点图标
 * @param hwnd 树形框窗口句柄
 * @param node_id 节点 ID
 * @param buffer 接收图标的缓冲区
 * @param buffer_size 缓冲区大小
 * @return 实际图标字节长度，失败返回 -1
 */
int __stdcall GetNodeIcon(
    HWND hwnd,
    int node_id,
    unsigned char* buffer,
    int buffer_size
);

/**
 * 设置节点前景色
 * @param hwnd 树形框窗口句柄
 * @param node_id 节点 ID
 * @param color 颜色值 (ARGB)
 * @return 成功返回 true，失败返回 false
 */
bool __stdcall SetNodeForeColor(HWND hwnd, int node_id, unsigned int color);

/**
 * 设置节点背景色
 * @param hwnd 树形框窗口句柄
 * @param node_id 节点 ID
 * @param color 颜色值 (ARGB)
 * @return 成功返回 true，失败返回 false
 */
bool __stdcall SetNodeBackColor(HWND hwnd, int node_id, unsigned int color);

/**
 * 设置节点启用状态
 * @param hwnd 树形框窗口句柄
 * @param node_id 节点 ID
 * @param enabled 是否启用
 * @return 成功返回 true，失败返回 false
 */
bool __stdcall SetNodeEnabled(HWND hwnd, int node_id, bool enabled);

/**
 * 检查节点是否启用
 * @param hwnd 树形框窗口句柄
 * @param node_id 节点 ID
 * @return 启用返回 true，禁用或失败返回 false
 */
bool __stdcall IsNodeEnabled(HWND hwnd, int node_id);

// ============================================================================
// 复选框
// ============================================================================

/**
 * 设置节点复选框显示
 * @param hwnd 树形框窗口句柄
 * @param node_id 节点 ID
 * @param show 是否显示复选框
 * @return 成功返回 true，失败返回 false
 */
bool __stdcall SetNodeCheckBox(HWND hwnd, int node_id, bool show);

/**
 * 设置节点复选框状态
 * @param hwnd 树形框窗口句柄
 * @param node_id 节点 ID
 * @param checked 是否选中
 * @return 成功返回 true，失败返回 false
 */
bool __stdcall SetNodeChecked(HWND hwnd, int node_id, bool checked);

/**
 * 获取节点复选框状态
 * @param hwnd 树形框窗口句柄
 * @param node_id 节点 ID
 * @return 选中返回 1，未选中返回 0，失败返回 0
 */
BOOL __stdcall GetNodeChecked(HWND hwnd, int node_id);

// ============================================================================
// 节点查找
// ============================================================================

/**
 * 根据文本查找节点
 * @param hwnd 树形框窗口句柄
 * @param text UTF-8 编码的文本
 * @param text_len 文本字节长度
 * @return 第一个匹配节点的 ID，未找到返回 -1
 */
int __stdcall FindNodeByText(
    HWND hwnd,
    const unsigned char* text,
    int text_len
);

/**
 * 获取节点的父节点
 * @param hwnd 树形框窗口句柄
 * @param node_id 节点 ID
 * @return 父节点 ID，根节点或失败返回 -1
 */
int __stdcall GetNodeParent(HWND hwnd, int node_id);

/**
 * 获取节点的子节点数量
 * @param hwnd 树形框窗口句柄
 * @param node_id 节点 ID
 * @return 子节点数量，失败返回 -1
 */
int __stdcall GetNodeChildCount(HWND hwnd, int node_id);

/**
 * 获取节点的所有子节点 ID
 * @param hwnd 树形框窗口句柄
 * @param node_id 节点 ID
 * @param buffer 接收子节点 ID 的缓冲区
 * @param buffer_size 缓冲区大小（元素数量）
 * @return 实际子节点数量，失败返回 -1
 */
int __stdcall GetNodeChildren(
    HWND hwnd,
    int node_id,
    int* buffer,
    int buffer_size
);

/**
 * 获取节点的层级深度
 * @param hwnd 树形框窗口句柄
 * @param node_id 节点 ID
 * @return 层级深度（根节点为 0），失败返回 -1
 */
int __stdcall GetNodeLevel(HWND hwnd, int node_id);

// ============================================================================
// 滚动控制
// ============================================================================

/**
 * 滚动到指定节点
 * @param hwnd 树形框窗口句柄
 * @param node_id 节点 ID
 * @return 成功返回 true，失败返回 false
 */
bool __stdcall ScrollToNode(HWND hwnd, int node_id);

/**
 * 设置滚动位置
 * @param hwnd 树形框窗口句柄
 * @param pos 滚动位置（像素）
 * @return 成功返回 true，失败返回 false
 */
bool __stdcall SetTreeViewScrollPos(HWND hwnd, int pos);

/**
 * 获取滚动位置
 * @param hwnd 树形框窗口句柄
 * @return 当前滚动位置（像素），失败返回 -1
 */
int __stdcall GetTreeViewScrollPos(HWND hwnd);

// ============================================================================
// 回调设置
// ============================================================================

/**
 * 设置树形框回调函数
 * @param hwnd 树形框窗口句柄
 * @param callback_type 回调类型（TreeViewCallbackType 枚举）
 * @param callback_func 回调函数指针
 * @return 成功返回 true，失败返回 false
 */
bool __stdcall SetTreeViewCallback(
    HWND hwnd,
    int callback_type,
    void* callback_func
);

// ============================================================================
// 拖放控制
// ============================================================================

/**
 * 启用/禁用拖放功能
 * @param hwnd 树形框窗口句柄
 * @param enable 是否启用
 * @return 成功返回 true，失败返回 false
 */
bool __stdcall EnableTreeViewDragDrop(HWND hwnd, bool enable);

#ifdef __cplusplus
}
#endif

#endif // TREEVIEW_WINDOW_H
