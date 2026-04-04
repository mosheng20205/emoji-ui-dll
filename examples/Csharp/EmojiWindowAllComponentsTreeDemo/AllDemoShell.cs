using System;
using System.Collections.Generic;
using System.IO;
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
        private const int ShellBottomGap = 96;
        private const int HeaderHeight = 88;
        private const int PageMargin = 16;
        private const int MenuY = ShellTop;
        private const int MenuHeight = 34;
        private const int SidebarTop = ShellTop + MenuHeight + 12;
        private const int SidebarTreeTop = 116;

        private const int MenuComponentNavRoot = 1100;
        private const int MenuExpandAll = 1101;
        private const int MenuCollapseAll = 1102;
        private const int MenuLocateCurrent = 1103;

        private const int MenuViewLocateCurrent = 2101;
        private const int MenuViewExpandAll = 2102;
        private const int MenuViewCollapseAll = 2103;
        private const int MenuViewCurrentOverview = 2104;

        private const int MenuBuildRefreshCurrent = 3101;
        private const int MenuBuildRelayout = 3102;
        private const int MenuBuildReapplyTheme = 3103;

        private const int MenuDebugCurrentKey = 4101;
        private const int MenuDebugSyncTree = 4102;
        private const int MenuDebugHeaderInfo = 4103;

        private const int MenuToolsThemeLight = 5101;
        private const int MenuToolsThemeDark = 5102;
        private const int MenuToolsThemeRefresh = 5103;

        private const int MenuHelpGuide = 6101;
        private const int MenuHelpCurrentPage = 6102;
        private const int MenuHelpAbout = 6103;

        private readonly DemoCategoryDefinition[] _categories;
        private readonly Dictionary<string, DemoPageDefinition> _pagesByKey = new Dictionary<string, DemoPageDefinition>();
        private readonly Dictionary<string, IntPtr> _pageHandles = new Dictionary<string, IntPtr>();
        private readonly Dictionary<int, string> _nodePageMap = new Dictionary<int, string>();
        private readonly Dictionary<string, int> _pageNodeMap = new Dictionary<string, int>();
        private readonly Dictionary<int, string> _menuPageMap = new Dictionary<int, string>();
        private readonly Dictionary<string, string> _pageOverviewMap = new Dictionary<string, string>();
        private readonly Dictionary<IntPtr, Action> _pageThemeHandlers = new Dictionary<IntPtr, Action>();
        private readonly List<int> _topMenuButtons = new List<int>();

        private IntPtr _sidebar;
        private IntPtr _host;
        private IntPtr _navTree;
        private IntPtr _sidebarTitle;
        private IntPtr _sidebarDesc;
        private IntPtr _headerTitle;
        private IntPtr _headerDesc;
        private string _currentPageKey = string.Empty;
        private int _suppressNavNodeOnce;
        private bool? _lastAppliedDarkMode;

        public AllDemoShell(DemoApp app)
        {
            App = app;
            _categories = AllDemoCatalog.Create();
        }

        public DemoApp App { get; }
        public DemoThemePalette Palette => App.GetThemePalette();
        private uint MenuBackground => Palette.PageBackground;

        private int HostWidth => Math.Max(560, App.Width - ContentX - 24);
        private int HostHeight => Math.Max(520, App.Height - ShellTop - ShellBottomGap);
        private int SidebarHeight => Math.Max(480, App.Height - SidebarTop - ShellBottomGap);
        private int PageWidth => Math.Max(480, HostWidth - (PageMargin * 2));
        private int PageHeight => Math.Max(360, HostHeight - HeaderHeight - PageMargin);

        public void Build()
        {
            DemoThemePalette palette = Palette;
            App.CreateWindow("EmojiWindowAllComponentsTreeDemo - C# x64", WindowWidth, WindowHeight, DemoColors.Blue, DemoColors.WindowBg);

            _sidebar = App.Panel(16, SidebarTop, SidebarWidth, SidebarHeight, palette.SidebarBackground);
            _host = App.Panel(ContentX, ShellTop, HostWidth, HostHeight, palette.PageBackground);

            _sidebarTitle = App.Label(16, 14, 200, 28, "组件树导航", palette.SidebarText, palette.SidebarBackground, 18, 0, false, _sidebar);
            _sidebarDesc = App.Label(16, 42, 240, 20, "参考 Python 增强版布局", palette.SidebarMuted, palette.SidebarBackground, 11, 0, false, _sidebar);
            _headerTitle = App.Label(24, 18, 720, 32, "等待选择组件", palette.Text, palette.PageBackground, 20, 0, false, _host);
            _headerDesc = App.Label(24, 50, 1120, 24, "从左侧树形导航中选择一个组件页。", palette.Muted, palette.PageBackground, 12, 0, true, _host);

            BuildNavigationTree();
            BuildTopMenus();
            App.CreateStatusBar("C# 树形总演示已启动");
            BringStatusToFront();

            var themeChanged = App.Pin(new EmojiWindowNative.ThemeChangedCallback(_ => ApplyTheme()));
            EmojiWindowNative.SetThemeChangedCallback(themeChanged);
            ApplyTheme(true);
        }

        private void BuildNavigationTree()
        {
            DemoThemePalette palette = Palette;
            _navTree = EmojiWindowNative.CreateTreeView(_sidebar, 16, SidebarTreeTop, SidebarWidth - 32, SidebarHeight - SidebarTreeTop - 16, palette.TreeBackground, palette.SidebarText, IntPtr.Zero);
            EmojiWindowNative.SetTreeViewSidebarMode(_navTree, 1);
            EmojiWindowNative.SetTreeViewRowHeight(_navTree, 38f);
            EmojiWindowNative.SetTreeViewItemSpacing(_navTree, 6f);
            EmojiWindowNative.SetTreeViewBackgroundColor(_navTree, palette.TreeBackground);
            EmojiWindowNative.SetTreeViewTextColor(_navTree, palette.SidebarText);
            EmojiWindowNative.SetTreeViewSelectedBgColor(_navTree, DemoColors.Blue);
            EmojiWindowNative.SetTreeViewSelectedForeColor(_navTree, DemoColors.White);
            EmojiWindowNative.SetTreeViewHoverBgColor(_navTree, palette.TreeHoverBackground);
            byte[] font = App.U("Microsoft YaHei UI");
            EmojiWindowNative.SetTreeViewFont(_navTree, font, font.Length, 13f, 500, 0);
            EmojiWindowNative.EnableTreeViewDragDrop(_navTree, 0);

            string firstPageKey = string.Empty;
            foreach (DemoCategoryDefinition category in _categories)
            {
                int rootNode = AddTreeNode(_navTree, 0, category.Title, category.Icon);
                RegisterPage(category.OverviewPage, rootNode);
                _pageOverviewMap[category.OverviewPage.Key] = category.OverviewPage.Key;
                if (string.IsNullOrEmpty(firstPageKey) && category.Pages.Count > 0)
                {
                    firstPageKey = category.Pages[0].Key;
                }

                foreach (DemoPageDefinition page in category.Pages)
                {
                    int nodeId = AddTreeNode(_navTree, rootNode, page.Title, "•");
                    RegisterPage(page, nodeId);
                    _pageOverviewMap[page.Key] = category.OverviewPage.Key;
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

        private void BuildTopMenus()
        {
            var callback = App.Pin(new EmojiWindowNative.MenuItemClickCallback((_, itemId) => OnMenuAction(itemId)));

            IntPtr filePopup = BuildFilePopup(callback);
            IntPtr viewPopup = BuildViewPopup(callback);
            IntPtr projectPopup = BuildProjectPopup(callback);
            IntPtr buildPopup = BuildBuildPopup(callback);
            IntPtr debugPopup = BuildDebugPopup(callback);
            IntPtr toolsPopup = BuildToolsPopup(callback);
            IntPtr helpPopup = BuildHelpPopup(callback);

            CreateTopMenuButton(16, 104, "文件", "📁", filePopup);
            CreateTopMenuButton(128, 104, "视图", "👁️", viewPopup);
            CreateTopMenuButton(240, 104, "项目", "🗂️", projectPopup);
            CreateTopMenuButton(352, 104, "生成", "🧱", buildPopup);
            CreateTopMenuButton(464, 104, "调试", "🐞", debugPopup);
            CreateTopMenuButton(576, 104, "工具", "🛠️", toolsPopup);
            CreateTopMenuButton(688, 104, "帮助", "❓", helpPopup);
        }

        private IntPtr BuildFilePopup(EmojiWindowNative.MenuItemClickCallback callback)
        {
            IntPtr popup = EmojiWindowNative.CreateEmojiPopupMenu(App.Window);
            AddPopupItem(popup, "🧭 组件导航", MenuComponentNavRoot);
            AddPopupItem(popup, "📂 全部展开", MenuExpandAll);
            AddPopupItem(popup, "📁 全部折叠", MenuCollapseAll);
            AddPopupItem(popup, "📍 定位当前页", MenuLocateCurrent);

            int nextMenuId = 2000;
            foreach (DemoCategoryDefinition category in _categories)
            {
                int categoryMenuId = nextMenuId++;
                AddPopupSubItem(popup, MenuComponentNavRoot, category.Icon + " " + category.Title, categoryMenuId);

                int overviewMenuId = nextMenuId++;
                _menuPageMap[overviewMenuId] = category.OverviewPage.Key;
                AddPopupSubItem(popup, categoryMenuId, "🗂️ 概览 / " + category.OverviewPage.Title, overviewMenuId);

                foreach (DemoPageDefinition page in category.Pages)
                {
                    int pageMenuId = nextMenuId++;
                    _menuPageMap[pageMenuId] = page.Key;
                    AddPopupSubItem(popup, categoryMenuId, "✨ " + page.Title, pageMenuId);
                }
            }

            EmojiWindowNative.SetPopupMenuCallback(popup, callback);
            return popup;
        }

        private IntPtr BuildViewPopup(EmojiWindowNative.MenuItemClickCallback callback)
        {
            IntPtr popup = EmojiWindowNative.CreateEmojiPopupMenu(App.Window);
            AddPopupItem(popup, "📍 定位当前页", MenuViewLocateCurrent);
            AddPopupItem(popup, "📂 展开树节点", MenuViewExpandAll);
            AddPopupItem(popup, "📁 折叠树节点", MenuViewCollapseAll);
            AddPopupItem(popup, "🗂️ 打开当前分类概览", MenuViewCurrentOverview);
            EmojiWindowNative.SetPopupMenuCallback(popup, callback);
            return popup;
        }

        private IntPtr BuildProjectPopup(EmojiWindowNative.MenuItemClickCallback callback)
        {
            IntPtr popup = EmojiWindowNative.CreateEmojiPopupMenu(App.Window);
            int nextMenuId = 2600;
            foreach (DemoCategoryDefinition category in _categories)
            {
                int menuId = nextMenuId++;
                _menuPageMap[menuId] = category.OverviewPage.Key;
                AddPopupItem(popup, category.Icon + " " + category.Title, menuId);
            }

            EmojiWindowNative.SetPopupMenuCallback(popup, callback);
            return popup;
        }

        private IntPtr BuildBuildPopup(EmojiWindowNative.MenuItemClickCallback callback)
        {
            IntPtr popup = EmojiWindowNative.CreateEmojiPopupMenu(App.Window);
            AddPopupItem(popup, "🔄 刷新当前页", MenuBuildRefreshCurrent);
            AddPopupItem(popup, "📐 重新布局窗口", MenuBuildRelayout);
            AddPopupItem(popup, "🎨 重应用主题", MenuBuildReapplyTheme);
            EmojiWindowNative.SetPopupMenuCallback(popup, callback);
            return popup;
        }

        private IntPtr BuildDebugPopup(EmojiWindowNative.MenuItemClickCallback callback)
        {
            IntPtr popup = EmojiWindowNative.CreateEmojiPopupMenu(App.Window);
            AddPopupItem(popup, "🧪 输出当前页 Key", MenuDebugCurrentKey);
            AddPopupItem(popup, "🌲 强制同步树选中", MenuDebugSyncTree);
            AddPopupItem(popup, "📝 输出页标题", MenuDebugHeaderInfo);
            EmojiWindowNative.SetPopupMenuCallback(popup, callback);
            return popup;
        }

        private IntPtr BuildToolsPopup(EmojiWindowNative.MenuItemClickCallback callback)
        {
            IntPtr popup = EmojiWindowNative.CreateEmojiPopupMenu(App.Window);
            AddPopupItem(popup, "🌞 浅色主题", MenuToolsThemeLight);
            AddPopupItem(popup, "🌙 深色主题", MenuToolsThemeDark);
            AddPopupItem(popup, "🛠️ 刷新主题", MenuToolsThemeRefresh);
            EmojiWindowNative.SetPopupMenuCallback(popup, callback);
            return popup;
        }

        private IntPtr BuildHelpPopup(EmojiWindowNative.MenuItemClickCallback callback)
        {
            IntPtr popup = EmojiWindowNative.CreateEmojiPopupMenu(App.Window);
            AddPopupItem(popup, "📖 使用说明", MenuHelpGuide);
            AddPopupItem(popup, "💡 当前页说明", MenuHelpCurrentPage);
            AddPopupItem(popup, "🧩 关于演示", MenuHelpAbout);
            EmojiWindowNative.SetPopupMenuCallback(popup, callback);
            return popup;
        }

        private void CreateTopMenuButton(int x, int width, string text, string emoji, IntPtr popup)
        {
            int buttonId = 0;
            buttonId = App.Button(
                x,
                MenuY,
                width,
                MenuHeight,
                text,
                emoji,
                MenuBackground,
                () => ShowTopMenuPopup(popup, buttonId));
            _topMenuButtons.Add(buttonId);
            StyleTopMenuButton(buttonId);
        }

        private void ShowTopMenuPopup(IntPtr popup, int buttonId)
        {
            if (popup == IntPtr.Zero)
            {
                return;
            }

            CloseTopMenuPopup();
            EmojiWindowNative.GetWindowBounds(App.Window, out int windowX, out int windowY, out _, out _);
            EmojiWindowNative.GetButtonBounds(buttonId, out int buttonX, out int buttonY, out _, out int buttonHeight);
            EmojiWindowNative.ShowContextMenu(popup, windowX + buttonX + 8, windowY + buttonY + buttonHeight + 8);
        }

        private void CloseTopMenuPopup()
        {
            if (App.Window == IntPtr.Zero)
            {
                return;
            }

            Win32Native.SendMessage(App.Window, Win32Native.WM_CANCELMODE, IntPtr.Zero, IntPtr.Zero);
            IntPtr focused = Win32Native.GetFocus();
            if (focused != IntPtr.Zero)
            {
                Win32Native.PostMessage(focused, Win32Native.WM_KEYDOWN, new IntPtr(Win32Native.VK_ESCAPE), IntPtr.Zero);
                Win32Native.PostMessage(focused, Win32Native.WM_KEYUP, new IntPtr(Win32Native.VK_ESCAPE), IntPtr.Zero);
            }

            Win32Native.PostMessage(App.Window, Win32Native.WM_KEYDOWN, new IntPtr(Win32Native.VK_ESCAPE), IntPtr.Zero);
            Win32Native.PostMessage(App.Window, Win32Native.WM_KEYUP, new IntPtr(Win32Native.VK_ESCAPE), IntPtr.Zero);
        }

        private void StyleTopMenuButton(int buttonId)
        {
            DemoThemePalette palette = Palette;
            uint background = palette.PageBackground;
            uint border = background;
            uint hoverBackground = palette.Dark
                ? EmojiWindowNative.ARGB(255, 37, 39, 42)
                : EmojiWindowNative.ARGB(255, 236, 240, 245);
            uint hoverBorder = hoverBackground;

            EmojiWindowNative.SetButtonBackgroundColor(buttonId, background);
            EmojiWindowNative.SetButtonBorderColor(buttonId, border);
            EmojiWindowNative.SetButtonTextColor(buttonId, palette.Text);
            EmojiWindowNative.SetButtonHoverColors(buttonId, hoverBackground, hoverBorder, palette.Text);
            EmojiWindowNative.SetButtonStyle(buttonId, 0);
            EmojiWindowNative.SetButtonSize(buttonId, 0);
            EmojiWindowNative.SetButtonRound(buttonId, 4);
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
            _pageNodeMap[page.Key] = nodeId;
        }

        public void RegisterPageThemeHandler(IntPtr pageHandle, Action applyTheme)
        {
            if (pageHandle == IntPtr.Zero || applyTheme == null)
            {
                return;
            }

            _pageThemeHandlers[pageHandle] = applyTheme;
        }

        private void OnTreeNodeSelected(int nodeId)
        {
            if (_suppressNavNodeOnce == nodeId)
            {
                _suppressNavNodeOnce = 0;
                return;
            }

            if (_nodePageMap.TryGetValue(nodeId, out string pageKey))
            {
                ShowPage(pageKey);
            }
        }

        private void OnMenuAction(int itemId)
        {
            if (_menuPageMap.TryGetValue(itemId, out string pageKey))
            {
                ShowPage(pageKey);
                return;
            }

            switch (itemId)
            {
                case MenuExpandAll:
                case MenuViewExpandAll:
                    EmojiWindowNative.ExpandAll(_navTree);
                    SetStatus("菜单操作：树形导航已全部展开");
                    break;
                case MenuCollapseAll:
                case MenuViewCollapseAll:
                    EmojiWindowNative.CollapseAll(_navTree);
                    SetStatus("菜单操作：树形导航已全部折叠");
                    break;
                case MenuLocateCurrent:
                case MenuViewLocateCurrent:
                    SyncTreeSelection(_currentPageKey, true);
                    SetStatus("菜单操作：已定位到当前页对应的树节点");
                    break;
                case MenuViewCurrentOverview:
                    OpenCurrentOverview();
                    break;
                case MenuBuildRefreshCurrent:
                    RefreshCurrentPage("生成菜单：当前页已刷新");
                    break;
                case MenuBuildRelayout:
                    ResizeShell(App.Width, App.Height);
                    SetStatus("生成菜单：窗口布局已重新应用");
                    break;
                case MenuBuildReapplyTheme:
                    ApplyTheme(true);
                    SetStatus("生成菜单：主题已重新应用");
                    break;
                case MenuDebugCurrentKey:
                    SetStatus("调试菜单：current_page=" + _currentPageKey);
                    break;
                case MenuDebugSyncTree:
                    SyncTreeSelection(_currentPageKey, true);
                    SetStatus("调试菜单：树节点选中已强制同步");
                    break;
                case MenuDebugHeaderInfo:
                    if (_pagesByKey.TryGetValue(_currentPageKey, out DemoPageDefinition currentPage))
                    {
                        SetStatus("调试菜单：title=" + currentPage.Title);
                    }
                    break;
                case MenuToolsThemeLight:
                    SetDarkTheme(false);
                    SetStatus("工具菜单：已切换到浅色主题");
                    break;
                case MenuToolsThemeDark:
                    SetDarkTheme(true);
                    SetStatus("工具菜单：已切换到深色主题");
                    break;
                case MenuToolsThemeRefresh:
                    ApplyTheme(true);
                    SetStatus("工具菜单：主题刷新完成");
                    break;
                case MenuHelpGuide:
                    ShowMessage("📖 使用说明", "左上角菜单可快速切页、切主题、同步树节点和查看当前页信息。", "📖");
                    SetStatus("帮助菜单：已打开使用说明");
                    break;
                case MenuHelpCurrentPage:
                    ShowCurrentPageHelp();
                    break;
                case MenuHelpAbout:
                    ShowMessage("🧩 关于演示", "这是 EmojiWindow C# 全组件树形演示，布局与 Python 增强版保持对齐。", "🧩");
                    SetStatus("帮助菜单：已打开关于演示");
                    break;
            }
        }

        private void OpenCurrentOverview()
        {
            if (!string.IsNullOrEmpty(_currentPageKey) && _pageOverviewMap.TryGetValue(_currentPageKey, out string overviewKey))
            {
                ShowPage(overviewKey);
                SetStatus("视图菜单：已跳转到当前分类概览");
            }
        }

        private void RefreshCurrentPage(string note)
        {
            if (!string.IsNullOrEmpty(_currentPageKey) && _pageHandles.TryGetValue(_currentPageKey, out IntPtr pageHandle))
            {
                ApplyThemeForPage(pageHandle);
                SyncTreeSelection(_currentPageKey, true);
                BringStatusToFront();
            }

            SetStatus(note);
        }

        private void ShowCurrentPageHelp()
        {
            if (_pagesByKey.TryGetValue(_currentPageKey, out DemoPageDefinition page))
            {
                ShowMessage("💡 当前页说明", page.Description, "💡");
                SetStatus("帮助菜单：已打开当前页说明");
            }
        }

        private void ShowMessage(string title, string message, string icon)
        {
            byte[] titleBytes = App.U(title);
            byte[] messageBytes = App.U(message);
            byte[] iconBytes = App.U(icon);
            EmojiWindowNative.show_message_box_bytes(App.Window, titleBytes, titleBytes.Length, messageBytes, messageBytes.Length, iconBytes, iconBytes.Length);
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
                pageHandle = App.Panel(PageMargin, HeaderHeight, PageWidth, PageHeight, Palette.PageBackground, _host);
                _pageHandles[pageKey] = pageHandle;
                page.Build(this, pageHandle);
            }

            EmojiWindowNative.SetPanelBackgroundColor(pageHandle, Palette.PageBackground);
            ApplyThemeForPage(pageHandle);
            SetLabelText(_headerTitle, page.Title);
            SetLabelText(_headerDesc, page.Description);
            Win32Native.ShowWindow(pageHandle, Win32Native.SW_SHOW);
            _currentPageKey = pageKey;
            SyncTreeSelection(pageKey);
            BringStatusToFront();
            SetStatus("已切换到 " + page.Title);
        }

        public void SetStatus(string text) => App.SetStatus(text);

        public void SetDarkTheme(bool darkMode)
        {
            EmojiWindowNative.SetDarkMode(darkMode ? 1 : 0);
            ApplyTheme();
        }

        public void ApplyTheme(bool force = false)
        {
            DemoThemePalette palette = Palette;
            if (!force && _lastAppliedDarkMode.HasValue && _lastAppliedDarkMode.Value == palette.Dark)
            {
                return;
            }

            ThemeRedrawScope redrawScope = BeginThemeRedraw();
            try
            {
                if (App.Window != IntPtr.Zero)
                {
                    EmojiWindowNative.SetWindowBackgroundColor(App.Window, palette.PageBackground);
                }

                if (_sidebar != IntPtr.Zero)
                {
                    EmojiWindowNative.SetPanelBackgroundColor(_sidebar, palette.SidebarBackground);
                }

                if (_host != IntPtr.Zero)
                {
                    EmojiWindowNative.SetPanelBackgroundColor(_host, palette.PageBackground);
                }

                IntPtr currentPageHandle = GetCurrentPageHandle();
                if (currentPageHandle != IntPtr.Zero)
                {
                    EmojiWindowNative.SetPanelBackgroundColor(currentPageHandle, palette.PageBackground);
                    ApplyThemeForPage(currentPageHandle);
                }

                if (_navTree != IntPtr.Zero)
                {
                    EmojiWindowNative.SetTreeViewBackgroundColor(_navTree, palette.TreeBackground);
                    EmojiWindowNative.SetTreeViewTextColor(_navTree, palette.SidebarText);
                    EmojiWindowNative.SetTreeViewSelectedBgColor(_navTree, DemoColors.Blue);
                    EmojiWindowNative.SetTreeViewSelectedForeColor(_navTree, DemoColors.White);
                    EmojiWindowNative.SetTreeViewHoverBgColor(_navTree, palette.TreeHoverBackground);
                }

                foreach (int buttonId in _topMenuButtons)
                {
                    StyleTopMenuButton(buttonId);
                }

                if (_sidebarTitle != IntPtr.Zero)
                {
                    EmojiWindowNative.SetLabelColor(_sidebarTitle, palette.SidebarText, palette.SidebarBackground);
                }

                if (_sidebarDesc != IntPtr.Zero)
                {
                    EmojiWindowNative.SetLabelColor(_sidebarDesc, palette.SidebarMuted, palette.SidebarBackground);
                }

                if (_headerTitle != IntPtr.Zero)
                {
                    EmojiWindowNative.SetLabelColor(_headerTitle, palette.Text, palette.PageBackground);
                }

                if (_headerDesc != IntPtr.Zero)
                {
                    EmojiWindowNative.SetLabelColor(_headerDesc, palette.Muted, palette.PageBackground);
                }

                if (App.StatusLabel != IntPtr.Zero)
                {
                    EmojiWindowNative.SetLabelColor(App.StatusLabel, palette.Muted, palette.PageBackground);
                }

                BringStatusToFront();
                _lastAppliedDarkMode = palette.Dark;
            }
            finally
            {
                redrawScope.Dispose();
            }
        }

        private void ApplyThemeForPage(IntPtr pageHandle)
        {
            if (_pageThemeHandlers.TryGetValue(pageHandle, out Action applyTheme))
            {
                applyTheme();
            }
        }

        private IntPtr GetCurrentPageHandle()
        {
            return !string.IsNullOrEmpty(_currentPageKey) && _pageHandles.TryGetValue(_currentPageKey, out IntPtr pageHandle)
                ? pageHandle
                : IntPtr.Zero;
        }

        private ThemeRedrawScope BeginThemeRedraw()
        {
            var handles = new List<IntPtr>(5);
            AddRedrawHandle(handles, App.Window);
            AddRedrawHandle(handles, _sidebar);
            AddRedrawHandle(handles, _host);
            AddRedrawHandle(handles, _navTree);
            AddRedrawHandle(handles, GetCurrentPageHandle());
            return new ThemeRedrawScope(handles);
        }

        private static void AddRedrawHandle(List<IntPtr> handles, IntPtr handle)
        {
            if (handle != IntPtr.Zero && !handles.Contains(handle))
            {
                Win32Native.SetRedraw(handle, false);
                handles.Add(handle);
            }
        }

        private sealed class ThemeRedrawScope : IDisposable
        {
            private readonly List<IntPtr> _handles;

            public ThemeRedrawScope(List<IntPtr> handles)
            {
                _handles = handles;
            }

            public void Dispose()
            {
                for (int i = _handles.Count - 1; i >= 0; i--)
                {
                    IntPtr handle = _handles[i];
                    Win32Native.SetRedraw(handle, true);
                    Win32Native.RefreshWindowTree(handle);
                }
            }
        }

        public void ResizeShell(int width, int height)
        {
            App.UpdateWindowSize(width, height);

            if (_sidebar != IntPtr.Zero)
            {
                Win32Native.MoveWindow(_sidebar, 16, SidebarTop, SidebarWidth, SidebarHeight, true);
            }

            if (_host != IntPtr.Zero)
            {
                Win32Native.MoveWindow(_host, ContentX, ShellTop, HostWidth, HostHeight, true);
            }

            if (_navTree != IntPtr.Zero)
            {
                Win32Native.MoveWindow(_navTree, 16, SidebarTreeTop, SidebarWidth - 32, SidebarHeight - SidebarTreeTop - 16, true);
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
                Win32Native.MoveWindow(App.StatusLabel, 16, App.Height - 72, App.Width - 32, 32, true);
            }

            BringStatusToFront();
        }

        private void SyncTreeSelection(string pageKey, bool forceScroll = false)
        {
            if (_navTree == IntPtr.Zero || string.IsNullOrEmpty(pageKey) || !_pageNodeMap.TryGetValue(pageKey, out int nodeId))
            {
                return;
            }

            if (EmojiWindowNative.GetSelectedNode(_navTree) != nodeId)
            {
                _suppressNavNodeOnce = nodeId;
                EmojiWindowNative.SetSelectedNode(_navTree, nodeId);
            }

            if (forceScroll || EmojiWindowNative.GetSelectedNode(_navTree) == nodeId)
            {
                EmojiWindowNative.ScrollToNode(_navTree, nodeId);
            }
        }

        private void BringStatusToFront()
        {
            if (App.StatusLabel == IntPtr.Zero)
            {
                return;
            }

            Win32Native.SetWindowPos(
                App.StatusLabel,
                Win32Native.HWND_TOP,
                0,
                0,
                0,
                0,
                Win32Native.SWP_NOMOVE | Win32Native.SWP_NOSIZE | Win32Native.SWP_NOACTIVATE | Win32Native.SWP_SHOWWINDOW);
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

        private void AddPopupItem(IntPtr popupMenu, string text, int itemId)
        {
            byte[] bytes = App.U(text);
            EmojiWindowNative.PopupMenuAddItem(popupMenu, bytes, bytes.Length, itemId);
        }

        private void AddPopupSubItem(IntPtr popupMenu, int parentId, string text, int itemId)
        {
            byte[] bytes = App.U(text);
            EmojiWindowNative.PopupMenuAddSubItem(popupMenu, parentId, bytes, bytes.Length, itemId);
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
            Win32Native.TrySetWindowIcon(app.Window, Path.Combine(AppDomain.CurrentDomain.BaseDirectory, "icon.ico"));
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
