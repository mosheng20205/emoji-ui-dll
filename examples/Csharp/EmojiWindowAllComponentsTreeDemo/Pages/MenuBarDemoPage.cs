using System;

namespace EmojiWindowDemo
{
    internal static class MenuBarDemoPage
    {
        private const int MenuFile = 5000;
        private const int MenuDialog = 5100;
        private const int MenuMessage = 5110;
        private const int MenuConfirm = 5111;
        private const int MenuScene = 5200;
        private const int MenuSceneRunning = 5210;
        private const int MenuSceneDone = 5211;
        private const int MenuLayout = 6000;
        private const int MenuLayoutLeft = 6110;
        private const int MenuLayoutStd = 6111;
        private const int MenuLayoutWide = 6112;
        private const int MenuLayoutDown = 6113;
        private const int MenuLayoutReset = 6114;
        private const int MenuTools = 7000;
        private const int MenuToolRename = 7100;
        private const int MenuToolRestore = 7101;
        private const int MenuToolToggle = 7102;
        private const int MenuToolRefresh = 7103;

        public static void Build(AllDemoShell shell, IntPtr page)
        {
            var app = shell.App;
            IntPtr stagePanel = EmojiWindowNative.CreatePanel(page, 40, 92, 932, 382, DemoColors.Surface);
            IntPtr sceneLabel = app.Label(64, 126, 860, 34, "等待菜单操作", DemoColors.Blue, DemoColors.Surface, 22, PageCommon.AlignLeft, false, page);
            IntPtr detailLabel = app.Label(64, 170, 860, 44, "直接点击上方菜单，验证三级子菜单、布局调整、文案更新和回调写回。", DemoColors.Gray, DemoColors.Surface, 13, PageCommon.AlignLeft, true, page);
            IntPtr readout = app.Label(1040, 100, 392, 110, "等待读取 MenuBar 状态。", DemoColors.Black, DemoColors.WindowBg, 12, PageCommon.AlignLeft, true, page);
            IntPtr callbackLabel = app.Label(1040, 254, 392, 92, "等待触发 MenuBar 回调。", DemoColors.Gray, DemoColors.WindowBg, 12, PageCommon.AlignLeft, true, page);
            IntPtr stateLabel = app.Label(1040, 360, 392, 24, "MenuBar 页状态将在这里更新。", DemoColors.Blue, DemoColors.WindowBg, 12, PageCommon.AlignLeft, false, page);
            IntPtr apiLabel = app.Label(40, 594, 1380, 120, "1. CreateMenuBar / DestroyMenuBar: 创建并重建菜单栏。\r\n2. MenuBarAddItem / MenuBarAddSubItem: 组合一级、二级、三级菜单。\r\n3. SetMenuBarPlacement / MenuBarUpdateSubItemText / SetMenuBarCallback: 演示布局调整、文案更新和回调写回。", DemoColors.Black, DemoColors.WindowBg, 12, PageCommon.AlignLeft, true, page);

            IntPtr bar = IntPtr.Zero;
            int x = 24;
            int y = 24;
            int width = 760;
            int height = 34;
            string mode = "default";
            string messageText = "显示消息框";
            string scene = "等待菜单操作";
            string lastEvent = "尚未触发菜单回调。";
            int lastMenuId = 0;
            int lastItemId = 0;
            EmojiWindowNative.MenuItemClickCallback menuCallback = null;

            var confirmCallback = app.Pin(new EmojiWindowNative.MessageBoxCallback(confirmed =>
                shell.SetStatus("MenuBar ConfirmBox 返回: " + (confirmed != 0 ? "确定" : "取消"))));

            void Refresh(string note)
            {
                shell.SetLabelText(sceneLabel, scene);
                shell.SetLabelText(detailLabel, $"菜单方案={(mode == "default" ? "默认三级菜单" : "紧凑菜单")}  位置=({x}, {y})  尺寸={width} x {height}\r\n最近动作: {lastEvent}");
                shell.SetLabelText(readout, $"方案={(mode == "default" ? "默认三级菜单" : "紧凑菜单")}\r\n位置=({x}, {y})  尺寸={width} x {height}\r\n消息子项={messageText}\r\n最近回调: menu_id={lastMenuId} item_id={lastItemId}");
                shell.SetLabelText(callbackLabel, lastEvent);
                shell.SetLabelText(stateLabel, note);
                shell.SetStatus(note);
            }

            void AddItem(IntPtr menuBar, string text, int itemId)
            {
                byte[] bytes = app.U(text);
                EmojiWindowNative.MenuBarAddItem(menuBar, bytes, bytes.Length, itemId);
            }

            void AddSub(IntPtr menuBar, int parentId, string text, int itemId)
            {
                byte[] bytes = app.U(text);
                EmojiWindowNative.MenuBarAddSubItem(menuBar, parentId, bytes, bytes.Length, itemId);
            }

            void ApplyPlacement(int newX, int newY, int newWidth, int newHeight, string note)
            {
                x = newX;
                y = newY;
                width = newWidth;
                height = newHeight;
                if (bar != IntPtr.Zero)
                {
                    EmojiWindowNative.SetMenuBarPlacement(bar, x, y, width, height);
                }

                Refresh(note);
            }

            void UpdateMessageText(string text, string note)
            {
                if (bar == IntPtr.Zero)
                {
                    Refresh("MenuBar 尚未创建");
                    return;
                }

                byte[] bytes = app.U(text);
                if (EmojiWindowNative.MenuBarUpdateSubItemText(bar, MenuDialog, MenuMessage, bytes, bytes.Length) != 0)
                {
                    messageText = text;
                    lastEvent = "MenuBar 子项文案已更新";
                }
                else
                {
                    lastEvent = "MenuBar 子项文案更新失败";
                }

                Refresh(note);
            }

            void RebuildMenu(string nextMode, string note)
            {
                if (bar != IntPtr.Zero)
                {
                    EmojiWindowNative.DestroyMenuBar(bar);
                }

                mode = nextMode;
                bar = EmojiWindowNative.CreateMenuBar(stagePanel);
                EmojiWindowNative.SetMenuBarPlacement(bar, x, y, width, height);

                if (mode == "default")
                {
                    AddItem(bar, "📁 文件", MenuFile);
                    AddSub(bar, MenuFile, "💬 对话框", MenuDialog);
                    AddSub(bar, MenuDialog, messageText, MenuMessage);
                    AddSub(bar, MenuDialog, "❓ 显示确认框", MenuConfirm);
                    AddSub(bar, MenuFile, "🧭 场景文案", MenuScene);
                    AddSub(bar, MenuScene, "🟡 运行中", MenuSceneRunning);
                    AddSub(bar, MenuScene, "🟢 已完成", MenuSceneDone);
                    AddItem(bar, "📐 布局", MenuLayout);
                }
                else
                {
                    AddItem(bar, "⚡ 快捷操作", MenuFile);
                    AddSub(bar, MenuFile, "💬 对话框", MenuDialog);
                    AddSub(bar, MenuDialog, messageText, MenuMessage);
                    AddSub(bar, MenuDialog, "❓ 显示确认框", MenuConfirm);
                    AddItem(bar, "📐 布局", MenuLayout);
                }

                AddSub(bar, MenuLayout, "⬅ 靠左 360", MenuLayoutLeft);
                AddSub(bar, MenuLayout, "↔ 标准 560", MenuLayoutStd);
                AddSub(bar, MenuLayout, "⟷ 满宽 900", MenuLayoutWide);
                AddSub(bar, MenuLayout, "⬇ 下移 28", MenuLayoutDown);
                AddSub(bar, MenuLayout, "⬆ 顶部归位", MenuLayoutReset);
                AddItem(bar, "🛠️ 工具", MenuTools);
                AddSub(bar, MenuTools, "✏️ 更新子项文案", MenuToolRename);
                AddSub(bar, MenuTools, "↺ 恢复子项文案", MenuToolRestore);
                AddSub(bar, MenuTools, "🔁 切换菜单方案", MenuToolToggle);
                AddSub(bar, MenuTools, "📡 刷新状态", MenuToolRefresh);

                EmojiWindowNative.SetMenuBarCallback(bar, menuCallback);
                lastEvent = $"已重建 {(mode == "default" ? "默认三级菜单" : "紧凑菜单")}。";
                Refresh(note);
            }

            void HandleMenu(int menuId, int itemId)
            {
                lastMenuId = menuId;
                lastItemId = itemId;

                if (itemId == MenuMessage)
                {
                    scene = "消息框已通过 MenuBar 打开。";
                    lastEvent = $"触发菜单项 -> {messageText}";
                    byte[] title = app.U("MenuBar 消息框");
                    byte[] message = app.U("这是从 MenuBar 子菜单打开的 MessageBox。");
                    byte[] icon = app.U("📁");
                    EmojiWindowNative.show_message_box_bytes(page, title, title.Length, message, message.Length, icon, icon.Length);
                    Refresh("MenuBar 已触发消息框");
                }
                else if (itemId == MenuConfirm)
                {
                    scene = "确认框已通过 MenuBar 打开。";
                    lastEvent = "触发菜单项 -> 显示确认框";
                    byte[] title = app.U("MenuBar 确认框");
                    byte[] message = app.U("这是从 MenuBar 子菜单打开的 ConfirmBox。");
                    byte[] icon = app.U("📁");
                    EmojiWindowNative.show_confirm_box_bytes(page, title, title.Length, message, message.Length, icon, icon.Length, confirmCallback);
                    Refresh("MenuBar 已触发确认框");
                }
                else if (itemId == MenuSceneRunning)
                {
                    scene = "运行中 - 菜单栏已写回场景文案。";
                    lastEvent = "触发菜单项 -> 运行中";
                    Refresh("MenuBar 已切换场景为运行中");
                }
                else if (itemId == MenuSceneDone)
                {
                    scene = "已完成 - 菜单栏已写回场景文案。";
                    lastEvent = "触发菜单项 -> 已完成";
                    Refresh("MenuBar 已切换场景为已完成");
                }
                else if (itemId == MenuLayoutLeft)
                {
                    lastEvent = "触发菜单项 -> 靠左 360";
                    ApplyPlacement(24, 24, 360, 34, "MenuBar 已切到靠左 360");
                }
                else if (itemId == MenuLayoutStd)
                {
                    lastEvent = "触发菜单项 -> 标准 560";
                    ApplyPlacement(24, 24, 560, 34, "MenuBar 已切到标准宽度 560");
                }
                else if (itemId == MenuLayoutWide)
                {
                    lastEvent = "触发菜单项 -> 满宽 900";
                    ApplyPlacement(24, 24, 900, 34, "MenuBar 已切到满宽 900");
                }
                else if (itemId == MenuLayoutDown)
                {
                    lastEvent = "触发菜单项 -> 下移 28";
                    ApplyPlacement(x, 52, width, height, "MenuBar 已下移 28 像素");
                }
                else if (itemId == MenuLayoutReset)
                {
                    lastEvent = "触发菜单项 -> 顶部归位";
                    ApplyPlacement(24, 24, mode == "default" ? 760 : 620, 34, "MenuBar 已恢复默认位置");
                }
                else if (itemId == MenuToolRename)
                {
                    UpdateMessageText("打开消息框", "MenuBar 子项文案已改为“打开消息框”");
                }
                else if (itemId == MenuToolRestore)
                {
                    UpdateMessageText("显示消息框", "MenuBar 子项文案已恢复");
                }
                else if (itemId == MenuToolToggle)
                {
                    RebuildMenu(mode == "default" ? "compact" : "default", "MenuBar 已切换菜单方案");
                }
                else if (itemId == MenuToolRefresh)
                {
                    lastEvent = "已刷新 MenuBar 状态";
                    Refresh("MenuBar 状态已刷新");
                }
                else
                {
                    lastEvent = $"MenuBar 收到 item_id={itemId}";
                    Refresh("MenuBar 已收到菜单回调");
                }
            }

            menuCallback = app.Pin(new EmojiWindowNative.MenuItemClickCallback(HandleMenu));

            app.Button(1040, 410, 120, 34, "靠左 360", "<", DemoColors.Blue, () => ApplyPlacement(24, 24, 360, 34, "MenuBar 已切到靠左 360"), page);
            app.Button(1172, 410, 120, 34, "标准 560", "=", DemoColors.Green, () => ApplyPlacement(24, 24, 560, 34, "MenuBar 已切到标准宽度 560"), page);
            app.Button(1304, 410, 120, 34, "满宽 900", "[]", DemoColors.Orange, () => ApplyPlacement(24, 24, 900, 34, "MenuBar 已切到满宽 900"), page);
            app.Button(1040, 454, 120, 34, "下移 28", "v", DemoColors.Purple, () => ApplyPlacement(x, 52, width, height, "MenuBar 已下移 28 像素"), page);
            app.Button(1172, 454, 120, 34, "恢复位置", "R", DemoColors.Gray, () => ApplyPlacement(24, 24, mode == "default" ? 760 : 620, 34, "MenuBar 已恢复默认位置"), page);
            app.Button(1304, 454, 120, 34, "改子项文案", "T", DemoColors.Blue, () => UpdateMessageText("打开消息框", "MenuBar 子项文案已改为“打开消息框”"), page);
            app.Button(1040, 498, 120, 34, "恢复文案", "<-", DemoColors.Green, () => UpdateMessageText("显示消息框", "MenuBar 子项文案已恢复"), page);
            app.Button(1172, 498, 120, 34, "默认菜单", "A", DemoColors.Orange, () => RebuildMenu("default", "MenuBar 已重建为默认三级菜单"), page);
            app.Button(1304, 498, 120, 34, "紧凑菜单", "B", DemoColors.Purple, () => RebuildMenu("compact", "MenuBar 已重建为紧凑菜单"), page);

            RebuildMenu("default", "MenuBar 页面已重排，只保留菜单栏本身和相关操作。");
            _ = apiLabel;
        }
    }
}
