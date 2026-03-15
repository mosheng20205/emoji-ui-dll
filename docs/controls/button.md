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

## 相关文档

- [按钮启用/禁用详细说明](按钮启用禁用.md)
- [分组框控件](groupbox.md)
- [常见问题](../faq.md)
