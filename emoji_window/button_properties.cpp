#include "emoji_window.h"

#include <algorithm>
#include <cstring>
#include <string>

std::wstring Utf8ToWide(const unsigned char* bytes, int len);

static std::string WideToUtf8(const std::wstring& wide) {
    if (wide.empty()) return "";

    int utf8_len = WideCharToMultiByte(
        CP_UTF8, 0, wide.c_str(), (int)wide.length(), nullptr, 0, nullptr, nullptr);
    if (utf8_len <= 0) return "";

    std::string result(utf8_len, 0);
    WideCharToMultiByte(
        CP_UTF8, 0, wide.c_str(), (int)wide.length(), &result[0], utf8_len, nullptr, nullptr);
    return result;
}

static EmojiButton* FindButton(int button_id, HWND* out_parent = nullptr) {
    return EW_FindButtonById(button_id, out_parent);
}

extern std::map<int, UINT32> g_button_text_colors;
extern std::map<int, UINT32> g_button_border_colors;
extern std::map<int, UINT32> g_button_hover_bg_colors;
extern std::map<int, UINT32> g_button_hover_border_colors;
extern std::map<int, UINT32> g_button_hover_text_colors;

static void RefreshButtonVisual(EmojiButton* button, HWND parent) {
    if (!button) return;

    if (button->hwnd && IsWindow(button->hwnd)) {
        HWND actual_parent = GetParent(button->hwnd);
        POINT child_pt = { button->x, button->y };
        if (parent && actual_parent && actual_parent != parent) {
            MapWindowPoints(parent, actual_parent, &child_pt, 1);
        }

        UINT flags = SWP_NOACTIVATE;
        flags |= button->visible ? SWP_SHOWWINDOW : SWP_HIDEWINDOW;
        SetWindowPos(
            button->hwnd,
            nullptr,
            child_pt.x,
            child_pt.y,
            (button->width > 0 ? button->width : 1),
            (button->height > 0 ? button->height : 1),
            flags
        );
        ApplyEmojiButtonWindowRegion(button);

        if (button->loading) {
            SetTimer(button->hwnd, 1, 120, nullptr);
        } else {
            KillTimer(button->hwnd, 1);
            button->loading_phase = 0;
        }

        RedrawWindow(button->hwnd, nullptr, nullptr, RDW_INVALIDATE | RDW_UPDATENOW | RDW_NOERASE);
        UpdateWindow(button->hwnd);
    }

    if (parent && IsWindow(parent)) {
        InvalidateRect(parent, nullptr, FALSE);
    }
}

extern "C" __declspec(dllexport) int __stdcall GetButtonText(int button_id, unsigned char* buffer, int buffer_size) {
    EmojiButton* button = FindButton(button_id);
    if (!button) return -1;

    std::string utf8_text = WideToUtf8(button->text);
    if (buffer == nullptr) {
        return (int)utf8_text.size();
    }
    if (buffer_size < (int)utf8_text.size()) return -1;

    memcpy(buffer, utf8_text.c_str(), utf8_text.size());
    return (int)utf8_text.size();
}

extern "C" __declspec(dllexport) void __stdcall SetButtonText(int button_id, const unsigned char* text, int text_len) {
    HWND parent = nullptr;
    EmojiButton* button = FindButton(button_id, &parent);
    if (!button || !parent) return;

    button->text = Utf8ToWide(text, text_len);
    RefreshButtonVisual(button, parent);
}

extern "C" __declspec(dllexport) int __stdcall GetButtonEmoji(int button_id, unsigned char* buffer, int buffer_size) {
    EmojiButton* button = FindButton(button_id);
    if (!button) return -1;

    std::string utf8_emoji = WideToUtf8(button->emoji);
    if (buffer == nullptr) {
        return (int)utf8_emoji.size();
    }
    if (buffer_size < (int)utf8_emoji.size()) return -1;

    memcpy(buffer, utf8_emoji.c_str(), utf8_emoji.size());
    return (int)utf8_emoji.size();
}

extern "C" __declspec(dllexport) void __stdcall SetButtonEmoji(int button_id, const unsigned char* emoji, int emoji_len) {
    HWND parent = nullptr;
    EmojiButton* button = FindButton(button_id, &parent);
    if (!button || !parent) return;

    button->emoji = Utf8ToWide(emoji, emoji_len);
    RefreshButtonVisual(button, parent);
}

extern "C" __declspec(dllexport) int __stdcall GetButtonBounds(int button_id, int* x, int* y, int* width, int* height) {
    EmojiButton* button = FindButton(button_id);
    if (!button) return -1;

    LogicalBoundsInfo logical = {};
    if (EW_GetButtonLogicalBounds(button_id, &logical)) {
        if (x) *x = logical.x;
        if (y) *y = logical.y;
        if (width) *width = logical.width;
        if (height) *height = logical.height;
        return 0;
    }

    if (x) *x = button->x;
    if (y) *y = button->y;
    if (width) *width = button->width;
    if (height) *height = button->height;
    return 0;
}

extern "C" __declspec(dllexport) void __stdcall SetButtonBounds(int button_id, int x, int y, int width, int height) {
    HWND parent = nullptr;
    EmojiButton* button = FindButton(button_id, &parent);
    if (!button || !parent) return;

    HWND logical_parent = (button->hwnd && IsWindow(button->hwnd)) ? GetParent(button->hwnd) : parent;
    if (!logical_parent || !IsWindow(logical_parent)) {
        logical_parent = parent;
    }

    RECT px_rect = EW_LogicalChildRectToPx(logical_parent, x, y, width, height, true);
    POINT host_pt = { px_rect.left, px_rect.top };
    if (logical_parent != parent) {
        MapWindowPoints(logical_parent, parent, &host_pt, 1);
    }

    EW_StoreButtonLogicalBounds(button_id, x, y, width, height);
    button->x = host_pt.x;
    button->y = host_pt.y;
    button->width = (std::max)(1, static_cast<int>(px_rect.right - px_rect.left));
    button->height = (std::max)(1, static_cast<int>(px_rect.bottom - px_rect.top));
    RefreshButtonVisual(button, parent);
}

extern "C" __declspec(dllexport) UINT32 __stdcall GetButtonBackgroundColor(int button_id) {
    EmojiButton* button = FindButton(button_id);
    if (!button) return 0;
    return button->bg_color;
}

extern "C" __declspec(dllexport) void __stdcall SetButtonBackgroundColor(int button_id, UINT32 color) {
    HWND parent = nullptr;
    EmojiButton* button = FindButton(button_id, &parent);
    if (!button || !parent) return;

    button->bg_color = color;
    RefreshButtonVisual(button, parent);
}

extern "C" __declspec(dllexport) UINT32 __stdcall GetButtonTextColor(int button_id) {
    auto it = g_button_text_colors.find(button_id);
    return it != g_button_text_colors.end() ? it->second : 0;
}

extern "C" __declspec(dllexport) void __stdcall SetButtonTextColor(int button_id, UINT32 color) {
    HWND parent = nullptr;
    EmojiButton* button = FindButton(button_id, &parent);
    if (!button || !parent) return;

    g_button_text_colors[button_id] = color;
    RefreshButtonVisual(button, parent);
}

extern "C" __declspec(dllexport) UINT32 __stdcall GetButtonBorderColor(int button_id) {
    auto it = g_button_border_colors.find(button_id);
    return it != g_button_border_colors.end() ? it->second : 0;
}

extern "C" __declspec(dllexport) void __stdcall SetButtonBorderColor(int button_id, UINT32 color) {
    HWND parent = nullptr;
    EmojiButton* button = FindButton(button_id, &parent);
    if (!button || !parent) return;

    g_button_border_colors[button_id] = color;
    RefreshButtonVisual(button, parent);
}

extern "C" __declspec(dllexport) int __stdcall GetButtonHoverColors(
    int button_id, UINT32* bg_color, UINT32* border_color, UINT32* text_color) {
    if (bg_color) {
        auto it = g_button_hover_bg_colors.find(button_id);
        *bg_color = it != g_button_hover_bg_colors.end() ? it->second : 0;
    }
    if (border_color) {
        auto it = g_button_hover_border_colors.find(button_id);
        *border_color = it != g_button_hover_border_colors.end() ? it->second : 0;
    }
    if (text_color) {
        auto it = g_button_hover_text_colors.find(button_id);
        *text_color = it != g_button_hover_text_colors.end() ? it->second : 0;
    }
    return 0;
}

extern "C" __declspec(dllexport) void __stdcall SetButtonHoverColors(
    int button_id, UINT32 bg_color, UINT32 border_color, UINT32 text_color) {
    HWND parent = nullptr;
    EmojiButton* button = FindButton(button_id, &parent);
    if (!button || !parent) return;

    g_button_hover_bg_colors[button_id] = bg_color;
    g_button_hover_border_colors[button_id] = border_color;
    g_button_hover_text_colors[button_id] = text_color;
    RefreshButtonVisual(button, parent);
}

extern "C" __declspec(dllexport) void __stdcall ResetButtonColorOverrides(int button_id) {
    HWND parent = nullptr;
    EmojiButton* button = FindButton(button_id, &parent);
    if (!button || !parent) return;

    g_button_text_colors.erase(button_id);
    g_button_border_colors.erase(button_id);
    g_button_hover_bg_colors.erase(button_id);
    g_button_hover_border_colors.erase(button_id);
    g_button_hover_text_colors.erase(button_id);
    RefreshButtonVisual(button, parent);
}

extern "C" __declspec(dllexport) void __stdcall SetButtonType(int button_id, int type) {
    HWND parent = nullptr;
    EmojiButton* button = FindButton(button_id, &parent);
    if (!button || !parent) return;
    if (type < BUTTON_TYPE_AUTO || type > BUTTON_TYPE_INFO) return;

    button->visual_type = type;
    RefreshButtonVisual(button, parent);
}

extern "C" __declspec(dllexport) int __stdcall GetButtonType(int button_id) {
    EmojiButton* button = FindButton(button_id);
    return button ? button->visual_type : BUTTON_TYPE_AUTO;
}

extern "C" __declspec(dllexport) void __stdcall SetButtonStyle(int button_id, int style) {
    HWND parent = nullptr;
    EmojiButton* button = FindButton(button_id, &parent);
    if (!button || !parent) return;
    if (style < BUTTON_STYLE_SOLID || style > BUTTON_STYLE_LINK) return;

    button->visual_style = style;
    RefreshButtonVisual(button, parent);
}

extern "C" __declspec(dllexport) int __stdcall GetButtonStyle(int button_id) {
    EmojiButton* button = FindButton(button_id);
    return button ? button->visual_style : BUTTON_STYLE_SOLID;
}

extern "C" __declspec(dllexport) void __stdcall SetButtonSize(int button_id, int size) {
    HWND parent = nullptr;
    EmojiButton* button = FindButton(button_id, &parent);
    if (!button || !parent) return;
    if (size < BUTTON_SIZE_LARGE || size > BUTTON_SIZE_SMALL) return;

    button->visual_size = size;
    RefreshButtonVisual(button, parent);
}

extern "C" __declspec(dllexport) int __stdcall GetButtonSize(int button_id) {
    EmojiButton* button = FindButton(button_id);
    return button ? button->visual_size : BUTTON_SIZE_DEFAULT;
}

extern "C" __declspec(dllexport) void __stdcall SetButtonRound(int button_id, BOOL round) {
    HWND parent = nullptr;
    EmojiButton* button = FindButton(button_id, &parent);
    if (!button || !parent) return;

    button->round = (round != 0);
    RefreshButtonVisual(button, parent);
}

extern "C" __declspec(dllexport) BOOL __stdcall GetButtonRound(int button_id) {
    EmojiButton* button = FindButton(button_id);
    return (button && button->round) ? TRUE : FALSE;
}

extern "C" __declspec(dllexport) void __stdcall SetButtonCircle(int button_id, BOOL circle) {
    HWND parent = nullptr;
    EmojiButton* button = FindButton(button_id, &parent);
    if (!button || !parent) return;

    button->circle = (circle != 0);
    RefreshButtonVisual(button, parent);
}

extern "C" __declspec(dllexport) BOOL __stdcall GetButtonCircle(int button_id) {
    EmojiButton* button = FindButton(button_id);
    return (button && button->circle) ? TRUE : FALSE;
}

extern "C" __declspec(dllexport) void __stdcall SetButtonLoading(int button_id, BOOL loading) {
    HWND parent = nullptr;
    EmojiButton* button = FindButton(button_id, &parent);
    if (!button || !parent) return;

    button->loading = (loading != 0);
    button->is_pressed = false;
    button->is_hovered = false;
    RefreshButtonVisual(button, parent);
}

extern "C" __declspec(dllexport) BOOL __stdcall GetButtonLoading(int button_id) {
    EmojiButton* button = FindButton(button_id);
    return (button && button->loading) ? TRUE : FALSE;
}

extern "C" __declspec(dllexport) BOOL __stdcall GetButtonVisible(int button_id) {
    EmojiButton* button = FindButton(button_id);
    if (!button) return FALSE;
    return button->visible ? TRUE : FALSE;
}

extern "C" __declspec(dllexport) void __stdcall ShowButton(int button_id, BOOL visible) {
    HWND parent = nullptr;
    EmojiButton* button = FindButton(button_id, &parent);
    if (!button || !parent) return;

    button->visible = (visible != 0);
    RefreshButtonVisual(button, parent);
}

extern "C" __declspec(dllexport) BOOL __stdcall GetButtonEnabled(int button_id) {
    EmojiButton* button = FindButton(button_id);
    if (!button) return FALSE;
    return button->enabled ? TRUE : FALSE;
}
