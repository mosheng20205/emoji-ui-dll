#pragma once

#include <windows.h>
#include <commctrl.h>
#include <d2d1.h>
#include <dwrite.h>
#include <uxtheme.h>
#include <wincodec.h>  // WIC (Windows Imaging Component)
#include <richedit.h>  // RichEdit鎺т欢锛堟敮鎸佸僵鑹瞖moji锛?
#include <dwmapi.h>    // DWM (Desktop Window Manager) - 鑷畾涔夋爣棰樻爮
#include <string>
#include <vector>
#include <map>

#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "dwrite.lib")
#pragma comment(lib, "comctl32.lib")
#pragma comment(lib, "uxtheme.lib")
#pragma comment(lib, "windowscodecs.lib")  // WIC搴?
#pragma comment(lib, "dwmapi.lib")         // DWM搴?

// Button click callback type (stdcall)
// 鍙傛暟: button_id - 鎸夐挳ID, parent_hwnd - 鐖剁獥鍙ｅ彞鏌?
typedef void (__stdcall *ButtonClickCallback)(int button_id, HWND parent_hwnd);

// Message box callback type (confirmed: 1=OK, 0=Cancel)
typedef void (__stdcall *MessageBoxCallback)(int confirmed);

// Tab 鍒囨崲鍥炶皟鍑芥暟绫诲瀷 (stdcall 璋冪敤绾﹀畾)
typedef void (__stdcall *TAB_CALLBACK)(HWND hTabControl, int selectedIndex);

// 鑾峰彇鐖剁獥鍙ｇ殑鏍囬鏍忓亸绉婚噺锛堝鏋滄湁鑷畾涔夋爣棰樻爮锛?
int GetTitleBarOffset(HWND hParent);

// Tab 鍏抽棴鍥炶皟鍑芥暟绫诲瀷 (stdcall 璋冪敤绾﹀畾)
typedef void (__stdcall *TAB_CLOSE_CALLBACK)(HWND hTabControl, int index);

// Tab 鍙抽敭鐐瑰嚮鍥炶皟鍑芥暟绫诲瀷 (stdcall 璋冪敤绾﹀畾)
typedef void (__stdcall *TAB_RIGHTCLICK_CALLBACK)(HWND hTabControl, int index, int x, int y);

// Tab 鍙屽嚮鍥炶皟鍑芥暟绫诲瀷 (stdcall 璋冪敤绾﹀畾)
typedef void (__stdcall *TAB_DBLCLICK_CALLBACK)(HWND hTabControl, int index);
typedef void (__stdcall *TAB_NEW_BUTTON_CALLBACK)(HWND hTabControl);

// 绐楀彛澶у皬鏀瑰彉鍥炶皟鍑芥暟绫诲瀷 (stdcall 璋冪敤绾﹀畾)
typedef void (__stdcall *WindowResizeCallback)(HWND hwnd, int width, int height);

// 鑷粯绐楀彛鍏抽棴鍥炶皟鍑芥暟绫诲瀷 (stdcall 璋冪敤绾﹀畾)
// hwnd: 琚叧闂殑绐楀彛鍙ユ焺锛堟鏃?HWND 宸插け鏁堬紝浠呯敤浜庤瘑鍒槸鍝釜绐楀彛锛?
typedef void (__stdcall *WindowCloseCallback)(HWND hwnd);

// 鑿滃崟椤圭偣鍑诲洖璋冨嚱鏁扮被鍨?(stdcall 璋冪敤绾﹀畾)
// menu_id: 椤剁骇鑿滃崟ID锛堝 #鑿滃崟_涓婚璁剧疆 / #鑿滃崟_娴嬭瘯鑿滃崟锛?
// item_id: 琚偣鍑荤殑鑿滃崟椤笽D锛堝瓙椤笽D鎴栭《绾D锛?
typedef void (__stdcall *MenuItemClickCallback)(int menu_id, int item_id);

// 鍓嶅悜澹版槑瀛愯彍鍗曠獥鍙ｇ被
class SubMenuWindow;

// 缂栬緫妗嗘寜閿洖璋?(stdcall)锛歨Edit 鍙ユ焺, key_code 铏氭嫙閿爜, key_down 1=鎸変笅 0=鏉惧紑, shift/ctrl/alt 淇グ閿槸鍚︽寜涓?0/1)
typedef void (__stdcall *EditBoxKeyCallback)(HWND hEdit, int key_code, int key_down, int shift, int ctrl, int alt);

// 澶嶉€夋鍥炶皟鍑芥暟绫诲瀷 (stdcall 璋冪敤绾﹀畾)
typedef void (__stdcall *CheckBoxCallback)(HWND hCheckBox, BOOL checked);

// 杩涘害鏉″洖璋冨嚱鏁扮被鍨?(stdcall 璋冪敤绾﹀畾)
typedef void (__stdcall *ProgressBarCallback)(HWND hProgressBar, int value);

// 鍥剧墖缂╂斁妯″紡
enum ImageScaleMode {
    SCALE_NONE = 0,         // 涓嶇缉鏀?
    SCALE_STRETCH = 1,      // 鎷変几濉厖
    SCALE_FIT = 2,          // 绛夋瘮缂╂斁閫傚簲
    SCALE_CENTER = 3        // 灞呬腑鏄剧ず
};

// 鍥剧墖妗嗗洖璋冨嚱鏁扮被鍨?(stdcall 璋冪敤绾﹀畾)
typedef void (__stdcall *PictureBoxCallback)(HWND hPictureBox);

// 鍗曢€夋寜閽洖璋冨嚱鏁扮被鍨?(stdcall 璋冪敤绾﹀畾)
typedef void (__stdcall *RadioButtonCallback)(HWND hRadioButton, int group_id, BOOL checked);

typedef void (__stdcall *SliderCallback)(HWND hSlider, int value);
typedef void (__stdcall *SwitchCallback)(HWND hSwitch, BOOL checked);
typedef void (__stdcall *NotificationCallback)(HWND hNotification, int event_type);

// 鍒楄〃妗嗗洖璋冨嚱鏁扮被鍨?(stdcall 璋冪敤绾﹀畾)
typedef void (__stdcall *ListBoxCallback)(HWND hListBox, int index);

// 缁勫悎妗嗗洖璋冨嚱鏁扮被鍨?(stdcall 璋冪敤绾﹀畾)
typedef void (__stdcall *ComboBoxCallback)(HWND hComboBox, int index);

// 鐑敭鎺т欢鍥炶皟鍑芥暟绫诲瀷 (stdcall 璋冪敤绾﹀畾)
typedef void (__stdcall *HotKeyCallback)(HWND hHotKey, int vk_code, int modifiers);

// ========== 閫氱敤浜嬩欢鍥炶皟绫诲瀷 (闇€姹?8.1-8.10) ==========

// 榧犳爣杩涘叆/绂诲紑鍥炶皟 (hwnd)
typedef void (__stdcall *MouseEnterCallback)(HWND hwnd);
typedef void (__stdcall *MouseLeaveCallback)(HWND hwnd);

// 鍙屽嚮鍥炶皟 (hwnd, x, y)
typedef void (__stdcall *DoubleClickCallback)(HWND hwnd, int x, int y);

// 鍙抽敭鐐瑰嚮鍥炶皟 (hwnd, x, y)
typedef void (__stdcall *RightClickCallback)(HWND hwnd, int x, int y);

// 鐒︾偣鍥炶皟 (hwnd)
typedef void (__stdcall *FocusCallback)(HWND hwnd);
typedef void (__stdcall *BlurCallback)(HWND hwnd);

// 閿洏鍥炶皟 (hwnd, vk_code, shift, ctrl, alt)
typedef void (__stdcall *KeyDownCallback)(HWND hwnd, int vk_code, int shift, int ctrl, int alt);
typedef void (__stdcall *KeyUpCallback)(HWND hwnd, int vk_code, int shift, int ctrl, int alt);

// 瀛楃杈撳叆鍥炶皟 (hwnd, char_code) - Unicode瀛楃
typedef void (__stdcall *CharCallback)(HWND hwnd, int char_code);

// 鍊兼敼鍙樺洖璋?(hwnd)
typedef void (__stdcall *ValueChangedCallback)(HWND hwnd);

// 閫氱敤浜嬩欢鍥炶皟闆嗗悎
struct EventCallbacks {
    MouseEnterCallback on_mouse_enter;
    MouseLeaveCallback on_mouse_leave;
    DoubleClickCallback on_double_click;
    RightClickCallback on_right_click;
    FocusCallback on_focus;
    BlurCallback on_blur;
    KeyDownCallback on_key_down;
    KeyUpCallback on_key_up;
    CharCallback on_char;
    ValueChangedCallback on_value_changed;

    EventCallbacks() : on_mouse_enter(nullptr), on_mouse_leave(nullptr),
        on_double_click(nullptr), on_right_click(nullptr),
        on_focus(nullptr), on_blur(nullptr),
        on_key_down(nullptr), on_key_up(nullptr),
        on_char(nullptr), on_value_changed(nullptr) {}
};

// 鏂囨湰瀵归綈鏂瑰紡
enum TextAlignment {
    ALIGN_LEFT = 0,
    ALIGN_CENTER = 1,
    ALIGN_RIGHT = 2
};

enum CheckBoxVisualStyle {
    CHECKBOX_STYLE_DEFAULT = 0,
    CHECKBOX_STYLE_FILL = 1,
    CHECKBOX_STYLE_BUTTON = 2,
    CHECKBOX_STYLE_CARD = 3
};

enum RadioVisualStyle {
    RADIO_STYLE_DEFAULT = 0,
    RADIO_STYLE_BORDER = 1,
    RADIO_STYLE_BUTTON = 2
};

enum ButtonVisualType {
    BUTTON_TYPE_AUTO = -1,
    BUTTON_TYPE_DEFAULT = 0,
    BUTTON_TYPE_PRIMARY = 1,
    BUTTON_TYPE_SUCCESS = 2,
    BUTTON_TYPE_WARNING = 3,
    BUTTON_TYPE_DANGER = 4,
    BUTTON_TYPE_INFO = 5
};

enum ButtonVisualStyle {
    BUTTON_STYLE_SOLID = 0,
    BUTTON_STYLE_PLAIN = 1,
    BUTTON_STYLE_TEXT = 2,
    BUTTON_STYLE_LINK = 3
};

enum ButtonVisualSize {
    BUTTON_SIZE_LARGE = 0,
    BUTTON_SIZE_DEFAULT = 1,
    BUTTON_SIZE_SMALL = 2
};

enum PopupPlacement {
    POPUP_TOP = 0,
    POPUP_TOP_START = 1,
    POPUP_TOP_END = 2,
    POPUP_BOTTOM = 3,
    POPUP_BOTTOM_START = 4,
    POPUP_BOTTOM_END = 5,
    POPUP_LEFT = 6,
    POPUP_LEFT_START = 7,
    POPUP_LEFT_END = 8,
    POPUP_RIGHT = 9,
    POPUP_RIGHT_START = 10,
    POPUP_RIGHT_END = 11
};

enum NotificationPosition {
    NOTIFY_TOP_RIGHT = 0,
    NOTIFY_TOP_LEFT = 1,
    NOTIFY_BOTTOM_RIGHT = 2,
    NOTIFY_BOTTOM_LEFT = 3
};

enum NotificationType {
    NOTIFY_INFO = 0,
    NOTIFY_SUCCESS = 1,
    NOTIFY_WARNING = 2,
    NOTIFY_ERROR = 3
};

enum TooltipThemeMode {
    TOOLTIP_THEME_DARK = 0,
    TOOLTIP_THEME_LIGHT = 1,
    TOOLTIP_THEME_CUSTOM = 2
};

enum TooltipTriggerMode {
    TOOLTIP_TRIGGER_HOVER = 0,
    TOOLTIP_TRIGGER_CLICK = 1
};

enum GroupBoxVisualStyle {
    GROUPBOX_STYLE_OUTLINE = 0,
    GROUPBOX_STYLE_CARD = 1,
    GROUPBOX_STYLE_PLAIN = 2,
    GROUPBOX_STYLE_HEADER_BAR = 3
};

enum TabHeaderVisualStyle {
    TAB_HEADER_STYLE_LINE = 0,
    TAB_HEADER_STYLE_CARD = 1,
    TAB_HEADER_STYLE_CARD_PLAIN = 2,
    TAB_HEADER_STYLE_SEGMENTED = 3
};

enum DataGridHeaderStyle {
    DGHEADER_STYLE_PLAIN = 0,
    DGHEADER_STYLE_FILLED = 1,
    DGHEADER_STYLE_DARK = 2,
    DGHEADER_STYLE_BORDERED = 3
};

// 瀛椾綋鏍峰紡缁撴瀯
struct FontStyle {
    std::wstring font_name;     // 瀛椾綋鍚嶇О
    int font_size;              // 瀛椾綋澶у皬
    bool bold;                  // 绮椾綋
    bool italic;                // 鏂滀綋
    bool underline;             // 涓嬪垝绾?
};

// 缂栬緫妗嗙姸鎬?
struct EditBoxState {
    HWND hwnd;                  // 缂栬緫妗嗗彞鏌?
    HWND parent;                // 父窗口句柄
    int id;                     // 控件ID
    UINT32 fg_color;            // 鍓嶆櫙鑹?(ARGB)
    UINT32 bg_color;            // 鑳屾櫙鑹?(ARGB)
    FontStyle font;             // 瀛椾綋鏍峰紡
    TextAlignment alignment;    // 鏂囧瓧瀵归綈
    bool multiline;             // 澶氳妯″紡
    bool readonly;              // 鍙妯″紡
    bool password;              // 瀵嗙爜妗?
    bool has_border;            // 鏄惁鏈夎竟妗?
    bool vertical_center;       // 鏂囨湰鍨傜洿灞呬腑锛堜粎鍗曡鏈夋晥锛?
    HBRUSH bg_brush;            // 鑳屾櫙鐢诲埛锛堥伩鍏嶆瘡娆″垱寤猴級
    EditBoxKeyCallback key_callback;  // 鎸夐敭鎸変笅/鏉惧紑鍥炶皟锛屽彲涓?NULL
    EventCallbacks events;      // 閫氱敤浜嬩欢鍥炶皟
};

// D2D鑷畾涔夌粯鍒剁紪杈戞鐘舵€侊紙鏀寔褰╄壊emoji锛?
struct D2DEditBoxState {
    HWND hwnd;                  // 鎺т欢鍙ユ焺
    HWND parent;                // 鐖剁獥鍙ｅ彞鏌?
    int id;                     // 鎺т欢ID
    int x, y, width, height;    // 浣嶇疆鍜屽昂瀵?
    std::wstring text;          // 鏂囨湰鍐呭
    int cursor_pos;             // 鍏夋爣浣嶇疆锛堝瓧绗︾储寮曪級
    int selection_start;        // 閫夋嫨璧峰浣嶇疆锛?1琛ㄧず鏃犻€夋嫨锛?
    int selection_end;          // 閫夋嫨缁撴潫浣嶇疆
    bool has_focus;             // 鏄惁鏈夌劍鐐?
    bool cursor_visible;        // 鍏夋爣鏄惁鍙锛堥棯鐑侊級
    UINT_PTR cursor_timer;      // 鍏夋爣闂儊瀹氭椂鍣?
    int scroll_offset_x;        // 姘村钩婊氬姩鍋忕Щ锛堝崟琛岋級
    int scroll_offset_y;        // 鍨傜洿婊氬姩鍋忕Щ锛堝琛岋級
    bool is_composing;          // 鏄惁姝ｅ湪杈撳叆娉曠粍鍚?
    std::wstring composition_text; // 杈撳叆娉曠粍鍚堟枃鏈?
    UINT32 fg_color;            // 鍓嶆櫙鑹?
    UINT32 bg_color;            // 鑳屾櫙鑹?
    UINT32 selection_color;     // 閫夋嫨鑳屾櫙鑹?
    UINT32 border_color;        // 杈规棰滆壊
    FontStyle font;             // 瀛椾綋鏍峰紡
    TextAlignment alignment;    // 鏂囧瓧瀵归綈
    bool multiline;             // 澶氳妯″紡
    bool readonly;              // 鍙妯″紡
    bool password;              // 瀵嗙爜妗?
    bool has_border;            // 鏄惁鏈夎竟妗?
    bool vertical_center;       // 鏂囨湰鍨傜洿灞呬腑锛堜粎鍗曡鏈夋晥锛?
    bool enabled;               // 鍚敤鐘舵€?
    bool scrollbar_dragging;    // 鍨傜洿婊氬姩鏉℃嫋鍔ㄤ腑
    int scrollbar_drag_offset;  // 婊氬姩鏉″唴閮ㄦ嫋鍔ㄥ亸绉?
    bool selecting_with_mouse;  // 榧犳爣姝ｅ湪鎷栨嫿閫夋嫨鏂囨湰
    ID2D1HwndRenderTarget* render_target; // D2D娓叉煋鐩爣
    IDWriteFactory* dwrite_factory;       // DirectWrite宸ュ巶
    EditBoxKeyCallback key_callback;      // 鎸夐敭鍥炶皟
    EventCallbacks events;                // 閫氱敤浜嬩欢鍥炶皟
};

// 鏍囩鐘舵€?
struct LabelState {
    HWND hwnd;                  // 鏍囩鍙ユ焺
    HWND parent;                // 鐖剁獥鍙ｅ彞鏌?
    int id;                     // 鎺т欢ID
    int x, y, width, height;    // 浣嶇疆鍜屽昂瀵?
    std::wstring text;          // 鏂囨湰鍐呭
    UINT32 fg_color;            // 鍓嶆櫙鑹?(ARGB)
    UINT32 bg_color;            // 鑳屾櫙鑹?(ARGB)
    FontStyle font;             // 瀛椾綋鏍峰紡
    TextAlignment alignment;    // 鏂囧瓧瀵归綈
    HBRUSH bg_brush;            // 鑳屾櫙鐢诲埛锛堥伩鍏嶆瘡娆″垱寤猴級
    bool word_wrap;             // 鏄惁鎹㈣鏄剧ず
    bool parent_drawn = false;  // 鏄惁鏀逛负鐖剁獥鍙ｇ粺涓€缁樺埗
    bool visible = true;        // 鐖剁粯鍒舵ā寮忎笅鐨勫彲瑙佺姸鎬?
    EventCallbacks events;      // 閫氱敤浜嬩欢鍥炶皟
};

// 澶嶉€夋鐘舵€?
struct CheckBoxState {
    HWND hwnd;                  // 鎺т欢鍙ユ焺
    HWND parent;                // 鐖剁獥鍙ｅ彞鏌?
    int id;                     // 鎺т欢ID
    int x, y, width, height;    // 浣嶇疆鍜屽昂瀵?
    std::wstring text;          // 鏄剧ず鏂囨湰
    bool checked;               // 閫変腑鐘舵€?
    bool enabled;               // 鍚敤鐘舵€?
    bool hovered;               // 鎮仠鐘舵€?
    bool pressed;               // 鎸変笅鐘舵€?
    UINT32 fg_color;            // 鍓嶆櫙鑹?
    UINT32 bg_color;            // 鑳屾櫙鑹?
    UINT32 check_color;         // 鍕鹃€夋爣璁伴鑹?
    FontStyle font;             // 瀛椾綋鏍峰紡
    CheckBoxCallback callback;  // 鍥炶皟鍑芥暟
    EventCallbacks events;      // 閫氱敤浜嬩欢鍥炶皟
};

// 杩涘害鏉＄姸鎬?
struct ProgressBarState {
    HWND hwnd;                  // 鎺т欢鍙ユ焺
    HWND parent;                // 鐖剁獥鍙ｅ彞鏌?
    int id;                     // 鎺т欢ID
    int x, y, width, height;    // 浣嶇疆鍜屽昂瀵?
    int current_value;          // 褰撳墠鍊?(0-100)
    int target_value;           // 鐩爣鍊?(0-100)
    float animation_value;      // 鍔ㄧ敾涓棿鍊?(鐢ㄤ簬骞虫粦杩囨浮)
    bool indeterminate;         // 涓嶇‘瀹氭ā寮?
    float indeterminate_pos;    // 涓嶇‘瀹氭ā寮忓姩鐢讳綅缃?(0.0-1.0)
    bool enabled;               // 鍚敤鐘舵€?
    UINT32 fg_color;            // 鍓嶆櫙鑹?(杩涘害鏉￠鑹?
    UINT32 bg_color;            // 鑳屾櫙鑹?
    UINT32 border_color;        // 杈规棰滆壊
    UINT32 text_color;          // 鏂囨湰棰滆壊
    bool show_text;             // 鏄惁鏄剧ず鐧惧垎姣旀枃鏈?
    FontStyle font;             // 瀛椾綋鏍峰紡
    ProgressBarCallback callback; // 鍥炶皟鍑芥暟
    UINT_PTR timer_id;          // 鍔ㄧ敾瀹氭椂鍣↖D
    EventCallbacks events;      // 閫氱敤浜嬩欢鍥炶皟
};

// 鍥剧墖妗嗙姸鎬?
struct PictureBoxState {
    HWND hwnd;                  // 鎺т欢鍙ユ焺
    HWND parent;                // 鐖剁獥鍙ｅ彞鏌?
    int id;                     // 鎺т欢ID
    int x, y, width, height;    // 浣嶇疆鍜屽昂瀵?
    ID2D1Bitmap* bitmap;        // D2D1浣嶅浘
    IWICBitmapSource* wic_source; // WIC浣嶅浘婧?
    ImageScaleMode scale_mode;  // 缂╂斁妯″紡
    float opacity;              // 閫忔槑搴?(0.0 - 1.0)
    UINT32 bg_color;            // 鑳屾櫙鑹?
    bool enabled;               // 鍚敤鐘舵€?
    PictureBoxCallback callback; // 鍥炶皟鍑芥暟
    EventCallbacks events;      // 閫氱敤浜嬩欢鍥炶皟
};

// 鍗曢€夋寜閽姸鎬?
struct RadioButtonState {
    HWND hwnd;                  // 鎺т欢鍙ユ焺
    HWND parent;                // 鐖剁獥鍙ｅ彞鏌?
    int id;                     // 鎺т欢ID
    int group_id;               // 鍒嗙粍ID锛屽悓缁勪簰鏂?
    int x, y, width, height;    // 浣嶇疆鍜屽昂瀵?
    std::wstring text;          // 鏄剧ず鏂囨湰
    bool checked;               // 閫変腑鐘舵€?
    bool enabled;               // 鍚敤鐘舵€?
    bool hovered;               // 鎮仠鐘舵€?
    bool pressed;               // 鎸変笅鐘舵€?
    UINT32 fg_color;            // 鍓嶆櫙鑹?
    UINT32 bg_color;            // 鑳屾櫙鑹?
    UINT32 dot_color;           // 鍦嗙偣棰滆壊
    FontStyle font;             // 瀛椾綋鏍峰紡
    RadioButtonCallback callback; // 鍥炶皟鍑芥暟
    EventCallbacks events;      // 閫氱敤浜嬩欢鍥炶皟
};

struct SliderState {
    HWND hwnd;
    HWND parent;
    int id;
    int x, y, width, height;
    int min_value;
    int max_value;
    int value;
    int step;
    bool dragging;
    bool enabled;
    bool hovered;
    bool show_stops;
    UINT32 bg_color;
    UINT32 active_color;
    UINT32 button_color;
    SliderCallback callback;
    EventCallbacks events;
};

struct SwitchState {
    HWND hwnd;
    HWND parent;
    int id;
    int x, y, width, height;
    bool checked;
    bool enabled;
    bool hovered;
    bool pressed;
    UINT32 active_color;
    UINT32 inactive_color;
    UINT32 active_text_color;
    UINT32 inactive_text_color;
    std::wstring active_text;
    std::wstring inactive_text;
    FontStyle font;
    SwitchCallback callback;
    EventCallbacks events;
};

struct TooltipState {
    HWND hwnd;
    HWND owner;
    HWND target;
    HWND bound_target;
    std::wstring text;
    int placement;
    UINT32 bg_color;
    UINT32 fg_color;
    UINT32 border_color;
    std::wstring font_name;
    float font_size;
    int theme_mode;
    int trigger_mode;
    int max_width;
    int padding_x;
    int padding_y;
    bool visible;
};

struct NotificationState {
    HWND hwnd;
    HWND owner;
    std::wstring title;
    std::wstring message;
    int type;
    int position;
    int duration_ms;
    UINT_PTR timer_id;
    bool visible;
    bool hover_close;
    NotificationCallback callback;
};

// 鍒楄〃妗嗛」鐩?
struct ListBoxItem {
    std::wstring text;          // 椤圭洰鏂囨湰
    int id;                     // 椤圭洰ID
    void* user_data;            // 鐢ㄦ埛鑷畾涔夋暟鎹?
};

// 鍒楄〃妗嗙姸鎬?
struct ListBoxState {
    HWND hwnd;                  // 鎺т欢鍙ユ焺
    HWND parent;                // 鐖剁獥鍙ｅ彞鏌?
    int id;                     // 鎺т欢ID
    int x, y, width, height;    // 浣嶇疆鍜屽昂瀵?
    std::vector<ListBoxItem> items; // 鎵€鏈夐」鐩?
    int selected_index;         // 褰撳墠閫変腑椤?(-1琛ㄧず鏃犻€変腑)
    int hovered_index;          // 鎮仠椤?(-1琛ㄧず鏃犳偓鍋?
    int scroll_offset;          // 婊氬姩鍋忕Щ閲忥紙鍍忕礌锛?
    int item_height;            // 椤圭洰楂樺害
    bool multi_select;          // 澶氶€夋ā寮?
    std::vector<int> selected_indices; // 澶氶€夋椂鐨勯€変腑椤?
    bool scrollbar_dragging;    // 婊氬姩鏉℃嫋鍔ㄤ腑
    int scrollbar_drag_offset;  // 婊氬潡鍐呴儴鐨勬嫋鍔ㄥ亸绉?
    bool enabled;               // 鍚敤鐘舵€?
    UINT32 fg_color;            // 鍓嶆櫙鑹?
    UINT32 bg_color;            // 鑳屾櫙鑹?
    UINT32 select_color;        // 閫変腑鑳屾櫙鑹?
    UINT32 hover_color;         // 鎮仠鑳屾櫙鑹?
    FontStyle font;             // 瀛椾綋鏍峰紡
    ListBoxCallback callback;   // 鍥炶皟鍑芥暟
    EventCallbacks events;      // 閫氱敤浜嬩欢鍥炶皟
};

// 缁勫悎妗嗙姸鎬?
struct ComboBoxState {
    HWND hwnd;                  // 涓绘帶浠跺彞鏌?
    HWND parent;                // 鐖剁獥鍙ｅ彞鏌?
    HWND edit_hwnd;             // 缂栬緫妗嗗彞鏌?
    HWND dropdown_hwnd;         // 涓嬫媺鍒楄〃绐楀彛鍙ユ焺
    int id;                     // 鎺т欢ID
    int x, y, width, height;    // 浣嶇疆鍜屽昂瀵?
    std::vector<std::wstring> items; // 涓嬫媺椤圭洰
    int selected_index;         // 褰撳墠閫変腑椤?(-1琛ㄧず鏃犻€変腑)
    int hovered_index;          // 鎮仠椤?(-1琛ㄧず鏃犳偓鍋?
    int scroll_offset;          // 婊氬姩鍋忕Щ閲忥紙鍍忕礌锛?
    bool dropdown_visible;      // 涓嬫媺鍒楄〃鏄惁鍙
    bool readonly;              // 鍙妯″紡
    bool enabled;               // 鍚敤鐘舵€?
    bool button_hovered;        // 涓嬫媺鎸夐挳鎮仠鐘舵€?
    bool button_pressed;        // 涓嬫媺鎸夐挳鎸変笅鐘舵€?
    int item_height;            // 琛ㄩ」楂樺害
    UINT32 fg_color;            // 鍓嶆櫙鑹?
    UINT32 bg_color;            // 鑳屾櫙鑹?
    UINT32 select_color;        // 閫変腑鑳屾櫙鑹?
    UINT32 hover_color;         // 鎮仠鑳屾櫙鑹?
    FontStyle font;             // 瀛椾綋鏍峰紡
    ComboBoxCallback callback;  // 鍥炶皟鍑芥暟
    EventCallbacks events;      // 閫氱敤浜嬩欢鍥炶皟
};


// D2D缁勫悎妗嗙姸鎬侊紙瀹屽叏鑷畾涔夛紝鏀寔褰╄壊emoji锛?
struct D2DComboBoxState {
    HWND hwnd;                      // 涓绘帶浠跺彞鏌?
    HWND parent;                    // 鐖剁獥鍙ｅ彞鏌?
    HWND edit_hwnd;                 // D2D缂栬緫妗嗗彞鏌?
    HWND dropdown_hwnd;             // 涓嬫媺鍒楄〃绐楀彛鍙ユ焺
    int id;                         // 鎺т欢ID
    int x, y, width, height;        // 浣嶇疆鍜屽昂瀵?
    
    std::vector<std::wstring> items; // 涓嬫媺椤圭洰
    int selected_index;             // 褰撳墠閫変腑椤?(-1琛ㄧず鏃犻€変腑)
    int hovered_index;              // 鎮仠椤?(-1琛ㄧず鏃犳偓鍋?
    int scroll_offset;              // 婊氬姩鍋忕Щ閲忥紙鍍忕礌锛?
    
    bool dropdown_visible;          // 涓嬫媺鍒楄〃鏄惁鍙
    bool readonly;                  // 鍙妯″紡
    bool enabled;                   // 鍚敤鐘舵€?
    bool button_hovered;            // 涓嬫媺鎸夐挳鎮仠鐘舵€?
    bool button_pressed;            // 涓嬫媺鎸夐挳鎸変笅鐘舵€?
    
    int item_height;                // 琛ㄩ」楂樺害
    int button_width;               // 涓嬫媺鎸夐挳瀹藉害锛堥粯璁?0锛?
    int max_dropdown_items;         // 涓嬫媺鍒楄〃鏈€澶氭樉绀洪」鏁帮紙榛樿10锛?
    
    UINT32 fg_color;                // 鍓嶆櫙鑹?
    UINT32 bg_color;                // 鑳屾櫙鑹?
    UINT32 select_color;            // 閫変腑鑳屾櫙鑹?
    UINT32 hover_color;             // 鎮仠鑳屾櫙鑹?
    UINT32 border_color;            // 杈规棰滆壊
    UINT32 button_color;            // 鎸夐挳棰滆壊
    
    FontStyle font;                 // 瀛椾綋鏍峰紡
    ComboBoxCallback callback;      // 鍥炶皟鍑芥暟
    EventCallbacks events;          // 閫氱敤浜嬩欢鍥炶皟

    ID2D1HwndRenderTarget* render_target; // D2D娓叉煋鐩爣锛堢敤浜庝笅鎷夊垪琛級
    IDWriteFactory* dwrite_factory;       // DirectWrite宸ュ巶
};

// D2D 鏃ユ湡鏃堕棿閫夋嫨鍣細鏄剧ず绮惧害锛堜笌 Element DateTimePicker 绮掑害瀵瑰簲锛?
enum DateTimePickerPrecision {
    DTP_PRECISION_YEAR = 0,      // 浠呭勾
    DTP_PRECISION_YMD = 1,       // 骞存湀鏃?
    DTP_PRECISION_YMDH = 2,        // 骞存湀鏃ユ椂
    DTP_PRECISION_YMDHM = 3,     // 骞存湀鏃ユ椂鍒?
    DTP_PRECISION_YMDHMS = 4     // 骞存湀鏃ユ椂鍒嗙
};

// 鐑敭鎺т欢鐘舵€?
struct HotKeyState {
    HWND hwnd;                  // 控件句柄
    HWND parent;                // 父窗口句柄
    HWND input_hwnd;            // 隐藏输入子控件
    int id;                     // 控件ID
    int x, y, width, height;    // 浣嶇疆鍜屽昂瀵?
    int vk_code;                // 铏氭嫙閿爜
    int modifiers;              // 淇グ閿?(Ctrl=1, Shift=2, Alt=4)
    std::wstring display_text;  // 鏄剧ず鏂囨湰
    bool capturing;             // 鏄惁姝ｅ湪鎹曡幏
    bool has_focus;             // 鏄惁鏈夌劍鐐?
    bool enabled;               // 鍚敤鐘舵€?
    UINT32 fg_color;            // 鍓嶆櫙鑹?
    UINT32 bg_color;            // 鑳屾櫙鑹?
    UINT32 border_color;        // 杈规棰滆壊
    FontStyle font;             // 瀛椾綋鏍峰紡
    HotKeyCallback callback;    // 鍥炶皟鍑芥暟
    EventCallbacks events;      // 閫氱敤浜嬩欢鍥炶皟
};

// 鍒嗙粍妗嗗洖璋冨嚱鏁扮被鍨?(stdcall 璋冪敤绾﹀畾)
typedef void (__stdcall *GroupBoxCallback)(HWND hGroupBox);

// ========== DataGridView 鍥炶皟鍑芥暟绫诲瀷 ==========

// 鍗曞厓鏍肩偣鍑诲洖璋?(hGrid, row, col)
typedef void (__stdcall *DataGridCellClickCallback)(HWND hGrid, int row, int col);

// 鍗曞厓鏍煎弻鍑诲洖璋?(hGrid, row, col)
typedef void (__stdcall *DataGridCellDoubleClickCallback)(HWND hGrid, int row, int col);

// 鍗曞厓鏍煎€兼敼鍙樺洖璋?(hGrid, row, col)
typedef void (__stdcall *DataGridCellValueChangedCallback)(HWND hGrid, int row, int col);

// 鍒楀ご鐐瑰嚮鍥炶皟 (hGrid, col)
typedef void (__stdcall *DataGridColumnHeaderClickCallback)(HWND hGrid, int col);

// 閫夋嫨鏀瑰彉鍥炶皟 (hGrid, row, col)
typedef void (__stdcall *DataGridSelectionChangedCallback)(HWND hGrid, int row, int col);

// 铏氭嫙妯″紡鏁版嵁璇锋眰鍥炶皟 (hGrid, row, col, buffer, buffer_size) -> 杩斿洖鍐欏叆鐨勫瓧鑺傛暟
typedef int (__stdcall *DataGridVirtualDataCallback)(HWND hGrid, int row, int col, unsigned char* buffer, int buffer_size);

// ========== DataGridView 鏋氫妇鍜岀粨鏋?==========

// 鍒楃被鍨?
enum DataGridColumnTypeFixed {
    DGCOL_TEXT = 0,
    DGCOL_CHECKBOX = 1,
    DGCOL_BUTTON = 2,
    DGCOL_LINK = 3,
    DGCOL_IMAGE = 4,
    DGCOL_COMBOBOX = 5,
    DGCOL_TAG = 6,
    DGCOL_PROGRESS = 7
};

#if 0
enum DataGridColumnType {
    DGCOL_TEXT = 0,         // 鏂囨湰鍒?    DGCOL_CHECKBOX = 1,     // 澶嶉€夋鍒?    DGCOL_BUTTON = 2,       // 鎸夐挳鍒?    DGCOL_LINK = 3,         // 閾炬帴鍒?    DGCOL_IMAGE = 4,        // 鍥剧墖鍒?    DGCOL_COMBOBOX = 5,     // 缁勫悎妗嗗垪
    DGCOL_TAG = 6,          // 标签列
    DGCOL_PROGRESS = 7      // 进度条列
};

// 鎺掑簭鏂瑰悜
#endif

typedef DataGridColumnTypeFixed DataGridColumnType;

enum DataGridSortOrder {
    DGSORT_NONE = 0,        // 鏃犳帓搴?
    DGSORT_ASC = 1,         // 鍗囧簭
    DGSORT_DESC = 2         // 闄嶅簭
};

// 閫夋嫨妯″紡
enum DataGridSelectionMode {
    DGSEL_CELL = 0,         // 鍗曞厓鏍奸€夋嫨
    DGSEL_ROW = 1           // 鏁磋閫夋嫨
};

// 鍗曞厓鏍兼牱寮?
struct DataGridCellStyle {
    UINT32 fg_color;        // 鍓嶆櫙鑹?(0=浣跨敤榛樿)
    UINT32 bg_color;        // 鑳屾櫙鑹?(0=浣跨敤榛樿)
    bool bold;              // 绮椾綋
    bool italic;            // 鏂滀綋
};

// 鍒楀畾涔?
struct DataGridColumn {
    std::wstring header_text;   // 鍒楀ご鏂囨湰
    int width;                  // 鍒楀锛堝儚绱狅級
    int min_width;              // 鏈€灏忓垪瀹?
    DataGridColumnType type;    // 鍒楃被鍨?
    bool resizable;             // 鏄惁鍙皟鏁村搴?
    bool sortable;              // 鏄惁鍙帓搴?
    DataGridSortOrder sort_order; // 褰撳墠鎺掑簭鏂瑰悜
    DWRITE_TEXT_ALIGNMENT header_alignment;  // 鍒楀ご瀵归綈鏂瑰紡
    DWRITE_TEXT_ALIGNMENT cell_alignment;    // 鍗曞厓鏍煎榻愭柟寮?
    
    // 鏋勯€犲嚱鏁帮紝璁剧疆榛樿鍊?
    DataGridColumn() : 
        width(100), min_width(30), type(DGCOL_TEXT), 
        resizable(true), sortable(true), sort_order(DGSORT_NONE),
        header_alignment(DWRITE_TEXT_ALIGNMENT_LEADING),
        cell_alignment(DWRITE_TEXT_ALIGNMENT_LEADING) {}
};

// 鍗曞厓鏍兼暟鎹?
struct DataGridCell {
    std::wstring text;          // 鏂囨湰鍐呭
    bool checked;               // 澶嶉€夋鐘舵€?(DGCOL_CHECKBOX)
    int progress;               // 杩涘害鍊?0-100锛圗GCOL_PROGRESS锛?
    DataGridCellStyle style;    // 鍗曞厓鏍兼牱寮?
    std::vector<unsigned char> image_data; // 鍥剧墖鍘熷缂栫爜鏁版嵁锛堢敤浜庣湡 bitmap 鍥剧墖鍒楋級

    DataGridCell() : checked(false), progress(0), style{} {}
};

// 琛屾暟鎹?
struct DataGridRow {
    std::vector<DataGridCell> cells; // 鍗曞厓鏍煎垪琛?
    int height;                      // 琛岄珮锛?=浣跨敤榛樿锛?
};

// DataGridView 鐘舵€?
struct DataGridViewState {
    HWND hwnd;                  // 鎺т欢鍙ユ焺
    HWND parent;                // 鐖剁獥鍙ｅ彞鏌?
    int id;                     // 鎺т欢ID
    int x, y, width, height;   // 浣嶇疆鍜屽昂瀵?

    // 鍒楀拰琛屾暟鎹?
    std::vector<DataGridColumn> columns;
    std::vector<DataGridRow> rows;

    // 铏氭嫙妯″紡
    bool virtual_mode;          // 鏄惁铏氭嫙妯″紡
    int virtual_row_count;      // 铏氭嫙妯″紡鎬昏鏁?

    // 閫夋嫨鐘舵€?
    int selected_row;           // 閫変腑琛?(-1=鏃?
    int selected_col;           // 閫変腑鍒?(-1=鏃?
    DataGridSelectionMode selection_mode; // 閫夋嫨妯″紡

    // 缂栬緫鐘舵€?
    bool editing;               // 鏄惁姝ｅ湪缂栬緫
    int edit_row;               // 缂栬緫琛?
    int edit_col;               // 缂栬緫鍒?
    HWND edit_hwnd;             // 缂栬緫妗嗗彞鏌?

    // 鎮仠鐘舵€?
    int hovered_row;            // 鎮仠琛?
    int hovered_col;            // 鎮仠鍒?

    // 婊氬姩
    int scroll_x;               // 姘村钩婊氬姩鍋忕Щ
    int scroll_y;               // 鍨傜洿婊氬姩鍋忕Щ

    // 鍒楀璋冩暣
    bool resizing_col;          // 鏄惁姝ｅ湪璋冩暣鍒楀
    int resize_col_index;       // 姝ｅ湪璋冩暣鐨勫垪绱㈠紩
    int resize_start_x;         // 璋冩暣璧峰X鍧愭爣
    int resize_start_width;     // 璋冩暣璧峰鍒楀

    // 婊氬姩鏉℃嫋鎷?
    bool scrollbar_v_dragging;  // 鏄惁姝ｅ湪鎷栨嫿绾靛悜婊氬姩鏉?
    bool scrollbar_h_dragging;  // 鏄惁姝ｅ湪鎷栨嫿妯悜婊氬姩鏉?
    float scrollbar_drag_offset; // 鎷栨嫿璧峰鍋忕Щ锛堥紶鏍囩浉瀵箃humb椤堕儴锛?

    // 鎺掑簭
    int sort_col;               // 鎺掑簭鍒?(-1=鏃?
    DataGridSortOrder sort_order; // 鎺掑簭鏂瑰悜

    // 鍐荤粨
    bool freeze_header;         // 鍐荤粨棣栬锛堝垪澶达級
    int freeze_col_count;       // 鍐荤粨鍓嶯鍒楋紙鍐呴儴涓荤姸鎬侊級
    int freeze_row_count;       // 鍐荤粨鍓嶯琛岋紙鍐呴儴棰勭暀锛?
    bool freeze_first_col;      // 鍐荤粨棣栧垪

    // 澶栬
    int header_height;          // 鍒楀ご楂樺害
    int default_row_height;     // 榛樿琛岄珮
    bool zebra_stripe;          // 闅旇鍙樿壊
    bool show_grid_lines;       // 鏄剧ず缃戞牸绾?
    bool enabled;               // 鍚敤鐘舵€?
    UINT32 fg_color;            // 鍓嶆櫙鑹?
    UINT32 bg_color;            // 鑳屾櫙鑹?
    UINT32 header_bg_color;     // 鍒楀ご鑳屾櫙鑹?
    UINT32 header_fg_color;     // 鍒楀ご鍓嶆櫙鑹?
    UINT32 grid_line_color;     // 缃戞牸绾块鑹?
    UINT32 select_color;        // 閫変腑鑳屾櫙鑹?
    UINT32 hover_color;         // 鎮仠鑳屾櫙鑹?
    UINT32 zebra_color;         // 闅旇鍙樿壊鑳屾櫙鑹?
    FontStyle font;             // 瀛椾綋鏍峰紡

    // 鍥炶皟
    DataGridCellClickCallback cell_click_cb;
    DataGridCellDoubleClickCallback cell_dblclick_cb;
    DataGridCellValueChangedCallback cell_value_changed_cb;
    DataGridColumnHeaderClickCallback col_header_click_cb;
    DataGridSelectionChangedCallback selection_changed_cb;
    DataGridVirtualDataCallback virtual_data_cb;
    EventCallbacks events;      // 閫氱敤浜嬩欢鍥炶皟
};

// 鍒嗙粍妗嗙姸鎬?
struct GroupBoxState {
    HWND hwnd;                  // 鎺т欢鍙ユ焺
    HWND parent;                // 鐖剁獥鍙ｅ彞鏌?
    int id;                     // 鎺т欢ID
    int x, y, width, height;    // 浣嶇疆鍜屽昂瀵?
    std::wstring title;         // 鏍囬鏂囨湰
    std::vector<HWND> children; // 瀛愭帶浠跺垪琛紙鍗曢€夋寜閽€佸閫夋绛塇WND鎺т欢锛?
    std::vector<int> button_ids; // 鎸夐挳ID鍒楄〃锛堜富绐楀彛涓婄粯鍒剁殑鎸夐挳锛?
    bool enabled;               // 鍚敤鐘舵€?
    bool visible;               // 鍙鐘舵€?
    UINT32 border_color;        // 杈规棰滆壊
    UINT32 title_color;         // 鏍囬棰滆壊
    UINT32 bg_color;            // 鑳屾櫙鑹?
    FontStyle font;             // 瀛椾綋鏍峰紡
    GroupBoxCallback callback;  // 鍥炶皟鍑芥暟
    EventCallbacks events;      // 閫氱敤浜嬩欢鍥炶皟
};

struct PanelState {
    HWND hwnd;
    HWND parent;
    int x, y, width, height;
    UINT32 bg_color;
    EventCallbacks events;
};

// Button structure
struct EmojiButton {
    int id;
    HWND hwnd = nullptr;
    std::wstring emoji;
    std::wstring text;
    int x, y, width, height;
    UINT32 bg_color;
    bool is_hovered;
    bool is_pressed;
    bool enabled = true;  // 鎸夐挳鍚敤鐘舵€?
    bool visible = true;  // 鎸夐挳鍙鐘舵€?
    int visual_type = BUTTON_TYPE_AUTO;
    int visual_style = BUTTON_STYLE_SOLID;
    int visual_size = BUTTON_SIZE_DEFAULT;
    bool round = false;
    bool circle = false;
    bool loading = false;
    int loading_phase = 0;

    bool ContainsPoint(int px, int py) const {
        return px >= x && px <= x + width && py >= y && py <= y + height;
    }
};

// Window state
struct WindowState {
    HWND hwnd;
    ID2D1HwndRenderTarget* render_target;
    IDWriteFactory* dwrite_factory;
    std::vector<EmojiButton> buttons;
    // 鑷畾涔夋爣棰樻爮鏀寔
    std::wstring title;           // 绐楀彛鏍囬锛堢敤浜嶥2D褰╄壊emoji缁樺埗锛?
    UINT32 titlebar_color = 0;    // 鏍囬鏍忚儗鏅壊锛?=璺熼殢涓婚锛?
    UINT32 client_bg_color = 0;   // 瀹㈡埛鍖鸿儗鏅壊锛?=浣跨敤 ThemeColor_Background 绾櫧锛?
    int titlebar_height = 30;     // 鏍囬鏍忛珮搴︼紙鍍忕礌锛?
    bool custom_titlebar = true;  // 鏄惁鍚敤鑷畾涔夋爣棰樻爮
    int hovered_titlebar_button = 0; // 0=鏃?1=鏈€灏忓寲 2=鏈€澶у寲 3=鍏抽棴
    bool titlebar_mouse_tracking = false;

    // ===== 鏍囬鏍忔枃瀛楁牱寮忓瓧娈?=====
    UINT32 titlebar_text_color = 0;                          // 鏍囬鏂囧瓧棰滆壊锛圓RGB锛夛紝0=璺熼殢涓婚
    std::wstring titlebar_font_name = L"Segoe UI Emoji";     // 鏍囬瀛椾綋鍚嶇О
    float titlebar_font_size = 13.0f;                        // 鏍囬瀛楀彿锛堝儚绱狅級
    int titlebar_alignment = 0;                              // 标题对齐方式
    EventCallbacks events;                                   // 通用事件回调
};

// 鑿滃崟椤圭姸鎬?
struct MenuItem {
    int id;                     // 鑿滃崟椤笽D
    std::wstring text;          // 鏄剧ず鏂囨湰
    std::wstring shortcut;      // 蹇嵎閿枃鏈?
    bool enabled = true;        // 鏄惁鍚敤
    bool checked = false;       // 鏄惁鍕鹃€?
    bool separator = false;     // 鏄惁鍒嗛殧绾?
    std::vector<MenuItem> sub_items; // 瀛愯彍鍗曢」
    D2D1_RECT_F bounds = {};    // 鑿滃崟椤硅竟鐣岋紙鐢ㄤ簬鍛戒腑娴嬭瘯锛?
};

// 鑿滃崟鏍忕姸鎬?
struct MenuBarState {
    HWND hwnd;                      // 鎵€灞炵獥鍙ｅ彞鏌?
    std::vector<MenuItem> items;    // 鑿滃崟椤瑰垪琛?
    int x = 0;                      // 绘制起点X
    int y = 0;                      // 绘制起点Y
    int width = 0;                  // 绘制宽度（0=跟随宿主宽度）
    int height = 30;                // 菜单栏高度
    int hovered_index = -1;         // 褰撳墠鎮仠鐨勮彍鍗曠储寮?
    int opened_index = -1;          // 褰撳墠灞曞紑鐨勮彍鍗曠储寮?
    int opened_menu_id = 0;         // 褰撳墠灞曞紑鐨勯《绾ц彍鍗旾D锛堢敤浜庡洖璋冧紶鍙傦級
    bool visible = true;            // 鏄惁鍙
    UINT32 bg_color = 0;            // 鑳屾櫙鑹诧紙0=榛樿锛?
    UINT32 fg_color = 0;            // 鍓嶆櫙鑹诧紙0=榛樿锛?
    FontStyle font;                 // 瀛椾綋鏍峰紡
    MenuItemClickCallback callback; // 菜单项点击回调
    HWND popup_menu_handle = nullptr; // 当前打开的顶部菜单弹层句柄
    SubMenuWindow* submenu = nullptr; // 子菜单窗口
};

// 寮瑰嚭鑿滃崟鐘舵€?
struct PopupMenuState {
    HWND handle_key = nullptr;      // 閫昏緫鑿滃崟鍙ユ焺锛圕reateEmojiPopupMenu 杩斿洖鍊硷級
    HWND hwnd = nullptr;            // 寮瑰嚭鑿滃崟绐楀彛鍙ユ焺
    HWND owner_hwnd;                // 鍏宠仈绐楀彛鍙ユ焺
    ID2D1HwndRenderTarget* render_target = nullptr; // D2D娓叉煋鐩爣
    IDWriteFactory* dwrite_factory = nullptr;       // DirectWrite宸ュ巶
    std::vector<MenuItem> items;    // 鑿滃崟椤瑰垪琛?
    int hovered_index = -1;         // 褰撳墠鎮仠鐨勮彍鍗曠储寮?
    bool visible = false;           // 鏄惁鍙
    int x = 0;                      // 鏄剧ず浣嶇疆X
    int y = 0;                      // 鏄剧ず浣嶇疆Y
    int width = 0;                  // 鑿滃崟瀹藉害
    int height = 0;                 // 鑿滃崟楂樺害
    int item_height = 0;            // 鑿滃崟椤归珮搴?
    UINT32 bg_color = 0;            // 鑳屾櫙鑹诧紙0=榛樿锛?
    UINT32 fg_color = 0;            // 鍓嶆櫙鑹诧紙0=榛樿锛?
    UINT32 hover_color = 0;         // 鎮仠鑳屾櫙鑹?
    FontStyle font;                 // 瀛椾綋鏍峰紡
    MenuItemClickCallback callback; // 菜单项点击回调
    int callback_menu_id = 0;       // 回调中的 menu_id
    bool auto_destroy = false;      // 窗口销毁后是否释放逻辑菜单状态
    HWND owner_menubar = nullptr;   // 顶部菜单宿主
    SubMenuWindow* submenu = nullptr; // 当前展开的二级菜单
    HWND child_popup_handle = nullptr; // 当前展开的二级弹出菜单句柄
    HWND parent_popup_handle = nullptr; // 父级弹出菜单窗口句柄
    int submenu_owner_index = -1;   // 二级菜单对应的父项索引
    DWORD ignore_initial_button_up_until = 0; // 刚弹出时忽略触发弹出的那次鼠标抬起
};

// Message box button type
enum MsgBoxButtonType {
    MSGBOX_OK = 0,
    MSGBOX_OKCANCEL = 1
};

// Message box state
struct MsgBoxState {
    HWND hwnd;
    HWND parent_hwnd;
    ID2D1HwndRenderTarget* render_target;
    IDWriteFactory* dwrite_factory;
    std::wstring title;
    std::wstring message;
    std::wstring icon_emoji;
    MsgBoxButtonType button_type;
    MessageBoxCallback callback;
    EmojiButton ok_button;
    EmojiButton cancel_button;
    bool close_hovered;
    bool result;
};

// Tab 椤典俊鎭粨鏋?
struct TabPageInfo {
    int index;                      // Tab 绱㈠紩
    std::wstring title;             // Tab 鏍囬
    HWND hContentWindow;            // 鍐呭瀹瑰櫒绐楀彛鍙ユ焺
    bool visible;                   // 鏄惁鍙
    bool enabled;                   // 鏄惁鍚敤锛堥粯璁?true锛?
    std::vector<unsigned char> iconData; // 鍥炬爣 PNG 瀛楄妭鏁版嵁锛堥粯璁ょ┖锛?
    UINT32 contentBgColor;          // 鍐呭鍖哄煙鑳屾櫙鑹诧紙榛樿 0xFFFFFFFF 鐧借壊锛?
};

// TabControl 鐘舵€佺鐞?
struct TabControlState {
    HWND hTabControl;               // Tab Control 鍙ユ焺
    HWND hParent;                   // 父窗口句柄
    std::vector<TabPageInfo> pages; // 所有 Tab 页信息
    TAB_CALLBACK callback;          // Tab 切换回调
    int currentIndex;               // 当前选中 Tab 索引

    // ===== 澶栬瀛楁 =====
    int tabWidth;                   // 鏍囩瀹藉害锛堥粯璁?120锛?
    int tabHeight;                  // 鏍囩楂樺害锛堥粯璁?34锛?
    std::wstring fontName;          // 瀛椾綋鍚嶇О锛堥粯璁?L"Segoe UI Emoji"锛?
    float fontSize;                 // 瀛楀彿锛堥粯璁?13.0f锛?
    UINT32 selectedBgColor;         // 閫変腑鑳屾櫙鑹诧紙榛樿 0xFFFFFFFF 鐧借壊锛?
    UINT32 unselectedBgColor;       // 鏈€変腑鑳屾櫙鑹诧紙榛樿 0xFFF5F7FA 娴呯伆锛?
    UINT32 selectedTextColor;       // 閫変腑鏂囧瓧鑹诧紙榛樿 0xFF409EFF 钃濊壊锛?
    UINT32 unselectedTextColor;     // 鏈€変腑鏂囧瓧鑹诧紙榛樿 0xFF606266 娣辩伆锛?
    UINT32 indicatorColor;          // 閫変腑鎸囩ず鏉￠鑹诧紙榛樿 0xFF409EFF锛?
    int paddingH;                   // 姘村钩鍐呰竟璺濓紙榛樿 2锛?
    int paddingV;                   // 鍨傜洿鍐呰竟璺濓紙榛樿 0锛?

    // ===== 浜や簰瀛楁 =====
    bool closable;                  // 鏄惁鏄剧ず鍏抽棴鎸夐挳锛堥粯璁?false锛?
    TAB_CLOSE_CALLBACK closeCallback;       // 鍏抽棴鍥炶皟锛堥粯璁?nullptr锛?
    TAB_RIGHTCLICK_CALLBACK rightClickCallback; // 鍙抽敭鍥炶皟锛堥粯璁?nullptr锛?
    TAB_DBLCLICK_CALLBACK dblClickCallback;     // 鍙屽嚮鍥炶皟锛堥粯璁?nullptr锛?
    TAB_NEW_BUTTON_CALLBACK newButtonCallback;  // 新建标签按钮回调
    EventCallbacks events;          // 通用事件回调
    bool draggable;                 // 鏄惁鍙嫋鎷芥帓搴忥紙榛樿 false锛?

    // ===== 甯冨眬瀛楁 =====
    int tabPosition;                // 鏍囩鏍忎綅缃細0=涓?1=涓?2=宸?3=鍙筹紙榛樿 0锛?
    int tabAlignment;               // 鏍囩瀵归綈锛?=宸?1=灞呬腑 2=鍙筹紙榛樿 0锛?
    bool scrollable;                // 鏄惁鍙粴鍔紙榛樿 false锛?
    int scrollOffset;               // 婊氬姩鍋忕Щ閲忥紙榛樿 0锛?

    // ===== 鎷栨嫿鐘舵€佸瓧娈?=====
    bool isDragging;                // 鏄惁姝ｅ湪鎷栨嫿
    int dragStartIndex;             // 鎷栨嫿璧峰鏍囩椤电储寮曪紙pages 鏁扮粍绱㈠紩锛?
    int dragTargetIndex;            // 鎷栨嫿鐩爣鎻掑叆浣嶇疆绱㈠紩锛坧ages 鏁扮粍绱㈠紩锛?
    POINT dragStartPoint;           // 鎷栨嫿璧峰榧犳爣浣嶇疆

    // ===== 缁樺埗杈呭姪瀛楁 =====
    int hoveredCloseTabIndex;       // 榧犳爣鎮仠鐨勫叧闂寜閽墍鍦ㄦ爣绛鹃〉绱㈠紩锛?1=鏃狅級
    int hoveredTabIndex;            // 鼠标悬停的标签页索引
    bool hoveredNewButton;          // 鼠标悬停在新建标签按钮
    bool layoutBatchInProgress;     // Tab 批处理布局中
};

// ========== 涓婚绯荤粺 (闇€姹?11.1-11.10) ==========

// 涓婚棰滆壊
struct ThemeColors {
    UINT32 primary;             // 涓昏壊 (榛樿 #409EFF)
    UINT32 success;             // 鎴愬姛鑹?(榛樿 #67C23A)
    UINT32 warning;             // 璀﹀憡鑹?(榛樿 #E6A23C)
    UINT32 danger;              // 鍗遍櫓鑹?(榛樿 #F56C6C)
    UINT32 info;                // 淇℃伅鑹?(榛樿 #909399)
    UINT32 text_primary;        // 涓昏鏂囨湰鑹?(榛樿 #303133)
    UINT32 text_regular;        // 甯歌鏂囨湰鑹?(榛樿 #606266)
    UINT32 text_secondary;      // 娆¤鏂囨湰鑹?(榛樿 #909399)
    UINT32 text_placeholder;    // 鍗犱綅鏂囨湰鑹?(榛樿 #C0C4CC)
    UINT32 border_base;         // 鍩虹杈规鑹?(榛樿 #DCDFE6)
    UINT32 border_light;        // 娴呰壊杈规鑹?(榛樿 #E4E7ED)
    UINT32 border_lighter;      // 鏇存祬杈规鑹?(榛樿 #EBEEF5)
    UINT32 border_extra_light;  // 鏋佹祬杈规鑹?(榛樿 #F2F6FC)
    UINT32 background;          // 鑳屾櫙鑹?(榛樿 #FFFFFF)
    UINT32 background_light;    // 娴呰壊鑳屾櫙 (榛樿 #F5F7FA)
};

// 涓婚瀛椾綋
struct ThemeFonts {
    std::wstring title_font;    // 鏍囬瀛椾綋 (榛樿 "Microsoft YaHei UI")
    std::wstring body_font;     // 姝ｆ枃瀛椾綋 (榛樿 "Microsoft YaHei UI")
    std::wstring mono_font;     // 绛夊瀛椾綋 (榛樿 "Consolas")
    int title_size;             // 鏍囬瀛楀彿 (榛樿 16)
    int body_size;              // 姝ｆ枃瀛楀彿 (榛樿 14)
    int small_size;             // 灏忓彿瀛楀彿 (榛樿 12)
};

// 涓婚灏哄
struct ThemeSizes {
    float border_radius;        // 鍦嗚鍗婂緞 (榛樿 4.0f)
    float border_width;         // 杈规瀹藉害 (榛樿 1.0f)
    int control_height;         // 鎺т欢楂樺害 (榛樿 32)
    int spacing_small;          // 灏忛棿璺?(榛樿 8)
    int spacing_medium;         // 涓棿璺?(榛樿 16)
    int spacing_large;          // 澶ч棿璺?(榛樿 24)
};

// 涓婚缁撴瀯
struct Theme {
    std::wstring name;          // 涓婚鍚嶇О
    bool dark_mode;             // 鏄惁鏆楄壊妯″紡
    ThemeColors colors;         // 棰滆壊
    ThemeFonts fonts;           // 瀛椾綋
    ThemeSizes sizes;           // 灏哄
};

// 涓婚鍥炶皟鍑芥暟绫诲瀷 (stdcall 璋冪敤绾﹀畾)
typedef void (__stdcall *ThemeChangedCallback)(const char* theme_name);

extern Theme* g_current_theme;
extern Theme g_light_theme;
extern Theme g_dark_theme;
extern ThemeChangedCallback g_theme_changed_callback;

// 涓婚杈呭姪鍑芥暟锛圕++鍐呴儴浣跨敤锛屼笉瀵煎嚭锛?
// 娉ㄦ剰锛氳繖浜涘嚱鏁颁笉鍦?extern "C" 鍧椾腑澹版槑锛屽湪 cpp 涓洿鎺ュ畾涔?

// ========== 甯冨眬绠＄悊鍣?==========

enum LayoutType {
    LAYOUT_NONE = 0,
    LAYOUT_FLOW_HORIZONTAL = 1,     // 姘村钩娴佸紡甯冨眬
    LAYOUT_FLOW_VERTICAL = 2,       // 鍨傜洿娴佸紡甯冨眬
    LAYOUT_GRID = 3,                // 缃戞牸甯冨眬
    LAYOUT_DOCK = 4                 // 鍋滈潬甯冨眬
};

enum DockPosition {
    DOCK_NONE = 0,
    DOCK_TOP = 1,
    DOCK_BOTTOM = 2,
    DOCK_LEFT = 3,
    DOCK_RIGHT = 4,
    DOCK_FILL = 5
};

struct LayoutProperties {
    int margin_left = 0;
    int margin_top = 0;
    int margin_right = 0;
    int margin_bottom = 0;
    DockPosition dock = DOCK_NONE;
    bool stretch_horizontal = false;
    bool stretch_vertical = false;
    int order = 0;  // 鎺т欢鍦ㄥ竷灞€涓殑椤哄簭
};

// 甯冨眬鎺т欢椤癸細鍙互鏄疕WND鎺т欢鎴朎moji鎸夐挳ID
struct LayoutItem {
    HWND hwnd = nullptr;       // HWND鎺т欢鍙ユ焺锛堝鏋滄槸HWND鎺т欢锛?
    int button_id = 0;         // Emoji鎸夐挳ID锛堝鏋滄槸Emoji鎸夐挳锛?
    bool is_button = false;    // true=Emoji鎸夐挳, false=HWND鎺т欢

    bool operator==(const LayoutItem& other) const {
        if (is_button != other.is_button) return false;
        return is_button ? (button_id == other.button_id) : (hwnd == other.hwnd);
    }
};

struct LayoutManager {
    HWND parent_hwnd = nullptr;
    LayoutType type = LAYOUT_NONE;
    int rows = 0;
    int columns = 0;
    int spacing = 0;
    int padding_left = 0;
    int padding_top = 0;
    int padding_right = 0;
    int padding_bottom = 0;
    std::map<HWND, LayoutProperties> control_props;
    std::vector<HWND> control_order;  // 鎸夋坊鍔犻『搴忔帓鍒楃殑HWND鎺т欢鍒楄〃
    // Emoji鎸夐挳甯冨眬鏀寔
    std::vector<LayoutItem> item_order;  // 缁熶竴椤哄簭鍒楄〃锛圚WND + Emoji鎸夐挳锛?
    std::map<int, LayoutProperties> button_props;  // Emoji鎸夐挳ID -> 甯冨眬灞炴€?
};

extern std::map<HWND, LayoutManager*> g_layout_managers;

// Global variables
extern std::map<HWND, WindowState*> g_windows;
extern std::map<HWND, MsgBoxState*> g_msgboxes;
extern std::map<HWND, TabControlState*> g_tab_controls;
extern std::map<HWND, EditBoxState*> g_editboxes;
extern std::map<HWND, LabelState*> g_labels;
extern std::map<HWND, CheckBoxState*> g_checkboxes;
extern std::map<HWND, ProgressBarState*> g_progressbars;
extern std::map<HWND, PictureBoxState*> g_pictureboxes;
extern std::map<HWND, RadioButtonState*> g_radiobuttons;
extern std::map<HWND, SliderState*> g_sliders;
extern std::map<HWND, SwitchState*> g_switches;
extern std::map<HWND, TooltipState*> g_tooltips;
extern std::map<HWND, NotificationState*> g_notifications;
extern std::map<int, std::vector<HWND>> g_radio_groups;  // 鍒嗙粍绠＄悊
extern std::map<HWND, ListBoxState*> g_listboxes;
extern std::map<HWND, ComboBoxState*> g_comboboxes;
extern std::map<HWND, D2DComboBoxState*> g_d2d_comboboxes;
extern std::map<HWND, HotKeyState*> g_hotkeys;
extern std::map<HWND, GroupBoxState*> g_groupboxes;
extern std::map<HWND, PanelState*> g_panels;
extern std::map<HWND, DataGridViewState*> g_datagrids;
extern std::map<HWND, MenuBarState*> g_menu_bars;
extern std::map<HWND, PopupMenuState*> g_popup_menus;
extern std::map<HWND, HWND> g_control_menu_bindings;
extern std::map<HWND, std::map<int, HWND>> g_button_popup_menu_bindings;
extern ButtonClickCallback g_button_callback;
extern WindowResizeCallback g_window_resize_callback;
extern WindowCloseCallback g_window_close_callback;

// Export functions (stdcall calling convention)
extern "C" {
    __declspec(dllexport) HWND __stdcall create_window(const char* title, int x, int y, int width, int height);
    __declspec(dllexport) HWND __stdcall create_window_bytes(const unsigned char* title_bytes, int title_len, int x, int y, int width, int height);
    __declspec(dllexport) HWND __stdcall create_window_bytes_ex(const unsigned char* title_bytes, int title_len, int x, int y, int width, int height, UINT32 titlebar_color, UINT32 client_bg_color);
    __declspec(dllexport) void __stdcall set_window_titlebar_color(HWND hwnd, UINT32 color);
    __declspec(dllexport) void __stdcall SetWindowBackgroundColor(HWND hwnd, UINT32 color);

    __declspec(dllexport) int __stdcall create_emoji_button_bytes(
        HWND parent,
        const unsigned char* emoji_bytes,
        int emoji_len,
        const unsigned char* text_bytes,
        int text_len,
        int x, int y, int width, int height,
        UINT32 bg_color
    );

    __declspec(dllexport) void __stdcall set_button_click_callback(ButtonClickCallback callback);
    
    // ========== 鎸夐挳灞炴€у懡浠?==========
    
    // 鑾峰彇鎸夐挳鏂囨湰 (UTF-8缂栫爜锛屼袱娆¤皟鐢ㄦā寮?
    __declspec(dllexport) int __stdcall GetButtonText(int button_id, unsigned char* buffer, int buffer_size);
    
    // 璁剧疆鎸夐挳鏂囨湰 (UTF-8缂栫爜)
    __declspec(dllexport) void __stdcall SetButtonText(int button_id, const unsigned char* text, int text_len);
    
    // 鑾峰彇鎸夐挳Emoji (UTF-8缂栫爜锛屼袱娆¤皟鐢ㄦā寮?
    __declspec(dllexport) int __stdcall GetButtonEmoji(int button_id, unsigned char* buffer, int buffer_size);
    
    // 璁剧疆鎸夐挳Emoji (UTF-8缂栫爜)
    __declspec(dllexport) void __stdcall SetButtonEmoji(int button_id, const unsigned char* emoji, int emoji_len);
    
    // 鑾峰彇鎸夐挳浣嶇疆鍜屽ぇ灏?
    __declspec(dllexport) int __stdcall GetButtonBounds(int button_id, int* x, int* y, int* width, int* height);
    
    // 璁剧疆鎸夐挳浣嶇疆鍜屽ぇ灏?
    __declspec(dllexport) void __stdcall SetButtonBounds(int button_id, int x, int y, int width, int height);
    
    // 鑾峰彇鎸夐挳鑳屾櫙鑹?(ARGB鏍煎紡)
    __declspec(dllexport) UINT32 __stdcall GetButtonBackgroundColor(int button_id);
    
    // 璁剧疆鎸夐挳鑳屾櫙鑹?(ARGB鏍煎紡)
    __declspec(dllexport) void __stdcall SetButtonBackgroundColor(int button_id, UINT32 color);
    __declspec(dllexport) UINT32 __stdcall GetButtonTextColor(int button_id);
    __declspec(dllexport) void __stdcall SetButtonTextColor(int button_id, UINT32 color);
    __declspec(dllexport) UINT32 __stdcall GetButtonBorderColor(int button_id);
    __declspec(dllexport) void __stdcall SetButtonBorderColor(int button_id, UINT32 color);
    __declspec(dllexport) int __stdcall GetButtonHoverColors(int button_id, UINT32* bg_color, UINT32* border_color, UINT32* text_color);
    __declspec(dllexport) void __stdcall SetButtonHoverColors(int button_id, UINT32 bg_color, UINT32 border_color, UINT32 text_color);
    __declspec(dllexport) void __stdcall ResetButtonColorOverrides(int button_id);
    __declspec(dllexport) void __stdcall SetButtonType(int button_id, int type);
    __declspec(dllexport) int __stdcall GetButtonType(int button_id);
    __declspec(dllexport) void __stdcall SetButtonStyle(int button_id, int style);
    __declspec(dllexport) int __stdcall GetButtonStyle(int button_id);
    __declspec(dllexport) void __stdcall SetButtonSize(int button_id, int size);
    __declspec(dllexport) int __stdcall GetButtonSize(int button_id);
    __declspec(dllexport) void __stdcall SetButtonRound(int button_id, BOOL round);
    __declspec(dllexport) BOOL __stdcall GetButtonRound(int button_id);
    __declspec(dllexport) void __stdcall SetButtonCircle(int button_id, BOOL circle);
    __declspec(dllexport) BOOL __stdcall GetButtonCircle(int button_id);
    __declspec(dllexport) void __stdcall SetButtonLoading(int button_id, BOOL loading);
    __declspec(dllexport) BOOL __stdcall GetButtonLoading(int button_id);

    // 鑾峰彇鎸夐挳鍙鐘舵€?
    __declspec(dllexport) BOOL __stdcall GetButtonVisible(int button_id);
    
    // 鏄剧ず/闅愯棌鎸夐挳
    __declspec(dllexport) void __stdcall ShowButton(int button_id, BOOL visible);
    
    // 鑾峰彇鎸夐挳鍚敤鐘舵€?
    __declspec(dllexport) BOOL __stdcall GetButtonEnabled(int button_id);
    
    // 鍚敤鎸夐挳 (宸插瓨鍦?
    __declspec(dllexport) void __stdcall EnableButton(HWND parent_hwnd, int button_id, BOOL enable);
    
    // 绂佺敤鎸夐挳 (宸插瓨鍦?
    __declspec(dllexport) void __stdcall DisableButton(HWND parent_hwnd, int button_id);
    
    // ========== 绐楀彛灞炴€у懡浠?==========
    
    // 鑾峰彇绐楀彛鏍囬 (UTF-8缂栫爜锛屼袱娆¤皟鐢ㄦā寮?
    __declspec(dllexport) int __stdcall GetWindowTitle(HWND hwnd, unsigned char* buffer, int buffer_size);
    
    // 鑾峰彇绐楀彛浣嶇疆鍜屽ぇ灏?
    __declspec(dllexport) int __stdcall GetWindowBounds(HWND hwnd, int* x, int* y, int* width, int* height);
    
    // 璁剧疆绐楀彛浣嶇疆鍜屽ぇ灏?
    __declspec(dllexport) void __stdcall SetWindowBounds(HWND hwnd, int x, int y, int width, int height);
    
    // 鑾峰彇绐楀彛鍙鐘舵€?
    __declspec(dllexport) int __stdcall GetWindowVisible(HWND hwnd);
    
    // 鏄剧ず鎴栭殣钘忕獥鍙?
    __declspec(dllexport) void __stdcall ShowEmojiWindow(HWND hwnd, int visible);
    
    // 鑾峰彇绐楀彛鏍囬鏍忛鑹?(RGB鏍煎紡)
    __declspec(dllexport) UINT32 __stdcall GetWindowTitlebarColor(HWND hwnd);
    __declspec(dllexport) int __stdcall SetTitleBarTextColor(HWND hwnd, UINT32 color);
    __declspec(dllexport) UINT32 __stdcall GetTitleBarTextColor(HWND hwnd);
    __declspec(dllexport) int __stdcall SetTitleBarFont(HWND hwnd, const unsigned char* fontName, int fontNameLen, float fontSize);
    __declspec(dllexport) int __stdcall SetTitleBarAlignment(HWND hwnd, int alignment);

    __declspec(dllexport) void __stdcall set_message_loop_main_window(HWND hwnd);
    __declspec(dllexport) int __stdcall run_message_loop();
    __declspec(dllexport) void __stdcall destroy_window(HWND hwnd);
    __declspec(dllexport) void __stdcall set_window_icon(HWND hwnd, const char* icon_path);
    // 浠庡瓧鑺傞泦璁剧疆绐楀彛鍥炬爣锛堟槗璇█鍙彃鍏ュ浘鐗囪祫婧愬悗浼犲叆 鍙栧彉閲忔暟鎹湴鍧€(瀛楄妭闆? 鍜?鍙栧瓧鑺傞泦闀垮害(瀛楄妭闆?锛?
    __declspec(dllexport) void __stdcall set_window_icon_bytes(HWND hwnd, const unsigned char* icon_data, int data_len);
    __declspec(dllexport) void __stdcall set_window_title(HWND hwnd, const char* title_utf8, int title_len);

    // Message box (OK button only, supports Unicode and Emoji)
    __declspec(dllexport) void __stdcall show_message_box_bytes(
        HWND parent,
        const unsigned char* title_bytes,
        int title_len,
        const unsigned char* message_bytes,
        int message_len,
        const unsigned char* icon_bytes,
        int icon_len
    );

    // Confirm box (with callback, supports Unicode and Emoji)
    __declspec(dllexport) void __stdcall show_confirm_box_bytes(
        HWND parent,
        const unsigned char* title_bytes,
        int title_len,
        const unsigned char* message_bytes,
        int message_len,
        const unsigned char* icon_bytes,
        int icon_len,
        MessageBoxCallback callback
    );

    // ========== TabControl 鍔熻兘 ==========

    // 鍒涘缓 TabControl
    __declspec(dllexport) HWND __stdcall CreateTabControl(
        HWND hParent,
        int x,
        int y,
        int width,
        int height
    );

    // 娣诲姞 Tab 椤?
    __declspec(dllexport) int __stdcall AddTabItem(
        HWND hTabControl,
        const unsigned char* title_bytes,
        int title_len,
        HWND hContentWindow  // 鍙€夛紝鑻ヤ负 NULL 鍒欒嚜鍔ㄥ垱寤?
    );

    // 绉婚櫎 Tab 椤?
    __declspec(dllexport) BOOL __stdcall RemoveTabItem(
        HWND hTabControl,
        int index
    );

    // 璁剧疆 Tab 鍒囨崲鍥炶皟
    __declspec(dllexport) void __stdcall SetTabCallback(
        HWND hTabControl,
        TAB_CALLBACK pCallback
    );

    // 鑾峰彇褰撳墠閫変腑鐨?Tab 绱㈠紩
    __declspec(dllexport) int __stdcall GetCurrentTabIndex(
        HWND hTabControl
    );

    // 鍒囨崲鍒版寚瀹?Tab
    __declspec(dllexport) BOOL __stdcall SelectTab(
        HWND hTabControl,
        int index
    );

    __declspec(dllexport) BOOL __stdcall SelectTabImmediate(
        HWND hTabControl,
        int index
    );

    // 鑾峰彇 Tab 鏁伴噺
    __declspec(dllexport) int __stdcall GetTabCount(
        HWND hTabControl
    );

    // 鑾峰彇鎸囧畾 Tab 鐨勫唴瀹圭獥鍙ｅ彞鏌?
    __declspec(dllexport) HWND __stdcall GetTabContentWindow(
        HWND hTabControl,
        int index
    );

    // 閿€姣?TabControl锛堟竻鐞嗚祫婧愶級
    __declspec(dllexport) void __stdcall DestroyTabControl(
        HWND hTabControl
    );

    // 鎵嬪姩鏇存柊 TabControl 甯冨眬锛堢獥鍙ｅぇ灏忔敼鍙樺悗璋冪敤锛?
    __declspec(dllexport) void __stdcall UpdateTabControlLayout(
        HWND hTabControl
    );

    __declspec(dllexport) BOOL __stdcall RedrawTabControl(
        HWND hTabControl
    );

    // 鑾峰彇鎸囧畾 Tab 椤电殑鏍囬锛圲TF-8锛屼袱娆¤皟鐢ㄦā寮忥級
    __declspec(dllexport) int __stdcall GetTabTitle(
        HWND hTabControl,
        int index,
        unsigned char* buffer,
        int bufferSize
    );

    // 璁剧疆鎸囧畾 Tab 椤电殑鏍囬锛圲TF-8锛?
    __declspec(dllexport) int __stdcall SetTabTitle(
        HWND hTabControl,
        int index,
        const unsigned char* title_bytes,
        int title_len
    );

    // 鑾峰彇 TabControl 鐨勪綅缃拰澶у皬
    __declspec(dllexport) int __stdcall GetTabControlBounds(
        HWND hTabControl,
        int* x,
        int* y,
        int* width,
        int* height
    );

    // 璁剧疆 TabControl 鐨勪綅缃拰澶у皬
    __declspec(dllexport) int __stdcall SetTabControlBounds(
        HWND hTabControl,
        int x,
        int y,
        int width,
        int height
    );

    // 鑾峰彇 TabControl 鐨勫彲瑙嗙姸鎬侊紙1=鍙, 0=涓嶅彲瑙? -1=閿欒锛?
    __declspec(dllexport) int __stdcall GetTabControlVisible(
        HWND hTabControl
    );

    // 鏄剧ず鎴栭殣钘?TabControl锛坴isible: 1=鏄剧ず, 0=闅愯棌锛?
    __declspec(dllexport) int __stdcall ShowTabControl(
        HWND hTabControl,
        int visible
    );

    // 鍚敤鎴栫鐢?TabControl锛坋nabled: 1=鍚敤, 0=绂佺敤锛?
    __declspec(dllexport) int __stdcall EnableTabControl(
        HWND hTabControl,
        int enabled
    );

    // ========== TabControl 澶栬鍑芥暟 ==========

    // 璁剧疆鏍囩椤靛浐瀹氬昂瀵革紙瀹藉害鍜岄珮搴︼級
    __declspec(dllexport) int __stdcall SetTabItemSize(
        HWND hTab,
        int width,
        int height
    );

    // 璁剧疆鏍囩椤靛瓧浣擄紙fontName 涓?UTF-8 缂栫爜锛?
    __declspec(dllexport) int __stdcall SetTabFont(
        HWND hTab,
        const unsigned char* fontName,
        int fontNameLen,
        float fontSize
    );

    // 璁剧疆鏍囩椤甸鑹诧紙閫変腑/鏈€変腑鐨勮儗鏅壊鍜屾枃瀛楄壊锛孉RGB 鏍煎紡锛?
    __declspec(dllexport) int __stdcall SetTabColors(
        HWND hTab,
        UINT32 selectedBg,
        UINT32 unselectedBg,
        UINT32 selectedText,
        UINT32 unselectedText
    );

    // 璁剧疆閫変腑鏍囩椤靛簳閮ㄦ寚绀烘潯棰滆壊锛圓RGB 鏍煎紡锛?
    __declspec(dllexport) int __stdcall SetTabIndicatorColor(
        HWND hTab,
        UINT32 color
    );

    // 璁剧疆鏍囩椤靛唴杈硅窛锛堟按骞冲拰鍨傜洿锛?
    __declspec(dllexport) int __stdcall SetTabPadding(
        HWND hTab,
        int horizontal,
        int vertical
    );

    __declspec(dllexport) int __stdcall SetTabHeaderStyle(
        HWND hTab,
        int style
    );

    // ========== TabControl 鍗曚釜鏍囩椤垫帶鍒跺嚱鏁?==========

    // 鍚敤/绂佺敤鍗曚釜鏍囩椤碉紙enabled: 1=鍚敤, 0=绂佺敤锛?
    __declspec(dllexport) int __stdcall EnableTabItem(
        HWND hTab,
        int index,
        int enabled
    );

    // 鑾峰彇鍗曚釜鏍囩椤电殑鍚敤鐘舵€侊紙杩斿洖 1=鍚敤, 0=绂佺敤, -1=閿欒锛?
    __declspec(dllexport) int __stdcall GetTabItemEnabled(
        HWND hTab,
        int index
    );

    // 鏄剧ず/闅愯棌鍗曚釜鏍囩椤碉紙visible: 1=鏄剧ず, 0=闅愯棌锛?
    __declspec(dllexport) int __stdcall ShowTabItem(
        HWND hTab,
        int index,
        int visible
    );

    // 璁剧疆鏍囩椤靛浘鏍囷紙PNG 瀛楄妭鏁版嵁锛宨conBytes=NULL 鎴?iconLen=0 鏃舵竻闄ゅ浘鏍囷級
    __declspec(dllexport) int __stdcall SetTabItemIcon(
        HWND hTab,
        int index,
        const unsigned char* iconBytes,
        int iconLen
    );

    // 璁剧疆鎸囧畾鏍囩椤电殑鍐呭鍖哄煙鑳屾櫙鑹诧紙ARGB 鏍煎紡锛?
    __declspec(dllexport) int __stdcall SetTabContentBgColor(
        HWND hTab,
        int index,
        UINT32 color
    );

    // 璁剧疆鎵€鏈夋爣绛鹃〉鐨勫唴瀹瑰尯鍩熻儗鏅壊锛圓RGB 鏍煎紡锛?
    __declspec(dllexport) int __stdcall SetTabContentBgColorAll(
        HWND hTab,
        UINT32 color
    );

    // ========== TabControl 浜や簰澧炲己鍑芥暟 ==========

    // 璁剧疆鏍囩椤垫槸鍚︽樉绀哄叧闂寜閽紙closable: 1=鏄剧ず, 0=闅愯棌锛?
    __declspec(dllexport) int __stdcall SetTabClosable(
        HWND hTab,
        int closable
    );

    // 璁剧疆鏍囩椤靛叧闂洖璋?
    __declspec(dllexport) int __stdcall SetTabCloseCallback(
        HWND hTab,
        TAB_CLOSE_CALLBACK callback
    );

    // 璁剧疆鏍囩椤靛彸閿偣鍑诲洖璋?
    __declspec(dllexport) int __stdcall SetTabRightClickCallback(
        HWND hTab,
        TAB_RIGHTCLICK_CALLBACK callback
    );

    // 璁剧疆鏍囩椤垫槸鍚﹀彲鎷栨嫿鎺掑簭锛坉raggable: 1=鍙嫋鎷? 0=涓嶅彲鎷栨嫿锛?
    __declspec(dllexport) int __stdcall SetTabDraggable(
        HWND hTab,
        int draggable
    );

    // 璁剧疆鏍囩椤靛弻鍑诲洖璋?
    __declspec(dllexport) int __stdcall SetTabDoubleClickCallback(
        HWND hTab,
        TAB_DBLCLICK_CALLBACK callback
    );

    __declspec(dllexport) int __stdcall SetTabNewButtonCallback(
        HWND hTab,
        TAB_NEW_BUTTON_CALLBACK callback
    );

    // ========== 甯冨眬涓庝綅缃嚱鏁?==========

    // 璁剧疆鏍囩鏍忎綅缃紙position: 0=涓? 1=涓? 2=宸? 3=鍙筹級
    __declspec(dllexport) int __stdcall SetTabPosition(
        HWND hTab,
        int position
    );

    // 璁剧疆鏍囩瀵归綈鏂瑰紡锛坅lign: 0=宸﹀榻? 1=灞呬腑, 2=鍙冲榻愶級
    __declspec(dllexport) int __stdcall SetTabAlignment(
        HWND hTab,
        int align
    );

    // 璁剧疆鏍囩鏍忔槸鍚﹀彲婊氬姩锛坰crollable: 1=鍙粴鍔? 0=涓嶅彲婊氬姩/澶氳锛?
    __declspec(dllexport) int __stdcall SetTabScrollable(
        HWND hTab,
        int scrollable
    );

    // ========== 鎵归噺鎿嶄綔鍑芥暟 ==========

    // 娓呯┖鎵€鏈夋爣绛鹃〉锛堥攢姣佸唴瀹圭獥鍙ｏ紝娓呯悊璧勬簮锛?
    __declspec(dllexport) int __stdcall RemoveAllTabs(
        HWND hTab
    );

    // 鍦ㄦ寚瀹氫綅缃彃鍏ユ爣绛鹃〉锛坕ndex < 0 杩斿洖 -1锛岃秴鍑鸿寖鍥磋拷鍔犲埌鏈熬锛?
    __declspec(dllexport) int __stdcall InsertTabItem(
        HWND hTab,
        int index,
        const unsigned char* title,
        int titleLen,
        HWND hContent
    );

    // 绉诲姩鏍囩椤典綅缃紙fromIndex == toIndex 杩斿洖 0 涓嶆搷浣滐級
    __declspec(dllexport) int __stdcall MoveTabItem(
        HWND hTab,
        int fromIndex,
        int toIndex
    );

    // 鏍规嵁鏍囬鏌ユ壘鏍囩椤电储寮曪紙绮剧‘鍖归厤锛屽尯鍒嗗ぇ灏忓啓锛屾湭鎵惧埌杩斿洖 -1锛?
    __declspec(dllexport) int __stdcall GetTabIndexByTitle(
        HWND hTab,
        const unsigned char* titleBytes,
        int titleLen
    );

    // 鑾峰彇鏁翠釜 TabControl 鐨勫惎鐢ㄧ姸鎬侊紙1=鍚敤, 0=绂佺敤, -1=鏃犳晥鍙ユ焺锛?
    __declspec(dllexport) int __stdcall GetTabEnabled(
        HWND hTab
    );

    // 鍒ゆ柇鎸囧畾鏍囩椤垫槸鍚︿负褰撳墠閫変腑锛?=閫変腑, 0=鏈€変腑, -1=閿欒锛?
    __declspec(dllexport) int __stdcall IsTabItemSelected(
        HWND hTab,
        int index
    );

    // ========== 绐楀彛澶у皬鏀瑰彉鍥炶皟 ==========

    // 璁剧疆绐楀彛澶у皬鏀瑰彉鍥炶皟
    __declspec(dllexport) void __stdcall SetWindowResizeCallback(
        WindowResizeCallback callback
    );

    // 璁剧疆鑷粯绐楀彛鍏抽棴鍥炶皟
    // 褰撹嚜缁樼獥鍙ｈ鍏抽棴锛堢敤鎴风偣 X 鎴栦唬鐮佽皟鐢?destroy_window锛夋椂瑙﹀彂
    __declspec(dllexport) void __stdcall SetWindowCloseCallback(
        WindowCloseCallback callback
    );

    // ========== 鑿滃崟鏍?/ 鍙抽敭鑿滃崟 ==========

    // 鍒涘缓鑿滃崟鏍忥紙缁戝畾鍒扮獥鍙ｏ級
    __declspec(dllexport) HWND __stdcall CreateMenuBar(
        HWND hWindow
    );

    // 閿€姣佽彍鍗曟爮
    __declspec(dllexport) void __stdcall DestroyMenuBar(
        HWND hMenuBar
    );

    // 娣诲姞鑿滃崟鏍忛」
    __declspec(dllexport) int __stdcall MenuBarAddItem(
        HWND hMenuBar,
        const unsigned char* text_bytes,
        int text_len,
        int item_id
    );

    // 娣诲姞鑿滃崟鏍忓瓙椤?
    __declspec(dllexport) int __stdcall MenuBarAddSubItem(
        HWND hMenuBar,
        int parent_item_id,
        const unsigned char* text_bytes,
        int text_len,
        int item_id
    );

    // 璁剧疆鑿滃崟鏍忎綅缃拰澶у皬
    __declspec(dllexport) void __stdcall SetMenuBarPlacement(
        HWND hMenuBar,
        int x, int y, int width, int height
    );

    // 璁剧疆鑿滃崟鏍忓洖璋?
    __declspec(dllexport) void __stdcall SetMenuBarCallback(
        HWND hMenuBar,
        MenuItemClickCallback callback
    );

    // 鏇存柊鑿滃崟鏍忓瓙椤规枃鏈?
    __declspec(dllexport) BOOL __stdcall MenuBarUpdateSubItemText(
        HWND hMenuBar,
        int parent_item_id,
        int item_id,
        const unsigned char* text_bytes,
        int text_len
    );

    // 鍒涘缓寮瑰嚭鑿滃崟锛堝彸閿彍鍗曪級
    __declspec(dllexport) HWND __stdcall CreateEmojiPopupMenu(
        HWND hOwner
    );

    // 閿€姣佸脊鍑鸿彍鍗?
    __declspec(dllexport) void __stdcall DestroyEmojiPopupMenu(
        HWND hPopupMenu
    );

    // 娣诲姞寮瑰嚭鑿滃崟椤?
    __declspec(dllexport) int __stdcall PopupMenuAddItem(
        HWND hPopupMenu,
        const unsigned char* text_bytes,
        int text_len,
        int item_id
    );

    // 娣诲姞寮瑰嚭鑿滃崟瀛愰」
    __declspec(dllexport) int __stdcall PopupMenuAddSubItem(
        HWND hPopupMenu,
        int parent_item_id,
        const unsigned char* text_bytes,
        int text_len,
        int item_id
    );

    // 缁戝畾鎺т欢涓庡脊鍑鸿彍鍗?
    __declspec(dllexport) void __stdcall BindControlMenu(
        HWND hControl,
        HWND hPopupMenu
    );
    __declspec(dllexport) void __stdcall BindButtonMenu(
        HWND hParent,
        int button_id,
        HWND hPopupMenu
    );

    // 鏄剧ず鍙抽敭鑿滃崟
    __declspec(dllexport) void __stdcall ShowContextMenu(
        HWND hPopupMenu,
        int x,
        int y
    );

    // 璁剧疆寮瑰嚭鑿滃崟鍥炶皟
    __declspec(dllexport) void __stdcall SetPopupMenuCallback(
        HWND hPopupMenu,
        MenuItemClickCallback callback
    );

    // ========== 缂栬緫妗嗗姛鑳?==========

    // 鍒涘缓缂栬緫妗嗭紙vertical_center 浠呭崟琛屾湁鏁堬級
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
        int alignment,  // 0=宸? 1=涓? 2=鍙?
        BOOL multiline,
        BOOL readonly,
        BOOL password,
        BOOL has_border,
        BOOL vertical_center  // 鏂囨湰鍨傜洿灞呬腑锛堜粎鍗曡鏈夋晥锛?
    );

    // 鑾峰彇缂栬緫妗嗘枃鏈?
    __declspec(dllexport) int __stdcall GetEditBoxText(
        HWND hEdit,
        unsigned char* buffer,
        int buffer_size
    );

    // 璁剧疆缂栬緫妗嗘枃鏈?
    __declspec(dllexport) void __stdcall SetEditBoxText(
        HWND hEdit,
        const unsigned char* text_bytes,
        int text_len
    );

    // 璁剧疆缂栬緫妗嗗瓧浣?
    __declspec(dllexport) void __stdcall SetEditBoxFont(
        HWND hEdit,
        const unsigned char* font_name_bytes,
        int font_name_len,
        int font_size,
        BOOL bold,
        BOOL italic,
        BOOL underline
    );

    // 璁剧疆缂栬緫妗嗛鑹?
    __declspec(dllexport) void __stdcall SetEditBoxColor(
        HWND hEdit,
        UINT32 fg_color,
        UINT32 bg_color
    );

    // 璁剧疆缂栬緫妗嗕綅缃拰澶у皬
    __declspec(dllexport) void __stdcall SetEditBoxBounds(
        HWND hEdit,
        int x, int y, int width, int height
    );

    // 鍚敤/绂佺敤缂栬緫妗?
    __declspec(dllexport) void __stdcall EnableEditBox(
        HWND hEdit,
        BOOL enable
    );

    // 鏄剧ず/闅愯棌缂栬緫妗?
    __declspec(dllexport) void __stdcall ShowEditBox(
        HWND hEdit,
        BOOL show
    );

    // 璁剧疆缂栬緫妗嗘枃鏈槸鍚﹀瀭鐩村眳涓紙浠呭崟琛岀紪杈戞鏈夋晥锛?
    __declspec(dllexport) void __stdcall SetEditBoxVerticalCenter(
        HWND hEdit,
        BOOL vertical_center
    );

    // 璁剧疆缂栬緫妗嗘寜閿洖璋冿紙key_down: 1=鎸変笅 0=鏉惧紑; shift/ctrl/alt: 0=鏈寜 1=鎸変笅锛?
    __declspec(dllexport) void __stdcall SetEditBoxKeyCallback(
        HWND hEdit,
        EditBoxKeyCallback callback
    );

    // 鑾峰彇缂栬緫妗嗗瓧浣撲俊鎭紙杩斿洖瀛椾綋鍚嶅瓧鑺傛暟锛?1=閿欒锛?
    __declspec(dllexport) int __stdcall GetEditBoxFont(
        HWND hEdit,
        unsigned char* font_name_buffer,
        int font_name_buffer_size,
        int* font_size,
        int* bold,
        int* italic,
        int* underline
    );

    // 鑾峰彇缂栬緫妗嗛鑹诧紙杩斿洖0=鎴愬姛锛?1=閿欒锛?
    __declspec(dllexport) int __stdcall GetEditBoxColor(
        HWND hEdit,
        UINT32* fg_color,
        UINT32* bg_color
    );

    // 鑾峰彇缂栬緫妗嗕綅缃拰澶у皬锛堣繑鍥?=鎴愬姛锛?1=閿欒锛?
    __declspec(dllexport) int __stdcall GetEditBoxBounds(
        HWND hEdit,
        int* x,
        int* y,
        int* width,
        int* height
    );

    // 鑾峰彇缂栬緫妗嗗榻愭柟寮忥紙0=宸? 1=涓? 2=鍙? -1=閿欒锛?
    __declspec(dllexport) int __stdcall GetEditBoxAlignment(
        HWND hEdit
    );

    __declspec(dllexport) void __stdcall SetEditBoxAlignment(
        HWND hEdit,
        int alignment
    );

    // 鑾峰彇缂栬緫妗嗗惎鐢ㄧ姸鎬侊紙1=鍚敤, 0=绂佺敤, -1=閿欒锛?
    __declspec(dllexport) int __stdcall GetEditBoxEnabled(
        HWND hEdit
    );

    // 鑾峰彇缂栬緫妗嗗彲瑙嗙姸鎬侊紙1=鍙, 0=涓嶅彲瑙? -1=閿欒锛?
    __declspec(dllexport) int __stdcall GetEditBoxVisible(
        HWND hEdit
    );

    // 鍒涘缓褰╄壊Emoji缂栬緫妗嗭紙浣跨敤RichEdit鎺т欢锛屾敮鎸佸僵鑹瞖moji鏄剧ず锛?
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
        int alignment,  // 0=宸? 1=涓? 2=鍙?
        BOOL multiline,
        BOOL readonly,
        BOOL password,
        BOOL has_border,
        BOOL vertical_center  // 鏂囨湰鍨傜洿灞呬腑锛堜粎鍗曡鏈夋晥锛?
    );

    __declspec(dllexport) void __stdcall SetD2DEditBoxColor(
        HWND hEdit,
        UINT32 fg_color,
        UINT32 bg_color
    );

    // ========== 鏍囩鍔熻兘 ==========  

    // 鍒涘缓鏍囩
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
        int alignment,  // 0=宸? 1=涓? 2=鍙?
        BOOL word_wrap  // 鏄惁鎹㈣鏄剧ず
    );

    // 鑾峰彇鏍囩鏂囨湰
    __declspec(dllexport) int __stdcall GetLabelText(
        HWND hLabel,
        unsigned char* buffer,
        int buffer_size
    );

    // 璁剧疆鏍囩鏂囨湰
    __declspec(dllexport) void __stdcall SetLabelText(
        HWND hLabel,
        const unsigned char* text_bytes,
        int text_len
    );

    // 璁剧疆鏍囩瀛椾綋
    __declspec(dllexport) void __stdcall SetLabelFont(
        HWND hLabel,
        const unsigned char* font_name_bytes,
        int font_name_len,
        int font_size,
        BOOL bold,
        BOOL italic,
        BOOL underline
    );

    // 璁剧疆鏍囩棰滆壊
    __declspec(dllexport) void __stdcall SetLabelColor(
        HWND hLabel,
        UINT32 fg_color,
        UINT32 bg_color
    );

    // 璁剧疆鏍囩浣嶇疆鍜屽ぇ灏?
    __declspec(dllexport) void __stdcall SetLabelBounds(
        HWND hLabel,
        int x, int y, int width, int height
    );

    // 鍚敤/绂佺敤鏍囩
    __declspec(dllexport) void __stdcall EnableLabel(
        HWND hLabel,
        BOOL enable
    );

    // 鏄剧ず/闅愯棌鏍囩
    __declspec(dllexport) void __stdcall ShowLabel(
        HWND hLabel,
        BOOL show
    );

    // 鑾峰彇鏍囩瀛椾綋淇℃伅
    __declspec(dllexport) int __stdcall GetLabelFont(
        HWND hLabel,
        unsigned char* font_name_buffer,
        int font_name_buffer_size,
        int* font_size,
        int* bold,
        int* italic,
        int* underline
    );

    // 鑾峰彇鏍囩棰滆壊
    __declspec(dllexport) int __stdcall GetLabelColor(
        HWND hLabel,
        UINT32* fg_color,
        UINT32* bg_color
    );

    // 鑾峰彇鏍囩浣嶇疆鍜屽ぇ灏?
    __declspec(dllexport) int __stdcall GetLabelBounds(
        HWND hLabel,
        int* x,
        int* y,
        int* width,
        int* height
    );

    // 鑾峰彇鏍囩瀵归綈鏂瑰紡 (0=宸? 1=涓? 2=鍙?
    __declspec(dllexport) int __stdcall GetLabelAlignment(
        HWND hLabel
    );

    // 鑾峰彇鏍囩鍚敤鐘舵€?(1=鍚敤, 0=绂佺敤, -1=閿欒)
    __declspec(dllexport) int __stdcall GetLabelEnabled(
        HWND hLabel
    );

    // 鑾峰彇鏍囩鍙鐘舵€?(1=鍙, 0=涓嶅彲瑙? -1=閿欒)
    __declspec(dllexport) int __stdcall GetLabelVisible(
        HWND hLabel
    );

    // ========== 澶嶉€夋鍔熻兘 ==========

    // 鍒涘缓澶嶉€夋
    __declspec(dllexport) HWND __stdcall CreateCheckBox(
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
    );

    // 鑾峰彇澶嶉€夋閫変腑鐘舵€?
    __declspec(dllexport) BOOL __stdcall GetCheckBoxState(
        HWND hCheckBox
    );

    // 璁剧疆澶嶉€夋閫変腑鐘舵€?
    __declspec(dllexport) void __stdcall SetCheckBoxState(
        HWND hCheckBox,
        BOOL checked
    );

    // 璁剧疆澶嶉€夋鍥炶皟
    __declspec(dllexport) void __stdcall SetCheckBoxCallback(
        HWND hCheckBox,
        CheckBoxCallback callback
    );

    // 鍚敤/绂佺敤澶嶉€夋
    __declspec(dllexport) void __stdcall EnableCheckBox(
        HWND hCheckBox,
        BOOL enable
    );

    // 鏄剧ず/闅愯棌澶嶉€夋
    __declspec(dllexport) void __stdcall ShowCheckBox(
        HWND hCheckBox,
        BOOL show
    );

    // 璁剧疆澶嶉€夋鏂囨湰
    __declspec(dllexport) void __stdcall SetCheckBoxText(
        HWND hCheckBox,
        const unsigned char* text_bytes,
        int text_len
    );

    // 璁剧疆澶嶉€夋浣嶇疆鍜屽ぇ灏?
    __declspec(dllexport) void __stdcall SetCheckBoxBounds(
        HWND hCheckBox,
        int x, int y, int width, int height
    );

    // 鑾峰彇澶嶉€夋鏂囨湰锛圲TF-8锛屼袱娆¤皟鐢ㄦā寮忥級
    __declspec(dllexport) int __stdcall GetCheckBoxText(
        HWND hCheckBox,
        unsigned char* buffer,
        int buffer_size
    );

    // 璁剧疆澶嶉€夋瀛椾綋
    __declspec(dllexport) void __stdcall SetCheckBoxFont(
        HWND hCheckBox,
        const unsigned char* font_name_bytes,
        int font_name_len,
        int font_size,
        int bold,
        int italic,
        int underline
    );

    // 鑾峰彇澶嶉€夋瀛椾綋淇℃伅锛堜袱娆¤皟鐢ㄦā寮忥紝杩斿洖瀛椾綋鍚峌TF-8瀛楄妭鏁帮級
    __declspec(dllexport) int __stdcall GetCheckBoxFont(
        HWND hCheckBox,
        unsigned char* font_name_buffer,
        int font_name_buffer_size,
        int* font_size,
        int* bold,
        int* italic,
        int* underline
    );

    // 璁剧疆澶嶉€夋棰滆壊
    __declspec(dllexport) void __stdcall SetCheckBoxColor(
        HWND hCheckBox,
        UINT32 fg_color,
        UINT32 bg_color
    );

    __declspec(dllexport) void __stdcall SetCheckBoxCheckColor(
        HWND hCheckBox,
        UINT32 check_color
    );

    __declspec(dllexport) int __stdcall GetCheckBoxCheckColor(
        HWND hCheckBox,
        UINT32* check_color
    );

    __declspec(dllexport) void __stdcall SetCheckBoxStyle(
        HWND hCheckBox,
        int style
    );

    __declspec(dllexport) int __stdcall GetCheckBoxStyle(
        HWND hCheckBox
    );

    // 鑾峰彇澶嶉€夋棰滆壊锛堣繑鍥?鎴愬姛锛?1澶辫触锛?
    __declspec(dllexport) int __stdcall GetCheckBoxColor(
        HWND hCheckBox,
        UINT32* fg_color,
        UINT32* bg_color
    );

    // ========== 杩涘害鏉″姛鑳?==========

    // 鍒涘缓杩涘害鏉?
    __declspec(dllexport) HWND __stdcall CreateProgressBar(
        HWND hParent,
        int x, int y, int width, int height,
        int initial_value,  // 鍒濆鍊?(0-100)
        UINT32 fg_color,
        UINT32 bg_color,
        BOOL show_text,     // 鏄惁鏄剧ず鐧惧垎姣旀枃鏈?
        UINT32 text_color   // 鏂囨湰棰滆壊
    );

    // 璁剧疆杩涘害鏉″€?(0-100)
    __declspec(dllexport) void __stdcall SetProgressValue(
        HWND hProgressBar,
        int value
    );

    // 鑾峰彇杩涘害鏉″€?
    __declspec(dllexport) int __stdcall GetProgressValue(
        HWND hProgressBar
    );

    // 璁剧疆杩涘害鏉′笉纭畾妯″紡
    __declspec(dllexport) void __stdcall SetProgressIndeterminate(
        HWND hProgressBar,
        BOOL indeterminate
    );

    // 璁剧疆杩涘害鏉￠鑹?
    __declspec(dllexport) void __stdcall SetProgressBarColor(
        HWND hProgressBar,
        UINT32 fg_color,
        UINT32 bg_color
    );

    __declspec(dllexport) void __stdcall SetProgressBarTextColor(
        HWND hProgressBar,
        UINT32 text_color
    );

    // 璁剧疆杩涘害鏉″洖璋?
    __declspec(dllexport) void __stdcall SetProgressBarCallback(
        HWND hProgressBar,
        ProgressBarCallback callback
    );

    // 鍚敤/绂佺敤杩涘害鏉?
    __declspec(dllexport) void __stdcall EnableProgressBar(
        HWND hProgressBar,
        BOOL enable
    );

    // 鏄剧ず/闅愯棌杩涘害鏉?
    __declspec(dllexport) void __stdcall ShowProgressBar(
        HWND hProgressBar,
        BOOL show
    );

    // 璁剧疆杩涘害鏉′綅缃拰澶у皬
    __declspec(dllexport) void __stdcall SetProgressBarBounds(
        HWND hProgressBar,
        int x, int y, int width, int height
    );

    // 璁剧疆鏄惁鏄剧ず鐧惧垎姣旀枃鏈?
    __declspec(dllexport) void __stdcall SetProgressBarShowText(
        HWND hProgressBar,
        BOOL show_text
    );

    // 鑾峰彇杩涘害鏉￠鑹?
    __declspec(dllexport) int __stdcall GetProgressBarColor(
        HWND hProgressBar,
        UINT32* fg_color,
        UINT32* bg_color
    );

    // 鑾峰彇杩涘害鏉′綅缃拰澶у皬
    __declspec(dllexport) int __stdcall GetProgressBarBounds(
        HWND hProgressBar,
        int* x,
        int* y,
        int* width,
        int* height
    );

    // 鑾峰彇杩涘害鏉″惎鐢ㄧ姸鎬?
    __declspec(dllexport) int __stdcall GetProgressBarEnabled(
        HWND hProgressBar
    );

    // 鑾峰彇杩涘害鏉″彲瑙嗙姸鎬?
    __declspec(dllexport) int __stdcall GetProgressBarVisible(
        HWND hProgressBar
    );

    // 鑾峰彇杩涘害鏉℃槸鍚︽樉绀虹櫨鍒嗘瘮鏂囨湰
    __declspec(dllexport) int __stdcall GetProgressBarShowText(
        HWND hProgressBar
    );

    // ========== 鍥剧墖妗嗗姛鑳?==========

    // 鍒涘缓鍥剧墖妗?
    __declspec(dllexport) HWND __stdcall CreatePictureBox(
        HWND hParent,
        int x, int y, int width, int height,
        int scale_mode,     // 缂╂斁妯″紡: 0=涓嶇缉鏀? 1=鎷変几, 2=绛夋瘮缂╂斁, 3=灞呬腑
        UINT32 bg_color
    );

    // 浠庢枃浠跺姞杞藉浘鐗?
    __declspec(dllexport) BOOL __stdcall LoadImageFromFile(
        HWND hPictureBox,
        const unsigned char* file_path_bytes,
        int path_len
    );

    // 浠庡唴瀛樺姞杞藉浘鐗?
    __declspec(dllexport) BOOL __stdcall LoadImageFromMemory(
        HWND hPictureBox,
        const unsigned char* image_data,
        int data_len
    );

    // 娓呴櫎鍥剧墖
    __declspec(dllexport) void __stdcall ClearImage(
        HWND hPictureBox
    );

    // 璁剧疆鍥剧墖閫忔槑搴?
    __declspec(dllexport) void __stdcall SetImageOpacity(
        HWND hPictureBox,
        float opacity  // 0.0 - 1.0
    );

    // 璁剧疆鍥剧墖妗嗗洖璋?
    __declspec(dllexport) void __stdcall SetPictureBoxCallback(
        HWND hPictureBox,
        PictureBoxCallback callback
    );

    // 鍚敤/绂佺敤鍥剧墖妗?
    __declspec(dllexport) void __stdcall EnablePictureBox(
        HWND hPictureBox,
        BOOL enable
    );

    // 鏄剧ず/闅愯棌鍥剧墖妗?
    __declspec(dllexport) void __stdcall ShowPictureBox(
        HWND hPictureBox,
        BOOL show
    );

    // 璁剧疆鍥剧墖妗嗕綅缃拰澶у皬
    __declspec(dllexport) void __stdcall SetPictureBoxBounds(
        HWND hPictureBox,
        int x, int y, int width, int height
    );

    // 璁剧疆鍥剧墖妗嗙缉鏀炬ā寮?
    __declspec(dllexport) void __stdcall SetPictureBoxScaleMode(
        HWND hPictureBox,
        int scale_mode
    );

    // 璁剧疆鍥剧墖妗嗚儗鏅壊
    __declspec(dllexport) void __stdcall SetPictureBoxBackgroundColor(
        HWND hPictureBox,
        UINT32 bg_color
    );

    // ========== 鍗曢€夋寜閽姛鑳?==========

    // 鍒涘缓鍗曢€夋寜閽?
    __declspec(dllexport) HWND __stdcall CreateRadioButton(
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
    );

    // 鑾峰彇鍗曢€夋寜閽€変腑鐘舵€?
    __declspec(dllexport) BOOL __stdcall GetRadioButtonState(
        HWND hRadioButton
    );

    // 璁剧疆鍗曢€夋寜閽€変腑鐘舵€?
    __declspec(dllexport) void __stdcall SetRadioButtonState(
        HWND hRadioButton,
        BOOL checked
    );

    // 璁剧疆鍗曢€夋寜閽洖璋?
    __declspec(dllexport) void __stdcall SetRadioButtonCallback(
        HWND hRadioButton,
        RadioButtonCallback callback
    );

    // 鍚敤/绂佺敤鍗曢€夋寜閽?
    __declspec(dllexport) void __stdcall EnableRadioButton(
        HWND hRadioButton,
        BOOL enable
    );

    // 鏄剧ず/闅愯棌鍗曢€夋寜閽?
    __declspec(dllexport) void __stdcall ShowRadioButton(
        HWND hRadioButton,
        BOOL show
    );

    // 璁剧疆鍗曢€夋寜閽枃鏈?
    __declspec(dllexport) void __stdcall SetRadioButtonText(
        HWND hRadioButton,
        const unsigned char* text_bytes,
        int text_len
    );

    // 璁剧疆鍗曢€夋寜閽綅缃拰澶у皬
    __declspec(dllexport) void __stdcall SetRadioButtonBounds(
        HWND hRadioButton,
        int x, int y, int width, int height
    );

    // 鑾峰彇鍗曢€夋寜閽綅缃拰澶у皬
    __declspec(dllexport) int __stdcall GetRadioButtonBounds(
        HWND hRadioButton,
        int* x,
        int* y,
        int* width,
        int* height
    );

    // 鑾峰彇鍗曢€夋寜閽彲瑙嗙姸鎬?
    __declspec(dllexport) int __stdcall GetRadioButtonVisible(
        HWND hRadioButton
    );

    // 鑾峰彇鍗曢€夋寜閽惎鐢ㄧ姸鎬?
    __declspec(dllexport) int __stdcall GetRadioButtonEnabled(
        HWND hRadioButton
    );

    // 鑾峰彇鍗曢€夋寜閽枃鏈紙UTF-8锛屼袱娆¤皟鐢ㄦā寮忥級
    __declspec(dllexport) int __stdcall GetRadioButtonText(
        HWND hRadioButton,
        unsigned char* buffer,
        int buffer_size
    );

    // 璁剧疆鍗曢€夋寜閽瓧浣?
    __declspec(dllexport) void __stdcall SetRadioButtonFont(
        HWND hRadioButton,
        const unsigned char* font_name_bytes,
        int font_name_len,
        int font_size,
        int bold,
        int italic,
        int underline
    );

    // 鑾峰彇鍗曢€夋寜閽瓧浣撲俊鎭紙涓ゆ璋冪敤妯″紡锛?
    __declspec(dllexport) int __stdcall GetRadioButtonFont(
        HWND hRadioButton,
        unsigned char* font_name_buffer,
        int font_name_buffer_size,
        int* font_size,
        int* bold,
        int* italic,
        int* underline
    );

    // 璁剧疆鍗曢€夋寜閽鑹?
    __declspec(dllexport) void __stdcall SetRadioButtonColor(
        HWND hRadioButton,
        UINT32 fg_color,
        UINT32 bg_color
    );

    // 鑾峰彇鍗曢€夋寜閽鑹诧紙杩斿洖0鎴愬姛锛?1澶辫触锛?
    __declspec(dllexport) int __stdcall GetRadioButtonColor(
        HWND hRadioButton,
        UINT32* fg_color,
        UINT32* bg_color
    );

    __declspec(dllexport) void __stdcall SetRadioButtonDotColor(
        HWND hRadioButton,
        UINT32 dot_color
    );

    __declspec(dllexport) int __stdcall GetRadioButtonDotColor(
        HWND hRadioButton,
        UINT32* dot_color
    );

    __declspec(dllexport) void __stdcall SetRadioButtonStyle(
        HWND hRadioButton,
        int style
    );

    __declspec(dllexport) int __stdcall GetRadioButtonStyle(
        HWND hRadioButton
    );

    // ========== Slider ==========
    __declspec(dllexport) HWND __stdcall CreateSlider(
        HWND hParent,
        int x, int y, int width, int height,
        int min_value,
        int max_value,
        int value,
        int step,
        UINT32 active_color,
        UINT32 bg_color
    );

    __declspec(dllexport) int __stdcall GetSliderValue(HWND hSlider);
    __declspec(dllexport) void __stdcall SetSliderValue(HWND hSlider, int value);
    __declspec(dllexport) void __stdcall SetSliderRange(HWND hSlider, int min_value, int max_value);
    __declspec(dllexport) void __stdcall SetSliderStep(HWND hSlider, int step);
    __declspec(dllexport) void __stdcall SetSliderShowStops(HWND hSlider, BOOL show_stops);
    __declspec(dllexport) void __stdcall SetSliderColors(HWND hSlider, UINT32 active_color, UINT32 bg_color, UINT32 button_color);
    __declspec(dllexport) int __stdcall GetSliderColors(HWND hSlider, UINT32* active_color, UINT32* bg_color, UINT32* button_color);
    __declspec(dllexport) void __stdcall SetSliderCallback(HWND hSlider, SliderCallback callback);
    __declspec(dllexport) void __stdcall EnableSlider(HWND hSlider, BOOL enable);
    __declspec(dllexport) void __stdcall ShowSlider(HWND hSlider, BOOL show);
    __declspec(dllexport) void __stdcall SetSliderBounds(HWND hSlider, int x, int y, int width, int height);

    // ========== Switch ==========
    __declspec(dllexport) HWND __stdcall CreateSwitch(
        HWND hParent,
        int x, int y, int width, int height,
        BOOL checked,
        UINT32 active_color,
        UINT32 inactive_color,
        const unsigned char* active_text_bytes,
        int active_text_len,
        const unsigned char* inactive_text_bytes,
        int inactive_text_len
    );

    __declspec(dllexport) BOOL __stdcall GetSwitchState(HWND hSwitch);
    __declspec(dllexport) void __stdcall SetSwitchState(HWND hSwitch, BOOL checked);
    __declspec(dllexport) void __stdcall SetSwitchText(
        HWND hSwitch,
        const unsigned char* active_text_bytes,
        int active_text_len,
        const unsigned char* inactive_text_bytes,
        int inactive_text_len
    );
    __declspec(dllexport) void __stdcall SetSwitchColors(HWND hSwitch, UINT32 active_color, UINT32 inactive_color);
    __declspec(dllexport) void __stdcall SetSwitchTextColors(HWND hSwitch, UINT32 active_text_color, UINT32 inactive_text_color);
    __declspec(dllexport) int __stdcall GetSwitchColors(HWND hSwitch, UINT32* active_color, UINT32* inactive_color, UINT32* active_text_color, UINT32* inactive_text_color);
    __declspec(dllexport) void __stdcall SetSwitchCallback(HWND hSwitch, SwitchCallback callback);
    __declspec(dllexport) void __stdcall EnableSwitch(HWND hSwitch, BOOL enable);
    __declspec(dllexport) void __stdcall ShowSwitch(HWND hSwitch, BOOL show);
    __declspec(dllexport) void __stdcall SetSwitchBounds(HWND hSwitch, int x, int y, int width, int height);

    // ========== Tooltip ==========
    __declspec(dllexport) HWND __stdcall CreateTooltip(
        HWND hOwner,
        const unsigned char* text_bytes,
        int text_len,
        int placement,
        UINT32 bg_color,
        UINT32 fg_color
    );

    __declspec(dllexport) void __stdcall SetTooltipText(
        HWND hTooltip,
        const unsigned char* text_bytes,
        int text_len
    );
    __declspec(dllexport) void __stdcall SetTooltipPlacement(HWND hTooltip, int placement);
    __declspec(dllexport) void __stdcall SetTooltipTheme(HWND hTooltip, int theme_mode);
    __declspec(dllexport) void __stdcall SetTooltipColors(HWND hTooltip, UINT32 bg_color, UINT32 fg_color, UINT32 border_color);
    __declspec(dllexport) void __stdcall SetTooltipFont(HWND hTooltip, const unsigned char* font_bytes, int font_len, float font_size);
    __declspec(dllexport) void __stdcall SetTooltipTrigger(HWND hTooltip, int trigger_mode);
    __declspec(dllexport) void __stdcall BindTooltipToControl(HWND hTooltip, HWND hTarget);
    __declspec(dllexport) void __stdcall ShowTooltipForControl(HWND hTooltip, HWND hTarget);
    __declspec(dllexport) void __stdcall HideTooltip(HWND hTooltip);
    __declspec(dllexport) void __stdcall DestroyTooltip(HWND hTooltip);

    // ========== Notification ==========
    __declspec(dllexport) HWND __stdcall ShowNotification(
        HWND hOwner,
        const unsigned char* title_bytes,
        int title_len,
        const unsigned char* message_bytes,
        int message_len,
        int type,
        int position,
        int duration_ms
    );

    __declspec(dllexport) void __stdcall SetNotificationCallback(HWND hNotification, NotificationCallback callback);
    __declspec(dllexport) void __stdcall CloseNotification(HWND hNotification);

    // ========== 鍒楄〃妗嗗姛鑳?==========

    // 鍒涘缓鍒楄〃妗?
    __declspec(dllexport) HWND __stdcall CreateListBox(
        HWND hParent,
        int x, int y, int width, int height,
        BOOL multi_select,
        UINT32 fg_color,
        UINT32 bg_color
    );

    // 娣诲姞鍒楄〃椤?
    __declspec(dllexport) int __stdcall AddListItem(
        HWND hListBox,
        const unsigned char* text_bytes,
        int text_len
    );

    // 绉婚櫎鍒楄〃椤?
    __declspec(dllexport) void __stdcall RemoveListItem(
        HWND hListBox,
        int index
    );

    // 娓呯┖鍒楄〃妗?
    __declspec(dllexport) void __stdcall ClearListBox(
        HWND hListBox
    );

    // 鑾峰彇閫変腑椤圭储寮?
    __declspec(dllexport) int __stdcall GetSelectedIndex(
        HWND hListBox
    );

    // 璁剧疆閫変腑椤圭储寮?
    __declspec(dllexport) void __stdcall SetSelectedIndex(
        HWND hListBox,
        int index
    );

    // 鑾峰彇鍒楄〃椤规暟閲?
    __declspec(dllexport) int __stdcall GetListItemCount(
        HWND hListBox
    );

    // 鑾峰彇鍒楄〃椤规枃鏈?
    __declspec(dllexport) int __stdcall GetListItemText(
        HWND hListBox,
        int index,
        unsigned char* buffer,
        int buffer_size
    );

    // 璁剧疆鍒楄〃妗嗗洖璋?
    __declspec(dllexport) void __stdcall SetListBoxCallback(
        HWND hListBox,
        ListBoxCallback callback
    );

    // 鍚敤/绂佺敤鍒楄〃妗?
    __declspec(dllexport) void __stdcall EnableListBox(
        HWND hListBox,
        BOOL enable
    );

    // 鏄剧ず/闅愯棌鍒楄〃妗?
    __declspec(dllexport) void __stdcall ShowListBox(
        HWND hListBox,
        BOOL show
    );

    // 璁剧疆鍒楄〃妗嗕綅缃拰澶у皬
    __declspec(dllexport) void __stdcall SetListBoxBounds(
        HWND hListBox,
        int x, int y, int width, int height
    );

    __declspec(dllexport) void __stdcall SetListBoxColors(
        HWND hListBox,
        UINT32 fg_color,
        UINT32 bg_color,
        UINT32 select_color,
        UINT32 hover_color
    );

    __declspec(dllexport) int __stdcall GetListBoxColors(
        HWND hListBox,
        UINT32* fg_color,
        UINT32* bg_color,
        UINT32* select_color,
        UINT32* hover_color
    );

    // 璁剧疆鍒楄〃椤规枃鏈?
    __declspec(dllexport) BOOL __stdcall SetListItemText(
        HWND hListBox,
        int index,
        const unsigned char* text_bytes,
        int text_len
    );

    // ========== 缁勫悎妗嗗姛鑳?==========

    // 鍒涘缓缁勫悎妗嗭紙澧炲己鐗堬級
    __declspec(dllexport) HWND __stdcall CreateComboBox(
        HWND hParent,
        int x, int y, int width, int height,
        BOOL readonly,
        UINT32 fg_color,
        UINT32 bg_color,
        int item_height,                        // 琛ㄩ」楂樺害
        const unsigned char* font_name_bytes,  // 瀛椾綋鍚嶇О
        int font_name_len,
        int font_size,                          // 瀛椾綋澶у皬
        BOOL bold,                              // 绮椾綋
        BOOL italic,                            // 鏂滀綋
        BOOL underline                          // 涓嬪垝绾?
    );

    // 娣诲姞缁勫悎妗嗛」鐩?
    __declspec(dllexport) int __stdcall AddComboItem(
        HWND hComboBox,
        const unsigned char* text_bytes,
        int text_len
    );

    // 绉婚櫎缁勫悎妗嗛」鐩?
    __declspec(dllexport) void __stdcall RemoveComboItem(
        HWND hComboBox,
        int index
    );

    // 娓呯┖缁勫悎妗?
    __declspec(dllexport) void __stdcall ClearComboBox(
        HWND hComboBox
    );

    // 鑾峰彇缁勫悎妗嗛€変腑椤圭储寮?
    __declspec(dllexport) int __stdcall GetComboSelectedIndex(
        HWND hComboBox
    );

    // 璁剧疆缁勫悎妗嗛€変腑椤圭储寮?
    __declspec(dllexport) void __stdcall SetComboSelectedIndex(
        HWND hComboBox,
        int index
    );

    // 鑾峰彇缁勫悎妗嗛」鐩暟閲?
    __declspec(dllexport) int __stdcall GetComboItemCount(
        HWND hComboBox
    );

    // 鑾峰彇缁勫悎妗嗛」鐩枃鏈?
    __declspec(dllexport) int __stdcall GetComboItemText(
        HWND hComboBox,
        int index,
        unsigned char* buffer,
        int buffer_size
    );

    // 璁剧疆缁勫悎妗嗗洖璋?
    __declspec(dllexport) void __stdcall SetComboBoxCallback(
        HWND hComboBox,
        ComboBoxCallback callback
    );

    // 鍚敤/绂佺敤缁勫悎妗?
    __declspec(dllexport) void __stdcall EnableComboBox(
        HWND hComboBox,
        BOOL enable
    );

    // 鏄剧ず/闅愯棌缁勫悎妗?
    __declspec(dllexport) void __stdcall ShowComboBox(
        HWND hComboBox,
        BOOL show
    );

    // 璁剧疆缁勫悎妗嗕綅缃拰澶у皬
    __declspec(dllexport) void __stdcall SetComboBoxBounds(
        HWND hComboBox,
        int x, int y, int width, int height
    );

    // 鑾峰彇缁勫悎妗嗘枃鏈?
    __declspec(dllexport) int __stdcall GetComboBoxText(
        HWND hComboBox,
        unsigned char* buffer,
        int buffer_size
    );

    // 璁剧疆缁勫悎妗嗘枃鏈?
    __declspec(dllexport) void __stdcall SetComboBoxText(
        HWND hComboBox,
        const unsigned char* text_bytes,
        int text_len
    );

    __declspec(dllexport) void __stdcall SetComboBoxColors(
        HWND hComboBox,
        UINT32 fg_color,
        UINT32 bg_color,
        UINT32 select_color,
        UINT32 hover_color
    );

    __declspec(dllexport) int __stdcall GetComboBoxColors(
        HWND hComboBox,
        UINT32* fg_color,
        UINT32* bg_color,
        UINT32* select_color,
        UINT32* hover_color
    );

    // ========== D2D缁勫悎妗嗗姛鑳斤紙鏀寔褰╄壊emoji锛?==========

    // 鍒涘缓D2D缁勫悎妗?
    __declspec(dllexport) HWND __stdcall CreateD2DComboBox(
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
    );

    // 娣诲姞D2D缁勫悎妗嗛」鐩?
    __declspec(dllexport) int __stdcall AddD2DComboItem(
        HWND hComboBox,
        const unsigned char* text_bytes,
        int text_len
    );

    // 绉婚櫎D2D缁勫悎妗嗛」鐩?
    __declspec(dllexport) void __stdcall RemoveD2DComboItem(
        HWND hComboBox,
        int index
    );

    // 娓呯┖D2D缁勫悎妗?
    __declspec(dllexport) void __stdcall ClearD2DComboBox(
        HWND hComboBox
    );

    // 鑾峰彇D2D缁勫悎妗嗛€変腑椤圭储寮?
    __declspec(dllexport) int __stdcall GetD2DComboSelectedIndex(
        HWND hComboBox
    );

    // 璁剧疆D2D缁勫悎妗嗛€変腑椤圭储寮?
    __declspec(dllexport) void __stdcall SetD2DComboSelectedIndex(
        HWND hComboBox,
        int index
    );

    // 鑾峰彇D2D缁勫悎妗嗛」鐩暟閲?
    __declspec(dllexport) int __stdcall GetD2DComboItemCount(
        HWND hComboBox
    );

    // 鑾峰彇D2D缁勫悎妗嗛」鐩枃鏈?
    __declspec(dllexport) int __stdcall GetD2DComboItemText(
        HWND hComboBox,
        int index,
        unsigned char* buffer,
        int buffer_size
    );

    // 鑾峰彇D2D缁勫悎妗嗙紪杈戞鏂囨湰
    __declspec(dllexport) int __stdcall GetD2DComboText(
        HWND hComboBox,
        unsigned char* buffer,
        int buffer_size
    );

    // 鑾峰彇D2D缁勫悎妗嗛€変腑椤规枃鏈?
    __declspec(dllexport) int __stdcall GetD2DComboSelectedText(
        HWND hComboBox,
        unsigned char* buffer,
        int buffer_size
    );

    // 璁剧疆D2D缁勫悎妗嗙紪杈戞鏂囨湰
    __declspec(dllexport) void __stdcall SetD2DComboText(
        HWND hComboBox,
        const unsigned char* text_bytes,
        int text_len
    );

    // 璁剧疆D2D缁勫悎妗嗗洖璋?
    __declspec(dllexport) void __stdcall SetD2DComboBoxCallback(
        HWND hComboBox,
        ComboBoxCallback callback
    );

    // 鍚敤/绂佺敤D2D缁勫悎妗?
    __declspec(dllexport) void __stdcall EnableD2DComboBox(
        HWND hComboBox,
        BOOL enable
    );

    // 鏄剧ず/闅愯棌D2D缁勫悎妗?
    __declspec(dllexport) void __stdcall ShowD2DComboBox(
        HWND hComboBox,
        BOOL show
    );

    // 璁剧疆D2D缁勫悎妗嗕綅缃拰澶у皬
    __declspec(dllexport) void __stdcall SetD2DComboBoxBounds(
        HWND hComboBox,
        int x, int y, int width, int height
    );

    __declspec(dllexport) void __stdcall SetD2DComboBoxColors(
        HWND hComboBox,
        UINT32 fg_color,
        UINT32 bg_color,
        UINT32 select_color,
        UINT32 hover_color,
        UINT32 border_color,
        UINT32 button_color
    );

    __declspec(dllexport) int __stdcall GetD2DComboBoxColors(
        HWND hComboBox,
        UINT32* fg_color,
        UINT32* bg_color,
        UINT32* select_color,
        UINT32* hover_color,
        UINT32* border_color,
        UINT32* button_color
    );

    // ========== D2D 鏃ユ湡鏃堕棿閫夋嫨鍣紙Element 椋庢牸寮瑰嚭灞傦紝鏀寔褰╄壊 emoji 鏄剧ず鏂囨湰锛?==========

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
        BOOL underline
    );

    __declspec(dllexport) int __stdcall GetD2DDateTimePickerPrecision(HWND hPicker);
    __declspec(dllexport) void __stdcall SetD2DDateTimePickerPrecision(HWND hPicker, int precision);

    __declspec(dllexport) void __stdcall GetD2DDateTimePickerDateTime(
        HWND hPicker,
        int* year,
        int* month,
        int* day,
        int* hour,
        int* minute,
        int* second
    );

    __declspec(dllexport) void __stdcall SetD2DDateTimePickerDateTime(
        HWND hPicker,
        int year,
        int month,
        int day,
        int hour,
        int minute,
        int second
    );

    __declspec(dllexport) void __stdcall SetD2DDateTimePickerCallback(
        HWND hPicker,
        ValueChangedCallback callback
    );

    __declspec(dllexport) void __stdcall EnableD2DDateTimePicker(HWND hPicker, BOOL enable);
    __declspec(dllexport) void __stdcall ShowD2DDateTimePicker(HWND hPicker, BOOL show);
    __declspec(dllexport) void __stdcall SetD2DDateTimePickerBounds(
        HWND hPicker,
        int x, int y, int width, int height
    );

    __declspec(dllexport) void __stdcall SetD2DDateTimePickerColors(
        HWND hPicker,
        UINT32 fg_color,
        UINT32 bg_color,
        UINT32 border_color
    );

    __declspec(dllexport) int __stdcall GetD2DDateTimePickerColors(
        HWND hPicker,
        UINT32* fg_color,
        UINT32* bg_color,
        UINT32* border_color
    );


    // ========== 鐑敭鎺т欢鍔熻兘 ==========

    // 鍒涘缓鐑敭鎺т欢
    __declspec(dllexport) HWND __stdcall CreateHotKeyControl(
        HWND hParent,
        int x, int y, int width, int height,
        UINT32 fg_color,
        UINT32 bg_color
    );

    // 鑾峰彇鐑敭
    __declspec(dllexport) void __stdcall GetHotKey(
        HWND hHotKey,
        int* vk_code,
        int* modifiers
    );

    // 璁剧疆鐑敭
    __declspec(dllexport) void __stdcall SetHotKey(
        HWND hHotKey,
        int vk_code,
        int modifiers
    );

    // 娓呴櫎鐑敭
    __declspec(dllexport) void __stdcall ClearHotKey(
        HWND hHotKey
    );

    // 璁剧疆鐑敭鍥炶皟
    __declspec(dllexport) void __stdcall SetHotKeyCallback(
        HWND hHotKey,
        HotKeyCallback callback
    );

    // 鍚敤/绂佺敤鐑敭鎺т欢
    __declspec(dllexport) void __stdcall EnableHotKeyControl(
        HWND hHotKey,
        BOOL enable
    );

    // 鏄剧ず/闅愯棌鐑敭鎺т欢
    __declspec(dllexport) void __stdcall ShowHotKeyControl(
        HWND hHotKey,
        BOOL show
    );

    // 璁剧疆鐑敭鎺т欢浣嶇疆鍜屽ぇ灏?
    __declspec(dllexport) void __stdcall SetHotKeyControlBounds(
        HWND hHotKey,
        int x, int y, int width, int height
    );

    __declspec(dllexport) void __stdcall SetHotKeyColors(
        HWND hHotKey,
        UINT32 fg_color,
        UINT32 bg_color,
        UINT32 border_color
    );

    __declspec(dllexport) int __stdcall GetHotKeyColors(
        HWND hHotKey,
        UINT32* fg_color,
        UINT32* bg_color,
        UINT32* border_color
    );

    // ========== 鍒嗙粍妗嗗姛鑳?==========

    // 鍒涘缓鍒嗙粍妗?
    __declspec(dllexport) HWND __stdcall CreateGroupBox(
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
    );

    // 娣诲姞瀛愭帶浠跺埌鍒嗙粍妗?
    __declspec(dllexport) void __stdcall AddChildToGroup(
        HWND hGroupBox,
        HWND hChild
    );

    // 浠庡垎缁勬绉婚櫎瀛愭帶浠?
    __declspec(dllexport) void __stdcall RemoveChildFromGroup(
        HWND hGroupBox,
        HWND hChild
    );

    // 璁剧疆鍒嗙粍妗嗘爣棰?
    __declspec(dllexport) void __stdcall SetGroupBoxTitle(
        HWND hGroupBox,
        const unsigned char* title_bytes,
        int title_len
    );

    // 鍚敤/绂佺敤鍒嗙粍妗?
    __declspec(dllexport) void __stdcall EnableGroupBox(
        HWND hGroupBox,
        BOOL enable
    );

    // 鏄剧ず/闅愯棌鍒嗙粍妗?
    __declspec(dllexport) void __stdcall ShowGroupBox(
        HWND hGroupBox,
        BOOL show
    );

    // 璁剧疆鍒嗙粍妗嗕綅缃拰澶у皬
    __declspec(dllexport) void __stdcall SetGroupBoxBounds(
        HWND hGroupBox,
        int x, int y, int width, int height
    );

    // 璁剧疆鍒嗙粍妗嗗洖璋?
    __declspec(dllexport) void __stdcall SetGroupBoxCallback(
        HWND hGroupBox,
        GroupBoxCallback callback
    );

    __declspec(dllexport) HWND __stdcall CreatePanel(
        HWND hParent,
        int x, int y, int width, int height,
        UINT32 bg_color
    );

    __declspec(dllexport) void __stdcall SetPanelBackgroundColor(
        HWND hPanel,
        UINT32 bg_color
    );

    __declspec(dllexport) int __stdcall GetPanelBackgroundColor(
        HWND hPanel,
        UINT32* bg_color
    );

    // 鑾峰彇鍒嗙粍妗嗘爣棰?
    __declspec(dllexport) int __stdcall GetGroupBoxTitle(
        HWND hGroupBox,
        unsigned char* buffer,
        int buffer_size
    );

    // 鑾峰彇鍒嗙粍妗嗕綅缃拰澶у皬
    __declspec(dllexport) int __stdcall GetGroupBoxBounds(
        HWND hGroupBox,
        int* x,
        int* y,
        int* width,
        int* height
    );

    // 鑾峰彇鍒嗙粍妗嗗彲瑙嗙姸鎬侊紙1=鍙, 0=涓嶅彲瑙? -1=閿欒锛?
    __declspec(dllexport) int __stdcall GetGroupBoxVisible(
        HWND hGroupBox
    );

    // 鑾峰彇鍒嗙粍妗嗗惎鐢ㄧ姸鎬侊紙1=鍚敤, 0=绂佺敤, -1=閿欒锛?
    __declspec(dllexport) int __stdcall GetGroupBoxEnabled(
        HWND hGroupBox
    );

    // 鑾峰彇鍒嗙粍妗嗛鑹诧紙杈规鑹插拰鑳屾櫙鑹诧紝ARGB鏍煎紡锛?
    __declspec(dllexport) int __stdcall GetGroupBoxColor(
        HWND hGroupBox,
        UINT32* border_color,
        UINT32* bg_color
    );

    __declspec(dllexport) void __stdcall SetGroupBoxTitleColor(
        HWND hGroupBox,
        UINT32 title_color
    );

    __declspec(dllexport) int __stdcall GetGroupBoxTitleColor(
        HWND hGroupBox,
        UINT32* title_color
    );

    __declspec(dllexport) void __stdcall SetGroupBoxStyle(
        HWND hGroupBox,
        int style
    );

    __declspec(dllexport) int __stdcall GetGroupBoxStyle(
        HWND hGroupBox
    );

    // ========== DataGridView 鍔熻兘 ==========

    // 鍒涘缓DataGridView
    __declspec(dllexport) HWND __stdcall CreateDataGridView(
        HWND hParent,
        int x, int y, int width, int height,
        BOOL virtual_mode,
        BOOL zebra_stripe,
        UINT32 fg_color,
        UINT32 bg_color
    );

    // --- 鍒楃鐞?---

    // 娣诲姞鏂囨湰鍒?
    __declspec(dllexport) int __stdcall DataGrid_AddTextColumn(
        HWND hGrid,
        const unsigned char* header_bytes,
        int header_len,
        int width
    );

    // 娣诲姞澶嶉€夋鍒?
    __declspec(dllexport) int __stdcall DataGrid_AddCheckBoxColumn(
        HWND hGrid,
        const unsigned char* header_bytes,
        int header_len,
        int width
    );

    // 娣诲姞鎸夐挳鍒?
    __declspec(dllexport) int __stdcall DataGrid_AddButtonColumn(
        HWND hGrid,
        const unsigned char* header_bytes,
        int header_len,
        int width
    );

    // 娣诲姞閾炬帴鍒?
    __declspec(dllexport) int __stdcall DataGrid_AddLinkColumn(
        HWND hGrid,
        const unsigned char* header_bytes,
        int header_len,
        int width
    );

    // 娣诲姞鍥剧墖鍒?
    __declspec(dllexport) int __stdcall DataGrid_AddImageColumn(
        HWND hGrid,
        const unsigned char* header_bytes,
        int header_len,
        int width
    );

    __declspec(dllexport) int __stdcall DataGrid_AddComboBoxColumn(
        HWND hGrid,
        const unsigned char* header_bytes,
        int header_len,
        int width
    );

    __declspec(dllexport) int __stdcall DataGrid_AddTagColumn(
        HWND hGrid,
        const unsigned char* header_bytes,
        int header_len,
        int width
    );

    __declspec(dllexport) int __stdcall DataGrid_AddProgressColumn(
        HWND hGrid,
        const unsigned char* header_bytes,
        int header_len,
        int width
    );

    // 绉婚櫎鍒?
    __declspec(dllexport) void __stdcall DataGrid_RemoveColumn(
        HWND hGrid,
        int col_index
    );

    // 娓呯┖鎵€鏈夊垪
    __declspec(dllexport) void __stdcall DataGrid_ClearColumns(
        HWND hGrid
    );

    // 鑾峰彇鍒楁暟
    __declspec(dllexport) int __stdcall DataGrid_GetColumnCount(
        HWND hGrid
    );

    // 璁剧疆鍒楀
    __declspec(dllexport) void __stdcall DataGrid_SetColumnWidth(
        HWND hGrid,
        int col_index,
        int width
    );

    // 设置列表头文本
    __declspec(dllexport) void __stdcall DataGrid_SetColumnHeaderText(
        HWND hGrid,
        int col_index,
        const unsigned char* header_bytes,
        int header_len
    );

    // 获取列表头文本
    __declspec(dllexport) int __stdcall DataGrid_GetColumnHeaderText(
        HWND hGrid,
        int col_index,
        unsigned char* buffer,
        int buffer_size
    );

    // --- 琛岀鐞?---

    // 娣诲姞琛?
    __declspec(dllexport) int __stdcall DataGrid_AddRow(
        HWND hGrid
    );

    // 绉婚櫎琛?
    __declspec(dllexport) void __stdcall DataGrid_RemoveRow(
        HWND hGrid,
        int row_index
    );

    // 娓呯┖鎵€鏈夎
    __declspec(dllexport) void __stdcall DataGrid_ClearRows(
        HWND hGrid
    );

    // 鑾峰彇琛屾暟
    __declspec(dllexport) int __stdcall DataGrid_GetRowCount(
        HWND hGrid
    );

    // --- 鍗曞厓鏍兼搷浣?---

    // 璁剧疆鍗曞厓鏍兼枃鏈?
    __declspec(dllexport) void __stdcall DataGrid_SetCellText(
        HWND hGrid,
        int row, int col,
        const unsigned char* text_bytes,
        int text_len
    );

    // 鑾峰彇鍗曞厓鏍兼枃鏈?
    __declspec(dllexport) int __stdcall DataGrid_GetCellText(
        HWND hGrid,
        int row, int col,
        unsigned char* buffer,
        int buffer_size
    );

    // 从文件设置图片单元格
    __declspec(dllexport) BOOL __stdcall DataGrid_SetCellImageFromFile(
        HWND hGrid,
        int row, int col,
        const unsigned char* file_path_bytes,
        int path_len
    );

    // 从内存设置图片单元格
    __declspec(dllexport) BOOL __stdcall DataGrid_SetCellImageFromMemory(
        HWND hGrid,
        int row, int col,
        const unsigned char* image_data,
        int data_len
    );

    // 清除图片单元格
    __declspec(dllexport) void __stdcall DataGrid_ClearCellImage(
        HWND hGrid,
        int row, int col
    );

    // 璁剧疆鍗曞厓鏍煎閫夋鐘舵€?
    __declspec(dllexport) void __stdcall DataGrid_SetCellChecked(
        HWND hGrid,
        int row, int col,
        BOOL checked
    );

    // 鑾峰彇鍗曞厓鏍煎閫夋鐘舵€?
    __declspec(dllexport) BOOL __stdcall DataGrid_GetCellChecked(
        HWND hGrid,
        int row, int col
    );

    __declspec(dllexport) void __stdcall DataGrid_SetCellProgress(
        HWND hGrid,
        int row, int col,
        int progress
    );

    __declspec(dllexport) int __stdcall DataGrid_GetCellProgress(
        HWND hGrid,
        int row, int col
    );

    // 璁剧疆鍗曞厓鏍兼牱寮?
    __declspec(dllexport) void __stdcall DataGrid_SetCellStyle(
        HWND hGrid,
        int row, int col,
        UINT32 fg_color,
        UINT32 bg_color,
        BOOL bold,
        BOOL italic
    );

    // --- 閫夋嫨鍜屽鑸?---

    // 鑾峰彇閫変腑琛?
    __declspec(dllexport) int __stdcall DataGrid_GetSelectedRow(
        HWND hGrid
    );

    // 鑾峰彇閫変腑鍒?
    __declspec(dllexport) int __stdcall DataGrid_GetSelectedCol(
        HWND hGrid
    );

    // 璁剧疆閫変腑鍗曞厓鏍?
    __declspec(dllexport) void __stdcall DataGrid_SetSelectedCell(
        HWND hGrid,
        int row, int col
    );

    // 璁剧疆閫夋嫨妯″紡
    __declspec(dllexport) void __stdcall DataGrid_SetSelectionMode(
        HWND hGrid,
        int mode  // 0=鍗曞厓鏍? 1=鏁磋
    );

    // --- 鎺掑簭 ---

    // 鎸夊垪鎺掑簭
    __declspec(dllexport) void __stdcall DataGrid_SortByColumn(
        HWND hGrid,
        int col_index,
        int sort_order  // 0=鏃? 1=鍗囧簭, 2=闄嶅簭
    );

    // --- 鍐荤粨 ---

    // 璁剧疆鍐荤粨棣栬
    __declspec(dllexport) void __stdcall DataGrid_SetFreezeHeader(
        HWND hGrid,
        BOOL freeze
    );

    // 璁剧疆鍐荤粨棣栧垪
    __declspec(dllexport) void __stdcall DataGrid_SetFreezeFirstColumn(
        HWND hGrid,
        BOOL freeze
    );

    // 璁剧疆鍐荤粨鍓嶯鍒?
    __declspec(dllexport) void __stdcall DataGrid_SetFreezeColumnCount(
        HWND hGrid,
        int count
    );

    // 璁剧疆鍐荤粨鍓嶯琛?
    __declspec(dllexport) void __stdcall DataGrid_SetFreezeRowCount(
        HWND hGrid,
        int count
    );

    // --- 铏氭嫙妯″紡 ---

    // 璁剧疆铏氭嫙妯″紡琛屾暟
    __declspec(dllexport) void __stdcall DataGrid_SetVirtualRowCount(
        HWND hGrid,
        int row_count
    );

    // 璁剧疆铏氭嫙妯″紡鏁版嵁鍥炶皟
    __declspec(dllexport) void __stdcall DataGrid_SetVirtualDataCallback(
        HWND hGrid,
        DataGridVirtualDataCallback callback
    );

    // --- 澶栬 ---

    // 璁剧疆鏄剧ず缃戞牸绾?
    __declspec(dllexport) void __stdcall DataGrid_SetShowGridLines(
        HWND hGrid,
        BOOL show
    );

    // 璁剧疆榛樿琛岄珮
    __declspec(dllexport) void __stdcall DataGrid_SetDefaultRowHeight(
        HWND hGrid,
        int height
    );

    // 璁剧疆鍒楀ご楂樺害
    __declspec(dllexport) void __stdcall DataGrid_SetHeaderHeight(
        HWND hGrid,
        int height
    );

    __declspec(dllexport) void __stdcall DataGrid_SetDoubleClickEnabled(
        HWND hGrid,
        BOOL enabled
    );

    __declspec(dllexport) void __stdcall DataGrid_SetHeaderMultiline(
        HWND hGrid,
        BOOL enabled
    );

    __declspec(dllexport) void __stdcall DataGrid_SetHeaderStyle(
        HWND hGrid,
        int style
    );

    __declspec(dllexport) void __stdcall DataGrid_SetColumnComboItems(
        HWND hGrid,
        int col_index,
        const unsigned char* items_bytes,
        int items_len
    );

    // --- 浜嬩欢鍥炶皟 ---

    // 璁剧疆鍗曞厓鏍肩偣鍑诲洖璋?
    __declspec(dllexport) void __stdcall DataGrid_SetCellClickCallback(
        HWND hGrid,
        DataGridCellClickCallback callback
    );

    // 璁剧疆鍗曞厓鏍煎弻鍑诲洖璋?
    __declspec(dllexport) void __stdcall DataGrid_SetCellDoubleClickCallback(
        HWND hGrid,
        DataGridCellDoubleClickCallback callback
    );

    // 璁剧疆鍗曞厓鏍煎€兼敼鍙樺洖璋?
    __declspec(dllexport) void __stdcall DataGrid_SetCellValueChangedCallback(
        HWND hGrid,
        DataGridCellValueChangedCallback callback
    );

    // 璁剧疆鍒楀ご鐐瑰嚮鍥炶皟
    __declspec(dllexport) void __stdcall DataGrid_SetColumnHeaderClickCallback(
        HWND hGrid,
        DataGridColumnHeaderClickCallback callback
    );

    // 璁剧疆閫夋嫨鏀瑰彉鍥炶皟
    __declspec(dllexport) void __stdcall DataGrid_SetSelectionChangedCallback(
        HWND hGrid,
        DataGridSelectionChangedCallback callback
    );

    // --- 鍏朵粬 ---

    // 鍚敤/绂佺敤DataGridView
    __declspec(dllexport) void __stdcall DataGrid_Enable(
        HWND hGrid,
        BOOL enable
    );

    // 鏄剧ず/闅愯棌DataGridView
    __declspec(dllexport) void __stdcall DataGrid_Show(
        HWND hGrid,
        BOOL show
    );

    // 璁剧疆DataGridView浣嶇疆鍜屽ぇ灏?
    __declspec(dllexport) void __stdcall DataGrid_SetBounds(
        HWND hGrid,
        int x, int y, int width, int height
    );

    // 鍒锋柊DataGridView
    __declspec(dllexport) void __stdcall DataGrid_Refresh(
        HWND hGrid
    );

    __declspec(dllexport) void __stdcall DataGrid_SetColors(
        HWND hGrid,
        UINT32 fg_color,
        UINT32 bg_color,
        UINT32 header_bg_color,
        UINT32 header_fg_color,
        UINT32 select_color,
        UINT32 hover_color,
        UINT32 grid_line_color
    );

    __declspec(dllexport) int __stdcall DataGrid_GetColors(
        HWND hGrid,
        UINT32* fg_color,
        UINT32* bg_color,
        UINT32* header_bg_color,
        UINT32* header_fg_color,
        UINT32* select_color,
        UINT32* hover_color,
        UINT32* grid_line_color
    );

    // 璁剧疆鍒楀ご瀵归綈鏂瑰紡 (0=宸﹀榻? 1=灞呬腑, 2=鍙冲榻?
    __declspec(dllexport) void __stdcall DataGrid_SetColumnHeaderAlignment(
        HWND hGrid,
        int col_index,
        int alignment
    );

    // 璁剧疆鍒楀崟鍏冩牸瀵归綈鏂瑰紡 (0=宸﹀榻? 1=灞呬腑, 2=鍙冲榻?
    __declspec(dllexport) void __stdcall DataGrid_SetColumnCellAlignment(
        HWND hGrid,
        int col_index,
        int alignment
    );

    // 瀵煎嚭CSV
    __declspec(dllexport) BOOL __stdcall DataGrid_ExportCSV(
        HWND hGrid,
        const unsigned char* file_path_bytes,
        int path_len
    );

    // ========== 閫氱敤浜嬩欢鍥炶皟璁剧疆 (闇€姹?8.1-8.10) ==========

    // 璁剧疆榧犳爣杩涘叆鍥炶皟
    __declspec(dllexport) void __stdcall SetMouseEnterCallback(HWND hControl, MouseEnterCallback callback);

    // 璁剧疆榧犳爣绂诲紑鍥炶皟
    __declspec(dllexport) void __stdcall SetMouseLeaveCallback(HWND hControl, MouseLeaveCallback callback);

    // 璁剧疆鍙屽嚮鍥炶皟
    __declspec(dllexport) void __stdcall SetDoubleClickCallback(HWND hControl, DoubleClickCallback callback);

    // 璁剧疆鍙抽敭鐐瑰嚮鍥炶皟
    __declspec(dllexport) void __stdcall SetRightClickCallback(HWND hControl, RightClickCallback callback);

    // 璁剧疆鑾峰緱鐒︾偣鍥炶皟
    __declspec(dllexport) void __stdcall SetFocusCallback(HWND hControl, FocusCallback callback);

    // 璁剧疆澶卞幓鐒︾偣鍥炶皟
    __declspec(dllexport) void __stdcall SetBlurCallback(HWND hControl, BlurCallback callback);

    // 璁剧疆閿洏鎸変笅鍥炶皟
    __declspec(dllexport) void __stdcall SetKeyDownCallback(HWND hControl, KeyDownCallback callback);

    // 璁剧疆閿洏鏉惧紑鍥炶皟
    __declspec(dllexport) void __stdcall SetKeyUpCallback(HWND hControl, KeyUpCallback callback);

    // 璁剧疆瀛楃杈撳叆鍥炶皟
    __declspec(dllexport) void __stdcall SetCharCallback(HWND hControl, CharCallback callback);

    // 璁剧疆鍊兼敼鍙樺洖璋?
    __declspec(dllexport) void __stdcall SetValueChangedCallback(HWND hControl, ValueChangedCallback callback);

    // ========== 甯冨眬绠＄悊鍣ㄥ姛鑳?==========

    // 璁剧疆绐楀彛鐨勫竷灞€绠＄悊鍣?
    // layout_type: 0=鏃? 1=姘村钩娴佸紡, 2=鍨傜洿娴佸紡, 3=缃戞牸, 4=鍋滈潬
    __declspec(dllexport) void __stdcall SetLayoutManager(
        HWND hParent,
        int layout_type,
        int rows,
        int columns,
        int spacing
    );

    // 璁剧疆甯冨眬绠＄悊鍣ㄧ殑鍐呰竟璺?
    __declspec(dllexport) void __stdcall SetLayoutPadding(
        HWND hParent,
        int padding_left,
        int padding_top,
        int padding_right,
        int padding_bottom
    );

    // 璁剧疆鎺т欢鐨勫竷灞€灞炴€?
    __declspec(dllexport) void __stdcall SetControlLayoutProps(
        HWND hControl,
        int margin_left,
        int margin_top,
        int margin_right,
        int margin_bottom,
        int dock_position,
        BOOL stretch_horizontal,
        BOOL stretch_vertical
    );

    // 灏嗘帶浠舵坊鍔犲埌甯冨眬绠＄悊鍣?
    __declspec(dllexport) void __stdcall AddControlToLayout(
        HWND hParent,
        HWND hControl
    );

    // 浠庡竷灞€绠＄悊鍣ㄧЩ闄ゆ帶浠?
    __declspec(dllexport) void __stdcall RemoveControlFromLayout(
        HWND hParent,
        HWND hControl
    );

    // 绔嬪嵆閲嶆柊璁＄畻甯冨眬
    __declspec(dllexport) void __stdcall UpdateLayout(HWND hParent);

    // 绉婚櫎绐楀彛鐨勫竷灞€绠＄悊鍣?
    __declspec(dllexport) void __stdcall RemoveLayoutManager(HWND hParent);

    // ========== 涓婚绯荤粺 API ==========

    // 浠嶫SON瀛楃涓插姞杞戒富棰?
    __declspec(dllexport) BOOL __stdcall LoadThemeFromJSON(
        const unsigned char* json_bytes,
        int json_len
    );

    // 浠庢枃浠跺姞杞戒富棰?
    __declspec(dllexport) BOOL __stdcall LoadThemeFromFile(
        const unsigned char* file_path_bytes,
        int path_len
    );

    // 璁剧疆褰撳墠涓婚 (theme_name: "light" 鎴?"dark" 鎴栬嚜瀹氫箟鍚嶇О)
    __declspec(dllexport) void __stdcall SetTheme(
        const unsigned char* theme_name_bytes,
        int name_len
    );

    // 璁剧疆鏆楄壊妯″紡 (鍒囨崲浜壊/鏆楄壊涓婚)
    __declspec(dllexport) void __stdcall SetDarkMode(BOOL dark_mode);

    // 鑾峰彇涓婚棰滆壊 (color_name: "primary", "success", "warning", "danger", "info",
    //   "text_primary", "text_regular", "text_secondary", "text_placeholder",
    //   "border_base", "border_light", "border_lighter", "border_extra_light",
    //   "background", "background_light")
    __declspec(dllexport) UINT32 __stdcall EW_GetThemeColor(
        const unsigned char* color_name_bytes,
        int name_len
    );

    // 鑾峰彇涓婚瀛椾綋鍚嶇О (font_type: 0=鏍囬, 1=姝ｆ枃, 2=绛夊)
    // 杩斿洖UTF-8瀛楄妭鏁帮紝鍐欏叆buffer
    __declspec(dllexport) int __stdcall EW_GetThemeFont(
        int font_type,
        unsigned char* buffer,
        int buffer_size
    );

    // 鑾峰彇涓婚瀛楀彿 (font_type: 0=鏍囬, 1=姝ｆ枃, 2=灏忓彿)
    __declspec(dllexport) int __stdcall GetThemeFontSize(int font_type);

    // 鑾峰彇涓婚灏哄 (size_type: 0=鍦嗚鍗婂緞, 1=杈规瀹藉害, 2=鎺т欢楂樺害,
    //   3=灏忛棿璺? 4=涓棿璺? 5=澶ч棿璺?
    __declspec(dllexport) int __stdcall GetThemeSize(int size_type);

    // 鑾峰彇褰撳墠鏄惁鏆楄壊妯″紡
    __declspec(dllexport) BOOL __stdcall IsDarkMode();

    // 鑾峰彇褰撳墠涓婚鍚嶇О (杩斿洖UTF-8瀛楄妭鏁帮紝鍐欏叆buffer)
    __declspec(dllexport) int __stdcall EW_GetCurrentThemeName(
        unsigned char* buffer,
        int buffer_size
    );

    // 璁剧疆涓婚鍒囨崲鍥炶皟
    __declspec(dllexport) void __stdcall SetThemeChangedCallback(ThemeChangedCallback callback);
}

// Internal functions
LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
LRESULT CALLBACK MsgBoxProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
LRESULT CALLBACK CheckBoxProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData);
LRESULT CALLBACK ProgressBarProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData);
LRESULT CALLBACK PictureBoxProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData);
LRESULT CALLBACK RadioButtonProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData);
LRESULT CALLBACK SliderProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData);
LRESULT CALLBACK SwitchProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData);
LRESULT CALLBACK ListBoxProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData);
LRESULT CALLBACK ComboBoxProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData);
LRESULT CALLBACK ComboDropDownProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
LRESULT CALLBACK HotKeyProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData);
LRESULT CALLBACK GroupBoxProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData);
LRESULT CALLBACK PanelProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData);
LRESULT CALLBACK DataGridViewProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData);
LRESULT CALLBACK PopupMenuProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
LRESULT CALLBACK TooltipProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData);
LRESULT CALLBACK NotificationProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData);
void DrawButton(ID2D1HwndRenderTarget* rt, IDWriteFactory* factory, const EmojiButton& button);
void DrawPopupMenu(ID2D1HwndRenderTarget* rt, IDWriteFactory* factory, PopupMenuState* popup);
void DrawMsgBox(ID2D1HwndRenderTarget* rt, IDWriteFactory* factory, MsgBoxState* state);
void DrawCheckBox(ID2D1HwndRenderTarget* rt, IDWriteFactory* factory, CheckBoxState* state);
void DrawProgressBar(ID2D1HwndRenderTarget* rt, IDWriteFactory* factory, ProgressBarState* state);
void DrawPictureBox(ID2D1HwndRenderTarget* rt, IDWriteFactory* factory, PictureBoxState* state);
void DrawRadioButton(ID2D1HwndRenderTarget* rt, IDWriteFactory* factory, RadioButtonState* state);
void DrawSlider(ID2D1HwndRenderTarget* rt, IDWriteFactory* factory, SliderState* state);
void DrawSwitch(ID2D1HwndRenderTarget* rt, IDWriteFactory* factory, SwitchState* state);
void DrawListBox(ID2D1HwndRenderTarget* rt, IDWriteFactory* factory, ListBoxState* state);
void DrawComboBox(ID2D1HwndRenderTarget* rt, IDWriteFactory* factory, ComboBoxState* state);
void DrawComboDropDown(ID2D1HwndRenderTarget* rt, IDWriteFactory* factory, ComboBoxState* state);
void DrawHotKey(ID2D1HwndRenderTarget* rt, IDWriteFactory* factory, HotKeyState* state);
void DrawGroupBox(ID2D1HwndRenderTarget* rt, IDWriteFactory* factory, GroupBoxState* state);
void DrawDataGridView(ID2D1HwndRenderTarget* rt, IDWriteFactory* factory, DataGridViewState* state);
std::wstring Utf8ToWide(const unsigned char* bytes, int len);
D2D1_COLOR_F ColorFromUInt32(UINT32 color);
UINT32 LightenColor(UINT32 color, float factor);
UINT32 DarkenColor(UINT32 color, float factor);
EventCallbacks* FindEventCallbacks(HWND hwnd);
HWND CreateMessageBoxWindow(HWND parent, const std::wstring& title, const std::wstring& message,
                            const std::wstring& icon, MsgBoxButtonType type, MessageBoxCallback callback);
void CloseMessageBox(HWND hwnd, bool result);

// TabControl 鍐呴儴杈呭姪鍑芥暟
void UpdateTabLayout(TabControlState* state);
LRESULT CALLBACK TabControlSubclassProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData);

// 甯冨眬绠＄悊鍣ㄥ唴閮ㄥ嚱鏁?
void CalculateFlowLayout(LayoutManager* lm, int client_width, int client_height, HDWP* hdwp);
void CalculateGridLayout(LayoutManager* lm, int client_width, int client_height, HDWP* hdwp);
void CalculateDockLayout(LayoutManager* lm, int client_width, int client_height, HDWP* hdwp);
