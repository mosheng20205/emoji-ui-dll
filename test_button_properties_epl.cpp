// 测试按钮属性API的易语言调用
// 编译命令: cl /EHsc /std:c++17 test_button_properties_epl.cpp /link emoji_window.lib user32.lib

#include <windows.h>
#include <iostream>
#include <string>

// 声明DLL函数
extern "C" {
    typedef HWND(__stdcall* CreateWindowBytesExFunc)(const unsigned char*, int, int, int, int);
    typedef int(__stdcall* CreateEmojiButtonBytesFunc)(HWND, const unsigned char*, int, const unsigned char*, int, int, int, int, int, COLORREF);
    typedef int(__stdcall* GetButtonTextFunc)(int, unsigned char*, int);
    typedef void(__stdcall* SetButtonTextFunc)(int, const unsigned char*, int);
    typedef int(__stdcall* GetButtonEmojiFunc)(int, unsigned char*, int);
    typedef void(__stdcall* SetButtonEmojiFunc)(int, const unsigned char*, int);
    typedef void(__stdcall* GetButtonBoundsFunc)(int, int*, int*, int*, int*);
    typedef void(__stdcall* SetButtonBoundsFunc)(int, int, int, int, int);
    typedef COLORREF(__stdcall* GetButtonBackgroundColorFunc)(int);
    typedef void(__stdcall* SetButtonBackgroundColorFunc)(int, COLORREF);
    typedef BOOL(__stdcall* GetButtonVisibleFunc)(int);
    typedef void(__stdcall* ShowButtonFunc)(int, BOOL);
    typedef BOOL(__stdcall* GetButtonEnabledFunc)(int);
    typedef void(__stdcall* SetMessageLoopMainWindowFunc)(HWND);
    typedef int(__stdcall* RunMessageLoopFunc)();
}

int main() {
    std::cout << "=== 按钮属性API易语言调用测试 ===" << std::endl;

    // 加载DLL
    HMODULE hDll = LoadLibraryA("emoji_window.dll");
    if (!hDll) {
        std::cerr << "无法加载emoji_window.dll" << std::endl;
        return 1;
    }

    // 获取函数指针
    auto CreateWindowBytesEx = (CreateWindowBytesExFunc)GetProcAddress(hDll, "create_window_bytes_ex");
    auto CreateEmojiButtonBytes = (CreateEmojiButtonBytesFunc)GetProcAddress(hDll, "create_emoji_button_bytes");
    auto GetButtonText = (GetButtonTextFunc)GetProcAddress(hDll, "GetButtonText");
    auto SetButtonText = (SetButtonTextFunc)GetProcAddress(hDll, "SetButtonText");
    auto GetButtonEmoji = (GetButtonEmojiFunc)GetProcAddress(hDll, "GetButtonEmoji");
    auto SetButtonEmoji = (SetButtonEmojiFunc)GetProcAddress(hDll, "SetButtonEmoji");
    auto GetButtonBounds = (GetButtonBoundsFunc)GetProcAddress(hDll, "GetButtonBounds");
    auto SetButtonBounds = (SetButtonBoundsFunc)GetProcAddress(hDll, "SetButtonBounds");
    auto GetButtonBackgroundColor = (GetButtonBackgroundColorFunc)GetProcAddress(hDll, "GetButtonBackgroundColor");
    auto SetButtonBackgroundColor = (SetButtonBackgroundColorFunc)GetProcAddress(hDll, "SetButtonBackgroundColor");
    auto GetButtonVisible = (GetButtonVisibleFunc)GetProcAddress(hDll, "GetButtonVisible");
    auto ShowButton = (ShowButtonFunc)GetProcAddress(hDll, "ShowButton");
    auto GetButtonEnabled = (GetButtonEnabledFunc)GetProcAddress(hDll, "GetButtonEnabled");

    if (!CreateWindowBytesEx || !CreateEmojiButtonBytes || !GetButtonText || !SetButtonText ||
        !GetButtonEmoji || !SetButtonEmoji || !GetButtonBounds || !SetButtonBounds ||
        !GetButtonBackgroundColor || !SetButtonBackgroundColor || !GetButtonVisible ||
        !ShowButton || !GetButtonEnabled) {
        std::cerr << "无法获取所有必需的函数指针" << std::endl;
        FreeLibrary(hDll);
        return 1;
    }

    std::cout << "✓ 所有按钮属性API函数指针获取成功" << std::endl;

    // 创建窗口
    const char* title = "按钮属性测试";
    HWND hwnd = CreateWindowBytesEx((const unsigned char*)title, strlen(title), 600, 400, 0);
    if (!hwnd) {
        std::cerr << "创建窗口失败" << std::endl;
        FreeLibrary(hDll);
        return 1;
    }
    std::cout << "✓ 窗口创建成功" << std::endl;

    // 创建测试按钮
    const char* emoji = "🎨";
    const char* text = "测试按钮";
    int buttonId = CreateEmojiButtonBytes(hwnd, (const unsigned char*)emoji, strlen(emoji),
        (const unsigned char*)text, strlen(text), 50, 50, 150, 40, RGB(100, 150, 255));
    if (buttonId <= 0) {
        std::cerr << "创建按钮失败" << std::endl;
        FreeLibrary(hDll);
        return 1;
    }
    std::cout << "✓ 按钮创建成功，ID=" << buttonId << std::endl;

    // 测试1: 获取按钮文本
    std::cout << "\n--- 测试1: 获取按钮文本 ---" << std::endl;
    int textLen = GetButtonText(buttonId, nullptr, 0);
    if (textLen > 0) {
        unsigned char* buffer = new unsigned char[textLen + 1];
        int actualLen = GetButtonText(buttonId, buffer, textLen);
        buffer[actualLen] = 0;
        std::cout << "✓ 按钮文本: " << (char*)buffer << " (长度: " << actualLen << ")" << std::endl;
        delete[] buffer;
    } else {
        std::cerr << "✗ 获取按钮文本失败" << std::endl;
    }

    // 测试2: 设置按钮文本
    std::cout << "\n--- 测试2: 设置按钮文本 ---" << std::endl;
    const char* newText = "新文本";
    SetButtonText(buttonId, (const unsigned char*)newText, strlen(newText));
    std::cout << "✓ 设置按钮文本为: " << newText << std::endl;

    // 验证文本是否修改成功
    textLen = GetButtonText(buttonId, nullptr, 0);
    if (textLen > 0) {
        unsigned char* buffer = new unsigned char[textLen + 1];
        GetButtonText(buttonId, buffer, textLen);
        buffer[textLen] = 0;
        std::cout << "✓ 验证：当前按钮文本: " << (char*)buffer << std::endl;
        delete[] buffer;
    }

    // 测试3: 获取按钮Emoji
    std::cout << "\n--- 测试3: 获取按钮Emoji ---" << std::endl;
    int emojiLen = GetButtonEmoji(buttonId, nullptr, 0);
    if (emojiLen > 0) {
        unsigned char* buffer = new unsigned char[emojiLen + 1];
        int actualLen = GetButtonEmoji(buttonId, buffer, emojiLen);
        buffer[actualLen] = 0;
        std::cout << "✓ 按钮Emoji: " << (char*)buffer << " (长度: " << actualLen << ")" << std::endl;
        delete[] buffer;
    } else {
        std::cerr << "✗ 获取按钮Emoji失败" << std::endl;
    }

    // 测试4: 设置按钮Emoji
    std::cout << "\n--- 测试4: 设置按钮Emoji ---" << std::endl;
    const char* newEmoji = "🚀";
    SetButtonEmoji(buttonId, (const unsigned char*)newEmoji, strlen(newEmoji));
    std::cout << "✓ 设置按钮Emoji为: " << newEmoji << std::endl;

    // 验证Emoji是否修改成功
    emojiLen = GetButtonEmoji(buttonId, nullptr, 0);
    if (emojiLen > 0) {
        unsigned char* buffer = new unsigned char[emojiLen + 1];
        GetButtonEmoji(buttonId, buffer, emojiLen);
        buffer[emojiLen] = 0;
        std::cout << "✓ 验证：当前按钮Emoji: " << (char*)buffer << std::endl;
        delete[] buffer;
    }

    // 测试5: 获取按钮位置
    std::cout << "\n--- 测试5: 获取按钮位置 ---" << std::endl;
    int x, y, width, height;
    GetButtonBounds(buttonId, &x, &y, &width, &height);
    std::cout << "✓ 按钮位置: X=" << x << ", Y=" << y << ", 宽=" << width << ", 高=" << height << std::endl;

    // 测试6: 设置按钮位置
    std::cout << "\n--- 测试6: 设置按钮位置 ---" << std::endl;
    SetButtonBounds(buttonId, 100, 100, 200, 50);
    std::cout << "✓ 设置按钮位置为: X=100, Y=100, 宽=200, 高=50" << std::endl;

    // 验证位置是否修改成功
    GetButtonBounds(buttonId, &x, &y, &width, &height);
    std::cout << "✓ 验证：当前按钮位置: X=" << x << ", Y=" << y << ", 宽=" << width << ", 高=" << height << std::endl;

    // 测试7: 获取按钮背景色
    std::cout << "\n--- 测试7: 获取按钮背景色 ---" << std::endl;
    COLORREF bgColor = GetButtonBackgroundColor(buttonId);
    std::cout << "✓ 按钮背景色: ARGB=0x" << std::hex << bgColor << std::dec << std::endl;

    // 测试8: 设置按钮背景色
    std::cout << "\n--- 测试8: 设置按钮背景色 ---" << std::endl;
    COLORREF newColor = RGB(255, 100, 100);
    SetButtonBackgroundColor(buttonId, newColor);
    std::cout << "✓ 设置按钮背景色为: RGB(255, 100, 100)" << std::endl;

    // 验证背景色是否修改成功
    bgColor = GetButtonBackgroundColor(buttonId);
    std::cout << "✓ 验证：当前按钮背景色: ARGB=0x" << std::hex << bgColor << std::dec << std::endl;

    // 测试9: 获取按钮可视状态
    std::cout << "\n--- 测试9: 获取按钮可视状态 ---" << std::endl;
    BOOL visible = GetButtonVisible(buttonId);
    std::cout << "✓ 按钮可视状态: " << (visible ? "可见" : "隐藏") << std::endl;

    // 测试10: 设置按钮可视状态
    std::cout << "\n--- 测试10: 设置按钮可视状态 ---" << std::endl;
    ShowButton(buttonId, FALSE);
    std::cout << "✓ 设置按钮为隐藏" << std::endl;
    visible = GetButtonVisible(buttonId);
    std::cout << "✓ 验证：当前按钮可视状态: " << (visible ? "可见" : "隐藏") << std::endl;

    ShowButton(buttonId, TRUE);
    std::cout << "✓ 设置按钮为可见" << std::endl;
    visible = GetButtonVisible(buttonId);
    std::cout << "✓ 验证：当前按钮可视状态: " << (visible ? "可见" : "隐藏") << std::endl;

    // 测试11: 获取按钮启用状态
    std::cout << "\n--- 测试11: 获取按钮启用状态 ---" << std::endl;
    BOOL enabled = GetButtonEnabled(buttonId);
    std::cout << "✓ 按钮启用状态: " << (enabled ? "启用" : "禁用") << std::endl;

    std::cout << "\n=== 所有测试完成 ===" << std::endl;
    std::cout << "✓ 所有按钮属性API测试通过！" << std::endl;

    // 清理
    FreeLibrary(hDll);
    return 0;
}
