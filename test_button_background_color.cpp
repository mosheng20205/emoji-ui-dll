// 测试按钮背景色属性命令
#include <windows.h>
#include <stdio.h>

// 函数声明
typedef HWND (__stdcall *CreateWindowFunc)(const char*, int, int);
typedef int (__stdcall *CreateButtonFunc)(HWND, const unsigned char*, int, const unsigned char*, int, int, int, int, int, UINT32);
typedef UINT32 (__stdcall *GetButtonBackgroundColorFunc)(int);
typedef void (__stdcall *SetButtonBackgroundColorFunc)(int, UINT32);

int main() {
    printf("=== 测试按钮背景色属性命令 ===\n\n");
    
    // 加载DLL
    HMODULE hDll = LoadLibraryA("emoji_window.dll");
    if (!hDll) {
        printf("错误: 无法加载 emoji_window.dll\n");
        return 1;
    }
    printf("✓ 成功加载 emoji_window.dll\n");
    
    // 获取函数指针
    auto create_window = (CreateWindowFunc)GetProcAddress(hDll, "create_window");
    auto create_emoji_button_bytes = (CreateButtonFunc)GetProcAddress(hDll, "create_emoji_button_bytes");
    auto GetButtonBackgroundColor = (GetButtonBackgroundColorFunc)GetProcAddress(hDll, "GetButtonBackgroundColor");
    auto SetButtonBackgroundColor = (SetButtonBackgroundColorFunc)GetProcAddress(hDll, "SetButtonBackgroundColor");
    
    if (!create_window || !create_emoji_button_bytes || !GetButtonBackgroundColor || !SetButtonBackgroundColor) {
        printf("错误: 无法获取函数指针\n");
        if (!GetButtonBackgroundColor) printf("  - GetButtonBackgroundColor 未找到\n");
        if (!SetButtonBackgroundColor) printf("  - SetButtonBackgroundColor 未找到\n");
        FreeLibrary(hDll);
        return 1;
    }
    printf("✓ 成功获取所有函数指针\n\n");
    
    // 创建窗口
    HWND hwnd = create_window("测试窗口", 400, 300);
    if (!hwnd) {
        printf("错误: 无法创建窗口\n");
        FreeLibrary(hDll);
        return 1;
    }
    printf("✓ 成功创建窗口\n");
    
    // 创建按钮（初始背景色为蓝色 0xFF0000FF）
    const char* emoji = "🎨";
    const char* text = "测试按钮";
    UINT32 initial_color = 0xFF0000FF;  // 蓝色 (ARGB)
    
    int button_id = create_emoji_button_bytes(
        hwnd,
        (const unsigned char*)emoji, strlen(emoji),
        (const unsigned char*)text, strlen(text),
        50, 50, 150, 40,
        initial_color
    );
    
    if (button_id < 0) {
        printf("错误: 无法创建按钮\n");
        FreeLibrary(hDll);
        return 1;
    }
    printf("✓ 成功创建按钮 (ID=%d)\n\n", button_id);
    
    // 测试1: 获取初始背景色
    printf("测试1: 获取初始背景色\n");
    UINT32 color = GetButtonBackgroundColor(button_id);
    printf("  初始背景色: 0x%08X\n", color);
    if (color == initial_color) {
        printf("  ✓ 背景色正确 (预期: 0x%08X)\n\n", initial_color);
    } else {
        printf("  ✗ 背景色错误 (预期: 0x%08X, 实际: 0x%08X)\n\n", initial_color, color);
    }
    
    // 测试2: 设置新的背景色（红色）
    printf("测试2: 设置新的背景色\n");
    UINT32 new_color = 0xFFFF0000;  // 红色 (ARGB)
    SetButtonBackgroundColor(button_id, new_color);
    printf("  设置背景色为: 0x%08X (红色)\n", new_color);
    
    // 验证设置是否成功
    color = GetButtonBackgroundColor(button_id);
    printf("  读取背景色: 0x%08X\n", color);
    if (color == new_color) {
        printf("  ✓ 背景色设置成功\n\n");
    } else {
        printf("  ✗ 背景色设置失败 (预期: 0x%08X, 实际: 0x%08X)\n\n", new_color, color);
    }
    
    // 测试3: 设置绿色背景
    printf("测试3: 设置绿色背景\n");
    UINT32 green_color = 0xFF00FF00;  // 绿色 (ARGB)
    SetButtonBackgroundColor(button_id, green_color);
    printf("  设置背景色为: 0x%08X (绿色)\n", green_color);
    
    color = GetButtonBackgroundColor(button_id);
    printf("  读取背景色: 0x%08X\n", color);
    if (color == green_color) {
        printf("  ✓ 背景色设置成功\n\n");
    } else {
        printf("  ✗ 背景色设置失败 (预期: 0x%08X, 实际: 0x%08X)\n\n", green_color, color);
    }
    
    // 测试4: 设置半透明颜色
    printf("测试4: 设置半透明颜色\n");
    UINT32 alpha_color = 0x80FF00FF;  // 半透明紫色 (ARGB)
    SetButtonBackgroundColor(button_id, alpha_color);
    printf("  设置背景色为: 0x%08X (半透明紫色)\n", alpha_color);
    
    color = GetButtonBackgroundColor(button_id);
    printf("  读取背景色: 0x%08X\n", color);
    if (color == alpha_color) {
        printf("  ✓ 背景色设置成功\n\n");
    } else {
        printf("  ✗ 背景色设置失败 (预期: 0x%08X, 实际: 0x%08X)\n\n", alpha_color, color);
    }
    
    // 测试5: 测试无效按钮ID
    printf("测试5: 测试无效按钮ID\n");
    color = GetButtonBackgroundColor(9999);
    printf("  无效ID返回值: 0x%08X\n", color);
    if (color == 0) {
        printf("  ✓ 正确处理无效ID (返回0)\n\n");
    } else {
        printf("  ✗ 无效ID处理错误 (应返回0)\n\n");
    }
    
    printf("=== 测试完成 ===\n");
    printf("\n提示: 窗口将保持打开5秒，您可以查看按钮颜色变化\n");
    
    // 显示窗口
    ShowWindow(hwnd, SW_SHOW);
    UpdateWindow(hwnd);
    
    // 等待5秒让用户看到效果
    Sleep(5000);
    
    // 清理
    FreeLibrary(hDll);
    
    return 0;
}
