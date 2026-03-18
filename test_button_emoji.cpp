// 测试按钮Emoji属性命令
#include <windows.h>
#include <stdio.h>
#include <string.h>

// 函数声明
typedef HWND (__stdcall *CreateWindowBytesFunc)(const unsigned char*, int, int, int);
typedef int (__stdcall *CreateEmojiButtonBytesFunc)(HWND, const unsigned char*, int, const unsigned char*, int, int, int, int, int, UINT32);
typedef int (__stdcall *GetButtonEmojiFunc)(int, unsigned char*, int);
typedef void (__stdcall *SetButtonEmojiFunc)(int, const unsigned char*, int);
typedef int (__stdcall *GetButtonTextFunc)(int, unsigned char*, int);
typedef void (__stdcall *SetButtonTextFunc)(int, const unsigned char*, int);

int main() {
    // 加载DLL
    HMODULE hDll = LoadLibraryA("emoji_window.dll");
    if (!hDll) {
        printf("错误: 无法加载 emoji_window.dll\n");
        return 1;
    }

    // 获取函数指针
    auto CreateWindowBytes = (CreateWindowBytesFunc)GetProcAddress(hDll, "create_window_bytes");
    auto CreateEmojiButtonBytes = (CreateEmojiButtonBytesFunc)GetProcAddress(hDll, "create_emoji_button_bytes");
    auto GetButtonEmoji = (GetButtonEmojiFunc)GetProcAddress(hDll, "GetButtonEmoji");
    auto SetButtonEmoji = (SetButtonEmojiFunc)GetProcAddress(hDll, "SetButtonEmoji");
    auto GetButtonText = (GetButtonTextFunc)GetProcAddress(hDll, "GetButtonText");
    auto SetButtonText = (SetButtonTextFunc)GetProcAddress(hDll, "SetButtonText");

    if (!CreateWindowBytes || !CreateEmojiButtonBytes || !GetButtonEmoji || !SetButtonEmoji || !GetButtonText || !SetButtonText) {
        printf("错误: 无法获取函数指针\n");
        FreeLibrary(hDll);
        return 1;
    }

    printf("=== 按钮Emoji属性命令测试 ===\n\n");

    // 创建窗口
    const char* title = "测试窗口";
    HWND hwnd = CreateWindowBytes((const unsigned char*)title, strlen(title), 800, 600);
    if (!hwnd) {
        printf("错误: 无法创建窗口\n");
        FreeLibrary(hDll);
        return 1;
    }
    printf("✓ 窗口创建成功\n");

    // 创建按钮
    const char* emoji = "😀";
    const char* text = "测试按钮";
    int button_id = CreateEmojiButtonBytes(hwnd, (const unsigned char*)emoji, strlen(emoji),
                                           (const unsigned char*)text, strlen(text),
                                           100, 100, 200, 50, 0xFF4CAF50);
    if (button_id <= 0) {
        printf("错误: 无法创建按钮\n");
        FreeLibrary(hDll);
        return 1;
    }
    printf("✓ 按钮创建成功 (ID: %d)\n\n", button_id);

    // 测试1: 获取初始Emoji
    printf("测试1: 获取初始Emoji\n");
    int emoji_len = GetButtonEmoji(button_id, nullptr, 0);
    if (emoji_len < 0) {
        printf("✗ 获取Emoji长度失败\n");
    } else {
        printf("  Emoji长度: %d 字节\n", emoji_len);
        
        unsigned char* buffer = new unsigned char[emoji_len + 1];
        int actual_len = GetButtonEmoji(button_id, buffer, emoji_len);
        buffer[actual_len] = 0;
        
        if (actual_len == emoji_len && strcmp((char*)buffer, emoji) == 0) {
            printf("✓ Emoji内容正确: %s\n", buffer);
        } else {
            printf("✗ Emoji内容不匹配\n");
            printf("  期望: %s\n", emoji);
            printf("  实际: %s\n", buffer);
        }
        delete[] buffer;
    }

    // 测试2: 设置新的Emoji
    printf("\n测试2: 设置新的Emoji\n");
    const char* new_emoji = "🎉";
    SetButtonEmoji(button_id, (const unsigned char*)new_emoji, strlen(new_emoji));
    printf("  已设置新Emoji: %s\n", new_emoji);

    // 验证设置
    emoji_len = GetButtonEmoji(button_id, nullptr, 0);
    if (emoji_len > 0) {
        unsigned char* buffer = new unsigned char[emoji_len + 1];
        int actual_len = GetButtonEmoji(button_id, buffer, emoji_len);
        buffer[actual_len] = 0;
        
        if (strcmp((char*)buffer, new_emoji) == 0) {
            printf("✓ Emoji设置成功: %s\n", buffer);
        } else {
            printf("✗ Emoji设置失败\n");
            printf("  期望: %s\n", new_emoji);
            printf("  实际: %s\n", buffer);
        }
        delete[] buffer;
    }

    // 测试3: 设置空Emoji
    printf("\n测试3: 设置空Emoji\n");
    SetButtonEmoji(button_id, (const unsigned char*)"", 0);
    emoji_len = GetButtonEmoji(button_id, nullptr, 0);
    if (emoji_len == 0) {
        printf("✓ 空Emoji设置成功\n");
    } else {
        printf("✗ 空Emoji设置失败，长度: %d\n", emoji_len);
    }

    // 测试4: 设置包含多个Emoji的字符串
    printf("\n测试4: 设置多个Emoji\n");
    const char* multi_emoji = "😀🎉🚀";
    SetButtonEmoji(button_id, (const unsigned char*)multi_emoji, strlen(multi_emoji));
    emoji_len = GetButtonEmoji(button_id, nullptr, 0);
    if (emoji_len > 0) {
        unsigned char* buffer = new unsigned char[emoji_len + 1];
        int actual_len = GetButtonEmoji(button_id, buffer, emoji_len);
        buffer[actual_len] = 0;
        
        if (strcmp((char*)buffer, multi_emoji) == 0) {
            printf("✓ 多个Emoji设置成功: %s\n", buffer);
        } else {
            printf("✗ 多个Emoji设置失败\n");
        }
        delete[] buffer;
    }

    // 测试5: 测试无效按钮ID
    printf("\n测试5: 测试无效按钮ID\n");
    int invalid_len = GetButtonEmoji(9999, nullptr, 0);
    if (invalid_len == -1) {
        printf("✓ 无效ID正确返回-1\n");
    } else {
        printf("✗ 无效ID应返回-1，实际返回: %d\n", invalid_len);
    }

    // 测试6: 验证文本不受影响
    printf("\n测试6: 验证文本不受Emoji修改影响\n");
    int text_len = GetButtonText(button_id, nullptr, 0);
    if (text_len > 0) {
        unsigned char* buffer = new unsigned char[text_len + 1];
        int actual_len = GetButtonText(button_id, buffer, text_len);
        buffer[actual_len] = 0;
        
        if (strcmp((char*)buffer, text) == 0) {
            printf("✓ 文本未受影响: %s\n", buffer);
        } else {
            printf("✗ 文本被意外修改\n");
        }
        delete[] buffer;
    }

    // 测试7: 测试中文Emoji
    printf("\n测试7: 测试中文字符作为Emoji\n");
    const char* chinese_emoji = "中文";
    SetButtonEmoji(button_id, (const unsigned char*)chinese_emoji, strlen(chinese_emoji));
    emoji_len = GetButtonEmoji(button_id, nullptr, 0);
    if (emoji_len > 0) {
        unsigned char* buffer = new unsigned char[emoji_len + 1];
        int actual_len = GetButtonEmoji(button_id, buffer, emoji_len);
        buffer[actual_len] = 0;
        
        if (strcmp((char*)buffer, chinese_emoji) == 0) {
            printf("✓ 中文字符设置成功: %s\n", buffer);
        } else {
            printf("✗ 中文字符设置失败\n");
        }
        delete[] buffer;
    }

    printf("\n=== 测试完成 ===\n");

    // 清理
    FreeLibrary(hDll);
    return 0;
}
