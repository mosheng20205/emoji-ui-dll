#include "emoji_window.h"
#include <algorithm>

// Global variables
std::map<HWND, WindowState*> g_windows;
std::map<HWND, MsgBoxState*> g_msgboxes;
std::map<HWND, TabControlState*> g_tab_controls;
std::map<HWND, EditBoxState*> g_editboxes;
std::map<HWND, LabelState*> g_labels;
std::map<HWND, CheckBoxState*> g_checkboxes;
std::map<HWND, ProgressBarState*> g_progressbars;
std::map<HWND, PictureBoxState*> g_pictureboxes;
ButtonClickCallback g_button_callback = nullptr;
WindowResizeCallback g_window_resize_callback = nullptr;
WindowCloseCallback g_window_close_callback = nullptr;
ID2D1Factory* g_d2d_factory = nullptr;
IDWriteFactory* g_dwrite_factory = nullptr;
IWICImagingFactory* g_wic_factory = nullptr;  // WIC工厂
int g_next_control_id = 10000;  // 控件ID起始值
// 标记 run_message_loop 是否正在运行
// 只有在 DLL 自己的消息循环里才应该 PostQuitMessage，
// 从易语言 _窗口1_将被销毁 调用 destroy_window 时不能 PostQuitMessage，
// 否则易语言自己的消息循环会被误杀。
static bool g_own_message_loop_running = false;

// Forward declarations
LRESULT CALLBACK TabControlParentSubclassProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData);

// UTF-8 to Wide String
std::wstring Utf8ToWide(const unsigned char* bytes, int len) {
    if (len <= 0) return L"";

    int wide_len = MultiByteToWideChar(CP_UTF8, 0, (LPCCH)bytes, len, nullptr, 0);
    if (wide_len <= 0) return L"";

    std::wstring result(wide_len, 0);
    MultiByteToWideChar(CP_UTF8, 0, (LPCCH)bytes, len, &result[0], wide_len);
    return result;
}

// UINT32 color to D2D1_COLOR_F
D2D1_COLOR_F ColorFromUInt32(UINT32 color) {
    return D2D1::ColorF(
        ((color >> 16) & 0xFF) / 255.0f,
        ((color >> 8) & 0xFF) / 255.0f,
        (color & 0xFF) / 255.0f,
        ((color >> 24) & 0xFF) / 255.0f
    );
}

// Lighten color
UINT32 LightenColor(UINT32 color, float factor) {
    UINT32 a = (color >> 24) & 0xFF;
    UINT32 r = min(255, (UINT32)(((color >> 16) & 0xFF) * factor));
    UINT32 g = min(255, (UINT32)(((color >> 8) & 0xFF) * factor));
    UINT32 b = min(255, (UINT32)((color & 0xFF) * factor));
    return (a << 24) | (r << 16) | (g << 8) | b;
}

// Darken color
UINT32 DarkenColor(UINT32 color, float factor) {
    UINT32 a = (color >> 24) & 0xFF;
    UINT32 r = (UINT32)(((color >> 16) & 0xFF) * factor);
    UINT32 g = (UINT32)(((color >> 8) & 0xFF) * factor);
    UINT32 b = (UINT32)((color & 0xFF) * factor);
    return (a << 24) | (r << 16) | (g << 8) | b;
}

// Draw button (Element UI style - supports both main window buttons and message box buttons)
void DrawButton(ID2D1HwndRenderTarget* rt, IDWriteFactory* factory, const EmojiButton& button) {
    // ========== Calculate button color based on state (Element UI style) ==========
    UINT32 bg_color = button.bg_color;
    UINT32 bg_rgb = bg_color & 0x00FFFFFF;

    // Element UI button colors
    bool is_primary = (bg_rgb == 0x409EFF);  // Primary blue
    bool is_success = (bg_rgb == 0x67C23A);  // Success green
    bool is_warning = (bg_rgb == 0xE6A23C);  // Warning orange
    bool is_danger = (bg_rgb == 0xF56C6C);   // Danger red
    bool is_info = (bg_rgb == 0x909399);     // Info gray
    bool is_default = (bg_rgb == 0xFFFFFF || bg_rgb == 0xF2F2F7);  // Default white/light gray

    if (button.is_pressed) {
        // Pressed state: darker
        if (is_primary) bg_color = 0xFF3A8EE6;
        else if (is_success) bg_color = 0xFF5DAF34;
        else if (is_warning) bg_color = 0xFFCF9236;
        else if (is_danger) bg_color = 0xFFDD6161;
        else if (is_info) bg_color = 0xFF82848A;
        else if (is_default) bg_color = 0xFFECF5FF;
        else bg_color = DarkenColor(bg_color, 0.9f);
    } else if (button.is_hovered) {
        // Hover state: lighter
        if (is_primary) bg_color = 0xFF66B1FF;
        else if (is_success) bg_color = 0xFF85CE61;
        else if (is_warning) bg_color = 0xFFEBB563;
        else if (is_danger) bg_color = 0xFFF78989;
        else if (is_info) bg_color = 0xFFA6A9AD;
        else if (is_default) bg_color = 0xFFECF5FF;
        else bg_color = LightenColor(bg_color, 1.1f);
    }

    // ========== Draw button with Element UI style ==========
    ID2D1SolidColorBrush* brush = nullptr;
    rt->CreateSolidColorBrush(ColorFromUInt32(bg_color), &brush);

    D2D1_ROUNDED_RECT rect = D2D1::RoundedRect(
        D2D1::RectF(
            (FLOAT)button.x,
            (FLOAT)button.y,
            (FLOAT)(button.x + button.width),
            (FLOAT)(button.y + button.height)
        ),
        4.0f, 4.0f  // Element UI uses 4px border radius
    );
    rt->FillRoundedRectangle(rect, brush);
    brush->Release();

    // ========== Draw border for default button ==========
    if (is_default) {
        ID2D1SolidColorBrush* border_brush = nullptr;
        rt->CreateSolidColorBrush(D2D1::ColorF(0xDCDFE6, 1.0f), &border_brush);  // Element UI border color
        rt->DrawRoundedRectangle(rect, border_brush, 1.0f);
        border_brush->Release();
    }

    // ========== Determine text color ==========
    UINT32 text_color;
    if (is_default) {
        text_color = 0xFF606266;  // Element UI regular text color for default button
    } else {
        text_color = 0xFFFFFFFF;  // White text for colored buttons
    }

    ID2D1SolidColorBrush* text_brush = nullptr;
    rt->CreateSolidColorBrush(ColorFromUInt32(text_color), &text_brush);

    // ========== Calculate total content width for centering ==========
    float emoji_width = 0.0f;
    float text_width = 0.0f;
    float spacing = 6.0f;  // Space between emoji and text

    // Measure emoji width
    if (!button.emoji.empty()) {
        IDWriteTextFormat* emoji_format_measure = nullptr;
        factory->CreateTextFormat(
            L"Segoe UI Emoji",
            nullptr,
            DWRITE_FONT_WEIGHT_NORMAL,
            DWRITE_FONT_STYLE_NORMAL,
            DWRITE_FONT_STRETCH_NORMAL,
            18.0f,
            L"zh-CN",
            &emoji_format_measure
        );

        IDWriteTextLayout* emoji_layout = nullptr;
        factory->CreateTextLayout(
            button.emoji.c_str(),
            (UINT32)button.emoji.length(),
            emoji_format_measure,
            1000.0f,
            1000.0f,
            &emoji_layout
        );

        DWRITE_TEXT_METRICS emoji_metrics;
        emoji_layout->GetMetrics(&emoji_metrics);
        emoji_width = emoji_metrics.width;

        emoji_layout->Release();
        emoji_format_measure->Release();
    }

    // Measure text width
    if (!button.text.empty()) {
        IDWriteTextFormat* text_format_measure = nullptr;
        factory->CreateTextFormat(
            L"Microsoft YaHei UI",
            nullptr,
            DWRITE_FONT_WEIGHT_NORMAL,
            DWRITE_FONT_STYLE_NORMAL,
            DWRITE_FONT_STRETCH_NORMAL,
            14.0f,
            L"zh-CN",
            &text_format_measure
        );

        IDWriteTextLayout* text_layout = nullptr;
        factory->CreateTextLayout(
            button.text.c_str(),
            (UINT32)button.text.length(),
            text_format_measure,
            1000.0f,
            1000.0f,
            &text_layout
        );

        DWRITE_TEXT_METRICS text_metrics;
        text_layout->GetMetrics(&text_metrics);
        text_width = text_metrics.width;

        text_layout->Release();
        text_format_measure->Release();
    }

    // Calculate total content width and starting position for centering
    float total_content_width = emoji_width + (emoji_width > 0 && text_width > 0 ? spacing : 0) + text_width;
    float content_start_x = button.x + (button.width - total_content_width) / 2.0f;

    // ========== Draw emoji icon (larger size for consistency) ==========
    if (!button.emoji.empty()) {
        IDWriteTextFormat* emoji_format = nullptr;
        factory->CreateTextFormat(
            L"Segoe UI Emoji",  // Use emoji font for better rendering
            nullptr,
            DWRITE_FONT_WEIGHT_NORMAL,
            DWRITE_FONT_STYLE_NORMAL,
            DWRITE_FONT_STRETCH_NORMAL,
            18.0f,  // Larger size for emoji (18px instead of 14px)
            L"zh-CN",
            &emoji_format
        );

        emoji_format->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
        emoji_format->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);

        D2D1_RECT_F emoji_rect = D2D1::RectF(
            content_start_x,
            (FLOAT)button.y,
            content_start_x + emoji_width + 10.0f,  // Add some extra space
            (FLOAT)(button.y + button.height)
        );

        rt->DrawText(
            button.emoji.c_str(),
            (UINT32)button.emoji.length(),
            emoji_format,
            emoji_rect,
            text_brush,
            D2D1_DRAW_TEXT_OPTIONS_ENABLE_COLOR_FONT
        );

        emoji_format->Release();
    }

    // ========== Draw button text ==========
    if (!button.text.empty()) {
        IDWriteTextFormat* text_format = nullptr;
        factory->CreateTextFormat(
            L"Microsoft YaHei UI",  // Element UI 推荐字体
            nullptr,
            DWRITE_FONT_WEIGHT_NORMAL,  // 400 weight
            DWRITE_FONT_STYLE_NORMAL,
            DWRITE_FONT_STRETCH_NORMAL,
            14.0f,  // Element UI standard button font size
            L"zh-CN",
            &text_format
        );

        text_format->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
        text_format->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);

        float text_start_x = content_start_x + emoji_width + (emoji_width > 0 ? spacing : 0);
        D2D1_RECT_F text_rect = D2D1::RectF(
            text_start_x,
            (FLOAT)button.y,
            text_start_x + text_width + 10.0f,  // Add some extra space
            (FLOAT)(button.y + button.height)
        );

        rt->DrawText(
            button.text.c_str(),
            (UINT32)button.text.length(),
            text_format,
            text_rect,
            text_brush,
            D2D1_DRAW_TEXT_OPTIONS_NONE
        );

        text_format->Release();
    }

    text_brush->Release();
}

// Window procedure
LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
    auto it = g_windows.find(hwnd);
    if (it == g_windows.end() && msg != WM_CREATE) {
        return DefWindowProc(hwnd, msg, wparam, lparam);
    }

    WindowState* state = (it != g_windows.end()) ? it->second : nullptr;

    switch (msg) {
    case WM_PAINT: {
        if (state) {
            PAINTSTRUCT ps;
            BeginPaint(hwnd, &ps);

            state->render_target->BeginDraw();
            state->render_target->Clear(D2D1::ColorF(0xF5F5FA, 1.0f));

            for (const auto& button : state->buttons) {
                DrawButton(state->render_target, state->dwrite_factory, button);
            }

            state->render_target->EndDraw();
            EndPaint(hwnd, &ps);
        }
        return 0;
    }

    case WM_LBUTTONDOWN: {
        if (state) {
            int x = LOWORD(lparam);
            int y = HIWORD(lparam);

            for (auto& button : state->buttons) {
                if (button.ContainsPoint(x, y)) {
                    button.is_pressed = true;
                    InvalidateRect(hwnd, nullptr, FALSE);
                    break;
                }
            }
        }
        return 0;
    }

    case WM_LBUTTONUP: {
        if (state) {
            int x = LOWORD(lparam);
            int y = HIWORD(lparam);

            for (auto& button : state->buttons) {
                if (button.is_pressed && button.ContainsPoint(x, y)) {
                    button.is_pressed = false;

                    if (g_button_callback) {
                        g_button_callback(button.id);
                    }

                    InvalidateRect(hwnd, nullptr, FALSE);
                    break;
                }
                button.is_pressed = false;
            }
        }
        return 0;
    }

    case WM_MOUSEMOVE: {
        if (state) {
            int x = LOWORD(lparam);
            int y = HIWORD(lparam);
            bool needs_redraw = false;

            for (auto& button : state->buttons) {
                bool hovered = button.ContainsPoint(x, y);
                if (hovered != button.is_hovered) {
                    button.is_hovered = hovered;
                    needs_redraw = true;
                }
            }

            if (needs_redraw) {
                InvalidateRect(hwnd, nullptr, FALSE);
            }
        }
        return 0;
    }

    case WM_SIZE: {
        if (state) {
            UINT width = LOWORD(lparam);
            UINT height = HIWORD(lparam);
            state->render_target->Resize(D2D1::SizeU(width, height));

            if (g_window_resize_callback) {
                g_window_resize_callback(hwnd, (int)width, (int)height);
            }
        }
        return 0;
    }

    case WM_CTLCOLOREDIT:
    case WM_CTLCOLORSTATIC: {
        // ✅ 处理编辑框和标签的颜色消息
        // 这些消息会发送到父窗口，我们需要查找对应的子控件状态
        HWND hChild = (HWND)lparam;
        
        // 先查找编辑框
        auto edit_it = g_editboxes.find(hChild);
        if (edit_it != g_editboxes.end()) {
            EditBoxState* edit_state = edit_it->second;
            HDC hdc = (HDC)wparam;
            SetTextColor(hdc, RGB(
                (edit_state->fg_color >> 16) & 0xFF,
                (edit_state->fg_color >> 8) & 0xFF,
                edit_state->fg_color & 0xFF
            ));
            SetBkColor(hdc, RGB(
                (edit_state->bg_color >> 16) & 0xFF,
                (edit_state->bg_color >> 8) & 0xFF,
                edit_state->bg_color & 0xFF
            ));
            return (LRESULT)edit_state->bg_brush;
        }
        
        // 再查找标签
        auto label_it = g_labels.find(hChild);
        if (label_it != g_labels.end()) {
            LabelState* label_state = label_it->second;
            HDC hdc = (HDC)wparam;
            SetTextColor(hdc, RGB(
                (label_state->fg_color >> 16) & 0xFF,
                (label_state->fg_color >> 8) & 0xFF,
                label_state->fg_color & 0xFF
            ));
            SetBkColor(hdc, RGB(
                (label_state->bg_color >> 16) & 0xFF,
                (label_state->bg_color >> 8) & 0xFF,
                label_state->bg_color & 0xFF
            ));
            return (LRESULT)label_state->bg_brush;
        }
        
        break;
    }

    case WM_CLOSE: {
        DestroyWindow(hwnd);
        return 0;
    }

    case WM_DESTROY: {
        // 清理 WindowState 资源（render_target 等）
        auto destroy_it = g_windows.find(hwnd);
        if (destroy_it != g_windows.end()) {
            WindowState* destroy_state = destroy_it->second;
            if (destroy_state->render_target) {
                destroy_state->render_target->Release();
            }
            delete destroy_state;
            g_windows.erase(destroy_it);
        }

        // 注意：子编辑框和标签的清理由各自的 SubclassProc 在 WM_NCDESTROY 中处理，
        // 不在此处清理，避免 double-free（DestroyWindow 销毁子窗口时会触发 WM_NCDESTROY）

        // 顶层窗口关闭时：先触发用户回调，再决定是否退出消息循环
        LONG style = GetWindowLong(hwnd, GWL_STYLE);
        if (!(style & WS_CHILD)) {
            // 触发窗口关闭回调，通知易语言侧（如重置窗口句柄变量）
            if (g_window_close_callback) {
                g_window_close_callback(hwnd);
            }
            // 仅在 run_message_loop 运行时才 PostQuitMessage，
            // 避免从易语言 _窗口1_将被销毁 调用时误杀易语言消息循环
            if (g_own_message_loop_running) {
                PostQuitMessage(0);
            }
        }
        return 0;
    }
    }

    return DefWindowProc(hwnd, msg, wparam, lparam);
}

// Create window
HWND __stdcall create_window(const char* title, int width, int height) {
    static bool com_initialized = false;
    if (!com_initialized) {
        CoInitialize(nullptr);
        com_initialized = true;
    }

    if (!g_d2d_factory) {
        D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &g_d2d_factory);
    }

    if (!g_dwrite_factory) {
        DWriteCreateFactory(
            DWRITE_FACTORY_TYPE_SHARED,
            __uuidof(IDWriteFactory),
            reinterpret_cast<IUnknown**>(&g_dwrite_factory)
        );
    }

    static bool class_registered = false;
    const wchar_t* class_name = L"EmojiWindowClass";

    if (!class_registered) {
        WNDCLASSW wc = {};
        wc.lpfnWndProc = WindowProc;
        wc.hInstance = GetModuleHandle(nullptr);
        wc.lpszClassName = class_name;
        wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
        wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
        RegisterClassW(&wc);
        class_registered = true;
    }

    int title_len = MultiByteToWideChar(CP_ACP, 0, title, -1, nullptr, 0);
    std::wstring wtitle(title_len, 0);
    MultiByteToWideChar(CP_ACP, 0, title, -1, &wtitle[0], title_len);

    HWND hwnd = CreateWindowExW(
        0,
        class_name,
        wtitle.c_str(),
        WS_OVERLAPPEDWINDOW | WS_VISIBLE | WS_CLIPCHILDREN,  // WS_CLIPCHILDREN: 绘制时裁剪子控件区域
        CW_USEDEFAULT, CW_USEDEFAULT,
        width, height,
        nullptr, nullptr,
        GetModuleHandle(nullptr),
        nullptr
    );

    if (!hwnd) return nullptr;

    RECT rc;
    GetClientRect(hwnd, &rc);

    ID2D1HwndRenderTarget* render_target = nullptr;
    g_d2d_factory->CreateHwndRenderTarget(
        D2D1::RenderTargetProperties(),
        D2D1::HwndRenderTargetProperties(
            hwnd,
            D2D1::SizeU(rc.right - rc.left, rc.bottom - rc.top)
        ),
        &render_target
    );

    WindowState* state = new WindowState();
    state->hwnd = hwnd;
    state->render_target = render_target;
    state->dwrite_factory = g_dwrite_factory;
    g_windows[hwnd] = state;

    ShowWindow(hwnd, SW_SHOW);
    UpdateWindow(hwnd);

    return hwnd;
}

// Create emoji button (bytes version)
int __stdcall create_emoji_button_bytes(
    HWND parent,
    const unsigned char* emoji_bytes,
    int emoji_len,
    const unsigned char* text_bytes,
    int text_len,
    int x, int y, int width, int height,
    UINT32 bg_color
) {
    auto it = g_windows.find(parent);
    if (it == g_windows.end()) return 0;

    WindowState* state = it->second;

    EmojiButton button;
    button.id = (int)state->buttons.size() + 1000;
    button.emoji = Utf8ToWide(emoji_bytes, emoji_len);
    button.text = Utf8ToWide(text_bytes, text_len);
    button.x = x;
    button.y = y;
    button.width = width;
    button.height = height;
    button.bg_color = bg_color;
    button.is_hovered = false;
    button.is_pressed = false;

    state->buttons.push_back(button);

    InvalidateRect(parent, nullptr, FALSE);

    return button.id;
}

// Set button click callback
void __stdcall set_button_click_callback(ButtonClickCallback callback) {
    g_button_callback = callback;
}

// Set window resize callback
void __stdcall SetWindowResizeCallback(WindowResizeCallback callback) {
    g_window_resize_callback = callback;
}

// Set window close callback
// 当自绘顶层窗口被关闭（WM_DESTROY）时触发，易语言侧可在此重置句柄变量
void __stdcall SetWindowCloseCallback(WindowCloseCallback callback) {
    g_window_close_callback = callback;
}

// Run message loop
int __stdcall run_message_loop() {
    g_own_message_loop_running = true;
    MSG msg = {};
    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    g_own_message_loop_running = false;

    // 消息循环退出后（收到 WM_QUIT），清理所有残留的 DLL 窗口和控件
    // 这确保即使易语言 _将被销毁 事件未触发，DLL 窗口也能被正确销毁
    {
        std::vector<HWND> remaining_tabs;
        for (auto& pair : g_tab_controls) {
            remaining_tabs.push_back(pair.first);
        }
        for (HWND h : remaining_tabs) {
            auto it = g_tab_controls.find(h);
            if (it != g_tab_controls.end()) {
                TabControlState* state = it->second;
                RemoveWindowSubclass(state->hParent, TabControlParentSubclassProc, (UINT_PTR)h);
                for (auto& page : state->pages) {
                    if (page.hContentWindow && IsWindow(page.hContentWindow)) {
                        auto win_it = g_windows.find(page.hContentWindow);
                        if (win_it != g_windows.end()) {
                            WindowState* win_state = win_it->second;
                            if (win_state->render_target) {
                                win_state->render_target->Release();
                            }
                            delete win_state;
                            g_windows.erase(win_it);
                        }
                        DestroyWindow(page.hContentWindow);
                    }
                }
                delete state;
                g_tab_controls.erase(it);
                DestroyWindow(h);
            }
        }

        std::vector<HWND> remaining_windows;
        for (auto& pair : g_windows) {
            remaining_windows.push_back(pair.first);
        }
        for (HWND hwnd : remaining_windows) {
            auto it = g_windows.find(hwnd);
            if (it != g_windows.end()) {
                WindowState* state = it->second;
                if (state->render_target) {
                    state->render_target->Release();
                }
                delete state;
                g_windows.erase(it);
                DestroyWindow(hwnd);
            }
        }
    }

    return (int)msg.wParam;
}

// Destroy window
// 注意：此函数不调用 PostQuitMessage。
// PostQuitMessage 由 WM_DESTROY 负责，且仅在 run_message_loop 运行时才发送，
// 避免在易语言 _窗口1_将被销毁 事件中误杀易语言消息循环。
void __stdcall destroy_window(HWND hwnd) {
    if (!hwnd || !IsWindow(hwnd)) return;

    auto it = g_windows.find(hwnd);
    if (it != g_windows.end()) {
        WindowState* state = it->second;
        if (state->render_target) {
            state->render_target->Release();
        }
        delete state;
        g_windows.erase(it);
    }

    DestroyWindow(hwnd);
    // WM_DESTROY 已处理 PostQuitMessage，此处不再重复调用
}

// Set window icon
void __stdcall set_window_icon(HWND hwnd, const char* icon_path) {
    if (!hwnd || !icon_path) return;

    int path_len = MultiByteToWideChar(CP_ACP, 0, icon_path, -1, nullptr, 0);
    std::wstring wpath(path_len, 0);
    MultiByteToWideChar(CP_ACP, 0, icon_path, -1, &wpath[0], path_len);

    HICON hIcon = (HICON)LoadImageW(nullptr, wpath.c_str(), IMAGE_ICON, 0, 0, LR_LOADFROMFILE | LR_DEFAULTSIZE);
    if (hIcon) {
        SendMessage(hwnd, WM_SETICON, ICON_BIG, (LPARAM)hIcon);
        SendMessage(hwnd, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);
    }
}

// Set window title (UTF-8 input)
void __stdcall set_window_title(HWND hwnd, const char* title_utf8, int title_len) {
    if (!hwnd || !title_utf8 || title_len <= 0) return;

    // Convert UTF-8 to wide string
    int wlen = MultiByteToWideChar(CP_UTF8, 0, title_utf8, title_len, nullptr, 0);
    if (wlen <= 0) return;

    std::wstring wtitle(wlen, 0);
    MultiByteToWideChar(CP_UTF8, 0, title_utf8, title_len, &wtitle[0], wlen);

    // Set window title
    SetWindowTextW(hwnd, wtitle.c_str());
}

// Draw message box - Element UI Style
void DrawMsgBox(ID2D1HwndRenderTarget* rt, IDWriteFactory* factory, MsgBoxState* state) {
    RECT rc;
    GetClientRect(state->hwnd, &rc);
    float width = (float)(rc.right - rc.left);
    float height = (float)(rc.bottom - rc.top);

    // ========== 1. Background with subtle gradient (Element UI style) ==========
    ID2D1SolidColorBrush* bg_brush = nullptr;
    rt->CreateSolidColorBrush(D2D1::ColorF(0xFFFFFF, 1.0f), &bg_brush);  // Pure white

    D2D1_ROUNDED_RECT bg_rect = D2D1::RoundedRect(
        D2D1::RectF(0, 0, width, height),
        4.0f, 4.0f  // 4px rounded corners (Element UI standard)
    );
    rt->FillRoundedRectangle(bg_rect, bg_brush);
    bg_brush->Release();

    // ========== 2. Border (1px, #DCDFE6 - Element UI border color) ==========
    ID2D1SolidColorBrush* border_brush = nullptr;
    rt->CreateSolidColorBrush(D2D1::ColorF(0xDCDFE6, 1.0f), &border_brush);
    rt->DrawRoundedRectangle(bg_rect, border_brush, 1.0f);
    border_brush->Release();

    // ========== 3. Top accent bar (Element UI primary color) ==========
    ID2D1SolidColorBrush* accent_brush = nullptr;
    rt->CreateSolidColorBrush(D2D1::ColorF(0x409EFF, 1.0f), &accent_brush);  // Element UI primary blue
    
    D2D1_ROUNDED_RECT accent_rect = D2D1::RoundedRect(
        D2D1::RectF(0, 0, width, 3.0f),
        4.0f, 4.0f
    );
    rt->FillRoundedRectangle(accent_rect, accent_brush);
    accent_brush->Release();

    // ========== 4. Icon Emoji (28px, with colored background circle) ==========
    if (!state->icon_emoji.empty()) {
        // Draw colored circle background for icon
        ID2D1SolidColorBrush* icon_bg_brush = nullptr;
        rt->CreateSolidColorBrush(D2D1::ColorF(0xECF5FF, 1.0f), &icon_bg_brush);  // Light blue background
        
        D2D1_ELLIPSE icon_circle = D2D1::Ellipse(
            D2D1::Point2F(width / 2, 45.0f),
            24.0f, 24.0f
        );
        rt->FillEllipse(icon_circle, icon_bg_brush);
        icon_bg_brush->Release();

        // Draw icon emoji
        IDWriteTextFormat* icon_format = nullptr;
        factory->CreateTextFormat(
            L"Segoe UI Emoji",
            nullptr,
            DWRITE_FONT_WEIGHT_NORMAL,
            DWRITE_FONT_STYLE_NORMAL,
            DWRITE_FONT_STRETCH_NORMAL,
            28.0f,  // Larger icon
            L"zh-CN",
            &icon_format
        );
        icon_format->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
        icon_format->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);

        ID2D1SolidColorBrush* icon_brush = nullptr;
        rt->CreateSolidColorBrush(D2D1::ColorF(0x409EFF, 1.0f), &icon_brush);  // Primary blue

        D2D1_RECT_F icon_rect = D2D1::RectF(24, 20, width - 24, 70);
        rt->DrawText(
            state->icon_emoji.c_str(),
            (UINT32)state->icon_emoji.length(),
            icon_format,
            icon_rect,
            icon_brush,
            D2D1_DRAW_TEXT_OPTIONS_ENABLE_COLOR_FONT
        );

        icon_brush->Release();
        icon_format->Release();
    }

    // ========== 5. Title (16px, Medium 500, #303133 - Element UI text color) ==========
    IDWriteTextFormat* title_format = nullptr;
    factory->CreateTextFormat(
        L"Microsoft YaHei UI",  // 微软雅黑 UI (Element UI 推荐字体)
        nullptr,
        DWRITE_FONT_WEIGHT_MEDIUM,  // 500 weight
        DWRITE_FONT_STYLE_NORMAL,
        DWRITE_FONT_STRETCH_NORMAL,
        16.0f,
        L"zh-CN",
        &title_format
    );
    title_format->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
    title_format->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);

    ID2D1SolidColorBrush* title_brush = nullptr;
    rt->CreateSolidColorBrush(D2D1::ColorF(0x303133, 1.0f), &title_brush);  // Element UI primary text

    D2D1_RECT_F title_rect = D2D1::RectF(24, 75, width - 24, 105);
    rt->DrawText(
        state->title.c_str(),
        (UINT32)state->title.length(),
        title_format,
        title_rect,
        title_brush,
        D2D1_DRAW_TEXT_OPTIONS_NONE
    );

    title_brush->Release();
    title_format->Release();

    // ========== 6. Message (14px, Regular 400, #606266 - Element UI regular text) ==========
    IDWriteTextFormat* msg_format = nullptr;
    factory->CreateTextFormat(
        L"Microsoft YaHei UI",
        nullptr,
        DWRITE_FONT_WEIGHT_NORMAL,  // 400 weight
        DWRITE_FONT_STYLE_NORMAL,
        DWRITE_FONT_STRETCH_NORMAL,
        14.0f,
        L"zh-CN",
        &msg_format
    );
    msg_format->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
    msg_format->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
    msg_format->SetWordWrapping(DWRITE_WORD_WRAPPING_WRAP);
    msg_format->SetLineSpacing(DWRITE_LINE_SPACING_METHOD_UNIFORM, 22.0f, 14.0f);  // 1.57x line height

    ID2D1SolidColorBrush* msg_brush = nullptr;
    rt->CreateSolidColorBrush(D2D1::ColorF(0x606266, 1.0f), &msg_brush);  // Element UI regular text

    float msg_top = 115;
    float msg_bottom = (float)(state->ok_button.y - 20);
    D2D1_RECT_F msg_rect = D2D1::RectF(30, msg_top, width - 30, msg_bottom);
    rt->DrawText(
        state->message.c_str(),
        (UINT32)state->message.length(),
        msg_format,
        msg_rect,
        msg_brush,
        D2D1_DRAW_TEXT_OPTIONS_ENABLE_COLOR_FONT
    );

    msg_brush->Release();
    msg_format->Release();

    // ========== 7. Buttons (Element UI style) ==========
    DrawButton(rt, factory, state->ok_button);

    if (state->button_type == MSGBOX_OKCANCEL) {
        DrawButton(rt, factory, state->cancel_button);
    }
}

// Message box window procedure
LRESULT CALLBACK MsgBoxProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
    auto it = g_msgboxes.find(hwnd);
    MsgBoxState* state = (it != g_msgboxes.end()) ? it->second : nullptr;

    switch (msg) {
    case WM_PAINT: {
        if (state) {
            PAINTSTRUCT ps;
            BeginPaint(hwnd, &ps);

            state->render_target->BeginDraw();
            DrawMsgBox(state->render_target, state->dwrite_factory, state);
            state->render_target->EndDraw();

            EndPaint(hwnd, &ps);
        }
        return 0;
    }

    case WM_LBUTTONDOWN: {
        if (state) {
            int x = LOWORD(lparam);
            int y = HIWORD(lparam);

            if (state->ok_button.ContainsPoint(x, y)) {
                state->ok_button.is_pressed = true;
                InvalidateRect(hwnd, nullptr, FALSE);
            } else if (state->button_type == MSGBOX_OKCANCEL && state->cancel_button.ContainsPoint(x, y)) {
                state->cancel_button.is_pressed = true;
                InvalidateRect(hwnd, nullptr, FALSE);
            }
        }
        return 0;
    }

    case WM_LBUTTONUP: {
        if (state) {
            int x = LOWORD(lparam);
            int y = HIWORD(lparam);

            if (state->ok_button.is_pressed && state->ok_button.ContainsPoint(x, y)) {
                state->ok_button.is_pressed = false;
                CloseMessageBox(hwnd, true);
                return 0;
            }

            if (state->button_type == MSGBOX_OKCANCEL &&
                state->cancel_button.is_pressed && state->cancel_button.ContainsPoint(x, y)) {
                state->cancel_button.is_pressed = false;
                CloseMessageBox(hwnd, false);
                return 0;
            }

            state->ok_button.is_pressed = false;
            state->cancel_button.is_pressed = false;
            InvalidateRect(hwnd, nullptr, FALSE);
        }
        return 0;
    }

    case WM_MOUSEMOVE: {
        if (state) {
            int x = LOWORD(lparam);
            int y = HIWORD(lparam);
            bool needs_redraw = false;

            bool ok_hovered = state->ok_button.ContainsPoint(x, y);
            if (ok_hovered != state->ok_button.is_hovered) {
                state->ok_button.is_hovered = ok_hovered;
                needs_redraw = true;
            }

            if (state->button_type == MSGBOX_OKCANCEL) {
                bool cancel_hovered = state->cancel_button.ContainsPoint(x, y);
                if (cancel_hovered != state->cancel_button.is_hovered) {
                    state->cancel_button.is_hovered = cancel_hovered;
                    needs_redraw = true;
                }
            }

            if (needs_redraw) {
                InvalidateRect(hwnd, nullptr, FALSE);
            }
        }
        return 0;
    }

    case WM_KEYDOWN: {
        if (wparam == VK_RETURN || wparam == VK_SPACE) {
            if (state) {
                CloseMessageBox(hwnd, true);
            }
            return 0;
        }
        if (wparam == VK_ESCAPE) {
            if (state) {
                CloseMessageBox(hwnd, false);
            }
            return 0;
        }
        break;
    }

    case WM_CLOSE:
        if (state) {
            CloseMessageBox(hwnd, false);
        }
        return 0;
    }

    return DefWindowProc(hwnd, msg, wparam, lparam);
}

// Create message box window
HWND CreateMessageBoxWindow(HWND parent, const std::wstring& title, const std::wstring& message,
                            const std::wstring& icon, MsgBoxButtonType type, MessageBoxCallback callback) {
    static bool com_initialized = false;
    if (!com_initialized) {
        CoInitialize(nullptr);
        com_initialized = true;
    }

    if (!g_d2d_factory) {
        D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &g_d2d_factory);
    }
    if (!g_dwrite_factory) {
        DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory),
                            reinterpret_cast<IUnknown**>(&g_dwrite_factory));
    }

    static bool class_registered = false;
    const wchar_t* class_name = L"EmojiMessageBoxClass";

    if (!class_registered) {
        WNDCLASSEXW wc = {};
        wc.cbSize = sizeof(WNDCLASSEXW);
        wc.lpfnWndProc = MsgBoxProc;
        wc.hInstance = GetModuleHandle(nullptr);
        wc.lpszClassName = class_name;
        wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
        wc.hbrBackground = nullptr;
        wc.cbWndExtra = 0;
        wc.style = CS_DROPSHADOW;  // Soft shadow effect
        RegisterClassExW(&wc);
        class_registered = true;
    }

    // ========== Calculate text height for auto-sizing ==========
    int box_width = 400;   // Fixed width for readability

    // Create temporary text format to measure message height
    // ⚠️ CRITICAL: Use the SAME font as actual rendering (Microsoft YaHei UI)
    IDWriteTextFormat* temp_format = nullptr;
    g_dwrite_factory->CreateTextFormat(
        L"Microsoft YaHei UI",  // ✅ Same as DrawMsgBox
        nullptr,
        DWRITE_FONT_WEIGHT_NORMAL,
        DWRITE_FONT_STYLE_NORMAL,
        DWRITE_FONT_STRETCH_NORMAL,
        14.0f,
        L"zh-CN",
        &temp_format
    );
    temp_format->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
    temp_format->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
    temp_format->SetWordWrapping(DWRITE_WORD_WRAPPING_WRAP);
    temp_format->SetLineSpacing(DWRITE_LINE_SPACING_METHOD_UNIFORM, 22.0f, 14.0f);  // ✅ Same as DrawMsgBox (1.57x line height)

    // Create text layout to measure actual text height
    IDWriteTextLayout* text_layout = nullptr;
    g_dwrite_factory->CreateTextLayout(
        message.c_str(),
        (UINT32)message.length(),
        temp_format,
        (float)(box_width - 60),  // ✅ Text area width: 30px padding on each side (same as DrawMsgBox)
        2000.0f,  // Max height for measurement (increased for long messages)
        &text_layout
    );

    DWRITE_TEXT_METRICS text_metrics;
    text_layout->GetMetrics(&text_metrics);
    float measured_text_height = text_metrics.height;

    text_layout->Release();
    temp_format->Release();

    // Calculate window height based on content
    // Layout: Top bar(3px) + Icon area(70px: 20px top + 50px icon) + Title(30px) + Message top spacing(10px) + Message(variable) + Message bottom spacing(20px) + Button(40px) + Bottom padding(15px)
    float content_height = 3 + 70 + 30 + 10 + measured_text_height + 20 + 40 + 15;
    int min_height = (type == MSGBOX_OKCANCEL) ? 240 : 220;  // Minimum height
    int box_height = (int)max(content_height, (float)min_height);

    // Calculate button Y position based on actual window height
    int btn_y = box_height - 55;  // 40px button + 15px bottom padding

    RECT parent_rect = {0};
    if (parent && IsWindow(parent)) {
        GetWindowRect(parent, &parent_rect);
    } else {
        parent_rect.left = GetSystemMetrics(SM_CXSCREEN) / 2 - 200;
        parent_rect.top = GetSystemMetrics(SM_CYSCREEN) / 2 - 150;
        parent_rect.right = parent_rect.left + 400;
        parent_rect.bottom = parent_rect.top + 300;
    }

    int x = parent_rect.left + (parent_rect.right - parent_rect.left - box_width) / 2;
    int y = parent_rect.top + (parent_rect.bottom - parent_rect.top - box_height) / 2;

    HWND hwnd = CreateWindowExW(
        WS_EX_TOPMOST | WS_EX_LAYERED | WS_EX_TOOLWINDOW,
        class_name,
        nullptr,
        WS_POPUP,
        x, y, box_width, box_height,
        parent, nullptr,
        GetModuleHandle(nullptr),
        nullptr
    );

    if (!hwnd) return nullptr;

    // Rounded corners (10px)
    HRGN hRgn = CreateRoundRectRgn(0, 0, box_width + 1, box_height + 1, 20, 20);
    SetWindowRgn(hwnd, hRgn, TRUE);

    // Set window opacity for soft appearance
    SetLayeredWindowAttributes(hwnd, 0, 250, LWA_ALPHA);

    RECT rc;
    GetClientRect(hwnd, &rc);

    ID2D1HwndRenderTarget* render_target = nullptr;
    g_d2d_factory->CreateHwndRenderTarget(
        D2D1::RenderTargetProperties(),
        D2D1::HwndRenderTargetProperties(hwnd, D2D1::SizeU(rc.right - rc.left, rc.bottom - rc.top)),
        &render_target
    );

    MsgBoxState* state = new MsgBoxState();
    state->hwnd = hwnd;
    state->parent_hwnd = parent;
    state->render_target = render_target;
    state->dwrite_factory = g_dwrite_factory;
    state->title = title;
    state->message = message;
    state->icon_emoji = icon.empty() ? L"" : icon;  // No default icon
    state->button_type = type;
    state->callback = callback;
    state->result = false;

    // ========== Element UI style buttons with Emoji ==========
    // OK button - Primary button (Element UI #409EFF blue, white text, with emoji)
    state->ok_button.id = 1;
    state->ok_button.emoji = L"\u2713";  // ✓ checkmark emoji
    state->ok_button.text = L"\u786E\u5B9A";  // "确定"
    state->ok_button.bg_color = 0xFF409EFF;  // Element UI primary blue
    state->ok_button.is_hovered = false;
    state->ok_button.is_pressed = false;

    if (type == MSGBOX_OKCANCEL) {
        // Two buttons layout
        int btn_width = 140;
        int btn_height = 40;
        int btn_spacing = 12;  // 12px spacing between buttons

        // Cancel button on left (default style, with emoji)
        state->cancel_button.id = 2;
        state->cancel_button.emoji = L"\u2717";  // ✗ cross emoji
        state->cancel_button.text = L"\u53D6\u6D88";  // "取消"
        state->cancel_button.bg_color = 0xFFFFFFFF;  // Element UI default button (white background)
        state->cancel_button.x = (box_width / 2) - btn_width - (btn_spacing / 2);
        state->cancel_button.y = btn_y;  // Use calculated position
        state->cancel_button.width = btn_width;
        state->cancel_button.height = btn_height;
        state->cancel_button.is_hovered = false;
        state->cancel_button.is_pressed = false;

        // OK button on right (primary style)
        state->ok_button.x = (box_width / 2) + (btn_spacing / 2);
        state->ok_button.y = btn_y;  // Use calculated position
        state->ok_button.width = btn_width;
        state->ok_button.height = btn_height;
    } else {
        // Single button layout (centered)
        int btn_width = 160;
        int btn_height = 40;
        state->ok_button.x = (box_width - btn_width) / 2;
        state->ok_button.y = btn_y;  // Use calculated position
        state->ok_button.width = btn_width;
        state->ok_button.height = btn_height;
    }

    g_msgboxes[hwnd] = state;

    if (parent && IsWindow(parent)) {
        EnableWindow(parent, FALSE);
    }

    ShowWindow(hwnd, SW_SHOW);
    UpdateWindow(hwnd);
    SetFocus(hwnd);

    return hwnd;
}

// Close message box
void CloseMessageBox(HWND hwnd, bool result) {
    auto it = g_msgboxes.find(hwnd);
    if (it == g_msgboxes.end()) return;

    MsgBoxState* state = it->second;
    MessageBoxCallback callback = state->callback;
    HWND parent = state->parent_hwnd;

    state->result = result;

    if (parent && IsWindow(parent)) {
        EnableWindow(parent, TRUE);
        SetForegroundWindow(parent);
    }

    if (state->render_target) {
        state->render_target->Release();
    }
    delete state;
    g_msgboxes.erase(it);

    DestroyWindow(hwnd);

    if (callback) {
        callback(result ? 1 : 0);
    }
}

// Show message box (OK button only) - NON-BLOCKING
void __stdcall show_message_box_bytes(
    HWND parent,
    const unsigned char* title_bytes,
    int title_len,
    const unsigned char* message_bytes,
    int message_len,
    const unsigned char* icon_bytes,
    int icon_len
) {
    std::wstring title = Utf8ToWide(title_bytes, title_len);
    std::wstring message = Utf8ToWide(message_bytes, message_len);
    std::wstring icon = Utf8ToWide(icon_bytes, icon_len);

    // Create window and show it (non-blocking)
    HWND hwnd = CreateMessageBoxWindow(parent, title, message, icon, MSGBOX_OK, nullptr);
    if (!hwnd) return;

    // Show window without blocking
    ShowWindow(hwnd, SW_SHOW);
    UpdateWindow(hwnd);

    // No message loop here - let the main window's message loop handle it
}

// Show confirm box (with callback) - NON-BLOCKING
void __stdcall show_confirm_box_bytes(
    HWND parent,
    const unsigned char* title_bytes,
    int title_len,
    const unsigned char* message_bytes,
    int message_len,
    const unsigned char* icon_bytes,
    int icon_len,
    MessageBoxCallback callback
) {
    std::wstring title = Utf8ToWide(title_bytes, title_len);
    std::wstring message = Utf8ToWide(message_bytes, message_len);
    std::wstring icon = Utf8ToWide(icon_bytes, icon_len);

    // Create window and show it (non-blocking)
    HWND hwnd = CreateMessageBoxWindow(parent, title, message, icon, MSGBOX_OKCANCEL, callback);
    if (!hwnd) {
        if (callback) callback(0);
        return;
    }

    // Show window without blocking
    ShowWindow(hwnd, SW_SHOW);
    UpdateWindow(hwnd);

    // No message loop here - let the main window's message loop handle it
}


// ========== TabControl 实现 ==========

// 更新 Tab 布局（显示/隐藏内容窗口）
void UpdateTabLayout(TabControlState* state) {
    if (!state || !state->hTabControl) return;

    // 获取 Tab Control 的显示区域（相对于 TabControl）
    RECT rcTab;
    GetClientRect(state->hTabControl, &rcTab);
    TabCtrl_AdjustRect(state->hTabControl, FALSE, &rcTab);

    // 将坐标转换为相对于父窗口的坐标
    POINT pt = { rcTab.left, rcTab.top };
    MapWindowPoints(state->hTabControl, state->hParent, &pt, 1);
    rcTab.left = pt.x;
    rcTab.top = pt.y;

    POINT pt2 = { rcTab.right, rcTab.bottom };
    MapWindowPoints(state->hTabControl, state->hParent, &pt2, 1);
    rcTab.right = pt2.x;
    rcTab.bottom = pt2.y;

    // 遍历所有 Tab 页，更新可见性和位置
    for (size_t i = 0; i < state->pages.size(); i++) {
        TabPageInfo& page = state->pages[i];
        if (page.hContentWindow && IsWindow(page.hContentWindow)) {
            if ((int)i == state->currentIndex) {
                // 显示当前选中的页面
                SetWindowPos(
                    page.hContentWindow,
                    HWND_TOP,
                    rcTab.left,
                    rcTab.top,
                    rcTab.right - rcTab.left,
                    rcTab.bottom - rcTab.top,
                    SWP_SHOWWINDOW
                );
                page.visible = true;
            } else {
                // 隐藏其他页面（但不销毁，保留内容）
                ShowWindow(page.hContentWindow, SW_HIDE);
                page.visible = false;
            }
        }
    }
}

// 父窗口子类化过程（处理 TabControl 的通知消息）
LRESULT CALLBACK TabControlParentSubclassProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData) {
    // dwRefData 存储的是 TabControl 的句柄
    HWND hTabControl = (HWND)dwRefData;

    auto it = g_tab_controls.find(hTabControl);
    if (it == g_tab_controls.end()) {
        return DefSubclassProc(hwnd, msg, wparam, lparam);
    }

    TabControlState* state = it->second;

    switch (msg) {
    case WM_NOTIFY: {
        NMHDR* pnmhdr = (NMHDR*)lparam;

        // 检查是否是来自这个 TabControl 的通知
        if (pnmhdr->hwndFrom == hTabControl) {
            if (pnmhdr->code == TCN_SELCHANGE) {
                // Tab 切换事件
                int newIndex = TabCtrl_GetCurSel(hTabControl);
                if (newIndex >= 0 && newIndex < (int)state->pages.size()) {
                    state->currentIndex = newIndex;
                    UpdateTabLayout(state);

                    // 触发回调
                    if (state->callback) {
                        state->callback(hTabControl, newIndex);
                    }
                }
            }
        }
        break;
    }

    // TCS_OWNERDRAWFIXED: 父窗口负责绘制每个 Tab 标签
    // 用 ID2D1DCRenderTarget 渲染彩色 Emoji（等同于按钮的 D2D 渲染路径）
    case WM_DRAWITEM: {
        DRAWITEMSTRUCT* dis = (DRAWITEMSTRUCT*)lparam;
        // 只处理属于此 TabControl 的绘制请求
        if (dis->CtlType != ODT_TAB || dis->hwndItem != hTabControl) break;

        int tabIdx = (int)dis->itemID;
        // 若标题尚未加入 pages（极少数时序问题），跳过，让系统默认处理
        if (tabIdx < 0 || tabIdx >= (int)state->pages.size()) break;

        const std::wstring& title = state->pages[tabIdx].title;
        bool isSelected = (dis->itemState & ODS_SELECTED) != 0;

        int w = dis->rcItem.right  - dis->rcItem.left;
        int h = dis->rcItem.bottom - dis->rcItem.top;
        if (w <= 0 || h <= 0) break;

        // ── 1. 创建内存 DC + 32 位 DIBSection（D2D Premultiplied Alpha 所需）──
        HDC memDC = CreateCompatibleDC(dis->hDC);
        if (!memDC) break;

        BITMAPINFO bmi        = {};
        bmi.bmiHeader.biSize        = sizeof(BITMAPINFOHEADER);
        bmi.bmiHeader.biWidth       = w;
        bmi.bmiHeader.biHeight      = -h;   // top-down
        bmi.bmiHeader.biPlanes      = 1;
        bmi.bmiHeader.biBitCount    = 32;
        bmi.bmiHeader.biCompression = BI_RGB;

        void*   pvBits  = nullptr;
        HBITMAP hBmp    = CreateDIBSection(dis->hDC, &bmi, DIB_RGB_COLORS, &pvBits, nullptr, 0);
        if (!hBmp) { DeleteDC(memDC); break; }
        HBITMAP hOldBmp = (HBITMAP)SelectObject(memDC, hBmp);

        // ── 2. 创建 D2D DCRenderTarget（Premultiplied Alpha = 彩色 Emoji 渲染所需）──
        D2D1_RENDER_TARGET_PROPERTIES rtProps = D2D1::RenderTargetProperties(
            D2D1_RENDER_TARGET_TYPE_DEFAULT,
            D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED)
        );
        ID2D1DCRenderTarget* dcRT = nullptr;
        if (SUCCEEDED(g_d2d_factory->CreateDCRenderTarget(&rtProps, &dcRT))) {
            RECT rcBind = { 0, 0, w, h };
            dcRT->BindDC(memDC, &rcBind);
            // Grayscale 抗锯齿：与 Premultiplied Alpha 兼容，且支持彩色字体
            dcRT->SetTextAntialiasMode(D2D1_TEXT_ANTIALIAS_MODE_GRAYSCALE);

            dcRT->BeginDraw();

            // ── 3. 背景填充 ──
            // 选中: 白色；未选中: Element UI 浅灰 #F5F7FA
            D2D1_COLOR_F bgColor = isSelected
                ? D2D1::ColorF(1.00f, 1.00f, 1.00f, 1.0f)
                : D2D1::ColorF(0.961f, 0.969f, 0.980f, 1.0f);

            ID2D1SolidColorBrush* bgBrush = nullptr;
            dcRT->CreateSolidColorBrush(bgColor, &bgBrush);
            D2D1_RECT_F fillRect = D2D1::RectF(0.0f, 0.0f, (FLOAT)w, (FLOAT)h);
            dcRT->FillRectangle(fillRect, bgBrush);
            bgBrush->Release();

            // ── 4. 文字（含彩色 Emoji） ──
            if (!title.empty() && g_dwrite_factory) {
                IDWriteTextFormat* fmt = nullptr;
                g_dwrite_factory->CreateTextFormat(
                    L"Segoe UI Emoji",   // 支持彩色 Emoji；中文字符由 DWrite 自动 Fallback
                    nullptr,
                    DWRITE_FONT_WEIGHT_NORMAL,
                    DWRITE_FONT_STYLE_NORMAL,
                    DWRITE_FONT_STRETCH_NORMAL,
                    13.0f,
                    L"zh-CN",
                    &fmt
                );
                if (fmt) {
                    fmt->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
                    fmt->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);

                    // 选中: Element UI 主蓝 #409EFF；未选中: 常规文字色 #606266
                    D2D1_COLOR_F textColor = isSelected
                        ? D2D1::ColorF(0x409EFF)
                        : D2D1::ColorF(0x606266);

                    ID2D1SolidColorBrush* textBrush = nullptr;
                    dcRT->CreateSolidColorBrush(textColor, &textBrush);

                    // 文字区域：底部留 2px 给选中指示条
                    D2D1_RECT_F textRect = D2D1::RectF(
                        2.0f, 0.0f,
                        (FLOAT)(w - 2), (FLOAT)(h - (isSelected ? 2.0f : 0.0f))
                    );

                    // D2D1_DRAW_TEXT_OPTIONS_ENABLE_COLOR_FONT: 关键 —— 彩色 Emoji 渲染
                    dcRT->DrawText(
                        title.c_str(),
                        (UINT32)title.length(),
                        fmt,
                        textRect,
                        textBrush,
                        D2D1_DRAW_TEXT_OPTIONS_ENABLE_COLOR_FONT
                    );

                    textBrush->Release();
                    fmt->Release();
                }
            }

            // ── 5. 选中指示条（底部 2px 蓝线，Element UI 风格）──
            if (isSelected) {
                ID2D1SolidColorBrush* lineBrush = nullptr;
                dcRT->CreateSolidColorBrush(D2D1::ColorF(0x409EFF), &lineBrush);
                D2D1_RECT_F lineRect = D2D1::RectF(0.0f, (FLOAT)(h - 2), (FLOAT)w, (FLOAT)h);
                dcRT->FillRectangle(lineRect, lineBrush);
                lineBrush->Release();
            }

            dcRT->EndDraw();
            dcRT->Release();
        }

        // ── 6. 将渲染结果 Blit 到 TabControl 的 HDC ──
        BitBlt(dis->hDC, dis->rcItem.left, dis->rcItem.top, w, h, memDC, 0, 0, SRCCOPY);

        // ── 7. 清理资源 ──
        SelectObject(memDC, hOldBmp);
        DeleteObject(hBmp);
        DeleteDC(memDC);

        return TRUE;
    }

    case WM_SIZE: {
        // 窗口大小改变时，更新布局
        UpdateTabLayout(state);
        break;
    }

    case WM_DESTROY: {
        // 清理子类化
        RemoveWindowSubclass(hwnd, TabControlParentSubclassProc, uIdSubclass);
        break;
    }
    }

    return DefSubclassProc(hwnd, msg, wparam, lparam);
}

// 创建 TabControl
HWND __stdcall CreateTabControl(HWND hParent, int x, int y, int width, int height) {
    // 初始化 Common Controls
    static bool comctl_initialized = false;
    if (!comctl_initialized) {
        INITCOMMONCONTROLSEX icex;
        icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
        icex.dwICC = ICC_TAB_CLASSES;
        InitCommonControlsEx(&icex);
        comctl_initialized = true;
    }

    // 创建 Tab Control（TCS_OWNERDRAWFIXED：父窗口自绘标签，实现彩色 Emoji 渲染）
    HWND hTabControl = CreateWindowExW(
        0,
        WC_TABCONTROLW,
        L"",
        WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | TCS_TABS | TCS_FOCUSNEVER
        | TCS_OWNERDRAWFIXED | TCS_FIXEDWIDTH,
        x, y, width, height,
        hParent,
        nullptr,
        GetModuleHandle(nullptr),
        nullptr
    );

    if (!hTabControl) return nullptr;

    // 设置固定标签尺寸（宽=120px 能容纳 Emoji + 中文；高=34px 给 Emoji 足够空间）
    SendMessage(hTabControl, TCM_SETITEMSIZE, 0, MAKELPARAM(120, 34));

    // 启用现代视觉样式（XP/Vista+ 风格，仅影响标签边框；标签内容由 WM_DRAWITEM 自绘）
    SetWindowTheme(hTabControl, L"Explorer", nullptr);

    // 设置字体（使用 Segoe UI 以获得现代外观）
    HFONT hFont = CreateFontW(
        -14,                        // 字体高度
        0,                          // 字体宽度
        0,                          // 倾斜角度
        0,                          // 基线角度
        FW_NORMAL,                  // 字体粗细
        FALSE,                      // 斜体
        FALSE,                      // 下划线
        FALSE,                      // 删除线
        DEFAULT_CHARSET,            // 字符集
        OUT_DEFAULT_PRECIS,         // 输出精度
        CLIP_DEFAULT_PRECIS,        // 裁剪精度
        CLEARTYPE_QUALITY,          // 输出质量（ClearType 抗锯齿）
        DEFAULT_PITCH | FF_DONTCARE,// 字体间距和族
        L"Segoe UI"                 // 字体名称
    );
    SendMessage(hTabControl, WM_SETFONT, (WPARAM)hFont, TRUE);

    // 创建状态对象
    TabControlState* state = new TabControlState();
    state->hTabControl = hTabControl;
    state->hParent = hParent;
    state->currentIndex = -1;
    state->callback = nullptr;

    // 保存到全局映射表
    g_tab_controls[hTabControl] = state;

    // 子类化父窗口以接收 WM_NOTIFY 消息
    // 注意：WM_NOTIFY 消息是发送给父窗口的，不是发送给 TabControl 的
    SetWindowSubclass(hParent, TabControlParentSubclassProc, (UINT_PTR)hTabControl, (DWORD_PTR)hTabControl);

    return hTabControl;
}

// 添加 Tab 页
int __stdcall AddTabItem(HWND hTabControl, const unsigned char* title_bytes, int title_len, HWND hContentWindow) {
    auto it = g_tab_controls.find(hTabControl);
    if (it == g_tab_controls.end()) return -1;

    TabControlState* state = it->second;

    // 转换标题
    std::wstring title = Utf8ToWide(title_bytes, title_len);

    // 如果没有提供内容窗口，则创建一个支持 Emoji 按钮的窗口
    if (!hContentWindow || !IsWindow(hContentWindow)) {
        // 确保 D2D 和 DWrite 工厂已初始化
        if (!g_d2d_factory) {
            D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &g_d2d_factory);
        }
        if (!g_dwrite_factory) {
            DWriteCreateFactory(
                DWRITE_FACTORY_TYPE_SHARED,
                __uuidof(IDWriteFactory),
                reinterpret_cast<IUnknown**>(&g_dwrite_factory)
            );
        }

        // 注册窗口类（如果还没有注册）
        static bool content_class_registered = false;
        const wchar_t* content_class_name = L"EmojiTabContentClass";

        if (!content_class_registered) {
            WNDCLASSW wc = {};
            wc.lpfnWndProc = WindowProc;  // 使用与主窗口相同的窗口过程
            wc.hInstance = GetModuleHandle(nullptr);
            wc.lpszClassName = content_class_name;
            wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
            wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);  // ✅ 使用标准背景色，避免闪烁
            RegisterClassW(&wc);
            content_class_registered = true;
        }

        // 创建内容窗口（使用 EmojiWindowClass 以支持按钮）
        hContentWindow = CreateWindowExW(
            0,
            content_class_name,
            L"",
            WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,  // ✅ 添加 WS_CLIPCHILDREN 避免子控件闪烁
            0, 0, 0, 0,
            state->hParent,
            nullptr,
            GetModuleHandle(nullptr),
            nullptr
        );

        if (!hContentWindow) return -1;

        // 为内容窗口创建 D2D 渲染目标
        RECT rc;
        GetClientRect(hContentWindow, &rc);

        ID2D1HwndRenderTarget* render_target = nullptr;
        HRESULT hr = g_d2d_factory->CreateHwndRenderTarget(
            D2D1::RenderTargetProperties(),
            D2D1::HwndRenderTargetProperties(
                hContentWindow,
                D2D1::SizeU(rc.right > 0 ? rc.right : 1, rc.bottom > 0 ? rc.bottom : 1)
            ),
            &render_target
        );

        if (SUCCEEDED(hr) && render_target) {
            // 创建 WindowState 并添加到全局映射表
            WindowState* content_state = new WindowState();
            content_state->hwnd = hContentWindow;
            content_state->render_target = render_target;
            content_state->dwrite_factory = g_dwrite_factory;
            g_windows[hContentWindow] = content_state;
        }
    }

    // 添加 Tab 项到 TabControl
    TCITEMW tci = {};
    tci.mask = TCIF_TEXT;
    tci.pszText = (LPWSTR)title.c_str();

    int index = TabCtrl_GetItemCount(hTabControl);
    int result = TabCtrl_InsertItem(hTabControl, index, &tci);

    if (result == -1) {
        // 清理失败时创建的窗口和 WindowState
        if (hContentWindow && IsWindow(hContentWindow)) {
            auto win_it = g_windows.find(hContentWindow);
            if (win_it != g_windows.end()) {
                WindowState* win_state = win_it->second;
                if (win_state->render_target) {
                    win_state->render_target->Release();
                }
                delete win_state;
                g_windows.erase(win_it);
            }
            DestroyWindow(hContentWindow);
        }
        return -1;
    }

    // 保存 Tab 页信息
    TabPageInfo pageInfo;
    pageInfo.index = index;
    pageInfo.title = title;
    pageInfo.hContentWindow = hContentWindow;
    pageInfo.visible = false;

    state->pages.push_back(pageInfo);

    // 如果是第一个 Tab，自动选中
    if (index == 0) {
        TabCtrl_SetCurSel(hTabControl, 0);
        state->currentIndex = 0;
        UpdateTabLayout(state);

        if (state->callback) {
            state->callback(hTabControl, 0);
        }
    }

    return index;
}

// 移除 Tab 页
BOOL __stdcall RemoveTabItem(HWND hTabControl, int index) {
    auto it = g_tab_controls.find(hTabControl);
    if (it == g_tab_controls.end()) return FALSE;

    TabControlState* state = it->second;

    if (index < 0 || index >= (int)state->pages.size()) return FALSE;

    TabPageInfo& pageInfo = state->pages[index];

    if (pageInfo.hContentWindow && IsWindow(pageInfo.hContentWindow)) {
        // 清理内容窗口的 WindowState（如果存在）
        auto win_it = g_windows.find(pageInfo.hContentWindow);
        if (win_it != g_windows.end()) {
            WindowState* win_state = win_it->second;
            if (win_state->render_target) {
                win_state->render_target->Release();
            }
            delete win_state;
            g_windows.erase(win_it);
        }
        DestroyWindow(pageInfo.hContentWindow);
    }

    TabCtrl_DeleteItem(hTabControl, index);
    state->pages.erase(state->pages.begin() + index);

    for (size_t i = index; i < state->pages.size(); i++) {
        state->pages[i].index = (int)i;
    }

    int tabCount = (int)state->pages.size();
    if (tabCount > 0) {
        if (index == state->currentIndex) {
            int newIndex = (index > 0) ? (index - 1) : 0;
            TabCtrl_SetCurSel(hTabControl, newIndex);
            state->currentIndex = newIndex;
            UpdateTabLayout(state);

            if (state->callback) {
                state->callback(hTabControl, newIndex);
            }
        } else if (index < state->currentIndex) {
            state->currentIndex--;
            TabCtrl_SetCurSel(hTabControl, state->currentIndex);
        }
    } else {
        state->currentIndex = -1;
    }

    return TRUE;
}

// 设置 Tab 切换回调
void __stdcall SetTabCallback(HWND hTabControl, TAB_CALLBACK pCallback) {
    auto it = g_tab_controls.find(hTabControl);
    if (it == g_tab_controls.end()) return;

    TabControlState* state = it->second;
    state->callback = pCallback;
}

// 获取当前选中的 Tab 索引
int __stdcall GetCurrentTabIndex(HWND hTabControl) {
    auto it = g_tab_controls.find(hTabControl);
    if (it == g_tab_controls.end()) return -1;

    return it->second->currentIndex;
}

// 切换到指定 Tab
BOOL __stdcall SelectTab(HWND hTabControl, int index) {
    auto it = g_tab_controls.find(hTabControl);
    if (it == g_tab_controls.end()) return FALSE;

    TabControlState* state = it->second;

    if (index < 0 || index >= (int)state->pages.size()) return FALSE;

    TabCtrl_SetCurSel(hTabControl, index);
    state->currentIndex = index;
    UpdateTabLayout(state);

    if (state->callback) {
        state->callback(hTabControl, index);
    }

    return TRUE;
}

// 获取 Tab 数量
int __stdcall GetTabCount(HWND hTabControl) {
    auto it = g_tab_controls.find(hTabControl);
    if (it == g_tab_controls.end()) return 0;

    return (int)it->second->pages.size();
}

// 获取指定 Tab 的内容窗口句柄
HWND __stdcall GetTabContentWindow(HWND hTabControl, int index) {
    auto it = g_tab_controls.find(hTabControl);
    if (it == g_tab_controls.end()) return nullptr;

    TabControlState* state = it->second;

    if (index < 0 || index >= (int)state->pages.size()) return nullptr;

    return state->pages[index].hContentWindow;
}

// 销毁 TabControl（清理资源）
void __stdcall DestroyTabControl(HWND hTabControl) {
    auto it = g_tab_controls.find(hTabControl);
    if (it == g_tab_controls.end()) return;

    TabControlState* state = it->second;

    // 清理父窗口的子类化
    RemoveWindowSubclass(state->hParent, TabControlParentSubclassProc, (UINT_PTR)hTabControl);

    for (auto& page : state->pages) {
        if (page.hContentWindow && IsWindow(page.hContentWindow)) {
            // 清理内容窗口的 WindowState（如果存在）
            auto win_it = g_windows.find(page.hContentWindow);
            if (win_it != g_windows.end()) {
                WindowState* win_state = win_it->second;
                if (win_state->render_target) {
                    win_state->render_target->Release();
                }
                delete win_state;
                g_windows.erase(win_it);
            }
            DestroyWindow(page.hContentWindow);
        }
    }

    delete state;
    g_tab_controls.erase(it);

    DestroyWindow(hTabControl);
}

// 手动更新 TabControl 布局（窗口大小改变后调用）
void __stdcall UpdateTabControlLayout(HWND hTabControl) {
    auto it = g_tab_controls.find(hTabControl);
    if (it == g_tab_controls.end()) return;

    UpdateTabLayout(it->second);
}

// ========================================
// 编辑框和标签辅助函数
// ========================================

// 创建字体
HFONT CreateCustomFont(const FontStyle& font, int dpi = 96) {
    int height = -MulDiv(font.font_size, dpi, 72);
    return CreateFontW(
        height,
        0,
        0,
        0,
        font.bold ? FW_BOLD : FW_NORMAL,
        font.italic ? TRUE : FALSE,
        font.underline ? TRUE : FALSE,
        FALSE,
        DEFAULT_CHARSET,
        OUT_DEFAULT_PRECIS,
        CLIP_DEFAULT_PRECIS,
        CLEARTYPE_QUALITY,
        DEFAULT_PITCH | FF_DONTCARE,
        font.font_name.c_str()
    );
}

// 更新编辑框格式矩形以实现垂直居中（需为“多行样式”的编辑框，EM_SETRECTNP 仅对多行有效）
static void UpdateEditBoxFormatRect(HWND hwnd) {
    auto it = g_editboxes.find(hwnd);
    if (it == g_editboxes.end()) return;
    EditBoxState* state = it->second;
    if (!state->vertical_center) return;
    // 多行编辑框不设垂直居中；单行垂直居中时 state->multiline 为 false 但控件实际带 ES_MULTILINE
    if (state->multiline) return;

    RECT client;
    if (!GetClientRect(hwnd, &client) || client.bottom <= 0) return;

    HDC hdc = GetDC(hwnd);
    HFONT hFont = (HFONT)SendMessageW(hwnd, WM_GETFONT, 0, 0);
    HGDIOBJ oldFont = hFont ? SelectObject(hdc, hFont) : NULL;
    TEXTMETRICW tm = {};
    if (GetTextMetricsW(hdc, &tm)) {
        int line_height = tm.tmHeight;
        int top = (client.bottom - line_height) / 2;
        if (top < 0) top = 0;
        RECT format_rect = { 0, top, client.right, top + line_height };
        SendMessageW(hwnd, EM_SETRECTNP, 0, (LPARAM)&format_rect);
        // 左右边距设为 0，避免多行编辑框默认留白造成两侧白边
        SendMessageW(hwnd, EM_SETMARGINS, EC_LEFTMARGIN | EC_RIGHTMARGIN, MAKELPARAM(0, 0));
        InvalidateRect(hwnd, NULL, TRUE);
    }
    if (oldFont) SelectObject(hdc, oldFont);
    ReleaseDC(hwnd, hdc);
}

// 编辑框子类化处理
LRESULT CALLBACK EditBoxSubclassProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData) {
    EditBoxState* state = (EditBoxState*)dwRefData;
    
    switch (msg) {
        case WM_CTLCOLOREDIT:
        case WM_CTLCOLORSTATIC: {
            HDC hdc = (HDC)wparam;
            SetTextColor(hdc, RGB(
                (state->fg_color >> 16) & 0xFF,
                (state->fg_color >> 8) & 0xFF,
                state->fg_color & 0xFF
            ));
            SetBkColor(hdc, RGB(
                (state->bg_color >> 16) & 0xFF,
                (state->bg_color >> 8) & 0xFF,
                state->bg_color & 0xFF
            ));
            return (LRESULT)state->bg_brush;
        }
        case WM_SIZE: {
            if (state->vertical_center && !state->multiline) {
                UpdateEditBoxFormatRect(hwnd);
            }
            break;
        }
        case WM_CHAR: {
            // 单行+垂直居中时用多行样式创建，需拦截回车防止换行
            if (state->vertical_center && !state->multiline) {
                if (wparam == VK_RETURN || wparam == '\r' || wparam == '\n') {
                    return 0;
                }
            }
            break;
        }
        case WM_KEYDOWN:
        case WM_KEYUP: {
            if (state->key_callback) {
                int key_down = (msg == WM_KEYDOWN) ? 1 : 0;
                int shift = (GetKeyState(VK_SHIFT) & 0x8000) ? 1 : 0;
                int ctrl = (GetKeyState(VK_CONTROL) & 0x8000) ? 1 : 0;
                int alt = (GetKeyState(VK_MENU) & 0x8000) ? 1 : 0;
                state->key_callback(hwnd, (int)wparam, key_down, shift, ctrl, alt);
            }
            break;
        }
        case WM_NCDESTROY: {
            RemoveWindowSubclass(hwnd, EditBoxSubclassProc, uIdSubclass);
            g_editboxes.erase(hwnd);
            if (state->bg_brush) {
                DeleteObject(state->bg_brush);
            }
            delete state;
            break;
        }
    }
    
    return DefSubclassProc(hwnd, msg, wparam, lparam);
}

// 标签子类化处理
LRESULT CALLBACK LabelSubclassProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData) {
    LabelState* state = (LabelState*)dwRefData;
    
    switch (msg) {
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            
            RECT rect;
            GetClientRect(hwnd, &rect);
            
            FillRect(hdc, &rect, state->bg_brush);
            
            SetTextColor(hdc, RGB(
                (state->fg_color >> 16) & 0xFF,
                (state->fg_color >> 8) & 0xFF,
                state->fg_color & 0xFF
            ));
            SetBkMode(hdc, TRANSPARENT);
            
            HFONT hFont = CreateCustomFont(state->font);
            HFONT hOldFont = (HFONT)SelectObject(hdc, hFont);
            
            // ✅ 根据 word_wrap 参数决定是否换行
            UINT format = 0;
            if (state->word_wrap) {
                // 换行模式: 使用 DT_WORDBREAK 自动换行
                format = DT_WORDBREAK;
            } else {
                // 单行模式: 使用 DT_SINGLELINE 和 DT_VCENTER
                format = DT_VCENTER | DT_SINGLELINE;
            }
            
            // 添加对齐方式
            switch (state->alignment) {
                case ALIGN_LEFT: format |= DT_LEFT; break;
                case ALIGN_CENTER: format |= DT_CENTER; break;
                case ALIGN_RIGHT: format |= DT_RIGHT; break;
            }
            
            DrawTextW(hdc, state->text.c_str(), -1, &rect, format);
            
            SelectObject(hdc, hOldFont);
            DeleteObject(hFont);
            
            EndPaint(hwnd, &ps);
            return 0;
        }
        case WM_NCDESTROY: {
            RemoveWindowSubclass(hwnd, LabelSubclassProc, uIdSubclass);
            g_labels.erase(hwnd);
            if (state->bg_brush) {
                DeleteObject(state->bg_brush);
            }
            delete state;
            break;
        }
    }
    
    return DefSubclassProc(hwnd, msg, wparam, lparam);
}

// ========================================
// 编辑框导出函数
// ========================================

extern "C" {

__declspec(dllexport) HWND __stdcall CreateEditBox(
    HWND hParent,
    int x, int y, int width, int height,
    const unsigned char* text_bytes,
    int text_len,
    UINT32 fg_color,
    UINT32 bg_color,
    const unsigned char* font_name_bytes,
    int font_name_len,
    int font_size,
    BOOL bold,
    BOOL italic,
    BOOL underline,
    int alignment,
    BOOL multiline,
    BOOL readonly,
    BOOL password,
    BOOL has_border,
    BOOL vertical_center
) {
    // 转换文本
    std::wstring text = Utf8ToWide(text_bytes, text_len);
    std::wstring font_name = Utf8ToWide(font_name_bytes, font_name_len);
    
    // 创建编辑框样式
    // 单行+垂直居中时用 ES_MULTILINE+ES_AUTOHSCROLL，使 EM_SETRECTNP 生效（系统单行 EDIT 忽略格式矩形）
    BOOL use_multiline_for_center = (vertical_center && !multiline);
    DWORD style = WS_CHILD | WS_VISIBLE | WS_TABSTOP;
    if (multiline || use_multiline_for_center) {
        style |= ES_MULTILINE | ES_AUTOHSCROLL;
        if (multiline) {
            style |= ES_AUTOVSCROLL | WS_VSCROLL;
        }
    }
    if (readonly) {
        style |= ES_READONLY;
    }
    if (password) {
        style |= ES_PASSWORD;
    }
    if (has_border) {
        style |= WS_BORDER;
    }
    
    // 设置对齐方式
    switch (alignment) {
        case ALIGN_LEFT: style |= ES_LEFT; break;
        case ALIGN_CENTER: style |= ES_CENTER; break;
        case ALIGN_RIGHT: style |= ES_RIGHT; break;
    }
    
    // 创建编辑框
    int id = g_next_control_id++;
    HWND hEdit = CreateWindowExW(
        0,
        L"EDIT",
        text.c_str(),
        style,
        x, y, width, height,
        hParent,
        (HMENU)(INT_PTR)id,
        GetModuleHandle(NULL),
        NULL
    );
    
    if (!hEdit) {
        return NULL;
    }
    
    // 创建状态对象
    EditBoxState* state = new EditBoxState();
    state->hwnd = hEdit;
    state->parent = hParent;
    state->id = id;
    state->fg_color = fg_color;
    state->bg_color = bg_color;
    state->font.font_name = font_name;
    state->font.font_size = font_size;
    state->font.bold = bold != 0;
    state->font.italic = italic != 0;
    state->font.underline = underline != 0;
    state->alignment = (TextAlignment)alignment;
    state->multiline = multiline != 0;
    state->readonly = readonly != 0;
    state->password = password != 0;
    state->has_border = has_border != 0;
    state->vertical_center = (vertical_center != 0);
    state->key_callback = nullptr;

    // ✅ 创建背景画刷（只创建一次，避免重复创建导致闪烁）
    state->bg_brush = CreateSolidBrush(RGB(
        (bg_color >> 16) & 0xFF,
        (bg_color >> 8) & 0xFF,
        bg_color & 0xFF
    ));
    
    g_editboxes[hEdit] = state;
    
    // 设置字体
    HFONT hFont = CreateCustomFont(state->font);
    SendMessage(hEdit, WM_SETFONT, (WPARAM)hFont, TRUE);

    // 子类化以处理颜色、垂直居中、按键回调
    SetWindowSubclass(hEdit, EditBoxSubclassProc, 0, (DWORD_PTR)state);

    // 所有编辑框：左右边距设为 0，去掉系统默认留白（消除左右白边）
    SendMessageW(hEdit, EM_SETMARGINS, EC_LEFTMARGIN | EC_RIGHTMARGIN, MAKELPARAM(0, 0));

    if (state->vertical_center && !state->multiline) {
        UpdateEditBoxFormatRect(hEdit);
    }

    return hEdit;
}

__declspec(dllexport) int __stdcall GetEditBoxText(
    HWND hEdit,
    unsigned char* buffer,
    int buffer_size
) {
    if (!hEdit || !buffer || buffer_size <= 0) {
        return 0;
    }
    
    int len = GetWindowTextLengthW(hEdit);
    if (len <= 0) {
        return 0;
    }
    
    std::wstring text(len + 1, 0);
    GetWindowTextW(hEdit, &text[0], len + 1);
    
    // 转换为 UTF-8
    int utf8_len = WideCharToMultiByte(CP_UTF8, 0, text.c_str(), -1, NULL, 0, NULL, NULL);
    if (utf8_len <= 0 || utf8_len > buffer_size) {
        return 0;
    }
    
    WideCharToMultiByte(CP_UTF8, 0, text.c_str(), -1, (LPSTR)buffer, buffer_size, NULL, NULL);
    return utf8_len - 1;  // 不包括结尾的 null
}

__declspec(dllexport) void __stdcall SetEditBoxText(
    HWND hEdit,
    const unsigned char* text_bytes,
    int text_len
) {
    if (!hEdit) return;
    
    std::wstring text = Utf8ToWide(text_bytes, text_len);
    SetWindowTextW(hEdit, text.c_str());
}

__declspec(dllexport) void __stdcall SetEditBoxFont(
    HWND hEdit,
    const unsigned char* font_name_bytes,
    int font_name_len,
    int font_size,
    BOOL bold,
    BOOL italic,
    BOOL underline
) {
    auto it = g_editboxes.find(hEdit);
    if (it == g_editboxes.end()) return;
    
    EditBoxState* state = it->second;
    state->font.font_name = Utf8ToWide(font_name_bytes, font_name_len);
    state->font.font_size = font_size;
    state->font.bold = bold != 0;
    state->font.italic = italic != 0;
    state->font.underline = underline != 0;
    
    HFONT hFont = CreateCustomFont(state->font);
    SendMessage(hEdit, WM_SETFONT, (WPARAM)hFont, TRUE);
    if (state->vertical_center && !state->multiline) {
        UpdateEditBoxFormatRect(hEdit);
    }
}

__declspec(dllexport) void __stdcall SetEditBoxColor(
    HWND hEdit,
    UINT32 fg_color,
    UINT32 bg_color
) {
    auto it = g_editboxes.find(hEdit);
    if (it == g_editboxes.end()) return;
    
    EditBoxState* state = it->second;
    state->fg_color = fg_color;
    state->bg_color = bg_color;
    
    // ✅ 重新创建背景画刷
    if (state->bg_brush) {
        DeleteObject(state->bg_brush);
    }
    state->bg_brush = CreateSolidBrush(RGB(
        (bg_color >> 16) & 0xFF,
        (bg_color >> 8) & 0xFF,
        bg_color & 0xFF
    ));
    
    InvalidateRect(hEdit, NULL, TRUE);
}

__declspec(dllexport) void __stdcall SetEditBoxBounds(
    HWND hEdit,
    int x, int y, int width, int height
) {
    if (!hEdit) return;
    SetWindowPos(hEdit, NULL, x, y, width, height, SWP_NOZORDER);
    auto it = g_editboxes.find(hEdit);
    if (it != g_editboxes.end() && it->second->vertical_center && !it->second->multiline) {
        UpdateEditBoxFormatRect(hEdit);
    }
}

__declspec(dllexport) void __stdcall EnableEditBox(
    HWND hEdit,
    BOOL enable
) {
    if (!hEdit) return;
    EnableWindow(hEdit, enable);
}

__declspec(dllexport) void __stdcall ShowEditBox(
    HWND hEdit,
    BOOL show
) {
    if (!hEdit) return;
    ShowWindow(hEdit, show ? SW_SHOW : SW_HIDE);
}

__declspec(dllexport) void __stdcall SetEditBoxVerticalCenter(
    HWND hEdit,
    BOOL vertical_center
) {
    auto it = g_editboxes.find(hEdit);
    if (it == g_editboxes.end()) return;
    EditBoxState* state = it->second;
    state->vertical_center = (vertical_center != 0);
    if (state->vertical_center && !state->multiline) {
        UpdateEditBoxFormatRect(hEdit);
    } else if (!state->vertical_center) {
        RECT client;
        if (GetClientRect(hEdit, &client)) {
            SendMessageW(hEdit, EM_SETRECTNP, 0, (LPARAM)&client);
        }
    }
}

__declspec(dllexport) void __stdcall SetEditBoxKeyCallback(
    HWND hEdit,
    EditBoxKeyCallback callback
) {
    auto it = g_editboxes.find(hEdit);
    if (it == g_editboxes.end()) return;
    it->second->key_callback = callback;
}

// ========================================
// 标签导出函数
// ========================================

__declspec(dllexport) HWND __stdcall CreateLabel(
    HWND hParent,
    int x, int y, int width, int height,
    const unsigned char* text_bytes,
    int text_len,
    UINT32 fg_color,
    UINT32 bg_color,
    const unsigned char* font_name_bytes,
    int font_name_len,
    int font_size,
    BOOL bold,
    BOOL italic,
    BOOL underline,
    int alignment,
    BOOL word_wrap  // ✅ 新增参数: 是否换行显示
) {
    // 转换文本
    std::wstring text = Utf8ToWide(text_bytes, text_len);
    std::wstring font_name = Utf8ToWide(font_name_bytes, font_name_len);
    
    // 创建标签
    int id = g_next_control_id++;
    HWND hLabel = CreateWindowExW(
        0,
        L"STATIC",
        text.c_str(),
        WS_CHILD | WS_VISIBLE | SS_OWNERDRAW,
        x, y, width, height,
        hParent,
        (HMENU)(INT_PTR)id,
        GetModuleHandle(NULL),
        NULL
    );
    
    if (!hLabel) {
        return NULL;
    }
    
    // 创建状态对象
    LabelState* state = new LabelState();
    state->hwnd = hLabel;
    state->parent = hParent;
    state->id = id;
    state->text = text;
    state->fg_color = fg_color;
    state->bg_color = bg_color;
    state->font.font_name = font_name;
    state->font.font_size = font_size;
    state->font.bold = bold != 0;
    state->font.italic = italic != 0;
    state->font.underline = underline != 0;
    state->alignment = (TextAlignment)alignment;
    state->word_wrap = word_wrap != 0;  // ✅ 保存换行设置
    
    // ✅ 创建背景画刷（只创建一次，避免重复创建导致闪烁）
    state->bg_brush = CreateSolidBrush(RGB(
        (bg_color >> 16) & 0xFF,
        (bg_color >> 8) & 0xFF,
        bg_color & 0xFF
    ));
    
    g_labels[hLabel] = state;
    
    // 子类化以自定义绘制
    SetWindowSubclass(hLabel, LabelSubclassProc, 0, (DWORD_PTR)state);
    
    return hLabel;
}

__declspec(dllexport) void __stdcall SetLabelText(
    HWND hLabel,
    const unsigned char* text_bytes,
    int text_len
) {
    auto it = g_labels.find(hLabel);
    if (it == g_labels.end()) return;
    
    LabelState* state = it->second;
    state->text = Utf8ToWide(text_bytes, text_len);
    
    InvalidateRect(hLabel, NULL, TRUE);
}

__declspec(dllexport) void __stdcall SetLabelFont(
    HWND hLabel,
    const unsigned char* font_name_bytes,
    int font_name_len,
    int font_size,
    BOOL bold,
    BOOL italic,
    BOOL underline
) {
    auto it = g_labels.find(hLabel);
    if (it == g_labels.end()) return;
    
    LabelState* state = it->second;
    state->font.font_name = Utf8ToWide(font_name_bytes, font_name_len);
    state->font.font_size = font_size;
    state->font.bold = bold != 0;
    state->font.italic = italic != 0;
    state->font.underline = underline != 0;
    
    InvalidateRect(hLabel, NULL, TRUE);
}

__declspec(dllexport) void __stdcall SetLabelColor(
    HWND hLabel,
    UINT32 fg_color,
    UINT32 bg_color
) {
    auto it = g_labels.find(hLabel);
    if (it == g_labels.end()) return;
    
    LabelState* state = it->second;
    state->fg_color = fg_color;
    state->bg_color = bg_color;
    
    // ✅ 重新创建背景画刷
    if (state->bg_brush) {
        DeleteObject(state->bg_brush);
    }
    state->bg_brush = CreateSolidBrush(RGB(
        (bg_color >> 16) & 0xFF,
        (bg_color >> 8) & 0xFF,
        bg_color & 0xFF
    ));
    
    InvalidateRect(hLabel, NULL, TRUE);
}

__declspec(dllexport) void __stdcall SetLabelBounds(
    HWND hLabel,
    int x, int y, int width, int height
) {
    if (!hLabel) return;
    SetWindowPos(hLabel, NULL, x, y, width, height, SWP_NOZORDER);
}

__declspec(dllexport) void __stdcall EnableLabel(
    HWND hLabel,
    BOOL enable
) {
    if (!hLabel) return;
    EnableWindow(hLabel, enable);
}

__declspec(dllexport) void __stdcall ShowLabel(
    HWND hLabel,
    BOOL show
) {
    if (!hLabel) return;
    ShowWindow(hLabel, show ? SW_SHOW : SW_HIDE);
}

} // extern "C"


// ========== 复选框功能实现 ==========

// 绘制复选框（Element UI风格）
void DrawCheckBox(ID2D1HwndRenderTarget* rt, IDWriteFactory* factory, CheckBoxState* state) {
    if (!rt || !factory || !state) return;

    // Element UI 配色
    UINT32 primary_color = 0xFF409EFF;  // 主色
    UINT32 border_color = 0xFFDCDFE6;   // 边框色
    UINT32 disabled_color = 0xFFC0C4CC; // 禁用色
    UINT32 hover_border = 0xFF409EFF;   // 悬停边框色

    // 复选框尺寸（Element UI标准）
    int box_size = 14;
    int box_x = state->x;
    int box_y = state->y + (state->height - box_size) / 2;

    // 确定复选框颜色
    UINT32 current_bg = state->bg_color;
    UINT32 current_border = border_color;
    
    if (!state->enabled) {
        current_bg = 0xFFF5F7FA;
        current_border = disabled_color;
    } else if (state->checked) {
        current_bg = primary_color;
        current_border = primary_color;
    } else if (state->hovered) {
        current_border = hover_border;
    }

    // 绘制复选框背景
    ID2D1SolidColorBrush* bg_brush = nullptr;
    rt->CreateSolidColorBrush(ColorFromUInt32(current_bg), &bg_brush);
    
    D2D1_ROUNDED_RECT box_rect = D2D1::RoundedRect(
        D2D1::RectF(
            (FLOAT)box_x,
            (FLOAT)box_y,
            (FLOAT)(box_x + box_size),
            (FLOAT)(box_y + box_size)
        ),
        2.0f, 2.0f  // Element UI 圆角
    );
    rt->FillRoundedRectangle(box_rect, bg_brush);
    bg_brush->Release();

    // 绘制边框
    ID2D1SolidColorBrush* border_brush = nullptr;
    rt->CreateSolidColorBrush(ColorFromUInt32(current_border), &border_brush);
    rt->DrawRoundedRectangle(box_rect, border_brush, 1.0f);
    border_brush->Release();

    // 如果选中，绘制勾选标记
    if (state->checked) {
        ID2D1SolidColorBrush* check_brush = nullptr;
        rt->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White), &check_brush);

        // 绘制勾选路径（使用路径几何）
        ID2D1PathGeometry* path = nullptr;
        g_d2d_factory->CreatePathGeometry(&path);
        
        ID2D1GeometrySink* sink = nullptr;
        path->Open(&sink);
        
        // 勾选标记的路径点
        float cx = box_x + box_size / 2.0f;
        float cy = box_y + box_size / 2.0f;
        
        sink->BeginFigure(D2D1::Point2F(cx - 3, cy), D2D1_FIGURE_BEGIN_FILLED);
        sink->AddLine(D2D1::Point2F(cx - 1, cy + 2));
        sink->AddLine(D2D1::Point2F(cx + 3, cy - 2));
        sink->EndFigure(D2D1_FIGURE_END_OPEN);
        sink->Close();
        
        rt->DrawGeometry(path, check_brush, 1.5f);
        
        sink->Release();
        path->Release();
        check_brush->Release();
    }

    // 绘制文本标签
    if (!state->text.empty()) {
        IDWriteTextFormat* text_format = nullptr;
        factory->CreateTextFormat(
            state->font.font_name.empty() ? L"Microsoft YaHei UI" : state->font.font_name.c_str(),
            nullptr,
            state->font.bold ? DWRITE_FONT_WEIGHT_BOLD : DWRITE_FONT_WEIGHT_NORMAL,
            state->font.italic ? DWRITE_FONT_STYLE_ITALIC : DWRITE_FONT_STYLE_NORMAL,
            DWRITE_FONT_STRETCH_NORMAL,
            (FLOAT)(state->font.font_size > 0 ? state->font.font_size : 14),
            L"zh-CN",
            &text_format
        );

        text_format->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
        text_format->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);

        UINT32 text_color = state->enabled ? state->fg_color : disabled_color;
        ID2D1SolidColorBrush* text_brush = nullptr;
        rt->CreateSolidColorBrush(ColorFromUInt32(text_color), &text_brush);

        D2D1_RECT_F text_rect = D2D1::RectF(
            (FLOAT)(box_x + box_size + 8),
            (FLOAT)state->y,
            (FLOAT)(state->x + state->width),
            (FLOAT)(state->y + state->height)
        );

        rt->DrawText(
            state->text.c_str(),
            (UINT32)state->text.length(),
            text_format,
            text_rect,
            text_brush
        );

        text_brush->Release();
        text_format->Release();
    }
}

// 复选框窗口过程（子类化）
LRESULT CALLBACK CheckBoxProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData) {
    CheckBoxState* state = (CheckBoxState*)dwRefData;
    
    switch (msg) {
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            
            // 创建D2D渲染目标
            ID2D1HwndRenderTarget* rt = nullptr;
            D2D1_RENDER_TARGET_PROPERTIES props = D2D1::RenderTargetProperties();
            D2D1_HWND_RENDER_TARGET_PROPERTIES hwnd_props = D2D1::HwndRenderTargetProperties(
                hwnd,
                D2D1::SizeU(state->width, state->height)
            );
            
            g_d2d_factory->CreateHwndRenderTarget(props, hwnd_props, &rt);
            
            if (rt) {
                rt->BeginDraw();
                rt->Clear(ColorFromUInt32(state->bg_color));
                DrawCheckBox(rt, g_dwrite_factory, state);
                rt->EndDraw();
                rt->Release();
            }
            
            EndPaint(hwnd, &ps);
            return 0;
        }
        
        case WM_LBUTTONDOWN: {
            if (state->enabled) {
                state->pressed = true;
                InvalidateRect(hwnd, nullptr, FALSE);
            }
            return 0;
        }
        
        case WM_LBUTTONUP: {
            if (state->enabled && state->pressed) {
                state->pressed = false;
                
                // 切换选中状态
                state->checked = !state->checked;
                InvalidateRect(hwnd, nullptr, FALSE);
                
                // 触发回调
                if (state->callback) {
                    state->callback(hwnd, state->checked);
                }
            }
            return 0;
        }
        
        case WM_MOUSEMOVE: {
            if (state->enabled && !state->hovered) {
                state->hovered = true;
                InvalidateRect(hwnd, nullptr, FALSE);
                
                // 跟踪鼠标离开
                TRACKMOUSEEVENT tme = { sizeof(TRACKMOUSEEVENT) };
                tme.dwFlags = TME_LEAVE;
                tme.hwndTrack = hwnd;
                TrackMouseEvent(&tme);
            }
            return 0;
        }
        
        case WM_MOUSELEAVE: {
            if (state->hovered) {
                state->hovered = false;
                state->pressed = false;
                InvalidateRect(hwnd, nullptr, FALSE);
            }
            return 0;
        }
        
        case WM_NCDESTROY: {
            // 清理资源
            RemoveWindowSubclass(hwnd, CheckBoxProc, uIdSubclass);
            g_checkboxes.erase(hwnd);
            delete state;
            return 0;
        }
    }
    
    return DefSubclassProc(hwnd, msg, wparam, lparam);
}

// 创建复选框
HWND __stdcall CreateCheckBox(
    HWND hParent,
    int x, int y, int width, int height,
    const unsigned char* text_bytes,
    int text_len,
    BOOL checked,
    UINT32 fg_color,
    UINT32 bg_color
) {
    if (!hParent) return nullptr;
    
    // 初始化D2D和DirectWrite（如果尚未初始化）
    if (!g_d2d_factory) {
        D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &g_d2d_factory);
    }
    if (!g_dwrite_factory) {
        DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory),
                           reinterpret_cast<IUnknown**>(&g_dwrite_factory));
    }
    
    // 创建静态控件作为复选框容器
    HWND hwnd = CreateWindowExW(
        0,
        L"STATIC",
        L"",
        WS_CHILD | WS_VISIBLE | SS_NOTIFY,
        x, y, width, height,
        hParent,
        (HMENU)(INT_PTR)g_next_control_id++,
        GetModuleHandle(nullptr),
        nullptr
    );
    
    if (!hwnd) return nullptr;
    
    // 创建状态对象
    CheckBoxState* state = new CheckBoxState();
    state->hwnd = hwnd;
    state->parent = hParent;
    state->id = (int)(INT_PTR)GetMenu(hwnd);
    state->x = 0;  // 相对于控件自身
    state->y = 0;
    state->width = width;
    state->height = height;
    state->text = Utf8ToWide(text_bytes, text_len);
    state->checked = (checked != 0);
    state->enabled = true;
    state->hovered = false;
    state->pressed = false;
    state->fg_color = fg_color ? fg_color : 0xFF303133;  // Element UI 主要文本色
    state->bg_color = bg_color ? bg_color : 0xFFFFFFFF;  // 白色背景
    state->check_color = 0xFF409EFF;  // Element UI 主色
    state->font.font_name = L"Microsoft YaHei UI";
    state->font.font_size = 14;
    state->font.bold = false;
    state->font.italic = false;
    state->font.underline = false;
    state->callback = nullptr;
    
    // 保存到全局map
    g_checkboxes[hwnd] = state;
    
    // 子类化窗口
    SetWindowSubclass(hwnd, CheckBoxProc, 0, (DWORD_PTR)state);
    
    return hwnd;
}

// 获取复选框选中状态
BOOL __stdcall GetCheckBoxState(HWND hCheckBox) {
    auto it = g_checkboxes.find(hCheckBox);
    if (it == g_checkboxes.end()) return FALSE;
    return it->second->checked ? TRUE : FALSE;
}

// 设置复选框选中状态
void __stdcall SetCheckBoxState(HWND hCheckBox, BOOL checked) {
    auto it = g_checkboxes.find(hCheckBox);
    if (it == g_checkboxes.end()) return;
    
    it->second->checked = (checked != 0);
    InvalidateRect(hCheckBox, nullptr, FALSE);
}

// 设置复选框回调
void __stdcall SetCheckBoxCallback(HWND hCheckBox, CheckBoxCallback callback) {
    auto it = g_checkboxes.find(hCheckBox);
    if (it == g_checkboxes.end()) return;
    
    it->second->callback = callback;
}

// 启用/禁用复选框
void __stdcall EnableCheckBox(HWND hCheckBox, BOOL enable) {
    auto it = g_checkboxes.find(hCheckBox);
    if (it == g_checkboxes.end()) return;
    
    it->second->enabled = (enable != 0);
    InvalidateRect(hCheckBox, nullptr, FALSE);
}

// 显示/隐藏复选框
void __stdcall ShowCheckBox(HWND hCheckBox, BOOL show) {
    if (!hCheckBox) return;
    ShowWindow(hCheckBox, show ? SW_SHOW : SW_HIDE);
}

// 设置复选框文本
void __stdcall SetCheckBoxText(HWND hCheckBox, const unsigned char* text_bytes, int text_len) {
    auto it = g_checkboxes.find(hCheckBox);
    if (it == g_checkboxes.end()) return;
    
    it->second->text = Utf8ToWide(text_bytes, text_len);
    InvalidateRect(hCheckBox, nullptr, FALSE);
}

// 设置复选框位置和大小
void __stdcall SetCheckBoxBounds(HWND hCheckBox, int x, int y, int width, int height) {
    auto it = g_checkboxes.find(hCheckBox);
    if (it == g_checkboxes.end()) return;
    
    it->second->width = width;
    it->second->height = height;
    SetWindowPos(hCheckBox, nullptr, x, y, width, height, SWP_NOZORDER);
    InvalidateRect(hCheckBox, nullptr, FALSE);
}


// ========== 进度条功能实现 ==========

// 进度条动画定时器ID基数
#define PROGRESSBAR_TIMER_BASE 20000

// 绘制进度条
void DrawProgressBar(ID2D1HwndRenderTarget* rt, IDWriteFactory* factory, ProgressBarState* state) {
    if (!rt || !factory || !state) return;
    
    ID2D1SolidColorBrush* brush = nullptr;
    rt->CreateSolidColorBrush(D2D1::ColorF(0, 0, 0), &brush);
    if (!brush) return;
    
    float width = (float)state->width;
    float height = (float)state->height;
    
    // Element UI 风格配色
    UINT32 bg_color = state->bg_color;
    UINT32 fg_color = state->fg_color;
    UINT32 border_color = state->border_color;
    
    // 如果禁用，使用灰色
    if (!state->enabled) {
        fg_color = 0xFFC0C4CC;  // Element UI 禁用色
        bg_color = 0xFFF5F7FA;
    }
    
    // 绘制背景（圆角矩形）
    D2D1_ROUNDED_RECT bg_rect = D2D1::RoundedRect(
        D2D1::RectF(0, 0, width, height),
        4.0f, 4.0f  // 4px 圆角
    );
    brush->SetColor(ColorFromUInt32(bg_color));
    rt->FillRoundedRectangle(bg_rect, brush);
    
    // 绘制边框
    brush->SetColor(ColorFromUInt32(border_color));
    rt->DrawRoundedRectangle(bg_rect, brush, 1.0f);
    
    // 计算进度条宽度
    float progress_width = 0;
    
    if (state->indeterminate) {
        // 不确定模式：绘制移动的进度条
        float bar_width = width * 0.3f;  // 进度条宽度为总宽度的30%
        float start_x = state->indeterminate_pos * (width + bar_width) - bar_width;
        progress_width = bar_width;
        
        if (start_x + bar_width > 0 && start_x < width) {
            float visible_start = (std::max)(0.0f, start_x);
            float visible_end = (std::min)(width, start_x + bar_width);
            float visible_width = visible_end - visible_start;
            
            if (visible_width > 0) {
                D2D1_ROUNDED_RECT progress_rect = D2D1::RoundedRect(
                    D2D1::RectF(visible_start + 2, 2, visible_end - 2, height - 2),
                    3.0f, 3.0f
                );
                
                // 使用渐变色
                ID2D1GradientStopCollection* gradient_stops = nullptr;
                D2D1_GRADIENT_STOP stops[2];
                stops[0].position = 0.0f;
                stops[0].color = ColorFromUInt32(LightenColor(fg_color, 1.2f));
                stops[1].position = 1.0f;
                stops[1].color = ColorFromUInt32(fg_color);
                
                rt->CreateGradientStopCollection(stops, 2, &gradient_stops);
                if (gradient_stops) {
                    ID2D1LinearGradientBrush* gradient_brush = nullptr;
                    rt->CreateLinearGradientBrush(
                        D2D1::LinearGradientBrushProperties(
                            D2D1::Point2F(0, 0),
                            D2D1::Point2F(0, height)
                        ),
                        gradient_stops,
                        &gradient_brush
                    );
                    
                    if (gradient_brush) {
                        rt->FillRoundedRectangle(progress_rect, gradient_brush);
                        gradient_brush->Release();
                    }
                    gradient_stops->Release();
                }
            }
        }
    } else {
        // 确定模式：根据动画值绘制进度
        progress_width = (state->animation_value / 100.0f) * (width - 4);
        
        if (progress_width > 0) {
            D2D1_ROUNDED_RECT progress_rect = D2D1::RoundedRect(
                D2D1::RectF(2, 2, 2 + progress_width, height - 2),
                3.0f, 3.0f
            );
            
            // 使用渐变色
            ID2D1GradientStopCollection* gradient_stops = nullptr;
            D2D1_GRADIENT_STOP stops[2];
            stops[0].position = 0.0f;
            stops[0].color = ColorFromUInt32(LightenColor(fg_color, 1.2f));
            stops[1].position = 1.0f;
            stops[1].color = ColorFromUInt32(fg_color);
            
            rt->CreateGradientStopCollection(stops, 2, &gradient_stops);
            if (gradient_stops) {
                ID2D1LinearGradientBrush* gradient_brush = nullptr;
                rt->CreateLinearGradientBrush(
                    D2D1::LinearGradientBrushProperties(
                        D2D1::Point2F(0, 0),
                        D2D1::Point2F(0, height)
                    ),
                    gradient_stops,
                    &gradient_brush
                );
                
                if (gradient_brush) {
                    rt->FillRoundedRectangle(progress_rect, gradient_brush);
                    gradient_brush->Release();
                }
                gradient_stops->Release();
            }
        }
    }
    
    // 绘制百分比文本
    if (state->show_text && !state->indeterminate) {
        wchar_t text[32];
        swprintf_s(text, 32, L"%d%%", state->current_value);
        
        IDWriteTextFormat* text_format = nullptr;
        factory->CreateTextFormat(
            state->font.font_name.c_str(),
            nullptr,
            state->font.bold ? DWRITE_FONT_WEIGHT_BOLD : DWRITE_FONT_WEIGHT_NORMAL,
            state->font.italic ? DWRITE_FONT_STYLE_ITALIC : DWRITE_FONT_STYLE_NORMAL,
            DWRITE_FONT_STRETCH_NORMAL,
            (float)state->font.font_size,
            L"zh-cn",
            &text_format
        );
        
        if (text_format) {
            text_format->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
            text_format->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
            
            brush->SetColor(ColorFromUInt32(state->text_color));
            rt->DrawText(
                text,
                (UINT32)wcslen(text),
                text_format,
                D2D1::RectF(0, 0, width, height),
                brush
            );
            
            text_format->Release();
        }
    }
    
    brush->Release();
}

// 进度条窗口过程
LRESULT CALLBACK ProgressBarProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData) {
    ProgressBarState* state = (ProgressBarState*)dwRefData;
    
    switch (msg) {
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            
            // 创建 D2D 渲染目标
            ID2D1HwndRenderTarget* rt = nullptr;
            D2D1_RENDER_TARGET_PROPERTIES props = D2D1::RenderTargetProperties();
            D2D1_HWND_RENDER_TARGET_PROPERTIES hwnd_props = D2D1::HwndRenderTargetProperties(
                hwnd,
                D2D1::SizeU(state->width, state->height)
            );
            
            if (g_d2d_factory) {
                g_d2d_factory->CreateHwndRenderTarget(props, hwnd_props, &rt);
            }
            
            if (rt && g_dwrite_factory) {
                rt->BeginDraw();
                rt->Clear(D2D1::ColorF(D2D1::ColorF::White, 0.0f));
                
                DrawProgressBar(rt, g_dwrite_factory, state);
                
                rt->EndDraw();
                rt->Release();
            }
            
            EndPaint(hwnd, &ps);
            return 0;
        }
        
        case WM_TIMER: {
            if (wparam == state->timer_id) {
                bool need_redraw = false;
                
                if (state->indeterminate) {
                    // 不确定模式动画
                    state->indeterminate_pos += 0.02f;
                    if (state->indeterminate_pos > 1.0f) {
                        state->indeterminate_pos = 0.0f;
                    }
                    need_redraw = true;
                } else {
                    // 平滑过渡动画
                    float diff = state->target_value - state->animation_value;
                    if (fabs(diff) > 0.1f) {
                        state->animation_value += diff * 0.15f;  // 平滑系数
                        need_redraw = true;
                    } else {
                        state->animation_value = (float)state->target_value;
                        if (state->current_value == state->target_value) {
                            // 动画完成，停止定时器
                            KillTimer(hwnd, state->timer_id);
                            state->timer_id = 0;
                        }
                        need_redraw = true;
                    }
                }
                
                if (need_redraw) {
                    InvalidateRect(hwnd, NULL, FALSE);
                }
            }
            return 0;
        }
        
        case WM_NCDESTROY: {
            // 清理定时器
            if (state->timer_id != 0) {
                KillTimer(hwnd, state->timer_id);
            }
            
            // 移除子类化
            RemoveWindowSubclass(hwnd, ProgressBarProc, uIdSubclass);
            
            // 从全局map中移除
            g_progressbars.erase(hwnd);
            delete state;
            return 0;
        }
    }
    
    return DefSubclassProc(hwnd, msg, wparam, lparam);
}

// 创建进度条
__declspec(dllexport) HWND __stdcall CreateProgressBar(
    HWND hParent,
    int x, int y, int width, int height,
    int initial_value,
    UINT32 fg_color,
    UINT32 bg_color,
    BOOL show_text,
    UINT32 text_color
) {
    if (!hParent) return NULL;
    
    // 限制初始值范围
    if (initial_value < 0) initial_value = 0;
    if (initial_value > 100) initial_value = 100;
    
    // 生成唯一ID
    int id = g_next_control_id++;
    
    // 创建静态控件作为容器
    HWND hProgressBar = CreateWindowExW(
        0,
        L"STATIC",
        L"",
        WS_CHILD | WS_VISIBLE | SS_OWNERDRAW,
        x, y, width, height,
        hParent,
        (HMENU)(INT_PTR)id,
        GetModuleHandle(NULL),
        NULL
    );
    
    if (!hProgressBar) {
        return NULL;
    }
    
    // 创建状态对象
    ProgressBarState* state = new ProgressBarState();
    state->hwnd = hProgressBar;
    state->parent = hParent;
    state->id = id;
    state->x = x;
    state->y = y;
    state->width = width;
    state->height = height;
    state->current_value = initial_value;
    state->target_value = initial_value;
    state->animation_value = (float)initial_value;
    state->indeterminate = false;
    state->indeterminate_pos = 0.0f;
    state->enabled = true;
    state->fg_color = fg_color;
    state->bg_color = bg_color;
    state->border_color = 0xFFDCDFE6;  // Element UI 边框色
    state->text_color = text_color;
    state->show_text = show_text != 0;
    state->font.font_name = L"Microsoft YaHei UI";
    state->font.font_size = 12;
    state->font.bold = false;
    state->font.italic = false;
    state->font.underline = false;
    state->callback = nullptr;
    state->timer_id = 0;
    
    g_progressbars[hProgressBar] = state;
    
    // 子类化以自定义绘制
    SetWindowSubclass(hProgressBar, ProgressBarProc, 0, (DWORD_PTR)state);
    
    return hProgressBar;
}

// 设置进度条值
__declspec(dllexport) void __stdcall SetProgressValue(
    HWND hProgressBar,
    int value
) {
    auto it = g_progressbars.find(hProgressBar);
    if (it == g_progressbars.end()) return;
    
    ProgressBarState* state = it->second;
    
    // 限制值范围
    if (value < 0) value = 0;
    if (value > 100) value = 100;
    
    int old_value = state->current_value;
    state->target_value = value;
    state->current_value = value;
    
    // 启动动画定时器
    if (state->timer_id == 0) {
        state->timer_id = SetTimer(hProgressBar, PROGRESSBAR_TIMER_BASE + state->id, 16, NULL);  // 60fps
    }
    
    // 触发回调
    if (state->callback && old_value != value) {
        state->callback(hProgressBar, value);
    }
    
    InvalidateRect(hProgressBar, NULL, FALSE);
}

// 获取进度条值
__declspec(dllexport) int __stdcall GetProgressValue(
    HWND hProgressBar
) {
    auto it = g_progressbars.find(hProgressBar);
    if (it == g_progressbars.end()) return 0;
    
    return it->second->current_value;
}

// 设置进度条不确定模式
__declspec(dllexport) void __stdcall SetProgressIndeterminate(
    HWND hProgressBar,
    BOOL indeterminate
) {
    auto it = g_progressbars.find(hProgressBar);
    if (it == g_progressbars.end()) return;
    
    ProgressBarState* state = it->second;
    state->indeterminate = indeterminate != 0;
    state->indeterminate_pos = 0.0f;
    
    // 启动或停止动画定时器
    if (state->indeterminate) {
        if (state->timer_id == 0) {
            state->timer_id = SetTimer(hProgressBar, PROGRESSBAR_TIMER_BASE + state->id, 16, NULL);
        }
    } else {
        if (state->timer_id != 0 && state->current_value == state->target_value) {
            KillTimer(hProgressBar, state->timer_id);
            state->timer_id = 0;
        }
    }
    
    InvalidateRect(hProgressBar, NULL, FALSE);
}

// 设置进度条颜色
__declspec(dllexport) void __stdcall SetProgressBarColor(
    HWND hProgressBar,
    UINT32 fg_color,
    UINT32 bg_color
) {
    auto it = g_progressbars.find(hProgressBar);
    if (it == g_progressbars.end()) return;
    
    ProgressBarState* state = it->second;
    state->fg_color = fg_color;
    state->bg_color = bg_color;
    
    InvalidateRect(hProgressBar, NULL, FALSE);
}

// 设置进度条回调
__declspec(dllexport) void __stdcall SetProgressBarCallback(
    HWND hProgressBar,
    ProgressBarCallback callback
) {
    auto it = g_progressbars.find(hProgressBar);
    if (it == g_progressbars.end()) return;
    
    it->second->callback = callback;
}

// 启用/禁用进度条
__declspec(dllexport) void __stdcall EnableProgressBar(
    HWND hProgressBar,
    BOOL enable
) {
    auto it = g_progressbars.find(hProgressBar);
    if (it == g_progressbars.end()) return;
    
    ProgressBarState* state = it->second;
    state->enabled = enable != 0;
    
    EnableWindow(hProgressBar, enable);
    InvalidateRect(hProgressBar, NULL, FALSE);
}

// 显示/隐藏进度条
__declspec(dllexport) void __stdcall ShowProgressBar(
    HWND hProgressBar,
    BOOL show
) {
    auto it = g_progressbars.find(hProgressBar);
    if (it == g_progressbars.end()) return;
    
    ShowWindow(hProgressBar, show ? SW_SHOW : SW_HIDE);
}

// 设置进度条位置和大小
__declspec(dllexport) void __stdcall SetProgressBarBounds(
    HWND hProgressBar,
    int x, int y, int width, int height
) {
    auto it = g_progressbars.find(hProgressBar);
    if (it == g_progressbars.end()) return;
    
    ProgressBarState* state = it->second;
    state->x = x;
    state->y = y;
    state->width = width;
    state->height = height;
    
    SetWindowPos(hProgressBar, NULL, x, y, width, height, SWP_NOZORDER);
    InvalidateRect(hProgressBar, NULL, FALSE);
}

// 设置是否显示百分比文本
__declspec(dllexport) void __stdcall SetProgressBarShowText(
    HWND hProgressBar,
    BOOL show_text
) {
    auto it = g_progressbars.find(hProgressBar);
    if (it == g_progressbars.end()) return;
    
    ProgressBarState* state = it->second;
    state->show_text = show_text != 0;
    
    InvalidateRect(hProgressBar, NULL, FALSE);
}

// ========== 图片框功能实现 ==========

// 绘制图片框
void DrawPictureBox(ID2D1HwndRenderTarget* rt, IDWriteFactory* factory, PictureBoxState* state) {
    if (!rt || !state) return;
    
    float width = (float)state->width;
    float height = (float)state->height;
    
    // 绘制背景
    ID2D1SolidColorBrush* brush = nullptr;
    rt->CreateSolidColorBrush(ColorFromUInt32(state->bg_color), &brush);
    if (brush) {
        rt->FillRectangle(D2D1::RectF(0, 0, width, height), brush);
        brush->Release();
    }
    
    // 如果有图片，绘制图片
    if (state->bitmap) {
        D2D1_SIZE_F bitmapSize = state->bitmap->GetSize();
        D2D1_RECT_F destRect;
        
        switch (state->scale_mode) {
            case SCALE_NONE: {
                // 不缩放，左上角对齐
                destRect = D2D1::RectF(0, 0, bitmapSize.width, bitmapSize.height);
                break;
            }
            case SCALE_STRETCH: {
                // 拉伸填充整个控件
                destRect = D2D1::RectF(0, 0, width, height);
                break;
            }
            case SCALE_FIT: {
                // 等比缩放适应控件
                float scaleX = width / bitmapSize.width;
                float scaleY = height / bitmapSize.height;
                float scale = min(scaleX, scaleY);
                
                float scaledWidth = bitmapSize.width * scale;
                float scaledHeight = bitmapSize.height * scale;
                float offsetX = (width - scaledWidth) / 2.0f;
                float offsetY = (height - scaledHeight) / 2.0f;
                
                destRect = D2D1::RectF(offsetX, offsetY, offsetX + scaledWidth, offsetY + scaledHeight);
                break;
            }
            case SCALE_CENTER: {
                // 居中显示，不缩放
                float offsetX = (width - bitmapSize.width) / 2.0f;
                float offsetY = (height - bitmapSize.height) / 2.0f;
                
                destRect = D2D1::RectF(offsetX, offsetY, offsetX + bitmapSize.width, offsetY + bitmapSize.height);
                break;
            }
            default:
                destRect = D2D1::RectF(0, 0, width, height);
                break;
        }
        
        // 绘制图片，应用透明度
        rt->DrawBitmap(
            state->bitmap,
            destRect,
            state->opacity,
            D2D1_BITMAP_INTERPOLATION_MODE_LINEAR,
            nullptr
        );
    }
}

// 图片框窗口过程
LRESULT CALLBACK PictureBoxProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData) {
    PictureBoxState* state = (PictureBoxState*)dwRefData;
    
    switch (msg) {
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            
            // 创建D2D渲染目标
            ID2D1HwndRenderTarget* rt = nullptr;
            D2D1_RENDER_TARGET_PROPERTIES props = D2D1::RenderTargetProperties();
            D2D1_HWND_RENDER_TARGET_PROPERTIES hwndProps = D2D1::HwndRenderTargetProperties(
                hwnd,
                D2D1::SizeU(state->width, state->height)
            );
            
            if (g_d2d_factory) {
                g_d2d_factory->CreateHwndRenderTarget(props, hwndProps, &rt);
            }
            
            if (rt) {
                rt->BeginDraw();
                rt->Clear(D2D1::ColorF(D2D1::ColorF::White, 0.0f));
                
                // ⚠️ 关键修复：每次都从WIC源重新创建位图，确保位图与当前渲染目标兼容
                if (state->wic_source) {
                    // 释放旧位图（如果存在）
                    if (state->bitmap) {
                        state->bitmap->Release();
                        state->bitmap = nullptr;
                    }
                    // 从WIC源创建新位图
                    HRESULT hr = rt->CreateBitmapFromWicBitmap(state->wic_source, nullptr, &state->bitmap);
                    
                    // 调试输出
                    if (SUCCEEDED(hr) && state->bitmap) {
                        D2D1_SIZE_F size = state->bitmap->GetSize();
                        OutputDebugStringA("位图创建成功，尺寸: ");
                        char buf[256];
                        sprintf_s(buf, "%.0fx%.0f\n", size.width, size.height);
                        OutputDebugStringA(buf);
                    } else {
                        OutputDebugStringA("位图创建失败！\n");
                    }
                }
                
                DrawPictureBox(rt, g_dwrite_factory, state);
                
                rt->EndDraw();
                rt->Release();
            }
            
            EndPaint(hwnd, &ps);
            return 0;
        }
        
        case WM_LBUTTONDOWN: {
            if (state->enabled && state->callback) {
                state->callback(hwnd);
            }
            return 0;
        }
        
        case WM_NCDESTROY: {
            // 清理资源
            if (state->bitmap) {
                state->bitmap->Release();
                state->bitmap = nullptr;
            }
            if (state->wic_source) {
                state->wic_source->Release();
                state->wic_source = nullptr;
            }
            
            // 从全局map中移除
            g_pictureboxes.erase(hwnd);
            delete state;
            
            RemoveWindowSubclass(hwnd, PictureBoxProc, 0);
            return 0;
        }
    }
    
    return DefSubclassProc(hwnd, msg, wparam, lparam);
}

// 创建图片框
HWND __stdcall CreatePictureBox(
    HWND hParent,
    int x, int y, int width, int height,
    int scale_mode,
    UINT32 bg_color
) {
    if (!hParent) return nullptr;
    
    // 初始化D2D和DirectWrite（如果尚未初始化）
    if (!g_d2d_factory) {
        D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &g_d2d_factory);
    }
    if (!g_dwrite_factory) {
        DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory),
                           reinterpret_cast<IUnknown**>(&g_dwrite_factory));
    }
    // 初始化WIC工厂（如果尚未初始化）
    if (!g_wic_factory) {
        CoInitialize(nullptr);
        CoCreateInstance(
            CLSID_WICImagingFactory,
            nullptr,
            CLSCTX_INPROC_SERVER,
            IID_PPV_ARGS(&g_wic_factory)
        );
    }
    
    // 创建静态控件作为图片框容器
    HWND hwnd = CreateWindowExW(
        0,
        L"STATIC",
        L"",
        WS_CHILD | WS_VISIBLE | SS_NOTIFY,
        x, y, width, height,
        hParent,
        (HMENU)(INT_PTR)g_next_control_id++,
        GetModuleHandle(nullptr),
        nullptr
    );
    
    if (!hwnd) return nullptr;
    
    // 创建状态对象
    PictureBoxState* state = new PictureBoxState();
    state->hwnd = hwnd;
    state->parent = hParent;
    state->id = (int)(INT_PTR)GetMenu(hwnd);
    state->x = 0;  // 相对于控件自身
    state->y = 0;
    state->width = width;
    state->height = height;
    state->bitmap = nullptr;
    state->wic_source = nullptr;
    state->scale_mode = (ImageScaleMode)scale_mode;
    state->opacity = 1.0f;
    state->bg_color = bg_color ? bg_color : 0xFFF5F7FA;  // Element UI 浅色背景
    state->enabled = true;
    state->callback = nullptr;
    
    // 保存到全局map
    g_pictureboxes[hwnd] = state;
    
    // 子类化窗口
    SetWindowSubclass(hwnd, PictureBoxProc, 0, (DWORD_PTR)state);
    
    return hwnd;
}

// 从文件加载图片
BOOL __stdcall LoadImageFromFile(
    HWND hPictureBox,
    const unsigned char* file_path_bytes,
    int path_len
) {
    auto it = g_pictureboxes.find(hPictureBox);
    if (it == g_pictureboxes.end()) return FALSE;
    
    PictureBoxState* state = it->second;
    
    // 清除旧图片
    if (state->bitmap) {
        state->bitmap->Release();
        state->bitmap = nullptr;
    }
    if (state->wic_source) {
        state->wic_source->Release();
        state->wic_source = nullptr;
    }
    
    if (!g_wic_factory) return FALSE;
    
    // 转换文件路径
    std::wstring file_path = Utf8ToWide(file_path_bytes, path_len);
    
    // 使用WIC加载图片
    IWICBitmapDecoder* decoder = nullptr;
    HRESULT hr = g_wic_factory->CreateDecoderFromFilename(
        file_path.c_str(),
        nullptr,
        GENERIC_READ,
        WICDecodeMetadataCacheOnLoad,
        &decoder
    );
    
    if (FAILED(hr)) return FALSE;
    
    // 获取第一帧
    IWICBitmapFrameDecode* frame = nullptr;
    hr = decoder->GetFrame(0, &frame);
    decoder->Release();
    
    if (FAILED(hr)) return FALSE;
    
    // 转换为32bppPBGRA格式
    IWICFormatConverter* converter = nullptr;
    hr = g_wic_factory->CreateFormatConverter(&converter);
    
    if (SUCCEEDED(hr)) {
        hr = converter->Initialize(
            frame,
            GUID_WICPixelFormat32bppPBGRA,
            WICBitmapDitherTypeNone,
            nullptr,
            0.0,
            WICBitmapPaletteTypeMedianCut
        );
    }
    
    frame->Release();
    
    if (FAILED(hr)) {
        if (converter) converter->Release();
        return FALSE;
    }
    
    // ⚠️ 关键修复：只保存WIC源，不在这里创建D2D位图
    // D2D位图会在WM_PAINT时从WIC源创建，这样可以确保位图与渲染目标匹配
    state->wic_source = converter;
    
    // 触发重绘
    InvalidateRect(hPictureBox, nullptr, TRUE);
    
    return TRUE;
}

// 从内存加载图片
BOOL __stdcall LoadImageFromMemory(
    HWND hPictureBox,
    const unsigned char* image_data,
    int data_len
) {
    auto it = g_pictureboxes.find(hPictureBox);
    if (it == g_pictureboxes.end()) {
        OutputDebugStringA("LoadImageFromMemory: 图片框未找到\n");
        return FALSE;
    }
    
    PictureBoxState* state = it->second;
    
    // 清除旧图片
    if (state->bitmap) {
        state->bitmap->Release();
        state->bitmap = nullptr;
    }
    if (state->wic_source) {
        state->wic_source->Release();
        state->wic_source = nullptr;
    }
    
    if (!g_wic_factory) {
        OutputDebugStringA("LoadImageFromMemory: WIC工厂未初始化\n");
        return FALSE;
    }
    
    char buf[256];
    sprintf_s(buf, "LoadImageFromMemory: 数据长度=%d\n", data_len);
    OutputDebugStringA(buf);
    
    // 创建内存流
    IWICStream* stream = nullptr;
    HRESULT hr = g_wic_factory->CreateStream(&stream);
    if (FAILED(hr)) {
        OutputDebugStringA("LoadImageFromMemory: 创建流失败\n");
        return FALSE;
    }
    
    hr = stream->InitializeFromMemory((BYTE*)image_data, data_len);
    if (FAILED(hr)) {
        stream->Release();
        OutputDebugStringA("LoadImageFromMemory: 初始化流失败\n");
        return FALSE;
    }
    
    // 使用WIC解码图片
    IWICBitmapDecoder* decoder = nullptr;
    hr = g_wic_factory->CreateDecoderFromStream(
        stream,
        nullptr,
        WICDecodeMetadataCacheOnLoad,
        &decoder
    );
    
    stream->Release();
    
    if (FAILED(hr)) {
        OutputDebugStringA("LoadImageFromMemory: 创建解码器失败\n");
        return FALSE;
    }
    
    // 获取第一帧
    IWICBitmapFrameDecode* frame = nullptr;
    hr = decoder->GetFrame(0, &frame);
    decoder->Release();
    
    if (FAILED(hr)) {
        OutputDebugStringA("LoadImageFromMemory: 获取帧失败\n");
        return FALSE;
    }
    
    // 转换为32bppPBGRA格式
    IWICFormatConverter* converter = nullptr;
    hr = g_wic_factory->CreateFormatConverter(&converter);
    
    if (SUCCEEDED(hr)) {
        hr = converter->Initialize(
            frame,
            GUID_WICPixelFormat32bppPBGRA,
            WICBitmapDitherTypeNone,
            nullptr,
            0.0,
            WICBitmapPaletteTypeMedianCut
        );
    }
    
    frame->Release();
    
    if (FAILED(hr)) {
        if (converter) converter->Release();
        OutputDebugStringA("LoadImageFromMemory: 格式转换失败\n");
        return FALSE;
    }
    
    // 获取图片尺寸
    UINT width, height;
    converter->GetSize(&width, &height);
    sprintf_s(buf, "LoadImageFromMemory: WIC源创建成功，尺寸=%dx%d\n", width, height);
    OutputDebugStringA(buf);
    
    // 只保存WIC源，不创建D2D位图
    // D2D位图将在WM_PAINT中从WIC源创建，确保与当前渲染目标兼容
    state->wic_source = converter;
    
    // 刷新显示（TRUE表示擦除背景）
    InvalidateRect(hPictureBox, nullptr, TRUE);
    
    OutputDebugStringA("LoadImageFromMemory: 成功\n");
    return TRUE;
}

// 清除图片
void __stdcall ClearImage(HWND hPictureBox) {
    auto it = g_pictureboxes.find(hPictureBox);
    if (it == g_pictureboxes.end()) return;
    
    PictureBoxState* state = it->second;
    
    // 释放图片资源
    if (state->bitmap) {
        state->bitmap->Release();
        state->bitmap = nullptr;
    }
    if (state->wic_source) {
        state->wic_source->Release();
        state->wic_source = nullptr;
    }
    
    // 刷新显示
    InvalidateRect(hPictureBox, nullptr, FALSE);
}

// 设置图片透明度
void __stdcall SetImageOpacity(HWND hPictureBox, float opacity) {
    auto it = g_pictureboxes.find(hPictureBox);
    if (it == g_pictureboxes.end()) return;
    
    PictureBoxState* state = it->second;
    
    // 限制范围在0.0-1.0之间
    if (opacity < 0.0f) opacity = 0.0f;
    if (opacity > 1.0f) opacity = 1.0f;
    
    state->opacity = opacity;
    
    // 刷新显示
    InvalidateRect(hPictureBox, nullptr, FALSE);
}

// 设置图片框回调
void __stdcall SetPictureBoxCallback(HWND hPictureBox, PictureBoxCallback callback) {
    auto it = g_pictureboxes.find(hPictureBox);
    if (it == g_pictureboxes.end()) return;
    
    it->second->callback = callback;
}

// 启用/禁用图片框
void __stdcall EnablePictureBox(HWND hPictureBox, BOOL enable) {
    auto it = g_pictureboxes.find(hPictureBox);
    if (it == g_pictureboxes.end()) return;
    
    it->second->enabled = (enable != 0);
    InvalidateRect(hPictureBox, nullptr, FALSE);
}

// 显示/隐藏图片框
void __stdcall ShowPictureBox(HWND hPictureBox, BOOL show) {
    if (!hPictureBox) return;
    ShowWindow(hPictureBox, show ? SW_SHOW : SW_HIDE);
}

// 设置图片框位置和大小
void __stdcall SetPictureBoxBounds(HWND hPictureBox, int x, int y, int width, int height) {
    auto it = g_pictureboxes.find(hPictureBox);
    if (it == g_pictureboxes.end()) return;
    
    it->second->width = width;
    it->second->height = height;
    SetWindowPos(hPictureBox, nullptr, x, y, width, height, SWP_NOZORDER);
    InvalidateRect(hPictureBox, nullptr, FALSE);
}

// 设置图片框缩放模式
void __stdcall SetPictureBoxScaleMode(HWND hPictureBox, int scale_mode) {
    auto it = g_pictureboxes.find(hPictureBox);
    if (it == g_pictureboxes.end()) return;
    
    it->second->scale_mode = (ImageScaleMode)scale_mode;
    InvalidateRect(hPictureBox, nullptr, FALSE);
}

// 设置图片框背景色
void __stdcall SetPictureBoxBackgroundColor(HWND hPictureBox, UINT32 bg_color) {
    auto it = g_pictureboxes.find(hPictureBox);
    if (it == g_pictureboxes.end()) return;
    
    it->second->bg_color = bg_color;
    InvalidateRect(hPictureBox, nullptr, FALSE);
}
