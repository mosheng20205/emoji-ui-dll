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

extern "C" HWND __stdcall CreateD2DColorEmojiEditBox(
    HWND hParent, int x, int y, int width, int height,
    const unsigned char* text_bytes, int text_len,
    UINT32 fg_color, UINT32 bg_color,
    const unsigned char* font_name_bytes, int font_name_len,
    int font_size, bool bold, bool italic, bool underline,
    int alignment, bool multiline, bool readonly, bool password,
    bool has_border, bool vertical_center);

extern "C" void __stdcall SetD2DEditBoxText(
    HWND hEdit, const unsigned char* text_bytes, int text_len);

extern "C" UINT32 __stdcall EW_GetThemeColorByIndex(int color_index);

static std::wstring Utf8BytesToWide(const unsigned char* bytes, int len) {
    if (!bytes || len <= 0) return L"Microsoft YaHei UI";
    int n = MultiByteToWideChar(CP_UTF8, 0, (LPCCH)bytes, len, nullptr, 0);
    if (n <= 0) return L"Microsoft YaHei UI";
    std::wstring w(n, 0);
    MultiByteToWideChar(CP_UTF8, 0, (LPCCH)bytes, len, &w[0], n);
    return w;
}

namespace {

const int kButtonWidth = 34;
const int kPopupMinW = 280;
const int WM_DTP_CLOSE_POPUP = WM_USER + 350;
const UINT kEditSubclassId = 0x44545031;
const UINT kMsgEditOpen = WM_APP + 80;
const float kHostCornerRadius = 6.0f;
const int kHostPaddingX = 12;
const int kHostPaddingY = 2;

static const wchar_t kHostClass[] = L"D2DDateTimePickerHostClass";
static const wchar_t kPopupClass[] = L"D2DDateTimePickerPopupClass";

static UINT32 ResolveDtpThemeColor(UINT32 argb) {
    if (argb <= 19) {
        return EW_GetThemeColorByIndex((int)argb);
    }
    return argb;
}

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
    bool host_hovered = false;
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
    int time_typing = 0;       // 0=无 1=已输入第一位数字，待第二位
    int time_typing_digit = 0;
    bool time_dropdown_visible = false;
    int time_dropdown_segment = -1;
    int time_dropdown_hover = -1;
    int time_dropdown_scroll = 0;
    bool hover_ok = false;
    bool hover_cancel = false;
    bool hover_prev = false;
    bool hover_next = false;
};

std::map<HWND, DtpState*> g_dt_pick;

static int GetTimeDropdownPanelHeight(const DtpState* s);
static void HideTimeDropdown(DtpState* s);

static void LayoutHostEdit(DtpState* s) {
    if (!s || !s->edit_hwnd) return;
    int edit_x = kHostPaddingX;
    int edit_y = kHostPaddingY;
    int edit_w = s->width - s->button_width - kHostPaddingX - 4;
    int edit_h = s->height - kHostPaddingY * 2;
    if (edit_w < 1) edit_w = 1;
    if (edit_h < 1) edit_h = 1;
    SetWindowPos(
        s->edit_hwnd,
        nullptr,
        edit_x,
        edit_y,
        edit_w,
        edit_h,
        SWP_NOZORDER | SWP_NOACTIVATE
    );
}

static void ApplyHostRegion(DtpState* s) {
    if (!s || !s->hwnd || s->width <= 0 || s->height <= 0) return;
    int diameter = (int)(kHostCornerRadius * 2.0f) + 2;
    HRGN region = CreateRoundRectRgn(0, 0, s->width + 1, s->height + 1, diameter, diameter);
    if (region) {
        SetWindowRgn(s->hwnd, region, TRUE);
    }
}

static void DrawChevron(ID2D1HwndRenderTarget* rt, ID2D1SolidColorBrush* brush, float cx, float cy, bool expanded) {
    if (!rt || !brush) return;
    const float half_w = 4.5f;
    const float half_h = 2.5f;
    if (expanded) {
        rt->DrawLine(D2D1::Point2F(cx - half_w, cy + half_h), D2D1::Point2F(cx, cy - half_h), brush, 1.6f);
        rt->DrawLine(D2D1::Point2F(cx, cy - half_h), D2D1::Point2F(cx + half_w, cy + half_h), brush, 1.6f);
    } else {
        rt->DrawLine(D2D1::Point2F(cx - half_w, cy - half_h), D2D1::Point2F(cx, cy + half_h), brush, 1.6f);
        rt->DrawLine(D2D1::Point2F(cx, cy + half_h), D2D1::Point2F(cx + half_w, cy - half_h), brush, 1.6f);
    }
}

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

static float DtpColorLuma(UINT32 color) {
    float r = ((color >> 16) & 0xFF) / 255.0f;
    float g = ((color >> 8) & 0xFF) / 255.0f;
    float b = (color & 0xFF) / 255.0f;
    return 0.299f * r + 0.587f * g + 0.114f * b;
}

static UINT32 DtpAutoContrastText(UINT32 bg_color) {
    return DtpColorLuma(bg_color) >= 0.62f ? 0xFF303133 : 0xFFFFFFFF;
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
    // 与 DrawPopup 一致：星期行 Mo..Su 占用约 [hdr-2, hdr+14]，日期网格从 cal_top 开始
    const int cal_top = hdr + 20;
    const int date_grid_bottom = cal_top + 6 * row;
    bool need_time = s->precision >= DTP_PRECISION_YMDH;
    int dropdown_h = (need_time && s && s->time_dropdown_visible) ? (GetTimeDropdownPanelHeight(s) + pad) : 0;
    int h = date_grid_bottom + pad + (need_time ? timeh + pad : 0) + dropdown_h + (need_time ? okh + pad : 0);
    if (s->precision == DTP_PRECISION_YMD)
        h = date_grid_bottom + pad;
    return h;
}

static void HidePopup(DtpState* s);
static void UpdatePopupPlacement(DtpState* s);

static void ShowPopup(DtpState* s) {
    if (!s || s->dropdown_visible) return;
    s->temp = s->st;
    s->view_y = s->temp.wYear;
    s->view_m = s->temp.wMonth;
    if (s->view_m < 1) s->view_m = 1;
    s->year_base = (s->temp.wYear / 12) * 12;
    if (s->year_base < 1601) s->year_base = 1601;

    if (!s->popup_hwnd) {
        RECT r{};
        GetWindowRect(s->hwnd, &r);
        s->popup_hwnd = CreateWindowExW(
            WS_EX_TOPMOST | WS_EX_TOOLWINDOW,
            kPopupClass, L"", WS_POPUP,
            r.left, r.bottom, kPopupMinW, PopupHeight(s),
            s->hwnd, nullptr, GetModuleHandleW(nullptr), s);
        if (s->popup_hwnd)
            SetWindowLongPtrW(s->popup_hwnd, GWLP_USERDATA, (LONG_PTR)s);
    }
    s->dropdown_visible = true;
    s->time_typing = 0;
    HideTimeDropdown(s);
    if (s->precision >= DTP_PRECISION_YMDH)
        s->hover_time = 0;
    UpdatePopupPlacement(s);
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
    s->time_typing = 0;
    HideTimeDropdown(s);
}

static void UpdatePopupPlacement(DtpState* s) {
    if (!s || !s->popup_hwnd) return;
    RECT r{};
    GetWindowRect(s->hwnd, &r);
    RECT wa{};
    SystemParametersInfo(SPI_GETWORKAREA, 0, &wa, 0);
    int ph = PopupHeight(s);
    int py = r.bottom;
    if (py + ph > wa.bottom)
        py = r.top - ph;
    SetWindowPos(s->popup_hwnd, HWND_TOPMOST, r.left, py, kPopupMinW, ph, SWP_NOACTIVATE | SWP_SHOWWINDOW);
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
    UINT32 bg_color = ResolveDtpThemeColor(s->bg_color ? s->bg_color : 13);
    UINT32 border_color = ResolveDtpThemeColor(s->border_color ? s->border_color : 9);
    UINT32 divider_color = 0xFFF0F2F5;
    UINT32 button_color = bg_color;
    UINT32 arrow_color = 0xFFC0C4CC;
    float border_width = 1.0f;

    if (!s->enabled) {
        bg_color = 0xFFF5F7FA;
        button_color = 0xFFF5F7FA;
        border_color = 0xFFE4E7ED;
        arrow_color = 0xFFC0C4CC;
    } else if (s->dropdown_visible) {
        border_color = 0xFF409EFF;
        divider_color = 0xFFE3F0FF;
        button_color = 0xFFF5FAFF;
        arrow_color = 0xFF409EFF;
        border_width = 1.5f;
    } else if (s->button_pressed) {
        border_color = 0xFF409EFF;
        divider_color = 0xFFE3F0FF;
        button_color = 0xFFECF5FF;
        arrow_color = 0xFF409EFF;
    } else if (s->button_hovered || s->host_hovered) {
        border_color = 0xFFC0C4CC;
        divider_color = 0xFFEAECEF;
        button_color = s->button_hovered ? 0xFFF5F7FA : bg_color;
        arrow_color = s->button_hovered ? 0xFF909399 : 0xFFC0C4CC;
    }

    D2D1_ROUNDED_RECT host_rect = D2D1::RoundedRect(
        D2D1::RectF(0.5f, 0.5f, (float)W - 0.5f, (float)H - 0.5f),
        kHostCornerRadius,
        kHostCornerRadius
    );

    ID2D1SolidColorBrush* bg = nullptr;
    ID2D1SolidColorBrush* btn = nullptr;
    ID2D1SolidColorBrush* brd = nullptr;
    ID2D1SolidColorBrush* divider = nullptr;
    ID2D1SolidColorBrush* arrow = nullptr;
    s->render_target->CreateSolidColorBrush(ColorFromUInt32(bg_color), &bg);
    s->render_target->CreateSolidColorBrush(ColorFromUInt32(button_color), &btn);
    s->render_target->CreateSolidColorBrush(ColorFromUInt32(border_color), &brd);
    s->render_target->CreateSolidColorBrush(ColorFromUInt32(divider_color), &divider);
    s->render_target->CreateSolidColorBrush(ColorFromUInt32(arrow_color), &arrow);

    if (bg) {
        s->render_target->FillRoundedRectangle(host_rect, bg);
    }
    if (btn) {
        D2D1_ROUNDED_RECT btn_rect = D2D1::RoundedRect(
            D2D1::RectF((float)bx + 2.0f, 2.0f, (float)W - 2.0f, (float)H - 2.0f),
            5.0f,
            5.0f
        );
        s->render_target->FillRoundedRectangle(btn_rect, btn);
    }
    if (divider) {
        s->render_target->DrawLine(
            D2D1::Point2F((float)bx + 0.5f, 6.0f),
            D2D1::Point2F((float)bx + 0.5f, (float)H - 6.0f),
            divider,
            1.0f
        );
    }
    if (brd) {
        s->render_target->DrawRoundedRectangle(host_rect, brd, border_width);
    }
    DrawChevron(s->render_target, arrow, (float)bx + s->button_width * 0.5f, (float)H * 0.5f, s->dropdown_visible);

    if (arrow) arrow->Release();
    if (divider) divider->Release();
    if (brd) brd->Release();
    if (btn) btn->Release();
    if (bg) bg->Release();

    s->render_target->EndDraw();
}

// 时间行：ty 为时间文本顶边；确定按钮与绘制、命中共用同一宽度
static void GetTimeAndOkLayout(int W, int pad, int date_grid_bottom, int* ty, int* oky, int* ok_btn_w) {
    *ty = date_grid_bottom + pad;
    *oky = *ty + 30 + pad;
    *ok_btn_w = 56;
}

// 时分秒分段几何（与绘制一致）：时/分/秒各占 segW，中间为冒号区
static void GetTimeSegmentLayout(int W, int pad, DateTimePickerPrecision p,
    int* nseg, int seg_left[3], int seg_right[3], int* colon1_left, int* colon2_left) {
    const int segW = (p == DTP_PRECISION_YMDH) ? 86 : 58;
    const int colonW = 14;
    *colon1_left = *colon2_left = -1;
    if (p == DTP_PRECISION_YMDH) {
        *nseg = 1;
        int left = (W - segW) / 2;
        if (left < pad) left = pad;
        seg_left[0] = left;
        seg_right[0] = left + segW;
        return;
    }
    *nseg = (p == DTP_PRECISION_YMDHM) ? 2 : 3;
    int total_w = segW * (*nseg) + colonW * (*nseg - 1);
    int x = (W - total_w) / 2;
    if (x < pad) x = pad;
    seg_left[0] = x;
    seg_right[0] = x + segW;
    x = seg_right[0];
    *colon1_left = x;
    x += colonW;
    seg_left[1] = x;
    seg_right[1] = x + segW;
    if (p == DTP_PRECISION_YMDHM) {
        return;
    }
    x = seg_right[1];
    *colon2_left = x;
    x += colonW;
    seg_left[2] = x;
    seg_right[2] = x + segW;
}

static int HitTestTimeSegment(DtpState* s, int W, int pad, int x, int y, int ty) {
    if (s->precision < DTP_PRECISION_YMDH) return -1;
    if (y < ty || y >= ty + 32) return -1;
    int nseg = 0, seg_left[3], seg_right[3], c1 = -1, c2 = -1;
    GetTimeSegmentLayout(W, pad, s->precision, &nseg, seg_left, seg_right, &c1, &c2);
    for (int i = 0; i < nseg; i++) {
        if (x >= seg_left[i] && x < seg_right[i])
            return i;
    }
    if (nseg <= 1)
        return (x >= pad && x < W - pad) ? 0 : -1;
    int best = 0, bestd = 0x7fffffff;
    for (int i = 0; i < nseg; i++) {
        int cx = (seg_left[i] + seg_right[i]) / 2;
        int d = (x > cx) ? (x - cx) : (cx - x);
        if (d < bestd) {
            bestd = d;
            best = i;
        }
    }
    return best;
}

static void ApplyTimeDigit(DtpState* s, int d) {
    if (s->precision < DTP_PRECISION_YMDH) return;
    int ht = s->hover_time;
    if (ht < 0)
        ht = 0;
    if (s->precision == DTP_PRECISION_YMDH)
        ht = 0;
    if (s->time_typing == 0) {
        s->time_typing = 1;
        s->time_typing_digit = d;
        return;
    }
    int val = s->time_typing_digit * 10 + d;
    s->time_typing = 0;
    if (ht == 0) {
        if (val > 23) val = 23;
        s->temp.wHour = (WORD)val;
    } else if (ht == 1 && s->precision >= DTP_PRECISION_YMDHM) {
        if (val > 59) val = 59;
        s->temp.wMinute = (WORD)val;
    } else if (ht == 2 && s->precision >= DTP_PRECISION_YMDHMS) {
        if (val > 59) val = 59;
        s->temp.wSecond = (WORD)val;
    }
    ClampDate(s->temp);
}

static void CommitPendingTimeDigit(DtpState* s) {
    if (s->time_typing != 1) return;
    int d = s->time_typing_digit;
    s->time_typing = 0;
    int ht = s->hover_time;
    if (ht < 0) ht = 0;
    if (s->precision == DTP_PRECISION_YMDH) ht = 0;
    if (ht == 0) {
        if (d > 23) d = 23;
        s->temp.wHour = (WORD)d;
    } else if (ht == 1 && s->precision >= DTP_PRECISION_YMDHM) {
        if (d > 59) d = 59;
        s->temp.wMinute = (WORD)d;
    } else if (ht == 2 && s->precision >= DTP_PRECISION_YMDHMS) {
        if (d > 59) d = 59;
        s->temp.wSecond = (WORD)d;
    }
    ClampDate(s->temp);
}

static void AdjustTimeByDelta(DtpState* s, int inc) {
    s->time_typing = 0;
    if (s->precision == DTP_PRECISION_YMDH) s->hover_time = 0;
    if (s->precision == DTP_PRECISION_YMDHM && s->hover_time > 1) s->hover_time = 1;
    int ht = s->hover_time;
    if (ht < 0) ht = 0;
    if (ht == 0) {
        int h = (int)s->temp.wHour + inc;
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
}

static int GetTimeDropdownItemCount(const DtpState* s) {
    if (!s) return 0;
    if (s->time_dropdown_segment == 0) return 24;
    if (s->time_dropdown_segment == 1 && s->precision >= DTP_PRECISION_YMDHM) return 60;
    if (s->time_dropdown_segment == 2 && s->precision >= DTP_PRECISION_YMDHMS) return 60;
    return 0;
}

static int GetTimeDropdownCurrentValue(const DtpState* s) {
    if (!s) return 0;
    if (s->time_dropdown_segment == 0) return (int)s->temp.wHour;
    if (s->time_dropdown_segment == 1) return (int)s->temp.wMinute;
    if (s->time_dropdown_segment == 2) return (int)s->temp.wSecond;
    return 0;
}

static int GetTimeDropdownVisibleRows(const DtpState* s) {
    int count = GetTimeDropdownItemCount(s);
    return (std::min)(count, 8);
}

static int GetTimeDropdownPanelHeight(const DtpState* s) {
    return GetTimeDropdownVisibleRows(s) * 28 + 8;
}

static void ClampTimeDropdownScroll(DtpState* s) {
    if (!s) return;
    int max_scroll = (std::max)(0, GetTimeDropdownItemCount(s) - GetTimeDropdownVisibleRows(s));
    s->time_dropdown_scroll = (std::max)(0, (std::min)(s->time_dropdown_scroll, max_scroll));
}

static void ShowTimeDropdown(DtpState* s, int segment) {
    if (!s) return;
    s->time_dropdown_segment = segment;
    s->time_dropdown_visible = true;
    s->time_dropdown_hover = -1;
    s->time_dropdown_scroll = GetTimeDropdownCurrentValue(s) - GetTimeDropdownVisibleRows(s) / 2;
    ClampTimeDropdownScroll(s);
}

static void HideTimeDropdown(DtpState* s) {
    if (!s) return;
    s->time_dropdown_visible = false;
    s->time_dropdown_segment = -1;
    s->time_dropdown_hover = -1;
    s->time_dropdown_scroll = 0;
}

static void ApplyTimeDropdownValue(DtpState* s, int value) {
    if (!s) return;
    value = (std::max)(0, value);
    if (s->time_dropdown_segment == 0) s->temp.wHour = (WORD)((std::min)(23, value));
    else if (s->time_dropdown_segment == 1) s->temp.wMinute = (WORD)((std::min)(59, value));
    else if (s->time_dropdown_segment == 2) s->temp.wSecond = (WORD)((std::min)(59, value));
    ClampDate(s->temp);
}

static void GetTimeLayout(DtpState* s, int W, int pad, int date_grid_bottom, int* ty, int* dropdown_top, int* oky, int* ok_btn_w) {
    *ty = date_grid_bottom + pad;
    *dropdown_top = *ty + 34 + 6;
    int drop_h = (s && s->time_dropdown_visible) ? GetTimeDropdownPanelHeight(s) : 0;
    *oky = *dropdown_top + drop_h + (drop_h > 0 ? pad : 0);
    *ok_btn_w = 64;
    UNREFERENCED_PARAMETER(W);
}

static void GetTimeDropdownRect(DtpState* s, int W, int pad, int date_grid_bottom, D2D1_RECT_F* rect) {
    if (!rect) return;
    rect->left = rect->top = rect->right = rect->bottom = 0.0f;
    if (!s || !s->time_dropdown_visible) return;

    int ty = 0, dropdown_top = 0, oky = 0, ok_btn_w = 0;
    GetTimeLayout(s, W, pad, date_grid_bottom, &ty, &dropdown_top, &oky, &ok_btn_w);
    int nseg = 0, sl[3], sr[3], c1 = -1, c2 = -1;
    GetTimeSegmentLayout(W, pad, s->precision, &nseg, sl, sr, &c1, &c2);
    int seg = s->time_dropdown_segment;
    if (seg < 0 || seg >= nseg) return;

    float segment_w = (float)(sr[seg] - sl[seg]);
    float panel_w = (std::max)(72.0f, segment_w);
    float left = (float)sl[seg] + (segment_w - panel_w) * 0.5f;
    if (left + panel_w > W - pad) left = (float)(W - pad) - panel_w;
    if (left < pad) left = (float)pad;
    rect->left = left;
    rect->top = (float)dropdown_top;
    rect->right = left + panel_w;
    rect->bottom = rect->top + (float)GetTimeDropdownPanelHeight(s);
}

static int HitTestTimeDropdownItem(DtpState* s, int W, int pad, int date_grid_bottom, int x, int y) {
    if (!s || !s->time_dropdown_visible) return -1;
    D2D1_RECT_F rect{};
    GetTimeDropdownRect(s, W, pad, date_grid_bottom, &rect);
    if ((float)x < rect.left || (float)x >= rect.right || (float)y < rect.top || (float)y >= rect.bottom) return -1;
    int local_y = y - (int)rect.top - 4;
    if (local_y < 0) return -1;
    int row = local_y / 28;
    int index = s->time_dropdown_scroll + row;
    if (index < 0 || index >= GetTimeDropdownItemCount(s)) return -1;
    return index;
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

static int GetCalendarGridLeft(int W, int pad, int row) {
    int grid_left = (W - row * 7) / 2;
    if (grid_left < pad) grid_left = pad;
    return grid_left;
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

    const UINT32 popup_bg = ResolveDtpThemeColor(s->bg_color ? s->bg_color : 13);
    const UINT32 popup_fg = ResolveDtpThemeColor(s->fg_color ? s->fg_color : 5);
    const UINT32 popup_border = ResolveDtpThemeColor(s->border_color ? s->border_color : 9);
    const UINT32 popup_surface = ResolveDtpThemeColor(14);
    const UINT32 popup_surface_primary = ResolveDtpThemeColor(15);
    const UINT32 popup_primary = ResolveDtpThemeColor(0);
    const UINT32 popup_muted = ResolveDtpThemeColor(6);
    const UINT32 popup_border_light = ResolveDtpThemeColor(10);
    const bool dark_popup = DtpColorLuma(popup_bg) < 0.45f;
    const UINT32 popup_surface_hover = dark_popup ? 0xFF2F4669 : 0xFFEAF3FF;
    const UINT32 popup_border_hover = dark_popup ? 0xFF6AA9FF : 0xFFB3D8FF;

    rt->BeginDraw();
    rt->Clear(ColorFromUInt32(popup_bg));

    ID2D1SolidColorBrush* header_bg = nullptr;
    rt->CreateSolidColorBrush(ColorFromUInt32(popup_surface), &header_bg);
    if (header_bg) {
        rt->FillRectangle(D2D1::RectF(0.0f, 0.0f, (float)W, 44.0f), header_bg);
        header_bg->Release();
    }

    ID2D1SolidColorBrush* border = nullptr;
    rt->CreateSolidColorBrush(ColorFromUInt32(popup_border), &border);
    if (border) {
        rt->DrawRectangle(D2D1::RectF(0.5f, 0.5f, (float)W - 0.5f, (float)H - 0.5f), border, 1.0f);
        rt->DrawLine(D2D1::Point2F(0.0f, 44.0f), D2D1::Point2F((float)W, 44.0f), border, 1.0f);
    }

    IDWriteTextFormat* tf = nullptr;
    IDWriteTextFormat* title_tf = nullptr;
    g_dwrite_factory->CreateTextFormat(
        s->font.font_name.empty() ? L"Microsoft YaHei UI" : s->font.font_name.c_str(),
        nullptr,
        DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL,
        13.0f, L"zh-cn", &tf);
    g_dwrite_factory->CreateTextFormat(
        s->font.font_name.empty() ? L"Microsoft YaHei UI" : s->font.font_name.c_str(),
        nullptr,
        DWRITE_FONT_WEIGHT_BOLD, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL,
        16.0f, L"zh-cn", &title_tf);

    ID2D1SolidColorBrush* fg = nullptr;
    rt->CreateSolidColorBrush(ColorFromUInt32(popup_fg), &fg);

    const int hdr = 36, row = 26, pad = 8;
    const int cal_top = hdr + 20;
    const int date_grid_bottom = cal_top + 6 * row;

    if (s->precision == DTP_PRECISION_YEAR) {
        if (tf && fg) {
            tf->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
            tf->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
            rt->DrawTextW(L"\x2039", 1, tf, D2D1::RectF(8.0f, 6.0f, 36.0f, 34.0f), fg, D2D1_DRAW_TEXT_OPTIONS_ENABLE_COLOR_FONT);
            rt->DrawTextW(L"\x203A", 1, tf, D2D1::RectF((float)W - 36.0f, 6.0f, (float)W - 8.0f, 34.0f), fg,
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
                    rt->CreateSolidColorBrush(ColorFromUInt32(popup_surface_primary), &hb);
                    if (hb) { rt->FillRectangle(cell, hb); hb->Release(); }
                }
                wchar_t ys[8];
                swprintf_s(ys, L"%d", y);
                if (tf && fg)
                    rt->DrawTextW(ys, (UINT32)wcslen(ys), tf, cell, fg, D2D1_DRAW_TEXT_OPTIONS_ENABLE_COLOR_FONT);
            }
        }
    } else {
        if (s->hover_prev || s->hover_next) {
            ID2D1SolidColorBrush* nav_hover = nullptr;
            rt->CreateSolidColorBrush(ColorFromUInt32(popup_surface_primary), &nav_hover);
            if (nav_hover) {
                if (s->hover_prev) {
                    rt->FillRoundedRectangle(
                        D2D1::RoundedRect(D2D1::RectF(10.0f, 8.0f, 34.0f, 32.0f), 6.0f, 6.0f),
                        nav_hover);
                }
                if (s->hover_next) {
                    rt->FillRoundedRectangle(
                        D2D1::RoundedRect(D2D1::RectF((float)W - 34.0f, 8.0f, (float)W - 10.0f, 32.0f), 6.0f, 6.0f),
                        nav_hover);
                }
                nav_hover->Release();
            }
        }
        if (tf && fg) {
            tf->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
            tf->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
            rt->DrawTextW(L"\x2039", 1, tf, D2D1::RectF(10.0f, 8.0f, 34.0f, 32.0f), fg,
                D2D1_DRAW_TEXT_OPTIONS_ENABLE_COLOR_FONT);
            rt->DrawTextW(L"\x203A", 1, tf, D2D1::RectF((float)W - 34.0f, 8.0f, (float)W - 10.0f, 32.0f), fg,
                D2D1_DRAW_TEXT_OPTIONS_ENABLE_COLOR_FONT);
        }
        wchar_t title[32];
        swprintf_s(title, L"%04d-%02d", s->view_y, s->view_m);
        if (title_tf) {
            title_tf->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
            title_tf->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
        }
        if (title_tf && fg)
            rt->DrawTextW(title, (UINT32)wcslen(title), title_tf, D2D1::RectF(44.0f, 6.0f, (float)W - 44.0f, 38.0f), fg,
                D2D1_DRAW_TEXT_OPTIONS_ENABLE_COLOR_FONT);

        int grid_left = GetCalendarGridLeft(W, pad, row);
        static const wchar_t* labs[] = { L"Mo", L"Tu", L"We", L"Th", L"Fr", L"Sa", L"Su" };
        for (int i = 0; i < 7; i++) {
            if (tf && fg)
                rt->DrawTextW(labs[i], (UINT32)wcslen(labs[i]), tf,
                    D2D1::RectF((float)(grid_left + i * row), (float)hdr + 2, (float)(grid_left + (i + 1) * row), (float)hdr + 18),
                    fg, D2D1_DRAW_TEXT_OPTIONS_ENABLE_COLOR_FONT);
        }

        if (tf) {
            tf->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
            tf->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
        }
        for (int i = 0; i < 42; i++) {
            bool in = false;
            int disp = 0;
            CellDay(s->view_y, s->view_m, i, in, disp);
            int r = i / 7, c = i % 7;
            float x0 = (float)(grid_left + c * row);
            float y0 = (float)(cal_top + r * row);
            D2D1_RECT_F cell = D2D1::RectF(x0, y0, x0 + row - 2, y0 + row - 4);
            bool sel = in && (int)s->temp.wYear == s->view_y && (int)s->temp.wMonth == s->view_m &&
                disp == (int)s->temp.wDay;
            wchar_t ds[8];
            swprintf_s(ds, L"%d", disp);
            UINT32 col = popup_fg;
            if (!in)
                col = popup_muted;
            if (sel) {
                float cx = (cell.left + cell.right) * 0.5f;
                float cy = (cell.top + cell.bottom) * 0.5f;
                float rx = (cell.right - cell.left) * 0.5f - 1.0f;
                float ry = (cell.bottom - cell.top) * 0.5f - 1.0f;
                float rad = (std::min)(rx, ry);
                if (rad > 2.0f) {
                    ID2D1SolidColorBrush* sb = nullptr;
                    rt->CreateSolidColorBrush(ColorFromUInt32(popup_primary), &sb);
                    if (sb) {
                        rt->FillEllipse(D2D1::Ellipse(D2D1::Point2F(cx, cy), rad, rad), sb);
                        sb->Release();
                    }
                }
                col = DtpAutoContrastText(popup_primary);
            } else if (s->hover_cell == i) {
                ID2D1SolidColorBrush* hb = nullptr;
                ID2D1SolidColorBrush* hover_border = nullptr;
                D2D1_ROUNDED_RECT hover_rect = D2D1::RoundedRect(cell, 6.0f, 6.0f);
                rt->CreateSolidColorBrush(ColorFromUInt32(popup_surface_hover), &hb);
                rt->CreateSolidColorBrush(ColorFromUInt32(popup_border_hover), &hover_border);
                if (hb) {
                    rt->FillRoundedRectangle(hover_rect, hb);
                    hb->Release();
                }
                if (hover_border) {
                    rt->DrawRoundedRectangle(hover_rect, hover_border, 1.0f);
                    hover_border->Release();
                }
            }
            ID2D1SolidColorBrush* tc = nullptr;
            rt->CreateSolidColorBrush(ColorFromUInt32(col), &tc);
            if (tf && tc) {
                D2D1_DRAW_TEXT_OPTIONS dopts = (D2D1_DRAW_TEXT_OPTIONS)(
                    (int)D2D1_DRAW_TEXT_OPTIONS_ENABLE_COLOR_FONT | (int)D2D1_DRAW_TEXT_OPTIONS_CLIP);
                rt->DrawTextW(ds, (UINT32)wcslen(ds), tf, cell, tc, dopts);
            }
            if (tc) tc->Release();
        }
        if (tf) {
            tf->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
            tf->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
        }

        if (s->precision >= DTP_PRECISION_YMDH) {
            int ty = 0, dropdown_top = 0, oky = 0, ok_btn_w = 56;
            GetTimeLayout(s, W, pad, date_grid_bottom, &ty, &dropdown_top, &oky, &ok_btn_w);
            int nseg = 0, sl[3], sr[3], c1 = -1, c2 = -1;
            GetTimeSegmentLayout(W, pad, s->precision, &nseg, sl, sr, &c1, &c2);
            if (tf) {
                tf->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
                tf->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
            }
            wchar_t buf[8];
            auto drawSeg = [&](int idx, int value) {
                swprintf_s(buf, L"%02d", value);
                D2D1_RECT_F seg_rect = D2D1::RectF((float)sl[idx], (float)ty, (float)sr[idx], (float)(ty + 32));
                bool active = s->time_dropdown_visible && s->time_dropdown_segment == idx;
                bool hovered = s->hover_time == idx;

                ID2D1SolidColorBrush* seg_bg = nullptr;
                ID2D1SolidColorBrush* seg_border = nullptr;
                UINT32 bg_color = active ? popup_surface_primary : (hovered ? popup_surface_hover : popup_bg);
                UINT32 border_color = active ? popup_primary : (hovered ? popup_border_hover : popup_border_light);
                rt->CreateSolidColorBrush(ColorFromUInt32(bg_color), &seg_bg);
                rt->CreateSolidColorBrush(ColorFromUInt32(border_color), &seg_border);
                if (seg_bg) {
                    rt->FillRoundedRectangle(D2D1::RoundedRect(seg_rect, 4.0f, 4.0f), seg_bg);
                    seg_bg->Release();
                }
                if (seg_border) {
                    rt->DrawRoundedRectangle(D2D1::RoundedRect(seg_rect, 4.0f, 4.0f), seg_border, active ? 1.5f : 1.0f);
                    seg_border->Release();
                }
                if (tf && fg) {
                    D2D1_DRAW_TEXT_OPTIONS dopts = (D2D1_DRAW_TEXT_OPTIONS)(
                        (int)D2D1_DRAW_TEXT_OPTIONS_ENABLE_COLOR_FONT | (int)D2D1_DRAW_TEXT_OPTIONS_CLIP);
                    rt->DrawTextW(buf, 2, tf,
                        D2D1::RectF(seg_rect.left + 2.0f, seg_rect.top, seg_rect.right - 16.0f, seg_rect.bottom),
                        fg, dopts);
                    rt->DrawTextW(L"\u25BE", 1, tf,
                        D2D1::RectF(seg_rect.right - 18.0f, seg_rect.top, seg_rect.right - 4.0f, seg_rect.bottom),
                        fg, dopts);
                }
            };
            if (s->precision == DTP_PRECISION_YMDH) {
                drawSeg(0, (int)s->temp.wHour);
            } else if (s->precision == DTP_PRECISION_YMDHM) {
                drawSeg(0, (int)s->temp.wHour);
                if (tf && fg && c1 >= 0) {
                    D2D1_DRAW_TEXT_OPTIONS dopts = (D2D1_DRAW_TEXT_OPTIONS)(
                        (int)D2D1_DRAW_TEXT_OPTIONS_ENABLE_COLOR_FONT | (int)D2D1_DRAW_TEXT_OPTIONS_CLIP);
                    rt->DrawTextW(L":", 1, tf,
                        D2D1::RectF((float)c1, (float)ty, (float)(c1 + 14), (float)(ty + 32)), fg, dopts);
                }
                drawSeg(1, (int)s->temp.wMinute);
            } else {
                drawSeg(0, (int)s->temp.wHour);
                if (tf && fg && c1 >= 0) {
                    D2D1_DRAW_TEXT_OPTIONS dopts = (D2D1_DRAW_TEXT_OPTIONS)(
                        (int)D2D1_DRAW_TEXT_OPTIONS_ENABLE_COLOR_FONT | (int)D2D1_DRAW_TEXT_OPTIONS_CLIP);
                    rt->DrawTextW(L":", 1, tf,
                        D2D1::RectF((float)c1, (float)ty, (float)(c1 + 14), (float)(ty + 32)), fg, dopts);
                }
                drawSeg(1, (int)s->temp.wMinute);
                if (tf && fg && c2 >= 0) {
                    D2D1_DRAW_TEXT_OPTIONS dopts = (D2D1_DRAW_TEXT_OPTIONS)(
                        (int)D2D1_DRAW_TEXT_OPTIONS_ENABLE_COLOR_FONT | (int)D2D1_DRAW_TEXT_OPTIONS_CLIP);
                    rt->DrawTextW(L":", 1, tf,
                        D2D1::RectF((float)c2, (float)ty, (float)(c2 + 14), (float)(ty + 32)), fg, dopts);
                }
                drawSeg(2, (int)s->temp.wSecond);
            }
            float ok_left = (float)(W - pad - ok_btn_w);
            D2D1_RECT_F cancelr = D2D1::RectF((float)pad, (float)oky, (float)(pad + ok_btn_w), (float)(oky + 26));
            D2D1_RECT_F okr = D2D1::RectF(ok_left, (float)oky, (float)(W - pad), (float)(oky + 26));
            ID2D1SolidColorBrush* cancel_bg = nullptr;
            ID2D1SolidColorBrush* cancel_border = nullptr;
            ID2D1SolidColorBrush* ok_bg = nullptr;
            rt->CreateSolidColorBrush(ColorFromUInt32(s->hover_cancel ? popup_surface_primary : popup_surface), &cancel_bg);
            rt->CreateSolidColorBrush(ColorFromUInt32(s->hover_cancel ? popup_primary : popup_border_light), &cancel_border);
            rt->CreateSolidColorBrush(ColorFromUInt32(popup_primary), &ok_bg);
            if (cancel_bg) {
                rt->FillRoundedRectangle(D2D1::RoundedRect(cancelr, 4.0f, 4.0f), cancel_bg);
                cancel_bg->Release();
            }
            if (cancel_border) {
                rt->DrawRoundedRectangle(D2D1::RoundedRect(cancelr, 4.0f, 4.0f), cancel_border, 1.0f);
                cancel_border->Release();
            }
            if (ok_bg) {
                rt->FillRoundedRectangle(D2D1::RoundedRect(okr, 4.0f, 4.0f), ok_bg);
                ok_bg->Release();
            }
            if (tf && fg) {
                ID2D1SolidColorBrush* cancel_text = nullptr;
                ID2D1SolidColorBrush* ok_text = nullptr;
                rt->CreateSolidColorBrush(ColorFromUInt32(s->hover_cancel ? popup_primary : popup_fg), &cancel_text);
                rt->CreateSolidColorBrush(ColorFromUInt32(DtpAutoContrastText(popup_primary)), &ok_text);
                D2D1_DRAW_TEXT_OPTIONS cancelopts = (D2D1_DRAW_TEXT_OPTIONS)(
                    (int)D2D1_DRAW_TEXT_OPTIONS_ENABLE_COLOR_FONT | (int)D2D1_DRAW_TEXT_OPTIONS_CLIP);
                rt->DrawTextW(L"\u53d6\u6d88", 2, tf, cancelr, cancel_text ? cancel_text : fg, cancelopts);
                D2D1_DRAW_TEXT_OPTIONS okopts = (D2D1_DRAW_TEXT_OPTIONS)(
                    (int)D2D1_DRAW_TEXT_OPTIONS_ENABLE_COLOR_FONT | (int)D2D1_DRAW_TEXT_OPTIONS_CLIP);
                rt->DrawTextW(L"\u786e\u5b9a", 2, tf, okr, ok_text ? ok_text : fg, okopts);
                if (cancel_text) cancel_text->Release();
                if (ok_text) ok_text->Release();
            }
            if (tf) {
                tf->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
                tf->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
            }

            if (s->time_dropdown_visible) {
                D2D1_RECT_F dropdown_rect{};
                GetTimeDropdownRect(s, W, pad, date_grid_bottom, &dropdown_rect);
                if (dropdown_rect.right > dropdown_rect.left && dropdown_rect.bottom > dropdown_rect.top) {
                    ID2D1SolidColorBrush* dd_bg = nullptr;
                    ID2D1SolidColorBrush* dd_border = nullptr;
                    rt->CreateSolidColorBrush(ColorFromUInt32(popup_bg), &dd_bg);
                    rt->CreateSolidColorBrush(ColorFromUInt32(popup_border_light), &dd_border);
                    if (dd_bg) {
                        rt->FillRoundedRectangle(D2D1::RoundedRect(dropdown_rect, 6.0f, 6.0f), dd_bg);
                        dd_bg->Release();
                    }
                    if (dd_border) {
                        rt->DrawRoundedRectangle(D2D1::RoundedRect(dropdown_rect, 6.0f, 6.0f), dd_border, 1.0f);
                        dd_border->Release();
                    }

                    if (tf) {
                        tf->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
                        tf->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
                    }
                    int visible_rows = GetTimeDropdownVisibleRows(s);
                    int item_count = GetTimeDropdownItemCount(s);
                    for (int i = 0; i < visible_rows; ++i) {
                        int item_index = s->time_dropdown_scroll + i;
                        if (item_index >= item_count) break;
                        float top = dropdown_rect.top + 4.0f + i * 28.0f;
                        D2D1_RECT_F item_rect = D2D1::RectF(dropdown_rect.left + 4.0f, top, dropdown_rect.right - 4.0f, top + 24.0f);
                        bool selected = (item_index == GetTimeDropdownCurrentValue(s));
                        bool hovered = (item_index == s->time_dropdown_hover);
                        if (selected || hovered) {
                            ID2D1SolidColorBrush* hi = nullptr;
                            ID2D1SolidColorBrush* hi_border = nullptr;
                            UINT32 hi_color = selected ? popup_surface_primary : popup_surface_hover;
                            rt->CreateSolidColorBrush(ColorFromUInt32(hi_color), &hi);
                            rt->CreateSolidColorBrush(ColorFromUInt32(selected ? popup_primary : popup_border_hover), &hi_border);
                            if (hi) {
                                rt->FillRoundedRectangle(D2D1::RoundedRect(item_rect, 4.0f, 4.0f), hi);
                                hi->Release();
                            }
                            if (hi_border) {
                                rt->DrawRoundedRectangle(D2D1::RoundedRect(item_rect, 4.0f, 4.0f), hi_border, 1.0f);
                                hi_border->Release();
                            }
                        }
                        wchar_t item_text[8];
                        swprintf_s(item_text, L"%02d", item_index);
                        if (tf && fg) {
                            rt->DrawTextW(item_text, 2, tf, item_rect, fg, D2D1_DRAW_TEXT_OPTIONS_ENABLE_COLOR_FONT);
                        }
                    }
                    if (tf) {
                        tf->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
                        tf->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
                    }
                }
            }
        }
    }

    if (fg) fg->Release();
    if (border) border->Release();
    if (title_tf) title_tf->Release();
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
        s->hover_cancel = false;
        s->hover_cell = -1;
        s->hover_prev = (x >= 8 && x <= 36 && y >= 6 && y <= 34);
        s->hover_next = false;
        const int hdr = 36, row = 26, pad = 8;
        const int cal_top = hdr + 20;
        const int date_grid_bottom = cal_top + 6 * row;
        RECT rc{};
        GetClientRect(hwnd, &rc);
        int W = rc.right - rc.left;
        int grid_left = GetCalendarGridLeft(W, pad, row);
        s->hover_next = (x >= W - 36 && x <= W - 8 && y >= 6 && y <= 34);
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
            if (y > cal_top && y < date_grid_bottom) {
                int col = (x - grid_left) / row;
                int rowidx = (y - cal_top) / row;
                if (col >= 0 && col < 7 && rowidx >= 0 && rowidx < 6)
                    s->hover_cell = rowidx * 7 + col;
            }
            if (s->precision >= DTP_PRECISION_YMDH) {
                int ty = 0, dropdown_top = 0, oky = 0, ok_btn_w = 56;
                GetTimeLayout(s, W, pad, date_grid_bottom, &ty, &dropdown_top, &oky, &ok_btn_w);
                int cancel_right = pad + ok_btn_w;
                int ok_left = W - pad - ok_btn_w;
                s->hover_cancel = (x >= pad && x < cancel_right && y > oky && y < oky + 26);
                s->hover_ok = (x >= ok_left && x < W - pad && y > oky && y < oky + 26);
                s->time_dropdown_hover = HitTestTimeDropdownItem(s, W, pad, date_grid_bottom, x, y);
                if (y >= ty && y < ty + 32 && x >= pad && x < W - pad) {
                    int ht = HitTestTimeSegment(s, W, pad, x, y, ty);
                    if (ht >= 0 && ht != s->hover_time)
                        s->time_typing = 0;
                    if (ht >= 0)
                        s->hover_time = ht;
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
        const int cal_top = hdr + 20;
        const int date_grid_bottom = cal_top + 6 * row;
        int grid_left = GetCalendarGridLeft(W, pad, row);

        if (x >= 8 && x <= 36 && y >= 6 && y <= 34) {
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
        if (x >= W - 36 && x <= W - 8 && y >= 6 && y <= 34) {
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
            int col = (x - grid_left) / row;
            int rowidx = (y - cal_top) / row;
            if (col >= 0 && col < 7 && rowidx >= 0 && rowidx < 6 && y > cal_top && y < date_grid_bottom) {
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
                int ty = 0, dropdown_top = 0, oky = 0, ok_btn_w = 56;
                GetTimeLayout(s, W, pad, date_grid_bottom, &ty, &dropdown_top, &oky, &ok_btn_w);
                int cancel_right = pad + ok_btn_w;
                int ok_left = W - pad - ok_btn_w;
                int dropdown_item = HitTestTimeDropdownItem(s, W, pad, date_grid_bottom, x, y);
                if (dropdown_item >= 0) {
                    ApplyTimeDropdownValue(s, dropdown_item);
                    HideTimeDropdown(s);
                    UpdatePopupPlacement(s);
                    InvalidateRect(hwnd, nullptr, FALSE);
                    return 0;
                }
                if (y >= ty && y < ty + 32 && x >= pad && x < W - pad) {
                    int ht = HitTestTimeSegment(s, W, pad, x, y, ty);
                    if (ht >= 0) {
                        if (ht != s->hover_time)
                            s->time_typing = 0;
                        s->hover_time = ht;
                        if (s->time_dropdown_visible && s->time_dropdown_segment == ht) HideTimeDropdown(s);
                        else ShowTimeDropdown(s, ht);
                        UpdatePopupPlacement(s);
                    }
                    SetFocus(hwnd);
                    InvalidateRect(hwnd, nullptr, FALSE);
                    return 0;
                }
                if (s->time_dropdown_visible) {
                    HideTimeDropdown(s);
                    UpdatePopupPlacement(s);
                }
                if (x >= pad && x < cancel_right && y > oky && y < oky + 26) {
                    HidePopup(s);
                    InvalidateRect(s->hwnd, nullptr, FALSE);
                    return 0;
                }
                if (x >= ok_left && x < W - pad && y > oky && y < oky + 26) {
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
    case WM_CHAR: {
        if (!s || s->precision < DTP_PRECISION_YMDH) break;
        if (GetFocus() != hwnd) break;
        wchar_t ch = (wchar_t)wParam;
        if (ch >= L'0' && ch <= L'9') {
            if (s->hover_time < 0)
                s->hover_time = 0;
            ApplyTimeDigit(s, (int)(ch - L'0'));
            InvalidateRect(hwnd, nullptr, FALSE);
            return 0;
        }
        break;
    }
    case WM_KEYDOWN: {
        if (!s || s->precision < DTP_PRECISION_YMDH) break;
        if (GetFocus() != hwnd) break;
        int vk = (int)wParam;
        if (vk == VK_BACK) {
            s->time_typing = 0;
            InvalidateRect(hwnd, nullptr, FALSE);
            return 0;
        }
        if (vk == VK_RETURN) {
            CommitPendingTimeDigit(s);
            InvalidateRect(hwnd, nullptr, FALSE);
            return 0;
        }
        if (vk == VK_ESCAPE) {
            HidePopup(s);
            InvalidateRect(s->hwnd, nullptr, FALSE);
            return 0;
        }
        if (vk == VK_UP) {
            if (s->hover_time < 0) s->hover_time = 0;
            AdjustTimeByDelta(s, 1);
            InvalidateRect(hwnd, nullptr, FALSE);
            return 0;
        }
        if (vk == VK_DOWN) {
            if (s->hover_time < 0) s->hover_time = 0;
            AdjustTimeByDelta(s, -1);
            InvalidateRect(hwnd, nullptr, FALSE);
            return 0;
        }
        int maxt = 0;
        if (s->precision == DTP_PRECISION_YMDHM) maxt = 1;
        else if (s->precision == DTP_PRECISION_YMDHMS) maxt = 2;
        if (vk == VK_LEFT) {
            if (s->hover_time < 0) s->hover_time = 0;
            s->time_typing = 0;
            if (s->hover_time <= 0) s->hover_time = maxt;
            else s->hover_time--;
            InvalidateRect(hwnd, nullptr, FALSE);
            return 0;
        }
        if (vk == VK_RIGHT) {
            if (s->hover_time < 0) s->hover_time = 0;
            s->time_typing = 0;
            if (s->hover_time >= maxt) s->hover_time = 0;
            else s->hover_time++;
            InvalidateRect(hwnd, nullptr, FALSE);
            return 0;
        }
        break;
    }
    case WM_MOUSEWHEEL: {
        if (!s || s->precision < DTP_PRECISION_YMDH) break;
        short d = GET_WHEEL_DELTA_WPARAM(wParam);
        int inc = (d > 0) ? 1 : -1;
        if (s->time_dropdown_visible) {
            s->time_dropdown_scroll -= inc;
            ClampTimeDropdownScroll(s);
            InvalidateRect(hwnd, nullptr, FALSE);
            return 0;
        }
        if (s->hover_time < 0) s->hover_time = 0;
        AdjustTimeByDelta(s, inc);
        InvalidateRect(hwnd, nullptr, FALSE);
        return 0;
    }
    case WM_ACTIVATE:
        if (LOWORD(wParam) == WA_INACTIVE && s)
            PostMessageW(s->hwnd, WM_DTP_CLOSE_POPUP, 0, 0);
        break;
    case WM_MOUSEACTIVATE:
        return MA_ACTIVATE;
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
        bool needs_redraw = false;
        if (!s->host_hovered) {
            s->host_hovered = true;
            TRACKMOUSEEVENT tme = { sizeof(tme), TME_LEAVE, hwnd, 0 };
            TrackMouseEvent(&tme);
            needs_redraw = true;
        }
        bool h = (x >= s->width - s->button_width);
        if (h != s->button_hovered) {
            s->button_hovered = h;
            needs_redraw = true;
        }
        if (needs_redraw) {
            InvalidateRect(hwnd, nullptr, FALSE);
        }
        break;
    }
    case WM_MOUSELEAVE:
        if (s) {
            bool needs_redraw = s->host_hovered || s->button_hovered;
            s->host_hovered = false;
            s->button_hovered = false;
            if (needs_redraw) {
                InvalidateRect(hwnd, nullptr, FALSE);
            }
        }
        break;
    case WM_SIZE:
        if (s) {
            s->width = LOWORD(lParam);
            s->height = HIWORD(lParam);
            LayoutHostEdit(s);
            ApplyHostRegion(s);
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
    ApplyHostRegion(st);

    int ew = width - kButtonWidth - kHostPaddingX - 4;
    if (ew < 1) ew = 1;
    st->edit_hwnd = CreateD2DColorEmojiEditBox(
        hwnd, kHostPaddingX, kHostPaddingY, ew, height - kHostPaddingY * 2,
        (const unsigned char*)"", 0,
        fg_color, bg_color,
        font_name_bytes, font_name_len,
        font_size, bold != 0, italic != 0, underline != 0,
        0, false, true, false, false, true);
    if (st->edit_hwnd) {
        SetWindowSubclass(st->edit_hwnd, EditSubclassProc, kEditSubclassId, (DWORD_PTR)st);
        LayoutHostEdit(st);
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

__declspec(dllexport) void __stdcall SetD2DDateTimePickerColors(
    HWND hPicker,
    UINT32 fg_color,
    UINT32 bg_color,
    UINT32 border_color
) {
    auto it = g_dt_pick.find(hPicker);
    if (it == g_dt_pick.end()) return;

    DtpState* s = it->second;
    s->fg_color = fg_color;
    s->bg_color = bg_color;
    s->border_color = border_color;
    if (s->edit_hwnd) {
        SetD2DEditBoxColor(s->edit_hwnd, fg_color, bg_color);
    }
    InvalidateRect(hPicker, nullptr, FALSE);
    if (s->dropdown_visible && s->popup_hwnd) {
        InvalidateRect(s->popup_hwnd, nullptr, FALSE);
    }
}

__declspec(dllexport) int __stdcall GetD2DDateTimePickerColors(
    HWND hPicker,
    UINT32* fg_color,
    UINT32* bg_color,
    UINT32* border_color
) {
    auto it = g_dt_pick.find(hPicker);
    if (it == g_dt_pick.end()) return -1;

    DtpState* s = it->second;
    if (fg_color) *fg_color = s->fg_color;
    if (bg_color) *bg_color = s->bg_color;
    if (border_color) *border_color = s->border_color;
    return 0;
}

} // extern C
