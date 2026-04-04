using System;
using System.Runtime.InteropServices;

namespace EmojiWindowDemo
{
    internal static class EditBoxDemoPage
    {
        public static void Build(AllDemoShell shell, IntPtr page)
        {
            var app = shell.App;
            byte[] font = app.U("Segoe UI Emoji");
            string memoSeed = "📝 已从主编辑框同步：\r\n1. 这里用于演示多行 EditBox\r\n2. 方便观察换行和主题色\r\n3. 适合作为备注或说明区域";

            app.GroupBox(16, 16, 980, 248, "📝 EditBox 实时属性读取", DemoTheme.Border, DemoTheme.Background, page);
            app.GroupBox(16, 286, 980, 254, "🎨 文本 / 颜色 / 尺寸 / 状态", DemoTheme.Border, DemoTheme.Background, page);
            app.GroupBox(1020, 16, 444, 524, "📋 多行 EditBox 预览", DemoTheme.Border, DemoTheme.Background, page);
            app.GroupBox(16, 558, 1448, 230, "🔧 EditBox API 说明", DemoTheme.Border, DemoTheme.Background, page);

            IntPtr demoEdit = app.EditBox(56, 120, 420, 38, "📝 单行 EditBox：可以直接读取和修改文本。", false, false, false, false, page);
            byte[] memoBytes = app.U(memoSeed);
            IntPtr memo = EmojiWindowNative.CreateEditBox(
                page,
                1044,
                72,
                396,
                244,
                memoBytes,
                memoBytes.Length,
                DemoTheme.Text,
                DemoTheme.Background,
                font,
                font.Length,
                13,
                0,
                0,
                0,
                PageCommon.AlignLeft,
                1,
                0,
                0,
                1,
                0);

            app.Label(40, 56, 900, 24, "这一页直接读取文本、颜色、位置、字体、启用态和可见态，不再只是放一个输入框。", DemoTheme.Muted, DemoTheme.Background, 12, PageCommon.AlignLeft, false, page);
            IntPtr readout = app.Label(40, 184, 920, 88, "等待读取 EditBox 属性。", DemoTheme.Text, DemoTheme.Background, 12, PageCommon.AlignLeft, true, page);
            IntPtr previewHint = app.Label(1044, 336, 380, 52, "右侧只保留多行 EditBox 和布局控制，不再混入无关标签，层次和 Python 版保持一致。", DemoTheme.Muted, DemoTheme.Background, 12, PageCommon.AlignLeft, true, page);
            IntPtr stateLabel = app.Label(40, 760, 1360, 22, "EditBox 页面状态会显示在这里。", DemoTheme.Primary, DemoTheme.Background, 12, PageCommon.AlignLeft, false, page);

            string initialText = EmojiWindowNative.ReadUtf8(EmojiWindowNative.GetEditBoxText, demoEdit);
            EmojiWindowNative.GetEditBoxBounds(demoEdit, out int initialX, out int initialY, out int initialWidth, out int initialHeight);
            EmojiWindowNative.GetEditBoxColor(demoEdit, out uint initialFg, out uint initialBg);
            string initialFontName = ReadFontName(demoEdit, out int initialFontSize, out int initialBold, out _, out _);
            string colorMode = "theme";
            string lastGoodText = initialText;

            string ReadStableText()
            {
                string text = EmojiWindowNative.ReadUtf8(EmojiWindowNative.GetEditBoxText, demoEdit);
                if (string.IsNullOrEmpty(text) || text.IndexOf('\uFFFD') >= 0)
                {
                    return lastGoodText;
                }

                lastGoodText = text;
                return text;
            }

            void Refresh(string note)
            {
                string text = ReadStableText();
                EmojiWindowNative.GetEditBoxColor(demoEdit, out uint fg, out uint bg);
                EmojiWindowNative.GetEditBoxBounds(demoEdit, out int x, out int y, out int width, out int height);
                string fontName = ReadFontName(demoEdit, out int fontSize, out int bold, out _, out _);

                shell.SetLabelText(
                    readout,
                    $"文本长度={text.Length}  预览请直接查看上方输入框  {(EmojiWindowNative.GetEditBoxVisible(demoEdit) == 1 ? "显示" : "隐藏")}/{(EmojiWindowNative.GetEditBoxEnabled(demoEdit) == 1 ? "启用" : "禁用")}\r\n" +
                    $"bounds=({x}, {y}, {width}, {height})  align={PageCommon.AlignmentName(EmojiWindowNative.GetEditBoxAlignment(demoEdit))}  font={(string.IsNullOrEmpty(fontName) ? "default" : fontName)} {fontSize}px bold={bold}\r\n" +
                    $"fg={PageCommon.FormatColor(fg)}  bg={PageCommon.FormatColor(bg)}");
                shell.SetLabelText(stateLabel, note);
                shell.SetStatus(note);
            }

            void SetTextValue(string text, string note)
            {
                byte[] textBytes = app.U(text);
                byte[] syncBytes = app.U("📝 已从主编辑框同步：\r\n" + text);
                EmojiWindowNative.SetEditBoxText(demoEdit, textBytes, textBytes.Length);
                EmojiWindowNative.SetEditBoxText(memo, syncBytes, syncBytes.Length);
                lastGoodText = text;
                Refresh(note);
            }

            void ApplyEditColors()
            {
                switch (colorMode)
                {
                    case "cool":
                        EmojiWindowNative.SetEditBoxColor(demoEdit, DemoTheme.Primary, DemoTheme.SurfacePrimary);
                        break;
                    case "warm":
                        EmojiWindowNative.SetEditBoxColor(demoEdit, DemoTheme.Warning, DemoTheme.SurfaceWarning);
                        break;
                    default:
                        EmojiWindowNative.SetEditBoxColor(demoEdit, DemoTheme.Text, DemoTheme.Background);
                        break;
                }
            }

            void SetFont(string fontName, int fontSize, bool bold, string note)
            {
                byte[] fontNameBytes = app.U(fontName);
                EmojiWindowNative.GetEditBoxBounds(demoEdit, out int x, out int y, out int width, out int height);
                EmojiWindowNative.SetEditBoxBounds(demoEdit, x, y, width, fontSize >= 16 ? 44 : height);
                EmojiWindowNative.SetEditBoxFont(demoEdit, fontNameBytes, fontNameBytes.Length, fontSize, bold ? 1 : 0, 0, 0);
                Refresh(note);
            }

            void MoveEdit(int dx = 0, int dy = 0, int dw = 0)
            {
                EmojiWindowNative.GetEditBoxBounds(demoEdit, out int x, out int y, out int width, out int height);
                EmojiWindowNative.SetEditBoxBounds(demoEdit, x + dx, y + dy, width + dw, height);
                Refresh($"EditBox 位置或尺寸已更新: dx={dx}, dy={dy}, dw={dw}");
            }

            void Restore()
            {
                byte[] textBytes = app.U(initialText);
                byte[] fontNameBytes = app.U(string.IsNullOrEmpty(initialFontName) ? "Segoe UI Emoji" : initialFontName);
                byte[] restoreMemo = app.U(memoSeed);

                EmojiWindowNative.SetEditBoxText(demoEdit, textBytes, textBytes.Length);
                EmojiWindowNative.SetEditBoxText(memo, restoreMemo, restoreMemo.Length);
                EmojiWindowNative.SetEditBoxColor(demoEdit, initialFg, initialBg);
                EmojiWindowNative.SetEditBoxBounds(demoEdit, initialX, initialY, initialWidth, initialHeight);
                EmojiWindowNative.SetEditBoxFont(demoEdit, fontNameBytes, fontNameBytes.Length, initialFontSize, initialBold, 0, 0);
                EmojiWindowNative.EnableEditBox(demoEdit, 1);
                EmojiWindowNative.ShowEditBox(demoEdit, 1);
                colorMode = "theme";
                lastGoodText = initialText;
                Refresh("EditBox 属性已恢复默认。");
            }

            app.Label(40, 326, 180, 22, "🧾 文本预设", DemoTheme.Text, DemoTheme.Background, 15, PageCommon.AlignLeft, false, page);
            app.Button(40, 360, 156, 36, "写入表单文案", "🪪", DemoColors.Blue, () => SetTextValue("请输入项目名称 / 关键词 / 标题", "EditBox 文本已切换到表单模式。"), page);
            app.Button(212, 360, 156, 36, "写入混排文案", "🌟", DemoColors.Green, () => SetTextValue("🌟 EmojiWindow 支持 emoji / English / 数字 123", "EditBox 文本已切换到混排模式。"), page);
            app.Button(384, 360, 156, 36, "同步到多行框", "📋", DemoColors.Purple, () =>
            {
                byte[] syncBytes = app.U("📝 已从主编辑框同步：\r\n" + ReadStableText());
                EmojiWindowNative.SetEditBoxText(memo, syncBytes, syncBytes.Length);
                Refresh("当前单行内容已同步到右侧多行 EditBox。");
            }, page);

            app.Label(40, 414, 180, 22, "🎨 颜色 / 字体", DemoTheme.Text, DemoTheme.Background, 15, PageCommon.AlignLeft, false, page);
            app.Button(40, 448, 118, 36, "冷色", "❄", DemoColors.Blue, () =>
            {
                colorMode = "cool";
                ApplyEditColors();
                Refresh("EditBox 已切换到冷色方案。");
            }, page);
            app.Button(172, 448, 118, 36, "暖色", "☀", DemoColors.Orange, () =>
            {
                colorMode = "warm";
                ApplyEditColors();
                Refresh("EditBox 已切换到暖色方案。");
            }, page);
            app.Button(304, 448, 118, 36, "16px Bold", "🔤", DemoColors.Green, () => SetFont("Segoe UI Emoji", 16, true, "EditBox 字体已切换到 16px Bold。"), page);
            app.Button(436, 448, 118, 36, "13px", "📝", DemoColors.Gray, () => SetFont("Segoe UI Emoji", 13, false, "EditBox 字体已切回 13px。"), page);

            app.Label(1044, 404, 180, 22, "📐 布局 / 状态", DemoTheme.Text, DemoTheme.Background, 15, PageCommon.AlignLeft, false, page);
            app.Button(1044, 438, 112, 32, "右移 80", "➡", DemoColors.Blue, () => MoveEdit(dx: 80), page);
            app.Button(1168, 438, 112, 32, "下移 24", "⬇", DemoColors.Green, () => MoveEdit(dy: 24), page);
            app.Button(1292, 438, 132, 32, "加宽 120", "↔", DemoColors.Orange, () => MoveEdit(dw: 120), page);
            app.Button(1044, 478, 112, 32, "禁用/启用", "🚫", DemoColors.Purple, () =>
            {
                EmojiWindowNative.EnableEditBox(demoEdit, EmojiWindowNative.GetEditBoxEnabled(demoEdit) == 1 ? 0 : 1);
                Refresh("EditBox 启用状态已切换。");
            }, page);
            app.Button(1168, 478, 112, 32, "显示/隐藏", "👁", DemoColors.Gray, () =>
            {
                EmojiWindowNative.ShowEditBox(demoEdit, EmojiWindowNative.GetEditBoxVisible(demoEdit) == 1 ? 0 : 1);
                Refresh("EditBox 可见状态已切换。");
            }, page);
            app.Button(1292, 478, 132, 32, "恢复默认", "↺", DemoColors.Blue, Restore, page);

            app.Label(40, 598, 1040, 24, "1. GetEditBoxText / SetEditBoxText：读取和写入输入框文本。", DemoTheme.Text, DemoTheme.Background, 12, PageCommon.AlignLeft, false, page);
            app.Label(40, 632, 1040, 24, "2. GetEditBoxColor / SetEditBoxColor：读取和切换前景色、背景色。", DemoTheme.Text, DemoTheme.Background, 12, PageCommon.AlignLeft, false, page);
            app.Label(40, 666, 1040, 24, "3. GetEditBoxBounds / SetEditBoxBounds：直接调整控件的位置与宽度。", DemoTheme.Text, DemoTheme.Background, 12, PageCommon.AlignLeft, false, page);
            app.Label(40, 700, 1040, 24, "4. GetEditBoxFont / SetEditBoxFont：读取和切换字体名、字号与粗体。", DemoTheme.Text, DemoTheme.Background, 12, PageCommon.AlignLeft, false, page);
            app.Label(40, 734, 1040, 24, "5. EnableEditBox / ShowEditBox：演示启用态和可见态切换。", DemoTheme.Text, DemoTheme.Background, 12, PageCommon.AlignLeft, false, page);

            void ApplyTheme()
            {
                ApplyEditColors();
                EmojiWindowNative.SetEditBoxColor(memo, DemoTheme.Text, DemoTheme.Background);
                shell.SetLabelText(previewHint, shell.Palette.Dark
                    ? "右侧保留真实多行 EditBox 和布局控制，深色主题下也不会再出现读数区乱码或层次发花。"
                    : "右侧保留真实多行 EditBox 和布局控制，浅色主题下的卡片层次与 Python 版保持一致。");
            }

            shell.RegisterPageThemeHandler(page, ApplyTheme);
            ApplyTheme();
            Refresh("EditBox 页面已整理：读数区改为稳定 UTF-8 文本，并收紧了右侧层次。");
        }

        private static string ReadFontName(IntPtr hEdit, out int fontSize, out int bold, out int italic, out int underline)
        {
            int nameLength = EmojiWindowNative.GetEditBoxFont(hEdit, IntPtr.Zero, 0, out fontSize, out bold, out italic, out underline);
            if (nameLength <= 0)
            {
                return string.Empty;
            }

            IntPtr buffer = Marshal.AllocHGlobal(nameLength);
            try
            {
                EmojiWindowNative.GetEditBoxFont(hEdit, buffer, nameLength, out fontSize, out bold, out italic, out underline);
                byte[] bytes = new byte[nameLength];
                Marshal.Copy(buffer, bytes, 0, nameLength);
                return EmojiWindowNative.FromUtf8(bytes);
            }
            finally
            {
                Marshal.FreeHGlobal(buffer);
            }
        }
    }
}
