using System;
using System.Collections.Generic;

namespace EmojiWindowDemo
{
    internal static class PopupMenuDemoPage
    {
        private const int PopInfo = 5100;
        private const int PopInfoMsg = 5101;
        private const int PopInfoConfirm = 5102;
        private const int PopState = 5110;
        private const int PopStateActive = 5111;
        private const int PopStateReset = 5112;
        private const int PopBindings = 5121;
        private const int BtnMenuHit = 5201;
        private const int BtnMenuReset = 5202;

        public static void Build(AllDemoShell shell, IntPtr page)
        {
            var app = shell.App;
            IntPtr stagePanel = EmojiWindowNative.CreatePanel(page, 40, 92, 932, 382, DemoColors.Surface);
            IntPtr sceneLabel = app.Label(64, 150, 860, 34, "等待右键操作", DemoColors.Blue, DemoColors.Surface, 22, PageCommon.AlignLeft, false, page);
            IntPtr detailLabel = app.Label(64, 194, 860, 44, "在左侧区域点右键，或对右侧蓝色按钮点右键。", DemoColors.Gray, DemoColors.Surface, 13, PageCommon.AlignLeft, true, page);
            IntPtr menuZone = EmojiWindowNative.CreatePanel(stagePanel, 24, 250, 620, 96, DemoColors.White);
            app.Label(88, 344, 360, 28, "在这块白色区域点击鼠标右键", DemoColors.Black, DemoColors.Surface, 18, PageCommon.AlignLeft, false, page);

            IntPtr readout = app.Label(1040, 100, 392, 108, "等待读取 PopupMenu 状态。", DemoColors.Black, DemoColors.WindowBg, 12, PageCommon.AlignLeft, true, page);
            IntPtr callbackLabel = app.Label(1040, 244, 392, 82, "尚未触发 PopupMenu 回调。", DemoColors.Gray, DemoColors.WindowBg, 12, PageCommon.AlignLeft, true, page);
            IntPtr stateLabel = app.Label(1040, 340, 392, 24, "PopupMenu 页状态将在这里更新。", DemoColors.Blue, DemoColors.WindowBg, 12, PageCommon.AlignLeft, false, page);
            IntPtr eventFeed = app.Label(40, 604, 900, 116, "等待触发 PopupMenu 回调。", DemoColors.Black, DemoColors.WindowBg, 12, PageCommon.AlignLeft, true, page);
            app.Label(980, 604, 430, 92, "1. CreateEmojiPopupMenu / PopupMenuAddItem / PopupMenuAddSubItem：创建主菜单、按钮菜单和二级子菜单。\r\n2. BindControlMenu / BindButtonMenu：分别绑定左侧右键区和右侧按钮。\r\n3. SetPopupMenuCallback：所有菜单项都会把 menu_id / item_id 和最近动作写回页面。", DemoColors.Black, DemoColors.WindowBg, 12, PageCommon.AlignLeft, true, page);

            var events = new List<string>();
            string scene = "等待右键操作";
            string detail = "在左侧区域点右键，或对右侧蓝色按钮点右键。";
            string lastEvent = "尚未触发 PopupMenu 回调。";
            int lastMenuId = 0;
            int lastItemId = 0;

            var confirmCallback = app.Pin(new EmojiWindowNative.MessageBoxCallback(confirmed =>
                shell.SetStatus("PopupMenu ConfirmBox 返回: " + (confirmed != 0 ? "确定" : "取消"))));

            void PushEvent(string text)
            {
                events.Insert(0, text);
                if (events.Count > 6)
                {
                    events.RemoveAt(events.Count - 1);
                }

                shell.SetLabelText(eventFeed, string.Join("\r\n", events));
            }

            void Refresh(string note)
            {
                shell.SetLabelText(sceneLabel, scene);
                shell.SetLabelText(detailLabel, detail);
                shell.SetLabelText(readout, $"主菜单绑定: 左侧右键区\r\n按钮菜单绑定: 蓝色按钮\r\n最近回调: menu_id={lastMenuId} item_id={lastItemId}\r\n当前场景: {scene}");
                shell.SetLabelText(callbackLabel, lastEvent);
                shell.SetLabelText(stateLabel, note);
                shell.SetStatus(note);
            }

            void RestoreDefault(string note)
            {
                scene = "等待右键操作";
                detail = "在左侧区域点右键，或对右侧蓝色按钮点右键。";
                lastEvent = "页面状态已恢复默认。";
                PushEvent(lastEvent);
                Refresh(note);
            }

            int buttonMenuId = app.Button(1040, 414, 180, 38, "按钮右键菜单", "⋯", DemoColors.Blue, () =>
            {
                scene = "等待按钮右键";
                detail = "请直接对这个蓝色按钮点右键，打开按钮专属 PopupMenu。";
                lastEvent = "按钮提示：请对蓝色按钮点右键。";
                PushEvent(lastEvent);
                Refresh("蓝色按钮已点击，请继续点右键。");
            }, page);
            app.Button(1244, 414, 180, 38, "读取绑定", "i", DemoColors.Green, () =>
            {
                scene = "当前绑定已读取";
                detail = "主菜单绑定=左侧右键区；按钮菜单绑定=右侧蓝色按钮。";
                lastEvent = "已读取当前 PopupMenu 绑定。";
                PushEvent(lastEvent);
                Refresh("PopupMenu 已读取当前绑定");
            }, page);
            app.Button(1040, 462, 180, 38, "恢复默认", "↺", DemoColors.Purple, () => RestoreDefault("PopupMenu 已恢复默认状态"), page);
            app.Button(1244, 462, 180, 38, "操作提示", "?", DemoColors.Gray, () =>
            {
                scene = "等待右键操作";
                detail = "左侧区域和蓝色按钮都支持右键菜单。";
                lastEvent = "操作提示：左侧区域和蓝色按钮都支持右键菜单。";
                PushEvent(lastEvent);
                Refresh("请直接用右键触发 PopupMenu。");
            }, page);

            IntPtr popupMenu = EmojiWindowNative.CreateEmojiPopupMenu(page);
            AddMenuItem(app, popupMenu, "查看说明", PopInfo);
            AddSubItem(app, popupMenu, PopInfo, "打开消息框", PopInfoMsg);
            AddSubItem(app, popupMenu, PopInfo, "打开确认框", PopInfoConfirm);
            AddMenuItem(app, popupMenu, "写回状态", PopState);
            AddSubItem(app, popupMenu, PopState, "标记为已触发", PopStateActive);
            AddSubItem(app, popupMenu, PopState, "恢复等待态", PopStateReset);
            AddMenuItem(app, popupMenu, "读取当前绑定", PopBindings);

            IntPtr buttonMenu = EmojiWindowNative.CreateEmojiPopupMenu(page);
            AddMenuItem(app, buttonMenu, "按钮菜单已触发", BtnMenuHit);
            AddMenuItem(app, buttonMenu, "恢复默认状态", BtnMenuReset);

            var menuCallback = app.Pin(new EmojiWindowNative.MenuItemClickCallback((menuId, itemId) =>
            {
                lastMenuId = menuId;
                lastItemId = itemId;

                if (itemId == PopInfoMsg)
                {
                    scene = "右键菜单已打开消息框";
                    detail = "主菜单的二级子菜单和回调写回都已生效。";
                    lastEvent = "主菜单 -> 查看说明 -> 打开消息框";
                    byte[] title = app.U("PopupMenu 消息框");
                    byte[] message = app.U("这是从右键菜单打开的 MessageBox。");
                    byte[] icon = app.U("菜单");
                    EmojiWindowNative.show_message_box_bytes(page, title, title.Length, message, message.Length, icon, icon.Length);
                    PushEvent(lastEvent);
                    Refresh("PopupMenu 已触发消息框");
                }
                else if (itemId == PopInfoConfirm)
                {
                    scene = "右键菜单已打开确认框";
                    detail = "ConfirmBox 已弹出，当前菜单回调也已同步写回。";
                    lastEvent = "主菜单 -> 查看说明 -> 打开确认框";
                    byte[] title = app.U("PopupMenu 确认框");
                    byte[] message = app.U("这是从右键菜单打开的 ConfirmBox。");
                    byte[] icon = app.U("菜单");
                    EmojiWindowNative.show_confirm_box_bytes(page, title, title.Length, message, message.Length, icon, icon.Length, confirmCallback);
                    PushEvent(lastEvent);
                    Refresh("PopupMenu 已触发确认框");
                }
                else if (itemId == PopStateActive)
                {
                    scene = "状态已标记为已触发";
                    detail = "这是通过右键菜单直接写回到页面的状态文本。";
                    lastEvent = "主菜单 -> 写回状态 -> 标记为已触发";
                    PushEvent(lastEvent);
                    Refresh("PopupMenu 已写回“已触发”状态");
                }
                else if (itemId == PopStateReset)
                {
                    scene = "等待右键操作";
                    detail = "页面状态已通过右键菜单恢复为初始等待态。";
                    lastEvent = "主菜单 -> 写回状态 -> 恢复等待态";
                    PushEvent(lastEvent);
                    Refresh("PopupMenu 已恢复等待态");
                }
                else if (itemId == PopBindings)
                {
                    scene = "当前绑定已读取";
                    detail = "主菜单绑定=左侧右键区；按钮菜单绑定=右侧蓝色按钮。";
                    lastEvent = "主菜单 -> 读取当前绑定";
                    PushEvent(lastEvent);
                    Refresh("PopupMenu 已读取当前绑定");
                }
                else if (itemId == BtnMenuHit)
                {
                    scene = "按钮菜单已触发";
                    detail = "当前命中的是蓝色按钮专属 PopupMenu。";
                    lastEvent = "按钮菜单 -> 按钮菜单已触发";
                    PushEvent(lastEvent);
                    Refresh("按钮右键菜单回调正常");
                }
                else if (itemId == BtnMenuReset)
                {
                    RestoreDefault("按钮菜单已恢复默认状态");
                    lastEvent = "按钮菜单 -> 恢复默认状态";
                    PushEvent(lastEvent);
                    Refresh("按钮菜单已恢复默认状态");
                }
                else
                {
                    scene = "收到 PopupMenu 回调";
                    detail = $"menu_id={menuId}，item_id={itemId} 已写回页面。";
                    lastEvent = $"PopupMenu 收到 item_id={itemId}";
                    PushEvent(lastEvent);
                    Refresh("PopupMenu 已收到回调");
                }
            }));

            EmojiWindowNative.SetPopupMenuCallback(popupMenu, menuCallback);
            EmojiWindowNative.SetPopupMenuCallback(buttonMenu, menuCallback);
            EmojiWindowNative.BindControlMenu(menuZone, popupMenu);
            EmojiWindowNative.BindButtonMenu(page, buttonMenuId, buttonMenu);

            RestoreDefault("PopupMenu 页面已重排，只保留控件右键菜单和按钮菜单两类绑定。");
        }

        private static void AddMenuItem(DemoApp app, IntPtr menu, string text, int itemId)
        {
            byte[] bytes = app.U(text);
            EmojiWindowNative.PopupMenuAddItem(menu, bytes, bytes.Length, itemId);
        }

        private static void AddSubItem(DemoApp app, IntPtr menu, int parentId, string text, int itemId)
        {
            byte[] bytes = app.U(text);
            EmojiWindowNative.PopupMenuAddSubItem(menu, parentId, bytes, bytes.Length, itemId);
        }
    }
}
