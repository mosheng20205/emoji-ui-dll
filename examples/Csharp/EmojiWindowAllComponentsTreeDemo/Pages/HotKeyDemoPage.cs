using System;

namespace EmojiWindowDemo
{
    internal static class HotKeyDemoPage
    {
        public static void Build(AllDemoShell shell, IntPtr page)
        {
            var app = shell.App;
            IntPtr hotKey = EmojiWindowNative.CreateHotKeyControl(page, 48, 110, 280, 38, DemoColors.Black, DemoColors.White);
            EmojiWindowNative.SetHotKeyColors(hotKey, DemoColors.Black, DemoColors.White, DemoColors.Border);

            IntPtr readout = app.Label(40, 176, 820, 76, string.Empty, DemoColors.Black, DemoColors.WindowBg, 13, PageCommon.AlignLeft, true, page);
            IntPtr state = app.Label(40, 760, 1360, 22, "热键页状态将在这里更新。", DemoColors.Blue, DemoColors.WindowBg, 12, PageCommon.AlignLeft, false, page);

            int x = 48;
            int y = 110;
            int width = 280;
            int height = 38;
            bool visible = true;
            bool enabled = true;

            void Refresh(string note)
            {
                EmojiWindowNative.GetHotKey(hotKey, out int vkCode, out int modifiers);
                EmojiWindowNative.GetHotKeyColors(hotKey, out uint fg, out uint bg, out uint border);
                shell.SetLabelText(
                    readout,
                    $"hotkey={PageCommon.FormatHotKey(vkCode, modifiers)}  {(visible ? "显示" : "隐藏")}/{(enabled ? "启用" : "禁用")}\r\n" +
                    $"bounds=({x}, {y}, {width}, {height})\r\n" +
                    $"fg={PageCommon.FormatColor(fg)}  bg={PageCommon.FormatColor(bg)}  border={PageCommon.FormatColor(border)}");
                shell.SetLabelText(state, note);
                shell.SetStatus(note);
            }

            var callback = app.Pin(new EmojiWindowNative.HotKeyCallback((_, vkCode, modifiers) =>
                Refresh("热键回调: " + PageCommon.FormatHotKey(vkCode, modifiers))));
            EmojiWindowNative.SetHotKeyCallback(hotKey, callback);

            app.Button(40, 310, 166, 36, "Ctrl+Shift+S", "⌨️", DemoColors.Blue, () =>
            {
                EmojiWindowNative.SetHotKey(hotKey, 0x53, 3);
                Refresh("热键已设为 Ctrl+Shift+S");
            }, page);
            app.Button(222, 310, 166, 36, "Alt+F4", "⚠️", DemoColors.Orange, () =>
            {
                EmojiWindowNative.SetHotKey(hotKey, 0x73, 4);
                Refresh("热键已设为 Alt+F4");
            }, page);
            app.Button(404, 310, 166, 36, "清空热键", "🧹", DemoColors.Red, () =>
            {
                EmojiWindowNative.ClearHotKey(hotKey);
                Refresh("热键已清空");
            }, page);
            app.Button(586, 310, 166, 36, "立即读取", "📡", DemoColors.Green, () => Refresh("已重新读取当前热键"), page);

            app.Button(40, 362, 140, 36, "冷色方案", "💙", DemoColors.Blue, () =>
            {
                EmojiWindowNative.SetHotKeyColors(hotKey, DemoColors.Black, DemoColors.LightBlue, DemoColors.Blue);
                Refresh("热键控件已切到冷色方案");
            }, page);
            app.Button(196, 362, 140, 36, "暖色方案", "🧡", DemoColors.Orange, () =>
            {
                EmojiWindowNative.SetHotKeyColors(hotKey, DemoColors.Black, DemoColors.Yellow, DemoColors.Orange);
                Refresh("热键控件已切到暖色方案");
            }, page);
            app.Button(352, 362, 140, 36, "右移 80", "➡️", DemoColors.Green, () =>
            {
                x = 128;
                EmojiWindowNative.SetHotKeyControlBounds(hotKey, x, y, width, height);
                Refresh("热键控件已右移 80");
            }, page);
            app.Button(508, 362, 140, 36, "加宽 100", "↔️", DemoColors.Purple, () =>
            {
                width = 380;
                EmojiWindowNative.SetHotKeyControlBounds(hotKey, x, y, width, height);
                Refresh("热键控件已加宽到 380");
            }, page);

            app.Button(1044, 286, 118, 36, "禁用/启用", "🚫", DemoColors.Blue, () =>
            {
                enabled = !enabled;
                EmojiWindowNative.EnableHotKeyControl(hotKey, enabled ? 1 : 0);
                Refresh("热键控件启用状态已切换");
            }, page);
            app.Button(1176, 286, 118, 36, "显示/隐藏", "👁️", DemoColors.Gray, () =>
            {
                visible = !visible;
                EmojiWindowNative.ShowHotKeyControl(hotKey, visible ? 1 : 0);
                Refresh("热键控件可见状态已切换");
            }, page);
            app.Button(1308, 286, 118, 36, "恢复默认", "↺", DemoColors.Green, () =>
            {
                x = 48;
                y = 110;
                width = 280;
                height = 38;
                visible = true;
                enabled = true;
                EmojiWindowNative.SetHotKeyControlBounds(hotKey, x, y, width, height);
                EmojiWindowNative.SetHotKeyColors(hotKey, DemoColors.Black, DemoColors.White, DemoColors.Border);
                EmojiWindowNative.EnableHotKeyControl(hotKey, 1);
                EmojiWindowNative.ShowHotKeyControl(hotKey, 1);
                EmojiWindowNative.ClearHotKey(hotKey);
                Refresh("热键页已恢复默认状态");
            }, page);

            app.Label(40, 598, 1320, 22, "1. GetHotKey / SetHotKey / ClearHotKey：读取、预设和清空热键。", DemoColors.Black, DemoColors.WindowBg, 12, PageCommon.AlignLeft, false, page);
            app.Label(40, 632, 1320, 22, "2. SetHotKeyCallback：热键变化会把格式化后的组合键写回页面状态。", DemoColors.Black, DemoColors.WindowBg, 12, PageCommon.AlignLeft, false, page);
            app.Label(40, 666, 1320, 22, "3. GetHotKeyColors / SetHotKeyColors：读取和切换前景、背景与边框色。", DemoColors.Black, DemoColors.WindowBg, 12, PageCommon.AlignLeft, false, page);
            app.Label(40, 700, 1320, 22, "4. SetHotKeyControlBounds / EnableHotKeyControl / ShowHotKeyControl：切换布局与状态。", DemoColors.Black, DemoColors.WindowBg, 12, PageCommon.AlignLeft, false, page);

            Refresh("热键页已加载，可直接测试热键属性读取与设置");
        }
    }
}
