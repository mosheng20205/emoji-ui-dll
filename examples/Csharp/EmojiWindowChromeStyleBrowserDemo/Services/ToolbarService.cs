using System;
using EmojiWindowChromeStyleBrowserDemo.Core;
using EmojiWindowChromeStyleBrowserDemo.Styling;
using EmojiWindowChromeStyleBrowserDemo.UI;
using EmojiWindowDemo;

namespace EmojiWindowChromeStyleBrowserDemo.Services
{
    internal static class ToolbarService
    {
        public static void CreateSharedControls(BrowserState state)
        {
            int toolbarWidth = ChromeLayoutMetrics.GetToolbarWidth(state.Width);
            bool dark = state.DarkThemeEnabled;
            state.ToolbarPanel = ChromeControlFactory.CreateToolbarPanel(state);
            state.AddressShellLabel = ChromeControlFactory.CreateLabel(
                state,
                ChromeLayoutMetrics.ToolbarX + ChromeLayoutMetrics.AddressShellX,
                ChromeLayoutMetrics.ToolbarY + ChromeLayoutMetrics.AddressShellY,
                ChromeLayoutMetrics.GetAddressShellWidth(toolbarWidth),
                ChromeLayoutMetrics.AddressShellHeight,
                string.Empty,
                ChromePalette.AddressShellBackground(dark),
                ChromePalette.AddressShellBackground(dark));

            state.AddressBadgeLabel = ChromeControlFactory.CreateLabel(
                state,
                ChromeLayoutMetrics.ToolbarX + ChromeLayoutMetrics.AddressShellX + ChromeLayoutMetrics.AddressBadgeOffsetX,
                ChromeLayoutMetrics.ToolbarY + ChromeLayoutMetrics.AddressBadgeOffsetY,
                ChromeLayoutMetrics.AddressBadgeSize,
                ChromeLayoutMetrics.AddressBadgeSize,
                "G",
                ChromePalette.AddressBackground(dark),
                ChromePalette.AddressBadgeBackground("https://www.google.com/"),
                12,
                1);

            state.BackButtonId = CreateToolbarButton(state, "<", ChromeLayoutMetrics.ToolbarButtonStartX, toolbarWidth);
            state.ForwardButtonId = CreateToolbarButton(state, ">", ChromeLayoutMetrics.ToolbarButtonStartX + ChromeLayoutMetrics.ToolbarButtonGap, toolbarWidth);
            state.ReloadButtonId = CreateToolbarButton(state, "R", ChromeLayoutMetrics.ToolbarButtonStartX + (ChromeLayoutMetrics.ToolbarButtonGap * 2), toolbarWidth);
            state.HomeButtonId = CreateToolbarButton(state, "H", ChromeLayoutMetrics.ToolbarButtonStartX + (ChromeLayoutMetrics.ToolbarButtonGap * 3), toolbarWidth);
            state.FavoriteButtonId = CreateToolbarButton(state, "*", ChromeLayoutMetrics.GetRightFavoriteX(toolbarWidth), toolbarWidth);
            state.ThemeButtonId = CreateToolbarButton(state, "D", ChromeLayoutMetrics.GetRightThemeX(toolbarWidth), toolbarWidth);

            state.AddressEdit = ChromeControlFactory.CreateAddressEditBox(state, "https://www.google.com/");
            EmojiWindowNative.SetEditBoxKeyCallback(state.AddressEdit, state.EditBoxKeyCallback);
            ApplyTheme(state);

            EmojiWindowNative.ShowEditBox(state.AddressEdit, 1);
            EmojiWindowNative.ShowLabel(state.AddressShellLabel, 1);
            EmojiWindowNative.ShowLabel(state.AddressBadgeLabel, 1);
            Win32Native.EnsureVisible(state.AddressEdit);
        }

        public static void Layout(BrowserState state)
        {
            int toolbarWidth = ChromeLayoutMetrics.GetToolbarWidth(state.Width);
            int toolbarX = ChromeLayoutMetrics.ToolbarX;
            int toolbarY = ChromeLayoutMetrics.ToolbarY;

            Win32Native.MoveChild(state.ToolbarPanel, toolbarX, toolbarY, toolbarWidth, ChromeLayoutMetrics.ToolbarHeight);
            LayoutToolbarButton(state.BackButtonId, toolbarX + ChromeLayoutMetrics.ToolbarButtonStartX, toolbarY + ChromeLayoutMetrics.ToolbarInnerTop);
            LayoutToolbarButton(state.ForwardButtonId, toolbarX + ChromeLayoutMetrics.ToolbarButtonStartX + ChromeLayoutMetrics.ToolbarButtonGap, toolbarY + ChromeLayoutMetrics.ToolbarInnerTop);
            LayoutToolbarButton(state.ReloadButtonId, toolbarX + ChromeLayoutMetrics.ToolbarButtonStartX + (ChromeLayoutMetrics.ToolbarButtonGap * 2), toolbarY + ChromeLayoutMetrics.ToolbarInnerTop);
            LayoutToolbarButton(state.HomeButtonId, toolbarX + ChromeLayoutMetrics.ToolbarButtonStartX + (ChromeLayoutMetrics.ToolbarButtonGap * 3), toolbarY + ChromeLayoutMetrics.ToolbarInnerTop);
            LayoutToolbarButton(state.FavoriteButtonId, toolbarX + ChromeLayoutMetrics.GetRightFavoriteX(toolbarWidth), toolbarY + ChromeLayoutMetrics.ToolbarInnerTop);
            LayoutToolbarButton(state.ThemeButtonId, toolbarX + ChromeLayoutMetrics.GetRightThemeX(toolbarWidth), toolbarY + ChromeLayoutMetrics.ToolbarInnerTop);

            int shellWidth = ChromeLayoutMetrics.GetAddressShellWidth(toolbarWidth);
            EmojiWindowNative.SetLabelBounds(
                state.AddressShellLabel,
                toolbarX + ChromeLayoutMetrics.AddressShellX,
                toolbarY + ChromeLayoutMetrics.AddressShellY,
                shellWidth,
                ChromeLayoutMetrics.AddressShellHeight);
            EmojiWindowNative.SetLabelBounds(
                state.AddressBadgeLabel,
                toolbarX + ChromeLayoutMetrics.AddressShellX + ChromeLayoutMetrics.AddressBadgeOffsetX,
                toolbarY + ChromeLayoutMetrics.AddressBadgeOffsetY,
                ChromeLayoutMetrics.AddressBadgeSize,
                ChromeLayoutMetrics.AddressBadgeSize);
            EmojiWindowNative.SetEditBoxBounds(
                state.AddressEdit,
                toolbarX + ChromeLayoutMetrics.AddressShellX + ChromeLayoutMetrics.AddressEditOffsetX,
                toolbarY + ChromeLayoutMetrics.AddressEditOffsetY,
                Math.Max(140, shellWidth - 52),
                ChromeLayoutMetrics.AddressBarHeight);

            EmojiWindowNative.ShowEditBox(state.AddressEdit, 1);
            EmojiWindowNative.ShowLabel(state.AddressShellLabel, 1);
            EmojiWindowNative.ShowLabel(state.AddressBadgeLabel, 1);
            Win32Native.EnsureVisible(state.AddressEdit);
            Win32Native.RaiseChild(state.ToolbarPanel);
            Win32Native.RaiseChild(state.AddressShellLabel);
            Win32Native.RaiseChild(state.AddressBadgeLabel);
            Win32Native.RaiseChild(state.AddressEdit);
        }

        public static void SyncAddressBar(BrowserState state)
        {
            if (state.ActiveTab == null)
            {
                return;
            }

            string url = state.ActiveTab.CurrentUrl;
            ChromeControlFactory.SetEditText(state.AddressEdit, url);
            ChromeControlFactory.SetLabelText(state.AddressBadgeLabel, BrowserText.BadgeFromUrl(url));
            ChromeControlFactory.SetButtonText(state.FavoriteButtonId, NavigationService.IsCurrentFavorite(state) ? "*" : "*");
            ChromeControlFactory.SetButtonText(state.ThemeButtonId, state.DarkThemeEnabled ? "L" : "D");

            EmojiWindowNative.SetLabelColor(
                state.AddressBadgeLabel,
                ChromePalette.AddressBackground(state.DarkThemeEnabled),
                ChromePalette.AddressBadgeBackground(url));

            UpdateButtonStates(state);
            UpdateWindowTitle(state);
            Win32Native.EnsureVisible(state.AddressEdit);
            Win32Native.RaiseChild(state.AddressEdit);
        }

        public static void CommitAddressBar(BrowserState state)
        {
            if (state.ActiveTab == null || state.AddressEdit == IntPtr.Zero)
            {
                return;
            }

            string text = EmojiWindowNative.ReadUtf8(EmojiWindowNative.GetEditBoxText, state.AddressEdit);
            NavigationService.NavigateActiveTab(state, text);
            TabBarService.ApplyStyles(state);
            FavoriteMenuService.Rebuild(state);
            SyncAddressBar(state);
        }

        public static bool HandleToolbarButton(BrowserState state, int buttonId)
        {
            if (buttonId == state.BackButtonId)
            {
                bool changed = NavigationService.GoBack(state);
                ContentService.SetStatus(state, changed ? "已回退到上一条地址历史。" : "当前标签没有可回退的历史。");
                return true;
            }

            if (buttonId == state.ForwardButtonId)
            {
                bool changed = NavigationService.GoForward(state);
                ContentService.SetStatus(state, changed ? "已前进到下一条地址历史。" : "当前标签没有可前进的历史。");
                return true;
            }

            if (buttonId == state.ReloadButtonId)
            {
                NavigationService.Reload(state);
                ContentService.SetStatus(state, "已刷新当前标签信息。");
                return true;
            }

            if (buttonId == state.HomeButtonId)
            {
                NavigationService.GoHome(state);
                ContentService.SetStatus(state, "已回到主页。");
                return true;
            }

            if (buttonId == state.FavoriteButtonId)
            {
                bool added = NavigationService.ToggleFavorite(state);
                FavoriteMenuService.Rebuild(state);
                ContentService.SetStatus(state, added ? "已加入收藏。": "已从收藏中移除。");
                return true;
            }

            if (buttonId == state.ThemeButtonId)
            {
                ThemeService.Toggle(state);
                ContentService.SetStatus(state, state.DarkThemeEnabled ? "已切换到深色主题。" : "已切换到浅色主题。");
                return true;
            }

            return false;
        }

        public static void ApplyTheme(BrowserState state)
        {
            bool dark = state.DarkThemeEnabled;
            if (state.ToolbarPanel != IntPtr.Zero)
            {
                EmojiWindowNative.SetPanelBackgroundColor(state.ToolbarPanel, ChromePalette.ToolbarBackground(dark));
            }

            if (state.AddressShellLabel != IntPtr.Zero)
            {
                EmojiWindowNative.SetLabelColor(state.AddressShellLabel, ChromePalette.AddressShellBackground(dark), ChromePalette.AddressShellBackground(dark));
            }

            if (state.AddressEdit != IntPtr.Zero)
            {
                EmojiWindowNative.SetEditBoxColor(state.AddressEdit, ChromePalette.TextPrimary(dark), ChromePalette.AddressBackground(dark));
            }

            ApplyToolbarButtonStyle(state, state.BackButtonId, false);
            ApplyToolbarButtonStyle(state, state.ForwardButtonId, false);
            ApplyToolbarButtonStyle(state, state.ReloadButtonId, false);
            ApplyToolbarButtonStyle(state, state.HomeButtonId, false);
            ApplyToolbarButtonStyle(state, state.FavoriteButtonId, NavigationService.IsCurrentFavorite(state));
            ApplyToolbarButtonStyle(state, state.ThemeButtonId, state.DarkThemeEnabled);
        }

        private static int CreateToolbarButton(BrowserState state, string text, int x, int toolbarWidth)
        {
            int buttonId = ChromeControlFactory.CreateTextButton(
                state,
                text,
                ChromeLayoutMetrics.ToolbarX + x,
                ChromeLayoutMetrics.ToolbarY + ChromeLayoutMetrics.ToolbarInnerTop,
                ChromeLayoutMetrics.ToolbarButtonSize,
                ChromeLayoutMetrics.ToolbarButtonSize,
                ChromePalette.ToolbarButtonBackground(state.DarkThemeEnabled));
            ApplyToolbarButtonStyle(state, buttonId, false);
            return buttonId;
        }

        private static void LayoutToolbarButton(int buttonId, int x, int y)
        {
            EmojiWindowNative.SetButtonBounds(buttonId, x, y, ChromeLayoutMetrics.ToolbarButtonSize, ChromeLayoutMetrics.ToolbarButtonSize);
            EmojiWindowNative.ShowButton(buttonId, 1);
        }

        private static void UpdateButtonStates(BrowserState state)
        {
            ApplyToolbarButtonStyle(state, state.BackButtonId, false);
            ApplyToolbarButtonStyle(state, state.ForwardButtonId, false);
            ApplyToolbarButtonStyle(state, state.ReloadButtonId, false);
            ApplyToolbarButtonStyle(state, state.HomeButtonId, false);
            ApplyToolbarButtonStyle(state, state.FavoriteButtonId, NavigationService.IsCurrentFavorite(state));
            ApplyToolbarButtonStyle(state, state.ThemeButtonId, state.DarkThemeEnabled);

            if (state.ActiveTab == null)
            {
                return;
            }

            EmojiWindowNative.EnableButton(state.Window, state.BackButtonId, state.ActiveTab.CanGoBack ? 1 : 0);
            EmojiWindowNative.EnableButton(state.Window, state.ForwardButtonId, state.ActiveTab.CanGoForward ? 1 : 0);
            EmojiWindowNative.EnableButton(state.Window, state.ReloadButtonId, 1);
            EmojiWindowNative.EnableButton(state.Window, state.HomeButtonId, 1);
            EmojiWindowNative.EnableButton(state.Window, state.FavoriteButtonId, 1);
            EmojiWindowNative.EnableButton(state.Window, state.ThemeButtonId, 1);
        }

        private static void ApplyToolbarButtonStyle(BrowserState state, int buttonId, bool active)
        {
            if (buttonId == 0)
            {
                return;
            }

            bool dark = state.DarkThemeEnabled;
            EmojiWindowNative.SetButtonStyle(buttonId, 0);
            EmojiWindowNative.SetButtonRound(buttonId, 1);
            EmojiWindowNative.SetButtonBackgroundColor(buttonId, active ? ChromePalette.ToolbarButtonActive(dark) : ChromePalette.ToolbarButtonBackground(dark));
            EmojiWindowNative.SetButtonTextColor(buttonId, ChromePalette.TextSecondary(dark));
            EmojiWindowNative.SetButtonBorderColor(buttonId, ChromePalette.ToolbarButtonBorder(dark));
            EmojiWindowNative.SetButtonHoverColors(buttonId, ChromePalette.ToolbarButtonHover(dark), ChromePalette.ToolbarButtonBorder(dark), ChromePalette.TextPrimary(dark));
        }

        private static void UpdateWindowTitle(BrowserState state)
        {
            if (state.Window == IntPtr.Zero || state.ActiveTab == null)
            {
                return;
            }

            byte[] titleBytes = EmojiWindowNative.ToUtf8(state.ActiveTab.Title + " - Chrome风格多标签浏览器 C#");
            EmojiWindowNative.set_window_title(state.Window, titleBytes, titleBytes.Length);
        }
    }
}
