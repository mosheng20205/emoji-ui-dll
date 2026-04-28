# Emoji Window 火山 32/64 Demo

这个目录现在只保留“纯火山代码 + 直接调用 `emoji_window.dll`”的示例，不再包含旧的 C++ `native_bridge` 桥接方案。

## 当前保留内容

- `EmojiWindowDemo.vsln`：总解决方案，只包含 DLL 版 Win32 / x64 两个项目
- `EmojiWindowDemo_dll_win32.vprj`：32 位项目，调试目录和输出目录都是 `runtime_win32`
- `EmojiWindowDemo_dll_x64.vprj`：64 位项目，调试目录和输出目录都是 `runtime_x64`
- `EmojiWindowDemo_dll_win32.vsln`：只包含 32 位 DLL 项目
- `EmojiWindowDemo_dll_x64.vsln`：只包含 64 位 DLL 项目
- `src_dll\emoji_window_接口.wsv`：DLL 命令声明，直接使用火山 `@视窗.输入 = "emoji_window.dll"` 导入
- `src_dll\main.wsv`：演示界面源码
- `runtime_win32\`、`runtime_x64\`：运行目录，放对应架构的 `emoji_window.dll` 以及示例资源文件

## 运行方式

1. 用火山视窗打开 `EmojiWindowDemo.vsln`，或者按架构分别打开 `EmojiWindowDemo_dll_win32.vsln`、`EmojiWindowDemo_dll_x64.vsln`。
2. 选择对应项目：
   `EmojiWindow火山Demo_DLL_Win32` 或 `EmojiWindow火山Demo_DLL_x64`。
3. 确认目标运行目录里已经有同架构的 `emoji_window.dll`。
4. 编译运行。

默认输出文件名：

- `runtime_win32\EmojiWindowVolcanoDllDemo_Win32.exe`
- `runtime_x64\EmojiWindowVolcanoDllDemo_x64.exe`

如果你改了输出目录，至少要把下面这些文件放到生成 EXE 的同级目录：

- `emoji_window.dll`
- `icon.ico`
- `hero.png`

## 编码要求

`.vsln`、`.vprj` 和 `src_dll\*.wsv` 都应保持为 `UTF-16 LE` 且带 `BOM`。

如果你用外部编辑器修改过这些文件，又遇到下面这类错误：

- `从文件 "...\\src_dll\\main.wsv" 载入文档数据失败`
- `文档首成员不为有效的文档格式及版本号声明结点`
- `没有找到所指定名称的类 "启动类"`

通常不是源码逻辑错了，而是文件编码被改坏了。建议按下面顺序处理：

1. 关闭火山视窗 IDE。
2. 确认 `src_dll\main.wsv` 和 `src_dll\emoji_window_接口.wsv` 仍然是 `UTF-16 LE BOM`。
3. 删除工程目录下的 `_int` 缓存目录后重新打开解决方案。
4. 重新编译。

## 实现说明

这套示例现在不再依赖任何 C++ 桥接源码：

- 不再使用 `src\native_bridge.cpp`
- 不再通过 `LoadLibraryW` 手写桥接层
- 所有接口都直接在 `src_dll\emoji_window_接口.wsv` 里声明

也就是说，火山代码直接调用 `emoji_window.dll` 的导出函数；窗口创建、控件创建、回调绑定和样式设置都在火山源码里完成。
