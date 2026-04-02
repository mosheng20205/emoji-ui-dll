using System;

namespace EmojiWindowDemo
{
    internal static class ButtonDemoPage
    {
        public static void Build(AllDemoShell shell, IntPtr page)
        {
            var app = shell.App;
            app.GroupBox(16, 16, 980, 248, "🔘 EmojiButton 属性读取", DemoColors.Border, DemoColors.WindowBg, page);
            app.GroupBox(16, 286, 980, 254, "🛠️ 按钮快捷设置", DemoColors.Border, DemoColors.WindowBg, page);
            app.GroupBox(1020, 212, 444, 328, "🎨 样式 / 可见性 / 启用态", DemoColors.Border, DemoColors.WindowBg, page);
            app.GroupBox(16, 558, 1448, 230, "📘 Button API 说明", DemoColors.Border, DemoColors.WindowBg, page);

            IntPtr stateLabel = app.Label(40, 760, 1360, 22, "按钮页状态将在这里更新。", DemoColors.Blue, DemoColors.WindowBg, 12, PageCommon.AlignLeft, false, page);
            IntPtr readout = app.Label(40, 184, 920, 56, "等待读取按钮属性…", DemoColors.Black, DemoColors.WindowBg, 12, PageCommon.AlignLeft, true, page);
            app.Label(40, 56, 900, 24, "这一页直接读取按钮文本、emoji、位置大小、颜色、类型、样式、尺寸、圆角、可见性和启用状态。", DemoColors.Gray, DemoColors.WindowBg, 12, PageCommon.AlignLeft, false, page);

            int demoButton = 0;
            demoButton = app.Button(56, 122, 208, 44, "主操作按钮", "🚀", DemoColors.Blue, () => Refresh("主演示按钮被点击"), page);

            string initialText = EmojiWindowNative.ReadUtf8(EmojiWindowNative.GetButtonText, demoButton);
            string initialEmoji = EmojiWindowNative.ReadUtf8(EmojiWindowNative.GetButtonEmoji, demoButton);
            EmojiWindowNative.GetButtonBounds(demoButton, out int initialX, out int initialY, out int initialWidth, out int initialHeight);
            uint initialBg = EmojiWindowNative.GetButtonBackgroundColor(demoButton);
            int initialType = EmojiWindowNative.GetButtonType(demoButton);
            int initialStyle = EmojiWindowNative.GetButtonStyle(demoButton);
            int initialSize = EmojiWindowNative.GetButtonSize(demoButton);
            int initialRound = EmojiWindowNative.GetButtonRound(demoButton);
            int initialCircle = EmojiWindowNative.GetButtonCircle(demoButton);
            int initialLoading = EmojiWindowNative.GetButtonLoading(demoButton);

            void Refresh(string note)
            {
                string text = EmojiWindowNative.ReadUtf8(EmojiWindowNative.GetButtonText, demoButton);
                string emoji = EmojiWindowNative.ReadUtf8(EmojiWindowNative.GetButtonEmoji, demoButton);
                EmojiWindowNative.GetButtonBounds(demoButton, out int x, out int y, out int width, out int height);
                uint bg = EmojiWindowNative.GetButtonBackgroundColor(demoButton);
                uint fg = EmojiWindowNative.GetButtonTextColor(demoButton);
                uint border = EmojiWindowNative.GetButtonBorderColor(demoButton);
                string visible = EmojiWindowNative.GetButtonVisible(demoButton) != 0 ? "显示" : "隐藏";
                string enabled = EmojiWindowNative.GetButtonEnabled(demoButton) != 0 ? "启用" : "禁用";

                shell.SetLabelText(
                    readout,
                    $"text={text}    emoji={emoji}    {visible}/{enabled}\r\n" +
                    $"bounds=({x}, {y}, {width}, {height})  type={ButtonTypeName(EmojiWindowNative.GetButtonType(demoButton))}  style={ButtonStyleName(EmojiWindowNative.GetButtonStyle(demoButton))}  size={ButtonSizeName(EmojiWindowNative.GetButtonSize(demoButton))}  round={EmojiWindowNative.GetButtonRound(demoButton)}  circle={EmojiWindowNative.GetButtonCircle(demoButton)}  loading={EmojiWindowNative.GetButtonLoading(demoButton)}\r\n" +
                    $"bg={PageCommon.FormatColor(bg)}  fg={PageCommon.FormatColor(fg)}  border={PageCommon.FormatColor(border)}");
                shell.SetLabelText(stateLabel, note);
                shell.SetStatus(note);
            }

            void SetButtonTextValue(string text, string note)
            {
                byte[] bytes = app.U(text);
                EmojiWindowNative.SetButtonText(demoButton, bytes, bytes.Length);
                Refresh(note);
            }

            void SetButtonEmojiValue(string emoji, string note)
            {
                byte[] bytes = app.U(emoji);
                EmojiWindowNative.SetButtonEmoji(demoButton, bytes, bytes.Length);
                Refresh(note);
            }

            void SetButtonColors(uint bg, uint fg, uint border, string note)
            {
                EmojiWindowNative.SetButtonBackgroundColor(demoButton, bg);
                EmojiWindowNative.SetButtonTextColor(demoButton, fg);
                EmojiWindowNative.SetButtonBorderColor(demoButton, border);
                EmojiWindowNative.SetButtonHoverColors(demoButton, bg, border, fg);
                Refresh(note);
            }

            void MoveButton(int dx = 0, int dy = 0, int dw = 0)
            {
                EmojiWindowNative.GetButtonBounds(demoButton, out int x, out int y, out int width, out int height);
                EmojiWindowNative.SetButtonBounds(demoButton, x + dx, y + dy, width + dw, height);
                Refresh($"按钮位置/尺寸已更新: dx={dx}, dy={dy}, dw={dw}");
            }

            void RestoreButton()
            {
                byte[] textBytes = app.U(initialText);
                byte[] emojiBytes = app.U(initialEmoji);
                EmojiWindowNative.SetButtonText(demoButton, textBytes, textBytes.Length);
                EmojiWindowNative.SetButtonEmoji(demoButton, emojiBytes, emojiBytes.Length);
                EmojiWindowNative.SetButtonBounds(demoButton, initialX, initialY, initialWidth, initialHeight);
                EmojiWindowNative.SetButtonBackgroundColor(demoButton, initialBg);
                EmojiWindowNative.ResetButtonColorOverrides(demoButton);
                EmojiWindowNative.SetButtonType(demoButton, initialType);
                EmojiWindowNative.SetButtonStyle(demoButton, initialStyle);
                EmojiWindowNative.SetButtonSize(demoButton, initialSize);
                EmojiWindowNative.SetButtonRound(demoButton, initialRound);
                EmojiWindowNative.SetButtonCircle(demoButton, initialCircle);
                EmojiWindowNative.SetButtonLoading(demoButton, initialLoading);
                EmojiWindowNative.ShowButton(demoButton, 1);
                EmojiWindowNative.EnableButton(page, demoButton, 1);
                Refresh("按钮属性已恢复默认");
            }

            app.Label(40, 326, 180, 22, "📝 文本 / Emoji", DemoColors.Black, DemoColors.WindowBg, 15, PageCommon.AlignLeft, false, page);
            app.Button(40, 360, 156, 36, "改成保存草稿", "📝", DemoColors.Blue, () => SetButtonTextValue("保存草稿", "按钮文本已改成“保存草稿”"), page);
            app.Button(212, 360, 156, 36, "改成立即发布", "✨", DemoColors.Green, () => SetButtonTextValue("立即发布", "按钮文本已改成“立即发布”"), page);
            app.Button(384, 360, 156, 36, "切换 Emoji", "🎯", DemoColors.Purple, () => SetButtonEmojiValue("🎯", "按钮 emoji 已切到 🎯"), page);

            app.Label(40, 414, 180, 22, "📐 位置 / 尺寸", DemoColors.Black, DemoColors.WindowBg, 15, PageCommon.AlignLeft, false, page);
            app.Button(40, 448, 140, 36, "右移 80", "➡️", DemoColors.Orange, () => MoveButton(dx: 80), page);
            app.Button(196, 448, 140, 36, "下移 24", "⬇️", DemoColors.Blue, () => MoveButton(dy: 24), page);
            app.Button(352, 448, 140, 36, "加宽 60", "↔️", DemoColors.Green, () => MoveButton(dw: 60), page);
            app.Button(508, 448, 140, 36, "恢复默认", "↺", DemoColors.Gray, RestoreButton, page);

            app.Label(1044, 252, 180, 22, "🎨 颜色 / 样式", DemoColors.Black, DemoColors.WindowBg, 15, PageCommon.AlignLeft, false, page);
            app.Button(1044, 286, 118, 36, "冷色", "💙", DemoColors.Blue, () => SetButtonColors(DemoColors.Blue, DemoColors.White, DemoColors.Blue, "按钮已切到冷色方案"), page);
            app.Button(1176, 286, 118, 36, "暖色", "🧡", DemoColors.Orange, () => SetButtonColors(DemoColors.Orange, DemoColors.White, DemoColors.Orange, "按钮已切到暖色方案"), page);
            app.Button(1308, 286, 118, 36, "浅灰", "🌫️", DemoColors.Gray, () => SetButtonColors(DemoColors.Surface, DemoColors.Gray, DemoColors.Border, "按钮已切到浅灰方案"), page);
            app.Button(1044, 336, 118, 36, "Plain", "🫧", DemoColors.Blue, () =>
            {
                EmojiWindowNative.SetButtonStyle(demoButton, PageCommon.ButtonStylePlain);
                Refresh("按钮样式已切到 plain");
            }, page);
            app.Button(1176, 336, 118, 36, "Link", "🔗", DemoColors.Green, () =>
            {
                EmojiWindowNative.SetButtonStyle(demoButton, PageCommon.ButtonStyleLink);
                Refresh("按钮样式已切到 link");
            }, page);
            app.Button(1308, 336, 118, 36, "Solid", "🧱", DemoColors.Gray, () =>
            {
                EmojiWindowNative.SetButtonStyle(demoButton, PageCommon.ButtonStyleSolid);
                Refresh("按钮样式已切回 solid");
            }, page);

            app.Label(1044, 390, 180, 22, "⚙️ 行为状态", DemoColors.Black, DemoColors.WindowBg, 15, PageCommon.AlignLeft, false, page);
            app.Button(1044, 424, 118, 36, "Round", "⭕", DemoColors.Blue, () =>
            {
                EmojiWindowNative.SetButtonRound(demoButton, 1);
                EmojiWindowNative.SetButtonCircle(demoButton, 0);
                Refresh("按钮已切到 round");
            }, page);
            app.Button(1176, 424, 118, 36, "Circle", "⚪", DemoColors.Green, () =>
            {
                EmojiWindowNative.SetButtonCircle(demoButton, 1);
                Refresh("按钮已切到 circle");
            }, page);
            app.Button(1308, 424, 118, 36, "Loading", "⏳", DemoColors.Purple, () =>
            {
                EmojiWindowNative.SetButtonLoading(demoButton, EmojiWindowNative.GetButtonLoading(demoButton) == 0 ? 1 : 0);
                Refresh("按钮 loading 状态已切换");
            }, page);
            app.Button(1044, 474, 118, 36, "禁用/启用", "🚫", DemoColors.Orange, () =>
            {
                EmojiWindowNative.EnableButton(page, demoButton, EmojiWindowNative.GetButtonEnabled(demoButton) == 0 ? 1 : 0);
                Refresh("按钮启用状态已切换");
            }, page);
            app.Button(1176, 474, 118, 36, "显示/隐藏", "👁️", DemoColors.Gray, () =>
            {
                EmojiWindowNative.ShowButton(demoButton, EmojiWindowNative.GetButtonVisible(demoButton) == 0 ? 1 : 0);
                Refresh("按钮可见状态已切换");
            }, page);
            app.Button(1308, 474, 118, 36, "Large", "📏", DemoColors.Blue, () =>
            {
                EmojiWindowNative.SetButtonSize(demoButton, PageCommon.ButtonSizeLarge);
                Refresh("按钮尺寸已切到 large");
            }, page);

            app.Label(40, 598, 760, 24, "1. GetButtonText / GetButtonEmoji / GetButtonBounds：读取文本、emoji 和位置尺寸。", DemoColors.Black, DemoColors.WindowBg, 12, PageCommon.AlignLeft, false, page);
            app.Label(40, 632, 760, 24, "2. SetButtonText / SetButtonEmoji / SetButtonBounds：直接修改按钮文案与布局。", DemoColors.Black, DemoColors.WindowBg, 12, PageCommon.AlignLeft, false, page);
            app.Label(40, 666, 760, 24, "3. GetButtonBackgroundColor / TextColor / BorderColor：读取按钮三类颜色。", DemoColors.Black, DemoColors.WindowBg, 12, PageCommon.AlignLeft, false, page);
            app.Label(40, 700, 940, 24, "4. SetButtonStyle / SetButtonType / SetButtonSize / SetButtonRound / SetButtonCircle / SetButtonLoading：读取和切换样式。", DemoColors.Black, DemoColors.WindowBg, 12, PageCommon.AlignLeft, false, page);
            app.Label(40, 734, 760, 24, "5. ShowButton / EnableButton：这里直接做真状态切换，不是静态文案。", DemoColors.Black, DemoColors.WindowBg, 12, PageCommon.AlignLeft, false, page);

            Refresh("按钮页已加载，可直接测试按钮属性读取与设置");
        }

        private static string ButtonTypeName(int type)
        {
            switch (type)
            {
                case 0: return "default";
                case 1: return "primary";
                case 2: return "success";
                case 3: return "warning";
                case 4: return "danger";
                case 5: return "info";
                default: return "unknown";
            }
        }

        private static string ButtonStyleName(int style)
        {
            switch (style)
            {
                case PageCommon.ButtonStyleSolid: return "solid";
                case PageCommon.ButtonStylePlain: return "plain";
                case PageCommon.ButtonStyleText: return "text";
                case PageCommon.ButtonStyleLink: return "link";
                default: return "unknown";
            }
        }

        private static string ButtonSizeName(int size)
        {
            switch (size)
            {
                case PageCommon.ButtonSizeLarge: return "large";
                case PageCommon.ButtonSizeDefault: return "default";
                case PageCommon.ButtonSizeSmall: return "small";
                default: return "unknown";
            }
        }
    }
}
