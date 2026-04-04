using System;

namespace EmojiWindowDemo
{
    internal static class RadioButtonDemoPage
    {
        public static void Build(AllDemoShell shell, IntPtr page)
        {
            var app = shell.App;
            byte[] font = app.U("Microsoft YaHei UI");

            app.GroupBox(16, 16, 980, 324, "🔘 RadioButton 样式演示", DemoTheme.Border, DemoTheme.Background, page);
            app.GroupBox(1020, 16, 444, 324, "🧩 状态 / 样式 / 分组", DemoTheme.Border, DemoTheme.Background, page);
            app.GroupBox(16, 538, 1448, 220, "📌 RadioButton API 说明", DemoTheme.Border, DemoTheme.Background, page);

            app.Label(40, 56, 980, 24, "这一页只保留 RadioButton，不再混入 CheckBox、ProgressBar、Slider 和 Switch。", DemoTheme.Muted, DemoTheme.Background, 12, PageCommon.AlignLeft, false, page);
            app.Label(40, 288, 960, 24, "左侧展示 default / border / button 三种主样式，并补上禁用态和一个可动态切换样式的单选框。", DemoTheme.Muted, DemoTheme.Background, 12, PageCommon.AlignLeft, true, page);

            byte[] defaultText = app.U("🔘 默认样式");
            byte[] borderText = app.U("🟠 Border 样式");
            byte[] buttonText = app.U("🟢 Button 样式");
            byte[] disabledText = app.U("🚫 禁用态");
            byte[] dynamicText = app.U("🧩 动态样式");

            IntPtr rbDefault = EmojiWindowNative.CreateRadioButton(page, 56, 118, 250, 34, defaultText, defaultText.Length, 301, 1, DemoTheme.Text, DemoTheme.Background, font, font.Length, 13, 0, 0, 0);
            IntPtr rbBorder = EmojiWindowNative.CreateRadioButton(page, 56, 166, 250, 36, borderText, borderText.Length, 301, 0, DemoTheme.Text, DemoTheme.Background, font, font.Length, 13, 0, 0, 0);
            IntPtr rbButton = EmojiWindowNative.CreateRadioButton(page, 56, 214, 250, 38, buttonText, buttonText.Length, 301, 0, DemoTheme.Text, DemoTheme.Surface, font, font.Length, 13, 0, 0, 0);
            IntPtr rbDisabled = EmojiWindowNative.CreateRadioButton(page, 360, 118, 260, 34, disabledText, disabledText.Length, 302, 1, DemoTheme.Text, DemoTheme.Background, font, font.Length, 13, 0, 0, 0);
            IntPtr rbDynamic = EmojiWindowNative.CreateRadioButton(page, 360, 170, 260, 38, dynamicText, dynamicText.Length, 302, 0, DemoTheme.Text, DemoTheme.Background, font, font.Length, 13, 0, 0, 0);

            EmojiWindowNative.SetRadioButtonStyle(rbBorder, PageCommon.RadioStyleBorder);
            EmojiWindowNative.SetRadioButtonStyle(rbButton, PageCommon.RadioStyleButton);
            EmojiWindowNative.SetRadioButtonStyle(rbDynamic, PageCommon.RadioStyleBorder);
            EmojiWindowNative.SetRadioButtonDotColor(rbDefault, DemoTheme.Primary);
            EmojiWindowNative.SetRadioButtonDotColor(rbBorder, DemoTheme.Warning);
            EmojiWindowNative.SetRadioButtonDotColor(rbButton, DemoTheme.Success);
            EmojiWindowNative.SetRadioButtonDotColor(rbDisabled, DemoTheme.Muted);
            EmojiWindowNative.SetRadioButtonDotColor(rbDynamic, DemoColors.Purple);
            EmojiWindowNative.EnableRadioButton(rbDisabled, 0);

            IntPtr readout = app.Label(40, 366, 1384, 108, "等待读取 RadioButton 属性。", DemoTheme.Text, DemoTheme.Background, 12, PageCommon.AlignLeft, true, page);
            IntPtr stateLabel = app.Label(40, 490, 1360, 22, "RadioButton 页面状态会显示在这里。", DemoTheme.Primary, DemoTheme.Background, 12, PageCommon.AlignLeft, false, page);

            IntPtr[] radios = { rbDefault, rbBorder, rbButton, rbDisabled, rbDynamic };
            string[] names = { "default", "border", "button", "disabled", "dynamic" };
            int[] groups = { 301, 301, 301, 302, 302 };
            int dynamicX = 360;
            bool dynamicAltText = false;

            string StyleName(int style)
            {
                switch (style)
                {
                    case 0:
                        return "default";
                    case PageCommon.RadioStyleBorder:
                        return "border";
                    case PageCommon.RadioStyleButton:
                        return "button";
                    default:
                        return "unknown(" + style + ")";
                }
            }

            void ApplyTheme()
            {
                EmojiWindowNative.SetRadioButtonColor(rbDefault, DemoTheme.Text, DemoTheme.Background);
                EmojiWindowNative.SetRadioButtonColor(rbBorder, DemoTheme.Text, DemoTheme.Background);
                EmojiWindowNative.SetRadioButtonColor(rbButton, DemoTheme.Text, DemoTheme.Surface);
                EmojiWindowNative.SetRadioButtonColor(rbDisabled, DemoTheme.Text, DemoTheme.Background);
                EmojiWindowNative.SetRadioButtonColor(rbDynamic, DemoTheme.Text, DemoTheme.Background);
            }

            void Refresh(string note)
            {
                string text = note + "\r\n";
                for (int i = 0; i < radios.Length; i++)
                {
                    EmojiWindowNative.GetRadioButtonColor(radios[i], out uint fg, out uint bg);
                    EmojiWindowNative.GetRadioButtonDotColor(radios[i], out uint dot);
                    string visible = Win32Native.IsWindowVisible(radios[i]) ? "显示" : "隐藏";
                    string enabled = Win32Native.IsWindowEnabled(radios[i]) ? "启用" : "禁用";
                    text += $"{names[i]}: checked={EmojiWindowNative.GetRadioButtonState(radios[i])} group={groups[i]} style={StyleName(EmojiWindowNative.GetRadioButtonStyle(radios[i]))} {visible}/{enabled} text={EmojiWindowNative.ReadUtf8(EmojiWindowNative.GetRadioButtonText, radios[i])} fg={PageCommon.FormatColor(fg)} bg={PageCommon.FormatColor(bg)} dot={PageCommon.FormatColor(dot)}";
                    if (i < radios.Length - 1)
                    {
                        text += "\r\n";
                    }
                }

                shell.SetLabelText(readout, text);
                shell.SetLabelText(stateLabel, note);
                shell.SetStatus(note);
            }

            void SelectRadio(IntPtr radio, string note)
            {
                EmojiWindowNative.SetRadioButtonState(radio, 1);
                Refresh(note);
            }

            void SetScheme(uint a, uint b, uint c, uint d, uint e, string note)
            {
                EmojiWindowNative.SetRadioButtonDotColor(rbDefault, a);
                EmojiWindowNative.SetRadioButtonDotColor(rbBorder, b);
                EmojiWindowNative.SetRadioButtonDotColor(rbButton, c);
                EmojiWindowNative.SetRadioButtonDotColor(rbDisabled, d);
                EmojiWindowNative.SetRadioButtonDotColor(rbDynamic, e);
                Refresh(note);
            }

            void Restore()
            {
                EmojiWindowNative.SetRadioButtonText(rbDefault, defaultText, defaultText.Length);
                EmojiWindowNative.SetRadioButtonText(rbBorder, borderText, borderText.Length);
                EmojiWindowNative.SetRadioButtonText(rbButton, buttonText, buttonText.Length);
                EmojiWindowNative.SetRadioButtonText(rbDisabled, disabledText, disabledText.Length);
                EmojiWindowNative.SetRadioButtonText(rbDynamic, dynamicText, dynamicText.Length);
                EmojiWindowNative.SetRadioButtonState(rbDefault, 1);
                EmojiWindowNative.SetRadioButtonState(rbDisabled, 1);
                EmojiWindowNative.SetRadioButtonStyle(rbDefault, 0);
                EmojiWindowNative.SetRadioButtonStyle(rbBorder, PageCommon.RadioStyleBorder);
                EmojiWindowNative.SetRadioButtonStyle(rbButton, PageCommon.RadioStyleButton);
                EmojiWindowNative.SetRadioButtonStyle(rbDynamic, PageCommon.RadioStyleBorder);
                EmojiWindowNative.EnableRadioButton(rbDisabled, 0);
                EmojiWindowNative.ShowRadioButton(rbDynamic, 1);
                dynamicX = 360;
                EmojiWindowNative.SetRadioButtonBounds(rbDynamic, dynamicX, 170, 260, 38);
                dynamicAltText = false;
                ApplyTheme();
                SetScheme(DemoTheme.Primary, DemoTheme.Warning, DemoTheme.Success, DemoTheme.Muted, DemoColors.Purple, "RadioButton 页面已恢复默认。");
            }

            var callback = app.Pin(new EmojiWindowNative.RadioButtonCallback((handle, groupId, checkedState) =>
            {
                Refresh($"RadioButton 回调: {EmojiWindowNative.ReadUtf8(EmojiWindowNative.GetRadioButtonText, handle)} group={groupId} checked={checkedState}");
            }));
            foreach (IntPtr radio in radios)
            {
                EmojiWindowNative.SetRadioButtonCallback(radio, callback);
            }

            app.Label(1044, 56, 220, 22, "分组选中", DemoTheme.Text, DemoTheme.Background, 14, PageCommon.AlignLeft, false, page);
            app.Button(1044, 94, 116, 34, "选中默认", "1", DemoColors.Blue, () => SelectRadio(rbDefault, "默认样式单选框已设为选中。"), page);
            app.Button(1172, 94, 116, 34, "选中 Border", "2", DemoColors.Orange, () => SelectRadio(rbBorder, "Border 样式单选框已设为选中。"), page);
            app.Button(1300, 94, 124, 34, "选中 Button", "3", DemoColors.Green, () => SelectRadio(rbButton, "Button 样式单选框已设为选中。"), page);

            app.Label(1044, 148, 220, 22, "动态样式", DemoTheme.Text, DemoTheme.Background, 14, PageCommon.AlignLeft, false, page);
            app.Button(1044, 182, 116, 34, "选中动态", "4", DemoColors.Purple, () => SelectRadio(rbDynamic, "动态样式单选框已设为选中。"), page);
            app.Button(1172, 182, 116, 34, "禁用/启用", "E", DemoColors.Gray, () =>
            {
                EmojiWindowNative.EnableRadioButton(rbDisabled, Win32Native.IsWindowEnabled(rbDisabled) ? 0 : 1);
                Refresh("禁用态单选框已切换启用状态。");
            }, page);
            app.Button(1300, 182, 124, 34, "显示/隐藏", "V", DemoColors.Blue, () =>
            {
                EmojiWindowNative.ShowRadioButton(rbDynamic, Win32Native.IsWindowVisible(rbDynamic) ? 0 : 1);
                Refresh("动态单选框已切换显示状态。");
            }, page);
            app.Button(1044, 226, 116, 34, "默认样式", "A", DemoColors.Gray, () =>
            {
                EmojiWindowNative.SetRadioButtonStyle(rbDynamic, 0);
                Refresh("动态单选框已切到 default 样式。");
            }, page);
            app.Button(1172, 226, 116, 34, "Border 样式", "B", DemoColors.Orange, () =>
            {
                EmojiWindowNative.SetRadioButtonStyle(rbDynamic, PageCommon.RadioStyleBorder);
                Refresh("动态单选框已切到 border 样式。");
            }, page);
            app.Button(1300, 226, 124, 34, "Button 样式", "C", DemoColors.Green, () =>
            {
                EmojiWindowNative.SetRadioButtonStyle(rbDynamic, PageCommon.RadioStyleButton);
                Refresh("动态单选框已切到 button 样式。");
            }, page);

            app.Label(1044, 280, 220, 22, "文本与配色", DemoTheme.Text, DemoTheme.Background, 14, PageCommon.AlignLeft, false, page);
            app.Button(1044, 314, 116, 34, "切换文本", "T", DemoColors.Purple, () =>
            {
                dynamicAltText = !dynamicAltText;
                byte[] text = app.U(dynamicAltText ? "🧩 动态样式 / 文案已切换" : "🧩 动态样式");
                EmojiWindowNative.SetRadioButtonText(rbDynamic, text, text.Length);
                Refresh("动态单选框文本已切换。");
            }, page);
            app.Button(1172, 314, 116, 34, "右移 60", ">", DemoColors.Gray, () =>
            {
                dynamicX += 60;
                EmojiWindowNative.SetRadioButtonBounds(rbDynamic, dynamicX, 170, 260, 38);
                Refresh("动态单选框位置已更新: dx=60");
            }, page);
            app.Button(1300, 314, 124, 34, "恢复默认", "R", DemoColors.Blue, Restore, page);
            app.Button(1044, 358, 116, 34, "默认配色", "D", DemoColors.Blue, () => SetScheme(DemoTheme.Primary, DemoTheme.Warning, DemoTheme.Success, DemoTheme.Muted, DemoColors.Purple, "RadioButton 已切换到默认配色方案。"), page);
            app.Button(1172, 358, 116, 34, "暖色方案", "W", DemoColors.Orange, () => SetScheme(DemoTheme.Warning, DemoTheme.Danger, DemoTheme.Warning, DemoTheme.Muted, DemoTheme.Danger, "RadioButton 已切换到暖色配色方案。"), page);

            app.Label(40, 582, 1120, 24, "1. GetRadioButtonState / SetRadioButtonState：读取与切换当前组内选中项。", DemoTheme.Text, DemoTheme.Background, 12, PageCommon.AlignLeft, false, page);
            app.Label(40, 616, 1120, 24, "2. GetRadioButtonStyle / SetRadioButtonStyle：直接切换 default / border / button。", DemoTheme.Text, DemoTheme.Background, 12, PageCommon.AlignLeft, false, page);
            app.Label(40, 650, 1120, 24, "3. GetRadioButtonColor / SetRadioButtonColor / SetRadioButtonDotColor：读取文本色、背景色和圆点色。", DemoTheme.Text, DemoTheme.Background, 12, PageCommon.AlignLeft, false, page);
            app.Label(40, 684, 1120, 24, "4. GetRadioButtonText / SetRadioButtonText：动态切换单选框文案。", DemoTheme.Text, DemoTheme.Background, 12, PageCommon.AlignLeft, false, page);
            app.Label(40, 718, 1120, 24, "5. EnableRadioButton / ShowRadioButton / SetRadioButtonBounds：演示启用态、显示态和位置更新。", DemoTheme.Text, DemoTheme.Background, 12, PageCommon.AlignLeft, false, page);

            shell.RegisterPageThemeHandler(page, ApplyTheme);
            ApplyTheme();
            Refresh("RadioButton 页面已整理：保留了样式、分组和状态控制，并按 Python 版重新收口了颜色层次。");
        }
    }
}
