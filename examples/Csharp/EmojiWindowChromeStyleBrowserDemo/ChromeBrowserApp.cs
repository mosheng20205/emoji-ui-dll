using System;
using System.Collections.Generic;
using System.IO;
using EmojiWindowChromeStyleBrowserDemo.Core;
using EmojiWindowChromeStyleBrowserDemo.Services;
using EmojiWindowChromeStyleBrowserDemo.Styling;
using EmojiWindowChromeStyleBrowserDemo.UI;
using EmojiWindowDemo;

namespace EmojiWindowChromeStyleBrowserDemo
{
    internal sealed class ChromeBrowserApp
    {
        private readonly BrowserState _state = new BrowserState();

        public void Run()
        {
            SeedFavorites();
            CreateWindow();
            EmojiWindowNative.ShowEmojiWindow(_state.Window, 0);
            RegisterCallbacks();
            CreateUi();
            AddInitialTab();
            ThemeService.Apply(_state);
            LayoutService.ApplyAll(_state);
            SyncActiveTab();
            EmojiWindowNative.ShowEmojiWindow(_state.Window, 1);

            EmojiWindowNative.set_message_loop_main_window(_state.Window);
            EmojiWindowNative.run_message_loop();
        }

        private void SeedFavorites()
        {
            _state.Favorites.Add("https://www.google.com/");
            _state.Favorites.Add("https://news.google.com/");
            _state.Favorites.Add("https://github.com/");
            _state.Favorites.Add("https://stackoverflow.com/");
        }

        private void CreateWindow()
        {
            _state.Width = ChromeLayoutMetrics.WindowWidth;
            _state.Height = ChromeLayoutMetrics.WindowHeight;

            byte[] titleBytes = EmojiWindowNative.ToUtf8("Chrome风格多标签浏览器 C#");
            _state.Window = EmojiWindowNative.create_window_bytes_ex(
                titleBytes,
                titleBytes.Length,
                -1,
                -1,
                _state.Width,
                _state.Height,
                ChromePalette.TitleBarBackground(_state.DarkThemeEnabled),
                ChromePalette.WindowBackground(_state.DarkThemeEnabled));

            if (_state.Window == IntPtr.Zero)
            {
                throw new InvalidOperationException("Failed to create browser window.");
            }

            ApplyWindowIcon();
        }

        private void ApplyWindowIcon()
        {
            const string preferredIconPath = @"T:\易语言源码\API创建窗口\emoji_window_cpp\examples\Csharp\谷歌.ico";
            string iconPath = File.Exists(preferredIconPath)
                ? preferredIconPath
                : Path.Combine(AppDomain.CurrentDomain.BaseDirectory, "谷歌.ico");

            if (!File.Exists(iconPath))
            {
                return;
            }

            byte[] iconBytes = File.ReadAllBytes(iconPath);
            if (iconBytes.Length > 0)
            {
                EmojiWindowNative.set_window_icon_bytes(_state.Window, iconBytes, iconBytes.Length);
            }
        }

        private void RegisterCallbacks()
        {
            _state.ButtonClickCallback = OnButtonClick;
            _state.MenuItemClickCallback = OnMenuItemClick;
            _state.EditBoxKeyCallback = OnEditKey;
            _state.WindowResizeCallback = OnWindowResize;
            _state.WindowCloseCallback = OnWindowClose;

            EmojiWindowNative.set_button_click_callback(_state.ButtonClickCallback);
            EmojiWindowNative.SetWindowResizeCallback(_state.WindowResizeCallback);
            EmojiWindowNative.SetWindowCloseCallback(_state.WindowCloseCallback);
        }

        private void CreateUi()
        {
            TabBarService.CreateSharedControls(_state);
            ToolbarService.CreateSharedControls(_state);
            ContentService.CreateSharedControls(_state);
            FavoriteMenuService.Rebuild(_state);
            ContentService.SetStatus(_state, "C# Chrome style browser demo started.");
        }

        private void AddInitialTab()
        {
            NavigationService.AddTab(_state, "https://www.google.com/", true);
        }

        private void SyncActiveTab()
        {
            ToolbarService.SyncAddressBar(_state);
            TabBarService.ApplyStyles(_state);
            ContentService.SyncFromActiveTab(_state);
        }

        private void OnButtonClick(int buttonId, IntPtr parentHwnd)
        {
            if (buttonId == _state.NewTabButtonId)
            {
                WindowRedrawScope redrawScope = BeginWindowRedraw();
                try
                {
                    NavigationService.AddTab(_state, "https://www.google.com/", true);
                    FavoriteMenuService.Rebuild(_state);
                    LayoutService.ApplyAll(_state);
                    SyncActiveTab();
                    ContentService.SetStatus(_state, "New tab created.");
                }
                finally
                {
                    redrawScope.Dispose();
                }

                return;
            }

            if (buttonId == _state.ThemeButtonId)
            {
                HandleThemeToggle();
                return;
            }

            if (ToolbarService.HandleToolbarButton(_state, buttonId))
            {
                FavoriteMenuService.Rebuild(_state);
                LayoutService.ApplyAll(_state);
                SyncActiveTab();
                return;
            }

            if (TabBarService.TryActivateByButton(_state, buttonId))
            {
                LayoutService.ApplyAll(_state);
                SyncActiveTab();
                ContentService.SetStatus(_state, "Switched to tab " + (_state.ActiveTabIndex + 1) + ".");
            }
        }

        private void OnMenuItemClick(int menuId, int itemId)
        {
            if (FavoriteMenuService.HandleMenuClick(_state, itemId))
            {
                FavoriteMenuService.Rebuild(_state);
                LayoutService.ApplyAll(_state);
                SyncActiveTab();
                ContentService.SetStatus(_state, "Favorites menu action handled.");
            }
        }

        private void OnEditKey(IntPtr hEdit, int keyCode, int keyDown, int shift, int ctrl, int alt)
        {
            if (keyDown == 0 || keyCode != 13)
            {
                return;
            }

            ToolbarService.CommitAddressBar(_state);
            LayoutService.ApplyAll(_state);
            SyncActiveTab();
            ContentService.SetStatus(_state, "Address updated.");
        }

        private void OnWindowResize(IntPtr hwnd, int width, int height)
        {
            if (hwnd != _state.Window || width <= 0 || height <= 0)
            {
                return;
            }

            if (width == _state.Width && height == _state.Height)
            {
                return;
            }

            _state.Width = width;
            _state.Height = height;
            LayoutService.ApplyAll(_state);
            SyncActiveTab();
        }

        private void HandleThemeToggle()
        {
            WindowRedrawScope redrawScope = BeginWindowRedraw();
            try
            {
                ThemeService.Toggle(_state);
                ContentService.RefreshThemeSummary(_state);
                ContentService.SetStatus(_state, _state.DarkThemeEnabled ? "Switched to dark theme." : "Switched to light theme.");
            }
            finally
            {
                redrawScope.Dispose();
            }
        }

        private WindowRedrawScope BeginWindowRedraw()
        {
            return new WindowRedrawScope(Win32Native.CollectWindowTree(_state.Window));
        }

        private void OnWindowClose(IntPtr hwnd)
        {
        }

        private sealed class WindowRedrawScope : IDisposable
        {
            private readonly List<IntPtr> _handles;

            public WindowRedrawScope(List<IntPtr> handles)
            {
                _handles = handles ?? new List<IntPtr>();
                for (int i = 0; i < _handles.Count; i++)
                {
                    Win32Native.SetRedraw(_handles[i], false);
                }
            }

            public void Dispose()
            {
                for (int i = _handles.Count - 1; i >= 0; i--)
                {
                    Win32Native.SetRedraw(_handles[i], true);
                }

                if (_handles.Count > 0)
                {
                    Win32Native.RefreshWindowTree(_handles[0]);
                }
            }
        }
    }
}
