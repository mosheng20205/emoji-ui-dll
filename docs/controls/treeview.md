# 树形框控件 (TreeView)

[← 返回主文档](../../README.md) | [← 返回文档中心](../README.md)

## 概述

树形框控件用于显示层次结构数据，支持 Emoji 图标、复选框、拖放排序、键盘导航等功能。采用 Element UI 设计风格。

### 侧栏折叠菜单模式

若需要 **左侧缩进 + 右侧 ∨/∧ 箭头**、**Element 式选中条与主色字**、以及 **父级折叠/展开与选中回调** 的细粒度约定，请见 **[侧栏折叠菜单（TreeView 侧栏模式）](./sidebar_menu_treeview.md)**。启用方式：对同一 `CreateTreeView` 返回的句柄调用 **`SetTreeViewSidebarMode(hwnd, true)`**，并配合 **行高 / 行间距 / 全局文字色与选中·悬停色 / 字体** 与 **`MoveTreeViewNode`** 等 API（详见下文「侧栏模式与全局样式」及侧栏专文）。

**仓库内易语言示例（Markdown 文档，可复制到 IDE）：**

- `examples/易语言代码/易语言代码文档/侧栏折叠菜单树形框示例.md` — 侧栏样式与完整菜单树  
- `examples/易语言代码/易语言代码文档/侧栏折叠菜单升级版_方案一分组框.md` — 左侧树 + 右侧多分组框切换  
- `examples/易语言代码/易语言代码文档/侧栏折叠菜单升级版_方案二TabControl.md` — 左侧树 + 右侧 TabControl  

易语言 **DLL 中文命令名** 以 `examples/易语言代码/易语言代码文档/DLL命令.md` 中「树形框功能」为准。

### 可视化设计器（`designer/`）

仓库根目录下的 **Web/Tauri 可视化设计器** 已支持：

| 工具箱项 | 说明 |
|---------|------|
| **树形框** | 普通树；生成代码为 `CreateTreeView` + 节点（设计器内「节点」多行文本，行首两空格表示子级） |
| **侧栏树形框** | 预设侧栏菜单尺寸与样式属性；生成时在建树之后追加 `SetTreeViewRowHeight` / `SetTreeViewItemSpacing` / `SetTreeViewTextColor` / 选中·悬停色 / `SetTreeViewSidebarMode(..., true)`（易语言/C#/Python 模板路径见 `designer/src/data/controlDefs.ts` 与 `designer/src/codegen/`） |

画布上对「侧栏树形框」会显示简化侧栏预览（行高、选中条等示意），便于与右侧内容区布局对齐。

## 创建树形框

签名与 `emoji_window/treeview_window.h` 一致：

```c++
HWND __stdcall CreateTreeView(
    HWND parent,
    int x, int y,
    int width, int height,
    unsigned int bg_color,
    unsigned int text_color,
    void* callback_context
);
```

### 参数说明

| 参数 | 说明 |
|------|------|
| `parent` | 父窗口句柄 |
| `x`, `y` | 控件客户区位置（逻辑单位） |
| `width`, `height` | 控件尺寸 |
| `bg_color` | 背景色（ARGB，例如 `0xFFFFFFFF`） |
| `text_color` | 默认文本色（ARGB） |
| `callback_context` | 回调上下文指针；无特殊需求时传 `NULL` / 易语言中传 `0` |

**返回值：** 树形框窗口句柄；失败返回 `NULL`（易语言中与 `0` 比较）

## 节点管理

### 添加根节点

```c++
int __stdcall AddRootNode(
    HWND hTreeView,
    const unsigned char* text_bytes, int text_len,
    const unsigned char* icon_bytes, int icon_len
);
```

### 添加子节点

```c++
int __stdcall AddChildNode(
    HWND hTreeView,
    int parent_node_id,
    const unsigned char* text_bytes, int text_len,
    const unsigned char* icon_bytes, int icon_len
);
```

### 删除节点

```c++
BOOL __stdcall RemoveNode(HWND hTreeView, int node_id);
```

### 清空树形框

```c++
BOOL __stdcall ClearTree(HWND hTreeView);
```

## 节点展开/折叠

```c++
BOOL __stdcall ExpandNode(HWND hTreeView, int node_id);
BOOL __stdcall CollapseNode(HWND hTreeView, int node_id);
BOOL __stdcall ExpandAll(HWND hTreeView);
BOOL __stdcall CollapseAll(HWND hTreeView);
BOOL __stdcall IsNodeExpanded(HWND hTreeView, int node_id);
```

## 节点选择

```c++
BOOL __stdcall SetSelectedNode(HWND hTreeView, int node_id);
int  __stdcall GetSelectedNode(HWND hTreeView);
```

## 节点属性

### 文本和图标

```c++
// 设置/获取节点文本
BOOL __stdcall SetNodeText(HWND hTreeView, int node_id, const unsigned char* text_bytes, int text_len);
int  __stdcall GetNodeText(HWND hTreeView, int node_id, unsigned char* buffer, int buffer_size);

// 设置/获取节点图标
BOOL __stdcall SetNodeIcon(HWND hTreeView, int node_id, const unsigned char* icon_bytes, int icon_len);
int  __stdcall GetNodeIcon(HWND hTreeView, int node_id, unsigned char* buffer, int buffer_size);
```

### 颜色和状态

```c++
// 设置节点颜色
BOOL __stdcall SetNodeForeColor(HWND hTreeView, int node_id, UINT32 color);
BOOL __stdcall SetNodeBackColor(HWND hTreeView, int node_id, UINT32 color);

// 启用/禁用节点
BOOL __stdcall SetNodeEnabled(HWND hTreeView, int node_id, BOOL enabled);
BOOL __stdcall IsNodeEnabled(HWND hTreeView, int node_id);
```

## 复选框功能

```c++
// 显示/隐藏节点复选框
BOOL __stdcall SetNodeCheckBox(HWND hTreeView, int node_id, BOOL show);

// 设置/获取复选框状态
BOOL __stdcall SetNodeChecked(HWND hTreeView, int node_id, BOOL checked);
BOOL __stdcall GetNodeChecked(HWND hTreeView, int node_id);
```

## 节点查找

```c++
// 根据文本查找节点
int __stdcall FindNodeByText(HWND hTreeView, const unsigned char* text_bytes, int text_len);

// 获取节点关系
int __stdcall GetNodeParent(HWND hTreeView, int node_id);
int __stdcall GetNodeChildCount(HWND hTreeView, int node_id);
int __stdcall GetNodeChildren(HWND hTreeView, int node_id, unsigned char* buffer, int buffer_size);
int __stdcall GetNodeLevel(HWND hTreeView, int node_id);
```

## 滚动控制

```c++
BOOL __stdcall ScrollToNode(HWND hTreeView, int node_id);
BOOL __stdcall SetTreeViewScrollPos(HWND hTreeView, int position);
int  __stdcall GetTreeViewScrollPos(HWND hTreeView);
```

## 侧栏模式与全局样式（折叠菜单）

以下 API 与 `CreateTreeView` 返回的 **树形框句柄** 相同，用于侧栏菜单与主题调节（详见 [sidebar_menu_treeview.md](./sidebar_menu_treeview.md)）。

```c++
BOOL __stdcall SetTreeViewSidebarMode(HWND hwnd, bool enable);
BOOL __stdcall GetTreeViewSidebarMode(HWND hwnd);

BOOL __stdcall SetTreeViewRowHeight(HWND hwnd, float height);
float __stdcall GetTreeViewRowHeight(HWND hwnd);

BOOL __stdcall SetTreeViewItemSpacing(HWND hwnd, float spacing);
float __stdcall GetTreeViewItemSpacing(HWND hwnd);

BOOL __stdcall SetTreeViewTextColor(HWND hwnd, unsigned int argb);
unsigned int __stdcall GetTreeViewTextColor(HWND hwnd);

BOOL __stdcall SetTreeViewSelectedBgColor(HWND hwnd, unsigned int argb);
unsigned int __stdcall GetTreeViewSelectedBgColor(HWND hwnd);

BOOL __stdcall SetTreeViewSelectedForeColor(HWND hwnd, unsigned int argb);
unsigned int __stdcall GetTreeViewSelectedForeColor(HWND hwnd);

BOOL __stdcall SetTreeViewHoverBgColor(HWND hwnd, unsigned int argb);
unsigned int __stdcall GetTreeViewHoverBgColor(HWND hwnd);

BOOL __stdcall SetTreeViewFont(
    HWND hwnd,
    const unsigned char* family_utf8, int family_len,
    float font_size, int font_weight, bool italic
);
BOOL __stdcall GetTreeViewFont(
    HWND hwnd,
    unsigned char* family_buf, int family_buf_size,
    float* out_font_size, int* out_font_weight, bool* out_italic
);

BOOL __stdcall MoveTreeViewNode(HWND hwnd, int node_id, int new_parent_id, int insert_index);
```

## 拖放功能

```c++
BOOL __stdcall EnableTreeViewDragDrop(HWND hTreeView, BOOL enable);
```

启用后，用户可以通过拖动节点来重新排列树形结构。

## 事件回调

### 设置回调函数

```c++
BOOL __stdcall SetTreeViewCallback(
    HWND hTreeView,
    int callback_type,
    void* callback_function
);
```

### 回调类型常量

| 常量 | 值 | 说明 | 回调签名 |
|------|-----|------|---------|
| `CALLBACK_NODE_SELECTED` | 1 | 节点被选中 | `(node_id, context)` |
| `CALLBACK_NODE_EXPANDED` | 2 | 节点被展开 | `(node_id, context)` |
| `CALLBACK_NODE_COLLAPSED` | 3 | 节点被折叠 | `(node_id, context)` |
| `CALLBACK_NODE_DOUBLE_CLICK` | 4 | 节点被双击 | `(node_id, context)` |
| `CALLBACK_NODE_RIGHT_CLICK` | 5 | 节点被右键点击 | `(node_id, x, y, context)` |
| `CALLBACK_NODE_TEXT_CHANGED` | 6 | 节点文本改变 | `(node_id, text, text_len, context)` |
| `CALLBACK_NODE_CHECKED` | 7 | 节点复选框改变 | `(node_id, checked, context)` |
| `CALLBACK_NODE_MOVED` | 8 | 节点被移动 | `(node_id, new_parent_id, new_index, context)` |

## 树形框特性

- ✅ **Emoji 图标**：支持以 UTF-8 字节集作为节点图标（彩色 Emoji）
- ✅ **复选框**：可为任意节点添加复选框
- ✅ **拖放排序**：支持拖动节点重新排列（需 `EnableTreeViewDragDrop`）
- ✅ **侧栏模式**：`SetTreeViewSidebarMode` 开启后右侧 ∨/∧、全局行高/间距/主题色（见侧栏专文）
- ✅ **键盘导航**：方向键、Home/End、PageUp/Down
- ✅ **滚轮滚动**：平滑滚动支持
- ✅ **自定义颜色**：每节点前景/背景；侧栏模式下另有全局默认字色与选中·悬停色
- ✅ **层级缩进**：自动计算层级缩进
- ✅ **展开/折叠动画**：平滑的展开折叠效果
- ✅ **Element UI 风格**：统一的视觉设计

## 易语言完整示例

以下为示意代码：若 IDE 中不能直接写 Emoji，请改为 **UTF-8 字节集**（参见《侧栏折叠菜单树形框示例》）。DLL 声明使用中文命令名（与 `examples/易语言代码/易语言代码文档/DLL命令.md` 中「树形框功能」一致）；引号内仍为英文导出名。

**创建树形框** 时参数顺序为：`父窗口, X, Y, 宽, 高, 背景色 ARGB, 文本色 ARGB, 回调上下文`（与上表一致）。

```
.版本 2

.程序集变量 窗口句柄, 整数型
.程序集变量 树形框句柄, 整数型
.程序集变量 根节点1, 整数型
.程序集变量 子节点1, 整数型

.子程序 _启动窗口_创建完毕

' 创建窗口
窗口句柄 = 创建Emoji窗口 ("树形框示例", 500, 600)

' 创建树形框
树形框句柄 = 创建树形框 (窗口句柄, 20, 20, 450, 550, #FFFFFFFF, #FF000000, 0)

如果 (树形框句柄 = 0)
    信息框 ("创建树形框失败！", 0, )
    返回 ()
如果结束

' 设置回调函数
设置树形框回调 (树形框句柄, 1, &节点选中回调)
设置树形框回调 (树形框句柄, 2, &节点展开回调)
设置树形框回调 (树形框句柄, 7, &节点复选框回调)

' 添加节点
.局部变量 文本, 字节集
.局部变量 图标, 字节集

' 添加根节点 - 文档文件夹
文本 = 到UTF8 ("📁 我的文档")
图标 = 到UTF8 ("📁")
根节点1 = 添加根节点 (树形框句柄, 文本, 取字节集长度 (文本), 图标, 取字节集长度 (图标))

' 添加子节点 - Word 文档
文本 = 到UTF8 ("📄 工作报告.docx")
图标 = 到UTF8 ("📄")
子节点1 = 添加子节点 (树形框句柄, 根节点1, 文本, 取字节集长度 (文本), 图标, 取字节集长度 (图标))

' 设置复选框
设置树节点复选框 (树形框句柄, 根节点1, 真)
设置树节点复选框 (树形框句柄, 子节点1, 真)

' 展开根节点
展开树节点 (树形框句柄, 根节点1)

调试输出 ("树形框创建完成！")


.子程序 节点选中回调, , 公开, , 节点ID, 整数型, , 上下文, 整数型

调试输出 ("节点被选中: ID = " + 到文本 (节点ID))

' 获取节点信息
.局部变量 父节点ID, 整数型
.局部变量 层级, 整数型
.局部变量 子节点数, 整数型

父节点ID = 取树节点父节点 (树形框句柄, 节点ID)
层级 = 取树节点层级 (树形框句柄, 节点ID)
子节点数 = 取树子节点数量 (树形框句柄, 节点ID)

调试输出 ("  父节点: " + 到文本 (父节点ID))
调试输出 ("  层级: " + 到文本 (层级))
调试输出 ("  子节点数: " + 到文本 (子节点数))


.子程序 节点展开回调, , 公开, , 节点ID, 整数型, , 上下文, 整数型

调试输出 ("节点被展开: ID = " + 到文本 (节点ID))


.子程序 节点复选框回调, , 公开, , 节点ID, 整数型, 选中状态, 逻辑型, 上下文, 整数型

调试输出 ("节点复选框改变: ID = " + 到文本 (节点ID) + ", 选中: " + 到文本 (选中状态))


.子程序 _按钮_展开所有_被单击

展开全部树节点 (树形框句柄)
调试输出 ("展开所有节点")


.子程序 _按钮_折叠所有_被单击

折叠全部树节点 (树形框句柄)
调试输出 ("折叠所有节点")


.子程序 _按钮_启用拖放_被单击

启用树形框拖放 (树形框句柄, 真)
调试输出 ("启用拖放功能")


.子程序 _按钮_查找节点_被单击

.局部变量 文本, 字节集
.局部变量 查找结果, 整数型

文本 = 到UTF8 ("工作报告.docx")
查找结果 = 按文本查找树节点 (树形框句柄, 文本, 取字节集长度 (文本))

如果 (查找结果 > 0)
    调试输出 ("找到节点: ID = " + 到文本 (查找结果))
    设置选中树节点 (树形框句柄, 查找结果)
    滚动到树节点 (树形框句柄, 查找结果)
否则
    调试输出 ("未找到节点")
如果结束


.子程序 __启动窗口_可否被关闭, 逻辑型

' 清理资源
如果 (树形框句柄 ≠ 0)
    销毁树形框 (树形框句柄)
如果结束

返回 (真)
```

## 高级用法

### 动态加载子节点

```
.子程序 节点展开回调, , 公开, , 节点ID, 整数型, , 上下文, 整数型

' 检查是否已加载子节点
.局部变量 子节点数, 整数型
子节点数 = 取树子节点数量 (树形框句柄, 节点ID)

如果 (子节点数 = 0)
    ' 动态加载子节点
    调用子程序 (&加载子节点, 节点ID)
如果结束


.子程序 加载子节点
.参数 父节点ID, 整数型

' 从数据库或文件系统加载子节点
' ...
```

### 自定义节点样式

```
' 设置重要节点为红色
设置树节点前景色 (树形框句柄, 节点ID, #FFFF0000)

' 设置选中节点为黄色背景
设置树节点背景色 (树形框句柄, 节点ID, #FFFFFF00)

' 禁用某些节点
设置树节点启用 (树形框句柄, 节点ID, 假)
```

### 实现右键菜单

```
.子程序 节点右键回调, , 公开, , 节点ID, 整数型, X坐标, 整数型, Y坐标, 整数型, 上下文, 整数型

' 显示右键菜单
弹出菜单 (右键菜单, X坐标, Y坐标)

' 保存当前右键的节点ID
当前右键节点 = 节点ID
```

## 性能建议

- 大量节点时使用动态加载（展开时才加载子节点）
- 避免频繁调用 `展开全部树节点()`，可能导致性能问题
- 使用 `按文本查找树节点()` 前先缓存节点ID
- 拖放功能会增加内存占用，不需要时应禁用

## 键盘快捷键

| 按键 | 功能 |
|------|------|
| ↑/↓ | 上下移动选中节点 |
| ←/→ | 折叠/展开节点 |
| Home | 跳到第一个节点 |
| End | 跳到最后一个节点 |
| PageUp/PageDown | 翻页 |
| Space | 切换复选框状态 |
| Enter | 展开/折叠节点 |

## 技术说明

- **渲染引擎**：Direct2D
- **文本渲染**：DirectWrite
- **Emoji 支持**：完整支持彩色 Emoji
- **滚动**：自定义滚动条，支持平滑滚动
- **拖放**：使用 Windows 拖放 API
- **动画**：展开/折叠使用缓动动画

## 相关文档

- [侧栏折叠菜单（TreeView 侧栏模式）](./sidebar_menu_treeview.md) — 侧栏交互与主题 API 详解
- [列表框](listbox.md) — 简单列表
- [DataGridView](datagridview.md) — 表格数据
- [常见问题](../faq.md)
