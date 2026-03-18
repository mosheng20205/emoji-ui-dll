# 选项卡控件 (TabControl)

[← 返回主文档](../../README.md)

## 概述

多标签页容器控件,支持动态添加/删除标签页、自动创建内容窗口和标签页切换回调。

## API 文档

### 创建选项卡控件

```c++
HWND __stdcall CreateTabControl(
    HWND hParent,
    int x, int y, int width, int height
);
```

**参数说明:**

| 参数 | 说明 |
|------|------|
| `hParent` | 父窗口句柄 |
| `x, y` | 控件位置 |
| `width, height` | 控件尺寸 |

**返回值:** 选项卡控件句柄

### 添加/删除标签页

```c++
int __stdcall AddTabPage(HWND hTabControl, const unsigned char* title_bytes, int title_len, int icon_index);
void __stdcall RemoveTabPage(HWND hTabControl, int index);
```

**参数说明:**
- `title_bytes`: UTF-8 编码的标签页标题(支持 Emoji)
- `title_len`: 标题长度
- `icon_index`: 图标索引(暂未实现,传 0)
- `index`: 标签页索引(从 0 开始)

### 获取/设置当前标签页

```c++
int __stdcall GetCurrentTabIndex(HWND hTabControl);
void __stdcall SetCurrentTabIndex(HWND hTabControl, int index);
```

### 获取标签页内容窗口

```c++
HWND __stdcall GetTabContentWindow(HWND hTabControl, int index);
```

每个标签页都有一个独立的内容窗口,可以在其中创建子控件。

### 设置标签页标题

```c++
void __stdcall SetTabPageTitle(HWND hTabControl, int index, const unsigned char* title_bytes, int title_len);
```

### 设置切换回调

```c++
typedef void (__stdcall *TabSwitchCallback)(HWND hTabControl, int current_index);
void __stdcall SetTabSwitchCallback(HWND hTabControl, TabSwitchCallback callback);
```

**回调参数:**
- `hTabControl`: 选项卡控件句柄
- `current_index`: 当前选中的标签页索引

### 销毁选项卡

```c++
void __stdcall DestroyTabControl(HWND hTabControl);
```

### 其他操作

```c++
void __stdcall EnableTabControl(HWND hTabControl, BOOL enable);
void __stdcall ShowTabControl(HWND hTabControl, BOOL show);
```

---

## 属性获取/设置 API（v2.1 新增）

### 获取 Tab 页标题

```c++
int __stdcall GetTabTitle(
    HWND hTabControl,
    int index,
    unsigned char* buffer,
    int bufferSize
);
```

**参数说明：**

| 参数 | 说明 |
|------|------|
| `hTabControl` | TabControl 窗口句柄 |
| `index` | Tab 页索引（从 0 开始） |
| `buffer` | 接收标题的缓冲区指针（传 NULL 获取所需大小） |
| `bufferSize` | 缓冲区大小（字节） |

**返回值：** 成功返回 UTF-8 字节数，失败返回 -1

**两次调用模式：**
- 第一次：`buffer=NULL, bufferSize=0`，返回所需字节数
- 第二次：传入足够大的缓冲区，返回实际写入字节数

**易语言示例：**
```
.局部变量 标题长度, 整数型
.局部变量 标题缓冲区, 字节集
.局部变量 结果字节集, 字节集

' 第一次调用获取所需长度
标题长度 ＝ 获取Tab页标题 (TabControl句柄, 0, 0, 0)
.如果 (标题长度 ＞ 0)
    ' 第二次调用获取实际内容
    标题缓冲区 ＝ 取空白字节集 (标题长度)
    获取Tab页标题 (TabControl句柄, 0, 取变量数据地址 (标题缓冲区), 标题长度)
    ' 标题缓冲区 即为 UTF-8 字节集，可直接拼接显示
    结果字节集 ＝ 编码_Ansi到Utf8 ("Tab[0]标题：") ＋ 标题缓冲区
.如果结束
```

---

### 设置 Tab 页标题

```c++
int __stdcall SetTabTitle(
    HWND hTabControl,
    int index,
    const unsigned char* title_bytes,
    int title_len
);
```

**参数说明：**

| 参数 | 说明 |
|------|------|
| `hTabControl` | TabControl 窗口句柄 |
| `index` | Tab 页索引（从 0 开始） |
| `title_bytes` | UTF-8 编码的新标题字节集指针 |
| `title_len` | 标题字节集长度 |

**返回值：** 成功返回 0，失败返回 -1

**易语言示例：**
```
.局部变量 新标题字节集, 字节集

' 设置 Tab[0] 的标题（支持 Emoji）
' "✅ 已完成"  ✅ = { 226, 156, 133 }
新标题字节集 ＝ { 226, 156, 133, 32 } ＋ 编码_Ansi到Utf8 ("已完成")
设置Tab页标题 (TabControl句柄, 0, 取变量数据地址 (新标题字节集), 取字节集长度 (新标题字节集))
```

---

### 获取 TabControl 位置和大小

```c++
int __stdcall GetTabControlBounds(
    HWND hTabControl,
    int* x,
    int* y,
    int* width,
    int* height
);
```

**参数说明：**

| 参数 | 说明 |
|------|------|
| `hTabControl` | TabControl 窗口句柄 |
| `x` | 输出：X 坐标（相对父窗口客户区） |
| `y` | 输出：Y 坐标（相对父窗口客户区） |
| `width` | 输出：宽度 |
| `height` | 输出：高度 |

**返回值：** 成功返回 0，失败返回 -1

**易语言示例：**
```
.局部变量 X, 整数型
.局部变量 Y, 整数型
.局部变量 宽度, 整数型
.局部变量 高度, 整数型

.如果真 (获取TabControl位置 (TabControl句柄, X, Y, 宽度, 高度) ＝ 0)
    ' 使用 X, Y, 宽度, 高度
.如果真结束
```

---

### 设置 TabControl 位置和大小

```c++
int __stdcall SetTabControlBounds(
    HWND hTabControl,
    int x,
    int y,
    int width,
    int height
);
```

**参数说明：**

| 参数 | 说明 |
|------|------|
| `hTabControl` | TabControl 窗口句柄 |
| `x` | 新 X 坐标（相对父窗口客户区） |
| `y` | 新 Y 坐标（相对父窗口客户区） |
| `width` | 新宽度 |
| `height` | 新高度 |

**返回值：** 成功返回 0，失败返回 -1

**易语言示例：**
```
' 将 TabControl 移动到新位置并调整大小
设置TabControl位置 (TabControl句柄, 20, 100, 760, 400)
```

---

### 获取 TabControl 可视状态

```c++
int __stdcall GetTabControlVisible(HWND hTabControl);
```

**返回值：** 1（可见），0（不可见），-1（无效句柄）

**易语言示例：**
```
.局部变量 可视状态, 整数型

可视状态 ＝ 获取TabControl可视状态 (TabControl句柄)
.判断开始 (可视状态 ＝ 1)
    ' TabControl 可见
.判断 (可视状态 ＝ 0)
    ' TabControl 不可见
.默认
    ' 获取失败（无效句柄）
.判断结束
```

---

### 显示或隐藏 TabControl

```c++
int __stdcall ShowTabControl(HWND hTabControl, int visible);
```

**参数说明：**

| 参数 | 说明 |
|------|------|
| `hTabControl` | TabControl 窗口句柄 |
| `visible` | 1=显示，0=隐藏 |

**返回值：** 成功返回 0，失败返回 -1

**行为说明：**
- 隐藏 TabControl 时，当前激活的内容窗口也会同步隐藏
- 显示 TabControl 时，当前激活的内容窗口也会同步显示

**易语言示例：**
```
' 隐藏 TabControl
显示隐藏TabControl (TabControl句柄, 0)

' 显示 TabControl
显示隐藏TabControl (TabControl句柄, 1)
```

---

### 启用或禁用 TabControl

```c++
int __stdcall EnableTabControl(HWND hTabControl, int enabled);
```

**参数说明：**

| 参数 | 说明 |
|------|------|
| `hTabControl` | TabControl 窗口句柄 |
| `enabled` | 1=启用，0=禁用 |

**返回值：** 成功返回 0，失败返回 -1

**行为说明：**
- 禁用 TabControl 后，所有 Tab 页不可切换
- 禁用状态下 TabControl 仍然可见，但不响应鼠标点击

**易语言示例：**
```
' 禁用 TabControl（Tab 页不可切换）
启用禁用TabControl (TabControl句柄, 0)

' 重新启用 TabControl
启用禁用TabControl (TabControl句柄, 1)
```

## 样式说明

- 标签页高度: 40px
- 标签页最小宽度: 80px
- 标签页最大宽度: 200px
- 标签页间距: 2px
- 选中标签页背景: #409EFF
- 选中标签页文本: #FFFFFF
- 未选中标签页背景: #F5F7FA
- 未选中标签页文本: #606266
- 悬停背景: #ECF5FF
- 内容区域背景: #FFFFFF
- 支持彩色 Emoji 标题

## 使用流程

1. 创建 TabControl
2. 添加标签页(自动创建内容窗口)
3. 获取标签页内容窗口
4. 在内容窗口中创建子控件
5. 设置切换回调(可选)

## 易语言声明

```
.DLL命令 创建TabControl, 整数型, "emoji_window.dll", "CreateTabControl"
    .参数 父窗口句柄, 整数型
    .参数 X坐标, 整数型
    .参数 Y坐标, 整数型
    .参数 宽度, 整数型
    .参数 高度, 整数型

.DLL命令 添加Tab页, 整数型, "emoji_window.dll", "AddTabItem"
    .参数 TabControl句柄, 整数型
    .参数 标题字节集指针, 整数型
    .参数 标题长度, 整数型
    .参数 图标索引, 整数型

.DLL命令 移除Tab页, 逻辑型, "emoji_window.dll", "RemoveTabItem"
    .参数 TabControl句柄, 整数型
    .参数 索引, 整数型

.DLL命令 设置Tab切换回调, , "emoji_window.dll", "SetTabCallback"
    .参数 TabControl句柄, 整数型
    .参数 回调函数, 子程序指针

.DLL命令 获取当前Tab索引, 整数型, "emoji_window.dll", "GetCurrentTabIndex"
    .参数 TabControl句柄, 整数型

.DLL命令 切换到Tab, 逻辑型, "emoji_window.dll", "SelectTab"
    .参数 TabControl句柄, 整数型
    .参数 索引, 整数型

.DLL命令 获取Tab数量, 整数型, "emoji_window.dll", "GetTabCount"
    .参数 TabControl句柄, 整数型

.DLL命令 获取Tab内容窗口, 整数型, "emoji_window.dll", "GetTabContentWindow"
    .参数 TabControl句柄, 整数型
    .参数 索引, 整数型

.DLL命令 销毁TabControl, , "emoji_window.dll", "DestroyTabControl"
    .参数 TabControl句柄, 整数型

.DLL命令 更新TabControl布局, , "emoji_window.dll", "UpdateTabControlLayout"
    .参数 TabControl句柄, 整数型

' ========== TabControl 属性命令（v2.1 新增）==========

.DLL命令 获取Tab页标题, 整数型, "emoji_window.dll", "GetTabTitle", , , 获取指定Tab页的标题（返回UTF-8字节数，缓冲区指针为0时返回所需大小）
    .参数 TabControl句柄, 整数型, , TabControl窗口句柄
    .参数 索引, 整数型, , Tab页索引（从0开始）
    .参数 缓冲区指针, 整数型, , 接收标题的缓冲区指针（传0获取所需大小）
    .参数 缓冲区大小, 整数型, , 缓冲区大小（字节）

.DLL命令 设置Tab页标题, 整数型, "emoji_window.dll", "SetTabTitle", , , 设置指定Tab页的标题（成功返回0，失败返回-1）
    .参数 TabControl句柄, 整数型, , TabControl窗口句柄
    .参数 索引, 整数型, , Tab页索引（从0开始）
    .参数 标题字节集指针, 整数型, , UTF-8编码的标题字节集指针
    .参数 标题长度, 整数型, , 标题字节集长度

.DLL命令 获取TabControl位置, 整数型, "emoji_window.dll", "GetTabControlBounds", , , 获取TabControl位置和大小（成功返回0，失败返回-1）
    .参数 TabControl句柄, 整数型, , TabControl窗口句柄
    .参数 X坐标, 整数型, 传址, 输出：X坐标（相对父窗口）
    .参数 Y坐标, 整数型, 传址, 输出：Y坐标（相对父窗口）
    .参数 宽度, 整数型, 传址, 输出：宽度
    .参数 高度, 整数型, 传址, 输出：高度

.DLL命令 设置TabControl位置, 整数型, "emoji_window.dll", "SetTabControlBounds", , , 设置TabControl位置和大小（成功返回0，失败返回-1）
    .参数 TabControl句柄, 整数型, , TabControl窗口句柄
    .参数 X坐标, 整数型, , 新X坐标（相对父窗口）
    .参数 Y坐标, 整数型, , 新Y坐标（相对父窗口）
    .参数 宽度, 整数型, , 新宽度
    .参数 高度, 整数型, , 新高度

.DLL命令 获取TabControl可视状态, 整数型, "emoji_window.dll", "GetTabControlVisible", , , 获取TabControl可视状态（1=可见，0=不可见，-1=错误）
    .参数 TabControl句柄, 整数型, , TabControl窗口句柄

.DLL命令 显示隐藏TabControl, 整数型, "emoji_window.dll", "ShowTabControl", , , 显示或隐藏TabControl（成功返回0，失败返回-1）
    .参数 TabControl句柄, 整数型, , TabControl窗口句柄
    .参数 是否显示, 整数型, , 1=显示，0=隐藏

.DLL命令 启用禁用TabControl, 整数型, "emoji_window.dll", "EnableTabControl", , , 启用或禁用TabControl（成功返回0，失败返回-1）
    .参数 TabControl句柄, 整数型, , TabControl窗口句柄
    .参数 是否启用, 整数型, , 1=启用，0=禁用
```

## 易语言示例

### 基础示例（创建 TabControl 并添加 Tab 页）

```
.版本 2

.程序集变量 窗口句柄, 整数型
.程序集变量 TabControl句柄, 整数型
.程序集变量 Tab1内容窗口, 整数型
.程序集变量 Tab2内容窗口, 整数型
.程序集变量 Tab3内容窗口, 整数型
.程序集变量 按钮1, 整数型
.程序集变量 按钮2, 整数型

.子程序 _启动窗口_创建完毕

窗口句柄 = 创建Emoji窗口 ("选项卡示例", 800, 600)

' 创建 TabControl
TabControl句柄 = 创建TabControl (窗口句柄, 20, 20, 760, 560)

' 添加标签页(支持 Emoji)
添加Tab页_辅助 (TabControl句柄, "❤️ 首页", 0)
添加Tab页_辅助 (TabControl句柄, "🔧 设置", 0)
添加Tab页_辅助 (TabControl句柄, "⏰ 关于", 0)

' 设置切换回调
设置Tab切换回调 (TabControl句柄, &Tab切换回调)

' 获取各个标签页的内容窗口
Tab1内容窗口 = 获取Tab内容窗口 (TabControl句柄, 0)
Tab2内容窗口 = 获取Tab内容窗口 (TabControl句柄, 1)
Tab3内容窗口 = 获取Tab内容窗口 (TabControl句柄, 2)

' 在 Tab1 中创建控件
按钮1 = 创建Emoji按钮_辅助 (Tab1内容窗口, "⭐", "收藏", 30, 30, 150, 50, #COLOR_PRIMARY)

' 在 Tab2 中创建控件
按钮2 = 创建Emoji按钮_辅助 (Tab2内容窗口, "⚙️", "设置项", 30, 30, 150, 50, #COLOR_SUCCESS)

' 在 Tab3 中创建标签
创建标签_辅助 (Tab3内容窗口, 30, 30, 700, 100, "这是关于页面" + #换行符 + "版本: 1.0.0" + #换行符 + "作者: XXX", #COLOR_TEXT_PRIMARY, #COLOR_BG_LIGHT, , , , , , , 真)

设置按钮点击回调 (&按钮点击回调)

运行消息循环 ()


.子程序 Tab切换回调, , 公开, stdcall
.参数 TabControl, 整数型
.参数 当前索引, 整数型

调试输出 ("Tab切换到索引: " + 到文本 (当前索引))


.子程序 按钮点击回调, , 公开, stdcall
.参数 按钮ID, 整数型
.参数 父窗口句柄, 整数型

.判断开始 (父窗口句柄 = Tab1内容窗口)
    .如果真 (按钮ID = 按钮1)
        信息框 ("这是Tab1中的按钮", 0, "提示")
    .如果真结束
    
.判断 (父窗口句柄 = Tab2内容窗口)
    .如果真 (按钮ID = 按钮2)
        信息框 ("这是Tab2中的按钮", 0, "提示")
    .如果真结束
.判断结束


.子程序 _启动窗口_将被销毁

' 销毁 TabControl
.如果真 (TabControl句柄 ≠ 0)
    销毁TabControl (TabControl句柄)
    TabControl句柄 = 0
.如果真结束

' 销毁窗口
.如果真 (窗口句柄 ≠ 0)
    销毁窗口 (窗口句柄)
    窗口句柄 = 0
.如果真结束
```

## 动态添加/删除示例

```
.版本 2

.程序集变量 计数器, 整数型

.子程序 添加新标签页

计数器 = 计数器 + 1
添加Tab页_辅助 (TabControl句柄, "新标签页 " + 到文本 (计数器), 0)

' 获取新标签页的内容窗口
.局部变量 新窗口, 整数型
.局部变量 索引, 整数型

索引 = 获取当前Tab索引 (TabControl句柄)
新窗口 = 获取Tab内容窗口 (TabControl句柄, 索引)

' 在新窗口中创建控件
创建标签_辅助 (新窗口, 20, 20, 300, 30, "这是新添加的标签页", #COLOR_TEXT_PRIMARY, #COLOR_BG_WHITE)


.子程序 删除当前标签页
.局部变量 索引, 整数型

索引 = 获取当前Tab索引 (TabControl句柄)
.如果真 (索引 ≥ 0)
    移除Tab页 (TabControl句柄, 索引)
.如果真结束
```

### 属性操作示例（v2.1 新增）

```
.版本 2

.程序集变量 主窗口句柄, 整数型
.程序集变量 TabControl句柄, 整数型
.程序集变量 标签_结果, 整数型
.程序集变量 按钮_获取标题, 整数型
.程序集变量 按钮_设置标题, 整数型
.程序集变量 按钮_获取位置, 整数型
.程序集变量 按钮_切换可视, 整数型
.程序集变量 按钮_切换启用, 整数型
.程序集变量 TabControl是否可见, 整数型

.子程序 _启动子程序, 整数型
.局部变量 utf8字节集, 字节集
.局部变量 字体名utf8, 字节集
.局部变量 Tab标题字节集, 字节集
.局部变量 按钮文本字节集, 字节集

' 创建窗口
utf8字节集 ＝ 编码_Ansi到Utf8 ("TabControl属性操作示例")
主窗口句柄 ＝ 创建Emoji窗口_字节集_扩展 (取变量数据地址 (utf8字节集), 取字节集长度 (utf8字节集), 800, 600, 到RGB (70, 130, 180))

' 创建结果显示标签
utf8字节集 ＝ 编码_Ansi到Utf8 ("点击下方按钮操作TabControl属性")
字体名utf8 ＝ 编码_Ansi到Utf8 ("微软雅黑")
标签_结果 ＝ 创建标签 (主窗口句柄, 20, 10, 760, 100, 取变量数据地址 (utf8字节集), 取字节集长度 (utf8字节集), 到ARGB (255, 0, 0, 0), 到ARGB (255, 245, 245, 245), 取变量数据地址 (字体名utf8), 取字节集长度 (字体名utf8), 12, 假, 假, 假, 0, 真)

' 创建 TabControl（3个Tab页）
TabControl句柄 ＝ 创建TabControl (主窗口句柄, 20, 120, 760, 300)

' 添加Tab页（使用字节集方式支持Emoji）
' "📋 基本信息"  📋 = { 240, 159, 147, 139 }
Tab标题字节集 ＝ { 240, 159, 147, 139, 32 } ＋ 编码_Ansi到Utf8 ("基本信息")
添加Tab页 (TabControl句柄, 取变量数据地址 (Tab标题字节集), 取字节集长度 (Tab标题字节集), 0)

' "⚙ 设置选项"  ⚙ = { 226, 154, 153 }
Tab标题字节集 ＝ { 226, 154, 153, 32 } ＋ 编码_Ansi到Utf8 ("设置选项")
添加Tab页 (TabControl句柄, 取变量数据地址 (Tab标题字节集), 取字节集长度 (Tab标题字节集), 0)

' "🔧 高级功能"  🔧 = { 240, 159, 148, 167 }
Tab标题字节集 ＝ { 240, 159, 148, 167, 32 } ＋ 编码_Ansi到Utf8 ("高级功能")
添加Tab页 (TabControl句柄, 取变量数据地址 (Tab标题字节集), 取字节集长度 (Tab标题字节集), 0)

TabControl是否可见 ＝ 1

' 创建操作按钮（保存ID到程序集变量）
按钮文本字节集 ＝ 编码_Ansi到Utf8 ("获取Tab标题")
按钮_获取标题 ＝ 创建Emoji按钮_字节集 (主窗口句柄, 0, 0, 取变量数据地址 (按钮文本字节集), 取字节集长度 (按钮文本字节集), 20, 440, 150, 35, 到ARGB (255, 64, 158, 255))

按钮文本字节集 ＝ 编码_Ansi到Utf8 ("设置Tab标题")
按钮_设置标题 ＝ 创建Emoji按钮_字节集 (主窗口句柄, 0, 0, 取变量数据地址 (按钮文本字节集), 取字节集长度 (按钮文本字节集), 185, 440, 150, 35, 到ARGB (255, 64, 158, 255))

按钮文本字节集 ＝ 编码_Ansi到Utf8 ("获取位置大小")
按钮_获取位置 ＝ 创建Emoji按钮_字节集 (主窗口句柄, 0, 0, 取变量数据地址 (按钮文本字节集), 取字节集长度 (按钮文本字节集), 350, 440, 150, 35, 到ARGB (255, 64, 158, 255))

按钮文本字节集 ＝ 编码_Ansi到Utf8 ("切换显示/隐藏")
按钮_切换可视 ＝ 创建Emoji按钮_字节集 (主窗口句柄, 0, 0, 取变量数据地址 (按钮文本字节集), 取字节集长度 (按钮文本字节集), 20, 490, 150, 35, 到ARGB (255, 230, 162, 60))

按钮文本字节集 ＝ 编码_Ansi到Utf8 ("切换启用/禁用")
按钮_切换启用 ＝ 创建Emoji按钮_字节集 (主窗口句柄, 0, 0, 取变量数据地址 (按钮文本字节集), 取字节集长度 (按钮文本字节集), 185, 490, 150, 35, 到ARGB (255, 230, 162, 60))

设置按钮点击回调 (&按钮点击回调)
设置消息循环主窗口 (主窗口句柄)
运行消息循环 ()
返回 (0)


.子程序 按钮点击回调, , , stdcall
.参数 按钮ID, 整数型
.参数 _父窗口句柄, 整数型
.局部变量 结果字节集, 字节集
.局部变量 标题长度, 整数型
.局部变量 标题缓冲区, 字节集
.局部变量 新标题字节集, 字节集
.局部变量 X, 整数型
.局部变量 Y, 整数型
.局部变量 宽度, 整数型
.局部变量 高度, 整数型
.局部变量 i, 整数型
.局部变量 Tab数量, 整数型

.判断开始 (按钮ID ＝ 按钮_获取标题)
    ' 获取所有Tab页标题
    Tab数量 ＝ 获取Tab数量 (TabControl句柄)
    结果字节集 ＝ 编码_Ansi到Utf8 ("【获取Tab页标题】") ＋ { 10 } ＋ 编码_Ansi到Utf8 ("Tab总数：") ＋ 编码_Ansi到Utf8 (到文本 (Tab数量)) ＋ { 10 }
    i ＝ 0
    .判断循环首 (i ＜ Tab数量)
        标题长度 ＝ 获取Tab页标题 (TabControl句柄, i, 0, 0)
        .如果 (标题长度 ＞ 0)
            标题缓冲区 ＝ 取空白字节集 (标题长度)
            获取Tab页标题 (TabControl句柄, i, 取变量数据地址 (标题缓冲区), 标题长度)
            结果字节集 ＝ 结果字节集 ＋ 编码_Ansi到Utf8 ("Tab[") ＋ 编码_Ansi到Utf8 (到文本 (i)) ＋ 编码_Ansi到Utf8 ("]：") ＋ 标题缓冲区 ＋ { 10 }
        .否则
            结果字节集 ＝ 结果字节集 ＋ 编码_Ansi到Utf8 ("Tab[") ＋ 编码_Ansi到Utf8 (到文本 (i)) ＋ 编码_Ansi到Utf8 ("]：（空）") ＋ { 10 }
        .如果结束
        i ＝ i ＋ 1
    .判断循环尾 ()

.判断 (按钮ID ＝ 按钮_设置标题)
    ' 设置Tab[0]标题（含Emoji）
    ' "✅ 已修改"  ✅ = { 226, 156, 133 }
    新标题字节集 ＝ { 226, 156, 133, 32 } ＋ 编码_Ansi到Utf8 ("已修改标题")
    .如果 (设置Tab页标题 (TabControl句柄, 0, 取变量数据地址 (新标题字节集), 取字节集长度 (新标题字节集)) ＝ 0)
        结果字节集 ＝ 编码_Ansi到Utf8 ("【设置Tab页标题】") ＋ { 10 } ＋ 编码_Ansi到Utf8 ("已将Tab[0]标题修改为：") ＋ 新标题字节集
    .否则
        结果字节集 ＝ 编码_Ansi到Utf8 ("【设置Tab页标题】") ＋ { 10 } ＋ 编码_Ansi到Utf8 ("设置失败（返回-1）")
    .如果结束

.判断 (按钮ID ＝ 按钮_获取位置)
    ' 获取TabControl位置和大小
    .如果 (获取TabControl位置 (TabControl句柄, X, Y, 宽度, 高度) ＝ 0)
        结果字节集 ＝ 编码_Ansi到Utf8 ("【获取TabControl位置大小】") ＋ { 10 } ＋ 编码_Ansi到Utf8 ("X=") ＋ 编码_Ansi到Utf8 (到文本 (X)) ＋ 编码_Ansi到Utf8 ("  Y=") ＋ 编码_Ansi到Utf8 (到文本 (Y)) ＋ { 10 } ＋ 编码_Ansi到Utf8 ("宽=") ＋ 编码_Ansi到Utf8 (到文本 (宽度)) ＋ 编码_Ansi到Utf8 ("  高=") ＋ 编码_Ansi到Utf8 (到文本 (高度))
    .否则
        结果字节集 ＝ 编码_Ansi到Utf8 ("【获取TabControl位置大小】") ＋ { 10 } ＋ 编码_Ansi到Utf8 ("获取失败（返回-1）")
    .如果结束

.判断 (按钮ID ＝ 按钮_切换可视)
    ' 切换显示/隐藏
    .如果 (TabControl是否可见 ＝ 1)
        TabControl是否可见 ＝ 0
    .否则
        TabControl是否可见 ＝ 1
    .如果结束
    显示隐藏TabControl (TabControl句柄, TabControl是否可见)
    .如果 (TabControl是否可见 ＝ 1)
        结果字节集 ＝ 编码_Ansi到Utf8 ("【切换可视状态】") ＋ { 10 } ＋ 编码_Ansi到Utf8 ("已显示TabControl")
    .否则
        结果字节集 ＝ 编码_Ansi到Utf8 ("【切换可视状态】") ＋ { 10 } ＋ 编码_Ansi到Utf8 ("已隐藏TabControl（再次点击可重新显示）")
    .如果结束

.判断 (按钮ID ＝ 按钮_切换启用)
    ' 禁用后立即重新启用（演示效果）
    启用禁用TabControl (TabControl句柄, 0)
    结果字节集 ＝ 编码_Ansi到Utf8 ("【切换启用/禁用】") ＋ { 10 } ＋ 编码_Ansi到Utf8 ("已禁用TabControl（Tab页不可切换）") ＋ { 10 } ＋ 编码_Ansi到Utf8 ("正在自动重新启用...")
    启用禁用TabControl (TabControl句柄, 1)
    结果字节集 ＝ 结果字节集 ＋ { 10 } ＋ 编码_Ansi到Utf8 ("已重新启用TabControl")

.默认

.判断结束

设置标签文本 (标签_结果, 取变量数据地址 (结果字节集), 取字节集长度 (结果字节集))


.子程序 到ARGB, 整数型
.参数 A, 整数型
.参数 R, 整数型
.参数 G, 整数型
.参数 B, 整数型

返回 (左移 (A, 24) ＋ 左移 (R, 16) ＋ 左移 (G, 8) ＋ B)

.子程序 到RGB, 整数型
.参数 R, 整数型
.参数 G, 整数型
.参数 B, 整数型

返回 (左移 (R, 16) ＋ 左移 (G, 8) ＋ B)
```

## 注意事项

⚠️ **重要提示：**

1. 标签页标题支持 UTF-8 编码和彩色 Emoji，使用字节集方式传递（参考易语言 Emoji 使用注意事项）
2. 每个标签页都有独立的内容窗口
3. 在内容窗口中创建的控件坐标相对于内容窗口
4. 切换标签页时，其他标签页的内容会自动隐藏
5. 销毁 TabControl 前应先销毁其中的子控件
6. 窗口大小改变时需要手动调整 TabControl 大小
7. **两次调用模式**：`GetTabTitle` 使用两次调用模式，第一次传 0 获取所需缓冲区大小，第二次传入缓冲区获取实际内容
8. **UTF-8 字节集显示**：DLL 返回的标题缓冲区是 UTF-8 字节集，不能用 `到文本()` 转换，必须全程用字节集拼接后传给显示函数
9. **按钮 ID 保存**：创建按钮时必须将返回的 ID 保存到程序集变量，不能用硬编码数字判断
10. **ShowTabControl 行为**：隐藏 TabControl 时，当前激活的内容窗口也会同步隐藏；显示时同步显示

## 窗口大小改变处理

```
.子程序 窗口大小改变回调, , 公开, stdcall
.参数 窗口句柄, 整数型
.参数 新宽度, 整数型
.参数 新高度, 整数型

' 调整 TabControl 大小
.如果真 (TabControl句柄 ≠ 0)
    MoveWindow (TabControl句柄, 20, 20, 新宽度 - 40, 新高度 - 40, 真)
.如果真结束
```

## 相关文档

- [分组框](groupbox.md)
- [按钮控件](button.md)
- [布局管理器](../layout.md)
