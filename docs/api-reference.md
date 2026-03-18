# API 参考文档

[ 返回主文档](../README.md)

本文档按控件类型分类列出 emoji_window.dll 所有属性操作 API，包含完整的参数说明、返回值说明和注意事项。

---

## 目录

- [通用约定](#通用约定)
- [窗口 Window](#窗口-window)
- [按钮 Button](#按钮-button)
- [标签 Label](#标签-label)
- [编辑框 EditBox](#编辑框-editbox)
- [复选框 CheckBox](#复选框-checkbox)
- [单选按钮 RadioButton](#单选按钮-radiobutton)
- [分组框 GroupBox](#分组框-groupbox)
- [选项卡控件 TabControl](#选项卡控件-tabcontrol)
- [进度条 ProgressBar](#进度条-progressbar)
- [返回值速查表](#返回值速查表)

---

## 通用约定

### 调用约定

所有导出函数使用 __stdcall 调用约定。

### 文本编码

所有文本参数均使用 UTF-8 编码（unsigned char* + 长度）。

### 两次调用模式

返回文本内容的函数统一采用两次调用模式：

第一次：传 NULL/0 获取所需字节数
第二次：分配缓冲区后获取实际内容

### 颜色格式

颜色统一使用 ARGB 32 位整数格式：0xAARRGGBB

AA（24-31位）：Alpha透明度，0xFF=不透明
RR（16-23位）：红色分量
GG（8-15位）：绿色分量
BB（0-7位）：蓝色分量

### 错误返回值

-1：失败（无效句柄/ID、参数错误等）
0：成功（无返回数据的操作）
1：真/可见/启用
0：假/不可见/禁用

---
## 窗口 Window

详细文档：[README.md](../README.md)

### 创建窗口

| 函数名 | 说明 |
|--------|------|
| create_emoji_window(title, width, height) | 创建窗口（ANSI标题） |
| create_emoji_window_bytes(title_bytes, title_len, width, height) | 创建窗口（UTF-8标题） |
| create_emoji_window_bytes_ex(title_bytes, title_len, width, height, titlebar_color) | 创建窗口并指定标题栏颜色 |

### 窗口属性 API

#### GetWindowTitle  获取窗口标题

`c
int __stdcall GetWindowTitle(HWND hwnd, unsigned char* buffer, int buffer_size);
`

| 参数 | 类型 | 说明 |
|------|------|------|
| hwnd | HWND | 窗口句柄 |
| buffer | unsigned char* | 接收标题的缓冲区（传NULL获取所需大小） |
| buffer_size | int | 缓冲区大小（字节） |

返回值：成功返回UTF-8字节数，失败返回-1

---

#### GetWindowBounds  获取窗口位置和大小

`c
int __stdcall GetWindowBounds(HWND hwnd, int* x, int* y, int* width, int* height);
`

| 参数 | 类型 | 说明 |
|------|------|------|
| hwnd | HWND | 窗口句柄 |
| x | int* | 输出：X坐标（屏幕坐标） |
| y | int* | 输出：Y坐标（屏幕坐标） |
| width | int* | 输出：宽度 |
| height | int* | 输出：高度 |

返回值：成功返回0，失败返回-1

---

#### SetWindowBounds  设置窗口位置和大小

`c
int __stdcall SetWindowBounds(HWND hwnd, int x, int y, int width, int height);
`

| 参数 | 类型 | 说明 |
|------|------|------|
| hwnd | HWND | 窗口句柄 |
| x | int | 新X坐标 |
| y | int | 新Y坐标 |
| width | int | 新宽度 |
| height | int | 新高度 |

返回值：成功返回0，失败返回-1

---

#### GetWindowVisible  获取窗口可视状态

`c
int __stdcall GetWindowVisible(HWND hwnd);
`

返回值：1=可见，0=不可见，-1=失败

---

#### ShowEmojiWindow  显示或隐藏窗口

`c
int __stdcall ShowEmojiWindow(HWND hwnd, int visible);
`

| 参数 | 类型 | 说明 |
|------|------|------|
| hwnd | HWND | 窗口句柄 |
| visible | int | 1=显示，0=隐藏 |

返回值：成功返回0，失败返回-1

注意：隐藏窗口时，窗口及其所有子控件均不可见。

---

#### GetWindowTitlebarColor  获取标题栏颜色

`c
int __stdcall GetWindowTitlebarColor(HWND hwnd);
`

返回值：RGB格式颜色值（去掉Alpha通道），失败返回-1

---

## 按钮 Button

详细文档：[docs/controls/button.md](controls/button.md)

### 创建按钮

`c
int __stdcall create_emoji_button_bytes(
    HWND parent,
    const unsigned char* emoji_bytes, int emoji_len,
    const unsigned char* text_bytes, int text_len,
    int x, int y, int width, int height,
    unsigned int bg_color
);
`

返回值：按钮ID（动态分配），失败返回-1

注意：返回的按钮ID必须保存到程序集变量，不能用硬编码数字判断。

### 按钮属性 API

#### GetButtonText  获取按钮文本

`c
int __stdcall GetButtonText(int button_id, unsigned char* buffer, int buffer_size);
`

| 参数 | 类型 | 说明 |
|------|------|------|
| button_id | int | 按钮ID |
| buffer | unsigned char* | 接收文本的缓冲区（传NULL获取所需大小） |
| buffer_size | int | 缓冲区大小（字节） |

返回值：成功返回UTF-8字节数，失败返回-1

---

#### SetButtonText  设置按钮文本

`c
void __stdcall SetButtonText(int button_id, const unsigned char* text, int text_len);
`

| 参数 | 类型 | 说明 |
|------|------|------|
| button_id | int | 按钮ID |
| text | const unsigned char* | UTF-8文本字节集指针 |
| text_len | int | 文本字节集长度 |

修改后立即触发重绘。

---

#### GetButtonEmoji  获取按钮Emoji

`c
int __stdcall GetButtonEmoji(int button_id, unsigned char* buffer, int buffer_size);
`

| 参数 | 类型 | 说明 |
|------|------|------|
| button_id | int | 按钮ID |
| buffer | unsigned char* | 接收Emoji的缓冲区（传NULL获取所需大小） |
| buffer_size | int | 缓冲区大小（字节） |

返回值：成功返回UTF-8字节数，失败返回-1

---

#### SetButtonEmoji  设置按钮Emoji

`c
void __stdcall SetButtonEmoji(int button_id, const unsigned char* emoji, int emoji_len);
`

| 参数 | 类型 | 说明 |
|------|------|------|
| button_id | int | 按钮ID |
| emoji | const unsigned char* | UTF-8 Emoji字节集指针 |
| emoji_len | int | Emoji字节集长度 |

修改后立即触发重绘。

---

#### GetButtonBounds  获取按钮位置和大小

`c
int __stdcall GetButtonBounds(int button_id, int* x, int* y, int* width, int* height);
`

| 参数 | 类型 | 说明 |
|------|------|------|
| button_id | int | 按钮ID |
| x | int* | 输出：X坐标（相对父窗口） |
| y | int* | 输出：Y坐标（相对父窗口） |
| width | int* | 输出：宽度 |
| height | int* | 输出：高度 |

返回值：成功返回0，失败返回-1

---

#### SetButtonBounds  设置按钮位置和大小

`c
void __stdcall SetButtonBounds(int button_id, int x, int y, int width, int height);
`

| 参数 | 类型 | 说明 |
|------|------|------|
| button_id | int | 按钮ID |
| x | int | 新X坐标 |
| y | int | 新Y坐标 |
| width | int | 新宽度 |
| height | int | 新高度 |

修改后立即更新显示。

---

#### GetButtonBackgroundColor  获取按钮背景色

`c
unsigned int __stdcall GetButtonBackgroundColor(int button_id);
`

返回值：ARGB格式颜色值，失败返回0

---

#### SetButtonBackgroundColor  设置按钮背景色

`c
void __stdcall SetButtonBackgroundColor(int button_id, unsigned int color);
`

| 参数 | 类型 | 说明 |
|------|------|------|
| button_id | int | 按钮ID |
| color | unsigned int | ARGB格式颜色值 |

修改后立即触发重绘。

---

#### GetButtonVisible  获取按钮可视状态

`c
int __stdcall GetButtonVisible(int button_id);
`

返回值：1=可见，0=隐藏，-1=失败

---

#### ShowButton  显示或隐藏按钮

`c
void __stdcall ShowButton(int button_id, int visible);
`

| 参数 | 类型 | 说明 |
|------|------|------|
| button_id | int | 按钮ID |
| visible | int | 1=显示，0=隐藏 |

隐藏时按钮不响应任何交互。

---

#### GetButtonEnabled  获取按钮启用状态

`c
int __stdcall GetButtonEnabled(int button_id);
`

返回值：1=启用，0=禁用，-1=失败

---

#### EnableButton  启用或禁用按钮

`c
void __stdcall EnableButton(HWND parent_hwnd, int button_id, BOOL enable);
`

| 参数 | 类型 | 说明 |
|------|------|------|
| parent_hwnd | HWND | 按钮所在父窗口句柄 |
| button_id | int | 按钮ID |
| enable | BOOL | TRUE=启用，FALSE=禁用 |

禁用的按钮显示为灰色，不触发回调。

---

## 标签 Label

详细文档：[docs/controls/label.md](controls/label.md)

### 创建标签

`c
HWND __stdcall CreateLabel(
    HWND hParent,
    int x, int y, int width, int height,
    const unsigned char* text_bytes, int text_len,
    UINT32 fg_color, UINT32 bg_color,
    const unsigned char* font_name_bytes, int font_name_len,
    int font_size,
    int bold, int italic, int underline,
    int alignment,
    int word_wrap
);
`

返回值：标签控件句柄

### 标签属性 API

#### GetLabelText  获取标签文本

`c
int __stdcall GetLabelText(HWND hLabel, unsigned char* buffer, int buffer_size);
`

| 参数 | 类型 | 说明 |
|------|------|------|
| hLabel | HWND | 标签窗口句柄 |
| buffer | unsigned char* | 接收文本的缓冲区（传NULL获取所需大小） |
| buffer_size | int | 缓冲区大小（字节） |

返回值：成功返回UTF-8字节数，失败返回-1。文本为空时返回0。

---

#### SetLabelText  设置标签文本

`c
void __stdcall SetLabelText(HWND hLabel, const unsigned char* text_bytes, int text_len);
`

---

#### GetLabelFont  获取标签字体

`c
int __stdcall GetLabelFont(
    HWND hLabel,
    unsigned char* font_name_buffer, int buffer_size,
    int* font_size,
    int* bold, int* italic, int* underline
);
`

| 参数 | 类型 | 说明 |
|------|------|------|
| hLabel | HWND | 标签窗口句柄 |
| font_name_buffer | unsigned char* | 接收字体名的缓冲区（传NULL获取所需大小） |
| buffer_size | int | 缓冲区大小 |
| font_size | int* | 输出：字体大小 |
| bold | int* | 输出：是否粗体（1=是，0=否） |
| italic | int* | 输出：是否斜体（1=是，0=否） |
| underline | int* | 输出：是否下划线（1=是，0=否） |

返回值：成功返回字体名UTF-8字节数，失败返回-1

---

#### GetLabelColor  获取标签颜色

`c
int __stdcall GetLabelColor(HWND hLabel, UINT32* fg_color, UINT32* bg_color);
`

| 参数 | 类型 | 说明 |
|------|------|------|
| hLabel | HWND | 标签窗口句柄 |
| fg_color | UINT32* | 输出：前景色（ARGB格式） |
| bg_color | UINT32* | 输出：背景色（ARGB格式） |

返回值：成功返回0，失败返回-1

---

#### GetLabelBounds  获取标签位置和大小

`c
int __stdcall GetLabelBounds(HWND hLabel, int* x, int* y, int* width, int* height);
`

返回值：成功返回0，失败返回-1

---

#### GetLabelAlignment  获取标签对齐方式

`c
int __stdcall GetLabelAlignment(HWND hLabel);
`

返回值：0=左对齐，1=居中，2=右对齐，-1=错误

---

#### GetLabelEnabled  获取标签启用状态

`c
int __stdcall GetLabelEnabled(HWND hLabel);
`

返回值：1=启用，0=禁用，-1=失败

---

#### GetLabelVisible  获取标签可视状态

`c
int __stdcall GetLabelVisible(HWND hLabel);
`

返回值：1=可见，0=不可见，-1=失败

---

## 编辑框 EditBox

详细文档：[docs/controls/editbox.md](controls/editbox.md)

### 编辑框属性 API

#### GetEditBoxText  获取编辑框文本

`c
int __stdcall GetEditBoxText(HWND hEdit, unsigned char* buffer, int buffer_size);
`

返回值：成功返回UTF-8字节数，失败返回-1

---

#### GetEditBoxFont  获取编辑框字体

`c
int __stdcall GetEditBoxFont(
    HWND hEdit,
    unsigned char* font_name_buffer, int buffer_size,
    int* font_size,
    int* bold, int* italic, int* underline
);
`

| 参数 | 类型 | 说明 |
|------|------|------|
| hEdit | HWND | 编辑框句柄 |
| font_name_buffer | unsigned char* | 接收字体名的缓冲区（传NULL获取所需大小） |
| buffer_size | int | 缓冲区大小 |
| font_size | int* | 输出：字体大小 |
| bold | int* | 输出：是否粗体（1=是，0=否） |
| italic | int* | 输出：是否斜体（1=是，0=否） |
| underline | int* | 输出：是否下划线（1=是，0=否） |

返回值：成功返回字体名UTF-8字节数，失败返回-1

---

#### GetEditBoxColor  获取编辑框颜色

`c
int __stdcall GetEditBoxColor(HWND hEdit, UINT32* fg_color, UINT32* bg_color);
`

| 参数 | 类型 | 说明 |
|------|------|------|
| hEdit | HWND | 编辑框句柄 |
| fg_color | UINT32* | 输出：前景色（ARGB格式） |
| bg_color | UINT32* | 输出：背景色（ARGB格式） |

返回值：成功返回0，失败返回-1

---

#### GetEditBoxBounds  获取编辑框位置和大小

`c
int __stdcall GetEditBoxBounds(HWND hEdit, int* x, int* y, int* width, int* height);
`

返回值：成功返回0，失败返回-1

---

#### GetEditBoxAlignment  获取编辑框对齐方式

`c
int __stdcall GetEditBoxAlignment(HWND hEdit);
`

返回值：0=左对齐，1=居中，2=右对齐，-1=错误

---

#### GetEditBoxEnabled  获取编辑框启用状态

`c
int __stdcall GetEditBoxEnabled(HWND hEdit);
`

返回值：1=启用，0=禁用，-1=失败

---

#### GetEditBoxVisible  获取编辑框可视状态

`c
int __stdcall GetEditBoxVisible(HWND hEdit);
`

返回值：1=可见，0=不可见，-1=失败

---

## 复选框 CheckBox

详细文档：[docs/controls/checkbox.md](controls/checkbox.md)

### 复选框属性 API

#### GetCheckBoxText  获取复选框文本

`c
int __stdcall GetCheckBoxText(HWND hCheckBox, unsigned char* buffer, int buffer_size);
`

| 参数 | 类型 | 说明 |
|------|------|------|
| hCheckBox | HWND | 复选框窗口句柄 |
| buffer | unsigned char* | 接收文本的缓冲区（传0获取所需大小） |
| buffer_size | int | 缓冲区大小（字节） |

返回值：成功返回UTF-8字节数，失败返回-1

---

#### SetCheckBoxFont  设置复选框字体

`c
void __stdcall SetCheckBoxFont(
    HWND hCheckBox,
    const unsigned char* font_name, int font_name_len,
    int font_size,
    int bold, int italic, int underline
);
`

| 参数 | 类型 | 说明 |
|------|------|------|
| hCheckBox | HWND | 复选框窗口句柄 |
| font_name | const unsigned char* | UTF-8字体名字节集指针 |
| font_name_len | int | 字体名字节集长度 |
| font_size | int | 字体大小（0=不修改） |
| bold | int | 1=粗体，0=非粗体 |
| italic | int | 1=斜体，0=非斜体 |
| underline | int | 1=下划线，0=无下划线 |

修改后立即触发重绘。

---

#### GetCheckBoxFont  获取复选框字体

`c
int __stdcall GetCheckBoxFont(
    HWND hCheckBox,
    unsigned char* font_name_buffer, int buffer_size,
    int* font_size,
    int* bold, int* italic, int* underline
);
`

返回值：成功返回字体名UTF-8字节数，失败返回-1

---

#### SetCheckBoxColor  设置复选框颜色

`c
void __stdcall SetCheckBoxColor(HWND hCheckBox, UINT32 fg_color, UINT32 bg_color);
`

| 参数 | 类型 | 说明 |
|------|------|------|
| hCheckBox | HWND | 复选框窗口句柄 |
| fg_color | UINT32 | 前景色（ARGB格式） |
| bg_color | UINT32 | 背景色（ARGB格式） |

修改后立即触发重绘。

---

#### GetCheckBoxColor  获取复选框颜色

`c
int __stdcall GetCheckBoxColor(HWND hCheckBox, UINT32* fg_color, UINT32* bg_color);
`

返回值：成功返回0，失败返回-1

---

## 单选按钮 RadioButton

详细文档：[docs/controls/radiobutton.md](controls/radiobutton.md)

### 单选按钮属性 API

#### GetRadioButtonText  获取单选按钮文本

`c
int __stdcall GetRadioButtonText(HWND hRadioButton, unsigned char* buffer, int buffer_size);
`

返回值：成功返回UTF-8字节数，失败返回-1

---

#### SetRadioButtonFont  设置单选按钮字体

`c
void __stdcall SetRadioButtonFont(
    HWND hRadioButton,
    const unsigned char* font_name, int font_name_len,
    int font_size,
    int bold, int italic, int underline
);
`

参数说明同 SetCheckBoxFont，修改后立即触发重绘。

---

#### GetRadioButtonFont  获取单选按钮字体

`c
int __stdcall GetRadioButtonFont(
    HWND hRadioButton,
    unsigned char* font_name_buffer, int buffer_size,
    int* font_size,
    int* bold, int* italic, int* underline
);
`

返回值：成功返回字体名UTF-8字节数，失败返回-1

---

#### SetRadioButtonColor  设置单选按钮颜色

`c
void __stdcall SetRadioButtonColor(HWND hRadioButton, UINT32 fg_color, UINT32 bg_color);
`

修改后立即触发重绘。

---

#### GetRadioButtonColor  获取单选按钮颜色

`c
int __stdcall GetRadioButtonColor(HWND hRadioButton, UINT32* fg_color, UINT32* bg_color);
`

返回值：成功返回0，失败返回-1

---

## 分组框 GroupBox

详细文档：[docs/controls/groupbox.md](controls/groupbox.md)

### 分组框属性 API

#### GetGroupBoxTitle  获取分组框标题

`c
int __stdcall GetGroupBoxTitle(HWND hGroupBox, unsigned char* buffer, int buffer_size);
`

| 参数 | 类型 | 说明 |
|------|------|------|
| hGroupBox | HWND | 分组框句柄 |
| buffer | unsigned char* | 接收标题的缓冲区（传NULL获取所需大小） |
| buffer_size | int | 缓冲区大小（字节） |

返回值：成功返回UTF-8字节数，失败返回-1

---

#### GetGroupBoxBounds  获取分组框位置和大小

`c
int __stdcall GetGroupBoxBounds(HWND hGroupBox, int* x, int* y, int* width, int* height);
`

| 参数 | 类型 | 说明 |
|------|------|------|
| hGroupBox | HWND | 分组框句柄 |
| x | int* | 输出：X坐标（相对父窗口） |
| y | int* | 输出：Y坐标（相对父窗口） |
| width | int* | 输出：宽度 |
| height | int* | 输出：高度 |

返回值：成功返回0，失败返回-1

---

#### GetGroupBoxVisible  获取分组框可视状态

`c
int __stdcall GetGroupBoxVisible(HWND hGroupBox);
`

返回值：1=可见，0=不可见，-1=失败

---

#### GetGroupBoxEnabled  获取分组框启用状态

`c
int __stdcall GetGroupBoxEnabled(HWND hGroupBox);
`

返回值：1=启用，0=禁用，-1=失败

---

#### GetGroupBoxColor  获取分组框颜色

`c
int __stdcall GetGroupBoxColor(HWND hGroupBox, UINT32* border_color, UINT32* bg_color);
`

| 参数 | 类型 | 说明 |
|------|------|------|
| hGroupBox | HWND | 分组框句柄 |
| border_color | UINT32* | 输出：边框颜色（ARGB格式） |
| bg_color | UINT32* | 输出：背景色（ARGB格式） |

返回值：成功返回0，失败返回-1

---

## 选项卡控件 TabControl

详细文档：[docs/controls/tabcontrol.md](controls/tabcontrol.md)

### TabControl 属性 API

#### GetTabTitle  获取Tab页标题

`c
int __stdcall GetTabTitle(HWND hTabControl, int index, unsigned char* buffer, int bufferSize);
`

| 参数 | 类型 | 说明 |
|------|------|------|
| hTabControl | HWND | TabControl窗口句柄 |
| index | int | Tab页索引（从0开始） |
| buffer | unsigned char* | 接收标题的缓冲区（传NULL获取所需大小） |
| bufferSize | int | 缓冲区大小（字节） |

返回值：成功返回UTF-8字节数，失败返回-1

---

#### SetTabTitle  设置Tab页标题

`c
int __stdcall SetTabTitle(HWND hTabControl, int index, const unsigned char* title_bytes, int title_len);
`

| 参数 | 类型 | 说明 |
|------|------|------|
| hTabControl | HWND | TabControl窗口句柄 |
| index | int | Tab页索引（从0开始） |
| title_bytes | const unsigned char* | UTF-8编码的新标题字节集指针 |
| title_len | int | 标题字节集长度 |

返回值：成功返回0，失败返回-1。修改后立即重绘。

---

#### GetTabControlBounds  获取TabControl位置和大小

`c
int __stdcall GetTabControlBounds(HWND hTabControl, int* x, int* y, int* width, int* height);
`

返回值：成功返回0，失败返回-1

---

#### SetTabControlBounds  设置TabControl位置和大小

`c
int __stdcall SetTabControlBounds(HWND hTabControl, int x, int y, int width, int height);
`

返回值：成功返回0，失败返回-1

---

#### GetTabControlVisible  获取TabControl可视状态

`c
int __stdcall GetTabControlVisible(HWND hTabControl);
`

返回值：1=可见，0=不可见，-1=失败

---

#### ShowTabControl  显示或隐藏TabControl

`c
int __stdcall ShowTabControl(HWND hTabControl, int visible);
`

| 参数 | 类型 | 说明 |
|------|------|------|
| hTabControl | HWND | TabControl窗口句柄 |
| visible | int | 1=显示，0=隐藏 |

返回值：成功返回0，失败返回-1

注意：隐藏TabControl时，当前激活的内容窗口也会同步隐藏。

---

#### EnableTabControl  启用或禁用TabControl

`c
int __stdcall EnableTabControl(HWND hTabControl, int enabled);
`

| 参数 | 类型 | 说明 |
|------|------|------|
| hTabControl | HWND | TabControl窗口句柄 |
| enabled | int | 1=启用，0=禁用 |

返回值：成功返回0，失败返回-1

注意：禁用后所有Tab页不可切换，但TabControl仍然可见。

---

## 进度条 ProgressBar

详细文档：[docs/controls/progressbar.md](controls/progressbar.md)

### 进度条属性 API

#### GetProgressBarColor — 获取进度条颜色

```c
int __stdcall GetProgressBarColor(HWND hProgressBar, UINT32* fg_color, UINT32* bg_color);
```

| 参数 | 类型 | 说明 |
|------|------|------|
| hProgressBar | HWND | 进度条句柄 |
| fg_color | UINT32* | 输出：前景色/进度色（ARGB格式） |
| bg_color | UINT32* | 输出：背景色（ARGB格式） |

返回值：成功返回0，失败返回-1

---

#### GetProgressBarBounds — 获取进度条位置和大小

```c
int __stdcall GetProgressBarBounds(HWND hProgressBar, int* x, int* y, int* width, int* height);
```

返回值：成功返回0，失败返回-1

---

#### GetProgressBarEnabled — 获取进度条启用状态

```c
int __stdcall GetProgressBarEnabled(HWND hProgressBar);
```

返回值：1=启用，0=禁用，-1=失败

---

#### GetProgressBarVisible — 获取进度条可视状态

```c
int __stdcall GetProgressBarVisible(HWND hProgressBar);
```

返回值：1=可见，0=不可见，-1=失败

---

#### GetProgressBarShowText — 获取进度条是否显示百分比文本

```c
int __stdcall GetProgressBarShowText(HWND hProgressBar);
```

返回值：1=显示文本，0=不显示，-1=失败

---

## 返回值速查表

| 返回值 | 适用场景 | 含义 |
|--------|----------|------|
| 正整数 | GetXxxText、GetXxxFont | UTF-8字节数（成功） |
| `0` | GetXxxText | 文本为空（成功） |
| `0` | SetXxx、GetXxxBounds、GetXxxColor | 操作成功 |
| `1` | GetXxxVisible、GetXxxEnabled | 可见/启用 |
| `0` | GetXxxVisible、GetXxxEnabled | 不可见/禁用 |
| `-1` | 所有API | 失败（无效句柄/ID、参数错误） |

---

## 注意事项

### 1. 两次调用模式

所有返回文本的 Get 函数（GetButtonText、GetLabelText、GetTabTitle 等）均使用两次调用模式：

```
' 易语言示例
长度 = 获取按钮文本 (按钮ID, 0, 0)
.如果真 (长度 > 0)
    缓冲区 = 取空白字节集 (长度)
    获取按钮文本 (按钮ID, 取变量数据地址 (缓冲区), 长度)
    ' 缓冲区 即为 UTF-8 字节集
.如果真结束
```

### 2. UTF-8 字节集不能用到文本()转换

DLL 返回的文本缓冲区是 UTF-8 字节集，直接用 `到文本()` 会导致中文和 Emoji 乱码。
必须全程用字节集拼接，最后用 `取变量数据地址` 传给显示函数。

### 3. 按钮ID必须保存到程序集变量

`create_emoji_button_bytes` 返回的按钮ID是动态分配的，不保证从1开始。
必须将返回值保存到程序集变量，不能在回调中用硬编码数字判断。

### 4. 颜色格式

所有颜色参数使用 ARGB 格式（32位整数）：`0xAARRGGBB`
- 完全不透明蓝色：`0xFF409EFF`
- 完全不透明绿色：`0xFF67C23A`

### 5. 属性修改立即生效

所有 Set 函数修改属性后会立即触发控件重绘，无需手动调用刷新函数。

### 6. 线程安全

所有 API 应在主 UI 线程调用，不支持多线程并发访问。

### 7. 句柄/ID有效性

所有 API 都会验证句柄或按钮ID是否有效，无效时返回 -1 或不执行操作。
控件销毁后，其句柄/ID不再有效。

---

## 相关文档

- [按钮控件](controls/button.md)
- [标签控件](controls/label.md)
- [编辑框控件](controls/editbox.md)
- [复选框控件](controls/checkbox.md)
- [单选按钮控件](controls/radiobutton.md)
- [分组框控件](controls/groupbox.md)
- [选项卡控件](controls/tabcontrol.md)
- [进度条控件](controls/progressbar.md)
- [主题文档](theme.md)
- [布局文档](layout.md)
- [常见问题](faq.md)
