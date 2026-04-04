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

            app.GroupBox(16, 16, 980, 520, "🔔 Notification 舞台区", DemoTheme.Border, DemoTheme.Background, page);
            app.GroupBox(1020, 16, 444, 252, "🪄 通知触发区", DemoTheme.Border, DemoTheme.Background, page);
            app.GroupBox(1020, 284, 444, 252, "📋 状态 / 回调读数区", DemoTheme.Border, DemoTheme.Background, page);
            app.GroupBox(16, 558, 1448, 220, "📘 Notification API 说明", DemoTheme.Border, DemoTheme.Background, page);

            app.Label(
                40,
                56,
                920,
                42,
                "通知固定从软件窗口右上角弹出。这里把四种语义通知和最近一次事件回写拆开，避免页面只剩下一排按钮。",
                DemoTheme.Muted,
                DemoTheme.Background,
                12,
                PageCommon.AlignLeft,
                true,
                page);
            app.Label(56, 118, 260, 22, "🎯 语义预览卡", DemoTheme.Text, DemoTheme.Background, 14, PageCommon.AlignLeft, false, page);

            IntPtr cardInfo = app.Label(56, 156, 196, 64, "🔵 信息通知\r\n说明常规状态变化", DemoTheme.Primary, DemoTheme.SurfacePrimary, 13, PageCommon.AlignCenter, true, page);
            IntPtr cardSuccess = app.Label(276, 156, 196, 64, "🟢 成功通知\r\n用于完成反馈", DemoTheme.Success, DemoTheme.SurfaceSuccess, 13, PageCommon.AlignCenter, true, page);
            IntPtr cardWarning = app.Label(496, 156, 196, 64, "🟠 警告通知\r\n提醒用户注意风险", DemoTheme.Warning, DemoTheme.SurfaceWarning, 13, PageCommon.AlignCenter, true, page);
            IntPtr cardError = app.Label(716, 156, 196, 64, "🔴 错误通知\r\n用于失败反馈", DemoTheme.Danger, DemoTheme.SurfaceDanger, 13, PageCommon.AlignCenter, true, page);
            IntPtr stageHint = app.Label(
                56,
                252,
                860,
                56,
                "上面四张卡就是右侧按钮的语义映射。点击任意按钮时，页面会先把当前通知意图写回，再弹出真实通知。",
                DemoTheme.Muted,
                DemoTheme.Background,
                12,
                PageCommon.AlignLeft,
                true,
                page);
            app.Label(56, 340, 260, 22, "🧭 连续通知场景", DemoTheme.Text, DemoTheme.Background, 14, PageCommon.AlignLeft, false, page);
            IntPtr stageTip = app.Label(
                56,
                372,
                860,
                76,
                "“连续两条”用来验证消息叠加和回调是否还能正确写回状态区。Python 版也是通过语义色和分区来避免通知页看起来很扁平。",
                DemoTheme.Muted,
                DemoTheme.Background,
                12,
                PageCommon.AlignLeft,
                true,
                page);

            IntPtr stateLabel = app.Label(1044, 324, 396, 24, "等待触发通知。", DemoTheme.Primary, DemoTheme.Background, 12, PageCommon.AlignLeft, false, page);
            IntPtr detailLabel = app.Label(
                1044,
                360,
                396,
                112,
                "状态区会记录最近一次通知标题、类型，以及回调返回的事件编号。",
                DemoTheme.Text,
                DemoTheme.Background,
                12,
                PageCommon.AlignLeft,
                true,
                page);
            IntPtr readoutHint = app.Label(
                1044,
                484,
                396,
                28,
                "事件编号由 DLL 回调返回，这里只负责如实回写。",
                DemoTheme.Muted,
                DemoTheme.Background,
                12,
                PageCommon.AlignLeft,
                true,
                page);

            string currentType = "信息通知";
            string currentTitle = "🔔 信息通知";
            string lastEvent = "尚未回调";

            void Refresh(string note)
            {
                shell.SetLabelText(stateLabel, note);
                shell.SetLabelText(
                    detailLabel,
                    $"最近通知类型：{currentType}\r\n" +
                    $"最近通知标题：{currentTitle}\r\n" +
                    $"最近事件：{lastEvent}");
                shell.SetStatus(note);
            }

            string NotificationEventName(int eventType)
            {
                switch (eventType)
                {
                    case 0: return "eventType=0";
                    case 1: return "eventType=1";
                    case 2: return "eventType=2";
                    default: return "eventType=" + eventType;
                }
            }

            var callback = app.Pin(new EmojiWindowNative.NotificationCallback((_, eventType) =>
            {
                lastEvent = NotificationEventName(eventType);
                Refresh($"Notification 回调 -> {currentType} / {lastEvent}");
            }));

            void Show(string title, string message, int type, string typeName, string note)
            {
                currentTitle = title;
                currentType = typeName;
                lastEvent = "等待回调";

                byte[] titleBytes = app.U(title);
                byte[] messageBytes = app.U(message);
                IntPtr handle = EmojiWindowNative.ShowNotification(app.Window, titleBytes, titleBytes.Length, messageBytes, messageBytes.Length, type, 0, 2500);
                if (handle != IntPtr.Zero)
                {
                    EmojiWindowNative.SetNotificationCallback(handle, callback);
                }

                Refresh(note);
            }

            app.Label(1044, 56, 380, 42, "按钮按通知语义分组，和舞台区四张预览卡一一对应；这样切主题后层次也不会散。", DemoTheme.Muted, DemoTheme.Background, 12, PageCommon.AlignLeft, true, page);
            app.Button(1044, 122, 116, 38, "信息通知", "🔵", DemoColors.Blue, () =>
            {
                Show("🔔 信息通知", "📝 这是一条 info 类型通知。", NotifyInfo, "信息通知", "Notification -> 信息通知");
            }, page);
            app.Button(1172, 122, 116, 38, "成功通知", "🟢", DemoColors.Green, () =>
            {
                Show("✅ 操作成功", "🎉 这是一条 success 类型通知。", NotifySuccess, "成功通知", "Notification -> 成功通知");
            }, page);
            app.Button(1300, 122, 124, 38, "警告通知", "🟠", DemoColors.Orange, () =>
            {
                Show("⚠️ 注意", "📌 这是一条 warning 类型通知。", NotifyWarning, "警告通知", "Notification -> 警告通知");
            }, page);
            app.Button(1044, 176, 116, 38, "错误通知", "🔴", DemoColors.Red, () =>
            {
                Show("❌ 失败", "🧯 这是一条 error 类型通知。", NotifyError, "错误通知", "Notification -> 错误通知");
            }, page);
            app.Button(1172, 176, 252, 38, "连续两条", "📣", DemoColors.Purple, () =>
            {
                Show("📣 批量通知", "第一条通知。", NotifyInfo, "批量通知 / 第 1 条", "Notification -> 连续两条（第 1 条）");
                Show("📣 批量通知", "第二条通知。", NotifySuccess, "批量通知 / 第 2 条", "Notification -> 连续两条（第 2 条）");
            }, page);

            app.Label(40, 598, 1360, 24, "1. `ShowNotification`：弹出通知并返回通知句柄，可继续绑定回调。", DemoTheme.Text, DemoTheme.Background, 12, PageCommon.AlignLeft, false, page);
            app.Label(40, 632, 1360, 24, "2. `SetNotificationCallback`：把通知关闭、点击等事件通过 `eventType` 写回读数区。", DemoTheme.Text, DemoTheme.Background, 12, PageCommon.AlignLeft, false, page);
            app.Label(40, 666, 1360, 24, "3. 舞台区四张语义卡对应 info / success / warning / error，和 Python 版的视觉引导一致。", DemoTheme.Text, DemoTheme.Background, 12, PageCommon.AlignLeft, false, page);
            app.Label(40, 700, 1360, 24, "4. 连续两条用于验证通知叠加时状态仍能持续回写，而不是页面只展示静态按钮。", DemoTheme.Text, DemoTheme.Background, 12, PageCommon.AlignLeft, false, page);

            void ApplyTheme()
            {
                shell.SetLabelText(stageHint, shell.Palette.Dark
                    ? "深色主题下保留语义卡和读数区分层，通知页不会退化成一排高饱和按钮。"
                    : "浅色主题下通过语义卡 + 状态区，让 Notification 页的阅读顺序更像 Python 版。");
                shell.SetLabelText(stageTip, shell.Palette.Dark
                    ? "深色主题中连续通知更容易显得杂乱，所以这里专门留出状态区承接回调。"
                    : "浅色主题下把批量通知单独说明，页面分区会比旧版更稳。");
                shell.SetLabelText(readoutHint, shell.Palette.Dark
                    ? "回调只回写 eventType，本页负责把它放进可读的状态上下文里。"
                    : "状态区不仅显示标题，还会携带通知类型和事件回写结果。");
                Refresh(lastEvent == "尚未回调" ? "Notification 页面已加载，可直接验证四种通知和回调。" : $"Notification 回调 -> {currentType} / {lastEvent}");
            }

            shell.RegisterPageThemeHandler(page, ApplyTheme);
            ApplyTheme();
            _ = cardInfo;
            _ = cardSuccess;
            _ = cardWarning;
            _ = cardError;
        }
    }
}
