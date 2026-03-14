# C# 示例代码

这是 emoji_window.dll 的 C# 调用示例，包含 32位 和 64位 两个版本。

## 📁 项目结构

```
Csharp/
├── EmojiWindowDemo_x86/          # 32位示例项目
│   ├── EmojiWindowNative.cs      # P/Invoke 声明
│   ├── Program.cs                # 主程序
│   └── EmojiWindowDemo_x86.csproj
├── EmojiWindowDemo_x64/          # 64位示例项目
│   ├── EmojiWindowNative.cs      # P/Invoke 声明
│   ├── Program.cs                # 主程序
│   └── EmojiWindowDemo_x64.csproj
└── README.md                     # 本文件
```

## ✨ 功能展示

本示例参考截图实现了以下功能：

- ✅ 创建主窗口（带 Unicode 表情标题）
- ✅ 3个分组框（控制面板、生成结果、验证测试）
- ✅ 多个标签（带 Unicode 表情图标）
- ✅ 多个按钮（带 Emoji 图标）
- ✅ 复选框（带回调）
- ✅ 信息框（带 Unicode 表情）

## 🚀 快速开始

### 前置要求

- Visual Studio 2019 或更高版本
- .NET Framework 4.8
- .NET SDK（用于 dotnet 命令）
- emoji_window.dll（32位或64位）

### 编译步骤

#### ⚠️ 重要：首次编译前需要还原 NuGet 包

**方法 1：自动还原（推荐）**
```cmd
# 双击运行
restore.bat
```

**方法 2：使用简化编译脚本（自动还原）**
```cmd
# 双击运行
build_simple.bat
```

#### 方法 1：使用 Visual Studio

1. 打开 `EmojiWindowDemo.sln`
2. 右键解决方案 → "还原 NuGet 包"
3. 选择项目（x86 或 x64）
4. 按 F5 运行

#### 方法 2：使用简化编译脚本（推荐）

```cmd
# 双击运行（自动还原 NuGet 包）
build_simple.bat

# 选择编译选项
[1] 编译 32位版本
[2] 编译 64位版本
[3] 编译全部版本
```

#### 方法 3：使用 MSBuild 编译脚本

```cmd
# 1. 先还原 NuGet 包
restore.bat

# 2. 再编译
build.bat
```

#### 方法 4：使用命令行

```cmd
# 32位（自动还原包）
cd EmojiWindowDemo_x86
dotnet build -c Release

# 64位（自动还原包）
cd EmojiWindowDemo_x64
dotnet build -c Release
```

### 运行程序

**32位：**
```cmd
cd examples\Csharp\EmojiWindowDemo_x86\bin\Release\net48
EmojiWindowDemo_x86.exe
```

**64位：**
```cmd
cd examples\Csharp\EmojiWindowDemo_x64\bin\Release\net48
EmojiWindowDemo_x64.exe
```

## 📝 代码说明

### 1. P/Invoke 声明

`EmojiWindowNative.cs` 包含了所有 DLL 函数的声明：

```csharp
[DllImport("emoji_window.dll", CallingConvention = CallingConvention.StdCall)]
public static extern IntPtr create_window_bytes(
    byte[] titleBytes,
    int titleLen,
    int width,
    int height
);
```

### 2. UTF-8 编码转换

由于 DLL 使用 UTF-8 编码，需要转换字符串：

```csharp
public static byte[] StringToUtf8Bytes(string str)
{
    return Encoding.UTF8.GetBytes(str);
}
```

### 3. 创建窗口

```csharp
string windowTitle = "🎨 抖店安全参数生成器 - C# Demo";
byte[] titleBytes = EmojiWindowNative.StringToUtf8Bytes(windowTitle);
IntPtr mainWindow = EmojiWindowNative.create_window_bytes(
    titleBytes, titleBytes.Length, 800, 600
);
```

### 4. 创建控件

**标签（带 Emoji）：**
```csharp
byte[] labelText = EmojiWindowNative.StringToUtf8Bytes("🌐 URL:");
byte[] fontName = EmojiWindowNative.StringToUtf8Bytes("Microsoft YaHei UI");
IntPtr label = EmojiWindowNative.CreateLabel(
    mainWindow,
    40, 80, 80, 30,
    labelText, labelText.Length,
    EmojiWindowNative.ARGB(255, 50, 50, 50),
    EmojiWindowNative.ARGB(0, 0, 0, 0),
    fontName, fontName.Length,
    14, false, false, false,
    0, false
);
```

**分组框：**
```csharp
byte[] groupTitle = EmojiWindowNative.StringToUtf8Bytes("📋 控制面板");
IntPtr groupBox = EmojiWindowNative.CreateGroupBox(
    mainWindow,
    20, 50, 360, 200,
    groupTitle, groupTitle.Length,
    EmojiWindowNative.ARGB(255, 220, 220, 220),
    EmojiWindowNative.ARGB(255, 250, 250, 250)
);
```

**按钮（带 Emoji）：**
```csharp
byte[] btnEmoji = EmojiWindowNative.StringToUtf8Bytes("🚀");
byte[] btnText = EmojiWindowNative.StringToUtf8Bytes("批量生成");
int btnId = EmojiWindowNative.create_emoji_button_bytes(
    mainWindow,
    btnEmoji, btnEmoji.Length,
    btnText, btnText.Length,
    40, 200, 120, 35,
    EmojiWindowNative.ARGB(255, 64, 158, 255)
);
```

### 5. 回调函数

**按钮点击回调：**
```csharp
private static EmojiWindowNative.ButtonClickCallback buttonCallback;

buttonCallback = OnButtonClick;
EmojiWindowNative.set_button_click_callback(buttonCallback);

static void OnButtonClick(int buttonId, IntPtr parentHwnd)
{
    Console.WriteLine($"按钮被点击: ID = {buttonId}");
    // 显示信息框
}
```

**复选框回调：**
```csharp
private static EmojiWindowNative.CheckBoxCallback checkBoxCallback;

checkBoxCallback = OnCheckBoxChanged;
EmojiWindowNative.SetCheckBoxCallback(checkBox1, checkBoxCallback);

static void OnCheckBoxChanged(IntPtr hCheckBox, bool isChecked)
{
    Console.WriteLine($"复选框状态改变: {(isChecked ? "选中" : "未选中")}");
}
```

### 6. 消息循环

```csharp
EmojiWindowNative.set_message_loop_main_window(mainWindow);
EmojiWindowNative.run_message_loop();
```

## ⚠️ 注意事项

### 1. 回调函数生命周期

**必须保持回调委托的引用**，否则会被垃圾回收导致崩溃：

```csharp
// ✅ 正确：使用静态字段保持引用
private static EmojiWindowNative.ButtonClickCallback buttonCallback;

buttonCallback = OnButtonClick;
EmojiWindowNative.set_button_click_callback(buttonCallback);

// ❌ 错误：局部变量会被回收
void SetupCallback()
{
    var callback = new ButtonClickCallback(OnButtonClick);
    EmojiWindowNative.set_button_click_callback(callback);
    // callback 离开作用域后会被回收！
}
```

### 2. 平台目标

- 32位项目必须设置 `<PlatformTarget>x86</PlatformTarget>`
- 64位项目必须设置 `<PlatformTarget>x64</PlatformTarget>`
- 不能使用 `AnyCPU`，否则会加载错误的 DLL

### 3. DLL 路径

确保 `emoji_window.dll` 在以下位置之一：

- 程序输出目录（推荐）
- 系统 PATH 环境变量中的目录
- Windows 系统目录

项目文件已配置自动复制 DLL：

```xml
<ItemGroup>
  <None Include="..\..\..\Release\emoji_window.dll">
    <CopyToOutputDirectory>PreserveNewest</CopyToOutputDirectory>
  </None>
</ItemGroup>
```

### 4. 调用约定

所有 DLL 函数使用 `StdCall` 调用约定：

```csharp
[DllImport("emoji_window.dll", CallingConvention = CallingConvention.StdCall)]
```

### 5. Unicode 表情支持

- 使用 UTF-8 编码传递字符串
- 确保字体支持 Emoji（如 "Segoe UI Emoji", "Microsoft YaHei UI"）
- 所有文本都需要转换为字节数组

## 🎨 颜色常量

```csharp
// Element UI 标准配色
uint COLOR_PRIMARY = ARGB(255, 64, 158, 255);   // #409EFF 主题蓝
uint COLOR_SUCCESS = ARGB(255, 103, 194, 58);   // #67C23A 成功绿
uint COLOR_WARNING = ARGB(255, 230, 162, 60);   // #E6A23C 警告橙
uint COLOR_DANGER = ARGB(255, 245, 108, 108);   // #F56C6C 危险红
uint COLOR_INFO = ARGB(255, 144, 147, 153);     // #909399 信息灰

// 文本颜色
uint TEXT_PRIMARY = ARGB(255, 48, 49, 51);      // #303133 主要文本
uint TEXT_REGULAR = ARGB(255, 96, 98, 102);     // #606266 常规文本
uint TEXT_SECONDARY = ARGB(255, 144, 147, 153); // #909399 次要文本

// 边框颜色
uint BORDER_BASE = ARGB(255, 220, 223, 230);    // #DCDFE6 基础边框
uint BORDER_LIGHT = ARGB(255, 228, 231, 237);   // #E4E7ED 浅色边框

// 背景颜色
uint BG_WHITE = ARGB(255, 255, 255, 255);       // #FFFFFF 白色背景
uint BG_LIGHT = ARGB(255, 245, 247, 250);       // #F5F7FA 浅色背景
```

## 🔧 常见问题

### Q1: 编译时提示 "找不到资产文件 project.assets.json"？

**错误信息：**
```
error NETSDK1004: 找不到资产文件"...\obj\project.assets.json"。
运行 NuGet 包还原以生成此文件。
```

**解决方案：**

**方法 1：使用还原脚本（最简单）**
```cmd
restore.bat
```

**方法 2：使用简化编译脚本（自动还原）**
```cmd
build_simple.bat
```

**方法 3：手动还原**
```cmd
cd EmojiWindowDemo_x86
dotnet restore
cd ..

cd EmojiWindowDemo_x64
dotnet restore
cd ..
```

**方法 4：Visual Studio**
- 右键解决方案 → "还原 NuGet 包"

### Q2: 运行时提示找不到 DLL？

**解决方案：**
1. 检查 DLL 是否在程序目录
2. 检查平台目标是否匹配（x86/x64）
3. 使用 [Dependency Walker](http://www.dependencywalker.com/) 检查依赖项

### Q3: 程序崩溃或无响应？

**可能原因：**
1. 回调委托被垃圾回收（使用静态字段保持引用）
2. 平台目标不匹配（x86 程序加载 x64 DLL）
3. 传递了无效的句柄或参数

### Q4: Emoji 显示为方框？

**解决方案：**
1. 确保使用 UTF-8 编码
2. 使用支持 Emoji 的字体（"Segoe UI Emoji", "Microsoft YaHei UI"）
3. 检查 Windows 版本（建议 Windows 10+）

### Q4: 如何调试 P/Invoke 调用？

**方法：**
1. 启用非托管代码调试（项目属性 → 调试 → 启用本机代码调试）
2. 使用 `Marshal.GetLastWin32Error()` 获取错误码
3. 添加 `Console.WriteLine` 输出调试信息

## 📚 更多资源

- [DLL 完整文档](../../README.md)
- [控件文档](../../docs/controls/)
- [易语言示例](../../易语言代码/)
- [C++ 示例](../C++/)
- [Python 示例](../Python/)

## 💡 扩展建议

1. **封装辅助类**：创建 `EmojiWindow` 类封装常用操作
2. **资源管理**：实现 `IDisposable` 接口管理窗口句柄
3. **异步操作**：使用 `async/await` 处理耗时操作
4. **MVVM 模式**：结合 WPF/WinForms 实现数据绑定
5. **单元测试**：编写测试用例验证 P/Invoke 调用

## 📄 许可证

MIT License

---

如有问题，请联系：
- QQ：1098901025
- 微信：zhx_ms
