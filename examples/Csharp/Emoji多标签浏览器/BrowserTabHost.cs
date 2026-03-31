using FBroSharp;
using FBroSharp.Lib;
using System;
using System.Collections.Generic;
using System.Threading;
using System.Windows.Forms;

namespace BaseTest
{
    internal sealed class BrowserTabInfo
    {
        public string BrowserFlag { get; set; }
        public string CurrentTitle { get; set; }
        public string CurrentUrl { get; set; }
        public string FaviconUrl { get; set; }
        public IntPtr ContentWindow { get; set; }
        public FBroSharpBrowser Browser { get; set; }
        public bool CanGoBack { get; set; }
        public bool CanGoForward { get; set; }
    }

    internal sealed class BrowserTabState
    {
        public string CurrentTitle { get; set; }
        public string CurrentUrl { get; set; }
        public string CurrentFaviconUrl { get; set; }
        public bool HasBrowser { get; set; }
        public bool CanGoBack { get; set; }
        public bool CanGoForward { get; set; }
    }

    internal static class BrowserTabHost
    {
        private static readonly object SyncRoot = new object();
        private static readonly List<BrowserTabInfo> Tabs = new List<BrowserTabInfo>();
        private static readonly byte[] TabFontName = NativeTabControl.ToUtf8("Microsoft YaHei UI");

        private const int TabHeaderWidth = 220;
        private const int TabHeaderHeight = 36;
        private const uint WM_LBUTTONDOWN = 0x0201;
        private const uint WM_LBUTTONUP = 0x0202;
        private const int MK_LBUTTON = 0x0001;

        private static Control _ownerControl;
        private static IntPtr _tabControl;
        private static int _browserSeed;
        private static NativeTabControl.TabCallback _tabChangedCallback;
        private static NativeTabControl.TabCloseCallback _tabCloseCallback;

        internal static event Action<BrowserTabState> CurrentTabStateChanged;

        internal static void Initialize(Control ownerControl, int width, int height)
        {
            if (ownerControl == null)
            {
                return;
            }

            _ownerControl = ownerControl;
            var ownerHandle = ownerControl.Handle;
            if (ownerHandle == IntPtr.Zero)
            {
                return;
            }

            if (_tabControl != IntPtr.Zero)
            {
                Layout(width, height);
                return;
            }

            _tabControl = RunOnUiThread(() => NativeTabControl.CreateTabControl(ownerHandle, 0, 0, Math.Max(width, 1), Math.Max(height, 1)));
            if (_tabControl == IntPtr.Zero)
            {
                return;
            }

            _tabChangedCallback = OnTabChanged;
            _tabCloseCallback = OnTabClose;

            RunOnUiThread(() =>
            {
                NativeTabControl.SetTabItemSize(_tabControl, TabHeaderWidth, TabHeaderHeight);
                NativeTabControl.SetTabFont(_tabControl, TabFontName, TabFontName.Length, 11f);
                NativeTabControl.SetTabColors(_tabControl, Argb(255, 255, 255, 255), Argb(255, 239, 241, 244), Argb(255, 32, 33, 36), Argb(255, 95, 99, 104));
                NativeTabControl.SetTabIndicatorColor(_tabControl, Argb(255, 64, 158, 255));
                NativeTabControl.SetTabPadding(_tabControl, 8, 3);
                NativeTabControl.SetTabClosable(_tabControl, 1);
                NativeTabControl.SetTabDraggable(_tabControl, 0);
                NativeTabControl.SetTabScrollable(_tabControl, 1);
                NativeTabControl.SetTabCallback(_tabControl, _tabChangedCallback);
                NativeTabControl.SetTabCloseCallback(_tabControl, _tabCloseCallback);
                NativeTabControl.UpdateTabControlLayout(_tabControl);
                NativeTabControl.RedrawTabControl(_tabControl);
            });

            NotifyCurrentTabState();
        }

        internal static BrowserTabInfo CreateBrowserTab(string title, string url)
        {
            var tab = CreateTab(title, url, true);
            if (tab == null)
            {
                return null;
            }

            var size = GetClientSize(tab.ContentWindow);
            BrowserDis.CreatBrowser(tab.CurrentTitle, url, tab.ContentWindow, 0, 0, size.width, size.height, tab.BrowserFlag);
            return tab;
        }

        internal static BrowserTabInfo CreatePopupTab(string targetUrl)
        {
            return CreateTab(NormalizeTitle(null, targetUrl), targetUrl, true);
        }

        internal static void AttachBrowser(IFBroSharpBrowser browser)
        {
            if (browser == null)
            {
                return;
            }

            BrowserTabInfo tab;
            int index;
            lock (SyncRoot)
            {
                index = Tabs.FindIndex(item => string.Equals(item.BrowserFlag, browser.GetUserFlag(), StringComparison.Ordinal));
                if (index < 0)
                {
                    return;
                }

                tab = Tabs[index];
                tab.Browser = (FBroSharpBrowser)browser;
                tab.CanGoBack = browser.CanGoBack();
                tab.CanGoForward = browser.CanGoForward();
            }

            UpdateTabTitle(browser.GetUserFlag(), browser.GetWindowsTitle());
            ActivateTab(index, tab.Browser);
            LayoutTab(tab);
            NotifyCurrentTabState();
        }

        internal static void UpdateTabTitle(string browserFlag, string title)
        {
            UpdateTab(browserFlag, tab => tab.CurrentTitle = NormalizeTitle(title, tab.CurrentUrl), true);
        }

        internal static void UpdateTabUrl(string browserFlag, string url)
        {
            UpdateTab(browserFlag, tab =>
            {
                tab.CurrentUrl = url ?? string.Empty;
                if (string.IsNullOrWhiteSpace(tab.CurrentTitle))
                {
                    tab.CurrentTitle = NormalizeTitle(null, tab.CurrentUrl);
                }
            }, true);
        }

        internal static void UpdateNavigationState(string browserFlag, bool canGoBack, bool canGoForward)
        {
            UpdateTab(browserFlag, tab =>
            {
                tab.CanGoBack = canGoBack;
                tab.CanGoForward = canGoForward;
            }, false);
        }

        internal static void UpdateFaviconUrl(string browserFlag, string faviconUrl)
        {
            UpdateTab(browserFlag, tab => tab.FaviconUrl = faviconUrl ?? string.Empty, false);
        }

        internal static void Layout(int width, int height)
        {
            if (_tabControl == IntPtr.Zero)
            {
                return;
            }

            RunOnUiThread(() =>
            {
                NativeTabControl.SetTabControlBounds(_tabControl, 0, 0, Math.Max(width, 1), Math.Max(height, 1));
                NativeTabControl.UpdateTabControlLayout(_tabControl);
                NativeTabControl.RedrawTabControl(_tabControl);
            });

            BrowserTabInfo[] tabs;
            lock (SyncRoot)
            {
                tabs = Tabs.ToArray();
            }

            foreach (var tab in tabs)
            {
                LayoutTab(tab);
            }

            NotifyCurrentTabState();
        }

        internal static void RequestCloseTab(int index)
        {
            BrowserTabInfo tab;
            lock (SyncRoot)
            {
                if (index < 0 || index >= Tabs.Count)
                {
                    return;
                }

                tab = Tabs[index];
            }

            if (IsBrowserAlive(tab.Browser))
            {
                tab.Browser.CloseBrowser(true, false);
                return;
            }

            RemoveTabByFlag(tab.BrowserFlag);
        }

        internal static void HandleBrowserClosed(IFBroSharpBrowser browser)
        {
            if (browser == null)
            {
                return;
            }

            RemoveTab(browser);
        }

        internal static void GoBackCurrent()
        {
            var tab = GetCurrentTab();
            if (IsBrowserAlive(tab?.Browser) && tab.Browser.CanGoBack())
            {
                tab.Browser.GoBack();
            }
        }

        internal static void GoForwardCurrent()
        {
            var tab = GetCurrentTab();
            if (IsBrowserAlive(tab?.Browser) && tab.Browser.CanGoForward())
            {
                tab.Browser.GoForward();
            }
        }

        internal static void ReloadCurrent()
        {
            var tab = GetCurrentTab();
            if (IsBrowserAlive(tab?.Browser))
            {
                tab.Browser.Reload();
            }
        }

        internal static void NavigateCurrent(string url)
        {
            var tab = GetCurrentTab();
            if (!IsBrowserAlive(tab?.Browser))
            {
                return;
            }

            var normalizedUrl = NormalizeAddress(url);
            if (string.IsNullOrWhiteSpace(normalizedUrl))
            {
                return;
            }

            tab.CurrentUrl = normalizedUrl;
            var frame = tab.Browser.GetMainFrame();
            if (frame != null)
            {
                frame.LoadURL(normalizedUrl);
            }

            NotifyCurrentTabState();
        }

        private static BrowserTabInfo CreateTab(string title, string url, bool select)
        {
            if (_tabControl == IntPtr.Zero)
            {
                return null;
            }

            var safeTitle = NormalizeTitle(title, url);
            var titleBytes = NativeTabControl.ToUtf8(safeTitle);
            var index = RunOnUiThread(() =>
            {
                var currentIndex = NativeTabControl.GetTabCount(_tabControl);
                NativeTabControl.AddTabItem(_tabControl, titleBytes, titleBytes.Length, IntPtr.Zero);
                NativeTabControl.UpdateTabControlLayout(_tabControl);
                NativeTabControl.RedrawTabControl(_tabControl);
                return currentIndex;
            });

            var tab = new BrowserTabInfo
            {
                BrowserFlag = "BrowserTab_" + Interlocked.Increment(ref _browserSeed),
                CurrentTitle = safeTitle,
                CurrentUrl = url ?? string.Empty,
                ContentWindow = RunOnUiThread(() => NativeTabControl.GetTabContentWindow(_tabControl, index))
            };

            lock (SyncRoot)
            {
                Tabs.Add(tab);
            }

            if (select)
            {
                ActivateTab(index, null);
            }

            NotifyCurrentTabState();
            return tab;
        }

        private static void UpdateTab(string browserFlag, Action<BrowserTabInfo> updateAction, bool redrawTitle)
        {
            if (_tabControl == IntPtr.Zero || string.IsNullOrWhiteSpace(browserFlag))
            {
                return;
            }

            BrowserTabInfo tab;
            int index;
            lock (SyncRoot)
            {
                index = Tabs.FindIndex(item => string.Equals(item.BrowserFlag, browserFlag, StringComparison.Ordinal));
                if (index < 0)
                {
                    return;
                }

                tab = Tabs[index];
                updateAction(tab);
            }

            if (redrawTitle)
            {
                var titleBytes = NativeTabControl.ToUtf8(NormalizeTitle(tab.CurrentTitle, tab.CurrentUrl));
                RunOnUiThread(() =>
                {
                    NativeTabControl.SetTabTitle(_tabControl, index, titleBytes, titleBytes.Length);
                    NativeTabControl.RedrawTabControl(_tabControl);
                });
            }

            NotifyCurrentTabState();
        }

        private static void RemoveTab(IFBroSharpBrowser browser)
        {
            if (_tabControl == IntPtr.Zero || browser == null)
            {
                return;
            }

            int index;
            lock (SyncRoot)
            {
                index = Tabs.FindIndex(item =>
                    (!string.IsNullOrWhiteSpace(item.BrowserFlag) && string.Equals(item.BrowserFlag, browser.GetUserFlag(), StringComparison.Ordinal)) ||
                    (item.Browser != null && item.Browser.IsSame(browser)));
                if (index < 0)
                {
                    return;
                }

                Tabs.RemoveAt(index);
            }

            RemoveNativeTab(index);
        }

        private static void RemoveTabByFlag(string browserFlag)
        {
            if (_tabControl == IntPtr.Zero || string.IsNullOrWhiteSpace(browserFlag))
            {
                return;
            }

            int index;
            lock (SyncRoot)
            {
                index = Tabs.FindIndex(item => string.Equals(item.BrowserFlag, browserFlag, StringComparison.Ordinal));
                if (index < 0)
                {
                    return;
                }

                Tabs.RemoveAt(index);
            }

            RemoveNativeTab(index);
        }

        private static void RemoveNativeTab(int index)
        {
            RunOnUiThread(() =>
            {
                if (index >= 0 && index < NativeTabControl.GetTabCount(_tabControl))
                {
                    NativeTabControl.RemoveTabItem(_tabControl, index);
                    NativeTabControl.UpdateTabControlLayout(_tabControl);
                }

                var remaining = NativeTabControl.GetTabCount(_tabControl);
                if (remaining > 0)
                {
                    NativeTabControl.SelectTab(_tabControl, Math.Min(index, remaining - 1));
                }

                NativeTabControl.RedrawTabControl(_tabControl);
            });

            NotifyCurrentTabState();
        }

        private static void ActivateTab(int index, FBroSharpBrowser browser)
        {
            ActivateTab(index, browser, 6);
        }

        private static void ActivateTab(int index, FBroSharpBrowser browser, int remainingAttempts)
        {
            PostToUiThread(() =>
            {
                if (_tabControl == IntPtr.Zero)
                {
                    return;
                }

                if (index < 0 || index >= NativeTabControl.GetTabCount(_tabControl))
                {
                    return;
                }

                NativeTabControl.SelectTab(_tabControl, index);
                NativeTabControl.UpdateTabControlLayout(_tabControl);
                NativeTabControl.RedrawTabControl(_tabControl);

                if (browser != null && browser.IsValid && !browser.IsNULL())
                {
                    browser.SetFocus(true);
                }

                if (NativeTabControl.GetCurrentTabIndex(_tabControl) == index)
                {
                    NotifyCurrentTabState();
                    return;
                }

                if (TryActivateTabByClick(index, browser))
                {
                    NotifyCurrentTabState();
                    return;
                }

                if (remainingAttempts <= 0)
                {
                    return;
                }

                QueueDelayedUi(() => ActivateTab(index, browser, remainingAttempts - 1));
            });
        }

        private static bool TryActivateTabByClick(int index, FBroSharpBrowser browser)
        {
            if (_tabControl == IntPtr.Zero || index < 0)
            {
                return false;
            }

            if (!NativeTabControl.GetClientRect(_tabControl, out var rect))
            {
                return false;
            }

            var clientWidth = Math.Max(rect.Right - rect.Left, 1);
            var x = index * TabHeaderWidth + Math.Min(24, Math.Max(TabHeaderWidth / 4, 8));
            var y = Math.Min(TabHeaderHeight / 2, Math.Max(rect.Bottom - rect.Top - 1, 0));
            if (x >= clientWidth)
            {
                return false;
            }

            var lParam = MakeLParam(x, y);
            NativeTabControl.SendMessage(_tabControl, WM_LBUTTONDOWN, (IntPtr)MK_LBUTTON, lParam);
            NativeTabControl.SendMessage(_tabControl, WM_LBUTTONUP, IntPtr.Zero, lParam);

            if (browser != null && browser.IsValid && !browser.IsNULL())
            {
                browser.SetFocus(true);
            }

            return NativeTabControl.GetCurrentTabIndex(_tabControl) == index;
        }

        private static void OnTabChanged(IntPtr hTabControl, int selectedIndex)
        {
            BrowserTabInfo tab;
            lock (SyncRoot)
            {
                tab = selectedIndex >= 0 && selectedIndex < Tabs.Count ? Tabs[selectedIndex] : null;
            }

            if (tab != null)
            {
                LayoutTab(tab);
            }

            NotifyCurrentTabState();
        }

        private static void OnTabClose(IntPtr hTabControl, int index)
        {
            RequestCloseTab(index);
        }

        private static void LayoutTab(BrowserTabInfo tab)
        {
            if (tab == null || !IsBrowserAlive(tab.Browser))
            {
                return;
            }

            var size = GetClientSize(tab.ContentWindow);
            tab.Browser.MoveWindow(0, 0, size.width, size.height, true);
        }

        private static (int width, int height) GetClientSize(IntPtr windowHandle)
        {
            if (windowHandle == IntPtr.Zero)
            {
                return (1, 1);
            }

            var result = RunOnUiThread(() =>
            {
                if (!NativeTabControl.GetClientRect(windowHandle, out var rect))
                {
                    return Tuple.Create(false, default(NativeTabControl.RECT));
                }

                return Tuple.Create(true, rect);
            });

            if (!result.Item1)
            {
                return (1, 1);
            }

            var width = Math.Max(result.Item2.Right - result.Item2.Left, 1);
            var height = Math.Max(result.Item2.Bottom - result.Item2.Top, 1);
            return (width, height);
        }

        private static BrowserTabInfo GetCurrentTab()
        {
            if (_tabControl == IntPtr.Zero)
            {
                return null;
            }

            var index = RunOnUiThread(() => NativeTabControl.GetCurrentTabIndex(_tabControl));
            lock (SyncRoot)
            {
                return index >= 0 && index < Tabs.Count ? Tabs[index] : null;
            }
        }

        private static void NotifyCurrentTabState()
        {
            var state = BuildCurrentTabState();
            PostToUiThread(() => CurrentTabStateChanged?.Invoke(state));
        }

        private static BrowserTabState BuildCurrentTabState()
        {
            var tab = GetCurrentTab();
            if (tab == null)
            {
                return new BrowserTabState
                {
                    CurrentTitle = string.Empty,
                    CurrentUrl = string.Empty,
                    CurrentFaviconUrl = string.Empty,
                    HasBrowser = false,
                    CanGoBack = false,
                    CanGoForward = false
                };
            }

            return new BrowserTabState
            {
                CurrentTitle = tab.CurrentTitle ?? string.Empty,
                CurrentUrl = tab.CurrentUrl ?? string.Empty,
                CurrentFaviconUrl = tab.FaviconUrl ?? string.Empty,
                HasBrowser = IsBrowserAlive(tab.Browser),
                CanGoBack = tab.CanGoBack,
                CanGoForward = tab.CanGoForward
            };
        }

        private static bool IsBrowserAlive(FBroSharpBrowser browser)
        {
            return browser != null && browser.IsValid && !browser.IsNULL();
        }

        private static string NormalizeTitle(string title, string url)
        {
            if (!string.IsNullOrWhiteSpace(title))
            {
                return title.Trim();
            }

            if (!string.IsNullOrWhiteSpace(url))
            {
                return url.Trim();
            }

            return "新标签页";
        }

        private static string NormalizeAddress(string url)
        {
            if (string.IsNullOrWhiteSpace(url))
            {
                return string.Empty;
            }

            var value = url.Trim();
            if (value.Contains("://"))
            {
                return value;
            }

            return "https://" + value;
        }

        private static uint Argb(int a, int r, int g, int b)
        {
            return (uint)(((a & 255) << 24) | ((r & 255) << 16) | ((g & 255) << 8) | (b & 255));
        }

        private static void RunOnUiThread(Action action)
        {
            if (action == null)
            {
                return;
            }

            if (_ownerControl != null && _ownerControl.IsHandleCreated && !_ownerControl.IsDisposed && _ownerControl.InvokeRequired)
            {
                _ownerControl.Invoke(action);
                return;
            }

            action();
        }

        private static T RunOnUiThread<T>(Func<T> func)
        {
            if (func == null)
            {
                return default;
            }

            if (_ownerControl != null && _ownerControl.IsHandleCreated && !_ownerControl.IsDisposed && _ownerControl.InvokeRequired)
            {
                return (T)_ownerControl.Invoke(func);
            }

            return func();
        }

        private static void PostToUiThread(Action action)
        {
            if (action == null)
            {
                return;
            }

            if (_ownerControl != null && _ownerControl.IsHandleCreated && !_ownerControl.IsDisposed)
            {
                _ownerControl.BeginInvoke(action);
                return;
            }

            action();
        }

        private static void QueueDelayedUi(Action action)
        {
            if (action == null)
            {
                return;
            }

            ThreadPool.QueueUserWorkItem(_ =>
            {
                Thread.Sleep(60);
                PostToUiThread(action);
            });
        }

        private static IntPtr MakeLParam(int x, int y)
        {
            return (IntPtr)((y << 16) | (x & 0xFFFF));
        }
    }
}
