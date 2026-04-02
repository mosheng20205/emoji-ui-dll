using System;

namespace EmojiWindowDemo
{
    internal static class SliderDemoPage
    {
        public static void Build(AllDemoShell shell, IntPtr page)
        {
            var app = shell.App;
            IntPtr slider = EmojiWindowNative.CreateSlider(page, 56, 120, 520, 40, 0, 100, 35, 5, DemoColors.Blue, DemoColors.LightBlue);
            EmojiWindowNative.SetSliderShowStops(slider, 1);
            EmojiWindowNative.SetSliderColors(slider, DemoColors.Blue, DemoColors.LightBlue, DemoColors.Orange);

            IntPtr readout = app.Label(40, 184, 900, 72, string.Empty, DemoColors.Black, DemoColors.WindowBg, 13, PageCommon.AlignLeft, true, page);
            IntPtr state = app.Label(40, 760, 1360, 22, "Slider 页状态将在这里更新。", DemoColors.Blue, DemoColors.WindowBg, 12, PageCommon.AlignLeft, false, page);

            bool showStops = true;

            void Refresh(string note)
            {
                shell.SetLabelText(
                    readout,
                    $"value={EmojiWindowNative.GetSliderValue(slider)}  range=0..100  step=5  showStops={showStops}\r\n" +
                    $"{note}");
                shell.SetLabelText(state, note);
                shell.SetStatus(note);
            }

            var callback = app.Pin(new EmojiWindowNative.SliderCallback((_, value) => Refresh($"Slider 回调: value={value}")));
            EmojiWindowNative.SetSliderCallback(slider, callback);

            app.Button(40, 320, 120, 36, "设为 0", "0", DemoColors.Gray, () =>
            {
                EmojiWindowNative.SetSliderValue(slider, 0);
                Refresh("Slider 已设为 0");
            }, page);
            app.Button(176, 320, 120, 36, "设为 50", "50", DemoColors.Blue, () =>
            {
                EmojiWindowNative.SetSliderValue(slider, 50);
                Refresh("Slider 已设为 50");
            }, page);
            app.Button(312, 320, 120, 36, "设为 100", "100", DemoColors.Green, () =>
            {
                EmojiWindowNative.SetSliderValue(slider, 100);
                Refresh("Slider 已设为 100");
            }, page);
            app.Button(448, 320, 140, 36, "读取当前值", "📄", DemoColors.Orange, () => Refresh("已读取当前 Slider 值"), page);

            app.Button(40, 372, 140, 36, "蓝色方案", "💙", DemoColors.Blue, () =>
            {
                EmojiWindowNative.SetSliderColors(slider, DemoColors.Blue, DemoColors.LightBlue, DemoColors.Orange);
                Refresh("Slider 已切到蓝色方案");
            }, page);
            app.Button(196, 372, 140, 36, "绿色方案", "💚", DemoColors.Green, () =>
            {
                EmojiWindowNative.SetSliderColors(slider, DemoColors.Green, DemoColors.LightGreen, DemoColors.Blue);
                Refresh("Slider 已切到绿色方案");
            }, page);
            app.Button(352, 372, 140, 36, "停靠点开关", "⛳", DemoColors.Purple, () =>
            {
                showStops = !showStops;
                EmojiWindowNative.SetSliderShowStops(slider, showStops ? 1 : 0);
                Refresh(showStops ? "Slider 停靠点已开启" : "Slider 停靠点已关闭");
            }, page);
            app.Button(508, 372, 140, 36, "恢复默认", "↺", DemoColors.Gray, () =>
            {
                showStops = true;
                EmojiWindowNative.SetSliderValue(slider, 35);
                EmojiWindowNative.SetSliderShowStops(slider, 1);
                EmojiWindowNative.SetSliderColors(slider, DemoColors.Blue, DemoColors.LightBlue, DemoColors.Orange);
                Refresh("Slider 已恢复默认状态");
            }, page);

            app.Label(40, 598, 1320, 22, "1. GetSliderValue / SetSliderValue / SetSliderCallback：读取、设置并监听滑块数值。", DemoColors.Black, DemoColors.WindowBg, 12, PageCommon.AlignLeft, false, page);
            app.Label(40, 632, 1320, 22, "2. SetSliderShowStops：切换停靠点显示。", DemoColors.Black, DemoColors.WindowBg, 12, PageCommon.AlignLeft, false, page);
            app.Label(40, 666, 1320, 22, "3. SetSliderColors：切换激活轨道、背景轨道和圆点颜色。", DemoColors.Black, DemoColors.WindowBg, 12, PageCommon.AlignLeft, false, page);

            Refresh("Slider 页已加载，可直接测试数值、颜色和停靠点切换");
        }
    }
}
