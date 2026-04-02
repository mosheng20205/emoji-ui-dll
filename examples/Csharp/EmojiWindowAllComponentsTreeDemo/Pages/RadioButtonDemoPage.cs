using System;

namespace EmojiWindowDemo
{
    internal static class RadioButtonDemoPage
    {
        public static void Build(AllDemoShell shell, IntPtr page)
        {
            var app = shell.App;
            byte[] font = app.U("Microsoft YaHei UI");
            IntPtr readout = app.Label(40, 184, 900, 72, string.Empty, DemoColors.Black, DemoColors.WindowBg, 13, PageCommon.AlignLeft, true, page);
            IntPtr state = app.Label(40, 760, 1360, 22, "RadioButton 页状态将在这里更新。", DemoColors.Blue, DemoColors.WindowBg, 12, PageCommon.AlignLeft, false, page);

            byte[] t1 = app.U("🅰️ 方案 A");
            byte[] t2 = app.U("🅱️ 方案 B");
            byte[] t3 = app.U("🅲️ 按钮样式");
            IntPtr rb1 = EmojiWindowNative.CreateRadioButton(page, 56, 110, 180, 34, t1, t1.Length, 99, 1, DemoColors.Black, DemoColors.Transparent, font, font.Length, 13, 0, 0, 0);
            IntPtr rb2 = EmojiWindowNative.CreateRadioButton(page, 256, 110, 180, 34, t2, t2.Length, 99, 0, DemoColors.Black, DemoColors.Transparent, font, font.Length, 13, 0, 0, 0);
            IntPtr rb3 = EmojiWindowNative.CreateRadioButton(page, 456, 110, 200, 38, t3, t3.Length, 99, 0, DemoColors.Black, DemoColors.Transparent, font, font.Length, 13, 0, 0, 0);
            EmojiWindowNative.SetRadioButtonStyle(rb2, PageCommon.RadioStyleBorder);
            EmojiWindowNative.SetRadioButtonStyle(rb3, PageCommon.RadioStyleButton);
            EmojiWindowNative.SetRadioButtonDotColor(rb1, DemoColors.Blue);
            EmojiWindowNative.SetRadioButtonDotColor(rb2, DemoColors.Orange);
            EmojiWindowNative.SetRadioButtonDotColor(rb3, DemoColors.Green);

            void Refresh(string note)
            {
                shell.SetLabelText(
                    readout,
                    $"A={EmojiWindowNative.GetRadioButtonState(rb1)}  B={EmojiWindowNative.GetRadioButtonState(rb2)}  C={EmojiWindowNative.GetRadioButtonState(rb3)}\r\n" +
                    $"文本={EmojiWindowNative.ReadUtf8(EmojiWindowNative.GetRadioButtonText, rb1)} / {EmojiWindowNative.ReadUtf8(EmojiWindowNative.GetRadioButtonText, rb2)} / {EmojiWindowNative.ReadUtf8(EmojiWindowNative.GetRadioButtonText, rb3)}\r\n" +
                    $"{note}");
                shell.SetLabelText(state, note);
                shell.SetStatus(note);
            }

            var callback = app.Pin(new EmojiWindowNative.RadioButtonCallback((handle, groupId, checkedState) =>
            {
                string label = EmojiWindowNative.ReadUtf8(EmojiWindowNative.GetRadioButtonText, handle);
                Refresh($"Radio 回调: {label} group={groupId} checked={checkedState}");
            }));
            EmojiWindowNative.SetRadioButtonCallback(rb1, callback);
            EmojiWindowNative.SetRadioButtonCallback(rb2, callback);
            EmojiWindowNative.SetRadioButtonCallback(rb3, callback);

            app.Button(40, 320, 140, 36, "读取状态", "📄", DemoColors.Blue, () => Refresh("已读取全部 RadioButton 状态"), page);
            app.Button(196, 320, 140, 36, "读取文本", "ℹ", DemoColors.Gray, () =>
            {
                Refresh("已读取当前 RadioButton 文本和分组状态");
            }, page);
            app.Button(352, 320, 140, 36, "蓝色方案", "💙", DemoColors.Blue, () =>
            {
                EmojiWindowNative.SetRadioButtonDotColor(rb1, DemoColors.Blue);
                EmojiWindowNative.SetRadioButtonDotColor(rb2, DemoColors.Blue);
                EmojiWindowNative.SetRadioButtonDotColor(rb3, DemoColors.Blue);
                Refresh("RadioButton 圆点色已统一切到蓝色方案");
            }, page);
            app.Button(508, 320, 140, 36, "彩色方案", "🌈", DemoColors.Purple, () =>
            {
                EmojiWindowNative.SetRadioButtonDotColor(rb1, DemoColors.Blue);
                EmojiWindowNative.SetRadioButtonDotColor(rb2, DemoColors.Orange);
                EmojiWindowNative.SetRadioButtonDotColor(rb3, DemoColors.Green);
                Refresh("RadioButton 圆点色已恢复彩色方案");
            }, page);

            app.Label(40, 598, 1320, 22, "1. GetRadioButtonState / SetRadioButtonCallback：读取当前选中项并监听分组变化。", DemoColors.Black, DemoColors.WindowBg, 12, PageCommon.AlignLeft, false, page);
            app.Label(40, 632, 1320, 22, "2. GetRadioButtonText：读取当前 RadioButton 文本。", DemoColors.Black, DemoColors.WindowBg, 12, PageCommon.AlignLeft, false, page);
            app.Label(40, 666, 1320, 22, "3. SetRadioButtonStyle / SetRadioButtonDotColor：切换边框样式、按钮样式和圆点色。", DemoColors.Black, DemoColors.WindowBg, 12, PageCommon.AlignLeft, false, page);

            Refresh("RadioButton 页已加载，可直接测试分组单选和样式切换");
        }
    }
}
