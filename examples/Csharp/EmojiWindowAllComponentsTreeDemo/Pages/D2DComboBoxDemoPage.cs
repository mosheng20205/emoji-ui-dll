using System;

namespace EmojiWindowDemo
{
    internal static class D2DComboBoxDemoPage
    {
        public static void Build(AllDemoShell shell, IntPtr page)
        {
            var app = shell.App;
            byte[] font = app.U("Microsoft YaHei UI");
            IntPtr combo = EmojiWindowNative.CreateD2DComboBox(page, 56, 120, 340, 40, 0, DemoColors.Black, DemoColors.White, 32, font, font.Length, 13, 0, 0, 0);
            foreach (string item in new[] { "🌟 D2D 默认", "🎨 D2D 主题", "🚀 D2D 高亮", "🧩 D2D 大空间" })
            {
                byte[] text = app.U(item);
                EmojiWindowNative.AddD2DComboItem(combo, text, text.Length);
            }
            EmojiWindowNative.SetD2DComboSelectedIndex(combo, 2);
            EmojiWindowNative.SetD2DComboBoxColors(combo, DemoColors.Black, DemoColors.White, DemoColors.LightBlue, DemoColors.Surface, DemoColors.Border, DemoColors.Blue);

            IntPtr readout = app.Label(40, 184, 960, 72, string.Empty, DemoColors.Black, DemoColors.WindowBg, 13, PageCommon.AlignLeft, true, page);
            IntPtr state = app.Label(40, 760, 1360, 22, "D2DComboBox 页状态将在这里更新。", DemoColors.Blue, DemoColors.WindowBg, 12, PageCommon.AlignLeft, false, page);

            void Refresh(string note)
            {
                string text = EmojiWindowNative.ReadUtf8(EmojiWindowNative.GetD2DComboText, combo);
                shell.SetLabelText(readout, $"text={text}\r\n{note}");
                shell.SetLabelText(state, note);
                shell.SetStatus(note);
            }

            var callback = app.Pin(new EmojiWindowNative.ComboBoxCallback((handle, index) =>
            {
                string text = index >= 0 ? EmojiWindowNative.ReadUtf8(EmojiWindowNative.GetD2DComboItemText, handle, index) : "(未选中)";
                Refresh("D2DComboBox 回调: " + text);
            }));
            EmojiWindowNative.SetD2DComboBoxCallback(combo, callback);

            app.Button(40, 320, 140, 36, "选第 1 项", "1", DemoColors.Blue, () =>
            {
                EmojiWindowNative.SetD2DComboSelectedIndex(combo, 0);
                Refresh("D2DComboBox 已切到第 1 项");
            }, page);
            app.Button(196, 320, 140, 36, "选第 4 项", "4", DemoColors.Green, () =>
            {
                EmojiWindowNative.SetD2DComboSelectedIndex(combo, 3);
                Refresh("D2DComboBox 已切到第 4 项");
            }, page);
            app.Button(352, 320, 156, 36, "写入企业蓝", "✏️", DemoColors.Purple, () =>
            {
                byte[] text = app.U("🧾 企业蓝主题");
                EmojiWindowNative.SetD2DComboText(combo, text, text.Length);
                Refresh("D2DComboBox 文本已写入企业蓝主题");
            }, page);
            app.Button(524, 320, 140, 36, "读取文本", "📄", DemoColors.Orange, () => Refresh("已读取当前 D2DComboBox 文本"), page);

            app.Button(40, 372, 140, 36, "蓝色方案", "💙", DemoColors.Blue, () =>
            {
                EmojiWindowNative.SetD2DComboBoxColors(combo, DemoColors.Black, DemoColors.White, DemoColors.LightBlue, DemoColors.Surface, DemoColors.Border, DemoColors.Blue);
                Refresh("D2DComboBox 已切到蓝色方案");
            }, page);
            app.Button(196, 372, 140, 36, "暖色方案", "🧡", DemoColors.Orange, () =>
            {
                EmojiWindowNative.SetD2DComboBoxColors(combo, DemoColors.Black, DemoColors.Yellow, DemoColors.Yellow, DemoColors.White, DemoColors.Orange, DemoColors.Orange);
                Refresh("D2DComboBox 已切到暖色方案");
            }, page);
            app.Button(352, 372, 156, 36, "恢复默认", "↺", DemoColors.Gray, () =>
            {
                EmojiWindowNative.SetD2DComboSelectedIndex(combo, 2);
                EmojiWindowNative.SetD2DComboBoxColors(combo, DemoColors.Black, DemoColors.White, DemoColors.LightBlue, DemoColors.Surface, DemoColors.Border, DemoColors.Blue);
                Refresh("D2DComboBox 已恢复默认状态");
            }, page);

            app.Label(40, 598, 1320, 22, "1. AddD2DComboItem / SetD2DComboSelectedIndex / SetD2DComboBoxCallback：操作项目、程序选中和回调写回。", DemoColors.Black, DemoColors.WindowBg, 12, PageCommon.AlignLeft, false, page);
            app.Label(40, 632, 1320, 22, "2. GetD2DComboText / SetD2DComboText：读取和修改输入区文本。", DemoColors.Black, DemoColors.WindowBg, 12, PageCommon.AlignLeft, false, page);
            app.Label(40, 666, 1320, 22, "3. SetD2DComboBoxColors：切换边框、按钮、选中和悬停配色。", DemoColors.Black, DemoColors.WindowBg, 12, PageCommon.AlignLeft, false, page);

            Refresh("D2DComboBox 页已加载，可直接测试项目、文本和颜色切换");
        }
    }
}
