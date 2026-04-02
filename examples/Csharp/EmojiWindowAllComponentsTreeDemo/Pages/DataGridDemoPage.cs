using System;
using System.Collections.Generic;
using System.IO;

namespace EmojiWindowDemo
{
    internal static class DataGridDemoPage
    {
        private const int HeaderStylePlain = 0;
        private const int HeaderStyleDark = 2;

        public static void Build(AllDemoShell shell, IntPtr page)
        {
            var app = shell.App;
            IntPtr modeLabel = app.Label(40, 48, 240, 24, "当前模式: 普通表格", DemoColors.Blue, DemoColors.Surface, 13, PageCommon.AlignLeft, false, page);
            IntPtr gridState = app.Label(296, 48, 760, 58, "等待表格状态。", DemoColors.Gray, DemoColors.Surface, 12, PageCommon.AlignLeft, true, page);
            IntPtr exportLabel = app.Label(1096, 48, 328, 58, "CSV 尚未导出。", DemoColors.Gray, DemoColors.Surface, 12, PageCommon.AlignLeft, true, page);
            IntPtr headerReadout = app.Label(1104, 328, 336, 74, "等待读取表头。", DemoColors.Gray, DemoColors.Surface, 12, PageCommon.AlignLeft, true, page);
            IntPtr cellReadout = app.Label(1104, 542, 336, 74, "等待读取单元格。", DemoColors.Gray, DemoColors.Surface, 12, PageCommon.AlignLeft, true, page);

            IntPtr toolbar = EmojiWindowNative.CreatePanel(page, 40, 146, 1384, 42, DemoColors.Surface);
            IntPtr normalGrid = EmojiWindowNative.CreateDataGridView(page, 36, 300, 996, 448, 0, 1, DemoColors.Black, DemoColors.WindowBg);
            IntPtr virtualGrid = EmojiWindowNative.CreateDataGridView(page, 36, 300, 996, 448, 1, 1, DemoColors.Black, DemoColors.WindowBg);
            EmojiWindowNative.DataGrid_Show(virtualGrid, 0);

            string[] normalHeaders = { "🧾 任务", "☑️ 启用", "🚦 状态", "🏷️ 标签", "🔘 动作", "🔗 链接", "🖼️ 图片", "📝 备注" };
            string[] virtualHeaders = { "🧾 序号", "🚦 状态", "🏷️ 优先级", "👤 节点", "🛠 路由", "🖼️ 图片", "📝 虚拟备注" };
            (string Title, Func<IntPtr, byte[], int, int, int> Add, int Width)[] normalColumns =
            {
                (normalHeaders[0], EmojiWindowNative.DataGrid_AddTextColumn, 168),
                (normalHeaders[1], EmojiWindowNative.DataGrid_AddCheckBoxColumn, 68),
                (normalHeaders[2], EmojiWindowNative.DataGrid_AddComboBoxColumn, 116),
                (normalHeaders[3], EmojiWindowNative.DataGrid_AddTagColumn, 90),
                (normalHeaders[4], EmojiWindowNative.DataGrid_AddButtonColumn, 92),
                (normalHeaders[5], EmojiWindowNative.DataGrid_AddLinkColumn, 118),
                (normalHeaders[6], EmojiWindowNative.DataGrid_AddImageColumn, 88),
                (normalHeaders[7], EmojiWindowNative.DataGrid_AddTextColumn, 246),
            };
            int[] virtualWidths = { 124, 118, 108, 110, 126, 88, 270 };

            foreach (var column in normalColumns)
            {
                byte[] header = app.U(column.Title);
                column.Add(normalGrid, header, header.Length, column.Width);
            }

            for (int i = 0; i < virtualHeaders.Length; i++)
            {
                byte[] header = app.U(virtualHeaders[i]);
                EmojiWindowNative.DataGrid_AddTextColumn(virtualGrid, header, header.Length, virtualWidths[i]);
            }

            EmojiWindowNative.DataGrid_SetColors(normalGrid, DemoColors.Black, DemoColors.WindowBg, DemoColors.Surface, DemoColors.Black, DemoColors.LightBlue, DemoColors.Surface, DemoColors.Border);
            EmojiWindowNative.DataGrid_SetColors(virtualGrid, DemoColors.Black, DemoColors.WindowBg, DemoColors.Surface, DemoColors.Black, DemoColors.LightBlue, DemoColors.Surface, DemoColors.Border);
            ApplyCommonGridStyle(normalGrid, normalHeaders.Length);
            ApplyCommonGridStyle(virtualGrid, virtualHeaders.Length);
            EmojiWindowNative.DataGrid_SetColumnCellAlignment(normalGrid, 1, PageCommon.AlignCenter);
            EmojiWindowNative.DataGrid_SetColumnCellAlignment(normalGrid, 2, PageCommon.AlignCenter);
            EmojiWindowNative.DataGrid_SetColumnCellAlignment(normalGrid, 3, PageCommon.AlignCenter);
            EmojiWindowNative.DataGrid_SetColumnCellAlignment(normalGrid, 4, PageCommon.AlignCenter);
            EmojiWindowNative.DataGrid_SetColumnCellAlignment(normalGrid, 6, PageCommon.AlignCenter);

            byte[] comboItems = app.U("🕒 待处理\n🚧 进行中\n✅ 已完成\n⏸️ 已暂停");
            EmojiWindowNative.DataGrid_SetColumnComboItems(normalGrid, 2, comboItems, comboItems.Length);
            EmojiWindowNative.DataGrid_SetFreezeHeader(normalGrid, 1);
            EmojiWindowNative.DataGrid_SetFreezeColumnCount(normalGrid, 2);
            EmojiWindowNative.DataGrid_SetFreezeRowCount(normalGrid, 2);

            string[] imagePaths =
            {
                PageCommon.FindFirstExistingPath(app, @"imgs\1.png"),
                PageCommon.FindFirstExistingPath(app, @"imgs\2.png"),
                PageCommon.FindFirstExistingPath(app, @"imgs\3.png"),
            };

            var rows = new List<RowSeed>
            {
                new RowSeed("🧾 任务 1", true, "🚧 进行中", "🔵 P1", "执行", "查看详情", "图片-A", "支持下拉、勾选和标签色块"),
                new RowSeed("🧾 任务 2", false, "🕒 待处理", "🟢 P2", "审核", "打开文档", "图片-B", "支持按钮列、链接列和图片列"),
                new RowSeed("🧾 任务 3", true, "✅ 已完成", "🟠 P3", "归档", "查看报告", "图片-C", "支持排序、导出 CSV 与表头读写"),
            };

            bool virtualMode = false;
            bool headerDark = true;
            bool accentStyle = false;
            bool doubleClickEnabled = true;
            int selectedRow = 0;
            int selectedCol = 0;
            int headerAlignment = PageCommon.AlignCenter;
            int cellAlignment = PageCommon.AlignLeft;
            var sortStates = new Dictionary<int, int>();

            void SetState(string text)
            {
                shell.SetLabelText(gridState, text);
                shell.SetStatus(text);
            }

            IntPtr ActiveGrid() => virtualMode ? virtualGrid : normalGrid;

            void UpdateModeLabel()
            {
                shell.SetLabelText(modeLabel, virtualMode ? "当前模式: 虚拟表格 1,000,000 行" : $"当前模式: 普通表格 {rows.Count} 行");
            }

            string ReadHeader(IntPtr grid, int index) => EmojiWindowNative.ReadUtf8(EmojiWindowNative.DataGrid_GetColumnHeaderText, grid, index);

            string ReadCell(IntPtr grid, int row, int col)
            {
                if (grid == virtualGrid)
                {
                    return PageCommon.BuildVirtualGridText(row, col);
                }

                return EmojiWindowNative.ReadUtf8(EmojiWindowNative.DataGrid_GetCellText, grid, row, col);
            }

            void RefreshHeaderReadout(string prefix)
            {
                string normalText = string.Join(" | ", ReadHeaders(normalGrid));
                string virtualText = string.Join(" | ", ReadHeaders(virtualGrid));
                shell.SetLabelText(headerReadout, $"{prefix}\r\n普通表头: {normalText}\r\n虚拟表头: {virtualText}");
            }

            IEnumerable<string> ReadHeaders(IntPtr grid)
            {
                int count = EmojiWindowNative.DataGrid_GetColumnCount(grid);
                for (int i = 0; i < count; i++)
                {
                    yield return ReadHeader(grid, i);
                }
            }

            void RefreshCellReadout(string prefix)
            {
                IntPtr grid = ActiveGrid();
                int row = EmojiWindowNative.DataGrid_GetSelectedRow(grid);
                int col = EmojiWindowNative.DataGrid_GetSelectedCol(grid);
                if (row < 0 || col < 0)
                {
                    row = selectedRow;
                    col = selectedCol;
                }

                string extra = string.Empty;
                if (grid == normalGrid && col == 1 && row >= 0)
                {
                    extra = $"\r\n勾选状态: {EmojiWindowNative.DataGrid_GetCellChecked(grid, row, col) != 0}";
                }

                shell.SetLabelText(cellReadout, $"{prefix}\r\nrow={row}, col={col}\r\nvalue={ReadCell(grid, row, col)}{extra}");
            }

            void ApplyRowStyles()
            {
                for (int row = 0; row < rows.Count; row++)
                {
                    uint tagFg = row % 3 == 0 ? DemoColors.Blue : (row % 3 == 1 ? DemoColors.Green : DemoColors.Orange);
                    uint tagBg = row % 3 == 0 ? DemoColors.LightBlue : (row % 3 == 1 ? DemoColors.LightGreen : DemoColors.Yellow);
                    EmojiWindowNative.DataGrid_SetCellStyle(normalGrid, row, 3, tagFg, tagBg, 0, 0);
                    EmojiWindowNative.DataGrid_SetCellStyle(normalGrid, row, 4, DemoColors.White, row % 2 == 0 ? DemoColors.Blue : DemoColors.Green, 0, 0);
                    EmojiWindowNative.DataGrid_SetCellStyle(normalGrid, row, 5, DemoColors.Blue, 0, 0, 0);
                    EmojiWindowNative.DataGrid_SetCellStyle(normalGrid, row, 6, tagFg, tagBg, 0, 0);
                    EmojiWindowNative.DataGrid_SetCellStyle(normalGrid, row, 0, accentStyle ? DemoColors.Purple : 0, accentStyle ? DemoColors.LightBlue : 0, accentStyle ? 1 : 0, 0);
                    EmojiWindowNative.DataGrid_SetCellStyle(normalGrid, row, 7, 0, accentStyle ? DemoColors.Yellow : 0, 0, 0);
                }

                EmojiWindowNative.DataGrid_Refresh(normalGrid);
            }

            void LoadImageCell(int row, int imageIndex)
            {
                string path = imagePaths[imageIndex % imagePaths.Length];
                if (!string.IsNullOrEmpty(path))
                {
                    byte[] bytes = app.U(path);
                    EmojiWindowNative.DataGrid_SetCellImageFromFile(normalGrid, row, 6, bytes, bytes.Length);
                }
            }

            void RebuildNormalRows()
            {
                EmojiWindowNative.DataGrid_ClearRows(normalGrid);
                for (int i = 0; i < rows.Count; i++)
                {
                    int row = EmojiWindowNative.DataGrid_AddRow(normalGrid);
                    RowSeed item = rows[i];
                    PageCommon.SetGridCellText(app, normalGrid, row, 0, item.Task);
                    EmojiWindowNative.DataGrid_SetCellChecked(normalGrid, row, 1, item.Enabled ? 1 : 0);
                    PageCommon.SetGridCellText(app, normalGrid, row, 2, item.Status);
                    PageCommon.SetGridCellText(app, normalGrid, row, 3, item.Tag);
                    PageCommon.SetGridCellText(app, normalGrid, row, 4, item.ActionText);
                    PageCommon.SetGridCellText(app, normalGrid, row, 5, item.LinkText);
                    PageCommon.SetGridCellText(app, normalGrid, row, 6, item.ImageAlt);
                    PageCommon.SetGridCellText(app, normalGrid, row, 7, item.Note);
                    LoadImageCell(row, i);
                }

                ApplyRowStyles();
                UpdateModeLabel();
            }

            void AddRow()
            {
                rows.Add(new RowSeed($"🧾 新任务 {rows.Count + 1}", rows.Count % 2 == 0, "🕒 待处理", rows.Count % 2 == 0 ? "🔵 P1" : "🟢 P2", "打开", "查看详情", "图片-N", "运行时追加的数据"));
                RebuildNormalRows();
                SetState("➕ 已向普通表格新增一行。");
            }

            void ClearRows()
            {
                rows.Clear();
                RebuildNormalRows();
                SetState("🧹 已清空普通表格。");
                RefreshCellReadout("清空后读取单元格");
            }

            void ToggleVirtualMode()
            {
                virtualMode = !virtualMode;
                EmojiWindowNative.DataGrid_Show(normalGrid, virtualMode ? 0 : 1);
                EmojiWindowNative.DataGrid_Show(virtualGrid, virtualMode ? 1 : 0);
                UpdateModeLabel();
                RefreshCellReadout("切换模式后读取单元格");
                SetState(virtualMode ? "🚀 已切换到虚拟表格 1,000,000 行。" : "🧾 已切回普通表格。");
            }

            void ToggleSort(int col)
            {
                if (virtualMode)
                {
                    SetState("虚拟表格不做排序演示，请先切回普通表格。");
                    return;
                }

                int nextOrder = !sortStates.ContainsKey(col) || sortStates[col] != 1 ? 1 : 2;
                sortStates[col] = nextOrder;
                EmojiWindowNative.DataGrid_SortByColumn(normalGrid, col, nextOrder);
                EmojiWindowNative.DataGrid_Refresh(normalGrid);
                SetState($"🔀 已按列 {col} {(nextOrder == 1 ? "升序" : "降序")} 排序。");
                RefreshCellReadout("排序后读取单元格");
            }

            void RenameHeader(IntPtr grid, int col, string text, string note)
            {
                byte[] bytes = app.U(text);
                EmojiWindowNative.DataGrid_SetColumnHeaderText(grid, col, bytes, bytes.Length);
                EmojiWindowNative.DataGrid_Refresh(grid);
                RefreshHeaderReadout("修改表头后读取");
                SetState(note);
            }

            void RestoreHeaders()
            {
                for (int i = 0; i < normalHeaders.Length; i++)
                {
                    byte[] text = app.U(normalHeaders[i]);
                    EmojiWindowNative.DataGrid_SetColumnHeaderText(normalGrid, i, text, text.Length);
                }

                for (int i = 0; i < virtualHeaders.Length; i++)
                {
                    byte[] text = app.U(virtualHeaders[i]);
                    EmojiWindowNative.DataGrid_SetColumnHeaderText(virtualGrid, i, text, text.Length);
                }

                EmojiWindowNative.DataGrid_Refresh(normalGrid);
                EmojiWindowNative.DataGrid_Refresh(virtualGrid);
                RefreshHeaderReadout("已恢复默认表头");
                SetState("↩️ 已恢复普通表格和虚拟表格默认表头。");
            }

            void SelectCell(int row, int col)
            {
                IntPtr grid = ActiveGrid();
                int maxCol = EmojiWindowNative.DataGrid_GetColumnCount(grid) - 1;
                row = Math.Max(0, row);
                col = Math.Max(0, Math.Min(col, maxCol));
                EmojiWindowNative.DataGrid_SetSelectedCell(grid, row, col);
                selectedRow = row;
                selectedCol = col;
                RefreshCellReadout($"已定位到 [{row}, {col}]");
            }

            void ToggleSelectedCheck()
            {
                if (virtualMode)
                {
                    SetState("虚拟表格没有勾选列，请先切回普通表格。");
                    return;
                }

                int row = EmojiWindowNative.DataGrid_GetSelectedRow(normalGrid);
                if (row < 0)
                {
                    row = 0;
                }

                bool nextValue = EmojiWindowNative.DataGrid_GetCellChecked(normalGrid, row, 1) == 0;
                EmojiWindowNative.DataGrid_SetCellChecked(normalGrid, row, 1, nextValue ? 1 : 0);
                EmojiWindowNative.DataGrid_Refresh(normalGrid);
                selectedRow = row;
                selectedCol = 1;
                SetState($"☑️ 已切换第 {row} 行勾选状态为 {nextValue}。");
                RefreshCellReadout("切换勾选后读取单元格");
            }

            void WriteSelectedCell()
            {
                if (virtualMode)
                {
                    SetState("虚拟表格不支持直接改单元格，请先切回普通表格。");
                    return;
                }

                int row = EmojiWindowNative.DataGrid_GetSelectedRow(normalGrid);
                int col = EmojiWindowNative.DataGrid_GetSelectedCol(normalGrid);
                if (row < 0 || col < 0)
                {
                    row = 0;
                    col = 7;
                }

                PageCommon.SetGridCellText(app, normalGrid, row, col, $"🛠️ 已修改 [{row},{col}]");
                EmojiWindowNative.DataGrid_Refresh(normalGrid);
                selectedRow = row;
                selectedCol = col;
                SetState($"✏️ 已修改普通表格单元格 [{row}, {col}]。");
                RefreshCellReadout("修改后读取单元格");
            }

            void ToggleHeaderStyle()
            {
                headerDark = !headerDark;
                EmojiWindowNative.DataGrid_SetHeaderStyle(normalGrid, headerDark ? HeaderStyleDark : HeaderStylePlain);
                EmojiWindowNative.DataGrid_SetHeaderStyle(virtualGrid, headerDark ? HeaderStyleDark : HeaderStylePlain);
                EmojiWindowNative.DataGrid_Refresh(normalGrid);
                EmojiWindowNative.DataGrid_Refresh(virtualGrid);
                SetState($"🎨 已切换表头样式为 {(headerDark ? "Dark" : "Plain")}。");
            }

            void ToggleDoubleClickEdit()
            {
                doubleClickEnabled = !doubleClickEnabled;
                EmojiWindowNative.DataGrid_SetDoubleClickEnabled(normalGrid, doubleClickEnabled ? 1 : 0);
                SetState($"🖱️ 普通表格双击编辑已{(doubleClickEnabled ? "开启" : "关闭")}。");
            }

            void ToggleAccentStyle()
            {
                accentStyle = !accentStyle;
                ApplyRowStyles();
                SetState($"🪄 单元格演示样式已{(accentStyle ? "开启" : "恢复默认")}。");
            }

            void CycleHeaderAlignment()
            {
                headerAlignment = headerAlignment == PageCommon.AlignLeft ? PageCommon.AlignCenter : (headerAlignment == PageCommon.AlignCenter ? PageCommon.AlignRight : PageCommon.AlignLeft);
                int count = EmojiWindowNative.DataGrid_GetColumnCount(ActiveGrid());
                for (int i = 0; i < count; i++)
                {
                    EmojiWindowNative.DataGrid_SetColumnHeaderAlignment(ActiveGrid(), i, headerAlignment);
                }

                EmojiWindowNative.DataGrid_Refresh(ActiveGrid());
                SetState($"📐 表头对齐已切到 {AlignmentName(headerAlignment)}。");
            }

            void CycleCellAlignment()
            {
                cellAlignment = cellAlignment == PageCommon.AlignLeft ? PageCommon.AlignCenter : (cellAlignment == PageCommon.AlignCenter ? PageCommon.AlignRight : PageCommon.AlignLeft);
                int count = EmojiWindowNative.DataGrid_GetColumnCount(ActiveGrid());
                for (int i = 0; i < count; i++)
                {
                    EmojiWindowNative.DataGrid_SetColumnCellAlignment(ActiveGrid(), i, cellAlignment);
                }

                EmojiWindowNative.DataGrid_Refresh(ActiveGrid());
                SetState($"📏 单元格对齐已切到 {AlignmentName(cellAlignment)}。");
            }

            void ExportCsv()
            {
                string exportPath = Path.Combine(AppDomain.CurrentDomain.BaseDirectory, "_datagrid_export_demo.csv");
                byte[] pathBytes = app.U(exportPath);
                bool ok = EmojiWindowNative.DataGrid_ExportCSV(normalGrid, pathBytes, pathBytes.Length) != 0;
                shell.SetLabelText(exportLabel, ok ? "CSV 已导出:\r\n" + exportPath : "CSV 导出失败。");
                SetState(ok ? "📤 已导出 CSV。" : "CSV 导出失败。");
            }

            int OnGridMenuAdd = 9301;
            int OnGridMenuClear = 9302;
            int OnGridMenuToggle = 9303;
            int OnGridMenuRead = 9304;
            int OnGridMenuHeader = 9305;
            IntPtr popupMenu = EmojiWindowNative.CreateEmojiPopupMenu(page);
            AddMenuItem(app, popupMenu, "➕ 添加一行", OnGridMenuAdd);
            AddMenuItem(app, popupMenu, "🧹 清空普通表格", OnGridMenuClear);
            AddMenuItem(app, popupMenu, "🚀 切换虚拟表格", OnGridMenuToggle);
            AddMenuItem(app, popupMenu, "📄 读取当前单元格", OnGridMenuRead);
            AddMenuItem(app, popupMenu, "🎨 切换表头样式", OnGridMenuHeader);
            var popupCallback = app.Pin(new EmojiWindowNative.MenuItemClickCallback((_, itemId) =>
            {
                if (itemId == OnGridMenuAdd) AddRow();
                else if (itemId == OnGridMenuClear) ClearRows();
                else if (itemId == OnGridMenuToggle) ToggleVirtualMode();
                else if (itemId == OnGridMenuRead) RefreshCellReadout("右键菜单读取单元格");
                else if (itemId == OnGridMenuHeader) ToggleHeaderStyle();
            }));
            EmojiWindowNative.SetPopupMenuCallback(popupMenu, popupCallback);
            EmojiWindowNative.BindControlMenu(normalGrid, popupMenu);
            EmojiWindowNative.BindControlMenu(virtualGrid, popupMenu);

            var clickCallback = app.Pin(new EmojiWindowNative.DataGridCellCallback((grid, row, col) =>
            {
                selectedRow = row;
                selectedCol = col;
                string value = ReadCell(grid, row, col);
                if (grid == normalGrid && col == 4)
                {
                    byte[] title = app.U("按钮列点击");
                    byte[] message = app.U($"点击了按钮列: row={row}, value={value}");
                    byte[] icon = app.U("🔘");
                    EmojiWindowNative.show_message_box_bytes(page, title, title.Length, message, message.Length, icon, icon.Length);
                }
                else if (grid == normalGrid && col == 5)
                {
                    byte[] title = app.U("链接列点击");
                    byte[] message = app.U($"点击了链接列: row={row}, value={value}");
                    byte[] icon = app.U("🔗");
                    EmojiWindowNative.show_message_box_bytes(page, title, title.Length, message, message.Length, icon, icon.Length);
                }
                else if (grid == normalGrid && col == 6)
                {
                    byte[] title = app.U("图片列点击");
                    byte[] message = app.U($"点击了图片列: row={row}, alt={value}");
                    byte[] icon = app.U("🖼️");
                    EmojiWindowNative.show_message_box_bytes(page, title, title.Length, message, message.Length, icon, icon.Length);
                }

                SetState($"🧾 单元格点击: row={row}, col={col}, value={value}");
                RefreshCellReadout("点击后读取单元格");
            }));
            var doubleClickCallback = app.Pin(new EmojiWindowNative.DataGridCellCallback((_, row, col) => SetState($"🖱️ 双击编辑: row={row}, col={col}")));
            var valueChangedCallback = app.Pin(new EmojiWindowNative.DataGridCellCallback((grid, row, col) =>
            {
                selectedRow = row;
                selectedCol = col;
                SetState($"✏️ 单元格值变化: row={row}, col={col}, value={ReadCell(grid, row, col)}");
                RefreshCellReadout("值变化后读取单元格");
            }));
            var selectionChangedCallback = app.Pin(new EmojiWindowNative.DataGridCellCallback((grid, row, col) =>
            {
                selectedRow = row;
                selectedCol = col;
                SetState($"🎯 选中单元格: row={row}, col={col}, value={ReadCell(grid, row, col)}");
                RefreshCellReadout("选中后读取单元格");
            }));
            var headerClickCallback = app.Pin(new EmojiWindowNative.DataGridColumnHeaderCallback((grid, col) =>
            {
                if (grid == normalGrid)
                {
                    ToggleSort(col);
                }
                else
                {
                    SetState($"📑 虚拟表格列表头点击: col={col}");
                }
            }));
            var virtualCallback = app.Pin(new EmojiWindowNative.DataGridVirtualDataCallback((_, row, col, buffer, bufferSize) =>
            {
                byte[] bytes = app.U(PageCommon.BuildVirtualGridText(row, col));
                if (buffer == IntPtr.Zero || bufferSize <= 0)
                {
                    return bytes.Length;
                }

                int copied = Math.Min(bytes.Length, bufferSize);
                System.Runtime.InteropServices.Marshal.Copy(bytes, 0, buffer, copied);
                return copied;
            }));

            EmojiWindowNative.DataGrid_SetCellClickCallback(normalGrid, clickCallback);
            EmojiWindowNative.DataGrid_SetCellClickCallback(virtualGrid, clickCallback);
            EmojiWindowNative.DataGrid_SetCellDoubleClickCallback(normalGrid, doubleClickCallback);
            EmojiWindowNative.DataGrid_SetCellDoubleClickCallback(virtualGrid, doubleClickCallback);
            EmojiWindowNative.DataGrid_SetCellValueChangedCallback(normalGrid, valueChangedCallback);
            EmojiWindowNative.DataGrid_SetSelectionChangedCallback(normalGrid, selectionChangedCallback);
            EmojiWindowNative.DataGrid_SetSelectionChangedCallback(virtualGrid, selectionChangedCallback);
            EmojiWindowNative.DataGrid_SetColumnHeaderClickCallback(normalGrid, headerClickCallback);
            EmojiWindowNative.DataGrid_SetColumnHeaderClickCallback(virtualGrid, headerClickCallback);
            EmojiWindowNative.DataGrid_SetVirtualDataCallback(virtualGrid, virtualCallback);
            EmojiWindowNative.DataGrid_SetVirtualRowCount(virtualGrid, 1_000_000);

            AddToolbarButton(app, toolbar, 10, "➕", "加一行", DemoColors.Blue, AddRow);
            AddToolbarButton(app, toolbar, 112, "🧹", "清空表格", DemoColors.Red, ClearRows);
            AddToolbarButton(app, toolbar, 222, "🚀", "切换虚拟", DemoColors.Purple, ToggleVirtualMode);
            AddToolbarButton(app, toolbar, 332, "🖼️", "定位图片列", DemoColors.Blue, () =>
            {
                SelectCell(0, virtualMode ? 5 : 6);
                SetState(virtualMode ? "🖼️ 已定位到虚拟表格图片列（col=5）。" : "🖼️ 已定位到普通表格图片列（col=6）。");
            });
            AddToolbarButton(app, toolbar, 446, "🔀", "任务排序", DemoColors.Blue, () => ToggleSort(0));
            AddToolbarButton(app, toolbar, 550, "🖱️", "双击编辑", DemoColors.Gray, ToggleDoubleClickEdit);
            AddToolbarButton(app, toolbar, 676, "📤", "导出 CSV", DemoColors.Green, ExportCsv);
            AddToolbarButton(app, toolbar, 784, "🎨", "单元格样式", DemoColors.Orange, ToggleAccentStyle);

            app.Button(1104, 416, 150, 36, "读取表头", "📄", DemoColors.Blue, () => RefreshHeaderReadout("手动读取表头"), page);
            app.Button(1266, 416, 174, 36, "改普通首列", "1", DemoColors.Orange, () => RenameHeader(normalGrid, 0, "🧩 工单", "🗂️ 已修改普通表格首列表头"), page);
            app.Button(1104, 460, 150, 36, "改虚拟末列", "7", DemoColors.Green, () => RenameHeader(virtualGrid, 6, "📝 虚拟说明", "🗂️ 已修改虚拟表格末列表头"), page);
            app.Button(1266, 460, 174, 36, "恢复默认表头", "↩", DemoColors.Gray, RestoreHeaders, page);

            app.Button(1104, 630, 150, 34, "定位 [0,0]", "🎯", DemoColors.Blue, () => SelectCell(0, 0), page);
            app.Button(1266, 630, 174, 34, "定位末列", "📍", DemoColors.Orange, () => SelectCell(0, virtualMode ? 6 : 7), page);
            app.Button(1104, 672, 150, 34, "修改当前格", "📝", DemoColors.Purple, WriteSelectedCell, page);
            app.Button(1266, 672, 174, 34, "切换勾选", "☑️", DemoColors.Green, ToggleSelectedCheck, page);

            app.Button(1104, 744, 104, 34, "表头对齐", "📐", DemoColors.Blue, CycleHeaderAlignment, page);
            app.Button(1218, 744, 104, 34, "内容对齐", "📏", DemoColors.Orange, CycleCellAlignment, page);
            app.Button(1332, 744, 108, 34, "切换表头", "🎨", DemoColors.Gray, ToggleHeaderStyle, page);

            RebuildNormalRows();
            RefreshHeaderReadout("初始表头");
            UpdateModeLabel();
            SelectCell(0, 0);
            SetState("🧾 表格页已升级：高级列、排序、回调、CSV、普通 / 虚拟模式和冻结前 N 列 / 前 N 行都已接入。");
        }

        private static void ApplyCommonGridStyle(IntPtr grid, int columnCount)
        {
            EmojiWindowNative.DataGrid_SetSelectionMode(grid, 0);
            EmojiWindowNative.DataGrid_SetShowGridLines(grid, 1);
            EmojiWindowNative.DataGrid_SetDefaultRowHeight(grid, 40);
            EmojiWindowNative.DataGrid_SetHeaderHeight(grid, 48);
            EmojiWindowNative.DataGrid_SetHeaderStyle(grid, HeaderStyleDark);
            EmojiWindowNative.DataGrid_SetHeaderMultiline(grid, 0);
            for (int i = 0; i < columnCount; i++)
            {
                EmojiWindowNative.DataGrid_SetColumnHeaderAlignment(grid, i, PageCommon.AlignCenter);
                EmojiWindowNative.DataGrid_SetColumnCellAlignment(grid, i, PageCommon.AlignLeft);
            }
            EmojiWindowNative.DataGrid_Refresh(grid);
        }

        private static string AlignmentName(int alignment)
        {
            if (alignment == PageCommon.AlignCenter) return "居中";
            if (alignment == PageCommon.AlignRight) return "靠右";
            return "靠左";
        }

        private static void AddMenuItem(DemoApp app, IntPtr menu, string text, int itemId)
        {
            byte[] bytes = app.U(text);
            EmojiWindowNative.PopupMenuAddItem(menu, bytes, bytes.Length, itemId);
        }

        private static void AddToolbarButton(DemoApp app, IntPtr toolbar, int x, string emoji, string text, uint color, Action action)
            => app.Button(x, 4, 96, 34, text, emoji, color, action, toolbar);

        private sealed class RowSeed
        {
            public RowSeed(string task, bool enabled, string status, string tag, string actionText, string linkText, string imageAlt, string note)
            {
                Task = task;
                Enabled = enabled;
                Status = status;
                Tag = tag;
                ActionText = actionText;
                LinkText = linkText;
                ImageAlt = imageAlt;
                Note = note;
            }

            public string Task { get; }
            public bool Enabled { get; }
            public string Status { get; }
            public string Tag { get; }
            public string ActionText { get; }
            public string LinkText { get; }
            public string ImageAlt { get; }
            public string Note { get; }
        }
    }
}
