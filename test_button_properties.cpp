// 测试按钮属性命令
#include <windows.h>
#include <stdio.h>

// 函数声明
typedef HWND (__stdcall *CreateWindowBytesFunc)(const unsigned char*, int, int, int);
typedef int (__stdcall *CreateEmojiButtonBytesFunc)(HWND, const unsigned char*, int, const unsigned char*, int, int, int, int, int, UINT32);
typedef int (__stdcall *GetButtonTextFunc)(int, unsigned char*, int);
typedef void (__stdcall *SetButtonTextFunc)(int, const unsigned char*, int);
typedef int (__stdcall *RunMessageLoopFunc)();

int main() {
    // 加载DLL
    HMODULE hDll = LoadLibraryA("Release\\emoji_window.dll");
    if (!hDll) {
        printf("无法加载DLL\n");
        return 1;
    }

    // 获取函数指针
    auto create_window_bytes = (CreateWindowBytesFunc)GetProcAddress(hDll, "create_window_bytes");
    auto create_emoji_button_bytes = (CreateEmojiButtonBytesFunc)GetProcAddress(hDll, "create_emoji_button_bytes");
    auto GetButtonText = (GetButtonTextFunc)GetProcAddress(hDll, "GetButtonText");
    auto SetButtonText = (SetButtonTextFunc)GetProcAddress(hDll, "SetButtonText");
    auto run_message_loop = (RunMessageLoopFunc)GetProcAddress(hDll, "run_message_loop");

    if (!create_window_bytes || !create_emoji_button_bytes || !GetButtonText || !SetButtonText) {
        printf("无法获取函数指针\n");
        FreeLibrary(hDll);
        return 1;
    }

    // 创建窗口
    const unsigned char title[] = u8"按钮属性测试";
    HWND hwnd = create_window_bytes(title, sizeof(title) - 1, 600, 400);
    if (!hwnd) {
        printf("无法创建窗口\n");
        FreeLibrary(hDll);
        return 1;
    }

    // 创建按钮
    const unsigned char emoji[] = u8"✅";
    const unsigned char text[] = u8"测试按钮";
    int button_id = create_emoji_button_bytes(hwnd, emoji, sizeof(emoji) - 1, text, sizeof(text) - 1, 50, 50, 200, 40, 0xFF409EFF);
    
    printf("按钮ID: %d\n", button_id);

    // 测试GetButtonText - 第一次调用获取长度
    int len = GetButtonText(button_id, nullptr, 0);
    printf("按钮文本长度: %d\n", len);

    if (len > 0) {
        // 第二次调用获取内容
        unsigned char* buffer = new unsigned char[len + 1];
        int actual_len = GetButtonText(button_id, buffer, len);
        buffer[actual_len] = 0;
        printf("按钮文本: %s\n", buffer);
        delete[] buffer;
    }

    // 测试SetButtonText
    const unsigned char new_text[] = u8"新文本🎉";
    SetButtonText(button_id, new_text, sizeof(new_text) - 1);
    printf("已设置新文本\n");

    // 再次获取文本验证
    len = GetButtonText(button_id, nullptr, 0);
    if (len > 0) {
        unsigned char* buffer = new unsigned char[len + 1];
        int actual_len = GetButtonText(button_id, buffer, len);
        buffer[actual_len] = 0;
        printf("新按钮文本: %s\n", buffer);
        delete[] buffer;
    }

    printf("测试完成!按任意键退出...\n");
    getchar();

    FreeLibrary(hDll);
    return 0;
}
