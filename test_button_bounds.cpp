#include <windows.h>
#include <stdio.h>

// 函数指针类型定义
typedef HWND (__stdcall *CreateWindowBytesFunc)(const unsigned char*, int, int, int);
typedef int (__stdcall *CreateEmojiButtonBytesFunc)(HWND, const unsigned char*, int, const unsigned char*, int, int, int, int, int, UINT32);
typedef int (__stdcall *GetButtonBoundsFunc)(int, int*, int*, int*, int*);
typedef void (__stdcall *SetButtonBoundsFunc)(int, int, int, int, int);
typedef void (__stdcall *DestroyWindowFunc)(HWND);

int main() {
    // 加载DLL
    HMODULE hDll = LoadLibraryA("emoji_window/x64/Release/emoji_window.dll");
    if (!hDll) {
        printf("错误: 无法加载DLL\n");
        return 1;
    }

    // 获取函数指针
    auto CreateWindowBytes = (CreateWindowBytesFunc)GetProcAddress(hDll, "create_window_bytes");
    auto CreateEmojiButtonBytes = (CreateEmojiButtonBytesFunc)GetProcAddress(hDll, "create_emoji_button_bytes");
    auto GetButtonBounds = (GetButtonBoundsFunc)GetProcAddress(hDll, "GetButtonBounds");
    auto SetButtonBounds = (SetButtonBoundsFunc)GetProcAddress(hDll, "SetButtonBounds");
    auto DestroyWindow = (DestroyWindowFunc)GetProcAddress(hDll, "destroy_window");

    if (!CreateWindowBytes || !CreateEmojiButtonBytes || !GetButtonBounds || !SetButtonBounds) {
        printf("错误: 无法获取函数指针\n");
        FreeLibrary(hDll);
        return 1;
    }

    printf("=== 按钮位置和大小属性测试 ===\n\n");

    // 创建窗口
    const char* title = u8"按钮位置大小测试";
    HWND hwnd = CreateWindowBytes((const unsigned char*)title, strlen(title), 800, 600);
    if (!hwnd) {
        printf("错误: 无法创建窗口\n");
        FreeLibrary(hDll);
        return 1;
    }
    printf("✓ 窗口创建成功\n");

    // 创建按钮
    const char* emoji = u8"📏";
    const char* text = u8"测试按钮";
    int button_id = CreateEmojiButtonBytes(
        hwnd,
        (const unsigned char*)emoji, strlen(emoji),
        (const unsigned char*)text, strlen(text),
        100, 100, 200, 50,
        0xFF4CAF50
    );

    if (button_id < 0) {
        printf("错误: 无法创建按钮\n");
        DestroyWindow(hwnd);
        FreeLibrary(hDll);
        return 1;
    }
    printf("✓ 按钮创建成功 (ID: %d)\n\n", button_id);

    // 测试1: 获取初始位置和大小
    printf("测试1: 获取初始位置和大小\n");
    int x, y, width, height;
    int result = GetButtonBounds(button_id, &x, &y, &width, &height);
    
    if (result == 0) {
        printf("✓ 获取成功\n");
        printf("  位置: (%d, %d)\n", x, y);
        printf("  大小: %d x %d\n", width, height);
        
        // 验证值是否正确
        if (x == 100 && y == 100 && width == 200 && height == 50) {
            printf("✓ 值正确匹配初始设置\n");
        } else {
            printf("✗ 值不匹配: 期望(100, 100, 200, 50)\n");
        }
    } else {
        printf("✗ 获取失败，返回值: %d\n", result);
    }

    // 测试2: 设置新的位置和大小
    printf("\n测试2: 设置新的位置和大小\n");
    SetButtonBounds(button_id, 150, 200, 250, 60);
    printf("  已设置新位置: (150, 200)\n");
    printf("  已设置新大小: 250 x 60\n");

    // 验证设置
    result = GetButtonBounds(button_id, &x, &y, &width, &height);
    if (result == 0) {
        printf("✓ 验证获取成功\n");
        printf("  位置: (%d, %d)\n", x, y);
        printf("  大小: %d x %d\n", width, height);
        
        if (x == 150 && y == 200 && width == 250 && height == 60) {
            printf("✓ 新值设置成功\n");
        } else {
            printf("✗ 新值不匹配: 期望(150, 200, 250, 60)\n");
        }
    } else {
        printf("✗ 验证失败\n");
    }

    // 测试3: 设置边界值
    printf("\n测试3: 设置边界值\n");
    SetButtonBounds(button_id, 0, 0, 1, 1);
    result = GetButtonBounds(button_id, &x, &y, &width, &height);
    if (result == 0 && x == 0 && y == 0 && width == 1 && height == 1) {
        printf("✓ 最小值设置成功: (0, 0, 1, 1)\n");
    } else {
        printf("✗ 最小值设置失败\n");
    }

    SetButtonBounds(button_id, 10000, 10000, 5000, 5000);
    result = GetButtonBounds(button_id, &x, &y, &width, &height);
    if (result == 0 && x == 10000 && y == 10000 && width == 5000 && height == 5000) {
        printf("✓ 大值设置成功: (10000, 10000, 5000, 5000)\n");
    } else {
        printf("✗ 大值设置失败\n");
    }

    // 测试4: 部分参数为NULL
    printf("\n测试4: 部分参数为NULL\n");
    SetButtonBounds(button_id, 300, 300, 300, 80);
    
    // 只获取x和y
    int x_only, y_only;
    result = GetButtonBounds(button_id, &x_only, &y_only, nullptr, nullptr);
    if (result == 0 && x_only == 300 && y_only == 300) {
        printf("✓ 只获取位置成功: (%d, %d)\n", x_only, y_only);
    } else {
        printf("✗ 只获取位置失败\n");
    }

    // 只获取width和height
    int width_only, height_only;
    result = GetButtonBounds(button_id, nullptr, nullptr, &width_only, &height_only);
    if (result == 0 && width_only == 300 && height_only == 80) {
        printf("✓ 只获取大小成功: %d x %d\n", width_only, height_only);
    } else {
        printf("✗ 只获取大小失败\n");
    }

    // 测试5: 无效按钮ID
    printf("\n测试5: 测试无效按钮ID\n");
    result = GetButtonBounds(9999, &x, &y, &width, &height);
    if (result == -1) {
        printf("✓ 无效ID正确返回-1\n");
    } else {
        printf("✗ 无效ID应返回-1，实际返回: %d\n", result);
    }

    // 测试6: 负数坐标
    printf("\n测试6: 测试负数坐标\n");
    SetButtonBounds(button_id, -50, -50, 100, 100);
    result = GetButtonBounds(button_id, &x, &y, &width, &height);
    if (result == 0 && x == -50 && y == -50) {
        printf("✓ 负数坐标设置成功: (%d, %d)\n", x, y);
    } else {
        printf("✗ 负数坐标设置失败\n");
    }

    // 测试7: 连续多次设置
    printf("\n测试7: 连续多次设置\n");
    bool all_success = true;
    for (int i = 0; i < 10; i++) {
        int test_x = i * 10;
        int test_y = i * 20;
        int test_w = 100 + i * 5;
        int test_h = 50 + i * 3;
        
        SetButtonBounds(button_id, test_x, test_y, test_w, test_h);
        result = GetButtonBounds(button_id, &x, &y, &width, &height);
        
        if (result != 0 || x != test_x || y != test_y || width != test_w || height != test_h) {
            all_success = false;
            printf("✗ 第%d次设置失败\n", i + 1);
            break;
        }
    }
    if (all_success) {
        printf("✓ 连续10次设置全部成功\n");
    }

    printf("\n=== 所有测试完成 ===\n");

    // 清理
    DestroyWindow(hwnd);
    FreeLibrary(hDll);

    return 0;
}
