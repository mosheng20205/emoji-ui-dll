// 按钮属性命令实现
#include "emoji_window.h"
#include <string>

// 前向声明 (C++函数)
std::wstring Utf8ToWide(const unsigned char* bytes, int len);

// Wide String to UTF-8 (辅助函数)
static std::string WideToUtf8(const std::wstring& wide) {
    if (wide.empty()) return "";
    
    int utf8_len = WideCharToMultiByte(CP_UTF8, 0, wide.c_str(), (int)wide.length(), nullptr, 0, nullptr, nullptr);
    if (utf8_len <= 0) return "";
    
    std::string result(utf8_len, 0);
    WideCharToMultiByte(CP_UTF8, 0, wide.c_str(), (int)wide.length(), &result[0], utf8_len, nullptr, nullptr);
    return result;
}

// 查找按钮的辅助函数
static EmojiButton* FindButton(int button_id, HWND* out_parent = nullptr) {
    extern std::map<HWND, WindowState*> g_windows;
    
    for (auto& win_pair : g_windows) {
        WindowState* state = win_pair.second;
        for (auto& button : state->buttons) {
            if (button.id == button_id) {
                if (out_parent) *out_parent = win_pair.first;
                return &button;
            }
        }
    }
    return nullptr;
}

// 获取按钮文本
extern "C" __declspec(dllexport) int __stdcall GetButtonText(int button_id, unsigned char* buffer, int buffer_size) {
    EmojiButton* button = FindButton(button_id);
    if (!button) return -1;
    
    std::string utf8_text = WideToUtf8(button->text);
    
    // 第一次调用：返回所需长度
    if (buffer == nullptr) {
        return (int)utf8_text.size();
    }
    
    // 第二次调用：复制数据
    if (buffer_size < (int)utf8_text.size()) return -1;
    memcpy(buffer, utf8_text.c_str(), utf8_text.size());
    return (int)utf8_text.size();
}

// 设置按钮文本
extern "C" __declspec(dllexport) void __stdcall SetButtonText(int button_id, const unsigned char* text, int text_len) {
    HWND parent = nullptr;
    EmojiButton* button = FindButton(button_id, &parent);
    if (!button || !parent) return;
    
    button->text = Utf8ToWide(text, text_len);
    
    // 触发重绘
    InvalidateRect(parent, nullptr, FALSE);
}

// 获取按钮Emoji
extern "C" __declspec(dllexport) int __stdcall GetButtonEmoji(int button_id, unsigned char* buffer, int buffer_size) {
    EmojiButton* button = FindButton(button_id);
    if (!button) return -1;
    
    std::string utf8_emoji = WideToUtf8(button->emoji);
    
    // 第一次调用：返回所需长度
    if (buffer == nullptr) {
        return (int)utf8_emoji.size();
    }
    
    // 第二次调用：复制数据
    if (buffer_size < (int)utf8_emoji.size()) return -1;
    memcpy(buffer, utf8_emoji.c_str(), utf8_emoji.size());
    return (int)utf8_emoji.size();
}

// 设置按钮Emoji
extern "C" __declspec(dllexport) void __stdcall SetButtonEmoji(int button_id, const unsigned char* emoji, int emoji_len) {
    HWND parent = nullptr;
    EmojiButton* button = FindButton(button_id, &parent);
    if (!button || !parent) return;
    
    button->emoji = Utf8ToWide(emoji, emoji_len);
    
    // 触发重绘
    InvalidateRect(parent, nullptr, FALSE);
}

// 获取按钮位置和大小
extern "C" __declspec(dllexport) int __stdcall GetButtonBounds(int button_id, int* x, int* y, int* width, int* height) {
    EmojiButton* button = FindButton(button_id);
    if (!button) return -1;
    
    if (x) *x = button->x;
    if (y) *y = button->y;
    if (width) *width = button->width;
    if (height) *height = button->height;
    
    return 0;
}

// 设置按钮位置和大小
extern "C" __declspec(dllexport) void __stdcall SetButtonBounds(int button_id, int x, int y, int width, int height) {
    HWND parent = nullptr;
    EmojiButton* button = FindButton(button_id, &parent);
    if (!button || !parent) return;
    
    button->x = x;
    button->y = y;
    button->width = width;
    button->height = height;
    
    // 触发重绘
    InvalidateRect(parent, nullptr, FALSE);
}

// 获取按钮背景色
extern "C" __declspec(dllexport) UINT32 __stdcall GetButtonBackgroundColor(int button_id) {
    EmojiButton* button = FindButton(button_id);
    if (!button) return 0;
    
    return button->bg_color;
}

// 设置按钮背景色
extern "C" __declspec(dllexport) void __stdcall SetButtonBackgroundColor(int button_id, UINT32 color) {
    HWND parent = nullptr;
    EmojiButton* button = FindButton(button_id, &parent);
    if (!button || !parent) return;
    
    button->bg_color = color;
    
    // 触发重绘
    InvalidateRect(parent, nullptr, FALSE);
}

// 获取按钮可视状态
extern "C" __declspec(dllexport) BOOL __stdcall GetButtonVisible(int button_id) {
    EmojiButton* button = FindButton(button_id);
    if (!button) return FALSE;
    
    return button->visible ? TRUE : FALSE;
}

// 显示/隐藏按钮
extern "C" __declspec(dllexport) void __stdcall ShowButton(int button_id, BOOL visible) {
    HWND parent = nullptr;
    EmojiButton* button = FindButton(button_id, &parent);
    if (!button || !parent) return;
    
    button->visible = (visible != 0);
    
    // 触发重绘
    InvalidateRect(parent, nullptr, FALSE);
}

// 获取按钮启用状态
extern "C" __declspec(dllexport) BOOL __stdcall GetButtonEnabled(int button_id) {
    EmojiButton* button = FindButton(button_id);
    if (!button) return FALSE;
    
    return button->enabled ? TRUE : FALSE;
}
