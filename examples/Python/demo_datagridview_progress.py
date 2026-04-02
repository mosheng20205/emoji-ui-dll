"""DataGridView 进度条单元格示例。

演示内容：
- 在 DataGridView 中使用进度条单元格
- 读取选中单元格的进度百分比
- 设置选中单元格的进度百分比
- 在进度列上调用 DataGrid_SetCellText("88%") 的兼容行为
"""

from __future__ import annotations

import ctypes
import platform
import sys
from pathlib import Path


PROGRESS_COL = 2
WINDOW_X = 60
WINDOW_Y = 40
WINDOW_W = 1040
WINDOW_H = 840
CONTENT_X = 20
CONTENT_W = 1000
TITLE_Y = 12
SUBTITLE_Y = 46
INTRO_Y = 84
GRID_Y = 138
GRID_H = 410
BUTTON_Y = 596
STATUS_Y = 664
INFO_Y = 734


def repo_root() -> Path:
    return Path(__file__).resolve().parents[2]


def load_dll() -> ctypes.WinDLL:
    if sys.platform != "win32":
        raise OSError("该示例仅支持 Windows。")
    if platform.architecture()[0] != "64bit":
        raise OSError("请使用 64 位 Python，并加载 x64 版本的 DLL。")

    candidates = [
        repo_root() / "bin" / "x64" / "Release" / "emoji_window.dll",
        repo_root() / "emoji_window.dll",
        Path(__file__).resolve().parent / "emoji_window.dll",
    ]
    for path in candidates:
        if path.is_file():
            return ctypes.WinDLL(str(path))

    raise FileNotFoundError(
        "未找到 emoji_window.dll。请先编译 Release|x64，或将 DLL 复制到脚本同目录。"
    )


DLL = load_dll()


def u(text: str) -> bytes:
    return text.encode("utf-8")


def argb(a: int, r: int, g: int, b: int) -> int:
    return ((a & 0xFF) << 24) | ((r & 0xFF) << 16) | ((g & 0xFF) << 8) | (b & 0xFF)


def setup_prototypes() -> None:
    DLL.create_window_bytes.argtypes = [
        ctypes.c_char_p,
        ctypes.c_int,
        ctypes.c_int,
        ctypes.c_int,
        ctypes.c_int,
        ctypes.c_int,
    ]
    DLL.create_window_bytes.restype = ctypes.c_void_p

    DLL.set_message_loop_main_window.argtypes = [ctypes.c_void_p]
    DLL.set_message_loop_main_window.restype = None
    DLL.run_message_loop.argtypes = []
    DLL.run_message_loop.restype = ctypes.c_int

    DLL.CreateLabel.argtypes = [
        ctypes.c_void_p,
        ctypes.c_int,
        ctypes.c_int,
        ctypes.c_int,
        ctypes.c_int,
        ctypes.c_char_p,
        ctypes.c_int,
        ctypes.c_uint,
        ctypes.c_uint,
        ctypes.c_char_p,
        ctypes.c_int,
        ctypes.c_int,
        ctypes.c_int,
        ctypes.c_int,
        ctypes.c_int,
        ctypes.c_int,
        ctypes.c_int,
    ]
    DLL.CreateLabel.restype = ctypes.c_int
    DLL.SetLabelText.argtypes = [ctypes.c_int, ctypes.c_char_p, ctypes.c_int]
    DLL.SetLabelText.restype = None

    DLL.create_emoji_button_bytes.argtypes = [
        ctypes.c_void_p,
        ctypes.c_char_p,
        ctypes.c_int,
        ctypes.c_char_p,
        ctypes.c_int,
        ctypes.c_int,
        ctypes.c_int,
        ctypes.c_int,
        ctypes.c_int,
        ctypes.c_uint,
    ]
    DLL.create_emoji_button_bytes.restype = ctypes.c_int

    DLL.CreateDataGridView.argtypes = [
        ctypes.c_void_p,
        ctypes.c_int,
        ctypes.c_int,
        ctypes.c_int,
        ctypes.c_int,
        ctypes.c_int,
        ctypes.c_int,
        ctypes.c_uint,
        ctypes.c_uint,
    ]
    DLL.CreateDataGridView.restype = ctypes.c_int

    DLL.DataGrid_AddTextColumn.argtypes = [ctypes.c_int, ctypes.c_char_p, ctypes.c_int, ctypes.c_int]
    DLL.DataGrid_AddTextColumn.restype = ctypes.c_int
    DLL.DataGrid_AddProgressColumn.argtypes = [ctypes.c_int, ctypes.c_char_p, ctypes.c_int, ctypes.c_int]
    DLL.DataGrid_AddProgressColumn.restype = ctypes.c_int

    DLL.DataGrid_AddRow.argtypes = [ctypes.c_int]
    DLL.DataGrid_AddRow.restype = ctypes.c_int

    DLL.DataGrid_SetCellText.argtypes = [ctypes.c_int, ctypes.c_int, ctypes.c_int, ctypes.c_char_p, ctypes.c_int]
    DLL.DataGrid_SetCellText.restype = None
    DLL.DataGrid_GetCellText.argtypes = [ctypes.c_int, ctypes.c_int, ctypes.c_int, ctypes.c_void_p, ctypes.c_int]
    DLL.DataGrid_GetCellText.restype = ctypes.c_int

    DLL.DataGrid_SetCellProgress.argtypes = [ctypes.c_int, ctypes.c_int, ctypes.c_int, ctypes.c_int]
    DLL.DataGrid_SetCellProgress.restype = None
    DLL.DataGrid_GetCellProgress.argtypes = [ctypes.c_int, ctypes.c_int, ctypes.c_int]
    DLL.DataGrid_GetCellProgress.restype = ctypes.c_int

    DLL.DataGrid_SetCellStyle.argtypes = [
        ctypes.c_int,
        ctypes.c_int,
        ctypes.c_int,
        ctypes.c_uint,
        ctypes.c_uint,
        ctypes.c_int,
        ctypes.c_int,
    ]
    DLL.DataGrid_SetCellStyle.restype = None

    DLL.DataGrid_GetSelectedRow.argtypes = [ctypes.c_int]
    DLL.DataGrid_GetSelectedRow.restype = ctypes.c_int
    DLL.DataGrid_GetSelectedCol.argtypes = [ctypes.c_int]
    DLL.DataGrid_GetSelectedCol.restype = ctypes.c_int
    DLL.DataGrid_SetSelectedCell.argtypes = [ctypes.c_int, ctypes.c_int, ctypes.c_int]
    DLL.DataGrid_SetSelectedCell.restype = None
    DLL.DataGrid_SetSelectionMode.argtypes = [ctypes.c_int, ctypes.c_int]
    DLL.DataGrid_SetSelectionMode.restype = None
    DLL.DataGrid_SetShowGridLines.argtypes = [ctypes.c_int, ctypes.c_int]
    DLL.DataGrid_SetColumnWidth.argtypes = [ctypes.c_int, ctypes.c_int, ctypes.c_int]
    DLL.DataGrid_SetColumnWidth.restype = None
    DLL.DataGrid_SetDefaultRowHeight.argtypes = [ctypes.c_int, ctypes.c_int]
    DLL.DataGrid_SetHeaderHeight.argtypes = [ctypes.c_int, ctypes.c_int]
    DLL.DataGrid_SetFreezeHeader.argtypes = [ctypes.c_int, ctypes.c_int]
    DLL.DataGrid_SetColumnHeaderAlignment.argtypes = [ctypes.c_int, ctypes.c_int, ctypes.c_int]
    DLL.DataGrid_SetColumnCellAlignment.argtypes = [ctypes.c_int, ctypes.c_int, ctypes.c_int]
    DLL.DataGrid_Refresh.argtypes = [ctypes.c_int]
    DLL.DataGrid_Refresh.restype = None

    cell_click_cb_type = ctypes.CFUNCTYPE(None, ctypes.c_int, ctypes.c_int, ctypes.c_int)
    button_click_cb_type = ctypes.CFUNCTYPE(None, ctypes.c_int, ctypes.c_void_p)

    DLL.DataGrid_SetCellClickCallback.argtypes = [ctypes.c_int, cell_click_cb_type]
    DLL.DataGrid_SetCellClickCallback.restype = None
    DLL.set_button_click_callback.argtypes = [button_click_cb_type]
    DLL.set_button_click_callback.restype = None

    global CELL_CLICK_CB_TYPE, BUTTON_CLICK_CB_TYPE
    CELL_CLICK_CB_TYPE = cell_click_cb_type
    BUTTON_CLICK_CB_TYPE = button_click_cb_type


HWND_MAIN = None
GRID = 0
LABEL_STATUS = 0
BTN_READ = 0
BTN_SET_25 = 0
BTN_SET_75 = 0
BTN_PLUS_10 = 0
BTN_MINUS_10 = 0
BTN_TEXT_88 = 0


def set_status(text: str) -> None:
    raw = u(text)
    DLL.SetLabelText(LABEL_STATUS, raw, len(raw))
    print(text)


def set_cell_text(grid: int, row: int, col: int, text: str) -> None:
    raw = u(text)
    DLL.DataGrid_SetCellText(grid, row, col, raw, len(raw))


def get_cell_text(grid: int, row: int, col: int) -> str:
    needed = DLL.DataGrid_GetCellText(grid, row, col, None, 0)
    if needed <= 0:
        return ""
    buf = ctypes.create_string_buffer(needed + 1)
    written = DLL.DataGrid_GetCellText(grid, row, col, buf, len(buf))
    if written <= 0:
        return ""
    return buf.value.decode("utf-8", errors="replace")


def current_progress_target() -> tuple[int, int] | None:
    row = DLL.DataGrid_GetSelectedRow(GRID)
    col = DLL.DataGrid_GetSelectedCol(GRID)
    if row < 0:
        set_status("请先在表格中选中一行，建议直接点击进度条单元格。")
        return None
    if col != PROGRESS_COL:
        DLL.DataGrid_SetSelectedCell(GRID, row, PROGRESS_COL)
    return row, PROGRESS_COL


def read_selected_progress() -> None:
    target = current_progress_target()
    if target is None:
        return
    row, col = target
    progress = DLL.DataGrid_GetCellProgress(GRID, row, col)
    text = get_cell_text(GRID, row, col)
    task = get_cell_text(GRID, row, 0)
    set_status(f"状态：第 {row + 1} 行“{task}”当前进度为 {progress}%，单元格显示文本为 {text or '(空)'}。")


def set_selected_progress(value: int) -> None:
    target = current_progress_target()
    if target is None:
        return
    row, col = target
    DLL.DataGrid_SetCellProgress(GRID, row, col, value)
    DLL.DataGrid_Refresh(GRID)
    read_selected_progress()


def add_selected_progress(delta: int) -> None:
    target = current_progress_target()
    if target is None:
        return
    row, col = target
    current = DLL.DataGrid_GetCellProgress(GRID, row, col)
    DLL.DataGrid_SetCellProgress(GRID, row, col, current + delta)
    DLL.DataGrid_Refresh(GRID)
    read_selected_progress()


def set_selected_progress_by_text(text_value: str) -> None:
    target = current_progress_target()
    if target is None:
        return
    row, col = target
    set_cell_text(GRID, row, col, text_value)
    DLL.DataGrid_Refresh(GRID)
    read_selected_progress()


def on_grid_cell_click(_grid: int, row: int, col: int) -> None:
    if col == PROGRESS_COL:
        progress = DLL.DataGrid_GetCellProgress(GRID, row, col)
        text = get_cell_text(GRID, row, col)
        set_status(f"状态：已选中第 {row + 1} 行的进度单元格，当前值 {progress}%，显示文本 {text}。")
    else:
        task = get_cell_text(GRID, row, 0)
        set_status(f"状态：已选中第 {row + 1} 行“{task}”。如需读写进度，请点击该行“进度”列。")


def on_button_click(button_id: int, _parent_hwnd: ctypes.c_void_p) -> None:
    if button_id == BTN_READ:
        read_selected_progress()
    elif button_id == BTN_SET_25:
        set_selected_progress(25)
    elif button_id == BTN_SET_75:
        set_selected_progress(75)
    elif button_id == BTN_PLUS_10:
        add_selected_progress(10)
    elif button_id == BTN_MINUS_10:
        add_selected_progress(-10)
    elif button_id == BTN_TEXT_88:
        set_selected_progress_by_text("88%")


GRID_CELL_CLICK_CB = None
BUTTON_CLICK_CB = None


def build_demo_grid() -> None:
    headers = [
        ("📋 任务名称", DLL.DataGrid_AddTextColumn, 220),
        ("📝 执行说明", DLL.DataGrid_AddTextColumn, 390),
        ("📈 完成进度", DLL.DataGrid_AddProgressColumn, 190),
        ("🏷️ 当前状态", DLL.DataGrid_AddTextColumn, 200),
    ]
    for index, (title, fn, width) in enumerate(headers):
        raw = u(title)
        fn(GRID, raw, len(raw), width)
        DLL.DataGrid_SetColumnWidth(GRID, index, width)
        DLL.DataGrid_SetColumnHeaderAlignment(GRID, index, 1)
        if index == PROGRESS_COL:
            DLL.DataGrid_SetColumnCellAlignment(GRID, index, 1)

    rows = [
        ("需求梳理", "和产品确认 MVP 范围", 15, "🕒 初始阶段"),
        ("界面开发", "完成主界面和导航区域", 42, "🚧 正在推进"),
        ("接口联调", "对接登录与列表接口", 68, "🔄 接近完成"),
        ("回归测试", "重点检查表格与状态同步", 86, "🧪 准备收尾"),
        ("发布准备", "整理脚本与变更说明", 100, "✅ 已完成"),
    ]

    row_colors = [
        argb(255, 138, 149, 166),
        argb(255, 210, 167, 96),
        argb(255, 110, 151, 201),
        argb(255, 116, 174, 134),
        argb(255, 206, 128, 128),
    ]

    for row_index, (task, desc, progress, note) in enumerate(rows):
        row = DLL.DataGrid_AddRow(GRID)
        set_cell_text(GRID, row, 0, task)
        set_cell_text(GRID, row, 1, desc)
        DLL.DataGrid_SetCellProgress(GRID, row, PROGRESS_COL, progress)
        set_cell_text(GRID, row, 3, note)

        DLL.DataGrid_SetCellStyle(
            GRID,
            row,
            PROGRESS_COL,
            argb(255, 48, 49, 51),
            row_colors[row_index],
            0,
            0,
        )

    # 演示：在进度列中直接写入文本，也能同步更新进度条。
    set_cell_text(GRID, 1, PROGRESS_COL, "55%")
    DLL.DataGrid_SetCellStyle(GRID, 1, PROGRESS_COL, argb(255, 48, 49, 51), argb(255, 230, 162, 60), 0, 0)

    DLL.DataGrid_SetSelectedCell(GRID, 0, PROGRESS_COL)


def create_buttons() -> None:
    global BTN_READ, BTN_SET_25, BTN_SET_75, BTN_PLUS_10, BTN_MINUS_10, BTN_TEXT_88

    button_specs = [
        ("📖", "读取当前进度", 20, argb(255, 64, 158, 255)),
        ("🟢", "设置为 25%", 180, argb(255, 103, 194, 58)),
        ("🟠", "设置为 75%", 340, argb(255, 230, 162, 60)),
        ("➕", "增加 10%", 500, argb(255, 64, 158, 255)),
        ("➖", "减少 10%", 660, argb(255, 245, 108, 108)),
        ("📝", "文本写入 88%", 820, argb(255, 144, 147, 153)),
    ]

    ids: list[int] = []
    for emoji, text, x, color in button_specs:
        emoji_raw = u(emoji)
        text_raw = u(text)
        button_id = DLL.create_emoji_button_bytes(
            HWND_MAIN,
            emoji_raw,
            len(emoji_raw),
            text_raw,
            len(text_raw),
            x,
            BUTTON_Y,
            150,
            44,
            color,
        )
        ids.append(button_id)

    BTN_READ, BTN_SET_25, BTN_SET_75, BTN_PLUS_10, BTN_MINUS_10, BTN_TEXT_88 = ids


def main() -> None:
    global HWND_MAIN, GRID, LABEL_STATUS, GRID_CELL_CLICK_CB, BUTTON_CLICK_CB

    setup_prototypes()

    title = u("📊 DataGridView 表格进度条单元格正式示例 - emoji_window")
    HWND_MAIN = DLL.create_window_bytes(title, len(title), WINDOW_X, WINDOW_Y, WINDOW_W, WINDOW_H)
    if not HWND_MAIN:
        raise RuntimeError("创建窗口失败。")

    font = u("Microsoft YaHei UI")

    title_text = u("📊 DataGridView 表格进度条单元格正式示例")
    DLL.CreateLabel(
        HWND_MAIN,
        CONTENT_X,
        TITLE_Y,
        CONTENT_W,
        30,
        title_text,
        len(title_text),
        argb(255, 48, 49, 51),
        argb(0, 0, 0, 0),
        font,
        len(font),
        16,
        1,
        0,
        0,
        0,
        0,
    )

    subtitle_text = u("演示表格内嵌进度条列、百分比读写接口，以及文本与进度值的同步行为。")
    DLL.CreateLabel(
        HWND_MAIN,
        CONTENT_X,
        SUBTITLE_Y,
        CONTENT_W,
        28,
        subtitle_text,
        len(subtitle_text),
        argb(255, 96, 98, 102),
        argb(0, 0, 0, 0),
        font,
        len(font),
        12,
        0,
        0,
        0,
        0,
        0,
    )

    intro = (
        "💡 使用方式：点击表格中的“完成进度”单元格，再通过下方按钮读取、设置或增减百分比。"
        " “文本写入 88%”用于演示 DataGrid_SetCellText('88%') 与进度条的同步能力。"
    )
    intro_raw = u(intro)
    DLL.CreateLabel(
        HWND_MAIN,
        CONTENT_X,
        INTRO_Y,
        CONTENT_W,
        38,
        intro_raw,
        len(intro_raw),
        argb(255, 80, 87, 96),
        argb(255, 245, 247, 250),
        font,
        len(font),
        12,
        0,
        0,
        0,
        0,
        1,
    )

    status_raw = u("状态：示例已就绪，默认选中第 1 行“完成进度”单元格，可直接操作下方按钮。")
    LABEL_STATUS = DLL.CreateLabel(
        HWND_MAIN,
        CONTENT_X,
        STATUS_Y,
        CONTENT_W,
        52,
        status_raw,
        len(status_raw),
        argb(255, 34, 36, 38),
        argb(255, 250, 251, 252),
        font,
        len(font),
        12,
        0,
        0,
        0,
        0,
        1,
    )

    info_raw = u("📌 重点接口：DataGrid_AddProgressColumn / DataGrid_SetCellProgress / DataGrid_GetCellProgress / DataGrid_SetCellText")
    DLL.CreateLabel(
        HWND_MAIN,
        CONTENT_X,
        INFO_Y,
        CONTENT_W,
        42,
        info_raw,
        len(info_raw),
        argb(255, 255, 255, 255),
        argb(255, 64, 144, 228),
        font,
        len(font),
        12,
        0,
        0,
        0,
        0,
        1,
    )

    GRID = DLL.CreateDataGridView(
        HWND_MAIN,
        CONTENT_X,
        GRID_Y,
        CONTENT_W,
        GRID_H,
        0,
        1,
        argb(255, 48, 49, 51),
        argb(255, 255, 255, 255),
    )

    DLL.DataGrid_SetDefaultRowHeight(GRID, 38)
    DLL.DataGrid_SetHeaderHeight(GRID, 42)
    DLL.DataGrid_SetShowGridLines(GRID, 1)
    DLL.DataGrid_SetFreezeHeader(GRID, 1)
    DLL.DataGrid_SetSelectionMode(GRID, 0)

    build_demo_grid()
    create_buttons()

    GRID_CELL_CLICK_CB = CELL_CLICK_CB_TYPE(on_grid_cell_click)
    BUTTON_CLICK_CB = BUTTON_CLICK_CB_TYPE(on_button_click)
    DLL.DataGrid_SetCellClickCallback(GRID, GRID_CELL_CLICK_CB)
    DLL.set_button_click_callback(BUTTON_CLICK_CB)

    read_selected_progress()

    DLL.set_message_loop_main_window(HWND_MAIN)
    DLL.run_message_loop()


if __name__ == "__main__":
    main()
