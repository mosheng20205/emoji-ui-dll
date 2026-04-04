using System;
using System.Runtime.InteropServices;

namespace EmojiWindowDemo
{
    internal static class ColorEmojiEditDemoPage
    {
        public static void Build(AllDemoShell shell, IntPtr page)
        {
            var app = shell.App;
            byte[] font = app.U("Segoe UI Emoji");
            byte[] previewInitialBytes = app.U("📎 第二组彩色内容：\r\nEmojiWindow Pro / 支持 Unicode 彩色 emoji\r\n多行彩色 Emoji 预览中");

            app.GroupBox(16, 16, 980, 248, "🌈 ColorEmojiEditBox 实时属性读取", DemoTheme.Border, DemoTheme.Background, page);
            app.GroupBox(16, 286, 980, 254, "🎨 文本 / 颜色 / 尺寸 / 状态", DemoTheme.Border, DemoTheme.Background, page);
            app.GroupBox(1020, 16, 444, 524, "📋 彩色 Emoji 多行预览", DemoTheme.Border, DemoTheme.Background, page);
            app.GroupBox(16, 558, 1448, 230, "🔧 ColorEmojiEditBox API 说明", DemoTheme.Border, DemoTheme.Background, page);

            byte[] demoBytes = app.U("🌈 EmojiWindow Pro / 支持 Unicode 彩色 emoji");
            IntPtr demoEdit = EmojiWindowNative.CreateColorEmojiEditBox(
                page,
                56,
                120,
                460,
                40,
                demoBytes,
                demoBytes.Length,
                DemoTheme.Text,
                DemoTheme.Background,
                font,
                font.Length,
                13,
                0,
                0,
                0,
                PageCommon.AlignLeft,
                0,
                0,
                0,
                1,
                1);
            IntPtr previewEdit = EmojiWindowNative.CreateColorEmojiEditBox(
                page,
                1044,
                72,
                396,
                132,
                previewInitialBytes,
                previewInitialBytes.Length,
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
            IntPtr readout = app.Label(40, 184, 920, 60, "等待读取 ColorEmojiEditBox 属性。", DemoTheme.Text, DemoTheme.Background, 12, PageCommon.AlignLeft, true, page);
            IntPtr stateLabel = app.Label(40, 760, 1360, 22, "ColorEmojiEditBox 页面状态会显示在这里。", DemoTheme.Primary, DemoTheme.Background, 12, PageCommon.AlignLeft, false, page);
            IntPtr intro = app.Label(40, 56, 920, 24, "这一页专门验证彩色 emoji 文本的读取、配色、位置、字体、启用态和可见态。", DemoTheme.Muted, DemoTheme.Background, 12, PageCommon.AlignLeft, false, page);
            IntPtr previewHint = app.Label(1044, 222, 382, 88, "右侧保留真正的多行彩色预览，层次与 Python 版 ColorEmoji 页面统一。", DemoTheme.Muted, DemoTheme.Background, 12, PageCommon.AlignLeft, true, page);

            string initialText = EmojiWindowNative.ReadUtf8(EmojiWindowNative.GetEditBoxText, demoEdit);
            string previewInitial = EmojiWindowNative.ReadUtf8(EmojiWindowNative.GetEditBoxText, previewEdit);
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
                byte[] bytes = app.U(text);
                byte[] previewBytes = app.U("📎 第二组彩色内容：\r\n" + text + "\r\n多行彩色 Emoji 预览中");
                EmojiWindowNative.SetEditBoxText(demoEdit, bytes, bytes.Length);
                EmojiWindowNative.SetEditBoxText(previewEdit, previewBytes, previewBytes.Length);
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

            void SetFontValue(int fontSize, bool bold, string note)
            {
                byte[] fontBytes = app.U("Segoe UI Emoji");
                EmojiWindowNative.GetEditBoxBounds(demoEdit, out int x, out int y, out int width, out int height);
                EmojiWindowNative.SetEditBoxBounds(demoEdit, x, y, width, fontSize >= 16 ? 44 : initialHeight);
                EmojiWindowNative.SetEditBoxFont(demoEdit, fontBytes, fontBytes.Length, fontSize, bold ? 1 : 0, 0, 0);
                Refresh(note);
            }

            void MoveBox(int dx = 0, int dy = 0, int dw = 0)
            {
                EmojiWindowNative.GetEditBoxBounds(demoEdit, out int x, out int y, out int width, out int height);
                EmojiWindowNative.SetEditBoxBounds(demoEdit, x + dx, y + dy, width + dw, height);
                Refresh($"ColorEmojiEditBox 位置或尺寸已更新: dx={dx}, dy={dy}, dw={dw}");
            }

            void Restore()
            {
                byte[] textBytes = app.U(initialText);
                byte[] previewBytes = app.U(previewInitial);
                byte[] fontBytes = app.U(string.IsNullOrEmpty(initialFontName) ? "Segoe UI Emoji" : initialFontName);

                EmojiWindowNative.SetEditBoxText(demoEdit, textBytes, textBytes.Length);
                EmojiWindowNative.SetEditBoxText(previewEdit, previewBytes, previewBytes.Length);
                EmojiWindowNative.SetEditBoxColor(demoEdit, initialFg, initialBg);
                EmojiWindowNative.SetEditBoxBounds(demoEdit, initialX, initialY, initialWidth, initialHeight);
                EmojiWindowNative.SetEditBoxFont(demoEdit, fontBytes, fontBytes.Length, initialFontSize, initialBold, 0, 0);
                EmojiWindowNative.EnableEditBox(demoEdit, 1);
                EmojiWindowNative.ShowEditBox(demoEdit, 1);
                colorMode = "theme";
                lastGoodText = initialText;
                Refresh("ColorEmojiEditBox 属性已恢复默认。");
            }

            app.Label(40, 326, 180, 22, "🌈 文本预设", DemoTheme.Text, DemoTheme.Background, 15, PageCommon.AlignLeft, false, page);
            app.Button(40, 360, 156, 36, "表情混排", "🌟", DemoColors.Blue, () => SetTextValue("Emoji / English / 数字 123 / 混排", "ColorEmojiEditBox 文本已切换到混排模式。"), page);
            app.Button(212, 360, 156, 36, "产品文案", "🪪", DemoColors.Green, () => SetTextValue("EmojiWindow Pro / 支持 Unicode 彩色 emoji", "ColorEmojiEditBox 文本已切换到产品文案。"), page);
            app.Button(384, 360, 156, 36, "主题文案", "🎛", DemoColors.Purple, () => SetTextValue("Light / Dark / Custom theme preview", "ColorEmojiEditBox 文本已切换到主题文案。"), page);

            app.Label(40, 414, 180, 22, "🎨 颜色 / 字体", DemoTheme.Text, DemoTheme.Background, 15, PageCommon.AlignLeft, false, page);
            app.Button(40, 448, 118, 36, "冷色", "❄", DemoColors.Blue, () =>
            {
                colorMode = "cool";
                ApplyEditColors();
                Refresh("ColorEmojiEditBox 已切换到冷色方案。");
            }, page);
            app.Button(172, 448, 118, 36, "暖色", "☀", DemoColors.Orange, () =>
            {
                colorMode = "warm";
                ApplyEditColors();
                Refresh("ColorEmojiEditBox 已切换到暖色方案。");
            }, page);
            app.Button(304, 448, 118, 36, "16px Bold", "🔤", DemoColors.Green, () => SetFontValue(16, true, "ColorEmojiEditBox 字体已切换到 16px Bold。"), page);
            app.Button(436, 448, 118, 36, "13px", "📝", DemoColors.Gray, () => SetFontValue(13, false, "ColorEmojiEditBox 字体已切回 13px。"), page);

            app.Label(1044, 352, 180, 22, "📐 布局 / 状态", DemoTheme.Text, DemoTheme.Background, 15, PageCommon.AlignLeft, false, page);
            app.Button(1044, 386, 116, 32, "右移 80", "➡", DemoColors.Blue, () => MoveBox(dx: 80), page);
            app.Button(1172, 386, 116, 32, "下移 20", "⬇", DemoColors.Green, () => MoveBox(dy: 20), page);
            app.Button(1300, 386, 124, 32, "加宽 120", "↔", DemoColors.Orange, () => MoveBox(dw: 120), page);
            app.Button(1044, 426, 116, 32, "禁用/启用", "🚫", DemoColors.Purple, () =>
            {
                EmojiWindowNative.EnableEditBox(demoEdit, EmojiWindowNative.GetEditBoxEnabled(demoEdit) == 1 ? 0 : 1);
                Refresh("ColorEmojiEditBox 启用状态已切换。");
            }, page);
            app.Button(1172, 426, 116, 32, "显示/隐藏", "👁", DemoColors.Gray, () =>
            {
                EmojiWindowNative.ShowEditBox(demoEdit, EmojiWindowNative.GetEditBoxVisible(demoEdit) == 1 ? 0 : 1);
                Refresh("ColorEmojiEditBox 可见状态已切换。");
            }, page);
            app.Button(1300, 426, 124, 32, "恢复默认", "↺", DemoColors.Blue, Restore, page);

            app.Label(40, 598, 940, 24, "1. GetEditBoxText / SetEditBoxText：直接读写彩色 emoji 文本，并同步到右侧多行预览。", DemoTheme.Text, DemoTheme.Background, 12, PageCommon.AlignLeft, false, page);
            app.Label(40, 632, 940, 24, "2. GetEditBoxColor / SetEditBoxColor：读取前景色 / 背景色，冷暖方案会直接回写主编辑框。", DemoTheme.Text, DemoTheme.Background, 12, PageCommon.AlignLeft, false, page);
            app.Label(40, 666, 940, 24, "3. GetEditBoxBounds / SetEditBoxBounds：直接调整位置与宽度。", DemoTheme.Text, DemoTheme.Background, 12, PageCommon.AlignLeft, false, page);
            app.Label(40, 700, 940, 24, "4. GetEditBoxFont / SetEditBoxFont：读取和切换字体名、字号和粗体。", DemoTheme.Text, DemoTheme.Background, 12, PageCommon.AlignLeft, false, page);
            app.Label(40, 734, 940, 24, "5. 重点不是接口差异，而是验证 DLL 的彩色 emoji 文本在真实读写后仍保持彩色。", DemoTheme.Text, DemoTheme.Background, 12, PageCommon.AlignLeft, false, page);

            void ApplyTheme()
            {
                if (colorMode == "theme")
                {
                    EmojiWindowNative.SetEditBoxColor(demoEdit, DemoTheme.Text, DemoTheme.Background);
                }

                EmojiWindowNative.SetEditBoxColor(previewEdit, DemoTheme.Text, DemoTheme.Background);
                shell.SetLabelText(intro, shell.Palette.Dark
                    ? "这一页专门验证彩色 emoji 文本的读取、配色、位置、字体、启用态和可见态，深色主题下也保持稳定。"
                    : "这一页专门验证彩色 emoji 文本的读取、配色、位置、字体、启用态和可见态。");
                shell.SetLabelText(previewHint, shell.Palette.Dark
                    ? "右侧保留真正的多行彩色预览，深色主题下也不会退化成普通单行输入框。"
                    : "右侧保留真正的多行彩色预览，层次与 Python 版 ColorEmoji 页面统一。");
                Refresh("ColorEmojiEditBox 页面已整理：右侧保持多行彩色预览。");
            }

            shell.RegisterPageThemeHandler(page, ApplyTheme);
            ApplyTheme();
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
