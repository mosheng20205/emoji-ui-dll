using System;

namespace EmojiWindowDemo
{
    internal static class CheckBoxDemoPage
    {
        public static void Build(AllDemoShell shell, IntPtr page)
        {
            var app = shell.App;
            byte[] font = app.U("Microsoft YaHei UI");
            IntPtr readout = app.Label(40, 184, 900, 72, string.Empty, DemoColors.Black, DemoColors.WindowBg, 13, PageCommon.AlignLeft, true, page);
            IntPtr state = app.Label(40, 760, 1360, 22, "CheckBox 页状态将在这里更新。", DemoColors.Blue, DemoColors.WindowBg, 12, PageCommon.AlignLeft, false, page);

            byte[] t1 = app.U("☑️ 启用高级模式");
            byte[] t2 = app.U("🧱 卡片样式");
            byte[] t3 = app.U("🔘 按钮样式");
            IntPtr cb1 = EmojiWindowNative.CreateCheckBox(page, 56, 110, 260, 34, t1, t1.Length, 1, DemoColors.Black, DemoColors.Transparent, font, font.Length, 13, 0, 0, 0);
            IntPtr cb2 = EmojiWindowNative.CreateCheckBox(page, 56, 154, 260, 40, t2, t2.Length, 0, DemoColors.Black, DemoColors.Transparent, font, font.Length, 13, 0, 0, 0);
            IntPtr cb3 = EmojiWindowNative.CreateCheckBox(page, 356, 110, 260, 40, t3, t3.Length, 1, DemoColors.Black, DemoColors.Transparent, font, font.Length, 13, 0, 0, 0);
            EmojiWindowNative.SetCheckBoxStyle(cb2, PageCommon.CheckBoxStyleCard);
            EmojiWindowNative.SetCheckBoxStyle(cb3, PageCommon.CheckBoxStyleButton);
            EmojiWindowNative.SetCheckBoxCheckColor(cb1, DemoColors.Blue);
            EmojiWindowNative.SetCheckBoxCheckColor(cb2, DemoColors.Green);
            EmojiWindowNative.SetCheckBoxCheckColor(cb3, DemoColors.Orange);

            void Refresh(string note)
            {
                shell.SetLabelText(
                    readout,
                    $"CheckBox1={EmojiWindowNative.GetCheckBoxState(cb1)}  CheckBox2={EmojiWindowNative.GetCheckBoxState(cb2)}  CheckBox3={EmojiWindowNative.GetCheckBoxState(cb3)}\r\n" +
                    $"文本1={EmojiWindowNative.ReadUtf8(EmojiWindowNative.GetCheckBoxText, cb1)}\r\n" +
                    $"{note}");
                shell.SetLabelText(state, note);
                shell.SetStatus(note);
            }

            var callback = app.Pin(new EmojiWindowNative.CheckBoxCallback((handle, checkedState) =>
            {
                string label = EmojiWindowNative.ReadUtf8(EmojiWindowNative.GetCheckBoxText, handle);
                Refresh($"CheckBox 回调: {label} -> {checkedState}");
            }));
            EmojiWindowNative.SetCheckBoxCallback(cb1, callback);
            EmojiWindowNative.SetCheckBoxCallback(cb2, callback);
            EmojiWindowNative.SetCheckBoxCallback(cb3, callback);

            app.Button(40, 320, 140, 36, "读取状态", "📄", DemoColors.Blue, () => Refresh("已读取全部 CheckBox 状态"), page);
            app.Button(196, 320, 140, 36, "切换第 1 项", "↺", DemoColors.Green, () =>
            {
                EmojiWindowNative.SetCheckBoxState(cb1, EmojiWindowNative.GetCheckBoxState(cb1) == 0 ? 1 : 0);
                Refresh("程序切换 CheckBox1");
            }, page);
            app.Button(352, 320, 140, 36, "切换第 2 项", "↺", DemoColors.Orange, () =>
            {
                EmojiWindowNative.SetCheckBoxState(cb2, EmojiWindowNative.GetCheckBoxState(cb2) == 0 ? 1 : 0);
                Refresh("程序切换 CheckBox2");
            }, page);
            app.Button(508, 320, 140, 36, "切换第 3 项", "↺", DemoColors.Purple, () =>
            {
                EmojiWindowNative.SetCheckBoxState(cb3, EmojiWindowNative.GetCheckBoxState(cb3) == 0 ? 1 : 0);
                Refresh("程序切换 CheckBox3");
            }, page);

            app.Button(40, 372, 140, 36, "蓝色方案", "💙", DemoColors.Blue, () =>
            {
                EmojiWindowNative.SetCheckBoxCheckColor(cb1, DemoColors.Blue);
                EmojiWindowNative.SetCheckBoxCheckColor(cb2, DemoColors.Blue);
                EmojiWindowNative.SetCheckBoxCheckColor(cb3, DemoColors.Blue);
                Refresh("CheckBox 勾选色已统一切到蓝色方案");
            }, page);
            app.Button(196, 372, 140, 36, "绿色方案", "💚", DemoColors.Green, () =>
            {
                EmojiWindowNative.SetCheckBoxCheckColor(cb1, DemoColors.Green);
                EmojiWindowNative.SetCheckBoxCheckColor(cb2, DemoColors.Green);
                EmojiWindowNative.SetCheckBoxCheckColor(cb3, DemoColors.Green);
                Refresh("CheckBox 勾选色已统一切到绿色方案");
            }, page);
            app.Button(352, 372, 140, 36, "恢复样式", "↩", DemoColors.Gray, () =>
            {
                EmojiWindowNative.SetCheckBoxStyle(cb2, PageCommon.CheckBoxStyleCard);
                EmojiWindowNative.SetCheckBoxStyle(cb3, PageCommon.CheckBoxStyleButton);
                EmojiWindowNative.SetCheckBoxCheckColor(cb1, DemoColors.Blue);
                EmojiWindowNative.SetCheckBoxCheckColor(cb2, DemoColors.Green);
                EmojiWindowNative.SetCheckBoxCheckColor(cb3, DemoColors.Orange);
                Refresh("CheckBox 样式已恢复默认");
            }, page);

            app.Label(40, 598, 1320, 22, "1. GetCheckBoxState / SetCheckBoxState / SetCheckBoxCallback：读取、写入和监听勾选状态。", DemoColors.Black, DemoColors.WindowBg, 12, PageCommon.AlignLeft, false, page);
            app.Label(40, 632, 1320, 22, "2. GetCheckBoxText：读取控件文本并在回调里回写。", DemoColors.Black, DemoColors.WindowBg, 12, PageCommon.AlignLeft, false, page);
            app.Label(40, 666, 1320, 22, "3. SetCheckBoxStyle / SetCheckBoxCheckColor：切换卡片样式、按钮样式和勾选色。", DemoColors.Black, DemoColors.WindowBg, 12, PageCommon.AlignLeft, false, page);

            Refresh("CheckBox 页已加载，可直接测试状态读取、程序切换和样式切换");
        }
    }
}
