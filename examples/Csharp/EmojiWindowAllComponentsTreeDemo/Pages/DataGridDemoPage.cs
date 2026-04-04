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

            app.GroupBox(16, 16, 1448, 192, "🧩 DataGridView 属性读取 / 主操作工具栏", DemoTheme.Border, DemoTheme.Background, page);
            app.GroupBox(16, 224, 1048, 500, "📋 全列型 DataGridView 演示", DemoTheme.Border, DemoTheme.Background, page);
            app.GroupBox(1080, 224, 384, 236, "📚 表头 / 列操作", DemoTheme.Border, DemoTheme.Background, page);
            app.GroupBox(1080, 472, 384, 156, "🧪 单元格 / 定位", DemoTheme.Border, DemoTheme.Background, page);
            app.GroupBox(1080, 640, 384, 84, "🎨 勾选 / 对齐 / 表头样式", DemoTheme.Border, DemoTheme.Background, page);

            IntPtr modeLabel = app.Label(40, 48, 250, 24, "当前模式: 普通表格", DemoTheme.Primary, DemoTheme.Background, 13, PageCommon.AlignLeft, false, page);
            IntPtr stateLabel = app.Label(296, 48, 760, 58, "等待表格状态。", DemoTheme.Muted, DemoTheme.Background, 12, PageCommon.AlignLeft, true, page);
            IntPtr exportLabel = app.Label(1096, 48, 328, 58, "CSV 尚未导出。", DemoTheme.Muted, DemoTheme.Background, 12, PageCommon.AlignLeft, true, page);
            app.Label(40, 116, 1180, 18, "这一页只保留表格相关能力：普通 / 虚拟表格、排序、单元格读写、表头读写、对齐切换和 CSV 导出。", DemoTheme.Muted, DemoTheme.Background, 12, PageCommon.AlignLeft, true, page);

            IntPtr toolbar = app.Panel(40, 146, 1384, 42, DemoTheme.Surface, page);
            app.Label(40, 256, 960, 24, "左侧是完整表格区，支持普通表格与 1,000,000 行虚拟表格切换，并绑定右键菜单。", DemoTheme.Muted, DemoTheme.Background, 12, PageCommon.AlignLeft, true, page);
            app.Label(0, 0, 1, 1, string.Empty, DemoTheme.Muted, DemoTheme.Background, 12, PageCommon.AlignLeft, true, page);
            IntPtr headerReadout = app.Label(1096, 252, 344, 88, "蓝=读取两套表头；橙=改普通表首列；绿=改虚拟表末列；灰=恢复默认。", DemoTheme.Muted, DemoTheme.Surface, 11, PageCommon.AlignLeft, true, page);

            string[] normalHeaders = { "📝 任务", "☑️ 启用", "📍 状态", "🏷️ 标签", "🔘 动作", "🔗 链接", "🖼️ 图片", "🗒️ 备注" };
            string[] virtualHeaders = { "📝 序号", "📍 状态", "🏷️ 优先级", "👁 节点", "🛠 路由", "🖼️ 图片", "🗒️ 虚拟备注" };
            (string title, Func<IntPtr, byte[], int, int, int> add, int width)[] normalColumns =
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

            IntPtr normalGrid = EmojiWindowNative.CreateDataGridView(page, 36, 292, 1008, 420, 0, 1, DemoTheme.Text, DemoTheme.Background);
            IntPtr virtualGrid = EmojiWindowNative.CreateDataGridView(page, 36, 292, 1008, 420, 1, 1, DemoTheme.Text, DemoTheme.Background);
            EmojiWindowNative.DataGrid_Show(virtualGrid, 0);

            foreach (var column in normalColumns)
            {
                byte[] text = app.U(column.title);
                column.add(normalGrid, text, text.Length, column.width);
            }

            for (int i = 0; i < virtualHeaders.Length; i++)
            {
                byte[] text = app.U(virtualHeaders[i]);
                EmojiWindowNative.DataGrid_AddTextColumn(virtualGrid, text, text.Length, virtualWidths[i]);
            }

            var rows = new List<RowSeed>
            {
                new RowSeed("📝 任务 1", true, "🟡 进行中", "🔵 P1", "执行", "查看详情", "图片-A", "支持下拉、勾选和标签色块"),
                new RowSeed("📝 任务 2", false, "🕓 待处理", "🟢 P2", "审核", "打开文档", "图片-B", "支持按钮列、链接列和图片列"),
                new RowSeed("📝 任务 3", true, "✅ 已完成", "🟠 P3", "归档", "查看报告", "图片-C", "支持排序、导出 CSV 与表头读写"),
            };
            string[] imagePaths =
            {
                PageCommon.FindFirstExistingPath(app, @"imgs\1.png"),
                PageCommon.FindFirstExistingPath(app, @"imgs\2.png"),
                PageCommon.FindFirstExistingPath(app, @"imgs\3.png"),
            };

            bool virtualMode = false;
            bool headerDark = true;
            bool accentStyle = false;
            bool doubleClickEnabled = true;
            int selectedRow = 0;
            int selectedCol = 0;
            int headerAlignment = PageCommon.AlignCenter;
            int cellAlignment = PageCommon.AlignLeft;
            string lastCellSnapshot = string.Empty;
            var sortStates = new Dictionary<int, int>();

            void SetState(string text)
            {
                shell.SetLabelText(stateLabel, text);
                shell.SetStatus(text);
            }

            IntPtr ActiveGrid() => virtualMode ? virtualGrid : normalGrid;

            string ReadHeader(IntPtr grid, int index) => EmojiWindowNative.ReadUtf8(EmojiWindowNative.DataGrid_GetColumnHeaderText, grid, index);
            string ReadCell(IntPtr grid, int row, int col) => grid == virtualGrid ? PageCommon.BuildVirtualGridText(row, col) : EmojiWindowNative.ReadUtf8(EmojiWindowNative.DataGrid_GetCellText, grid, row, col);

            void ApplyTheme()
            {
                EmojiWindowNative.SetPanelBackgroundColor(toolbar, shell.Palette.CardBackground);
                uint selectColor = shell.Palette.Dark
                    ? EmojiWindowNative.ARGB(255, 52, 95, 168)
                    : EmojiWindowNative.ARGB(255, 186, 224, 255);
                uint hoverColor = shell.Palette.Dark
                    ? EmojiWindowNative.ARGB(255, 42, 47, 57)
                    : EmojiWindowNative.ARGB(255, 236, 243, 252);
                EmojiWindowNative.DataGrid_SetColors(normalGrid, DemoTheme.Text, DemoTheme.Background, DemoTheme.Surface, DemoTheme.Text, selectColor, hoverColor, DemoTheme.BorderLight);
                EmojiWindowNative.DataGrid_SetColors(virtualGrid, DemoTheme.Text, DemoTheme.Background, DemoTheme.Surface, DemoTheme.Text, selectColor, hoverColor, DemoTheme.BorderLight);
                EmojiWindowNative.DataGrid_SetHeaderStyle(normalGrid, headerDark ? HeaderStyleDark : HeaderStylePlain);
                EmojiWindowNative.DataGrid_SetHeaderStyle(virtualGrid, headerDark ? HeaderStyleDark : HeaderStylePlain);
                EmojiWindowNative.DataGrid_SetDoubleClickEnabled(normalGrid, doubleClickEnabled ? 1 : 0);
                EmojiWindowNative.DataGrid_Refresh(normalGrid);
                EmojiWindowNative.DataGrid_Refresh(virtualGrid);
            }

            void RefreshMode()
            {
                shell.SetLabelText(modeLabel, virtualMode ? "当前模式: 虚拟表格 1,000,000 行" : $"当前模式: 普通表格 {rows.Count} 行");
            }

            void RefreshHeaders(string prefix)
            {
                shell.SetLabelText(
                    headerReadout,
                    $"{prefix}\r\n普通表: {FormatHeaderSummary(ReadAllHeaders(normalGrid), 4)}\r\n虚拟表: {FormatHeaderSummary(ReadAllHeaders(virtualGrid), 4)}");
            }

            IEnumerable<string> ReadAllHeaders(IntPtr grid)
            {
                int count = EmojiWindowNative.DataGrid_GetColumnCount(grid);
                for (int i = 0; i < count; i++)
                {
                    yield return ReadHeader(grid, i);
                }
            }

            string FormatHeaderSummary(IEnumerable<string> headers, int wrapAfter)
            {
                var list = new List<string>(headers);
                if (list.Count == 0)
                {
                    return "(空)";
                }

                var lines = new List<string>();
                for (int i = 0; i < list.Count; i += wrapAfter)
                {
                    lines.Add(string.Join(" | ", list.GetRange(i, Math.Min(wrapAfter, list.Count - i))));
                }

                return string.Join("\r\n", lines);
            }

            void ShowActionButtonInfo(int row, int col, string buttonText)
            {
                string buttonName = string.IsNullOrWhiteSpace(buttonText) ? "(空)" : buttonText;
                string title = "🧩 动作列按钮点击";
                string message = $"点击了第 {row + 1} 行，第 {col + 1} 列\r\n按钮名：{buttonName}";
                byte[] titleBytes = app.U(title);
                byte[] messageBytes = app.U(message);
                byte[] iconBytes = app.U("ℹ️");
                EmojiWindowNative.show_message_box_bytes(app.Window, titleBytes, titleBytes.Length, messageBytes, messageBytes.Length, iconBytes, iconBytes.Length);
            }

            void RefreshCell(string prefix)
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

                string value = ReadCell(grid, row, col);
                lastCellSnapshot = $"{prefix}\r\nrow={row}, col={col}\r\nvalue={(string.IsNullOrEmpty(value) ? "(空)" : value)}{extra}";
            }

            void ApplyRowStyles()
            {
                for (int row = 0; row < rows.Count; row++)
                {
                    uint fg = row % 3 == 0 ? DemoTheme.Primary : row % 3 == 1 ? DemoTheme.Success : DemoTheme.Warning;
                    uint bg = row % 3 == 0 ? DemoTheme.SurfacePrimary : row % 3 == 1 ? DemoTheme.SurfaceSuccess : DemoTheme.SurfaceWarning;
                    EmojiWindowNative.DataGrid_SetCellStyle(normalGrid, row, 3, fg, bg, 0, 0);
                    EmojiWindowNative.DataGrid_SetCellStyle(normalGrid, row, 4, DemoColors.White, row % 2 == 0 ? DemoColors.Blue : DemoColors.Green, 0, 0);
                    EmojiWindowNative.DataGrid_SetCellStyle(normalGrid, row, 5, DemoTheme.Primary, 0, 0, 0);
                    EmojiWindowNative.DataGrid_SetCellStyle(normalGrid, row, 6, fg, bg, 0, 0);
                    EmojiWindowNative.DataGrid_SetCellStyle(normalGrid, row, 0, accentStyle ? DemoColors.Purple : 0, accentStyle ? DemoTheme.SurfaceInfo : 0, accentStyle ? 1 : 0, 0);
                    EmojiWindowNative.DataGrid_SetCellStyle(normalGrid, row, 7, accentStyle ? DemoTheme.Text : 0, accentStyle ? DemoTheme.SurfaceWarning : 0, 0, 0);
                }

                EmojiWindowNative.DataGrid_Refresh(normalGrid);
            }

            void RebuildRows()
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

                    string imagePath = imagePaths[i % imagePaths.Length];
                    if (!string.IsNullOrEmpty(imagePath))
                    {
                        byte[] bytes = app.U(imagePath);
                        EmojiWindowNative.DataGrid_SetCellImageFromFile(normalGrid, row, 6, bytes, bytes.Length);
                    }
                }

                ApplyRowStyles();
                RefreshMode();
                ApplyTheme();
            }

            void ToggleSort(int col)
            {
                if (virtualMode)
                {
                    SetState("虚拟表格不做排序演示，请先切回普通表格。");
                    return;
                }

                int next = !sortStates.ContainsKey(col) || sortStates[col] != 1 ? 1 : 2;
                sortStates[col] = next;
                EmojiWindowNative.DataGrid_SortByColumn(normalGrid, col, next);
                EmojiWindowNative.DataGrid_Refresh(normalGrid);
                SetState($"📊 已按列 {col} {(next == 1 ? "升序" : "降序")} 排序。");
                RefreshCell("排序后读取单元格");
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
                RefreshCell($"已在{(grid == virtualGrid ? "虚拟表" : "普通表")}选中 [{row}, {col}]");
            }

            void RenameHeader(IntPtr grid, int col, string text, string note)
            {
                byte[] bytes = app.U(text);
                EmojiWindowNative.DataGrid_SetColumnHeaderText(grid, col, bytes, bytes.Length);
                EmojiWindowNative.DataGrid_Refresh(grid);
                RefreshHeaders("修改表头后读取");
                SetState(note);
            }

            void RestoreHeaders()
            {
                for (int i = 0; i < normalHeaders.Length; i++)
                {
                    byte[] bytes = app.U(normalHeaders[i]);
                    EmojiWindowNative.DataGrid_SetColumnHeaderText(normalGrid, i, bytes, bytes.Length);
                }

                for (int i = 0; i < virtualHeaders.Length; i++)
                {
                    byte[] bytes = app.U(virtualHeaders[i]);
                    EmojiWindowNative.DataGrid_SetColumnHeaderText(virtualGrid, i, bytes, bytes.Length);
                }

                EmojiWindowNative.DataGrid_Refresh(normalGrid);
                EmojiWindowNative.DataGrid_Refresh(virtualGrid);
                RefreshHeaders("已恢复默认表头");
                SetState("↩ 已恢复默认表头。");
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
                SetState($"✏️ 已把当前选中格 [{row}, {col}] 改成测试文本。");
                RefreshCell("修改后读取当前选中格");
            }

            void ToggleCheck()
            {
                if (virtualMode)
                {
                    SetState("虚拟表格没有勾选列，请先切回普通表格。");
                    return;
                }

                int row = EmojiWindowNative.DataGrid_GetSelectedRow(normalGrid);
                if (row < 0) row = 0;
                bool next = EmojiWindowNative.DataGrid_GetCellChecked(normalGrid, row, 1) == 0;
                EmojiWindowNative.DataGrid_SetCellChecked(normalGrid, row, 1, next ? 1 : 0);
                EmojiWindowNative.DataGrid_Refresh(normalGrid);
                selectedRow = row;
                selectedCol = 1;
                SetState($"☑️ 第 {row} 行勾选状态已切到 {next}。");
                RefreshCell("切换勾选后读取单元格");
            }

            void ToggleVirtual()
            {
                virtualMode = !virtualMode;
                EmojiWindowNative.DataGrid_Show(normalGrid, virtualMode ? 0 : 1);
                EmojiWindowNative.DataGrid_Show(virtualGrid, virtualMode ? 1 : 0);
                RefreshMode();
                RefreshCell("切换模式后读取单元格");
                SetState(virtualMode ? "🚀 已切换到虚拟表格 1,000,000 行。" : "📋 已切回普通表格。");
            }

            void ToggleHeaderStyle()
            {
                headerDark = !headerDark;
                ApplyTheme();
                SetState($"🎛️ 表头样式已切到 {(headerDark ? "Dark" : "Plain")}。");
            }

            void ToggleAccent()
            {
                accentStyle = !accentStyle;
                ApplyRowStyles();
                SetState($"🎨 单元格演示样式已{(accentStyle ? "开启" : "恢复默认")}。");
            }

            void ToggleDoubleClick()
            {
                doubleClickEnabled = !doubleClickEnabled;
                ApplyTheme();
                SetState($"🖋️ 双击编辑已{(doubleClickEnabled ? "开启" : "关闭")}。");
            }

            void CycleHeaderAlign()
            {
                headerAlignment = headerAlignment == PageCommon.AlignLeft ? PageCommon.AlignCenter : headerAlignment == PageCommon.AlignCenter ? PageCommon.AlignRight : PageCommon.AlignLeft;
                int count = EmojiWindowNative.DataGrid_GetColumnCount(ActiveGrid());
                for (int i = 0; i < count; i++) EmojiWindowNative.DataGrid_SetColumnHeaderAlignment(ActiveGrid(), i, headerAlignment);
                EmojiWindowNative.DataGrid_Refresh(ActiveGrid());
                SetState($"📐 表头对齐已切到 {AlignmentName(headerAlignment)}。");
            }

            void CycleCellAlign()
            {
                cellAlignment = cellAlignment == PageCommon.AlignLeft ? PageCommon.AlignCenter : cellAlignment == PageCommon.AlignCenter ? PageCommon.AlignRight : PageCommon.AlignLeft;
                int count = EmojiWindowNative.DataGrid_GetColumnCount(ActiveGrid());
                for (int i = 0; i < count; i++) EmojiWindowNative.DataGrid_SetColumnCellAlignment(ActiveGrid(), i, cellAlignment);
                EmojiWindowNative.DataGrid_Refresh(ActiveGrid());
                SetState($"🧭 单元格对齐已切到 {AlignmentName(cellAlignment)}。");
            }

            void ExportCsv()
            {
                string path = Path.Combine(AppDomain.CurrentDomain.BaseDirectory, "_datagrid_export_demo.csv");
                byte[] bytes = app.U(path);
                bool ok = EmojiWindowNative.DataGrid_ExportCSV(normalGrid, bytes, bytes.Length) != 0;
                shell.SetLabelText(exportLabel, ok ? "CSV 已导出\r\n" + path : "CSV 导出失败。");
                SetState(ok ? "💾 已导出 CSV。" : "CSV 导出失败。");
            }

            ApplyCommonGridStyle(normalGrid, normalHeaders.Length);
            ApplyCommonGridStyle(virtualGrid, virtualHeaders.Length);
            EmojiWindowNative.DataGrid_SetColumnCellAlignment(normalGrid, 1, PageCommon.AlignCenter);
            EmojiWindowNative.DataGrid_SetColumnCellAlignment(normalGrid, 2, PageCommon.AlignCenter);
            EmojiWindowNative.DataGrid_SetColumnCellAlignment(normalGrid, 3, PageCommon.AlignCenter);
            EmojiWindowNative.DataGrid_SetColumnCellAlignment(normalGrid, 4, PageCommon.AlignCenter);
            EmojiWindowNative.DataGrid_SetColumnCellAlignment(normalGrid, 6, PageCommon.AlignCenter);
            byte[] comboItems = app.U("🕓 待处理\n🟡 进行中\n✅ 已完成\n⚪ 已暂停");
            EmojiWindowNative.DataGrid_SetColumnComboItems(normalGrid, 2, comboItems, comboItems.Length);
            EmojiWindowNative.DataGrid_SetFreezeHeader(normalGrid, 1);
            EmojiWindowNative.DataGrid_SetFreezeColumnCount(normalGrid, 2);
            EmojiWindowNative.DataGrid_SetFreezeRowCount(normalGrid, 2);

            const int MenuAdd = 9301;
            const int MenuClear = 9302;
            const int MenuToggle = 9303;
            const int MenuRead = 9304;
            const int MenuHeader = 9305;
            IntPtr popupMenu = EmojiWindowNative.CreateEmojiPopupMenu(page);
            AddMenuItem(app, popupMenu, "➕ 添加一行", MenuAdd);
            AddMenuItem(app, popupMenu, "🧹 清空普通表格", MenuClear);
            AddMenuItem(app, popupMenu, "🚀 切换虚拟表格", MenuToggle);
            AddMenuItem(app, popupMenu, "📖 读取当前单元格", MenuRead);
            AddMenuItem(app, popupMenu, "🎛️ 切换表头样式", MenuHeader);
            var popupCallback = app.Pin(new EmojiWindowNative.MenuItemClickCallback((_, itemId) =>
            {
                if (itemId == MenuAdd) { rows.Add(new RowSeed($"📝 新任务 {rows.Count + 1}", rows.Count % 2 == 0, "🕓 待处理", rows.Count % 2 == 0 ? "🔵 P1" : "🟢 P2", "打开", "查看详情", "图片-N", "运行时追加的数据")); RebuildRows(); SetState("➕ 已新增一行。"); }
                else if (itemId == MenuClear) { rows.Clear(); RebuildRows(); SetState("🧹 已清空普通表格。"); }
                else if (itemId == MenuToggle) ToggleVirtual();
                else if (itemId == MenuRead) RefreshCell("右键菜单读取单元格");
                else if (itemId == MenuHeader) ToggleHeaderStyle();
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
                    SetState($"🔘 动作列按钮点击: 第 {row + 1} 行，第 {col + 1} 列，按钮={value}");
                    ShowActionButtonInfo(row, col, value);
                    return;
                }

                SetState($"📋 单元格点击: row={row}, col={col}, value={value}");
                RefreshCell("点击后读取单元格");
            }));
            var doubleClickCallback = app.Pin(new EmojiWindowNative.DataGridCellCallback((_, row, col) => SetState($"🖋️ 双击编辑: row={row}, col={col}")));
            var valueChangedCallback = app.Pin(new EmojiWindowNative.DataGridCellCallback((grid, row, col) =>
            {
                selectedRow = row;
                selectedCol = col;
                SetState($"✏️ 单元格值变化: row={row}, col={col}, value={ReadCell(grid, row, col)}");
                RefreshCell("值变化后读取单元格");
            }));
            var selectionChangedCallback = app.Pin(new EmojiWindowNative.DataGridCellCallback((grid, row, col) =>
            {
                selectedRow = row;
                selectedCol = col;
                SetState($"🎯 选中单元格: row={row}, col={col}, value={ReadCell(grid, row, col)}");
                RefreshCell("选中后读取单元格");
            }));
            var headerClickCallback = app.Pin(new EmojiWindowNative.DataGridColumnHeaderCallback((grid, col) =>
            {
                if (grid == normalGrid) ToggleSort(col);
                else SetState($"🧭 虚拟表格列表头点击: col={col}");
            }));
            var virtualCallback = app.Pin(new EmojiWindowNative.DataGridVirtualDataCallback((_, row, col, buffer, bufferSize) =>
            {
                byte[] bytes = app.U(PageCommon.BuildVirtualGridText(row, col));
                if (buffer == IntPtr.Zero || bufferSize <= 0) return bytes.Length;
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

            AddToolbarButton(app, toolbar, 10, "➕", "加一行", DemoColors.Blue, () =>
            {
                rows.Add(new RowSeed($"📝 新任务 {rows.Count + 1}", rows.Count % 2 == 0, "🕓 待处理", rows.Count % 2 == 0 ? "🔵 P1" : "🟢 P2", "打开", "查看详情", "图片-N", "运行时追加的数据"));
                RebuildRows();
                SetState("➕ 已新增一行。");
            });
            AddToolbarButton(app, toolbar, 112, "🧹", "清空表格", DemoColors.Red, () =>
            {
                rows.Clear();
                RebuildRows();
                SetState("🧹 已清空普通表格。");
            });
            AddToolbarButton(app, toolbar, 222, "🚀", "切换虚拟", DemoColors.Purple, ToggleVirtual);
            AddToolbarButton(app, toolbar, 332, "🖼️", "定位图片列", DemoColors.Blue, () => SelectCell(0, virtualMode ? 5 : 6));
            AddToolbarButton(app, toolbar, 446, "📊", "任务排序", DemoColors.Blue, () => ToggleSort(0));
            AddToolbarButton(app, toolbar, 550, "🖋️", "双击编辑", DemoColors.Gray, ToggleDoubleClick);
            AddToolbarButton(app, toolbar, 676, "💾", "导出 CSV", DemoColors.Green, ExportCsv);
            AddToolbarButton(app, toolbar, 784, "🎨", "单元格样式", DemoColors.Orange, ToggleAccent);

            app.Button(1104, 348, 150, 34, "读两套表头", "📖", DemoColors.Blue, () => RefreshHeaders("手动读取普通表 + 虚拟表的表头"), page);
            app.Button(1266, 348, 174, 34, "普通首列→工单", "1", DemoColors.Orange, () => RenameHeader(normalGrid, 0, "🧾 工单", "✏️ 已将普通表第 1 列改为“工单”"), page);
            app.Button(1104, 390, 150, 34, "虚拟末列→说明", "7", DemoColors.Green, () => RenameHeader(virtualGrid, 6, "🗒️ 虚拟说明", "✏️ 已将虚拟表最后 1 列改为“虚拟说明”"), page);
            app.Button(1266, 390, 174, 34, "恢复两套表头", "↩", DemoColors.Gray, RestoreHeaders, page);

            app.Button(1104, 510, 150, 34, "选中首格 [0,0]", "🎯", DemoColors.Blue, () => SelectCell(0, 0), page);
            app.Button(1266, 510, 174, 34, "选中当前末列", "📌", DemoColors.Orange, () => SelectCell(0, virtualMode ? 6 : 7), page);
            app.Button(1104, 552, 336, 34, "把当前选中格改成测试文本", "🗒️", DemoColors.Purple, WriteSelectedCell, page);

            app.Button(1104, 674, 78, 32, "勾选", "☑️", DemoColors.Green, ToggleCheck, page);
            app.Button(1190, 674, 78, 32, "头对齐", "📐", DemoColors.Blue, CycleHeaderAlign, page);
            app.Button(1276, 674, 78, 32, "格对齐", "🧭", DemoColors.Orange, CycleCellAlign, page);
            app.Button(1362, 674, 78, 32, "头样式", "🎛️", DemoColors.Gray, ToggleHeaderStyle, page);

            shell.RegisterPageThemeHandler(page, ApplyTheme);
            ApplyTheme();
            RebuildRows();
            RefreshHeaders("初始表头");
            RefreshMode();
            SelectCell(0, 0);
            SetState("📋 表格页已统一成 Python 版的分区结构。");
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

