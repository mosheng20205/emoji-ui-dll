using System;

namespace EmojiWindowDemo
{
    internal static class NotificationDemoPage
    {
        private const int NotifyInfo = 0;
        private const int NotifySuccess = 1;
        private const int NotifyWarning = 2;
        private const int NotifyError = 3;

        public static void Build(AllDemoShell shell, IntPtr page)
        {
            var app = shell.App;
            IntPtr state = app.Label(40, 184, 960, 72, "等待通知动作。", DemoColors.Black, DemoColors.WindowBg, 13, PageCommon.AlignLeft, true, page);

            var callback = app.Pin(new EmojiWindowNative.NotificationCallback((_, eventType) =>
            {
                string text = "Notification 回调: eventType=" + eventType;
                shell.SetLabelText(state, text);
                shell.SetStatus(text);
            }));

            void Show(string title, string message, int type, string note)
            {
                byte[] titleBytes = app.U(title);
                byte[] messageBytes = app.U(message);
                IntPtr handle = EmojiWindowNative.ShowNotification(app.Window, titleBytes, titleBytes.Length, messageBytes, messageBytes.Length, type, 1, 2500);
                if (handle != IntPtr.Zero)
                {
                    EmojiWindowNative.SetNotificationCallback(handle, callback);
                }

                shell.SetLabelText(state, note);
                shell.SetStatus(note);
            }

            app.Button(40, 110, 150, 40, "信息通知", "🔵", DemoColors.Blue, () => Show("🔔 信息通知", "📝 这是一条 info 类型通知。", NotifyInfo, "已弹出信息通知"), page);
            app.Button(206, 110, 150, 40, "成功通知", "🟢", DemoColors.Green, () => Show("✅ 操作成功", "🎉 这是一条 success 类型通知。", NotifySuccess, "已弹出成功通知"), page);
            app.Button(372, 110, 150, 40, "警告通知", "🟠", DemoColors.Orange, () => Show("⚠️ 注意", "📌 这是一条 warning 类型通知。", NotifyWarning, "已弹出警告通知"), page);
            app.Button(538, 110, 150, 40, "错误通知", "🔴", DemoColors.Red, () => Show("❌ 失败", "🧯 这是一条 error 类型通知。", NotifyError, "已弹出错误通知"), page);
            app.Button(704, 110, 150, 40, "连续两条", "📣", DemoColors.Purple, () =>
            {
                Show("📣 批量通知", "第一条通知。", NotifyInfo, "已连续弹出两条通知");
                Show("📣 批量通知", "第二条通知。", NotifySuccess, "已连续弹出两条通知");
            }, page);

            app.Label(40, 598, 1320, 22, "1. ShowNotification：分别演示 info / success / warning / error 四种通知类型。", DemoColors.Black, DemoColors.WindowBg, 12, PageCommon.AlignLeft, false, page);
            app.Label(40, 632, 1320, 22, "2. SetNotificationCallback：通知事件会把 eventType 回写到状态区。", DemoColors.Black, DemoColors.WindowBg, 12, PageCommon.AlignLeft, false, page);

            shell.SetStatus("Notification 页已加载，可直接验证四种通知和回调。");
        }
    }
}
