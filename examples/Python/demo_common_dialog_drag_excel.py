# -*- coding: utf-8 -*-
"""
GUI demo for the new common dialog, file-drop, and Excel/table APIs.

Run from examples/Python or the repository root:
    python examples/Python/demo_common_dialog_drag_excel.py
"""

from __future__ import annotations

import ctypes
import platform
import sys
from pathlib import Path


SCRIPT_DIR = Path(__file__).resolve().parent
REPO_ROOT = SCRIPT_DIR.parent.parent


def u(text: str) -> bytes:
    return text.encode("utf-8")


def argb(a: int, r: int, g: int, b: int) -> int:
    return ((a & 0xFF) << 24) | ((r & 0xFF) << 16) | ((g & 0xFF) << 8) | (b & 0xFF)


def find_dll() -> Path:
    is_64bit = ctypes.sizeof(ctypes.c_void_p) == 8
    candidates = []
    if is_64bit:
        candidates.extend(
            [
                REPO_ROOT / "bin" / "x64" / "Release" / "emoji_window.dll",
                REPO_ROOT / "DLL" / "emoji_window_x64.dll",
                REPO_ROOT / "emoji_window.dll",
                SCRIPT_DIR / "emoji_window.dll",
            ]
        )
    else:
        candidates.extend(
            [
                REPO_ROOT / "bin" / "Win32" / "Release" / "emoji_window.dll",
                REPO_ROOT / "DLL" / "emoji_window_x86.dll",
                SCRIPT_DIR / "emoji_window.dll",
            ]
        )

    for path in candidates:
        if path.exists():
            return path
    raise FileNotFoundError("emoji_window.dll not found. Build Release DLLs first.")


DLL_PATH = find_dll()
DLL = ctypes.WinDLL(str(DLL_PATH))

HWND = ctypes.c_void_p
UINT32 = ctypes.c_uint32
BOOL = ctypes.c_int

FONT = u("Microsoft YaHei UI")

COLOR_PRIMARY = argb(255, 64, 158, 255)
COLOR_SUCCESS = argb(255, 103, 194, 58)
COLOR_WARNING = argb(255, 230, 162, 60)
COLOR_DANGER = argb(255, 245, 108, 108)
COLOR_GRAY = argb(255, 144, 147, 153)
COLOR_TEXT = argb(255, 48, 49, 51)
COLOR_BG = argb(255, 248, 250, 252)
COLOR_PANEL = argb(255, 245, 247, 250)


def setup_prototypes() -> None:
    DLL.create_window_bytes.argtypes = [
        ctypes.c_char_p,
        ctypes.c_int,
        ctypes.c_int,
        ctypes.c_int,
        ctypes.c_int,
        ctypes.c_int,
    ]
    DLL.create_window_bytes.restype = HWND

    DLL.set_message_loop_main_window.argtypes = [HWND]
    DLL.set_message_loop_main_window.restype = None
    DLL.run_message_loop.argtypes = []
    DLL.run_message_loop.restype = ctypes.c_int
    DLL.SetWindowBackgroundColor.argtypes = [HWND, UINT32]
    DLL.SetWindowBackgroundColor.restype = None

    DLL.create_emoji_button_bytes.argtypes = [
        HWND,
        ctypes.c_char_p,
        ctypes.c_int,
        ctypes.c_char_p,
        ctypes.c_int,
        ctypes.c_int,
        ctypes.c_int,
        ctypes.c_int,
        ctypes.c_int,
        UINT32,
    ]
    DLL.create_emoji_button_bytes.restype = ctypes.c_int

    DLL.CreateLabel.argtypes = [
        HWND,
        ctypes.c_int,
        ctypes.c_int,
        ctypes.c_int,
        ctypes.c_int,
        ctypes.c_char_p,
        ctypes.c_int,
        UINT32,
        UINT32,
        ctypes.c_char_p,
        ctypes.c_int,
        ctypes.c_int,
        BOOL,
        BOOL,
        BOOL,
        ctypes.c_int,
        BOOL,
    ]
    DLL.CreateLabel.restype = HWND
    DLL.SetLabelText.argtypes = [HWND, ctypes.c_char_p, ctypes.c_int]
    DLL.SetLabelText.restype = None

    DLL.CreateDataGridView.argtypes = [
        HWND,
        ctypes.c_int,
        ctypes.c_int,
        ctypes.c_int,
        ctypes.c_int,
        BOOL,
        BOOL,
        UINT32,
        UINT32,
    ]
    DLL.CreateDataGridView.restype = HWND
    DLL.DataGrid_SetDefaultRowHeight.argtypes = [HWND, ctypes.c_int]
    DLL.DataGrid_SetHeaderHeight.argtypes = [HWND, ctypes.c_int]
    DLL.DataGrid_SetShowGridLines.argtypes = [HWND, BOOL]
    DLL.DataGrid_SetFreezeHeader.argtypes = [HWND, BOOL]
    DLL.DataGrid_GetRowCount.argtypes = [HWND]
    DLL.DataGrid_GetRowCount.restype = ctypes.c_int
    DLL.DataGrid_GetColumnCount.argtypes = [HWND]
    DLL.DataGrid_GetColumnCount.restype = ctypes.c_int
    DLL.DataGrid_ExportCSV.argtypes = [HWND, ctypes.c_char_p, ctypes.c_int]
    DLL.DataGrid_ExportCSV.restype = BOOL

    DLL.ShowOpenFileDialog.argtypes = [
        HWND,
        ctypes.c_char_p,
        ctypes.c_int,
        ctypes.c_char_p,
        ctypes.c_int,
        ctypes.c_char_p,
        ctypes.c_int,
        BOOL,
        ctypes.c_void_p,
        ctypes.c_int,
    ]
    DLL.ShowOpenFileDialog.restype = ctypes.c_int
    DLL.ShowSaveFileDialog.argtypes = [
        HWND,
        ctypes.c_char_p,
        ctypes.c_int,
        ctypes.c_char_p,
        ctypes.c_int,
        ctypes.c_char_p,
        ctypes.c_int,
        ctypes.c_char_p,
        ctypes.c_int,
        ctypes.c_void_p,
        ctypes.c_int,
    ]
    DLL.ShowSaveFileDialog.restype = ctypes.c_int
    DLL.ShowSelectFolderDialog.argtypes = [
        HWND,
        ctypes.c_char_p,
        ctypes.c_int,
        ctypes.c_char_p,
        ctypes.c_int,
        ctypes.c_void_p,
        ctypes.c_int,
    ]
    DLL.ShowSelectFolderDialog.restype = ctypes.c_int
    DLL.ShowColorDialog.argtypes = [HWND, UINT32, ctypes.POINTER(UINT32)]
    DLL.ShowColorDialog.restype = BOOL

    DLL.EnableFileDrop.argtypes = [HWND, BOOL]
    DLL.EnableFileDrop.restype = BOOL
    DLL.SetFileDropCallback.argtypes = [HWND, FILE_DROP_CALLBACK]
    DLL.SetFileDropCallback.restype = None

    DLL.Excel_OpenWorkbook.argtypes = [ctypes.c_char_p, ctypes.c_int, ctypes.c_int]
    DLL.Excel_OpenWorkbook.restype = ctypes.c_int
    DLL.Excel_CloseWorkbook.argtypes = [ctypes.c_int]
    DLL.Excel_CloseWorkbook.restype = None
    DLL.Excel_GetRowCount.argtypes = [ctypes.c_int]
    DLL.Excel_GetRowCount.restype = ctypes.c_int
    DLL.Excel_GetColumnCount.argtypes = [ctypes.c_int]
    DLL.Excel_GetColumnCount.restype = ctypes.c_int
    DLL.Excel_GetCellText.argtypes = [
        ctypes.c_int,
        ctypes.c_int,
        ctypes.c_int,
        ctypes.c_void_p,
        ctypes.c_int,
    ]
    DLL.Excel_GetCellText.restype = ctypes.c_int
    DLL.Excel_LoadWorkbookToDataGrid.argtypes = [ctypes.c_int, HWND, BOOL]
    DLL.Excel_LoadWorkbookToDataGrid.restype = BOOL
    DLL.Excel_ReadFileToDataGrid.argtypes = [
        ctypes.c_char_p,
        ctypes.c_int,
        HWND,
        ctypes.c_int,
        BOOL,
    ]
    DLL.Excel_ReadFileToDataGrid.restype = BOOL

    DLL.set_button_click_callback.argtypes = [BUTTON_CALLBACK]
    DLL.set_button_click_callback.restype = None


BUTTON_CALLBACK = ctypes.WINFUNCTYPE(None, ctypes.c_int, HWND)
FILE_DROP_CALLBACK = ctypes.WINFUNCTYPE(
    None, HWND, ctypes.c_void_p, ctypes.c_int, ctypes.c_int, ctypes.c_int, ctypes.c_int, ctypes.c_int
)


main_window = HWND()
grid = HWND()
label_status = HWND()
label_info = HWND()

btn_open_file = 0
btn_save_file = 0
btn_pick_folder = 0
btn_pick_color = 0
btn_load_sample = 0

current_bg_color = COLOR_BG


def make_sample_csv() -> Path:
    path = SCRIPT_DIR / "_new_api_dialog_excel_sample.csv"
    path.write_text(
        "\n".join(
            [
                "名称,数量,状态,备注",
                "苹果,12,已入库,UTF-8 CSV 示例",
                "香蕉,8,待确认,可拖拽 CSV/XLS/XLSX 到窗口",
                "表情行,3,正常,emoji 😊",
            ]
        ),
        encoding="utf-8-sig",
    )
    return path


def set_status(text: str) -> None:
    raw = u(text)
    if label_status:
        DLL.SetLabelText(label_status, raw, len(raw))
    print(text)


def set_info(text: str) -> None:
    raw = u(text)
    if label_info:
        DLL.SetLabelText(label_info, raw, len(raw))


def dialog_result_to_text(result_len: int, buffer: ctypes.Array[ctypes.c_char]) -> str:
    if result_len <= 0:
        return ""
    return buffer.raw[:result_len].decode("utf-8", errors="replace")


def call_path_dialog(kind: str) -> str:
    buffer = ctypes.create_string_buffer(32768)
    title = u(
        {
            "open": "选择要读取的表格文件",
            "save": "导出当前表格为 CSV",
            "folder": "选择一个文件夹",
        }[kind]
    )

    if kind == "open":
        file_filter = u("表格文件|*.xlsx;*.xls;*.csv;*.tsv;*.txt|所有文件|*.*")
        result = DLL.ShowOpenFileDialog(
            main_window,
            title,
            len(title),
            file_filter,
            len(file_filter),
            None,
            0,
            0,
            buffer,
            len(buffer),
        )
    elif kind == "save":
        file_filter = u("CSV 文件|*.csv|所有文件|*.*")
        default_ext = u("csv")
        result = DLL.ShowSaveFileDialog(
            main_window,
            title,
            len(title),
            file_filter,
            len(file_filter),
            None,
            0,
            default_ext,
            len(default_ext),
            buffer,
            len(buffer),
        )
    else:
        result = DLL.ShowSelectFolderDialog(
            main_window,
            title,
            len(title),
            None,
            0,
            buffer,
            len(buffer),
        )

    if result < 0:
        set_status(f"缓冲区不足，需要 {-result} 字节")
        return ""
    return dialog_result_to_text(result, buffer)


def read_excel_cell_text(handle: int, row: int, col: int) -> str:
    needed = DLL.Excel_GetCellText(handle, row, col, None, 0)
    if needed <= 0:
        return ""
    buf = ctypes.create_string_buffer(needed)
    written = DLL.Excel_GetCellText(handle, row, col, buf, needed)
    if written <= 0:
        return ""
    return buf.raw[:written].decode("utf-8", errors="replace")


def load_table_file(path_text: str, source: str) -> None:
    if not path_text:
        set_status("没有选择文件")
        return

    first_path = path_text.splitlines()[0]
    path = Path(first_path)
    if not path.exists():
        set_status(f"文件不存在: {first_path}")
        return

    raw_path = str(path).encode("utf-8")
    handle = DLL.Excel_OpenWorkbook(raw_path, len(raw_path), 1)
    if handle <= 0:
        set_status(f"读取失败: {path.name}，错误码 {handle}。XLS/XLSX 需要本机安装 Excel。")
        return

    try:
        rows = DLL.Excel_GetRowCount(handle)
        cols = DLL.Excel_GetColumnCount(handle)
        first_cell = read_excel_cell_text(handle, 1, 1)

        if not DLL.Excel_LoadWorkbookToDataGrid(handle, grid, 1):
            set_status(f"文件已读取，但加载到表格失败: {path.name}")
            return

        set_status(f"{source}: 已加载 {path.name}，{rows} 行 x {cols} 列")
        set_info(
            "当前文件: "
            f"{path}\n"
            f"首个单元格: {first_cell or '(空)'}\n"
            "可继续拖拽 CSV/XLS/XLSX/TXT 到窗口或表格区域。"
        )
    finally:
        DLL.Excel_CloseWorkbook(handle)


def export_grid_to_csv(path_text: str) -> None:
    if not path_text:
        set_status("已取消保存")
        return

    path = Path(path_text)
    raw_path = str(path).encode("utf-8")
    ok = DLL.DataGrid_ExportCSV(grid, raw_path, len(raw_path))
    if ok:
        rows = DLL.DataGrid_GetRowCount(grid)
        cols = DLL.DataGrid_GetColumnCount(grid)
        set_status(f"已导出 CSV: {path.name}，{rows} 行 x {cols} 列")
        set_info(f"导出路径:\n{path}")
    else:
        set_status(f"导出失败: {path}")


def on_button_click(button_id: int, parent_hwnd: HWND) -> None:
    global current_bg_color
    try:
        if button_id == btn_open_file:
            selected = call_path_dialog("open")
            if selected:
                load_table_file(selected, "打开文件对话框")
            else:
                set_status("已取消打开文件")

        elif button_id == btn_save_file:
            export_grid_to_csv(call_path_dialog("save"))

        elif button_id == btn_pick_folder:
            folder = call_path_dialog("folder")
            if folder:
                set_status(f"已选择文件夹: {folder}")
                set_info(f"选择文件夹结果:\n{folder}")
            else:
                set_status("已取消选择文件夹")

        elif button_id == btn_pick_color:
            selected = UINT32(current_bg_color)
            if DLL.ShowColorDialog(main_window, current_bg_color, ctypes.byref(selected)):
                current_bg_color = int(selected.value)
                DLL.SetWindowBackgroundColor(main_window, current_bg_color)
                set_status(f"已切换窗口背景色: 0x{current_bg_color:08X}")
            else:
                set_status("已取消选择颜色")

        elif button_id == btn_load_sample:
            sample = make_sample_csv()
            load_table_file(str(sample), "样例 CSV")
    except Exception as exc:
        set_status(f"按钮回调异常: {exc}")


def on_file_drop(
    hwnd: HWND,
    file_path_ptr: ctypes.c_void_p,
    path_len: int,
    file_index: int,
    file_count: int,
    x: int,
    y: int,
) -> None:
    try:
        path = ctypes.string_at(file_path_ptr, path_len).decode("utf-8", errors="replace")
        set_status(f"拖拽文件 {file_index + 1}/{file_count}: {path}")
        if file_index == 0:
            load_table_file(path, f"拖拽({x},{y})")
    except Exception as exc:
        set_status(f"拖拽回调异常: {exc}")


button_callback_ref = BUTTON_CALLBACK(on_button_click)
file_drop_callback_ref = FILE_DROP_CALLBACK(on_file_drop)


def create_label(parent: HWND, text: str, x: int, y: int, w: int, h: int, size: int = 13, wrap: int = 0) -> HWND:
    raw = u(text)
    return DLL.CreateLabel(
        parent,
        x,
        y,
        w,
        h,
        raw,
        len(raw),
        COLOR_TEXT,
        COLOR_PANEL,
        FONT,
        len(FONT),
        size,
        0,
        0,
        0,
        0,
        wrap,
    )


def create_button(parent: HWND, emoji: str, text: str, x: int, y: int, w: int, color: int) -> int:
    emoji_raw = u(emoji)
    text_raw = u(text)
    return DLL.create_emoji_button_bytes(
        parent,
        emoji_raw,
        len(emoji_raw),
        text_raw,
        len(text_raw),
        x,
        y,
        w,
        38,
        color,
    )


def create_ui() -> None:
    global main_window, grid, label_status, label_info
    global btn_open_file, btn_save_file, btn_pick_folder, btn_pick_color, btn_load_sample

    title = u("新增功能测试 - 通用对话框 / 拖拽 / Excel读取")
    main_window = DLL.create_window_bytes(title, len(title), -1, -1, 1080, 720)
    if not main_window:
        raise RuntimeError("create_window_bytes failed")

    DLL.SetWindowBackgroundColor(main_window, COLOR_BG)

    label_status = create_label(
        main_window,
        "就绪：点击按钮测试通用对话框；也可以把 CSV/XLS/XLSX/TXT 拖到窗口或表格里。",
        24,
        18,
        1030,
        34,
        13,
        0,
    )
    label_info = create_label(
        main_window,
        "测试项：打开文件、保存文件、选择文件夹、颜色选择、拖拽接收、本地表格读取到 DataGridView。",
        24,
        600,
        1030,
        72,
        12,
        1,
    )

    btn_open_file = create_button(main_window, "📂", "打开表格", 24, 70, 132, COLOR_PRIMARY)
    btn_save_file = create_button(main_window, "💾", "导出CSV", 170, 70, 132, COLOR_SUCCESS)
    btn_pick_folder = create_button(main_window, "📁", "选文件夹", 316, 70, 132, COLOR_WARNING)
    btn_pick_color = create_button(main_window, "🎨", "选颜色", 462, 70, 132, COLOR_PRIMARY)
    btn_load_sample = create_button(main_window, "🧪", "加载样例", 608, 70, 132, COLOR_GRAY)

    grid = DLL.CreateDataGridView(
        main_window,
        24,
        124,
        1030,
        456,
        0,
        1,
        COLOR_TEXT,
        argb(255, 255, 255, 255),
    )
    DLL.DataGrid_SetDefaultRowHeight(grid, 34)
    DLL.DataGrid_SetHeaderHeight(grid, 38)
    DLL.DataGrid_SetShowGridLines(grid, 1)
    DLL.DataGrid_SetFreezeHeader(grid, 1)

    DLL.set_button_click_callback(button_callback_ref)
    DLL.SetFileDropCallback(main_window, file_drop_callback_ref)
    DLL.SetFileDropCallback(grid, file_drop_callback_ref)
    DLL.EnableFileDrop(main_window, 1)
    DLL.EnableFileDrop(grid, 1)

    sample = make_sample_csv()
    load_table_file(str(sample), "启动样例")


def main() -> int:
    if sys.platform != "win32":
        print("This demo only runs on Windows.")
        return 1

    print(f"Python: {platform.architecture()[0]}")
    print(f"DLL: {DLL_PATH}")
    setup_prototypes()
    create_ui()
    DLL.set_message_loop_main_window(main_window)
    return DLL.run_message_loop()


if __name__ == "__main__":
    raise SystemExit(main())
