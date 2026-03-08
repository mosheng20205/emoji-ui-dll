// D2D组合框实现 - 第1部分：基础结构和全局变量
// 支持彩色emoji的完全自定义组合框控件

#include "emoji_window.h"
#include <algorithm>

// 全局变量
std::map<HWND, D2DComboBoxState*> g_d2d_comboboxes;

// 窗口类名
static const wchar_t* D2D_COMBOBOX_CLASS = L"D2DComboBoxClass";
static const wchar_t* D2D_DROPDOWN_CLASS = L"D2DDropDownClass";
static bool g_d2d_combobox_class_registered = false;

// 常量定义
const int DEFAULT_BUTTON_WIDTH = 30;
const int DEFAULT_MAX_DROPDOWN_ITEMS = 10;
const int DEFAULT_ITEM_HEIGHT = 35;

// 颜色定义
const UINT32 COLOR_HOVER = 0xFFE8F4FD;      // 悬停背景色
const UINT32 COLOR_SELECT = 0xFFCCE8FF;     // 选中背景色
const UINT32 COLOR_BORDER = 0xFFCCCCCC;     // 边框颜色
const UINT32 COLOR_BUTTON = 0xFFF0F0F0;     // 按钮颜色
const UINT32 COLOR_BUTTON_HOVER = 0xFFE0E0E0; // 按钮悬停色
const UINT32 COLOR_BUTTON_PRESS = 0xFFD0D0D0; // 按钮按下色

// 前向声明
LRESULT CALLBACK D2DComboBoxWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK D2DDropDownWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
void DrawD2DComboBox(D2DComboBoxState* state);
void DrawD2DDropDown(D2DComboBoxState* state);
void ShowDropDown(D2DComboBoxState* state);
void HideDropDown(D2DComboBoxState* state);
void SelectItem(D2DComboBoxState* state, int index);

// 注册D2D组合框窗口类
static bool RegisterD2DComboBoxClass() {
    if (g_d2d_combobox_class_registered) {
        return true;
    }

    WNDCLASSEXW wc = {};
    wc.cbSize = sizeof(WNDCLASSEXW);
    wc.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
    wc.lpfnWndProc = D2DComboBoxWndProc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszClassName = D2D_COMBOBOX_CLASS;

    if (!RegisterClassExW(&wc)) {
        return false;
    }

    // 注册下拉列表窗口类
    wc.lpfnWndProc = D2DDropDownWndProc;
    wc.lpszClassName = D2D_DROPDOWN_CLASS;
    wc.style = CS_HREDRAW | CS_VREDRAW | CS_DROPSHADOW;
    wc.hbrBackground = NULL;

    if (!RegisterClassExW(&wc)) {
        return false;
    }

    g_d2d_combobox_class_registered = true;
    return true;
}

// 创建D2D组合框
extern "C" __declspec(dllexport) HWND __stdcall CreateD2DComboBox(
    HWND hParent,
    int x, int y, int width, int height,
    BOOL readonly,
    UINT32 fg_color,
    UINT32 bg_color,
    int item_height,
    const unsigned char* font_name_bytes,
    int font_name_len,
    int font_size,
    BOOL bold,
    BOOL italic,
    BOOL underline
) {
    if (!RegisterD2DComboBoxClass()) {
        return NULL;
    }

    // 创建主容器窗口
    HWND hwnd = CreateWindowExW(
        0,
        D2D_COMBOBOX_CLASS,
        L"",
        WS_CHILD | WS_VISIBLE,
        x, y, width, height,
        hParent,
        NULL,
        GetModuleHandle(NULL),
        NULL
    );

    if (!hwnd) {
        return NULL;
    }

    // 创建状态对象
    D2DComboBoxState* state = new D2DComboBoxState();
    state->hwnd = hwnd;
    state->parent = hParent;
    state->id = (int)(INT_PTR)hwnd;
    state->x = x;
    state->y = y;
    state->width = width;
    state->height = height;

    state->selected_index = -1;
    state->hovered_index = -1;
    state->scroll_offset = 0;
    state->dropdown_visible = false;
    state->readonly = readonly;
    state->enabled = true;
    state->button_hovered = false;
    state->button_pressed = false;
    state->item_height = (item_height > 0) ? item_height : DEFAULT_ITEM_HEIGHT;
    state->button_width = DEFAULT_BUTTON_WIDTH;
    state->max_dropdown_items = DEFAULT_MAX_DROPDOWN_ITEMS;
    state->fg_color = fg_color;
    state->bg_color = bg_color;
    state->select_color = COLOR_SELECT;
    state->hover_color = COLOR_HOVER;
    state->border_color = COLOR_BORDER;
    state->button_color = COLOR_BUTTON;
    state->callback = NULL;
    state->render_target = NULL;
    state->dwrite_factory = NULL;
    state->dropdown_hwnd = NULL;

    // 设置字体
    std::string font_name_str((const char*)font_name_bytes, font_name_len);
    state->font.font_name = std::wstring(font_name_str.begin(), font_name_str.end());
    state->font.font_size = font_size;
    state->font.bold = bold;
    state->font.italic = italic;
    state->font.underline = underline;

    // 创建D2D编辑框（占据除按钮外的所有空间）
    int edit_width = width - state->button_width;
    state->edit_hwnd = CreateD2DColorEmojiEditBox(
        hwnd, 0, 0, edit_width, height,
        (const unsigned char*)"", 0,
        fg_color, bg_color,
        font_name_bytes, font_name_len,
        font_size, bold, italic, underline,
        0, FALSE, readonly, FALSE, TRUE, TRUE
    );

    // 保存状态
    g_d2d_comboboxes[hwnd] = state;

    return hwnd;
}

// D2D组合框窗口过程
LRESULT CALLBACK D2DComboBoxWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    auto it = g_d2d_comboboxes.find(hwnd);
    if (it == g_d2d_comboboxes.end() && msg != WM_CREATE) {
        return DefWindowProcW(hwnd, msg, wParam, lParam);
    }

    D2DComboBoxState* state = (it != g_d2d_comboboxes.end()) ? it->second : NULL;

    switch (msg) {
    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        DrawD2DComboBox(state);
        EndPaint(hwnd, &ps);
        return 0;
    }

    case WM_LBUTTONDOWN: {
        if (!state->enabled) break;
        
        int x = LOWORD(lParam);
        int y = HIWORD(lParam);
        
        // 检查是否点击下拉按钮
        int button_x = state->width - state->button_width;
        if (x >= button_x) {
            state->button_pressed = true;
            InvalidateRect(hwnd, NULL, FALSE);
            SetCapture(hwnd);
        }
        break;
    }

    case WM_LBUTTONUP: {
        if (!state->enabled) break;
        
        if (state->button_pressed) {
            state->button_pressed = false;
            ReleaseCapture();
            
            int x = LOWORD(lParam);
            int button_x = state->width - state->button_width;
            if (x >= button_x) {
                // 切换下拉列表显示状态
                if (state->dropdown_visible) {
                    HideDropDown(state);
                } else {
                    ShowDropDown(state);
                }
            }
            InvalidateRect(hwnd, NULL, FALSE);
        }
        break;
    }

    case WM_MOUSEMOVE: {
        if (!state->enabled) break;
        
        int x = LOWORD(lParam);
        int button_x = state->width - state->button_width;
        bool was_hovered = state->button_hovered;
        state->button_hovered = (x >= button_x);
        
        if (was_hovered != state->button_hovered) {
            InvalidateRect(hwnd, NULL, FALSE);
        }
        break;
    }

    case WM_MOUSELEAVE: {
        if (state->button_hovered) {
            state->button_hovered = false;
            InvalidateRect(hwnd, NULL, FALSE);
        }
        break;
    }

    case WM_SIZE: {
        int new_width = LOWORD(lParam);
        int new_height = HIWORD(lParam);
        state->width = new_width;
        state->height = new_height;
        
        // 调整编辑框大小
        if (state->edit_hwnd) {
            int edit_width = new_width - state->button_width;
            SetWindowPos(state->edit_hwnd, NULL, 0, 0, edit_width, new_height,
                SWP_NOZORDER | SWP_NOMOVE);
        }
        break;
    }

    case WM_DESTROY: {
        // 清理资源
        if (state->dropdown_hwnd) {
            DestroyWindow(state->dropdown_hwnd);
        }
        if (state->render_target) {
            state->render_target->Release();
        }
        g_d2d_comboboxes.erase(hwnd);
        delete state;
        break;
    }

    default:
        return DefWindowProcW(hwnd, msg, wParam, lParam);
    }

    return 0;
}

// 绘制D2D组合框（主要是下拉按钮）
void DrawD2DComboBox(D2DComboBoxState* state) {
    if (!state) return;

    // 初始化D2D资源
    if (!state->render_target) {
        ID2D1Factory* factory = NULL;
        D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &factory);
        
        if (factory) {
            RECT rc;
            GetClientRect(state->hwnd, &rc);
            
            D2D1_RENDER_TARGET_PROPERTIES props = D2D1::RenderTargetProperties();
            D2D1_HWND_RENDER_TARGET_PROPERTIES hwndProps = D2D1::HwndRenderTargetProperties(
                state->hwnd,
                D2D1::SizeU(rc.right - rc.left, rc.bottom - rc.top)
            );
            
            factory->CreateHwndRenderTarget(props, hwndProps, &state->render_target);
            factory->Release();
        }
        
        DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory),
            reinterpret_cast<IUnknown**>(&state->dwrite_factory));
    }

    if (!state->render_target) return;

    state->render_target->BeginDraw();
    state->render_target->Clear(D2D1::ColorF(
        ((state->bg_color >> 16) & 0xFF) / 255.0f,
        ((state->bg_color >> 8) & 0xFF) / 255.0f,
        (state->bg_color & 0xFF) / 255.0f,
        ((state->bg_color >> 24) & 0xFF) / 255.0f
    ));

    // 绘制下拉按钮
    int button_x = state->width - state->button_width;
    D2D1_RECT_F button_rect = D2D1::RectF(
        (float)button_x, 0.0f,
        (float)state->width, (float)state->height
    );

    // 按钮背景色（根据状态）
    UINT32 btn_color = state->button_color;
    if (state->button_pressed) {
        btn_color = COLOR_BUTTON_PRESS;
    } else if (state->button_hovered) {
        btn_color = COLOR_BUTTON_HOVER;
    }

    ID2D1SolidColorBrush* brush = NULL;
    state->render_target->CreateSolidColorBrush(
        D2D1::ColorF(
            ((btn_color >> 16) & 0xFF) / 255.0f,
            ((btn_color >> 8) & 0xFF) / 255.0f,
            (btn_color & 0xFF) / 255.0f,
            1.0f
        ),
        &brush
    );

    if (brush) {
        state->render_target->FillRectangle(button_rect, brush);
        brush->Release();
    }

    // 绘制下拉箭头（▼）
    if (state->dwrite_factory) {
        IDWriteTextFormat* text_format = NULL;
        state->dwrite_factory->CreateTextFormat(
            L"Segoe UI Symbol",
            NULL,
            DWRITE_FONT_WEIGHT_NORMAL,
            DWRITE_FONT_STYLE_NORMAL,
            DWRITE_FONT_STRETCH_NORMAL,
            14.0f,
            L"",
            &text_format
        );

        if (text_format) {
            text_format->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
            text_format->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);

            ID2D1SolidColorBrush* text_brush = NULL;
            state->render_target->CreateSolidColorBrush(
                D2D1::ColorF(0.3f, 0.3f, 0.3f, 1.0f),
                &text_brush
            );

            if (text_brush) {
                const wchar_t* arrow = L"▼";
                state->render_target->DrawText(
                    arrow, 1,
                    text_format,
                    button_rect,
                    text_brush,
                    D2D1_DRAW_TEXT_OPTIONS_ENABLE_COLOR_FONT
                );
                text_brush->Release();
            }
            text_format->Release();
        }
    }

    // 绘制边框
    ID2D1SolidColorBrush* border_brush = NULL;
    state->render_target->CreateSolidColorBrush(
        D2D1::ColorF(
            ((state->border_color >> 16) & 0xFF) / 255.0f,
            ((state->border_color >> 8) & 0xFF) / 255.0f,
            (state->border_color & 0xFF) / 255.0f,
            1.0f
        ),
        &border_brush
    );

    if (border_brush) {
        D2D1_RECT_F border_rect = D2D1::RectF(
            0.5f, 0.5f,
            (float)state->width - 0.5f, (float)state->height - 0.5f
        );
        state->render_target->DrawRectangle(border_rect, border_brush, 1.0f);
        
        // 绘制按钮分隔线
        state->render_target->DrawLine(
            D2D1::Point2F((float)button_x, 0.0f),
            D2D1::Point2F((float)button_x, (float)state->height),
            border_brush, 1.0f
        );
        
        border_brush->Release();
    }

    state->render_target->EndDraw();
}

// 显示下拉列表
void ShowDropDown(D2DComboBoxState* state) {
    if (!state || state->items.empty()) return;

    // 计算下拉列表位置和大小
    RECT combo_rect;
    GetWindowRect(state->hwnd, &combo_rect);

    int visible_items = min((int)state->items.size(), state->max_dropdown_items);
    int dropdown_height = visible_items * state->item_height + 2; // +2 for border

    // 检查屏幕空间
    RECT screen_rect;
    SystemParametersInfo(SPI_GETWORKAREA, 0, &screen_rect, 0);
    
    int dropdown_y = combo_rect.bottom;
    if (dropdown_y + dropdown_height > screen_rect.bottom) {
        // 如果下方空间不够，显示在上方
        dropdown_y = combo_rect.top - dropdown_height;
    }

    // 创建下拉列表窗口
    if (!state->dropdown_hwnd) {
        state->dropdown_hwnd = CreateWindowExW(
            WS_EX_TOPMOST | WS_EX_TOOLWINDOW,
            D2D_DROPDOWN_CLASS,
            L"",
            WS_POPUP,
            combo_rect.left, dropdown_y,
            state->width, dropdown_height,
            state->hwnd,
            NULL,
            GetModuleHandle(NULL),
            NULL
        );
        
        if (state->dropdown_hwnd) {
            SetWindowLongPtr(state->dropdown_hwnd, GWLP_USERDATA, (LONG_PTR)state);
        }
    } else {
        SetWindowPos(state->dropdown_hwnd, HWND_TOPMOST,
            combo_rect.left, dropdown_y,
            state->width, dropdown_height,
            SWP_SHOWWINDOW);
    }

    state->dropdown_visible = true;
    ShowWindow(state->dropdown_hwnd, SW_SHOW);
    SetCapture(state->dropdown_hwnd);
}

// 隐藏下拉列表
void HideDropDown(D2DComboBoxState* state) {
    if (!state || !state->dropdown_visible) return;

    if (state->dropdown_hwnd) {
        ShowWindow(state->dropdown_hwnd, SW_HIDE);
        ReleaseCapture();
    }

    state->dropdown_visible = false;
    state->hovered_index = -1;
}

// 选择项目
void SelectItem(D2DComboBoxState* state, int index) {
    if (!state || index < -1 || index >= (int)state->items.size()) return;

    state->selected_index = index;

    // 更新编辑框文本
    if (index >= 0 && state->edit_hwnd) {
        std::wstring text = state->items[index];
        std::string utf8_text;
        int len = WideCharToMultiByte(CP_UTF8, 0, text.c_str(), -1, NULL, 0, NULL, NULL);
        if (len > 0) {
            utf8_text.resize(len - 1);
            WideCharToMultiByte(CP_UTF8, 0, text.c_str(), -1, &utf8_text[0], len, NULL, NULL);
        }
        
        SetD2DEditBoxText(state->edit_hwnd, 
            (const unsigned char*)utf8_text.c_str(), 
            (int)utf8_text.length());
    }

    // 触发回调
    if (state->callback) {
        state->callback(state->hwnd, index);
    }

    HideDropDown(state);
}

// 下拉列表窗口过程
LRESULT CALLBACK D2DDropDownWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    D2DComboBoxState* state = (D2DComboBoxState*)GetWindowLongPtr(hwnd, GWLP_USERDATA);

    switch (msg) {
    case WM_PAINT: {
        PAINTSTRUCT ps;
        BeginPaint(hwnd, &ps);
        DrawD2DDropDown(state);
        EndPaint(hwnd, &ps);
        return 0;
    }

    case WM_LBUTTONDOWN: {
        if (!state) break;
        
        int y = HIWORD(lParam);
        int index = (y + state->scroll_offset) / state->item_height;
        
        if (index >= 0 && index < (int)state->items.size()) {
            SelectItem(state, index);
        }
        return 0;
    }

    case WM_MOUSEMOVE: {
        if (!state) break;
        
        int y = HIWORD(lParam);
        int new_hovered = (y + state->scroll_offset) / state->item_height;
        
        if (new_hovered >= 0 && new_hovered < (int)state->items.size()) {
            if (new_hovered != state->hovered_index) {
                state->hovered_index = new_hovered;
                InvalidateRect(hwnd, NULL, FALSE);
            }
        }
        break;
    }

    case WM_MOUSEWHEEL: {
        if (!state) break;
        
        int delta = GET_WHEEL_DELTA_WPARAM(wParam);
        int scroll_lines = 3;
        
        RECT rc;
        GetClientRect(hwnd, &rc);
        int visible_height = rc.bottom - rc.top;
        int total_height = (int)state->items.size() * state->item_height;
        int max_scroll = max(0, total_height - visible_height);
        
        if (delta > 0) {
            state->scroll_offset = max(0, state->scroll_offset - scroll_lines * state->item_height);
        } else {
            state->scroll_offset = min(max_scroll, state->scroll_offset + scroll_lines * state->item_height);
        }
        
        InvalidateRect(hwnd, NULL, FALSE);
        return 0;
    }

    case WM_KILLFOCUS:
    case WM_ACTIVATE: {
        if (state && LOWORD(wParam) == WA_INACTIVE) {
            // 失去焦点时关闭下拉列表
            PostMessage(state->hwnd, WM_USER + 100, 0, 0); // 通知主窗口关闭
        }
        break;
    }

    case WM_KEYDOWN: {
        if (!state) break;
        
        switch (wParam) {
        case VK_UP:
            if (state->hovered_index > 0) {
                state->hovered_index--;
                InvalidateRect(hwnd, NULL, FALSE);
            }
            return 0;
            
        case VK_DOWN:
            if (state->hovered_index < (int)state->items.size() - 1) {
                state->hovered_index++;
                InvalidateRect(hwnd, NULL, FALSE);
            }
            return 0;
            
        case VK_RETURN:
            if (state->hovered_index >= 0) {
                SelectItem(state, state->hovered_index);
            }
            return 0;
            
        case VK_ESCAPE:
            HideDropDown(state);
            return 0;
        }
        break;
    }

    default:
        return DefWindowProcW(hwnd, msg, wParam, lParam);
    }

    return 0;
}

// 绘制下拉列表
void DrawD2DDropDown(D2DComboBoxState* state) {
    if (!state || !state->dropdown_hwnd) return;

    // 获取或创建渲染目标
    ID2D1HwndRenderTarget* rt = NULL;
    ID2D1Factory* factory = NULL;
    D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &factory);
    
    if (factory) {
        RECT rc;
        GetClientRect(state->dropdown_hwnd, &rc);
        
        D2D1_RENDER_TARGET_PROPERTIES props = D2D1::RenderTargetProperties();
        D2D1_HWND_RENDER_TARGET_PROPERTIES hwndProps = D2D1::HwndRenderTargetProperties(
            state->dropdown_hwnd,
            D2D1::SizeU(rc.right - rc.left, rc.bottom - rc.top)
        );
        
        factory->CreateHwndRenderTarget(props, hwndProps, &rt);
        factory->Release();
    }

    if (!rt) return;

    rt->BeginDraw();
    
    // 背景
    rt->Clear(D2D1::ColorF(
        ((state->bg_color >> 16) & 0xFF) / 255.0f,
        ((state->bg_color >> 8) & 0xFF) / 255.0f,
        (state->bg_color & 0xFF) / 255.0f,
        1.0f
    ));

    // 创建文本格式
    IDWriteTextFormat* text_format = NULL;
    if (state->dwrite_factory) {
        state->dwrite_factory->CreateTextFormat(
            state->font.font_name.c_str(),
            NULL,
            state->font.bold ? DWRITE_FONT_WEIGHT_BOLD : DWRITE_FONT_WEIGHT_NORMAL,
            state->font.italic ? DWRITE_FONT_STYLE_ITALIC : DWRITE_FONT_STYLE_NORMAL,
            DWRITE_FONT_STRETCH_NORMAL,
            (float)state->font.font_size,
            L"",
            &text_format
        );

        if (text_format) {
            text_format->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
            text_format->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
        }
    }

    // 绘制每个项目
    RECT rc;
    GetClientRect(state->dropdown_hwnd, &rc);
    int visible_height = rc.bottom - rc.top;
    
    int start_index = state->scroll_offset / state->item_height;
    int end_index = min((int)state->items.size(), 
        (state->scroll_offset + visible_height + state->item_height - 1) / state->item_height);

    for (int i = start_index; i < end_index; i++) {
        float y = (float)(i * state->item_height - state->scroll_offset);
        D2D1_RECT_F item_rect = D2D1::RectF(
            1.0f, y,
            (float)(state->width - 1), y + (float)state->item_height
        );

        // 绘制背景
        UINT32 bg_color = state->bg_color;
        if (i == state->selected_index) {
            bg_color = state->select_color;
        } else if (i == state->hovered_index) {
            bg_color = state->hover_color;
        }

        ID2D1SolidColorBrush* bg_brush = NULL;
        rt->CreateSolidColorBrush(
            D2D1::ColorF(
                ((bg_color >> 16) & 0xFF) / 255.0f,
                ((bg_color >> 8) & 0xFF) / 255.0f,
                (bg_color & 0xFF) / 255.0f,
                1.0f
            ),
            &bg_brush
        );

        if (bg_brush) {
            rt->FillRectangle(item_rect, bg_brush);
            bg_brush->Release();
        }

        // 绘制文本（支持彩色emoji）
        if (text_format) {
            ID2D1SolidColorBrush* text_brush = NULL;
            rt->CreateSolidColorBrush(
                D2D1::ColorF(
                    ((state->fg_color >> 16) & 0xFF) / 255.0f,
                    ((state->fg_color >> 8) & 0xFF) / 255.0f,
                    (state->fg_color & 0xFF) / 255.0f,
                    1.0f
                ),
                &text_brush
            );

            if (text_brush) {
                D2D1_RECT_F text_rect = D2D1::RectF(
                    item_rect.left + 5.0f, item_rect.top,
                    item_rect.right - 5.0f, item_rect.bottom
                );
                
                rt->DrawText(
                    state->items[i].c_str(),
                    (UINT32)state->items[i].length(),
                    text_format,
                    text_rect,
                    text_brush,
                    D2D1_DRAW_TEXT_OPTIONS_ENABLE_COLOR_FONT  // 关键：支持彩色emoji
                );
                text_brush->Release();
            }
        }
    }

    // 绘制边框
    ID2D1SolidColorBrush* border_brush = NULL;
    rt->CreateSolidColorBrush(
        D2D1::ColorF(
            ((state->border_color >> 16) & 0xFF) / 255.0f,
            ((state->border_color >> 8) & 0xFF) / 255.0f,
            (state->border_color & 0xFF) / 255.0f,
            1.0f
        ),
        &border_brush
    );

    if (border_brush) {
        D2D1_RECT_F border_rect = D2D1::RectF(
            0.5f, 0.5f,
            (float)state->width - 0.5f, (float)visible_height - 0.5f
        );
        rt->DrawRectangle(border_rect, border_brush, 1.0f);
        border_brush->Release();
    }

    if (text_format) {
        text_format->Release();
    }

    rt->EndDraw();
    rt->Release();
}

// ========== API函数实现 ==========

// 添加项目
extern "C" __declspec(dllexport) int __stdcall AddD2DComboItem(
    HWND hComboBox,
    const unsigned char* text_bytes,
    int text_len
) {
    auto it = g_d2d_comboboxes.find(hComboBox);
    if (it == g_d2d_comboboxes.end()) return -1;

    D2DComboBoxState* state = it->second;
    
    // UTF-8转换为宽字符
    std::string utf8_str((const char*)text_bytes, text_len);
    int wlen = MultiByteToWideChar(CP_UTF8, 0, utf8_str.c_str(), -1, NULL, 0);
    if (wlen <= 0) return -1;

    std::wstring wstr(wlen - 1, L'\0');
    MultiByteToWideChar(CP_UTF8, 0, utf8_str.c_str(), -1, &wstr[0], wlen);

    state->items.push_back(wstr);
    return (int)state->items.size() - 1;
}

// 移除项目
extern "C" __declspec(dllexport) void __stdcall RemoveD2DComboItem(
    HWND hComboBox,
    int index
) {
    auto it = g_d2d_comboboxes.find(hComboBox);
    if (it == g_d2d_comboboxes.end()) return;

    D2DComboBoxState* state = it->second;
    if (index < 0 || index >= (int)state->items.size()) return;

    state->items.erase(state->items.begin() + index);
    
    if (state->selected_index == index) {
        state->selected_index = -1;
    } else if (state->selected_index > index) {
        state->selected_index--;
    }
}

// 清空
extern "C" __declspec(dllexport) void __stdcall ClearD2DComboBox(
    HWND hComboBox
) {
    auto it = g_d2d_comboboxes.find(hComboBox);
    if (it == g_d2d_comboboxes.end()) return;

    D2DComboBoxState* state = it->second;
    state->items.clear();
    state->selected_index = -1;
    state->hovered_index = -1;
    state->scroll_offset = 0;
    
    if (state->edit_hwnd) {
        SetD2DEditBoxText(state->edit_hwnd, (const unsigned char*)"", 0);
    }
}

// 获取选中项索引
extern "C" __declspec(dllexport) int __stdcall GetD2DComboSelectedIndex(
    HWND hComboBox
) {
    auto it = g_d2d_comboboxes.find(hComboBox);
    if (it == g_d2d_comboboxes.end()) return -1;

    return it->second->selected_index;
}

// 设置选中项索引
extern "C" __declspec(dllexport) void __stdcall SetD2DComboSelectedIndex(
    HWND hComboBox,
    int index
) {
    auto it = g_d2d_comboboxes.find(hComboBox);
    if (it == g_d2d_comboboxes.end()) return;

    D2DComboBoxState* state = it->second;
    if (index < -1 || index >= (int)state->items.size()) return;

    SelectItem(state, index);
}

// 获取项目数量
extern "C" __declspec(dllexport) int __stdcall GetD2DComboItemCount(
    HWND hComboBox
) {
    auto it = g_d2d_comboboxes.find(hComboBox);
    if (it == g_d2d_comboboxes.end()) return 0;

    return (int)it->second->items.size();
}

// 获取项目文本
extern "C" __declspec(dllexport) int __stdcall GetD2DComboItemText(
    HWND hComboBox,
    int index,
    unsigned char* buffer,
    int buffer_size
) {
    auto it = g_d2d_comboboxes.find(hComboBox);
    if (it == g_d2d_comboboxes.end()) return 0;

    D2DComboBoxState* state = it->second;
    if (index < 0 || index >= (int)state->items.size()) return 0;

    std::wstring wstr = state->items[index];
    int len = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, NULL, 0, NULL, NULL);
    if (len <= 0) return 0;

    if (buffer && buffer_size > 0) {
        int actual_len = min(len - 1, buffer_size - 1);
        WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, (char*)buffer, actual_len + 1, NULL, NULL);
        return actual_len;
    }

    return len - 1;
}

// 获取编辑框文本
extern "C" __declspec(dllexport) int __stdcall GetD2DComboText(
    HWND hComboBox,
    unsigned char* buffer,
    int buffer_size
) {
    auto it = g_d2d_comboboxes.find(hComboBox);
    if (it == g_d2d_comboboxes.end()) return 0;

    D2DComboBoxState* state = it->second;
    if (!state->edit_hwnd) return 0;

    return GetD2DEditBoxText(state->edit_hwnd, buffer, buffer_size);
}

// 设置编辑框文本
extern "C" __declspec(dllexport) void __stdcall SetD2DComboText(
    HWND hComboBox,
    const unsigned char* text_bytes,
    int text_len
) {
    auto it = g_d2d_comboboxes.find(hComboBox);
    if (it == g_d2d_comboboxes.end()) return;

    D2DComboBoxState* state = it->second;
    if (!state->edit_hwnd) return;

    SetD2DEditBoxText(state->edit_hwnd, text_bytes, text_len);
}

// 设置回调
extern "C" __declspec(dllexport) void __stdcall SetD2DComboBoxCallback(
    HWND hComboBox,
    ComboBoxCallback callback
) {
    auto it = g_d2d_comboboxes.find(hComboBox);
    if (it == g_d2d_comboboxes.end()) return;

    it->second->callback = callback;
}

// 启用/禁用
extern "C" __declspec(dllexport) void __stdcall EnableD2DComboBox(
    HWND hComboBox,
    BOOL enable
) {
    auto it = g_d2d_comboboxes.find(hComboBox);
    if (it == g_d2d_comboboxes.end()) return;

    D2DComboBoxState* state = it->second;
    state->enabled = enable;
    
    if (state->edit_hwnd) {
        EnableWindow(state->edit_hwnd, enable);
    }
    
    InvalidateRect(hComboBox, NULL, FALSE);
}

// 显示/隐藏
extern "C" __declspec(dllexport) void __stdcall ShowD2DComboBox(
    HWND hComboBox,
    BOOL show
) {
    ShowWindow(hComboBox, show ? SW_SHOW : SW_HIDE);
}

// 设置位置和大小
extern "C" __declspec(dllexport) void __stdcall SetD2DComboBoxBounds(
    HWND hComboBox,
    int x, int y, int width, int height
) {
    auto it = g_d2d_comboboxes.find(hComboBox);
    if (it == g_d2d_comboboxes.end()) return;

    D2DComboBoxState* state = it->second;
    state->x = x;
    state->y = y;
    state->width = width;
    state->height = height;

    SetWindowPos(hComboBox, NULL, x, y, width, height, SWP_NOZORDER);
}
