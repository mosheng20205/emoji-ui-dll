using System;

namespace EmojiWindowDemo
{
    internal static class TooltipDemoPage
    {
        private const int PopupTop = 0;
        private const int PopupBottom = 1;
        private const int PopupLeft = 2;
        private const int PopupRight = 3;
        private const int ThemeDark = 1;
        private const int ThemeLight = 2;
        private const int ThemeCustom = 3;
        private const int TriggerHover = 0;
        private const int TriggerClick = 1;

        public static void Build(AllDemoShell shell, IntPtr page)
        {
            var app = shell.App;
            IntPtr state = app.Label(40, 760, 1360, 22, "Tooltip 页状态将在这里更新。", DemoColors.Blue, DemoColors.WindowBg, 12, PageCommon.AlignLeft, false, page);

            IntPtr topTarget = app.Label(40, 110, 210, 42, "Top / Dark / Hover", DemoColors.Blue, DemoColors.LightBlue, 13, PageCommon.AlignCenter, false, page);
            IntPtr bottomTarget = app.Label(270, 110, 220, 42, "Bottom / Light / Hover", DemoColors.Green, DemoColors.LightGreen, 13, PageCommon.AlignCenter, false, page);
            IntPtr leftTarget = app.Label(510, 110, 220, 42, "Left / Custom / Hover", DemoColors.Orange, DemoColors.Yellow, 13, PageCommon.AlignCenter, false, page);
            IntPtr rightTarget = app.Label(750, 110, 220, 42, "Right / Dark / Click", DemoColors.Purple, DemoColors.Surface, 13, PageCommon.AlignCenter, false, page);

            byte[] font = app.U("Microsoft YaHei UI");

            IntPtr topTooltip = CreateTooltip(app, page, "🖱️ Hover Top\ndark 主题 / 上方", PopupTop, ThemeDark, TriggerHover, DemoColors.Black, DemoColors.White, DemoColors.Border, font);
            IntPtr bottomTooltip = CreateTooltip(app, page, "🖱️ Hover Bottom\nlight 主题 / 下方", PopupBottom, ThemeLight, TriggerHover, DemoColors.White, DemoColors.Black, DemoColors.Border, font);
            IntPtr leftTooltip = CreateTooltip(app, page, "🖱️ Hover Left\ncustom 主题 / 左侧", PopupLeft, ThemeCustom, TriggerHover, DemoColors.Yellow, EmojiWindowNative.ARGB(255, 140, 74, 0), DemoColors.Orange, font);
            IntPtr rightTooltip = CreateTooltip(app, page, "🖱️ Click / Right / Dark\n点击目标显示，再点一次收起。", PopupRight, ThemeDark, TriggerClick, DemoColors.Black, DemoColors.White, DemoColors.Border, font);

            EmojiWindowNative.BindTooltipToControl(topTooltip, topTarget);
            EmojiWindowNative.BindTooltipToControl(bottomTooltip, bottomTarget);
            EmojiWindowNative.BindTooltipToControl(leftTooltip, leftTarget);
            EmojiWindowNative.BindTooltipToControl(rightTooltip, rightTarget);

            app.Button(40, 220, 160, 36, "主动显示 Top", "📌", DemoColors.Blue, () =>
            {
                EmojiWindowNative.ShowTooltipForControl(topTooltip, topTarget);
                shell.SetLabelText(state, "已主动显示 Top Tooltip");
                shell.SetStatus("已主动显示 Top Tooltip");
            }, page);
            app.Button(216, 220, 160, 36, "主动显示 Left", "📌", DemoColors.Green, () =>
            {
                EmojiWindowNative.ShowTooltipForControl(leftTooltip, leftTarget);
                shell.SetLabelText(state, "已主动显示 Left Tooltip");
                shell.SetStatus("已主动显示 Left Tooltip");
            }, page);
            app.Button(392, 220, 160, 36, "隐藏 Left", "🙈", DemoColors.Orange, () =>
            {
                EmojiWindowNative.HideTooltip(leftTooltip);
                shell.SetLabelText(state, "已隐藏 Left Tooltip");
                shell.SetStatus("已隐藏 Left Tooltip");
            }, page);
            app.Button(568, 220, 180, 36, "显示 Right Click", "🖱️", DemoColors.Purple, () =>
            {
                EmojiWindowNative.ShowTooltipForControl(rightTooltip, rightTarget);
                shell.SetLabelText(state, "已主动显示 Right Tooltip");
                shell.SetStatus("已主动显示 Right Tooltip");
            }, page);

            app.Label(40, 300, 1320, 22, "提示：Top / Bottom / Left 使用 Hover 触发；Right 使用 Click 触发。", DemoColors.Gray, DemoColors.WindowBg, 12, PageCommon.AlignLeft, false, page);
            app.Label(40, 598, 1320, 22, "1. CreateTooltip / BindTooltipToControl：创建并绑定四个不同方向的 Tooltip。", DemoColors.Black, DemoColors.WindowBg, 12, PageCommon.AlignLeft, false, page);
            app.Label(40, 632, 1320, 22, "2. SetTooltipTheme / SetTooltipPlacement / SetTooltipTrigger：切换主题、方向和触发方式。", DemoColors.Black, DemoColors.WindowBg, 12, PageCommon.AlignLeft, false, page);
            app.Label(40, 666, 1320, 22, "3. SetTooltipColors / SetTooltipFont / ShowTooltipForControl / HideTooltip：切换配色、字体并做主动显示隐藏。", DemoColors.Black, DemoColors.WindowBg, 12, PageCommon.AlignLeft, false, page);

            shell.SetStatus("Tooltip 页已加载，悬停或点击目标即可验证四个方向和三种主题。");
        }

        private static IntPtr CreateTooltip(DemoApp app, IntPtr page, string text, int placement, int theme, int trigger, uint bg, uint fg, uint border, byte[] font)
        {
            byte[] bytes = app.U(text);
            IntPtr tooltip = EmojiWindowNative.CreateTooltip(page, bytes, bytes.Length, placement, bg, fg);
            EmojiWindowNative.SetTooltipTheme(tooltip, theme);
            EmojiWindowNative.SetTooltipPlacement(tooltip, placement);
            EmojiWindowNative.SetTooltipTrigger(tooltip, trigger);
            EmojiWindowNative.SetTooltipColors(tooltip, bg, fg, border);
            EmojiWindowNative.SetTooltipFont(tooltip, font, font.Length, 14f);
            return tooltip;
        }
    }
}
