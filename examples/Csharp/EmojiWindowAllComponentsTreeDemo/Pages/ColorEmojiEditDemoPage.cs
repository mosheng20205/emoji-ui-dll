using System;
using System.Runtime.InteropServices;

namespace EmojiWindowDemo
{
    internal static class ColorEmojiEditDemoPage
    {
        public static void Build(AllDemoShell shell, IntPtr page)
        {
            var app = shell.App;

            app.GroupBox(16, 16, 980, 248, "🌈 ColorEmojiEditBox 实时属性读取", DemoColors.Border, DemoColors.WindowBg, page);
            app.GroupBox(16, 286, 980, 254, "🛠️ 文本 / 颜色 / 尺寸 / 状态", DemoColors.Border, DemoColors.WindowBg, page);
            app.GroupBox(1020, 212, 444, 328, "✨ 彩色 Emoji 能力说明", DemoColors.Border, DemoColors.WindowBg, page);
            app.GroupBox(16, 558, 1448, 230, "📘 ColorEmojiEditBox API 说明", DemoColors.Border, DemoColors.WindowBg, page);

            IntPtr demoEdit = app.EditBox(56, 120, 460, 40, "🌈 ColorEmojiEditBox：😀🚀📘✅", false, false, false, true, page);
            IntPtr previewEdit = app.EditBox(1044, 262, 380, 40, "🧪 第二组彩色内容：🎯🪄🫧🎉", false, false, false, true, page);
            IntPtr readout = app.Label(40, 184, 920, 56, "等待读取彩色编辑框属性…", DemoColors.Black, DemoColors.WindowBg, 12, PageCommon.AlignLeft, true, page);
            IntPtr stateLabel = app.Label(40, 760, 1360, 22, "彩色 Emoji 编辑框页状态将在这里更新。", DemoColors.Blue, DemoColors.WindowBg, 12, PageCommon.AlignLeft, false, page);
            app.Label(40, 56, 900, 24, "这一页直接用真接口读取彩色 Emoji 编辑框的文本、颜色、位置、字体、启用态和可见态。", DemoColors.Gray, DemoColors.WindowBg, 12, PageCommon.AlignLeft, false, page);
            app.Label(1044, 322, 382, 70, "ColorEmojiEditBox 和普通 EditBox 共享文本/颜色/位置/显示控制接口，但这一页强调的是彩色 emoji 读写后仍保持彩色渲染。", DemoColors.Gray, DemoColors.WindowBg, 12, PageCommon.AlignLeft, true, page);

            string initialText = EmojiWindowNative.ReadUtf8(EmojiWindowNative.GetEditBoxText, demoEdit);
            EmojiWindowNative.GetEditBoxBounds(demoEdit, out int initialX, out int initialY, out int initialWidth, out int initialHeight);
            EmojiWindowNative.GetEditBoxColor(demoEdit, out uint initialFg, out uint initialBg);
            string initialFontName = ReadFontName(demoEdit, out int initialFontSize, out int initialBold, out _, out _);

            void Refresh(string note)
            {
                EmojiWindowNative.GetEditBoxColor(demoEdit, out uint fg, out uint bg);
                EmojiWindowNative.GetEditBoxBounds(demoEdit, out int x, out int y, out int width, out int height);
                string fontName = ReadFontName(demoEdit, out int fontSize, out int bold, out _, out _);
                shell.SetLabelText(
                    readout,
                    $"text={EmojiWindowNative.ReadUtf8(EmojiWindowNative.GetEditBoxText, demoEdit)}  {(EmojiWindowNative.GetEditBoxVisible(demoEdit) == 1 ? "显示" : "隐藏")}/{(EmojiWindowNative.GetEditBoxEnabled(demoEdit) == 1 ? "启用" : "禁用")}\r\n" +
                    $"bounds=({x}, {y}, {width}, {height})  font={(string.IsNullOrEmpty(fontName) ? "default" : fontName)} {fontSize}px bold={bold}\r\n" +
                    $"fg={PageCommon.FormatColor(fg)}  bg={PageCommon.FormatColor(bg)}");
                shell.SetLabelText(stateLabel, note);
                shell.SetStatus(note);
            }

            void SetTextValue(string text, string note)
            {
                byte[] bytes = app.U(text);
                byte[] previewBytes = app.U("🔁 预览区同步： " + text);
                EmojiWindowNative.SetEditBoxText(demoEdit, bytes, bytes.Length);
                EmojiWindowNative.SetEditBoxText(previewEdit, previewBytes, previewBytes.Length);
                Refresh(note);
            }

            void SetColors(uint fg, uint bg, string note)
            {
                EmojiWindowNative.SetEditBoxColor(demoEdit, fg, bg);
                Refresh(note);
            }

            void SetFont(int fontSize, bool bold, string note)
            {
                byte[] bytes = app.U("Segoe UI Emoji");
                EmojiWindowNative.SetEditBoxFont(demoEdit, bytes, bytes.Length, fontSize, bold ? 1 : 0, 0, 0);
                Refresh(note);
            }

            void MoveEdit(int dx = 0, int dy = 0, int dw = 0)
            {
                EmojiWindowNative.GetEditBoxBounds(demoEdit, out int x, out int y, out int width, out int height);
                EmojiWindowNative.SetEditBoxBounds(demoEdit, x + dx, y + dy, width + dw, height);
                Refresh($"彩色编辑框位置/尺寸已更新: dx={dx}, dy={dy}, dw={dw}");
            }

            void Restore()
            {
                byte[] textBytes = app.U(initialText);
                byte[] fontBytes = app.U(initialFontName);
                EmojiWindowNative.SetEditBoxText(demoEdit, textBytes, textBytes.Length);
                EmojiWindowNative.SetEditBoxColor(demoEdit, initialFg, initialBg);
                EmojiWindowNative.SetEditBoxBounds(demoEdit, initialX, initialY, initialWidth, initialHeight);
                EmojiWindowNative.SetEditBoxFont(demoEdit, fontBytes, fontBytes.Length, initialFontSize, initialBold, 0, 0);
                EmojiWindowNative.EnableEditBox(demoEdit, 1);
                EmojiWindowNative.ShowEditBox(demoEdit, 1);
                Refresh("彩色 Emoji 编辑框属性已恢复默认");
            }

            app.Label(40, 326, 180, 22, "📝 文本预设", DemoColors.Black, DemoColors.WindowBg, 15, PageCommon.AlignLeft, false, page);
            app.Button(40, 360, 156, 36, "表情混排", "😀", DemoColors.Blue, () => SetTextValue("😀 Emoji / English / 数字 123 / ✅", "彩色编辑框文本已切到混排模式"), page);
            app.Button(212, 360, 156, 36, "产品文案", "🚀", DemoColors.Green, () => SetTextValue("🚀 EmojiWindow Pro / 支持 Unicode 彩色 emoji", "彩色编辑框文本已切到产品文案"), page);
            app.Button(384, 360, 156, 36, "主题文案", "🎨", DemoColors.Purple, () => SetTextValue("🎨 Light / Dark / Custom theme preview", "彩色编辑框文本已切到主题文案"), page);

            app.Label(40, 414, 180, 22, "🎨 颜色 / 字体", DemoColors.Black, DemoColors.WindowBg, 15, PageCommon.AlignLeft, false, page);
            app.Button(40, 448, 118, 36, "冷色", "💙", DemoColors.Blue, () => SetColors(DemoColors.Blue, DemoColors.LightBlue, "彩色编辑框已切到冷色方案"), page);
            app.Button(172, 448, 118, 36, "暖色", "🧡", DemoColors.Orange, () => SetColors(DemoColors.Orange, DemoColors.Yellow, "彩色编辑框已切到暖色方案"), page);
            app.Button(304, 448, 118, 36, "16px Bold", "🔤", DemoColors.Green, () => SetFont(16, true, "彩色编辑框字体已切到 16px Bold"), page);
            app.Button(436, 448, 118, 36, "13px", "🔡", DemoColors.Gray, () => SetFont(13, false, "彩色编辑框字体已切回 13px"), page);

            app.Label(1044, 406, 180, 22, "📐 布局 / 状态", DemoColors.Black, DemoColors.WindowBg, 15, PageCommon.AlignLeft, false, page);
            app.Button(1044, 440, 118, 36, "右移 80", "➡️", DemoColors.Blue, () => MoveEdit(dx: 80), page);
            app.Button(1176, 440, 118, 36, "下移 20", "⬇️", DemoColors.Green, () => MoveEdit(dy: 20), page);
            app.Button(1308, 440, 118, 36, "加宽 120", "↔️", DemoColors.Orange, () => MoveEdit(dw: 120), page);
            app.Button(1044, 490, 118, 36, "禁用/启用", "🚫", DemoColors.Purple, () =>
            {
                EmojiWindowNative.EnableEditBox(demoEdit, EmojiWindowNative.GetEditBoxEnabled(demoEdit) == 1 ? 0 : 1);
                Refresh("彩色编辑框启用状态已切换");
            }, page);
            app.Button(1176, 490, 118, 36, "显示/隐藏", "👁️", DemoColors.Gray, () =>
            {
                EmojiWindowNative.ShowEditBox(demoEdit, EmojiWindowNative.GetEditBoxVisible(demoEdit) == 1 ? 0 : 1);
                Refresh("彩色编辑框可见状态已切换");
            }, page);
            app.Button(1308, 490, 118, 36, "恢复默认", "↺", DemoColors.Blue, Restore, page);

            app.Label(40, 598, 700, 24, "1. GetEditBoxText / SetEditBoxText：直接读写彩色 emoji 文本。", DemoColors.Black, DemoColors.WindowBg, 12, PageCommon.AlignLeft, false, page);
            app.Label(40, 632, 700, 24, "2. GetEditBoxColor / SetEditBoxColor：读写前景色 / 背景色。", DemoColors.Black, DemoColors.WindowBg, 12, PageCommon.AlignLeft, false, page);
            app.Label(40, 666, 760, 24, "3. GetEditBoxBounds / SetEditBoxBounds：直接调整位置与宽度。", DemoColors.Black, DemoColors.WindowBg, 12, PageCommon.AlignLeft, false, page);
            app.Label(40, 700, 760, 24, "4. EnableEditBox / ShowEditBox：演示启用态和可见态切换。", DemoColors.Black, DemoColors.WindowBg, 12, PageCommon.AlignLeft, false, page);
            app.Label(40, 734, 940, 24, "5. 这一页的重点不是接口差异，而是确认 DLL 的彩色 emoji 文本在真实读写后仍然保持彩色。", DemoColors.Black, DemoColors.WindowBg, 12, PageCommon.AlignLeft, false, page);

            Refresh("彩色 Emoji 编辑框页已加载，可直接测试属性读取与设置");
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
