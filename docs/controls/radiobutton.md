# 单选按钮控件 (RadioButton)

[← 返回主文档](../../README.md)

## 概述

Element UI 风格的单选按钮控件，支持分组互斥选择。同一分组内只能选中一个按钮。

## 创建单选按钮

```c++
HWND __stdcall CreateRadioButton(
    HWND hParent,
    int x, int y, int width, int height,
    const unsigned char* text_bytes, int text_len,
    int group_id,
    BOOL checked,
    UINT32 fg_color,
    UINT32 bg_color,
    const unsigned char* font_name_bytes,
    int font_name_len,
    int font_size,
    BOOL bold,
    BOOL italic,
    BOOL underline
);
```

**参数说明：**

| 参数 | 说明 |
|------|------|
| `hParent` | 父窗口句柄 |
| `x, y` | 控件位置 |
| `width, height` | 控件尺寸 |
| `text_bytes` | UTF-8 编码的文本字节集指针 |
| `text_len` | 文本字节集长度 |
| `group_id` | 分组ID（相同ID的单选按钮互斥） |
| `checked` | 初始选中状态（TRUE=选中，FALSE=未选中） |
| `fg_color` | 前景色（ARGB格式） |
| `bg_color` | 背景色（ARGB格式） |
| `font_name_bytes` | UTF-8 编码的字体名字节集指针（传NULL使用默认字体） |
| `font_name_len` | 字体名字节集长度 |
| `font_size` | 字体大小，像素（传0使用默认14px） |
| `bold` | 是否粗体（TRUE=粗体） |
| `italic` | 是否斜体（TRUE=斜体） |
| `underline` | 是否下划线（TRUE=下划线） |

**返回值：** 单选按钮控件句柄

## 状态操作

### 获取选中状态

```c++
BOOL __stdcall GetRadioButtonState(HWND hRadioButton);
```

**返回值：** TRUE=选中，FALSE=未选中

### 设置选中状态

```c++
void __stdcall SetRadioButtonState(HWND hRadioButton, BOOL checked);
```

### 设置回调

```c++
typedef void (__stdcall *RadioButtonCallback)(HWND hRadioButton, int group_id, BOOL checked);
void __stdcall SetRadioButtonCallback(HWND hRadioButton, RadioButtonCallback callback);
```

**回调参数：**

| 参数 | 说明 |
|------|------|
| `hRadioButton` | 触发回调的单选按钮句柄 |
| `group_id` | 分组ID |
| `checked` | 选中状态 |

## 基础属性操作

### 启用/禁用

```c++
void __stdcall EnableRadioButton(HWND hRadioButton, BOOL enable);
```

### 显示/隐藏

```c++
void __stdcall ShowRadioButton(HWND hRadioButton, BOOL show);
```

### 设置文本

```c++
void __stdcall SetRadioButtonText(HWND hRadioButton, const unsigned char* text_bytes, int text_len);
```

### 设置位置和大小

```c++
void __stdcall SetRadioButtonBounds(HWND hRadioButton, int x, int y, int width, int height);
```

## 扩展属性操作

### 获取单选按钮文本

获取单选按钮的当前文本内容（UTF-8格式）。

```c++
int __stdcall GetRadioButtonText(HWND hRadioButton, unsigned char* buffer, int buffer_size);
```

**参数说明：**

| 参数 | 说明 |
|------|------|
| `hRadioButton` | 单选按钮窗口句柄 |
| `buffer` | 接收文本的缓冲区指针（传0获取所需大小） |
| `buffer_size` | 缓冲区大小（字节） |

**返回值：** 成功返回实际字节数，失败返回-1

**使用模式（两次调用）：**
1. 第一次传0获取所需缓冲区大小
2. 第二次传入缓冲区获取实际内容

### 设置单选按钮字体

设置单选按钮的字体并立即重绘。

```c++
void __stdcall SetRadioButtonFont(
    HWND hRadioButton,
    const unsigned char* font_name, int font_name_len,
    int font_size,
    int bold, int italic, int underline
);
```

**参数说明：**

| 参数 | 说明 |
|------|------|
| `hRadioButton` | 单选按钮窗口句柄 |
| `font_name` | UTF-8字体名字节集指针 |
| `font_name_len` | 字体名字节集长度 |
| `font_size` | 字体大小（0=不修改） |
| `bold` | 1=粗体，0=非粗体 |
| `italic` | 1=斜体，0=非斜体 |
| `underline` | 1=下划线，0=无下划线 |

### 获取单选按钮字体

获取单选按钮的字体信息。

```c++
int __stdcall GetRadioButtonFont(
    HWND hRadioButton,
    unsigned char* font_name_buffer, int buffer_size,
    int* font_size,
    int* bold, int* italic, int* underline
);
```

**参数说明：**

| 参数 | 说明 |
|------|------|
| `hRadioButton` | 单选按钮窗口句柄 |
| `font_name_buffer` | 接收字体名的缓冲区指针（传0获取所需大小） |
| `buffer_size` | 缓冲区大小 |
| `font_size` | 输出参数：字体大小 |
| `bold` | 输出参数：是否粗体（1=是，0=否） |
| `italic` | 输出参数：是否斜体（1=是，0=否） |
| `underline` | 输出参数：是否下划线（1=是，0=否） |

**返回值：** 成功返回字体名UTF-8字节数，失败返回-1

### 设置单选按钮颜色

设置单选按钮的前景色和背景色并立即重绘。

```c++
void __stdcall SetRadioButtonColor(HWND hRadioButton, UINT32 fg_color, UINT32 bg_color);
```

**参数说明：**

| 参数 | 说明 |
|------|------|
| `hRadioButton` | 单选按钮窗口句柄 |
| `fg_color` | 前景色（ARGB格式） |
| `bg_color` | 背景色（ARGB格式） |

### 获取单选按钮颜色

获取单选按钮的前景色和背景色。

```c++
int __stdcall GetRadioButtonColor(HWND hRadioButton, UINT32* fg_color, UINT32* bg_color);
```

**参数说明：**

| 参数 | 说明 |
|------|------|
| `hRadioButton` | 单选按钮窗口句柄 |
| `fg_color` | 输出参数：前景色ARGB |
| `bg_color` | 输出参数：背景色ARGB |

**返回值：** 成功返回0，失败返回-1

## 分组机制

- 相同 `group_id` 的单选按钮属于同一组
- 同一组内只能有一个按钮被选中
- 选中一个按钮会自动取消同组其他按钮的选中状态
- 不同组的单选按钮互不影响

## 样式说明

单选按钮采用 Element UI 设计规范：

- 圆形按钮尺寸：14x14 像素
- 选中颜色：#409EFF（Element UI 主色）
- 边框颜色：#DCDFE6（默认）/ #409EFF（悬停/选中）
- 禁用颜色：#C0C4CC
- 文本字体：Microsoft YaHei UI，14px
- 文本间距：单选按钮右侧 8px

## 易语言声明

```
.DLL命令 创建单选按钮, 整数型, "emoji_window.dll", "CreateRadioButton"
    .参数 父窗口句柄, 整数型
    .参数 X坐标, 整数型
    .参数 Y坐标, 整数型
    .参数 宽度, 整数型
    .参数 高度, 整数型
    .参数 文本字节集指针, 整数型
    .参数 文本长度, 整数型
    .参数 分组ID, 整数型
    .参数 初始选中, 逻辑型
    .参数 前景色, 整数型
    .参数 背景色, 整数型
    .参数 字体名字节集指针, 整数型, , UTF-8字体名字节集指针
    .参数 字体名长度, 整数型, , 字体名字节集长度
    .参数 字体大小, 整数型, , 字体大小（像素）
    .参数 粗体, 逻辑型
    .参数 斜体, 逻辑型
    .参数 下划线, 逻辑型

.DLL命令 获取单选按钮状态, 逻辑型, "emoji_window.dll", "GetRadioButtonState"
    .参数 单选按钮句柄, 整数型

.DLL命令 设置单选按钮状态, , "emoji_window.dll", "SetRadioButtonState"
    .参数 单选按钮句柄, 整数型
    .参数 选中状态, 逻辑型

.DLL命令 设置单选按钮回调, , "emoji_window.dll", "SetRadioButtonCallback"
    .参数 单选按钮句柄, 整数型
    .参数 回调函数, 子程序指针

.DLL命令 启用单选按钮, , "emoji_window.dll", "EnableRadioButton"
    .参数 单选按钮句柄, 整数型
    .参数 启用, 逻辑型

.DLL命令 显示单选按钮, , "emoji_window.dll", "ShowRadioButton"
    .参数 单选按钮句柄, 整数型
    .参数 显示, 逻辑型

.DLL命令 设置单选按钮文本, , "emoji_window.dll", "SetRadioButtonText"
    .参数 单选按钮句柄, 整数型
    .参数 文本字节集指针, 整数型
    .参数 文本长度, 整数型

.DLL命令 设置单选按钮位置, , "emoji_window.dll", "SetRadioButtonBounds"
    .参数 单选按钮句柄, 整数型
    .参数 X坐标, 整数型
    .参数 Y坐标, 整数型
    .参数 宽度, 整数型
    .参数 高度, 整数型

' ========== 单选按钮扩展属性命令 ==========

.DLL命令 获取单选按钮文本, 整数型, "emoji_window.dll", "GetRadioButtonText"
    .参数 单选按钮句柄, 整数型
    .参数 缓冲区指针, 整数型
    .参数 缓冲区大小, 整数型

.DLL命令 设置单选按钮字体, , "emoji_window.dll", "SetRadioButtonFont"
    .参数 单选按钮句柄, 整数型
    .参数 字体名字节集指针, 整数型
    .参数 字体名长度, 整数型
    .参数 字体大小, 整数型
    .参数 粗体, 整数型
    .参数 斜体, 整数型
    .参数 下划线, 整数型

.DLL命令 获取单选按钮字体, 整数型, "emoji_window.dll", "GetRadioButtonFont"
    .参数 单选按钮句柄, 整数型
    .参数 字体名缓冲区指针, 整数型
    .参数 字体名缓冲区大小, 整数型
    .参数 字体大小, 整数型, 传址
    .参数 粗体, 整数型, 传址
    .参数 斜体, 整数型, 传址
    .参数 下划线, 整数型, 传址

.DLL命令 设置单选按钮颜色, , "emoji_window.dll", "SetRadioButtonColor"
    .参数 单选按钮句柄, 整数型
    .参数 前景色, 整数型
    .参数 背景色, 整数型

.DLL命令 获取单选按钮颜色, 整数型, "emoji_window.dll", "GetRadioButtonColor"
    .参数 单选按钮句柄, 整数型
    .参数 前景色, 整数型, 传址
    .参数 背景色, 整数型, 传址
```
