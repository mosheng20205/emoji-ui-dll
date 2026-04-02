using System;

namespace EmojiWindowDemo
{
    internal static class MessageBoxDemoPage
    {
        public static void Build(AllDemoShell shell, IntPtr page)
        {
            var app = shell.App;
            app.GroupBox(16, 16, 960, 260, "📝 MessageBox 功能演示", DemoColors.Border, DemoColors.WindowBg, page);
            app.GroupBox(1000, 16, 464, 176, "📘 MessageBox 说明", DemoColors.Border, DemoColors.WindowBg, page);
            app.GroupBox(16, 304, 1448, 220, "📡 最近动作", DemoColors.Border, DemoColors.WindowBg, page);

            IntPtr stateLabel = app.Label(40, 340, 1380, 28, "等待弹出消息框。", DemoColors.Blue, DemoColors.WindowBg, 13, PageCommon.AlignLeft, false, page);
            IntPtr detailLabel = app.Label(40, 384, 1380, 92, "消息框页不只是一个按钮，而是保留多种消息内容、图标和说明组件，进入页面后能直接验证普通消息、成功消息、警告消息和错误消息。", DemoColors.Black, DemoColors.WindowBg, 12, PageCommon.AlignLeft, true, page);

            void ShowMessage(string title, string message, string icon, string note, string detail)
            {
                byte[] titleBytes = app.U(title);
                byte[] messageBytes = app.U(message);
                byte[] iconBytes = app.U(icon);
                EmojiWindowNative.show_message_box_bytes(app.Window, titleBytes, titleBytes.Length, messageBytes, messageBytes.Length, iconBytes, iconBytes.Length);
                shell.SetLabelText(stateLabel, note);
                shell.SetLabelText(detailLabel, detail);
                shell.SetStatus(note);
            }

            app.Label(40, 56, 860, 24, "消息框页保留多种消息内容、图标和按钮，避免进入页面后只看到一个空按钮。", DemoColors.Gray, DemoColors.WindowBg, 12, PageCommon.AlignLeft, false, page);
            app.Button(40, 110, 150, 38, "普通消息框", "💬", DemoColors.Blue, () =>
            {
                ShowMessage("📝 普通消息", "🧪 这是普通消息框演示。", "💬", "MessageBox -> 普通消息", "普通消息适合提示成功以外的常规说明、帮助信息或状态反馈。");
            }, page);
            app.Button(206, 110, 150, 38, "成功消息", "✅", DemoColors.Green, () =>
            {
                ShowMessage("✅ 操作成功", "🎉 这是成功消息框演示。", "✅", "MessageBox -> 成功消息", "成功消息通常用于提交成功、保存完成、构建通过这类正向结果。");
            }, page);
            app.Button(372, 110, 150, 38, "警告消息", "⚠️", DemoColors.Orange, () =>
            {
                ShowMessage("⚠️ 注意", "📌 这是警告消息框演示。", "⚠️", "MessageBox -> 警告消息", "警告消息适合提醒用户继续操作前再确认上下文，但不一定是错误。");
            }, page);
            app.Button(538, 110, 150, 38, "错误消息", "❌", DemoColors.Red, () =>
            {
                ShowMessage("❌ 错误", "接口调用失败，请检查参数和日志输出。", "❌", "MessageBox -> 错误消息", "错误消息用于明确说明失败动作，通常应该给出原因或下一步处理建议。");
            }, page);

            app.Label(1024, 52, 396, 108, "1. `show_message_box_bytes` 直接弹出原生消息框。\r\n2. 标题、正文、图标全部走 UTF-8 字节传参。\r\n3. 这页重点是验证不同消息语义在 C# 集成页里都能直接跑通。", DemoColors.Black, DemoColors.WindowBg, 12, PageCommon.AlignLeft, true, page);

            shell.SetLabelText(stateLabel, "MessageBox 页已加载，可直接测试四种消息框。");
            shell.SetStatus("MessageBox 页已加载，可直接测试四种消息框。");
        }
    }
}
