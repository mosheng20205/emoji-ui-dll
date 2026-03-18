# 复选框控件 (CheckBox)

[← 返回主文档](../../README.md)

## 概述

Element UI 风格的复选框控件，支持选中/未选中状态切换。

## 创建复选框

```c++
HWND __stdcall CreateCheckBox(
    HWND hParent,
    int x, int y, int width, int height,
    const unsigned char* text_bytes, int text_len,
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
| `checked` | 初始选中状态（TRUE=选中，FALSE=未选中） |
| `fg_color` | 前景色（ARGB格式，如 0xFF303133） |
| `bg_color` | 背景色（ARGB格式，如 0xFFFFFFFF） |
| `font_name_bytes` | UTF-8 编码的字体名字节集指针（传NULL使用默认字体） |
| `font_name_len` | 字体名字节集长度 |
| `font_size` | 字体大小，像素（传0使用默认14px） |
| `bold` | 是否粗体（TRUE=粗体） |
| `italic` | 是否斜体（TRUE=斜体） |
| `underline` | 是否下划线（TRUE=下划线） |

**返回值：** 复选框控件句柄

## 状态操作

### 获取选中状态

```c++
BOOL __stdcall GetCheckBoxState(HWND hCheckBox);
```

**返回值：** TRUE=选中，FALSE=未选中

### 设置选中状态

```c++
void __stdcall SetCheckBoxState(HWND hCheckBox, BOOL checked);
```

### 设置回调

```c++
typedef void (__stdcall *CheckBoxCallback)(HWND hCheckBox, BOOL checked);
void __stdcall SetCheckBoxCallback(HWND hCheckBox, CheckBoxCallback callback);
```

**回调参数：**

| 参数 | 说明 |
|------|------|
| `hCheckBox` | 触发回调的复选框句柄 |
| `checked` | 当前选中状态 |

## 基础属性操作

### 启用/禁用

```c++
void __stdcall EnableCheckBox(HWND hCheckBox, BOOL enable);
```

### 显示/隐藏

```c++
void __stdcall ShowCheckBox(HWND hCheckBox, BOOL show);
```

### 设置文本

```c++
void __stdcall SetCheckBoxText(HWND hCheckBox, const unsigned char* text_bytes, int text_len);
```

### 设置位置和大小

```c++
void __stdcall SetCheckBoxBounds(HWND hCheckBox, int x, int y, int width, int height);
```

## 扩展属性操作

### 获取复选框文本

获取复选框的当前文本内容（UTF-8格式）。

```c++
int __stdcall GetCheckBoxText(HWND hCheckBox, unsigned char* buffer, int buffer_size);
```

**参数说明：**

| 参数 | 说明 |
|------|------|
| `hCheckBox` | 复选框窗口句柄 |
| `buffer` | 接收文本的缓冲区指针（传0获取所需大小） |
| `buffer_size` | 缓冲区大小（字节） |

**返回值：** 成功返回实际字节数，失败返回-1

**使用模式（两次调用）：**
1. 第一次传0获取所需缓冲区大小
2. 第二次传入缓冲区获取实际内容

### 设置复选框字体

设置复选框的字体并立即重绘。

```c++
void __stdcall SetCheckBoxFont(
    HWND hCheckBox,
    const unsigned char* font_name, int font_name_len,
    int font_size,
    int bold, int italic, int underline
);
```

**参数说明：**

| 参数 | 说明 |
|------|------|
| `hCheckBox` | 复选框窗口句柄 |
| `font_name` | UTF-8字体名字节集指针 |
| `font_name_len` | 字体名字节集长度 |
| `font_size` | 字体大小（0=不修改） |
| `bold` | 1=粗体，0=非粗体 |
| `italic` | 1=斜体，0=非斜体 |
| `underline` | 1=下划线，0=无下划线 |

### 获取复选框字体

获取复选框的字体信息。

```c++
int __stdcall GetCheckBoxFont(
    HWND hCheckBox,
    unsigned char* font_name_buffer, int buffer_size,
    int* font_size,
    int* bold, int* italic, int* underline
);
```

**参数说明：**

| 参数 | 说明 |
|------|------|
| `hCheckBox` | 复选框窗口句柄 |
| `font_name_buffer` | 接收字体名的缓冲区指针（传0获取所需大小） |
| `buffer_size` | 缓冲区大小 |
| `font_size` | 输出参数：字体大小 |
| `bold` | 输出参数：是否粗体（1=是，0=否） |
| `italic` | 输出参数：是否斜体（1=是，0=否） |
| `underline` | 输出参数：是否下划线（1=是，0=否） |

**返回值：** 成功返回字体名UTF-8字节数，失败返回-1

### 设置复选框颜色

设置复选框的前景色和背景色并立即重绘。

```c++
void __stdcall SetCheckBoxColor(HWND hCheckBox, UINT32 fg_color, UINT32 bg_color);
```

**参数说明：**

| 参数 | 说明 |
|------|------|
| `hCheckBox` | 复选框窗口句柄 |
| `fg_color` | 前景色（ARGB格式） |
| `bg_color` | 背景色（ARGB格式） |

### 获取复选框颜色

获取复选框的前景色和背景色。

```c++
int __stdcall GetCheckBoxColor(HWND hCheckBox, UINT32* fg_color, UINT32* bg_color);
```

**参数说明：**

| 参数 | 说明 |
|------|------|
| `hCheckBox` | 复选框窗口句柄 |
| `fg_color` | 输出参数：前景色ARGB |
| `bg_color` | 输出参数：背景色ARGB |

**返回值：** 成功返回0，失败返回-1

## 样式说明

复选框采用 Element UI 设计规范：

- 复选框尺寸：14x14 像素
- 圆角半径：2px
- 选中颜色：#409EFF（Element UI 主色）
- 边框颜色：#DCDFE6（默认）/ #409EFF（悬停/选中）
- 禁用颜色：#C0C4CC
- 文本字体：Microsoft YaHei UI，14px
- 文本间距：复选框右侧 8px

## 易语言声明

```
.DLL命令 创建复选框, 整数型, "emoji_window.dll", "CreateCheckBox"
    .参数 父窗口句柄, 整数型
    .参数 X坐标, 整数型
    .参数 Y坐标, 整数型
    .参数 宽度, 整数型
    .参数 高度, 整数型
    .参数 文本字节集指针, 整数型
    .参数 文本长度, 整数型
    .参数 初始选中, 逻辑型
    .参数 前景色, 整数型
    .参数 背景色, 整数型
    .参数 字体名字节集指针, 整数型, , UTF-8字体名字节集指针
    .参数 字体名长度, 整数型, , 字体名字节集长度
    .参数 字体大小, 整数型, , 字体大小（像素）
    .参数 粗体, 逻辑型
    .参数 斜体, 逻辑型
    .参数 下划线, 逻辑型

.DLL命令 获取复选框状态, 逻辑型, "emoji_window.dll", "GetCheckBoxState"
    .参数 复选框句柄, 整数型

.DLL命令 设置复选框状态, , "emoji_window.dll", "SetCheckBoxState"
    .参数 复选框句柄, 整数型
    .参数 选中状态, 逻辑型

.DLL命令 设置复选框回调, , "emoji_window.dll", "SetCheckBoxCallback"
    .参数 复选框句柄, 整数型
    .参数 回调函数, 子程序指针

.DLL命令 启用复选框, , "emoji_window.dll", "EnableCheckBox"
    .参数 复选框句柄, 整数型
    .参数 启用, 逻辑型

.DLL命令 显示复选框, , "emoji_window.dll", "ShowCheckBox"
    .参数 复选框句柄, 整数型
    .参数 显示, 逻辑型

.DLL命令 设置复选框文本, , "emoji_window.dll", "SetCheckBoxText"
    .参数 复选框句柄, 整数型
    .参数 文本字节集指针, 整数型
    .参数 文本长度, 整数型

.DLL命令 设置复选框位置, , "emoji_window.dll", "SetCheckBoxBounds"
    .参数 复选框句柄, 整数型
    .参数 X坐标, 整数型
    .参数 Y坐标, 整数型
    .参数 宽度, 整数型
    .参数 高度, 整数型

' ========== 复选框扩展属性命令 ==========

.DLL命令 获取复选框文本, 整数型, "emoji_window.dll", "GetCheckBoxText"
    .参数 复选框句柄, 整数型
    .参数 缓冲区指针, 整数型
    .参数 缓冲区大小, 整数型

.DLL命令 设置复选框字体, , "emoji_window.dll", "SetCheckBoxFont"
    .参数 复选框句柄, 整数型
    .参数 字体名字节集指针, 整数型
    .参数 字体名长度, 整数型
    .参数 字体大小, 整数型
    .参数 粗体, 整数型
    .参数 斜体, 整数型
    .参数 下划线, 整数型

.DLL命令 获取复选框字体, 整数型, "emoji_window.dll", "GetCheckBoxFont"
    .参数 复选框句柄, 整数型
    .参数 字体名缓冲区指针, 整数型
    .参数 字体名缓冲区大小, 整数型
    .参数 字体大小, 整数型, 传址
    .参数 粗体, 整数型, 传址
    .参数 斜体, 整数型, 传址
    .参数 下划线, 整数型, 传址

.DLL命令 设置复选框颜色, , "emoji_window.dll", "SetCheckBoxColor"
    .参数 复选框句柄, 整数型
    .参数 前景色, 整数型
    .参数 背景色, 整数型

.DLL命令 获取复选框颜色, 整数型, "emoji_window.dll", "GetCheckBoxColor"
    .参数 复选框句柄, 整数型
    .参数 前景色, 整数型, 传址
    .参数 背景色, 整数型, 传址
```

## 易语言使用示例

### 基础示例

```
.版本 2

.程序集变量 窗口句柄, 整数型
.程序集变量 复选框1, 整数型
.程序集变量 复选框2, 整数型
.程序集变量 复选框3, 整数型

.子程序 _启动窗口_创建完毕
.局部变量 文本字节集, 字节集

窗口句柄 ＝ 创建Emoji窗口 ("复选框示例", 500, 400)

' 创建复选框
文本字节集 ＝ 编码_Ansi到Utf8 ("接受用户协议")
复选框1 ＝ 创建复选框 (窗口句柄, 50, 50, 200, 30, 取变量数据地址 (文本字节集), 取字节集长度 (文本字节集), 假, 到ARGB (255, 48, 49, 51), 到ARGB (255, 255, 255, 255), 0, 0, 0, 假, 假, 假)

文本字节集 ＝ 编码_Ansi到Utf8 ("订阅邮件通知")
复选框2 ＝ 创建复选框 (窗口句柄, 50, 90, 200, 30, 取变量数据地址 (文本字节集), 取字节集长度 (文本字节集), 真, 到ARGB (255, 48, 49, 51), 到ARGB (255, 255, 255, 255), 0, 0, 0, 假, 假, 假)

文本字节集 ＝ 编码_Ansi到Utf8 ("记住登录状态")
复选框3 ＝ 创建复选框 (窗口句柄, 50, 130, 200, 30, 取变量数据地址 (文本字节集), 取字节集长度 (文本字节集), 假, 到ARGB (255, 48, 49, 51), 到ARGB (255, 255, 255, 255), 0, 0, 0, 假, 假, 假)

设置复选框回调 (复选框1, &复选框回调)
设置复选框回调 (复选框2, &复选框回调)
设置复选框回调 (复选框3, &复选框回调)

运行消息循环 ()


.子程序 复选框回调, , 公开, stdcall
.参数 复选框句柄, 整数型
.参数 选中状态, 逻辑型
.局部变量 结果字节集, 字节集

.如果 (选中状态)
    结果字节集 ＝ 编码_Ansi到Utf8 ("已选中")
.否则
    结果字节集 ＝ 编码_Ansi到Utf8 ("已取消")
.如果结束
```

### 获取复选框文本示例

```
.子程序 获取文本示例
.局部变量 文本长度, 整数型
.局部变量 文本缓冲区, 字节集
.局部变量 结果字节集, 字节集

' 第一次调用获取所需长度
文本长度 ＝ 获取复选框文本 (复选框1, 0, 0)
.如果 (文本长度 ＞ 0)
    ' 第二次调用获取实际内容
    文本缓冲区 ＝ 取空白字节集 (文本长度)
    获取复选框文本 (复选框1, 取变量数据地址 (文本缓冲区), 文本长度)
    
    ' 拼接结果（全程UTF-8字节集）
    结果字节集 ＝ 编码_Ansi到Utf8 ("复选框文本：") ＋ 文本缓冲区
    设置标签文本 (标签_结果, 取变量数据地址 (结果字节集), 取字节集长度 (结果字节集))
.如果结束
```

### 修改字体和颜色示例

```
.子程序 修改样式示例
.局部变量 字体名字节集, 字节集
.局部变量 字体大小, 整数型
.局部变量 粗体, 整数型
.局部变量 斜体, 整数型
.局部变量 下划线, 整数型
.局部变量 前景色, 整数型
.局部变量 背景色, 整数型

' 设置字体为宋体16号粗体
字体名字节集 ＝ 编码_Ansi到Utf8 ("宋体")
设置复选框字体 (复选框1, 取变量数据地址 (字体名字节集), 取字节集长度 (字体名字节集), 16, 1, 0, 0)

' 获取当前字体信息
字体大小 ＝ 0
粗体 ＝ 0
斜体 ＝ 0
下划线 ＝ 0
获取复选框字体 (复选框1, 0, 0, 字体大小, 粗体, 斜体, 下划线)

' 设置颜色（蓝色文字，白色背景）
设置复选框颜色 (复选框1, 到ARGB (255, 64, 158, 255), 到ARGB (255, 255, 255, 255))

' 获取当前颜色
获取复选框颜色 (复选框1, 前景色, 背景色)
```

### 动态控制示例

```
.程序集变量 窗口句柄, 整数型
.程序集变量 复选框_全选, 整数型
.程序集变量 复选框_选项1, 整数型
.程序集变量 复选框_选项2, 整数型
.程序集变量 复选框_选项3, 整数型
.程序集变量 按钮_提交, 整数型

.子程序 _启动窗口_创建完毕
.局部变量 文本字节集, 字节集

窗口句柄 ＝ 创建Emoji窗口 ("多选示例", 400, 350)

文本字节集 ＝ 编码_Ansi到Utf8 ("全选")
复选框_全选 ＝ 创建复选框 (窗口句柄, 30, 30, 150, 30, 取变量数据地址 (文本字节集), 取字节集长度 (文本字节集), 假, 到ARGB (255, 48, 49, 51), 到ARGB (255, 255, 255, 255), 0, 0, 0, 假, 假, 假)

文本字节集 ＝ 编码_Ansi到Utf8 ("选项一")
复选框_选项1 ＝ 创建复选框 (窗口句柄, 50, 70, 150, 30, 取变量数据地址 (文本字节集), 取字节集长度 (文本字节集), 假, 到ARGB (255, 48, 49, 51), 到ARGB (255, 255, 255, 255), 0, 0, 0, 假, 假, 假)

文本字节集 ＝ 编码_Ansi到Utf8 ("选项二")
复选框_选项2 ＝ 创建复选框 (窗口句柄, 50, 110, 150, 30, 取变量数据地址 (文本字节集), 取字节集长度 (文本字节集), 假, 到ARGB (255, 48, 49, 51), 到ARGB (255, 255, 255, 255), 0, 0, 0, 假, 假, 假)

文本字节集 ＝ 编码_Ansi到Utf8 ("选项三")
复选框_选项3 ＝ 创建复选框 (窗口句柄, 50, 150, 150, 30, 取变量数据地址 (文本字节集), 取字节集长度 (文本字节集), 假, 到ARGB (255, 48, 49, 51), 到ARGB (255, 255, 255, 255), 0, 0, 0, 假, 假, 假)

设置复选框回调 (复选框_全选, &全选回调)

文本字节集 ＝ 编码_Ansi到Utf8 ("提交")
按钮_提交 ＝ 创建Emoji按钮_字节集 (窗口句柄, 0, 0, 取变量数据地址 (文本字节集), 取字节集长度 (文本字节集), 30, 200, 100, 35, 到ARGB (255, 64, 158, 255))

设置按钮点击回调 (&按钮回调)
运行消息循环 ()


.子程序 全选回调, , 公开, stdcall
.参数 复选框句柄, 整数型
.参数 选中状态, 逻辑型

' 全选/全不选
设置复选框状态 (复选框_选项1, 选中状态)
设置复选框状态 (复选框_选项2, 选中状态)
设置复选框状态 (复选框_选项3, 选中状态)


.子程序 按钮回调, , 公开, stdcall
.参数 按钮ID, 整数型
.参数 父窗口句柄, 整数型
.局部变量 结果字节集, 字节集

.如果真 (按钮ID ＝ 按钮_提交)
    结果字节集 ＝ 编码_Ansi到Utf8 ("已选中：")
    .如果真 (获取复选框状态 (复选框_选项1))
        结果字节集 ＝ 结果字节集 ＋ 编码_Ansi到Utf8 ("选项一 ")
    .如果真结束
    .如果真 (获取复选框状态 (复选框_选项2))
        结果字节集 ＝ 结果字节集 ＋ 编码_Ansi到Utf8 ("选项二 ")
    .如果真结束
    .如果真 (获取复选框状态 (复选框_选项3))
        结果字节集 ＝ 结果字节集 ＋ 编码_Ansi到Utf8 ("选项三")
    .如果真结束
    信息框 (到文本 (结果字节集), 0, "提交结果")
.如果真结束
```

## 注意事项

⚠️ **重要提示：**

1. **文本编码**：所有文本参数必须使用UTF-8编码，使用 `编码_Ansi到Utf8()` 转换中文文本

2. **两次调用模式**：`GetCheckBoxText` 和 `GetCheckBoxFont` 使用两次调用模式
   - 第一次传0获取所需缓冲区大小
   - 第二次传入缓冲区获取实际内容

3. **颜色格式**：颜色使用ARGB格式（32位整数），格式为 `0xAARRGGBB`

4. **字体大小为0**：`SetCheckBoxFont` 中字体大小传0表示不修改当前字体大小

5. **属性修改立即生效**：`SetCheckBoxFont` 和 `SetCheckBoxColor` 修改后立即触发重绘

6. **句柄有效性**：所有API都会验证句柄是否有效，无效句柄返回-1或不执行操作

7. **UTF-8字节集显示**：DLL返回的文本缓冲区是UTF-8字节集，不能用 `到文本()` 转换，必须全程用字节集拼接后传给显示函数

## 相关文档

- [单选按钮](radiobutton.md)
- [按钮控件](button.md)
- [分组框](groupbox.md)
