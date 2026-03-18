# 按钮控件 (Button)

[← 返回主文档](../../README.md)

## 创建 Emoji 按钮

创建支持彩色 Emoji 的按钮控件。

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

### 参数说明

| 参数 | 说明 |
|------|------|
| `parent` | 父窗口句柄 |
| `emoji_bytes` | UTF-8 编码的 Emoji 字节集指针 |
| `emoji_len` | Emoji 字节集长度 |
| `text_bytes` | UTF-8 编码的文本字节集指针 |
| `text_len` | 文本字节集长度 |
| `x, y` | 按钮位置 |
| `width, height` | 按钮尺寸 |
| `bg_color` | 背景色（ARGB格式） |

### 返回值

按钮 ID，失败返回 -1

## 设置按钮点击回调

```c++
typedef void (__stdcall *ButtonClickCallback)(int button_id, HWND parent_hwnd);
void __stdcall set_button_click_callback(ButtonClickCallback callback);
```

### 回调参数说明

| 参数 | 说明 |
|------|------|
| `button_id` | 被点击的按钮 ID |
| `parent_hwnd` | 按钮所在的父窗口句柄 |

> **提示**：`parent_hwnd` 参数在 TabControl 等多窗口场景中非常有用，可以通过父窗口句柄区分不同 Tab 页中的按钮。

## 按钮属性操作

### 获取按钮文本

获取按钮的当前文本内容（UTF-8格式）。

```c++
int __stdcall GetButtonText(int button_id, unsigned char* buffer, int buffer_size);
```

**参数说明:**

| 参数 | 说明 |
|------|------|
| `button_id` | 按钮 ID |
| `buffer` | 接收文本的缓冲区指针（第一次调用传NULL获取长度） |
| `buffer_size` | 缓冲区大小（字节） |

**返回值:** 成功返回实际字节数，失败返回-1

**使用模式（两次调用）:**
1. 第一次调用传NULL获取所需缓冲区大小
2. 第二次调用传入缓冲区获取实际内容

### 设置按钮文本

设置按钮的文本内容并立即重绘。

```c++
void __stdcall SetButtonText(int button_id, const unsigned char* text, int text_len);
```

**参数说明:**

| 参数 | 说明 |
|------|------|
| `button_id` | 按钮 ID |
| `text` | UTF-8编码的文本字节集指针 |
| `text_len` | 文本字节集长度 |

### 获取按钮Emoji

获取按钮的当前Emoji内容（UTF-8格式）。

```c++
int __stdcall GetButtonEmoji(int button_id, unsigned char* buffer, int buffer_size);
```

**参数说明:**

| 参数 | 说明 |
|------|------|
| `button_id` | 按钮 ID |
| `buffer` | 接收Emoji的缓冲区指针（第一次调用传NULL获取长度） |
| `buffer_size` | 缓冲区大小（字节） |

**返回值:** 成功返回实际字节数，失败返回-1

### 设置按钮Emoji

设置按钮的Emoji内容并立即重绘。

```c++
void __stdcall SetButtonEmoji(int button_id, const unsigned char* emoji, int emoji_len);
```

**参数说明:**

| 参数 | 说明 |
|------|------|
| `button_id` | 按钮 ID |
| `emoji` | UTF-8编码的Emoji字节集指针 |
| `emoji_len` | Emoji字节集长度 |

### 获取按钮位置和大小

获取按钮的位置和尺寸信息。

```c++
int __stdcall GetButtonBounds(int button_id, int* x, int* y, int* width, int* height);
```

**参数说明:**

| 参数 | 说明 |
|------|------|
| `button_id` | 按钮 ID |
| `x` | 接收X坐标的指针 |
| `y` | 接收Y坐标的指针 |
| `width` | 接收宽度的指针 |
| `height` | 接收高度的指针 |

**返回值:** 成功返回0，失败返回-1

### 设置按钮位置和大小

设置按钮的位置和尺寸并立即更新显示。

```c++
void __stdcall SetButtonBounds(int button_id, int x, int y, int width, int height);
```

**参数说明:**

| 参数 | 说明 |
|------|------|
| `button_id` | 按钮 ID |
| `x` | 新的X坐标 |
| `y` | 新的Y坐标 |
| `width` | 新的宽度 |
| `height` | 新的高度 |

### 获取按钮背景色

获取按钮的背景颜色（ARGB格式）。

```c++
unsigned int __stdcall GetButtonBackgroundColor(int button_id);
```

**参数说明:**

| 参数 | 说明 |
|------|------|
| `button_id` | 按钮 ID |

**返回值:** ARGB格式的颜色值（32位整数）

### 设置按钮背景色

设置按钮的背景颜色并立即重绘。

```c++
void __stdcall SetButtonBackgroundColor(int button_id, unsigned int color);
```

**参数说明:**

| 参数 | 说明 |
|------|------|
| `button_id` | 按钮 ID |
| `color` | ARGB格式的颜色值 |

### 获取按钮可视状态

获取按钮的可视状态。

```c++
int __stdcall GetButtonVisible(int button_id);
```

**参数说明:**

| 参数 | 说明 |
|------|------|
| `button_id` | 按钮 ID |

**返回值:** 1=可见，0=隐藏，-1=失败

### 显示/隐藏按钮

显示或隐藏按钮。

```c++
void __stdcall ShowButton(int button_id, int visible);
```

**参数说明:**

| 参数 | 说明 |
|------|------|
| `button_id` | 按钮 ID |
| `visible` | 1=显示，0=隐藏 |

### 获取按钮启用状态

获取按钮的启用状态。

```c++
int __stdcall GetButtonEnabled(int button_id);
```

**参数说明:**

| 参数 | 说明 |
|------|------|
| `button_id` | 按钮 ID |

**返回值:** 1=启用，0=禁用，-1=失败

## 启用/禁用按钮

### 启用按钮

```c++
void __stdcall EnableButton(HWND parent_hwnd, int button_id, BOOL enable);
```

**参数说明:**

| 参数 | 说明 |
|------|------|
| `parent_hwnd` | 按钮所在的父窗口句柄 |
| `button_id` | 按钮 ID |
| `enable` | TRUE=启用，FALSE=禁用 |

### 禁用按钮

```c++
void __stdcall DisableButton(HWND parent_hwnd, int button_id);
```

这是 `EnableButton` 的快捷方式，等同于 `EnableButton(parent_hwnd, button_id, FALSE)`。

### 禁用状态视觉效果

- 浅灰色背景 (#F5F7FA)
- 浅灰色边框 (#E4E7ED)
- 浅灰色文本 (#C0C4CC)
- 鼠标悬停无效果
- 点击无响应，不触发回调

## 易语言声明

```
.DLL命令 创建Emoji按钮_字节集, 整数型, "emoji_window.dll", "create_emoji_button_bytes"
    .参数 父窗口句柄, 整数型
    .参数 Emoji字节集指针, 整数型
    .参数 Emoji长度, 整数型
    .参数 文本字节集指针, 整数型
    .参数 文本长度, 整数型
    .参数 X坐标, 整数型
    .参数 Y坐标, 整数型
    .参数 宽度, 整数型
    .参数 高度, 整数型
    .参数 背景色, 整数型

.DLL命令 设置按钮点击回调, , "emoji_window.dll", "set_button_click_callback"
    .参数 回调函数指针, 子程序指针

.DLL命令 启用按钮, , "emoji_window.dll", "EnableButton"
    .参数 父窗口句柄, 整数型
    .参数 按钮ID, 整数型
    .参数 启用, 逻辑型

.DLL命令 禁用按钮, , "emoji_window.dll", "DisableButton"
    .参数 父窗口句柄, 整数型
    .参数 按钮ID, 整数型

' ========== 按钮属性操作命令 ==========

.DLL命令 获取按钮文本, 整数型, "emoji_window.dll", "GetButtonText"
    .参数 按钮ID, 整数型
    .参数 缓冲区, 字节集, 传址, 可空
    .参数 缓冲区大小, 整数型

.DLL命令 设置按钮文本, , "emoji_window.dll", "SetButtonText"
    .参数 按钮ID, 整数型
    .参数 文本, 字节集
    .参数 文本长度, 整数型

.DLL命令 获取按钮Emoji, 整数型, "emoji_window.dll", "GetButtonEmoji"
    .参数 按钮ID, 整数型
    .参数 缓冲区, 字节集, 传址, 可空
    .参数 缓冲区大小, 整数型

.DLL命令 设置按钮Emoji, , "emoji_window.dll", "SetButtonEmoji"
    .参数 按钮ID, 整数型
    .参数 Emoji, 字节集
    .参数 Emoji长度, 整数型

.DLL命令 获取按钮位置, 整数型, "emoji_window.dll", "GetButtonBounds"
    .参数 按钮ID, 整数型
    .参数 X坐标, 整数型, 传址
    .参数 Y坐标, 整数型, 传址
    .参数 宽度, 整数型, 传址
    .参数 高度, 整数型, 传址

.DLL命令 设置按钮位置, , "emoji_window.dll", "SetButtonBounds"
    .参数 按钮ID, 整数型
    .参数 X坐标, 整数型
    .参数 Y坐标, 整数型
    .参数 宽度, 整数型
    .参数 高度, 整数型

.DLL命令 获取按钮背景色, 整数型, "emoji_window.dll", "GetButtonBackgroundColor"
    .参数 按钮ID, 整数型

.DLL命令 设置按钮背景色, , "emoji_window.dll", "SetButtonBackgroundColor"
    .参数 按钮ID, 整数型
    .参数 颜色, 整数型

.DLL命令 获取按钮可视状态, 整数型, "emoji_window.dll", "GetButtonVisible"
    .参数 按钮ID, 整数型

.DLL命令 显示隐藏按钮, , "emoji_window.dll", "ShowButton"
    .参数 按钮ID, 整数型
    .参数 可见, 整数型

.DLL命令 获取按钮启用状态, 整数型, "emoji_window.dll", "GetButtonEnabled"
    .参数 按钮ID, 整数型
```

## 易语言使用示例

### 基础示例

```
.程序集变量 按钮ID, 整数型

.子程序 _启动窗口_创建完毕
.局部变量 emoji字节集, 字节集
.局部变量 文本字节集, 字节集

emoji字节集 = { 240, 159, 152, 128 }  ' 😀
文本字节集 = 到UTF8 ("点击我")

按钮ID = 创建Emoji按钮_字节集 (窗口句柄, 取字节集数据地址 (emoji字节集), 取字节集长度 (emoji字节集), 取字节集数据地址 (文本字节集), 取字节集长度 (文本字节集), 10, 10, 100, 40, #409EFF)

设置按钮点击回调 (&按钮点击处理)


.子程序 按钮点击处理, , 公开, stdcall
.参数 按钮ID_, 整数型
.参数 父窗口句柄, 整数型

.如果真 (按钮ID_ = 按钮ID)
    信息框 ("按钮被点击了！", 0, "提示")
.如果真结束
```

### 按钮属性操作示例

```
.程序集变量 窗口句柄, 整数型
.程序集变量 按钮ID, 整数型

.子程序 _启动窗口_创建完毕
.局部变量 长度, 整数型
.局部变量 缓冲区, 字节集
.局部变量 文本, 文本型
.局部变量 x, 整数型
.局部变量 y, 整数型
.局部变量 宽度, 整数型
.局部变量 高度, 整数型
.局部变量 颜色, 整数型
.局部变量 新文本, 字节集

窗口句柄 = 创建Emoji窗口 ("按钮属性操作示例", 500, 400)
按钮ID = 创建Emoji按钮_辅助 (窗口句柄, "😀", "原始文本", 50, 50, 150, 40, #409EFF)

设置按钮点击回调 (&按钮回调)

' 获取按钮文本（两次调用模式）
长度 = 获取按钮文本 (按钮ID, , 0)  ' 第一次调用获取长度
.如果真 (长度 > 0)
    缓冲区 = 取空白字节集 (长度)
    长度 = 获取按钮文本 (按钮ID, 缓冲区, 长度)  ' 第二次调用获取内容
    文本 = 到文本 (缓冲区)
    调试输出 ("按钮文本: " + 文本)
.如果真结束

' 修改按钮文本
新文本 = 到UTF8 ("新文本")
设置按钮文本 (按钮ID, 新文本, 取字节集长度 (新文本))

' 获取按钮位置和大小
获取按钮位置 (按钮ID, x, y, 宽度, 高度)
调试输出 ("按钮位置: " + 到文本 (x) + ", " + 到文本 (y))
调试输出 ("按钮大小: " + 到文本 (宽度) + " x " + 到文本 (高度))

' 修改按钮位置和大小
设置按钮位置 (按钮ID, 100, 100, 200, 50)

' 获取和修改背景色
颜色 = 获取按钮背景色 (按钮ID)
调试输出 ("原始背景色: " + 到文本 (颜色))
设置按钮背景色 (按钮ID, #67C23A)  ' 改为绿色

' 显示/隐藏按钮
显示隐藏按钮 (按钮ID, 0)  ' 隐藏
延时 (1000)
显示隐藏按钮 (按钮ID, 1)  ' 显示

运行消息循环 ()


.子程序 按钮回调, , 公开, stdcall
.参数 按钮ID_, 整数型
.参数 父窗口句柄, 整数型

.如果真 (按钮ID_ = 按钮ID)
    信息框 ("按钮被点击了！", 0, "提示")
.如果真结束
```

### 动态修改按钮示例

```
.程序集变量 窗口句柄, 整数型
.程序集变量 按钮ID, 整数型
.程序集变量 计数器, 整数型

.子程序 _启动窗口_创建完毕

窗口句柄 = 创建Emoji窗口 ("动态按钮示例", 400, 300)
按钮ID = 创建Emoji按钮_辅助 (窗口句柄, "🔢", "点击次数: 0", 100, 100, 200, 50, #409EFF)
计数器 = 0

设置按钮点击回调 (&按钮回调)
运行消息循环 ()


.子程序 按钮回调, , 公开, stdcall
.参数 按钮ID_, 整数型
.参数 父窗口句柄, 整数型
.局部变量 新文本, 字节集
.局部变量 颜色, 整数型

.如果真 (按钮ID_ = 按钮ID)
    计数器 = 计数器 + 1
    
    ' 更新按钮文本
    新文本 = 到UTF8 ("点击次数: " + 到文本 (计数器))
    设置按钮文本 (按钮ID, 新文本, 取字节集长度 (新文本))
    
    ' 根据点击次数改变颜色
    .判断开始 (计数器 ≤ 3)
        颜色 = #409EFF  ' 蓝色
    .判断 (计数器 ≤ 6)
        颜色 = #67C23A  ' 绿色
    .判断 (计数器 ≤ 9)
        颜色 = #E6A23C  ' 橙色
    .默认
        颜色 = #F56C6C  ' 红色
    .判断结束
    
    设置按钮背景色 (按钮ID, 颜色)
    
    ' 点击10次后隐藏按钮
    .如果真 (计数器 ≥ 10)
        显示隐藏按钮 (按钮ID, 0)
        信息框 ("按钮已隐藏！", 0, "提示")
    .如果真结束
.如果真结束
```

### 按钮启用/禁用示例

```
.程序集变量 窗口句柄, 整数型
.程序集变量 按钮1, 整数型
.程序集变量 按钮2, 整数型
.程序集变量 按钮3, 整数型

.子程序 _启动窗口_创建完毕

窗口句柄 = 创建Emoji窗口 ("按钮启用/禁用示例", 400, 300)

' 创建三个按钮
按钮1 = 创建Emoji按钮_辅助 (窗口句柄, "", "正常按钮", 50, 50, 120, 40, #COLOR_PRIMARY)
按钮2 = 创建Emoji按钮_辅助 (窗口句柄, "", "禁用按钮2", 50, 110, 120, 40, #COLOR_DANGER)
按钮3 = 创建Emoji按钮_辅助 (窗口句柄, "", "启用按钮2", 50, 170, 120, 40, #COLOR_SUCCESS)

设置按钮点击回调 (&按钮回调)

' 初始状态：按钮2 被禁用
禁用按钮 (窗口句柄, 按钮2)

运行消息循环 ()


.子程序 按钮回调, , 公开, stdcall
.参数 按钮ID, 整数型
.参数 父窗口句柄, 整数型

.如果真 (按钮ID = 按钮1)
    信息框 ("你点击了正常按钮", 0, "提示")
.如果真结束

.如果真 (按钮ID = 按钮2)
    ' 这个按钮被禁用时不会触发回调
    信息框 ("按钮2被点击了", 0, "提示")
.如果真结束

.如果真 (按钮ID = 按钮3)
    ' 启用按钮2
    启用按钮 (父窗口句柄, 按钮2, 真)
    信息框 ("已启用按钮2", 0, "提示")
.如果真结束
```

### TabControl 多窗口示例

在 TabControl 中，不同 Tab 页的按钮 ID 可能重复。此时可以通过父窗口句柄来区分：

```
.程序集变量 Tab1内容窗口, 整数型
.程序集变量 Tab2内容窗口, 整数型
.程序集变量 按钮1_ID, 整数型
.程序集变量 按钮2_ID, 整数型

.子程序 创建界面
' 创建 TabControl 和 Tab 页
TabControl句柄 = 创建TabControl (主窗口, 10, 10, 600, 400)
添加Tab页 (TabControl句柄, "首页", 0)
添加Tab页 (TabControl句柄, "设置", 0)

' 获取各 Tab 的内容窗口
Tab1内容窗口 = 获取Tab内容窗口 (TabControl句柄, 0)
Tab2内容窗口 = 获取Tab内容窗口 (TabControl句柄, 1)

' 在不同 Tab 中创建按钮（ID 可能相同）
按钮1_ID = 创建Emoji按钮_字节集 (Tab1内容窗口, ...)  ' 返回 1000
按钮2_ID = 创建Emoji按钮_字节集 (Tab2内容窗口, ...)  ' 也可能返回 1000

设置按钮点击回调 (&按钮点击处理)


.子程序 按钮点击处理, , 公开, stdcall
.参数 按钮ID, 整数型
.参数 父窗口句柄, 整数型

' 通过父窗口句柄区分不同 Tab
.判断开始 (父窗口句柄 = Tab1内容窗口)
    .判断开始 (按钮ID = 按钮1_ID)
        信息框 ("Tab1 的按钮被点击", 0, )
    .判断结束
    
.判断 (父窗口句柄 = Tab2内容窗口)
    .判断开始 (按钮ID = 按钮2_ID)
        信息框 ("Tab2 的按钮被点击", 0, )
    .判断结束
    
.判断结束
```

## 注意事项

⚠️ **重要提示**：

1. **Emoji 输入**: 在易语言 IDE 中，Unicode 特殊符号（如 emoji）不支持直接输入，因为易语言使用 ANSI 编码。必须使用预先转换好的 UTF-8 字节集或从外部文件读取。详见 [常见问题](../faq.md#emoji-显示问题)。

2. **启用/禁用**: 
   - 禁用的按钮会显示为灰色，无法点击
   - 禁用的按钮不会触发回调函数
   - 分组框被禁用时，分组框内的所有按钮也会自动禁用
   - 使用 `EnableButton` 或 `DisableButton` 控制按钮状态

3. **父窗口句柄**: 
   - 启用/禁用按钮时必须传入正确的父窗口句柄
   - 在分组框中创建的按钮，父窗口句柄仍然是主窗口句柄，不是分组框句柄

4. **两次调用模式**:
   - 所有获取文本的API（GetButtonText、GetButtonEmoji）使用两次调用模式
   - 第一次调用传NULL获取所需缓冲区大小
   - 第二次调用传入缓冲区获取实际内容
   - 这种模式避免了内存管理问题，让调用者控制缓冲区

5. **属性修改立即生效**:
   - 所有Set函数（SetButtonText、SetButtonEmoji、SetButtonBounds、SetButtonBackgroundColor）修改后立即触发重绘
   - 无需手动调用刷新函数

6. **颜色格式**:
   - 背景色使用ARGB格式（32位整数）
   - 格式：0xAARRGGBB（AA=透明度，RR=红色，GG=绿色，BB=蓝色）
   - 示例：0xFF409EFF（完全不透明的蓝色）

7. **按钮ID有效性**:
   - 所有属性API都会验证按钮ID是否有效
   - 无效ID会返回-1或不执行操作
   - 按钮销毁后，其ID不再有效

8. **线程安全**:
   - 所有API应在主UI线程调用
   - 不支持多线程并发访问

9. **UTF-8编码**:
   - 所有文本参数必须使用UTF-8编码
   - 易语言中使用"到UTF8"函数转换文本
   - 支持中文、日文、韩文、Emoji等多语言字符

10. **性能考虑**:
    - 频繁修改属性可能影响性能
    - 如需批量修改，考虑在修改完成后统一刷新
    - 获取属性操作非常快速（<1ms）

## 相关文档

- [按钮启用/禁用详细说明](按钮启用禁用.md)
- [分组框控件](groupbox.md)
- [常见问题](../faq.md)
