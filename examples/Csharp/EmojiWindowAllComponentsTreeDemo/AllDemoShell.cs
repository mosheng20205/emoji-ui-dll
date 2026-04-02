using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;

namespace EmojiWindowDemo
{
    internal sealed class AllDemoShell
    {
        private const int CallbackNodeSelected = 1;
        private const int WindowWidth = 1820;
        private const int WindowHeight = 980;
        private const int SidebarWidth = 292;
        private const int ContentX = 324;
        private const int ShellTop = 34;
        private const int ShellBottomGap = 72;
        private const int HeaderHeight = 88;
        private const int PageMargin = 16;

        private readonly DemoCategoryDefinition[] _categories;
        private readonly Dictionary<string, DemoPageDefinition> _pagesByKey = new Dictionary<string, DemoPageDefinition>();
        private readonly Dictionary<string, IntPtr> _pageHandles = new Dictionary<string, IntPtr>();
        private readonly Dictionary<int, string> _nodePageMap = new Dictionary<int, string>();

        private IntPtr _sidebar;
        private IntPtr _host;
        private IntPtr _navTree;
        private IntPtr _headerTitle;
        private IntPtr _headerDesc;
        private string _currentPageKey = string.Empty;

        public AllDemoShell(DemoApp app)
        {
            App = app;
            _categories = AllDemoCatalog.Create();
        }

        public DemoApp App { get; }

        private int HostWidth => Math.Max(560, App.Width - ContentX - 24);
        private int HostHeight => Math.Max(520, App.Height - ShellTop - ShellBottomGap);
        private int PageWidth => Math.Max(480, HostWidth - (PageMargin * 2));
        private int PageHeight => Math.Max(360, HostHeight - HeaderHeight - PageMargin);

        public void Build()
        {
            App.CreateWindow("EmojiWindowAllComponentsTreeDemo - C# x64", WindowWidth, WindowHeight, DemoColors.Blue, DemoColors.WindowBg);
            App.CreateStatusBar("C# 树形总演示已启动");

            _sidebar = App.Panel(16, ShellTop, SidebarWidth, HostHeight, DemoColors.Surface);
            _host = App.Panel(ContentX, ShellTop, HostWidth, HostHeight, DemoColors.WindowBg);

            App.Label(16, 14, 200, 28, "组件树导航", DemoColors.Black, DemoColors.Surface, 18, 0, false, _sidebar);
            App.Label(16, 42, 240, 20, "参考 Python 增强版总演示结构", DemoColors.Gray, DemoColors.Surface, 11, 0, false, _sidebar);
            _headerTitle = App.Label(24, 18, 720, 32, "等待选择组件", DemoColors.Black, DemoColors.WindowBg, 20, 0, false, _host);
            _headerDesc = App.Label(24, 50, 1120, 24, "从左侧树形导航中选择一个组件页面。", DemoColors.Gray, DemoColors.WindowBg, 12, 0, true, _host);

            BuildNavigationTree();
        }

        private void BuildNavigationTree()
        {
            _navTree = EmojiWindowNative.CreateTreeView(_sidebar, 16, 72, SidebarWidth - 32, HostHeight - 96, DemoColors.White, DemoColors.Black, IntPtr.Zero);
            EmojiWindowNative.SetTreeViewSidebarMode(_navTree, 1);
            EmojiWindowNative.SetTreeViewRowHeight(_navTree, 38f);
            EmojiWindowNative.SetTreeViewItemSpacing(_navTree, 6f);
            EmojiWindowNative.SetTreeViewBackgroundColor(_navTree, DemoColors.White);
            EmojiWindowNative.SetTreeViewTextColor(_navTree, DemoColors.Black);
            EmojiWindowNative.SetTreeViewSelectedBgColor(_navTree, DemoColors.Blue);
            EmojiWindowNative.SetTreeViewSelectedForeColor(_navTree, DemoColors.White);
            EmojiWindowNative.SetTreeViewHoverBgColor(_navTree, DemoColors.LightBlue);
            byte[] font = App.U("Microsoft YaHei UI");
            EmojiWindowNative.SetTreeViewFont(_navTree, font, font.Length, 13f, 500, 0);
            EmojiWindowNative.EnableTreeViewDragDrop(_navTree, 0);

            string firstPageKey = string.Empty;
            foreach (DemoCategoryDefinition category in _categories)
            {
                int rootNode = AddTreeNode(_navTree, 0, category.Title, category.Icon);
                RegisterPage(category.OverviewPage, rootNode);
                if (string.IsNullOrEmpty(firstPageKey) && category.Pages.Count > 0)
                {
                    firstPageKey = category.Pages[0].Key;
                }

                foreach (DemoPageDefinition page in category.Pages)
                {
                    int nodeId = AddTreeNode(_navTree, rootNode, page.Title, "•");
                    RegisterPage(page, nodeId);
                }
            }

            var treeCallback = App.Pin(new EmojiWindowNative.TreeNodeCallback((nodeId, _) => OnTreeNodeSelected(nodeId)));
            EmojiWindowNative.SetTreeViewCallback(_navTree, CallbackNodeSelected, treeCallback);
            EmojiWindowNative.ExpandAll(_navTree);

            if (!string.IsNullOrEmpty(firstPageKey))
            {
                ShowPage(firstPageKey);
            }
        }

        private static int AddTreeNode(IntPtr tree, int parentId, string title, string icon)
        {
            byte[] titleBytes = EmojiWindowNative.ToUtf8(title);
            byte[] iconBytes = EmojiWindowNative.ToUtf8(icon);
            return parentId == 0
                ? EmojiWindowNative.AddRootNode(tree, titleBytes, titleBytes.Length, iconBytes, iconBytes.Length)
                : EmojiWindowNative.AddChildNode(tree, parentId, titleBytes, titleBytes.Length, iconBytes, iconBytes.Length);
        }

        private void RegisterPage(DemoPageDefinition page, int nodeId)
        {
            _pagesByKey[page.Key] = page;
            _nodePageMap[nodeId] = page.Key;
        }

        private void OnTreeNodeSelected(int nodeId)
        {
            if (_nodePageMap.TryGetValue(nodeId, out string pageKey))
            {
                ShowPage(pageKey);
            }
        }

        public void ShowPage(string pageKey)
        {
            if (!_pagesByKey.TryGetValue(pageKey, out DemoPageDefinition page))
            {
                return;
            }

            if (!string.IsNullOrEmpty(_currentPageKey) && _pageHandles.TryGetValue(_currentPageKey, out IntPtr currentPage))
            {
                Win32Native.ShowWindow(currentPage, Win32Native.SW_HIDE);
            }

            if (!_pageHandles.TryGetValue(pageKey, out IntPtr pageHandle))
            {
                pageHandle = App.Panel(PageMargin, HeaderHeight, PageWidth, PageHeight, DemoColors.WindowBg, _host);
                _pageHandles[pageKey] = pageHandle;
                page.Build(this, pageHandle);
            }

            SetLabelText(_headerTitle, page.Title);
            SetLabelText(_headerDesc, page.Description);
            Win32Native.ShowWindow(pageHandle, Win32Native.SW_SHOW);
            _currentPageKey = pageKey;
            SetStatus("已切换到 " + page.Title);
        }

        public void SetStatus(string text) => App.SetStatus(text);

        public void ResizeShell(int width, int height)
        {
            App.UpdateWindowSize(width, height);

            if (_sidebar != IntPtr.Zero)
            {
                Win32Native.MoveWindow(_sidebar, 16, ShellTop, SidebarWidth, HostHeight, true);
            }

            if (_host != IntPtr.Zero)
            {
                Win32Native.MoveWindow(_host, ContentX, ShellTop, HostWidth, HostHeight, true);
            }

            if (_navTree != IntPtr.Zero)
            {
                Win32Native.MoveWindow(_navTree, 16, 72, SidebarWidth - 32, HostHeight - 96, true);
            }

            if (_headerTitle != IntPtr.Zero)
            {
                Win32Native.MoveWindow(_headerTitle, 24, 18, HostWidth - 48, 32, true);
            }

            if (_headerDesc != IntPtr.Zero)
            {
                Win32Native.MoveWindow(_headerDesc, 24, 50, HostWidth - 48, 24, true);
            }

            foreach (IntPtr pageHandle in _pageHandles.Values)
            {
                Win32Native.MoveWindow(pageHandle, PageMargin, HeaderHeight, PageWidth, PageHeight, true);
            }

            if (App.StatusLabel != IntPtr.Zero)
            {
                Win32Native.MoveWindow(App.StatusLabel, 16, App.Height - 52, App.Width - 32, 28, true);
            }
        }

        public void SetLabelText(IntPtr label, string text)
        {
            byte[] bytes = App.U(text);
            EmojiWindowNative.SetLabelText(label, bytes, bytes.Length);
        }

        public string ReadTreeNodeText(IntPtr tree, int nodeId)
        {
            int size = EmojiWindowNative.GetNodeText(tree, nodeId, IntPtr.Zero, 0);
            if (size <= 0)
            {
                return string.Empty;
            }

            IntPtr buffer = Marshal.AllocHGlobal(size);
            try
            {
                EmojiWindowNative.GetNodeText(tree, nodeId, buffer, size);
                byte[] bytes = new byte[size];
                Marshal.Copy(buffer, bytes, 0, size);
                return EmojiWindowNative.FromUtf8(bytes);
            }
            finally
            {
                Marshal.FreeHGlobal(buffer);
            }
        }
    }

    internal static class AllComponentsTreeDemoRunner
    {
        private static readonly EmojiWindowNative.ButtonClickCallback ButtonDispatcher = OnButtonClick;
        private static AllDemoShell _shell;

        public static void Run()
        {
            var app = new DemoApp();
            _shell = new AllDemoShell(app);
            EmojiWindowNative.set_button_click_callback(ButtonDispatcher);
            _shell.Build();
            EmojiWindowNative.set_message_loop_main_window(app.Window);
            EmojiWindowNative.run_message_loop();
            _shell = null;
        }

        private static void OnButtonClick(int buttonId, IntPtr parentHwnd)
        {
            _shell?.App.DispatchButton(buttonId);
        }
    }
}
