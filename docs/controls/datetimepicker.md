# D2D 日期时间选择器（时间日期组件）

[← 返回主文档](../../README.md) | [← 返回文档中心](../README.md)

## 概述

**D2D 日期时间选择器**在单行编辑区域展示当前日期/时间，右侧为下拉按钮；点击后弹出 **Element 风格** 的日历与（按精度）时间区，使用 **Direct2D + DirectWrite** 绘制，主行文本可走 **彩色 Emoji 编辑框** 管线，适合与现有 Emoji 窗口体系一致的皮肤与字体。

- **实现文件**：`emoji_window/d2d_datetime_picker.cpp`
- **头文件声明**：`emoji_window/emoji_window.h`（导出函数与 `DateTimePickerPrecision` 枚举）
- **易语言 DLL 中文命令名**：以 `examples/易语言代码/易语言代码文档/DLL命令.md` 中「D2D日期时间选择器」一节为准

## 显示精度

枚举 `DateTimePickerPrecision`（与创建参数、获取/设置精度共用）：

| 值 | 含义 | 主行显示示例（示意） |
|----|------|----------------------|
| `0` | 仅年 | `2024` |
| `1` | 年月日 | `2024-06-15` |
| `2` | 年月日时 | `2024-06-15 14` |
| `3` | 年月日时分 | `2024-06-15 14:30` |
| `4` | 年月日时分秒 | `2024-06-15 14:30:45` |

切换精度时，DLL 会对内部 `SYSTEMTIME` 做 **规范化**（例如降到「仅年月日」时会把时分秒清零），具体逻辑见源码中的 `NormalizeToPrecision` / `UpgradePrecisionFill`。

## 创建控件

```c++
HWND __stdcall CreateD2DDateTimePicker(
    HWND hParent,
    int x, int y, int width, int height,
    int initial_precision,
    UINT32 fg_color,
    UINT32 bg_color,
    UINT32 border_color,
    const unsigned char* font_name_bytes,
    int font_name_len,
    int font_size,
    BOOL bold,
    BOOL italic,
    BOOL underline
);
```

| 参数 | 说明 |
|------|------|
| `hParent` | 父窗口句柄 |
| `x`, `y` | 控件客户区位置 |
| `width`, `height` | 控件尺寸 |
| `initial_precision` | 初始精度，`0`～`4`，含义见上表 |
| `fg_color` / `bg_color` / `border_color` | ARGB（如 `0xFF606266`） |
| `font_name_bytes` / `font_name_len` | 字体名称 **UTF-8** 字节与长度 |
| `font_size` | 字号（像素） |
| `bold` / `italic` / `underline` | 字体样式 |

**返回值**：成功为控件 **宿主窗口** 句柄；失败为 `NULL`（易语言中与 `0` 比较）。

## 精度与日期时间读写

```c++
int  __stdcall GetD2DDateTimePickerPrecision(HWND hPicker);
void __stdcall SetD2DDateTimePickerPrecision(HWND hPicker, int precision);

void __stdcall GetD2DDateTimePickerDateTime(
    HWND hPicker,
    int* year, int* month, int* day,
    int* hour, int* minute, int* second);

void __stdcall SetD2DDateTimePickerDateTime(
    HWND hPicker,
    int year, int month, int day,
    int hour, int minute, int second);
```

- **读取**：`GetD2DDateTimePickerDateTime` 中不需要的指针可传 **`NULL`**（易语言中对应参数传 `0` 表示不读该项）。
- **写入**：按 **当前精度** 生效；超出部分由 DLL 忽略或规范化。

## 值改变回调

```c++
typedef void (__stdcall *ValueChangedCallback)(HWND hwnd);

void __stdcall SetD2DDateTimePickerCallback(HWND hPicker, ValueChangedCallback callback);
```

用户在弹出层确认日期/时间，或程序调用 `SetD2DDateTimePickerDateTime` 等导致值变化时，可触发回调（以当前实现为准）。回调仅传入 **控件句柄** `hwnd`。

## 启用、显示与布局

```c++
void __stdcall EnableD2DDateTimePicker(HWND hPicker, BOOL enable);
void __stdcall ShowD2DDateTimePicker(HWND hPicker, BOOL show);
void __stdcall SetD2DDateTimePickerBounds(HWND hPicker, int x, int y, int width, int height);
```

## 交互说明（弹出层）

- **日期**：月份导航、点击日格；选中逻辑与视图月份一致。
- **时间**：当精度包含时/分/秒时，弹出层提供 **分段** 的时、分、秒区域；可用 **方向键**、**滚轮**、**数字键** 等方式修改（与当前 DLL 版本行为一致）。
- **确定**：弹出层内确认后写回主行并触发值变化回调（若已设置）。

## 易语言示例与文档

- **Markdown 示例（可复制到 IDE）**：`examples/易语言代码/易语言代码文档/窗口程序集_D2D日期时间选择器示例.md`
- **示例源码**：`examples/易语言代码/日期时间选择器示例.e`

使用 DLL 命令前，请在程序中按 `DLL命令.md` 声明 **创建 / 获取 / 设置精度 / 读写日期时间 / 回调 / 启用 / 显示 / 设置位置** 等命令。

## 可视化设计器（`designer/`）

仓库根目录下的 **Web/Tauri 可视化设计器** 已支持 **「日期时间」** 控件（内部类型名 `datetimepicker`）：

| 项目 | 说明 |
|------|------|
| 工具箱 | **📅 日期时间** |
| 属性 | 精度、前景/背景/边框色、字体、初始年月日时分秒、启用/可见、值改变事件名等 |
| 生成代码 | 易语言 / C# / Python 模板见 `designer/src/data/controlDefs.ts` 与 `designer/src/codegen/` |

画布上为示意样式（日期时间行 + 下拉按钮），便于排版；实际行为以运行 `emoji_window.dll` 为准。

## 相关链接

- [编辑框（彩色 Emoji 文本）](editbox.md) — 同一套 D2D 文本显示能力
- [组合框](combobox.md) — 另一类下拉选择控件
