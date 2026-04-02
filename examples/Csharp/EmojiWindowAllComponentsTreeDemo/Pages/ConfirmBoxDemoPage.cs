using System;

namespace EmojiWindowDemo
{
    internal static class ConfirmBoxDemoPage
    {
        public static void Build(AllDemoShell shell, IntPtr page)
        {
            var app = shell.App;
            app.GroupBox(16, 16, 960, 260, "❓ ConfirmBox 功能演示", DemoColors.Border, DemoColors.WindowBg, page);
            app.GroupBox(1000, 16, 464, 176, "📘 ConfirmBox 说明", DemoColors.Border, DemoColors.WindowBg, page);
            app.GroupBox(16, 304, 1448, 220, "📡 最近动作 / 回调结果", DemoColors.Border, DemoColors.WindowBg, page);

            string pendingAction = "尚未发起确认动作";
            IntPtr stateLabel = app.Label(40, 340, 1380, 28, "等待触发 ConfirmBox。", DemoColors.Blue, DemoColors.WindowBg, 13, PageCommon.AlignLeft, false, page);
            IntPtr detailLabel = app.Label(40, 384, 1380, 92, "确认框页保留多个触发入口，用来验证确定 / 取消回调，以及不同业务语义的确认提示是否都能被正确写回页面状态。", DemoColors.Black, DemoColors.WindowBg, 12, PageCommon.AlignLeft, true, page);

            var callback = app.Pin(new EmojiWindowNative.MessageBoxCallback(confirmed =>
            {
                string result = confirmed != 0 ? "确认" : "取消";
                string note = $"ConfirmBox 回调 -> {pendingAction} / {result}";
                shell.SetLabelText(stateLabel, note);
                shell.SetLabelText(detailLabel, $"最近动作：{pendingAction}\r\n用户选择：{result}\r\n这里验证的是 ConfirmBox 回调是否真的从 DLL 返回到 C# 页面状态，而不是只弹一个框。");
                shell.SetStatus(note);
            }));

            void ShowConfirm(string title, string message, string icon, string actionName, string note)
            {
                pendingAction = actionName;
                byte[] titleBytes = app.U(title);
                byte[] messageBytes = app.U(message);
                byte[] iconBytes = app.U(icon);
                EmojiWindowNative.show_confirm_box_bytes(app.Window, titleBytes, titleBytes.Length, messageBytes, messageBytes.Length, iconBytes, iconBytes.Length, callback);
                shell.SetLabelText(stateLabel, note);
                shell.SetLabelText(detailLabel, $"当前待确认动作：{actionName}\r\n确认框已弹出，等待用户点击“确认”或“取消”。");
                shell.SetStatus(note);
            }

            app.Label(40, 56, 860, 24, "确认框页保留多个触发入口，用来验证确定 / 取消回调。", DemoColors.Gray, DemoColors.WindowBg, 12, PageCommon.AlignLeft, false, page);
            app.Button(40, 110, 150, 38, "普通确认框", "❓", DemoColors.Orange, () =>
            {
                ShowConfirm("📣 ConfirmBox", "🧪 这是普通确认框演示。", "❓", "普通确认框", "ConfirmBox -> 普通确认框");
            }, page);
            app.Button(206, 110, 150, 38, "删除确认", "🧹", DemoColors.Red, () =>
            {
                ShowConfirm("🗑️ 删除确认", "⚠️ 这是一条删除确认提示。", "🗑️", "删除确认", "ConfirmBox -> 删除确认");
            }, page);
            app.Button(372, 110, 150, 38, "继续流程", "🚀", DemoColors.Blue, () =>
            {
                ShowConfirm("🚀 继续流程", "📌 确认继续执行下一步吗？", "🚀", "继续流程", "ConfirmBox -> 继续流程");
            }, page);
            app.Button(538, 110, 180, 38, "关闭当前标签页", "📑", DemoColors.Purple, () =>
            {
                ShowConfirm("📑 关闭标签页", "当前标签页还有未保存内容，确认关闭吗？", "📑", "关闭标签页", "ConfirmBox -> 关闭标签页");
            }, page);

            app.Label(1024, 52, 396, 108, "1. `show_confirm_box_bytes` 会在用户点击后回调 MessageBoxCallback。\r\n2. 这页保留多个业务语义入口，避免只验证一个固定场景。\r\n3. 最近动作和回调结果都会写回页面状态区。", DemoColors.Black, DemoColors.WindowBg, 12, PageCommon.AlignLeft, true, page);

            shell.SetLabelText(stateLabel, "ConfirmBox 页已加载，可直接验证确认 / 取消回调。");
            shell.SetStatus("ConfirmBox 页已加载，可直接验证确认 / 取消回调。");
        }
    }
}
