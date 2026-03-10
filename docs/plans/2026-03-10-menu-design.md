# 设计文档：文件菜单与右键菜单（彩色 emoji 自绘）

## 背景与目标
当前项目已有自绘控件与自定义标题栏，但**没有菜单栏/右键菜单实现**。需求是继续封装**文件菜单**与**右键菜单**，并且**必须支持彩色 Unicode 表情**。用户选择完全自绘方案，并要求：
- 菜单栏默认位于**客户区首行**，但**可切换到标题栏内**；若在标题栏内需避免与 HTCAPTION 拖拽冲突。
- 菜单栏**绑定某个控件作为父窗口**（坐标与可见性跟随控件）。
- 右键菜单为**控件自定义菜单**（按控件实例绑定）。
- 支持**键盘导航**（Alt/方向键/Enter/Esc/Shift+F10/VK_APPS）。

## 方案选择
采用**完全自绘方案**（Direct2D + DirectWrite）：
- 彩色 emoji 需要 `DWRITE_DRAW_TEXT_OPTIONS_ENABLE_COLOR_FONT`，系统菜单无法可靠支持。
- 与现有 D2D 主题体系一致，风格统一，可扩展。

## 设计概要
### 1) 数据结构与回调
在 [emoji_window/emoji_window.h](../../emoji_window/emoji_window.h) 中新增：
- `MenuItem`：包含文本/emoji、id、enabled、children、bounds 等
- `MenuBarState`：菜单栏状态（items、hovered、active、height、绑定控件 HWND、位置模式）
- `PopupMenuState`：弹出菜单窗口状态（hwnd、owner、items、hovered、width/height）
- 新增菜单点击回调 typedef 与“控件 → 菜单”绑定映射

### 2) 导出 API
在 [emoji_window/emoji_window.h](../../emoji_window/emoji_window.h) + [emoji_window/emoji_window.def](../../emoji_window/emoji_window.def) 中新增：
- 创建/销毁菜单栏
- 添加菜单项/子菜单项
- 绑定回调
- 绑定控件实例（用于控件自定义右键菜单）
- 显示右键菜单（支持坐标/当前鼠标位置）
- 设置菜单栏位置（客户区/标题栏）

### 3) 渲染
在 [emoji_window/emoji_window.cpp](../../emoji_window/emoji_window.cpp) 中：
- 菜单栏与弹出菜单均使用 D2D + DWrite 绘制
- 文本/emoji 绘制使用 `DWRITE_DRAW_TEXT_OPTIONS_ENABLE_COLOR_FONT`
- 使用 `IDWriteTextLayout::GetMetrics` 计算 item 尺寸
- 颜色来源于主题系统（ThemeColors），hover/pressed 使用亮/暗化辅助函数

### 4) 布局与命中
- 默认客户区首行绘制菜单栏；
- 可切换至标题栏内时，需要在 `WM_NCHITTEST` 中排除菜单区域（避免被判定为 `HTCAPTION`）。
- 菜单栏绑定控件：菜单区域随控件移动/尺寸变化更新。

### 5) 交互与键盘
- 鼠标：悬停高亮、点击展开/关闭、外部点击关闭
- 键盘：Alt 打开菜单栏焦点；方向键切换；Enter 触发；Esc 关闭
- 右键菜单：支持 Shift+F10 / VK_APPS 触发
- 弹出菜单使用 WS_POPUP 自绘窗口；失焦或外部点击关闭
- 控件右键：通过 RightClickCallback 触发显示绑定菜单

## 关键复用点
- 标题栏命中测试与拖拽逻辑：[emoji_window/emoji_window.cpp](../../emoji_window/emoji_window.cpp)
- 主题系统：[emoji_window/emoji_window.h](../../emoji_window/emoji_window.h)
- 彩色 emoji 文字绘制选项：`DWRITE_DRAW_TEXT_OPTIONS_ENABLE_COLOR_FONT`
- 通用事件回调结构：`EventCallbacks`

## 验证计划
- VS 或 MSBuild Release|x64 构建
- 手动验证：
  1. 菜单栏与右键菜单显示彩色 emoji
  2. 菜单栏客户区/标题栏切换无冲突
  3. 控件自定义右键菜单正常弹出
  4. Alt/方向键/Enter/Esc/Shift+F10/VK_APPS 导航可用
  5. 外部点击/失焦自动关闭
