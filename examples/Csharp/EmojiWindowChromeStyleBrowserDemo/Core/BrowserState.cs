using System;
using System.Collections.Generic;
using EmojiWindowChromeStyleBrowserDemo.Models;
using EmojiWindowDemo;

namespace EmojiWindowChromeStyleBrowserDemo.Core
{
    internal sealed class BrowserState
    {
        public IntPtr Window;
        public int Width;
        public int Height;

        public IntPtr TabStripBackground;
        public IntPtr ToolbarPanel;
        public IntPtr AddressShellLabel;
        public IntPtr AddressBadgeLabel;
        public int NewTabButtonId;
        public int BackButtonId;
        public int ForwardButtonId;
        public int ReloadButtonId;
        public int HomeButtonId;
        public int FavoriteButtonId;
        public int ThemeButtonId;
        public bool DarkThemeEnabled;

        public IntPtr AddressEdit;

        public IntPtr FooterBackgroundLabel;
        public IntPtr PageCardLabel;
        public IntPtr PageTitleLabel;
        public IntPtr PageUrlLabel;
        public IntPtr PageStateLabel;
        public IntPtr InfoCardLabel;
        public IntPtr InfoTitleLabel;
        public IntPtr InfoLine1Label;
        public IntPtr InfoLine2Label;
        public IntPtr InfoLine3Label;
        public IntPtr InfoLine4Label;
        public IntPtr FooterStatusLabel;

        public readonly List<BrowserTab> Tabs = new List<BrowserTab>();
        public readonly List<string> Favorites = new List<string>();
        public int ActiveTabIndex = -1;

        public readonly byte[] FontBytes = EmojiWindowNative.ToUtf8("Microsoft YaHei UI");
        public readonly byte[] EmptyBytes = Array.Empty<byte>();

        public EmojiWindowNative.ButtonClickCallback ButtonClickCallback;
        public EmojiWindowNative.MenuItemClickCallback MenuItemClickCallback;
        public EmojiWindowNative.EditBoxKeyCallback EditBoxKeyCallback;
        public EmojiWindowNative.WindowResizeCallback WindowResizeCallback;
        public EmojiWindowNative.WindowCloseCallback WindowCloseCallback;
        public IntPtr FavoriteMenu;

        public BrowserTab ActiveTab
        {
            get
            {
                if (ActiveTabIndex < 0 || ActiveTabIndex >= Tabs.Count)
                {
                    return null;
                }

                return Tabs[ActiveTabIndex];
            }
        }
    }
}
