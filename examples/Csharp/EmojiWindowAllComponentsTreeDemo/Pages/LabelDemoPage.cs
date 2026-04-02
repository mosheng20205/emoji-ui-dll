using System;
using System.Runtime.InteropServices;

namespace EmojiWindowDemo
{
    internal static class LabelDemoPage
    {
        public static void Build(AllDemoShell shell, IntPtr page)
        {
            var app = shell.App;
            app.GroupBox(16, 16, 980, 248, "🏷️ Label 实时属性读取", DemoColors.Border, DemoColors.WindowBg, page);
            app.GroupBox(16, 286, 980, 254, "🛠️ 文本 / 颜色 / 位置 / 字体", DemoColors.Border, DemoColors.WindowBg, page);
            app.GroupBox(1020, 212, 444, 328, "📐 Label 状态开关", DemoColors.Border, DemoColors.WindowBg, page);
            app.GroupBox(16, 558, 1448, 230, "📘 Label API 说明", DemoColors.Border, DemoColors.WindowBg, page);

            IntPtr demoLabel = app.Label(56, 124, 560, 36, "🏷️ 这是主演示标签：支持 Unicode 彩色文案和属性读取。", DemoColors.Black, DemoColors.LightBlue, 15, PageCommon.AlignLeft, false, page);
            IntPtr readout = app.Label(40, 184, 920, 56, "等待读取标签属性…", DemoColors.Black, DemoColors.WindowBg, 12, PageCommon.AlignLeft, true, page);
            IntPtr stateLabel = app.Label(40, 760, 1360, 22, "标签页状态将在这里更新。", DemoColors.Blue, DemoColors.WindowBg, 12, PageCommon.AlignLeft, false, page);
            app.Label(40, 56, 900, 24, "标签页现在直接展示 Label 的文本、颜色、位置、字体、对齐、可见性和启用态读取。", DemoColors.Gray, DemoColors.WindowBg, 12, PageCommon.AlignLeft, false, page);

            string initialText = EmojiWindowNative.ReadUtf8(EmojiWindowNative.GetLabelText, demoLabel);
            EmojiWindowNative.GetLabelBounds(demoLabel, out int initialX, out int initialY, out int initialWidth, out int initialHeight);
            EmojiWindowNative.GetLabelColor(demoLabel, out uint initialFg, out uint initialBg);
            string initialFontName = ReadFontName(demoLabel, out int initialFontSize, out int initialBold, out _, out _);

            void Refresh(string note)
            {
                EmojiWindowNative.GetLabelColor(demoLabel, out uint fg, out uint bg);
                EmojiWindowNative.GetLabelBounds(demoLabel, out int x, out int y, out int width, out int height);
                string fontName = ReadFontName(demoLabel, out int fontSize, out int bold, out _, out _);
                shell.SetLabelText(
                    readout,
                    $"text={EmojiWindowNative.ReadUtf8(EmojiWindowNative.GetLabelText, demoLabel)}  {(EmojiWindowNative.GetLabelVisible(demoLabel) == 1 ? "显示" : "隐藏")}/{(EmojiWindowNative.GetLabelEnabled(demoLabel) == 1 ? "启用" : "禁用")}\r\n" +
                    $"bounds=({x}, {y}, {width}, {height})  align={PageCommon.AlignmentName(EmojiWindowNative.GetLabelAlignment(demoLabel))}  font={(string.IsNullOrEmpty(fontName) ? "default" : fontName)} {fontSize}px bold={bold}\r\n" +
                    $"fg={PageCommon.FormatColor(fg)}  bg={PageCommon.FormatColor(bg)}");
                shell.SetLabelText(stateLabel, note);
                shell.SetStatus(note);
            }

            void SetTextValue(string text, string note)
            {
                byte[] bytes = app.U(text);
                EmojiWindowNative.SetLabelText(demoLabel, bytes, bytes.Length);
                Refresh(note);
            }

            void SetColors(uint fg, uint bg, string note)
            {
                EmojiWindowNative.SetLabelColor(demoLabel, fg, bg);
                Refresh(note);
            }

            void SetFont(string fontName, int fontSize, bool bold, string note)
            {
                byte[] bytes = app.U(fontName);
                EmojiWindowNative.SetLabelFont(demoLabel, bytes, bytes.Length, fontSize, bold ? 1 : 0, 0, 0);
                Refresh(note);
            }

            void MoveLabel(int dx = 0, int dy = 0, int dw = 0)
            {
                EmojiWindowNative.GetLabelBounds(demoLabel, out int x, out int y, out int width, out int height);
                EmojiWindowNative.SetLabelBounds(demoLabel, x + dx, y + dy, width + dw, height);
                Refresh($"标签位置/尺寸已更新: dx={dx}, dy={dy}, dw={dw}");
            }

            void Restore()
            {
                byte[] textBytes = app.U(initialText);
                byte[] fontBytes = app.U(initialFontName);
                EmojiWindowNative.SetLabelText(demoLabel, textBytes, textBytes.Length);
                EmojiWindowNative.SetLabelColor(demoLabel, initialFg, initialBg);
                EmojiWindowNative.SetLabelBounds(demoLabel, initialX, initialY, initialWidth, initialHeight);
                EmojiWindowNative.SetLabelFont(demoLabel, fontBytes, fontBytes.Length, initialFontSize, initialBold, 0, 0);
                EmojiWindowNative.EnableLabel(demoLabel, 1);
                EmojiWindowNative.ShowLabel(demoLabel, 1);
                Refresh("标签属性已恢复默认");
            }

            app.Label(40, 326, 180, 22, "📝 文本预设", DemoColors.Black, DemoColors.WindowBg, 15, PageCommon.AlignLeft, false, page);
            app.Button(40, 360, 146, 36, "说明标签", "💬", DemoColors.Blue, () => SetTextValue("💬 当前标签已切到说明文案模式。", "标签文本已切到说明模式"), page);
            app.Button(202, 360, 146, 36, "强调标签", "📣", DemoColors.Green, () => SetTextValue("📣 当前标签是强调提示，用于展示高关注状态。", "标签文本已切到强调模式"), page);
            app.Button(364, 360, 146, 36, "彩色标签", "🌈", DemoColors.Purple, () => SetTextValue("🌈 Unicode 标签：🚀 ✅ 📘 🧩", "标签文本已切到 Unicode 彩色模式"), page);

            app.Label(40, 414, 180, 22, "🎨 颜色 / 字体", DemoColors.Black, DemoColors.WindowBg, 15, PageCommon.AlignLeft, false, page);
            app.Button(40, 448, 118, 36, "冷色", "💙", DemoColors.Blue, () => SetColors(DemoColors.Blue, DemoColors.LightBlue, "标签已切到冷色方案"), page);
            app.Button(172, 448, 118, 36, "暖色", "🧡", DemoColors.Orange, () => SetColors(DemoColors.Orange, DemoColors.Yellow, "标签已切到暖色方案"), page);
            app.Button(304, 448, 118, 36, "18px Bold", "🔤", DemoColors.Green, () => SetFont("Segoe UI Emoji", 18, true, "标签字体已切到 18px Bold"), page);
            app.Button(436, 448, 118, 36, "14px", "🔡", DemoColors.Gray, () => SetFont("Segoe UI Emoji", 14, false, "标签字体已切到 14px"), page);

            app.Label(1044, 252, 180, 22, "📐 位置 / 状态", DemoColors.Black, DemoColors.WindowBg, 15, PageCommon.AlignLeft, false, page);
            app.Button(1044, 286, 118, 36, "右移 80", "➡️", DemoColors.Blue, () => MoveLabel(dx: 80), page);
            app.Button(1176, 286, 118, 36, "下移 20", "⬇️", DemoColors.Green, () => MoveLabel(dy: 20), page);
            app.Button(1308, 286, 118, 36, "加宽 120", "↔️", DemoColors.Orange, () => MoveLabel(dw: 120), page);
            app.Button(1044, 336, 118, 36, "禁用/启用", "🚫", DemoColors.Purple, () =>
            {
                EmojiWindowNative.EnableLabel(demoLabel, EmojiWindowNative.GetLabelEnabled(demoLabel) == 1 ? 0 : 1);
                Refresh("标签启用状态已切换");
            }, page);
            app.Button(1176, 336, 118, 36, "显示/隐藏", "👁️", DemoColors.Gray, () =>
            {
                EmojiWindowNative.ShowLabel(demoLabel, EmojiWindowNative.GetLabelVisible(demoLabel) == 1 ? 0 : 1);
                Refresh("标签可见状态已切换");
            }, page);
            app.Button(1308, 336, 118, 36, "恢复默认", "↺", DemoColors.Blue, Restore, page);
            app.Label(1044, 392, 382, 56, "当前 DLL 对 Label 提供了对齐方式读取接口 GetLabelAlignment，但没有 SetLabelAlignment 真接口，所以这里明确只做“可读不伪造可写”。", DemoColors.Gray, DemoColors.WindowBg, 12, PageCommon.AlignLeft, true, page);

            app.Label(40, 598, 640, 24, "1. GetLabelText / SetLabelText：读取和修改标签文本。", DemoColors.Black, DemoColors.WindowBg, 12, PageCommon.AlignLeft, false, page);
            app.Label(40, 632, 700, 24, "2. GetLabelColor / SetLabelColor：读取和切换前景色 / 背景色。", DemoColors.Black, DemoColors.WindowBg, 12, PageCommon.AlignLeft, false, page);
            app.Label(40, 666, 720, 24, "3. GetLabelBounds / SetLabelBounds：直接修改标签位置和宽度。", DemoColors.Black, DemoColors.WindowBg, 12, PageCommon.AlignLeft, false, page);
            app.Label(40, 700, 820, 24, "4. GetLabelFont / SetLabelFont：这里用真实字体读写展示字体名、字号和粗体。", DemoColors.Black, DemoColors.WindowBg, 12, PageCommon.AlignLeft, false, page);
            app.Label(40, 734, 720, 24, "5. EnableLabel / ShowLabel：演示启用态和可见态切换。", DemoColors.Black, DemoColors.WindowBg, 12, PageCommon.AlignLeft, false, page);

            Refresh("标签页已加载，可直接测试标签属性读取与设置");
        }

        private static string ReadFontName(IntPtr hLabel, out int fontSize, out int bold, out int italic, out int underline)
        {
            int nameLength = EmojiWindowNative.GetLabelFont(hLabel, IntPtr.Zero, 0, out fontSize, out bold, out italic, out underline);
            if (nameLength <= 0)
            {
                return string.Empty;
            }

            IntPtr buffer = Marshal.AllocHGlobal(nameLength);
            try
            {
                EmojiWindowNative.GetLabelFont(hLabel, buffer, nameLength, out fontSize, out bold, out italic, out underline);
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
