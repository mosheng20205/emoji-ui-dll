#include "log.h"
#include "emoji_window.h"
#include "treeview_window.h"  // WM_EW_TABPAGE_VISIBLE
#include "submenu_window.h"
#include <algorithm>
#include <windowsx.h>  // For GET_X_LPARAM and GET_Y_LPARAM
#include <set>

// Global variables
std::map<HWND, WindowState*> g_windows;
std::map<HWND, MsgBoxState*> g_msgboxes;
std::map<HWND, TabControlState*> g_tab_controls;
std::map<HWND, EditBoxState*> g_editboxes;
std::map<HWND, D2DEditBoxState*> g_d2d_editboxes;  // D2D自定义绘制编辑框
std::map<HWND, LabelState*> g_labels;
std::map<HWND, CheckBoxState*> g_checkboxes;
std::map<HWND, ProgressBarState*> g_progressbars;
std::map<HWND, PictureBoxState*> g_pictureboxes;
std::map<HWND, RadioButtonState*> g_radiobuttons;
std::map<int, std::vector<HWND>> g_radio_groups;  // 分组管理
std::map<HWND, ListBoxState*> g_listboxes;
std::map<HWND, ComboBoxState*> g_comboboxes;
std::map<HWND, HotKeyState*> g_hotkeys;
std::map<HWND, GroupBoxState*> g_groupboxes;
std::map<HWND, DataGridViewState*> g_datagrids;
std::map<HWND, LayoutManager*> g_layout_managers;
std::map<HWND, MenuBarState*> g_menubars;
std::map<HWND, PopupMenuState*> g_popup_menus;
std::map<HWND, HWND> g_control_popup_menu_bindings;  // 控件 → 弹出菜单绑定
ButtonClickCallback g_button_callback = nullptr;
WindowResizeCallback g_window_resize_callback = nullptr;
WindowCloseCallback g_window_close_callback = nullptr;

// ========== 主题系统全局变量 ==========
Theme g_light_theme;
Theme g_dark_theme;
Theme* g_current_theme = nullptr;
ThemeChangedCallback g_theme_changed_callback = nullptr;
static bool g_themes_initialized = false;
ID2D1Factory* g_d2d_factory = nullptr;
IDWriteFactory* g_dwrite_factory = nullptr;
IWICImagingFactory* g_wic_factory = nullptr;  // WIC工厂
int g_next_control_id = 10000;  // 控件ID起始值
// 标记 run_message_loop 是否正在运行
// 只有在 DLL 自己的消息循环里才应该 PostQuitMessage，
// 从易语言 _窗口1_将被销毁 调用 destroy_window 时不能 PostQuitMessage，
// 否则易语言自己的消息循环会被误杀。
static bool g_own_message_loop_running = false;
static HWND g_message_loop_main_window = nullptr;

// Forward declarations
LRESULT CALLBACK TabControlParentSubclassProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData);
LRESULT CALLBACK TabControlSubclassProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData);
static void EnsureThemesInitialized();
static int VisibleIndexToPageIndex(TabControlState* state, int visibleIdx);

// ========== 主题辅助函数 ==========
// 获取主题颜色，如果主题未初始化则返回默认值
static inline UINT32 ThemeColor_Primary() {
    if (g_current_theme) return g_current_theme->colors.primary;
    return 0xFF409EFF;
}
static inline UINT32 ThemeColor_TextPrimary() {
    if (g_current_theme) return g_current_theme->colors.text_primary;
    return 0xFF303133;
}
static inline UINT32 ThemeColor_TextRegular() {
    if (g_current_theme) return g_current_theme->colors.text_regular;
    return 0xFF606266;
}
static inline UINT32 ThemeColor_TextPlaceholder() {
    if (g_current_theme) return g_current_theme->colors.text_placeholder;
    return 0xFFC0C4CC;
}
static inline UINT32 ThemeColor_BorderBase() {
    if (g_current_theme) return g_current_theme->colors.border_base;
    return 0xFFDCDFE6;
}
static inline UINT32 ThemeColor_BorderLight() {
    if (g_current_theme) return g_current_theme->colors.border_light;
    return 0xFFE4E7ED;
}
static inline UINT32 ThemeColor_BorderLighter() {
    if (g_current_theme) return g_current_theme->colors.border_lighter;
    return 0xFFEBEEF5;
}
static inline UINT32 ThemeColor_Background() {
    if (g_current_theme) return g_current_theme->colors.background;
    return 0xFFFFFFFF;
}
static inline UINT32 ThemeColor_BackgroundLight() {
    if (g_current_theme) return g_current_theme->colors.background_light;
    return 0xFFF5F7FA;
}
static inline UINT32 ThemeColor_Success() {
    if (g_current_theme) return g_current_theme->colors.success;
    return 0xFF67C23A;
}
static inline UINT32 ThemeColor_Warning() {
    if (g_current_theme) return g_current_theme->colors.warning;
    return 0xFFE6A23C;
}
static inline UINT32 ThemeColor_Danger() {
    if (g_current_theme) return g_current_theme->colors.danger;
    return 0xFFF56C6C;
}
static inline UINT32 ThemeColor_Info() {
    if (g_current_theme) return g_current_theme->colors.info;
    return 0xFF909399;
}
// 根据基色生成悬停色（变亮20%）
static inline UINT32 ThemeColor_Hover(UINT32 base) {
    int a = (base >> 24) & 0xFF;
    int r = (base >> 16) & 0xFF;
    int g = (base >> 8) & 0xFF;
    int b = base & 0xFF;
    r = min(255, r + (255 - r) * 20 / 100);
    g = min(255, g + (255 - g) * 20 / 100);
    b = min(255, b + (255 - b) * 20 / 100);
    return (a << 24) | (r << 16) | (g << 8) | b;
}
// 根据基色生成按下色（变暗10%）
static inline UINT32 ThemeColor_Pressed(UINT32 base) {
    int a = (base >> 24) & 0xFF;
    int r = (base >> 16) & 0xFF;
    int g = (base >> 8) & 0xFF;
    int b = base & 0xFF;
    r = max(0, r * 90 / 100);
    g = max(0, g * 90 / 100);
    b = max(0, b * 90 / 100);
    return (a << 24) | (r << 16) | (g << 8) | b;
}
// 根据基色生成浅色背景（混合白色90%）
static inline UINT32 ThemeColor_LightBg(UINT32 base) {
    int a = (base >> 24) & 0xFF;
    int r = (base >> 16) & 0xFF;
    int g = (base >> 8) & 0xFF;
    int b = base & 0xFF;
    r = r + (255 - r) * 90 / 100;
    g = g + (255 - g) * 90 / 100;
    b = b + (255 - b) * 90 / 100;
    return (a << 24) | (r << 16) | (g << 8) | b;
}
static inline float ThemeSize_BorderRadius() {
    if (g_current_theme) return g_current_theme->sizes.border_radius;
    return 4.0f;
}
static inline float ThemeSize_BorderWidth() {
    if (g_current_theme) return g_current_theme->sizes.border_width;
    return 1.0f;
}

// ========== 主题颜色索引解析 ==========
// 易语言侧传入的颜色值如果是 0~14，表示主题颜色索引，需要在渲染时动态解析为当前主题颜色
// 索引: 0=primary, 1=success, 2=warning, 3=danger, 4=info,
//       5=text_primary, 6=text_regular, 7=text_secondary, 8=text_placeholder,
//       9=border_base, 10=border_light, 11=border_lighter, 12=border_extra_light,
//       13=background, 14=background_light
static inline bool IsThemeColorIndex(UINT32 color) {
    return color <= 14;
}

static inline UINT32 ResolveThemeColor(UINT32 color) {
    if (!IsThemeColorIndex(color)) return color;
    EnsureThemesInitialized();
    const ThemeColors& c = g_current_theme->colors;
    switch (color) {
        case 0:  return c.primary;
        case 1:  return c.success;
        case 2:  return c.warning;
        case 3:  return c.danger;
        case 4:  return c.info;
        case 5:  return c.text_primary;
        case 6:  return c.text_regular;
        case 7:  return c.text_secondary;
        case 8:  return c.text_placeholder;
        case 9:  return c.border_base;
        case 10: return c.border_light;
        case 11: return c.border_lighter;
        case 12: return c.border_extra_light;
        case 13: return c.background;
        case 14: return c.background_light;
        default: return color;
    }
}

// 鼠标进入跟踪集合
static std::set<HWND> g_mouse_tracking_set;

// 通用事件消息处理辅助函数（前向声明+定义）
static bool HandleCommonEvents(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam, EventCallbacks* ec) {
    if (!ec) return false;

    switch (msg) {
    case WM_MOUSEMOVE:
        if (g_mouse_tracking_set.find(hwnd) == g_mouse_tracking_set.end()) {
            g_mouse_tracking_set.insert(hwnd);
            TRACKMOUSEEVENT tme = {};
            tme.cbSize = sizeof(tme);
            tme.dwFlags = TME_LEAVE;
            tme.hwndTrack = hwnd;
            TrackMouseEvent(&tme);
            if (ec->on_mouse_enter) {
                ec->on_mouse_enter(hwnd);
            }
        }
        break;

    case WM_MOUSELEAVE:
        g_mouse_tracking_set.erase(hwnd);
        if (ec->on_mouse_leave) {
            ec->on_mouse_leave(hwnd);
        }
        break;

    case WM_LBUTTONDBLCLK:
        if (ec->on_double_click) {
            int x = GET_X_LPARAM(lparam);
            int y = GET_Y_LPARAM(lparam);
            ec->on_double_click(hwnd, x, y);
            return true;
        }
        break;

    case WM_RBUTTONUP:
        if (ec->on_right_click) {
            int x = GET_X_LPARAM(lparam);
            int y = GET_Y_LPARAM(lparam);
            ec->on_right_click(hwnd, x, y);
            return true;
        }
        break;

    case WM_SETFOCUS:
        if (ec->on_focus) {
            ec->on_focus(hwnd);
        }
        break;

    case WM_KILLFOCUS:
        if (ec->on_blur) {
            ec->on_blur(hwnd);
        }
        break;

    case WM_KEYDOWN:
        if (ec->on_key_down) {
            int shift = (GetKeyState(VK_SHIFT) & 0x8000) ? 1 : 0;
            int ctrl = (GetKeyState(VK_CONTROL) & 0x8000) ? 1 : 0;
            int alt = (GetKeyState(VK_MENU) & 0x8000) ? 1 : 0;
            ec->on_key_down(hwnd, (int)wparam, shift, ctrl, alt);
        }
        break;

    case WM_KEYUP:
        if (ec->on_key_up) {
            int shift = (GetKeyState(VK_SHIFT) & 0x8000) ? 1 : 0;
            int ctrl = (GetKeyState(VK_CONTROL) & 0x8000) ? 1 : 0;
            int alt = (GetKeyState(VK_MENU) & 0x8000) ? 1 : 0;
            ec->on_key_up(hwnd, (int)wparam, shift, ctrl, alt);
        }
        break;

    case WM_CHAR:
        if (ec->on_char) {
            ec->on_char(hwnd, (int)wparam);
        }
        break;

    case WM_NCDESTROY:
        g_mouse_tracking_set.erase(hwnd);
        break;
    }

    return false;
}

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
    float a = ((color >> 24) & 0xFF) / 255.0f;
    // 如果alpha为0，默认为不透明（兼容易语言RGB颜色常量没有alpha通道）
    if (a == 0.0f) a = 1.0f;
    return D2D1::ColorF(
        ((color >> 16) & 0xFF) / 255.0f,
        ((color >> 8) & 0xFF) / 255.0f,
        (color & 0xFF) / 255.0f,
        a
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

// ========== 自定义标题栏辅助函数 ==========

struct WindowCreateInit {
    std::wstring title;
    UINT32 titlebar_color = 0;
    UINT32 client_bg_color = 0;   // 客户区背景色，0=纯白(ThemeColor_Background)
    bool custom_titlebar = true;
};

enum TitleBarButtonType {
    TITLEBAR_BUTTON_NONE = 0,
    TITLEBAR_BUTTON_MINIMIZE,
    TITLEBAR_BUTTON_MAXIMIZE,
    TITLEBAR_BUTTON_CLOSE
};

static HICON ResolveWindowIcon(HWND hwnd) {
    if (!hwnd) return nullptr;

    HICON hicon = reinterpret_cast<HICON>(SendMessage(hwnd, WM_GETICON, ICON_SMALL, 0));
    if (hicon) return hicon;

    hicon = reinterpret_cast<HICON>(SendMessage(hwnd, WM_GETICON, ICON_BIG, 0));
    if (hicon) return hicon;

    hicon = reinterpret_cast<HICON>(GetClassLongPtr(hwnd, GCLP_HICONSM));
    if (hicon) return hicon;

    hicon = reinterpret_cast<HICON>(GetClassLongPtr(hwnd, GCLP_HICON));
    if (hicon) return hicon;

    return nullptr;
}

static bool EnsureWicFactoryInitialized() {
    if (g_wic_factory) return true;

    CoInitialize(nullptr);
    HRESULT hr = CoCreateInstance(
        CLSID_WICImagingFactory,
        nullptr,
        CLSCTX_INPROC_SERVER,
        IID_PPV_ARGS(&g_wic_factory)
    );
    return SUCCEEDED(hr) && g_wic_factory != nullptr;
}

static ID2D1Bitmap* CreateBitmapFromHICONForD2D(ID2D1HwndRenderTarget* rt, HICON hicon) {
    if (!rt || !hicon || !EnsureWicFactoryInitialized()) return nullptr;

    IWICBitmap* wic_bitmap = nullptr;
    HRESULT hr = g_wic_factory->CreateBitmapFromHICON(hicon, &wic_bitmap);
    if (FAILED(hr) || !wic_bitmap) return nullptr;

    IWICFormatConverter* converter = nullptr;
    hr = g_wic_factory->CreateFormatConverter(&converter);
    if (SUCCEEDED(hr) && converter) {
        hr = converter->Initialize(
            wic_bitmap,
            GUID_WICPixelFormat32bppPBGRA,
            WICBitmapDitherTypeNone,
            nullptr,
            0.0,
            WICBitmapPaletteTypeMedianCut
        );
    }

    ID2D1Bitmap* bitmap = nullptr;
    if (SUCCEEDED(hr) && converter) {
        hr = rt->CreateBitmapFromWicBitmap(converter, nullptr, &bitmap);
    }

    if (converter) converter->Release();
    wic_bitmap->Release();

    if (FAILED(hr)) {
        if (bitmap) bitmap->Release();
        return nullptr;
    }

    return bitmap;
}

static void GetTitleBarButtonRects(WindowState* state, RECT* min_rect, RECT* max_rect, RECT* close_rect) {
    RECT rc = {};
    GetClientRect(state->hwnd, &rc);
    int btn_w = 44;
    int btn_h = state->titlebar_height;

    if (close_rect) {
        close_rect->left = rc.right - btn_w;
        close_rect->top = 0;
        close_rect->right = rc.right;
        close_rect->bottom = btn_h;
    }
    if (max_rect) {
        max_rect->left = rc.right - btn_w * 2;
        max_rect->top = 0;
        max_rect->right = rc.right - btn_w;
        max_rect->bottom = btn_h;
    }
    if (min_rect) {
        min_rect->left = rc.right - btn_w * 3;
        min_rect->top = 0;
        min_rect->right = rc.right - btn_w * 2;
        min_rect->bottom = btn_h;
    }
}

static TitleBarButtonType HitTestTitleBarButton(WindowState* state, POINT pt) {
    RECT min_rect = {}, max_rect = {}, close_rect = {};
    GetTitleBarButtonRects(state, &min_rect, &max_rect, &close_rect);
    if (PtInRect(&close_rect, pt)) return TITLEBAR_BUTTON_CLOSE;
    if (PtInRect(&max_rect, pt)) return TITLEBAR_BUTTON_MAXIMIZE;
    if (PtInRect(&min_rect, pt)) return TITLEBAR_BUTTON_MINIMIZE;
    return TITLEBAR_BUTTON_NONE;
}

static TitleBarButtonType HitTestTitleBarButtonFromScreen(WindowState* state, LPARAM lparam) {
    POINT pt = { GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam) };
    ScreenToClient(state->hwnd, &pt);
    return HitTestTitleBarButton(state, pt);
}

// 获取父窗口的标题栏偏移量（如果有自定义标题栏）
int GetTitleBarOffset(HWND hParent) {
    auto it = g_windows.find(hParent);
    if (it != g_windows.end() && it->second->custom_titlebar) {
        return it->second->titlebar_height;
    }
    return 0;
}

// 获取窗口标题栏图标（用于自绘标题栏）
static HICON GetWindowTitleBarIcon(HWND hwnd) {
    // 1. 尝试获取小图标
    HICON icon = (HICON)SendMessage(hwnd, WM_GETICON, ICON_SMALL, 0);
    if (icon) return icon;

    // 2. 尝试获取大图标
    icon = (HICON)SendMessage(hwnd, WM_GETICON, ICON_BIG, 0);
    if (icon) return icon;

    // 3. 尝试获取窗口类小图标
    icon = (HICON)GetClassLongPtr(hwnd, GCLP_HICONSM);
    if (icon) return icon;

    // 4. 尝试获取窗口类大图标
    icon = (HICON)GetClassLongPtr(hwnd, GCLP_HICON);
    if (icon) return icon;

    return nullptr;
}

// 绘制自定义标题栏（D2D彩色emoji支持）
void DrawWindowTitleBar(ID2D1HwndRenderTarget* rt, IDWriteFactory* factory, WindowState* state) {
    if (!state->custom_titlebar) return;

    RECT rc;
    GetClientRect(state->hwnd, &rc);
    float width = (float)(rc.right - rc.left);
    float tb_h = (float)state->titlebar_height;

    // 1. 标题栏背景色
    UINT32 bg = state->titlebar_color;
    if (bg == 0) bg = ThemeColor_BackgroundLight();  // 跟随主题
    bg &= 0x00FFFFFF;  // 去掉alpha

    ID2D1SolidColorBrush* bg_brush = nullptr;
    rt->CreateSolidColorBrush(D2D1::ColorF(
        ((bg >> 16) & 0xFF) / 255.0f,
        ((bg >> 8) & 0xFF) / 255.0f,
        (bg & 0xFF) / 255.0f,
        1.0f), &bg_brush);
    rt->FillRectangle(D2D1::RectF(0, 0, width, tb_h), bg_brush);
    bg_brush->Release();

    // 1.5 绘制标题栏按钮（最小化/最大化/关闭）
    RECT min_rect = {}, max_rect = {}, close_rect = {};
    GetTitleBarButtonRects(state, &min_rect, &max_rect, &close_rect);

    UINT32 hover_bg = IsDarkMode() ? LightenColor(bg | 0xFF000000, 1.18f) : DarkenColor(bg | 0xFF000000, 0.92f);
    hover_bg &= 0x00FFFFFF;

    auto fill_rect = [&](RECT r, UINT32 color) {
        ID2D1SolidColorBrush* b = nullptr;
        rt->CreateSolidColorBrush(D2D1::ColorF(
            ((color >> 16) & 0xFF) / 255.0f,
            ((color >> 8) & 0xFF) / 255.0f,
            (color & 0xFF) / 255.0f,
            1.0f), &b);
        rt->FillRectangle(D2D1::RectF((FLOAT)r.left, (FLOAT)r.top, (FLOAT)r.right, (FLOAT)r.bottom), b);
        b->Release();
    };

    if (state->hovered_titlebar_button == TITLEBAR_BUTTON_MINIMIZE) fill_rect(min_rect, hover_bg);
    if (state->hovered_titlebar_button == TITLEBAR_BUTTON_MAXIMIZE) fill_rect(max_rect, hover_bg);
    if (state->hovered_titlebar_button == TITLEBAR_BUTTON_CLOSE) fill_rect(close_rect, 0xE81123);

    ID2D1SolidColorBrush* icon_brush = nullptr;
    UINT32 icon_fg = ThemeColor_TextPrimary() & 0x00FFFFFF;
    rt->CreateSolidColorBrush(D2D1::ColorF(
        ((icon_fg >> 16) & 0xFF) / 255.0f,
        ((icon_fg >> 8) & 0xFF) / 255.0f,
        (icon_fg & 0xFF) / 255.0f,
        1.0f), &icon_brush);

    ID2D1SolidColorBrush* close_brush = nullptr;
    UINT32 close_icon_fg = (state->hovered_titlebar_button == TITLEBAR_BUTTON_CLOSE) ? 0xFFFFFF : 0xE81123;
    rt->CreateSolidColorBrush(D2D1::ColorF(
        ((close_icon_fg >> 16) & 0xFF) / 255.0f,
        ((close_icon_fg >> 8) & 0xFF) / 255.0f,
        (close_icon_fg & 0xFF) / 255.0f,
        1.0f), &close_brush);

    // 最小化图标
    rt->DrawLine(
        D2D1::Point2F((FLOAT)min_rect.left + 15.0f, tb_h / 2 + 4.5f),
        D2D1::Point2F((FLOAT)min_rect.right - 15.0f, tb_h / 2 + 4.5f),
        icon_brush,
        1.0f);

    // 最大化/还原图标
    bool is_zoomed = IsZoomed(state->hwnd) ? true : false;
    if (!is_zoomed) {
        rt->DrawRectangle(
            D2D1::RectF((FLOAT)max_rect.left + 15.5f, 9.0f, (FLOAT)max_rect.right - 15.5f, tb_h - 9.0f),
            icon_brush,
            1.0f);
    } else {
        rt->DrawRectangle(
            D2D1::RectF((FLOAT)max_rect.left + 18.0f, 10.0f, (FLOAT)max_rect.right - 12.5f, tb_h - 10.5f),
            icon_brush,
            0.95f);
        rt->DrawRectangle(
            D2D1::RectF((FLOAT)max_rect.left + 12.5f, 13.5f, (FLOAT)max_rect.right - 18.0f, tb_h - 7.0f),
            icon_brush,
            0.95f);
    }

    // 关闭图标
    rt->DrawLine(
        D2D1::Point2F((FLOAT)close_rect.left + 16.0f, 9.0f),
        D2D1::Point2F((FLOAT)close_rect.right - 16.0f, tb_h - 9.0f),
        close_brush,
        1.2f);
    rt->DrawLine(
        D2D1::Point2F((FLOAT)close_rect.left + 16.0f, tb_h - 9.0f),
        D2D1::Point2F((FLOAT)close_rect.right - 16.0f, 9.0f),
        close_brush,
        1.2f);

    close_brush->Release();
    icon_brush->Release();

    // 1.6 绘制窗口图标（如果存在）
    HICON htitle_icon = GetWindowTitleBarIcon(state->hwnd);
    float text_left = 10.0f;
    if (htitle_icon) {
        int icon_w = GetSystemMetrics(SM_CXSMICON);
        int icon_h = GetSystemMetrics(SM_CYSMICON);
        if (icon_w <= 0) icon_w = 16;
        if (icon_h <= 0) icon_h = 16;

        ID2D1Bitmap* title_icon_bitmap = CreateBitmapFromHICONForD2D(rt, htitle_icon);
        if (title_icon_bitmap) {
            float icon_left = 10.0f;
            float icon_top = (tb_h - (float)icon_h) * 0.5f;
            D2D1_RECT_F icon_rect = D2D1::RectF(
                icon_left,
                icon_top,
                icon_left + (float)icon_w,
                icon_top + (float)icon_h
            );
            rt->DrawBitmap(title_icon_bitmap, icon_rect, 1.0f, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR);
            title_icon_bitmap->Release();
            text_left = icon_left + (float)icon_w + 8.0f;
        }
    }

    // 2. 标题文本（彩色emoji）
    if (!state->title.empty()) {
        IDWriteTextFormat* fmt = nullptr;
        factory->CreateTextFormat(
            state->titlebar_font_name.c_str(),
            nullptr,
            DWRITE_FONT_WEIGHT_NORMAL,
            DWRITE_FONT_STYLE_NORMAL,
            DWRITE_FONT_STRETCH_NORMAL,
            state->titlebar_font_size,
            L"zh-CN",
            &fmt);
        if (fmt) {
            // 对齐方式映射：0→LEADING，1→CENTER，2→TRAILING
            DWRITE_TEXT_ALIGNMENT dw_align = DWRITE_TEXT_ALIGNMENT_LEADING;
            if (state->titlebar_alignment == 1) dw_align = DWRITE_TEXT_ALIGNMENT_CENTER;
            else if (state->titlebar_alignment == 2) dw_align = DWRITE_TEXT_ALIGNMENT_TRAILING;
            fmt->SetTextAlignment(dw_align);
            fmt->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);

            // 文字颜色：0 跟随主题，非 0 使用自定义值
            UINT32 fg = (state->titlebar_text_color != 0)
                ? (state->titlebar_text_color & 0x00FFFFFF)
                : (ThemeColor_TextPrimary() & 0x00FFFFFF);
            ID2D1SolidColorBrush* text_brush = nullptr;
            rt->CreateSolidColorBrush(D2D1::ColorF(
                ((fg >> 16) & 0xFF) / 255.0f,
                ((fg >> 8) & 0xFF) / 255.0f,
                (fg & 0xFF) / 255.0f,
                1.0f), &text_brush);

            // 左侧保留图标和间距，右侧留136px给标题栏按钮
            D2D1_RECT_F text_rect = D2D1::RectF(text_left, 0, width - 136.0f, tb_h);
            rt->DrawText(
                state->title.c_str(),
                (UINT32)state->title.length(),
                fmt,
                text_rect,
                text_brush,
                D2D1_DRAW_TEXT_OPTIONS_ENABLE_COLOR_FONT);

            text_brush->Release();
            fmt->Release();
        }
    }
}

// 绘制菜单栏
void DrawMenuBar(ID2D1HwndRenderTarget* rt, IDWriteFactory* factory, MenuBarState* menubar) {
    if (!menubar || !menubar->visible || menubar->items.empty()) return;
    
    RECT rc;
    GetClientRect(menubar->hwnd, &rc);
    float width = (float)(rc.right - rc.left);
    
    // 菜单栏在标题栏下方
    float menu_y = 0.0f;
    auto win_it = g_windows.find(menubar->hwnd);
    if (win_it != g_windows.end() && win_it->second->custom_titlebar) {
        menu_y = (float)win_it->second->titlebar_height;
    }
    
    float menu_h = 30.0f; // 固定高度
    
    // 绘制菜单栏背景
    UINT32 bg = menubar->bg_color ? menubar->bg_color : ThemeColor_Background();
    ID2D1SolidColorBrush* bg_brush = nullptr;
    rt->CreateSolidColorBrush(D2D1::ColorF(
        ((bg >> 16) & 0xFF) / 255.0f,
        ((bg >> 8) & 0xFF) / 255.0f,
        (bg & 0xFF) / 255.0f,
        1.0f), &bg_brush);
    rt->FillRectangle(D2D1::RectF(0, menu_y, width, menu_y + menu_h), bg_brush);
    bg_brush->Release();
    
    // 创建文本格式
    IDWriteTextFormat* fmt = nullptr;
    factory->CreateTextFormat(
        menubar->font.font_name.empty() ? L"Segoe UI Emoji" : menubar->font.font_name.c_str(),
        nullptr,
        menubar->font.bold ? DWRITE_FONT_WEIGHT_BOLD : DWRITE_FONT_WEIGHT_NORMAL,
        menubar->font.italic ? DWRITE_FONT_STYLE_ITALIC : DWRITE_FONT_STYLE_NORMAL,
        DWRITE_FONT_STRETCH_NORMAL,
        menubar->font.font_size > 0 ? (float)menubar->font.font_size : 14.0f,
        L"zh-CN",
        &fmt);
    
    if (!fmt) return;
    
    fmt->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
    fmt->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
    
    // 绘制菜单项
    float x = 10.0f;
    for (size_t i = 0; i < menubar->items.size(); i++) {
        MenuItem& item = menubar->items[i];
        
        // 计算文本宽度
        IDWriteTextLayout* layout = nullptr;
        factory->CreateTextLayout(
            item.text.c_str(),
            (UINT32)item.text.length(),
            fmt,
            1000.0f,
            menu_h,
            &layout);
        
        if (!layout) continue;
        
        DWRITE_TEXT_METRICS metrics;
        layout->GetMetrics(&metrics);
        float item_width = metrics.width + 20.0f; // 左右各10px padding
        
        // 保存边界用于命中测试
        item.bounds = D2D1::RectF(x, menu_y, x + item_width, menu_y + menu_h);
        
        // 绘制悬停/激活背景
        if ((int)i == menubar->hovered_index || (int)i == menubar->opened_index) {
            UINT32 hover_bg = ThemeColor_BackgroundLight();
            ID2D1SolidColorBrush* hover_brush = nullptr;
            rt->CreateSolidColorBrush(D2D1::ColorF(
                ((hover_bg >> 16) & 0xFF) / 255.0f,
                ((hover_bg >> 8) & 0xFF) / 255.0f,
                (hover_bg & 0xFF) / 255.0f,
                1.0f), &hover_brush);
            rt->FillRectangle(item.bounds, hover_brush);
            hover_brush->Release();
        }
        
        // 绘制文本
        UINT32 fg = item.enabled ? (menubar->fg_color ? menubar->fg_color : ThemeColor_TextPrimary()) : ThemeColor_TextPlaceholder();
        ID2D1SolidColorBrush* text_brush = nullptr;
        rt->CreateSolidColorBrush(D2D1::ColorF(
            ((fg >> 16) & 0xFF) / 255.0f,
            ((fg >> 8) & 0xFF) / 255.0f,
            (fg & 0xFF) / 255.0f,
            1.0f), &text_brush);
        
        D2D1_RECT_F text_rect = D2D1::RectF(x + 10.0f, menu_y, x + item_width - 10.0f, menu_y + menu_h);
        rt->DrawText(
            item.text.c_str(),
            (UINT32)item.text.length(),
            fmt,
            text_rect,
            text_brush,
            D2D1_DRAW_TEXT_OPTIONS_ENABLE_COLOR_FONT);
        
        text_brush->Release();
        layout->Release();
        
        x += item_width;
    }
    
    fmt->Release();
}

// 绘制弹出菜单
void DrawPopupMenu(ID2D1HwndRenderTarget* rt, IDWriteFactory* factory, PopupMenuState* popup) {
    if (!popup || popup->items.empty()) {
        char debug_msg[256];
        sprintf_s(debug_msg, "DrawPopupMenu: popup=%p, items.size()=%zu\n", 
            popup, popup ? popup->items.size() : 0);
        OutputDebugStringA(debug_msg);
        return;
    }
    
    OutputDebugStringA("DrawPopupMenu: Starting to draw menu items\n");
    
    // 创建文本格式
    IDWriteTextFormat* fmt = nullptr;
    factory->CreateTextFormat(
        L"Segoe UI Emoji",
        nullptr,
        DWRITE_FONT_WEIGHT_NORMAL,
        DWRITE_FONT_STYLE_NORMAL,
        DWRITE_FONT_STRETCH_NORMAL,
        14.0f,
        L"zh-CN",
        &fmt);
    
    if (!fmt) return;
    
    fmt->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
    fmt->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
    
    // 垂直布局菜单项
    float y = 0.0f;
    float item_height = 32.0f;
    float separator_height = 8.0f;
    
    for (size_t i = 0; i < popup->items.size(); i++) {
        MenuItem& item = popup->items[i];
        
        if (item.separator) {
            // 绘制分隔符
            float sep_y = y + separator_height / 2.0f;
            UINT32 border_color = ThemeColor_BorderLight();
            ID2D1SolidColorBrush* sep_brush = nullptr;
            rt->CreateSolidColorBrush(D2D1::ColorF(
                ((border_color >> 16) & 0xFF) / 255.0f,
                ((border_color >> 8) & 0xFF) / 255.0f,
                (border_color & 0xFF) / 255.0f,
                1.0f), &sep_brush);
            rt->DrawLine(
                D2D1::Point2F(10.0f, sep_y),
                D2D1::Point2F((float)popup->width - 10.0f, sep_y),
                sep_brush,
                1.0f);
            sep_brush->Release();
            
            item.bounds = D2D1::RectF(0, y, (float)popup->width, y + separator_height);
            y += separator_height;
        } else {
            // 保存边界
            item.bounds = D2D1::RectF(0, y, (float)popup->width, y + item_height);
            
            // 绘制悬停背景
            if ((int)i == popup->hovered_index) {
                UINT32 hover_bg = ThemeColor_BackgroundLight();
                ID2D1SolidColorBrush* hover_brush = nullptr;
                rt->CreateSolidColorBrush(D2D1::ColorF(
                    ((hover_bg >> 16) & 0xFF) / 255.0f,
                    ((hover_bg >> 8) & 0xFF) / 255.0f,
                    (hover_bg & 0xFF) / 255.0f,
                    1.0f), &hover_brush);
                rt->FillRectangle(item.bounds, hover_brush);
                hover_brush->Release();
            }
            
            // 绘制文本
            UINT32 fg = item.enabled ? ThemeColor_TextPrimary() : ThemeColor_TextPlaceholder();
            ID2D1SolidColorBrush* text_brush = nullptr;
            rt->CreateSolidColorBrush(D2D1::ColorF(
                ((fg >> 16) & 0xFF) / 255.0f,
                ((fg >> 8) & 0xFF) / 255.0f,
                (fg & 0xFF) / 255.0f,
                1.0f), &text_brush);
            
            D2D1_RECT_F text_rect = D2D1::RectF(20.0f, y, (float)popup->width - 20.0f, y + item_height);
            rt->DrawText(
                item.text.c_str(),
                (UINT32)item.text.length(),
                fmt,
                text_rect,
                text_brush,
                D2D1_DRAW_TEXT_OPTIONS_ENABLE_COLOR_FONT);
            
            text_brush->Release();
            
            // 如果有子菜单,绘制箭头
            if (!item.sub_items.empty()) {
                // 绘制右箭头 ">"
                D2D1_RECT_F arrow_rect = D2D1::RectF((float)popup->width - 30.0f, y, (float)popup->width - 10.0f, y + item_height);
                ID2D1SolidColorBrush* arrow_brush = nullptr;
                rt->CreateSolidColorBrush(D2D1::ColorF(
                    ((fg >> 16) & 0xFF) / 255.0f,
                    ((fg >> 8) & 0xFF) / 255.0f,
                    (fg & 0xFF) / 255.0f,
                    1.0f), &arrow_brush);
                rt->DrawText(L">", 1, fmt, arrow_rect, arrow_brush);
                arrow_brush->Release();
            }
            
            y += item_height;
        }
    }
    
    // 绘制边框
    UINT32 border_color = ThemeColor_BorderBase();
    ID2D1SolidColorBrush* border_brush = nullptr;
    rt->CreateSolidColorBrush(D2D1::ColorF(
        ((border_color >> 16) & 0xFF) / 255.0f,
        ((border_color >> 8) & 0xFF) / 255.0f,
        (border_color & 0xFF) / 255.0f,
        1.0f), &border_brush);
    rt->DrawRectangle(
        D2D1::RectF(0, 0, (float)popup->width, y),
        border_brush,
        1.0f);
    border_brush->Release();
    
    fmt->Release();
}

// Draw button (Element UI style - supports both main window buttons and message box buttons)
void DrawButton(ID2D1HwndRenderTarget* rt, IDWriteFactory* factory, const EmojiButton& button) {
    // ========== 检查可见状态 ==========
    if (!button.visible) {
        return;  // 不可见的按钮不绘制
    }
    
    // ========== 处理禁用状态 ==========
    if (!button.enabled) {
        // 禁用状态：使用灰色绘制
        ID2D1SolidColorBrush* brush = nullptr;
        UINT32 disabled_bg = 0xFFF5F7FA;  // 浅灰色背景
        rt->CreateSolidColorBrush(ColorFromUInt32(disabled_bg), &brush);

        D2D1_ROUNDED_RECT rect = D2D1::RoundedRect(
            D2D1::RectF(
                (FLOAT)button.x,
                (FLOAT)button.y,
                (FLOAT)(button.x + button.width),
                (FLOAT)(button.y + button.height)
            ),
            4.0f, 4.0f
        );
        rt->FillRoundedRectangle(rect, brush);
        brush->Release();

        // 绘制边框
        ID2D1SolidColorBrush* border_brush = nullptr;
        UINT32 disabled_border = 0xFFE4E7ED;  // 浅灰色边框
        rt->CreateSolidColorBrush(ColorFromUInt32(disabled_border), &border_brush);
        rt->DrawRoundedRectangle(rect, border_brush, 1.0f);
        border_brush->Release();

        // 绘制禁用文本
        ID2D1SolidColorBrush* text_brush = nullptr;
        UINT32 disabled_text = 0xFFC0C4CC;  // 占位文本色
        rt->CreateSolidColorBrush(ColorFromUInt32(disabled_text), &text_brush);

        // 创建文本格式
        IDWriteTextFormat* fmt = nullptr;
        factory->CreateTextFormat(
            L"Segoe UI Emoji",
            nullptr,
            DWRITE_FONT_WEIGHT_NORMAL,
            DWRITE_FONT_STYLE_NORMAL,
            DWRITE_FONT_STRETCH_NORMAL,
            14.0f,
            L"zh-CN",
            &fmt
        );
        if (fmt) {
            fmt->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
            fmt->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);

            // 绘制文本（emoji + text）
            std::wstring full_text = button.emoji + button.text;
            D2D1_RECT_F text_rect = D2D1::RectF(
                (FLOAT)button.x,
                (FLOAT)button.y,
                (FLOAT)(button.x + button.width),
                (FLOAT)(button.y + button.height)
            );
            rt->DrawText(
                full_text.c_str(),
                (UINT32)full_text.length(),
                fmt,
                text_rect,
                text_brush,
                D2D1_DRAW_TEXT_OPTIONS_ENABLE_COLOR_FONT
            );
            fmt->Release();
        }
        text_brush->Release();
        return;  // 禁用状态绘制完成，直接返回
    }

    // ========== Calculate button color based on state (Element UI style) ==========
    // 解析主题颜色索引（如果传入的是0-14的索引值）
    UINT32 bg_color = ResolveThemeColor(button.bg_color);
    UINT32 bg_rgb = bg_color & 0x00FFFFFF;

    // 将默认亮色主题的固定颜色映射到当前主题颜色
    // 这样当主题切换时，按钮颜色也会跟着变化
    if (bg_rgb == 0x409EFF) bg_color = ThemeColor_Primary();
    else if (bg_rgb == 0x67C23A) bg_color = ThemeColor_Success();
    else if (bg_rgb == 0xE6A23C) bg_color = ThemeColor_Warning();
    else if (bg_rgb == 0xF56C6C) bg_color = ThemeColor_Danger();
    else if (bg_rgb == 0x909399) bg_color = ThemeColor_Info();
    bg_rgb = bg_color & 0x00FFFFFF;

    // Element UI button colors (使用当前主题颜色比较)
    UINT32 theme_primary_rgb = ThemeColor_Primary() & 0x00FFFFFF;
    UINT32 theme_success_rgb = ThemeColor_Success() & 0x00FFFFFF;
    UINT32 theme_warning_rgb = ThemeColor_Warning() & 0x00FFFFFF;
    UINT32 theme_danger_rgb = ThemeColor_Danger() & 0x00FFFFFF;
    UINT32 theme_info_rgb = ThemeColor_Info() & 0x00FFFFFF;
    bool is_primary = (bg_rgb == theme_primary_rgb);
    bool is_success = (bg_rgb == theme_success_rgb);
    bool is_warning = (bg_rgb == theme_warning_rgb);
    bool is_danger = (bg_rgb == theme_danger_rgb);
    bool is_info = (bg_rgb == theme_info_rgb);
    bool is_default = (bg_rgb == 0xFFFFFF || bg_rgb == 0xF2F2F7);

    if (button.is_pressed) {
        // Pressed state: 使用主题辅助函数动态计算按下色
        bg_color = DarkenColor(bg_color, 0.85f);
    } else if (button.is_hovered) {
        // Hover state: 使用主题辅助函数动态计算悬停色
        bg_color = LightenColor(bg_color, 1.15f);
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
        UINT32 bdr = ThemeColor_BorderBase();
        rt->CreateSolidColorBrush(ColorFromUInt32(bdr), &border_brush);
        rt->DrawRoundedRectangle(rect, border_brush, ThemeSize_BorderWidth());
        border_brush->Release();
    }

    // ========== Determine text color ==========
    UINT32 text_color;
    if (is_default) {
        text_color = ThemeColor_TextRegular();  // 使用主题常规文本色
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

static void DrawParentLabel(ID2D1HwndRenderTarget* rt, IDWriteFactory* factory, LabelState* state) {
    if (!rt || !factory || !state || !state->visible) return;

    D2D1_RECT_F rect = D2D1::RectF(
        (FLOAT)state->x,
        (FLOAT)state->y,
        (FLOAT)(state->x + state->width),
        (FLOAT)(state->y + state->height)
    );

    ID2D1SolidColorBrush* bg_brush = nullptr;
    ID2D1SolidColorBrush* text_brush = nullptr;
    IDWriteTextFormat* text_format = nullptr;
    IDWriteTextLayout* text_layout = nullptr;

    rt->CreateSolidColorBrush(ColorFromUInt32(ResolveThemeColor(state->bg_color)), &bg_brush);
    rt->CreateSolidColorBrush(ColorFromUInt32(ResolveThemeColor(state->fg_color)), &text_brush);

    if (bg_brush) {
        rt->FillRectangle(rect, bg_brush);
    }

    HRESULT hr = factory->CreateTextFormat(
        state->font.font_name.c_str(),
        nullptr,
        state->font.bold ? DWRITE_FONT_WEIGHT_BOLD : DWRITE_FONT_WEIGHT_NORMAL,
        state->font.italic ? DWRITE_FONT_STYLE_ITALIC : DWRITE_FONT_STYLE_NORMAL,
        DWRITE_FONT_STRETCH_NORMAL,
        (FLOAT)state->font.font_size,
        L"zh-CN",
        &text_format
    );

    if (SUCCEEDED(hr) && text_format && text_brush) {
        switch (state->alignment) {
        case ALIGN_LEFT:
            text_format->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
            break;
        case ALIGN_CENTER:
            text_format->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
            break;
        case ALIGN_RIGHT:
            text_format->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_TRAILING);
            break;
        }

        if (state->word_wrap) {
            text_format->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
            text_format->SetWordWrapping(DWRITE_WORD_WRAPPING_WRAP);
        } else {
            text_format->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
            text_format->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
        }

        hr = factory->CreateTextLayout(
            state->text.c_str(),
            (UINT32)state->text.length(),
            text_format,
            (FLOAT)state->width,
            (FLOAT)state->height,
            &text_layout
        );

        if (SUCCEEDED(hr) && text_layout) {
            rt->DrawTextLayout(
                D2D1::Point2F((FLOAT)state->x, (FLOAT)state->y),
                text_layout,
                text_brush,
                D2D1_DRAW_TEXT_OPTIONS_ENABLE_COLOR_FONT
            );
        }
    }

    if (text_layout) text_layout->Release();
    if (text_format) text_format->Release();
    if (text_brush) text_brush->Release();
    if (bg_brush) bg_brush->Release();
}

static void ApplyShowGroupBox(HWND hGroupBox, BOOL show);
static BOOL ApplySelectTab(HWND hTabControl, int index);

// Window procedure
LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
    auto it = g_windows.find(hwnd);
    if (it == g_windows.end() && msg != WM_NCCREATE && msg != WM_CREATE) {
        return DefWindowProc(hwnd, msg, wparam, lparam);
    }

    WindowState* state = (it != g_windows.end()) ? it->second : nullptr;

    switch (msg) {
    case WM_NCCREATE: {
        CREATESTRUCTW* cs = reinterpret_cast<CREATESTRUCTW*>(lparam);
        WindowCreateInit* init = reinterpret_cast<WindowCreateInit*>(cs->lpCreateParams);

        WindowState* new_state = new WindowState();
        new_state->hwnd = hwnd;
        if (init) {
            new_state->title = init->title;
            new_state->titlebar_color = init->titlebar_color;
            new_state->client_bg_color = init->client_bg_color;
            new_state->custom_titlebar = init->custom_titlebar;
        }
        g_windows[hwnd] = new_state;
        state = new_state;
        return TRUE;
    }

    default:
        break;
    }

    // DWM自定义标题栏：让DWM优先处理系统按钮（最小化/最大化/关闭）
    if (state && state->custom_titlebar) {
        LRESULT dwm_result = 0;
        if (DwmDefWindowProc(hwnd, msg, wparam, lparam, &dwm_result))
            return dwm_result;
    }

    switch (msg) {
    case WM_EW_TV_INVALIDATE_DEFERRED: {
        HWND hTree = reinterpret_cast<HWND>(wparam);
        if (hTree && IsWindow(hTree)) {
            InvalidateRect(hTree, nullptr, FALSE);
        }
        return 0;
    }

    case WM_EW_GROUPBOX_SHOW_DEFERRED: {
        HWND hGb = reinterpret_cast<HWND>(wparam);
        WriteLog("WindowProc: WM_EW_GROUPBOX_SHOW_DEFERRED hwnd=%p, hGroupBox=%p, show=%d, is_window=%d",
            hwnd, hGb, (int)lparam, (hGb && IsWindow(hGb)) ? 1 : 0);
        if (hGb && IsWindow(hGb)) {
            ApplyShowGroupBox(hGb, (BOOL)lparam);
        }
        return 0;
    }

    case WM_EW_TV_NODE_SELECTED_DEFERRED: {
        HWND hTree = reinterpret_cast<HWND>(wparam);
        int node_id = static_cast<int>(static_cast<INT_PTR>(lparam));
        WriteLog("WindowProc: WM_EW_TV_NODE_SELECTED_DEFERRED hwnd=%p, hTree=%p, node_id=%d, is_window=%d",
            hwnd, hTree, node_id, (hTree && IsWindow(hTree)) ? 1 : 0);
        if (!hTree || !IsWindow(hTree)) {
            return 0;
        }
        // 再投递一帧，避免本消息处理末尾仍在本轮 WindowProc 栈上同步进入易语言（部分环境下仍会崩）
        if (!PostMessageW(hwnd, WM_EW_TV_NODE_SELECTED_INVOKE, (WPARAM)hTree, (LPARAM)(INT_PTR)node_id)) {
            TreeViewOnNodeSelectedDeferred(hTree, node_id);
            WriteLog("WindowProc: TreeViewOnNodeSelectedDeferred returned (PostMessage INVOKE failed), hTree=%p, node_id=%d", hTree, node_id);
            PostMessageW(hwnd, WM_EW_TV_INVALIDATE_DEFERRED, (WPARAM)hTree, 0);
        }
        return 0;
    }

    case WM_EW_TV_NODE_SELECTED_INVOKE: {
        HWND hTree = reinterpret_cast<HWND>(wparam);
        int node_id = static_cast<int>(static_cast<INT_PTR>(lparam));
        WriteLog("WindowProc: WM_EW_TV_NODE_SELECTED_INVOKE hwnd=%p, hTree=%p, node_id=%d", hwnd, hTree, node_id);
        if (!hTree || !IsWindow(hTree)) {
            return 0;
        }
        TreeViewOnNodeSelectedDeferred(hTree, node_id);
        WriteLog("WindowProc: TreeViewOnNodeSelectedDeferred returned, hTree=%p, node_id=%d", hTree, node_id);
        PostMessageW(hwnd, WM_EW_TV_INVALIDATE_DEFERRED, (WPARAM)hTree, 0);
        return 0;
    }

    case WM_EW_SELECT_TAB_DEFERRED: {
        HWND hTab = reinterpret_cast<HWND>(wparam);
        int tabIdx = static_cast<int>(static_cast<INT_PTR>(lparam));
        WriteLog("WindowProc: WM_EW_SELECT_TAB_DEFERRED hwnd=%p, hTab=%p, index=%d", hwnd, hTab, tabIdx);
        if (!hTab || !IsWindow(hTab)) {
            return 0;
        }
        ApplySelectTab(hTab, tabIdx);
        return 0;
    }

    case WM_NCCALCSIZE: {
        // 自定义标题栏：移除系统标题栏，让客户区占满整个窗口
        if (state && state->custom_titlebar) {
            return 0;
        }
        break;
    }

    case WM_NCHITTEST: {
        if (state && state->custom_titlebar) {
            POINT screen_pt = { GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam) };
            RECT win_rc = {};
            GetWindowRect(hwnd, &win_rc);

            const int resize_border = GetSystemMetrics(SM_CXSIZEFRAME) + GetSystemMetrics(SM_CXPADDEDBORDER);
            bool on_left = screen_pt.x >= win_rc.left && screen_pt.x < win_rc.left + resize_border;
            bool on_right = screen_pt.x < win_rc.right && screen_pt.x >= win_rc.right - resize_border;
            bool on_top = screen_pt.y >= win_rc.top && screen_pt.y < win_rc.top + resize_border;
            bool on_bottom = screen_pt.y < win_rc.bottom && screen_pt.y >= win_rc.bottom - resize_border;

            if (on_top && on_left) return HTTOPLEFT;
            if (on_top && on_right) return HTTOPRIGHT;
            if (on_bottom && on_left) return HTBOTTOMLEFT;
            if (on_bottom && on_right) return HTBOTTOMRIGHT;
            if (on_left) return HTLEFT;
            if (on_right) return HTRIGHT;
            if (on_top) return HTTOP;
            if (on_bottom) return HTBOTTOM;

            POINT pt = screen_pt;
            ScreenToClient(hwnd, &pt);

            TitleBarButtonType btn = HitTestTitleBarButton(state, pt);
            if (btn == TITLEBAR_BUTTON_CLOSE) return HTCLOSE;
            if (btn == TITLEBAR_BUTTON_MAXIMIZE) return HTMAXBUTTON;
            if (btn == TITLEBAR_BUTTON_MINIMIZE) return HTMINBUTTON;

            // 标题栏区域 → HTCAPTION（支持拖拽移动窗口）
            if (pt.y < state->titlebar_height) {
                return HTCAPTION;
            }
            return HTCLIENT;
        }
        break;
    }

    case WM_SETCURSOR: {
        if (state && state->custom_titlebar) {
            switch (LOWORD(lparam)) {
            case HTCAPTION:
                SetCursor(LoadCursor(nullptr, IDC_SIZEALL));
                return TRUE;
            case HTMINBUTTON:
            case HTMAXBUTTON:
            case HTCLOSE:
                SetCursor(LoadCursor(nullptr, IDC_ARROW));
                return TRUE;
            }
        }
        break;
    }

    case WM_NCMOUSEMOVE: {
        if (state && state->custom_titlebar) {
            TitleBarButtonType hovered_btn = HitTestTitleBarButtonFromScreen(state, lparam);
            if ((int)hovered_btn != state->hovered_titlebar_button) {
                state->hovered_titlebar_button = (int)hovered_btn;
                InvalidateRect(hwnd, nullptr, FALSE);
            }
            if (!state->titlebar_mouse_tracking) {
                TRACKMOUSEEVENT tme = { sizeof(TRACKMOUSEEVENT) };
                tme.dwFlags = TME_LEAVE | TME_NONCLIENT;
                tme.hwndTrack = hwnd;
                TrackMouseEvent(&tme);
                state->titlebar_mouse_tracking = true;
            }
            return 0;
        }
        break;
    }

    case WM_NCMOUSELEAVE: {
        if (state && state->custom_titlebar) {
            state->titlebar_mouse_tracking = false;
            if (state->hovered_titlebar_button != 0) {
                state->hovered_titlebar_button = 0;
                InvalidateRect(hwnd, nullptr, FALSE);
            }
            return 0;
        }
        break;
    }

    case WM_NCLBUTTONDOWN:
    case WM_NCLBUTTONUP: {
        if (state && state->custom_titlebar) {
            switch ((int)wparam) {
            case HTMINBUTTON:
                ShowWindow(hwnd, SW_MINIMIZE);
                return 0;
            case HTMAXBUTTON:
                ShowWindow(hwnd, IsZoomed(hwnd) ? SW_RESTORE : SW_MAXIMIZE);
                return 0;
            case HTCLOSE:
                SendMessage(hwnd, WM_CLOSE, 0, 0);
                return 0;
            }
        }
        break;
    }

    case WM_ACTIVATE: {
        if (state && state->custom_titlebar) {
            MARGINS margins = { 0, 0, state->titlebar_height, 0 };
            DwmExtendFrameIntoClientArea(hwnd, &margins);
        }
        break;
    }
    case WM_PAINT: {
        if (state && state->render_target) {
            PAINTSTRUCT ps;
            BeginPaint(hwnd, &ps);

            state->render_target->BeginDraw();
            // 客户区背景色：0=纯白(ThemeColor_Background)，非0=用户指定色；与标签/复选框/单选框底色一致
            UINT32 win_bg = (state->client_bg_color != 0) ? state->client_bg_color : ThemeColor_Background();
            state->render_target->Clear(D2D1::ColorF(
                ((win_bg >> 16) & 0xFF) / 255.0f,
                ((win_bg >> 8) & 0xFF) / 255.0f,
                (win_bg & 0xFF) / 255.0f,
                1.0f));

            // 绘制自定义标题栏（彩色emoji）
            DrawWindowTitleBar(state->render_target, state->dwrite_factory, state);

            // 绘制菜单栏
            auto menu_it = g_menubars.find(hwnd);
            if (menu_it != g_menubars.end()) {
                DrawMenuBar(state->render_target, state->dwrite_factory, menu_it->second);
            }

            for (const auto& button : state->buttons) {
                // 检查按钮是否属于某个分组框
                bool should_draw = true;
                for (const auto& gb_pair : g_groupboxes) {
                    GroupBoxState* gb = gb_pair.second;
                    // 如果按钮在这个分组框的按钮列表中
                    if (std::find(gb->button_ids.begin(), gb->button_ids.end(), button.id) != gb->button_ids.end()) {
                        // 只有分组框可见时才绘制按钮
                        should_draw = gb->visible;
                        break;
                    }
                }
                
                if (should_draw) {
                    DrawButton(state->render_target, state->dwrite_factory, button);
                }
            }

            for (const auto& pair : g_labels) {
                LabelState* label = pair.second;
                if (label && label->parent == hwnd && label->parent_drawn) {
                    DrawParentLabel(state->render_target, state->dwrite_factory, label);
                }
            }

            state->render_target->EndDraw();
            EndPaint(hwnd, &ps);
        }
        return 0;
    }

    case WM_LBUTTONDOWN: {
        WriteLog("WM_LBUTTONDOWN: hwnd=%p, state=%p", hwnd, state);
        if (state) {
            int x = LOWORD(lparam);
            int y = HIWORD(lparam);
            WriteLog("WM_LBUTTONDOWN: x=%d, y=%d", x, y);

            // 顶栏菜单 bounds 为 DIP（D2D 绘制），鼠标为像素，需换算为 DIP 再命中
            UINT dpi = GetDpiForWindow(hwnd);
            if (dpi == 0) dpi = 96;
            float scale = 96.0f / (float)dpi;
            float fx = (float)x * scale;
            float fy = (float)y * scale;

            // 检查菜单栏点击
            auto menu_it = g_menubars.find(hwnd);
            WriteLog("WM_LBUTTONDOWN: g_menubars.size()=%zu", g_menubars.size());
            if (menu_it != g_menubars.end()) {
                MenuBarState* menubar = menu_it->second;
                WriteLog("WM_LBUTTONDOWN: menubar found, items.size()=%zu", menubar->items.size());
                
                for (size_t i = 0; i < menubar->items.size(); i++) {
                    const D2D1_RECT_F& bounds = menubar->items[i].bounds;
                    WriteLog("WM_LBUTTONDOWN: item[%zu] bounds=(%.1f,%.1f)-(%.1f,%.1f)", 
                            i, bounds.left, bounds.top, bounds.right, bounds.bottom);
                    if (fx >= bounds.left && fx <= bounds.right && 
                        fy >= bounds.top && fy <= bounds.bottom) {
                        MenuItem& item = menubar->items[i];
                        WriteLog("WM_LBUTTONDOWN: item clicked, id=%d, enabled=%d, sub_items.size()=%zu", 
                                item.id, item.enabled, item.sub_items.size());
                        if (item.enabled) {
                            // ✅ 实现子菜单弹出功能
                            if (!item.sub_items.empty()) {
                                // 有子菜单，弹出子菜单
                                if (!menubar->submenu) {
                                    menubar->submenu = new SubMenuWindow();
                                    menubar->submenu->Create(hwnd, 0);
                                }
                                
                                // 计算子菜单显示位置：bounds 为 DIP，ClientToScreen 需要客户区像素
                                float scaleToPx = (float)dpi / 96.0f;
                                POINT pt = {
                                    (LONG)(bounds.left * scaleToPx),
                                    (LONG)(bounds.bottom * scaleToPx)
                                };
                                ClientToScreen(hwnd, &pt);
                                
                                // 显示子菜单
                                menubar->opened_menu_id = item.id;
                                menubar->submenu->Show(
                                    item.sub_items,
                                    pt.x,
                                    pt.y,
                                    [](int /*menu_id*/, int item_id) {
                                        // 查找对应的菜单栏（当前有可见子菜单的那个）
                                        for (auto& pair : g_menubars) {
                                            MenuBarState* mb = pair.second;
                                            if (mb->submenu && mb->submenu->IsVisible()) {
                                                if (mb->callback) {
                                                    // 回调参数：menu_id=顶级菜单ID，item_id=子菜单项ID
                                                    mb->callback(mb->opened_menu_id, item_id);
                                                }
                                                break;
                                            }
                                        }
                                    });
                                
                                menubar->opened_index = (int)i;
                            } else {
                                // 没有子菜单，直接触发回调（顶级菜单本身被点击）
                                WriteLog("WM_LBUTTONDOWN: calling callback, menubar->callback=%p, item.id=%d", menubar->callback, item.id);
                                if (menubar->callback) {
                                    // 回调参数：menu_id=顶级菜单ID，item_id=顶级菜单ID
                                    menubar->callback(item.id, item.id);
                                    WriteLog("WM_LBUTTONDOWN: callback called successfully");
                                } else {
                                    WriteLog("WM_LBUTTONDOWN: callback is NULL!");
                                }
                            }
                            InvalidateRect(hwnd, nullptr, FALSE);
                        }
                        return 0;
                    }
                }
            }

            for (auto& button : state->buttons) {
                if (button.enabled && button.visible && button.ContainsPoint(x, y)) {
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
                if (button.is_pressed && button.enabled && button.visible && button.ContainsPoint(x, y)) {
                    button.is_pressed = false;

                    if (g_button_callback) {
                        g_button_callback(button.id, hwnd);
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

            // 顶栏菜单 bounds 为 DIP，鼠标为像素，换算为 DIP 再悬停命中
            UINT dpi = GetDpiForWindow(hwnd);
            if (dpi == 0) dpi = 96;
            float scale = 96.0f / (float)dpi;
            float fx = (float)x * scale;
            float fy = (float)y * scale;

            // 检查菜单栏悬停
            auto menu_it = g_menubars.find(hwnd);
            if (menu_it != g_menubars.end()) {
                MenuBarState* menubar = menu_it->second;
                int old_hovered = menubar->hovered_index;
                menubar->hovered_index = -1;
                
                for (size_t i = 0; i < menubar->items.size(); i++) {
                    const D2D1_RECT_F& bounds = menubar->items[i].bounds;
                    if (fx >= bounds.left && fx <= bounds.right && 
                        fy >= bounds.top && fy <= bounds.bottom) {
                        menubar->hovered_index = (int)i;
                        break;
                    }
                }
                
                if (old_hovered != menubar->hovered_index) {
                    needs_redraw = true;
                }
            }

            for (auto& button : state->buttons) {
                bool hovered = button.enabled && button.visible && button.ContainsPoint(x, y);
                if (hovered != button.is_hovered) {
                    button.is_hovered = hovered;
                    needs_redraw = true;
                }
            }

            if (state->custom_titlebar) {
                POINT pt = { x, y };
                TitleBarButtonType hovered_btn = HitTestTitleBarButton(state, pt);
                if ((int)hovered_btn != state->hovered_titlebar_button) {
                    state->hovered_titlebar_button = (int)hovered_btn;
                    needs_redraw = true;
                }
                if (!state->titlebar_mouse_tracking) {
                    TRACKMOUSEEVENT tme = { sizeof(TRACKMOUSEEVENT) };
                    tme.dwFlags = TME_LEAVE;
                    tme.hwndTrack = hwnd;
                    TrackMouseEvent(&tme);
                    state->titlebar_mouse_tracking = true;
                }
            }

            if (needs_redraw) {
                InvalidateRect(hwnd, nullptr, FALSE);
            }
        }
        return 0;
    }

    case WM_MOUSELEAVE: {
        if (state && state->custom_titlebar) {
            state->titlebar_mouse_tracking = false;
            if (state->hovered_titlebar_button != 0) {
                state->hovered_titlebar_button = 0;
                InvalidateRect(hwnd, nullptr, FALSE);
            }
        }
        return 0;
    }

    case WM_SYSKEYDOWN: {
        // 处理Alt键激活菜单栏
        if (wparam == VK_MENU) {
            auto menu_it = g_menubars.find(hwnd);
            if (menu_it != g_menubars.end()) {
                MenuBarState* menubar = menu_it->second;
                if (!menubar->items.empty()) {
                    // 激活第一个菜单项
                    menubar->hovered_index = 0;
                    InvalidateRect(hwnd, nullptr, FALSE);
                    return 0;
                }
            }
        }
        break;
    }

    case WM_KEYDOWN: {
        auto menu_it = g_menubars.find(hwnd);
        if (menu_it != g_menubars.end()) {
            MenuBarState* menubar = menu_it->second;
            
            // 如果菜单栏有激活项,处理方向键
            if (menubar->hovered_index >= 0) {
                if (wparam == VK_LEFT) {
                    // 向左切换
                    menubar->hovered_index--;
                    if (menubar->hovered_index < 0) {
                        menubar->hovered_index = (int)menubar->items.size() - 1;
                    }
                    InvalidateRect(hwnd, nullptr, FALSE);
                    return 0;
                }
                else if (wparam == VK_RIGHT) {
                    // 向右切换
                    menubar->hovered_index++;
                    if (menubar->hovered_index >= (int)menubar->items.size()) {
                        menubar->hovered_index = 0;
                    }
                    InvalidateRect(hwnd, nullptr, FALSE);
                    return 0;
                }
                else if (wparam == VK_RETURN || wparam == VK_SPACE) {
                    // 触发当前选中项
                    if (menubar->hovered_index >= 0 && menubar->hovered_index < (int)menubar->items.size()) {
                        MenuItem& item = menubar->items[menubar->hovered_index];
                        if (item.enabled && menubar->callback) {
                            menubar->callback(0, item.id);
                        }
                    }
                    return 0;
                }
                else if (wparam == VK_ESCAPE) {
                    // 取消菜单激活
                    menubar->hovered_index = -1;
                    InvalidateRect(hwnd, nullptr, FALSE);
                    return 0;
                }
            }
            
            // Shift+F10 或 VK_APPS 触发右键菜单
            if (wparam == VK_F10 && (GetKeyState(VK_SHIFT) & 0x8000)) {
                // TODO: 显示右键菜单
                return 0;
            }
            if (wparam == VK_APPS) {
                // TODO: 显示右键菜单
                return 0;
            }
        }
        break;
    }

    case WM_SIZE: {
        if (state && state->render_target) {
            UINT width = LOWORD(lparam);
            UINT height = HIWORD(lparam);
            state->render_target->Resize(D2D1::SizeU(width, height));

            // 自动触发布局管理器更新
            auto lm_it = g_layout_managers.find(hwnd);
            if (lm_it != g_layout_managers.end()) {
                UpdateLayout(hwnd);
            }

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
            // 解析主题颜色索引
            UINT32 resolved_fg = ResolveThemeColor(label_state->fg_color);
            UINT32 resolved_bg = ResolveThemeColor(label_state->bg_color);
            SetTextColor(hdc, RGB(
                (resolved_fg >> 16) & 0xFF,
                (resolved_fg >> 8) & 0xFF,
                resolved_fg & 0xFF
            ));
            SetBkColor(hdc, RGB(
                (resolved_bg >> 16) & 0xFF,
                (resolved_bg >> 8) & 0xFF,
                resolved_bg & 0xFF
            ));
            // 如果使用主题颜色索引，需要动态重建画刷
            if (IsThemeColorIndex(label_state->bg_color)) {
                if (label_state->bg_brush) DeleteObject(label_state->bg_brush);
                label_state->bg_brush = CreateSolidBrush(RGB(
                    (resolved_bg >> 16) & 0xFF,
                    (resolved_bg >> 8) & 0xFF,
                    resolved_bg & 0xFF
                ));
            }
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
                if (g_message_loop_main_window) {
                    if (hwnd == g_message_loop_main_window) {
                        PostQuitMessage(0);
                    }
                } else {
                    PostQuitMessage(0);
                }
            }
        }
        return 0;
    }
    }

    return DefWindowProc(hwnd, msg, wparam, lparam);
}

// Create window
// x, y: 窗口位置（像素），传 -1 表示使用系统默认位置 CW_USEDEFAULT
HWND __stdcall create_window(const char* title, int x, int y, int width, int height) {
    static bool com_initialized = false;
    if (!com_initialized) {
        CoInitialize(nullptr);
        com_initialized = true;
    }

    // 加载RichEdit库（支持彩色emoji）
    static bool richedit_loaded = false;
    if (!richedit_loaded) {
        LoadLibraryW(L"Msftedit.dll");  // RichEdit 4.1
        richedit_loaded = true;
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

    WindowCreateInit init_data;
    init_data.title = wtitle;
    init_data.titlebar_color = 0;
    init_data.custom_titlebar = true;

    int pos_x = (x < 0) ? CW_USEDEFAULT : x;
    int pos_y = (y < 0) ? CW_USEDEFAULT : y;

    HWND hwnd = CreateWindowExW(
        0,
        class_name,
        wtitle.c_str(),
        WS_OVERLAPPEDWINDOW | WS_VISIBLE | WS_CLIPCHILDREN,  // WS_CLIPCHILDREN: 绘制时裁剪子控件区域
        pos_x, pos_y,
        width, height,
        nullptr, nullptr,
        GetModuleHandle(nullptr),
        &init_data
    );

    if (!hwnd) return nullptr;

    RECT rc;
    GetClientRect(hwnd, &rc);

    ID2D1HwndRenderTarget* render_target = nullptr;
    if (g_d2d_factory) {
        HRESULT hr = g_d2d_factory->CreateHwndRenderTarget(
            D2D1::RenderTargetProperties(),
            D2D1::HwndRenderTargetProperties(
                hwnd,
                D2D1::SizeU(rc.right - rc.left, rc.bottom - rc.top)
            ),
            &render_target
        );
        if (SUCCEEDED(hr) && render_target) {
            // 使用窗口当前 DPI 作为渲染 DPI，保证坐标与客户区像素一致
            UINT dpi = GetDpiForWindow(hwnd);
            float fdpi = dpi > 0 ? (float)dpi : 96.0f;
            render_target->SetDpi(fdpi, fdpi);
        }
    }

    auto state_it = g_windows.find(hwnd);
    if (state_it == g_windows.end()) return nullptr;

    WindowState* state = state_it->second;
    state->render_target = render_target;
    state->dwrite_factory = g_dwrite_factory;

    // DWM扩展客户区到标题栏区域
    MARGINS margins = { 0, 0, state->titlebar_height, 0 };
    DwmExtendFrameIntoClientArea(hwnd, &margins);

    // Windows 11+ 圆角窗口（与设计器一致）
    #ifndef DWMWA_WINDOW_CORNER_PREFERENCE
    #define DWMWA_WINDOW_CORNER_PREFERENCE 33
    #endif
    #ifndef DWMWCP_ROUND
    #define DWMWCP_ROUND 2
    #endif
    DWORD corner_pref = DWMWCP_ROUND;
    DwmSetWindowAttribute(hwnd, DWMWA_WINDOW_CORNER_PREFERENCE, &corner_pref, sizeof(corner_pref));

    // 确保系统窗口文本存在，任务栏/Alt+Tab 也能显示标题
    SetWindowTextW(hwnd, wtitle.c_str());

    ShowWindow(hwnd, SW_SHOW);
    UpdateWindow(hwnd);

    return hwnd;
}

// Create window (UTF-8 bytes version) - supports emoji in title
HWND __stdcall create_window_bytes(const unsigned char* title_bytes, int title_len, int x, int y, int width, int height) {
    // 向后兼容：titlebar_color=0 跟随主题，client_bg_color=0 纯白
    return create_window_bytes_ex(title_bytes, title_len, x, y, width, height, 0, 0);
}

// Create window with custom titlebar and client background color (UTF-8 bytes version)
// x, y: 窗口位置（像素），传 -1 表示使用系统默认位置 CW_USEDEFAULT
HWND __stdcall create_window_bytes_ex(const unsigned char* title_bytes, int title_len, int x, int y, int width, int height, UINT32 titlebar_color, UINT32 client_bg_color) {
    // Convert UTF-8 bytes to wide string
    std::string utf8_title;
    if (title_bytes && title_len > 0) {
        utf8_title.assign(reinterpret_cast<const char*>(title_bytes), title_len);
    }
    int wlen = MultiByteToWideChar(CP_UTF8, 0, utf8_title.c_str(), (int)utf8_title.size(), nullptr, 0);
    std::wstring wtitle(wlen, 0);
    MultiByteToWideChar(CP_UTF8, 0, utf8_title.c_str(), (int)utf8_title.size(), &wtitle[0], wlen);

    // Delegate to create_window logic but with wide title
    static bool com_initialized = false;
    if (!com_initialized) {
        CoInitialize(nullptr);
        com_initialized = true;
    }

    static bool richedit_loaded = false;
    if (!richedit_loaded) {
        LoadLibraryW(L"Msftedit.dll");
        richedit_loaded = true;
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

    WindowCreateInit init_data;
    init_data.title = wtitle;
    init_data.titlebar_color = titlebar_color;
    init_data.client_bg_color = client_bg_color;
    init_data.custom_titlebar = true;

    int pos_x = (x < 0) ? CW_USEDEFAULT : x;
    int pos_y = (y < 0) ? CW_USEDEFAULT : y;

    HWND hwnd = CreateWindowExW(
        0,
        class_name,
        wtitle.c_str(),
        WS_OVERLAPPEDWINDOW | WS_VISIBLE | WS_CLIPCHILDREN,
        pos_x, pos_y,
        width, height,
        nullptr, nullptr,
        GetModuleHandle(nullptr),
        &init_data
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

    auto state_it = g_windows.find(hwnd);
    if (state_it == g_windows.end()) return nullptr;

    WindowState* state = state_it->second;
    state->render_target = render_target;
    state->dwrite_factory = g_dwrite_factory;

    // DWM扩展客户区到标题栏区域
    MARGINS margins = { 0, 0, state->titlebar_height, 0 };
    DwmExtendFrameIntoClientArea(hwnd, &margins);

    // Windows 11+ 圆角窗口（与设计器一致）
    #ifndef DWMWA_WINDOW_CORNER_PREFERENCE
    #define DWMWA_WINDOW_CORNER_PREFERENCE 33
    #endif
    #ifndef DWMWCP_ROUND
    #define DWMWCP_ROUND 2
    #endif
    DWORD corner_pref = DWMWCP_ROUND;
    DwmSetWindowAttribute(hwnd, DWMWA_WINDOW_CORNER_PREFERENCE, &corner_pref, sizeof(corner_pref));

    // 确保系统窗口文本存在，任务栏/Alt+Tab 也能显示标题
    SetWindowTextW(hwnd, wtitle.c_str());

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
    // 检查父窗口是否为分组框
    HWND actual_parent = parent;
    int actual_x = x;
    int actual_y = y;
    GroupBoxState* parent_groupbox = nullptr;
    
    auto groupbox_it = g_groupboxes.find(parent);
    if (groupbox_it != g_groupboxes.end()) {
        // 父窗口是分组框，需要转换坐标
        GroupBoxState* gb_state = groupbox_it->second;
        parent_groupbox = gb_state;  // 记录父分组框
        actual_parent = gb_state->parent;  // 使用分组框的父窗口
        
        // 转换坐标：分组框相对坐标 -> 窗口绝对坐标
        actual_x = gb_state->x + x + 10;  // 左边距10px
        actual_y = gb_state->y + y + 25;  // 标题高度约25px
    }
    
    auto it = g_windows.find(actual_parent);
    if (it == g_windows.end()) return 0;

    WindowState* state = it->second;

    EmojiButton button;
    button.id = (int)state->buttons.size() + 1000;
    button.emoji = Utf8ToWide(emoji_bytes, emoji_len);
    button.text = Utf8ToWide(text_bytes, text_len);
    button.x = actual_x;
    button.y = actual_y + GetTitleBarOffset(actual_parent);
    button.width = width;
    button.height = height;
    button.bg_color = bg_color;
    button.is_hovered = false;
    button.is_pressed = false;

    state->buttons.push_back(button);
    
    // 如果按钮属于分组框，记录按钮ID
    if (parent_groupbox) {
        parent_groupbox->button_ids.push_back(button.id);
    }

    // 立即触发重绘并更新，确保按钮立即显示
    InvalidateRect(actual_parent, nullptr, FALSE);
    UpdateWindow(actual_parent);

    return button.id;
}

// Set button click callback
void __stdcall set_button_click_callback(ButtonClickCallback callback) {
    g_button_callback = callback;
}

// 启用按钮
void __stdcall EnableButton(HWND parent_hwnd, int button_id, BOOL enable) {
    auto it = g_windows.find(parent_hwnd);
    if (it == g_windows.end()) return;

    WindowState* state = it->second;
    for (auto& button : state->buttons) {
        if (button.id == button_id) {
            button.enabled = (enable != 0);
            // 触发重绘
            InvalidateRect(parent_hwnd, nullptr, FALSE);
            break;
        }
    }
}

// 禁用按钮
void __stdcall DisableButton(HWND parent_hwnd, int button_id) {
    EnableButton(parent_hwnd, button_id, FALSE);
}

void __stdcall set_message_loop_main_window(HWND hwnd) {
    g_message_loop_main_window = hwnd;
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
                RemoveWindowSubclass(h, TabControlSubclassProc, 0);
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

// .ico 文件格式结构（用于从内存加载）
#pragma pack(push, 1)
struct IcoDirEntry {
    BYTE bWidth;
    BYTE bHeight;
    BYTE bColorCount;
    BYTE bReserved;
    WORD wPlanes;
    WORD wBitCount;
    DWORD dwBytesInRes;
    DWORD dwImageOffset;
};
#pragma pack(pop)

// 从字节集设置窗口图标（支持 .ico 文件格式，易语言可插入图片资源后传入字节集）
void __stdcall set_window_icon_bytes(HWND hwnd, const unsigned char* icon_data, int data_len) {
    if (!hwnd || !icon_data || data_len < 6) return;

    HICON hIcon = nullptr;

    // 解析 .ico 文件格式：ICONDIR(6字节) + ICONDIRENTRY(16字节)*N
    if (data_len >= 6 + 16) {
        WORD idCount = *(const WORD*)(icon_data + 4);
        if (idCount > 0 && data_len >= 6 + 16) {
            const IcoDirEntry* entry = (const IcoDirEntry*)(icon_data + 6);
            DWORD offset = entry->dwImageOffset;
            DWORD size = entry->dwBytesInRes;
            int w = (entry->bWidth == 0) ? 256 : entry->bWidth;
            int h = (entry->bHeight == 0) ? 256 : entry->bHeight;

            if (offset + size <= (DWORD)data_len && size > 0) {
                hIcon = (HICON)CreateIconFromResourceEx(
                    (PBYTE)icon_data + offset,
                    size,
                    TRUE,
                    0x00030000,
                    w, h,
                    LR_DEFAULTCOLOR
                );
            }
        }
    }

    // 若 CreateIconFromResourceEx 失败（如 PNG 图标），尝试临时文件方式
    if (!hIcon) {
        wchar_t tempPath[MAX_PATH], tempFile[MAX_PATH];
        if (GetTempPathW(MAX_PATH, tempPath) && GetTempFileNameW(tempPath, L"ico", 0, tempFile)) {
            HANDLE hFile = CreateFileW(tempFile, GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
            if (hFile != INVALID_HANDLE_VALUE) {
                DWORD written;
                WriteFile(hFile, icon_data, data_len, &written, nullptr);
                CloseHandle(hFile);
                if (written == (DWORD)data_len) {
                    hIcon = (HICON)LoadImageW(nullptr, tempFile, IMAGE_ICON, 0, 0, LR_LOADFROMFILE | LR_DEFAULTSIZE);
                }
                DeleteFileW(tempFile);
            }
        }
    }

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

    // 更新WindowState中的title用于D2D彩色emoji绘制
    auto it = g_windows.find(hwnd);
    if (it != g_windows.end()) {
        it->second->title = wtitle;
        InvalidateRect(hwnd, nullptr, FALSE);
    }
}

// 设置窗口标题栏颜色
void __stdcall set_window_titlebar_color(HWND hwnd, UINT32 color) {
    if (!hwnd) return;
    auto it = g_windows.find(hwnd);
    if (it != g_windows.end()) {
        it->second->titlebar_color = color;
        InvalidateRect(hwnd, nullptr, FALSE);
    }
}

// 设置窗口客户区背景色（ARGB，0=使用 ThemeColor_Background 纯白）
extern "C" __declspec(dllexport) void __stdcall SetWindowBackgroundColor(HWND hwnd, UINT32 color) {
    if (!hwnd) return;
    auto it = g_windows.find(hwnd);
    if (it != g_windows.end()) {
        it->second->client_bg_color = color;
        InvalidateRect(hwnd, nullptr, FALSE);
    }
}

// ========== 窗口属性命令 ==========

// Wide String to UTF-8 (辅助函数 - 用于窗口属性)
static std::string WindowWideToUtf8(const std::wstring& wide) {
    if (wide.empty()) return "";
    
    int utf8_len = WideCharToMultiByte(CP_UTF8, 0, wide.c_str(), (int)wide.length(), nullptr, 0, nullptr, nullptr);
    if (utf8_len <= 0) return "";
    
    std::string result(utf8_len, 0);
    WideCharToMultiByte(CP_UTF8, 0, wide.c_str(), (int)wide.length(), &result[0], utf8_len, nullptr, nullptr);
    return result;
}

// 获取窗口标题 (UTF-8编码，两次调用模式)
extern "C" __declspec(dllexport) int __stdcall GetWindowTitle(HWND hwnd, unsigned char* buffer, int buffer_size) {
    if (!hwnd) return -1;
    
    // 使用GetWindowTextW获取窗口标题
    int title_len = GetWindowTextLengthW(hwnd);
    if (title_len <= 0) {
        // 窗口标题为空
        if (buffer == nullptr) {
            return 0;
        }
        return 0;
    }
    
    std::wstring wtitle(title_len + 1, 0);
    GetWindowTextW(hwnd, &wtitle[0], title_len + 1);
    wtitle.resize(title_len);  // 移除多余的null字符
    
    // 转换为UTF-8
    std::string utf8_title = WindowWideToUtf8(wtitle);
    
    // 第一次调用：返回所需长度
    if (buffer == nullptr) {
        return (int)utf8_title.size();
    }
    
    // 第二次调用：复制数据
    if (buffer_size < (int)utf8_title.size()) return -1;
    memcpy(buffer, utf8_title.c_str(), utf8_title.size());
    return (int)utf8_title.size();
}

// 获取窗口位置和大小
extern "C" __declspec(dllexport) int __stdcall GetWindowBounds(HWND hwnd, int* x, int* y, int* width, int* height) {
    if (!hwnd) return -1;
    
    // 验证输出参数
    if (!x || !y || !width || !height) return -1;
    
    // 使用GetWindowRect获取窗口位置和大小
    RECT rect;
    if (!GetWindowRect(hwnd, &rect)) {
        return -1;  // 获取失败
    }
    
    // 返回窗口位置和大小
    *x = rect.left;
    *y = rect.top;
    *width = rect.right - rect.left;
    *height = rect.bottom - rect.top;
    
    return 0;  // 成功
}

// 设置窗口位置和大小
extern "C" __declspec(dllexport) void __stdcall SetWindowBounds(HWND hwnd, int x, int y, int width, int height) {
    if (!hwnd) return;
    
    // 验证参数
    if (width <= 0 || height <= 0) return;
    
    // 使用MoveWindow设置窗口位置和大小
    // SWP_NOZORDER: 保持窗口Z顺序
    // SWP_NOACTIVATE: 不激活窗口
    SetWindowPos(hwnd, nullptr, x, y, width, height, SWP_NOZORDER | SWP_NOACTIVATE);
}

// 获取窗口可视状态
extern "C" __declspec(dllexport) int __stdcall GetWindowVisible(HWND hwnd) {
    if (!hwnd) return -1;
    
    // 使用IsWindowVisible检查窗口是否可见
    return IsWindowVisible(hwnd) ? 1 : 0;
}

// 显示或隐藏窗口
extern "C" __declspec(dllexport) void __stdcall ShowEmojiWindow(HWND hwnd, int visible) {
    if (!hwnd) return;
    
    // 使用ShowWindow设置窗口可视状态
    // visible为1时显示窗口，为0时隐藏窗口
    ShowWindow(hwnd, visible ? SW_SHOW : SW_HIDE);
}

// 获取窗口标题栏颜色 (RGB格式)
extern "C" __declspec(dllexport) UINT32 __stdcall GetWindowTitlebarColor(HWND hwnd) {
    if (!hwnd) return 0;
    
    // 查找窗口状态
    auto it = g_windows.find(hwnd);
    if (it == g_windows.end()) {
        return 0;  // 窗口不存在，返回0
    }
    
    // 返回标题栏颜色 (RGB格式，去掉alpha通道)
    return it->second->titlebar_color & 0x00FFFFFF;
}

// 设置标题栏文字颜色
extern "C" __declspec(dllexport) int __stdcall SetTitleBarTextColor(HWND hwnd, UINT32 color) {
    auto it = g_windows.find(hwnd);
    if (it == g_windows.end()) return 0;

    it->second->titlebar_text_color = color;
    InvalidateRect(hwnd, nullptr, FALSE);
    return 1;
}

// 获取标题栏文字颜色
extern "C" __declspec(dllexport) UINT32 __stdcall GetTitleBarTextColor(HWND hwnd) {
    auto it = g_windows.find(hwnd);
    if (it == g_windows.end()) return 0;

    return it->second->titlebar_text_color;
}

// 设置标题栏字体和字号
extern "C" __declspec(dllexport) int __stdcall SetTitleBarFont(HWND hwnd, const unsigned char* fontName, int fontNameLen, float fontSize) {
    if (!fontName || fontNameLen <= 0 || fontSize <= 0) return 0;

    auto it = g_windows.find(hwnd);
    if (it == g_windows.end()) return 0;

    it->second->titlebar_font_name = Utf8ToWide(fontName, fontNameLen);
    it->second->titlebar_font_size = fontSize;
    InvalidateRect(hwnd, nullptr, FALSE);
    return 1;
}

// 设置标题栏文字对齐方式
extern "C" __declspec(dllexport) int __stdcall SetTitleBarAlignment(HWND hwnd, int alignment) {
    if (alignment < 0 || alignment > 2) return 0;

    auto it = g_windows.find(hwnd);
    if (it == g_windows.end()) return 0;

    it->second->titlebar_alignment = alignment;
    InvalidateRect(hwnd, nullptr, FALSE);
    return 1;
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

// ========== 弹出菜单窗口过程 ==========
LRESULT CALLBACK PopupMenuProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
    PopupMenuState* state = nullptr;
    
    if (msg == WM_CREATE) {
        // 从 CREATESTRUCT 中获取 lpParam（即 state 指针）
        CREATESTRUCT* cs = (CREATESTRUCT*)lparam;
        state = (PopupMenuState*)cs->lpCreateParams;
        
        // 将 state 存储到窗口数据中
        SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)state);
        
        // 同时添加到全局映射
        if (state) {
            g_popup_menus[hwnd] = state;
        }
    } else {
        // 从窗口数据中获取 state
        state = (PopupMenuState*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
    }

    switch (msg) {
    case WM_CREATE: {
        FILE* log = nullptr;
        fopen_s(&log, "C:\\popup_menu_debug.txt", "a");
        if (log) {
            fprintf(log, "WM_CREATE: hwnd=%p, state=%p\n", hwnd, state);
            fclose(log);
        }
        
        // 初始化D2D渲染目标
        if (state && !state->render_target) {
            RECT rc;
            GetClientRect(hwnd, &rc);
            
            D2D1_SIZE_U size = D2D1::SizeU(rc.right - rc.left, rc.bottom - rc.top);
            D2D1_RENDER_TARGET_PROPERTIES props = D2D1::RenderTargetProperties();
            D2D1_HWND_RENDER_TARGET_PROPERTIES hwndProps = D2D1::HwndRenderTargetProperties(hwnd, size);
            
            if (g_d2d_factory) {
                HRESULT hr = g_d2d_factory->CreateHwndRenderTarget(props, hwndProps, &state->render_target);
                
                FILE* log2 = nullptr;
                fopen_s(&log2, "C:\\popup_menu_debug.txt", "a");
                if (log2) {
                    fprintf(log2, "CreateHwndRenderTarget result: hr=0x%08X, render_target=%p\n", 
                        hr, state->render_target);
                    fclose(log2);
                }
            } else {
                FILE* log3 = nullptr;
                fopen_s(&log3, "C:\\popup_menu_debug.txt", "a");
                if (log3) {
                    fprintf(log3, "ERROR: g_d2d_factory is NULL\n");
                    fclose(log3);
                }
            }
            
            if (g_dwrite_factory) {
                state->dwrite_factory = g_dwrite_factory;
                state->dwrite_factory->AddRef();
            }
        }
        return 0;
    }

    case WM_PAINT: {
        if (state && state->render_target) {
            PAINTSTRUCT ps;
            BeginPaint(hwnd, &ps);

            char debug_msg[256];
            sprintf_s(debug_msg, "WM_PAINT: state=%p, items.size()=%zu\n", state, state->items.size());
            OutputDebugStringA(debug_msg);

            state->render_target->BeginDraw();
            
            // 清除背景
            UINT32 bg = ThemeColor_Background();
            state->render_target->Clear(D2D1::ColorF(
                ((bg >> 16) & 0xFF) / 255.0f,
                ((bg >> 8) & 0xFF) / 255.0f,
                (bg & 0xFF) / 255.0f,
                1.0f));
            
            // 绘制菜单项
            DrawPopupMenu(state->render_target, state->dwrite_factory, state);
            
            state->render_target->EndDraw();
            EndPaint(hwnd, &ps);
        } else {
            char debug_msg[256];
            sprintf_s(debug_msg, "WM_PAINT: state=%p, render_target=%p\n", 
                state, state ? state->render_target : nullptr);
            OutputDebugStringA(debug_msg);
        }
        return 0;
    }

    case WM_MOUSEMOVE: {
        if (state) {
            int x = LOWORD(lparam);
            int y = HIWORD(lparam);
            int old_hovered = state->hovered_index;
            state->hovered_index = -1;
            
            float fx = (float)x;
            float fy = (float)y;
            for (size_t i = 0; i < state->items.size(); i++) {
                const D2D1_RECT_F& bounds = state->items[i].bounds;
                if (fx >= bounds.left && fx <= bounds.right && 
                    fy >= bounds.top && fy <= bounds.bottom) {
                    state->hovered_index = (int)i;
                    break;
                }
            }
            
            if (old_hovered != state->hovered_index) {
                InvalidateRect(hwnd, nullptr, FALSE);
            }
        }
        return 0;
    }

    case WM_LBUTTONDOWN: {
        if (state) {
            int x = LOWORD(lparam);
            int y = HIWORD(lparam);
            float fx = (float)x;
            float fy = (float)y;
            
            for (size_t i = 0; i < state->items.size(); i++) {
                const D2D1_RECT_F& bounds = state->items[i].bounds;
                if (fx >= bounds.left && fx <= bounds.right && 
                    fy >= bounds.top && fy <= bounds.bottom) {
                    MenuItem& item = state->items[i];
                    if (item.enabled && !item.separator) {
                        // 触发回调
                        if (state->callback) {
                            state->callback(0, item.id);
                        }
                        // 关闭菜单
                        DestroyWindow(hwnd);
                    }
                    return 0;
                }
            }
        }
        return 0;
    }

    case WM_KEYDOWN: {
        if (state) {
            if (wparam == VK_UP) {
                // 向上切换
                int old_index = state->hovered_index;
                do {
                    state->hovered_index--;
                    if (state->hovered_index < 0) {
                        state->hovered_index = (int)state->items.size() - 1;
                    }
                    // 跳过分隔符
                    if (state->hovered_index == old_index) break; // 防止死循环
                } while (state->hovered_index >= 0 && 
                         state->hovered_index < (int)state->items.size() &&
                         state->items[state->hovered_index].separator);
                
                InvalidateRect(hwnd, nullptr, FALSE);
                return 0;
            }
            else if (wparam == VK_DOWN) {
                // 向下切换
                int old_index = state->hovered_index;
                do {
                    state->hovered_index++;
                    if (state->hovered_index >= (int)state->items.size()) {
                        state->hovered_index = 0;
                    }
                    // 跳过分隔符
                    if (state->hovered_index == old_index) break; // 防止死循环
                } while (state->hovered_index >= 0 && 
                         state->hovered_index < (int)state->items.size() &&
                         state->items[state->hovered_index].separator);
                
                InvalidateRect(hwnd, nullptr, FALSE);
                return 0;
            }
            else if (wparam == VK_RETURN || wparam == VK_SPACE) {
                // 触发当前选中项
                if (state->hovered_index >= 0 && state->hovered_index < (int)state->items.size()) {
                    MenuItem& item = state->items[state->hovered_index];
                    if (item.enabled && !item.separator) {
                        if (state->callback) {
                            state->callback(0, item.id);
                        }
                        DestroyWindow(hwnd);
                    }
                }
                return 0;
            }
            else if (wparam == VK_ESCAPE) {
                // 关闭菜单
                DestroyWindow(hwnd);
                return 0;
            }
        }
        break;
    }

    case WM_KILLFOCUS: {
        // 失去焦点时关闭菜单
        DestroyWindow(hwnd);
        return 0;
    }

    case WM_DESTROY: {
        // 清理资源
        if (state) {
            if (state->render_target) {
                state->render_target->Release();
                state->render_target = nullptr;
            }
            if (state->dwrite_factory) {
                state->dwrite_factory->Release();
                state->dwrite_factory = nullptr;
            }
            delete state;
            g_popup_menus.erase(hwnd);
        }
        return 0;
    }
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
    state->ok_button.bg_color = ThemeColor_Primary();  // Element UI primary (theme)
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

// EnumChildWindows 回调：显示子窗口并强制重绘（解决 Tab 切换后 TreeView 等不显示）
static BOOL CALLBACK ShowAndInvalidateChildProc(HWND hChild, LPARAM) {
    if (IsWindow(hChild)) {
        ShowWindow(hChild, SW_SHOW);
        SendMessage(hChild, WM_EW_TABPAGE_VISIBLE, 0, 0);  // 通知 D2D 控件重建渲染目标
        InvalidateRect(hChild, nullptr, TRUE);
    }
    return TRUE;
}

// Tab 内容窗口过程：支持 D2D 渲染和 Emoji 按钮
static const wchar_t* TAB_CONTENT_D2D_CLASS = L"TabContentD2DClass";

static void TabContentEnsureRenderTarget(HWND hwnd, WindowState* state) {
    if (!state || !g_d2d_factory) return;
    RECT rc;
    GetClientRect(hwnd, &rc);
    int w = rc.right - rc.left;
    int h = rc.bottom - rc.top;
    if (w < 1 || h < 1) return;
    if (state->render_target) {
        state->render_target->Release();
        state->render_target = nullptr;
    }
    ID2D1HwndRenderTarget* rt = nullptr;
    HRESULT hr = g_d2d_factory->CreateHwndRenderTarget(
        D2D1::RenderTargetProperties(),
        D2D1::HwndRenderTargetProperties(hwnd, D2D1::SizeU((UINT)w, (UINT)h)),
        &rt
    );
    if (SUCCEEDED(hr) && rt) {
        UINT dpi = GetDpiForWindow(hwnd);
        if (dpi > 0) rt->SetDpi((float)dpi, (float)dpi);
        state->render_target = rt;
        state->dwrite_factory = g_dwrite_factory;
    }
}

static LRESULT CALLBACK TabContentWindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
    auto it = g_windows.find(hwnd);
    if (it == g_windows.end() && msg != WM_CREATE) {
        return DefWindowProcW(hwnd, msg, wparam, lparam);
    }
    WindowState* state = (it != g_windows.end()) ? it->second : nullptr;

    switch (msg) {
    case WM_CREATE: {
        WindowState* new_state = new WindowState();
        new_state->hwnd = hwnd;
        new_state->custom_titlebar = false;
        new_state->titlebar_height = 0;
        new_state->client_bg_color = 0;
        g_windows[hwnd] = new_state;
        return 0;
    }
    case WM_SIZE: {
        if (state) TabContentEnsureRenderTarget(hwnd, state);
        break;
    }
    case WM_SHOWWINDOW:
        if (wparam && state) TabContentEnsureRenderTarget(hwnd, state);
        break;
    case WM_EW_TABPAGE_VISIBLE:
        if (state) TabContentEnsureRenderTarget(hwnd, state);
        return 0;
    case WM_PAINT: {
        if (state && state->render_target) {
            PAINTSTRUCT ps;
            BeginPaint(hwnd, &ps);
            state->render_target->BeginDraw();
            UINT32 win_bg = (state->client_bg_color != 0) ? state->client_bg_color : ThemeColor_Background();
            state->render_target->Clear(D2D1::ColorF(
                ((win_bg >> 16) & 0xFF) / 255.0f,
                ((win_bg >> 8) & 0xFF) / 255.0f,
                (win_bg & 0xFF) / 255.0f, 1.0f));
            for (const auto& button : state->buttons) {
                bool should_draw = true;
                for (const auto& gb_pair : g_groupboxes) {
                    GroupBoxState* gb = gb_pair.second;
                    if (std::find(gb->button_ids.begin(), gb->button_ids.end(), button.id) != gb->button_ids.end()) {
                        should_draw = gb->visible;
                        break;
                    }
                }
                if (should_draw) DrawButton(state->render_target, state->dwrite_factory, button);
            }
            for (const auto& pair : g_labels) {
                LabelState* label = pair.second;
                if (label && label->parent == hwnd && label->parent_drawn) {
                    DrawParentLabel(state->render_target, state->dwrite_factory, label);
                }
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
                if (button.enabled && button.visible && button.ContainsPoint(x, y)) {
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
                if (button.is_pressed && button.enabled && button.visible && button.ContainsPoint(x, y)) {
                    button.is_pressed = false;
                    if (g_button_callback) g_button_callback(button.id, hwnd);
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
                bool hovered = button.enabled && button.visible && button.ContainsPoint(x, y);
                if (hovered != button.is_hovered) {
                    button.is_hovered = hovered;
                    needs_redraw = true;
                }
            }
            if (needs_redraw) InvalidateRect(hwnd, nullptr, FALSE);
        }
        return 0;
    }
    case WM_DESTROY: {
        if (state) {
            if (state->render_target) {
                state->render_target->Release();
                state->render_target = nullptr;
            }
            delete state;
            g_windows.erase(hwnd);
        }
        return 0;
    }
    }
    return DefWindowProcW(hwnd, msg, wparam, lparam);
}

// 更新 Tab 布局（显示/隐藏内容窗口）
// 所有内容窗口保持在同一位置叠加，仅用 ShowWindow 控制可见性
void UpdateTabLayout(TabControlState* state) {
    if (!state || !state->hTabControl) return;

    RECT rcTab;
    GetClientRect(state->hTabControl, &rcTab);
    TabCtrl_AdjustRect(state->hTabControl, FALSE, &rcTab);

    int w = rcTab.right - rcTab.left;
    int h = rcTab.bottom - rcTab.top;
    if (w < 1) w = 1;
    if (h < 1) h = 1;

    for (size_t i = 0; i < state->pages.size(); i++) {
        TabPageInfo& page = state->pages[i];
        if (page.hContentWindow && IsWindow(page.hContentWindow)) {
            SetWindowPos(page.hContentWindow, nullptr, rcTab.left, rcTab.top, w, h,
                SWP_NOZORDER | SWP_NOACTIVATE);
            if ((int)i == state->currentIndex) {
                SetWindowPos(page.hContentWindow, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
                ShowWindow(page.hContentWindow, SW_SHOW);  // 确保子控件收到 WM_SHOWWINDOW
                SendMessage(page.hContentWindow, WM_EW_TABPAGE_VISIBLE, 0, 0);  // Tab 内容窗口 D2D 重建
                EnumChildWindows(page.hContentWindow, ShowAndInvalidateChildProc, 0);
                RedrawWindow(page.hContentWindow, nullptr, nullptr,
                    RDW_INVALIDATE | RDW_ERASE | RDW_UPDATENOW | RDW_ALLCHILDREN);
            } else {
                ShowWindow(page.hContentWindow, SW_HIDE);  // 隐藏以让 D2D 控件在显示时重建渲染目标
            }
        }
    }
}

// ========== TabControl 图标解码辅助函数 ==========
// 从 PNG 字节数据解码为 D2D1Bitmap（用于标签页图标绘制）
static ID2D1Bitmap* CreateBitmapFromPNGData(ID2D1RenderTarget* rt, const std::vector<unsigned char>& pngData) {
    if (!rt || pngData.empty() || !EnsureWicFactoryInitialized()) return nullptr;

    IWICStream* stream = nullptr;
    HRESULT hr = g_wic_factory->CreateStream(&stream);
    if (FAILED(hr) || !stream) return nullptr;

    hr = stream->InitializeFromMemory((BYTE*)pngData.data(), (DWORD)pngData.size());
    if (FAILED(hr)) { stream->Release(); return nullptr; }

    IWICBitmapDecoder* decoder = nullptr;
    hr = g_wic_factory->CreateDecoderFromStream(stream, nullptr, WICDecodeMetadataCacheOnLoad, &decoder);
    stream->Release();
    if (FAILED(hr) || !decoder) return nullptr;

    IWICBitmapFrameDecode* frame = nullptr;
    hr = decoder->GetFrame(0, &frame);
    decoder->Release();
    if (FAILED(hr) || !frame) return nullptr;

    IWICFormatConverter* converter = nullptr;
    hr = g_wic_factory->CreateFormatConverter(&converter);
    if (SUCCEEDED(hr) && converter) {
        hr = converter->Initialize(
            frame, GUID_WICPixelFormat32bppPBGRA,
            WICBitmapDitherTypeNone, nullptr, 0.0, WICBitmapPaletteTypeMedianCut);
    }
    frame->Release();
    if (FAILED(hr) || !converter) { if (converter) converter->Release(); return nullptr; }

    ID2D1Bitmap* bitmap = nullptr;
    hr = rt->CreateBitmapFromWicBitmap(converter, nullptr, &bitmap);
    converter->Release();
    return SUCCEEDED(hr) ? bitmap : nullptr;
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
            // ── 2.7: TCN_SELCHANGING 中拦截禁用标签页 ──
            if (pnmhdr->code == TCN_SELCHANGING) {
                // 获取即将切换到的目标标签页索引
                // TCN_SELCHANGING 时，TabCtrl_GetCurSel 返回的是当前选中页（即将离开的页）
                // 我们需要通过鼠标位置或键盘来判断目标页
                // 但 Win32 Tab Control 在 TCN_SELCHANGING 时不直接提供目标索引
                // 使用 TCM_HITTEST 获取鼠标点击的目标标签页
                POINT pt;
                GetCursorPos(&pt);
                ScreenToClient(hTabControl, &pt);
                TCHITTESTINFO hti = {};
                hti.pt = pt;
                int targetVisibleIdx = TabCtrl_HitTest(hTabControl, &hti);
                // 将 Win32 可见索引映射到 pages 数组索引
                int targetIdx = VisibleIndexToPageIndex(state, targetVisibleIdx);
                if (targetIdx >= 0 && targetIdx < (int)state->pages.size()) {
                    if (!state->pages[targetIdx].enabled) {
                        // 目标标签页被禁用，返回 TRUE 阻止切换
                        SetWindowLongPtr(hwnd, DWLP_MSGRESULT, TRUE);
                        return TRUE;
                    }
                }
            }
            else if (pnmhdr->code == TCN_SELCHANGE) {
                // Tab 切换事件
                int newVisibleIndex = TabCtrl_GetCurSel(hTabControl);
                // 将 Win32 可见索引映射到 pages 数组索引
                int newIndex = VisibleIndexToPageIndex(state, newVisibleIndex);
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
    // 重构：从 TabControlState 读取所有样式参数，支持图标、关闭按钮、禁用状态
    case WM_DRAWITEM: {
        DRAWITEMSTRUCT* dis = (DRAWITEMSTRUCT*)lparam;
        // 只处理属于此 TabControl 的绘制请求
        if (dis->CtlType != ODT_TAB || dis->hwndItem != hTabControl) break;

        int visibleTabIdx = (int)dis->itemID;
        // 将 Win32 可见索引映射到 pages 数组索引
        int tabIdx = VisibleIndexToPageIndex(state, visibleTabIdx);
        // 若标题尚未加入 pages（极少数时序问题），跳过，让系统默认处理
        if (tabIdx < 0 || tabIdx >= (int)state->pages.size()) break;

        const std::wstring& title = state->pages[tabIdx].title;
        bool isSelected = (dis->itemState & ODS_SELECTED) != 0;
        bool isEnabled = state->pages[tabIdx].enabled;  // 2.4: 禁用状态

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

            // 2.4: 禁用标签页使用 50% 透明度
            float opacity = isEnabled ? 1.0f : 0.5f;

            // ── 3. 背景填充（2.1: 从 state 读取颜色）──
            UINT32 bgArgb = isSelected ? state->selectedBgColor : state->unselectedBgColor;
            D2D1_COLOR_F bgColor = ColorFromUInt32(bgArgb);
            bgColor.a *= opacity;  // 禁用时半透明

            ID2D1SolidColorBrush* bgBrush = nullptr;
            dcRT->CreateSolidColorBrush(bgColor, &bgBrush);
            D2D1_RECT_F fillRect = D2D1::RectF(0.0f, 0.0f, (FLOAT)w, (FLOAT)h);
            dcRT->FillRectangle(fillRect, bgBrush);
            bgBrush->Release();

            // ── 4. 图标绘制（2.5）+ 文字（2.2）+ 关闭按钮（2.6）──
            float textLeft = (float)state->paddingH;   // 2.2: 使用 paddingH
            float textRight = (float)(w - state->paddingH);
            float closeBtnSize = 16.0f;  // 关闭按钮区域大小
            float closeBtnMargin = 4.0f; // 关闭按钮与文字间距

            // 2.5: 图标绘制
            float iconSize = 16.0f;
            float iconGap = 4.0f;
            bool hasIcon = !state->pages[tabIdx].iconData.empty();
            if (hasIcon) {
                ID2D1Bitmap* iconBitmap = CreateBitmapFromPNGData(dcRT, state->pages[tabIdx].iconData);
                if (iconBitmap) {
                    float iconY = ((float)h - iconSize) / 2.0f;
                    D2D1_RECT_F iconRect = D2D1::RectF(textLeft, iconY, textLeft + iconSize, iconY + iconSize);
                    dcRT->DrawBitmap(iconBitmap, iconRect, opacity, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR);
                    iconBitmap->Release();
                    textLeft += iconSize + iconGap;  // 文字起始位置右移
                }
            }

            // 2.6: 关闭按钮占位（预留右侧空间）
            if (state->closable) {
                textRight -= (closeBtnSize + closeBtnMargin);
            }

            if (!title.empty() && g_dwrite_factory) {
                IDWriteTextFormat* fmt = nullptr;
                // 2.2: 从 state 读取字体名称和字号
                g_dwrite_factory->CreateTextFormat(
                    state->fontName.c_str(),
                    nullptr,
                    DWRITE_FONT_WEIGHT_NORMAL,
                    DWRITE_FONT_STYLE_NORMAL,
                    DWRITE_FONT_STRETCH_NORMAL,
                    state->fontSize,
                    L"zh-CN",
                    &fmt
                );
                if (fmt) {
                    fmt->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
                    fmt->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);

                    // 2.2: 从 state 读取文字颜色
                    UINT32 textArgb = isSelected ? state->selectedTextColor : state->unselectedTextColor;
                    D2D1_COLOR_F textColor = ColorFromUInt32(textArgb);
                    textColor.a *= opacity;  // 禁用时半透明

                    ID2D1SolidColorBrush* textBrush = nullptr;
                    dcRT->CreateSolidColorBrush(textColor, &textBrush);

                    // 2.2: 使用 paddingH/paddingV 计算文字绘制区域
                    D2D1_RECT_F textRect = D2D1::RectF(
                        textLeft, (float)state->paddingV,
                        textRight, (FLOAT)(h - (isSelected ? 2.0f : 0.0f) - state->paddingV)
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

            // ── 2.6: 关闭按钮绘制 ──
            if (state->closable) {
                float btnX = (float)w - closeBtnSize - closeBtnMargin;
                float btnY = ((float)h - closeBtnSize) / 2.0f;
                bool isCloseHovered = (state->hoveredCloseTabIndex == tabIdx);

                // 悬停时绘制红色背景圆角矩形
                if (isCloseHovered) {
                    ID2D1SolidColorBrush* closeBgBrush = nullptr;
                    D2D1_COLOR_F closeBgColor = D2D1::ColorF(0xF56C6C);  // Element UI danger 红色
                    closeBgColor.a = opacity;
                    dcRT->CreateSolidColorBrush(closeBgColor, &closeBgBrush);
                    D2D1_ROUNDED_RECT closeRoundedRect = D2D1::RoundedRect(
                        D2D1::RectF(btnX - 2, btnY - 2, btnX + closeBtnSize + 2, btnY + closeBtnSize + 2),
                        3.0f, 3.0f);
                    dcRT->FillRoundedRectangle(closeRoundedRect, closeBgBrush);
                    closeBgBrush->Release();
                }

                // 绘制 × 符号
                ID2D1SolidColorBrush* closeBrush = nullptr;
                D2D1_COLOR_F closeColor = isCloseHovered
                    ? D2D1::ColorF(1.0f, 1.0f, 1.0f, opacity)   // 悬停时白色
                    : D2D1::ColorF(0.6f, 0.6f, 0.6f, opacity);  // 默认灰色
                dcRT->CreateSolidColorBrush(closeColor, &closeBrush);
                float crossMargin = 4.0f;
                dcRT->DrawLine(
                    D2D1::Point2F(btnX + crossMargin, btnY + crossMargin),
                    D2D1::Point2F(btnX + closeBtnSize - crossMargin, btnY + closeBtnSize - crossMargin),
                    closeBrush, 1.2f);
                dcRT->DrawLine(
                    D2D1::Point2F(btnX + closeBtnSize - crossMargin, btnY + crossMargin),
                    D2D1::Point2F(btnX + crossMargin, btnY + closeBtnSize - crossMargin),
                    closeBrush, 1.2f);
                closeBrush->Release();
            }

            // ── 5. 选中指示条（2.3: 从 state 读取颜色）──
            if (isSelected) {
                D2D1_COLOR_F indicatorColor = ColorFromUInt32(state->indicatorColor);
                indicatorColor.a *= opacity;
                ID2D1SolidColorBrush* lineBrush = nullptr;
                dcRT->CreateSolidColorBrush(indicatorColor, &lineBrush);
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
        icex.dwICC = ICC_TAB_CLASSES | ICC_TREEVIEW_CLASSES;
        InitCommonControlsEx(&icex);
        comctl_initialized = true;
    }

    // 创建 Tab Control（TCS_OWNERDRAWFIXED：父窗口自绘标签；TCS_FLATBUTTONS：扁平样式，与设计器一致）
    int tb_offset = GetTitleBarOffset(hParent);
    HWND hTabControl = CreateWindowExW(
        0,
        WC_TABCONTROLW,
        L"",
        WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | TCS_TABS | TCS_FOCUSNEVER
        | TCS_OWNERDRAWFIXED | TCS_FIXEDWIDTH | TCS_FLATBUTTONS,
        x, y + tb_offset, width, height,
        hParent,
        nullptr,
        GetModuleHandle(nullptr),
        nullptr
    );

    if (!hTabControl) return nullptr;

    // 设置固定标签尺寸（宽=120px 能容纳 Emoji + 中文；高=34px 给 Emoji 足够空间）
    SendMessage(hTabControl, TCM_SETITEMSIZE, 0, MAKELPARAM(120, 34));

    // 扁平化内容区边框，与设计器一致（去除 3D 凹陷）
    SetWindowTheme(hTabControl, L"", L"");

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

    // 外观字段默认值
    state->tabWidth = 120;
    state->tabHeight = 34;
    state->fontName = L"Segoe UI Emoji";
    state->fontSize = 13.0f;
    state->selectedBgColor = 0xFFFFFFFF;
    state->unselectedBgColor = 0xFFF5F7FA;
    state->selectedTextColor = 0xFF409EFF;
    state->unselectedTextColor = 0xFF606266;
    state->indicatorColor = 0xFF409EFF;
    state->paddingH = 2;
    state->paddingV = 0;

    // 交互字段默认值
    state->closable = false;
    state->closeCallback = nullptr;
    state->rightClickCallback = nullptr;
    state->dblClickCallback = nullptr;
    state->draggable = false;

    // 布局字段默认值
    state->tabPosition = 0;
    state->tabAlignment = 0;
    state->scrollable = false;
    state->scrollOffset = 0;

    // 绘制辅助字段默认值
    state->hoveredCloseTabIndex = -1;

    // 拖拽状态字段默认值
    state->isDragging = false;
    state->dragStartIndex = -1;
    state->dragTargetIndex = -1;
    state->dragStartPoint = { 0, 0 };

    // 保存到全局映射表
    g_tab_controls[hTabControl] = state;

    // 子类化父窗口以接收 WM_NOTIFY 消息
    // 注意：WM_NOTIFY 消息是发送给父窗口的，不是发送给 TabControl 的
    SetWindowSubclass(hParent, TabControlParentSubclassProc, (UINT_PTR)hTabControl, (DWORD_PTR)hTabControl);

    // 子类化 TabControl 自身以处理鼠标事件（右键、双击、拖拽）
    SetWindowSubclass(hTabControl, TabControlSubclassProc, 0, (DWORD_PTR)hTabControl);

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

        // 使用 TabContentD2DClass 支持 D2D 渲染和 Emoji 按钮
        static bool tab_content_class_registered = false;
        if (!tab_content_class_registered) {
            WNDCLASSW wc = {};
            wc.lpfnWndProc = TabContentWindowProc;
            wc.hInstance = GetModuleHandle(nullptr);
            wc.lpszClassName = TAB_CONTENT_D2D_CLASS;
            wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
            wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
            RegisterClassW(&wc);
            tab_content_class_registered = true;
        }

        hContentWindow = CreateWindowExW(
            0,
            TAB_CONTENT_D2D_CLASS,
            L"",
            WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
            0, 0, 0, 0,
            state->hTabControl,
            nullptr,
            GetModuleHandle(nullptr),
            nullptr
        );
        if (!hContentWindow) return -1;
    }

    // 添加 Tab 项到 TabControl
    TCITEMW tci = {};
    tci.mask = TCIF_TEXT;
    tci.pszText = (LPWSTR)title.c_str();

    int index = TabCtrl_GetItemCount(hTabControl);
    int result = TabCtrl_InsertItem(hTabControl, index, &tci);

    if (result == -1) {
        if (hContentWindow && IsWindow(hContentWindow)) {
            DestroyWindow(hContentWindow);
        }
        return -1;
    }

    // 保存 Tab 页信息
    TabPageInfo pageInfo;
    pageInfo.index = index;
    pageInfo.title = title;
    pageInfo.hContentWindow = hContentWindow;
    pageInfo.visible = true;   // 默认在标签栏中可见
    pageInfo.enabled = true;
    // iconData 默认为空（std::vector 默认构造）
    pageInfo.contentBgColor = 0xFFFFFFFF;

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

// 实际执行 Tab 切换（在消息泵中调用；勿在树选中易语言回调同栈同步 TabCtrl_SetCurSel，否则会 WM_NOTIFY/UpdateTabLayout 与 D2D 树重入崩溃）
static BOOL ApplySelectTab(HWND hTabControl, int index) {
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

// 切换到指定 Tab（投递到顶层主窗口延后执行，避免树回调内同步 SelectTab 崩溃）
BOOL __stdcall SelectTab(HWND hTabControl, int index) {
    auto it = g_tab_controls.find(hTabControl);
    if (it == g_tab_controls.end()) return FALSE;

    TabControlState* state = it->second;

    if (index < 0 || index >= (int)state->pages.size()) return FALSE;

    HWND root = GetAncestor(hTabControl, GA_ROOT);
    if (root && PostMessageW(root, WM_EW_SELECT_TAB_DEFERRED, (WPARAM)hTabControl, (LPARAM)(INT_PTR)index)) {
        return TRUE;
    }
    return ApplySelectTab(hTabControl, index);
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

    // 清理 TabControl 自身的子类化
    RemoveWindowSubclass(hTabControl, TabControlSubclassProc, 0);

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
            
            // 使用Direct2D渲染以支持彩色Emoji（先画到内存位图，再一次性 BitBlt 到窗口，避免缩放时闪烁/透底）
            int w = rect.right - rect.left;
            int h = rect.bottom - rect.top;
            if (w > 0 && h > 0 && g_d2d_factory && g_dwrite_factory) {
                HDC memDC = CreateCompatibleDC(hdc);
                if (memDC) {
                    BITMAPINFO bmi = {};
                    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
                    bmi.bmiHeader.biWidth = w;
                    bmi.bmiHeader.biHeight = -h;
                    bmi.bmiHeader.biPlanes = 1;
                    bmi.bmiHeader.biBitCount = 32;
                    bmi.bmiHeader.biCompression = BI_RGB;

                    void* pvBits = nullptr;
                    HBITMAP hBmp = CreateDIBSection(hdc, &bmi, DIB_RGB_COLORS, &pvBits, nullptr, 0);
                    if (hBmp) {
                        HBITMAP hOldBmp = (HBITMAP)SelectObject(memDC, hBmp);

                        ID2D1DCRenderTarget* rt = nullptr;
                        D2D1_RENDER_TARGET_PROPERTIES props = D2D1::RenderTargetProperties(
                            D2D1_RENDER_TARGET_TYPE_DEFAULT,
                            D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED)
                        );

                        if (SUCCEEDED(g_d2d_factory->CreateDCRenderTarget(&props, &rt))) {
                            RECT rcBind = { 0, 0, w, h };
                            rt->BindDC(memDC, &rcBind);
                            rt->SetTextAntialiasMode(D2D1_TEXT_ANTIALIAS_MODE_GRAYSCALE);
                            rt->BeginDraw();

                            // 绘制背景（解析主题颜色索引）
                            D2D1_COLOR_F bg_color = ColorFromUInt32(ResolveThemeColor(state->bg_color));
                            ID2D1SolidColorBrush* bg_brush = nullptr;
                            rt->CreateSolidColorBrush(bg_color, &bg_brush);
                            if (bg_brush) {
                                rt->FillRectangle(D2D1::RectF(0, 0, (FLOAT)w, (FLOAT)h), bg_brush);
                                bg_brush->Release();
                            }

                            // 创建文本格式（使用用户指定的字体，DirectWrite会自动 fallback 到 emoji 字体）
                            IDWriteTextFormat* text_format = nullptr;
                            HRESULT hr = g_dwrite_factory->CreateTextFormat(
                                state->font.font_name.c_str(),
                                nullptr,
                                state->font.bold ? DWRITE_FONT_WEIGHT_BOLD : DWRITE_FONT_WEIGHT_NORMAL,
                                state->font.italic ? DWRITE_FONT_STYLE_ITALIC : DWRITE_FONT_STYLE_NORMAL,
                                DWRITE_FONT_STRETCH_NORMAL,
                                (float)state->font.font_size,
                                L"zh-CN",
                                &text_format
                            );

                            if (SUCCEEDED(hr) && text_format) {
                                switch (state->alignment) {
                                case ALIGN_LEFT:
                                    text_format->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
                                    break;
                                case ALIGN_CENTER:
                                    text_format->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
                                    break;
                                case ALIGN_RIGHT:
                                    text_format->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_TRAILING);
                                    break;
                                }

                                if (state->word_wrap) {
                                    text_format->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
                                    text_format->SetWordWrapping(DWRITE_WORD_WRAPPING_WRAP);
                                } else {
                                    text_format->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
                                    text_format->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
                                }

                                IDWriteTextLayout* text_layout = nullptr;
                                hr = g_dwrite_factory->CreateTextLayout(
                                    state->text.c_str(),
                                    (UINT32)state->text.length(),
                                    text_format,
                                    (float)w,
                                    (float)h,
                                    &text_layout
                                );

                                if (SUCCEEDED(hr) && text_layout) {
                                    ID2D1SolidColorBrush* brush = nullptr;
                                    D2D1_COLOR_F fg_color = ColorFromUInt32(ResolveThemeColor(state->fg_color));
                                    rt->CreateSolidColorBrush(fg_color, &brush);
                                    if (brush) {
                                        rt->DrawTextLayout(
                                            D2D1::Point2F(0, 0),
                                            text_layout,
                                            brush,
                                            D2D1_DRAW_TEXT_OPTIONS_ENABLE_COLOR_FONT
                                        );
                                        brush->Release();
                                    }
                                    text_layout->Release();
                                }
                                text_format->Release();
                            }

                            rt->EndDraw();
                            rt->Release();
                        }

                        BitBlt(hdc, 0, 0, w, h, memDC, 0, 0, SRCCOPY);
                        SelectObject(memDC, hOldBmp);
                        DeleteObject(hBmp);
                    }
                    DeleteDC(memDC);
                }
            } else {
                // 降级到GDI渲染（如果Direct2D不可用）
                // 解析主题颜色索引
                UINT32 resolved_bg_gdi = ResolveThemeColor(state->bg_color);
                UINT32 resolved_fg_gdi = ResolveThemeColor(state->fg_color);
                HBRUSH hBrGdi = CreateSolidBrush(RGB(
                    (resolved_bg_gdi >> 16) & 0xFF,
                    (resolved_bg_gdi >> 8) & 0xFF,
                    resolved_bg_gdi & 0xFF
                ));
                FillRect(hdc, &rect, hBrGdi);
                DeleteObject(hBrGdi);

                SetTextColor(hdc, RGB(
                    (resolved_fg_gdi >> 16) & 0xFF,
                    (resolved_fg_gdi >> 8) & 0xFF,
                    resolved_fg_gdi & 0xFF
                ));
                SetBkMode(hdc, TRANSPARENT);
                
                HFONT hFont = CreateCustomFont(state->font);
                HFONT hOldFont = (HFONT)SelectObject(hdc, hFont);
                
                UINT format = 0;
                if (state->word_wrap) {
                    format = DT_WORDBREAK;
                } else {
                    format = DT_VCENTER | DT_SINGLELINE;
                }
                
                switch (state->alignment) {
                    case ALIGN_LEFT: format |= DT_LEFT; break;
                    case ALIGN_CENTER: format |= DT_CENTER; break;
                    case ALIGN_RIGHT: format |= DT_RIGHT; break;
                }
                
                DrawTextW(hdc, state->text.c_str(), -1, &rect, format);
                
                SelectObject(hdc, hOldFont);
                DeleteObject(hFont);
            }

            EndPaint(hwnd, &ps);
            return 0;
        }
        case WM_ERASEBKGND:
            return 1;  // 防止布局调整时标签先被系统擦背景导致闪烁
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
    int tb_offset = GetTitleBarOffset(hParent);
    int id = g_next_control_id++;
    HWND hEdit = CreateWindowExW(
        0,
        L"EDIT",
        text.c_str(),
        style,
        x, y + tb_offset, width, height,
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

// 获取编辑框字体信息
__declspec(dllexport) int __stdcall GetEditBoxFont(
    HWND hEdit,
    unsigned char* font_name_buffer,
    int font_name_buffer_size,
    int* font_size,
    int* bold,
    int* italic,
    int* underline
) {
    auto it = g_editboxes.find(hEdit);
    if (it == g_editboxes.end()) return -1;

    EditBoxState* state = it->second;
    std::string utf8_name = WindowWideToUtf8(state->font.font_name);

    if (font_size) *font_size = state->font.font_size;
    if (bold) *bold = state->font.bold ? 1 : 0;
    if (italic) *italic = state->font.italic ? 1 : 0;
    if (underline) *underline = state->font.underline ? 1 : 0;

    if (font_name_buffer == nullptr) {
        return (int)utf8_name.size();
    }
    if (font_name_buffer_size < (int)utf8_name.size()) return -1;
    memcpy(font_name_buffer, utf8_name.c_str(), utf8_name.size());
    return (int)utf8_name.size();
}

// 获取编辑框颜色
__declspec(dllexport) int __stdcall GetEditBoxColor(
    HWND hEdit,
    UINT32* fg_color,
    UINT32* bg_color
) {
    auto it = g_editboxes.find(hEdit);
    if (it == g_editboxes.end()) return -1;

    EditBoxState* state = it->second;
    if (fg_color) *fg_color = state->fg_color;
    if (bg_color) *bg_color = state->bg_color;
    return 0;
}

// 获取编辑框位置和大小
__declspec(dllexport) int __stdcall GetEditBoxBounds(
    HWND hEdit,
    int* x,
    int* y,
    int* width,
    int* height
) {
    if (!hEdit) return -1;
    RECT rc;
    if (!GetWindowRect(hEdit, &rc)) return -1;

    // 转换为父窗口客户区坐标
    HWND hParent = GetParent(hEdit);
    if (hParent) {
        POINT pt = { rc.left, rc.top };
        ScreenToClient(hParent, &pt);
        if (x) *x = pt.x;
        if (y) *y = pt.y;
    } else {
        if (x) *x = rc.left;
        if (y) *y = rc.top;
    }
    if (width) *width = rc.right - rc.left;
    if (height) *height = rc.bottom - rc.top;
    return 0;
}

// 获取编辑框对齐方式
__declspec(dllexport) int __stdcall GetEditBoxAlignment(
    HWND hEdit
) {
    auto it = g_editboxes.find(hEdit);
    if (it == g_editboxes.end()) return -1;
    return (int)it->second->alignment;
}

// 获取编辑框启用状态
__declspec(dllexport) int __stdcall GetEditBoxEnabled(
    HWND hEdit
) {
    if (!hEdit) return -1;
    return IsWindowEnabled(hEdit) ? 1 : 0;
}

// 获取编辑框可视状态
__declspec(dllexport) int __stdcall GetEditBoxVisible(
    HWND hEdit
) {
    if (!hEdit) return -1;
    return IsWindowVisible(hEdit) ? 1 : 0;
}

// 创建彩色Emoji编辑框（使用RichEdit控件）
// ⚠️ 注意：RichEdit 4.1 (MSFTEDIT_CLASS) 不支持彩色emoji！
// 彩色emoji需要RichEdit 8.0+（Windows 10 1809+）
// 当前实现：使用RichEdit 4.1，emoji显示为黑白
// 未来改进：检测Windows版本，如果支持则使用RichEdit 8.0
__declspec(dllexport) HWND __stdcall CreateColorEmojiEditBox(
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
    
    // 创建RichEdit控件样式
    DWORD style = WS_CHILD | WS_VISIBLE | WS_TABSTOP | ES_NOHIDESEL;
    
    if (multiline) {
        style |= ES_MULTILINE | ES_AUTOVSCROLL | WS_VSCROLL;
    } else {
        style |= ES_AUTOHSCROLL;
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
    
    // 创建RichEdit控件
    int tb_offset = GetTitleBarOffset(hParent);
    int id = g_next_control_id++;
    HWND hEdit = CreateWindowExW(
        0,
        MSFTEDIT_CLASS,  // "RICHEDIT50W" - RichEdit 4.1
        text.c_str(),
        style,
        x, y + tb_offset, width, height,
        hParent,
        (HMENU)(INT_PTR)id,
        GetModuleHandle(NULL),
        NULL
    );
    
    if (!hEdit) {
        return NULL;
    }
    
    // ⚠️ 关键修复：启用彩色emoji支持
    // 设置为纯文本模式（不需要RTF格式）
    SendMessageW(hEdit, EM_SETTEXTMODE, TM_PLAINTEXT, 0);
    
    // 启用彩色emoji渲染（Windows 10 1809+）
    // 使用EM_SETEDITSTYLE消息启用彩色emoji
    SendMessageW(hEdit, EM_SETEDITSTYLE, SES_EMULATESYSEDIT, 0);
    
    // 设置选项以支持彩色emoji
    LRESULT options = SendMessageW(hEdit, EM_GETOPTIONS, 0, 0);
    options |= ECO_NOHIDESEL;  // 失去焦点时保持选择
    SendMessageW(hEdit, EM_SETOPTIONS, ECOOP_SET, options);
    
    // 设置字体和样式
    CHARFORMAT2W cf = { 0 };
    cf.cbSize = sizeof(CHARFORMAT2W);
    cf.dwMask = CFM_FACE | CFM_SIZE | CFM_BOLD | CFM_ITALIC | CFM_UNDERLINE | CFM_COLOR;
    
    // 字体名称
    wcsncpy_s(cf.szFaceName, font_name.c_str(), LF_FACESIZE - 1);
    
    // 字体大小（单位是twips，1点 = 20 twips）
    cf.yHeight = font_size * 20;
    
    // 字体样式
    cf.dwEffects = 0;
    if (bold) cf.dwEffects |= CFE_BOLD;
    if (italic) cf.dwEffects |= CFE_ITALIC;
    if (underline) cf.dwEffects |= CFE_UNDERLINE;
    
    // 文本颜色
    cf.crTextColor = RGB(
        (fg_color >> 16) & 0xFF,
        (fg_color >> 8) & 0xFF,
        fg_color & 0xFF
    );
    
    SendMessageW(hEdit, EM_SETCHARFORMAT, SCF_ALL, (LPARAM)&cf);
    
    // 设置背景色
    SendMessageW(hEdit, EM_SETBKGNDCOLOR, 0, RGB(
        (bg_color >> 16) & 0xFF,
        (bg_color >> 8) & 0xFF,
        bg_color & 0xFF
    ));
    
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
    
    // 创建背景画刷
    state->bg_brush = CreateSolidBrush(RGB(
        (bg_color >> 16) & 0xFF,
        (bg_color >> 8) & 0xFF,
        bg_color & 0xFF
    ));
    
    g_editboxes[hEdit] = state;
    
    // 子类化以处理按键回调
    SetWindowSubclass(hEdit, EditBoxSubclassProc, 0, (DWORD_PTR)state);
    
    // 设置边距
    SendMessageW(hEdit, EM_SETMARGINS, EC_LEFTMARGIN | EC_RIGHTMARGIN, MAKELPARAM(2, 2));
    
    // 如果需要垂直居中（单行模式）
    if (vertical_center && !multiline) {
        // RichEdit的垂直居中需要通过段落格式设置
        PARAFORMAT2 pf = { 0 };
        pf.cbSize = sizeof(PARAFORMAT2);
        pf.dwMask = PFM_SPACEBEFORE | PFM_SPACEAFTER;
        
        // 计算垂直居中所需的上下间距
        RECT rect;
        GetClientRect(hEdit, &rect);
        int client_height = rect.bottom - rect.top;
        int line_height = font_size + 4;  // 估算行高
        int space = (client_height - line_height) / 2;
        if (space > 0) {
            pf.dySpaceBefore = space * 20;  // 转换为twips
        }
        
        SendMessageW(hEdit, EM_SETPARAFORMAT, 0, (LPARAM)&pf);
    }
    
    return hEdit;
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
    int tb_offset = GetTitleBarOffset(hParent);
    int id = g_next_control_id++;
    HWND hLabel = CreateWindowExW(
        0,
        L"STATIC",
        text.c_str(),
        WS_CHILD | WS_VISIBLE | SS_OWNERDRAW,
        x, y + tb_offset, width, height,
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
    state->x = x;
    state->y = y;
    state->width = width;
    state->height = height;
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

    // ✅ 创建背景画刷（解析主题颜色索引）
    UINT32 resolved_bg_init = ResolveThemeColor(bg_color);
    state->bg_brush = CreateSolidBrush(RGB(
        (resolved_bg_init >> 16) & 0xFF,
        (resolved_bg_init >> 8) & 0xFF,
        resolved_bg_init & 0xFF
    ));
    
    g_labels[hLabel] = state;
    
    // 子类化以自定义绘制
    SetWindowSubclass(hLabel, LabelSubclassProc, 0, (DWORD_PTR)state);
    
    return hLabel;
}

__declspec(dllexport) int __stdcall GetLabelText(
    HWND hLabel,
    unsigned char* buffer,
    int buffer_size
) {
    auto it = g_labels.find(hLabel);
    if (it == g_labels.end()) return -1;

    LabelState* state = it->second;
    std::string utf8_text = WindowWideToUtf8(state->text);

    // 第一次调用：返回所需长度
    if (buffer == nullptr) {
        return (int)utf8_text.size();
    }

    // 第二次调用：复制数据
    if (buffer_size < (int)utf8_text.size()) return -1;
    memcpy(buffer, utf8_text.c_str(), utf8_text.size());
    return (int)utf8_text.size();
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

    if (state->parent_drawn) InvalidateRect(state->parent, NULL, FALSE);
    else InvalidateRect(hLabel, NULL, TRUE);
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

    if (state->parent_drawn) InvalidateRect(state->parent, NULL, FALSE);
    else InvalidateRect(hLabel, NULL, TRUE);
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

    if (state->parent_drawn) InvalidateRect(state->parent, NULL, FALSE);
    else InvalidateRect(hLabel, NULL, TRUE);
}

__declspec(dllexport) void __stdcall SetLabelBounds(
    HWND hLabel,
    int x, int y, int width, int height
) {
    auto it = g_labels.find(hLabel);
    if (it == g_labels.end()) return;
    LabelState* state = it->second;
    state->x = x;
    state->y = y;
    state->width = width;
    state->height = height;
    if (state->parent_drawn) {
        InvalidateRect(state->parent, NULL, FALSE);
    } else {
        SetWindowPos(hLabel, NULL, x, y, width, height, SWP_NOZORDER);
    }
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
    auto it = g_labels.find(hLabel);
    if (it == g_labels.end()) return;
    LabelState* state = it->second;
    state->visible = show ? true : false;
    if (state->parent_drawn) {
        InvalidateRect(state->parent, NULL, FALSE);
    } else {
        ShowWindow(hLabel, show ? SW_SHOW : SW_HIDE);
    }
}

// 获取标签字体信息
__declspec(dllexport) int __stdcall GetLabelFont(
    HWND hLabel,
    unsigned char* font_name_buffer,
    int font_name_buffer_size,
    int* font_size,
    int* bold,
    int* italic,
    int* underline
) {
    auto it = g_labels.find(hLabel);
    if (it == g_labels.end()) return -1;

    LabelState* state = it->second;
    std::string utf8_name = WindowWideToUtf8(state->font.font_name);

    // 填充输出参数
    if (font_size) *font_size = state->font.font_size;
    if (bold) *bold = state->font.bold ? 1 : 0;
    if (italic) *italic = state->font.italic ? 1 : 0;
    if (underline) *underline = state->font.underline ? 1 : 0;

    // 第一次调用：返回字体名所需长度
    if (font_name_buffer == nullptr) {
        return (int)utf8_name.size();
    }

    // 第二次调用：复制字体名
    if (font_name_buffer_size < (int)utf8_name.size()) return -1;
    memcpy(font_name_buffer, utf8_name.c_str(), utf8_name.size());
    return (int)utf8_name.size();
}

// 获取标签颜色
__declspec(dllexport) int __stdcall GetLabelColor(
    HWND hLabel,
    UINT32* fg_color,
    UINT32* bg_color
) {
    auto it = g_labels.find(hLabel);
    if (it == g_labels.end()) return -1;

    LabelState* state = it->second;
    if (fg_color) *fg_color = state->fg_color;
    if (bg_color) *bg_color = state->bg_color;
    return 0;
}

// 获取标签位置和大小
__declspec(dllexport) int __stdcall GetLabelBounds(
    HWND hLabel,
    int* x,
    int* y,
    int* width,
    int* height
) {
    auto it = g_labels.find(hLabel);
    if (it == g_labels.end()) return -1;

    LabelState* state = it->second;
    if (x) *x = state->x;
    if (y) *y = state->y;
    if (width) *width = state->width;
    if (height) *height = state->height;
    return 0;
}

// 获取标签对齐方式
__declspec(dllexport) int __stdcall GetLabelAlignment(
    HWND hLabel
) {
    auto it = g_labels.find(hLabel);
    if (it == g_labels.end()) return -1;

    LabelState* state = it->second;
    return (int)state->alignment;
}

// 获取标签启用状态
__declspec(dllexport) int __stdcall GetLabelEnabled(
    HWND hLabel
) {
    if (!hLabel) return -1;
    // 对于父绘制模式的标签，通过IsWindowEnabled检查
    return IsWindowEnabled(hLabel) ? 1 : 0;
}

// 获取标签可视状态
__declspec(dllexport) int __stdcall GetLabelVisible(
    HWND hLabel
) {
    auto it = g_labels.find(hLabel);
    if (it == g_labels.end()) return -1;

    LabelState* state = it->second;
    // 优先使用state中存储的visible状态（父绘制模式下更准确）
    if (state->parent_drawn) {
        return state->visible ? 1 : 0;
    }
    return IsWindowVisible(hLabel) ? 1 : 0;
}

} // extern "C"


// ========== 复选框功能实现 ==========

// 绘制复选框（Element UI风格）
void DrawCheckBox(ID2D1HwndRenderTarget* rt, IDWriteFactory* factory, CheckBoxState* state) {
    if (!rt || !factory || !state) return;

    // Element UI 配色（使用主题颜色）
    UINT32 primary_color = ThemeColor_Primary();
    UINT32 border_color = ThemeColor_BorderBase();
    UINT32 disabled_color = ThemeColor_TextPlaceholder();
    UINT32 hover_border = ThemeColor_Primary();

    // 复选框尺寸（Element UI标准）
    int box_size = 14;
    int box_x = state->x;
    int box_y = state->y + (state->height - box_size) / 2;

    // 确定复选框颜色（解析主题颜色索引）
    UINT32 current_bg = ResolveThemeColor(state->bg_color);
    UINT32 current_border = border_color;

    if (!state->enabled) {
        current_bg = ThemeColor_BackgroundLight();
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

        UINT32 text_color = state->enabled ? ResolveThemeColor(state->fg_color) : disabled_color;
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
            text_brush,
            D2D1_DRAW_TEXT_OPTIONS_ENABLE_COLOR_FONT  // 启用彩色 Emoji
        );

        text_brush->Release();
        text_format->Release();
    }
}

// 复选框窗口过程（子类化）
LRESULT CALLBACK CheckBoxProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData) {
    CheckBoxState* state = (CheckBoxState*)dwRefData;

    // 通用事件处理
    HandleCommonEvents(hwnd, msg, wparam, lparam, state ? &state->events : nullptr);

    switch (msg) {
        case WM_NCHITTEST: {
            // 确保STATIC控件返回HTCLIENT，允许接收鼠标消息
            return HTCLIENT;
        }

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
                rt->Clear(ColorFromUInt32(ResolveThemeColor(state->bg_color)));
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
                SetCapture(hwnd);
                InvalidateRect(hwnd, nullptr, FALSE);
            }
            return 0;
        }

        case WM_LBUTTONUP: {
            if (state->enabled && state->pressed) {
                state->pressed = false;
                ReleaseCapture();

                // 检查鼠标是否仍在控件区域内
                POINT pt;
                pt.x = GET_X_LPARAM(lparam);
                pt.y = GET_Y_LPARAM(lparam);
                RECT rc;
                GetClientRect(hwnd, &rc);
                if (PtInRect(&rc, pt)) {
                    // 切换选中状态
                    state->checked = !state->checked;
                    InvalidateRect(hwnd, nullptr, FALSE);

                    // 触发回调
                    if (state->callback) {
                        state->callback(hwnd, state->checked);
                    }
                    if (state->events.on_value_changed) {
                        state->events.on_value_changed(hwnd);
                    }
                }
            } else if (state->pressed) {
                state->pressed = false;
                ReleaseCapture();
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
    UINT32 bg_color,
    const unsigned char* font_name_bytes,
    int font_name_len,
    int font_size,
    BOOL bold,
    BOOL italic,
    BOOL underline
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
    
    // 检查父窗口是否为分组框
    HWND actual_parent = hParent;
    int actual_x = x;
    int actual_y = y;
    bool is_groupbox_child = false;
    
    auto groupbox_it = g_groupboxes.find(hParent);
    if (groupbox_it != g_groupboxes.end()) {
        // 父窗口是分组框，需要转换坐标
        is_groupbox_child = true;
        GroupBoxState* gb_state = groupbox_it->second;
        actual_parent = gb_state->parent;  // 使用分组框的父窗口
        
        // 转换坐标：分组框相对坐标 -> 窗口绝对坐标
        actual_x = gb_state->x + x + 10;  // 左边距10px
        actual_y = gb_state->y + y + 25;  // 标题高度约25px
    }
    
    // 创建静态控件作为复选框容器
    int tb_offset = GetTitleBarOffset(actual_parent);
    HWND hwnd = CreateWindowExW(
        0,
        L"STATIC",
        L"",
        WS_CHILD | WS_VISIBLE | SS_NOTIFY,
        actual_x, actual_y + tb_offset, width, height,
        actual_parent,
        (HMENU)(INT_PTR)g_next_control_id++,
        GetModuleHandle(nullptr),
        nullptr
    );

    if (!hwnd) return nullptr;

    // 创建状态对象
    CheckBoxState* state = new CheckBoxState();
    state->hwnd = hwnd;
    state->parent = actual_parent;
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
    state->check_color = ThemeColor_Primary();  // 主题主色
    // 使用传入的字体参数，如果未提供则使用默认值
    if (font_name_bytes && font_name_len > 0) {
        state->font.font_name = Utf8ToWide(font_name_bytes, font_name_len);
    } else {
        state->font.font_name = L"Microsoft YaHei UI";
    }
    state->font.font_size = (font_size > 0) ? font_size : 14;
    state->font.bold = (bold != 0);
    state->font.italic = (italic != 0);
    state->font.underline = (underline != 0);
    state->callback = nullptr;
    
    // 保存到全局map
    g_checkboxes[hwnd] = state;
    
    // 如果父窗口是分组框，自动添加到分组框的子控件列表
    if (is_groupbox_child) {
        AddChildToGroup(hParent, hwnd);
    }
    
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

// 获取复选框文本（UTF-8，两次调用模式）
__declspec(dllexport) int __stdcall GetCheckBoxText(
    HWND hCheckBox,
    unsigned char* buffer,
    int buffer_size
) {
    auto it = g_checkboxes.find(hCheckBox);
    if (it == g_checkboxes.end()) return -1;

    std::string utf8_text = WindowWideToUtf8(it->second->text);

    if (buffer == nullptr) {
        return (int)utf8_text.size();
    }

    if (buffer_size < (int)utf8_text.size()) return -1;
    memcpy(buffer, utf8_text.c_str(), utf8_text.size());
    return (int)utf8_text.size();
}

// 设置复选框字体
__declspec(dllexport) void __stdcall SetCheckBoxFont(
    HWND hCheckBox,
    const unsigned char* font_name_bytes,
    int font_name_len,
    int font_size,
    int bold,
    int italic,
    int underline
) {
    auto it = g_checkboxes.find(hCheckBox);
    if (it == g_checkboxes.end()) return;

    CheckBoxState* state = it->second;
    if (font_name_bytes && font_name_len > 0) {
        state->font.font_name = Utf8ToWide(font_name_bytes, font_name_len);
    }
    if (font_size > 0) state->font.font_size = font_size;
    state->font.bold = (bold != 0);
    state->font.italic = (italic != 0);
    state->font.underline = (underline != 0);
    InvalidateRect(hCheckBox, nullptr, FALSE);
}

// 获取复选框字体信息（两次调用模式，返回字体名UTF-8字节数）
__declspec(dllexport) int __stdcall GetCheckBoxFont(
    HWND hCheckBox,
    unsigned char* font_name_buffer,
    int font_name_buffer_size,
    int* font_size,
    int* bold,
    int* italic,
    int* underline
) {
    auto it = g_checkboxes.find(hCheckBox);
    if (it == g_checkboxes.end()) return -1;

    CheckBoxState* state = it->second;
    std::string utf8_name = WindowWideToUtf8(state->font.font_name);

    if (font_size) *font_size = state->font.font_size;
    if (bold) *bold = state->font.bold ? 1 : 0;
    if (italic) *italic = state->font.italic ? 1 : 0;
    if (underline) *underline = state->font.underline ? 1 : 0;

    if (font_name_buffer == nullptr) {
        return (int)utf8_name.size();
    }

    if (font_name_buffer_size < (int)utf8_name.size()) return -1;
    memcpy(font_name_buffer, utf8_name.c_str(), utf8_name.size());
    return (int)utf8_name.size();
}

// 设置复选框颜色
__declspec(dllexport) void __stdcall SetCheckBoxColor(
    HWND hCheckBox,
    UINT32 fg_color,
    UINT32 bg_color
) {
    auto it = g_checkboxes.find(hCheckBox);
    if (it == g_checkboxes.end()) return;

    it->second->fg_color = fg_color;
    it->second->bg_color = bg_color;
    InvalidateRect(hCheckBox, nullptr, FALSE);
}

// 获取复选框颜色（返回0成功，-1失败）
__declspec(dllexport) int __stdcall GetCheckBoxColor(
    HWND hCheckBox,
    UINT32* fg_color,
    UINT32* bg_color
) {
    auto it = g_checkboxes.find(hCheckBox);
    if (it == g_checkboxes.end()) return -1;

    CheckBoxState* state = it->second;
    if (fg_color) *fg_color = state->fg_color;
    if (bg_color) *bg_color = state->bg_color;
    return 0;
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
    
    // Element UI 风格配色（解析主题颜色索引）
    UINT32 bg_color = ResolveThemeColor(state->bg_color);
    UINT32 fg_color = ResolveThemeColor(state->fg_color);
    UINT32 border_color = ResolveThemeColor(state->border_color);
    
    // 如果禁用，使用灰色
    if (!state->enabled) {
        fg_color = ThemeColor_TextPlaceholder();  // 主题禁用色
        bg_color = ThemeColor_BackgroundLight();
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
            
            brush->SetColor(ColorFromUInt32(ResolveThemeColor(state->text_color)));
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

    // 通用事件处理
    HandleCommonEvents(hwnd, msg, wparam, lparam, state ? &state->events : nullptr);

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

                UINT32 clear_color = ResolveThemeColor(state->bg_color);
                rt->Clear(D2D1::ColorF(
                    ((clear_color >> 16) & 0xFF) / 255.0f,
                    ((clear_color >> 8) & 0xFF) / 255.0f,
                    (clear_color & 0xFF) / 255.0f,
                    1.0f));

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
    int tb_offset = GetTitleBarOffset(hParent);
    HWND hProgressBar = CreateWindowExW(
        0,
        L"STATIC",
        L"",
        WS_CHILD | WS_VISIBLE | SS_OWNERDRAW,
        x, y + tb_offset, width, height,
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
    state->border_color = 9;  // 主题边框色索引 (THEME_COLOR_BORDER_BASE)
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
    if (state->events.on_value_changed && old_value != value) {
        state->events.on_value_changed(hProgressBar);
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

// 获取进度条颜色
__declspec(dllexport) int __stdcall GetProgressBarColor(
    HWND hProgressBar,
    UINT32* fg_color,
    UINT32* bg_color
) {
    auto it = g_progressbars.find(hProgressBar);
    if (it == g_progressbars.end()) return -1;

    ProgressBarState* state = it->second;
    if (fg_color) *fg_color = state->fg_color;
    if (bg_color) *bg_color = state->bg_color;
    return 0;
}

// 获取进度条位置和大小
__declspec(dllexport) int __stdcall GetProgressBarBounds(
    HWND hProgressBar,
    int* x,
    int* y,
    int* width,
    int* height
) {
    if (!hProgressBar) return -1;
    RECT rc;
    if (!GetWindowRect(hProgressBar, &rc)) return -1;

    HWND hParent = GetParent(hProgressBar);
    if (hParent) {
        POINT pt = { rc.left, rc.top };
        ScreenToClient(hParent, &pt);
        if (x) *x = pt.x;
        if (y) *y = pt.y;
    } else {
        if (x) *x = rc.left;
        if (y) *y = rc.top;
    }
    if (width) *width = rc.right - rc.left;
    if (height) *height = rc.bottom - rc.top;
    return 0;
}

// 获取进度条启用状态
__declspec(dllexport) int __stdcall GetProgressBarEnabled(
    HWND hProgressBar
) {
    if (!hProgressBar) return -1;
    return IsWindowEnabled(hProgressBar) ? 1 : 0;
}

// 获取进度条可视状态
__declspec(dllexport) int __stdcall GetProgressBarVisible(
    HWND hProgressBar
) {
    if (!hProgressBar) return -1;
    return IsWindowVisible(hProgressBar) ? 1 : 0;
}

// 获取进度条是否显示百分比文本
__declspec(dllexport) int __stdcall GetProgressBarShowText(
    HWND hProgressBar
) {
    auto it = g_progressbars.find(hProgressBar);
    if (it == g_progressbars.end()) return -1;
    return it->second->show_text ? 1 : 0;
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

    // 通用事件处理
    HandleCommonEvents(hwnd, msg, wparam, lparam, state ? &state->events : nullptr);

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
    int tb_offset = GetTitleBarOffset(hParent);
    HWND hwnd = CreateWindowExW(
        0,
        L"STATIC",
        L"",
        WS_CHILD | WS_VISIBLE | SS_NOTIFY,
        x, y + tb_offset, width, height,
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
    state->bg_color = bg_color ? bg_color : ThemeColor_BackgroundLight();  // 主题浅色背景
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

// ========== 单选按钮功能实现 ==========

// 绘制单选按钮（Element UI风格）
void DrawRadioButton(ID2D1HwndRenderTarget* rt, IDWriteFactory* factory, RadioButtonState* state) {
    if (!rt || !factory || !state) return;

    // Element UI 配色（使用主题颜色）
    UINT32 primary_color = ThemeColor_Primary();
    UINT32 border_color = ThemeColor_BorderBase();
    UINT32 disabled_color = ThemeColor_TextPlaceholder();
    UINT32 hover_border = ThemeColor_Primary();

    // 单选按钮尺寸（Element UI标准）
    int circle_size = 14;
    int circle_x = state->x + circle_size / 2;
    int circle_y = state->y + state->height / 2;

    // 确定单选按钮颜色（解析主题颜色索引）
    UINT32 current_bg = ResolveThemeColor(state->bg_color);
    UINT32 current_border = border_color;

    if (!state->enabled) {
        current_bg = ThemeColor_BackgroundLight();
        current_border = disabled_color;
    } else if (state->checked) {
        current_border = primary_color;
    } else if (state->hovered) {
        current_border = hover_border;
    }

    // 绘制单选按钮背景（圆形）
    ID2D1SolidColorBrush* bg_brush = nullptr;
    rt->CreateSolidColorBrush(ColorFromUInt32(current_bg), &bg_brush);
    
    D2D1_ELLIPSE circle = D2D1::Ellipse(
        D2D1::Point2F((FLOAT)circle_x, (FLOAT)circle_y),
        circle_size / 2.0f,
        circle_size / 2.0f
    );
    rt->FillEllipse(circle, bg_brush);
    bg_brush->Release();

    // 绘制边框
    ID2D1SolidColorBrush* border_brush = nullptr;
    rt->CreateSolidColorBrush(ColorFromUInt32(current_border), &border_brush);
    rt->DrawEllipse(circle, border_brush, 1.0f);
    border_brush->Release();

    // 如果选中，绘制内部圆点
    if (state->checked) {
        ID2D1SolidColorBrush* dot_brush = nullptr;
        rt->CreateSolidColorBrush(ColorFromUInt32(state->dot_color), &dot_brush);

        D2D1_ELLIPSE dot = D2D1::Ellipse(
            D2D1::Point2F((FLOAT)circle_x, (FLOAT)circle_y),
            circle_size / 2.0f - 3.0f,  // 内圆半径
            circle_size / 2.0f - 3.0f
        );
        rt->FillEllipse(dot, dot_brush);
        dot_brush->Release();
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

        UINT32 text_color = state->enabled ? ResolveThemeColor(state->fg_color) : disabled_color;
        ID2D1SolidColorBrush* text_brush = nullptr;
        rt->CreateSolidColorBrush(ColorFromUInt32(text_color), &text_brush);

        D2D1_RECT_F text_rect = D2D1::RectF(
            (FLOAT)(circle_x + circle_size / 2 + 8),
            (FLOAT)state->y,
            (FLOAT)(state->x + state->width),
            (FLOAT)(state->y + state->height)
        );

        rt->DrawText(
            state->text.c_str(),
            (UINT32)state->text.length(),
            text_format,
            text_rect,
            text_brush,
            D2D1_DRAW_TEXT_OPTIONS_ENABLE_COLOR_FONT  // 启用彩色 Emoji
        );

        text_brush->Release();
        text_format->Release();
    }
}

// 单选按钮窗口过程（子类化）
LRESULT CALLBACK RadioButtonProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData) {
    RadioButtonState* state = (RadioButtonState*)dwRefData;

    // 通用事件处理
    HandleCommonEvents(hwnd, msg, wparam, lparam, state ? &state->events : nullptr);

    switch (msg) {
        case WM_NCHITTEST: {
            // 确保STATIC控件返回HTCLIENT，允许接收鼠标消息
            return HTCLIENT;
        }

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
                DrawRadioButton(rt, g_dwrite_factory, state);
                rt->EndDraw();
                rt->Release();
            }

            EndPaint(hwnd, &ps);
            return 0;
        }

        case WM_LBUTTONDOWN: {
            if (state->enabled) {
                state->pressed = true;
                SetCapture(hwnd);
                InvalidateRect(hwnd, nullptr, FALSE);
            }
            return 0;
        }

        case WM_LBUTTONUP: {
            if (state->enabled && state->pressed) {
                state->pressed = false;
                ReleaseCapture();

                // 检查鼠标是否仍在控件区域内
                POINT pt;
                pt.x = GET_X_LPARAM(lparam);
                pt.y = GET_Y_LPARAM(lparam);
                RECT rc;
                GetClientRect(hwnd, &rc);
                if (PtInRect(&rc, pt)) {
                    // 如果未选中，则选中当前按钮并取消同组其他按钮
                    if (!state->checked) {
                        // 取消同组其他按钮的选中状态
                        auto group_it = g_radio_groups.find(state->group_id);
                        if (group_it != g_radio_groups.end()) {
                            for (HWND other_hwnd : group_it->second) {
                                if (other_hwnd != hwnd) {
                                    auto other_it = g_radiobuttons.find(other_hwnd);
                                    if (other_it != g_radiobuttons.end()) {
                                        other_it->second->checked = false;
                                        InvalidateRect(other_hwnd, nullptr, FALSE);
                                    }
                                }
                            }
                        }

                        // 选中当前按钮
                        state->checked = true;
                        InvalidateRect(hwnd, nullptr, FALSE);

                        // 触发回调
                        if (state->callback) {
                            state->callback(hwnd, state->group_id, TRUE);
                        }
                        if (state->events.on_value_changed) {
                            state->events.on_value_changed(hwnd);
                        }
                    }
                }
            } else if (state->pressed) {
                state->pressed = false;
                ReleaseCapture();
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
            // 从分组中移除
            auto group_it = g_radio_groups.find(state->group_id);
            if (group_it != g_radio_groups.end()) {
                auto& group = group_it->second;
                group.erase(std::remove(group.begin(), group.end(), hwnd), group.end());
                if (group.empty()) {
                    g_radio_groups.erase(group_it);
                }
            }

            // 清理资源
            RemoveWindowSubclass(hwnd, RadioButtonProc, uIdSubclass);
            g_radiobuttons.erase(hwnd);
            delete state;
            return 0;
        }
    }

    return DefSubclassProc(hwnd, msg, wparam, lparam);
}

// 创建单选按钮
HWND __stdcall CreateRadioButton(
    HWND hParent,
    int x, int y, int width, int height,
    const unsigned char* text_bytes,
    int text_len,
    int group_id,
    BOOL checked,
    UINT32 fg_color,
    UINT32 bg_color,
    const unsigned char* font_name_bytes,
    int font_name_len,
    int font_size,
    BOOL bold,
    BOOL italic,
    BOOL underline
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
    
    // 检查父窗口是否为分组框
    HWND actual_parent = hParent;
    int actual_x = x;
    int actual_y = y;
    bool is_groupbox_child = false;
    
    auto groupbox_it = g_groupboxes.find(hParent);
    if (groupbox_it != g_groupboxes.end()) {
        // 父窗口是分组框，需要转换坐标
        is_groupbox_child = true;
        GroupBoxState* gb_state = groupbox_it->second;
        actual_parent = gb_state->parent;  // 使用分组框的父窗口
        
        // 转换坐标：分组框相对坐标 -> 窗口绝对坐标
        // 分组框标题高度约为20px，内容区域从标题下方开始
        actual_x = gb_state->x + x + 10;  // 左边距10px
        actual_y = gb_state->y + y + 25;  // 标题高度约25px
    }
    
    // 创建静态控件作为单选按钮容器
    int tb_offset = GetTitleBarOffset(actual_parent);
    HWND hwnd = CreateWindowExW(
        0,
        L"STATIC",
        L"",
        WS_CHILD | WS_VISIBLE | SS_NOTIFY,
        actual_x, actual_y + tb_offset, width, height,
        actual_parent,
        (HMENU)(INT_PTR)g_next_control_id++,
        GetModuleHandle(nullptr),
        nullptr
    );
    
    if (!hwnd) return nullptr;
    
    // 创建状态对象
    RadioButtonState* state = new RadioButtonState();
    state->hwnd = hwnd;
    state->parent = actual_parent;
    state->id = (int)(INT_PTR)GetMenu(hwnd);
    state->group_id = group_id;
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
    state->dot_color = ThemeColor_Primary();  // 主题主色
    // 使用传入的字体参数，如果未提供则使用默认值
    if (font_name_bytes && font_name_len > 0) {
        state->font.font_name = Utf8ToWide(font_name_bytes, font_name_len);
    } else {
        state->font.font_name = L"Microsoft YaHei UI";
    }
    state->font.font_size = (font_size > 0) ? font_size : 14;
    state->font.bold = (bold != 0);
    state->font.italic = (italic != 0);
    state->font.underline = (underline != 0);
    state->callback = nullptr;
    
    // 保存到全局map
    g_radiobuttons[hwnd] = state;
    
    // 添加到分组
    g_radio_groups[group_id].push_back(hwnd);
    
    // 如果父窗口是分组框，自动添加到分组框的子控件列表
    if (is_groupbox_child) {
        AddChildToGroup(hParent, hwnd);
    }
    
    // 如果设置为选中，取消同组其他按钮
    if (state->checked) {
        for (HWND other_hwnd : g_radio_groups[group_id]) {
            if (other_hwnd != hwnd) {
                auto other_it = g_radiobuttons.find(other_hwnd);
                if (other_it != g_radiobuttons.end()) {
                    other_it->second->checked = false;
                }
            }
        }
    }
    
    // 子类化窗口
    SetWindowSubclass(hwnd, RadioButtonProc, 0, (DWORD_PTR)state);
    
    return hwnd;
}

// 获取单选按钮选中状态
BOOL __stdcall GetRadioButtonState(HWND hRadioButton) {
    auto it = g_radiobuttons.find(hRadioButton);
    if (it == g_radiobuttons.end()) return FALSE;
    return it->second->checked ? TRUE : FALSE;
}

// 设置单选按钮选中状态
void __stdcall SetRadioButtonState(HWND hRadioButton, BOOL checked) {
    auto it = g_radiobuttons.find(hRadioButton);
    if (it == g_radiobuttons.end()) return;
    
    if (checked) {
        // 如果设置为选中，取消同组其他按钮
        RadioButtonState* state = it->second;
        auto group_it = g_radio_groups.find(state->group_id);
        if (group_it != g_radio_groups.end()) {
            for (HWND other_hwnd : group_it->second) {
                if (other_hwnd != hRadioButton) {
                    auto other_it = g_radiobuttons.find(other_hwnd);
                    if (other_it != g_radiobuttons.end()) {
                        other_it->second->checked = false;
                        InvalidateRect(other_hwnd, nullptr, FALSE);
                    }
                }
            }
        }
    }
    
    it->second->checked = (checked != 0);
    InvalidateRect(hRadioButton, nullptr, FALSE);
}

// 设置单选按钮回调
void __stdcall SetRadioButtonCallback(HWND hRadioButton, RadioButtonCallback callback) {
    auto it = g_radiobuttons.find(hRadioButton);
    if (it == g_radiobuttons.end()) return;
    
    it->second->callback = callback;
}

// 启用/禁用单选按钮
void __stdcall EnableRadioButton(HWND hRadioButton, BOOL enable) {
    auto it = g_radiobuttons.find(hRadioButton);
    if (it == g_radiobuttons.end()) return;
    
    it->second->enabled = (enable != 0);
    InvalidateRect(hRadioButton, nullptr, FALSE);
}

// 显示/隐藏单选按钮
void __stdcall ShowRadioButton(HWND hRadioButton, BOOL show) {
    if (!hRadioButton) return;
    ShowWindow(hRadioButton, show ? SW_SHOW : SW_HIDE);
}

// 设置单选按钮文本
void __stdcall SetRadioButtonText(HWND hRadioButton, const unsigned char* text_bytes, int text_len) {
    auto it = g_radiobuttons.find(hRadioButton);
    if (it == g_radiobuttons.end()) return;
    
    it->second->text = Utf8ToWide(text_bytes, text_len);
    InvalidateRect(hRadioButton, nullptr, FALSE);
}

// 设置单选按钮位置和大小
void __stdcall SetRadioButtonBounds(HWND hRadioButton, int x, int y, int width, int height) {
    auto it = g_radiobuttons.find(hRadioButton);
    if (it == g_radiobuttons.end()) return;
    
    it->second->width = width;
    it->second->height = height;
    SetWindowPos(hRadioButton, nullptr, x, y, width, height, SWP_NOZORDER);
    InvalidateRect(hRadioButton, nullptr, FALSE);
}

// 获取单选按钮文本（UTF-8，两次调用模式）
__declspec(dllexport) int __stdcall GetRadioButtonText(
    HWND hRadioButton,
    unsigned char* buffer,
    int buffer_size
) {
    auto it = g_radiobuttons.find(hRadioButton);
    if (it == g_radiobuttons.end()) return -1;

    std::string utf8_text = WindowWideToUtf8(it->second->text);

    if (buffer == nullptr) {
        return (int)utf8_text.size();
    }

    if (buffer_size < (int)utf8_text.size()) return -1;
    memcpy(buffer, utf8_text.c_str(), utf8_text.size());
    return (int)utf8_text.size();
}

// 设置单选按钮字体
__declspec(dllexport) void __stdcall SetRadioButtonFont(
    HWND hRadioButton,
    const unsigned char* font_name_bytes,
    int font_name_len,
    int font_size,
    int bold,
    int italic,
    int underline
) {
    auto it = g_radiobuttons.find(hRadioButton);
    if (it == g_radiobuttons.end()) return;

    RadioButtonState* state = it->second;
    if (font_name_bytes && font_name_len > 0) {
        state->font.font_name = Utf8ToWide(font_name_bytes, font_name_len);
    }
    if (font_size > 0) state->font.font_size = font_size;
    state->font.bold = (bold != 0);
    state->font.italic = (italic != 0);
    state->font.underline = (underline != 0);
    InvalidateRect(hRadioButton, nullptr, FALSE);
}

// 获取单选按钮字体信息（两次调用模式，返回字体名UTF-8字节数）
__declspec(dllexport) int __stdcall GetRadioButtonFont(
    HWND hRadioButton,
    unsigned char* font_name_buffer,
    int font_name_buffer_size,
    int* font_size,
    int* bold,
    int* italic,
    int* underline
) {
    auto it = g_radiobuttons.find(hRadioButton);
    if (it == g_radiobuttons.end()) return -1;

    RadioButtonState* state = it->second;
    std::string utf8_name = WindowWideToUtf8(state->font.font_name);

    if (font_size) *font_size = state->font.font_size;
    if (bold) *bold = state->font.bold ? 1 : 0;
    if (italic) *italic = state->font.italic ? 1 : 0;
    if (underline) *underline = state->font.underline ? 1 : 0;

    if (font_name_buffer == nullptr) {
        return (int)utf8_name.size();
    }

    if (font_name_buffer_size < (int)utf8_name.size()) return -1;
    memcpy(font_name_buffer, utf8_name.c_str(), utf8_name.size());
    return (int)utf8_name.size();
}

// 设置单选按钮颜色
__declspec(dllexport) void __stdcall SetRadioButtonColor(
    HWND hRadioButton,
    UINT32 fg_color,
    UINT32 bg_color
) {
    auto it = g_radiobuttons.find(hRadioButton);
    if (it == g_radiobuttons.end()) return;

    it->second->fg_color = fg_color;
    it->second->bg_color = bg_color;
    InvalidateRect(hRadioButton, nullptr, FALSE);
}

// 获取单选按钮颜色（返回0成功，-1失败）
__declspec(dllexport) int __stdcall GetRadioButtonColor(
    HWND hRadioButton,
    UINT32* fg_color,
    UINT32* bg_color
) {
    auto it = g_radiobuttons.find(hRadioButton);
    if (it == g_radiobuttons.end()) return -1;

    RadioButtonState* state = it->second;
    if (fg_color) *fg_color = state->fg_color;
    if (bg_color) *bg_color = state->bg_color;
    return 0;
}

// ========== 列表框功能实现 ==========

// 绘制列表框 (Element UI风格)
void DrawListBox(ID2D1HwndRenderTarget* rt, IDWriteFactory* factory, ListBoxState* state) {
    if (!rt || !factory || !state) return;

    D2D1_SIZE_F size = rt->GetSize();

    // Element UI颜色（解析主题颜色索引）
    D2D1_COLOR_F bg_color = ColorFromUInt32(ResolveThemeColor(state->bg_color));
    D2D1_COLOR_F fg_color = ColorFromUInt32(ResolveThemeColor(state->fg_color));
    D2D1_COLOR_F select_color = ColorFromUInt32(ThemeColor_LightBg(ThemeColor_Primary()));
    D2D1_COLOR_F hover_color = ColorFromUInt32(ThemeColor_BackgroundLight());
    D2D1_COLOR_F border_color = ColorFromUInt32(ThemeColor_BorderBase());
    
    // 如果禁用，使用主题颜色
    if (!state->enabled) {
        bg_color = ColorFromUInt32(ThemeColor_BackgroundLight());
        fg_color = ColorFromUInt32(ThemeColor_TextPlaceholder());
    }
    
    // 创建画刷
    ID2D1SolidColorBrush* brush = nullptr;
    rt->CreateSolidColorBrush(bg_color, &brush);
    if (!brush) return;
    
    // 绘制背景
    D2D1_RECT_F rect = D2D1::RectF(0, 0, size.width, size.height);
    brush->SetColor(bg_color);
    rt->FillRectangle(rect, brush);
    
    // 绘制边框 (1px, Element UI风格)
    brush->SetColor(border_color);
    rt->DrawRectangle(rect, brush, 1.0f);
    
    // 创建文本格式（使用Segoe UI Emoji支持彩色Emoji）
    IDWriteTextFormat* text_format = nullptr;
    HRESULT hr = factory->CreateTextFormat(
        L"Segoe UI Emoji",  // 使用支持彩色Emoji的字体
        nullptr,
        DWRITE_FONT_WEIGHT_NORMAL,
        DWRITE_FONT_STYLE_NORMAL,
        DWRITE_FONT_STRETCH_NORMAL,
        (float)state->font.font_size,
        L"zh-CN",
        &text_format
    );
    
    if (FAILED(hr) || !text_format) {
        brush->Release();
        return;
    }
    
    text_format->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
    text_format->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
    
    // 创建备用文本格式（用于中文等非Emoji字符）
    IDWriteTextFormat* fallback_format = nullptr;
    factory->CreateTextFormat(
        state->font.font_name.c_str(),
        nullptr,
        state->font.bold ? DWRITE_FONT_WEIGHT_BOLD : DWRITE_FONT_WEIGHT_NORMAL,
        state->font.italic ? DWRITE_FONT_STYLE_ITALIC : DWRITE_FONT_STYLE_NORMAL,
        DWRITE_FONT_STRETCH_NORMAL,
        (float)state->font.font_size,
        L"zh-CN",
        &fallback_format
    );
    
    // 计算可见区域
    int visible_start = state->scroll_offset / state->item_height;
    int visible_count = (int)(size.height / state->item_height) + 2;
    int visible_end = min(visible_start + visible_count, (int)state->items.size());
    
    // 绘制可见的列表项
    for (int i = visible_start; i < visible_end; i++) {
        float y = (float)(i * state->item_height - state->scroll_offset);
        
        // 跳过不在可见区域的项目
        if (y + state->item_height < 0 || y > size.height) continue;
        
        D2D1_RECT_F item_rect = D2D1::RectF(
            1,
            y,
            size.width - 1,
            y + state->item_height
        );
        
        // 绘制选中背景
        bool is_selected = (i == state->selected_index);
        if (state->multi_select) {
            is_selected = std::find(state->selected_indices.begin(), 
                                   state->selected_indices.end(), i) != state->selected_indices.end();
        }
        
        if (is_selected) {
            brush->SetColor(select_color);
            rt->FillRectangle(item_rect, brush);
        }
        // 绘制悬停背景
        else if (i == state->hovered_index && state->enabled) {
            brush->SetColor(hover_color);
            rt->FillRectangle(item_rect, brush);
        }
        
        // 绘制文本（使用TextLayout支持彩色Emoji）
        D2D1_RECT_F text_rect = D2D1::RectF(
            item_rect.left + 12,  // 左边距12px
            item_rect.top,
            item_rect.right - 12,  // 右边距12px
            item_rect.bottom
        );
        
        // 创建TextLayout以支持彩色Emoji
        IDWriteTextLayout* text_layout = nullptr;
        hr = factory->CreateTextLayout(
            state->items[i].text.c_str(),
            (UINT32)state->items[i].text.length(),
            text_format,
            text_rect.right - text_rect.left,
            text_rect.bottom - text_rect.top,
            &text_layout
        );
        
        if (text_layout) {
            brush->SetColor(fg_color);
            
            // 使用DrawTextLayout而不是DrawTextW，这样可以支持彩色Emoji
            rt->DrawTextLayout(
                D2D1::Point2F(text_rect.left, text_rect.top),
                text_layout,
                brush,
                D2D1_DRAW_TEXT_OPTIONS_ENABLE_COLOR_FONT  // 启用彩色字体！
            );
            
            text_layout->Release();
        }
    }
    
    if (fallback_format) fallback_format->Release();
    
    // ===== 绘制滚动条 =====
    int total_content_height = (int)state->items.size() * state->item_height;
    int visible_height = (int)size.height;
    if (total_content_height > visible_height) {
        float scrollbar_width = 8.0f;
        float scrollbar_x = size.width - scrollbar_width - 2.0f;
        float track_height = size.height - 4.0f;
        float track_y = 2.0f;
        
        // 滚动条背景轨道
        ID2D1SolidColorBrush* sb_brush = nullptr;
        rt->CreateSolidColorBrush(D2D1::ColorF(0.0f, 0.0f, 0.0f, 0.05f), &sb_brush);
        if (sb_brush) {
            D2D1_ROUNDED_RECT track_rect = D2D1::RoundedRect(
                D2D1::RectF(scrollbar_x, track_y, scrollbar_x + scrollbar_width, track_y + track_height),
                scrollbar_width / 2, scrollbar_width / 2
            );
            rt->FillRoundedRectangle(track_rect, sb_brush);
            
            // 滚动条滑块
            float thumb_ratio = (float)visible_height / (float)total_content_height;
            float thumb_height = (std::max)(20.0f, track_height * thumb_ratio);
            int max_scroll = total_content_height - visible_height;
            float scroll_ratio = (max_scroll > 0) ? (float)state->scroll_offset / (float)max_scroll : 0.0f;
            float thumb_y = track_y + scroll_ratio * (track_height - thumb_height);
            
            sb_brush->SetColor(D2D1::ColorF(0.0f, 0.0f, 0.0f, 0.2f));
            D2D1_ROUNDED_RECT thumb_rect = D2D1::RoundedRect(
                D2D1::RectF(scrollbar_x + 1.0f, thumb_y, scrollbar_x + scrollbar_width - 1.0f, thumb_y + thumb_height),
                (scrollbar_width - 2.0f) / 2, (scrollbar_width - 2.0f) / 2
            );
            rt->FillRoundedRectangle(thumb_rect, sb_brush);
            
            sb_brush->Release();
        }
    }
    
    text_format->Release();
    brush->Release();
}

// 列表框窗口过程
LRESULT CALLBACK ListBoxProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam, 
                             UINT_PTR uIdSubclass, DWORD_PTR dwRefData) {
    auto it = g_listboxes.find(hwnd);
    if (it == g_listboxes.end()) {
        return DefSubclassProc(hwnd, msg, wparam, lparam);
    }
    
    ListBoxState* state = it->second;

    // 通用事件处理
    HandleCommonEvents(hwnd, msg, wparam, lparam, &state->events);

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
            
            if (g_d2d_factory) {
                g_d2d_factory->CreateHwndRenderTarget(props, hwnd_props, &rt);
            }
            
            if (rt && g_dwrite_factory) {
                rt->BeginDraw();
                rt->Clear(ColorFromUInt32(ResolveThemeColor(state->bg_color)));
                DrawListBox(rt, g_dwrite_factory, state);
                rt->EndDraw();
                rt->Release();
            }
            
            EndPaint(hwnd, &ps);
            return 0;
        }
        
        case WM_LBUTTONDOWN: {
            if (!state->enabled) break;
            
            int x = LOWORD(lparam);
            int y = HIWORD(lparam);
            
            // 计算点击的项目索引
            int clicked_index = (y + state->scroll_offset) / state->item_height;
            
            if (clicked_index >= 0 && clicked_index < (int)state->items.size()) {
                if (state->multi_select) {
                    // 多选模式：切换选中状态
                    auto it_sel = std::find(state->selected_indices.begin(), 
                                           state->selected_indices.end(), clicked_index);
                    if (it_sel != state->selected_indices.end()) {
                        state->selected_indices.erase(it_sel);
                    } else {
                        state->selected_indices.push_back(clicked_index);
                    }
                } else {
                    // 单选模式
                    state->selected_index = clicked_index;
                }
                
                // 触发回调
                if (state->callback) {
                    state->callback(hwnd, clicked_index);
                }
                if (state->events.on_value_changed) {
                    state->events.on_value_changed(hwnd);
                }
                
                InvalidateRect(hwnd, nullptr, TRUE);
            }
            return 0;
        }
        
        case WM_MOUSEMOVE: {
            if (!state->enabled) break;
            
            int y = HIWORD(lparam);
            int hovered_index = (y + state->scroll_offset) / state->item_height;
            
            if (hovered_index >= 0 && hovered_index < (int)state->items.size()) {
                if (hovered_index != state->hovered_index) {
                    state->hovered_index = hovered_index;
                    InvalidateRect(hwnd, nullptr, TRUE);
                }
            }
            
            // 追踪鼠标离开事件
            TRACKMOUSEEVENT tme = { sizeof(TRACKMOUSEEVENT) };
            tme.dwFlags = TME_LEAVE;
            tme.hwndTrack = hwnd;
            TrackMouseEvent(&tme);
            return 0;
        }
        
        case WM_MOUSELEAVE: {
            if (state->hovered_index != -1) {
                state->hovered_index = -1;
                InvalidateRect(hwnd, nullptr, TRUE);
            }
            return 0;
        }
        
        case WM_MOUSEWHEEL: {
            if (!state->enabled) break;
            
            int delta = GET_WHEEL_DELTA_WPARAM(wparam);
            int scroll_amount = -delta / 3;  // 滚动量
            
            // 更新滚动偏移
            int max_scroll = max(0, (int)state->items.size() * state->item_height - state->height);
            state->scroll_offset = max(0, min(max_scroll, state->scroll_offset + scroll_amount));
            
            InvalidateRect(hwnd, nullptr, TRUE);
            return 0;
        }
        
        case WM_SIZE: {
            state->width = LOWORD(lparam);
            state->height = HIWORD(lparam);
            InvalidateRect(hwnd, nullptr, TRUE);
            return 0;
        }
        
        case WM_NCDESTROY: {
            // 清理资源
            RemoveWindowSubclass(hwnd, ListBoxProc, uIdSubclass);
            g_listboxes.erase(hwnd);
            delete state;
            return 0;
        }
    }
    
    return DefSubclassProc(hwnd, msg, wparam, lparam);
}

// 创建列表框
HWND __stdcall CreateListBox(
    HWND hParent,
    int x, int y, int width, int height,
    BOOL multi_select,
    UINT32 fg_color,
    UINT32 bg_color
) {
    if (!hParent || !IsWindow(hParent)) return nullptr;

    // 创建窗口
    int tb_offset = GetTitleBarOffset(hParent);
    HWND hwnd = CreateWindowExW(
        0,
        L"STATIC",
        L"",
        WS_CHILD | WS_VISIBLE | SS_NOTIFY,
        x, y + tb_offset, width, height,
        hParent,
        (HMENU)(UINT_PTR)(g_next_control_id++),
        GetModuleHandle(nullptr),
        nullptr
    );

    if (!hwnd) return nullptr;

    // 创建状态
    ListBoxState* state = new ListBoxState();
    state->hwnd = hwnd;
    state->parent = hParent;
    state->id = (int)(UINT_PTR)GetMenu(hwnd);
    state->x = x;
    state->y = y;
    state->width = width;
    state->height = height;
    state->selected_index = -1;
    state->hovered_index = -1;
    state->scroll_offset = 0;
    state->item_height = 32;  // Element UI标准行高
    state->multi_select = (multi_select != 0);
    state->enabled = true;
    state->fg_color = fg_color;
    state->bg_color = bg_color;
    state->select_color = ThemeColor_LightBg(ThemeColor_Primary());  // 主题选中背景色
    state->hover_color = ThemeColor_BackgroundLight();   // 主题悬停背景色
    state->callback = nullptr;
    
    // 默认字体
    state->font.font_name = L"Microsoft YaHei UI";
    state->font.font_size = 14;
    state->font.bold = false;
    state->font.italic = false;
    state->font.underline = false;
    
    // 保存状态
    g_listboxes[hwnd] = state;
    
    // 子类化窗口
    SetWindowSubclass(hwnd, ListBoxProc, 0, (DWORD_PTR)state);
    
    return hwnd;
}

// 添加列表项
int __stdcall AddListItem(
    HWND hListBox,
    const unsigned char* text_bytes,
    int text_len
) {
    auto it = g_listboxes.find(hListBox);
    if (it == g_listboxes.end()) return -1;
    
    ListBoxState* state = it->second;
    
    // 创建新项目
    ListBoxItem item;
    item.text = Utf8ToWide(text_bytes, text_len);
    item.id = (int)state->items.size();
    item.user_data = nullptr;
    
    state->items.push_back(item);
    
    InvalidateRect(hListBox, nullptr, TRUE);
    
    return item.id;
}

// 移除列表项
void __stdcall RemoveListItem(
    HWND hListBox,
    int index
) {
    auto it = g_listboxes.find(hListBox);
    if (it == g_listboxes.end()) return;
    
    ListBoxState* state = it->second;
    
    if (index < 0 || index >= (int)state->items.size()) return;
    
    state->items.erase(state->items.begin() + index);
    
    // 更新选中索引
    if (state->selected_index == index) {
        state->selected_index = -1;
    } else if (state->selected_index > index) {
        state->selected_index--;
    }
    
    // 更新多选索引
    if (state->multi_select) {
        auto it_sel = std::find(state->selected_indices.begin(), 
                               state->selected_indices.end(), index);
        if (it_sel != state->selected_indices.end()) {
            state->selected_indices.erase(it_sel);
        }
        
        // 调整其他选中项的索引
        for (int& sel_idx : state->selected_indices) {
            if (sel_idx > index) {
                sel_idx--;
            }
        }
    }
    
    InvalidateRect(hListBox, nullptr, TRUE);
}

// 清空列表框
void __stdcall ClearListBox(
    HWND hListBox
) {
    auto it = g_listboxes.find(hListBox);
    if (it == g_listboxes.end()) return;
    
    ListBoxState* state = it->second;
    
    state->items.clear();
    state->selected_index = -1;
    state->hovered_index = -1;
    state->selected_indices.clear();
    state->scroll_offset = 0;
    
    InvalidateRect(hListBox, nullptr, TRUE);
}

// 获取选中项索引
int __stdcall GetSelectedIndex(
    HWND hListBox
) {
    auto it = g_listboxes.find(hListBox);
    if (it == g_listboxes.end()) return -1;
    
    return it->second->selected_index;
}

// 设置选中项索引
void __stdcall SetSelectedIndex(
    HWND hListBox,
    int index
) {
    auto it = g_listboxes.find(hListBox);
    if (it == g_listboxes.end()) return;
    
    ListBoxState* state = it->second;
    
    if (index < -1 || index >= (int)state->items.size()) return;
    
    state->selected_index = index;
    
    InvalidateRect(hListBox, nullptr, TRUE);
}

// 获取列表项数量
int __stdcall GetListItemCount(
    HWND hListBox
) {
    auto it = g_listboxes.find(hListBox);
    if (it == g_listboxes.end()) return 0;
    
    return (int)it->second->items.size();
}

// 获取列表项文本
int __stdcall GetListItemText(
    HWND hListBox,
    int index,
    unsigned char* buffer,
    int buffer_size
) {
    auto it = g_listboxes.find(hListBox);
    if (it == g_listboxes.end()) return 0;
    
    ListBoxState* state = it->second;
    
    if (index < 0 || index >= (int)state->items.size()) return 0;
    
    // 转换为UTF-8
    std::wstring text = state->items[index].text;
    int utf8_len = WideCharToMultiByte(CP_UTF8, 0, text.c_str(), -1, nullptr, 0, nullptr, nullptr);
    
    if (buffer && buffer_size > 0) {
        int copied = WideCharToMultiByte(CP_UTF8, 0, text.c_str(), -1, 
                                        (LPSTR)buffer, buffer_size, nullptr, nullptr);
        return copied - 1;  // 不包括null终止符
    }
    
    return utf8_len - 1;  // 不包括null终止符
}

// 设置列表框回调
void __stdcall SetListBoxCallback(
    HWND hListBox,
    ListBoxCallback callback
) {
    auto it = g_listboxes.find(hListBox);
    if (it == g_listboxes.end()) return;
    
    it->second->callback = callback;
}

// 启用/禁用列表框
void __stdcall EnableListBox(
    HWND hListBox,
    BOOL enable
) {
    auto it = g_listboxes.find(hListBox);
    if (it == g_listboxes.end()) return;
    
    it->second->enabled = (enable != 0);
    // 不调用 EnableWindow，避免 Windows 原生 STATIC 控件擦除背景导致闪烁/消失
    // 仅通过内部 enabled 状态控制交互行为
    RedrawWindow(hListBox, nullptr, nullptr, RDW_INVALIDATE | RDW_UPDATENOW);
}

// 显示/隐藏列表框
void __stdcall ShowListBox(
    HWND hListBox,
    BOOL show
) {
    if (!hListBox || !IsWindow(hListBox)) return;
    
    ShowWindow(hListBox, show ? SW_SHOW : SW_HIDE);
}

// 设置列表框位置和大小
void __stdcall SetListBoxBounds(
    HWND hListBox,
    int x, int y, int width, int height
) {
    auto it = g_listboxes.find(hListBox);
    if (it == g_listboxes.end()) return;
    
    ListBoxState* state = it->second;
    state->x = x;
    state->y = y;
    state->width = width;
    state->height = height;
    
    SetWindowPos(hListBox, nullptr, x, y, width, height, 
                SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOREDRAW);
    InvalidateRect(hListBox, nullptr, TRUE);
}

// 设置列表项文本
BOOL __stdcall SetListItemText(
    HWND hListBox,
    int index,
    const unsigned char* text_bytes,
    int text_len
) {
    auto it = g_listboxes.find(hListBox);
    if (it == g_listboxes.end()) return FALSE;
    
    ListBoxState* state = it->second;
    if (index < 0 || index >= (int)state->items.size()) return FALSE;
    if (!text_bytes || text_len <= 0) return FALSE;
    
    // UTF-8 转 wstring
    int wlen = MultiByteToWideChar(CP_UTF8, 0, (const char*)text_bytes, text_len, nullptr, 0);
    if (wlen <= 0) return FALSE;
    
    std::wstring wtext(wlen, L'\0');
    MultiByteToWideChar(CP_UTF8, 0, (const char*)text_bytes, text_len, &wtext[0], wlen);
    
    state->items[index].text = wtext;
    InvalidateRect(hListBox, nullptr, TRUE);
    return TRUE;
}

// ========== 组合框控件实�?==========

// 绘制组合�?
void DrawComboBox(ID2D1HwndRenderTarget* rt, IDWriteFactory* factory, ComboBoxState* state) {
    if (!rt || !factory || !state) return;
    
    rt->BeginDraw();
    rt->Clear(ColorFromUInt32(state->bg_color));
    
    // 绘制下拉按钮区域（右侧）
    int button_width = 30;
    D2D1_RECT_F button_rect = D2D1::RectF(
        (float)(state->width - button_width),
        0.0f,
        (float)state->width,
        (float)state->height
    );
    
    // 按钮背景�?
    UINT32 button_color = state->bg_color;
    if (!state->enabled) {
        button_color = ThemeColor_BackgroundLight();  // 禁用状态
    } else if (state->button_pressed) {
        button_color = ThemeColor_LightBg(ThemeColor_Primary());  // 按下状态
    } else if (state->button_hovered) {
        button_color = ThemeColor_BackgroundLight();  // 悬停状态
    }
    
    ID2D1SolidColorBrush* brush = nullptr;
    rt->CreateSolidColorBrush(ColorFromUInt32(button_color), &brush);
    if (brush) {
        D2D1_ROUNDED_RECT btn_rr = D2D1::RoundedRect(button_rect, 4.0f, 4.0f);
        rt->FillRoundedRectangle(btn_rr, brush);
        brush->Release();
    }
    
    // 绘制下拉箭头
    rt->CreateSolidColorBrush(ColorFromUInt32(0xFF909399), &brush);
    if (brush) {
        float arrow_x = state->width - button_width / 2.0f;
        float arrow_y = state->height / 2.0f;
        float arrow_size = 4.0f;
        
        // 创建箭头路径
        ID2D1PathGeometry* arrow_path = nullptr;
        g_d2d_factory->CreatePathGeometry(&arrow_path);
        if (arrow_path) {
            ID2D1GeometrySink* sink = nullptr;
            arrow_path->Open(&sink);
            if (sink) {
                sink->BeginFigure(D2D1::Point2F(arrow_x - arrow_size, arrow_y - arrow_size / 2), D2D1_FIGURE_BEGIN_FILLED);
                sink->AddLine(D2D1::Point2F(arrow_x + arrow_size, arrow_y - arrow_size / 2));
                sink->AddLine(D2D1::Point2F(arrow_x, arrow_y + arrow_size / 2));
                sink->EndFigure(D2D1_FIGURE_END_CLOSED);
                sink->Close();
                sink->Release();
            }
            rt->FillGeometry(arrow_path, brush);
            arrow_path->Release();
        }
        brush->Release();
    }
    
    // 绘制边框（圆角 4px，与设计器一致）
    rt->CreateSolidColorBrush(ColorFromUInt32(ThemeColor_BorderBase()), &brush);
    if (brush) {
        D2D1_RECT_F border_rect = D2D1::RectF(0.5f, 0.5f, (float)state->width - 0.5f, (float)state->height - 0.5f);
        D2D1_ROUNDED_RECT rr = D2D1::RoundedRect(border_rect, 4.0f, 4.0f);
        rt->DrawRoundedRectangle(rr, brush, 1.0f);
        brush->Release();
    }
    
    rt->EndDraw();
}

// 绘制组合框下拉列�?
void DrawComboDropDown(ID2D1HwndRenderTarget* rt, IDWriteFactory* factory, ComboBoxState* state) {
    if (!rt || !factory || !state) {
        OutputDebugStringW(L"DrawComboDropDown: 参数检查失败");
        return;
    }
    
    OutputDebugStringW((L"DrawComboDropDown: 开始绘制，项目总数: " + std::to_wstring(state->items.size()) + L", 滚动偏移: " + std::to_wstring(state->scroll_offset)).c_str());
    
    rt->BeginDraw();
    rt->Clear(ColorFromUInt32(0xFFFFFFFF));
    
    int item_height = state->item_height;  // 使用状态中的表项高度
    
    // 获取下拉列表窗口的客户区高度
    RECT client_rect;
    GetClientRect(state->dropdown_hwnd, &client_rect);
    int visible_height = client_rect.bottom;
    
    // 计算可见的起始和结束项目索引
    int start_index = state->scroll_offset / item_height;
    int end_index = min((int)state->items.size(), (state->scroll_offset + visible_height) / item_height + 1);
    
    OutputDebugStringW((L"DrawComboDropDown: 可见范围 " + std::to_wstring(start_index) + L" 到 " + std::to_wstring(end_index)).c_str());
    
    // 绘制每个可见项目
    for (int i = start_index; i < end_index; i++) {
        OutputDebugStringW((L"DrawComboDropDown: 绘制项目 " + std::to_wstring(i) + L": " + state->items[i]).c_str());
        
        // 计算项目的Y坐标（考虑滚动偏移）
        int y_offset = i * item_height - state->scroll_offset;
        D2D1_RECT_F item_rect = D2D1::RectF(
            0.0f,
            (float)y_offset,
            (float)state->width,
            (float)(y_offset + item_height)
        );
        
        // 背景�?
        UINT32 bg_color = 0xFFFFFFFF;
        if ((int)i == state->selected_index) {
            bg_color = state->select_color;
        } else if ((int)i == state->hovered_index) {
            bg_color = state->hover_color;
        }
        
        ID2D1SolidColorBrush* brush = nullptr;
        rt->CreateSolidColorBrush(ColorFromUInt32(bg_color), &brush);
        if (brush) {
            rt->FillRectangle(item_rect, brush);
            brush->Release();
        }
        
        // 绘制文本 - 使用TextLayout支持彩色Emoji
        rt->CreateSolidColorBrush(ColorFromUInt32(state->fg_color), &brush);
        if (brush) {
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
                text_format->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
                text_format->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
                
                // 创建TextLayout以支持彩色Emoji
                IDWriteTextLayout* text_layout = nullptr;
                factory->CreateTextLayout(
                    state->items[i].c_str(),
                    (UINT32)state->items[i].length(),
                    text_format,
                    (float)(state->width - 20),  // 最大宽度
                    (float)item_height,           // 最大高度
                    &text_layout
                );
                
                if (text_layout) {
                    // 使用DrawTextLayout而不是DrawTextW，这样可以支持彩色Emoji
                    rt->DrawTextLayout(
                        D2D1::Point2F(10.0f, (float)y_offset),
                        text_layout,
                        brush,
                        D2D1_DRAW_TEXT_OPTIONS_ENABLE_COLOR_FONT  // 关键：启用彩色字体
                    );
                    
                    text_layout->Release();
                }
                
                text_format->Release();
            }
            brush->Release();
        }
        
        y_offset += item_height;
    }
    
    // 绘制边框
    ID2D1SolidColorBrush* brush = nullptr;
    rt->CreateSolidColorBrush(ColorFromUInt32(0xFFDCDFE6), &brush);
    if (brush) {
        RECT client_rect;
        GetClientRect(state->dropdown_hwnd, &client_rect);
        D2D1_RECT_F border_rect = D2D1::RectF(
            0.5f, 0.5f,
            (float)(client_rect.right - 0.5f),
            (float)(client_rect.bottom - 0.5f)
        );
        rt->DrawRectangle(border_rect, brush, 1.0f);
        brush->Release();
    }
    
    rt->EndDraw();
}

// 组合框下拉列表窗口过�?
LRESULT CALLBACK ComboDropDownProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
    ComboBoxState* state = nullptr;
    
    // 从窗口属性获取状�?
    if (msg != WM_CREATE) {
        state = (ComboBoxState*)GetPropW(hwnd, L"ComboBoxState");
    }
    
    switch (msg) {
        case WM_CREATE: {
            CREATESTRUCT* cs = (CREATESTRUCT*)lparam;
            state = (ComboBoxState*)cs->lpCreateParams;
            SetPropW(hwnd, L"ComboBoxState", (HANDLE)state);
            
            // 创建渲染目标
            RECT rc;
            GetClientRect(hwnd, &rc);
            
            ID2D1HwndRenderTarget* rt = nullptr;
            g_d2d_factory->CreateHwndRenderTarget(
                D2D1::RenderTargetProperties(),
                D2D1::HwndRenderTargetProperties(hwnd, D2D1::SizeU(rc.right, rc.bottom)),
                &rt
            );
            
            SetPropW(hwnd, L"RenderTarget", (HANDLE)rt);
            return 0;
        }
        
        case WM_PAINT: {
            if (!state) break;
            
            OutputDebugStringW(L"ComboDropDownProc: WM_PAINT 收到，开始绘制");
            
            PAINTSTRUCT ps;
            BeginPaint(hwnd, &ps);
            
            ID2D1HwndRenderTarget* rt = (ID2D1HwndRenderTarget*)GetPropW(hwnd, L"RenderTarget");
            if (rt) {
                OutputDebugStringW(L"ComboDropDownProc: 渲染目标有效，调用DrawComboDropDown");
                DrawComboDropDown(rt, g_dwrite_factory, state);
            } else {
                OutputDebugStringW(L"ComboDropDownProc: 警告 - 渲染目标为空！");
            }
            
            EndPaint(hwnd, &ps);
            return 0;
        }
        
        case WM_LBUTTONDOWN: {
            if (!state) break;
            
            // 计算点击的项目索引（考虑滚动偏移）
            POINT pt = { GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam) };
            int item_height = state->item_height;
            int index = (pt.y + state->scroll_offset) / item_height;
            
            if (index >= 0 && index < (int)state->items.size()) {
                state->selected_index = index;
                
                // 更新编辑框文�?
                if (state->edit_hwnd) {
                    SetWindowTextW(state->edit_hwnd, state->items[index].c_str());
                }
                
                // 触发回调
                if (state->callback) {
                    state->callback(state->hwnd, index);
                }
                if (state->events.on_value_changed) {
                    state->events.on_value_changed(state->hwnd);
                }

                // 隐藏下拉列表
                ShowWindow(hwnd, SW_HIDE);
                state->dropdown_visible = false;
            }
            return 0;
        }
        
        case WM_MOUSEWHEEL: {
            if (!state) break;
            
            // 获取滚轮滚动量
            int delta = GET_WHEEL_DELTA_WPARAM(wparam);
            int scroll_lines = 3;  // 每次滚动3行
            
            // 计算总内容高度和可见高度
            int total_height = (int)state->items.size() * state->item_height;
            RECT client_rect;
            GetClientRect(hwnd, &client_rect);
            int visible_height = client_rect.bottom;
            
            // 计算最大滚动偏移
            int max_scroll = max(0, total_height - visible_height);
            
            // 更新滚动偏移
            if (delta > 0) {
                // 向上滚动
                state->scroll_offset = max(0, state->scroll_offset - scroll_lines * state->item_height);
            } else {
                // 向下滚动
                state->scroll_offset = min(max_scroll, state->scroll_offset + scroll_lines * state->item_height);
            }
            
            OutputDebugStringW((L"ComboDropDownProc: WM_MOUSEWHEEL - 滚动偏移: " + std::to_wstring(state->scroll_offset) + L", 最大: " + std::to_wstring(max_scroll)).c_str());
            
            // 重绘
            InvalidateRect(hwnd, nullptr, FALSE);
            return 0;
        }
        
        case WM_MOUSEMOVE: {
            if (!state) break;
            
            // 计算悬停的项目索引（考虑滚动偏移）
            POINT pt = { GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam) };
            int item_height = state->item_height;
            int index = (pt.y + state->scroll_offset) / item_height;
            
            if (index >= 0 && index < (int)state->items.size()) {
                if (state->hovered_index != index) {
                    state->hovered_index = index;
                    InvalidateRect(hwnd, nullptr, FALSE);
                }
            }
            return 0;
        }
        
        case WM_MOUSELEAVE: {
            if (state && state->hovered_index != -1) {
                state->hovered_index = -1;
                InvalidateRect(hwnd, nullptr, FALSE);
            }
            return 0;
        }
        
        case WM_SIZE: {
            // 窗口大小改变时，调整渲染目标大小
            if (state) {
                ID2D1HwndRenderTarget* rt = (ID2D1HwndRenderTarget*)GetPropW(hwnd, L"RenderTarget");
                if (rt) {
                    UINT width = LOWORD(lparam);
                    UINT height = HIWORD(lparam);
                    rt->Resize(D2D1::SizeU(width, height));
                }
            }
            return 0;
        }
        
        case WM_KILLFOCUS: {
            // 失去焦点时隐藏下拉列�?
            if (state) {
                ShowWindow(hwnd, SW_HIDE);
                state->dropdown_visible = false;
            }
            return 0;
        }
        
        case WM_DESTROY: {
            ID2D1HwndRenderTarget* rt = (ID2D1HwndRenderTarget*)GetPropW(hwnd, L"RenderTarget");
            if (rt) {
                rt->Release();
            }
            RemovePropW(hwnd, L"RenderTarget");
            RemovePropW(hwnd, L"ComboBoxState");
            return 0;
        }
    }
    
    return DefWindowProcW(hwnd, msg, wparam, lparam);
}

// 组合框主控件窗口过程
LRESULT CALLBACK ComboBoxProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData) {
    ComboBoxState* state = (ComboBoxState*)dwRefData;
    if (!state) return DefSubclassProc(hwnd, msg, wparam, lparam);

    // 通用事件处理
    HandleCommonEvents(hwnd, msg, wparam, lparam, &state->events);

    switch (msg) {
        case WM_PAINT: {
            PAINTSTRUCT ps;
            BeginPaint(hwnd, &ps);
            
            RECT rc;
            GetClientRect(hwnd, &rc);
            
            ID2D1HwndRenderTarget* rt = nullptr;
            g_d2d_factory->CreateHwndRenderTarget(
                D2D1::RenderTargetProperties(),
                D2D1::HwndRenderTargetProperties(hwnd, D2D1::SizeU(rc.right, rc.bottom)),
                &rt
            );
            
            if (rt) {
                DrawComboBox(rt, g_dwrite_factory, state);
                rt->Release();
            }
            
            EndPaint(hwnd, &ps);
            return 0;
        }
        
        case WM_LBUTTONDOWN: {
            if (!state->enabled) break;
            
            // 检查是否点击下拉按�?
            POINT pt = { GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam) };
            int button_width = 30;
            
            if (pt.x >= state->width - button_width) {
                state->button_pressed = true;
                InvalidateRect(hwnd, nullptr, FALSE);
            }
            return 0;
        }
        
        case WM_LBUTTONUP: {
            if (!state->enabled) break;
            
            if (state->button_pressed) {
                state->button_pressed = false;
                
                OutputDebugStringW((L"ComboBoxProc: WM_LBUTTONUP - 当前项目数: " + std::to_wstring(state->items.size())).c_str());
                
                // 切换下拉列表显示状�?
                if (state->dropdown_visible) {
                    OutputDebugStringW(L"ComboBoxProc: 隐藏下拉列表");
                    ShowWindow(state->dropdown_hwnd, SW_HIDE);
                    state->dropdown_visible = false;
                } else {
                    // 显示下拉列表
                    if (state->dropdown_hwnd && !state->items.empty()) {
                        OutputDebugStringW(L"ComboBoxProc: 显示下拉列表");
                        
                        // 重置滚动偏移
                        state->scroll_offset = 0;
                        
                        // 计算下拉列表位置
                        RECT rc;
                        GetWindowRect(hwnd, &rc);
                        
                        // 使用状态中的表项高度计算下拉列表高度
                        int dropdown_height = min((int)state->items.size() * state->item_height, 200);
                        
                        OutputDebugStringW((L"ComboBoxProc: 下拉列表高度: " + std::to_wstring(dropdown_height)).c_str());
                        
                        SetWindowPos(
                            state->dropdown_hwnd,
                            HWND_TOPMOST,
                            rc.left,
                            rc.bottom,
                            state->width,
                            dropdown_height,
                            SWP_SHOWWINDOW
                        );
                        
                        state->dropdown_visible = true;
                        SetFocus(state->dropdown_hwnd);
                        
                        OutputDebugStringW(L"ComboBoxProc: 下拉列表已显示");
                    } else {
                        if (!state->dropdown_hwnd) {
                            OutputDebugStringW(L"ComboBoxProc: 错误 - 下拉列表窗口不存在");
                        }
                        if (state->items.empty()) {
                            OutputDebugStringW(L"ComboBoxProc: 警告 - 项目列表为空");
                        }
                    }
                }
                
                InvalidateRect(hwnd, nullptr, FALSE);
            }
            return 0;
        }
        
        case WM_MOUSEMOVE: {
            if (!state->enabled) break;
            
            // 检查是否悬停在下拉按钮�?
            POINT pt = { GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam) };
            int button_width = 30;
            bool on_button = (pt.x >= state->width - button_width);
            
            if (on_button != state->button_hovered) {
                state->button_hovered = on_button;
                InvalidateRect(hwnd, nullptr, FALSE);
                
                if (on_button) {
                    // 跟踪鼠标离开
                    TRACKMOUSEEVENT tme = { sizeof(TRACKMOUSEEVENT) };
                    tme.dwFlags = TME_LEAVE;
                    tme.hwndTrack = hwnd;
                    TrackMouseEvent(&tme);
                }
            }
            return 0;
        }
        
        case WM_MOUSELEAVE: {
            if (state->button_hovered) {
                state->button_hovered = false;
                state->button_pressed = false;
                InvalidateRect(hwnd, nullptr, FALSE);
            }
            return 0;
        }
        
        case WM_SIZE: {
            state->width = LOWORD(lparam);
            state->height = HIWORD(lparam);
            
            // 调整编辑框大�?
            if (state->edit_hwnd) {
                int button_width = 30;
                SetWindowPos(
                    state->edit_hwnd,
                    nullptr,
                    0, 0,
                    state->width - button_width,
                    state->height,
                    SWP_NOZORDER | SWP_NOMOVE
                );
            }
            return 0;
        }
        
        case WM_NCDESTROY: {
            // 清理资源
            if (state->dropdown_hwnd) {
                DestroyWindow(state->dropdown_hwnd);
                state->dropdown_hwnd = nullptr;
            }
            
            RemoveWindowSubclass(hwnd, ComboBoxProc, uIdSubclass);
            g_comboboxes.erase(hwnd);
            delete state;
            return 0;
        }
    }
    
    return DefSubclassProc(hwnd, msg, wparam, lparam);
}

// 创建组合�?
HWND __stdcall CreateComboBox(
    HWND hParent,
    int x, int y, int width, int height,
    BOOL readonly,
    UINT32 fg_color,
    UINT32 bg_color,
    int item_height,
    const char* font_name_bytes,
    int font_name_len,
    int font_size,
    BOOL bold,
    BOOL italic,
    BOOL underline
) {
    if (!hParent) return nullptr;
    
    // 初始化D2D和DirectWrite
    if (!g_d2d_factory) {
        D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &g_d2d_factory);
    }
    if (!g_dwrite_factory) {
        DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory),
                           reinterpret_cast<IUnknown**>(&g_dwrite_factory));
    }
    
    // 创建主控件容�?
    int tb_offset = GetTitleBarOffset(hParent);
    HWND hwnd = CreateWindowExW(
        0,
        L"STATIC",
        L"",
        WS_CHILD | WS_VISIBLE | SS_NOTIFY,
        x, y + tb_offset, width, height,
        hParent,
        (HMENU)(INT_PTR)g_next_control_id++,
        GetModuleHandle(nullptr),
        nullptr
    );
    
    if (!hwnd) return nullptr;
    
    // 创建状态对�?
    ComboBoxState* state = new ComboBoxState();
    state->hwnd = hwnd;
    state->parent = hParent;
    state->id = (int)(INT_PTR)GetMenu(hwnd);
    state->x = x;
    state->y = y;
    state->width = width;
    state->height = height;
    state->selected_index = -1;
    state->hovered_index = -1;
    state->dropdown_visible = false;
    state->readonly = (readonly != 0);
    state->enabled = true;
    state->button_hovered = false;
    state->button_pressed = false;
    state->fg_color = fg_color ? fg_color : ThemeColor_TextPrimary();
    state->bg_color = bg_color ? bg_color : ThemeColor_Background();
    state->select_color = ThemeColor_BackgroundLight();
    state->hover_color = ThemeColor_BackgroundLight();
    
    // 设置表项高度（默认35）
    state->item_height = (item_height > 0) ? item_height : 35;
    
    // 设置字体属性
    if (font_name_bytes && font_name_len > 0) {
        state->font.font_name = Utf8ToWide((const unsigned char*)font_name_bytes, font_name_len);
    } else {
        state->font.font_name = L"Microsoft YaHei UI";
    }
    state->font.font_size = (font_size > 0) ? font_size : 14;
    state->font.bold = (bold != 0);
    state->font.italic = (italic != 0);
    state->font.underline = (underline != 0);
    state->callback = nullptr;
    
    // 创建编辑框（WS_BORDER：编辑区显示边框，与设计器一致）
    int button_width = 30;
    state->edit_hwnd = CreateWindowExW(
        0,
        L"EDIT",
        L"",
        WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL | (readonly ? ES_READONLY : 0),
        0, 0,
        width - button_width,
        height,
        hwnd,
        nullptr,
        GetModuleHandle(nullptr),
        nullptr
    );
    
    if (state->edit_hwnd) {
        // 设置编辑框字体
        // 将点大小转换为逻辑单位（负值表示字符高度）
        // 公式：-MulDiv(point_size, GetDeviceCaps(hdc, LOGPIXELSY), 72)
        HDC hdc = GetDC(state->edit_hwnd);
        int font_height = -MulDiv(state->font.font_size, GetDeviceCaps(hdc, LOGPIXELSY), 72);
        ReleaseDC(state->edit_hwnd, hdc);
        
        HFONT hFont = CreateFontW(
            font_height,  // 使用转换后的字体高度
            0, 0, 0,
            state->font.bold ? FW_BOLD : FW_NORMAL,
            state->font.italic,
            state->font.underline,
            FALSE,
            DEFAULT_CHARSET,
            OUT_DEFAULT_PRECIS,
            CLIP_DEFAULT_PRECIS,
            DEFAULT_QUALITY,
            DEFAULT_PITCH | FF_DONTCARE,
            state->font.font_name.c_str()
        );
        SendMessageW(state->edit_hwnd, WM_SETFONT, (WPARAM)hFont, TRUE);
        
        // 设置编辑框边距，使文本垂直居中
        // 计算垂直边距：(控件高度 - 字体高度) / 2
        int vertical_margin = (height - abs(font_height)) / 2;
        if (vertical_margin < 0) vertical_margin = 0;
        
        // 设置编辑框的矩形区域（左、上、右、下边距）
        RECT rc;
        rc.left = 5;  // 左边距
        rc.top = vertical_margin;  // 上边距（垂直居中）
        rc.right = width - button_width - 5;  // 右边距
        rc.bottom = height - vertical_margin;  // 下边距（垂直居中）
        SendMessageW(state->edit_hwnd, EM_SETRECT, 0, (LPARAM)&rc);
    }
    
    // 注册下拉列表窗口�?
    static bool dropdown_class_registered = false;
    if (!dropdown_class_registered) {
        WNDCLASSEXW wc = { sizeof(WNDCLASSEXW) };
        wc.lpfnWndProc = ComboDropDownProc;
        wc.hInstance = GetModuleHandle(nullptr);
        wc.lpszClassName = L"ComboBoxDropDown";
        wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
        wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
        RegisterClassExW(&wc);
        dropdown_class_registered = true;
    }
    
    // 创建下拉列表窗口（初始隐藏）
    state->dropdown_hwnd = CreateWindowExW(
        WS_EX_TOPMOST | WS_EX_TOOLWINDOW,
        L"ComboBoxDropDown",
        L"",
        WS_POPUP,
        0, 0, width, 200,
        hParent,
        nullptr,
        GetModuleHandle(nullptr),
        state
    );
    
    // 保存到全局map
    g_comboboxes[hwnd] = state;
    
    // 子类化主控件
    SetWindowSubclass(hwnd, ComboBoxProc, 0, (DWORD_PTR)state);
    
    return hwnd;
}

// 添加组合框项�?
int __stdcall AddComboItem(
    HWND hComboBox,
    const unsigned char* text_bytes,
    int text_len
) {
    auto it = g_comboboxes.find(hComboBox);
    if (it == g_comboboxes.end()) {
        OutputDebugStringW(L"AddComboItem: 错误 - 未找到组合框");
        return -1;
    }
    
    ComboBoxState* state = it->second;
    std::wstring text = Utf8ToWide(text_bytes, text_len);
    state->items.push_back(text);
    
    // 输出调试信息
    OutputDebugStringW((L"AddComboItem: 添加项目 '" + text + L"', 当前总数: " + std::to_wstring(state->items.size())).c_str());
    OutputDebugStringW((L"AddComboItem: 下拉列表可见性: " + std::to_wstring(state->dropdown_visible)).c_str());
    OutputDebugStringW((L"AddComboItem: 下拉列表窗口句柄: " + std::to_wstring((UINT_PTR)state->dropdown_hwnd)).c_str());
    
    // 如果下拉列表正在显示，需要重新调整大小和重绘
    if (state->dropdown_visible && state->dropdown_hwnd && IsWindow(state->dropdown_hwnd)) {
        OutputDebugStringW(L"AddComboItem: 下拉列表正在显示，更新大小和重绘");
        
        // 重新计算下拉列表高度
        RECT rc;
        GetWindowRect(state->hwnd, &rc);
        int dropdown_height = min((int)state->items.size() * state->item_height, 200);
        
        OutputDebugStringW((L"AddComboItem: 新的下拉列表高度: " + std::to_wstring(dropdown_height)).c_str());
        
        SetWindowPos(
            state->dropdown_hwnd,
            HWND_TOPMOST,
            rc.left,
            rc.bottom,
            state->width,
            dropdown_height,
            SWP_SHOWWINDOW | SWP_NOMOVE
        );
        
        // 自动滚动到最后一个项目（新添加的项目）
        int total_height = (int)state->items.size() * state->item_height;
        int max_scroll = max(0, total_height - dropdown_height);
        state->scroll_offset = max_scroll;  // 滚动到底部
        
        OutputDebugStringW((L"AddComboItem: 自动滚动到底部，偏移: " + std::to_wstring(state->scroll_offset)).c_str());
        
        // 调整渲染目标大小
        ID2D1HwndRenderTarget* rt = (ID2D1HwndRenderTarget*)GetPropW(state->dropdown_hwnd, L"RenderTarget");
        if (rt) {
            HRESULT hr = rt->Resize(D2D1::SizeU(state->width, dropdown_height));
            if (SUCCEEDED(hr)) {
                OutputDebugStringW(L"AddComboItem: 渲染目标已调整大小");
            } else {
                OutputDebugStringW((L"AddComboItem: 渲染目标调整大小失败，HRESULT: " + std::to_wstring(hr)).c_str());
            }
        } else {
            OutputDebugStringW(L"AddComboItem: 警告 - 未找到渲染目标");
        }
        
        // 强制重绘下拉列表
        InvalidateRect(state->dropdown_hwnd, nullptr, TRUE);
        UpdateWindow(state->dropdown_hwnd);
        OutputDebugStringW(L"AddComboItem: 已触发重绘");
    } else {
        OutputDebugStringW(L"AddComboItem: 下拉列表未显示，项目已添加到列表中");
        // 即使下拉列表未显示，项目也已经添加到state->items中
        // 下次打开下拉列表时会自动显示所有项目
    }
    
    return (int)state->items.size() - 1;
}

// 移除组合框项�?
void __stdcall RemoveComboItem(HWND hComboBox, int index) {
    auto it = g_comboboxes.find(hComboBox);
    if (it == g_comboboxes.end()) return;
    
    ComboBoxState* state = it->second;
    if (index >= 0 && index < (int)state->items.size()) {
        state->items.erase(state->items.begin() + index);
        
        // 调整选中索引
        if (state->selected_index == index) {
            state->selected_index = -1;
            if (state->edit_hwnd) {
                SetWindowTextW(state->edit_hwnd, L"");
            }
        } else if (state->selected_index > index) {
            state->selected_index--;
        }
    }
}

// 清空组合�?
void __stdcall ClearComboBox(HWND hComboBox) {
    auto it = g_comboboxes.find(hComboBox);
    if (it == g_comboboxes.end()) return;
    
    ComboBoxState* state = it->second;
    state->items.clear();
    state->selected_index = -1;
    state->hovered_index = -1;
    
    if (state->edit_hwnd) {
        SetWindowTextW(state->edit_hwnd, L"");
    }
}

// 获取组合框选中项索�?
int __stdcall GetComboSelectedIndex(HWND hComboBox) {
    auto it = g_comboboxes.find(hComboBox);
    if (it == g_comboboxes.end()) return -1;
    return it->second->selected_index;
}

// 设置组合框选中项索�?
void __stdcall SetComboSelectedIndex(HWND hComboBox, int index) {
    auto it = g_comboboxes.find(hComboBox);
    if (it == g_comboboxes.end()) return;
    
    ComboBoxState* state = it->second;
    if (index >= -1 && index < (int)state->items.size()) {
        state->selected_index = index;
        
        if (state->edit_hwnd) {
            if (index >= 0) {
                SetWindowTextW(state->edit_hwnd, state->items[index].c_str());
            } else {
                SetWindowTextW(state->edit_hwnd, L"");
            }
        }
        
        // 触发回调
        if (state->callback && index >= 0) {
            state->callback(hComboBox, index);
        }
        if (state->events.on_value_changed && index >= 0) {
            state->events.on_value_changed(hComboBox);
        }
    }
}

// 获取组合框项目数�?
int __stdcall GetComboItemCount(HWND hComboBox) {
    auto it = g_comboboxes.find(hComboBox);
    if (it == g_comboboxes.end()) return 0;
    return (int)it->second->items.size();
}

// 获取组合框项目文�?
int __stdcall GetComboItemText(
    HWND hComboBox,
    int index,
    unsigned char* buffer,
    int buffer_size
) {
    auto it = g_comboboxes.find(hComboBox);
    if (it == g_comboboxes.end()) return 0;
    
    ComboBoxState* state = it->second;
    if (index < 0 || index >= (int)state->items.size()) return 0;
    
    std::wstring text = state->items[index];
    int utf8_len = WideCharToMultiByte(CP_UTF8, 0, text.c_str(), -1, nullptr, 0, nullptr, nullptr);
    
    if (buffer && buffer_size > 0) {
        int copied = WideCharToMultiByte(CP_UTF8, 0, text.c_str(), -1, (LPSTR)buffer, buffer_size, nullptr, nullptr);
        return copied - 1;  // 不包括null终止�?
    }
    
    return utf8_len - 1;
}

// 设置组合框回�?
void __stdcall SetComboBoxCallback(HWND hComboBox, ComboBoxCallback callback) {
    auto it = g_comboboxes.find(hComboBox);
    if (it == g_comboboxes.end()) return;
    it->second->callback = callback;
}

// 启用/禁用组合�?
void __stdcall EnableComboBox(HWND hComboBox, BOOL enable) {
    auto it = g_comboboxes.find(hComboBox);
    if (it == g_comboboxes.end()) return;
    
    ComboBoxState* state = it->second;
    state->enabled = (enable != 0);
    
    if (state->edit_hwnd) {
        EnableWindow(state->edit_hwnd, enable);
    }
    
    InvalidateRect(hComboBox, nullptr, FALSE);
}

// 显示/隐藏组合�?
void __stdcall ShowComboBox(HWND hComboBox, BOOL show) {
    auto it = g_comboboxes.find(hComboBox);
    if (it == g_comboboxes.end()) return;
    
    ShowWindow(hComboBox, show ? SW_SHOW : SW_HIDE);
    
    if (!show && it->second->dropdown_visible) {
        ShowWindow(it->second->dropdown_hwnd, SW_HIDE);
        it->second->dropdown_visible = false;
    }
}

// 设置组合框位置和大小
void __stdcall SetComboBoxBounds(HWND hComboBox, int x, int y, int width, int height) {
    auto it = g_comboboxes.find(hComboBox);
    if (it == g_comboboxes.end()) return;
    
    ComboBoxState* state = it->second;
    state->x = x;
    state->y = y;
    state->width = width;
    state->height = height;
    
    SetWindowPos(hComboBox, nullptr, x, y, width, height, SWP_NOZORDER);
}

// 获取组合框文�?
int __stdcall GetComboBoxText(
    HWND hComboBox,
    unsigned char* buffer,
    int buffer_size
) {
    auto it = g_comboboxes.find(hComboBox);
    if (it == g_comboboxes.end()) return 0;
    
    ComboBoxState* state = it->second;
    if (!state->edit_hwnd) return 0;
    
    int text_len = GetWindowTextLengthW(state->edit_hwnd);
    if (text_len == 0) return 0;
    
    std::wstring text(text_len + 1, 0);
    GetWindowTextW(state->edit_hwnd, &text[0], text_len + 1);
    
    int utf8_len = WideCharToMultiByte(CP_UTF8, 0, text.c_str(), -1, nullptr, 0, nullptr, nullptr);
    
    if (buffer && buffer_size > 0) {
        int copied = WideCharToMultiByte(CP_UTF8, 0, text.c_str(), -1, (LPSTR)buffer, buffer_size, nullptr, nullptr);
        return copied - 1;
    }
    
    return utf8_len - 1;
}

// 设置组合框文�?
void __stdcall SetComboBoxText(
    HWND hComboBox,
    const unsigned char* text_bytes,
    int text_len
) {
    auto it = g_comboboxes.find(hComboBox);
    if (it == g_comboboxes.end()) return;
    
    ComboBoxState* state = it->second;
    if (!state->edit_hwnd) return;
    
    std::wstring text = Utf8ToWide(text_bytes, text_len);
    SetWindowTextW(state->edit_hwnd, text.c_str());
}

// ========== 热键控件实现 ==========

// 将虚拟键码转换为本地化键名
std::wstring GetKeyName(int vk_code) {
    // 特殊键名映射（中文）
    switch (vk_code) {
        case VK_BACK: return L"Backspace";
        case VK_TAB: return L"Tab";
        case VK_RETURN: return L"Enter";
        case VK_SHIFT: return L"Shift";
        case VK_CONTROL: return L"Ctrl";
        case VK_MENU: return L"Alt";
        case VK_PAUSE: return L"Pause";
        case VK_CAPITAL: return L"Caps Lock";
        case VK_ESCAPE: return L"Esc";
        case VK_SPACE: return L"Space";
        case VK_PRIOR: return L"Page Up";
        case VK_NEXT: return L"Page Down";
        case VK_END: return L"End";
        case VK_HOME: return L"Home";
        case VK_LEFT: return L"←";
        case VK_UP: return L"↑";
        case VK_RIGHT: return L"→";
        case VK_DOWN: return L"↓";
        case VK_INSERT: return L"Insert";
        case VK_DELETE: return L"Delete";
        case VK_LWIN: return L"Win";
        case VK_RWIN: return L"Win";
        case VK_NUMPAD0: return L"Num 0";
        case VK_NUMPAD1: return L"Num 1";
        case VK_NUMPAD2: return L"Num 2";
        case VK_NUMPAD3: return L"Num 3";
        case VK_NUMPAD4: return L"Num 4";
        case VK_NUMPAD5: return L"Num 5";
        case VK_NUMPAD6: return L"Num 6";
        case VK_NUMPAD7: return L"Num 7";
        case VK_NUMPAD8: return L"Num 8";
        case VK_NUMPAD9: return L"Num 9";
        case VK_MULTIPLY: return L"Num *";
        case VK_ADD: return L"Num +";
        case VK_SUBTRACT: return L"Num -";
        case VK_DECIMAL: return L"Num .";
        case VK_DIVIDE: return L"Num /";
        case VK_F1: return L"F1";
        case VK_F2: return L"F2";
        case VK_F3: return L"F3";
        case VK_F4: return L"F4";
        case VK_F5: return L"F5";
        case VK_F6: return L"F6";
        case VK_F7: return L"F7";
        case VK_F8: return L"F8";
        case VK_F9: return L"F9";
        case VK_F10: return L"F10";
        case VK_F11: return L"F11";
        case VK_F12: return L"F12";
        case VK_NUMLOCK: return L"Num Lock";
        case VK_SCROLL: return L"Scroll Lock";
        case VK_OEM_1: return L";";
        case VK_OEM_PLUS: return L"=";
        case VK_OEM_COMMA: return L",";
        case VK_OEM_MINUS: return L"-";
        case VK_OEM_PERIOD: return L".";
        case VK_OEM_2: return L"/";
        case VK_OEM_3: return L"`";
        case VK_OEM_4: return L"[";
        case VK_OEM_5: return L"\\";
        case VK_OEM_6: return L"]";
        case VK_OEM_7: return L"'";
    }
    
    // 字母和数字键
    if (vk_code >= '0' && vk_code <= '9') {
        return std::wstring(1, (wchar_t)vk_code);
    }
    if (vk_code >= 'A' && vk_code <= 'Z') {
        return std::wstring(1, (wchar_t)vk_code);
    }
    
    return L"";
}

// 格式化热键显示文本
std::wstring FormatHotKeyText(int vk_code, int modifiers) {
    if (vk_code == 0) {
        return L"无";
    }
    
    std::wstring text;
    
    // 添加修饰键
    if (modifiers & 1) {  // Ctrl
        text += L"Ctrl+";
    }
    if (modifiers & 2) {  // Shift
        text += L"Shift+";
    }
    if (modifiers & 4) {  // Alt
        text += L"Alt+";
    }
    
    // 添加主键
    std::wstring key_name = GetKeyName(vk_code);
    if (!key_name.empty()) {
        text += key_name;
    } else {
        text += L"?";
    }
    
    return text;
}

// 绘制热键控件
void DrawHotKey(ID2D1HwndRenderTarget* rt, IDWriteFactory* factory, HotKeyState* state) {
    if (!rt || !factory || !state) return;
    
    rt->BeginDraw();
    
    // 清除背景
    rt->Clear(D2D1::ColorF(D2D1::ColorF::White, 0.0f));
    
    // Element UI 配色
    UINT32 border_color = state->has_focus ? ThemeColor_Primary() : ThemeColor_BorderBase();  // 焦点时主题主色边框
    UINT32 bg_color = state->enabled ? state->bg_color : ThemeColor_BackgroundLight();
    UINT32 text_color = state->enabled ? state->fg_color : ThemeColor_TextPlaceholder();
    
    if (!state->enabled) {
        border_color = 0xFFE4E7ED;
    }
    
    D2D1_RECT_F rect = D2D1::RectF(0, 0, (float)state->width, (float)state->height);
    
    // 绘制背景
    ID2D1SolidColorBrush* bg_brush = nullptr;
    rt->CreateSolidColorBrush(ColorFromUInt32(bg_color), &bg_brush);
    if (bg_brush) {
        ID2D1RoundedRectangleGeometry* rounded_rect = nullptr;
        g_d2d_factory->CreateRoundedRectangleGeometry(
            D2D1::RoundedRect(rect, 4.0f, 4.0f), &rounded_rect);
        if (rounded_rect) {
            rt->FillGeometry(rounded_rect, bg_brush);
            rounded_rect->Release();
        }
        bg_brush->Release();
    }
    
    // 绘制边框
    ID2D1SolidColorBrush* border_brush = nullptr;
    rt->CreateSolidColorBrush(ColorFromUInt32(border_color), &border_brush);
    if (border_brush) {
        ID2D1RoundedRectangleGeometry* rounded_rect = nullptr;
        g_d2d_factory->CreateRoundedRectangleGeometry(
            D2D1::RoundedRect(rect, 4.0f, 4.0f), &rounded_rect);
        if (rounded_rect) {
            rt->DrawGeometry(rounded_rect, border_brush, state->has_focus ? 2.0f : 1.0f);
            rounded_rect->Release();
        }
        border_brush->Release();
    }
    
    // 绘制文本
    if (!state->display_text.empty()) {
        ID2D1SolidColorBrush* text_brush = nullptr;
        rt->CreateSolidColorBrush(ColorFromUInt32(text_color), &text_brush);
        if (text_brush) {
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
                
                D2D1_RECT_F text_rect = D2D1::RectF(
                    5, 0,
                    (float)state->width - 5,
                    (float)state->height
                );
                
                // 使用DrawText而不是DrawTextW
                rt->DrawText(
                    state->display_text.c_str(),
                    (UINT32)state->display_text.length(),
                    text_format,
                    text_rect,
                    text_brush
                );
                
                text_format->Release();
            }
            
            text_brush->Release();
        }
    }
    
    rt->EndDraw();
}

// 热键控件窗口过程
LRESULT CALLBACK HotKeyProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData) {
    auto it = g_hotkeys.find(hwnd);
    if (it == g_hotkeys.end()) {
        return DefSubclassProc(hwnd, msg, wparam, lparam);
    }
    
    HotKeyState* state = it->second;

    // 通用事件处理
    HandleCommonEvents(hwnd, msg, wparam, lparam, &state->events);

    switch (msg) {
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            
            // 创建渲染目标
            ID2D1HwndRenderTarget* rt = nullptr;
            D2D1_RENDER_TARGET_PROPERTIES props = D2D1::RenderTargetProperties();
            D2D1_HWND_RENDER_TARGET_PROPERTIES hwnd_props = D2D1::HwndRenderTargetProperties(
                hwnd,
                D2D1::SizeU(state->width, state->height)
            );
            
            HRESULT hr = g_d2d_factory->CreateHwndRenderTarget(props, hwnd_props, &rt);
            if (SUCCEEDED(hr) && rt) {
                DrawHotKey(rt, g_dwrite_factory, state);
                rt->Release();
            }
            
            EndPaint(hwnd, &ps);
            return 0;
        }
        
        case WM_SETFOCUS: {
            state->has_focus = true;
            state->capturing = true;
            InvalidateRect(hwnd, nullptr, FALSE);
            return 0;
        }
        
        case WM_KILLFOCUS: {
            state->has_focus = false;
            state->capturing = false;
            InvalidateRect(hwnd, nullptr, FALSE);
            return 0;
        }
        
        case WM_KEYDOWN:
        case WM_SYSKEYDOWN: {
            if (!state->enabled || !state->capturing) break;
            
            int vk = (int)wparam;
            
            // 忽略单独的修饰键
            if (vk == VK_CONTROL || vk == VK_SHIFT || vk == VK_MENU) {
                return 0;
            }
            
            // 获取修饰键状态
            int modifiers = 0;
            if (GetKeyState(VK_CONTROL) & 0x8000) modifiers |= 1;
            if (GetKeyState(VK_SHIFT) & 0x8000) modifiers |= 2;
            if (GetKeyState(VK_MENU) & 0x8000) modifiers |= 4;
            
            // 更新热键
            state->vk_code = vk;
            state->modifiers = modifiers;
            state->display_text = FormatHotKeyText(vk, modifiers);
            
            InvalidateRect(hwnd, nullptr, FALSE);
            
            // 触发回调
            if (state->callback) {
                state->callback(hwnd, vk, modifiers);
            }
            if (state->events.on_value_changed) {
                state->events.on_value_changed(hwnd);
            }
            
            return 0;
        }
        
        case WM_CHAR:
        case WM_SYSCHAR: {
            // 阻止字符输入
            return 0;
        }
        
        case WM_LBUTTONDOWN: {
            if (state->enabled) {
                SetFocus(hwnd);
            }
            return 0;
        }
        
        case WM_ENABLE: {
            state->enabled = (BOOL)wparam;
            InvalidateRect(hwnd, nullptr, FALSE);
            return 0;
        }
        
        case WM_NCDESTROY: {
            // 清理资源
            RemoveWindowSubclass(hwnd, HotKeyProc, uIdSubclass);
            
            auto it = g_hotkeys.find(hwnd);
            if (it != g_hotkeys.end()) {
                delete it->second;
                g_hotkeys.erase(it);
            }
            break;
        }
    }
    
    return DefSubclassProc(hwnd, msg, wparam, lparam);
}

// 创建热键控件
HWND __stdcall CreateHotKeyControl(
    HWND hParent,
    int x, int y, int width, int height,
    UINT32 fg_color,
    UINT32 bg_color
) {
    if (!hParent || !IsWindow(hParent)) return nullptr;
    
    // 创建静态控件作为基础
    int tb_offset = GetTitleBarOffset(hParent);
    HWND hwnd = CreateWindowExW(
        0,
        L"STATIC",
        L"",
        WS_CHILD | WS_VISIBLE | WS_TABSTOP | SS_NOTIFY,
        x, y + tb_offset, width, height,
        hParent,
        nullptr,
        GetModuleHandle(nullptr),
        nullptr
    );

    if (!hwnd) return nullptr;

    // 创建状态
    HotKeyState* state = new HotKeyState();
    state->hwnd = hwnd;
    state->parent = hParent;
    state->id = (int)(INT_PTR)hwnd;
    state->x = x;
    state->y = y;
    state->width = width;
    state->height = height;
    state->vk_code = 0;
    state->modifiers = 0;
    state->display_text = L"无";
    state->capturing = false;
    state->has_focus = false;
    state->enabled = true;
    state->fg_color = fg_color;
    state->bg_color = bg_color;
    state->border_color = 0xFFDCDFE6;
    state->font.font_name = L"Microsoft YaHei UI";
    state->font.font_size = 14;
    state->font.bold = false;
    state->font.italic = false;
    state->font.underline = false;
    state->callback = nullptr;
    
    g_hotkeys[hwnd] = state;
    
    // 子类化窗口
    SetWindowSubclass(hwnd, HotKeyProc, 0, 0);
    
    return hwnd;
}

// 获取热键
void __stdcall GetHotKey(
    HWND hHotKey,
    int* vk_code,
    int* modifiers
) {
    auto it = g_hotkeys.find(hHotKey);
    if (it == g_hotkeys.end()) return;
    
    HotKeyState* state = it->second;
    if (vk_code) *vk_code = state->vk_code;
    if (modifiers) *modifiers = state->modifiers;
}

// 设置热键
void __stdcall SetHotKey(
    HWND hHotKey,
    int vk_code,
    int modifiers
) {
    auto it = g_hotkeys.find(hHotKey);
    if (it == g_hotkeys.end()) return;
    
    HotKeyState* state = it->second;
    state->vk_code = vk_code;
    state->modifiers = modifiers;
    state->display_text = FormatHotKeyText(vk_code, modifiers);
    
    InvalidateRect(hHotKey, nullptr, FALSE);
}

// 清除热键
void __stdcall ClearHotKey(
    HWND hHotKey
) {
    SetHotKey(hHotKey, 0, 0);
}

// 设置热键回调
void __stdcall SetHotKeyCallback(
    HWND hHotKey,
    HotKeyCallback callback
) {
    auto it = g_hotkeys.find(hHotKey);
    if (it == g_hotkeys.end()) return;
    
    it->second->callback = callback;
}

// 启用/禁用热键控件
void __stdcall EnableHotKeyControl(
    HWND hHotKey,
    BOOL enable
) {
    if (!hHotKey || !IsWindow(hHotKey)) return;
    EnableWindow(hHotKey, enable);
}

// 显示/隐藏热键控件
void __stdcall ShowHotKeyControl(
    HWND hHotKey,
    BOOL show
) {
    if (!hHotKey || !IsWindow(hHotKey)) return;
    ShowWindow(hHotKey, show ? SW_SHOW : SW_HIDE);
}

// 设置热键控件位置和大小
void __stdcall SetHotKeyControlBounds(
    HWND hHotKey,
    int x, int y, int width, int height
) {
    auto it = g_hotkeys.find(hHotKey);
    if (it == g_hotkeys.end()) return;
    
    HotKeyState* state = it->second;
    state->x = x;
    state->y = y;
    state->width = width;
    state->height = height;
    
    SetWindowPos(hHotKey, nullptr, x, y, width, height, SWP_NOZORDER);
    InvalidateRect(hHotKey, nullptr, FALSE);
}

// ========== 分组框功能实现 ==========

// 创建分组框
HWND __stdcall CreateGroupBox(
    HWND hParent,
    int x, int y, int width, int height,
    const unsigned char* title_bytes,
    int title_len,
    UINT32 border_color,
    UINT32 bg_color,
    const unsigned char* font_name_bytes,
    int font_name_len,
    int font_size,
    BOOL bold,
    BOOL italic,
    BOOL underline
) {
    if (!hParent || !IsWindow(hParent)) return nullptr;

    // 创建静态控件作为基础
    int tb_offset = GetTitleBarOffset(hParent);
    HWND hwnd = CreateWindowExW(
        0,
        L"STATIC",
        L"",
        WS_CHILD | WS_VISIBLE | SS_NOTIFY,
        x, y + tb_offset, width, height,
        hParent,
        nullptr,
        GetModuleHandle(nullptr),
        nullptr
    );

    if (!hwnd) return nullptr;

    // 创建状态
    GroupBoxState* state = new GroupBoxState();
    state->hwnd = hwnd;
    state->parent = hParent;
    state->id = (int)(INT_PTR)hwnd;
    state->x = x;
    state->y = y;
    state->width = width;
    state->height = height;
    state->title = Utf8ToWide(title_bytes, title_len);
    state->enabled = true;
    state->visible = true;
    state->border_color = border_color;
    state->title_color = 0xFF303133;  // Element UI 主要文本色
    state->bg_color = bg_color;
    if (font_name_bytes && font_name_len > 0) {
        state->font.font_name = Utf8ToWide(font_name_bytes, font_name_len);
    } else {
        state->font.font_name = L"Microsoft YaHei UI";
    }
    state->font.font_size = (font_size > 0) ? font_size : 14;
    state->font.bold = bold;
    state->font.italic = italic;
    state->font.underline = underline;
    state->callback = nullptr;
    
    g_groupboxes[hwnd] = state;
    
    // 子类化窗口
    SetWindowSubclass(hwnd, GroupBoxProc, 0, 0);
    
    // 创建窗口区域：只包含边框和标题，内容区域留空（不遮挡按钮）
    // 这样分组框只绘制边框和标题，不会遮挡主窗口上的按钮
    HRGN hRgnOuter = CreateRoundRectRgn(0, 12, width, height, 8, 8);  // 外边框（从标题中间开始）
    HRGN hRgnInner = CreateRoundRectRgn(10, 25, width - 10, height - 10, 4, 4);  // 内容区域
    HRGN hRgnFinal = CreateRectRgn(0, 0, 0, 0);
    CombineRgn(hRgnFinal, hRgnOuter, hRgnInner, RGN_DIFF);  // 外边框减去内容区域
    
    // 添加标题区域
    HRGN hRgnTitle = CreateRectRgn(0, 0, width, 25);
    CombineRgn(hRgnFinal, hRgnFinal, hRgnTitle, RGN_OR);
    
    SetWindowRgn(hwnd, hRgnFinal, TRUE);
    
    // 清理临时区域（SetWindowRgn 会接管 hRgnFinal 的所有权）
    DeleteObject(hRgnOuter);
    DeleteObject(hRgnInner);
    DeleteObject(hRgnTitle);
    
    // 立即触发重绘，确保分组框显示
    InvalidateRect(hwnd, nullptr, TRUE);
    UpdateWindow(hwnd);
    
    return hwnd;
}

// 添加子控件到分组框
void __stdcall AddChildToGroup(
    HWND hGroupBox,
    HWND hChild
) {
    auto it = g_groupboxes.find(hGroupBox);
    if (it == g_groupboxes.end()) return;
    if (!hChild || !IsWindow(hChild)) return;
    
    GroupBoxState* state = it->second;
    
    // 检查是否已经在列表中
    auto child_it = std::find(state->children.begin(), state->children.end(), hChild);
    if (child_it != state->children.end()) return;
    
    // 添加到子控件列表
    state->children.push_back(hChild);
    
    // 如果是单选按钮,设置其分组ID为分组框的ID
    auto radio_it = g_radiobuttons.find(hChild);
    if (radio_it != g_radiobuttons.end()) {
        RadioButtonState* radio_state = radio_it->second;
        radio_state->group_id = state->id;
        
        // 更新单选按钮分组
        g_radio_groups[state->id].push_back(hChild);
    }
}

// 从分组框移除子控件
void __stdcall RemoveChildFromGroup(
    HWND hGroupBox,
    HWND hChild
) {
    auto it = g_groupboxes.find(hGroupBox);
    if (it == g_groupboxes.end()) return;
    
    GroupBoxState* state = it->second;
    
    // 从子控件列表中移除
    auto child_it = std::find(state->children.begin(), state->children.end(), hChild);
    if (child_it != state->children.end()) {
        state->children.erase(child_it);
    }
    
    // 如果是单选按钮,从分组中移除
    auto radio_it = g_radiobuttons.find(hChild);
    if (radio_it != g_radiobuttons.end()) {
        auto& group = g_radio_groups[state->id];
        auto group_it = std::find(group.begin(), group.end(), hChild);
        if (group_it != group.end()) {
            group.erase(group_it);
        }
    }
}

// 设置分组框标题
void __stdcall SetGroupBoxTitle(
    HWND hGroupBox,
    const unsigned char* title_bytes,
    int title_len
) {
    auto it = g_groupboxes.find(hGroupBox);
    if (it == g_groupboxes.end()) return;
    
    GroupBoxState* state = it->second;
    state->title = Utf8ToWide(title_bytes, title_len);
    
    InvalidateRect(hGroupBox, nullptr, FALSE);
}

// 启用/禁用分组框
void __stdcall EnableGroupBox(
    HWND hGroupBox,
    BOOL enable
) {
    auto it = g_groupboxes.find(hGroupBox);
    if (it == g_groupboxes.end()) return;

    GroupBoxState* state = it->second;
    state->enabled = enable ? true : false;

    // 同步启用/禁用所有子控件（使用各控件自己的启用函数）
    for (HWND hChild : state->children) {
        // 检查是否是复选框
        auto cb_it = g_checkboxes.find(hChild);
        if (cb_it != g_checkboxes.end()) {
            cb_it->second->enabled = (enable != 0);
            InvalidateRect(hChild, nullptr, FALSE);
            continue;
        }

        // 检查是否是单选按钮
        auto rb_it = g_radiobuttons.find(hChild);
        if (rb_it != g_radiobuttons.end()) {
            rb_it->second->enabled = (enable != 0);
            InvalidateRect(hChild, nullptr, FALSE);
            continue;
        }

        // 其他控件使用Windows API
        EnableWindow(hChild, enable);
    }

    InvalidateRect(hGroupBox, nullptr, FALSE);
    
    // 同步启用/禁用分组框内的按钮（主窗口上绘制的按钮）
    auto win_it = g_windows.find(state->parent);
    if (win_it != g_windows.end()) {
        WindowState* win_state = win_it->second;
        for (int button_id : state->button_ids) {
            // 查找按钮并设置启用状态
            for (auto& button : win_state->buttons) {
                if (button.id == button_id) {
                    button.enabled = (enable != 0);
                    break;
                }
            }
        }
    }
}

// 实际执行显示/隐藏（在消息泵中调用，勿在树回调同栈直接 ShowWindow）
static void ApplyShowGroupBox(HWND hGroupBox, BOOL show) {
    auto it = g_groupboxes.find(hGroupBox);
    if (it == g_groupboxes.end()) {
        WriteLog("ApplyShowGroupBox: groupbox not found, hGroupBox=%p, show=%d", hGroupBox, show ? 1 : 0);
        return;
    }

    GroupBoxState* state = it->second;
    WriteLog("ApplyShowGroupBox: begin hGroupBox=%p, show=%d, state=%p, child_count=%zu, visible_before=%d",
        hGroupBox,
        show ? 1 : 0,
        state,
        state->children.size(),
        state->visible ? 1 : 0);
    state->visible = show ? true : false;

    for (HWND hChild : state->children) {
        WriteLog("ApplyShowGroupBox: child ShowWindow hGroupBox=%p, hChild=%p, cmd=%d, child_is_window=%d",
            hGroupBox, hChild, show ? SW_SHOW : SW_HIDE, (hChild && IsWindow(hChild)) ? 1 : 0);
        ShowWindow(hChild, show ? SW_SHOW : SW_HIDE);
    }

    WriteLog("ApplyShowGroupBox: groupbox ShowWindow hGroupBox=%p, cmd=%d", hGroupBox, show ? SW_SHOW : SW_HIDE);
    ShowWindow(hGroupBox, show ? SW_SHOW : SW_HIDE);
    WriteLog("ApplyShowGroupBox: InvalidateRect hGroupBox=%p", hGroupBox);
    InvalidateRect(hGroupBox, nullptr, FALSE);
    WriteLog("ApplyShowGroupBox: end hGroupBox=%p, visible_now=%d", hGroupBox, state->visible ? 1 : 0);
}

// 显示/隐藏分组框（投递到顶层主窗口延后执行，避免易语言树选中回调内同步 ShowWindow 重入崩溃）
void __stdcall ShowGroupBox(
    HWND hGroupBox,
    BOOL show
) {
    auto it = g_groupboxes.find(hGroupBox);
    if (it == g_groupboxes.end()) {
        WriteLog("ShowGroupBox: groupbox not found, hGroupBox=%p, show=%d", hGroupBox, show ? 1 : 0);
        return;
    }

    HWND root = GetAncestor(hGroupBox, GA_ROOT);
    WriteLog("ShowGroupBox: request hGroupBox=%p, show=%d, root=%p", hGroupBox, show ? 1 : 0, root);
    if (root && PostMessageW(root, WM_EW_GROUPBOX_SHOW_DEFERRED, (WPARAM)hGroupBox, (LPARAM)(show ? 1 : 0))) {
        WriteLog("ShowGroupBox: posted WM_EW_GROUPBOX_SHOW_DEFERRED, hGroupBox=%p, root=%p, show=%d",
            hGroupBox, root, show ? 1 : 0);
        return;
    }
    WriteLog("ShowGroupBox: PostMessage failed or no root, applying inline, hGroupBox=%p, show=%d",
        hGroupBox, show ? 1 : 0);
    ApplyShowGroupBox(hGroupBox, show);
}

// 设置分组框位置和大小
void __stdcall SetGroupBoxBounds(
    HWND hGroupBox,
    int x, int y, int width, int height
) {
    auto it = g_groupboxes.find(hGroupBox);
    if (it == g_groupboxes.end()) return;
    
    GroupBoxState* state = it->second;
    
    // 计算偏移量
    int dx = x - state->x;
    int dy = y - state->y;
    
    // 更新分组框位置和大小
    state->x = x;
    state->y = y;
    state->width = width;
    state->height = height;
    
    SetWindowPos(hGroupBox, nullptr, x, y, width, height, SWP_NOZORDER);
    
    // 同步移动所有子控件
    for (HWND hChild : state->children) {
        RECT rect;
        GetWindowRect(hChild, &rect);
        POINT pt = { rect.left, rect.top };
        ScreenToClient(state->parent, &pt);
        
        SetWindowPos(hChild, nullptr, pt.x + dx, pt.y + dy, 
                    rect.right - rect.left, rect.bottom - rect.top, 
                    SWP_NOZORDER);
    }
    
    InvalidateRect(hGroupBox, nullptr, FALSE);
}

// 设置分组框回调
void __stdcall SetGroupBoxCallback(
    HWND hGroupBox,
    GroupBoxCallback callback
) {
    auto it = g_groupboxes.find(hGroupBox);
    if (it == g_groupboxes.end()) return;
    
    it->second->callback = callback;
}

// 获取分组框标题
__declspec(dllexport) int __stdcall GetGroupBoxTitle(
    HWND hGroupBox,
    unsigned char* buffer,
    int buffer_size
) {
    auto it = g_groupboxes.find(hGroupBox);
    if (it == g_groupboxes.end()) return -1;

    GroupBoxState* state = it->second;
    std::string utf8_title = WindowWideToUtf8(state->title);

    if (buffer == nullptr) {
        return (int)utf8_title.size();
    }

    if (buffer_size < (int)utf8_title.size()) return -1;
    memcpy(buffer, utf8_title.c_str(), utf8_title.size());
    return (int)utf8_title.size();
}

// 获取分组框位置和大小
__declspec(dllexport) int __stdcall GetGroupBoxBounds(
    HWND hGroupBox,
    int* x,
    int* y,
    int* width,
    int* height
) {
    auto it = g_groupboxes.find(hGroupBox);
    if (it == g_groupboxes.end()) return -1;

    GroupBoxState* state = it->second;
    if (x) *x = state->x;
    if (y) *y = state->y;
    if (width) *width = state->width;
    if (height) *height = state->height;
    return 0;
}

// 获取分组框可视状态
__declspec(dllexport) int __stdcall GetGroupBoxVisible(
    HWND hGroupBox
) {
    auto it = g_groupboxes.find(hGroupBox);
    if (it == g_groupboxes.end()) return -1;

    return it->second->visible ? 1 : 0;
}

// 获取分组框启用状态
__declspec(dllexport) int __stdcall GetGroupBoxEnabled(
    HWND hGroupBox
) {
    auto it = g_groupboxes.find(hGroupBox);
    if (it == g_groupboxes.end()) return -1;

    return it->second->enabled ? 1 : 0;
}

// 获取分组框颜色（边框色和背景色）
__declspec(dllexport) int __stdcall GetGroupBoxColor(
    HWND hGroupBox,
    UINT32* border_color,
    UINT32* bg_color
) {
    auto it = g_groupboxes.find(hGroupBox);
    if (it == g_groupboxes.end()) return -1;

    GroupBoxState* state = it->second;
    if (border_color) *border_color = state->border_color;
    if (bg_color) *bg_color = state->bg_color;
    return 0;
}

// ========================================
// TabControl 属性命令
// ========================================

// 获取指定 Tab 页的标题（UTF-8，两次调用模式）
__declspec(dllexport) int __stdcall GetTabTitle(
    HWND hTabControl,
    int index,
    unsigned char* buffer,
    int bufferSize
) {
    auto it = g_tab_controls.find(hTabControl);
    if (it == g_tab_controls.end()) return -1;

    TabControlState* state = it->second;
    if (index < 0 || index >= (int)state->pages.size()) return -1;

    std::string utf8 = WindowWideToUtf8(state->pages[index].title);

    if (buffer == nullptr) {
        return (int)utf8.size();
    }
    if (bufferSize < (int)utf8.size()) return -1;
    memcpy(buffer, utf8.c_str(), utf8.size());
    return (int)utf8.size();
}

// 设置指定 Tab 页的标题（UTF-8）
__declspec(dllexport) int __stdcall SetTabTitle(
    HWND hTabControl,
    int index,
    const unsigned char* title_bytes,
    int title_len
) {
    auto it = g_tab_controls.find(hTabControl);
    if (it == g_tab_controls.end()) return -1;

    TabControlState* state = it->second;
    if (index < 0 || index >= (int)state->pages.size()) return -1;
    if (!title_bytes || title_len < 0) return -1;

    std::wstring newTitle = Utf8ToWide(title_bytes, title_len);
    state->pages[index].title = newTitle;

    // 更新 TabControl 控件中的标题
    TCITEMW tci = {};
    tci.mask = TCIF_TEXT;
    tci.pszText = (LPWSTR)newTitle.c_str();
    TabCtrl_SetItem(hTabControl, index, &tci);

    // 触发重绘
    InvalidateRect(hTabControl, nullptr, TRUE);
    return 0;
}

// 获取 TabControl 的位置和大小
__declspec(dllexport) int __stdcall GetTabControlBounds(
    HWND hTabControl,
    int* x,
    int* y,
    int* width,
    int* height
) {
    if (!IsWindow(hTabControl)) return -1;
    auto it = g_tab_controls.find(hTabControl);
    if (it == g_tab_controls.end()) return -1;

    RECT rc;
    if (!GetWindowRect(hTabControl, &rc)) return -1;

    // 转换为父窗口客户区坐标
    HWND hParent = it->second->hParent;
    if (hParent && IsWindow(hParent)) {
        POINT pt = { rc.left, rc.top };
        ScreenToClient(hParent, &pt);
        if (x) *x = pt.x;
        if (y) *y = pt.y;
    } else {
        if (x) *x = rc.left;
        if (y) *y = rc.top;
    }
    if (width)  *width  = rc.right - rc.left;
    if (height) *height = rc.bottom - rc.top;
    return 0;
}

// 设置 TabControl 的位置和大小
__declspec(dllexport) int __stdcall SetTabControlBounds(
    HWND hTabControl,
    int x,
    int y,
    int width,
    int height
) {
    if (!IsWindow(hTabControl)) return -1;
    auto it = g_tab_controls.find(hTabControl);
    if (it == g_tab_controls.end()) return -1;

    // 补偿标题栏偏移
    int tb_offset = GetTitleBarOffset(it->second->hParent);
    MoveWindow(hTabControl, x, y + tb_offset, width, height, TRUE);

    // 更新内容窗口布局
    UpdateTabLayout(it->second);
    return 0;
}

// 获取 TabControl 的可视状态（1=可见, 0=不可见, -1=错误）
__declspec(dllexport) int __stdcall GetTabControlVisible(
    HWND hTabControl
) {
    if (!IsWindow(hTabControl)) return -1;
    auto it = g_tab_controls.find(hTabControl);
    if (it == g_tab_controls.end()) return -1;

    return IsWindowVisible(hTabControl) ? 1 : 0;
}

// 显示或隐藏 TabControl（visible: 1=显示, 0=隐藏）
__declspec(dllexport) int __stdcall ShowTabControl(
    HWND hTabControl,
    int visible
) {
    if (!IsWindow(hTabControl)) return -1;
    auto it = g_tab_controls.find(hTabControl);
    if (it == g_tab_controls.end()) return -1;

    TabControlState* state = it->second;
    ShowWindow(hTabControl, visible ? SW_SHOW : SW_HIDE);

    // 同步隐藏/显示当前内容窗口
    if (state->currentIndex >= 0 && state->currentIndex < (int)state->pages.size()) {
        HWND hContent = state->pages[state->currentIndex].hContentWindow;
        if (hContent && IsWindow(hContent)) {
            ShowWindow(hContent, visible ? SW_SHOW : SW_HIDE);
        }
    }
    return 0;
}

// 启用或禁用 TabControl（enabled: 1=启用, 0=禁用）
__declspec(dllexport) int __stdcall EnableTabControl(
    HWND hTabControl,
    int enabled
) {
    if (!IsWindow(hTabControl)) return -1;
    auto it = g_tab_controls.find(hTabControl);
    if (it == g_tab_controls.end()) return -1;

    EnableWindow(hTabControl, enabled ? TRUE : FALSE);
    return 0;
}


// ========== TabControl 外观函数 ==========

// 设置标签页固定尺寸
__declspec(dllexport) int __stdcall SetTabItemSize(HWND hTab, int width, int height) {
    auto it = g_tab_controls.find(hTab);
    if (it == g_tab_controls.end()) return -1;
    if (width <= 0 || height <= 0) return -1;

    TabControlState* state = it->second;
    state->tabWidth = width;
    state->tabHeight = height;

    // TCM_SETITEMSIZE: LOWORD=宽度, HIWORD=高度（无论水平还是垂直模式都一样传）
    SendMessage(hTab, TCM_SETITEMSIZE, 0, MAKELPARAM(width, height));
    InvalidateRect(hTab, NULL, TRUE);
    return 0;
}

// 设置标签页字体（fontName 为 UTF-8 编码）
__declspec(dllexport) int __stdcall SetTabFont(HWND hTab, const unsigned char* fontName, int fontNameLen, float fontSize) {
    auto it = g_tab_controls.find(hTab);
    if (it == g_tab_controls.end()) return -1;
    if (!fontName || fontNameLen <= 0) return -1;
    if (fontSize <= 0) return -1;

    TabControlState* state = it->second;
    state->fontName = Utf8ToWide(fontName, fontNameLen);
    state->fontSize = fontSize;

    InvalidateRect(hTab, NULL, TRUE);
    return 0;
}

// 设置标签页颜色（选中/未选中的背景色和文字色）
__declspec(dllexport) int __stdcall SetTabColors(HWND hTab, UINT32 selectedBg, UINT32 unselectedBg, UINT32 selectedText, UINT32 unselectedText) {
    auto it = g_tab_controls.find(hTab);
    if (it == g_tab_controls.end()) return -1;

    TabControlState* state = it->second;
    state->selectedBgColor = selectedBg;
    state->unselectedBgColor = unselectedBg;
    state->selectedTextColor = selectedText;
    state->unselectedTextColor = unselectedText;

    InvalidateRect(hTab, NULL, TRUE);
    return 0;
}

// 设置选中标签页底部指示条颜色
__declspec(dllexport) int __stdcall SetTabIndicatorColor(HWND hTab, UINT32 color) {
    auto it = g_tab_controls.find(hTab);
    if (it == g_tab_controls.end()) return -1;

    TabControlState* state = it->second;
    state->indicatorColor = color;

    InvalidateRect(hTab, NULL, TRUE);
    return 0;
}

// 设置标签页内边距
__declspec(dllexport) int __stdcall SetTabPadding(HWND hTab, int horizontal, int vertical) {
    auto it = g_tab_controls.find(hTab);
    if (it == g_tab_controls.end()) return -1;
    if (horizontal < 0 || vertical < 0) return -1;

    TabControlState* state = it->second;
    state->paddingH = horizontal;
    state->paddingV = vertical;

    InvalidateRect(hTab, NULL, TRUE);
    return 0;
}

// ========== TabControl 单个标签页控制函数 ==========

// 辅助函数：根据 pages 数组中的 visible 状态重建 Win32 Tab Control 的标签项
// pages 数组保存所有页面（包括隐藏的），Win32 Tab Control 只显示可见的
static void RebuildTabItems(TabControlState* state) {
    if (!state || !state->hTabControl) return;

    // 删除 Win32 Tab Control 中的所有标签项
    TabCtrl_DeleteAllItems(state->hTabControl);

    // 重新插入可见的标签项
    int visibleIdx = 0;
    for (size_t i = 0; i < state->pages.size(); i++) {
        if (state->pages[i].visible) {
            TCITEMW tci = {};
            tci.mask = TCIF_TEXT;
            tci.pszText = (LPWSTR)state->pages[i].title.c_str();
            TabCtrl_InsertItem(state->hTabControl, visibleIdx, &tci);
            visibleIdx++;
        }
    }

    // 恢复选中状态：将 currentIndex（pages 数组索引）映射到 Win32 可见索引
    if (state->currentIndex >= 0 && state->currentIndex < (int)state->pages.size()
        && state->pages[state->currentIndex].visible) {
        int visIdx = 0;
        for (int i = 0; i < state->currentIndex; i++) {
            if (state->pages[i].visible) visIdx++;
        }
        TabCtrl_SetCurSel(state->hTabControl, visIdx);
    }

    InvalidateRect(state->hTabControl, NULL, TRUE);
}

// 辅助函数：将 Win32 Tab Control 的可见索引转换为 pages 数组索引
static int VisibleIndexToPageIndex(TabControlState* state, int visibleIdx) {
    if (!state || visibleIdx < 0) return -1;
    int count = 0;
    for (size_t i = 0; i < state->pages.size(); i++) {
        if (state->pages[i].visible) {
            if (count == visibleIdx) return (int)i;
            count++;
        }
    }
    return -1;
}

// 启用/禁用单个标签页
__declspec(dllexport) int __stdcall EnableTabItem(HWND hTab, int index, int enabled) {
    auto it = g_tab_controls.find(hTab);
    if (it == g_tab_controls.end()) return -1;

    TabControlState* state = it->second;
    if (index < 0 || index >= (int)state->pages.size()) return -1;

    state->pages[index].enabled = (enabled != 0);
    InvalidateRect(hTab, NULL, TRUE);
    return 0;
}

// 获取单个标签页的启用状态
__declspec(dllexport) int __stdcall GetTabItemEnabled(HWND hTab, int index) {
    auto it = g_tab_controls.find(hTab);
    if (it == g_tab_controls.end()) return -1;

    TabControlState* state = it->second;
    if (index < 0 || index >= (int)state->pages.size()) return -1;

    return state->pages[index].enabled ? 1 : 0;
}

// 显示/隐藏单个标签页
__declspec(dllexport) int __stdcall ShowTabItem(HWND hTab, int index, int visible) {
    auto it = g_tab_controls.find(hTab);
    if (it == g_tab_controls.end()) return -1;

    TabControlState* state = it->second;
    if (index < 0 || index >= (int)state->pages.size()) return -1;

    bool newVisible = (visible != 0);
    if (state->pages[index].visible == newVisible) return 0;  // 状态未变

    state->pages[index].visible = newVisible;

    // 如果隐藏的是当前选中页，自动切换到下一个可见页
    if (!newVisible && index == state->currentIndex) {
        int newCurrent = -1;
        // 先向后找
        for (int i = index + 1; i < (int)state->pages.size(); i++) {
            if (state->pages[i].visible) { newCurrent = i; break; }
        }
        // 再向前找
        if (newCurrent < 0) {
            for (int i = index - 1; i >= 0; i--) {
                if (state->pages[i].visible) { newCurrent = i; break; }
            }
        }
        state->currentIndex = newCurrent;
        UpdateTabLayout(state);

        if (state->callback && newCurrent >= 0) {
            state->callback(hTab, newCurrent);
        }
    }

    // 隐藏标签页的内容窗口
    if (!newVisible && state->pages[index].hContentWindow && IsWindow(state->pages[index].hContentWindow)) {
        ShowWindow(state->pages[index].hContentWindow, SW_HIDE);
    }

    // 重建 Win32 Tab Control 的标签项
    RebuildTabItems(state);
    return 0;
}

// 设置标签页图标（PNG 字节数据）
__declspec(dllexport) int __stdcall SetTabItemIcon(HWND hTab, int index, const unsigned char* iconBytes, int iconLen) {
    auto it = g_tab_controls.find(hTab);
    if (it == g_tab_controls.end()) return -1;

    TabControlState* state = it->second;
    if (index < 0 || index >= (int)state->pages.size()) return -1;

    if (!iconBytes || iconLen <= 0) {
        // 清除图标
        state->pages[index].iconData.clear();
    } else {
        // 存储 PNG 字节数据
        state->pages[index].iconData.assign(iconBytes, iconBytes + iconLen);
    }

    InvalidateRect(hTab, NULL, TRUE);
    return 0;
}

// 辅助函数：将 ARGB 颜色转换为 COLORREF (BGR) 并设置内容窗口背景画刷
static void ApplyContentBgColor(HWND hContentWindow, UINT32 argbColor) {
    if (!hContentWindow || !IsWindow(hContentWindow)) return;

    // 更新 D2D 渲染的背景色（WindowState::client_bg_color）
    auto win_it = g_windows.find(hContentWindow);
    if (win_it != g_windows.end() && win_it->second) {
        win_it->second->client_bg_color = argbColor;
    }

    // 强制重绘（包括子控件）
    RedrawWindow(hContentWindow, NULL, NULL, RDW_INVALIDATE | RDW_ERASE | RDW_UPDATENOW | RDW_ALLCHILDREN);
}

// 设置指定标签页的内容区域背景色
__declspec(dllexport) int __stdcall SetTabContentBgColor(HWND hTab, int index, UINT32 color) {
    auto it = g_tab_controls.find(hTab);
    if (it == g_tab_controls.end()) return -1;

    TabControlState* state = it->second;
    if (index < 0 || index >= (int)state->pages.size()) return -1;

    state->pages[index].contentBgColor = color;
    ApplyContentBgColor(state->pages[index].hContentWindow, color);
    return 0;
}

// 设置所有标签页的内容区域背景色
__declspec(dllexport) int __stdcall SetTabContentBgColorAll(HWND hTab, UINT32 color) {
    auto it = g_tab_controls.find(hTab);
    if (it == g_tab_controls.end()) return -1;

    TabControlState* state = it->second;
    for (size_t i = 0; i < state->pages.size(); i++) {
        state->pages[i].contentBgColor = color;
        ApplyContentBgColor(state->pages[i].hContentWindow, color);
    }
    return 0;
}

// ========== TabControl 子类化过程（处理 TabControl 自身的鼠标事件）==========
// 处理右键点击、双击、拖拽、关闭按钮点击和悬停
LRESULT CALLBACK TabControlSubclassProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData) {
    auto it = g_tab_controls.find(hwnd);
    if (it == g_tab_controls.end()) {
        return DefSubclassProc(hwnd, msg, wparam, lparam);
    }

    TabControlState* state = it->second;

    switch (msg) {

    // ── 关闭按钮悬停跟踪 ──
    case WM_MOUSEMOVE: {
        if (state->closable) {
            POINT pt = { GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam) };
            TCHITTESTINFO hti = {};
            hti.pt = pt;
            int visibleIdx = TabCtrl_HitTest(hwnd, &hti);
            int pageIdx = VisibleIndexToPageIndex(state, visibleIdx);

            int newHoveredClose = -1;
            if (pageIdx >= 0 && pageIdx < (int)state->pages.size()) {
                // 获取该标签页的矩形区域
                RECT tabRect = {};
                TabCtrl_GetItemRect(hwnd, visibleIdx, &tabRect);
                int w = tabRect.right - tabRect.left;
                int h = tabRect.bottom - tabRect.top;
                float closeBtnSize = 16.0f;
                float closeBtnMargin = 4.0f;
                // 关闭按钮区域（相对于标签页矩形）
                float btnX = (float)tabRect.left + (float)w - closeBtnSize - closeBtnMargin;
                float btnY = (float)tabRect.top + ((float)h - closeBtnSize) / 2.0f;
                if (pt.x >= (int)(btnX - 2) && pt.x <= (int)(btnX + closeBtnSize + 2) &&
                    pt.y >= (int)(btnY - 2) && pt.y <= (int)(btnY + closeBtnSize + 2)) {
                    newHoveredClose = pageIdx;
                }
            }

            if (newHoveredClose != state->hoveredCloseTabIndex) {
                state->hoveredCloseTabIndex = newHoveredClose;
                InvalidateRect(hwnd, NULL, FALSE);
            }

            // 确保鼠标离开时能收到 WM_MOUSELEAVE
            TRACKMOUSEEVENT tme = {};
            tme.cbSize = sizeof(tme);
            tme.dwFlags = TME_LEAVE;
            tme.hwndTrack = hwnd;
            TrackMouseEvent(&tme);
        }

        // ── 拖拽移动 ──
        if (state->isDragging && state->draggable) {
            POINT pt = { GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam) };
            TCHITTESTINFO hti = {};
            hti.pt = pt;
            int visibleIdx = TabCtrl_HitTest(hwnd, &hti);
            int targetPageIdx = VisibleIndexToPageIndex(state, visibleIdx);
            if (targetPageIdx >= 0) {
                state->dragTargetIndex = targetPageIdx;
            }
            // 绘制拖拽指示线
            InvalidateRect(hwnd, NULL, FALSE);
        }
        break;
    }

    case WM_MOUSELEAVE: {
        if (state->hoveredCloseTabIndex != -1) {
            state->hoveredCloseTabIndex = -1;
            InvalidateRect(hwnd, NULL, FALSE);
        }
        break;
    }

    // ── 关闭按钮点击 + 拖拽开始 ──
    case WM_LBUTTONDOWN: {
        POINT pt = { GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam) };

        // 先检测是否点击了关闭按钮（优先于拖拽）
        bool clickedCloseBtn = false;
        if (state->closable) {
            TCHITTESTINFO hti = {};
            hti.pt = pt;
            int visibleIdx = TabCtrl_HitTest(hwnd, &hti);
            int pageIdx = VisibleIndexToPageIndex(state, visibleIdx);
            if (pageIdx >= 0 && pageIdx < (int)state->pages.size()) {
                RECT tabRect = {};
                TabCtrl_GetItemRect(hwnd, visibleIdx, &tabRect);
                int tw = tabRect.right - tabRect.left;
                int th = tabRect.bottom - tabRect.top;
                float closeBtnSize = 16.0f;
                float closeBtnMargin = 4.0f;
                float btnX = (float)tabRect.left + (float)tw - closeBtnSize - closeBtnMargin;
                float btnY = (float)tabRect.top + ((float)th - closeBtnSize) / 2.0f;
                if (pt.x >= (int)(btnX - 2) && pt.x <= (int)(btnX + closeBtnSize + 2) &&
                    pt.y >= (int)(btnY - 2) && pt.y <= (int)(btnY + closeBtnSize + 2)) {
                    clickedCloseBtn = true;
                    // 立即触发关闭回调
                    if (state->closeCallback) {
                        state->closeCallback(hwnd, pageIdx);
                    }
                    return 0;
                }
            }
        }

        // 拖拽开始（仅在未点击关闭按钮时）
        if (!clickedCloseBtn && state->draggable) {
            TCHITTESTINFO hti = {};
            hti.pt = pt;
            int visibleIdx = TabCtrl_HitTest(hwnd, &hti);
            int pageIdx = VisibleIndexToPageIndex(state, visibleIdx);
            if (pageIdx >= 0) {
                state->isDragging = true;
                state->dragStartIndex = pageIdx;
                state->dragTargetIndex = pageIdx;
                state->dragStartPoint = pt;
                SetCapture(hwnd);
            }
        }
        break;
    }

    case WM_LBUTTONUP: {
        POINT pt = { GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam) };

        // ── 拖拽完成 ──
        if (state->isDragging && state->draggable) {
            ReleaseCapture();
            state->isDragging = false;

            int fromIdx = state->dragStartIndex;
            int toIdx = state->dragTargetIndex;

            if (fromIdx >= 0 && toIdx >= 0 && fromIdx != toIdx &&
                fromIdx < (int)state->pages.size() && toIdx < (int)state->pages.size()) {
                // 移动 pages 数组中的元素
                TabPageInfo movedPage = state->pages[fromIdx];
                state->pages.erase(state->pages.begin() + fromIdx);
                if (toIdx > fromIdx) toIdx--;  // 删除后索引调整
                state->pages.insert(state->pages.begin() + toIdx, movedPage);

                // 更新所有页的 index 字段
                for (int i = 0; i < (int)state->pages.size(); i++) {
                    state->pages[i].index = i;
                }

                // 保持拖拽页为选中状态
                state->currentIndex = toIdx;

                // 重建 Win32 Tab Control 的标签
                TabCtrl_DeleteAllItems(hwnd);
                int visIdx = 0;
                for (size_t i = 0; i < state->pages.size(); i++) {
                    if (state->pages[i].visible) {
                        std::string utf8Title;
                        int len = WideCharToMultiByte(CP_UTF8, 0, state->pages[i].title.c_str(), -1, nullptr, 0, nullptr, nullptr);
                        if (len > 0) {
                            utf8Title.resize(len - 1);
                            WideCharToMultiByte(CP_UTF8, 0, state->pages[i].title.c_str(), -1, &utf8Title[0], len, nullptr, nullptr);
                        }
                        TCITEMW tci = {};
                        tci.mask = TCIF_TEXT;
                        tci.pszText = (LPWSTR)state->pages[i].title.c_str();
                        TabCtrl_InsertItem(hwnd, visIdx, &tci);
                        visIdx++;
                    }
                }

                // 设置选中状态
                if (state->currentIndex >= 0 && state->currentIndex < (int)state->pages.size()
                    && state->pages[state->currentIndex].visible) {
                    int selVisIdx = 0;
                    for (int i = 0; i < state->currentIndex; i++) {
                        if (state->pages[i].visible) selVisIdx++;
                    }
                    TabCtrl_SetCurSel(hwnd, selVisIdx);
                }

                UpdateTabLayout(state);
            }

            state->dragStartIndex = -1;
            state->dragTargetIndex = -1;
            InvalidateRect(hwnd, NULL, TRUE);
            return 0;
        }

        // ── 关闭按钮点击检测 ──
        if (state->closable) {
            TCHITTESTINFO hti = {};
            hti.pt = pt;
            int visibleIdx = TabCtrl_HitTest(hwnd, &hti);
            int pageIdx = VisibleIndexToPageIndex(state, visibleIdx);

            if (pageIdx >= 0 && pageIdx < (int)state->pages.size()) {
                RECT tabRect = {};
                TabCtrl_GetItemRect(hwnd, visibleIdx, &tabRect);
                int w = tabRect.right - tabRect.left;
                int h = tabRect.bottom - tabRect.top;
                float closeBtnSize = 16.0f;
                float closeBtnMargin = 4.0f;
                float btnX = (float)tabRect.left + (float)w - closeBtnSize - closeBtnMargin;
                float btnY = (float)tabRect.top + ((float)h - closeBtnSize) / 2.0f;

                if (pt.x >= (int)(btnX - 2) && pt.x <= (int)(btnX + closeBtnSize + 2) &&
                    pt.y >= (int)(btnY - 2) && pt.y <= (int)(btnY + closeBtnSize + 2)) {
                    // 点击了关闭按钮，触发回调
                    if (state->closeCallback) {
                        state->closeCallback(hwnd, pageIdx);
                    }
                    return 0;  // 不传递给默认处理，避免切换标签
                }
            }
        }
        break;
    }

    // ── 右键点击 ──
    case WM_RBUTTONUP: {
        if (state->rightClickCallback) {
            POINT pt = { GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam) };
            TCHITTESTINFO hti = {};
            hti.pt = pt;
            int visibleIdx = TabCtrl_HitTest(hwnd, &hti);
            int pageIdx = VisibleIndexToPageIndex(state, visibleIdx);

            if (pageIdx >= 0) {
                // 转换为屏幕坐标
                POINT screenPt = pt;
                ClientToScreen(hwnd, &screenPt);
                state->rightClickCallback(hwnd, pageIdx, screenPt.x, screenPt.y);
                return 0;
            }
        }
        break;
    }

    // ── 双击 ──
    case WM_LBUTTONDBLCLK: {
        if (state->dblClickCallback) {
            POINT pt = { GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam) };
            TCHITTESTINFO hti = {};
            hti.pt = pt;
            int visibleIdx = TabCtrl_HitTest(hwnd, &hti);
            int pageIdx = VisibleIndexToPageIndex(state, visibleIdx);

            if (pageIdx >= 0) {
                state->dblClickCallback(hwnd, pageIdx);
                return 0;
            }
        }
        break;
    }

    case WM_PAINT: {
        // 拖拽时绘制插入位置指示线
        if (state->isDragging && state->draggable && state->dragTargetIndex >= 0) {
            // 先让默认绘制完成
            LRESULT result = DefSubclassProc(hwnd, msg, wparam, lparam);

            // 在默认绘制之上绘制指示线
            HDC hdc = GetDC(hwnd);
            if (hdc) {
                // 找到目标位置的可见索引
                int targetVisIdx = 0;
                for (int i = 0; i < state->dragTargetIndex && i < (int)state->pages.size(); i++) {
                    if (state->pages[i].visible) targetVisIdx++;
                }

                RECT tabRect = {};
                int tabCount = TabCtrl_GetItemCount(hwnd);
                int lineX = 0;
                if (targetVisIdx < tabCount) {
                    TabCtrl_GetItemRect(hwnd, targetVisIdx, &tabRect);
                    lineX = tabRect.left;
                } else if (tabCount > 0) {
                    TabCtrl_GetItemRect(hwnd, tabCount - 1, &tabRect);
                    lineX = tabRect.right;
                }

                // 绘制 2px 宽的蓝色指示线
                HPEN hPen = CreatePen(PS_SOLID, 2, RGB(64, 158, 255));  // #409EFF
                HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);
                MoveToEx(hdc, lineX, tabRect.top, NULL);
                LineTo(hdc, lineX, tabRect.bottom);
                SelectObject(hdc, hOldPen);
                DeleteObject(hPen);

                ReleaseDC(hwnd, hdc);
            }
            return result;
        }
        break;
    }

    case WM_NCDESTROY: {
        RemoveWindowSubclass(hwnd, TabControlSubclassProc, uIdSubclass);
        break;
    }
    }

    return DefSubclassProc(hwnd, msg, wparam, lparam);
}

// ========== TabControl 交互增强函数 ==========

// 设置标签页是否显示关闭按钮
__declspec(dllexport) int __stdcall SetTabClosable(HWND hTab, int closable) {
    auto it = g_tab_controls.find(hTab);
    if (it == g_tab_controls.end()) return -1;

    TabControlState* state = it->second;
    state->closable = (closable != 0);
    InvalidateRect(hTab, NULL, TRUE);
    return 0;
}

// 设置标签页关闭回调
__declspec(dllexport) int __stdcall SetTabCloseCallback(HWND hTab, TAB_CLOSE_CALLBACK callback) {
    auto it = g_tab_controls.find(hTab);
    if (it == g_tab_controls.end()) return -1;

    TabControlState* state = it->second;
    state->closeCallback = callback;
    return 0;
}

// 设置标签页右键点击回调
__declspec(dllexport) int __stdcall SetTabRightClickCallback(HWND hTab, TAB_RIGHTCLICK_CALLBACK callback) {
    auto it = g_tab_controls.find(hTab);
    if (it == g_tab_controls.end()) return -1;

    TabControlState* state = it->second;
    state->rightClickCallback = callback;
    return 0;
}

// 设置标签页是否可拖拽排序
__declspec(dllexport) int __stdcall SetTabDraggable(HWND hTab, int draggable) {
    auto it = g_tab_controls.find(hTab);
    if (it == g_tab_controls.end()) return -1;

    TabControlState* state = it->second;
    state->draggable = (draggable != 0);
    return 0;
}

// 设置标签页双击回调
__declspec(dllexport) int __stdcall SetTabDoubleClickCallback(HWND hTab, TAB_DBLCLICK_CALLBACK callback) {
    auto it = g_tab_controls.find(hTab);
    if (it == g_tab_controls.end()) return -1;

    TabControlState* state = it->second;
    state->dblClickCallback = callback;
    return 0;
}

// ========== 布局与位置函数 ==========

// 设置标签栏位置（0=上, 1=下, 2=左, 3=右）
__declspec(dllexport) int __stdcall SetTabPosition(HWND hTab, int position) {
    auto it = g_tab_controls.find(hTab);
    if (it == g_tab_controls.end()) return -1;

    if (position < 0 || position > 3) return -1;

    TabControlState* state = it->second;
    state->tabPosition = position;

    // 获取当前窗口样式
    LONG style = GetWindowLong(hTab, GWL_STYLE);

    // 清除所有位置相关样式
    style &= ~(TCS_BOTTOM | TCS_VERTICAL | TCS_RIGHT);

    // 根据位置设置样式
    switch (position) {
    case 0: // 上（默认，无需额外样式）
        break;
    case 1: // 下
        style |= TCS_BOTTOM;
        break;
    case 2: // 左
        // TCS_VERTICAL 必须配合 TCS_MULTILINE 才能正常显示垂直标签
        style |= TCS_VERTICAL | TCS_MULTILINE;
        break;
    case 3: // 右
        style |= TCS_VERTICAL | TCS_RIGHT | TCS_MULTILINE;
        break;
    }

    SetWindowLong(hTab, GWL_STYLE, style);

    // 重新设置标签尺寸（确保垂直/水平模式下尺寸正确）
    SendMessage(hTab, TCM_SETITEMSIZE, 0, MAKELPARAM(state->tabWidth, state->tabHeight));

    // 重新计算内容区域布局
    UpdateTabLayout(state);
    InvalidateRect(hTab, NULL, TRUE);
    return 0;
}

// 设置标签对齐方式（0=左对齐, 1=居中, 2=右对齐）
__declspec(dllexport) int __stdcall SetTabAlignment(HWND hTab, int align) {
    auto it = g_tab_controls.find(hTab);
    if (it == g_tab_controls.end()) return -1;

    if (align < 0 || align > 2) return -1;

    TabControlState* state = it->second;
    state->tabAlignment = align;

    // 触发重绘，实际对齐效果在 WM_DRAWITEM 中根据 tabAlignment 调整
    InvalidateRect(hTab, NULL, TRUE);
    return 0;
}

// 设置标签栏是否可滚动（1=可滚动/单行, 0=不可滚动/多行）
__declspec(dllexport) int __stdcall SetTabScrollable(HWND hTab, int scrollable) {
    auto it = g_tab_controls.find(hTab);
    if (it == g_tab_controls.end()) return -1;

    TabControlState* state = it->second;
    state->scrollable = (scrollable != 0);
    state->scrollOffset = 0;

    // 获取当前窗口样式
    LONG style = GetWindowLong(hTab, GWL_STYLE);

    if (state->scrollable) {
        // 可滚动模式：移除 TCS_MULTILINE，Win32 Tab Control 自带单行滚动箭头
        style &= ~TCS_MULTILINE;
    } else {
        // 不可滚动模式：添加 TCS_MULTILINE，使用多行标签行为
        style |= TCS_MULTILINE;
    }

    SetWindowLong(hTab, GWL_STYLE, style);

    // 重新计算布局
    UpdateTabLayout(state);
    InvalidateRect(hTab, NULL, TRUE);
    return 0;
}


// ========== 批量操作函数 ==========

// 清空所有标签页
__declspec(dllexport) int __stdcall RemoveAllTabs(HWND hTab) {
    auto it = g_tab_controls.find(hTab);
    if (it == g_tab_controls.end()) return -1;

    TabControlState* state = it->second;

    // 遍历所有页面，销毁内容窗口，清理关联的 WindowState 和 D2D 渲染目标
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

    // 清空 pages 数组，设置 currentIndex = -1
    state->pages.clear();
    state->currentIndex = -1;

    // 删除 Win32 Tab Control 中的所有标签项并重绘
    TabCtrl_DeleteAllItems(hTab);
    InvalidateRect(hTab, NULL, TRUE);
    return 0;
}

// 在指定位置插入标签页
__declspec(dllexport) int __stdcall InsertTabItem(HWND hTab, int index, const unsigned char* title, int titleLen, HWND hContent) {
    auto it = g_tab_controls.find(hTab);
    if (it == g_tab_controls.end()) return -1;

    TabControlState* state = it->second;

    if (index < 0) return -1;

    // index 超过当前数量时追加到末尾
    int count = (int)state->pages.size();
    if (index > count) index = count;

    // 转换标题
    std::wstring wtitle = Utf8ToWide(title, titleLen);

    // 如果没有提供内容窗口，则创建一个
    if (!hContent || !IsWindow(hContent)) {
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

        static bool tab_content_class_registered = false;
        if (!tab_content_class_registered) {
            WNDCLASSW wc = {};
            wc.lpfnWndProc = TabContentWindowProc;
            wc.hInstance = GetModuleHandle(nullptr);
            wc.lpszClassName = TAB_CONTENT_D2D_CLASS;
            wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
            wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
            RegisterClassW(&wc);
            tab_content_class_registered = true;
        }

        hContent = CreateWindowExW(
            0,
            TAB_CONTENT_D2D_CLASS,
            L"",
            WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
            0, 0, 0, 0,
            state->hTabControl,
            nullptr,
            GetModuleHandle(nullptr),
            nullptr
        );
        if (!hContent) return -1;
    }

    // 创建新的 TabPageInfo
    TabPageInfo pageInfo;
    pageInfo.index = index;
    pageInfo.title = wtitle;
    pageInfo.hContentWindow = hContent;
    pageInfo.visible = true;
    pageInfo.enabled = true;
    pageInfo.contentBgColor = 0xFFFFFFFF;

    // 在 pages 数组中插入
    state->pages.insert(state->pages.begin() + index, pageInfo);

    // 更新后续标签页的 index 字段
    for (int i = index + 1; i < (int)state->pages.size(); i++) {
        state->pages[i].index = i;
    }

    // 如果插入位置 <= currentIndex，则 currentIndex++
    if (state->currentIndex >= 0 && index <= state->currentIndex) {
        state->currentIndex++;
    }

    // 重建 Win32 Tab Control 的标签项（处理 visible 状态）
    RebuildTabItems(state);

    // 如果是第一个标签页，自动选中
    if (count == 0) {
        state->currentIndex = 0;
        TabCtrl_SetCurSel(hTab, 0);
        UpdateTabLayout(state);
        if (state->callback) {
            state->callback(hTab, 0);
        }
    } else {
        UpdateTabLayout(state);
    }

    return index;
}

// 移动标签页位置
__declspec(dllexport) int __stdcall MoveTabItem(HWND hTab, int fromIndex, int toIndex) {
    auto it = g_tab_controls.find(hTab);
    if (it == g_tab_controls.end()) return -1;

    TabControlState* state = it->second;
    int count = (int)state->pages.size();

    if (fromIndex < 0 || fromIndex >= count) return -1;
    if (toIndex < 0 || toIndex >= count) return -1;
    if (fromIndex == toIndex) return 0;

    // 保存被移动的页面
    TabPageInfo movedPage = state->pages[fromIndex];

    // 从原位置移除
    state->pages.erase(state->pages.begin() + fromIndex);

    // 插入到目标位置
    state->pages.insert(state->pages.begin() + toIndex, movedPage);

    // 更新所有页面的 index 字段
    for (int i = 0; i < (int)state->pages.size(); i++) {
        state->pages[i].index = i;
    }

    // 更新 currentIndex
    if (state->currentIndex == fromIndex) {
        // 被移动的标签页是当前选中的
        state->currentIndex = toIndex;
    } else if (fromIndex < toIndex) {
        // 向后移动：fromIndex+1 到 toIndex 之间的元素前移一位
        if (state->currentIndex > fromIndex && state->currentIndex <= toIndex) {
            state->currentIndex--;
        }
    } else {
        // 向前移动：toIndex 到 fromIndex-1 之间的元素后移一位
        if (state->currentIndex >= toIndex && state->currentIndex < fromIndex) {
            state->currentIndex++;
        }
    }

    // 重建 Win32 Tab Control 的标签项
    RebuildTabItems(state);
    UpdateTabLayout(state);

    return 0;
}

// 根据标题查找标签页索引
__declspec(dllexport) int __stdcall GetTabIndexByTitle(HWND hTab, const unsigned char* titleBytes, int titleLen) {
    auto it = g_tab_controls.find(hTab);
    if (it == g_tab_controls.end()) return -1;

    if (!titleBytes) return -1;

    TabControlState* state = it->second;

    // 将 UTF-8 titleBytes 转换为 std::wstring
    std::wstring searchTitle = Utf8ToWide(titleBytes, titleLen);

    // 遍历 pages 数组精确匹配（区分大小写）
    for (size_t i = 0; i < state->pages.size(); i++) {
        if (state->pages[i].title == searchTitle) {
            return (int)i;
        }
    }

    return -1;
}

// 获取整个 TabControl 的启用状态
__declspec(dllexport) int __stdcall GetTabEnabled(HWND hTab) {
    auto it = g_tab_controls.find(hTab);
    if (it == g_tab_controls.end()) return -1;

    return IsWindowEnabled(hTab) ? 1 : 0;
}

// 判断指定标签页是否为当前选中
__declspec(dllexport) int __stdcall IsTabItemSelected(HWND hTab, int index) {
    auto it = g_tab_controls.find(hTab);
    if (it == g_tab_controls.end()) return -1;

    TabControlState* state = it->second;
    if (index < 0 || index >= (int)state->pages.size()) return -1;

    return (state->currentIndex == index) ? 1 : 0;
}

LRESULT CALLBACK GroupBoxProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData) {
    auto it = g_groupboxes.find(hwnd);
    if (it == g_groupboxes.end()) {
        return DefSubclassProc(hwnd, msg, wparam, lparam);
    }
    
    GroupBoxState* state = it->second;

    // 通用事件处理
    HandleCommonEvents(hwnd, msg, wparam, lparam, &state->events);

    switch (msg) {
        case WM_NCHITTEST: {
            // 将屏幕坐标转换为客户区坐标
            POINT pt = { GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam) };
            ScreenToClient(hwnd, &pt);
            
            // 计算边框和标题区域
            // 标题高度约 25px，边框宽度约 10px
            int title_height = 25;
            int border_width = 10;
            
            RECT client_rect;
            GetClientRect(hwnd, &client_rect);
            
            // 如果点击在边框或标题区域，返回 HTCLIENT（可以接收消息）
            bool in_border_or_title = 
                pt.y < title_height ||  // 标题区域
                pt.x < border_width ||  // 左边框
                pt.x > client_rect.right - border_width ||  // 右边框
                pt.y > client_rect.bottom - border_width;  // 下边框
            
            if (in_border_or_title) {
                return HTCLIENT;
            }
            
            // 内容区域：对所有点击返回 HTTRANSPARENT，让主窗口上的按钮能被点击
            return HTTRANSPARENT;
        }

        case WM_SHOWWINDOW:
            WriteLog("GroupBoxProc: WM_SHOWWINDOW hwnd=%p, show=%d, status=%d, visible=%d",
                hwnd,
                wparam ? 1 : 0,
                (int)lparam,
                state->visible ? 1 : 0);
            break;

        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            WriteLog("GroupBoxProc: WM_PAINT begin hwnd=%p, state=%p, visible=%d", hwnd, state, state->visible ? 1 : 0);

            // 创建 D2D1 渲染目标
            RECT rc;
            GetClientRect(hwnd, &rc);
            
            ID2D1HwndRenderTarget* render_target = nullptr;
            D2D1_RENDER_TARGET_PROPERTIES props = D2D1::RenderTargetProperties();
            D2D1_HWND_RENDER_TARGET_PROPERTIES hwndProps = D2D1::HwndRenderTargetProperties(
                hwnd,
                D2D1::SizeU(rc.right - rc.left, rc.bottom - rc.top)
            );
            
            HRESULT hr = g_d2d_factory->CreateHwndRenderTarget(props, hwndProps, &render_target);
            WriteLog("GroupBoxProc: CreateHwndRenderTarget hwnd=%p, hr=0x%08X, render_target=%p, size=%ldx%ld",
                hwnd,
                (unsigned int)hr,
                render_target,
                (long)(rc.right - rc.left),
                (long)(rc.bottom - rc.top));
            
            if (SUCCEEDED(hr) && render_target) {
                render_target->BeginDraw();
                render_target->Clear(D2D1::ColorF(D2D1::ColorF::White, 0.0f));  // 透明背景
                
                DrawGroupBox(render_target, g_dwrite_factory, state);
                
                HRESULT end_hr = render_target->EndDraw();
                WriteLog("GroupBoxProc: EndDraw hwnd=%p, hr=0x%08X", hwnd, (unsigned int)end_hr);
                render_target->Release();
            } else {
                WriteLog("GroupBoxProc: skipping draw because render target creation failed, hwnd=%p", hwnd);
            }
            
            EndPaint(hwnd, &ps);
            WriteLog("GroupBoxProc: WM_PAINT end hwnd=%p", hwnd);
            return 0;
        }
        
        case WM_ERASEBKGND:
            return 1;  // 防止闪烁
        
        case WM_NCDESTROY: {
            // 清理资源
            WriteLog("GroupBoxProc: WM_NCDESTROY hwnd=%p, state=%p", hwnd, state);
            RemoveWindowSubclass(hwnd, GroupBoxProc, 0);
            
            // 从全局map中移除
            auto it = g_groupboxes.find(hwnd);
            if (it != g_groupboxes.end()) {
                delete it->second;
                g_groupboxes.erase(it);
            }
            break;
        }
    }
    
    return DefSubclassProc(hwnd, msg, wparam, lparam);
}

// 绘制分组框
void DrawGroupBox(ID2D1HwndRenderTarget* rt, IDWriteFactory* factory, GroupBoxState* state) {
    if (!rt || !factory || !state) return;
    
    // 创建画刷
    ID2D1SolidColorBrush* border_brush = nullptr;
    ID2D1SolidColorBrush* title_brush = nullptr;
    // 不再需要背景画刷，分组框应该完全透明
    
    rt->CreateSolidColorBrush(ColorFromUInt32(state->border_color), &border_brush);
    rt->CreateSolidColorBrush(ColorFromUInt32(state->title_color), &title_brush);
    
    if (!border_brush || !title_brush) {
        if (border_brush) border_brush->Release();
        if (title_brush) title_brush->Release();
        return;
    }
    
    // 创建文本格式
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
    
    if (!text_format) {
        border_brush->Release();
        title_brush->Release();
        return;
    }
    
    text_format->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
    text_format->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
    
    // 测量标题文本尺寸
    IDWriteTextLayout* text_layout = nullptr;
    factory->CreateTextLayout(
        state->title.c_str(),
        (UINT32)state->title.length(),
        text_format,
        (float)state->width,
        (float)state->height,
        &text_layout
    );
    
    DWRITE_TEXT_METRICS text_metrics;
    float title_width = 0;
    float title_height = 0;
    if (text_layout) {
        text_layout->GetMetrics(&text_metrics);
        title_width = text_metrics.width;
        title_height = text_metrics.height;
        text_layout->Release();
    }
    
    // 不绘制背景，保持完全透明，这样主窗口上的按钮就能显示出来
    
    // 绘制边框（带缺口以容纳标题）
    float gap_start = 10.0f;
    float gap_end = gap_start + title_width + 10.0f;
    float border_y = title_height / 2;
    
    // 上边框（左段）
    rt->DrawLine(
        D2D1::Point2F(4.0f, border_y),
        D2D1::Point2F(gap_start, border_y),
        border_brush,
        1.0f
    );
    
    // 上边框（右段）
    rt->DrawLine(
        D2D1::Point2F(gap_end, border_y),
        D2D1::Point2F((float)state->width - 4.0f, border_y),
        border_brush,
        1.0f
    );
    
    // 左边框
    rt->DrawLine(
        D2D1::Point2F(0.5f, border_y + 4.0f),
        D2D1::Point2F(0.5f, (float)state->height - 4.0f),
        border_brush,
        1.0f
    );
    
    // 右边框
    rt->DrawLine(
        D2D1::Point2F((float)state->width - 0.5f, border_y + 4.0f),
        D2D1::Point2F((float)state->width - 0.5f, (float)state->height - 4.0f),
        border_brush,
        1.0f
    );
    
    // 下边框
    rt->DrawLine(
        D2D1::Point2F(4.0f, (float)state->height - 0.5f),
        D2D1::Point2F((float)state->width - 4.0f, (float)state->height - 0.5f),
        border_brush,
        1.0f
    );
    
    // 绘制圆角
    // 左上角
    D2D1_ELLIPSE corner_tl = D2D1::Ellipse(D2D1::Point2F(4.0f, border_y + 4.0f), 4.0f, 4.0f);
    rt->DrawEllipse(corner_tl, border_brush, 1.0f);
    
    // 右上角
    D2D1_ELLIPSE corner_tr = D2D1::Ellipse(D2D1::Point2F((float)state->width - 4.0f, border_y + 4.0f), 4.0f, 4.0f);
    rt->DrawEllipse(corner_tr, border_brush, 1.0f);
    
    // 左下角
    D2D1_ELLIPSE corner_bl = D2D1::Ellipse(D2D1::Point2F(4.0f, (float)state->height - 4.0f), 4.0f, 4.0f);
    rt->DrawEllipse(corner_bl, border_brush, 1.0f);
    
    // 右下角
    D2D1_ELLIPSE corner_br = D2D1::Ellipse(D2D1::Point2F((float)state->width - 4.0f, (float)state->height - 4.0f), 4.0f, 4.0f);
    rt->DrawEllipse(corner_br, border_brush, 1.0f);
    
    // 绘制标题背景（与设计器一致：标题处“切断”边框，背景与窗口一致）
    ID2D1SolidColorBrush* bg_brush = nullptr;
    rt->CreateSolidColorBrush(ColorFromUInt32(ResolveThemeColor(state->bg_color)), &bg_brush);
    if (bg_brush) {
        D2D1_RECT_F title_bg = D2D1::RectF(gap_start, 0, gap_end, title_height + 2.0f);
        rt->FillRectangle(title_bg, bg_brush);
        bg_brush->Release();
    }
    
    // 绘制标题文本
    D2D1_RECT_F title_rect = D2D1::RectF(
        gap_start + 5.0f,
        0,
        gap_end - 5.0f,
        title_height
    );
    
    rt->DrawText(
        state->title.c_str(),
        (UINT32)state->title.length(),
        text_format,
        title_rect,
        title_brush
    );
    
    // 清理资源
    text_format->Release();
    border_brush->Release();
    title_brush->Release();
}


// ========================================
// D2D自定义绘制编辑框实现（支持彩色emoji）
// ========================================

// 窗口类名
const wchar_t* D2D_EDITBOX_CLASS = L"D2DEditBoxClass";

// 光标闪烁定时器ID
#define CURSOR_TIMER_ID 1001
#define CURSOR_BLINK_INTERVAL 500  // 500ms闪烁间隔

// 辅助函数：获取字符在文本中的像素位置
float GetCharPositionX(IDWriteTextLayout* layout, int char_index) {
    if (!layout || char_index < 0) return 0.0f;
    
    DWRITE_HIT_TEST_METRICS metrics;
    float x, y;
    HRESULT hr = layout->HitTestTextPosition(
        char_index,
        false,  // isTrailingHit
        &x,
        &y,
        &metrics
    );
    
    return SUCCEEDED(hr) ? x : 0.0f;
}

// 辅助函数：从点击位置获取字符索引
int GetCharIndexFromPoint(IDWriteTextLayout* layout, float click_x, float click_y) {
    if (!layout) return 0;
    
    BOOL isTrailingHit, isInside;
    DWRITE_HIT_TEST_METRICS metrics;
    HRESULT hr = layout->HitTestPoint(
        click_x,
        click_y,
        &isTrailingHit,
        &isInside,
        &metrics
    );
    
    if (FAILED(hr)) return 0;
    
    int index = metrics.textPosition;
    if (isTrailingHit && index < (int)metrics.length) {
        index++;
    }
    
    return index;
}

// 绘制D2D编辑框
void DrawD2DEditBox(D2DEditBoxState* state) {
    if (!state || !state->render_target || !state->dwrite_factory) return;
    
    ID2D1HwndRenderTarget* rt = state->render_target;
    
    rt->BeginDraw();
    
    // 1. 绘制背景
    ID2D1SolidColorBrush* bg_brush = nullptr;
    rt->CreateSolidColorBrush(ColorFromUInt32(state->bg_color), &bg_brush);
    rt->FillRectangle(
        D2D1::RectF(0, 0, (float)state->width, (float)state->height),
        bg_brush
    );
    bg_brush->Release();
    
    // 2. 绘制边框
    if (state->has_border) {
        ID2D1SolidColorBrush* border_brush = nullptr;
        UINT32 border_color = state->has_focus ? ThemeColor_Primary() : state->border_color;
        rt->CreateSolidColorBrush(ColorFromUInt32(border_color), &border_brush);
        rt->DrawRectangle(
            D2D1::RectF(0, 0, (float)state->width, (float)state->height),
            border_brush,
            state->has_focus ? 2.0f : 1.0f
        );
        border_brush->Release();
    }
    
    // 3. 准备文本
    std::wstring display_text = state->text;
    if (state->password && !display_text.empty()) {
        display_text = std::wstring(display_text.length(), L'*');
    }
    
    // 添加输入法组合文本
    if (state->is_composing && !state->composition_text.empty()) {
        display_text.insert(state->cursor_pos, state->composition_text);
    }
    
    // 4. 创建文本布局
    IDWriteTextFormat* text_format = nullptr;
    state->dwrite_factory->CreateTextFormat(
        state->font.font_name.c_str(),
        nullptr,
        state->font.bold ? DWRITE_FONT_WEIGHT_BOLD : DWRITE_FONT_WEIGHT_NORMAL,
        state->font.italic ? DWRITE_FONT_STYLE_ITALIC : DWRITE_FONT_STYLE_NORMAL,
        DWRITE_FONT_STRETCH_NORMAL,
        (float)state->font.font_size,
        L"zh-CN",
        &text_format
    );
    
    // 设置对齐方式
    switch (state->alignment) {
        case ALIGN_LEFT:
            text_format->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
            break;
        case ALIGN_CENTER:
            text_format->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
            break;
        case ALIGN_RIGHT:
            text_format->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_TRAILING);
            break;
    }
    
    if (state->vertical_center && !state->multiline) {
        text_format->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
    } else {
        text_format->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
    }
    
    IDWriteTextLayout* layout = nullptr;
    state->dwrite_factory->CreateTextLayout(
        display_text.c_str(),
        (UINT32)display_text.length(),
        text_format,
        (float)state->width - 8.0f,  // 左右各留4px边距
        (float)state->height - 4.0f,
        &layout
    );
    
    // 设置下划线
    if (state->font.underline && !display_text.empty()) {
        DWRITE_TEXT_RANGE range = { 0, (UINT32)display_text.length() };
        layout->SetUnderline(TRUE, range);
    }
    
    // 5. 绘制选择背景
    if (state->selection_start >= 0 && state->selection_end > state->selection_start) {
        int sel_start = min(state->selection_start, state->selection_end);
        int sel_end = max(state->selection_start, state->selection_end);
        
        if (sel_start < (int)display_text.length()) {
            sel_end = min(sel_end, (int)display_text.length());
            
            float start_x = GetCharPositionX(layout, sel_start);
            float end_x = GetCharPositionX(layout, sel_end);
            
            ID2D1SolidColorBrush* sel_brush = nullptr;
            rt->CreateSolidColorBrush(ColorFromUInt32(state->selection_color), &sel_brush);
            
            float text_y = state->vertical_center && !state->multiline ? 
                (state->height - state->font.font_size) / 2.0f : 2.0f;
            
            rt->FillRectangle(
                D2D1::RectF(
                    4.0f + start_x - state->scroll_offset_x,
                    text_y,
                    4.0f + end_x - state->scroll_offset_x,
                    text_y + state->font.font_size + 4.0f
                ),
                sel_brush
            );
            sel_brush->Release();
        }
    }
    
    // 6. 绘制文本（支持彩色emoji）
    ID2D1SolidColorBrush* text_brush = nullptr;
    rt->CreateSolidColorBrush(ColorFromUInt32(state->fg_color), &text_brush);
    
    rt->DrawTextLayout(
        D2D1::Point2F(4.0f - state->scroll_offset_x, 2.0f - state->scroll_offset_y),
        layout,
        text_brush,
        D2D1_DRAW_TEXT_OPTIONS_ENABLE_COLOR_FONT  // ← 关键：启用彩色emoji！
    );
    
    text_brush->Release();
    
    // 7. 绘制光标
    if (state->has_focus && state->cursor_visible && state->selection_start < 0) {
        float cursor_x = GetCharPositionX(layout, state->cursor_pos);
        
        ID2D1SolidColorBrush* cursor_brush = nullptr;
        rt->CreateSolidColorBrush(ColorFromUInt32(state->fg_color), &cursor_brush);
        
        float text_y = state->vertical_center && !state->multiline ? 
            (state->height - state->font.font_size) / 2.0f : 2.0f;
        
        rt->DrawLine(
            D2D1::Point2F(4.0f + cursor_x - state->scroll_offset_x, text_y),
            D2D1::Point2F(4.0f + cursor_x - state->scroll_offset_x, text_y + state->font.font_size + 4.0f),
            cursor_brush,
            1.5f
        );
        
        cursor_brush->Release();
    }
    
    layout->Release();
    text_format->Release();
    
    rt->EndDraw();
}

// 插入文本
void InsertTextAtCursor(D2DEditBoxState* state, const std::wstring& text) {
    if (!state || state->readonly) return;
    
    // 如果有选择，先删除选择的文本
    if (state->selection_start >= 0) {
        int sel_start = min(state->selection_start, state->selection_end);
        int sel_end = max(state->selection_start, state->selection_end);
        state->text.erase(sel_start, sel_end - sel_start);
        state->cursor_pos = sel_start;
        state->selection_start = -1;
        state->selection_end = -1;
    }
    
    // 插入新文本
    state->text.insert(state->cursor_pos, text);
    state->cursor_pos += (int)text.length();
    
    InvalidateRect(state->hwnd, NULL, FALSE);
}

// 删除字符
void DeleteCharAtCursor(D2DEditBoxState* state, bool forward) {
    if (!state || state->readonly) return;
    
    // 如果有选择，删除选择的文本
    if (state->selection_start >= 0) {
        int sel_start = min(state->selection_start, state->selection_end);
        int sel_end = max(state->selection_start, state->selection_end);
        state->text.erase(sel_start, sel_end - sel_start);
        state->cursor_pos = sel_start;
        state->selection_start = -1;
        state->selection_end = -1;
        InvalidateRect(state->hwnd, NULL, FALSE);
        return;
    }
    
    // 删除单个字符
    if (forward) {
        // Delete键：删除光标后的字符
        if (state->cursor_pos < (int)state->text.length()) {
            state->text.erase(state->cursor_pos, 1);
        }
    } else {
        // Backspace键：删除光标前的字符
        if (state->cursor_pos > 0) {
            state->text.erase(state->cursor_pos - 1, 1);
            state->cursor_pos--;
        }
    }
    
    InvalidateRect(state->hwnd, NULL, FALSE);
}

// 移动光标
void MoveCursor(D2DEditBoxState* state, int delta, bool select) {
    if (!state) return;
    
    int new_pos = state->cursor_pos + delta;
    new_pos = max(0, min(new_pos, (int)state->text.length()));
    
    if (select) {
        // Shift+方向键：选择文本
        if (state->selection_start < 0) {
            state->selection_start = state->cursor_pos;
        }
        state->cursor_pos = new_pos;
        state->selection_end = new_pos;
    } else {
        // 普通移动：清除选择
        state->cursor_pos = new_pos;
        state->selection_start = -1;
        state->selection_end = -1;
    }
    
    InvalidateRect(state->hwnd, NULL, FALSE);
}

// 复制到剪贴板
void CopyToClipboard(D2DEditBoxState* state) {
    if (!state || state->selection_start < 0) return;
    
    int sel_start = min(state->selection_start, state->selection_end);
    int sel_end = max(state->selection_start, state->selection_end);
    
    if (sel_start >= sel_end) return;
    
    std::wstring selected_text = state->text.substr(sel_start, sel_end - sel_start);
    
    if (OpenClipboard(state->hwnd)) {
        EmptyClipboard();
        
        HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, (selected_text.length() + 1) * sizeof(wchar_t));
        if (hMem) {
            wchar_t* pMem = (wchar_t*)GlobalLock(hMem);
            if (pMem) {
                wcscpy_s(pMem, selected_text.length() + 1, selected_text.c_str());
                GlobalUnlock(hMem);
                SetClipboardData(CF_UNICODETEXT, hMem);
            }
        }
        
        CloseClipboard();
    }
}

// 从剪贴板粘贴
void PasteFromClipboard(D2DEditBoxState* state) {
    if (!state || state->readonly) return;
    
    if (OpenClipboard(state->hwnd)) {
        HANDLE hData = GetClipboardData(CF_UNICODETEXT);
        if (hData) {
            wchar_t* pData = (wchar_t*)GlobalLock(hData);
            if (pData) {
                std::wstring paste_text = pData;
                
                // 单行模式：移除换行符
                if (!state->multiline) {
                    paste_text.erase(
                        std::remove(paste_text.begin(), paste_text.end(), L'\r'),
                        paste_text.end()
                    );
                    paste_text.erase(
                        std::remove(paste_text.begin(), paste_text.end(), L'\n'),
                        paste_text.end()
                    );
                }
                
                InsertTextAtCursor(state, paste_text);
                GlobalUnlock(hData);
            }
        }
        CloseClipboard();
    }
}

// 剪切到剪贴板
void CutToClipboard(D2DEditBoxState* state) {
    if (!state || state->readonly) return;
    
    CopyToClipboard(state);
    DeleteCharAtCursor(state, true);  // 删除选择的文本
}


// D2D编辑框窗口过程
LRESULT CALLBACK D2DEditBoxProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
    D2DEditBoxState* state = nullptr;
    
    if (msg == WM_CREATE) {
        CREATESTRUCT* cs = (CREATESTRUCT*)lparam;
        state = (D2DEditBoxState*)cs->lpCreateParams;
        SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)state);
    } else {
        state = (D2DEditBoxState*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
    }
    
    if (!state) {
        return DefWindowProc(hwnd, msg, wparam, lparam);
    }

    // 通用事件处理
    HandleCommonEvents(hwnd, msg, wparam, lparam, &state->events);

    switch (msg) {
    case WM_PAINT: {
        PAINTSTRUCT ps;
        BeginPaint(hwnd, &ps);
        DrawD2DEditBox(state);
        EndPaint(hwnd, &ps);
        return 0;
    }
    
    case WM_SETFOCUS:
        state->has_focus = true;
        state->cursor_visible = true;
        state->cursor_timer = SetTimer(hwnd, CURSOR_TIMER_ID, CURSOR_BLINK_INTERVAL, NULL);
        InvalidateRect(hwnd, NULL, FALSE);
        return 0;
    
    case WM_KILLFOCUS:
        state->has_focus = false;
        state->cursor_visible = false;
        if (state->cursor_timer) {
            KillTimer(hwnd, CURSOR_TIMER_ID);
            state->cursor_timer = 0;
        }
        state->selection_start = -1;  // 失去焦点时清除选择
        state->selection_end = -1;
        InvalidateRect(hwnd, NULL, FALSE);
        return 0;
    
    case WM_TIMER:
        if (wparam == CURSOR_TIMER_ID) {
            state->cursor_visible = !state->cursor_visible;
            InvalidateRect(hwnd, NULL, FALSE);
        }
        return 0;
    
    case WM_CHAR: {
        if (state->readonly) return 0;
        
        wchar_t ch = (wchar_t)wparam;
        
        // 处理特殊字符
        if (ch == VK_BACK) {
            DeleteCharAtCursor(state, false);
        } else if (ch == VK_RETURN) {
            if (state->multiline) {
                InsertTextAtCursor(state, L"\n");
            }
        } else if (ch >= 32) {  // 可打印字符
            std::wstring text(1, ch);
            InsertTextAtCursor(state, text);
        }
        
        // 触发按键回调
        if (state->key_callback) {
            state->key_callback(hwnd, (int)wparam, 1, 0, 0, 0);
        }
        
        return 0;
    }
    
    case WM_KEYDOWN: {
        bool shift = (GetKeyState(VK_SHIFT) & 0x8000) != 0;
        bool ctrl = (GetKeyState(VK_CONTROL) & 0x8000) != 0;
        bool alt = (GetKeyState(VK_MENU) & 0x8000) != 0;
        
        switch (wparam) {
        case VK_LEFT:
            MoveCursor(state, -1, shift);
            break;
        case VK_RIGHT:
            MoveCursor(state, 1, shift);
            break;
        case VK_HOME:
            MoveCursor(state, -state->cursor_pos, shift);
            break;
        case VK_END:
            MoveCursor(state, (int)state->text.length() - state->cursor_pos, shift);
            break;
        case VK_DELETE:
            DeleteCharAtCursor(state, true);
            break;
        case 'A':
            if (ctrl) {
                // Ctrl+A: 全选
                state->selection_start = 0;
                state->selection_end = (int)state->text.length();
                state->cursor_pos = (int)state->text.length();
                InvalidateRect(hwnd, NULL, FALSE);
            }
            break;
        case 'C':
            if (ctrl) {
                // Ctrl+C: 复制
                CopyToClipboard(state);
            }
            break;
        case 'V':
            if (ctrl) {
                // Ctrl+V: 粘贴
                PasteFromClipboard(state);
            }
            break;
        case 'X':
            if (ctrl) {
                // Ctrl+X: 剪切
                CutToClipboard(state);
            }
            break;
        }
        
        // 触发按键回调
        if (state->key_callback) {
            state->key_callback(hwnd, (int)wparam, 1, shift ? 1 : 0, ctrl ? 1 : 0, alt ? 1 : 0);
        }
        
        return 0;
    }
    
    case WM_KEYUP: {
        // 触发按键回调
        if (state->key_callback) {
            bool shift = (GetKeyState(VK_SHIFT) & 0x8000) != 0;
            bool ctrl = (GetKeyState(VK_CONTROL) & 0x8000) != 0;
            bool alt = (GetKeyState(VK_MENU) & 0x8000) != 0;
            state->key_callback(hwnd, (int)wparam, 0, shift ? 1 : 0, ctrl ? 1 : 0, alt ? 1 : 0);
        }
        return 0;
    }
    
    case WM_LBUTTONDOWN: {
        SetFocus(hwnd);
        
        // 创建临时布局计算点击位置
        IDWriteTextFormat* text_format = nullptr;
        state->dwrite_factory->CreateTextFormat(
            state->font.font_name.c_str(),
            nullptr,
            state->font.bold ? DWRITE_FONT_WEIGHT_BOLD : DWRITE_FONT_WEIGHT_NORMAL,
            state->font.italic ? DWRITE_FONT_STYLE_ITALIC : DWRITE_FONT_STYLE_NORMAL,
            DWRITE_FONT_STRETCH_NORMAL,
            (float)state->font.font_size,
            L"zh-CN",
            &text_format
        );
        
        std::wstring display_text = state->password && !state->text.empty() ? 
            std::wstring(state->text.length(), L'*') : state->text;
        
        IDWriteTextLayout* layout = nullptr;
        state->dwrite_factory->CreateTextLayout(
            display_text.c_str(),
            (UINT32)display_text.length(),
            text_format,
            (float)state->width - 8.0f,
            (float)state->height - 4.0f,
            &layout
        );
        
        int x = GET_X_LPARAM(lparam);
        int y = GET_Y_LPARAM(lparam);
        
        int char_index = GetCharIndexFromPoint(layout, (float)(x - 4 + state->scroll_offset_x), (float)(y - 2 + state->scroll_offset_y));
        state->cursor_pos = char_index;
        state->selection_start = char_index;
        state->selection_end = char_index;
        
        layout->Release();
        text_format->Release();
        
        SetCapture(hwnd);
        InvalidateRect(hwnd, NULL, FALSE);
        return 0;
    }
    
    case WM_MOUSEMOVE: {
        if (GetCapture() == hwnd) {
            // 拖拽选择文本
            IDWriteTextFormat* text_format = nullptr;
            state->dwrite_factory->CreateTextFormat(
                state->font.font_name.c_str(),
                nullptr,
                state->font.bold ? DWRITE_FONT_WEIGHT_BOLD : DWRITE_FONT_WEIGHT_NORMAL,
                state->font.italic ? DWRITE_FONT_STYLE_ITALIC : DWRITE_FONT_STYLE_NORMAL,
                DWRITE_FONT_STRETCH_NORMAL,
                (float)state->font.font_size,
                L"zh-CN",
                &text_format
            );
            
            std::wstring display_text = state->password && !state->text.empty() ? 
                std::wstring(state->text.length(), L'*') : state->text;
            
            IDWriteTextLayout* layout = nullptr;
            state->dwrite_factory->CreateTextLayout(
                display_text.c_str(),
                (UINT32)display_text.length(),
                text_format,
                (float)state->width - 8.0f,
                (float)state->height - 4.0f,
                &layout
            );
            
            int x = GET_X_LPARAM(lparam);
            int y = GET_Y_LPARAM(lparam);
            
            int char_index = GetCharIndexFromPoint(layout, (float)(x - 4 + state->scroll_offset_x), (float)(y - 2 + state->scroll_offset_y));
            state->cursor_pos = char_index;
            state->selection_end = char_index;
            
            layout->Release();
            text_format->Release();
            
            InvalidateRect(hwnd, NULL, FALSE);
        }
        return 0;
    }
    
    case WM_LBUTTONUP:
        if (GetCapture() == hwnd) {
            ReleaseCapture();
            
            // 如果起始和结束位置相同，清除选择
            if (state->selection_start == state->selection_end) {
                state->selection_start = -1;
                state->selection_end = -1;
            }
            
            InvalidateRect(hwnd, NULL, FALSE);
        }
        return 0;
    
    case WM_LBUTTONDBLCLK: {
        // 双击选择单词（简化实现：选择整行）
        state->selection_start = 0;
        state->selection_end = (int)state->text.length();
        state->cursor_pos = (int)state->text.length();
        InvalidateRect(hwnd, NULL, FALSE);
        return 0;
    }
    
    case WM_IME_STARTCOMPOSITION:
        state->is_composing = true;
        return 0;
    
    case WM_IME_COMPOSITION: {
        if (lparam & GCS_COMPSTR) {
            HIMC hIMC = ImmGetContext(hwnd);
            if (hIMC) {
                LONG len = ImmGetCompositionStringW(hIMC, GCS_COMPSTR, NULL, 0);
                if (len > 0) {
                    std::wstring comp_text(len / sizeof(wchar_t), 0);
                    ImmGetCompositionStringW(hIMC, GCS_COMPSTR, &comp_text[0], len);
                    state->composition_text = comp_text;
                    InvalidateRect(hwnd, NULL, FALSE);
                }
                ImmReleaseContext(hwnd, hIMC);
            }
        }
        
        if (lparam & GCS_RESULTSTR) {
            HIMC hIMC = ImmGetContext(hwnd);
            if (hIMC) {
                LONG len = ImmGetCompositionStringW(hIMC, GCS_RESULTSTR, NULL, 0);
                if (len > 0) {
                    std::wstring result_text(len / sizeof(wchar_t), 0);
                    ImmGetCompositionStringW(hIMC, GCS_RESULTSTR, &result_text[0], len);
                    InsertTextAtCursor(state, result_text);
                }
                ImmReleaseContext(hwnd, hIMC);
            }
        }
        return 0;
    }
    
    case WM_IME_ENDCOMPOSITION:
        state->is_composing = false;
        state->composition_text.clear();
        InvalidateRect(hwnd, NULL, FALSE);
        return 0;
    
    case WM_SIZE: {
        UINT width = LOWORD(lparam);
        UINT height = HIWORD(lparam);
        state->width = width;
        state->height = height;
        if (state->render_target) {
            state->render_target->Resize(D2D1::SizeU(width, height));
        }
        return 0;
    }
    
    case WM_DESTROY:
        if (state->cursor_timer) {
            KillTimer(hwnd, CURSOR_TIMER_ID);
        }
        if (state->render_target) {
            state->render_target->Release();
            state->render_target = nullptr;
        }
        delete state;
        g_d2d_editboxes.erase(hwnd);
        return 0;
    }
    
    return DefWindowProc(hwnd, msg, wparam, lparam);
}


// ========================================
// D2D彩色Emoji编辑框 - 导出函数
// ========================================

// 创建D2D彩色Emoji编辑框
__declspec(dllexport) HWND __stdcall CreateD2DColorEmojiEditBox(
    HWND hParent,
    int x, int y, int width, int height,
    const unsigned char* text_bytes, int text_len,
    UINT32 fg_color,
    UINT32 bg_color,
    const unsigned char* font_name_bytes, int font_name_len,
    int font_size,
    bool bold, bool italic, bool underline,
    int alignment,
    bool multiline,
    bool readonly,
    bool password,
    bool has_border,
    bool vertical_center
) {
    static bool class_registered = false;
    if (!class_registered) {
        WNDCLASSEXW wc = { 0 };
        wc.cbSize = sizeof(WNDCLASSEXW);
        wc.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
        wc.lpfnWndProc = D2DEditBoxProc;
        wc.hInstance = GetModuleHandle(NULL);
        wc.hCursor = LoadCursor(NULL, IDC_IBEAM);
        wc.hbrBackground = NULL;
        wc.lpszClassName = D2D_EDITBOX_CLASS;
        
        if (RegisterClassExW(&wc)) {
            class_registered = true;
        } else {
            return NULL;
        }
    }
    
    D2DEditBoxState* state = new D2DEditBoxState();
    state->parent = hParent;
    state->id = 0;
    state->x = x;
    state->y = y;
    state->width = width;
    state->height = height;
    state->text = Utf8ToWide(text_bytes, text_len);
    state->cursor_pos = 0;
    state->selection_start = -1;
    state->selection_end = -1;
    state->has_focus = false;
    state->cursor_visible = true;
    state->cursor_timer = 0;
    state->scroll_offset_x = 0;
    state->scroll_offset_y = 0;
    state->is_composing = false;
    state->fg_color = fg_color;
    state->bg_color = bg_color;
    state->selection_color = 0x80409EFF;
    state->border_color = 0xFFDCDFE6;
    
    state->font.font_name = Utf8ToWide(font_name_bytes, font_name_len);
    state->font.font_size = font_size;
    state->font.bold = bold;
    state->font.italic = italic;
    state->font.underline = underline;
    
    if (alignment == 0) state->alignment = ALIGN_LEFT;
    else if (alignment == 1) state->alignment = ALIGN_CENTER;
    else state->alignment = ALIGN_RIGHT;
    
    state->multiline = multiline;
    state->readonly = readonly;
    state->password = password;
    state->has_border = has_border;
    state->vertical_center = vertical_center;
    state->enabled = true;
    state->render_target = nullptr;
    state->dwrite_factory = g_dwrite_factory;
    state->key_callback = nullptr;

    int tb_offset = GetTitleBarOffset(hParent);
    HWND hwnd = CreateWindowExW(
        WS_EX_CLIENTEDGE,
        D2D_EDITBOX_CLASS,
        L"",
        WS_CHILD | WS_VISIBLE | WS_TABSTOP,
        x, y + tb_offset, width, height,
        hParent,
        NULL,
        GetModuleHandle(NULL),
        state
    );
    
    if (!hwnd) {
        delete state;
        return NULL;
    }
    
    state->hwnd = hwnd;
    
    if (g_d2d_factory) {
        RECT rc;
        GetClientRect(hwnd, &rc);
        
        D2D1_RENDER_TARGET_PROPERTIES props = D2D1::RenderTargetProperties();
        D2D1_HWND_RENDER_TARGET_PROPERTIES hwndProps = D2D1::HwndRenderTargetProperties(
            hwnd,
            D2D1::SizeU(rc.right - rc.left, rc.bottom - rc.top)
        );
        
        HRESULT hr = g_d2d_factory->CreateHwndRenderTarget(
            props,
            hwndProps,
            &state->render_target
        );
        
        if (FAILED(hr)) {
            DestroyWindow(hwnd);
            delete state;
            return NULL;
        }
    }
    
    g_d2d_editboxes[hwnd] = state;
    
    return hwnd;
}

// 获取D2D编辑框文本
__declspec(dllexport) int __stdcall GetD2DEditBoxText(
    HWND hEdit,
    unsigned char* buffer,
    int buffer_size
) {
    auto it = g_d2d_editboxes.find(hEdit);
    if (it == g_d2d_editboxes.end()) {
        return 0;
    }
    
    D2DEditBoxState* state = it->second;
    
    int utf8_len = WideCharToMultiByte(CP_UTF8, 0, state->text.c_str(), -1, NULL, 0, NULL, NULL);
    if (buffer == NULL || buffer_size == 0) {
        return utf8_len - 1;
    }
    
    if (buffer_size < utf8_len) {
        return 0;
    }
    
    WideCharToMultiByte(CP_UTF8, 0, state->text.c_str(), -1, (LPSTR)buffer, buffer_size, NULL, NULL);
    return utf8_len - 1;
}

// 设置D2D编辑框文本
__declspec(dllexport) void __stdcall SetD2DEditBoxText(
    HWND hEdit,
    const unsigned char* text_bytes,
    int text_len
) {
    auto it = g_d2d_editboxes.find(hEdit);
    if (it == g_d2d_editboxes.end()) return;
    
    D2DEditBoxState* state = it->second;
    state->text = Utf8ToWide(text_bytes, text_len);
    state->cursor_pos = 0;
    state->selection_start = -1;
    state->selection_end = -1;
    state->scroll_offset_x = 0;
    state->scroll_offset_y = 0;
    
    InvalidateRect(hEdit, NULL, FALSE);
}

// 设置D2D编辑框按键回调
__declspec(dllexport) void __stdcall SetD2DEditBoxKeyCallback(
    HWND hEdit,
    EditBoxKeyCallback callback
) {
    auto it = g_d2d_editboxes.find(hEdit);
    if (it == g_d2d_editboxes.end()) return;
    it->second->key_callback = callback;
}

// 启用/禁用D2D编辑框
__declspec(dllexport) void __stdcall EnableD2DEditBox(
    HWND hEdit,
    bool enable
) {
    if (!hEdit) return;
    auto it = g_d2d_editboxes.find(hEdit);
    if (it != g_d2d_editboxes.end()) {
        it->second->enabled = enable;
    }
    EnableWindow(hEdit, enable);
    InvalidateRect(hEdit, NULL, FALSE);
}

// 显示/隐藏D2D编辑框
__declspec(dllexport) void __stdcall ShowD2DEditBox(
    HWND hEdit,
    bool show
) {
    if (!hEdit) return;
    ShowWindow(hEdit, show ? SW_SHOW : SW_HIDE);
}

// 设置D2D编辑框位置和大小
__declspec(dllexport) void __stdcall SetD2DEditBoxBounds(
    HWND hEdit,
    int x, int y, int width, int height
) {
    if (!hEdit) return;
    auto it = g_d2d_editboxes.find(hEdit);
    if (it != g_d2d_editboxes.end()) {
        it->second->x = x;
        it->second->y = y;
        it->second->width = width;
        it->second->height = height;
    }
    SetWindowPos(hEdit, NULL, x, y, width, height, SWP_NOZORDER);
}

// 设置D2D编辑框字体
__declspec(dllexport) void __stdcall SetD2DEditBoxFont(
    HWND hEdit,
    const unsigned char* font_name_bytes, int font_name_len,
    int font_size,
    bool bold, bool italic, bool underline
) {
    auto it = g_d2d_editboxes.find(hEdit);
    if (it == g_d2d_editboxes.end()) return;
    
    D2DEditBoxState* state = it->second;
    state->font.font_name = Utf8ToWide(font_name_bytes, font_name_len);
    state->font.font_size = font_size;
    state->font.bold = bold;
    state->font.italic = italic;
    state->font.underline = underline;
    
    InvalidateRect(hEdit, NULL, FALSE);
}

// 设置D2D编辑框颜色
__declspec(dllexport) void __stdcall SetD2DEditBoxColor(
    HWND hEdit,
    UINT32 fg_color,
    UINT32 bg_color
) {
    auto it = g_d2d_editboxes.find(hEdit);
    if (it == g_d2d_editboxes.end()) return;
    
    D2DEditBoxState* state = it->second;
    state->fg_color = fg_color;
    state->bg_color = bg_color;
    
    InvalidateRect(hEdit, NULL, FALSE);
}


// ========================================
// D2D组合框实现（支持彩色emoji）
// ========================================

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
    int tb_offset = GetTitleBarOffset(hParent);
    HWND hwnd = CreateWindowExW(
        0,
        D2D_COMBOBOX_CLASS,
        L"",
        WS_CHILD | WS_VISIBLE,
        x, y + tb_offset, width, height,
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

    // 通用事件处理
    if (state) HandleCommonEvents(hwnd, msg, wParam, lParam, &state->events);

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
    if (state->events.on_value_changed) {
        state->events.on_value_changed(state->hwnd);
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

// 获取选中项文本
extern "C" __declspec(dllexport) int __stdcall GetD2DComboSelectedText(
    HWND hComboBox,
    unsigned char* buffer,
    int buffer_size
) {
    auto it = g_d2d_comboboxes.find(hComboBox);
    if (it == g_d2d_comboboxes.end()) return 0;

    D2DComboBoxState* state = it->second;
    if (state->selected_index < 0 || state->selected_index >= (int)state->items.size()) return 0;

    std::wstring wstr = state->items[state->selected_index];
    int len = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, NULL, 0, NULL, NULL);
    if (len <= 0) return 0;

    if (buffer && buffer_size > 0) {
        int actual_len = min(len - 1, buffer_size - 1);
        WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, (char*)buffer, actual_len + 1, NULL, NULL);
        return actual_len;
    }

    return len - 1;
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

// ========================================================================
// DataGridView 实现
// ========================================================================

// --- 辅助函数 ---

// 计算所有列的总宽度
static int DataGrid_GetTotalColumnsWidth(DataGridViewState* state) {
    int total = 0;
    for (auto& col : state->columns) {
        total += col.width;
    }
    return total;
}

// 计算所有行的总高度
static int DataGrid_GetTotalRowsHeight(DataGridViewState* state) {
    int row_count = state->virtual_mode ? state->virtual_row_count : (int)state->rows.size();
    return row_count * state->default_row_height;
}

// 获取行数（考虑虚拟模式）
static int DataGrid_GetEffectiveRowCount(DataGridViewState* state) {
    return state->virtual_mode ? state->virtual_row_count : (int)state->rows.size();
}

// 获取单元格文本（考虑虚拟模式）
static std::wstring DataGrid_GetCellDisplayText(DataGridViewState* state, int row, int col) {
    if (state->virtual_mode) {
        if (state->virtual_data_cb) {
            unsigned char buffer[4096];
            int len = state->virtual_data_cb(state->hwnd, row, col, buffer, sizeof(buffer));
            if (len > 0) {
                return Utf8ToWide(buffer, len);
            }
        }
        return L"";
    }
    if (row >= 0 && row < (int)state->rows.size() &&
        col >= 0 && col < (int)state->rows[row].cells.size()) {
        return state->rows[row].cells[col].text;
    }
    return L"";
}

// 获取单元格复选框状态（考虑虚拟模式）
static bool DataGrid_GetCellCheckedState(DataGridViewState* state, int row, int col) {
    if (!state->virtual_mode &&
        row >= 0 && row < (int)state->rows.size() &&
        col >= 0 && col < (int)state->rows[row].cells.size()) {
        return state->rows[row].cells[col].checked;
    }
    return false;
}

// 命中测试：根据像素坐标返回行列
static void DataGrid_HitTest(DataGridViewState* state, int px, int py, int& out_row, int& out_col) {
    out_row = -1;
    out_col = -1;

    // 检查是否在列头区域
    if (py < state->header_height) {
        out_row = -1; // 列头
    } else {
        int y_offset = py - state->header_height + state->scroll_y;
        out_row = y_offset / state->default_row_height;
        int row_count = DataGrid_GetEffectiveRowCount(state);
        if (out_row < 0 || out_row >= row_count) {
            out_row = -1;
        }
    }

    // 计算列
    int x_accum = -state->scroll_x;
    for (int c = 0; c < (int)state->columns.size(); c++) {
        if (px >= x_accum && px < x_accum + state->columns[c].width) {
            out_col = c;
            break;
        }
        x_accum += state->columns[c].width;
    }
}

// 检查是否在列边界上（用于调整列宽）
static int DataGrid_HitTestColumnBorder(DataGridViewState* state, int px, int py) {
    if (py >= state->header_height) return -1; // 只在列头区域
    int x_accum = -state->scroll_x;
    for (int c = 0; c < (int)state->columns.size(); c++) {
        x_accum += state->columns[c].width;
        if (px >= x_accum - 4 && px <= x_accum + 4) {
            if (state->columns[c].resizable) return c;
        }
    }
    return -1;
}

// 确保选中单元格可见（自动滚动）
static void DataGrid_EnsureVisible(DataGridViewState* state, int row, int col) {
    if (row < 0) return;
    // 垂直滚动
    int row_top = row * state->default_row_height;
    int row_bottom = row_top + state->default_row_height;
    int visible_top = state->scroll_y;
    int visible_bottom = state->scroll_y + (state->height - state->header_height);
    if (row_top < visible_top) {
        state->scroll_y = row_top;
    } else if (row_bottom > visible_bottom) {
        state->scroll_y = row_bottom - (state->height - state->header_height);
    }
    // 水平滚动
    if (col >= 0 && col < (int)state->columns.size()) {
        int col_left = 0;
        for (int c = 0; c < col; c++) col_left += state->columns[c].width;
        int col_right = col_left + state->columns[col].width;
        if (col_left < state->scroll_x) {
            state->scroll_x = col_left;
        } else if (col_right > state->scroll_x + state->width) {
            state->scroll_x = col_right - state->width;
        }
    }
    // 限制滚动范围
    int max_scroll_y = max(0, DataGrid_GetTotalRowsHeight(state) - (state->height - state->header_height));
    int max_scroll_x = max(0, DataGrid_GetTotalColumnsWidth(state) - state->width);
    state->scroll_y = max(0, min(state->scroll_y, max_scroll_y));
    state->scroll_x = max(0, min(state->scroll_x, max_scroll_x));
}

// 结束编辑模式
static void DataGrid_EndEdit(DataGridViewState* state, bool apply) {
    if (!state->editing || !state->edit_hwnd) return;
    if (apply && !state->virtual_mode) {
        // 获取编辑框文本
        int len = GetWindowTextLengthW(state->edit_hwnd);
        std::wstring text(len, L'\0');
        GetWindowTextW(state->edit_hwnd, &text[0], len + 1);
        // 写回单元格
        int r = state->edit_row, c = state->edit_col;
        if (r >= 0 && r < (int)state->rows.size() &&
            c >= 0 && c < (int)state->rows[r].cells.size()) {
            state->rows[r].cells[c].text = text;
            if (state->cell_value_changed_cb) {
                state->cell_value_changed_cb(state->hwnd, r, c);
            }
        }
    }
    DestroyWindow(state->edit_hwnd);
    state->edit_hwnd = nullptr;
    state->editing = false;
    state->edit_row = -1;
    state->edit_col = -1;
    SetFocus(state->hwnd);
}

// 开始编辑模式
static void DataGrid_BeginEdit(DataGridViewState* state, int row, int col) {
    if (state->virtual_mode) return; // 虚拟模式不支持编辑
    if (col < 0 || col >= (int)state->columns.size()) return;
    if (state->columns[col].type != DGCOL_TEXT) return; // 只有文本列可编辑
    if (row < 0 || row >= (int)state->rows.size()) return;

    DataGrid_EndEdit(state, false); // 结束之前的编辑

    // 计算单元格位置
    int cell_x = -state->scroll_x;
    for (int c = 0; c < col; c++) cell_x += state->columns[c].width;
    int cell_y = state->header_height + row * state->default_row_height - state->scroll_y;
    int cell_w = state->columns[col].width;
    int cell_h = state->default_row_height;

    // 创建编辑框
    state->edit_hwnd = CreateWindowExW(
        0, L"EDIT", state->rows[row].cells[col].text.c_str(),
        WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL | WS_BORDER,
        cell_x, cell_y, cell_w, cell_h,
        state->hwnd, nullptr, GetModuleHandle(nullptr), nullptr
    );
    if (state->edit_hwnd) {
        state->editing = true;
        state->edit_row = row;
        state->edit_col = col;
        // 设置字体
        HFONT hFont = CreateFontW(
            -(int)state->font.font_size, 0, 0, 0,
            state->font.bold ? FW_BOLD : FW_NORMAL,
            state->font.italic, FALSE, FALSE,
            DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
            CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE,
            state->font.font_name.c_str()
        );
        SendMessageW(state->edit_hwnd, WM_SETFONT, (WPARAM)hFont, TRUE);
        SetFocus(state->edit_hwnd);
        SendMessageW(state->edit_hwnd, EM_SETSEL, 0, -1); // 全选
    }
}


// --- DrawDataGridView ---
void DrawDataGridView(ID2D1HwndRenderTarget* rt, IDWriteFactory* factory, DataGridViewState* state) {
    if (!rt || !factory || !state) return;

    D2D1_SIZE_F size = rt->GetSize();
    int row_count = DataGrid_GetEffectiveRowCount(state);

    ID2D1SolidColorBrush* brush = nullptr;
    rt->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White), &brush);
    if (!brush) return;

    IDWriteTextFormat* text_format = nullptr;
    factory->CreateTextFormat(L"Segoe UI Emoji", nullptr,
        DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL,
        (float)state->font.font_size, L"zh-CN", &text_format);
    if (!text_format) { brush->Release(); return; }
    text_format->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
    text_format->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
    text_format->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);

    IDWriteTextFormat* header_format = nullptr;
    factory->CreateTextFormat(L"Segoe UI Emoji", nullptr,
        DWRITE_FONT_WEIGHT_BOLD, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL,
        (float)state->font.font_size, L"zh-CN", &header_format);
    if (header_format) {
        header_format->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
        header_format->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
        header_format->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
    }

    brush->SetColor(ColorFromUInt32(state->bg_color));
    rt->FillRectangle(D2D1::RectF(0, 0, size.width, size.height), brush);

    float hdr_h = (float)state->header_height;
    D2D1_COLOR_F grid_color = ColorFromUInt32(state->grid_line_color);

    // Column headers
    brush->SetColor(ColorFromUInt32(state->header_bg_color));
    rt->FillRectangle(D2D1::RectF(0, 0, size.width, hdr_h), brush);

    float col_x = (float)(-state->scroll_x);
    for (int c = 0; c < (int)state->columns.size(); c++) {
        float col_w = (float)state->columns[c].width;
        if (col_x + col_w > 0 && col_x < size.width) {
            D2D1_RECT_F hdr_rect = D2D1::RectF(col_x + 8, 0, col_x + col_w - 8, hdr_h);
            brush->SetColor(ColorFromUInt32(state->header_fg_color));
            std::wstring hdr_text = state->columns[c].header_text;
            if (state->sort_col == c) {
                if (state->sort_order == DGSORT_ASC) hdr_text += L" \u25B2";
                else if (state->sort_order == DGSORT_DESC) hdr_text += L" \u25BC";
            }
            
            // 创建临时格式以应用列头对齐方式
            IDWriteTextFormat* hdr_fmt = header_format ? header_format : text_format;
            IDWriteTextFormat* custom_hdr_fmt = nullptr;
            if (state->columns[c].header_alignment != DWRITE_TEXT_ALIGNMENT_LEADING) {
                factory->CreateTextFormat(L"Segoe UI Emoji", nullptr,
                    DWRITE_FONT_WEIGHT_BOLD, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL,
                    (float)state->font.font_size, L"zh-CN", &custom_hdr_fmt);
                if (custom_hdr_fmt) {
                    custom_hdr_fmt->SetTextAlignment(state->columns[c].header_alignment);
                    custom_hdr_fmt->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
                    custom_hdr_fmt->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
                    hdr_fmt = custom_hdr_fmt;
                }
            }
            
            IDWriteTextLayout* layout = nullptr;
            factory->CreateTextLayout(hdr_text.c_str(), (UINT32)hdr_text.length(),
                hdr_fmt,
                hdr_rect.right - hdr_rect.left, hdr_rect.bottom - hdr_rect.top, &layout);
            if (layout) {
                rt->DrawTextLayout(D2D1::Point2F(hdr_rect.left, hdr_rect.top), layout, brush,
                    D2D1_DRAW_TEXT_OPTIONS_ENABLE_COLOR_FONT);
                layout->Release();
            }
            if (custom_hdr_fmt) custom_hdr_fmt->Release();
            if (state->show_grid_lines) {
                brush->SetColor(grid_color);
                rt->DrawLine(D2D1::Point2F(col_x + col_w, 0), D2D1::Point2F(col_x + col_w, hdr_h), brush, 1.0f);
            }
        }
        col_x += col_w;
    }
    if (state->show_grid_lines) {
        brush->SetColor(grid_color);
        rt->DrawLine(D2D1::Point2F(0, hdr_h), D2D1::Point2F(size.width, hdr_h), brush, 1.0f);
    }

    // Data rows
    rt->PushAxisAlignedClip(D2D1::RectF(0, hdr_h, size.width, size.height), D2D1_ANTIALIAS_MODE_ALIASED);

    int visible_start = state->scroll_y / state->default_row_height;
    int visible_count = (int)((size.height - hdr_h) / state->default_row_height) + 2;
    int visible_end = min(visible_start + visible_count, row_count);

    D2D1_COLOR_F select_color = ColorFromUInt32(state->select_color);
    D2D1_COLOR_F hover_color = ColorFromUInt32(state->hover_color);
    D2D1_COLOR_F zebra_color = ColorFromUInt32(state->zebra_color);

    for (int r = visible_start; r < visible_end; r++) {
        float row_y = hdr_h + (float)(r * state->default_row_height - state->scroll_y);
        float row_h = (float)state->default_row_height;
        if (row_y + row_h < hdr_h || row_y > size.height) continue;

        bool is_selected_row = (r == state->selected_row);
        bool is_hovered = (r == state->hovered_row);

        if (is_selected_row && state->selection_mode == DGSEL_ROW) {
            brush->SetColor(select_color);
            rt->FillRectangle(D2D1::RectF(0, row_y, size.width, row_y + row_h), brush);
        } else if (is_hovered && state->enabled) {
            brush->SetColor(hover_color);
            rt->FillRectangle(D2D1::RectF(0, row_y, size.width, row_y + row_h), brush);
        } else if (state->zebra_stripe && (r % 2 == 1)) {
            brush->SetColor(zebra_color);
            rt->FillRectangle(D2D1::RectF(0, row_y, size.width, row_y + row_h), brush);
        }

        col_x = (float)(-state->scroll_x);
        for (int c = 0; c < (int)state->columns.size(); c++) {
            float cw = (float)state->columns[c].width;
            if (col_x + cw <= 0 || col_x >= size.width) { col_x += cw; continue; }

            if (state->selection_mode == DGSEL_CELL && r == state->selected_row && c == state->selected_col) {
                brush->SetColor(select_color);
                rt->FillRectangle(D2D1::RectF(col_x, row_y, col_x + cw, row_y + row_h), brush);
            }

            DataGridCellStyle cell_style = {};
            if (!state->virtual_mode && r < (int)state->rows.size() && c < (int)state->rows[r].cells.size()) {
                cell_style = state->rows[r].cells[c].style;
            }

            switch (state->columns[c].type) {
                case DGCOL_TEXT: {
                    std::wstring text = DataGrid_GetCellDisplayText(state, r, c);
                    if (!text.empty()) {
                        D2D1_RECT_F tr = D2D1::RectF(col_x + 8, row_y, col_x + cw - 8, row_y + row_h);
                        
                        // 设置裁剪区域，防止文本溢出到相邻单元格
                        D2D1_RECT_F clip_rect = D2D1::RectF(col_x, row_y, col_x + cw, row_y + row_h);
                        rt->PushAxisAlignedClip(clip_rect, D2D1_ANTIALIAS_MODE_ALIASED);
                        
                        UINT32 cell_fg = cell_style.fg_color ? cell_style.fg_color : state->fg_color;
                        brush->SetColor(ColorFromUInt32(cell_fg));
                        IDWriteTextFormat* cf = text_format;
                        bool custom_f = false;
                        if (cell_style.bold || cell_style.italic || state->columns[c].cell_alignment != DWRITE_TEXT_ALIGNMENT_LEADING) {
                            IDWriteTextFormat* tmp = nullptr;
                            factory->CreateTextFormat(L"Segoe UI Emoji", nullptr,
                                cell_style.bold ? DWRITE_FONT_WEIGHT_BOLD : DWRITE_FONT_WEIGHT_NORMAL,
                                cell_style.italic ? DWRITE_FONT_STYLE_ITALIC : DWRITE_FONT_STYLE_NORMAL,
                                DWRITE_FONT_STRETCH_NORMAL, (float)state->font.font_size, L"zh-CN", &tmp);
                            if (tmp) { cf = tmp; custom_f = true;
                                cf->SetTextAlignment(state->columns[c].cell_alignment);
                                cf->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
                                cf->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
                            }
                        }
                        IDWriteTextLayout* layout = nullptr;
                        factory->CreateTextLayout(text.c_str(), (UINT32)text.length(), cf,
                            tr.right - tr.left, tr.bottom - tr.top, &layout);
                        if (layout) {
                            rt->DrawTextLayout(D2D1::Point2F(tr.left, tr.top), layout, brush,
                                D2D1_DRAW_TEXT_OPTIONS_ENABLE_COLOR_FONT);
                            layout->Release();
                        }
                        if (custom_f) cf->Release();
                        
                        // 恢复裁剪区域
                        rt->PopAxisAlignedClip();
                    }
                    break;
                }
                case DGCOL_CHECKBOX: {
                    bool checked = DataGrid_GetCellCheckedState(state, r, c);
                    float bs = 16.0f;
                    float bx = col_x + (cw - bs) / 2;
                    float by = row_y + (row_h - bs) / 2;
                    D2D1_RECT_F br = D2D1::RectF(bx, by, bx + bs, by + bs);
                    brush->SetColor(D2D1::ColorF(0xDCDFE6, 1.0f));
                    rt->DrawRectangle(br, brush, 1.0f);
                    if (checked) {
                        brush->SetColor(D2D1::ColorF(0x409EFF, 1.0f));
                        rt->FillRectangle(br, brush);
                        brush->SetColor(D2D1::ColorF(D2D1::ColorF::White));
                        rt->DrawLine(D2D1::Point2F(bx + 3, by + bs / 2),
                                     D2D1::Point2F(bx + bs / 2 - 1, by + bs - 4), brush, 2.0f);
                        rt->DrawLine(D2D1::Point2F(bx + bs / 2 - 1, by + bs - 4),
                                     D2D1::Point2F(bx + bs - 3, by + 3), brush, 2.0f);
                    }
                    break;
                }
                case DGCOL_BUTTON: {
                    std::wstring text = DataGrid_GetCellDisplayText(state, r, c);
                    if (text.empty()) text = L"...";
                    float bw = min(cw - 16, 80.0f);
                    float bh = row_h - 8;
                    float bx = col_x + (cw - bw) / 2;
                    float by = row_y + 4;
                    brush->SetColor(D2D1::ColorF(0x409EFF, 1.0f));
                    rt->FillRoundedRectangle(D2D1::RoundedRect(D2D1::RectF(bx, by, bx + bw, by + bh), 3.0f, 3.0f), brush);
                    brush->SetColor(D2D1::ColorF(D2D1::ColorF::White));
                    IDWriteTextLayout* layout = nullptr;
                    factory->CreateTextLayout(text.c_str(), (UINT32)text.length(), text_format, bw, bh, &layout);
                    if (layout) {
                        layout->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
                        rt->DrawTextLayout(D2D1::Point2F(bx, by), layout, brush, D2D1_DRAW_TEXT_OPTIONS_ENABLE_COLOR_FONT);
                        layout->Release();
                    }
                    break;
                }
                case DGCOL_LINK: {
                    std::wstring text = DataGrid_GetCellDisplayText(state, r, c);
                    if (!text.empty()) {
                        D2D1_RECT_F tr = D2D1::RectF(col_x + 8, row_y, col_x + cw - 8, row_y + row_h);
                        
                        // 设置裁剪区域，防止链接文本溢出
                        D2D1_RECT_F clip_rect = D2D1::RectF(col_x, row_y, col_x + cw, row_y + row_h);
                        rt->PushAxisAlignedClip(clip_rect, D2D1_ANTIALIAS_MODE_ALIASED);
                        
                        brush->SetColor(D2D1::ColorF(0x409EFF, 1.0f));
                        IDWriteTextLayout* layout = nullptr;
                        factory->CreateTextLayout(text.c_str(), (UINT32)text.length(), text_format,
                            tr.right - tr.left, tr.bottom - tr.top, &layout);
                        if (layout) {
                            DWRITE_TEXT_RANGE range = { 0, (UINT32)text.length() };
                            layout->SetUnderline(TRUE, range);
                            rt->DrawTextLayout(D2D1::Point2F(tr.left, tr.top), layout, brush,
                                D2D1_DRAW_TEXT_OPTIONS_ENABLE_COLOR_FONT);
                            layout->Release();
                        }
                        
                        // 恢复裁剪区域
                        rt->PopAxisAlignedClip();
                    }
                    break;
                }
                case DGCOL_IMAGE:
                    break;
            }

            if (state->show_grid_lines) {
                brush->SetColor(grid_color);
                rt->DrawLine(D2D1::Point2F(col_x + cw, row_y), D2D1::Point2F(col_x + cw, row_y + row_h), brush, 0.5f);
            }
            col_x += cw;
        }

        if (state->show_grid_lines) {
            brush->SetColor(grid_color);
            rt->DrawLine(D2D1::Point2F(0, row_y + row_h), D2D1::Point2F(size.width, row_y + row_h), brush, 0.5f);
        }
    }

    rt->PopAxisAlignedClip();

    brush->SetColor(D2D1::ColorF(0xDCDFE6, 1.0f));
    rt->DrawRectangle(D2D1::RectF(0, 0, size.width, size.height), brush, 1.0f);

    // Scrollbar indicators
    int total_rows_h = DataGrid_GetTotalRowsHeight(state);
    int visible_h = state->height - state->header_height;
    if (total_rows_h > visible_h && visible_h > 0) {
        float track_h = (float)visible_h;
        float thumb_h = max(20.0f, track_h * visible_h / total_rows_h);
        float thumb_y = hdr_h + (track_h - thumb_h) * state->scroll_y / (total_rows_h - visible_h);
        brush->SetColor(D2D1::ColorF(0xC0C4CC, 0.6f));
        rt->FillRoundedRectangle(D2D1::RoundedRect(
            D2D1::RectF(size.width - 6, thumb_y, size.width - 2, thumb_y + thumb_h), 2.0f, 2.0f), brush);
    }
    int total_cols_w = DataGrid_GetTotalColumnsWidth(state);
    if (total_cols_w > state->width && state->width > 0) {
        float track_w = (float)state->width;
        float thumb_w = max(20.0f, track_w * state->width / total_cols_w);
        float thumb_x = (track_w - thumb_w) * state->scroll_x / (total_cols_w - state->width);
        brush->SetColor(D2D1::ColorF(0xC0C4CC, 0.6f));
        rt->FillRoundedRectangle(D2D1::RoundedRect(
            D2D1::RectF(thumb_x, size.height - 6, thumb_x + thumb_w, size.height - 2), 2.0f, 2.0f), brush);
    }

    if (header_format) header_format->Release();
    text_format->Release();
    brush->Release();
}

// --- DataGridViewProc ---
LRESULT CALLBACK DataGridViewProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam,
                                   UINT_PTR uIdSubclass, DWORD_PTR dwRefData) {
    auto it = g_datagrids.find(hwnd);
    if (it == g_datagrids.end()) return DefSubclassProc(hwnd, msg, wparam, lparam);
    DataGridViewState* state = it->second;

    // 通用事件处理
    HandleCommonEvents(hwnd, msg, wparam, lparam, &state->events);

    switch (msg) {
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            ID2D1HwndRenderTarget* rt = nullptr;
            D2D1_RENDER_TARGET_PROPERTIES props = D2D1::RenderTargetProperties();
            D2D1_HWND_RENDER_TARGET_PROPERTIES hwnd_props = D2D1::HwndRenderTargetProperties(
                hwnd, D2D1::SizeU(state->width, state->height));
            if (g_d2d_factory) g_d2d_factory->CreateHwndRenderTarget(props, hwnd_props, &rt);
            if (rt && g_dwrite_factory) {
                rt->BeginDraw();
                rt->Clear(D2D1::ColorF(D2D1::ColorF::White));
                DrawDataGridView(rt, g_dwrite_factory, state);
                rt->EndDraw();
                rt->Release();
            }
            EndPaint(hwnd, &ps);
            return 0;
        }

        case WM_LBUTTONDOWN: {
            if (!state->enabled) break;
            SetFocus(hwnd);
            int px = GET_X_LPARAM(lparam);
            int py = GET_Y_LPARAM(lparam);

            // --- Vertical scrollbar hit-test ---
            {
                int total_rows_h = DataGrid_GetTotalRowsHeight(state);
                int visible_h = state->height - state->header_height;
                if (total_rows_h > visible_h && visible_h > 0 && px >= state->width - 12) {
                    float track_h = (float)visible_h;
                    float thumb_h = max(20.0f, track_h * visible_h / total_rows_h);
                    float hdr_h = (float)state->header_height;
                    int max_scroll = total_rows_h - visible_h;
                    float thumb_y = hdr_h + (track_h - thumb_h) * state->scroll_y / max_scroll;
                    if ((float)py >= thumb_y && (float)py <= thumb_y + thumb_h) {
                        // Click on thumb - start dragging
                        state->scrollbar_v_dragging = true;
                        state->scrollbar_drag_offset = (float)py - thumb_y;
                    } else if ((float)py >= hdr_h) {
                        // Click on track - jump to position
                        float ratio = ((float)py - hdr_h - thumb_h * 0.5f) / (track_h - thumb_h);
                        ratio = max(0.0f, min(1.0f, ratio));
                        state->scroll_y = (int)(ratio * max_scroll);
                        state->scrollbar_v_dragging = true;
                        state->scrollbar_drag_offset = thumb_h * 0.5f;
                    }
                    if (state->scrollbar_v_dragging) {
                        SetCapture(hwnd);
                        InvalidateRect(hwnd, nullptr, TRUE);
                        return 0;
                    }
                }
            }

            // --- Horizontal scrollbar hit-test ---
            {
                int total_cols_w = DataGrid_GetTotalColumnsWidth(state);
                if (total_cols_w > state->width && state->width > 0 && py >= state->height - 12) {
                    float track_w = (float)state->width;
                    float thumb_w = max(20.0f, track_w * state->width / total_cols_w);
                    int max_scroll = total_cols_w - state->width;
                    float thumb_x = (track_w - thumb_w) * state->scroll_x / max_scroll;
                    if ((float)px >= thumb_x && (float)px <= thumb_x + thumb_w) {
                        state->scrollbar_h_dragging = true;
                        state->scrollbar_drag_offset = (float)px - thumb_x;
                    } else {
                        float ratio = ((float)px - thumb_w * 0.5f) / (track_w - thumb_w);
                        ratio = max(0.0f, min(1.0f, ratio));
                        state->scroll_x = (int)(ratio * max_scroll);
                        state->scrollbar_h_dragging = true;
                        state->scrollbar_drag_offset = thumb_w * 0.5f;
                    }
                    if (state->scrollbar_h_dragging) {
                        SetCapture(hwnd);
                        InvalidateRect(hwnd, nullptr, TRUE);
                        return 0;
                    }
                }
            }

            // Check column resize
            int resize_col = DataGrid_HitTestColumnBorder(state, px, py);
            if (resize_col >= 0) {
                state->resizing_col = true;
                state->resize_col_index = resize_col;
                state->resize_start_x = px;
                state->resize_start_width = state->columns[resize_col].width;
                SetCapture(hwnd);
                return 0;
            }

            int hit_row, hit_col;
            DataGrid_HitTest(state, px, py, hit_row, hit_col);

            if (hit_row == -1 && hit_col >= 0 && py < state->header_height) {
                // Column header click
                if (state->col_header_click_cb) state->col_header_click_cb(hwnd, hit_col);
                // Auto sort
                if (state->columns[hit_col].sortable) {
                    DataGridSortOrder new_order = DGSORT_ASC;
                    if (state->sort_col == hit_col && state->sort_order == DGSORT_ASC) new_order = DGSORT_DESC;
                    else if (state->sort_col == hit_col && state->sort_order == DGSORT_DESC) new_order = DGSORT_NONE;
                    state->sort_col = hit_col;
                    state->sort_order = new_order;
                    // Sort rows
                    if (!state->virtual_mode && new_order != DGSORT_NONE) {
                        int sc = hit_col;
                        bool asc = (new_order == DGSORT_ASC);
                        std::sort(state->rows.begin(), state->rows.end(),
                            [sc, asc](const DataGridRow& a, const DataGridRow& b) {
                                if (sc >= (int)a.cells.size() || sc >= (int)b.cells.size()) return false;
                                return asc ? (a.cells[sc].text < b.cells[sc].text) : (a.cells[sc].text > b.cells[sc].text);
                            });
                    }
                    InvalidateRect(hwnd, nullptr, TRUE);
                }
                return 0;
            }

            if (hit_row >= 0) {
                // Handle checkbox toggle
                if (hit_col >= 0 && hit_col < (int)state->columns.size() &&
                    state->columns[hit_col].type == DGCOL_CHECKBOX && !state->virtual_mode) {
                    if (hit_row < (int)state->rows.size() && hit_col < (int)state->rows[hit_row].cells.size()) {
                        state->rows[hit_row].cells[hit_col].checked = !state->rows[hit_row].cells[hit_col].checked;
                        if (state->cell_value_changed_cb) state->cell_value_changed_cb(hwnd, hit_row, hit_col);
                    }
                }

                bool changed = (state->selected_row != hit_row || state->selected_col != hit_col);
                state->selected_row = hit_row;
                state->selected_col = hit_col;
                if (changed && state->selection_changed_cb) state->selection_changed_cb(hwnd, hit_row, hit_col);
                if (state->cell_click_cb) state->cell_click_cb(hwnd, hit_row, hit_col);
                InvalidateRect(hwnd, nullptr, TRUE);
            }
            return 0;
        }

        case WM_LBUTTONDBLCLK: {
            if (!state->enabled) break;
            int px = GET_X_LPARAM(lparam);
            int py = GET_Y_LPARAM(lparam);
            int hit_row, hit_col;
            DataGrid_HitTest(state, px, py, hit_row, hit_col);
            if (hit_row >= 0 && hit_col >= 0) {
                if (state->cell_dblclick_cb) state->cell_dblclick_cb(hwnd, hit_row, hit_col);
                // Enter edit mode for text columns
                if (state->columns[hit_col].type == DGCOL_TEXT) {
                    DataGrid_BeginEdit(state, hit_row, hit_col);
                }
            }
            return 0;
        }

        case WM_LBUTTONUP: {
            if (state->scrollbar_v_dragging || state->scrollbar_h_dragging) {
                state->scrollbar_v_dragging = false;
                state->scrollbar_h_dragging = false;
                ReleaseCapture();
                InvalidateRect(hwnd, nullptr, TRUE);
                return 0;
            }
            if (state->resizing_col) {
                state->resizing_col = false;
                ReleaseCapture();
                InvalidateRect(hwnd, nullptr, TRUE);
            }
            return 0;
        }

        case WM_MOUSEMOVE: {
            if (!state->enabled) break;
            int px = GET_X_LPARAM(lparam);
            int py = GET_Y_LPARAM(lparam);

            // Scrollbar dragging
            if (state->scrollbar_v_dragging) {
                int total_rows_h = DataGrid_GetTotalRowsHeight(state);
                int visible_h = state->height - state->header_height;
                int max_scroll = max(0, total_rows_h - visible_h);
                float track_h = (float)visible_h;
                float thumb_h = max(20.0f, track_h * visible_h / total_rows_h);
                float hdr_h = (float)state->header_height;
                float new_thumb_y = (float)py - state->scrollbar_drag_offset - hdr_h;
                float ratio = new_thumb_y / (track_h - thumb_h);
                ratio = max(0.0f, min(1.0f, ratio));
                state->scroll_y = (int)(ratio * max_scroll);
                InvalidateRect(hwnd, nullptr, TRUE);
                return 0;
            }
            if (state->scrollbar_h_dragging) {
                int total_cols_w = DataGrid_GetTotalColumnsWidth(state);
                int max_scroll = max(0, total_cols_w - state->width);
                float track_w = (float)state->width;
                float thumb_w = max(20.0f, track_w * state->width / total_cols_w);
                float new_thumb_x = (float)px - state->scrollbar_drag_offset;
                float ratio = new_thumb_x / (track_w - thumb_w);
                ratio = max(0.0f, min(1.0f, ratio));
                state->scroll_x = (int)(ratio * max_scroll);
                InvalidateRect(hwnd, nullptr, TRUE);
                return 0;
            }

            if (state->resizing_col) {
                int delta = px - state->resize_start_x;
                int new_width = max(state->columns[state->resize_col_index].min_width,
                                    state->resize_start_width + delta);
                state->columns[state->resize_col_index].width = new_width;
                InvalidateRect(hwnd, nullptr, TRUE);
                return 0;
            }

            // Check cursor for column resize
            int resize_col = DataGrid_HitTestColumnBorder(state, px, py);
            if (resize_col >= 0) {
                SetCursor(LoadCursor(nullptr, IDC_SIZEWE));
            } else {
                SetCursor(LoadCursor(nullptr, IDC_ARROW));
            }

            int hit_row, hit_col;
            DataGrid_HitTest(state, px, py, hit_row, hit_col);
            if (hit_row != state->hovered_row || hit_col != state->hovered_col) {
                state->hovered_row = hit_row;
                state->hovered_col = hit_col;
                InvalidateRect(hwnd, nullptr, TRUE);
            }

            TRACKMOUSEEVENT tme = { sizeof(TRACKMOUSEEVENT) };
            tme.dwFlags = TME_LEAVE;
            tme.hwndTrack = hwnd;
            TrackMouseEvent(&tme);
            return 0;
        }

        case WM_MOUSELEAVE: {
            if (state->hovered_row != -1 || state->hovered_col != -1) {
                state->hovered_row = -1;
                state->hovered_col = -1;
                InvalidateRect(hwnd, nullptr, TRUE);
            }
            return 0;
        }

        case WM_MOUSEWHEEL: {
            if (!state->enabled) break;
            int delta = GET_WHEEL_DELTA_WPARAM(wparam);
            int scroll_amount = -delta / 3;
            int max_scroll = max(0, DataGrid_GetTotalRowsHeight(state) - (state->height - state->header_height));
            state->scroll_y = max(0, min(max_scroll, state->scroll_y + scroll_amount));
            InvalidateRect(hwnd, nullptr, TRUE);
            return 0;
        }

        case WM_MOUSEHWHEEL: {
            if (!state->enabled) break;
            int delta = GET_WHEEL_DELTA_WPARAM(wparam);
            int scroll_amount = delta / 3;
            int max_scroll = max(0, DataGrid_GetTotalColumnsWidth(state) - state->width);
            state->scroll_x = max(0, min(max_scroll, state->scroll_x + scroll_amount));
            InvalidateRect(hwnd, nullptr, TRUE);
            return 0;
        }

        case WM_KEYDOWN: {
            if (!state->enabled) break;
            if (state->editing) break; // Let edit control handle keys

            int row = state->selected_row;
            int col = state->selected_col;
            int row_count = DataGrid_GetEffectiveRowCount(state);
            int col_count = (int)state->columns.size();

            switch (wparam) {
                case VK_UP:
                    if (row > 0) { state->selected_row = row - 1; }
                    break;
                case VK_DOWN:
                    if (row < row_count - 1) { state->selected_row = row + 1; }
                    break;
                case VK_LEFT:
                    if (state->selection_mode == DGSEL_CELL && col > 0) { state->selected_col = col - 1; }
                    break;
                case VK_RIGHT:
                    if (state->selection_mode == DGSEL_CELL && col < col_count - 1) { state->selected_col = col + 1; }
                    break;
                case VK_TAB:
                    if (state->selection_mode == DGSEL_CELL) {
                        if (GetKeyState(VK_SHIFT) & 0x8000) {
                            if (col > 0) state->selected_col = col - 1;
                            else if (row > 0) { state->selected_row = row - 1; state->selected_col = col_count - 1; }
                        } else {
                            if (col < col_count - 1) state->selected_col = col + 1;
                            else if (row < row_count - 1) { state->selected_row = row + 1; state->selected_col = 0; }
                        }
                    }
                    break;
                case VK_HOME:
                    state->selected_row = 0;
                    if (state->selection_mode == DGSEL_CELL) state->selected_col = 0;
                    break;
                case VK_END:
                    state->selected_row = row_count - 1;
                    if (state->selection_mode == DGSEL_CELL) state->selected_col = col_count - 1;
                    break;
                case VK_PRIOR: // Page Up
                    state->selected_row = max(0, row - (state->height - state->header_height) / state->default_row_height);
                    break;
                case VK_NEXT: // Page Down
                    state->selected_row = min(row_count - 1, row + (state->height - state->header_height) / state->default_row_height);
                    break;
                case VK_RETURN:
                case VK_F2:
                    if (col >= 0 && col < col_count && state->columns[col].type == DGCOL_TEXT) {
                        DataGrid_BeginEdit(state, state->selected_row, state->selected_col);
                    }
                    return 0;
                case VK_ESCAPE:
                    DataGrid_EndEdit(state, false);
                    return 0;
                default:
                    break;
            }

            if (state->selected_row != row || state->selected_col != col) {
                DataGrid_EnsureVisible(state, state->selected_row, state->selected_col);
                if (state->selection_changed_cb) state->selection_changed_cb(hwnd, state->selected_row, state->selected_col);
                InvalidateRect(hwnd, nullptr, TRUE);
            }
            return 0;
        }

        case WM_SIZE: {
            state->width = LOWORD(lparam);
            state->height = HIWORD(lparam);
            InvalidateRect(hwnd, nullptr, TRUE);
            return 0;
        }

        case WM_SETFOCUS: {
            InvalidateRect(hwnd, nullptr, TRUE);
            return 0;
        }

        case WM_KILLFOCUS: {
            if (state->editing && (HWND)wparam != state->edit_hwnd) {
                DataGrid_EndEdit(state, true);
            }
            return 0;
        }

        case WM_COMMAND: {
            // Handle edit box notifications
            if (state->editing && (HWND)lparam == state->edit_hwnd) {
                if (HIWORD(wparam) == EN_KILLFOCUS) {
                    DataGrid_EndEdit(state, true);
                }
            }
            return 0;
        }

        case WM_NCDESTROY: {
            DataGrid_EndEdit(state, false);
            RemoveWindowSubclass(hwnd, DataGridViewProc, uIdSubclass);
            g_datagrids.erase(hwnd);
            delete state;
            return 0;
        }
    }

    return DefSubclassProc(hwnd, msg, wparam, lparam);
}

// ========== DataGridView Export Functions ==========

extern "C" {

// Create DataGridView
__declspec(dllexport) HWND __stdcall CreateDataGridView(
    HWND hParent, int x, int y, int width, int height,
    BOOL virtual_mode, BOOL zebra_stripe, UINT32 fg_color, UINT32 bg_color
) {
    if (!hParent || !IsWindow(hParent)) return nullptr;

    int tb_offset = GetTitleBarOffset(hParent);
    HWND hwnd = CreateWindowExW(0, L"STATIC", L"",
        WS_CHILD | WS_VISIBLE | SS_NOTIFY | WS_TABSTOP,
        x, y + tb_offset, width, height, hParent,
        (HMENU)(UINT_PTR)(g_next_control_id++),
        GetModuleHandle(nullptr), nullptr);
    if (!hwnd) return nullptr;

    DataGridViewState* state = new DataGridViewState();
    state->hwnd = hwnd;
    state->parent = hParent;
    state->id = (int)(UINT_PTR)GetMenu(hwnd);
    state->x = x; state->y = y; state->width = width; state->height = height;
    state->virtual_mode = (virtual_mode != 0);
    state->virtual_row_count = 0;
    state->selected_row = -1; state->selected_col = -1;
    state->selection_mode = DGSEL_ROW;
    state->editing = false; state->edit_row = -1; state->edit_col = -1; state->edit_hwnd = nullptr;
    state->hovered_row = -1; state->hovered_col = -1;
    state->scroll_x = 0; state->scroll_y = 0;
    state->resizing_col = false; state->resize_col_index = -1;
    state->scrollbar_v_dragging = false; state->scrollbar_h_dragging = false; state->scrollbar_drag_offset = 0;
    state->sort_col = -1; state->sort_order = DGSORT_NONE;
    state->freeze_header = true; state->freeze_first_col = false;
    state->header_height = 36;
    state->default_row_height = 32;
    state->zebra_stripe = (zebra_stripe != 0);
    state->show_grid_lines = true;
    state->enabled = true;
    state->fg_color = fg_color;
    state->bg_color = bg_color;
    state->header_bg_color = ThemeColor_BackgroundLight();
    state->header_fg_color = ThemeColor_TextRegular();
    state->grid_line_color = ThemeColor_BorderLighter();
    state->select_color = ThemeColor_LightBg(ThemeColor_Primary());
    state->hover_color = ThemeColor_BackgroundLight();
    state->zebra_color = 0xFFFAFAFA;
    state->font.font_name = L"Microsoft YaHei UI";
    state->font.font_size = 14;
    state->font.bold = false; state->font.italic = false; state->font.underline = false;
    state->cell_click_cb = nullptr;
    state->cell_dblclick_cb = nullptr;
    state->cell_value_changed_cb = nullptr;
    state->col_header_click_cb = nullptr;
    state->selection_changed_cb = nullptr;
    state->virtual_data_cb = nullptr;

    g_datagrids[hwnd] = state;
    SetWindowSubclass(hwnd, DataGridViewProc, 0, (DWORD_PTR)state);
    return hwnd;
}

// --- Column management ---

static int DataGrid_AddColumn(HWND hGrid, const unsigned char* header_bytes, int header_len, int width, DataGridColumnType type) {
    auto it = g_datagrids.find(hGrid);
    if (it == g_datagrids.end()) return -1;
    DataGridViewState* state = it->second;
    DataGridColumn col;
    col.header_text = Utf8ToWide(header_bytes, header_len);
    col.width = width > 0 ? width : 100;
    col.min_width = 30;
    col.type = type;
    col.resizable = true;
    col.sortable = (type == DGCOL_TEXT);
    col.sort_order = DGSORT_NONE;
    state->columns.push_back(col);
    // Add cells to existing rows
    for (auto& row : state->rows) {
        DataGridCell cell;
        cell.checked = false;
        cell.style = {};
        row.cells.push_back(cell);
    }
    InvalidateRect(hGrid, nullptr, TRUE);
    return (int)state->columns.size() - 1;
}

__declspec(dllexport) int __stdcall DataGrid_AddTextColumn(HWND hGrid, const unsigned char* h, int hl, int w) {
    return DataGrid_AddColumn(hGrid, h, hl, w, DGCOL_TEXT);
}
__declspec(dllexport) int __stdcall DataGrid_AddCheckBoxColumn(HWND hGrid, const unsigned char* h, int hl, int w) {
    return DataGrid_AddColumn(hGrid, h, hl, w, DGCOL_CHECKBOX);
}
__declspec(dllexport) int __stdcall DataGrid_AddButtonColumn(HWND hGrid, const unsigned char* h, int hl, int w) {
    return DataGrid_AddColumn(hGrid, h, hl, w, DGCOL_BUTTON);
}
__declspec(dllexport) int __stdcall DataGrid_AddLinkColumn(HWND hGrid, const unsigned char* h, int hl, int w) {
    return DataGrid_AddColumn(hGrid, h, hl, w, DGCOL_LINK);
}
__declspec(dllexport) int __stdcall DataGrid_AddImageColumn(HWND hGrid, const unsigned char* h, int hl, int w) {
    return DataGrid_AddColumn(hGrid, h, hl, w, DGCOL_IMAGE);
}

__declspec(dllexport) void __stdcall DataGrid_RemoveColumn(HWND hGrid, int col_index) {
    auto it = g_datagrids.find(hGrid);
    if (it == g_datagrids.end()) return;
    DataGridViewState* state = it->second;
    if (col_index < 0 || col_index >= (int)state->columns.size()) return;
    state->columns.erase(state->columns.begin() + col_index);
    for (auto& row : state->rows) {
        if (col_index < (int)row.cells.size()) row.cells.erase(row.cells.begin() + col_index);
    }
    InvalidateRect(hGrid, nullptr, TRUE);
}

__declspec(dllexport) void __stdcall DataGrid_ClearColumns(HWND hGrid) {
    auto it = g_datagrids.find(hGrid);
    if (it == g_datagrids.end()) return;
    it->second->columns.clear();
    it->second->rows.clear();
    InvalidateRect(hGrid, nullptr, TRUE);
}

__declspec(dllexport) int __stdcall DataGrid_GetColumnCount(HWND hGrid) {
    auto it = g_datagrids.find(hGrid);
    if (it == g_datagrids.end()) return 0;
    return (int)it->second->columns.size();
}

__declspec(dllexport) void __stdcall DataGrid_SetColumnWidth(HWND hGrid, int col_index, int width) {
    auto it = g_datagrids.find(hGrid);
    if (it == g_datagrids.end()) return;
    if (col_index < 0 || col_index >= (int)it->second->columns.size()) return;
    it->second->columns[col_index].width = max(it->second->columns[col_index].min_width, width);
    InvalidateRect(hGrid, nullptr, TRUE);
}

// --- Row management ---

__declspec(dllexport) int __stdcall DataGrid_AddRow(HWND hGrid) {
    auto it = g_datagrids.find(hGrid);
    if (it == g_datagrids.end()) return -1;
    DataGridViewState* state = it->second;
    if (state->virtual_mode) return -1;
    DataGridRow row;
    row.height = 0;
    for (size_t i = 0; i < state->columns.size(); i++) {
        DataGridCell cell;
        cell.checked = false;
        cell.style = {};
        row.cells.push_back(cell);
    }
    state->rows.push_back(row);
    InvalidateRect(hGrid, nullptr, TRUE);
    return (int)state->rows.size() - 1;
}

__declspec(dllexport) void __stdcall DataGrid_RemoveRow(HWND hGrid, int row_index) {
    auto it = g_datagrids.find(hGrid);
    if (it == g_datagrids.end()) return;
    DataGridViewState* state = it->second;
    if (state->virtual_mode) return;
    if (row_index < 0 || row_index >= (int)state->rows.size()) return;
    state->rows.erase(state->rows.begin() + row_index);
    if (state->selected_row >= (int)state->rows.size()) state->selected_row = (int)state->rows.size() - 1;
    InvalidateRect(hGrid, nullptr, TRUE);
}

__declspec(dllexport) void __stdcall DataGrid_ClearRows(HWND hGrid) {
    auto it = g_datagrids.find(hGrid);
    if (it == g_datagrids.end()) return;
    DataGridViewState* state = it->second;
    if (state->virtual_mode) { state->virtual_row_count = 0; }
    else { state->rows.clear(); }
    state->selected_row = -1; state->selected_col = -1;
    state->scroll_y = 0;
    InvalidateRect(hGrid, nullptr, TRUE);
}

__declspec(dllexport) int __stdcall DataGrid_GetRowCount(HWND hGrid) {
    auto it = g_datagrids.find(hGrid);
    if (it == g_datagrids.end()) return 0;
    return DataGrid_GetEffectiveRowCount(it->second);
}

// --- Cell operations ---

__declspec(dllexport) void __stdcall DataGrid_SetCellText(HWND hGrid, int row, int col,
    const unsigned char* text_bytes, int text_len) {
    auto it = g_datagrids.find(hGrid);
    if (it == g_datagrids.end()) return;
    DataGridViewState* state = it->second;
    if (state->virtual_mode) return;
    if (row < 0 || row >= (int)state->rows.size()) return;
    if (col < 0 || col >= (int)state->rows[row].cells.size()) return;
    state->rows[row].cells[col].text = Utf8ToWide(text_bytes, text_len);
    InvalidateRect(hGrid, nullptr, TRUE);
}

__declspec(dllexport) int __stdcall DataGrid_GetCellText(HWND hGrid, int row, int col,
    unsigned char* buffer, int buffer_size) {
    auto it = g_datagrids.find(hGrid);
    if (it == g_datagrids.end()) return 0;
    DataGridViewState* state = it->second;
    std::wstring text = DataGrid_GetCellDisplayText(state, row, col);
    if (text.empty()) return 0;
    int needed = WideCharToMultiByte(CP_UTF8, 0, text.c_str(), -1, nullptr, 0, nullptr, nullptr);
    if (!buffer || buffer_size <= 0) return needed;
    int written = WideCharToMultiByte(CP_UTF8, 0, text.c_str(), -1, (char*)buffer, buffer_size, nullptr, nullptr);
    return written > 0 ? written - 1 : 0;
}

__declspec(dllexport) void __stdcall DataGrid_SetCellChecked(HWND hGrid, int row, int col, BOOL checked) {
    auto it = g_datagrids.find(hGrid);
    if (it == g_datagrids.end()) return;
    DataGridViewState* state = it->second;
    if (state->virtual_mode) return;
    if (row < 0 || row >= (int)state->rows.size()) return;
    if (col < 0 || col >= (int)state->rows[row].cells.size()) return;
    state->rows[row].cells[col].checked = (checked != 0);
    InvalidateRect(hGrid, nullptr, TRUE);
}

__declspec(dllexport) BOOL __stdcall DataGrid_GetCellChecked(HWND hGrid, int row, int col) {
    auto it = g_datagrids.find(hGrid);
    if (it == g_datagrids.end()) return FALSE;
    return DataGrid_GetCellCheckedState(it->second, row, col) ? TRUE : FALSE;
}

__declspec(dllexport) void __stdcall DataGrid_SetCellStyle(HWND hGrid, int row, int col,
    UINT32 fg_color, UINT32 bg_color, BOOL bold, BOOL italic) {
    auto it = g_datagrids.find(hGrid);
    if (it == g_datagrids.end()) return;
    DataGridViewState* state = it->second;
    if (state->virtual_mode) return;
    if (row < 0 || row >= (int)state->rows.size()) return;
    if (col < 0 || col >= (int)state->rows[row].cells.size()) return;
    state->rows[row].cells[col].style.fg_color = fg_color;
    state->rows[row].cells[col].style.bg_color = bg_color;
    state->rows[row].cells[col].style.bold = (bold != 0);
    state->rows[row].cells[col].style.italic = (italic != 0);
    InvalidateRect(hGrid, nullptr, TRUE);
}

// --- Selection ---

__declspec(dllexport) int __stdcall DataGrid_GetSelectedRow(HWND hGrid) {
    auto it = g_datagrids.find(hGrid);
    return it != g_datagrids.end() ? it->second->selected_row : -1;
}

__declspec(dllexport) int __stdcall DataGrid_GetSelectedCol(HWND hGrid) {
    auto it = g_datagrids.find(hGrid);
    return it != g_datagrids.end() ? it->second->selected_col : -1;
}

__declspec(dllexport) void __stdcall DataGrid_SetSelectedCell(HWND hGrid, int row, int col) {
    auto it = g_datagrids.find(hGrid);
    if (it == g_datagrids.end()) return;
    it->second->selected_row = row;
    it->second->selected_col = col;
    DataGrid_EnsureVisible(it->second, row, col);
    InvalidateRect(hGrid, nullptr, TRUE);
}

__declspec(dllexport) void __stdcall DataGrid_SetSelectionMode(HWND hGrid, int mode) {
    auto it = g_datagrids.find(hGrid);
    if (it == g_datagrids.end()) return;
    it->second->selection_mode = (mode == 1) ? DGSEL_ROW : DGSEL_CELL;
    InvalidateRect(hGrid, nullptr, TRUE);
}

// --- Sort ---

__declspec(dllexport) void __stdcall DataGrid_SortByColumn(HWND hGrid, int col_index, int sort_order) {
    auto it = g_datagrids.find(hGrid);
    if (it == g_datagrids.end()) return;
    DataGridViewState* state = it->second;
    if (col_index < 0 || col_index >= (int)state->columns.size()) return;
    state->sort_col = col_index;
    state->sort_order = (DataGridSortOrder)sort_order;
    if (!state->virtual_mode && state->sort_order != DGSORT_NONE) {
        int sc = col_index;
        bool asc = (state->sort_order == DGSORT_ASC);
        std::sort(state->rows.begin(), state->rows.end(),
            [sc, asc](const DataGridRow& a, const DataGridRow& b) {
                if (sc >= (int)a.cells.size() || sc >= (int)b.cells.size()) return false;
                return asc ? (a.cells[sc].text < b.cells[sc].text) : (a.cells[sc].text > b.cells[sc].text);
            });
    }
    InvalidateRect(hGrid, nullptr, TRUE);
}

// --- Freeze ---

__declspec(dllexport) void __stdcall DataGrid_SetFreezeHeader(HWND hGrid, BOOL freeze) {
    auto it = g_datagrids.find(hGrid);
    if (it != g_datagrids.end()) it->second->freeze_header = (freeze != 0);
}

__declspec(dllexport) void __stdcall DataGrid_SetFreezeFirstColumn(HWND hGrid, BOOL freeze) {
    auto it = g_datagrids.find(hGrid);
    if (it != g_datagrids.end()) it->second->freeze_first_col = (freeze != 0);
}

// --- Virtual mode ---

__declspec(dllexport) void __stdcall DataGrid_SetVirtualRowCount(HWND hGrid, int row_count) {
    auto it = g_datagrids.find(hGrid);
    if (it == g_datagrids.end()) return;
    it->second->virtual_row_count = row_count;
    InvalidateRect(hGrid, nullptr, TRUE);
}

__declspec(dllexport) void __stdcall DataGrid_SetVirtualDataCallback(HWND hGrid, DataGridVirtualDataCallback callback) {
    auto it = g_datagrids.find(hGrid);
    if (it != g_datagrids.end()) it->second->virtual_data_cb = callback;
}

// --- Appearance ---

__declspec(dllexport) void __stdcall DataGrid_SetShowGridLines(HWND hGrid, BOOL show) {
    auto it = g_datagrids.find(hGrid);
    if (it == g_datagrids.end()) return;
    it->second->show_grid_lines = (show != 0);
    InvalidateRect(hGrid, nullptr, TRUE);
}

__declspec(dllexport) void __stdcall DataGrid_SetDefaultRowHeight(HWND hGrid, int height) {
    auto it = g_datagrids.find(hGrid);
    if (it == g_datagrids.end()) return;
    it->second->default_row_height = max(16, height);
    InvalidateRect(hGrid, nullptr, TRUE);
}

__declspec(dllexport) void __stdcall DataGrid_SetHeaderHeight(HWND hGrid, int height) {
    auto it = g_datagrids.find(hGrid);
    if (it == g_datagrids.end()) return;
    it->second->header_height = max(16, height);
    InvalidateRect(hGrid, nullptr, TRUE);
}

// --- Event callbacks ---

__declspec(dllexport) void __stdcall DataGrid_SetCellClickCallback(HWND hGrid, DataGridCellClickCallback cb) {
    auto it = g_datagrids.find(hGrid); if (it != g_datagrids.end()) it->second->cell_click_cb = cb;
}
__declspec(dllexport) void __stdcall DataGrid_SetCellDoubleClickCallback(HWND hGrid, DataGridCellDoubleClickCallback cb) {
    auto it = g_datagrids.find(hGrid); if (it != g_datagrids.end()) it->second->cell_dblclick_cb = cb;
}
__declspec(dllexport) void __stdcall DataGrid_SetCellValueChangedCallback(HWND hGrid, DataGridCellValueChangedCallback cb) {
    auto it = g_datagrids.find(hGrid); if (it != g_datagrids.end()) it->second->cell_value_changed_cb = cb;
}
__declspec(dllexport) void __stdcall DataGrid_SetColumnHeaderClickCallback(HWND hGrid, DataGridColumnHeaderClickCallback cb) {
    auto it = g_datagrids.find(hGrid); if (it != g_datagrids.end()) it->second->col_header_click_cb = cb;
}
__declspec(dllexport) void __stdcall DataGrid_SetSelectionChangedCallback(HWND hGrid, DataGridSelectionChangedCallback cb) {
    auto it = g_datagrids.find(hGrid); if (it != g_datagrids.end()) it->second->selection_changed_cb = cb;
}

// --- Other ---

__declspec(dllexport) void __stdcall DataGrid_Enable(HWND hGrid, BOOL enable) {
    auto it = g_datagrids.find(hGrid);
    if (it == g_datagrids.end()) return;
    it->second->enabled = (enable != 0);
    EnableWindow(hGrid, enable);
    InvalidateRect(hGrid, nullptr, TRUE);
}

__declspec(dllexport) void __stdcall DataGrid_Show(HWND hGrid, BOOL show) {
    ShowWindow(hGrid, show ? SW_SHOW : SW_HIDE);
}

__declspec(dllexport) void __stdcall DataGrid_SetBounds(HWND hGrid, int x, int y, int width, int height) {
    auto it = g_datagrids.find(hGrid);
    if (it == g_datagrids.end()) return;
    it->second->x = x; it->second->y = y;
    it->second->width = width; it->second->height = height;
    SetWindowPos(hGrid, NULL, x, y, width, height, SWP_NOZORDER);
}

__declspec(dllexport) void __stdcall DataGrid_Refresh(HWND hGrid) {
    InvalidateRect(hGrid, nullptr, TRUE);
}

// 设置列头对齐方式 (0=左对齐, 1=居中, 2=右对齐)
__declspec(dllexport) void __stdcall DataGrid_SetColumnHeaderAlignment(HWND hGrid, int col_index, int alignment) {
    auto it = g_datagrids.find(hGrid);
    if (it == g_datagrids.end()) return;
    DataGridViewState* state = it->second;
    if (col_index < 0 || col_index >= (int)state->columns.size()) return;
    
    DWRITE_TEXT_ALIGNMENT align = DWRITE_TEXT_ALIGNMENT_LEADING;
    if (alignment == 1) align = DWRITE_TEXT_ALIGNMENT_CENTER;
    else if (alignment == 2) align = DWRITE_TEXT_ALIGNMENT_TRAILING;
    
    state->columns[col_index].header_alignment = align;
    InvalidateRect(hGrid, nullptr, TRUE);
}

// 设置列单元格对齐方式 (0=左对齐, 1=居中, 2=右对齐)
__declspec(dllexport) void __stdcall DataGrid_SetColumnCellAlignment(HWND hGrid, int col_index, int alignment) {
    auto it = g_datagrids.find(hGrid);
    if (it == g_datagrids.end()) return;
    DataGridViewState* state = it->second;
    if (col_index < 0 || col_index >= (int)state->columns.size()) return;
    
    DWRITE_TEXT_ALIGNMENT align = DWRITE_TEXT_ALIGNMENT_LEADING;
    if (alignment == 1) align = DWRITE_TEXT_ALIGNMENT_CENTER;
    else if (alignment == 2) align = DWRITE_TEXT_ALIGNMENT_TRAILING;
    
    state->columns[col_index].cell_alignment = align;
    InvalidateRect(hGrid, nullptr, TRUE);
}

__declspec(dllexport) BOOL __stdcall DataGrid_ExportCSV(HWND hGrid, const unsigned char* file_path_bytes, int path_len) {
    auto it = g_datagrids.find(hGrid);
    if (it == g_datagrids.end()) return FALSE;
    DataGridViewState* state = it->second;

    std::wstring path = Utf8ToWide(file_path_bytes, path_len);
    FILE* fp = nullptr;
    _wfopen_s(&fp, path.c_str(), L"w,ccs=UTF-8");
    if (!fp) return FALSE;

    // Write header
    for (int c = 0; c < (int)state->columns.size(); c++) {
        if (c > 0) fwprintf(fp, L",");
        fwprintf(fp, L"\"%s\"", state->columns[c].header_text.c_str());
    }
    fwprintf(fp, L"\n");

    // Write rows
    int row_count = DataGrid_GetEffectiveRowCount(state);
    for (int r = 0; r < row_count; r++) {
        for (int c = 0; c < (int)state->columns.size(); c++) {
            if (c > 0) fwprintf(fp, L",");
            std::wstring text = DataGrid_GetCellDisplayText(state, r, c);
            // Escape quotes
            std::wstring escaped;
            for (wchar_t ch : text) {
                if (ch == L'"') escaped += L"\"\"";
                else escaped += ch;
            }
            fwprintf(fp, L"\"%s\"", escaped.c_str());
        }
        fwprintf(fp, L"\n");
    }

    fclose(fp);
    return TRUE;
}

} // extern "C" (DataGridView block end)

// ========== 通用事件回调系统 (需求 8.1-8.10) ==========

// 查找任意控件的EventCallbacks指针 (C++ linkage, not exported)
EventCallbacks* FindEventCallbacks(HWND hwnd) {
    if (!hwnd) return nullptr;

    // 按使用频率排序查找
    { auto it = g_checkboxes.find(hwnd); if (it != g_checkboxes.end()) return &it->second->events; }
    { auto it = g_radiobuttons.find(hwnd); if (it != g_radiobuttons.end()) return &it->second->events; }
    { auto it = g_listboxes.find(hwnd); if (it != g_listboxes.end()) return &it->second->events; }
    { auto it = g_comboboxes.find(hwnd); if (it != g_comboboxes.end()) return &it->second->events; }
    { auto it = g_d2d_comboboxes.find(hwnd); if (it != g_d2d_comboboxes.end()) return &it->second->events; }
    { auto it = g_editboxes.find(hwnd); if (it != g_editboxes.end()) return &it->second->events; }
    { auto it = g_labels.find(hwnd); if (it != g_labels.end()) return &it->second->events; }
    { auto it = g_progressbars.find(hwnd); if (it != g_progressbars.end()) return &it->second->events; }
    { auto it = g_pictureboxes.find(hwnd); if (it != g_pictureboxes.end()) return &it->second->events; }
    { auto it = g_hotkeys.find(hwnd); if (it != g_hotkeys.end()) return &it->second->events; }
    { auto it = g_groupboxes.find(hwnd); if (it != g_groupboxes.end()) return &it->second->events; }
    { auto it = g_datagrids.find(hwnd); if (it != g_datagrids.end()) return &it->second->events; }

    // 也检查D2DEditBox
    { auto it = g_d2d_editboxes.find(hwnd); if (it != g_d2d_editboxes.end()) return &it->second->events; }

    return nullptr;
}

extern "C" {

__declspec(dllexport) void __stdcall SetMouseEnterCallback(HWND hControl, MouseEnterCallback callback) {
    EventCallbacks* ec = FindEventCallbacks(hControl);
    if (ec) ec->on_mouse_enter = callback;
}

__declspec(dllexport) void __stdcall SetMouseLeaveCallback(HWND hControl, MouseLeaveCallback callback) {
    EventCallbacks* ec = FindEventCallbacks(hControl);
    if (ec) ec->on_mouse_leave = callback;
}

__declspec(dllexport) void __stdcall SetDoubleClickCallback(HWND hControl, DoubleClickCallback callback) {
    EventCallbacks* ec = FindEventCallbacks(hControl);
    if (ec) ec->on_double_click = callback;
}

__declspec(dllexport) void __stdcall SetRightClickCallback(HWND hControl, RightClickCallback callback) {
    EventCallbacks* ec = FindEventCallbacks(hControl);
    if (ec) ec->on_right_click = callback;
}

__declspec(dllexport) void __stdcall SetFocusCallback(HWND hControl, FocusCallback callback) {
    EventCallbacks* ec = FindEventCallbacks(hControl);
    if (ec) ec->on_focus = callback;
}

__declspec(dllexport) void __stdcall SetBlurCallback(HWND hControl, BlurCallback callback) {
    EventCallbacks* ec = FindEventCallbacks(hControl);
    if (ec) ec->on_blur = callback;
}

__declspec(dllexport) void __stdcall SetKeyDownCallback(HWND hControl, KeyDownCallback callback) {
    EventCallbacks* ec = FindEventCallbacks(hControl);
    if (ec) ec->on_key_down = callback;
}

__declspec(dllexport) void __stdcall SetKeyUpCallback(HWND hControl, KeyUpCallback callback) {
    EventCallbacks* ec = FindEventCallbacks(hControl);
    if (ec) ec->on_key_up = callback;
}

__declspec(dllexport) void __stdcall SetCharCallback(HWND hControl, CharCallback callback) {
    EventCallbacks* ec = FindEventCallbacks(hControl);
    if (ec) ec->on_char = callback;
}

__declspec(dllexport) void __stdcall SetValueChangedCallback(HWND hControl, ValueChangedCallback callback) {
    EventCallbacks* ec = FindEventCallbacks(hControl);
    if (ec) ec->on_value_changed = callback;
}

// ========== 布局管理器实现 ==========

} // extern "C" (event callbacks block end)

// 辅助函数：在WindowState中查找Emoji按钮
static EmojiButton* FindEmojiButton(HWND hParent, int button_id) {
    auto win_it = g_windows.find(hParent);
    if (win_it == g_windows.end()) return nullptr;
    for (auto& btn : win_it->second->buttons) {
        if (btn.id == button_id) return &btn;
    }
    return nullptr;
}

static LabelState* FindLabelState(HWND hwnd) {
    auto it = g_labels.find(hwnd);
    return it != g_labels.end() ? it->second : nullptr;
}

static bool IsParentDrawnLabel(HWND hwnd) {
    LabelState* label = FindLabelState(hwnd);
    return label && label->parent_drawn;
}

static void SetParentDrawnLabelBounds(HWND hwnd, int x, int y, int width, int height) {
    LabelState* label = FindLabelState(hwnd);
    if (!label) return;
    label->x = x;
    label->y = y;
    label->width = width;
    label->height = height;
}

static void ApplyDeferredWindowPos(HDWP* hdwp, HWND hChild, int x, int y, int width, int height) {
    UINT flags = SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOREDRAW;
    if (hdwp && *hdwp) {
        *hdwp = DeferWindowPos(*hdwp, hChild, nullptr, x, y, width, height, flags);
        if (*hdwp) return;
    }
    SetWindowPos(hChild, nullptr, x, y, width, height, flags);
}

// 流式布局计算（水平/垂直）
void CalculateFlowLayout(LayoutManager* lm, int client_width, int client_height, HDWP* hdwp) {
    int tb_offset = GetTitleBarOffset(lm->parent_hwnd);
    int x = lm->padding_left;
    int y = lm->padding_top + tb_offset;
    int available_width = client_width - lm->padding_left - lm->padding_right;
    int available_height = client_height - lm->padding_top - lm->padding_bottom - tb_offset;
    int line_max_size = 0;

    for (auto& item : lm->item_order) {
        LayoutProperties props = {};
        int cw = 0, ch = 0;

        if (item.is_button) {
            EmojiButton* btn = FindEmojiButton(lm->parent_hwnd, item.button_id);
            if (!btn) continue;
            auto prop_it = lm->button_props.find(item.button_id);
            if (prop_it != lm->button_props.end()) props = prop_it->second;
            cw = btn->width;
            ch = btn->height;

            if (lm->type == LAYOUT_FLOW_HORIZONTAL) {
                int needed_width = props.margin_left + cw + props.margin_right;
                if (x + needed_width > lm->padding_left + available_width && x > lm->padding_left) {
                    x = lm->padding_left;
                    y += line_max_size + lm->spacing;
                    line_max_size = 0;
                }
                int ctrl_h = props.stretch_vertical ? (available_height - props.margin_top - props.margin_bottom) : ch;
                btn->x = x + props.margin_left;
                btn->y = y + props.margin_top;
                btn->height = ctrl_h;
                int total_h = props.margin_top + ctrl_h + props.margin_bottom;
                if (total_h > line_max_size) line_max_size = total_h;
                x += needed_width + lm->spacing;
            } else {
                int needed_height = props.margin_top + ch + props.margin_bottom;
                if (y + needed_height > lm->padding_top + tb_offset + available_height && y > lm->padding_top + tb_offset) {
                    y = lm->padding_top + tb_offset;
                    x += line_max_size + lm->spacing;
                    line_max_size = 0;
                }
                int ctrl_w = props.stretch_horizontal ? (available_width - props.margin_left - props.margin_right) : cw;
                btn->x = x + props.margin_left;
                btn->y = y + props.margin_top;
                btn->width = ctrl_w;
                int total_w = props.margin_left + ctrl_w + props.margin_right;
                if (total_w > line_max_size) line_max_size = total_w;
                y += needed_height + lm->spacing;
            }
        } else {
            HWND hChild = item.hwnd;
            if (!IsWindow(hChild) || (!IsWindowVisible(hChild) && !IsParentDrawnLabel(hChild))) continue;
            auto prop_it = lm->control_props.find(hChild);
            if (prop_it != lm->control_props.end()) props = prop_it->second;

            if (IsParentDrawnLabel(hChild)) {
                LabelState* label = FindLabelState(hChild);
                cw = label->width;
                ch = label->height;
            } else {
                RECT rc;
                GetWindowRect(hChild, &rc);
                HWND hParent = GetParent(hChild);
                MapWindowPoints(HWND_DESKTOP, hParent, (LPPOINT)&rc, 2);
                cw = rc.right - rc.left;
                ch = rc.bottom - rc.top;
            }

            if (lm->type == LAYOUT_FLOW_HORIZONTAL) {
                int needed_width = props.margin_left + cw + props.margin_right;
                if (x + needed_width > lm->padding_left + available_width && x > lm->padding_left) {
                    x = lm->padding_left;
                    y += line_max_size + lm->spacing;
                    line_max_size = 0;
                }
                int ctrl_h = props.stretch_vertical ? (available_height - props.margin_top - props.margin_bottom) : ch;
                if (IsParentDrawnLabel(hChild)) SetParentDrawnLabelBounds(hChild, x + props.margin_left, y + props.margin_top, cw, ctrl_h);
                else ApplyDeferredWindowPos(hdwp, hChild, x + props.margin_left, y + props.margin_top, cw, ctrl_h);
                int total_h = props.margin_top + ctrl_h + props.margin_bottom;
                if (total_h > line_max_size) line_max_size = total_h;
                x += needed_width + lm->spacing;
            } else {
                int needed_height = props.margin_top + ch + props.margin_bottom;
                if (y + needed_height > lm->padding_top + tb_offset + available_height && y > lm->padding_top + tb_offset) {
                    y = lm->padding_top + tb_offset;
                    x += line_max_size + lm->spacing;
                    line_max_size = 0;
                }
                int ctrl_w = props.stretch_horizontal ? (available_width - props.margin_left - props.margin_right) : cw;
                if (IsParentDrawnLabel(hChild)) SetParentDrawnLabelBounds(hChild, x + props.margin_left, y + props.margin_top, ctrl_w, ch);
                else ApplyDeferredWindowPos(hdwp, hChild, x + props.margin_left, y + props.margin_top, ctrl_w, ch);
                int total_w = props.margin_left + ctrl_w + props.margin_right;
                if (total_w > line_max_size) line_max_size = total_w;
                y += needed_height + lm->spacing;
            }
        }
    }
}

// 网格布局计算
void CalculateGridLayout(LayoutManager* lm, int client_width, int client_height, HDWP* hdwp) {
    if (lm->rows <= 0 || lm->columns <= 0) return;
    int tb_offset = GetTitleBarOffset(lm->parent_hwnd);

    int available_width = client_width - lm->padding_left - lm->padding_right;
    int available_height = client_height - lm->padding_top - lm->padding_bottom - tb_offset;

    // 计算每个单元格的大小（减去间距）
    int total_h_spacing = (lm->columns - 1) * lm->spacing;
    int total_v_spacing = (lm->rows - 1) * lm->spacing;
    int cell_width = (available_width - total_h_spacing) / lm->columns;
    int cell_height = (available_height - total_v_spacing) / lm->rows;

    if (cell_width < 0) cell_width = 0;
    if (cell_height < 0) cell_height = 0;

    int index = 0;
    for (auto& item : lm->item_order) {
        int row = index / lm->columns;
        int col = index % lm->columns;
        if (row >= lm->rows) break;

        LayoutProperties props = {};

        if (item.is_button) {
            // Emoji按钮
            EmojiButton* btn = FindEmojiButton(lm->parent_hwnd, item.button_id);
            if (!btn) continue;

            auto prop_it = lm->button_props.find(item.button_id);
            if (prop_it != lm->button_props.end()) props = prop_it->second;

            int cell_x = lm->padding_left + col * (cell_width + lm->spacing);
            int cell_y = lm->padding_top + tb_offset + row * (cell_height + lm->spacing);

            int ctrl_x = cell_x + props.margin_left;
            int ctrl_y = cell_y + props.margin_top;
            int ctrl_w = props.stretch_horizontal ? (cell_width - props.margin_left - props.margin_right) : btn->width;
            int ctrl_h = props.stretch_vertical ? (cell_height - props.margin_top - props.margin_bottom) : btn->height;
            if (ctrl_w < 0) ctrl_w = 0;
            if (ctrl_h < 0) ctrl_h = 0;

            btn->x = ctrl_x;
            btn->y = ctrl_y;
            btn->width = ctrl_w;
            btn->height = ctrl_h;
        } else {
            // HWND控件
            HWND hChild = item.hwnd;
            if (!IsWindow(hChild) || (!IsWindowVisible(hChild) && !IsParentDrawnLabel(hChild))) continue;

            auto prop_it = lm->control_props.find(hChild);
            if (prop_it != lm->control_props.end()) props = prop_it->second;

            int cell_x = lm->padding_left + col * (cell_width + lm->spacing);
            int cell_y = lm->padding_top + tb_offset + row * (cell_height + lm->spacing);

            int ctrl_x = cell_x + props.margin_left;
            int ctrl_y = cell_y + props.margin_top;
            int ctrl_w, ctrl_h;

            if (props.stretch_horizontal) {
                ctrl_w = cell_width - props.margin_left - props.margin_right;
            } else {
                if (!IsParentDrawnLabel(hChild)) {
                    RECT rc;
                    GetWindowRect(hChild, &rc);
                    ctrl_w = rc.right - rc.left;
                } else {
                    LabelState* label = FindLabelState(hChild);
                    ctrl_w = label->width;
                }
            }

            if (props.stretch_vertical) {
                ctrl_h = cell_height - props.margin_top - props.margin_bottom;
            } else {
                if (!IsParentDrawnLabel(hChild)) {
                    RECT rc;
                    GetWindowRect(hChild, &rc);
                    ctrl_h = rc.bottom - rc.top;
                } else {
                    LabelState* label = FindLabelState(hChild);
                    ctrl_h = label->height;
                }
            }

            if (ctrl_w < 0) ctrl_w = 0;
            if (ctrl_h < 0) ctrl_h = 0;

            if (IsParentDrawnLabel(hChild)) SetParentDrawnLabelBounds(hChild, ctrl_x, ctrl_y, ctrl_w, ctrl_h);
            else ApplyDeferredWindowPos(hdwp, hChild, ctrl_x, ctrl_y, ctrl_w, ctrl_h);
        }
        index++;
    }
}

// 停靠布局计算
void CalculateDockLayout(LayoutManager* lm, int client_width, int client_height, HDWP* hdwp) {
    // 可用区域（被停靠控件逐步缩小）
    int tb_offset = GetTitleBarOffset(lm->parent_hwnd);
    int left = lm->padding_left;
    int top = lm->padding_top + tb_offset;
    int right = client_width - lm->padding_right;
    int bottom = client_height - lm->padding_bottom;

    // 收集FILL项（最后处理）
    std::vector<LayoutItem> fill_items;

    for (auto& item : lm->item_order) {
        LayoutProperties props = {};
        int cw = 0, ch = 0;
        bool is_btn = item.is_button;

        EmojiButton* btn = nullptr;
        HWND hChild = nullptr;

        if (is_btn) {
            btn = FindEmojiButton(lm->parent_hwnd, item.button_id);
            if (!btn) continue;
            auto prop_it = lm->button_props.find(item.button_id);
            if (prop_it != lm->button_props.end()) props = prop_it->second;
            cw = btn->width;
            ch = btn->height;
        } else {
            hChild = item.hwnd;
            if (!IsWindow(hChild) || (!IsWindowVisible(hChild) && !IsParentDrawnLabel(hChild))) continue;
            auto prop_it = lm->control_props.find(hChild);
            if (prop_it != lm->control_props.end()) props = prop_it->second;
            if (IsParentDrawnLabel(hChild)) {
                LabelState* label = FindLabelState(hChild);
                cw = label->width;
                ch = label->height;
            } else {
                RECT rc;
                GetWindowRect(hChild, &rc);
                cw = rc.right - rc.left;
                ch = rc.bottom - rc.top;
            }
        }

        if (props.dock == DOCK_FILL) {
            fill_items.push_back(item);
            continue;
        }

        int ctrl_x, ctrl_y, ctrl_w, ctrl_h;

        switch (props.dock) {
        case DOCK_TOP:
            ctrl_x = left + props.margin_left;
            ctrl_y = top + props.margin_top;
            ctrl_w = (right - left) - props.margin_left - props.margin_right;
            ctrl_h = ch;
            if (ctrl_w < 0) ctrl_w = 0;
            if (is_btn) { btn->x = ctrl_x; btn->y = ctrl_y; btn->width = ctrl_w; btn->height = ctrl_h; }
            else if (IsParentDrawnLabel(hChild)) { SetParentDrawnLabelBounds(hChild, ctrl_x, ctrl_y, ctrl_w, ctrl_h); }
            else { ApplyDeferredWindowPos(hdwp, hChild, ctrl_x, ctrl_y, ctrl_w, ctrl_h); }
            top += props.margin_top + ctrl_h + props.margin_bottom + lm->spacing;
            break;
        case DOCK_BOTTOM:
            ctrl_w = (right - left) - props.margin_left - props.margin_right;
            ctrl_h = ch;
            ctrl_x = left + props.margin_left;
            ctrl_y = bottom - props.margin_bottom - ctrl_h;
            if (ctrl_w < 0) ctrl_w = 0;
            if (is_btn) { btn->x = ctrl_x; btn->y = ctrl_y; btn->width = ctrl_w; btn->height = ctrl_h; }
            else if (IsParentDrawnLabel(hChild)) { SetParentDrawnLabelBounds(hChild, ctrl_x, ctrl_y, ctrl_w, ctrl_h); }
            else { ApplyDeferredWindowPos(hdwp, hChild, ctrl_x, ctrl_y, ctrl_w, ctrl_h); }
            bottom -= props.margin_bottom + ctrl_h + props.margin_top + lm->spacing;
            break;
        case DOCK_LEFT:
            ctrl_x = left + props.margin_left;
            ctrl_y = top + props.margin_top;
            ctrl_w = cw;
            ctrl_h = (bottom - top) - props.margin_top - props.margin_bottom;
            if (ctrl_h < 0) ctrl_h = 0;
            if (is_btn) { btn->x = ctrl_x; btn->y = ctrl_y; btn->width = ctrl_w; btn->height = ctrl_h; }
            else if (IsParentDrawnLabel(hChild)) { SetParentDrawnLabelBounds(hChild, ctrl_x, ctrl_y, ctrl_w, ctrl_h); }
            else { ApplyDeferredWindowPos(hdwp, hChild, ctrl_x, ctrl_y, ctrl_w, ctrl_h); }
            left += props.margin_left + ctrl_w + props.margin_right + lm->spacing;
            break;
        case DOCK_RIGHT:
            ctrl_w = cw;
            ctrl_h = (bottom - top) - props.margin_top - props.margin_bottom;
            ctrl_x = right - props.margin_right - ctrl_w;
            ctrl_y = top + props.margin_top;
            if (ctrl_h < 0) ctrl_h = 0;
            if (is_btn) { btn->x = ctrl_x; btn->y = ctrl_y; btn->width = ctrl_w; btn->height = ctrl_h; }
            else if (IsParentDrawnLabel(hChild)) { SetParentDrawnLabelBounds(hChild, ctrl_x, ctrl_y, ctrl_w, ctrl_h); }
            else { ApplyDeferredWindowPos(hdwp, hChild, ctrl_x, ctrl_y, ctrl_w, ctrl_h); }
            right -= props.margin_right + ctrl_w + props.margin_left + lm->spacing;
            break;
        default:
            break;
        }
    }

    // 处理FILL项 - 填充剩余空间
    for (auto& item : fill_items) {
        LayoutProperties props = {};
        if (item.is_button) {
            EmojiButton* btn = FindEmojiButton(lm->parent_hwnd, item.button_id);
            if (!btn) continue;
            auto prop_it = lm->button_props.find(item.button_id);
            if (prop_it != lm->button_props.end()) props = prop_it->second;
            btn->x = left + props.margin_left;
            btn->y = top + props.margin_top;
            btn->width = (right - left) - props.margin_left - props.margin_right;
            btn->height = (bottom - top) - props.margin_top - props.margin_bottom;
            if (btn->width < 0) btn->width = 0;
            if (btn->height < 0) btn->height = 0;
        } else {
            HWND hChild = item.hwnd;
            if (!IsWindow(hChild)) continue;
            auto prop_it = lm->control_props.find(hChild);
            if (prop_it != lm->control_props.end()) props = prop_it->second;
            int ctrl_x = left + props.margin_left;
            int ctrl_y = top + props.margin_top;
            int ctrl_w = (right - left) - props.margin_left - props.margin_right;
            int ctrl_h = (bottom - top) - props.margin_top - props.margin_bottom;
            if (ctrl_w < 0) ctrl_w = 0;
            if (ctrl_h < 0) ctrl_h = 0;
            if (IsParentDrawnLabel(hChild)) SetParentDrawnLabelBounds(hChild, ctrl_x, ctrl_y, ctrl_w, ctrl_h);
            else ApplyDeferredWindowPos(hdwp, hChild, ctrl_x, ctrl_y, ctrl_w, ctrl_h);
        }
    }
}

// 设置窗口的布局管理器
extern "C" {

__declspec(dllexport) void __stdcall SetLayoutManager(
    HWND hParent, int layout_type, int rows, int columns, int spacing)
{
    if (!IsWindow(hParent)) return;

    // 如果已有布局管理器，先删除
    auto it = g_layout_managers.find(hParent);
    if (it != g_layout_managers.end()) {
        delete it->second;
        g_layout_managers.erase(it);
    }

    if (layout_type == LAYOUT_NONE) return;

    LayoutManager* lm = new LayoutManager();
    lm->parent_hwnd = hParent;
    lm->type = (LayoutType)layout_type;
    lm->rows = rows;
    lm->columns = columns;
    lm->spacing = spacing;
    g_layout_managers[hParent] = lm;
}

// 设置布局管理器的内边距
__declspec(dllexport) void __stdcall SetLayoutPadding(
    HWND hParent, int padding_left, int padding_top, int padding_right, int padding_bottom)
{
    auto it = g_layout_managers.find(hParent);
    if (it == g_layout_managers.end()) return;

    LayoutManager* lm = it->second;
    lm->padding_left = padding_left;
    lm->padding_top = padding_top;
    lm->padding_right = padding_right;
    lm->padding_bottom = padding_bottom;
}

// 设置控件的布局属性
__declspec(dllexport) void __stdcall SetControlLayoutProps(
    HWND hControl, int margin_left, int margin_top, int margin_right, int margin_bottom,
    int dock_position, BOOL stretch_horizontal, BOOL stretch_vertical)
{
    // 先检查是否是Emoji按钮ID（优先级高于IsWindow，因为按钮ID可能恰好是有效HWND值）
    int button_id = (int)(intptr_t)hControl;
    for (auto& pair : g_layout_managers) {
        LayoutManager* lm = pair.second;
        for (auto& item : lm->item_order) {
            if (item.is_button && item.button_id == button_id) {
                LayoutProperties& props = lm->button_props[button_id];
                props.margin_left = margin_left;
                props.margin_top = margin_top;
                props.margin_right = margin_right;
                props.margin_bottom = margin_bottom;
                props.dock = (DockPosition)dock_position;
                props.stretch_horizontal = stretch_horizontal ? true : false;
                props.stretch_vertical = stretch_vertical ? true : false;
                return;
            }
        }
    }

    // 不是Emoji按钮，尝试作为HWND控件处理
    if (IsWindow(hControl)) {
        HWND hParent = GetParent(hControl);
        if (!hParent) return;

        auto it = g_layout_managers.find(hParent);
        if (it == g_layout_managers.end()) return;

        LayoutManager* lm = it->second;
        LayoutProperties& props = lm->control_props[hControl];
        props.margin_left = margin_left;
        props.margin_top = margin_top;
        props.margin_right = margin_right;
        props.margin_bottom = margin_bottom;
        props.dock = (DockPosition)dock_position;
        props.stretch_horizontal = stretch_horizontal ? true : false;
        props.stretch_vertical = stretch_vertical ? true : false;
    }
}

// 将控件添加到布局管理器
__declspec(dllexport) void __stdcall AddControlToLayout(HWND hParent, HWND hControl) {
    if (!IsWindow(hParent)) return;

    auto it = g_layout_managers.find(hParent);
    if (it == g_layout_managers.end()) return;

    LayoutManager* lm = it->second;

    // 先检查是否是Emoji按钮ID（优先级高于IsWindow，因为按钮ID可能恰好是有效HWND值）
    int button_id = (int)(intptr_t)hControl;
    auto win_it = g_windows.find(hParent);
    bool is_emoji_button = false;
    if (win_it != g_windows.end()) {
        WindowState* ws = win_it->second;
        for (auto& btn : ws->buttons) {
            if (btn.id == button_id) { is_emoji_button = true; break; }
        }
    }

    if (is_emoji_button) {
        // Emoji按钮
        // 检查是否已添加
        for (auto& item : lm->item_order) {
            if (item.is_button && item.button_id == button_id) return;
        }
        LayoutItem item;
        item.button_id = button_id;
        item.is_button = true;
        lm->item_order.push_back(item);
        if (lm->button_props.find(button_id) == lm->button_props.end()) {
            lm->button_props[button_id] = LayoutProperties();
        }
    } else if (IsWindow(hControl)) {
        // HWND控件
        for (HWND h : lm->control_order) {
            if (h == hControl) return;
        }
        lm->control_order.push_back(hControl);
        if (lm->control_props.find(hControl) == lm->control_props.end()) {
            lm->control_props[hControl] = LayoutProperties();
        }
        // 如果是标签，则切换为父窗口统一绘制，避免 live resize 时子窗口闪烁
        auto label_it = g_labels.find(hControl);
        if (label_it != g_labels.end()) {
            label_it->second->parent_drawn = true;
            label_it->second->visible = IsWindowVisible(hControl) ? true : false;
            ShowWindow(hControl, SW_HIDE);
        }
        // 同步到统一顺序列表
        LayoutItem item;
        item.hwnd = hControl;
        item.is_button = false;
        lm->item_order.push_back(item);
    }
}

// 从布局管理器移除控件
__declspec(dllexport) void __stdcall RemoveControlFromLayout(HWND hParent, HWND hControl) {
    auto it = g_layout_managers.find(hParent);
    if (it == g_layout_managers.end()) return;

    LayoutManager* lm = it->second;

    // 先检查是否是Emoji按钮ID（优先级高于IsWindow）
    int button_id = (int)(intptr_t)hControl;
    LayoutItem btn_target;
    btn_target.button_id = button_id;
    btn_target.is_button = true;
    auto btn_it = std::find(lm->item_order.begin(), lm->item_order.end(), btn_target);
    if (btn_it != lm->item_order.end()) {
        lm->item_order.erase(btn_it);
        lm->button_props.erase(button_id);
        return;
    }

    // 不是Emoji按钮，尝试作为HWND控件处理
    if (IsWindow(hControl)) {
        auto label_it = g_labels.find(hControl);
        if (label_it != g_labels.end()) {
            label_it->second->parent_drawn = false;
            ShowWindow(hControl, label_it->second->visible ? SW_SHOW : SW_HIDE);
        }
        auto order_it = std::find(lm->control_order.begin(), lm->control_order.end(), hControl);
        if (order_it != lm->control_order.end()) {
            lm->control_order.erase(order_it);
        }
        lm->control_props.erase(hControl);
        LayoutItem target;
        target.hwnd = hControl;
        target.is_button = false;
        auto item_it = std::find(lm->item_order.begin(), lm->item_order.end(), target);
        if (item_it != lm->item_order.end()) lm->item_order.erase(item_it);
    }
}

// 立即重新计算布局
__declspec(dllexport) void __stdcall UpdateLayout(HWND hParent) {
    auto it = g_layout_managers.find(hParent);
    if (it == g_layout_managers.end()) return;

    LayoutManager* lm = it->second;

    RECT rc;
    GetClientRect(hParent, &rc);
    int client_width = rc.right - rc.left;
    int client_height = rc.bottom - rc.top;

    HDWP hdwp = nullptr;
    int child_count = 0;
    for (auto& item : lm->item_order) {
        if (!item.is_button && item.hwnd && IsWindow(item.hwnd) && (IsWindowVisible(item.hwnd) || IsParentDrawnLabel(item.hwnd))) {
            child_count++;
        }
    }
    if (child_count > 0) {
        hdwp = BeginDeferWindowPos(child_count);
    }

    switch (lm->type) {
    case LAYOUT_FLOW_HORIZONTAL:
    case LAYOUT_FLOW_VERTICAL:
        CalculateFlowLayout(lm, client_width, client_height, &hdwp);
        break;
    case LAYOUT_GRID:
        CalculateGridLayout(lm, client_width, client_height, &hdwp);
        break;
    case LAYOUT_DOCK:
        CalculateDockLayout(lm, client_width, client_height, &hdwp);
        break;
    default:
        break;
    }

    if (hdwp) {
        EndDeferWindowPos(hdwp);
    }

    // 布局更新后，统一触发子控件重绘，避免缩放时逐个 SetWindowPos 造成闪烁
    bool has_buttons = false;
    bool has_parent_drawn_labels = false;
    for (auto& item : lm->item_order) {
        if (item.is_button) {
            has_buttons = true;
        } else if (item.hwnd && IsWindow(item.hwnd)) {
            if (IsParentDrawnLabel(item.hwnd)) {
                has_parent_drawn_labels = true;
            } else if (IsWindowVisible(item.hwnd)) {
                InvalidateRect(item.hwnd, nullptr, TRUE);
            }
        }
    }
    if (has_buttons || has_parent_drawn_labels) {
        InvalidateRect(hParent, nullptr, FALSE);
    }
}

// 移除窗口的布局管理器
__declspec(dllexport) void __stdcall RemoveLayoutManager(HWND hParent) {
    auto it = g_layout_managers.find(hParent);
    if (it != g_layout_managers.end()) {
        delete it->second;
        g_layout_managers.erase(it);
    }
}

} // extern "C" (layout manager block end)

// ========== 主题系统实现 ==========

// 创建默认亮色主题
Theme CreateDefaultLightTheme() {
    Theme theme;
    theme.name = L"light";
    theme.dark_mode = false;

    // Element UI 标准亮色配色
    theme.colors.primary = 0xFF409EFF;
    theme.colors.success = 0xFF67C23A;
    theme.colors.warning = 0xFFE6A23C;
    theme.colors.danger = 0xFFF56C6C;
    theme.colors.info = 0xFF909399;
    theme.colors.text_primary = 0xFF303133;
    theme.colors.text_regular = 0xFF606266;
    theme.colors.text_secondary = 0xFF909399;
    theme.colors.text_placeholder = 0xFFC0C4CC;
    theme.colors.border_base = 0xFFDCDFE6;
    theme.colors.border_light = 0xFFE4E7ED;
    theme.colors.border_lighter = 0xFFEBEEF5;
    theme.colors.border_extra_light = 0xFFF2F6FC;
    theme.colors.background = 0xFFFFFFFF;
    theme.colors.background_light = 0xFFF5F7FA;

    theme.fonts.title_font = L"Microsoft YaHei UI";
    theme.fonts.body_font = L"Microsoft YaHei UI";
    theme.fonts.mono_font = L"Consolas";
    theme.fonts.title_size = 16;
    theme.fonts.body_size = 14;
    theme.fonts.small_size = 12;

    theme.sizes.border_radius = 4.0f;
    theme.sizes.border_width = 1.0f;
    theme.sizes.control_height = 32;
    theme.sizes.spacing_small = 8;
    theme.sizes.spacing_medium = 16;
    theme.sizes.spacing_large = 24;

    return theme;
}

// 创建默认暗色主题
Theme CreateDefaultDarkTheme() {
    Theme theme;
    theme.name = L"dark";
    theme.dark_mode = true;

    // 暗色主题配色
    theme.colors.primary = 0xFF409EFF;
    theme.colors.success = 0xFF67C23A;
    theme.colors.warning = 0xFFE6A23C;
    theme.colors.danger = 0xFFF56C6C;
    theme.colors.info = 0xFF909399;
    theme.colors.text_primary = 0xFFE5EAF3;
    theme.colors.text_regular = 0xFFCFD3DC;
    theme.colors.text_secondary = 0xFFA3A6AD;
    theme.colors.text_placeholder = 0xFF8D9095;
    theme.colors.border_base = 0xFF4C4D4F;
    theme.colors.border_light = 0xFF414243;
    theme.colors.border_lighter = 0xFF363637;
    theme.colors.border_extra_light = 0xFF2B2B2C;
    theme.colors.background = 0xFF1D1E1F;
    theme.colors.background_light = 0xFF262727;

    theme.fonts.title_font = L"Microsoft YaHei UI";
    theme.fonts.body_font = L"Microsoft YaHei UI";
    theme.fonts.mono_font = L"Consolas";
    theme.fonts.title_size = 16;
    theme.fonts.body_size = 14;
    theme.fonts.small_size = 12;

    theme.sizes.border_radius = 4.0f;
    theme.sizes.border_width = 1.0f;
    theme.sizes.control_height = 32;
    theme.sizes.spacing_small = 8;
    theme.sizes.spacing_medium = 16;
    theme.sizes.spacing_large = 24;

    return theme;
}

// 初始化主题系统
static void EnsureThemesInitialized() {
    if (!g_themes_initialized) {
        g_light_theme = CreateDefaultLightTheme();
        g_dark_theme = CreateDefaultDarkTheme();
        g_current_theme = &g_light_theme;
        g_themes_initialized = true;
    }
}

// 获取当前主题颜色（带自动初始化）
static UINT32 GetCurrentThemeColorValue(const std::string& name) {
    EnsureThemesInitialized();
    const ThemeColors& c = g_current_theme->colors;
    if (name == "primary") return c.primary;
    if (name == "success") return c.success;
    if (name == "warning") return c.warning;
    if (name == "danger") return c.danger;
    if (name == "info") return c.info;
    if (name == "text_primary") return c.text_primary;
    if (name == "text_regular") return c.text_regular;
    if (name == "text_secondary") return c.text_secondary;
    if (name == "text_placeholder") return c.text_placeholder;
    if (name == "border_base") return c.border_base;
    if (name == "border_light") return c.border_light;
    if (name == "border_lighter") return c.border_lighter;
    if (name == "border_extra_light") return c.border_extra_light;
    if (name == "background") return c.background;
    if (name == "background_light") return c.background_light;
    return 0;
}

// 简易JSON解析器 - 解析主题JSON
static bool ParseThemeJSON(const std::string& json, Theme& theme) {
    // 简易JSON解析：查找 "key": value 模式
    auto findString = [&](const std::string& key) -> std::string {
        std::string search = "\"" + key + "\"";
        size_t pos = json.find(search);
        if (pos == std::string::npos) return "";
        pos = json.find(':', pos);
        if (pos == std::string::npos) return "";
        pos = json.find('"', pos + 1);
        if (pos == std::string::npos) return "";
        size_t end = json.find('"', pos + 1);
        if (end == std::string::npos) return "";
        return json.substr(pos + 1, end - pos - 1);
    };

    auto findInt = [&](const std::string& key, int default_val) -> int {
        std::string search = "\"" + key + "\"";
        size_t pos = json.find(search);
        if (pos == std::string::npos) return default_val;
        pos = json.find(':', pos);
        if (pos == std::string::npos) return default_val;
        pos++;
        while (pos < json.size() && (json[pos] == ' ' || json[pos] == '\t')) pos++;
        try { return std::stoi(json.substr(pos)); }
        catch (...) { return default_val; }
    };

    auto findFloat = [&](const std::string& key, float default_val) -> float {
        std::string search = "\"" + key + "\"";
        size_t pos = json.find(search);
        if (pos == std::string::npos) return default_val;
        pos = json.find(':', pos);
        if (pos == std::string::npos) return default_val;
        pos++;
        while (pos < json.size() && (json[pos] == ' ' || json[pos] == '\t')) pos++;
        try { return std::stof(json.substr(pos)); }
        catch (...) { return default_val; }
    };

    auto findBool = [&](const std::string& key, bool default_val) -> bool {
        std::string search = "\"" + key + "\"";
        size_t pos = json.find(search);
        if (pos == std::string::npos) return default_val;
        pos = json.find(':', pos);
        if (pos == std::string::npos) return default_val;
        pos++;
        while (pos < json.size() && (json[pos] == ' ' || json[pos] == '\t')) pos++;
        if (json.substr(pos, 4) == "true") return true;
        if (json.substr(pos, 5) == "false") return false;
        return default_val;
    };

    // 解析颜色字符串 "#RRGGBB" -> 0xAARRGGBB
    auto parseColor = [&](const std::string& key, UINT32 default_val) -> UINT32 {
        std::string val = findString(key);
        if (val.empty() || val[0] != '#' || val.size() < 7) return default_val;
        try {
            unsigned long rgb = std::stoul(val.substr(1), nullptr, 16);
            return 0xFF000000 | (UINT32)rgb;
        }
        catch (...) { return default_val; }
    };

    // 先用默认亮色主题作为基础
    Theme base = CreateDefaultLightTheme();

    // 解析名称和模式
    std::string name = findString("name");
    if (!name.empty()) {
        theme.name = std::wstring(name.begin(), name.end());
    } else {
        theme.name = base.name;
    }
    theme.dark_mode = findBool("dark_mode", base.dark_mode);

    // 解析颜色
    theme.colors.primary = parseColor("primary", base.colors.primary);
    theme.colors.success = parseColor("success", base.colors.success);
    theme.colors.warning = parseColor("warning", base.colors.warning);
    theme.colors.danger = parseColor("danger", base.colors.danger);
    theme.colors.info = parseColor("info", base.colors.info);
    theme.colors.text_primary = parseColor("text_primary", base.colors.text_primary);
    theme.colors.text_regular = parseColor("text_regular", base.colors.text_regular);
    theme.colors.text_secondary = parseColor("text_secondary", base.colors.text_secondary);
    theme.colors.text_placeholder = parseColor("text_placeholder", base.colors.text_placeholder);
    theme.colors.border_base = parseColor("border_base", base.colors.border_base);
    theme.colors.border_light = parseColor("border_light", base.colors.border_light);
    theme.colors.border_lighter = parseColor("border_lighter", base.colors.border_lighter);
    theme.colors.border_extra_light = parseColor("border_extra_light", base.colors.border_extra_light);
    theme.colors.background = parseColor("background", base.colors.background);
    theme.colors.background_light = parseColor("background_light", base.colors.background_light);

    // 解析字体
    std::string title_font = findString("title_font");
    std::string body_font = findString("body_font");
    std::string mono_font = findString("mono_font");
    theme.fonts.title_font = title_font.empty() ? base.fonts.title_font : std::wstring(title_font.begin(), title_font.end());
    theme.fonts.body_font = body_font.empty() ? base.fonts.body_font : std::wstring(body_font.begin(), body_font.end());
    theme.fonts.mono_font = mono_font.empty() ? base.fonts.mono_font : std::wstring(mono_font.begin(), mono_font.end());
    theme.fonts.title_size = findInt("title_size", base.fonts.title_size);
    theme.fonts.body_size = findInt("body_size", base.fonts.body_size);
    theme.fonts.small_size = findInt("small_size", base.fonts.small_size);

    // 解析尺寸
    theme.sizes.border_radius = findFloat("border_radius", base.sizes.border_radius);
    theme.sizes.border_width = findFloat("border_width", base.sizes.border_width);
    theme.sizes.control_height = findInt("control_height", base.sizes.control_height);
    theme.sizes.spacing_small = findInt("spacing_small", base.sizes.spacing_small);
    theme.sizes.spacing_medium = findInt("spacing_medium", base.sizes.spacing_medium);
    theme.sizes.spacing_large = findInt("spacing_large", base.sizes.spacing_large);

    return true;
}

// 刷新所有控件（主题切换后调用）
void ApplyThemeToAllControls() {
    // 刷新所有窗口
    for (auto& pair : g_windows) {
        InvalidateRect(pair.first, NULL, TRUE);
    }
    // 刷新所有标签
    for (auto& pair : g_labels) {
        InvalidateRect(pair.first, NULL, TRUE);
    }
    // 刷新所有复选框
    for (auto& pair : g_checkboxes) {
        InvalidateRect(pair.first, NULL, TRUE);
    }
    // 刷新所有进度条
    for (auto& pair : g_progressbars) {
        InvalidateRect(pair.first, NULL, TRUE);
    }
    // 刷新所有图片框
    for (auto& pair : g_pictureboxes) {
        InvalidateRect(pair.first, NULL, TRUE);
    }
    // 刷新所有单选按钮
    for (auto& pair : g_radiobuttons) {
        InvalidateRect(pair.first, NULL, TRUE);
    }
    // 刷新所有列表框
    for (auto& pair : g_listboxes) {
        InvalidateRect(pair.first, NULL, TRUE);
    }
    // 刷新所有组合框
    for (auto& pair : g_comboboxes) {
        InvalidateRect(pair.first, NULL, TRUE);
    }
    // 刷新所有D2D组合框
    for (auto& pair : g_d2d_comboboxes) {
        InvalidateRect(pair.first, NULL, TRUE);
    }
    // 刷新所有热键控件
    for (auto& pair : g_hotkeys) {
        InvalidateRect(pair.first, NULL, TRUE);
    }
    // 刷新所有分组框
    for (auto& pair : g_groupboxes) {
        InvalidateRect(pair.first, NULL, TRUE);
    }
    // 刷新所有DataGridView
    for (auto& pair : g_datagrids) {
        InvalidateRect(pair.first, NULL, TRUE);
    }
}

// ========== 主题系统 API 实现 ==========
extern "C" {

// 从JSON字符串加载主题
__declspec(dllexport) BOOL __stdcall LoadThemeFromJSON(
    const unsigned char* json_bytes,
    int json_len
) {
    EnsureThemesInitialized();
    if (!json_bytes || json_len <= 0) return FALSE;

    std::string json((const char*)json_bytes, json_len);
    Theme new_theme;
    if (!ParseThemeJSON(json, new_theme)) return FALSE;

    // 根据名称决定存储位置
    if (new_theme.name == L"light") {
        g_light_theme = new_theme;
        if (g_current_theme == &g_light_theme || (!g_current_theme->dark_mode)) {
            g_current_theme = &g_light_theme;
            ApplyThemeToAllControls();
        }
    } else if (new_theme.name == L"dark") {
        g_dark_theme = new_theme;
        if (g_current_theme == &g_dark_theme || g_current_theme->dark_mode) {
            g_current_theme = &g_dark_theme;
            ApplyThemeToAllControls();
        }
    } else {
        // 自定义主题 - 存储到当前主题
        if (new_theme.dark_mode) {
            g_dark_theme = new_theme;
            g_current_theme = &g_dark_theme;
        } else {
            g_light_theme = new_theme;
            g_current_theme = &g_light_theme;
        }
        ApplyThemeToAllControls();
    }

    return TRUE;
}

// 从文件加载主题
__declspec(dllexport) BOOL __stdcall LoadThemeFromFile(
    const unsigned char* file_path_bytes,
    int path_len
) {
    EnsureThemesInitialized();
    if (!file_path_bytes || path_len <= 0) return FALSE;

    // 转换路径
    int wlen = MultiByteToWideChar(CP_UTF8, 0, (const char*)file_path_bytes, path_len, NULL, 0);
    if (wlen <= 0) return FALSE;
    std::wstring wpath(wlen, 0);
    MultiByteToWideChar(CP_UTF8, 0, (const char*)file_path_bytes, path_len, &wpath[0], wlen);

    // 读取文件
    HANDLE hFile = CreateFileW(wpath.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
    if (hFile == INVALID_HANDLE_VALUE) return FALSE;

    DWORD fileSize = GetFileSize(hFile, NULL);
    if (fileSize == INVALID_FILE_SIZE || fileSize == 0) {
        CloseHandle(hFile);
        return FALSE;
    }

    std::vector<unsigned char> buffer(fileSize);
    DWORD bytesRead = 0;
    BOOL readOk = ReadFile(hFile, buffer.data(), fileSize, &bytesRead, NULL);
    CloseHandle(hFile);

    if (!readOk || bytesRead == 0) return FALSE;

    return LoadThemeFromJSON(buffer.data(), (int)bytesRead);
}

// 设置当前主题
__declspec(dllexport) void __stdcall SetTheme(
    const unsigned char* theme_name_bytes,
    int name_len
) {
    EnsureThemesInitialized();
    if (!theme_name_bytes || name_len <= 0) return;

    std::string name((const char*)theme_name_bytes, name_len);

    Theme* old_theme = g_current_theme;
    if (name == "light") {
        g_current_theme = &g_light_theme;
    } else if (name == "dark") {
        g_current_theme = &g_dark_theme;
    } else {
        return; // 未知主题名称
    }

    if (g_current_theme != old_theme) {
        ApplyThemeToAllControls();
        if (g_theme_changed_callback) {
            g_theme_changed_callback(name.c_str());
        }
    }
}

// 设置暗色模式
__declspec(dllexport) void __stdcall SetDarkMode(BOOL dark_mode) {
    EnsureThemesInitialized();
    Theme* old_theme = g_current_theme;
    g_current_theme = dark_mode ? &g_dark_theme : &g_light_theme;

    if (g_current_theme != old_theme) {
        ApplyThemeToAllControls();
        if (g_theme_changed_callback) {
            std::string name = dark_mode ? "dark" : "light";
            g_theme_changed_callback(name.c_str());
        }
    }
}

// 获取主题颜色（字符串版本）
__declspec(dllexport) UINT32 __stdcall EW_GetThemeColor(
    const unsigned char* color_name_bytes,
    int name_len
) {
    EnsureThemesInitialized();
    if (!color_name_bytes || name_len <= 0) return 0;
    std::string name((const char*)color_name_bytes, name_len);
    return GetCurrentThemeColorValue(name);
}

// 获取主题颜色（数值索引版本，供易语言常量使用）
// 索引: 0=primary, 1=success, 2=warning, 3=danger, 4=info,
//       5=text_primary, 6=text_regular, 7=text_secondary, 8=text_placeholder,
//       9=border_base, 10=border_light, 11=border_lighter, 12=border_extra_light,
//       13=background, 14=background_light
__declspec(dllexport) UINT32 __stdcall EW_GetThemeColorByIndex(int color_index) {
    EnsureThemesInitialized();
    const ThemeColors& c = g_current_theme->colors;
    switch (color_index) {
        case 0:  return c.primary;
        case 1:  return c.success;
        case 2:  return c.warning;
        case 3:  return c.danger;
        case 4:  return c.info;
        case 5:  return c.text_primary;
        case 6:  return c.text_regular;
        case 7:  return c.text_secondary;
        case 8:  return c.text_placeholder;
        case 9:  return c.border_base;
        case 10: return c.border_light;
        case 11: return c.border_lighter;
        case 12: return c.border_extra_light;
        case 13: return c.background;
        case 14: return c.background_light;
        default: return 0;
    }
}

// 获取主题字体名称
__declspec(dllexport) int __stdcall EW_GetThemeFont(
    int font_type,
    unsigned char* buffer,
    int buffer_size
) {
    EnsureThemesInitialized();
    if (!buffer || buffer_size <= 0) return 0;

    const std::wstring* font = nullptr;
    switch (font_type) {
    case 0: font = &g_current_theme->fonts.title_font; break;
    case 1: font = &g_current_theme->fonts.body_font; break;
    case 2: font = &g_current_theme->fonts.mono_font; break;
    default: return 0;
    }

    int utf8_len = WideCharToMultiByte(CP_UTF8, 0, font->c_str(), (int)font->size(), NULL, 0, NULL, NULL);
    if (utf8_len <= 0 || utf8_len > buffer_size) return 0;
    WideCharToMultiByte(CP_UTF8, 0, font->c_str(), (int)font->size(), (char*)buffer, buffer_size, NULL, NULL);
    return utf8_len;
}

// 获取主题字号
__declspec(dllexport) int __stdcall GetThemeFontSize(int font_type) {
    EnsureThemesInitialized();
    switch (font_type) {
    case 0: return g_current_theme->fonts.title_size;
    case 1: return g_current_theme->fonts.body_size;
    case 2: return g_current_theme->fonts.small_size;
    default: return 14;
    }
}

// 获取主题尺寸
__declspec(dllexport) int __stdcall GetThemeSize(int size_type) {
    EnsureThemesInitialized();
    switch (size_type) {
    case 0: return (int)g_current_theme->sizes.border_radius;
    case 1: return (int)g_current_theme->sizes.border_width;
    case 2: return g_current_theme->sizes.control_height;
    case 3: return g_current_theme->sizes.spacing_small;
    case 4: return g_current_theme->sizes.spacing_medium;
    case 5: return g_current_theme->sizes.spacing_large;
    default: return 0;
    }
}

// 获取当前是否暗色模式
__declspec(dllexport) BOOL __stdcall IsDarkMode() {
    EnsureThemesInitialized();
    return g_current_theme->dark_mode ? TRUE : FALSE;
}

// 获取当前主题名称
__declspec(dllexport) int __stdcall EW_GetCurrentThemeName(
    unsigned char* buffer,
    int buffer_size
) {
    EnsureThemesInitialized();
    if (!buffer || buffer_size <= 0) return 0;

    const std::wstring& name = g_current_theme->name;
    int utf8_len = WideCharToMultiByte(CP_UTF8, 0, name.c_str(), (int)name.size(), NULL, 0, NULL, NULL);
    if (utf8_len <= 0 || utf8_len > buffer_size) return 0;
    WideCharToMultiByte(CP_UTF8, 0, name.c_str(), (int)name.size(), (char*)buffer, buffer_size, NULL, NULL);
    return utf8_len;
}

// 设置主题切换回调
__declspec(dllexport) void __stdcall SetThemeChangedCallback(ThemeChangedCallback callback) {
    g_theme_changed_callback = callback;
}

} // extern "C"

// ========== 菜单系统实现 ==========

// 辅助函数：查找菜单项
static MenuItem* FindMenuItem(std::vector<MenuItem>& items, int item_id) {
    for (auto& item : items) {
        if (item.id == item_id) return &item;
        if (!item.sub_items.empty()) {
            MenuItem* found = FindMenuItem(item.sub_items, item_id);
            if (found) return found;
        }
    }
    return nullptr;
}

// 辅助函数：计算菜单项文本宽度
static float CalculateMenuItemWidth(IDWriteFactory* factory, const std::wstring& text, const FontStyle& font) {
    if (text.empty()) return 0.0f;
    
    IDWriteTextFormat* fmt = nullptr;
    factory->CreateTextFormat(
        font.font_name.c_str(),
        nullptr,
        font.bold ? DWRITE_FONT_WEIGHT_BOLD : DWRITE_FONT_WEIGHT_NORMAL,
        font.italic ? DWRITE_FONT_STYLE_ITALIC : DWRITE_FONT_STYLE_NORMAL,
        DWRITE_FONT_STRETCH_NORMAL,
        (float)font.font_size,
        L"zh-CN",
        &fmt
    );
    
    if (!fmt) return 0.0f;
    
    IDWriteTextLayout* layout = nullptr;
    factory->CreateTextLayout(
        text.c_str(),
        (UINT32)text.length(),
        fmt,
        10000.0f,
        100.0f,
        &layout
    );
    
    float width = 0.0f;
    if (layout) {
        DWRITE_TEXT_METRICS metrics;
        layout->GetMetrics(&metrics);
        width = metrics.width;
        layout->Release();
    }
    
    fmt->Release();
    return width;
}

// 创建菜单栏
__declspec(dllexport) HWND __stdcall CreateMenuBar(HWND hWindow) {
    WriteLog("CreateMenuBar: hWindow=%p", hWindow);
    if (!hWindow) {
        WriteLog("CreateMenuBar: hWindow is NULL!");
        return nullptr;
    }
    
    auto it = g_windows.find(hWindow);
    if (it == g_windows.end()) {
        WriteLog("CreateMenuBar: window not found in g_windows!");
        return nullptr;
    }
    
    MenuBarState* state = new MenuBarState();
    state->hwnd = hWindow;
    state->visible = true;
    state->bg_color = 0;
    state->font.font_name = L"Segoe UI Emoji";
    state->font.font_size = 14;
    state->font.bold = false;
    state->font.italic = false;
    state->font.underline = false;
    state->callback = nullptr;
    
    // 使用窗口句柄作为菜单栏标识
    g_menubars[hWindow] = state;
    WriteLog("CreateMenuBar: menubar created, g_menubars.size()=%zu", g_menubars.size());
    
    // 触发窗口重绘
    InvalidateRect(hWindow, NULL, FALSE);
    
    return hWindow;
}

// 销毁菜单栏
__declspec(dllexport) void __stdcall DestroyMenuBar(HWND hMenuBar) {
    auto it = g_menubars.find(hMenuBar);
    if (it != g_menubars.end()) {
        MenuBarState* menubar = it->second;
        
        // 清理子菜单窗口
        if (menubar->submenu) {
            delete menubar->submenu;
            menubar->submenu = nullptr;
        }
        
        delete menubar;
        g_menubars.erase(it);
        InvalidateRect(hMenuBar, NULL, FALSE);
    }
}

// 添加菜单栏项
__declspec(dllexport) int __stdcall MenuBarAddItem(
    HWND hMenuBar,
    const unsigned char* text_bytes,
    int text_len,
    int item_id
) {
    auto it = g_menubars.find(hMenuBar);
    if (it == g_menubars.end()) return -1;
    
    MenuBarState* state = it->second;
    
    MenuItem item;
    item.id = item_id;
    item.text = Utf8ToWide(text_bytes, text_len);
    item.enabled = true;
    item.checked = false;
    item.separator = false;
    
    state->items.push_back(item);
    InvalidateRect(hMenuBar, NULL, FALSE);
    
    return (int)state->items.size() - 1;
}

// 添加菜单栏子项
__declspec(dllexport) int __stdcall MenuBarAddSubItem(
    HWND hMenuBar,
    int parent_item_id,
    const unsigned char* text_bytes,
    int text_len,
    int item_id
) {
    WriteLog("MenuBarAddSubItem: hMenuBar=%p, parent_item_id=%d, item_id=%d", hMenuBar, parent_item_id, item_id);
    auto it = g_menubars.find(hMenuBar);
    if (it == g_menubars.end()) {
        WriteLog("MenuBarAddSubItem: menubar not found!");
        return -1;
    }
    
    MenuBarState* state = it->second;
    MenuItem* parent = FindMenuItem(state->items, parent_item_id);
    if (!parent) {
        WriteLog("MenuBarAddSubItem: parent item not found!");
        return -1;
    }
    
    MenuItem item;
    item.id = item_id;
    
    // 检查是否为分隔符（item_id == -1 且 text_bytes == 0 或 text_len == 0）
    if (item_id == -1 && (text_bytes == nullptr || text_len == 0)) {
        item.separator = true;
        item.text = L"";
        item.enabled = false;
        item.checked = false;
    } else {
        item.separator = false;
        item.text = Utf8ToWide(text_bytes, text_len);
        item.enabled = true;
        item.checked = false;
    }
    
    parent->sub_items.push_back(item);
    WriteLog("MenuBarAddSubItem: sub_item added, parent->sub_items.size()=%zu", parent->sub_items.size());
    InvalidateRect(hMenuBar, NULL, FALSE);
    
    return (int)parent->sub_items.size() - 1;
}

// 设置菜单栏位置和大小
__declspec(dllexport) void __stdcall SetMenuBarPlacement(
    HWND hMenuBar,
    int x, int y, int width, int height
) {
    // 菜单栏位置由窗口自动管理，此函数预留用于未来扩展
    InvalidateRect(hMenuBar, NULL, FALSE);
}

// 设置菜单栏回调
__declspec(dllexport) void __stdcall SetMenuBarCallback(
    HWND hMenuBar,
    MenuItemClickCallback callback
) {
    WriteLog("SetMenuBarCallback: hMenuBar=%p, callback=%p", hMenuBar, callback);
    auto it = g_menubars.find(hMenuBar);
    if (it != g_menubars.end()) {
        it->second->callback = callback;
        WriteLog("SetMenuBarCallback: callback set successfully");
    } else {
        WriteLog("SetMenuBarCallback: menubar not found!");
    }
}

// 更新菜单栏子项文本
__declspec(dllexport) BOOL __stdcall MenuBarUpdateSubItemText(
    HWND hMenuBar,
    int parent_item_id,
    int item_id,
    const unsigned char* text_bytes,
    int text_len
) {
    WriteLog("MenuBarUpdateSubItemText: hMenuBar=%p, parent_item_id=%d, item_id=%d, text_len=%d",
             hMenuBar, parent_item_id, item_id, text_len);

    auto it = g_menubars.find(hMenuBar);
    if (it == g_menubars.end()) {
        WriteLog("MenuBarUpdateSubItemText: menubar not found!");
        return FALSE;
    }

    MenuBarState* state = it->second;
    if (!state) {
        WriteLog("MenuBarUpdateSubItemText: state is NULL!");
        return FALSE;
    }

    // 查找父菜单项
    MenuItem* parent = FindMenuItem(state->items, parent_item_id);
    if (!parent) {
        WriteLog("MenuBarUpdateSubItemText: parent item not found!");
        return FALSE;
    }

    // 在父项的子项中查找目标 item_id
    for (auto& sub : parent->sub_items) {
        if (sub.id == item_id) {
            sub.text = Utf8ToWide(text_bytes, text_len);
            WriteLog("MenuBarUpdateSubItemText: text updated successfully");
            InvalidateRect(hMenuBar, NULL, FALSE);
            return TRUE;
        }
    }

    WriteLog("MenuBarUpdateSubItemText: sub item not found!");
    return FALSE;
}

// 创建弹出菜单
__declspec(dllexport) HWND __stdcall CreateEmojiPopupMenu(HWND hOwner) {
    if (!hOwner) return nullptr;
    
    PopupMenuState* state = new PopupMenuState();
    state->owner_hwnd = hOwner;
    state->visible = false;
    state->hover_color = 0;
    state->item_height = 32;
    state->callback = nullptr;
    
    // 生成唯一句柄（使用指针地址）
    HWND hPopupMenu = (HWND)state;
    g_popup_menus[hPopupMenu] = state;
    
    return hPopupMenu;
}

// 销毁弹出菜单
__declspec(dllexport) void __stdcall DestroyEmojiPopupMenu(HWND hPopupMenu) {
    auto it = g_popup_menus.find(hPopupMenu);
    if (it != g_popup_menus.end()) {
        delete it->second;
        g_popup_menus.erase(it);
    }
}

// 添加弹出菜单项
__declspec(dllexport) int __stdcall PopupMenuAddItem(
    HWND hPopupMenu,
    const unsigned char* text_bytes,
    int text_len,
    int item_id
) {
    // 写入日志文件
    FILE* log = nullptr;
    fopen_s(&log, "C:\\popup_menu_debug.txt", "a");
    if (log) {
        fprintf(log, "PopupMenuAddItem called: hPopupMenu=%p, text_len=%d, item_id=%d\n", 
            hPopupMenu, text_len, item_id);
        fclose(log);
    }
    
    auto it = g_popup_menus.find(hPopupMenu);
    if (it == g_popup_menus.end()) {
        FILE* log2 = nullptr;
        fopen_s(&log2, "C:\\popup_menu_debug.txt", "a");
        if (log2) {
            fprintf(log2, "ERROR: hPopupMenu not found in g_popup_menus\n");
            fclose(log2);
        }
        return -1;
    }
    
    PopupMenuState* state = it->second;
    
    MenuItem item;
    item.id = item_id;
    
    // 如果 item_id == -1 且 text_len == 0，则为分隔符
    if (item_id == -1 && text_len == 0) {
        item.separator = true;
        item.text = L"";
        
        FILE* log3 = nullptr;
        fopen_s(&log3, "C:\\popup_menu_debug.txt", "a");
        if (log3) {
            fprintf(log3, "Adding separator\n");
            fclose(log3);
        }
    } else {
        item.text = Utf8ToWide(text_bytes, text_len);
        item.separator = false;
        
        FILE* log4 = nullptr;
        fopen_s(&log4, "C:\\popup_menu_debug.txt", "a");
        if (log4) {
            fprintf(log4, "Adding item: id=%d, text_len=%d\n", item_id, text_len);
            fclose(log4);
        }
    }
    
    item.enabled = true;
    item.checked = false;
    
    state->items.push_back(item);
    
    FILE* log5 = nullptr;
    fopen_s(&log5, "C:\\popup_menu_debug.txt", "a");
    if (log5) {
        fprintf(log5, "Total items in menu: %zu\n\n", state->items.size());
        fclose(log5);
    }
    
    return (int)state->items.size() - 1;
}

// 添加弹出菜单子项
__declspec(dllexport) int __stdcall PopupMenuAddSubItem(
    HWND hPopupMenu,
    int parent_item_id,
    const unsigned char* text_bytes,
    int text_len,
    int item_id
) {
    auto it = g_popup_menus.find(hPopupMenu);
    if (it == g_popup_menus.end()) return -1;
    
    PopupMenuState* state = it->second;
    MenuItem* parent = FindMenuItem(state->items, parent_item_id);
    if (!parent) return -1;
    
    MenuItem item;
    item.id = item_id;
    item.text = Utf8ToWide(text_bytes, text_len);
    item.enabled = true;
    item.checked = false;
    item.separator = false;
    
    parent->sub_items.push_back(item);
    
    return (int)parent->sub_items.size() - 1;
}

// 绑定控件与弹出菜单
__declspec(dllexport) void __stdcall BindControlMenu(
    HWND hControl,
    HWND hPopupMenu
) {
    if (!hControl || !hPopupMenu) return;
    g_control_popup_menu_bindings[hControl] = hPopupMenu;
}

// 显示右键菜单
__declspec(dllexport) void __stdcall ShowContextMenu(
    HWND hPopupMenu,
    int x,
    int y
) {
    // 写入日志
    FILE* log = nullptr;
    fopen_s(&log, "C:\\popup_menu_debug.txt", "a");
    if (log) {
        fprintf(log, "\n=== ShowContextMenu called ===\n");
        fprintf(log, "hPopupMenu=%p, x=%d, y=%d\n", hPopupMenu, x, y);
        fclose(log);
    }
    
    auto it = g_popup_menus.find(hPopupMenu);
    if (it == g_popup_menus.end()) {
        FILE* log2 = nullptr;
        fopen_s(&log2, "C:\\popup_menu_debug.txt", "a");
        if (log2) {
            fprintf(log2, "ERROR: hPopupMenu not found in g_popup_menus\n");
            fclose(log2);
        }
        return;
    }
    
    PopupMenuState* state = it->second;
    
    FILE* log3 = nullptr;
    fopen_s(&log3, "C:\\popup_menu_debug.txt", "a");
    if (log3) {
        fprintf(log3, "state=%p, items.size()=%zu\n", state, state->items.size());
        fclose(log3);
    }
    
    // 注册弹出菜单窗口类
    static bool class_registered = false;
    if (!class_registered) {
        WNDCLASSEXW wc = {};
        wc.cbSize = sizeof(WNDCLASSEXW);
        wc.style = CS_DROPSHADOW;
        wc.lpfnWndProc = PopupMenuProc;
        wc.hInstance = GetModuleHandle(nullptr);
        wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
        wc.hbrBackground = nullptr;
        wc.lpszClassName = L"EmojiPopupMenuClass";
        RegisterClassExW(&wc);
        class_registered = true;
    }
    
    // 计算菜单尺寸
    int menu_width = 200;
    int menu_height = 0;
    int item_height = 32;
    int separator_height = 8;
    
    for (const auto& item : state->items) {
        if (item.separator) {
            menu_height += separator_height;
        } else {
            menu_height += item_height;
        }
    }
    
    // 如果x,y为-1,使用当前鼠标位置
    if (x == -1 || y == -1) {
        POINT pt;
        GetCursorPos(&pt);
        x = pt.x;
        y = pt.y;
    }
    
    // 创建弹出窗口
    HWND hwnd = CreateWindowExW(
        WS_EX_TOPMOST | WS_EX_TOOLWINDOW,
        L"EmojiPopupMenuClass",
        L"",
        WS_POPUP | WS_BORDER,
        x, y, menu_width, menu_height,
        state->owner_hwnd,
        nullptr,
        GetModuleHandle(nullptr),
        state  // ⚠️ 关键：通过 lpParam 传递 state 指针
    );
    
    FILE* log4 = nullptr;
    fopen_s(&log4, "C:\\popup_menu_debug.txt", "a");
    if (log4) {
        fprintf(log4, "CreateWindowExW returned: hwnd=%p\n", hwnd);
        fprintf(log4, "Window position: x=%d, y=%d, width=%d, height=%d\n", 
            x, y, menu_width, menu_height);
        fclose(log4);
    }
    
    if (hwnd) {
        state->hwnd = hwnd;
        state->width = menu_width;
        state->height = menu_height;
        
        // 将state与窗口关联
        g_popup_menus[hwnd] = state;
        
        FILE* log5 = nullptr;
        fopen_s(&log5, "C:\\popup_menu_debug.txt", "a");
        if (log5) {
            fprintf(log5, "Window created successfully, calling ShowWindow\n");
            fclose(log5);
        }
        
        ShowWindow(hwnd, SW_SHOW);
        SetFocus(hwnd);
        
        FILE* log6 = nullptr;
        fopen_s(&log6, "C:\\popup_menu_debug.txt", "a");
        if (log6) {
            fprintf(log6, "ShowWindow and SetFocus called\n");
            fclose(log6);
        }
    } else {
        FILE* log7 = nullptr;
        fopen_s(&log7, "C:\\popup_menu_debug.txt", "a");
        if (log7) {
            DWORD err = GetLastError();
            fprintf(log7, "ERROR: CreateWindowExW failed, GetLastError=%lu\n", err);
            fclose(log7);
        }
    }
}

// 设置弹出菜单回调
__declspec(dllexport) void __stdcall SetPopupMenuCallback(
    HWND hPopupMenu,
    MenuItemClickCallback callback
) {
    auto it = g_popup_menus.find(hPopupMenu);
    if (it != g_popup_menus.end()) {
        it->second->callback = callback;
    }
}

// ========== 按钮属性命令 ==========

// Wide String to UTF-8 (辅助函数)
static std::string WideToUtf8(const std::wstring& wide) {
    if (wide.empty()) return "";
    
    int utf8_len = WideCharToMultiByte(CP_UTF8, 0, wide.c_str(), (int)wide.length(), nullptr, 0, nullptr, nullptr);
    if (utf8_len <= 0) return "";
    
    std::string result(utf8_len, 0);
    WideCharToMultiByte(CP_UTF8, 0, wide.c_str(), (int)wide.length(), &result[0], utf8_len, nullptr, nullptr);
    return result;
}






