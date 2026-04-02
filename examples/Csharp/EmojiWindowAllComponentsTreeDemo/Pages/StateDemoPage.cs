using System;

namespace EmojiWindowDemo
{
    internal static class StateDemoPage
    {
        public static void Build(AllDemoShell shell, IntPtr page)
        {
            var app = shell.App;
            IntPtr leftGroup = app.GroupBox(16, 16, 700, 330, "CheckBox / RadioButton", parent: page);
            IntPtr rightGroup = app.GroupBox(734, 16, 730, 330, "ProgressBar / Slider / Switch", parent: page);
            IntPtr output = app.Label(40, 374, 1320, 48, "状态读取区。", DemoColors.Gray, DemoColors.WindowBg, 12, PageCommon.AlignLeft, true, page);

            byte[] font = app.U("Microsoft YaHei UI");

            byte[] cb1Text = app.U("☑️ 启用高级模式");
            IntPtr cb1 = EmojiWindowNative.CreateCheckBox(page, 40, 90, 240, 34, cb1Text, cb1Text.Length, 1, DemoColors.Black, DemoColors.Transparent, font, font.Length, 13, 0, 0, 0);
            byte[] cb2Text = app.U("🧱 卡片样式");
            IntPtr cb2 = EmojiWindowNative.CreateCheckBox(page, 40, 136, 240, 40, cb2Text, cb2Text.Length, 0, DemoColors.Black, DemoColors.Transparent, font, font.Length, 13, 0, 0, 0);
            EmojiWindowNative.SetCheckBoxStyle(cb2, PageCommon.CheckBoxStyleCard);
            EmojiWindowNative.SetCheckBoxCheckColor(cb1, DemoColors.Blue);
            EmojiWindowNative.SetCheckBoxCheckColor(cb2, DemoColors.Green);

            byte[] rb1Text = app.U("🅰️ 方案 A");
            byte[] rb2Text = app.U("🅱️ 方案 B");
            byte[] rb3Text = app.U("🅲️ 按钮样式");
            IntPtr rb1 = EmojiWindowNative.CreateRadioButton(page, 340, 90, 130, 34, rb1Text, rb1Text.Length, 99, 1, DemoColors.Black, DemoColors.Transparent, font, font.Length, 13, 0, 0, 0);
            IntPtr rb2 = EmojiWindowNative.CreateRadioButton(page, 480, 90, 130, 34, rb2Text, rb2Text.Length, 99, 0, DemoColors.Black, DemoColors.Transparent, font, font.Length, 13, 0, 0, 0);
            IntPtr rb3 = EmojiWindowNative.CreateRadioButton(page, 340, 136, 150, 36, rb3Text, rb3Text.Length, 99, 0, DemoColors.Black, DemoColors.Transparent, font, font.Length, 13, 0, 0, 0);
            EmojiWindowNative.SetRadioButtonStyle(rb2, PageCommon.RadioStyleBorder);
            EmojiWindowNative.SetRadioButtonStyle(rb3, PageCommon.RadioStyleButton);
            EmojiWindowNative.SetRadioButtonDotColor(rb1, DemoColors.Blue);
            EmojiWindowNative.SetRadioButtonDotColor(rb2, DemoColors.Orange);
            EmojiWindowNative.SetRadioButtonDotColor(rb3, DemoColors.Green);

            IntPtr progress = EmojiWindowNative.CreateProgressBar(page, 758, 90, 420, 28, 35, DemoColors.Blue, DemoColors.Border, 1, DemoColors.Black);
            EmojiWindowNative.SetProgressBarShowText(progress, 1);
            EmojiWindowNative.SetProgressBarTextColor(progress, DemoColors.Black);

            IntPtr slider = EmojiWindowNative.CreateSlider(page, 758, 146, 260, 40, 0, 100, 36, 10, DemoColors.Blue, DemoColors.Border);
            EmojiWindowNative.SetSliderShowStops(slider, 1);

            byte[] onText = app.U("开");
            byte[] offText = app.U("关");
            IntPtr toggle = EmojiWindowNative.CreateSwitch(page, 1048, 142, 88, 34, 1, DemoColors.Green, DemoColors.Border, onText, onText.Length, offText, offText.Length);

            app.AttachToGroup(leftGroup, cb1, cb2, rb1, rb2, rb3);
            app.AttachToGroup(rightGroup, progress, slider, toggle);

            void Refresh(string prefix)
            {
                string text =
                    $"{prefix}\r\n" +
                    $"CheckBox1={EmojiWindowNative.GetCheckBoxState(cb1)}  CheckBox2={EmojiWindowNative.GetCheckBoxState(cb2)}  " +
                    $"RadioA={EmojiWindowNative.GetRadioButtonState(rb1)}  RadioB={EmojiWindowNative.GetRadioButtonState(rb2)}  RadioC={EmojiWindowNative.GetRadioButtonState(rb3)}\r\n" +
                    $"Progress={EmojiWindowNative.GetProgressValue(progress)}  Slider={EmojiWindowNative.GetSliderValue(slider)}  Switch={EmojiWindowNative.GetSwitchState(toggle)}";
                shell.SetLabelText(output, text);
                shell.SetStatus(prefix);
            }

            var checkCallback = app.Pin(new EmojiWindowNative.CheckBoxCallback((handle, checkedState) =>
            {
                string label = EmojiWindowNative.ReadUtf8(EmojiWindowNative.GetCheckBoxText, handle);
                Refresh($"CheckBox 回调: {label} -> {checkedState}");
            }));
            var radioCallback = app.Pin(new EmojiWindowNative.RadioButtonCallback((handle, groupId, checkedState) =>
            {
                string label = EmojiWindowNative.ReadUtf8(EmojiWindowNative.GetRadioButtonText, handle);
                Refresh($"Radio 回调: {label} group={groupId} checked={checkedState}");
            }));
            var progressCallback = app.Pin(new EmojiWindowNative.ProgressBarCallback((_, value) => Refresh($"Progress 回调: value={value}")));
            var sliderCallback = app.Pin(new EmojiWindowNative.SliderCallback((_, value) => Refresh($"Slider 回调: value={value}")));
            var switchCallback = app.Pin(new EmojiWindowNative.SwitchCallback((_, checkedState) => Refresh($"Switch 回调: checked={checkedState}")));
            EmojiWindowNative.SetCheckBoxCallback(cb1, checkCallback);
            EmojiWindowNative.SetCheckBoxCallback(cb2, checkCallback);
            EmojiWindowNative.SetRadioButtonCallback(rb1, radioCallback);
            EmojiWindowNative.SetRadioButtonCallback(rb2, radioCallback);
            EmojiWindowNative.SetRadioButtonCallback(rb3, radioCallback);
            EmojiWindowNative.SetProgressBarCallback(progress, progressCallback);
            EmojiWindowNative.SetSliderCallback(slider, sliderCallback);
            EmojiWindowNative.SetSwitchCallback(toggle, switchCallback);

            app.Button(40, 228, 140, 36, "切换勾选 1", "↺", DemoColors.Blue, () =>
            {
                EmojiWindowNative.SetCheckBoxState(cb1, EmojiWindowNative.GetCheckBoxState(cb1) == 0 ? 1 : 0);
                Refresh("程序切换 CheckBox1");
            }, page);
            app.Button(196, 228, 140, 36, "切换勾选 2", "↺", DemoColors.Green, () =>
            {
                EmojiWindowNative.SetCheckBoxState(cb2, EmojiWindowNative.GetCheckBoxState(cb2) == 0 ? 1 : 0);
                Refresh("程序切换 CheckBox2");
            }, page);
            app.Button(758, 228, 120, 36, "进度 -10", "📉", DemoColors.Gray, () =>
            {
                EmojiWindowNative.SetProgressValue(progress, Math.Max(0, EmojiWindowNative.GetProgressValue(progress) - 10));
                Refresh("程序设置 ProgressBar");
            }, page);
            app.Button(894, 228, 120, 36, "进度 +10", "📈", DemoColors.Blue, () =>
            {
                EmojiWindowNative.SetProgressValue(progress, Math.Min(100, EmojiWindowNative.GetProgressValue(progress) + 10));
                Refresh("程序设置 ProgressBar");
            }, page);
            app.Button(1030, 228, 120, 36, "Slider=75", "🎚️", DemoColors.Orange, () =>
            {
                EmojiWindowNative.SetSliderValue(slider, 75);
                Refresh("程序设置 Slider=75");
            }, page);
            app.Button(1166, 228, 120, 36, "切换开关", "🔀", DemoColors.Green, () =>
            {
                EmojiWindowNative.SetSwitchState(toggle, EmojiWindowNative.GetSwitchState(toggle) == 0 ? 1 : 0);
                Refresh("程序切换 Switch");
            }, page);

            Refresh("初始状态");
        }
    }
}
