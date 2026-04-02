using System;

namespace EmojiWindowDemo
{
    internal static class PanelDemoPage
    {
        public static void Build(AllDemoShell shell, IntPtr page)
        {
            var app = shell.App;
            app.GroupBox(16, 16, 1448, 500, "🧱 Panel 组合布局", DemoColors.Border, DemoColors.WindowBg, page);
            app.GroupBox(16, 540, 1448, 208, "📘 Panel 使用说明", DemoColors.Border, DemoColors.WindowBg, page);
            app.Label(40, 56, 1368, 24, "Panel 页面改成更像实际软件界面的组合布局：侧栏、摘要、快捷操作区和活动流都放在同一块主工作区里。", DemoColors.Gray, DemoColors.WindowBg, 12, PageCommon.AlignLeft, false, page);

            IntPtr sidebar = EmojiWindowNative.CreatePanel(page, 40, 96, 260, 392, DemoColors.White);
            IntPtr stats = EmojiWindowNative.CreatePanel(page, 324, 96, 360, 184, DemoColors.Surface);
            IntPtr actions = EmojiWindowNative.CreatePanel(page, 708, 96, 716, 184, DemoColors.LightBlue);
            IntPtr feed = EmojiWindowNative.CreatePanel(page, 324, 304, 1100, 184, DemoColors.White);
            IntPtr stateLabel = app.Label(40, 586, 1368, 28, "等待 Panel 动作。", DemoColors.Blue, DemoColors.WindowBg, 13, PageCommon.AlignLeft, false, page);
            IntPtr detailLabel = app.Label(40, 626, 1368, 50, "Panel 适合把同一业务域的控件组织成一个视觉整体，既能当背景容器，也能承担布局分区。", DemoColors.Black, DemoColors.WindowBg, 12, PageCommon.AlignLeft, true, page);

            void SetState(string state, string detail = null)
            {
                shell.SetLabelText(stateLabel, state);
                if (!string.IsNullOrEmpty(detail))
                {
                    shell.SetLabelText(detailLabel, detail);
                }

                shell.SetStatus(state);
            }

            app.Label(18, 16, 200, 24, "🧭 左侧导航 Panel", DemoColors.Black, DemoColors.White, 14, PageCommon.AlignLeft, false, sidebar);
            app.Label(18, 58, 170, 22, "📋 项目总览", DemoColors.Blue, DemoColors.White, 12, PageCommon.AlignLeft, false, sidebar);
            app.Label(18, 88, 170, 22, "🧾 表单区域", DemoColors.Gray, DemoColors.White, 12, PageCommon.AlignLeft, false, sidebar);
            app.Label(18, 118, 170, 22, "📊 数据面板", DemoColors.Gray, DemoColors.White, 12, PageCommon.AlignLeft, false, sidebar);
            app.Label(18, 148, 170, 22, "⚙️ 设置中心", DemoColors.Gray, DemoColors.White, 12, PageCommon.AlignLeft, false, sidebar);
            app.Label(18, 178, 170, 22, "📨 消息归档", DemoColors.Gray, DemoColors.White, 12, PageCommon.AlignLeft, false, sidebar);
            app.Button(18, 240, 196, 38, "进入工作区", "🚀", DemoColors.Blue, () =>
            {
                SetState("Panel -> 进入工作区", "侧栏型 Panel 适合承载主导航、功能入口和分区切换。");
            }, sidebar);
            app.Button(18, 288, 196, 38, "说明消息", "💬", DemoColors.Gray, () =>
            {
                byte[] title = app.U("🧱 Panel");
                byte[] message = app.U("左侧导航 Panel 常用于后台导航与功能入口。");
                byte[] icon = app.U("💬");
                EmojiWindowNative.show_message_box_bytes(app.Window, title, title.Length, message, message.Length, icon, icon.Length);
                SetState("Panel -> 说明消息已弹出", "说明消息和状态反馈也可以集中在容器区域里处理。");
            }, sidebar);
            app.Button(18, 336, 196, 38, "固定视图", "📌", DemoColors.Green, () =>
            {
                SetState("Panel -> 视图已固定", "把一组按钮、标签和状态绑在同一个 Panel 里，后续整体移动和显示隐藏会更轻松。");
            }, sidebar);

            app.Label(18, 16, 200, 24, "📈 数据摘要 Panel", DemoColors.Black, DemoColors.Surface, 14, PageCommon.AlignLeft, false, stats);
            app.Label(18, 58, 120, 20, "🟦 活跃项目", DemoColors.Gray, DemoColors.Surface, 12, PageCommon.AlignLeft, false, stats);
            app.Label(18, 82, 96, 34, "12", DemoColors.Blue, DemoColors.Surface, 26, PageCommon.AlignLeft, false, stats);
            app.Label(190, 58, 140, 20, "✅ 已完成任务", DemoColors.Gray, DemoColors.Surface, 12, PageCommon.AlignLeft, false, stats);
            app.Label(190, 82, 96, 34, "86", DemoColors.Green, DemoColors.Surface, 26, PageCommon.AlignLeft, false, stats);
            app.Label(18, 134, 314, 40, "容器内的数字卡片、摘要信息、统计指标很适合放进独立 Panel。", DemoColors.Gray, DemoColors.Surface, 12, PageCommon.AlignLeft, true, stats);

            app.Label(18, 16, 220, 24, "⚡ 快捷操作 Panel", DemoColors.Black, DemoColors.LightBlue, 14, PageCommon.AlignLeft, false, actions);
            app.Label(18, 48, 420, 22, "适合高频按钮、状态开关、筛选器和即时操作入口。", DemoColors.Gray, DemoColors.LightBlue, 12, PageCommon.AlignLeft, false, actions);
            app.Button(18, 88, 136, 38, "新建任务", "➕", DemoColors.Blue, () =>
            {
                SetState("Panel -> 新建任务", "操作区 Panel 适合放页面里最高频的一组操作。");
            }, actions);
            app.Button(168, 88, 136, 38, "测试接口", "🧪", DemoColors.Green, () =>
            {
                SetState("Panel -> 测试接口", "把同一类动作集中在 Panel 里，能快速建立区域语义。");
            }, actions);
            app.Button(318, 88, 136, 38, "清理缓存", "🧹", DemoColors.Orange, () =>
            {
                SetState("Panel -> 清理缓存", "有背景层的 Panel 很适合承载工具区、命令区和局部工作台。");
            }, actions);
            app.Button(468, 88, 136, 38, "刷新视图", "🔁", DemoColors.Purple, () =>
            {
                SetState("Panel -> 刷新视图", "刷新、过滤、切换这些操作，放在同一个 Panel 里会比散落在页面上更整洁。");
            }, actions);
            app.Label(18, 142, 662, 28, "当前建议：把操作按钮和说明文字放到同一块 Panel 里，用户更容易理解这是一个功能区。", DemoColors.Gray, DemoColors.LightBlue, 12, PageCommon.AlignLeft, true, actions);

            app.Label(18, 16, 180, 24, "📰 活动流 Panel", DemoColors.Black, DemoColors.White, 14, PageCommon.AlignLeft, false, feed);
            app.Label(18, 56, 440, 20, "🕘 10:20  已完成主题切换兼容修复", DemoColors.Gray, DemoColors.White, 12, PageCommon.AlignLeft, false, feed);
            app.Label(18, 86, 440, 20, "🕘 10:32  已补全 Tooltip 四方向与主题展示", DemoColors.Gray, DemoColors.White, 12, PageCommon.AlignLeft, false, feed);
            app.Label(18, 116, 440, 20, "🕘 10:48  已增加树形版独立组件详情页", DemoColors.Gray, DemoColors.White, 12, PageCommon.AlignLeft, false, feed);
            app.Label(18, 146, 440, 20, "🕘 11:06  已完成 Switch / GroupBox 页面重排", DemoColors.Gray, DemoColors.White, 12, PageCommon.AlignLeft, false, feed);
            app.Label(520, 56, 548, 74, "这一块模拟的是消息流、日志流或最近活动区域。Panel 在这里最重要的作用，是把连续文本和操作上下文打包成一个稳定区块。", DemoColors.Gray, DemoColors.White, 12, PageCommon.AlignLeft, true, feed);

            app.Label(40, 688, 620, 24, "1. CreatePanel：创建背景容器和布局分区。", DemoColors.Black, DemoColors.WindowBg, 12, PageCommon.AlignLeft, false, page);
            app.Label(40, 718, 760, 24, "2. Panel 适合承载导航、摘要卡、工具区、活动流这类强语义区域。", DemoColors.Black, DemoColors.WindowBg, 12, PageCommon.AlignLeft, false, page);

            SetState("🧱 Panel 页已重排完成", "主工作区已经铺满页面，上方是 4 块组合 Panel，下方是状态和使用说明。");
        }
    }
}
