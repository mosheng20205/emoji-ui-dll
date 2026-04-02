using System;

namespace EmojiWindowDemo
{
    internal static class ProgressBarDemoPage
    {
        public static void Build(AllDemoShell shell, IntPtr page)
        {
            var app = shell.App;
            IntPtr progress = EmojiWindowNative.CreateProgressBar(page, 56, 120, 520, 30, 35, DemoColors.Blue, DemoColors.Border, 1, DemoColors.Black);
            EmojiWindowNative.SetProgressBarShowText(progress, 1);
            EmojiWindowNative.SetProgressBarTextColor(progress, DemoColors.Black);

            IntPtr readout = app.Label(40, 184, 900, 56, string.Empty, DemoColors.Black, DemoColors.WindowBg, 13, PageCommon.AlignLeft, true, page);
            IntPtr state = app.Label(40, 760, 1360, 22, "ProgressBar 页状态将在这里更新。", DemoColors.Blue, DemoColors.WindowBg, 12, PageCommon.AlignLeft, false, page);
            bool indeterminate = false;

            void Refresh(string note)
            {
                shell.SetLabelText(readout, $"value={EmojiWindowNative.GetProgressValue(progress)}  indeterminate={indeterminate}\r\n{note}");
                shell.SetLabelText(state, note);
                shell.SetStatus(note);
            }

            var callback = app.Pin(new EmojiWindowNative.ProgressBarCallback((_, value) => Refresh($"Progress 回调: value={value}")));
            EmojiWindowNative.SetProgressBarCallback(progress, callback);

            app.Button(40, 320, 140, 36, "进度 -10", "📉", DemoColors.Gray, () =>
            {
                EmojiWindowNative.SetProgressValue(progress, Math.Max(0, EmojiWindowNative.GetProgressValue(progress) - 10));
                Refresh("已将进度减少 10");
            }, page);
            app.Button(196, 320, 140, 36, "进度 +10", "📈", DemoColors.Blue, () =>
            {
                EmojiWindowNative.SetProgressValue(progress, Math.Min(100, EmojiWindowNative.GetProgressValue(progress) + 10));
                Refresh("已将进度增加 10");
            }, page);
            app.Button(352, 320, 140, 36, "显示文本", "🔤", DemoColors.Green, () =>
            {
                EmojiWindowNative.SetProgressBarShowText(progress, 1);
                Refresh("已开启进度文本显示");
            }, page);
            app.Button(508, 320, 140, 36, "隐藏文本", "🙈", DemoColors.Orange, () =>
            {
                EmojiWindowNative.SetProgressBarShowText(progress, 0);
                Refresh("已关闭进度文本显示");
            }, page);

            app.Button(40, 372, 140, 36, "冷色方案", "💙", DemoColors.Blue, () =>
            {
                EmojiWindowNative.SetProgressBarTextColor(progress, DemoColors.Black);
                Refresh("进度条已切到冷色方案");
            }, page);
            app.Button(196, 372, 140, 36, "警示方案", "🧡", DemoColors.Orange, () =>
            {
                EmojiWindowNative.SetProgressBarTextColor(progress, DemoColors.Red);
                Refresh("进度条已切到警示方案");
            }, page);
            app.Button(352, 372, 140, 36, "不确定模式", "∞", DemoColors.Purple, () =>
            {
                indeterminate = !indeterminate;
                EmojiWindowNative.SetProgressIndeterminate(progress, indeterminate ? 1 : 0);
                Refresh(indeterminate ? "已开启不确定模式" : "已关闭不确定模式");
            }, page);
            app.Button(508, 372, 140, 36, "恢复默认", "↺", DemoColors.Green, () =>
            {
                indeterminate = false;
                EmojiWindowNative.SetProgressIndeterminate(progress, 0);
                EmojiWindowNative.SetProgressValue(progress, 35);
                EmojiWindowNative.SetProgressBarShowText(progress, 1);
                EmojiWindowNative.SetProgressBarTextColor(progress, DemoColors.Black);
                Refresh("ProgressBar 已恢复默认状态");
            }, page);

            app.Label(40, 598, 1320, 22, "1. GetProgressValue / SetProgressValue / SetProgressBarCallback：读取、写入进度值并监听变化。", DemoColors.Black, DemoColors.WindowBg, 12, PageCommon.AlignLeft, false, page);
            app.Label(40, 632, 1320, 22, "2. SetProgressIndeterminate：切换不确定模式。", DemoColors.Black, DemoColors.WindowBg, 12, PageCommon.AlignLeft, false, page);
            app.Label(40, 666, 1320, 22, "3. SetProgressBarShowText / SetProgressBarTextColor：切换文本显示和文本颜色。", DemoColors.Black, DemoColors.WindowBg, 12, PageCommon.AlignLeft, false, page);

            Refresh("ProgressBar 页已加载，可直接测试进度值、不确定模式和文本显示");
        }
    }
}
