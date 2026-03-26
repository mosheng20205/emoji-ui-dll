# 侧栏折叠菜单（TreeView 侧栏模式）

[← 返回主文档](../../README.md) | [← 树形框基础文档](./treeview.md)

## 概述

在 **同一套** `CreateTreeView` 控件上，通过 **`SetTreeViewSidebarMode(真)`** 切换为「后台管理式」侧栏菜单：

- **左侧**不再显示经典树形小三角；**层级缩进**仍按 `indent_width` 与 `level` 递增。
- **右侧**对有子节点的行显示 **∨（折叠）/ ∧（展开）**。
- 支持 **无限层级**、**彩色 Emoji + 文本**（`SetNodeIcon` + `SetNodeText`，或整段写在 `text` 中）。
- **全局样式**：行高、行间距、默认字色、选中背景/前景、悬停背景、字体（族名/字号/字重/斜体）均可运行时读写。

## 交互约定（侧栏模式）

| 场景 | 行为 |
|------|------|
| **有子节点且当前为折叠** | 点击整行（含图标、文字、右侧箭头）→ **展开** → **选中该节点** → 触发 `CALLBACK_NODE_SELECTED` |
| **有子节点且当前已展开** | 再次点击同一行 → **仅折叠** → **不**触发 `CALLBACK_NODE_SELECTED` |
| **叶子节点** | 点击整行 → **选中**；每次点击都会触发 `CALLBACK_NODE_SELECTED`（含重复点击同一叶子） |

复选框区域仍优先处理：点在复选框上只切换勾选，不走上述整行逻辑。

## 新增 / 相关的 C API（导出）

### 侧栏开关

| 函数 | 说明 |
|------|------|
| `bool SetTreeViewSidebarMode(HWND hwnd, bool enable)` | 开启/关闭侧栏模式 |
| `bool GetTreeViewSidebarMode(HWND hwnd)` | 查询是否为侧栏模式 |

### 布局

| 函数 | 说明 |
|------|------|
| `bool SetTreeViewRowHeight(HWND hwnd, float height)` | 行高（逻辑单位，建议 ≥4） |
| `float GetTreeViewRowHeight(HWND hwnd)` | 读取行高 |
| `bool SetTreeViewItemSpacing(HWND hwnd, float spacing)` | 行间额外间距（逻辑单位） |
| `float GetTreeViewItemSpacing(HWND hwnd)` | 读取行间距 |

### 颜色（ARGB，`0xAARRGGBB`）

| 函数 | 说明 |
|------|------|
| `bool SetTreeViewTextColor(HWND hwnd, unsigned int argb)` | **全局**默认文字色；会同步到已有全部节点的 `fore_color` |
| `unsigned int GetTreeViewTextColor(HWND hwnd)` | 读取默认文字色 |
| `bool SetTreeViewSelectedBgColor(HWND hwnd, unsigned int argb)` | 选中行背景 |
| `unsigned int GetTreeViewSelectedBgColor(HWND hwnd)` | 读取选中背景 |
| `bool SetTreeViewSelectedForeColor(HWND hwnd, unsigned int argb)` | 选中时文字、左侧 emoji、右侧箭头颜色 |
| `unsigned int GetTreeViewSelectedForeColor(HWND hwnd)` | 读取选中前景 |
| `bool SetTreeViewHoverBgColor(HWND hwnd, unsigned int argb)` | 悬停行背景 |
| `unsigned int GetTreeViewHoverBgColor(HWND hwnd)` | 读取悬停背景 |

### 字体

| 函数 | 说明 |
|------|------|
| `bool SetTreeViewFont(HWND hwnd, const unsigned char* family_utf8, int family_len, float font_size, int font_weight, bool italic)` | `font_weight` 如 400=正常，700=粗体 |
| `bool GetTreeViewFont(HWND hwnd, unsigned char* family_buf, int family_buf_size, float* out_size, int* out_weight, bool* out_italic)` | 字体族名 UTF-8 写入缓冲区；`out_*` 可为 `NULL` |

设置字体后会清空内部 `TextFormatCache` 并重建布局。

### 程序化移动节点

| 函数 | 说明 |
|------|------|
| `bool MoveTreeViewNode(HWND hwnd, int node_id, int new_parent_id, int insert_index)` | `new_parent_id == -1` 表示移到**根列表**；`insert_index` 为在兄弟中的插入位置。禁止将节点移入自己的子树（避免成环）。成功后会触发 `CALLBACK_NODE_MOVED`（若已注册）。 |

## 与经典树形模式的关系

- **关闭侧栏模式**（`SetTreeViewSidebarMode(假)`）后，恢复左侧展开三角、原有点击逻辑（点左侧三角展开/折叠，点行选中）。
- 侧栏专用 API（行高、间距、三色、字体等）在两种模式下均生效，仅影响绘制与布局。

## 易语言示例

见仓库：`examples/易语言代码/侧栏折叠菜单树形框示例.e`（`.程序集 程序集1`，入口 `_启动子程序`；末尾 `设置消息循环主窗口` → `运行消息循环` → `返回 (0)`。）  
易语言 IDE 中 Emoji/特殊符号勿直接写在源码里时，说明与 UTF-8 字节示例见：`examples/易语言代码/易语言代码文档/侧栏折叠菜单树形框示例.md`。  
易语言侧 **DLL 命令名**（如 `设置树形框侧栏模式`、`设置树形框选中前景色`、`设置树形框回调`）以 `examples/易语言代码/易语言代码文档/DLL命令.md` 为准；**引号内英文**为 DLL 导出函数名，与 C API 文档一致。
