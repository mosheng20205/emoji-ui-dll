using System;

namespace EmojiWindowDemo
{
    internal static class GroupBoxDemoPage
    {
        public static void Build(AllDemoShell shell, IntPtr page)
        {
            var app = shell.App;
            IntPtr outline = app.GroupBox(16, 16, 692, 284, "Outline 分组框", DemoColors.Blue, DemoColors.WindowBg, page);
            IntPtr card = app.GroupBox(756, 16, 692, 284, "Card 分组框", DemoColors.Orange, DemoColors.Surface, page);
            IntPtr header = app.GroupBox(16, 336, 692, 284, "Header Bar 分组框", DemoColors.Green, DemoColors.Surface, page);
            IntPtr noteBox = app.GroupBox(756, 336, 692, 284, "C# GroupBox 正确用法", DemoColors.Purple, DemoColors.Surface, page);

            EmojiWindowNative.SetGroupBoxStyle(outline, PageCommon.GroupBoxStyleOutline);
            EmojiWindowNative.SetGroupBoxStyle(card, PageCommon.GroupBoxStyleCard);
            EmojiWindowNative.SetGroupBoxStyle(header, PageCommon.GroupBoxStyleHeaderBar);
            EmojiWindowNative.SetGroupBoxStyle(noteBox, PageCommon.GroupBoxStyleCard);
            EmojiWindowNative.SetGroupBoxTitleColor(header, DemoColors.Green);
            EmojiWindowNative.SetGroupBoxTitleColor(noteBox, DemoColors.Purple);

            IntPtr outlineTitle = app.Label(34, 58, 628, 24, "Outline 风格更适合轻量分区。", DemoColors.Black, DemoColors.WindowBg, 14, PageCommon.AlignLeft, false, page);
            IntPtr outlineBody = app.Label(34, 90, 628, 44, "适合配置区、属性区、筛选栏这类弱层级布局，重点是把区域分开，而不是制造很重的卡片感。", DemoColors.Gray, DemoColors.WindowBg, 12, PageCommon.AlignLeft, true, page);
            IntPtr outlineFoot = app.Label(34, 206, 620, 24, "适合用在设置面板、筛选栏、轻分组块。", DemoColors.Gray, DemoColors.WindowBg, 12, PageCommon.AlignLeft, false, page);
            app.AttachToGroup(outline, outlineTitle, outlineBody, outlineFoot);

            IntPtr cardTitle = app.Label(774, 58, 628, 24, "Card 风格适合信息卡片和表单块。", DemoColors.Black, DemoColors.Surface, 14, PageCommon.AlignLeft, false, page);
            IntPtr cardBody = app.Label(774, 90, 628, 44, "Card 会提供更完整的容器感，适合资料摘要、状态卡片、表单区和需要承载连续内容的区域。", DemoColors.Gray, DemoColors.Surface, 12, PageCommon.AlignLeft, true, page);
            IntPtr cardFoot = app.Label(774, 206, 620, 24, "常用于资料卡、属性面板、摘要信息。", DemoColors.Gray, DemoColors.Surface, 12, PageCommon.AlignLeft, false, page);
            app.AttachToGroup(card, cardTitle, cardBody, cardFoot);

            IntPtr headerTitle = app.Label(34, 390, 628, 24, "Header Bar 风格适合主功能区标题。", DemoColors.Black, DemoColors.Surface, 14, PageCommon.AlignLeft, false, page);
            IntPtr headerBody = app.Label(34, 424, 628, 44, "顶部有更明确的抬头区，适合承担一级业务模块、工具区或页面主功能块，比 Outline 和 Card 更有章节感。", DemoColors.Gray, DemoColors.Surface, 12, PageCommon.AlignLeft, true, page);
            IntPtr headerFoot = app.Label(34, 554, 620, 24, "更适合大页面里的一级业务模块。", DemoColors.Gray, DemoColors.Surface, 12, PageCommon.AlignLeft, false, page);
            app.AttachToGroup(header, headerTitle, headerBody, headerFoot);

            IntPtr noteTitle = app.Label(774, 390, 628, 24, "C# 里不要把 HWND 控件直接建到 GroupBox 句柄上。", DemoColors.Black, DemoColors.Surface, 14, PageCommon.AlignLeft, false, page);
            IntPtr noteBody = app.Label(774, 424, 628, 96, "正确做法：先把 Label、EditBox、ListBox 这类 HWND 控件创建在页面或普通 Panel 上，再用 AddChildToGroup / AttachToGroup 做分组归属。这样既能保留视觉分组，也能避开直接挂到 GroupBox 句柄后出现空白的问题。按钮页内的 D2D Button 可以单独放在分组区域内，不必强行挂组。", DemoColors.Gray, DemoColors.Surface, 12, PageCommon.AlignLeft, true, page);
            IntPtr noteFoot = app.Label(774, 544, 620, 44, "这页故意把说明文本都创建在 page 上，再统一 AttachToGroup，用来强调这个规则。", DemoColors.Purple, DemoColors.Surface, 12, PageCommon.AlignLeft, true, page);
            app.AttachToGroup(noteBox, noteTitle, noteBody, noteFoot);

            IntPtr stateLabel = app.Label(16, 648, 1448, 24, "GroupBox 页状态将在这里更新。", DemoColors.Blue, DemoColors.WindowBg, 13, PageCommon.AlignLeft, false, page);
            IntPtr detailLabel = app.Label(16, 682, 1448, 66, "这页重点不是只看三种样式，而是强调 C# 的 GroupBox 宿主规则：GroupBox 负责视觉分组，普通 HWND 子控件先建到页面，再附着进组。", DemoColors.Black, DemoColors.WindowBg, 12, PageCommon.AlignLeft, true, page);

            void SetState(string state, string detail = null)
            {
                shell.SetLabelText(stateLabel, state);
                if (!string.IsNullOrEmpty(detail))
                {
                    shell.SetLabelText(detailLabel, detail);
                }

                shell.SetStatus(state);
            }

            app.Button(34, 144, 148, 38, "读取样式", "🔎", DemoColors.Blue, () =>
            {
                SetState("GroupBox -> Outline",
                    "Outline 更轻，适合做设置区和筛选区。这里按钮仍直接建在 page 上，只是视觉位置落在 GroupBox 区域里。");
            }, page);

            app.Button(196, 144, 148, 38, "测试按钮", "🧪", DemoColors.Green, () =>
            {
                SetState("GroupBox -> Outline 按钮点击",
                    "如果这颗按钮能正常显示和点击，说明页面级按钮与组内 HWND 文本可以稳定混排。");
            }, page);

            app.Button(774, 144, 136, 38, "消息", "💬", DemoColors.Orange, () =>
            {
                byte[] title = app.U("📦 GroupBox");
                byte[] message = app.U("这是 Card 分组框里的消息框。");
                byte[] icon = app.U("💬");
                EmojiWindowNative.show_message_box_bytes(app.Window, title, title.Length, message, message.Length, icon, icon.Length);
                SetState("GroupBox -> Card 弹出消息框",
                    "Card 适合承载连续内容和局部操作，消息按钮放在这里更像资料卡里的操作栏。");
            }, page);

            app.Button(924, 144, 148, 38, "标题色", "🎨", DemoColors.Purple, () =>
            {
                EmojiWindowNative.SetGroupBoxTitleColor(card, DemoColors.Purple);
                SetState("GroupBox -> Card 标题色已修改",
                    "Card 分组框标题色已切到紫色，验证标题色接口与卡片风格可以组合使用。");
            }, page);

            app.Button(34, 490, 148, 38, "成功动作", "✅", DemoColors.Green, () =>
            {
                SetState("GroupBox -> Header Bar 成功动作",
                    "Header Bar 更像大章节标题，适合主功能区和一级业务模块。");
            }, page);

            app.Button(196, 490, 148, 38, "状态动作", "📌", DemoColors.Blue, () =>
            {
                byte[] text = app.U("Header Bar 已切成运营中心");
                EmojiWindowNative.SetGroupBoxTitle(header, text, text.Length);
                SetState("GroupBox -> Header Bar 标题已更新",
                    "Header Bar 可以承担明显的章节标题，改标题后的层级感会比 Outline 更强。");
            }, page);

            app.Button(774, 250, 200, 38, "把右下标题改成安全说明", "✏️", DemoColors.Purple, () =>
            {
                byte[] text = app.U("C# GroupBox 安全说明");
                EmojiWindowNative.SetGroupBoxTitle(noteBox, text, text.Length);
                SetState("GroupBox -> 说明分组标题已更新",
                    "这里演示的不是样式，而是把规则直接写在页面里，避免后续 C# 示例再踩 GroupBox 空白坑。");
            }, page);

            app.Button(988, 250, 200, 38, "把 Header Bar 改成 Card", "🧱", DemoColors.Green, () =>
            {
                EmojiWindowNative.SetGroupBoxStyle(header, PageCommon.GroupBoxStyleCard);
                SetState("GroupBox -> Header Bar 已切到 Card",
                    "这也顺手验证了 GroupBox 风格可以在运行时切换。");
            }, page);

            app.Button(1202, 250, 200, 38, "恢复 Header Bar", "↺", DemoColors.Blue, () =>
            {
                EmojiWindowNative.SetGroupBoxStyle(header, PageCommon.GroupBoxStyleHeaderBar);
                EmojiWindowNative.SetGroupBoxTitleColor(header, DemoColors.Green);
                SetState("GroupBox -> Header Bar 已恢复默认",
                    "恢复 Header Bar 后，这页回到三种分组风格并列展示。");
            }, page);

            SetState("GroupBox 页已加载，可直接验证三种样式和 C# 正确宿主方式。");
        }
    }
}
