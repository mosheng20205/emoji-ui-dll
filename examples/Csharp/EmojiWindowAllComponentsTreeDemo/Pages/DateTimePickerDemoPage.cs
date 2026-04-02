using System;

namespace EmojiWindowDemo
{
    internal static class DateTimePickerDemoPage
    {
        public static void Build(AllDemoShell shell, IntPtr page)
        {
            var app = shell.App;
            byte[] font = app.U("Microsoft YaHei UI");
            IntPtr picker = EmojiWindowNative.CreateD2DDateTimePicker(page, 56, 120, 340, 38, PageCommon.DtpYmdHm, DemoColors.Black, DemoColors.White, DemoColors.Border, font, font.Length, 13, 0, 0, 0);
            EmojiWindowNative.SetD2DDateTimePickerDateTime(picker, 2026, 3, 30, 14, 30, 0);

            EmojiWindowNative.GetD2DDateTimePickerColors(picker, out uint defaultFg, out uint defaultBg, out uint defaultBorder);
            IntPtr readout = app.Label(40, 184, 920, 56, string.Empty, DemoColors.Black, DemoColors.WindowBg, 13, PageCommon.AlignLeft, true, page);
            IntPtr state = app.Label(40, 760, 1360, 22, "日期时间页状态将在这里更新。", DemoColors.Blue, DemoColors.WindowBg, 12, PageCommon.AlignLeft, false, page);

            int x = 56;
            int y = 120;
            int width = 340;
            int height = 38;
            bool visible = true;
            bool enabled = true;

            void Refresh(string note)
            {
                EmojiWindowNative.GetD2DDateTimePickerDateTime(picker, out int year, out int month, out int day, out int hour, out int minute, out int second);
                EmojiWindowNative.GetD2DDateTimePickerColors(picker, out uint fg, out uint bg, out uint border);
                int precision = EmojiWindowNative.GetD2DDateTimePickerPrecision(picker);
                shell.SetLabelText(
                    readout,
                    $"datetime={PageCommon.FormatDateTime(year, month, day, hour, minute, second)}  precision={precision}  {(visible ? "显示" : "隐藏")}/{(enabled ? "启用" : "禁用")}\r\n" +
                    $"bounds=({x}, {y}, {width}, {height})\r\n" +
                    $"fg={PageCommon.FormatColor(fg)}  bg={PageCommon.FormatColor(bg)}  border={PageCommon.FormatColor(border)}");
                shell.SetLabelText(state, note);
                shell.SetStatus(note);
            }

            var callback = app.Pin(new EmojiWindowNative.ValueChangedCallback(_ => Refresh("日期时间选择器值已变化")));
            EmojiWindowNative.SetD2DDateTimePickerCallback(picker, callback);

            app.Button(40, 360, 156, 36, "设为 2026-03-30", "📅", DemoColors.Blue, () =>
            {
                EmojiWindowNative.SetD2DDateTimePickerDateTime(picker, 2026, 3, 30, 14, 30, 0);
                Refresh("日期时间已设为 2026-03-30 14:30:00");
            }, page);
            app.Button(212, 360, 156, 36, "设为 2027-01-01", "⏰", DemoColors.Green, () =>
            {
                EmojiWindowNative.SetD2DDateTimePickerDateTime(picker, 2027, 1, 1, 9, 0, 0);
                Refresh("日期时间已设为 2027-01-01 09:00:00");
            }, page);
            app.Button(384, 360, 156, 36, "精度 YMD", "🕒", DemoColors.Purple, () =>
            {
                EmojiWindowNative.SetD2DDateTimePickerPrecision(picker, PageCommon.DtpYmd);
                Refresh("日期时间精度已切到 YMD");
            }, page);
            app.Button(556, 360, 156, 36, "精度 YMDHM", "🕓", DemoColors.Orange, () =>
            {
                EmojiWindowNative.SetD2DDateTimePickerPrecision(picker, PageCommon.DtpYmdHm);
                Refresh("日期时间精度已切到 YMDHM");
            }, page);
            app.Button(728, 360, 156, 36, "精度 YMDHMS", "🕕", DemoColors.Blue, () =>
            {
                EmojiWindowNative.SetD2DDateTimePickerPrecision(picker, PageCommon.DtpYmdHms);
                Refresh("日期时间精度已切到 YMDHMS");
            }, page);

            app.Button(40, 448, 118, 36, "冷色", "💙", DemoColors.Blue, () =>
            {
                EmojiWindowNative.SetD2DDateTimePickerColors(picker, DemoColors.Blue, DemoColors.LightBlue, DemoColors.Blue);
                Refresh("日期时间选择框已切到冷色方案");
            }, page);
            app.Button(172, 448, 118, 36, "暖色", "🧡", DemoColors.Orange, () =>
            {
                EmojiWindowNative.SetD2DDateTimePickerColors(picker, DemoColors.Orange, DemoColors.Yellow, DemoColors.Orange);
                Refresh("日期时间选择框已切到暖色方案");
            }, page);
            app.Button(304, 448, 118, 36, "右移 80", "➡️", DemoColors.Green, () =>
            {
                x = 136;
                EmojiWindowNative.SetD2DDateTimePickerBounds(picker, x, y, width, height);
                Refresh("日期时间选择框已右移 80");
            }, page);
            app.Button(436, 448, 118, 36, "加宽 100", "↔️", DemoColors.Purple, () =>
            {
                width = 440;
                EmojiWindowNative.SetD2DDateTimePickerBounds(picker, x, y, width, height);
                Refresh("日期时间选择框已加宽到 440");
            }, page);

            app.Button(1044, 286, 118, 36, "禁用/启用", "🚫", DemoColors.Blue, () =>
            {
                enabled = !enabled;
                EmojiWindowNative.EnableD2DDateTimePicker(picker, enabled ? 1 : 0);
                Refresh("日期时间选择框启用状态已切换");
            }, page);
            app.Button(1176, 286, 118, 36, "显示/隐藏", "👁️", DemoColors.Gray, () =>
            {
                visible = !visible;
                EmojiWindowNative.ShowD2DDateTimePicker(picker, visible ? 1 : 0);
                Refresh("日期时间选择框可见状态已切换");
            }, page);
            app.Button(1308, 286, 118, 36, "恢复默认", "↺", DemoColors.Green, () =>
            {
                x = 56;
                y = 120;
                width = 340;
                height = 38;
                visible = true;
                enabled = true;
                EmojiWindowNative.SetD2DDateTimePickerBounds(picker, x, y, width, height);
                EmojiWindowNative.SetD2DDateTimePickerPrecision(picker, PageCommon.DtpYmdHm);
                EmojiWindowNative.SetD2DDateTimePickerDateTime(picker, 2026, 3, 30, 14, 30, 0);
                EmojiWindowNative.SetD2DDateTimePickerColors(picker, defaultFg, defaultBg, defaultBorder);
                EmojiWindowNative.EnableD2DDateTimePicker(picker, 1);
                EmojiWindowNative.ShowD2DDateTimePicker(picker, 1);
                Refresh("日期时间页已恢复默认状态");
            }, page);

            app.Label(1044, 344, 382, 56, "这里保留绝对时间值，不用“今天/明天”这类相对词，方便做精确回归。默认值为 2026-03-30 14:30:00。", DemoColors.Gray, DemoColors.WindowBg, 12, PageCommon.AlignLeft, true, page);

            app.Label(40, 598, 1320, 22, "1. GetD2DDateTimePickerDateTime / SetD2DDateTimePickerDateTime：读写具体时间。", DemoColors.Black, DemoColors.WindowBg, 12, PageCommon.AlignLeft, false, page);
            app.Label(40, 632, 1320, 22, "2. GetD2DDateTimePickerPrecision / SetD2DDateTimePickerPrecision：切换显示精度。", DemoColors.Black, DemoColors.WindowBg, 12, PageCommon.AlignLeft, false, page);
            app.Label(40, 666, 1320, 22, "3. GetD2DDateTimePickerColors / SetD2DDateTimePickerColors：切换颜色方案。", DemoColors.Black, DemoColors.WindowBg, 12, PageCommon.AlignLeft, false, page);
            app.Label(40, 700, 1320, 22, "4. SetD2DDateTimePickerBounds / EnableD2DDateTimePicker / ShowD2DDateTimePicker：切换布局与状态。", DemoColors.Black, DemoColors.WindowBg, 12, PageCommon.AlignLeft, false, page);

            Refresh("日期时间页已加载，可直接测试日期时间属性读取与设置");
        }
    }
}
