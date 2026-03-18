#include <windows.h>
#include <iostream>

// 声明DLL函数
typedef HWND(__stdcall* CreateWindowBytesFunc)(const unsigned char*, int, int, int);
typedef int(__stdcall* CreateEmojiButtonBytesFunc)(HWND, const unsigned char*, int, const unsigned char*, int, int, int, int, int, UINT32);
typedef void(__stdcall* EnableButtonFunc)(HWND, int, BOOL);
typedef BOOL(__stdcall* GetButtonEnabledFunc)(int);

int main() {
    // 加载DLL
    HMODULE hDll = LoadLibraryA("emoji_window.dll");
    if (!hDll) {
        std::cout << "Failed to load emoji_window.dll" << std::endl;
        return 1;
    }

    // 获取函数指针
    auto create_window_bytes = (CreateWindowBytesFunc)GetProcAddress(hDll, "create_window_bytes");
    auto create_emoji_button_bytes = (CreateEmojiButtonBytesFunc)GetProcAddress(hDll, "create_emoji_button_bytes");
    auto enable_button = (EnableButtonFunc)GetProcAddress(hDll, "EnableButton");
    auto get_button_enabled = (GetButtonEnabledFunc)GetProcAddress(hDll, "GetButtonEnabled");

    if (!create_window_bytes || !create_emoji_button_bytes || !enable_button || !get_button_enabled) {
        std::cout << "Failed to get function addresses" << std::endl;
        FreeLibrary(hDll);
        return 1;
    }

    // 创建窗口
    const unsigned char title[] = "Test GetButtonEnabled";
    HWND hwnd = create_window_bytes(title, sizeof(title) - 1, 400, 300);
    if (!hwnd) {
        std::cout << "Failed to create window" << std::endl;
        FreeLibrary(hDll);
        return 1;
    }

    // 创建按钮
    const unsigned char emoji[] = "";
    const unsigned char text[] = "Test Button";
    int button_id = create_emoji_button_bytes(hwnd, emoji, sizeof(emoji) - 1, text, sizeof(text) - 1, 50, 50, 150, 40, 0xFF4CAF50);

    if (button_id < 0) {
        std::cout << "Failed to create button" << std::endl;
        DestroyWindow(hwnd);
        FreeLibrary(hDll);
        return 1;
    }

    std::cout << "=== Test GetButtonEnabled ===" << std::endl;

    // Test 1: Get initial enabled state (should be enabled)
    BOOL enabled = get_button_enabled(button_id);
    std::cout << "Test 1 - Initial state: " << (enabled ? "Enabled" : "Disabled") << std::endl;
    if (!enabled) {
        std::cout << "  [FAIL] Button should be enabled initially" << std::endl;
    } else {
        std::cout << "  [PASS]" << std::endl;
    }

    // Test 2: Get state after disabling
    enable_button(hwnd, button_id, FALSE);
    Sleep(100);
    enabled = get_button_enabled(button_id);
    std::cout << "Test 2 - After disable: " << (enabled ? "Enabled" : "Disabled") << std::endl;
    if (enabled) {
        std::cout << "  [FAIL] Button should be disabled" << std::endl;
    } else {
        std::cout << "  [PASS]" << std::endl;
    }

    // Test 3: Get state after re-enabling
    enable_button(hwnd, button_id, TRUE);
    Sleep(100);
    enabled = get_button_enabled(button_id);
    std::cout << "Test 3 - After re-enable: " << (enabled ? "Enabled" : "Disabled") << std::endl;
    if (!enabled) {
        std::cout << "  [FAIL] Button should be enabled" << std::endl;
    } else {
        std::cout << "  [PASS]" << std::endl;
    }

    // Test 4: Test invalid button ID
    enabled = get_button_enabled(99999);
    std::cout << "Test 4 - Invalid button ID: " << (enabled ? "Enabled" : "Disabled") << std::endl;
    if (enabled) {
        std::cout << "  [FAIL] Invalid button ID should return FALSE" << std::endl;
    } else {
        std::cout << "  [PASS]" << std::endl;
    }

    // Test 5: Multiple state toggles
    std::cout << "Test 5 - Multiple state toggles:" << std::endl;
    bool all_passed = true;
    for (int i = 0; i < 5; i++) {
        BOOL expected = (i % 2 == 0) ? FALSE : TRUE;
        enable_button(hwnd, button_id, expected);
        Sleep(50);
        enabled = get_button_enabled(button_id);
        std::cout << "  Toggle " << (i + 1) << ": Expected=" << (expected ? "Enabled" : "Disabled") 
                  << ", Actual=" << (enabled ? "Enabled" : "Disabled");
        if (enabled != expected) {
            std::cout << " [FAIL]" << std::endl;
            all_passed = false;
        } else {
            std::cout << " [PASS]" << std::endl;
        }
    }
    if (all_passed) {
        std::cout << "  [PASS] All toggle tests passed" << std::endl;
    }

    std::cout << "\n=== Tests Complete ===" << std::endl;
    std::cout << "Press Enter to close window..." << std::endl;
    std::cin.get();

    // 清理
    DestroyWindow(hwnd);
    FreeLibrary(hDll);

    return 0;
}
