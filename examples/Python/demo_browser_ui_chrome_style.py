# -*- coding: utf-8 -*-
from __future__ import annotations

import ctypes
import struct
import sys
from ctypes import wintypes
from dataclasses import dataclass, field
from pathlib import Path
from urllib.parse import urlparse

if hasattr(sys.stdout, "reconfigure"):
    try:
        sys.stdout.reconfigure(encoding="utf-8", errors="replace")
    except OSError:
        pass
if hasattr(sys.stderr, "reconfigure"):
    try:
        sys.stderr.reconfigure(encoding="utf-8", errors="replace")
    except OSError:
        pass

HWND = wintypes.HWND
UINT32 = wintypes.UINT
BOOL = wintypes.BOOL
POINT = wintypes.POINT
RECT = wintypes.RECT
VK_RETURN = 13

ALIGN_LEFT = 0
ALIGN_CENTER = 1
SW_SHOW = 5
SWP_NOSIZE = 0x0001
SWP_NOMOVE = 0x0002
SWP_NOACTIVATE = 0x0010

TOOLBAR_H = 48
STATUS_H = 34
TAB_ITEM_W = 196
TAB_ITEM_H = 38
TAB_HEADER_STYLE_CARD_PLAIN = 2
BUTTON_STYLE_SOLID = 0

ICON_BACK = "<"
ICON_FORWARD = ">"
ICON_RELOAD = "R"
ICON_HOME = "H"
ICON_FAVORITE_OFF = "*"
ICON_FAVORITE_ON = "*"
ICON_THEME_LIGHT = "D"
ICON_THEME_DARK = "L"

MENU_TAB_NEW = 1001
MENU_TAB_CLOSE = 1002
MENU_TAB_CLOSE_OTHERS = 1003
MENU_TAB_ADD_TO_FAVORITES = 1004
MENU_FAVORITE_TOGGLE = 2001
MENU_FAVORITE_HOME = 2002
MENU_FAVORITE_EMPTY = 2003
MENU_FAVORITE_BASE = 2100

FONT_RAW = "Microsoft YaHei UI".encode("utf-8")
FONT_BUF = (ctypes.c_ubyte * len(FONT_RAW))(*FONT_RAW)
FONT_PTR = ctypes.cast(FONT_BUF, ctypes.c_void_p)
FONT_LEN = len(FONT_RAW)


def argb(a: int, r: int, g: int, b: int) -> int:
    return ((a & 255) << 24) | ((r & 255) << 16) | ((g & 255) << 8) | (b & 255)


def utf8_ptr(text: str) -> tuple[ctypes.c_void_p, int, object | None]:
    raw = text.encode("utf-8")
    if not raw:
        return ctypes.c_void_p(), 0, None
    buf = (ctypes.c_ubyte * len(raw))(*raw)
    return ctypes.cast(buf, ctypes.c_void_p), len(raw), buf


def bytes_ptr(data: bytes) -> tuple[ctypes.c_void_p, int, object | None]:
    if not data:
        return ctypes.c_void_p(), 0, None
    buf = (ctypes.c_ubyte * len(data)).from_buffer_copy(data)
    return ctypes.cast(buf, ctypes.c_void_p), len(data), buf


def handle_value(hwnd: HWND | None) -> int:
    if not hwnd:
        return 0
    return int(ctypes.cast(hwnd, ctypes.c_void_p).value or 0)


def hwnd_rect(hwnd: HWND | None) -> RECT:
    rect = RECT()
    if hwnd:
        ctypes.windll.user32.GetWindowRect(hwnd, ctypes.byref(rect))
    return rect


def client_origin_in_parent(hwnd: HWND | None, parent: HWND | None) -> tuple[int, int]:
    if not hwnd or not parent:
        return (0, 0)
    user32 = ctypes.windll.user32
    pt = POINT(0, 0)
    user32.ClientToScreen(hwnd, ctypes.byref(pt))
    user32.ScreenToClient(parent, ctypes.byref(pt))
    return (pt.x, pt.y)


def raise_child(hwnd: HWND | None) -> None:
    if hwnd:
        ctypes.windll.user32.SetWindowPos(hwnd, HWND(), 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE)


def normalize_url(text: str) -> str:
    value = (text or "").strip()
    if not value:
        return "https://www.google.com/"
    if "://" not in value:
        value = f"https://{value}"
    parsed = urlparse(value)
    if not parsed.netloc and parsed.path:
        value = f"https://{parsed.path}"
    return value


def title_from_url(url: str) -> str:
    parsed = urlparse(url)
    host = parsed.netloc or parsed.path or "New Tab"
    path = parsed.path.strip("/")
    return f"{host} / {path.split('/')[0]}" if path else host


def short_url(url: str, limit: int = 72) -> str:
    return url if len(url) <= limit else f"{url[: limit - 3]}..."


def chrome_tab_caption(url: str) -> str:
    host = (urlparse(url).netloc or urlparse(url).path or "New Tab").lower()
    if host.startswith("www."):
        host = host[4:]
    return host if len(host) <= 20 else f"{host[:17]}..."


def repo_root() -> Path:
    return Path(__file__).resolve().parents[2]


def dll_path() -> Path:
    bundled = repo_root() / "bin" / "x64" / "Release" / "emoji_window.dll"
    return bundled if bundled.is_file() else Path(__file__).resolve().parent / "emoji_window.dll"


def browser_icon_path() -> Path:
    return Path(__file__).resolve().parent / "谷歌.ico"


def load_dll() -> ctypes.WinDLL:
    if struct.calcsize("P") * 8 != 64:
        raise OSError("Use 64-bit Python.")
    path = dll_path()
    if not path.is_file():
        raise FileNotFoundError(path)
    return ctypes.WinDLL(str(path))


@dataclass(frozen=True)
class Palette:
    name: str
    titlebar_bg: int
    titlebar_text: int
    window_bg: int
    tab_selected_bg: int
    tab_unselected_bg: int
    tab_selected_text: int
    tab_unselected_text: int
    tab_indicator: int
    toolbar_bg: int
    toolbar_button_bg: int
    toolbar_button_hover_bg: int
    toolbar_button_text: int
    toolbar_button_border: int
    address_bg: int
    address_text: int
    page_bg: int
    card_bg: int
    card_alt_bg: int
    text_primary: int
    text_secondary: int
    accent: int
    status_bg: int
    status_text: int
    progress_fg: int
    progress_bg: int


LIGHT = Palette(
    "Light",
    argb(255, 240, 243, 247),
    argb(255, 32, 33, 36),
    argb(255, 244, 246, 249),
    argb(255, 252, 253, 255),
    argb(255, 240, 243, 247),
    argb(255, 32, 33, 36),
    argb(255, 118, 123, 130),
    argb(255, 132, 157, 214),
    argb(255, 252, 253, 255),
    argb(255, 248, 249, 251),
    argb(255, 234, 238, 243),
    argb(255, 66, 74, 83),
    argb(255, 232, 236, 242),
    argb(255, 255, 255, 255),
    argb(255, 32, 33, 36),
    argb(255, 246, 248, 252),
    argb(255, 255, 255, 255),
    argb(255, 248, 250, 252),
    argb(255, 32, 33, 36),
    argb(255, 95, 99, 104),
    argb(255, 66, 133, 244),
    argb(255, 241, 243, 244),
    argb(255, 95, 99, 104),
    argb(255, 66, 133, 244),
    argb(255, 218, 220, 224),
)


DARK = Palette(
    "Dark",
    argb(255, 34, 37, 41),
    argb(255, 243, 246, 251),
    argb(255, 24, 27, 31),
    argb(255, 56, 61, 68),
    argb(255, 34, 37, 41),
    argb(255, 243, 246, 251),
    argb(255, 149, 158, 172),
    argb(255, 148, 188, 252),
    argb(255, 56, 61, 68),
    argb(255, 56, 61, 68),
    argb(255, 66, 72, 80),
    argb(255, 229, 233, 240),
    argb(255, 66, 72, 80),
    argb(255, 47, 52, 59),
    argb(255, 243, 246, 251),
    argb(255, 24, 27, 31),
    argb(255, 35, 39, 46),
    argb(255, 30, 33, 39),
    argb(255, 243, 246, 251),
    argb(255, 170, 177, 188),
    argb(255, 138, 180, 248),
    argb(255, 30, 33, 39),
    argb(255, 170, 177, 188),
    argb(255, 138, 180, 248),
    argb(255, 61, 67, 76),
)


@dataclass
class TabUi:
    content_hwnd: HWND
    page_card: HWND | None = None
    page_title: HWND | None = None
    page_url: HWND | None = None
    page_status: HWND | None = None
    info_card: HWND | None = None
    info_title: HWND | None = None
    info_line1: HWND | None = None
    info_line2: HWND | None = None
    info_line3: HWND | None = None
    info_line4: HWND | None = None
    history: list[str] = field(default_factory=list)
    history_index: int = -1
    home_url: str = "https://www.google.com/"

    @property
    def current_url(self) -> str:
        return self.history[self.history_index] if 0 <= self.history_index < len(self.history) else self.home_url


class BrowserUiApp:
    def __init__(self) -> None:
        self.dll = load_dll()
        self.user32 = ctypes.windll.user32
        self._bind_api()
        self.palette = LIGHT
        self.width = 1440
        self.height = 920
        self.window: HWND | None = None
        self.tab_control: HWND | None = None
        self.toolbar_panel: HWND | None = None
        self.toolbar_bg: HWND | None = None
        self.toolbar_address_shell: HWND | None = None
        self.toolbar_address_badge: HWND | None = None
        self.toolbar_address_hwnd: HWND | None = None
        self.toolbar_btn_back = 0
        self.toolbar_btn_forward = 0
        self.toolbar_btn_reload = 0
        self.toolbar_btn_home = 0
        self.toolbar_btn_favorite = 0
        self.toolbar_btn_theme = 0
        self.toolbar_favorite_menu: HWND | None = None
        self.status_bg: HWND | None = None
        self.status_label: HWND | None = None
        self.progress_bar: HWND | None = None
        self.tab_popup: HWND | None = None
        self.tab_popup_target = -1
        self.keepalive: list[object] = []
        self.tabs_by_content: dict[int, TabUi] = {}
        self.button_actions: dict[tuple[int, int], object] = {}
        self.favorites: list[str] = [
            "https://www.google.com/",
            "https://news.google.com/",
            "https://github.com/",
            "https://stackoverflow.com/",
        ]
        self._is_building = False
        self._suspend_tab_changed = False

    def _bind_api(self) -> None:
        dll = self.dll
        self.ButtonCallback = ctypes.WINFUNCTYPE(None, ctypes.c_int, HWND)
        self.EditKeyCallback = ctypes.WINFUNCTYPE(None, HWND, ctypes.c_int, ctypes.c_int, ctypes.c_int, ctypes.c_int, ctypes.c_int)
        self.TabCallback = ctypes.WINFUNCTYPE(None, HWND, ctypes.c_int)
        self.TabCloseCallback = ctypes.WINFUNCTYPE(None, HWND, ctypes.c_int)
        self.TabRightClickCallback = ctypes.WINFUNCTYPE(None, HWND, ctypes.c_int, ctypes.c_int, ctypes.c_int)
        self.TabNewButtonCallback = ctypes.WINFUNCTYPE(None, HWND)
        self.MenuCallback = ctypes.WINFUNCTYPE(None, ctypes.c_int, ctypes.c_int)
        self.WindowResizeCallback = ctypes.WINFUNCTYPE(None, HWND, ctypes.c_int, ctypes.c_int)
        self.WindowCloseCallback = ctypes.WINFUNCTYPE(None, HWND)

        dll.create_window_bytes_ex.argtypes = [ctypes.c_void_p, ctypes.c_int, ctypes.c_int, ctypes.c_int, ctypes.c_int, ctypes.c_int, UINT32, UINT32]
        dll.create_window_bytes_ex.restype = HWND
        dll.set_message_loop_main_window.argtypes = [HWND]
        dll.run_message_loop.argtypes = []
        dll.run_message_loop.restype = ctypes.c_int
        dll.set_window_title.argtypes = [HWND, ctypes.c_void_p, ctypes.c_int]
        dll.set_window_icon_bytes.argtypes = [HWND, ctypes.c_void_p, ctypes.c_int]
        dll.set_window_titlebar_color.argtypes = [HWND, UINT32]
        dll.SetWindowBackgroundColor.argtypes = [HWND, UINT32]
        dll.SetTitleBarTextColor.argtypes = [HWND, UINT32]
        dll.SetWindowResizeCallback.argtypes = [self.WindowResizeCallback]
        dll.SetWindowCloseCallback.argtypes = [self.WindowCloseCallback]
        dll.SetDarkMode.argtypes = [ctypes.c_int]

        dll.create_emoji_button_bytes.argtypes = [HWND, ctypes.c_void_p, ctypes.c_int, ctypes.c_void_p, ctypes.c_int, ctypes.c_int, ctypes.c_int, ctypes.c_int, ctypes.c_int, UINT32]
        dll.create_emoji_button_bytes.restype = ctypes.c_int
        dll.set_button_click_callback.argtypes = [self.ButtonCallback]
        dll.SetButtonText.argtypes = [ctypes.c_int, ctypes.c_void_p, ctypes.c_int]
        dll.SetButtonBounds.argtypes = [ctypes.c_int, ctypes.c_int, ctypes.c_int, ctypes.c_int, ctypes.c_int]
        dll.SetButtonBackgroundColor.argtypes = [ctypes.c_int, UINT32]
        dll.SetButtonTextColor.argtypes = [ctypes.c_int, UINT32]
        dll.SetButtonBorderColor.argtypes = [ctypes.c_int, UINT32]
        dll.SetButtonHoverColors.argtypes = [ctypes.c_int, UINT32, UINT32, UINT32]
        dll.SetButtonRound.argtypes = [ctypes.c_int, ctypes.c_int]
        dll.SetButtonStyle.argtypes = [ctypes.c_int, ctypes.c_int]
        dll.EnableButton.argtypes = [HWND, ctypes.c_int, ctypes.c_int]
        dll.ShowButton.argtypes = [ctypes.c_int, ctypes.c_int]

        dll.CreateLabel.argtypes = [HWND, ctypes.c_int, ctypes.c_int, ctypes.c_int, ctypes.c_int, ctypes.c_void_p, ctypes.c_int, UINT32, UINT32, ctypes.c_void_p, ctypes.c_int, ctypes.c_int, ctypes.c_int, ctypes.c_int, ctypes.c_int, ctypes.c_int, ctypes.c_int]
        dll.CreateLabel.restype = HWND
        dll.SetLabelText.argtypes = [HWND, ctypes.c_void_p, ctypes.c_int]
        dll.SetLabelColor.argtypes = [HWND, UINT32, UINT32]
        dll.SetLabelBounds.argtypes = [HWND, ctypes.c_int, ctypes.c_int, ctypes.c_int, ctypes.c_int]

        dll.CreatePanel.argtypes = [HWND, ctypes.c_int, ctypes.c_int, ctypes.c_int, ctypes.c_int, UINT32]
        dll.CreatePanel.restype = HWND
        dll.SetPanelBackgroundColor.argtypes = [HWND, UINT32]

        dll.CreateEditBox.argtypes = [HWND, ctypes.c_int, ctypes.c_int, ctypes.c_int, ctypes.c_int, ctypes.c_void_p, ctypes.c_int, UINT32, UINT32, ctypes.c_void_p, ctypes.c_int, ctypes.c_int, ctypes.c_int, ctypes.c_int, ctypes.c_int, ctypes.c_int, ctypes.c_int, ctypes.c_int, ctypes.c_int, ctypes.c_int, ctypes.c_int]
        dll.CreateEditBox.restype = HWND
        dll.CreateD2DColorEmojiEditBox.argtypes = list(dll.CreateEditBox.argtypes)
        dll.CreateD2DColorEmojiEditBox.restype = HWND
        dll.GetEditBoxText.argtypes = [HWND, ctypes.c_void_p, ctypes.c_int]
        dll.GetEditBoxText.restype = ctypes.c_int
        dll.SetEditBoxText.argtypes = [HWND, ctypes.c_void_p, ctypes.c_int]
        dll.SetEditBoxColor.argtypes = [HWND, UINT32, UINT32]
        dll.SetEditBoxBounds.argtypes = [HWND, ctypes.c_int, ctypes.c_int, ctypes.c_int, ctypes.c_int]
        dll.SetEditBoxKeyCallback.argtypes = [HWND, self.EditKeyCallback]
        dll.ShowEditBox.argtypes = [HWND, ctypes.c_int]

        dll.CreateProgressBar.argtypes = [HWND, ctypes.c_int, ctypes.c_int, ctypes.c_int, ctypes.c_int, ctypes.c_int, UINT32, UINT32, ctypes.c_int, UINT32]
        dll.CreateProgressBar.restype = HWND
        dll.SetProgressValue.argtypes = [HWND, ctypes.c_int]
        dll.SetProgressBarColor.argtypes = [HWND, UINT32, UINT32]
        dll.SetProgressBarTextColor.argtypes = [HWND, UINT32]
        dll.SetProgressBarBounds.argtypes = [HWND, ctypes.c_int, ctypes.c_int, ctypes.c_int, ctypes.c_int]

        dll.CreateTabControl.argtypes = [HWND, ctypes.c_int, ctypes.c_int, ctypes.c_int, ctypes.c_int]
        dll.CreateTabControl.restype = HWND
        dll.AddTabItem.argtypes = [HWND, ctypes.c_void_p, ctypes.c_int, HWND]
        dll.AddTabItem.restype = ctypes.c_int
        dll.RemoveTabItem.argtypes = [HWND, ctypes.c_int]
        dll.SetTabCallback.argtypes = [HWND, self.TabCallback]
        dll.SetTabCloseCallback.argtypes = [HWND, self.TabCloseCallback]
        dll.SetTabRightClickCallback.argtypes = [HWND, self.TabRightClickCallback]
        dll.SetTabNewButtonCallback.argtypes = [HWND, self.TabNewButtonCallback]
        dll.GetCurrentTabIndex.argtypes = [HWND]
        dll.GetCurrentTabIndex.restype = ctypes.c_int
        dll.SelectTab.argtypes = [HWND, ctypes.c_int]
        dll.SelectTabImmediate.argtypes = [HWND, ctypes.c_int]
        dll.GetTabCount.argtypes = [HWND]
        dll.GetTabCount.restype = ctypes.c_int
        dll.GetTabContentWindow.argtypes = [HWND, ctypes.c_int]
        dll.GetTabContentWindow.restype = HWND
        dll.SetTabItemSize.argtypes = [HWND, ctypes.c_int, ctypes.c_int]
        dll.SetTabFont.argtypes = [HWND, ctypes.c_void_p, ctypes.c_int, ctypes.c_float]
        dll.SetTabColors.argtypes = [HWND, UINT32, UINT32, UINT32, UINT32]
        dll.SetTabIndicatorColor.argtypes = [HWND, UINT32]
        dll.SetTabPadding.argtypes = [HWND, ctypes.c_int, ctypes.c_int]
        dll.SetTabClosable.argtypes = [HWND, ctypes.c_int]
        dll.SetTabDraggable.argtypes = [HWND, ctypes.c_int]
        dll.SetTabScrollable.argtypes = [HWND, ctypes.c_int]
        dll.SetTabHeaderStyle.argtypes = [HWND, ctypes.c_int]
        dll.SetTabControlBounds.argtypes = [HWND, ctypes.c_int, ctypes.c_int, ctypes.c_int, ctypes.c_int]
        dll.SetTabTitle.argtypes = [HWND, ctypes.c_int, ctypes.c_void_p, ctypes.c_int]
        dll.SetTabContentBgColorAll.argtypes = [HWND, UINT32]
        dll.UpdateTabControlLayout.argtypes = [HWND]
        dll.RedrawTabControl.argtypes = [HWND]

        dll.CreateEmojiPopupMenu.argtypes = [HWND]
        dll.CreateEmojiPopupMenu.restype = HWND
        dll.PopupMenuAddItem.argtypes = [HWND, ctypes.c_void_p, ctypes.c_int, ctypes.c_int]
        dll.SetPopupMenuCallback.argtypes = [HWND, self.MenuCallback]
        dll.BindButtonMenu.argtypes = [HWND, ctypes.c_int, HWND]
        dll.ShowContextMenu.argtypes = [HWND, ctypes.c_int, ctypes.c_int]

        self.user32.MoveWindow.argtypes = [HWND, ctypes.c_int, ctypes.c_int, ctypes.c_int, ctypes.c_int, BOOL]
        self.user32.MoveWindow.restype = BOOL
        self.user32.ShowWindow.argtypes = [HWND, ctypes.c_int]
        self.user32.ShowWindow.restype = BOOL

    def run(self) -> int:
        self._build()
        self.dll.set_message_loop_main_window(self.window)
        return int(self.dll.run_message_loop())

    def _build(self) -> None:
        self._is_building = True
        title_ptr, title_len, _ = utf8_ptr("Chrome Style Browser UI")
        self.window = self.dll.create_window_bytes_ex(title_ptr, title_len, -1, -1, self.width, self.height, self.palette.titlebar_bg, self.palette.window_bg)
        if not self.window:
            raise RuntimeError("Failed to create main window.")

        self._apply_window_icon()

        button_cb = self.ButtonCallback(self._on_button_click)
        edit_cb = self.EditKeyCallback(self._on_edit_key)
        tab_cb = self.TabCallback(self._on_tab_changed)
        tab_close_cb = self.TabCloseCallback(self._on_tab_close)
        tab_rc_cb = self.TabRightClickCallback(self._on_tab_right_click)
        tab_new_cb = self.TabNewButtonCallback(self._on_tab_new_button)
        menu_cb = self.MenuCallback(self._on_menu_click)
        resize_cb = self.WindowResizeCallback(self._on_window_resize)
        close_cb = self.WindowCloseCallback(self._on_window_close)
        self.keepalive.extend([button_cb, edit_cb, tab_cb, tab_close_cb, tab_rc_cb, tab_new_cb, menu_cb, resize_cb, close_cb])

        self.dll.set_button_click_callback(button_cb)
        self.dll.SetWindowResizeCallback(resize_cb)
        self.dll.SetWindowCloseCallback(close_cb)

        self.tab_control = self.dll.CreateTabControl(self.window, 0, 0, self.width, self.height - STATUS_H)
        self.dll.SetTabHeaderStyle(self.tab_control, TAB_HEADER_STYLE_CARD_PLAIN)
        self.dll.SetTabItemSize(self.tab_control, TAB_ITEM_W, TAB_ITEM_H)
        self.dll.SetTabPadding(self.tab_control, 24, 10)
        self.dll.SetTabClosable(self.tab_control, 1)
        self.dll.SetTabDraggable(self.tab_control, 1)
        self.dll.SetTabScrollable(self.tab_control, 1)
        self.dll.SetTabFont(self.tab_control, FONT_PTR, FONT_LEN, ctypes.c_float(12.0))
        self.dll.SetTabCallback(self.tab_control, tab_cb)
        self.dll.SetTabCloseCallback(self.tab_control, tab_close_cb)
        self.dll.SetTabRightClickCallback(self.tab_control, tab_rc_cb)
        self.dll.SetTabContentBgColorAll(self.tab_control, self.palette.page_bg)

        self.status_bg = self._create_label(self.window, 0, self.height - STATUS_H, self.width, STATUS_H, "", self.palette.status_text, self.palette.status_bg)
        self.status_label = self._create_label(self.window, 16, self.height - STATUS_H + 6, self.width - 180, 20, "", self.palette.status_text, self.palette.status_bg, font_size=12)
        self.progress_bar = self.dll.CreateProgressBar(self.window, self.width - 160, self.height - STATUS_H + 9, 132, 14, 100, self.palette.progress_fg, self.palette.progress_bg, 0, self.palette.status_bg)

        self.tab_popup = self.dll.CreateEmojiPopupMenu(self.window)
        self.dll.SetPopupMenuCallback(self.tab_popup, menu_cb)
        self._menu_add(self.tab_popup, "New Tab", MENU_TAB_NEW)
        self._menu_add(self.tab_popup, "Close Tab", MENU_TAB_CLOSE)
        self._menu_add(self.tab_popup, "Close Other Tabs", MENU_TAB_CLOSE_OTHERS)
        self._menu_add(self.tab_popup, "Add To Favorites", MENU_TAB_ADD_TO_FAVORITES)

        self.add_tab("https://www.google.com/", select=True)
        self._create_shared_toolbar(edit_cb)
        self.dll.SetTabNewButtonCallback(self.tab_control, tab_new_cb)
        self._rebuild_toolbar_favorite_menu()
        self._apply_theme()
        self._layout_all()
        self._sync_current_tab_ui()
        self._is_building = False

    def _apply_window_icon(self) -> None:
        if not self.window:
            return
        icon = browser_icon_path()
        if not icon.is_file():
            return
        data = icon.read_bytes()
        ptr, size, _ = bytes_ptr(data)
        self.dll.set_window_icon_bytes(self.window, ptr, size)

    def _create_label(self, parent: HWND, x: int, y: int, w: int, h: int, text: str, fg: int, bg: int, *, font_size: int = 13, align: int = ALIGN_LEFT, transparent: int = 0) -> HWND:
        ptr, size, _ = utf8_ptr(text)
        return self.dll.CreateLabel(parent, x, y, w, h, ptr, size, fg, bg, FONT_PTR, FONT_LEN, font_size, 0, 0, 0, align, transparent)

    def _create_edit(self, parent: HWND, x: int, y: int, w: int, h: int, text: str) -> HWND:
        ptr, size, _ = utf8_ptr(text)
        hwnd = self.dll.CreateD2DColorEmojiEditBox(parent, x, y, w, h, ptr, size, self.palette.address_text, self.palette.address_bg, FONT_PTR, FONT_LEN, 13, 0, 0, 0, ALIGN_LEFT, 0, 0, 0, 1, 1)
        self.user32.ShowWindow(hwnd, SW_SHOW)
        return hwnd

    def _create_button(self, parent: HWND, emoji: str, text: str, x: int, y: int, w: int, h: int, action) -> int:
        label = text or emoji
        ep, en, _ = utf8_ptr("")
        tp, tn, _ = utf8_ptr(label)
        button_id = int(self.dll.create_emoji_button_bytes(parent, ep, en, tp, tn, x, y, w, h, self.palette.toolbar_button_bg))
        self.button_actions[(handle_value(parent), button_id)] = action
        return button_id

    def _create_shared_toolbar(self, edit_cb) -> None:
        if not self.window:
            return
        x, y, w, h = self._toolbar_geometry()
        self.toolbar_panel = self.dll.CreatePanel(self.window, x, y, w, h, self.palette.toolbar_bg)
        self.toolbar_bg = None
        self.toolbar_address_shell = self._create_label(self.window, x + 160, y + 7, max(200, w - 244), 34, "", self.palette.toolbar_bg, self.palette.toolbar_bg)
        self.toolbar_address_badge = self._create_label(self.window, x + 172, y + 9, 28, 28, "G", argb(255, 255, 255, 255), self.palette.accent, font_size=12, align=ALIGN_CENTER)
        self.toolbar_address_hwnd = self._create_edit(self.window, x + 208, y + 10, max(200, w - 280), 28, "https://www.google.com/")
        self.dll.SetEditBoxKeyCallback(self.toolbar_address_hwnd, edit_cb)

        self.toolbar_btn_back = self._create_button(self.window, ICON_BACK, "", x + 8, y + 8, 32, 32, self.go_back)
        self.toolbar_btn_forward = self._create_button(self.window, ICON_FORWARD, "", x + 44, y + 8, 32, 32, self.go_forward)
        self.toolbar_btn_reload = self._create_button(self.window, ICON_RELOAD, "", x + 80, y + 8, 32, 32, self.reload_tab)
        self.toolbar_btn_home = self._create_button(self.window, ICON_HOME, "", x + 116, y + 8, 32, 32, lambda: self.navigate_tab(self.current_tab(), self.current_tab().home_url if self.current_tab() else "https://www.google.com/"))
        self.toolbar_btn_favorite = self._create_button(self.window, ICON_FAVORITE_OFF, "", x + max(0, w - 80), y + 8, 32, 32, self.toggle_favorite)
        self.toolbar_btn_theme = self._create_button(self.window, ICON_THEME_DARK, "", x + max(0, w - 44), y + 8, 32, 32, self.toggle_theme)
        self._layout_toolbar()

    def _toolbar_geometry(self) -> tuple[int, int, int, int]:
        current = self.current_tab()
        if self.window and current:
            origin_x, origin_y = client_origin_in_parent(current.content_hwnd, self.window)
            rect = hwnd_rect(current.content_hwnd)
            content_w = max(420, rect.right - rect.left)
            return (origin_x + 12, origin_y + 8, max(420, content_w - 24), TOOLBAR_H)
        return (12, 84, max(420, self.width - 24), TOOLBAR_H)

    def _layout_toolbar(self) -> None:
        if not self.toolbar_panel or not self.window:
            return
        x, y, w, h = self._toolbar_geometry()
        self.user32.MoveWindow(self.toolbar_panel, x, y, w, h, False)

        left_x = 8
        for button_id in (self.toolbar_btn_back, self.toolbar_btn_forward, self.toolbar_btn_reload, self.toolbar_btn_home):
            self.dll.SetButtonBounds(button_id, x + left_x, y + 8, 32, 32)
            self.dll.ShowButton(button_id, 1)
            left_x += 36

        right_theme_x = w - 44
        right_fav_x = w - 80
        self.dll.SetButtonBounds(self.toolbar_btn_favorite, x + right_fav_x, y + 8, 32, 32)
        self.dll.SetButtonBounds(self.toolbar_btn_theme, x + right_theme_x, y + 8, 32, 32)
        self.dll.ShowButton(self.toolbar_btn_favorite, 1)
        self.dll.ShowButton(self.toolbar_btn_theme, 1)

        address_x = left_x + 8
        address_w = max(220, right_fav_x - address_x - 12)
        if self.toolbar_address_shell:
            self.dll.SetLabelBounds(self.toolbar_address_shell, x + address_x, y + 7, address_w, 34)
        if self.toolbar_address_badge:
            self.dll.SetLabelBounds(self.toolbar_address_badge, x + address_x + 10, y + 10, 26, 26)
        if self.toolbar_address_hwnd:
            self.dll.SetEditBoxBounds(self.toolbar_address_hwnd, x + address_x + 42, y + 10, max(140, address_w - 52), 28)
            self.dll.ShowEditBox(self.toolbar_address_hwnd, 1)
            self.user32.ShowWindow(self.toolbar_address_hwnd, SW_SHOW)

        raise_child(self.toolbar_panel)
        raise_child(self.toolbar_address_shell)
        raise_child(self.toolbar_address_badge)
        raise_child(self.toolbar_address_hwnd)

    def _menu_add(self, menu: HWND | None, text: str, item_id: int) -> None:
        if not menu:
            return
        ptr, size, _ = utf8_ptr(text)
        self.dll.PopupMenuAddItem(menu, ptr, size, item_id)

    def _set_label_text(self, hwnd: HWND | None, text: str) -> None:
        if hwnd:
            ptr, size, _ = utf8_ptr(text)
            self.dll.SetLabelText(hwnd, ptr, size)

    def _set_edit_text(self, hwnd: HWND | None, text: str) -> None:
        if hwnd:
            ptr, size, _ = utf8_ptr(text)
            self.dll.SetEditBoxText(hwnd, ptr, size)
            self.user32.ShowWindow(hwnd, SW_SHOW)
            raise_child(hwnd)

    def _get_edit_text(self, hwnd: HWND | None) -> str:
        if not hwnd:
            return ""
        buf = ctypes.create_string_buffer(4096)
        size = int(self.dll.GetEditBoxText(hwnd, buf, len(buf)))
        if size <= 0:
            return ""
        return buf.raw[:size].decode("utf-8", errors="replace")

    def _set_button_text(self, button_id: int, text: str) -> None:
        ptr, size, _ = utf8_ptr(text)
        self.dll.SetButtonText(button_id, ptr, size)

    def _style_icon_button(self, button_id: int, *, active: bool = False, enabled: bool = True) -> None:
        if not self.window or not button_id:
            return
        bg = self.palette.accent if active else self.palette.toolbar_button_bg
        fg = argb(255, 255, 255, 255) if active else self.palette.toolbar_button_text
        border = bg if active else self.palette.toolbar_bg
        hover_bg = self.palette.accent if active else self.palette.toolbar_button_hover_bg
        self.dll.SetButtonStyle(button_id, BUTTON_STYLE_SOLID)
        self.dll.SetButtonRound(button_id, 1)
        self.dll.SetButtonBackgroundColor(button_id, bg)
        self.dll.SetButtonTextColor(button_id, fg if enabled else self.palette.text_secondary)
        self.dll.SetButtonBorderColor(button_id, border)
        self.dll.SetButtonHoverColors(button_id, hover_bg, border, fg)
        self.dll.EnableButton(self.window, button_id, 1 if enabled else 0)

    def current_tab(self) -> TabUi | None:
        if not self.tab_control:
            return None
        index = int(self.dll.GetCurrentTabIndex(self.tab_control))
        return self.tab_at_index(index)

    def tab_at_index(self, index: int) -> TabUi | None:
        if not self.tab_control or index < 0 or index >= int(self.dll.GetTabCount(self.tab_control)):
            return None
        content = self.dll.GetTabContentWindow(self.tab_control, index)
        return self.tabs_by_content.get(handle_value(content))

    def index_of_tab(self, tab: TabUi | None) -> int:
        if not self.tab_control or not tab:
            return -1
        count = int(self.dll.GetTabCount(self.tab_control))
        for index in range(count):
            content = self.dll.GetTabContentWindow(self.tab_control, index)
            if handle_value(content) == handle_value(tab.content_hwnd):
                return index
        return -1

    def add_tab(self, url: str | None = None, *, select: bool = True, sync_selected: bool = True) -> TabUi | None:
        if not self.tab_control:
            return None
        initial_url = normalize_url(url or "https://www.google.com/")
        title_ptr, title_len, _ = utf8_ptr(chrome_tab_caption(initial_url))
        index = int(self.dll.AddTabItem(self.tab_control, title_ptr, title_len, HWND()))
        content = self.dll.GetTabContentWindow(self.tab_control, index)
        tab = TabUi(content)
        self.tabs_by_content[handle_value(content)] = tab
        tab.history = [initial_url]
        tab.history_index = 0
        if select:
            self._suspend_tab_changed = True
            self.dll.SelectTabImmediate(self.tab_control, index)
            self._suspend_tab_changed = False
        if select and sync_selected and not self._is_building:
            self._sync_current_tab_ui()
        return tab

    def _ensure_tab_views(self, tab: TabUi | None) -> None:
        if not tab or tab.page_card:
            return
        content = tab.content_hwnd
        tab.page_card = self._create_label(content, 32, 84, 860, 132, "", self.palette.text_primary, self.palette.card_bg)
        tab.page_title = self._create_label(content, 56, 104, 760, 34, "", self.palette.text_primary, self.palette.card_bg, font_size=22)
        tab.page_url = self._create_label(content, 56, 146, 760, 24, "", self.palette.text_secondary, self.palette.card_bg, font_size=12)
        tab.page_status = self._create_label(content, 56, 178, 760, 22, "", self.palette.text_secondary, self.palette.card_bg, font_size=12)
        tab.info_card = self._create_label(content, 32, 236, 860, 184, "", self.palette.text_primary, self.palette.card_alt_bg)
        tab.info_title = self._create_label(content, 56, 256, 520, 28, "Tab Session", self.palette.text_primary, self.palette.card_alt_bg, font_size=16)
        tab.info_line1 = self._create_label(content, 56, 296, 760, 22, "", self.palette.text_secondary, self.palette.card_alt_bg, font_size=12)
        tab.info_line2 = self._create_label(content, 56, 326, 760, 22, "", self.palette.text_secondary, self.palette.card_alt_bg, font_size=12)
        tab.info_line3 = self._create_label(content, 56, 356, 760, 22, "", self.palette.text_secondary, self.palette.card_alt_bg, font_size=12)
        tab.info_line4 = self._create_label(content, 56, 386, 760, 22, "", self.palette.text_secondary, self.palette.card_alt_bg, font_size=12)
        self._apply_theme_to_tab(tab)

    def close_tab(self, index: int) -> None:
        if not self.tab_control:
            return
        count = int(self.dll.GetTabCount(self.tab_control))
        if count <= 1 or index < 0 or index >= count:
            self._set_status("At least one tab must remain.")
            return
        tab = self.tab_at_index(index)
        if tab:
            self.tabs_by_content.pop(handle_value(tab.content_hwnd), None)
        self.dll.RemoveTabItem(self.tab_control, index)
        self._layout_all()
        self._sync_current_tab_ui()

    def close_other_tabs(self, index: int) -> None:
        if not self.tab_control:
            return
        count = int(self.dll.GetTabCount(self.tab_control))
        if index < 0 or index >= count:
            return
        for current in range(count - 1, -1, -1):
            if current == index:
                continue
            tab = self.tab_at_index(current)
            if tab:
                self.tabs_by_content.pop(handle_value(tab.content_hwnd), None)
            self.dll.RemoveTabItem(self.tab_control, current)
        self.dll.SelectTab(self.tab_control, 0)
        self._layout_all()
        self._sync_current_tab_ui()

    def navigate_tab(self, tab: TabUi | None, url: str, *, add_history: bool = True, update_status: bool = True) -> None:
        if not tab:
            return
        normalized = normalize_url(url)
        if add_history:
            if 0 <= tab.history_index < len(tab.history) - 1:
                tab.history = tab.history[: tab.history_index + 1]
            tab.history.append(normalized)
            tab.history_index = len(tab.history) - 1
        elif tab.history:
            tab.history[tab.history_index] = normalized
        else:
            tab.history = [normalized]
            tab.history_index = 0
        self._render_tab(tab)
        if tab is self.current_tab():
            self._sync_current_tab_ui()
        if update_status:
            self._set_status(f"Loaded {normalized}")

    def reload_tab(self) -> None:
        tab = self.current_tab()
        if not tab:
            return
        self._render_tab(tab)
        self._sync_current_tab_ui()
        self._set_status(f"Reloaded {tab.current_url}")

    def go_back(self) -> None:
        tab = self.current_tab()
        if not tab or tab.history_index <= 0:
            return
        tab.history_index -= 1
        self._render_tab(tab)
        self._sync_current_tab_ui()
        self._set_status(f"Back to {tab.current_url}")

    def go_forward(self) -> None:
        tab = self.current_tab()
        if not tab or tab.history_index >= len(tab.history) - 1:
            return
        tab.history_index += 1
        self._render_tab(tab)
        self._sync_current_tab_ui()
        self._set_status(f"Forward to {tab.current_url}")

    def toggle_favorite(self) -> None:
        tab = self.current_tab()
        if not tab:
            return
        url = tab.current_url
        if url in self.favorites:
            self.favorites = [item for item in self.favorites if item != url]
            self._set_status(f"Removed favorite: {url}")
        else:
            self.favorites.append(url)
            self._set_status(f"Added favorite: {url}")
        self._rebuild_toolbar_favorite_menu()
        self._sync_toolbar()
        self._render_tab(tab)

    def _set_status(self, text: str) -> None:
        self._set_label_text(self.status_label, text)
        if self.progress_bar:
            self.dll.SetProgressValue(self.progress_bar, 100)

    def _render_tab(self, tab: TabUi) -> None:
        self._ensure_tab_views(tab)
        url = tab.current_url
        bookmarked = url in self.favorites
        self._set_label_text(tab.page_title, title_from_url(url))
        self._set_label_text(tab.page_url, short_url(url, 88))
        self._set_label_text(tab.page_status, f"{'Bookmarked' if bookmarked else 'Not bookmarked'}  |  Theme {self.palette.name}")
        self._set_label_text(tab.info_line1, f"Current URL: {url}")
        self._set_label_text(tab.info_line2, f"History: {tab.history_index + 1}/{len(tab.history)}")
        self._set_label_text(tab.info_line3, f"Home: {tab.home_url}")
        self._set_label_text(tab.info_line4, f"Favorites: {len(self.favorites)} saved")
        index = self.index_of_tab(tab)
        if self.tab_control and index >= 0:
            ptr, size, _ = utf8_ptr(chrome_tab_caption(url))
            self.dll.SetTabTitle(self.tab_control, index, ptr, size)

    def _apply_theme(self) -> None:
        if not self.window or not self.tab_control:
            return
        self.dll.SetDarkMode(1 if self.palette is DARK else 0)
        self.dll.set_window_titlebar_color(self.window, self.palette.titlebar_bg)
        self.dll.SetWindowBackgroundColor(self.window, self.palette.window_bg)
        self.dll.SetTitleBarTextColor(self.window, self.palette.titlebar_text)
        self.dll.SetTabColors(self.tab_control, self.palette.tab_selected_bg, self.palette.tab_unselected_bg, self.palette.tab_selected_text, self.palette.tab_unselected_text)
        self.dll.SetTabIndicatorColor(self.tab_control, self.palette.tab_indicator)
        self.dll.SetTabContentBgColorAll(self.tab_control, self.palette.page_bg)
        if self.toolbar_panel:
            self.dll.SetPanelBackgroundColor(self.toolbar_panel, self.palette.toolbar_bg)
        if self.toolbar_bg:
            self.dll.SetLabelColor(self.toolbar_bg, self.palette.toolbar_bg, self.palette.toolbar_bg)
        if self.toolbar_address_shell:
            self.dll.SetLabelColor(self.toolbar_address_shell, self.palette.address_text, self.palette.address_bg)
        if self.toolbar_address_hwnd:
            self.dll.SetEditBoxColor(self.toolbar_address_hwnd, self.palette.address_text, self.palette.address_bg)
        if self.status_bg:
            self.dll.SetLabelColor(self.status_bg, self.palette.status_bg, self.palette.status_bg)
        if self.status_label:
            self.dll.SetLabelColor(self.status_label, self.palette.status_text, self.palette.status_bg)
        if self.progress_bar:
            self.dll.SetProgressBarColor(self.progress_bar, self.palette.progress_fg, self.palette.progress_bg)
            self.dll.SetProgressBarTextColor(self.progress_bar, self.palette.status_text)
        for tab in list(self.tabs_by_content.values()):
            self._apply_theme_to_tab(tab)
            if tab.page_card:
                self._render_tab(tab)
        self._sync_toolbar()
        self._layout_all()

    def _apply_theme_to_tab(self, tab: TabUi) -> None:
        if not tab.page_card:
            return
        self.dll.SetLabelColor(tab.page_card, self.palette.card_bg, self.palette.card_bg)
        self.dll.SetLabelColor(tab.page_title, self.palette.text_primary, self.palette.card_bg)
        self.dll.SetLabelColor(tab.page_url, self.palette.text_secondary, self.palette.card_bg)
        self.dll.SetLabelColor(tab.page_status, self.palette.text_secondary, self.palette.card_bg)
        self.dll.SetLabelColor(tab.info_card, self.palette.card_alt_bg, self.palette.card_alt_bg)
        self.dll.SetLabelColor(tab.info_title, self.palette.text_primary, self.palette.card_alt_bg)
        for line in (tab.info_line1, tab.info_line2, tab.info_line3, tab.info_line4):
            self.dll.SetLabelColor(line, self.palette.text_secondary, self.palette.card_alt_bg)

    def _sync_toolbar(self) -> None:
        tab = self.current_tab()
        if not tab:
            return
        self._set_edit_text(self.toolbar_address_hwnd, tab.current_url)
        host = urlparse(tab.current_url).netloc or "site"
        badge = host[0].upper() if host else "S"
        self._set_label_text(self.toolbar_address_badge, badge)
        if self.toolbar_address_badge:
            self.dll.SetLabelColor(self.toolbar_address_badge, argb(255, 255, 255, 255), self._site_color_argb(tab.current_url))
        self._set_button_text(self.toolbar_btn_favorite, ICON_FAVORITE_ON if tab.current_url in self.favorites else ICON_FAVORITE_OFF)
        self._set_button_text(self.toolbar_btn_theme, ICON_THEME_LIGHT if self.palette is DARK else ICON_THEME_DARK)
        self._style_icon_button(self.toolbar_btn_back, enabled=tab.history_index > 0)
        self._style_icon_button(self.toolbar_btn_forward, enabled=tab.history_index < len(tab.history) - 1)
        self._style_icon_button(self.toolbar_btn_reload)
        self._style_icon_button(self.toolbar_btn_home)
        self._style_icon_button(self.toolbar_btn_favorite, active=tab.current_url in self.favorites)
        self._style_icon_button(self.toolbar_btn_theme)
        title_ptr, title_len, _ = utf8_ptr(f"{title_from_url(tab.current_url)} - Chrome Style Browser UI")
        self.dll.set_window_title(self.window, title_ptr, title_len)

    def _sync_current_tab_ui(self) -> None:
        tab = self.current_tab()
        if not tab:
            return
        self._render_tab(tab)
        self._sync_toolbar()
        self._layout_toolbar()
        self._layout_tab(tab)

    def _rebuild_toolbar_favorite_menu(self) -> None:
        if not self.window or not self.toolbar_btn_favorite:
            return
        self.toolbar_favorite_menu = self.dll.CreateEmojiPopupMenu(self.window)
        menu_cb = self.MenuCallback(self._on_menu_click)
        self.keepalive.append(menu_cb)
        self.dll.SetPopupMenuCallback(self.toolbar_favorite_menu, menu_cb)
        self._menu_add(self.toolbar_favorite_menu, "Toggle Current Favorite", MENU_FAVORITE_TOGGLE)
        self._menu_add(self.toolbar_favorite_menu, "Open Home", MENU_FAVORITE_HOME)
        if self.favorites:
            for index, url in enumerate(self.favorites[:30]):
                self._menu_add(self.toolbar_favorite_menu, short_url(url, 46), MENU_FAVORITE_BASE + index)
        else:
            self._menu_add(self.toolbar_favorite_menu, "No Favorites Yet", MENU_FAVORITE_EMPTY)
        self.dll.BindButtonMenu(self.window, self.toolbar_btn_favorite, self.toolbar_favorite_menu)

    def _layout_all(self) -> None:
        if not self.window or not self.tab_control:
            return
        self.dll.SetTabControlBounds(self.tab_control, 0, 0, self.width, max(100, self.height - STATUS_H))
        self.dll.UpdateTabControlLayout(self.tab_control)
        if self.status_bg:
            self.dll.SetLabelBounds(self.status_bg, 0, self.height - STATUS_H, self.width, STATUS_H)
        if self.status_label:
            self.dll.SetLabelBounds(self.status_label, 16, self.height - STATUS_H + 6, max(160, self.width - 190), 20)
        if self.progress_bar:
            self.dll.SetProgressBarBounds(self.progress_bar, max(16, self.width - 160), self.height - STATUS_H + 9, 132, 14)
        self._layout_toolbar()
        current = self.current_tab()
        if current:
            self._layout_tab(current)
        raise_child(self.status_bg)
        raise_child(self.status_label)
        raise_child(self.progress_bar)

    def _layout_tab(self, tab: TabUi) -> None:
        self._ensure_tab_views(tab)
        rect = hwnd_rect(tab.content_hwnd)
        width = max(640, rect.right - rect.left)
        page_width = min(width - 64, 960)
        self.dll.SetLabelBounds(tab.page_card, 32, 84, page_width, 132)
        self.dll.SetLabelBounds(tab.page_title, 56, 104, page_width - 48, 34)
        self.dll.SetLabelBounds(tab.page_url, 56, 146, page_width - 48, 24)
        self.dll.SetLabelBounds(tab.page_status, 56, 178, page_width - 48, 22)
        self.dll.SetLabelBounds(tab.info_card, 32, 236, page_width, 184)
        self.dll.SetLabelBounds(tab.info_title, 56, 256, page_width - 48, 28)
        self.dll.SetLabelBounds(tab.info_line1, 56, 296, page_width - 48, 22)
        self.dll.SetLabelBounds(tab.info_line2, 56, 326, page_width - 48, 22)
        self.dll.SetLabelBounds(tab.info_line3, 56, 356, page_width - 48, 22)
        self.dll.SetLabelBounds(tab.info_line4, 56, 386, page_width - 48, 22)

    def _invoke_current_tab(self, action) -> None:
        tab = self.current_tab()
        if tab:
            action(tab)

    def _site_color_argb(self, url: str) -> int:
        host = urlparse(url).netloc or urlparse(url).path or "site"
        seed = sum(ord(ch) for ch in host)
        colors = [
            argb(255, 66, 133, 244),
            argb(255, 52, 168, 83),
            argb(255, 251, 188, 5),
            argb(255, 234, 67, 53),
            argb(255, 120, 94, 240),
            argb(255, 0, 172, 193),
        ]
        return colors[seed % len(colors)]

    def _on_button_click(self, button_id: int, parent: HWND) -> None:
        action = self.button_actions.get((handle_value(parent), int(button_id)))
        if action:
            action()

    def _on_edit_key(self, hwnd: HWND, _msg: int, key_code: int, _scan: int, _flags: int, _mods: int) -> None:
        if key_code == VK_RETURN:
            self.navigate_tab(self.current_tab(), self._get_edit_text(hwnd))

    def _on_tab_changed(self, _hwnd: HWND, _index: int) -> None:
        if not self._is_building and not self._suspend_tab_changed:
            self._sync_current_tab_ui()

    def _on_tab_close(self, _hwnd: HWND, index: int) -> None:
        if not self._is_building:
            self.close_tab(index)

    def _on_tab_right_click(self, _hwnd: HWND, index: int, x: int, y: int) -> None:
        if self.tab_popup:
            self.tab_popup_target = index
            self.dll.ShowContextMenu(self.tab_popup, x, y)

    def _on_tab_new_button(self, _hwnd: HWND) -> None:
        if self._is_building:
            return
        self.add_tab("https://www.google.com/", select=True, sync_selected=False)
        self._set_status("Created new tab.")

    def _on_menu_click(self, _menu_id: int, item_id: int) -> None:
        if item_id == MENU_TAB_NEW:
            self._on_tab_new_button(HWND())
        elif item_id == MENU_TAB_CLOSE:
            self.close_tab(self.tab_popup_target)
        elif item_id == MENU_TAB_CLOSE_OTHERS:
            self.close_other_tabs(self.tab_popup_target)
        elif item_id == MENU_TAB_ADD_TO_FAVORITES:
            tab = self.tab_at_index(self.tab_popup_target)
            if tab and tab.current_url not in self.favorites:
                self.favorites.append(tab.current_url)
                self._rebuild_toolbar_favorite_menu()
                self._render_tab(tab)
                self._sync_toolbar()
                self._set_status(f"Added favorite: {tab.current_url}")
        elif item_id == MENU_FAVORITE_TOGGLE:
            self.toggle_favorite()
        elif item_id == MENU_FAVORITE_HOME:
            self.navigate_tab(self.current_tab(), "https://www.google.com/")
        elif item_id >= MENU_FAVORITE_BASE:
            fav_index = item_id - MENU_FAVORITE_BASE
            if 0 <= fav_index < len(self.favorites):
                self.navigate_tab(self.current_tab(), self.favorites[fav_index])

    def _on_window_resize(self, hwnd: HWND, width: int, height: int) -> None:
        if handle_value(hwnd) != handle_value(self.window):
            return
        self.width = max(920, width)
        self.height = max(640, height)
        if not self._is_building:
            self._layout_all()

    def _on_window_close(self, _hwnd: HWND) -> None:
        return

    def toggle_theme(self) -> None:
        self.palette = DARK if self.palette is LIGHT else LIGHT
        self._apply_theme()
        self._sync_current_tab_ui()
        self._set_status(f"Switched to {self.palette.name} theme.")


def main() -> int:
    return BrowserUiApp().run()


if __name__ == "__main__":
    raise SystemExit(main())
