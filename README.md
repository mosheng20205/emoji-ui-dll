# Emoji Window DLL - C++ 版本

使用 C++ 和 Direct2D/DirectWrite 实现的 Windows UI 控件库，完美支持彩色 Emoji 显示。提供 16 种控件、布局管理器、主题系统和扩展事件系统，专为易语言应用设计。

## ✨ 特性

- 🎨 **16 种控件**：按钮、编辑框、复选框、单选按钮、进度条、列表框、组合框、表格等
- 📋 **菜单系统**：顶部菜单栏、右键弹出菜单，支持 Emoji 和子菜单
- 🌈 **主题系统**：支持亮色/暗色主题切换，可自定义 JSON 主题
- 📐 **布局管理器**：流式布局、网格布局、停靠布局，自动响应窗口大小
- 🚀 **高性能**：表格虚拟模式支持 100000+ 行数据
- 🎯 **Element UI 风格**：统一的视觉设计，现代化界面
- 💾 **易语言友好**：完整的 DLL 声明和示例代码

## 📑 目录

- [快速开始](#-快速开始)
- [可视化设计器](#-可视化设计器)
- [AI Skill 技能包](#-ai-skill-技能包)
- [项目结构](#-项目结构)
- [编译步骤](#-编译步骤)
- [控件文档](#-控件文档)
- [核心功能](#-核心功能)
- [控件属性操作](#-控件属性操作)
- [完整控件列表](#-完整控件列表)
- [查看截图](#-查看截图)
- [常见问题](#-常见问题)
- [性能优化](#-性能优化)
- [技术细节](#-技术细节)
- [打赏支持](#-打赏支持)

---

## 🚀 快速开始

### 1. 下载 DLL

从 [Releases](../../releases) 下载最新版本的 `emoji_window.dll`，或自行编译。

### 2. 在易语言中引入

```
' 复制 DLL 到程序目录
' 导入 易语言代码/DLL命令.e
' 导入 易语言代码/常量表.e
' 导入 易语言代码/辅助程序集.e
```

### 3. 创建第一个窗口

```
.版本 2

.程序集变量 窗口句柄, 整数型
.程序集变量 按钮ID, 整数型

.子程序 _启动窗口_创建完毕

窗口句柄 = 创建Emoji窗口 ("我的第一个窗口", 800, 600)

按钮ID = 创建Emoji按钮_辅助 (窗口句柄, "🎉", "点击我", 50, 50, 120, 40, #COLOR_PRIMARY)

设置按钮点击回调 (&按钮点击处理)

运行消息循环 ()


.子程序 按钮点击处理, , 公开, stdcall
.参数 按钮ID_, 整数型

信息框 ("按钮被点击了！", 0, "提示")
```

---

## 🖥️ 可视化设计器

项目新增了基于 Rust（Tauri v2）打包的桌面可视化设计器，无需手写代码即可搭建界面。

### 核心能力

- **拖拽式搭建**：从工具箱拖入控件到画布，可视化调整位置和大小
- **一键生成代码**：支持生成易语言、Python、C# 三种语言的完整调用代码
- **AI 对话式设计**：内置 AI 助手，用自然语言描述需求，自动生成界面布局
- **多供应商 AI 适配**：支持 OpenAI、Anthropic、Gemini、DeepSeek、阿里通义
- **设计导入/导出**：布局可导出为 JSON，方便保存、共享和团队协作
- **Emoji 选择器**：内置分类 Emoji 面板，支持搜索和最近使用记录
- **广告与统计系统**：内置可配置的广告轮播和使用统计上报

### 下载安装

从 [Releases](../../releases) 页面下载最新安装包（`.exe` 或 `.msi`）。

### 本地开发

```bash
cd designer
npm install
npm run dev          # 启动前端开发服务器
npx tauri dev        # 启动 Tauri 桌面开发模式
npx tauri build      # 打包生产安装包
```

---

## 🤖 AI Skill 技能包

项目在 `skills/` 目录提供了面向 AI 编程助手的知识包，加载后 AI 可以：

- 为**易语言**、**Python**、**C#** 生成正确的 DLL 调用代码
- 自动处理 UTF-8 编码、两次调用模式、回调防 GC 等常见陷阱
- 了解全部 16 种控件的创建、属性设置和事件回调 API
- 正确使用布局管理器、主题系统和扩展事件系统

适用于 Kiro、Cursor 等支持 Skill 加载的 AI 编程工具。详见 [skills/emoji-window-dll/使用说明.md](skills/emoji-window-dll/使用说明.md)。

---

## 📁 项目结构

```
emoji_window_cpp/
├── emoji_window.sln              # Visual Studio 解决方案
├── emoji_window/                 # C++ DLL 源码
│   ├── dllmain.cpp               # DLL 入口
│   ├── emoji_window.h            # API 声明
│   ├── emoji_window.cpp          # 控件实现
│   └── emoji_window.def          # 导出定义（210+ 函数）
├── designer/                     # 🖥️ 可视化设计器（Tauri + React）
│   ├── src/                      # 前端源码（React + TypeScript）
│   ├── src-tauri/                # Rust 后端（Tauri v2）
│   ├── .env.example              # 环境变量模板
│   └── package.json
├── skills/                       # 🤖 AI Skill 技能包
│   └── emoji-window-dll/         # DLL API 知识包
│       ├── controls/             # 16 个控件 API 文档
│       ├── rules/                # 易语言/Python/C# 编码规则
│       └── templates/            # 代码生成模板
├── themes/                       # 主题配置
│   ├── light.json
│   └── dark.json
├── docs/                         # 文档
│   ├── controls/                 # 控件文档
│   ├── theme.md
│   ├── layout.md
│   └── ...
├── 易语言代码/                    # 易语言示例和声明
│   ├── DLL命令.e
│   ├── 常量表.e
│   └── 窗口程序集_*.e
└── x64/Release/
    └── emoji_window.dll
```

---

## 🔨 编译步骤

### 方法 1：使用 Visual Studio（推荐）

1. 安装 Visual Studio 2019 或更高版本
2. 打开 `emoji_window.sln`
3. 选择 Release | x64 配置
4. 生成解决方案（Ctrl+Shift+B）
5. DLL 输出到 `x64\Release\emoji_window.dll`

### 方法 2：使用命令行（MSBuild）

```cmd
"C:\Program Files\Microsoft Visual Studio\2022\Community\MSBuild\Current\Bin\MSBuild.exe" emoji_window.sln /p:Configuration=Release /p:Platform=x64
```

---

## 📚 控件文档

### 基础控件

| 控件 | 文档 | 说明 |
|------|------|------|
| 按钮 | [button.md](docs/controls/button.md) | 支持彩色 Emoji 的按钮控件 |
| 标签 | [label.md](docs/controls/label.md) | 文本标签，支持自动换行 |
| 编辑框 | [editbox.md](docs/controls/editbox.md) | 单行/多行编辑框，支持垂直居中 |
| 复选框 | [checkbox.md](docs/controls/checkbox.md) | Element UI 风格复选框 |
| 单选按钮 | [radiobutton.md](docs/controls/radiobutton.md) | 分组互斥的单选按钮 |
| 进度条 | [progressbar.md](docs/controls/progressbar.md) | 支持确定/不确定模式 |

### 高级控件

| 控件 | 文档 | 说明 |
|------|------|------|
| 列表框 | [listbox.md](docs/controls/listbox.md) | 可滚动的项目列表 |
| 组合框 | [combobox.md](docs/controls/combobox.md) | 下拉列表选择器 |
| 图片框 | [picturebox.md](docs/controls/picturebox.md) | 支持多种格式和缩放模式 |
| 分组框 | [groupbox.md](docs/controls/groupbox.md) | 控件分组容器 |
| 热键控件 | [hotkey.md](docs/controls/hotkey.md) | 键盘快捷键捕获 |
| 树形框 | [treeview.md](docs/controls/treeview.md) | 层次结构数据，支持 Emoji 图标和拖放 |
| 日期时间 | [datetimepicker.md](docs/controls/datetimepicker.md) | D2D 日期时间选择器，Element 风格弹出层 |
| 表格 | [datagridview.md](docs/controls/datagridview.md) | 高性能数据表格，支持虚拟模式 |
| 选项卡 | [tabcontrol.md](docs/controls/tabcontrol.md) | 多标签页容器，支持外观定制、拖拽排序、关闭按钮、滚动 |

### 菜单与右键

| 功能 | 说明 |
|------|------|
| 菜单栏 | `CreateMenuBar` / `MenuBarAddItem` / `MenuBarAddSubItem` - 窗口顶部菜单 |
| 右键弹出菜单 | `CreateEmojiPopupMenu` / `PopupMenuAddItem` / `BindControlMenu` - 控件右键弹出 |



---

## 🎯 核心功能

### 主题系统

支持亮色/暗色主题切换，可从 JSON 文件加载自定义主题。

```
' 切换暗色模式
设置暗色模式 (真)

' 从文件加载主题
从文件加载主题_辅助 ("themes/dark.json")
```

📖 [完整文档](docs/theme.md)

### 布局管理器

自动管理控件位置和大小，支持流式布局、网格布局、停靠布局。

```
' 设置水平流式布局
设置布局管理器 (窗口句柄, #LAYOUT_FLOW_H, 0, 0, 10)
添加控件到布局 (窗口句柄, 按钮句柄)
更新布局 (窗口句柄)
```

📖 [完整文档](docs/layout.md)

### 扩展事件系统

统一的事件回调机制，支持鼠标、键盘、焦点事件。

```
' 设置鼠标进入回调
设置鼠标进入回调 (控件句柄, &鼠标进入处理)

' 设置按键回调
设置按键按下回调 (控件句柄, &按键处理)
```

📖 完整文档（待完善）

### 窗口属性操作

动态获取和设置窗口的各种属性，包括标题、位置、大小、可视状态和标题栏颜色。

#### 窗口属性 API 列表

| API 函数 | 功能说明 | 返回值 |
|---------|---------|--------|
| `GetWindowTitle` | 获取窗口标题（UTF-8） | 文本长度（字节数），-1表示失败 |
| `GetWindowBounds` | 获取窗口位置和大小 | 通过参数返回x, y, width, height |
| `SetWindowBounds` | 设置窗口位置和大小 | 无返回值 |
| `GetWindowVisible` | 获取窗口可视状态 | 1=可见，0=不可见，-1=错误 |
| `ShowEmojiWindow` | 显示或隐藏窗口 | 无返回值 |
| `GetWindowTitlebarColor` | 获取窗口标题栏颜色 | RGB颜色值，-1表示失败 |

#### 使用示例

**获取窗口标题**（两次调用模式）：
```
' 第一次调用获取长度
文本长度 = 获取窗口标题 (窗口句柄, 0, 0)

' 第二次调用获取内容
文本缓冲区 = 取空白字节集 (文本长度)
获取窗口标题 (窗口句柄, 取字节集数据地址 (文本缓冲区), 文本长度)
文本内容 = 到文本 (文本缓冲区)
```

**获取和设置窗口位置**：
```
.局部变量 X, 整数型
.局部变量 Y, 整数型
.局部变量 宽度, 整数型
.局部变量 高度, 整数型

' 获取当前位置
获取窗口位置 (窗口句柄, X, Y, 宽度, 高度)

' 移动窗口到新位置
设置窗口位置 (窗口句柄, 100, 100, 800, 600)
```

**控制窗口可视状态**：
```
' 获取可视状态
可见状态 = 获取窗口可视状态 (窗口句柄)

' 显示窗口
显示隐藏窗口 (窗口句柄, 真)

' 隐藏窗口
显示隐藏窗口 (窗口句柄, 假)
```

**获取和设置标题栏颜色**：
```
' 获取标题栏颜色
标题栏颜色 = 获取窗口标题栏颜色 (窗口句柄)

' 提取RGB分量
R = 位与 (位右移 (标题栏颜色, 16), 255)
G = 位与 (位右移 (标题栏颜色, 8), 255)
B = 位与 (标题栏颜色, 255)

' 设置新的标题栏颜色
新颜色 = 到RGB (100, 150, 255)
设置窗口标题栏颜色 (窗口句柄, 新颜色)
```

#### 注意事项

1. **两次调用模式**：`GetWindowTitle` 使用两次调用模式获取文本
   - 第一次传入 buffer=0 获取所需长度
   - 第二次传入实际缓冲区获取内容

2. **UTF-8 编码**：窗口标题使用 UTF-8 编码，支持中文和 Emoji

3. **RGB 颜色格式**：标题栏颜色使用 RGB 格式（24位整数）
   - Red（红色）：位16-23
   - Green（绿色）：位8-15
   - Blue（蓝色）：位0-7

4. **传址参数**：`GetWindowBounds` 的位置和大小参数需要使用传址方式

5. **立即生效**：所有 Set 函数修改后立即生效，无需手动刷新

6. **窗口隐藏**：当窗口被隐藏时，所有子控件也会隐藏

📖 [完整示例代码](examples/易语言代码/易语言代码文档/窗口程序集_窗口属性操作示例.md)

---

## 🔧 控件属性操作

v2.1 新增了 57 个属性 API，覆盖所有主要控件，支持在运行时动态读取和修改控件属性。

### 快速参考表格

#### 按钮控件属性 API

| API 函数 | 功能 | 返回值 |
|---------|------|--------|
| `GetButtonText` | 获取按钮文本（UTF-8） | 字节数，-1=失败 |
| `SetButtonText` | 设置按钮文本 | 无 |
| `GetButtonEmoji` | 获取按钮 Emoji（UTF-8） | 字节数，-1=失败 |
| `SetButtonEmoji` | 设置按钮 Emoji | 无 |
| `GetButtonBounds` | 获取按钮位置和大小 | 0=成功，-1=失败 |
| `SetButtonBounds` | 设置按钮位置和大小 | 无 |
| `GetButtonBackgroundColor` | 获取按钮背景色（ARGB） | 颜色值 |
| `SetButtonBackgroundColor` | 设置按钮背景色 | 无 |
| `GetButtonVisible` | 获取可视状态 | 1=可见，0=隐藏，-1=失败 |
| `ShowButton` | 显示/隐藏按钮 | 无 |
| `GetButtonEnabled` | 获取启用状态 | 1=启用，0=禁用，-1=失败 |

#### 窗口属性 API

| API 函数 | 功能 | 返回值 |
|---------|------|--------|
| `GetWindowTitle` | 获取窗口标题（UTF-8） | 字节数，-1=失败 |
| `GetWindowBounds` | 获取窗口位置和大小 | 通过参数返回 |
| `SetWindowBounds` | 设置窗口位置和大小 | 无 |
| `GetWindowVisible` | 获取可视状态 | 1=可见，0=隐藏，-1=失败 |
| `ShowEmojiWindow` | 显示/隐藏窗口 | 无 |
| `GetWindowTitlebarColor` | 获取标题栏颜色（RGB） | 颜色值，-1=失败 |

#### 标签控件属性 API

| API 函数 | 功能 | 返回值 |
|---------|------|--------|
| `GetLabelText` | 获取标签文本（UTF-8） | 字节数，-1=失败 |
| `GetLabelFont` | 获取字体信息 | 字体名字节数，-1=失败 |
| `GetLabelColor` | 获取前景色和背景色（ARGB） | 0=成功，-1=失败 |
| `GetLabelBounds` | 获取位置和大小 | 0=成功，-1=失败 |
| `GetLabelAlignment` | 获取对齐方式（0=左，1=中，2=右） | 对齐值，-1=失败 |
| `GetLabelEnabled` | 获取启用状态 | 1=启用，0=禁用，-1=失败 |
| `GetLabelVisible` | 获取可视状态 | 1=可见，0=隐藏，-1=失败 |

#### 复选框属性 API

| API 函数 | 功能 | 返回值 |
|---------|------|--------|
| `GetCheckBoxText` | 获取文本（UTF-8） | 字节数，-1=失败 |
| `SetCheckBoxFont` | 设置字体 | 无 |
| `GetCheckBoxFont` | 获取字体信息 | 字体名字节数，-1=失败 |
| `SetCheckBoxColor` | 设置前景色和背景色 | 无 |
| `GetCheckBoxColor` | 获取前景色和背景色（ARGB） | 0=成功，-1=失败 |

#### 单选按钮属性 API

| API 函数 | 功能 | 返回值 |
|---------|------|--------|
| `GetRadioButtonText` | 获取文本（UTF-8） | 字节数，-1=失败 |
| `SetRadioButtonFont` | 设置字体 | 无 |
| `GetRadioButtonFont` | 获取字体信息 | 字体名字节数，-1=失败 |
| `SetRadioButtonColor` | 设置前景色和背景色 | 无 |
| `GetRadioButtonColor` | 获取前景色和背景色（ARGB） | 0=成功，-1=失败 |

#### 分组框属性 API

| API 函数 | 功能 | 返回值 |
|---------|------|--------|
| `GetGroupBoxTitle` | 获取标题（UTF-8） | 字节数，-1=失败 |
| `GetGroupBoxBounds` | 获取位置和大小 | 0=成功，-1=失败 |
| `GetGroupBoxVisible` | 获取可视状态 | 1=可见，0=隐藏，-1=失败 |
| `GetGroupBoxEnabled` | 获取启用状态 | 1=启用，0=禁用，-1=失败 |
| `GetGroupBoxColor` | 获取边框色和背景色（ARGB） | 0=成功，-1=失败 |

#### TabControl 属性 API

| API 函数 | 功能 | 返回值 |
|---------|------|--------|
| `GetTabTitle` | 获取 Tab 页标题（UTF-8） | 字节数，-1=失败 |
| `SetTabTitle` | 设置 Tab 页标题 | 0=成功，-1=失败 |
| `GetTabControlBounds` | 获取位置和大小 | 0=成功，-1=失败 |
| `SetTabControlBounds` | 设置位置和大小 | 0=成功，-1=失败 |
| `GetTabControlVisible` | 获取可视状态 | 1=可见，0=隐藏，-1=失败 |
| `ShowTabControl` | 显示/隐藏 TabControl | 0=成功，-1=失败 |
| `EnableTabControl` | 启用/禁用 TabControl | 0=成功，-1=失败 |

#### TabControl 增强 API（v3.0 新增）

| API 函数 | 功能 | 返回值 |
|---------|------|--------|
| `SetTabItemSize` | 设置标签页宽度和高度 | 0=成功，-1=失败 |
| `SetTabFont` | 设置标签字体名称和字号 | 0=成功，-1=失败 |
| `SetTabColors` | 设置选中/未选中背景色和文字色 | 0=成功，-1=失败 |
| `SetTabIndicatorColor` | 设置选中指示条颜色 | 0=成功，-1=失败 |
| `SetTabPadding` | 设置标签内边距 | 0=成功，-1=失败 |
| `EnableTabItem` | 启用/禁用单个标签页 | 0=成功，-1=失败 |
| `GetTabItemEnabled` | 获取标签页启用状态 | 1=启用，0=禁用，-1=失败 |
| `ShowTabItem` | 显示/隐藏单个标签页 | 0=成功，-1=失败 |
| `SetTabItemIcon` | 设置标签页 PNG 图标 | 0=成功，-1=失败 |
| `SetTabContentBgColor` | 设置单个标签页内容区域背景色 | 0=成功，-1=失败 |
| `SetTabContentBgColorAll` | 设置所有标签页内容区域背景色 | 0=成功，-1=失败 |
| `SetTabClosable` | 设置标签页关闭按钮显示 | 0=成功，-1=失败 |
| `SetTabCloseCallback` | 设置关闭按钮回调 | 0=成功，-1=失败 |
| `SetTabRightClickCallback` | 设置右键点击回调 | 0=成功，-1=失败 |
| `SetTabDraggable` | 设置标签页可拖拽排序 | 0=成功，-1=失败 |
| `SetTabDoubleClickCallback` | 设置双击回调 | 0=成功，-1=失败 |
| `SetTabPosition` | 设置标签栏位置（上/下/左/右） | 0=成功，-1=失败 |
| `SetTabAlignment` | 设置标签对齐方式 | 0=成功，-1=失败 |
| `SetTabScrollable` | 设置标签栏可滚动 | 0=成功，-1=失败 |
| `RemoveAllTabs` | 清空所有标签页 | 0=成功，-1=失败 |
| `InsertTabItem` | 在指定位置插入标签页 | 实际索引，-1=失败 |
| `MoveTabItem` | 移动标签页位置 | 0=成功，-1=失败 |
| `GetTabIndexByTitle` | 根据标题查找标签页索引 | 索引，-1=未找到 |
| `GetTabEnabled` | 获取 TabControl 启用状态 | 1=启用，0=禁用，-1=失败 |
| `IsTabItemSelected` | 判断标签页是否选中 | 1=选中，0=未选中，-1=失败 |

#### 编辑框扩展属性 API

| API 函数 | 功能 | 返回值 |
|---------|------|--------|
| `GetEditBoxFont` | 获取字体信息 | 字体名字节数，-1=失败 |
| `GetEditBoxColor` | 获取前景色和背景色（ARGB） | 0=成功，-1=失败 |
| `GetEditBoxBounds` | 获取位置和大小 | 0=成功，-1=失败 |
| `GetEditBoxAlignment` | 获取对齐方式（0=左，1=中，2=右） | 对齐值，-1=失败 |
| `GetEditBoxEnabled` | 获取启用状态 | 1=启用，0=禁用，-1=失败 |
| `GetEditBoxVisible` | 获取可视状态 | 1=可见，0=隐藏，-1=失败 |

#### 进度条扩展属性 API

| API 函数 | 功能 | 返回值 |
|---------|------|--------|
| `GetProgressBarColor` | 获取前景色和背景色（ARGB） | 0=成功，-1=失败 |
| `GetProgressBarBounds` | 获取位置和大小 | 0=成功，-1=失败 |
| `GetProgressBarEnabled` | 获取启用状态 | 1=启用，0=禁用，-1=失败 |
| `GetProgressBarVisible` | 获取可视状态 | 1=可见，0=隐藏，-1=失败 |
| `GetProgressBarShowText` | 获取是否显示百分比文本 | 1=显示，0=不显示，-1=失败 |

### 通用使用规则

**两次调用模式**（获取文本/字体名时）：
```
' 第一次调用：传 0 获取所需缓冲区大小
长度 = 获取按钮文本 (按钮ID, 0, 0)

' 第二次调用：传入缓冲区获取实际内容
缓冲区 = 取空白字节集 (长度)
获取按钮文本 (按钮ID, 取变量数据地址 (缓冲区), 长度)
```

**传址参数**（获取位置/颜色时）：
```
.局部变量 X, 整数型
.局部变量 Y, 整数型
.局部变量 宽度, 整数型
.局部变量 高度, 整数型

获取按钮位置 (按钮ID, X, Y, 宽度, 高度)
```

**UTF-8 字节集显示**（不能用 `到文本()` 转换）：
```
' ✅ 正确：全程字节集拼接
结果字节集 = 编码_Ansi到Utf8 ("文本：") ＋ 文本缓冲区
设置标签文本 (标签句柄, 取变量数据地址 (结果字节集), 取字节集长度 (结果字节集))

' ❌ 错误：到文本() 会导致中文/Emoji乱码
文本 = 到文本 (文本缓冲区)
```

### 常见使用场景

**场景 1：动态修改按钮外观**
```
' 根据状态切换按钮颜色和文本
.如果 (任务完成)
    新文本 = 编码_Ansi到Utf8 ("已完成")
    设置按钮文本 (按钮ID, 取变量数据地址 (新文本), 取字节集长度 (新文本))
    设置按钮背景色 (按钮ID, 到ARGB (255, 103, 194, 58))  ' 绿色
.否则
    新文本 = 编码_Ansi到Utf8 ("进行中")
    设置按钮文本 (按钮ID, 取变量数据地址 (新文本), 取字节集长度 (新文本))
    设置按钮背景色 (按钮ID, 到ARGB (255, 64, 158, 255))  ' 蓝色
.如果结束
```

**场景 2：读取并验证控件当前状态**
```
' 读取编辑框字体，验证是否符合预期
.局部变量 字体名长度, 整数型
.局部变量 字体名缓冲区, 字节集
.局部变量 字体大小, 整数型
.局部变量 粗体, 整数型
.局部变量 斜体, 整数型
.局部变量 下划线, 整数型

字体名长度 = 获取编辑框字体 (编辑框句柄, 0, 0, 字体大小, 粗体, 斜体, 下划线)
.如果 (字体名长度 ＞ 0)
    字体名缓冲区 = 取空白字节集 (字体名长度)
    获取编辑框字体 (编辑框句柄, 取变量数据地址 (字体名缓冲区), 字体名长度, 字体大小, 粗体, 斜体, 下划线)
    ' 字体名缓冲区 是 UTF-8 字节集，可直接拼接显示
.如果结束
```

**场景 3：批量显示/隐藏控件**
```
' 根据权限显示或隐藏功能区域
.子程序 设置管理员模式
.参数 是管理员, 逻辑型

显示隐藏按钮 (按钮_删除, 是管理员)
显示隐藏按钮 (按钮_编辑, 是管理员)
显示隐藏TabControl (TabControl_管理, 是管理员)
```

**场景 4：保存和恢复控件布局**
```
' 保存当前布局
.局部变量 X, 整数型
.局部变量 Y, 整数型
.局部变量 宽度, 整数型
.局部变量 高度, 整数型

获取按钮位置 (按钮ID, X, Y, 宽度, 高度)
' 保存 X, Y, 宽度, 高度 到配置文件...

' 恢复布局
设置按钮位置 (按钮ID, 保存的X, 保存的Y, 保存的宽度, 保存的高度)
```

**场景 5：动态修改 Tab 页标题**
```
' 在 Tab 标题中显示未读消息数
.局部变量 新标题字节集, 字节集
.局部变量 消息数, 整数型

消息数 = 获取未读消息数 ()
.如果 (消息数 ＞ 0)
    ' "消息 (3)"
    新标题字节集 = 编码_Ansi到Utf8 ("消息 (") ＋ 编码_Ansi到Utf8 (到文本 (消息数)) ＋ 编码_Ansi到Utf8 (")")
.否则
    新标题字节集 = 编码_Ansi到Utf8 ("消息")
.如果结束
设置Tab页标题 (TabControl句柄, 1, 取变量数据地址 (新标题字节集), 取字节集长度 (新标题字节集))
```

📖 完整示例代码：
- [按钮属性操作示例](examples/易语言代码/易语言代码文档/窗口程序集_按钮属性操作示例.md)
- [窗口属性操作示例](examples/易语言代码/易语言代码文档/窗口程序集_窗口属性操作示例.md)
- [标签属性操作示例](examples/易语言代码/易语言代码文档/窗口程序集_标签属性操作示例.md)
- [TabControl 属性操作示例](examples/易语言代码/易语言代码文档/窗口程序集_TabControl属性操作示例.md)
- [编辑框进度条属性示例](examples/易语言代码/易语言代码文档/窗口程序集_编辑框进度条属性示例.md)

---

## 📋 完整控件列表

| 控件 | 创建函数 | 关键特性 |
|------|----------|----------|
| 窗口 | `create_window()` | D2D 渲染，自定义回调 |
| 按钮 | `create_emoji_button_bytes()` | Emoji 支持，自定义颜色 |
| 标签 | `CreateLabel()` | 自动换行，对齐方式 |
| 编辑框 | `CreateEditBox()` | 垂直居中，按键回调 |
| 复选框 | `CreateCheckBox()` | Element UI 风格 |
| 单选按钮 | `CreateRadioButton()` | 分组互斥 |
| 进度条 | `CreateProgressBar()` | 动画，不确定模式 |
| 列表框 | `CreateListBox()` | 多选，自定义渲染 |
| 组合框 | `CreateComboBox()` | 下拉列表，Emoji 支持 |
| 热键控件 | `CreateHotKeyControl()` | 键盘捕获 |
| 图片框 | `CreatePictureBox()` | 文件/内存加载，缩放模式 |
| 分组框 | `CreateGroupBox()` | 子控件容器 |
| 树形框 | `CreateTreeView()` | 层次结构，Emoji 图标，拖放 |
| 表格 | `CreateDataGridView()` | 虚拟模式，多列类型 |
| 选项卡 | `CreateTabControl()` | 多标签页容器，支持外观定制、拖拽排序、关闭按钮、滚动 |

---

## 📸 查看截图

查看各控件的实际效果截图，请访问 [imgs 目录](imgs/)。

---

## ❓ 常见问题

### Emoji 显示为乱码？

易语言 IDE 使用 ANSI 编码，需要将 Emoji 转换为 UTF-8 字节集。

📖 [查看解决方案](docs/faq.md#emoji-显示问题)

### 如何处理大数据量表格？

使用虚拟模式，仅加载可见行数据。

📖 [查看虚拟模式文档](docs/controls/datagridview.md#虚拟模式)

### 如何实现响应式布局？

使用布局管理器自动调整控件位置。

📖 [查看布局文档](docs/layout.md)

### 图片从内存加载显示黑色？

必须使用程序集变量（全局变量）保存图片数据，不能使用局部变量。

📖 [查看详细说明](docs/faq.md#图片加载问题)

### 更多问题

📖 [完整 FAQ 文档](docs/faq.md)

---

## ⚡ 性能优化

### 关键优化建议

1. **表格大数据**：超过 1000 行使用虚拟模式
2. **批量布局更新**：批量操作后统一调用 `UpdateLayout()`
3. **图片缩放**：大图片使用 `SCALE_FIT` 模式
4. **事件回调**：避免在回调中执行耗时操作
5. **控件数量**：单窗口控件数量建议 < 200

📖 [完整性能优化文档](docs/performance.md)

---

## 🔧 技术细节

### 核心技术

- **渲染引擎**: Direct2D
- **文字渲染**: DirectWrite
- **彩色 Emoji**: `D2D1_DRAW_TEXT_OPTIONS_ENABLE_COLOR_FONT`
- **图片加载**: WIC (Windows Imaging Component)
- **字体**: Segoe UI Emoji, Microsoft YaHei UI
- **编译器**: MSVC 2019+
- **平台**: Windows 10+

### 依赖项

- Windows SDK 10.0 或更高
- Direct2D
- DirectWrite
- 无需额外运行时（静态链接）

### 许可证

MIT License

---

## 💰 打赏支持

如果这个项目对你有帮助，欢迎请我喝一杯 ☕

<div align="center">
  <table>
    <tr>
      <td align="center">
        <strong>支付宝</strong><br/>
        <img src="https://img.msblog.cc/image-20250523012804344.png" alt="支付宝" width="200">
      </td>
      <td align="center">
        <strong>微信</strong><br/>
        <img src="https://img.msblog.cc/image-20250523012814243.png" alt="微信" width="200">
      </td>
    </tr>
  </table>
</div>

## 💝 赞赏名单

感谢每一位支持者，排名不分先后。

<div align="center">

| 昵称 | 联系方式 |
|:---:|:---:|
| **秋叶** | QQ `121***007` |

</div>

## 📧 联系方式

- **QQ**：1098901025
- **微信**：zhx_ms

> 添加请注明来意

---

<div align="center">
  <p>⭐ 如果觉得项目不错，欢迎 Star 支持！</p>
  <p>MIT License © 2025</p>
</div>
