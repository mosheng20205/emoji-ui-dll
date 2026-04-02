using System;

namespace EmojiWindowDemo
{
    internal static class TreeViewDemoPage
    {
        private const int CallbackNodeSelected = 1;

        public static void Build(AllDemoShell shell, IntPtr page)
        {
            var app = shell.App;
            byte[] font = app.U("Microsoft YaHei UI");

            IntPtr intro = app.GroupBox(16, 16, 420, 136, "🌲 TreeView 页面说明", DemoColors.Border, DemoColors.WindowBg, page);
            IntPtr stageBox = app.GroupBox(460, 16, 1004, 136, "📘 当前页能力", DemoColors.Border, DemoColors.WindowBg, page);
            IntPtr content = EmojiWindowNative.CreatePanel(page, 16, 170, 1448, 650, DemoColors.Surface);

            IntPtr introText = app.Label(34, 58, 382, 56, "TreeView 页不再只放一个孤立树控件，而是带上状态读取、节点操作、样式切换和回调写回。", DemoColors.Gray, DemoColors.WindowBg, 12, PageCommon.AlignLeft, true, page);
            IntPtr stageText = app.Label(484, 58, 940, 42, "左侧是树舞台，右侧是状态、最近回调和快捷动作。可直接测试展开折叠、侧边栏模式、多级节点和节点文本更新。", DemoColors.Gray, DemoColors.WindowBg, 12, PageCommon.AlignLeft, true, page);
            app.AttachToGroup(intro, introText);
            app.AttachToGroup(stageBox, stageText);

            IntPtr treePanel = EmojiWindowNative.CreatePanel(content, 24, 24, 560, 586, DemoColors.White);
            IntPtr sidePanel = EmojiWindowNative.CreatePanel(content, 608, 24, 816, 586, DemoColors.White);
            IntPtr tree = EmojiWindowNative.CreateTreeView(treePanel, 18, 56, 524, 512, DemoColors.White, DemoColors.Black, IntPtr.Zero);

            EmojiWindowNative.SetTreeViewSidebarMode(tree, 1);
            EmojiWindowNative.SetTreeViewRowHeight(tree, 32f);
            EmojiWindowNative.SetTreeViewItemSpacing(tree, 6f);
            EmojiWindowNative.SetTreeViewBackgroundColor(tree, DemoColors.White);
            EmojiWindowNative.SetTreeViewTextColor(tree, DemoColors.Black);
            EmojiWindowNative.SetTreeViewSelectedBgColor(tree, DemoColors.Blue);
            EmojiWindowNative.SetTreeViewSelectedForeColor(tree, DemoColors.White);
            EmojiWindowNative.SetTreeViewHoverBgColor(tree, DemoColors.LightBlue);
            EmojiWindowNative.SetTreeViewFont(tree, font, font.Length, 13f, 500, 0);
            EmojiWindowNative.EnableTreeViewDragDrop(tree, 0);

            int rootWorkspace = EmojiWindowNative.AddRootNode(tree, app.U("工作区"), app.U("工作区").Length, app.U("🧭"), app.U("🧭").Length);
            int rootBookmarks = EmojiWindowNative.AddRootNode(tree, app.U("书签"), app.U("书签").Length, app.U("⭐"), app.U("⭐").Length);
            int rootSystem = EmojiWindowNative.AddRootNode(tree, app.U("系统"), app.U("系统").Length, app.U("⚙️"), app.U("⚙️").Length);

            int nodeBrowser = EmojiWindowNative.AddChildNode(tree, rootWorkspace, app.U("多标签浏览器"), app.U("多标签浏览器").Length, app.U("🌐"), app.U("🌐").Length);
            int nodeControls = EmojiWindowNative.AddChildNode(tree, rootWorkspace, app.U("控件示例"), app.U("控件示例").Length, app.U("🧩"), app.U("🧩").Length);
            int nodeTasks = EmojiWindowNative.AddChildNode(tree, rootWorkspace, app.U("测试任务"), app.U("测试任务").Length, app.U("🧪"), app.U("🧪").Length);
            int nodeOpenAi = EmojiWindowNative.AddChildNode(tree, rootBookmarks, app.U("OpenAI"), app.U("OpenAI").Length, app.U("🤖"), app.U("🤖").Length);
            int nodeDocs = EmojiWindowNative.AddChildNode(tree, rootBookmarks, app.U("接口文档"), app.U("接口文档").Length, app.U("📘"), app.U("📘").Length);
            int nodeTheme = EmojiWindowNative.AddChildNode(tree, rootSystem, app.U("主题设置"), app.U("主题设置").Length, app.U("🎨"), app.U("🎨").Length);
            int nodeRender = EmojiWindowNative.AddChildNode(tree, rootSystem, app.U("渲染日志"), app.U("渲染日志").Length, app.U("📰"), app.U("📰").Length);
            EmojiWindowNative.AddChildNode(tree, nodeControls, app.U("Button / Label / EditBox"), app.U("Button / Label / EditBox").Length, app.U("📦"), app.U("📦").Length);
            EmojiWindowNative.AddChildNode(tree, nodeControls, app.U("DataGrid / ListBox / ComboBox"), app.U("DataGrid / ListBox / ComboBox").Length, app.U("📊"), app.U("📊").Length);
            EmojiWindowNative.AddChildNode(tree, nodeTasks, app.U("验证冻结首列 / 首行"), app.U("验证冻结首列 / 首行").Length, app.U("✅"), app.U("✅").Length);
            EmojiWindowNative.SetNodeChecked(tree, nodeControls, 1);
            EmojiWindowNative.SetNodeChecked(tree, nodeTasks, 1);
            EmojiWindowNative.ExpandAll(tree);
            EmojiWindowNative.SetSelectedNode(tree, nodeControls);

            IntPtr selectedLabel = app.Label(18, 16, 500, 24, "🌲 Tree 舞台", DemoColors.Black, DemoColors.White, 14, PageCommon.AlignLeft, false, treePanel);
            IntPtr statusLabel = app.Label(18, 16, 760, 24, "等待 TreeView 动作。", DemoColors.Blue, DemoColors.White, 13, PageCommon.AlignLeft, false, sidePanel);
            IntPtr readoutLabel = app.Label(18, 56, 760, 120, "等待读取 TreeView 状态。", DemoColors.Black, DemoColors.White, 12, PageCommon.AlignLeft, true, sidePanel);
            IntPtr callbackLabel = app.Label(18, 202, 760, 84, "等待触发 TreeView 回调。", DemoColors.Gray, DemoColors.White, 12, PageCommon.AlignLeft, true, sidePanel);
            IntPtr detailLabel = app.Label(18, 520, 760, 44, "TreeView 这页重点是验证多级节点、回调、运行时改文本以及侧边栏样式是否一起正常。", DemoColors.Gray, DemoColors.White, 12, PageCommon.AlignLeft, true, sidePanel);

            bool sidebarMode = true;
            float rowHeight = 32f;
            float spacing = 6f;
            string lastEvent = "尚未触发节点回调。";

            void Refresh(string note)
            {
                int current = EmojiWindowNative.GetSelectedNode(tree);
                string currentText = current > 0 ? shell.ReadTreeNodeText(tree, current) : "(无选中)";
                shell.SetLabelText(readoutLabel,
                    $"当前节点={currentText}\r\n" +
                    $"sidebar_mode={(sidebarMode ? "ON" : "OFF")}  row_height={rowHeight:0}  spacing={spacing:0}\r\n" +
                    $"checked_node=控件示例 / 测试任务  右侧按钮会直接改树状态");
                shell.SetLabelText(callbackLabel, lastEvent);
                shell.SetLabelText(statusLabel, note);
                shell.SetStatus(note);
            }

            var callback = app.Pin(new EmojiWindowNative.TreeNodeCallback((nodeId, _) =>
            {
                string text = shell.ReadTreeNodeText(tree, nodeId);
                lastEvent = "节点回调 -> " + text;
                Refresh("TreeView -> 已选中 " + text);
            }));
            EmojiWindowNative.SetTreeViewCallback(tree, CallbackNodeSelected, callback);

            app.Button(18, 314, 150, 38, "全部展开", "📂", DemoColors.Blue, () =>
            {
                EmojiWindowNative.ExpandAll(tree);
                Refresh("TreeView -> 已展开全部节点");
            }, sidePanel);
            app.Button(184, 314, 150, 38, "全部折叠", "📁", DemoColors.Orange, () =>
            {
                EmojiWindowNative.CollapseAll(tree);
                Refresh("TreeView -> 已折叠全部节点");
            }, sidePanel);
            app.Button(350, 314, 182, 38, "选中控件示例", "🎯", DemoColors.Green, () =>
            {
                EmojiWindowNative.SetSelectedNode(tree, nodeControls);
                EmojiWindowNative.ScrollToNode(tree, nodeControls);
                lastEvent = "按钮动作 -> 已跳转到 控件示例";
                Refresh("TreeView -> 已选中“控件示例”");
            }, sidePanel);
            app.Button(548, 314, 214, 38, "改浏览器节点文本", "✏️", DemoColors.Red, () =>
            {
                byte[] text = app.U("多标签浏览器 Demo");
                EmojiWindowNative.SetNodeText(tree, nodeBrowser, text, text.Length);
                lastEvent = "按钮动作 -> 浏览器节点已重命名";
                Refresh("TreeView -> 树节点文本已修改");
            }, sidePanel);

            app.Button(18, 368, 150, 38, "跳到 OpenAI", "🤖", DemoColors.Purple, () =>
            {
                EmojiWindowNative.SetSelectedNode(tree, nodeOpenAi);
                EmojiWindowNative.ScrollToNode(tree, nodeOpenAi);
                lastEvent = "按钮动作 -> 已跳转到 OpenAI";
                Refresh("TreeView -> 已定位到 OpenAI 节点");
            }, sidePanel);
            app.Button(184, 368, 150, 38, "切换 Sidebar", "🧭", DemoColors.Gray, () =>
            {
                sidebarMode = !sidebarMode;
                EmojiWindowNative.SetTreeViewSidebarMode(tree, sidebarMode ? 1 : 0);
                Refresh("TreeView -> Sidebar 模式已切换");
            }, sidePanel);
            app.Button(350, 368, 182, 38, "加高行高", "📏", DemoColors.Green, () =>
            {
                rowHeight = rowHeight >= 40f ? 32f : 40f;
                EmojiWindowNative.SetTreeViewRowHeight(tree, rowHeight);
                Refresh("TreeView -> 行高已切换");
            }, sidePanel);
            app.Button(548, 368, 214, 38, "切换间距", "↕️", DemoColors.Blue, () =>
            {
                spacing = spacing >= 10f ? 6f : 10f;
                EmojiWindowNative.SetTreeViewItemSpacing(tree, spacing);
                Refresh("TreeView -> 节点间距已切换");
            }, sidePanel);

            app.Button(18, 422, 150, 38, "浅蓝悬停", "🎨", DemoColors.Blue, () =>
            {
                EmojiWindowNative.SetTreeViewHoverBgColor(tree, DemoColors.LightBlue);
                Refresh("TreeView -> Hover 背景已切到浅蓝");
            }, sidePanel);
            app.Button(184, 422, 150, 38, "暖色悬停", "🧡", DemoColors.Orange, () =>
            {
                EmojiWindowNative.SetTreeViewHoverBgColor(tree, DemoColors.Yellow);
                Refresh("TreeView -> Hover 背景已切到暖色");
            }, sidePanel);
            app.Button(350, 422, 182, 38, "白字蓝底选中", "✅", DemoColors.Green, () =>
            {
                EmojiWindowNative.SetTreeViewSelectedBgColor(tree, DemoColors.Blue);
                EmojiWindowNative.SetTreeViewSelectedForeColor(tree, DemoColors.White);
                Refresh("TreeView -> 选中颜色已恢复默认");
            }, sidePanel);
            app.Button(548, 422, 214, 38, "系统节点改名", "⚙️", DemoColors.Purple, () =>
            {
                byte[] text = app.U("系统 / 偏好设置");
                EmojiWindowNative.SetNodeText(tree, rootSystem, text, text.Length);
                lastEvent = "按钮动作 -> 系统根节点已改名";
                Refresh("TreeView -> 根节点文本已修改");
            }, sidePanel);

            Refresh("🌲 TreeView 页已加载，可直接测试展开/折叠/侧边栏与节点回调。");
        }
    }
}
