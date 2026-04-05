using System;
using EmojiWindowChromeStyleBrowserDemo.Core;
using EmojiWindowChromeStyleBrowserDemo.Services;
using EmojiWindowChromeStyleBrowserDemo.Styling;
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
            RegisterCallbacks();
            CreateUi();
            AddInitialTab();
            ThemeService.Apply(_state);
            LayoutService.ApplyAll(_state);
            SyncActiveTab();

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
            ContentService.SetStatus(_state, "C# 版 Chrome 风格多标签浏览器示例已启动。");
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
                NavigationService.AddTab(_state, "https://www.google.com/", true);
                FavoriteMenuService.Rebuild(_state);
                LayoutService.ApplyAll(_state);
                SyncActiveTab();
                ContentService.SetStatus(_state, "已新建标签页。");
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
                ContentService.SetStatus(_state, "已切换到标签：" + (_state.ActiveTabIndex + 1));
            }
        }

        private void OnMenuItemClick(int menuId, int itemId)
        {
            if (FavoriteMenuService.HandleMenuClick(_state, itemId))
            {
                FavoriteMenuService.Rebuild(_state);
                LayoutService.ApplyAll(_state);
                SyncActiveTab();
                ContentService.SetStatus(_state, "已处理收藏菜单动作。");
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
            ContentService.SetStatus(_state, "已更新当前标签地址。");
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

        private void OnWindowClose(IntPtr hwnd)
        {
        }
    }
}
