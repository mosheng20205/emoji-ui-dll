"""Minimal DataGridView demo for freeze column/row counts.

Runs a small table with:
- freeze_col_count = 2
- freeze_row_count = 2

The example is intentionally small and self-contained so it can be used
to visually verify the new frozen-region behavior.
"""

from __future__ import annotations

import ctypes
import platform
import sys
from pathlib import Path


def repo_root() -> Path:
    return Path(__file__).resolve().parents[2]


def load_dll() -> ctypes.CDLL:
    if sys.platform != "win32":
        raise OSError("This demo only supports Windows.")
    if platform.architecture()[0] != "64bit":
        raise OSError("Please use 64-bit Python with the x64 DLL.")

    candidates = [
        repo_root() / "bin" / "x64" / "Release" / "emoji_window.dll",
        repo_root() / "emoji_window.dll",
        Path(__file__).resolve().parent / "emoji_window.dll",
    ]
    for path in candidates:
        if path.is_file():
            return ctypes.WinDLL(str(path))

    raise FileNotFoundError(
        "emoji_window.dll not found. Build Release|x64 first, or copy the DLL next to this script."
    )


DLL = load_dll()


def u(text: str) -> bytes:
    return text.encode("utf-8")


def argb(a: int, r: int, g: int, b: int) -> int:
    return ((a & 0xFF) << 24) | ((r & 0xFF) << 16) | ((g & 0xFF) << 8) | (b & 0xFF)


def setup_prototypes() -> None:
    DLL.create_window_bytes.argtypes = [ctypes.c_char_p, ctypes.c_int, ctypes.c_int, ctypes.c_int]
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
    DLL.DataGrid_AddRow.argtypes = [ctypes.c_int]
    DLL.DataGrid_AddRow.restype = ctypes.c_int
    DLL.DataGrid_SetCellText.argtypes = [ctypes.c_int, ctypes.c_int, ctypes.c_int, ctypes.c_char_p, ctypes.c_int]
    DLL.DataGrid_SetCellStyle.argtypes = [
        ctypes.c_int,
        ctypes.c_int,
        ctypes.c_int,
        ctypes.c_uint,
        ctypes.c_uint,
        ctypes.c_int,
        ctypes.c_int,
    ]
    DLL.DataGrid_SetColumnWidth.argtypes = [ctypes.c_int, ctypes.c_int, ctypes.c_int]
    DLL.DataGrid_SetDefaultRowHeight.argtypes = [ctypes.c_int, ctypes.c_int]
    DLL.DataGrid_SetHeaderHeight.argtypes = [ctypes.c_int, ctypes.c_int]
    DLL.DataGrid_SetFreezeHeader.argtypes = [ctypes.c_int, ctypes.c_int]
    DLL.DataGrid_SetFreezeColumnCount.argtypes = [ctypes.c_int, ctypes.c_int]
    DLL.DataGrid_SetFreezeRowCount.argtypes = [ctypes.c_int, ctypes.c_int]
    DLL.DataGrid_SetSelectionMode.argtypes = [ctypes.c_int, ctypes.c_int]
    DLL.DataGrid_SetSelectedCell.argtypes = [ctypes.c_int, ctypes.c_int, ctypes.c_int]
    DLL.DataGrid_Refresh.argtypes = [ctypes.c_int]


def set_cell_text(grid: int, row: int, col: int, text: str) -> None:
    raw = u(text)
    DLL.DataGrid_SetCellText(grid, row, col, raw, len(raw))


def main() -> None:
    setup_prototypes()

    title = u("DataGrid Freeze Count Demo")
    hwnd = DLL.create_window_bytes(title, len(title), 1180, 720)
    if not hwnd:
        raise RuntimeError("Failed to create window.")

    font = u("Microsoft YaHei UI")

    note = (
        "Frozen setup: header + first 2 columns + first 2 data rows. "
        "Scroll horizontally and vertically to verify the pinned region stays stable."
    )
    note_raw = u(note)
    DLL.CreateLabel(
        hwnd,
        20,
        12,
        1120,
        44,
        note_raw,
        len(note_raw),
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

    grid = DLL.CreateDataGridView(
        hwnd,
        20,
        70,
        1120,
        600,
        0,
        1,
        argb(255, 48, 49, 51),
        argb(255, 255, 255, 255),
    )
    if not grid:
        raise RuntimeError("Failed to create DataGridView.")

    columns = [
        ("ID", 90),
        ("Name", 150),
        ("Region", 140),
        ("Owner", 150),
        ("Status", 140),
        ("Priority", 130),
        ("Progress", 140),
        ("Comment", 260),
    ]
    for index, (header, width) in enumerate(columns):
        header_raw = u(header)
        DLL.DataGrid_AddTextColumn(grid, header_raw, len(header_raw), width)
        DLL.DataGrid_SetColumnWidth(grid, index, width)

    DLL.DataGrid_SetDefaultRowHeight(grid, 34)
    DLL.DataGrid_SetHeaderHeight(grid, 42)
    DLL.DataGrid_SetFreezeHeader(grid, 1)
    DLL.DataGrid_SetFreezeColumnCount(grid, 2)
    DLL.DataGrid_SetFreezeRowCount(grid, 2)
    DLL.DataGrid_SetSelectionMode(grid, 0)

    statuses = ["Draft", "Review", "Active", "Blocked", "Done"]
    priorities = ["P0", "P1", "P2", "P3"]
    regions = ["APAC", "EMEA", "NA", "LATAM"]
    owners = ["Ava", "Liam", "Mia", "Noah", "Emma", "Leo"]

    for row in range(28):
        DLL.DataGrid_AddRow(grid)
        values = [
            f"{1000 + row}",
            f"Item {row + 1}",
            regions[row % len(regions)],
            owners[row % len(owners)],
            statuses[row % len(statuses)],
            priorities[row % len(priorities)],
            f"{(row * 7) % 101}%",
            f"Scroll check row {row + 1}, frozen cols=2, frozen rows=2",
        ]
        for col, value in enumerate(values):
            set_cell_text(grid, row, col, value)

    # Make the frozen region visually obvious.
    frozen_bg = argb(255, 236, 245, 255)
    frozen_fg = argb(255, 32, 79, 146)
    cross_bg = argb(255, 220, 238, 255)
    cross_fg = argb(255, 24, 58, 106)
    for row in range(28):
        for col in range(8):
            if row < 2 and col < 2:
                DLL.DataGrid_SetCellStyle(grid, row, col, cross_fg, cross_bg, 1, 0)
            elif row < 2 or col < 2:
                DLL.DataGrid_SetCellStyle(grid, row, col, frozen_fg, frozen_bg, 0, 0)

    DLL.DataGrid_SetSelectedCell(grid, 5, 5)
    DLL.DataGrid_Refresh(grid)

    DLL.set_message_loop_main_window(hwnd)
    DLL.run_message_loop()


if __name__ == "__main__":
    main()
