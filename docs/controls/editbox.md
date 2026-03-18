# 编辑框控件 (EditBox)

[← 返回主文档](../../README.md)

## 概述

单行/多行文本输入控件，支持自定义字体、颜色、对齐方式，以及密码模式、只读模式等。

## API 文档

### 创建编辑框

```c++
HWND __stdcall CreateEditBox(
    HWND hParent,
    int x, int y, int width, int height,
    const unsigned char* text_bytes, int text_len,
    UINT32 fg_color, UINT32 bg_color,
    const unsigned char* font_name_bytes, int font_name_len,
    int font_size,
    int bold, int italic, int underline,
    int alignment,
    int multiline, int readonly, int password,
    int has_border, int vertical_center
);
```

**参数说明：**

| 参数 | 说明 |
|------|------|
| `hParent` | 父窗口句柄 |
| `x, y` | 控件位置 |
| `width, height` | 控件尺寸 |
| `text_bytes` | UTF-8 编码的初始文本指针 |
| `text_len` | 初始文本字节长度 |
| `fg_color` | 前景色（ARGB格式） |
| `bg_color` | 背景色（ARGB格式） |
| `font_name_bytes` | UTF-8 编码的字体名指针 |
| `font_name_len` | 字体名字节长度 |
| `font_size` | 字体大小（像素） |
| `bold` | 1=粗体，0=正常 |
| `italic` | 1=斜体，0=正常 |
| `underline` | 1=下划线，0=无 |
| `alignment` | 对齐方式：0=左，1=中，2=右 |
| `multiline` | 1=多行，0=单行 |
| `readonly` | 1=只读，0=可编辑 |
| `password` | 1=密码模式，0=普通 |
| `has_border` | 1=有边框，0=无边框 |
| `vertical_center` | 1=文本垂直居中（仅单行有效） |

**返回值：** 编辑框控件句柄

### 获取/设置文本

```c++
int  __stdcall GetEditBoxText(HWND hEdit, unsigned char* buffer, int buffer_size);
void __stdcall SetEditBoxText(HWND hEdit, const unsigned char* text_bytes, int text_len);
```

`GetEditBoxText` 使用两次调用模式：第一次传 `NULL/0` 获取所需字节数，第二次传入缓冲区获取内容。

### 基础属性操作

```c++
void __stdcall SetEditBoxFont(HWND hEdit, const unsigned char* font_name_bytes, int font_name_len,
                              int font_size, int bold, int italic, int underline);
void __stdcall SetEditBoxColor(HWND hEdit, UINT32 fg_color, UINT32 bg_color);
void __stdcall SetEditBoxBounds(HWND hEdit, int x, int y, int width, int height);
void __stdcall EnableEditBox(HWND hEdit, BOOL enable);
void __stdcall ShowEditBox(HWND hEdit, BOOL show);
void __stdcall SetEditBoxVerticalCenter(HWND hEdit, BOOL vertical_center);
void __stdcall SetEditBoxKeyCallback(HWND hEdit, EditBoxKeyCallback callback);
```

---

## 扩展属性获取 API（v2.1 新增）

### 获取编辑框字体

```c++
int __stdcall GetEditBoxFont(
    HWND hEdit,
    unsigned char* font_name_buffer, int buffer_size,
    int* font_size,
    int* bold, int* italic, int* underline
);
```

**参数说明：**

| 参数 | 说明 |
|------|------|
| `hEdit` | 编辑框句柄 |
| `font_name_buffer` | 接收字体名的缓冲区（传 NULL 获取所需大小） |
| `buffer_size` | 缓冲区大小（字节） |
| `font_size` | 输出：字体大小 |
| `bold` | 输出：是否粗体（1=是，0=否） |
| `italic` | 输出：是否斜体（1=是，0=否） |
| `underline` | 输出：是否下划线（1=是，0=否） |

**返回值：** 成功返回字体名 UTF-8 字节数，失败返回 -1

**两次调用模式：**
- 第一次：`font_name_buffer=NULL, buffer_size=0`，返回所需字节数
- 第二次：传入足够大的缓冲区，返回实际写入字节数

**易语言示例：**
```
.局部变量 字体名长度, 整数型
.局部变量 字体名缓冲区, 字节集
.局部变量 字体大小, 整数型
.局部变量 粗体, 整数型
.局部变量 斜体, 整数型
.局部变量 下划线, 整数型
.局部变量 结果字节集, 字节集

' 第一次调用获取所需长度
字体名长度 ＝ 获取编辑框字体 (编辑框句柄, 0, 0, 字体大小, 粗体, 斜体, 下划线)
.如果 (字体名长度 ＞ 0)
    ' 第二次调用获取实际内容
    字体名缓冲区 ＝ 取空白字节集 (字体名长度)
    获取编辑框字体 (编辑框句柄, 取变量数据地址 (字体名缓冲区), 字体名长度, 字体大小, 粗体, 斜体, 下划线)
    ' 字体名缓冲区 是 UTF-8 字节集，直接拼接显示
    结果字节集 ＝ 编码_Ansi到Utf8 ("字体名：") ＋ 字体名缓冲区 ＋ { 10 } ＋ 编码_Ansi到Utf8 ("大小：") ＋ 编码_Ansi到Utf8 (到文本 (字体大小))
.如果结束
```

---

### 获取编辑框颜色

```c++
int __stdcall GetEditBoxColor(HWND hEdit, UINT32* fg_color, UINT32* bg_color);
```

**参数说明：**

| 参数 | 说明 |
|------|------|
| `hEdit` | 编辑框句柄 |
| `fg_color` | 输出：前景色（ARGB 格式） |
| `bg_color` | 输出：背景色（ARGB 格式） |

**返回值：** 0（成功），-1（失败）

**易语言示例：**
```
.局部变量 前景色, 整数型
.局部变量 背景色, 整数型

.如果真 (获取编辑框颜色 (编辑框句柄, 前景色, 背景色) ＝ 0)
    ' 使用 前景色, 背景色
.如果真结束
```

---

### 获取编辑框位置和大小

```c++
int __stdcall GetEditBoxBounds(HWND hEdit, int* x, int* y, int* width, int* height);
```

**参数说明：**

| 参数 | 说明 |
|------|------|
| `hEdit` | 编辑框句柄 |
| `x` | 输出：X 坐标（相对父窗口客户区） |
| `y` | 输出：Y 坐标（相对父窗口客户区） |
| `width` | 输出：宽度 |
| `height` | 输出：高度 |

**返回值：** 0（成功），-1（失败）

**易语言示例：**
```
.局部变量 X, 整数型
.局部变量 Y, 整数型
.局部变量 宽度, 整数型
.局部变量 高度, 整数型

.如果真 (获取编辑框位置 (编辑框句柄, X, Y, 宽度, 高度) ＝ 0)
    ' 使用 X, Y, 宽度, 高度
.如果真结束
```

---

### 获取编辑框对齐方式

```c++
int __stdcall GetEditBoxAlignment(HWND hEdit);
```

**返回值：** 0=左对齐，1=居中，2=右对齐，-1=错误

**易语言示例：**
```
.局部变量 对齐方式, 整数型

对齐方式 ＝ 获取编辑框对齐方式 (编辑框句柄)
.判断开始 (对齐方式 ＝ 0)
    ' 左对齐
.判断 (对齐方式 ＝ 1)
    ' 居中
.判断 (对齐方式 ＝ 2)
    ' 右对齐
.判断结束
```

---

### 获取编辑框启用状态

```c++
int __stdcall GetEditBoxEnabled(HWND hEdit);
```

**返回值：** 1（已启用），0（已禁用），-1（无效句柄）

---

### 获取编辑框可视状态

```c++
int __stdcall GetEditBoxVisible(HWND hEdit);
```

**返回值：** 1（可见），0（不可见），-1（无效句柄）

---

## 易语言声明

```
.DLL命令 创建编辑框, 整数型, "emoji_window.dll", "CreateEditBox"
    .参数 父窗口句柄, 整数型
    .参数 X坐标, 整数型
    .参数 Y坐标, 整数型
    .参数 宽度, 整数型
    .参数 高度, 整数型
    .参数 文本字节集指针, 整数型
    .参数 文本长度, 整数型
    .参数 前景色, 整数型
    .参数 背景色, 整数型
    .参数 字体名字节集指针, 整数型
    .参数 字体名长度, 整数型
    .参数 字体大小, 整数型
    .参数 粗体, 整数型
    .参数 斜体, 整数型
    .参数 下划线, 整数型
    .参数 对齐方式, 整数型
    .参数 多行, 整数型
    .参数 只读, 整数型
    .参数 密码模式, 整数型
    .参数 有边框, 整数型
    .参数 文本垂直居中, 整数型

.DLL命令 获取编辑框文本, 整数型, "emoji_window.dll", "GetEditBoxText"
    .参数 编辑框句柄, 整数型
    .参数 缓冲区指针, 整数型
    .参数 缓冲区大小, 整数型

.DLL命令 设置编辑框文本, , "emoji_window.dll", "SetEditBoxText"
    .参数 编辑框句柄, 整数型
    .参数 文本字节集指针, 整数型
    .参数 文本长度, 整数型

.DLL命令 设置编辑框字体, , "emoji_window.dll", "SetEditBoxFont"
    .参数 编辑框句柄, 整数型
    .参数 字体名字节集指针, 整数型
    .参数 字体名长度, 整数型
    .参数 字体大小, 整数型
    .参数 粗体, 整数型
    .参数 斜体, 整数型
    .参数 下划线, 整数型

.DLL命令 设置编辑框颜色, , "emoji_window.dll", "SetEditBoxColor"
    .参数 编辑框句柄, 整数型
    .参数 前景色, 整数型
    .参数 背景色, 整数型

.DLL命令 设置编辑框位置, , "emoji_window.dll", "SetEditBoxBounds"
    .参数 编辑框句柄, 整数型
    .参数 X坐标, 整数型
    .参数 Y坐标, 整数型
    .参数 宽度, 整数型
    .参数 高度, 整数型

.DLL命令 启用编辑框, , "emoji_window.dll", "EnableEditBox"
    .参数 编辑框句柄, 整数型
    .参数 启用, 整数型

.DLL命令 显示编辑框, , "emoji_window.dll", "ShowEditBox"
    .参数 编辑框句柄, 整数型
    .参数 显示, 整数型

' ========== 编辑框扩展属性命令（v2.1 新增）==========

.DLL命令 获取编辑框字体, 整数型, "emoji_window.dll", "GetEditBoxFont", , , 获取编辑框字体信息（返回字体名UTF-8字节数，缓冲区指针为0时返回所需大小）
    .参数 编辑框句柄, 整数型, , 编辑框窗口句柄
    .参数 字体名缓冲区指针, 整数型, , 接收字体名的缓冲区指针（传0获取所需大小）
    .参数 缓冲区大小, 整数型, , 缓冲区大小（字节）
    .参数 字体大小, 整数型, 传址, 输出：字体大小
    .参数 粗体, 整数型, 传址, 输出：是否粗体（1=是，0=否）
    .参数 斜体, 整数型, 传址, 输出：是否斜体（1=是，0=否）
    .参数 下划线, 整数型, 传址, 输出：是否下划线（1=是，0=否）

.DLL命令 获取编辑框颜色, 整数型, "emoji_window.dll", "GetEditBoxColor", , , 获取编辑框颜色（成功返回0，失败返回-1）
    .参数 编辑框句柄, 整数型, , 编辑框窗口句柄
    .参数 前景色, 整数型, 传址, 输出：前景色（ARGB格式）
    .参数 背景色, 整数型, 传址, 输出：背景色（ARGB格式）

.DLL命令 获取编辑框位置, 整数型, "emoji_window.dll", "GetEditBoxBounds", , , 获取编辑框位置和大小（成功返回0，失败返回-1）
    .参数 编辑框句柄, 整数型, , 编辑框窗口句柄
    .参数 X坐标, 整数型, 传址, 输出：X坐标（相对父窗口）
    .参数 Y坐标, 整数型, 传址, 输出：Y坐标（相对父窗口）
    .参数 宽度, 整数型, 传址, 输出：宽度
    .参数 高度, 整数型, 传址, 输出：高度

.DLL命令 获取编辑框对齐方式, 整数型, "emoji_window.dll", "GetEditBoxAlignment", , , 获取编辑框对齐方式（0=左，1=中，2=右，-1=错误）
    .参数 编辑框句柄, 整数型, , 编辑框窗口句柄

.DLL命令 获取编辑框启用状态, 整数型, "emoji_window.dll", "GetEditBoxEnabled", , , 获取编辑框启用状态（1=启用，0=禁用，-1=错误）
    .参数 编辑框句柄, 整数型, , 编辑框窗口句柄

.DLL命令 获取编辑框可视状态, 整数型, "emoji_window.dll", "GetEditBoxVisible", , , 获取编辑框可视状态（1=可见，0=不可见，-1=错误）
    .参数 编辑框句柄, 整数型, , 编辑框窗口句柄
```

## 易语言使用示例

### 基础示例

```
.版本 2

.程序集变量 主窗口句柄, 整数型
.程序集变量 编辑框句柄, 整数型
.程序集变量 按钮_获取文本, 整数型

.子程序 _启动子程序, 整数型
.局部变量 utf8字节集, 字节集
.局部变量 字体名utf8, 字节集
.局部变量 文本utf8, 字节集
.局部变量 按钮文本字节集, 字节集

utf8字节集 ＝ 编码_Ansi到Utf8 ("编辑框示例")
主窗口句柄 ＝ 创建Emoji窗口_字节集_扩展 (取变量数据地址 (utf8字节集), 取字节集长度 (utf8字节集), 600, 400, 到RGB (70, 130, 180))

' 创建编辑框（微软雅黑 14号 粗体 左对齐）
文本utf8 ＝ 编码_Ansi到Utf8 ("请输入内容")
字体名utf8 ＝ 编码_Ansi到Utf8 ("微软雅黑")
编辑框句柄 ＝ 创建编辑框 (主窗口句柄, 20, 20, 400, 35, 取变量数据地址 (文本utf8), 取字节集长度 (文本utf8), 到ARGB (255, 0, 0, 0), 到ARGB (255, 255, 255, 255), 取变量数据地址 (字体名utf8), 取字节集长度 (字体名utf8), 14, 1, 0, 0, 0, 0, 0, 0, 1, 1)

按钮文本字节集 ＝ 编码_Ansi到Utf8 ("获取文本")
按钮_获取文本 ＝ 创建Emoji按钮_字节集 (主窗口句柄, 0, 0, 取变量数据地址 (按钮文本字节集), 取字节集长度 (按钮文本字节集), 20, 70, 120, 35, 到ARGB (255, 64, 158, 255))

设置按钮点击回调 (&按钮点击回调)
设置消息循环主窗口 (主窗口句柄)
运行消息循环 ()
返回 (0)


.子程序 按钮点击回调, , , stdcall
.参数 按钮ID, 整数型
.参数 _父窗口句柄, 整数型
.局部变量 文本长度, 整数型
.局部变量 文本缓冲区, 字节集
.局部变量 结果字节集, 字节集

.如果真 (按钮ID ＝ 按钮_获取文本)
    文本长度 ＝ 获取编辑框文本 (编辑框句柄, 0, 0)
    .如果 (文本长度 ＞ 0)
        文本缓冲区 ＝ 取空白字节集 (文本长度)
        获取编辑框文本 (编辑框句柄, 取变量数据地址 (文本缓冲区), 文本长度)
        结果字节集 ＝ 编码_Ansi到Utf8 ("当前文本：") ＋ 文本缓冲区
    .否则
        结果字节集 ＝ 编码_Ansi到Utf8 ("编辑框为空")
    .如果结束
.如果真结束
```

### 获取扩展属性示例

```
.子程序 获取编辑框属性示例
.局部变量 字体名长度, 整数型
.局部变量 字体名缓冲区, 字节集
.局部变量 字体大小, 整数型
.局部变量 粗体, 整数型
.局部变量 斜体, 整数型
.局部变量 下划线, 整数型
.局部变量 前景色, 整数型
.局部变量 背景色, 整数型
.局部变量 X, 整数型
.局部变量 Y, 整数型
.局部变量 宽度, 整数型
.局部变量 高度, 整数型
.局部变量 对齐方式, 整数型
.局部变量 结果字节集, 字节集

' 获取字体信息
字体名长度 ＝ 获取编辑框字体 (编辑框句柄, 0, 0, 字体大小, 粗体, 斜体, 下划线)
.如果 (字体名长度 ＞ 0)
    字体名缓冲区 ＝ 取空白字节集 (字体名长度)
    获取编辑框字体 (编辑框句柄, 取变量数据地址 (字体名缓冲区), 字体名长度, 字体大小, 粗体, 斜体, 下划线)
    结果字节集 ＝ 编码_Ansi到Utf8 ("字体：") ＋ 字体名缓冲区 ＋ 编码_Ansi到Utf8 (" ") ＋ 编码_Ansi到Utf8 (到文本 (字体大小)) ＋ 编码_Ansi到Utf8 ("px") ＋ { 10 }
.如果结束

' 获取颜色
.如果真 (获取编辑框颜色 (编辑框句柄, 前景色, 背景色) ＝ 0)
    结果字节集 ＝ 结果字节集 ＋ 编码_Ansi到Utf8 ("前景色：0x") ＋ 编码_Ansi到Utf8 (到文本 (前景色)) ＋ { 10 }
.如果真结束

' 获取位置大小
.如果真 (获取编辑框位置 (编辑框句柄, X, Y, 宽度, 高度) ＝ 0)
    结果字节集 ＝ 结果字节集 ＋ 编码_Ansi到Utf8 ("位置：(") ＋ 编码_Ansi到Utf8 (到文本 (X)) ＋ 编码_Ansi到Utf8 (", ") ＋ 编码_Ansi到Utf8 (到文本 (Y)) ＋ 编码_Ansi到Utf8 (")  大小：") ＋ 编码_Ansi到Utf8 (到文本 (宽度)) ＋ 编码_Ansi到Utf8 ("x") ＋ 编码_Ansi到Utf8 (到文本 (高度)) ＋ { 10 }
.如果真结束

' 获取对齐方式
对齐方式 ＝ 获取编辑框对齐方式 (编辑框句柄)
.判断开始 (对齐方式 ＝ 0)
    结果字节集 ＝ 结果字节集 ＋ 编码_Ansi到Utf8 ("对齐：左对齐")
.判断 (对齐方式 ＝ 1)
    结果字节集 ＝ 结果字节集 ＋ 编码_Ansi到Utf8 ("对齐：居中")
.判断 (对齐方式 ＝ 2)
    结果字节集 ＝ 结果字节集 ＋ 编码_Ansi到Utf8 ("对齐：右对齐")
.判断结束
```

## 注意事项

⚠️ **重要提示：**

1. **两次调用模式**：`GetEditBoxFont` 使用两次调用模式，第一次传 0 获取所需缓冲区大小，第二次传入缓冲区获取实际内容

2. **UTF-8 字节集显示**：DLL 返回的字体名缓冲区是 UTF-8 字节集，不能用 `到文本()` 转换，必须全程用字节集拼接后传给显示函数

3. **颜色格式**：颜色使用 ARGB 格式（32位整数），格式为 `0xAARRGGBB`

4. **对齐方式**：0=左对齐，1=居中，2=右对齐，-1=错误（无效句柄）

5. **句柄有效性**：所有 API 都会验证句柄是否有效，无效句柄返回 -1

6. **按钮 ID 保存**：创建按钮时必须将返回的 ID 保存到程序集变量，不能用硬编码数字判断

## 相关文档

- [进度条](progressbar.md)
- [标签控件](label.md)
- [按钮控件](button.md)
