# Emoji Window DLL - C++ 版本

使用 C++ 和 Direct2D 实现彩色 Emoji 显示的 DLL。

## 项目结构

```
emoji_window_cpp/
├── emoji_window.sln          # Visual Studio 解决方案
├── emoji_window/
│   ├── emoji_window.vcxproj  # 项目文件
│   ├── dllmain.cpp           # DLL 入口
│   ├── emoji_window.h        # 头文件
│   ├── emoji_window.cpp      # 主实现
│   ├── renderer.h            # 渲染器头文件
│   ├── renderer.cpp          # 渲染器实现
│   └── exports.def           # 导出定义
└── output/
    └── emoji_window.dll      # 编译输出

```

## 编译步骤

### 1. 使用 Visual Studio（推荐）

1. 安装 Visual Studio 2019 或更高版本
2. 打开 `emoji_window.sln`
3. 选择 Release | x64 配置
4. 生成解决方案（Ctrl+Shift+B）
5. DLL 输出到 `x64\Release\emoji_window.dll`

### 2. 使用命令行（MSBuild）

```cmd
"C:\Program Files\Microsoft Visual Studio\2022\Community\MSBuild\Current\Bin\MSBuild.exe" emoji_window.sln /p:Configuration=Release /p:Platform=x64
```

## API 文档

### 创建窗口

```c++
HWND create_window(const char* title, int width, int height);
```

### 创建 Emoji 按钮（字节集）

```c++
int create_emoji_button_bytes(
    HWND parent,
    const unsigned char* emoji_bytes,
    int emoji_len,
    const unsigned char* text_bytes,
    int text_len,
    int x, int y, int width, int height,
    unsigned int bg_color
);
```

### 设置按钮点击回调

```c++
typedef void (__stdcall *ButtonClickCallback)(int button_id);
void __stdcall set_button_click_callback(ButtonClickCallback callback);
```

### 设置窗口大小改变回调

当自绘窗口大小被用户或代码改变时触发。

```c++
typedef void (__stdcall *WindowResizeCallback)(HWND hwnd, int width, int height);
void __stdcall SetWindowResizeCallback(WindowResizeCallback callback);
```

| 参数 | 说明 |
|------|------|
| `hwnd` | 发生大小改变的窗口句柄 |
| `width` | 窗口新的客户区宽度（像素） |
| `height` | 窗口新的客户区高度（像素） |

**易语言声明：**
```
.DLL命令 设置窗口大小改变回调, , "emoji_window.dll", "SetWindowResizeCallback"
    .参数 回调函数指针, 子程序指针
```

**易语言使用示例：**
```
.子程序 窗口大小改变回调, , 公开
.参数 窗口句柄_, 整数型
.参数 新宽度, 整数型
.参数 新高度, 整数型

' 窗口大小改变时更新布局
调试输出 ("窗口大小改变: " + 到文本 (新宽度) + " x " + 到文本 (新高度))

' 注册（程序初始化时调用一次）
设置窗口大小改变回调 (&窗口大小改变回调)
```

> **注意**：回调必须在创建窗口后、运行消息循环前完成注册。

---

### 设置窗口被关闭回调

当自绘窗口被关闭时触发（用户点击关闭按钮 ×，或代码调用 `destroy_window`，均会触发 `WM_DESTROY`）。

```c++
typedef void (__stdcall *WindowCloseCallback)(HWND hwnd);
void __stdcall SetWindowCloseCallback(WindowCloseCallback callback);
```

| 参数 | 说明 |
|------|------|
| `hwnd` | 被关闭的窗口句柄（触发时 HWND 已失效，仅用于识别是哪个窗口） |

**易语言声明：**
```
.DLL命令 设置窗口关闭回调, , "emoji_window.dll", "SetWindowCloseCallback"
    .参数 回调函数指针, 子程序指针
```

**易语言使用示例：**
```
.子程序 自绘窗口关闭回调, , 公开
.参数 已关闭的窗口句柄, 整数型

' 重置句柄变量，防止后续误用失效的 HWND
调试输出 ("自绘窗口已关闭, HWND=" + 到文本 (已关闭的窗口句柄))
.如果真 (窗口句柄 = 已关闭的窗口句柄)
    窗口句柄 = 0
.如果真结束
TabControl句柄 = 0

' 注册（程序初始化时调用一次）
设置窗口关闭回调 (&自绘窗口关闭回调)
```

> **注意**：
> - 回调触发时窗口已销毁，不要在回调内对该 `hwnd` 执行任何窗口操作。
> - 只有顶层窗口（非子窗口）关闭时才会触发此回调。
> - 若程序同时运行了 `run_message_loop`，关闭窗口后消息循环会自动退出；若由易语言消息循环驱动，则不会影响易语言进程。

---

### 设置窗口图标

设置窗口的图标（标题栏和任务栏显示）。

```c++
void __stdcall set_window_icon(HWND hwnd, const char* icon_path);
```

| 参数 | 说明 |
|------|------|
| `hwnd` | 窗口句柄 |
| `icon_path` | 图标文件路径（支持 .ico 格式） |

**易语言声明：**
```
.DLL命令 设置窗口图标, , "emoji_window.dll", "set_window_icon"
    .参数 窗口句柄, 整数型
    .参数 图标路径, 文本型
```

**易语言使用示例：**
```
设置窗口图标 (窗口句柄, "icon.ico")
```

---

### 设置窗口标题

设置窗口的标题文本（支持 UTF-8 编码）。

```c++
void __stdcall set_window_title(HWND hwnd, const unsigned char* title_bytes, int title_len);
```

| 参数 | 说明 |
|------|------|
| `hwnd` | 窗口句柄 |
| `title_bytes` | UTF-8 编码的标题字节集指针 |
| `title_len` | 字节集长度 |

**易语言声明：**
```
.DLL命令 设置窗口标题, , "emoji_window.dll", "set_window_title"
    .参数 窗口句柄, 整数型
    .参数 标题字节集指针, 整数型, , UTF-8字节集指针
    .参数 标题长度, 整数型, , 字节集长度
```

**易语言使用示例：**
```
.局部变量 标题文本, 文本型
.局部变量 标题UTF8, 字节集
.局部变量 标题指针, 整数型
.局部变量 标题长度, 整数型

标题文本 = "新窗口标题 🎉"
标题UTF8 = 到UTF8 (标题文本)
标题指针 = 取字节集数据地址 (标题UTF8)
标题长度 = 取字节集长度 (标题UTF8)

设置窗口标题 (窗口句柄, 标题指针, 标题长度)
```

---

### 编辑框

#### 创建编辑框（含文本垂直居中）

单行编辑框支持**文本垂直居中**：创建时最后一个参数传 `TRUE` 即可；仅对单行有效，多行编辑框忽略该选项。

```c++
HWND __stdcall CreateEditBox(
    HWND hParent,
    int x, int y, int width, int height,
    const unsigned char* text_bytes, int text_len,
    UINT32 fg_color, UINT32 bg_color,
    const unsigned char* font_name_bytes, int font_name_len,
    int font_size, BOOL bold, BOOL italic, BOOL underline,
    int alignment,    // 0=左 1=中 2=右
    BOOL multiline, BOOL readonly, BOOL password, BOOL has_border,
    BOOL vertical_center   // 文本垂直居中（仅单行有效）
);
```

**说明**：系统单行 EDIT 不支持垂直居中，DLL 在“单行 + 垂直居中”时内部使用多行样式（`EM_SETRECTNP`）实现垂直居中，并拦截回车以保持单行行为；左右边距设为 0 以减少白边。

**易语言声明：** 在 `创建编辑框` 的 DLL 命令中，最后一个参数为 `文本垂直居中`（逻辑型）。

**易语言示例：**
```
编辑框1 ＝ 创建编辑框 (父句柄, 140, 30, 280, 60, 文本指针, 文本长, 前景色, 背景色, 字体名指针, 字体名长, 12, 假, 假, 假, 1, 假, 假, 假, 真, 真)
' 最后一参 真 = 启用文本垂直居中
```

#### 设置编辑框垂直居中（创建后修改）

创建后也可随时开关垂直居中：

```c++
void __stdcall SetEditBoxVerticalCenter(HWND hEdit, BOOL vertical_center);
```

#### 设置编辑框按键回调

可接收编辑框的按键按下/松开通知（不拦截按键）：

```c++
typedef void (__stdcall *EditBoxKeyCallback)(HWND hEdit, int key_code, int key_down, int shift, int ctrl, int alt);
void __stdcall SetEditBoxKeyCallback(HWND hEdit, EditBoxKeyCallback callback);
```

| 参数 | 说明 |
|------|------|
| `key_code` | Windows 虚拟键码（如 13=回车 27=Esc） |
| `key_down` | 1=按下 0=松开 |
| `shift` / `ctrl` / `alt` | 1=按下 0=未按 |

**易语言**：子程序需 stdcall，参数为（编辑框句柄, 键码, 按下或松开, Shift, Ctrl, Alt）整数型；注册时传 `设置编辑框按键回调(编辑框句柄, 到整数(&子程序))`。

---

### 复选框（CheckBox）

#### 创建复选框

创建一个 Element UI 风格的复选框控件，支持选中/未选中状态切换。

```c++
HWND __stdcall CreateCheckBox(
    HWND hParent,
    int x, int y, int width, int height,
    const unsigned char* text_bytes, int text_len,
    BOOL checked,
    UINT32 fg_color,
    UINT32 bg_color
);
```

| 参数 | 说明 |
|------|------|
| `hParent` | 父窗口句柄 |
| `x, y` | 控件位置 |
| `width, height` | 控件尺寸 |
| `text_bytes` | UTF-8 编码的文本字节集指针 |
| `text_len` | 文本字节集长度 |
| `checked` | 初始选中状态（TRUE=选中，FALSE=未选中） |
| `fg_color` | 前景色（ARGB格式，如 0xFF303133） |
| `bg_color` | 背景色（ARGB格式，如 0xFFFFFFFF） |

**返回值**：复选框控件句柄

**易语言声明：**
```
.DLL命令 创建复选框, 整数型, "emoji_window.dll", "CreateCheckBox"
    .参数 父窗口句柄, 整数型
    .参数 X坐标, 整数型
    .参数 Y坐标, 整数型
    .参数 宽度, 整数型
    .参数 高度, 整数型
    .参数 文本字节集指针, 整数型
    .参数 文本长度, 整数型
    .参数 选中状态, 逻辑型
    .参数 前景色, 整数型, , ARGB颜色
    .参数 背景色, 整数型, , ARGB颜色
```

**易语言使用示例：**
```
.局部变量 复选框句柄, 整数型
.局部变量 文本UTF8, 字节集
.局部变量 文本指针, 整数型

文本UTF8 = 到UTF8 ("接受用户协议")
文本指针 = 取字节集数据地址 (文本UTF8)

复选框句柄 = 创建复选框 (窗口句柄, 50, 80, 200, 30, 文本指针, 取字节集长度 (文本UTF8), 假, #FF303133, #FFFFFFFF)
```

#### 获取复选框状态

获取复选框当前的选中状态。

```c++
BOOL __stdcall GetCheckBoxState(HWND hCheckBox);
```

**返回值**：TRUE=选中，FALSE=未选中

**易语言声明：**
```
.DLL命令 获取复选框状态, 逻辑型, "emoji_window.dll", "GetCheckBoxState"
    .参数 复选框句柄, 整数型
```

#### 设置复选框状态

设置复选框的选中状态。

```c++
void __stdcall SetCheckBoxState(HWND hCheckBox, BOOL checked);
```

**易语言声明：**
```
.DLL命令 设置复选框状态, , "emoji_window.dll", "SetCheckBoxState"
    .参数 复选框句柄, 整数型
    .参数 选中状态, 逻辑型
```

#### 设置复选框回调

设置复选框状态改变时的回调函数。

```c++
typedef void (__stdcall *CheckBoxCallback)(HWND hCheckBox, BOOL checked);
void __stdcall SetCheckBoxCallback(HWND hCheckBox, CheckBoxCallback callback);
```

| 回调参数 | 说明 |
|---------|------|
| `hCheckBox` | 复选框句柄 |
| `checked` | 新的选中状态（TRUE=选中，FALSE=未选中） |

**易语言声明：**
```
.DLL命令 设置复选框回调, , "emoji_window.dll", "SetCheckBoxCallback"
    .参数 复选框句柄, 整数型
    .参数 回调子程序指针, 整数型
```

**易语言回调示例：**
```
.子程序 复选框回调, , 公开, stdcall
.参数 复选框句柄, 整数型
.参数 选中状态, 逻辑型

调试输出 ("复选框状态改变：", 选中状态)

' 注册回调
设置复选框回调 (复选框句柄, &复选框回调)
```

#### 启用/禁用复选框

控制复选框是否可以交互。

```c++
void __stdcall EnableCheckBox(HWND hCheckBox, BOOL enable);
```

**易语言声明：**
```
.DLL命令 启用复选框, , "emoji_window.dll", "EnableCheckBox"
    .参数 复选框句柄, 整数型
    .参数 启用, 逻辑型
```

#### 显示/隐藏复选框

控制复选框的可见性。

```c++
void __stdcall ShowCheckBox(HWND hCheckBox, BOOL show);
```

**易语言声明：**
```
.DLL命令 显示复选框, , "emoji_window.dll", "ShowCheckBox"
    .参数 复选框句柄, 整数型
    .参数 显示, 逻辑型
```

#### 设置复选框文本

动态修改复选框的显示文本。

```c++
void __stdcall SetCheckBoxText(HWND hCheckBox, const unsigned char* text_bytes, int text_len);
```

**易语言声明：**
```
.DLL命令 设置复选框文本, , "emoji_window.dll", "SetCheckBoxText"
    .参数 复选框句柄, 整数型
    .参数 文本字节集指针, 整数型
    .参数 文本长度, 整数型
```

#### 设置复选框位置和大小

动态调整复选框的位置和尺寸。

```c++
void __stdcall SetCheckBoxBounds(HWND hCheckBox, int x, int y, int width, int height);
```

**易语言声明：**
```
.DLL命令 设置复选框位置, , "emoji_window.dll", "SetCheckBoxBounds"
    .参数 复选框句柄, 整数型
    .参数 X坐标, 整数型
    .参数 Y坐标, 整数型
    .参数 宽度, 整数型
    .参数 高度, 整数型
```

#### 复选框样式说明

复选框采用 Element UI 设计规范：
- 复选框尺寸：14x14 像素
- 圆角半径：2px
- 选中颜色：#409EFF（Element UI 主色）
- 边框颜色：#DCDFE6（默认）/ #409EFF（悬停/选中）
- 禁用颜色：#C0C4CC
- 文本字体：Microsoft YaHei UI，14px
- 文本间距：复选框右侧 8px

**完整易语言示例：**
```
.版本 2

.程序集变量 窗口句柄, 整数型
.程序集变量 复选框1, 整数型

.子程序 _启动窗口_创建完毕

窗口句柄 = 创建Emoji窗口 ("复选框示例", 400, 300)

' 创建复选框
.局部变量 文本UTF8, 字节集
.局部变量 文本指针, 整数型

文本UTF8 = 到UTF8 ("接受用户协议")
文本指针 = 取字节集数据地址 (文本UTF8)

复选框1 = 创建复选框 (窗口句柄, 50, 80, 200, 30, 文本指针, 取字节集长度 (文本UTF8), 假, #FF303133, #FFFFFFFF)

' 设置回调
设置复选框回调 (复选框1, &复选框回调)


.子程序 复选框回调, , 公开, stdcall
.参数 复选框句柄, 整数型
.参数 选中状态, 逻辑型

调试输出 ("复选框状态：", 选中状态)
```

---

### 进度条（ProgressBar）

#### 创建进度条

创建一个 Element UI 风格的进度条控件，支持确定模式和不确定模式。

```c++
HWND __stdcall CreateProgressBar(
    HWND hParent,
    int x, int y, int width, int height,
    int initial_value,
    UINT32 fg_color,
    UINT32 bg_color,
    BOOL show_text,
    UINT32 text_color
);
```

| 参数 | 说明 |
|------|------|
| `hParent` | 父窗口句柄 |
| `x, y` | 控件位置 |
| `width, height` | 控件尺寸 |
| `initial_value` | 初始进度值（0-100） |
| `fg_color` | 前景色/进度条颜色（ARGB格式，如 0xFF409EFF） |
| `bg_color` | 背景色（ARGB格式，如 0xFFEBEEF5） |
| `show_text` | 是否显示百分比文本（TRUE=显示，FALSE=不显示） |
| `text_color` | 文本颜色（ARGB格式，如 0xFFFFFFFF 白色） |

**返回值**：进度条控件句柄

**易语言声明：**
```
.DLL命令 创建进度条, 整数型, "emoji_window.dll", "CreateProgressBar"
    .参数 父窗口句柄, 整数型
    .参数 X坐标, 整数型
    .参数 Y坐标, 整数型
    .参数 宽度, 整数型
    .参数 高度, 整数型
    .参数 初始值, 整数型, , 0-100
    .参数 前景色, 整数型, , ARGB颜色
    .参数 背景色, 整数型, , ARGB颜色
    .参数 显示文本, 逻辑型
    .参数 文本颜色, 整数型, , ARGB颜色
```

**易语言使用示例：**
```
.局部变量 进度条句柄, 整数型

' 创建显示百分比的进度条（白色文本）
进度条句柄 = 创建进度条 (窗口句柄, 50, 80, 400, 30, 0, #409EFF, #EBEEF5, 真, #FFFFFFFF)

' 创建不显示百分比的进度条
进度条句柄 = 创建进度条 (窗口句柄, 50, 120, 400, 20, 50, #67C23A, #EBEEF5, 假, #FFFFFFFF)
```

#### 设置进度条值

设置进度条的当前值（0-100），支持平滑动画过渡。

```c++
void __stdcall SetProgressValue(HWND hProgressBar, int value);
```

| 参数 | 说明 |
|------|------|
| `hProgressBar` | 进度条句柄 |
| `value` | 进度值（0-100，超出范围会自动限制） |

**易语言声明：**
```
.DLL命令 设置进度条值, , "emoji_window.dll", "SetProgressValue"
    .参数 进度条句柄, 整数型
    .参数 值, 整数型, , 0-100
```

**易语言使用示例：**
```
设置进度条值 (进度条句柄, 75)  ' 设置为75%
```

#### 获取进度条值

获取进度条当前的进度值。

```c++
int __stdcall GetProgressValue(HWND hProgressBar);
```

**返回值**：当前进度值（0-100）

**易语言声明：**
```
.DLL命令 获取进度条值, 整数型, "emoji_window.dll", "GetProgressValue"
    .参数 进度条句柄, 整数型
```

#### 设置进度条不确定模式

设置进度条为不确定模式（循环动画），用于无法确定具体进度的场景。

```c++
void __stdcall SetProgressIndeterminate(HWND hProgressBar, BOOL indeterminate);
```

| 参数 | 说明 |
|------|------|
| `hProgressBar` | 进度条句柄 |
| `indeterminate` | TRUE=不确定模式，FALSE=确定模式 |

**易语言声明：**
```
.DLL命令 设置进度条不确定模式, , "emoji_window.dll", "SetProgressIndeterminate"
    .参数 进度条句柄, 整数型
    .参数 不确定模式, 逻辑型
```

**易语言使用示例：**
```
' 启用不确定模式（显示循环动画）
设置进度条不确定模式 (进度条句柄, 真)

' 恢复确定模式
设置进度条不确定模式 (进度条句柄, 假)
```

#### 设置进度条颜色

动态修改进度条的前景色和背景色。

```c++
void __stdcall SetProgressBarColor(HWND hProgressBar, UINT32 fg_color, UINT32 bg_color);
```

**易语言声明：**
```
.DLL命令 设置进度条颜色, , "emoji_window.dll", "SetProgressBarColor"
    .参数 进度条句柄, 整数型
    .参数 前景色, 整数型, , ARGB颜色
    .参数 背景色, 整数型, , ARGB颜色
```

**易语言使用示例：**
```
' 设置为成功状态（绿色）
设置进度条颜色 (进度条句柄, #67C23A, #EBEEF5)

' 设置为警告状态（橙色）
设置进度条颜色 (进度条句柄, #E6A23C, #EBEEF5)

' 设置为错误状态（红色）
设置进度条颜色 (进度条句柄, #F56C6C, #EBEEF5)
```

#### 设置进度条回调

设置进度条值改变时的回调函数。

```c++
typedef void (__stdcall *ProgressBarCallback)(HWND hProgressBar, int value);
void __stdcall SetProgressBarCallback(HWND hProgressBar, ProgressBarCallback callback);
```

| 回调参数 | 说明 |
|---------|------|
| `hProgressBar` | 进度条句柄 |
| `value` | 新的进度值（0-100） |

**易语言声明：**
```
.DLL命令 设置进度条回调, , "emoji_window.dll", "SetProgressBarCallback"
    .参数 进度条句柄, 整数型
    .参数 回调子程序指针, 整数型
```

**易语言回调示例：**
```
.子程序 进度条回调, , 公开, stdcall
.参数 进度条句柄, 整数型
.参数 值, 整数型

调试输出 ("进度改变：", 值, "%")

' 注册回调
设置进度条回调 (进度条句柄, &进度条回调)
```

#### 启用/禁用进度条

控制进度条是否可以交互（禁用时显示灰色）。

```c++
void __stdcall EnableProgressBar(HWND hProgressBar, BOOL enable);
```

**易语言声明：**
```
.DLL命令 启用进度条, , "emoji_window.dll", "EnableProgressBar"
    .参数 进度条句柄, 整数型
    .参数 启用, 逻辑型
```

#### 显示/隐藏进度条

控制进度条的可见性。

```c++
void __stdcall ShowProgressBar(HWND hProgressBar, BOOL show);
```

**易语言声明：**
```
.DLL命令 显示进度条, , "emoji_window.dll", "ShowProgressBar"
    .参数 进度条句柄, 整数型
    .参数 显示, 逻辑型
```

#### 设置进度条位置和大小

动态调整进度条的位置和尺寸。

```c++
void __stdcall SetProgressBarBounds(HWND hProgressBar, int x, int y, int width, int height);
```

**易语言声明：**
```
.DLL命令 设置进度条位置, , "emoji_window.dll", "SetProgressBarBounds"
    .参数 进度条句柄, 整数型
    .参数 X坐标, 整数型
    .参数 Y坐标, 整数型
    .参数 宽度, 整数型
    .参数 高度, 整数型
```

#### 设置是否显示百分比文本

动态控制是否在进度条上显示百分比文本。

```c++
void __stdcall SetProgressBarShowText(HWND hProgressBar, BOOL show_text);
```

**易语言声明：**
```
.DLL命令 设置进度条显示文本, , "emoji_window.dll", "SetProgressBarShowText"
    .参数 进度条句柄, 整数型
    .参数 显示文本, 逻辑型
```

#### 进度条样式说明

进度条采用 Element UI 设计规范：
- 圆角半径：4px
- 边框颜色：#DCDFE6
- 默认前景色：#409EFF（Element UI 主色）
- 默认背景色：#EBEEF5
- 禁用前景色：#C0C4CC
- 禁用背景色：#F5F7FA
- 渐变效果：垂直线性渐变（顶部较亮）
- 动画帧率：60fps
- 平滑过渡：使用缓动函数
- 不确定模式：30% 宽度的进度条循环移动

**Element UI 状态颜色：**
- 主要/信息：#409EFF
- 成功：#67C23A
- 警告：#E6A23C
- 危险/错误：#F56C6C

**完整易语言示例：**
```
.版本 2

.程序集变量 窗口句柄, 整数型
.程序集变量 进度条1, 整数型
.程序集变量 进度条2, 整数型
.程序集变量 当前值, 整数型

.子程序 _启动窗口_创建完毕

窗口句柄 = 创建Emoji窗口 ("进度条示例", 600, 400)

' 创建普通进度条（显示百分比，白色文本）
进度条1 = 创建进度条 (窗口句柄, 50, 80, 500, 30, 0, #409EFF, #EBEEF5, 真, #FFFFFFFF)

' 创建不确定模式进度条
进度条2 = 创建进度条 (窗口句柄, 50, 130, 500, 30, 0, #E6A23C, #EBEEF5, 假, #FFFFFFFF)
设置进度条不确定模式 (进度条2, 真)

' 设置回调
设置进度条回调 (进度条1, &进度条回调)

当前值 = 0


.子程序 按钮_增加进度_被单击

当前值 = 当前值 + 10
.如果真 (当前值 > 100)
    当前值 = 100
.如果真结束

设置进度条值 (进度条1, 当前值)


.子程序 进度条回调, , 公开, stdcall
.参数 进度条句柄, 整数型
.参数 值, 整数型

调试输出 ("进度：", 值, "%")

.如果真 (值 = 100)
    ' 完成时改为绿色
    设置进度条颜色 (进度条句柄, #67C23A, #EBEEF5)
.如果真结束
```

---

### 运行消息循环

```c++
int run_message_loop();
```

## 易语言调用示例

参见 `examples/test.txt`

## 技术细节

- **渲染引擎**: Direct2D
- **文字渲染**: DirectWrite
- **彩色 Emoji**: `D2D1_DRAW_TEXT_OPTIONS_ENABLE_COLOR_FONT`
- **字体**: Segoe UI Emoji
- **编译器**: MSVC 2019+
- **平台**: Windows 10+

## 依赖项

- Windows SDK 10.0 或更高
- Direct2D
- DirectWrite
- 无需额外运行时（静态链接）

## 许可证

MIT License


---

### 图片框（PictureBox）

#### 创建图片框

创建一个图片框控件，支持多种图片格式和缩放模式。

```c++
HWND __stdcall CreatePictureBox(
    HWND hParent,
    int x, int y, int width, int height,
    int scale_mode,
    UINT32 bg_color
);
```

| 参数 | 说明 |
|------|------|
| `hParent` | 父窗口句柄 |
| `x, y` | 控件位置 |
| `width, height` | 控件尺寸 |
| `scale_mode` | 缩放模式（0=不缩放，1=拉伸填充，2=等比缩放适应，3=居中显示） |
| `bg_color` | 背景色（ARGB格式，如 0xFFF5F7FA） |

**返回值**：图片框控件句柄

**缩放模式说明：**
- `SCALE_NONE (0)`: 不缩放，左上角对齐显示原始尺寸
- `SCALE_STRETCH (1)`: 拉伸填充整个控件区域（可能变形）
- `SCALE_FIT (2)`: 等比缩放适应控件（保持宽高比，居中显示）
- `SCALE_CENTER (3)`: 居中显示原始尺寸（不缩放）

**易语言声明：**
```
.DLL命令 创建图片框, 整数型, "emoji_window.dll", "CreatePictureBox"
    .参数 父窗口句柄, 整数型
    .参数 X坐标, 整数型
    .参数 Y坐标, 整数型
    .参数 宽度, 整数型
    .参数 高度, 整数型
    .参数 缩放模式, 整数型, , 0=不缩放 1=拉伸 2=等比缩放 3=居中
    .参数 背景色, 整数型, , ARGB颜色
```

**易语言使用示例：**
```
.局部变量 图片框句柄, 整数型

' 创建等比缩放的图片框
图片框句柄 = 创建图片框 (窗口句柄, 50, 80, 300, 200, 2, #FFF5F7FA)
```

#### 从文件加载图片

从文件加载图片到图片框，支持 PNG、JPG、BMP、GIF 格式。

```c++
BOOL __stdcall LoadImageFromFile(
    HWND hPictureBox,
    const unsigned char* file_path_bytes,
    int path_len
);
```

| 参数 | 说明 |
|------|------|
| `hPictureBox` | 图片框句柄 |
| `file_path_bytes` | UTF-8 编码的文件路径字节集指针 |
| `path_len` | 路径字节集长度 |

**返回值**：TRUE=加载成功，FALSE=加载失败

**支持的图片格式：**
- PNG（支持透明通道）
- JPEG/JPG
- BMP
- GIF（仅显示第一帧）

**易语言声明：**
```
.DLL命令 从文件加载图片, 逻辑型, "emoji_window.dll", "LoadImageFromFile"
    .参数 图片框句柄, 整数型
    .参数 文件路径字节集指针, 整数型
    .参数 路径长度, 整数型
```

**易语言使用示例：**
```
.局部变量 路径UTF8, 字节集
.局部变量 路径指针, 整数型
.局部变量 成功, 逻辑型

路径UTF8 = 到UTF8 ("images\\photo.png")
路径指针 = 取字节集数据地址 (路径UTF8)

成功 = 从文件加载图片 (图片框句柄, 路径指针, 取字节集长度 (路径UTF8))

.如果真 (成功)
    调试输出 ("图片加载成功")
.否则
    调试输出 ("图片加载失败")
.如果真结束
```

#### 从内存加载图片

从内存字节数组加载图片到图片框。

```c++
BOOL __stdcall LoadImageFromMemory(
    HWND hPictureBox,
    const unsigned char* image_data,
    int data_len
);
```

| 参数 | 说明 |
|------|------|
| `hPictureBox` | 图片框句柄 |
| `image_data` | 图片数据字节集指针 |
| `data_len` | 数据字节集长度 |

**返回值**：TRUE=加载成功，FALSE=加载失败

**易语言声明：**
```
.DLL命令 从内存加载图片, 逻辑型, "emoji_window.dll", "LoadImageFromMemory"
    .参数 图片框句柄, 整数型
    .参数 图片数据指针, 整数型
    .参数 数据长度, 整数型
```

**易语言使用示例：**
```
.局部变量 图片数据, 字节集
.局部变量 数据指针, 整数型
.局部变量 成功, 逻辑型

图片数据 = 读入文件 ("photo.png")
数据指针 = 取字节集数据地址 (图片数据)

成功 = 从内存加载图片 (图片框句柄, 数据指针, 取字节集长度 (图片数据))
```

#### 清除图片

清除图片框中的图片，释放图片资源。

```c++
void __stdcall ClearImage(HWND hPictureBox);
```

**易语言声明：**
```
.DLL命令 清除图片, , "emoji_window.dll", "ClearImage"
    .参数 图片框句柄, 整数型
```

**易语言使用示例：**
```
清除图片 (图片框句柄)
```

#### 设置图片透明度

设置图片的透明度（Alpha 通道混合）。

```c++
void __stdcall SetImageOpacity(HWND hPictureBox, float opacity);
```

| 参数 | 说明 |
|------|------|
| `hPictureBox` | 图片框句柄 |
| `opacity` | 透明度（0.0=完全透明，1.0=完全不透明） |

**易语言声明：**
```
.DLL命令 设置图片透明度, , "emoji_window.dll", "SetImageOpacity"
    .参数 图片框句柄, 整数型
    .参数 透明度, 小数型, , 0.0-1.0
```

**易语言使用示例：**
```
' 设置为50%透明度
设置图片透明度 (图片框句柄, 0.5)

' 设置为完全不透明
设置图片透明度 (图片框句柄, 1.0)

' 设置为完全透明
设置图片透明度 (图片框句柄, 0.0)
```

#### 设置图片框回调

设置图片框被点击时的回调函数。

```c++
typedef void (__stdcall *PictureBoxCallback)(HWND hPictureBox);
void __stdcall SetPictureBoxCallback(HWND hPictureBox, PictureBoxCallback callback);
```

| 回调参数 | 说明 |
|---------|------|
| `hPictureBox` | 图片框句柄 |

**易语言声明：**
```
.DLL命令 设置图片框回调, , "emoji_window.dll", "SetPictureBoxCallback"
    .参数 图片框句柄, 整数型
    .参数 回调子程序指针, 整数型
```

**易语言回调示例：**
```
.子程序 图片框回调, , 公开, stdcall
.参数 图片框句柄, 整数型

调试输出 ("图片框被点击")

' 注册回调
设置图片框回调 (图片框句柄, &图片框回调)
```

#### 启用/禁用图片框

控制图片框是否可以交互。

```c++
void __stdcall EnablePictureBox(HWND hPictureBox, BOOL enable);
```

**易语言声明：**
```
.DLL命令 启用图片框, , "emoji_window.dll", "EnablePictureBox"
    .参数 图片框句柄, 整数型
    .参数 启用, 逻辑型
```

#### 显示/隐藏图片框

控制图片框的可见性。

```c++
void __stdcall ShowPictureBox(HWND hPictureBox, BOOL show);
```

**易语言声明：**
```
.DLL命令 显示图片框, , "emoji_window.dll", "ShowPictureBox"
    .参数 图片框句柄, 整数型
    .参数 显示, 逻辑型
```

#### 设置图片框位置和大小

动态调整图片框的位置和尺寸。

```c++
void __stdcall SetPictureBoxBounds(HWND hPictureBox, int x, int y, int width, int height);
```

**易语言声明：**
```
.DLL命令 设置图片框位置, , "emoji_window.dll", "SetPictureBoxBounds"
    .参数 图片框句柄, 整数型
    .参数 X坐标, 整数型
    .参数 Y坐标, 整数型
    .参数 宽度, 整数型
    .参数 高度, 整数型
```

#### 设置图片框缩放模式

动态修改图片框的缩放模式。

```c++
void __stdcall SetPictureBoxScaleMode(HWND hPictureBox, int scale_mode);
```

**易语言声明：**
```
.DLL命令 设置图片框缩放模式, , "emoji_window.dll", "SetPictureBoxScaleMode"
    .参数 图片框句柄, 整数型
    .参数 缩放模式, 整数型, , 0=不缩放 1=拉伸 2=等比缩放 3=居中
```

**易语言使用示例：**
```
' 切换为拉伸模式
设置图片框缩放模式 (图片框句柄, 1)

' 切换为等比缩放模式
设置图片框缩放模式 (图片框句柄, 2)
```

#### 设置图片框背景色

动态修改图片框的背景色。

```c++
void __stdcall SetPictureBoxBackgroundColor(HWND hPictureBox, UINT32 bg_color);
```

**易语言声明：**
```
.DLL命令 设置图片框背景色, , "emoji_window.dll", "SetPictureBoxBackgroundColor"
    .参数 图片框句柄, 整数型
    .参数 背景色, 整数型, , ARGB颜色
```

#### 图片框技术说明

图片框使用以下技术实现：
- **图片加载**：WIC (Windows Imaging Component)
- **图片渲染**：Direct2D
- **支持格式**：PNG、JPEG、BMP、GIF（通过 WIC 解码器）
- **透明通道**：完全支持 Alpha 通道混合
- **硬件加速**：使用 GPU 加速渲染
- **插值模式**：线性插值（D2D1_BITMAP_INTERPOLATION_MODE_LINEAR）

**完整易语言示例：**
```
.版本 2

.程序集变量 窗口句柄, 整数型
.程序集变量 图片框1, 整数型
.程序集变量 图片框2, 整数型

.子程序 _启动窗口_创建完毕

窗口句柄 = 创建Emoji窗口 ("图片框示例", 800, 600)

' 创建拉伸模式图片框
图片框1 = 创建图片框 (窗口句柄, 50, 80, 300, 200, 1, #FFF5F7FA)

' 创建等比缩放模式图片框
图片框2 = 创建图片框 (窗口句柄, 400, 80, 300, 200, 2, #FFF5F7FA)

' 加载图片
.局部变量 路径UTF8, 字节集
.局部变量 路径指针, 整数型

路径UTF8 = 到UTF8 ("images\\photo.png")
路径指针 = 取字节集数据地址 (路径UTF8)

从文件加载图片 (图片框1, 路径指针, 取字节集长度 (路径UTF8))
从文件加载图片 (图片框2, 路径指针, 取字节集长度 (路径UTF8))

' 设置透明度
设置图片透明度 (图片框1, 0.8)

' 设置回调
设置图片框回调 (图片框1, &图片框回调)


.子程序 图片框回调, , 公开, stdcall
.参数 图片框句柄, 整数型

调试输出 ("图片框被点击")
```

**缩放模式常量（在常量表.e中定义）：**
```
.常量 SCALE_NONE, "0", , 不缩放
.常量 SCALE_STRETCH, "1", , 拉伸填充
.常量 SCALE_FIT, "2", , 等比缩放适应
.常量 SCALE_CENTER, "3", , 居中显示
```

