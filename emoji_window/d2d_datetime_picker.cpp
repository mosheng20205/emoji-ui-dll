// D2D 日期时间选择器：Element 风格弹出层，复用彩色 emoji 编辑框显示
#include "emoji_window.h"
#include <windowsx.h>
#include <algorithm>
#include <map>
#include <string>
#include <commctrl.h>

#pragma comment(lib, "comctl32.lib")

extern ID2D1Factory* g_d2d_factory;
extern IDWriteFactory* g_dwrite_factory;

HWND __stdcall CreateD2DColorEmojiEditBox(
    HWND hParent, int x, int y, int width, int height,
    const unsigned char* text_bytes, int text_len,
    UINT32 fg_color, UINT32 bg_color,
    const unsigned char* font_name_bytes, int font_name_len,
    int font_size, bool bold, bool italic, bool underline,
    int alignment, bool multiline, bool readonly, bool password,
    bool has_border, bool vertical_center);

void __stdcall SetD2DEditBoxText(
    HWND hEdit, const unsigned char* text_bytes, int text_len);

static std::wstring Utf8BytesToWide(const unsigned char* bytes, int len) {
    if (!bytes || len <= 0) return L"Microsoft YaHei UI";
    int n = MultiByteToWideChar(CP_UTF8, 0, (LPCCH)bytes, len, nullptr, 0);
    if (n <= 0) return L"Microsoft YaHei UI";
    std::wstring w(n, 0);
    MultiByteToWideChar(CP_UTF8, 0, (LPCCH)bytes, len, &w[0], n);
    return w;
}

namespace {

const int kButtonWidth = 30;
const int kPopupMinW = 280;
const int WM_DTP_CLOSE_POPUP = WM_USER + 350;
const UINT kEditSubclassId = 0x44545031;
const UINT kMsgEditOpen = WM_APP + 80;

static const wchar_t kHostClass[] = L"D2DDateTimePickerHostClass";
static const wchar_t kPopupClass[] = L"D2DDateTimePickerPopupClass";

struct DtpState {
    HWND hwnd = nullptr;
    HWND parent = nullptr;
    HWND edit_hwnd = nullptr;
    HWND popup_hwnd = nullptr;
    int x = 0, y = 0, width = 0, height = 0;
    int button_width = kButtonWidth;
    DateTimePickerPrecision precision = DTP_PRECISION_YMD;
    SYSTEMTIME st{};
    SYSTEMTIME temp{}; // 弹出层编辑中
    bool dropdown_visible = false;
    bool enabled = true;
    bool button_hovered = false;
    bool button_pressed = false;
    UINT32 fg_color = 0xFF606266;
    UINT32 bg_color = 0xFFFFFFFF;
    UINT32 border_color = 0xFFDCDFE6;
    FontStyle font{};
    ValueChangedCallback callback = nullptr;
    EventCallbacks events{};
    ID2D1HwndRenderTarget* render_target = nullptr;

    int view_y = 2026;
    int view_m = 1;
    int year_base = 2020;
    int hover_cell = -1;
    int hover_time = -1;
    bool hover_ok = false;
    bool hover_prev = false;
    bool hover_next = false;
};

std::map<HWND, DtpState*> g_dt_pick;

static std::string WideToUtf8Str(const std::wstring& w) {
    if (w.empty()) return "";
    int n = WideCharToMultiByte(CP_UTF8, 0, w.c_str(), -1, nullptr, 0, nullptr, nullptr);
    if (n <= 1) return "";
    std::string s(n - 1, 0);
    WideCharToMultiByte(CP_UTF8, 0, w.c_str(), -1, &s[0], n, nullptr, nullptr);
    return s;
}

static D2D1_COLOR_F ColorFromUInt32(UINT32 color) {
    float a = ((color >> 24) & 0xFF) / 255.0f;
    if (a == 0.0f) a = 1.0f;
    return D2D1::ColorF(
        ((color >> 16) & 0xFF) / 255.0f,
        ((color >> 8) & 0xFF) / 255.0f,
        (color & 0xFF) / 255.0f, a);
}

static int DaysInMonth(int y, int m) {
    static const int d[] = { 31,28,31,30,31,30,31,31,30,31,30,31 };
    if (m < 1 || m > 12) return 31;
    if (m == 2) {
        bool leap = (y % 4 == 0 && y % 100 != 0) || (y % 400 == 0);
        return leap ? 29 : 28;
    }
    return d[m - 1];
}

static void ClampDate(SYSTEMTIME& s) {
    if (s.wYear < 1601) s.wYear = 1601;
    if (s.wYear > 30827) s.wYear = 30827;
    if (s.wMonth < 1) s.wMonth = 1;
    if (s.wMonth > 12) s.wMonth = 12;
    int dim = DaysInMonth(s.wYear, s.wMonth);
    if (s.wDay < 1) s.wDay = 1;
    if (s.wDay > dim) s.wDay = dim;
    if (s.wHour > 23) s.wHour = 23;
    if (s.wMinute > 59) s.wMinute = 59;
    if (s.wSecond > 59) s.wSecond = 59;
    s.wMilliseconds = 0;
}

void NormalizeToPrecision(SYSTEMTIME& st, DateTimePickerPrecision p) {
    ClampDate(st);
    switch (p) {
    case DTP_PRECISION_YEAR:
        st.wMonth = 1; st.wDay = 1;
        st.wHour = st.wMinute = st.wSecond = 0; st.wMilliseconds = 0;
        break;
    case DTP_PRECISION_YMD:
        st.wHour = st.wMinute = st.wSecond = 0; st.wMilliseconds = 0;
        break;
    case DTP_PRECISION_YMDH:
        st.wMinute = st.wSecond = 0; st.wMilliseconds = 0;
        break;
    case DTP_PRECISION_YMDHM:
        st.wSecond = 0; st.wMilliseconds = 0;
        break;
    case DTP_PRECISION_YMDHMS:
        st.wMilliseconds = 0;
        break;
    default:
        break;
    }
}

void UpgradePrecisionFill(SYSTEMTIME& st, DateTimePickerPrecision oldp, DateTimePickerPrecision newp) {
    if ((int)newp <= (int)oldp) return;
    if (oldp < DTP_PRECISION_YMD && newp >= DTP_PRECISION_YMD) {
        st.wMonth = 1; st.wDay = 1;
    }
    if (oldp < DTP_PRECISION_YMDH && newp >= DTP_PRECISION_YMDH)
        st.wHour = 0;
    if (oldp < DTP_PRECISION_YMDHM && newp >= DTP_PRECISION_YMDHM)
        st.wMinute = 0;
    if (oldp < DTP_PRECISION_YMDHMS && newp >= DTP_PRECISION_YMDHMS)
        st.wSecond = 0;
    st.wMilliseconds = 0;
}

static std::wstring FormatSt(const SYSTEMTIME& st, DateTimePickerPrecision p) {
    wchar_t buf[96];
    switch (p) {
    case DTP_PRECISION_YEAR:
        swprintf_s(buf, L"%04d", (int)st.wYear);
        break;
    case DTP_PRECISION_YMD:
        swprintf_s(buf, L"%04d-%02d-%02d", (int)st.wYear, (int)st.wMonth, (int)st.wDay);
        break;
    case DTP_PRECISION_YMDH:
        swprintf_s(buf, L"%04d-%02d-%02d %02d", (int)st.wYear, (int)st.wMonth, (int)st.wDay, (int)st.wHour);
        break;
    case DTP_PRECISION_YMDHM:
        swprintf_s(buf, L"%04d-%02d-%02d %02d:%02d", (int)st.wYear, (int)st.wMonth, (int)st.wDay,
            (int)st.wHour, (int)st.wMinute);
        break;
    default:
        swprintf_s(buf, L"%04d-%02d-%02d %02d:%02d:%02d", (int)st.wYear, (int)st.wMonth, (int)st.wDay,
            (int)st.wHour, (int)st.wMinute, (int)st.wSecond);
        break;
    }
    return buf;
}

static void PushEditText(DtpState* s) {
    if (!s || !s->edit_hwnd) return;
    std::wstring w = FormatSt(s->st, s->precision);
    std::string u8 = WideToUtf8Str(w);
    SetD2DEditBoxText(s->edit_hwnd, (const unsigned char*)u8.c_str(), (int)u8.size());
}

static void FireChanged(DtpState* s) {
    if (!s) return;
    if (s->callback) s->callback(s->hwnd);
    if (s->events.on_value_changed) s->events.on_value_changed(s->hwnd);
}

static void ApplySetParts(DtpState* s, int y, int mo, int d, int h, int mi, int se) {
    SYSTEMTIME t = s->st;
    switch (s->precision) {
    case DTP_PRECISION_YEAR:
        t.wYear = (WORD)y;
        break;
    case DTP_PRECISION_YMD:
        t.wYear = (WORD)y; t.wMonth = (WORD)mo; t.wDay = (WORD)d;
        break;
    case DTP_PRECISION_YMDH:
        t.wYear = (WORD)y; t.wMonth = (WORD)mo; t.wDay = (WORD)d; t.wHour = (WORD)h;
        break;
    case DTP_PRECISION_YMDHM:
        t.wYear = (WORD)y; t.wMonth = (WORD)mo; t.wDay = (WORD)d;
        t.wHour = (WORD)h; t.wMinute = (WORD)mi;
        break;
    default:
        t.wYear = (WORD)y; t.wMonth = (WORD)mo; t.wDay = (WORD)d;
        t.wHour = (WORD)h; t.wMinute = (WORD)mi; t.wSecond = (WORD)se;
        break;
    }
    ClampDate(t);
    NormalizeToPrecision(t, s->precision);
    s->st = t;
}

static int PopupHeight(DtpState* s) {
    const int hdr = 36, row = 26, timeh = 34, okh = 30, pad = 8;
    if (s->precision == DTP_PRECISION_YEAR)
        return hdr + 3 * row + pad * 2;
    int grid = 7 * row + hdr;
    bool need_time = s->precision >= DTP_PRECISION_YMDH;
    int h = grid + pad + (need_time ? timeh + pad : 0) + (need_time ? okh + pad : 0);
    if (s->precision == DTP_PRECISION_YMD)
        h = grid + pad;
    return h;
}

static void HidePopup(DtpState* s);

static void ShowPopup(DtpState* s) {
    if (!s || s->dropdown_visible) return;
    s->temp = s->st;
    s->view_y = s->temp.wYear;
    s->view_m = s->temp.wMonth;
    if (s->view_m < 1) s->view_m = 1;
    s->year_base = (s->temp.wYear / 12) * 12;
    if (s->year_base < 1601) s->year_base = 1601;

    RECT r{};
    GetWindowRect(s->hwnd, &r);
    int pw = kPopupMinW;
    int ph = PopupHeight(s);

    RECT wa{};
    SystemParametersInfo(SPI_GETWORKAREA, 0, &wa, 0);
    int py = r.bottom;
    if (py + ph > wa.bottom)
        py = r.top - ph;

    if (!s->popup_hwnd) {
        s->popup_hwnd = CreateWindowExW(
            WS_EX_TOPMOST | WS_EX_TOOLWINDOW,
            kPopupClass, L"", WS_POPUP,
            r.left, py, pw, ph,
            s->hwnd, nullptr, GetModuleHandleW(nullptr), s);
        if (s->popup_hwnd)
            SetWindowLongPtrW(s->popup_hwnd, GWLP_USERDATA, (LONG_PTR)s);
    } else {
        SetWindowPos(s->popup_hwnd, HWND_TOPMOST, r.left, py, pw, ph, SWP_SHOWWINDOW);
    }
    s->dropdown_visible = true;
    ShowWindow(s->popup_hwnd, SW_SHOW);
    InvalidateRect(s->popup_hwnd, nullptr, FALSE);
}

static void HidePopup(DtpState* s) {
    if (!s || !s->dropdown_visible) return;
    if (s->popup_hwnd)
        ShowWindow(s->popup_hwnd, SW_HIDE);
    s->dropdown_visible = false;
    s->hover_cell = -1;
    s->hover_time = -1;
}

static LRESULT CALLBACK EditSubclassProc(HWND h, UINT m, WPARAM w, LPARAM l, UINT_PTR, DWORD_PTR dw) {
    DtpState* s = (DtpState*)dw;
    if (m == WM_LBUTTONDOWN || m == WM_LBUTTONDBLCLK) {
        if (s && s->enabled)
            SendMessageW(s->hwnd, kMsgEditOpen, 0, 0);
    }
    if (m == WM_NCDESTROY)
        RemoveWindowSubclass(h, EditSubclassProc, kEditSubclassId);
    return DefSubclassProc(h, m, w, l);
}

static void DrawHostChrome(DtpState* s) {
    if (!s || !s->hwnd) return;
    if (!g_d2d_factory) return;
    RECT rc{};
    GetClientRect(s->hwnd, &rc);
    int W = rc.right - rc.left, H = rc.bottom - rc.top;
    if (!s->render_target) {
        D2D1_RENDER_TARGET_PROPERTIES props = D2D1::RenderTargetProperties();
        D2D1_HWND_RENDER_TARGET_PROPERTIES hp = D2D1::HwndRenderTargetProperties(
            s->hwnd, D2D1::SizeU((UINT32)W, (UINT32)H));
        g_d2d_factory->CreateHwndRenderTarget(props, hp, &s->render_target);
    }
    if (!s->render_target) return;
    s->render_target->BeginDraw();
    s->render_target->Clear(D2D1::ColorF(0, 0, 0, 0));

    int bx = W - s->button_width;
    ID2D1SolidColorBrush* btn = nullptr;
    UINT32 bc = 0xFFF0F0F0;
    if (s->button_pressed) bc = 0xFFD0D0D0;
    else if (s->button_hovered) bc = 0xFFE0E0E0;
    s->render_target->CreateSolidColorBrush(ColorFromUInt32(bc), &btn);
    if (btn) {
        s->render_target->FillRectangle(D2D1::RectF((float)bx, 0, (float)W, (float)H), btn);
        btn->Release();
    }

    ID2D1SolidColorBrush* brd = nullptr;
    s->render_target->CreateSolidColorBrush(ColorFromUInt32(s->border_color), &brd);
    if (brd) {
        s->render_target->DrawRectangle(D2D1::RectF(0.5f, 0.5f, (float)W - 0.5f, (float)H - 0.5f), brd, 1.0f);
        s->render_target->DrawLine(D2D1::Point2F((float)bx, 0), D2D1::Point2F((float)bx, (float)H), brd, 1.0f);
        brd->Release();
    }

    if (g_dwrite_factory) {
        IDWriteTextFormat* tf = nullptr;
        g_dwrite_factory->CreateTextFormat(L"Segoe UI Symbol", nullptr, DWRITE_FONT_WEIGHT_NORMAL,
            DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 14.0f, L"", &tf);
        if (tf) {
            tf->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
            tf->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
            ID2D1SolidColorBrush* tb = nullptr;
            s->render_target->CreateSolidColorBrush(D2D1::ColorF(0.35f, 0.35f, 0.35f, 1.0f), &tb);
            if (tb) {
                s->render_target->DrawText(L"\u25BC", 1, tf,
                    D2D1::RectF((float)bx, 0, (float)W, (float)H), tb,
                    D2D1_DRAW_TEXT_OPTIONS_ENABLE_COLOR_FONT);
                tb->Release();
            }
            tf->Release();
        }
    }

    s->render_target->EndDraw();
}

static void CellDay(int vy, int vm, int cell, bool& in_month, int& display_day) {
    SYSTEMTIME fs{};
    fs.wYear = (WORD)vy; fs.wMonth = (WORD)vm; fs.wDay = 1;
    FILETIME ft{};
    SystemTimeToFileTime(&fs, &ft);
    FileTimeToSystemTime(&ft, &fs);
    int first = (fs.wDayOfWeek + 6) % 7;
    int dim = DaysInMonth(vy, vm);
    int prev_m = vm - 1, prev_y = vy;
    if (prev_m < 1) { prev_m = 12; prev_y--; }
    int prev_dim = DaysInMonth(prev_y, prev_m);
    int start = -first + 1;
    int d = start + cell;
    if (d >= 1 && d <= dim) {
        in_month = true;
        display_day = d;
    } else if (d < 1) {
        in_month = false;
        display_day = prev_dim + d;
    } else {
        in_month = false;
        display_day = d - dim;
    }
}

static void DrawPopup(DtpState* s) {
    if (!s || !s->popup_hwnd || !g_dwrite_factory) return;
    HWND ph = s->popup_hwnd;
    RECT rc{};
    GetClientRect(ph, &rc);
    int W = rc.right - rc.left, H = rc.bottom - rc.top;

    ID2D1HwndRenderTarget* rt = nullptr;
    if (g_d2d_factory) {
        D2D1_RENDER_TARGET_PROPERTIES props = D2D1::RenderTargetProperties();
        D2D1_HWND_RENDER_TARGET_PROPERTIES hp = D2D1::HwndRenderTargetProperties(
            ph, D2D1::SizeU((UINT32)W, (UINT32)H));
        g_d2d_factory->CreateHwndRenderTarget(props, hp, &rt);
    }
    if (!rt) return;

    rt->BeginDraw();
    rt->Clear(ColorFromUInt32(s->bg_color));

    ID2D1SolidColorBrush* border = nullptr;
    rt->CreateSolidColorBrush(ColorFromUInt32(s->border_color), &border);
    if (border) {
        rt->DrawRectangle(D2D1::RectF(0.5f, 0.5f, (float)W - 0.5f, (float)H - 0.5f), border, 1.0f);
    }

    IDWriteTextFormat* tf = nullptr;
    g_dwrite_factory->CreateTextFormat(
        s->font.font_name.empty() ? L"Microsoft YaHei UI" : s->font.font_name.c_str(),
        nullptr,
        DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL,
        13.0f, L"zh-cn", &tf);

    ID2D1SolidColorBrush* fg = nullptr;
    rt->CreateSolidColorBrush(ColorFromUInt32(s->fg_color), &fg);

    const int hdr = 36, row = 26, pad = 8;

    if (s->precision == DTP_PRECISION_YEAR) {
        if (tf && fg) {
            rt->DrawTextW(L"<", 1, tf, D2D1::RectF(4, 4, 28, (float)hdr), fg, D2D1_DRAW_TEXT_OPTIONS_ENABLE_COLOR_FONT);
            rt->DrawTextW(L">", 1, tf, D2D1::RectF((float)W - 28, 4, (float)W - 4, (float)hdr), fg,
                D2D1_DRAW_TEXT_OPTIONS_ENABLE_COLOR_FONT);
        }
        wchar_t nav[64];
        swprintf_s(nav, L"%d - %d", s->year_base, s->year_base + 11);
        if (tf && fg)
            rt->DrawTextW(nav, (UINT32)wcslen(nav), tf, D2D1::RectF(40, 4, (float)W - 40, (float)hdr), fg,
                D2D1_DRAW_TEXT_OPTIONS_ENABLE_COLOR_FONT);
        for (int r = 0; r < 3; r++) {
            for (int c = 0; c < 4; c++) {
                int idx = r * 4 + c;
                int y = s->year_base + idx;
                float x0 = (float)(pad + c * ((W - pad * 2) / 4));
                float y0 = (float)(hdr + pad + r * row);
                D2D1_RECT_F cell = D2D1::RectF(x0, y0, x0 + (W - pad * 2) / 4 - 4, y0 + row - 2);
                bool hv = (s->hover_cell == idx);
                if (hv) {
                    ID2D1SolidColorBrush* hb = nullptr;
                    rt->CreateSolidColorBrush(D2D1::ColorF(0.86f, 0.93f, 1.0f, 1.0f), &hb);
                    if (hb) { rt->FillRectangle(cell, hb); hb->Release(); }
                }
                wchar_t ys[8];
                swprintf_s(ys, L"%d", y);
                if (tf && fg)
                    rt->DrawTextW(ys, (UINT32)wcslen(ys), tf, cell, fg, D2D1_DRAW_TEXT_OPTIONS_ENABLE_COLOR_FONT);
            }
        }
    } else {
        if (tf && fg) {
            rt->DrawTextW(L"<", 1, tf, D2D1::RectF(4, 4, 28, (float)hdr), fg, D2D1_DRAW_TEXT_OPTIONS_ENABLE_COLOR_FONT);
            rt->DrawTextW(L">", 1, tf, D2D1::RectF((float)W - 28, 4, (float)W - 4, (float)hdr), fg,
                D2D1_DRAW_TEXT_OPTIONS_ENABLE_COLOR_FONT);
        }
        wchar_t title[32];
        swprintf_s(title, L"%04d-%02d", s->view_y, s->view_m);
        if (tf && fg)
            rt->DrawTextW(title, (UINT32)wcslen(title), tf, D2D1::RectF(36, 4, (float)W - 36, (float)hdr), fg,
                D2D1_DRAW_TEXT_OPTIONS_ENABLE_COLOR_FONT);

        static const wchar_t* labs[] = { L"Mo", L"Tu", L"We", L"Th", L"Fr", L"Sa", L"Su" };
        for (int i = 0; i < 7; i++) {
            if (tf && fg)
                rt->DrawTextW(labs[i], (UINT32)wcslen(labs[i]), tf,
                    D2D1::RectF((float)(pad + i * row), (float)hdr - 2, (float)(pad + (i + 1) * row), (float)hdr + 14),
                    fg, D2D1_DRAW_TEXT_OPTIONS_ENABLE_COLOR_FONT);
        }

        for (int i = 0; i < 42; i++) {
            bool in = false;
            int disp = 0;
            CellDay(s->view_y, s->view_m, i, in, disp);
            int r = i / 7, c = i % 7;
            float x0 = (float)(pad + c * row);
            float y0 = (float)(hdr + 4 + r * row);
            D2D1_RECT_F cell = D2D1::RectF(x0, y0, x0 + row - 2, y0 + row - 4);
            wchar_t ds[8];
            swprintf_s(ds, L"%d", disp);
            UINT32 col = s->fg_color;
            if (!in)
                col = (s->border_color & 0x00FFFFFF) | 0x88000000;
            if (s->hover_cell == i) {
                ID2D1SolidColorBrush* hb = nullptr;
                rt->CreateSolidColorBrush(D2D1::ColorF(0.86f, 0.93f, 1.0f, 1.0f), &hb);
                if (hb) { rt->FillRectangle(cell, hb); hb->Release(); }
            }
            ID2D1SolidColorBrush* tc = nullptr;
            rt->CreateSolidColorBrush(ColorFromUInt32(col), &tc);
            if (tf && tc)
                rt->DrawTextW(ds, (UINT32)wcslen(ds), tf, cell, tc, D2D1_DRAW_TEXT_OPTIONS_ENABLE_COLOR_FONT);
            if (tc) tc->Release();
        }

        if (s->precision >= DTP_PRECISION_YMDH) {
            int ty = hdr + 7 * row + pad;
            wchar_t tm[64];
            if (s->precision == DTP_PRECISION_YMDH)
                swprintf_s(tm, L"%02d", (int)s->temp.wHour);
            else if (s->precision == DTP_PRECISION_YMDHM)
                swprintf_s(tm, L"%02d : %02d", (int)s->temp.wHour, (int)s->temp.wMinute);
            else
                swprintf_s(tm, L"%02d : %02d : %02d", (int)s->temp.wHour, (int)s->temp.wMinute, (int)s->temp.wSecond);
            if (tf && fg)
                rt->DrawTextW(tm, (UINT32)wcslen(tm), tf, D2D1::RectF((float)pad, (float)ty, (float)W - pad, (float)ty + 28),
                    fg, D2D1_DRAW_TEXT_OPTIONS_ENABLE_COLOR_FONT);
            int oky = ty + 30 + pad;
            D2D1_RECT_F okr = D2D1::RectF((float)(W - 80), (float)oky, (float)(W - 8), (float)(oky + 26));
            if (s->hover_ok) {
                ID2D1SolidColorBrush* hb = nullptr;
                rt->CreateSolidColorBrush(D2D1::ColorF(0.86f, 0.93f, 1.0f, 1.0f), &hb);
                if (hb) { rt->FillRectangle(okr, hb); hb->Release(); }
            }
            if (tf && fg)
                rt->DrawTextW(L"\u786e\u5b9a", 2, tf, okr, fg, D2D1_DRAW_TEXT_OPTIONS_ENABLE_COLOR_FONT);
        }
    }

    if (fg) fg->Release();
    if (border) border->Release();
    if (tf) tf->Release();
    rt->EndDraw();
    rt->Release();
}

static LRESULT CALLBACK PopupProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    DtpState* s = (DtpState*)GetWindowLongPtrW(hwnd, GWLP_USERDATA);
    switch (msg) {
    case WM_PAINT: {
        PAINTSTRUCT ps;
        BeginPaint(hwnd, &ps);
        DrawPopup(s);
        EndPaint(hwnd, &ps);
        return 0;
    }
    case WM_MOUSEMOVE: {
        if (!s) break;
        int x = GET_X_LPARAM(lParam), y = GET_Y_LPARAM(lParam);
        s->hover_ok = false;
        s->hover_cell = -1;
        s->hover_prev = (x >= 4 && x <= 28 && y >= 4 && y <= 32);
        s->hover_next = false;
        const int hdr = 36, row = 26, pad = 8;
        RECT rc{};
        GetClientRect(hwnd, &rc);
        int W = rc.right - rc.left;
        s->hover_next = (x >= W - 28 && x <= W - 4 && y >= 4 && y <= 32);
        s->hover_time = -1;
        if (s->precision == DTP_PRECISION_YEAR) {
            for (int r = 0; r < 3; r++) {
                for (int c = 0; c < 4; c++) {
                    int idx = r * 4 + c;
                    float x0 = (float)(pad + c * ((W - pad * 2) / 4));
                    float y0 = (float)(hdr + pad + r * row);
                    D2D1_RECT_F cell = D2D1::RectF(x0, y0, x0 + (W - pad * 2) / 4 - 4, y0 + row - 2);
                    if ((float)x >= cell.left && (float)x < cell.right && (float)y >= cell.top && (float)y < cell.bottom)
                        s->hover_cell = idx;
                }
            }
        } else {
            int grid = hdr + 7 * row;
            if (y > hdr + 4 && y < grid) {
                int col = (x - pad) / row;
                int rowidx = (y - hdr - 4) / row;
                if (col >= 0 && col < 7 && rowidx >= 0 && rowidx < 6)
                    s->hover_cell = rowidx * 7 + col;
            }
            if (s->precision >= DTP_PRECISION_YMDH) {
                int ty = hdr + 7 * row + pad;
                int oky = ty + 30 + pad;
                s->hover_ok = (x > W - 80 && x < W - 8 && y > oky && y < oky + 26);
                if (y >= ty && y < ty + 28 && x >= pad && x < W - pad) {
                    int rel = x - pad;
                    int tw = W - pad * 2;
                    if (s->precision == DTP_PRECISION_YMDH)
                        s->hover_time = 0;
                    else if (s->precision == DTP_PRECISION_YMDHM) {
                        if (rel < tw / 2) s->hover_time = 0;
                        else s->hover_time = 1;
                    } else {
                        if (rel < tw / 3) s->hover_time = 0;
                        else if (rel < tw * 2 / 3) s->hover_time = 1;
                        else s->hover_time = 2;
                    }
                }
            }
        }
        InvalidateRect(hwnd, nullptr, FALSE);
        break;
    }
    case WM_LBUTTONDOWN: {
        if (!s) return 0;
        int x = GET_X_LPARAM(lParam), y = GET_Y_LPARAM(lParam);
        RECT rc{};
        GetClientRect(hwnd, &rc);
        int W = rc.right - rc.left;
        const int hdr = 36, row = 26, pad = 8;

        if (x >= 4 && x <= 28 && y >= 4 && y <= 32) {
            if (s->precision == DTP_PRECISION_YEAR) {
                s->year_base -= 12;
                if (s->year_base < 1601) s->year_base = 1601;
            } else {
                s->view_m--;
                if (s->view_m < 1) { s->view_m = 12; s->view_y--; }
            }
            InvalidateRect(hwnd, nullptr, FALSE);
            return 0;
        }
        if (x >= W - 28 && x <= W - 4 && y >= 4 && y <= 32) {
            if (s->precision == DTP_PRECISION_YEAR) {
                s->year_base += 12;
                if (s->year_base > 3070) s->year_base = 3070;
            } else {
                s->view_m++;
                if (s->view_m > 12) { s->view_m = 1; s->view_y++; }
            }
            InvalidateRect(hwnd, nullptr, FALSE);
            return 0;
        }

        if (s->precision == DTP_PRECISION_YEAR) {
            for (int r = 0; r < 3; r++) {
                for (int c = 0; c < 4; c++) {
                    int idx = r * 4 + c;
                    float x0 = (float)(pad + c * ((W - pad * 2) / 4));
                    float y0 = (float)(hdr + pad + r * row);
                    D2D1_RECT_F cell = D2D1::RectF(x0, y0, x0 + (W - pad * 2) / 4 - 4, y0 + row - 2);
                    if ((float)x >= cell.left && (float)x < cell.right && (float)y >= cell.top && (float)y < cell.bottom) {
                        int year = s->year_base + idx;
                        s->temp.wYear = (WORD)year;
                        NormalizeToPrecision(s->temp, s->precision);
                        s->st = s->temp;
                        PushEditText(s);
                        FireChanged(s);
                        HidePopup(s);
                        InvalidateRect(s->hwnd, nullptr, FALSE);
                        return 0;
                    }
                }
            }
        } else {
            int col = (x - pad) / row;
            int rowidx = (y - hdr - 4) / row;
            if (col >= 0 && col < 7 && rowidx >= 0 && rowidx < 6 && y > hdr + 4 && y < hdr + 4 + 6 * row) {
                int cell = rowidx * 7 + col;
                bool in_m = false;
                int disp = 0;
                CellDay(s->view_y, s->view_m, cell, in_m, disp);
                if (in_m) {
                    s->temp.wYear = (WORD)s->view_y;
                    s->temp.wMonth = (WORD)s->view_m;
                    s->temp.wDay = (WORD)disp;
                    ClampDate(s->temp);
                    if (s->precision == DTP_PRECISION_YMD) {
                        NormalizeToPrecision(s->temp, s->precision);
                        s->st = s->temp;
                        PushEditText(s);
                        FireChanged(s);
                        HidePopup(s);
                        InvalidateRect(s->hwnd, nullptr, FALSE);
                        return 0;
                    }
                    InvalidateRect(hwnd, nullptr, FALSE);
                }
            }
            if (s->precision >= DTP_PRECISION_YMDH) {
                int ty = hdr + 7 * row + pad;
                int oky = ty + 30 + pad;
                if (x > W - 80 && x < W - 8 && y > oky && y < oky + 26) {
                    s->st = s->temp;
                    NormalizeToPrecision(s->st, s->precision);
                    PushEditText(s);
                    FireChanged(s);
                    HidePopup(s);
                    InvalidateRect(s->hwnd, nullptr, FALSE);
                    return 0;
                }
            }
        }
        return 0;
    }
    case WM_MOUSEWHEEL: {
        if (!s || s->precision < DTP_PRECISION_YMDH) break;
        short d = GET_WHEEL_DELTA_WPARAM(wParam);
        int inc = (d > 0) ? 1 : -1;
        int ht = s->hover_time;
        if (s->precision == DTP_PRECISION_YMDH) ht = 0;
        if (s->precision == DTP_PRECISION_YMDHM && ht > 1) ht = 1;
        if (ht == 0) {
            int h = s->temp.wHour + inc;
            if (h < 0) h = 23;
            if (h > 23) h = 0;
            s->temp.wHour = (WORD)h;
        } else if (ht == 1 && s->precision >= DTP_PRECISION_YMDHM) {
            int m = (int)s->temp.wMinute + inc;
            if (m < 0) m = 59;
            if (m > 59) m = 0;
            s->temp.wMinute = (WORD)m;
        } else if (ht == 2 && s->precision >= DTP_PRECISION_YMDHMS) {
            int sec = (int)s->temp.wSecond + inc;
            if (sec < 0) sec = 59;
            if (sec > 59) sec = 0;
            s->temp.wSecond = (WORD)sec;
        }
        InvalidateRect(hwnd, nullptr, FALSE);
        return 0;
    }
    case WM_ACTIVATE:
        if (LOWORD(wParam) == WA_INACTIVE && s)
            PostMessageW(s->hwnd, WM_DTP_CLOSE_POPUP, 0, 0);
        break;
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

static LRESULT CALLBACK HostProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    DtpState* s = nullptr;
    if (msg == WM_NCCREATE) {
        CREATESTRUCTW* cs = (CREATESTRUCTW*)lParam;
        s = (DtpState*)cs->lpCreateParams;
        SetWindowLongPtrW(hwnd, GWLP_USERDATA, (LONG_PTR)s);
        return TRUE;
    }
    s = (DtpState*)GetWindowLongPtrW(hwnd, GWLP_USERDATA);

    switch (msg) {
    case WM_PAINT: {
        PAINTSTRUCT ps;
        BeginPaint(hwnd, &ps);
        DrawHostChrome(s);
        EndPaint(hwnd, &ps);
        return 0;
    }
    case kMsgEditOpen:
        if (s && s->enabled) {
            if (s->dropdown_visible) HidePopup(s);
            else ShowPopup(s);
        }
        return 0;
    case WM_DTP_CLOSE_POPUP:
        HidePopup(s);
        return 0;
    case WM_LBUTTONDOWN: {
        if (!s || !s->enabled) break;
        int x = LOWORD(lParam), y = HIWORD(lParam);
        if (x >= s->width - s->button_width) {
            s->button_pressed = true;
            SetCapture(hwnd);
            InvalidateRect(hwnd, nullptr, FALSE);
        }
        break;
    }
    case WM_LBUTTONUP: {
        if (!s || !s->enabled) break;
        if (s->button_pressed) {
            s->button_pressed = false;
            ReleaseCapture();
            int x = LOWORD(lParam);
            if (x >= s->width - s->button_width) {
                if (s->dropdown_visible) HidePopup(s);
                else ShowPopup(s);
            }
            InvalidateRect(hwnd, nullptr, FALSE);
        }
        break;
    }
    case WM_MOUSEMOVE: {
        if (!s || !s->enabled) break;
        int x = LOWORD(lParam);
        bool h = (x >= s->width - s->button_width);
        if (h != s->button_hovered) {
            s->button_hovered = h;
            InvalidateRect(hwnd, nullptr, FALSE);
        }
        break;
    }
    case WM_SIZE:
        if (s) {
            s->width = LOWORD(lParam);
            s->height = HIWORD(lParam);
            if (s->edit_hwnd)
                SetWindowPos(s->edit_hwnd, nullptr, 0, 0, s->width - s->button_width, s->height,
                    SWP_NOZORDER | SWP_NOMOVE);
            if (s->render_target) {
                s->render_target->Release();
                s->render_target = nullptr;
            }
        }
        break;
    case WM_DESTROY:
        HidePopup(s);
        if (s && s->popup_hwnd) {
            DestroyWindow(s->popup_hwnd);
            s->popup_hwnd = nullptr;
        }
        if (s && s->render_target) {
            s->render_target->Release();
            s->render_target = nullptr;
        }
        if (s) {
            g_dt_pick.erase(hwnd);
            delete s;
        }
        break;
    default:
        break;
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

static bool RegisterClasses() {
    static bool done = false;
    if (done) return true;
    WNDCLASSEXW wh = { sizeof(wh) };
    wh.style = CS_HREDRAW | CS_VREDRAW;
    wh.lpfnWndProc = HostProc;
    wh.hInstance = GetModuleHandleW(nullptr);
    wh.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    wh.hbrBackground = nullptr;
    wh.lpszClassName = kHostClass;
    if (!RegisterClassExW(&wh) && GetLastError() != ERROR_CLASS_ALREADY_EXISTS)
        return false;
    WNDCLASSEXW wp = { sizeof(wp) };
    wp.style = CS_HREDRAW | CS_VREDRAW;
    wp.lpfnWndProc = PopupProc;
    wp.hInstance = GetModuleHandleW(nullptr);
    wp.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    wp.hbrBackground = nullptr;
    wp.lpszClassName = kPopupClass;
    if (!RegisterClassExW(&wp) && GetLastError() != ERROR_CLASS_ALREADY_EXISTS)
        return false;
    done = true;
    return true;
}

} // namespace

extern "C" {

__declspec(dllexport) HWND __stdcall CreateD2DDateTimePicker(
    HWND hParent,
    int x, int y, int width, int height,
    int initial_precision,
    UINT32 fg_color,
    UINT32 bg_color,
    UINT32 border_color,
    const unsigned char* font_name_bytes,
    int font_name_len,
    int font_size,
    BOOL bold,
    BOOL italic,
    BOOL underline)
{
    if (!RegisterClasses()) return nullptr;
    DtpState* st = new DtpState();
    GetLocalTime(&st->st);
    st->precision = (DateTimePickerPrecision)initial_precision;
    if (st->precision < DTP_PRECISION_YEAR || st->precision > DTP_PRECISION_YMDHMS) st->precision = DTP_PRECISION_YMD;
    NormalizeToPrecision(st->st, st->precision);
    st->fg_color = fg_color;
    st->bg_color = bg_color;
    st->border_color = border_color;
    st->font.font_name = Utf8BytesToWide(font_name_bytes, font_name_len);
    st->font.font_size = font_size;
    st->font.bold = bold != 0;
    st->font.italic = italic != 0;
    st->font.underline = underline != 0;
    st->parent = hParent;
    st->width = width;
    st->height = height;
    st->x = x;
    st->y = y;

    HWND hwnd = CreateWindowExW(0, kHostClass, L"", WS_CHILD | WS_VISIBLE | WS_TABSTOP,
        x, y, width, height, hParent, nullptr, GetModuleHandleW(nullptr), st);
    if (!hwnd) {
        delete st;
        return nullptr;
    }
    st->hwnd = hwnd;
    g_dt_pick[hwnd] = st;

    int ew = width - kButtonWidth;
    st->edit_hwnd = CreateD2DColorEmojiEditBox(
        hwnd, 0, 0, ew, height,
        (const unsigned char*)"", 0,
        fg_color, bg_color,
        font_name_bytes, font_name_len,
        font_size, bold != 0, italic != 0, underline != 0,
        0, false, true, false, true, true);
    if (st->edit_hwnd) {
        SetWindowSubclass(st->edit_hwnd, EditSubclassProc, kEditSubclassId, (DWORD_PTR)st);
    }
    PushEditText(st);
    return hwnd;
}

__declspec(dllexport) int __stdcall GetD2DDateTimePickerPrecision(HWND hPicker) {
    auto it = g_dt_pick.find(hPicker);
    if (it == g_dt_pick.end()) return DTP_PRECISION_YMD;
    return (int)it->second->precision;
}

__declspec(dllexport) void __stdcall SetD2DDateTimePickerPrecision(HWND hPicker, int precision) {
    auto it = g_dt_pick.find(hPicker);
    if (it == g_dt_pick.end()) return;
    DtpState* s = it->second;
    DateTimePickerPrecision np = (DateTimePickerPrecision)precision;
    if (np < DTP_PRECISION_YEAR || np > DTP_PRECISION_YMDHMS) return;
    DateTimePickerPrecision op = s->precision;
    UpgradePrecisionFill(s->st, op, np);
    NormalizeToPrecision(s->st, np);
    s->precision = np;
    PushEditText(s);
    FireChanged(s);
    if (s->dropdown_visible) InvalidateRect(s->popup_hwnd, nullptr, FALSE);
    InvalidateRect(hPicker, nullptr, FALSE);
}

__declspec(dllexport) void __stdcall GetD2DDateTimePickerDateTime(
    HWND hPicker, int* year, int* month, int* day, int* hour, int* minute, int* second)
{
    auto it = g_dt_pick.find(hPicker);
    if (it == g_dt_pick.end()) return;
    SYSTEMTIME t = it->second->st;
    NormalizeToPrecision(t, it->second->precision);
    if (year) *year = t.wYear;
    if (month) *month = t.wMonth;
    if (day) *day = t.wDay;
    if (hour) *hour = t.wHour;
    if (minute) *minute = t.wMinute;
    if (second) *second = t.wSecond;
}

__declspec(dllexport) void __stdcall SetD2DDateTimePickerDateTime(
    HWND hPicker, int year, int month, int day, int hour, int minute, int second)
{
    auto it = g_dt_pick.find(hPicker);
    if (it == g_dt_pick.end()) return;
    ApplySetParts(it->second, year, month, day, hour, minute, second);
    PushEditText(it->second);
    FireChanged(it->second);
    InvalidateRect(hPicker, nullptr, FALSE);
    if (it->second->dropdown_visible) InvalidateRect(it->second->popup_hwnd, nullptr, FALSE);
}

__declspec(dllexport) void __stdcall SetD2DDateTimePickerCallback(HWND hPicker, ValueChangedCallback cb) {
    auto it = g_dt_pick.find(hPicker);
    if (it != g_dt_pick.end()) it->second->callback = cb;
}

__declspec(dllexport) void __stdcall EnableD2DDateTimePicker(HWND hPicker, BOOL enable) {
    auto it = g_dt_pick.find(hPicker);
    if (it == g_dt_pick.end()) return;
    it->second->enabled = enable != 0;
    EnableWindow(hPicker, enable);
    if (it->second->edit_hwnd) EnableWindow(it->second->edit_hwnd, enable);
}

__declspec(dllexport) void __stdcall ShowD2DDateTimePicker(HWND hPicker, BOOL show) {
    auto it = g_dt_pick.find(hPicker);
    if (it == g_dt_pick.end()) return;
    ShowWindow(hPicker, show ? SW_SHOW : SW_HIDE);
    if (it->second->edit_hwnd) ShowWindow(it->second->edit_hwnd, show ? SW_SHOW : SW_HIDE);
}

__declspec(dllexport) void __stdcall SetD2DDateTimePickerBounds(HWND hPicker, int x, int y, int width, int height) {
    auto it = g_dt_pick.find(hPicker);
    if (it == g_dt_pick.end()) return;
    DtpState* s = it->second;
    s->x = x; s->y = y; s->width = width; s->height = height;
    SetWindowPos(hPicker, nullptr, x, y, width, height, SWP_NOZORDER);
}

} // extern C
