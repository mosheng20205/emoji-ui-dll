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
            byte[] memoText = app.U("📄 已从主编辑框同步：\r\n1. 用于展示多行输入\r\n2. 保持统一浅底风格\r\n3. 可作为备注或说明区域");

            app.GroupBox(16, 16, 980, 248, "⌨️ EditBox 实时属性读取", DemoColors.Border, DemoColors.WindowBg, page);
            app.GroupBox(16, 286, 980, 254, "🪄 文本 / 颜色 / 尺寸 / 状态", DemoColors.Border, DemoColors.WindowBg, page);
            app.GroupBox(1020, 212, 444, 328, "📄 多行 EditBox 演示", DemoColors.Border, DemoColors.WindowBg, page);
            app.GroupBox(16, 558, 1448, 230, "📌 EditBox API 说明", DemoColors.Border, DemoColors.WindowBg, page);

            IntPtr demoEdit = app.EditBox(56, 120, 420, 38, "📌 单行 EditBox：可直接读取和设置文本。", false, false, false, false, page);
            IntPtr memo = EmojiWindowNative.CreateEditBox(page, 1044, 262, 380, 132, memoText, memoText.Length, DemoColors.Black, DemoColors.White, font, font.Length, 13, 0, 0, 0, PageCommon.AlignLeft, 1, 0, 0, 1, 0);
            IntPtr readout = app.Label(40, 184, 920, 56, "等待读取编辑框属性。", DemoColors.Black, DemoColors.WindowBg, 12, PageCommon.AlignLeft, true, page);
            IntPtr stateLabel = app.Label(40, 760, 1360, 22, "编辑框页状态将在这里更新。", DemoColors.Blue, DemoColors.WindowBg, 12, PageCommon.AlignLeft, false, page);
            app.Label(40, 56, 900, 24, "这一页直接读取文本、颜色、位置、字体、启用态和可见态，不再只是放一个输入框。", DemoColors.Gray, DemoColors.WindowBg, 12, PageCommon.AlignLeft, false, page);

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
                    $"bounds=({x}, {y}, {width}, {height})  align={PageCommon.AlignmentName(EmojiWindowNative.GetEditBoxAlignment(demoEdit))}  font={(string.IsNullOrEmpty(fontName) ? "default" : fontName)} {fontSize}px bold={bold}\r\n" +
                    $"fg={PageCommon.FormatColor(fg)}  bg={PageCommon.FormatColor(bg)}");
                shell.SetLabelText(stateLabel, note);
                shell.SetStatus(note);
            }

            void SetTextValue(string text, string note)
            {
                byte[] bytes = app.U(text);
                byte[] memoBytes = app.U("📄 已从主编辑框同步：\r\n" + text);
                EmojiWindowNative.SetEditBoxText(demoEdit, bytes, bytes.Length);
                EmojiWindowNative.SetEditBoxText(memo, memoBytes, memoBytes.Length);
                Refresh(note);
            }

            void SetColors(uint fg, uint bg, string note)
            {
                EmojiWindowNative.SetEditBoxColor(demoEdit, fg, bg);
                Refresh(note);
            }

            void SetFont(string fontName, int fontSize, bool bold, string note)
            {
                byte[] bytes = app.U(fontName);
                EmojiWindowNative.GetEditBoxBounds(demoEdit, out int x, out int y, out int width, out int height);
                EmojiWindowNative.SetEditBoxBounds(demoEdit, x, y, width, fontSize >= 16 ? 44 : height);
                EmojiWindowNative.SetEditBoxFont(demoEdit, bytes, bytes.Length, fontSize, bold ? 1 : 0, 0, 0);
                Refresh(note);
            }

            void MoveEdit(int dx = 0, int dy = 0, int dw = 0)
            {
                EmojiWindowNative.GetEditBoxBounds(demoEdit, out int x, out int y, out int width, out int height);
                EmojiWindowNative.SetEditBoxBounds(demoEdit, x + dx, y + dy, width + dw, height);
                Refresh($"编辑框位置/尺寸已更新: dx={dx}, dy={dy}, dw={dw}");
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
                Refresh("编辑框属性已恢复默认");
            }

            app.Label(40, 326, 180, 22, "📝 文本预设", DemoColors.Black, DemoColors.WindowBg, 15, PageCommon.AlignLeft, false, page);
            app.Button(40, 360, 156, 36, "写入表单文案", "🧾", DemoColors.Blue, () => SetTextValue("请输入项目名称 / 关键词 / 标题", "编辑框文本已切到表单模式"), page);
            app.Button(212, 360, 156, 36, "写入混排文案", "🌈", DemoColors.Green, () => SetTextValue("🌈 EmojiWindow 支持 emoji / English / 数字 123", "编辑框文本已切到混排模式"), page);
            app.Button(384, 360, 156, 36, "同步到多行框", "📄", DemoColors.Purple, () =>
            {
                string text = "📄 已从主编辑框同步：\r\n" + EmojiWindowNative.ReadUtf8(EmojiWindowNative.GetEditBoxText, demoEdit);
                byte[] bytes = app.U(text);
                EmojiWindowNative.SetEditBoxText(memo, bytes, bytes.Length);
                Refresh("当前单行内容已同步到多行 EditBox");
            }, page);

            app.Label(40, 414, 180, 22, "🎨 颜色 / 字体", DemoColors.Black, DemoColors.WindowBg, 15, PageCommon.AlignLeft, false, page);
            app.Button(40, 448, 118, 36, "冷色", "🧊", DemoColors.Blue, () => SetColors(DemoColors.Blue, DemoColors.LightBlue, "编辑框已切到冷色方案"), page);
            app.Button(172, 448, 118, 36, "暖色", "🧡", DemoColors.Orange, () => SetColors(DemoColors.Orange, DemoColors.Yellow, "编辑框已切到暖色方案"), page);
            app.Button(304, 448, 118, 36, "16px Bold", "🔠", DemoColors.Green, () => SetFont("Segoe UI Emoji", 16, true, "编辑框字体已切到 16px Bold"), page);
            app.Button(436, 448, 118, 36, "13px", "🔡", DemoColors.Gray, () => SetFont("Segoe UI Emoji", 13, false, "编辑框字体已切回 13px"), page);

            app.Label(1044, 404, 180, 22, "📻 布局 / 状态", DemoColors.Black, DemoColors.WindowBg, 15, PageCommon.AlignLeft, false, page);
            app.Button(1044, 438, 112, 32, "右移 80", "➡️", DemoColors.Blue, () => MoveEdit(dx: 80), page);
            app.Button(1168, 438, 112, 32, "下移 24", "⬇️", DemoColors.Green, () => MoveEdit(dy: 24), page);
            app.Button(1292, 438, 132, 32, "加宽 120", "↔️", DemoColors.Orange, () => MoveEdit(dw: 120), page);
            app.Button(1044, 478, 112, 32, "禁用/启用", "🚫", DemoColors.Purple, () =>
            {
                EmojiWindowNative.EnableEditBox(demoEdit, EmojiWindowNative.GetEditBoxEnabled(demoEdit) == 1 ? 0 : 1);
                Refresh("编辑框启用状态已切换");
            }, page);
            app.Button(1168, 478, 112, 32, "显示/隐藏", "👁️", DemoColors.Gray, () =>
            {
                EmojiWindowNative.ShowEditBox(demoEdit, EmojiWindowNative.GetEditBoxVisible(demoEdit) == 1 ? 0 : 1);
                Refresh("编辑框可见状态已切换");
            }, page);
            app.Button(1292, 478, 132, 32, "恢复默认", "↺", DemoColors.Blue, Restore, page);

            app.Label(40, 598, 700, 24, "1. GetEditBoxText / SetEditBoxText：读取和修改输入文本。", DemoColors.Black, DemoColors.WindowBg, 12, PageCommon.AlignLeft, false, page);
            app.Label(40, 632, 720, 24, "2. GetEditBoxColor / SetEditBoxColor：读取和切换前景色 / 背景色。", DemoColors.Black, DemoColors.WindowBg, 12, PageCommon.AlignLeft, false, page);
            app.Label(40, 666, 760, 24, "3. GetEditBoxBounds / SetEditBoxBounds：直接修改编辑框位置与宽度。", DemoColors.Black, DemoColors.WindowBg, 12, PageCommon.AlignLeft, false, page);
            app.Label(40, 700, 760, 24, "4. GetEditBoxFont / SetEditBoxFont：读取和切换字体名、字号和粗体。", DemoColors.Black, DemoColors.WindowBg, 12, PageCommon.AlignLeft, false, page);
            app.Label(40, 734, 760, 24, "5. EnableEditBox / ShowEditBox：演示启用态和可见态切换。", DemoColors.Black, DemoColors.WindowBg, 12, PageCommon.AlignLeft, false, page);

            Refresh("编辑框页已加载，可直接测试属性读取与设置");
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
