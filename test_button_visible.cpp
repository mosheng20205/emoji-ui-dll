// 测试按钮可视状态属性命令
#include <windows.h>
#include <stdio.h>

// 导入DLL函数
typedef HWND(__stdcall* CreateWindowBytesFunc)(const unsigned char*, int, int, int);
typedef int(__stdcall* CreateEmojiButtonBytesFunc)(HWND, const unsigned char*, int, const unsigned char*, int, int, int, int, int, UINT32);
typedef BOOL(__stdcall* GetButtonVisibleFunc)(int);
typedef void(__stdcall* ShowButtonFunc)(int, BOOL);
typedef void(__stdcall* RunMessageLoopFunc)();

int main() {
    // 加载DLL
    HMODULE hDll = LoadLibraryA("emoji_window.dll");
    if (!hDll) {
        printf("错误: 无法加载 emoji_window.dll\n");
        return 1;
    }

    // 获取函数指针
    auto create_window_bytes = (CreateWindowBytesFunc)GetProcAddress(hDll, "create_window_bytes");
    auto create_emoji_button_bytes = (CreateEmojiButtonBytesFunc)GetProcAddress(hDll, "create_emoji_button_bytes");
    auto GetButtonVisible = (GetButtonVisibleFunc)GetProcAddress(hDll, "GetButtonVisible");
    auto ShowButton = (ShowButtonFunc)GetProcAddress(hDll, "ShowButton");
    auto run_message_loop = (RunMessageLoopFunc)GetProcAddress(hDll, "run_message_loop");

    if (!create_window_bytes || !create_emoji_button_bytes || !GetButtonVisible || !ShowButton) {
        printf("错误: 无法获取函数指针\n");
        FreeLibrary(hDll);
        return 1;
    }

    printf("=== 按钮可视状态属性测试 ===\n\n");

    // 创建窗口
    const char* title = "按钮可视状态测试";
    HWND hwnd = create_window_bytes((const unsigned char*)title, strlen(title), 600, 400);
    if (!hwnd) {
        printf("错误: 无法创建窗口\n");
        FreeLibrary(hDll);
        return 1;
    }
    printf("✓ 窗口创建成功\n");

    // 创建测试按钮
    const char* emoji = "👁️";
    const char* text = "可见按钮";
    int button_id = create_emoji_button_bytes(hwnd, 
        (const unsigned char*)emoji, strlen(emoji),
        (const unsigned char*)text, strlen(text),
        50, 50, 200, 40, 0xFF4CAF50);
    
    if (button_id < 0) {
        printf("错误: 无法创建按钮\n");
        FreeLibrary(hDll);
        return 1;
    }
    printf("✓ 按钮创建成功 (ID: %d)\n\n", button_id);

    // 测试1: 获取初始可视状态
    printf("测试1: 获取初始可视状态\n");
    BOOL visible = GetButtonVisible(button_id);
    printf("  初始可视状态: %s\n", visible ? "可见" : "隐藏");
    if (visible) {
        printf("  ✓ 测试通过 - 按钮默认可见\n\n");
    } else {
        printf("  ✗ 测试失败 - 按钮应该默认可见\n\n");
    }

    // 测试2: 隐藏按钮
    printf("测试2: 隐藏按钮\n");
    ShowButton(button_id, FALSE);
    visible = GetButtonVisible(button_id);
    printf("  隐藏后可视状态: %s\n", visible ? "可见" : "隐藏");
    if (!visible) {
        printf("  ✓ 测试通过 - 按钮已隐藏\n\n");
    } else {
        printf("  ✗ 测试失败 - 按钮应该被隐藏\n\n");
    }

    // 测试3: 显示按钮
    printf("测试3: 显示按钮\n");
    ShowButton(button_id, TRUE);
    visible = GetButtonVisible(button_id);
    printf("  显示后可视状态: %s\n", visible ? "可见" : "隐藏");
    if (visible) {
        printf("  ✓ 测试通过 - 按钮已显示\n\n");
    } else {
        printf("  ✗ 测试失败 - 按钮应该被显示\n\n");
    }

    // 测试4: 测试无效按钮ID
    printf("测试4: 测试无效按钮ID\n");
    BOOL invalid_result = GetButtonVisible(9999);
    printf("  无效ID返回值: %s\n", invalid_result ? "TRUE" : "FALSE");
    if (!invalid_result) {
        printf("  ✓ 测试通过 - 无效ID返回FALSE\n\n");
    } else {
        printf("  ✗ 测试失败 - 无效ID应该返回FALSE\n\n");
    }

    // 测试5: 多次切换可视状态
    printf("测试5: 多次切换可视状态\n");
    bool all_passed = true;
    for (int i = 0; i < 5; i++) {
        ShowButton(button_id, i % 2 == 0);
        visible = GetButtonVisible(button_id);
        bool expected = (i % 2 == 0);
        printf("  第%d次切换: 期望=%s, 实际=%s\n", 
            i + 1, 
            expected ? "可见" : "隐藏",
            visible ? "可见" : "隐藏");
        if ((visible != 0) != expected) {
            all_passed = false;
        }
    }
    if (all_passed) {
        printf("  ✓ 测试通过 - 多次切换正常\n\n");
    } else {
        printf("  ✗ 测试失败 - 多次切换异常\n\n");
    }

    printf("=== 测试完成 ===\n");
    printf("提示: 窗口将保持打开，您可以手动验证按钮的可视状态\n");
    printf("      按钮应该是可见的（最后一次切换设置为可见）\n");

    // 运行消息循环
    if (run_message_loop) {
        run_message_loop();
    }

    FreeLibrary(hDll);
    return 0;
}
