using System;

namespace EmojiWindowDemo
{
    internal static class TabControlDemoPage
    {
        public static void Build(AllDemoShell shell, IntPtr page)
        {
            var app = shell.App;
            IntPtr tab = EmojiWindowNative.CreateTabControl(page, 24, 80, 980, 520);
            EmojiWindowNative.SetTabItemSize(tab, 140, 38);
            EmojiWindowNative.SetTabPadding(tab, 18, 10);
            EmojiWindowNative.SetTabColors(tab, DemoColors.Blue, DemoColors.Surface, DemoColors.White, DemoColors.Black);
            EmojiWindowNative.SetTabIndicatorColor(tab, DemoColors.Orange);
            EmojiWindowNative.SetTabClosable(tab, 1);
            EmojiWindowNative.SetTabScrollable(tab, 1);
            EmojiWindowNative.SetTabAlignment(tab, 0);
            EmojiWindowNative.SetTabHeaderStyle(tab, 1);

            IntPtr readout = app.Label(1040, 110, 380, 96, string.Empty, DemoColors.Black, DemoColors.WindowBg, 12, PageCommon.AlignLeft, true, page);
            IntPtr state = app.Label(1040, 230, 380, 24, "TabControl 页状态将在这里更新。", DemoColors.Blue, DemoColors.WindowBg, 12, PageCommon.AlignLeft, false, page);
            int tabCounter = 0;
            bool closable = true;
            bool scrollable = true;
            int alignment = 0;

            int AddTab(string title, string body)
            {
                byte[] titleBytes = app.U(title);
                int index = EmojiWindowNative.AddTabItem(tab, titleBytes, titleBytes.Length, IntPtr.Zero);
                IntPtr content = EmojiWindowNative.GetTabContentWindow(tab, index);
                app.Label(24, 24, 480, 30, title + " 内容区", DemoColors.Black, DemoColors.Transparent, 14, PageCommon.AlignLeft, false, content);
                app.Label(24, 64, 620, 60, body, DemoColors.Gray, DemoColors.Transparent, 12, PageCommon.AlignLeft, true, content);
                tabCounter = Math.Max(tabCounter, index + 1);
                return index;
            }

            void Refresh(string note)
            {
                int count = EmojiWindowNative.GetTabCount(tab);
                int current = EmojiWindowNative.GetCurrentTabIndex(tab);
                shell.SetLabelText(readout, $"count={count}  current={current}\r\nclosable={closable}  scrollable={scrollable}  alignment={alignment}\r\n{note}");
                shell.SetLabelText(state, note);
                shell.SetStatus(note);
            }

            AddTab("首页", "这里验证 TabControl 可以正常创建内容窗口、添加标签和切换标签。");
            AddTab("设置", "这一页用于测试标签切换、样式切换和关闭按钮。");
            AddTab("收藏", "这里保留独立标签页内容，方便做多页切换回归。");

            var callback = app.Pin(new EmojiWindowNative.TabCallback((_, selectedIndex) => Refresh("Tab 切换回调: index=" + selectedIndex)));
            var closeCallback = app.Pin(new EmojiWindowNative.TabCloseCallback((_, index) => Refresh("Tab 关闭回调: index=" + index)));
            EmojiWindowNative.SetTabCallback(tab, callback);
            EmojiWindowNative.SetTabCloseCallback(tab, closeCallback);
            EmojiWindowNative.SelectTab(tab, 0);

            app.Button(1040, 300, 160, 34, "新增标签", "➕", DemoColors.Green, () =>
            {
                int index = AddTab("新标签 " + (++tabCounter), "运行时动态创建的新标签内容区。");
                EmojiWindowNative.SelectTab(tab, index);
                Refresh("已新增并切换到新标签");
            }, page);
            app.Button(1216, 300, 160, 34, "关闭当前", "🗑️", DemoColors.Red, () =>
            {
                int index = EmojiWindowNative.GetCurrentTabIndex(tab);
                if (EmojiWindowNative.GetTabCount(tab) > 1 && index >= 0)
                {
                    EmojiWindowNative.RemoveTabItem(tab, index);
                    Refresh("已关闭当前标签");
                }
                else
                {
                    Refresh("至少保留一个标签页");
                }
            }, page);
            app.Button(1040, 344, 160, 34, "切到下一页", "➡️", DemoColors.Blue, () =>
            {
                int count = EmojiWindowNative.GetTabCount(tab);
                int current = EmojiWindowNative.GetCurrentTabIndex(tab);
                if (count > 0)
                {
                    EmojiWindowNative.SelectTab(tab, (current + 1) % count);
                }
                Refresh("已切换到下一页");
            }, page);
            app.Button(1216, 344, 160, 34, "切到第 1 页", "1", DemoColors.Orange, () =>
            {
                EmojiWindowNative.SelectTab(tab, 0);
                Refresh("已切换到第 1 页");
            }, page);

            app.Button(1040, 388, 160, 34, "关闭按钮开关", "✖", DemoColors.Purple, () =>
            {
                closable = !closable;
                EmojiWindowNative.SetTabClosable(tab, closable ? 1 : 0);
                Refresh(closable ? "已开启标签关闭按钮" : "已关闭标签关闭按钮");
            }, page);
            app.Button(1216, 388, 160, 34, "滚动开关", "↔", DemoColors.Gray, () =>
            {
                scrollable = !scrollable;
                EmojiWindowNative.SetTabScrollable(tab, scrollable ? 1 : 0);
                Refresh(scrollable ? "已开启标签滚动" : "已关闭标签滚动");
            }, page);
            app.Button(1040, 432, 160, 34, "左对齐", "L", DemoColors.Blue, () =>
            {
                alignment = 0;
                EmojiWindowNative.SetTabAlignment(tab, alignment);
                Refresh("标签头已切到左对齐");
            }, page);
            app.Button(1216, 432, 160, 34, "居中对齐", "C", DemoColors.Green, () =>
            {
                alignment = 1;
                EmojiWindowNative.SetTabAlignment(tab, alignment);
                Refresh("标签头已切到居中对齐");
            }, page);

            app.Label(40, 640, 1320, 22, "1. CreateTabControl / AddTabItem / GetTabContentWindow：创建标签页并挂载独立内容区。", DemoColors.Black, DemoColors.WindowBg, 12, PageCommon.AlignLeft, false, page);
            app.Label(40, 674, 1320, 22, "2. SetTabCallback / SetTabCloseCallback / SelectTab / RemoveTabItem：切换和关闭标签并回写状态。", DemoColors.Black, DemoColors.WindowBg, 12, PageCommon.AlignLeft, false, page);
            app.Label(40, 708, 1320, 22, "3. SetTabClosable / SetTabScrollable / SetTabAlignment / SetTabHeaderStyle：切换关闭按钮、滚动、对齐和头部样式。", DemoColors.Black, DemoColors.WindowBg, 12, PageCommon.AlignLeft, false, page);

            Refresh("TabControl 页已加载，可直接测试动态增删页、切换、滚动和对齐");
        }
    }
}
