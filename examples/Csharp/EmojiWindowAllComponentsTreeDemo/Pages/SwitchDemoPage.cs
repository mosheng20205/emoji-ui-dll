using System;

namespace EmojiWindowDemo
{
    internal static class SwitchDemoPage
    {
        public static void Build(AllDemoShell shell, IntPtr page)
        {
            var app = shell.App;
            byte[] on = app.U("开启");
            byte[] off = app.U("关闭");
            IntPtr toggle = EmojiWindowNative.CreateSwitch(page, 56, 120, 120, 40, 1, DemoColors.Green, DemoColors.Gray, on, on.Length, off, off.Length);

            IntPtr readout = app.Label(40, 184, 900, 72, string.Empty, DemoColors.Black, DemoColors.WindowBg, 13, PageCommon.AlignLeft, true, page);
            IntPtr state = app.Label(40, 760, 1360, 22, "Switch 页状态将在这里更新。", DemoColors.Blue, DemoColors.WindowBg, 12, PageCommon.AlignLeft, false, page);
            string activeText = "开启";
            string inactiveText = "关闭";

            void Refresh(string note)
            {
                shell.SetLabelText(
                    readout,
                    $"state={EmojiWindowNative.GetSwitchState(toggle)}  activeText={activeText}  inactiveText={inactiveText}\r\n" +
                    $"{note}");
                shell.SetLabelText(state, note);
                shell.SetStatus(note);
            }

            var callback = app.Pin(new EmojiWindowNative.SwitchCallback((_, checkedState) =>
                Refresh("Switch 回调: checked=" + checkedState)));
            EmojiWindowNative.SetSwitchCallback(toggle, callback);

            app.Button(40, 320, 140, 36, "切换状态", "↺", DemoColors.Blue, () =>
            {
                EmojiWindowNative.SetSwitchState(toggle, EmojiWindowNative.GetSwitchState(toggle) == 0 ? 1 : 0);
                Refresh("程序切换了 Switch 状态");
            }, page);
            app.Button(196, 320, 140, 36, "读取状态", "📄", DemoColors.Green, () => Refresh("已读取当前 Switch 状态"), page);
            app.Button(352, 320, 140, 36, "启用/停用", "🔁", DemoColors.Orange, () =>
            {
                activeText = "启用";
                inactiveText = "停用";
                byte[] active = app.U(activeText);
                byte[] inactive = app.U(inactiveText);
                EmojiWindowNative.SetSwitchText(toggle, active, active.Length, inactive, inactive.Length);
                Refresh("Switch 文本已切到 启用 / 停用");
            }, page);
            app.Button(508, 320, 140, 36, "开/关", "🔤", DemoColors.Purple, () =>
            {
                activeText = "开";
                inactiveText = "关";
                byte[] active = app.U(activeText);
                byte[] inactive = app.U(inactiveText);
                EmojiWindowNative.SetSwitchText(toggle, active, active.Length, inactive, inactive.Length);
                Refresh("Switch 文本已切到 开 / 关");
            }, page);

            app.Button(40, 372, 140, 36, "恢复默认", "↩", DemoColors.Gray, () =>
            {
                activeText = "开启";
                inactiveText = "关闭";
                byte[] active = app.U(activeText);
                byte[] inactive = app.U(inactiveText);
                EmojiWindowNative.SetSwitchText(toggle, active, active.Length, inactive, inactive.Length);
                EmojiWindowNative.SetSwitchState(toggle, 1);
                Refresh("Switch 已恢复默认状态");
            }, page);

            app.Label(40, 598, 1320, 22, "1. GetSwitchState / SetSwitchState / SetSwitchCallback：读取、设置并监听开关状态。", DemoColors.Black, DemoColors.WindowBg, 12, PageCommon.AlignLeft, false, page);
            app.Label(40, 632, 1320, 22, "2. SetSwitchText：切换激活态和未激活态的显示文本。", DemoColors.Black, DemoColors.WindowBg, 12, PageCommon.AlignLeft, false, page);

            Refresh("Switch 页已加载，可直接测试状态切换和文本切换");
        }
    }
}
