using System;

namespace EmojiWindowChromeStyleBrowserDemo.Styling
{
    internal static class ChromeLayoutMetrics
    {
        public const int WindowWidth = 1200;
        public const int WindowHeight = 780;

        public const int TabStripY = 30;
        public const int TabStripHeight = 44;
        public const int TabButtonX = 12;
        public const int TabButtonY = 36;
        public const int TabButtonWidth = 160;
        public const int TabButtonHeight = 32;
        public const int TabButtonPitch = 168;
        public const int NewTabButtonOffset = 8;

        public const int ToolbarX = 12;
        public const int ToolbarY = 84;
        public const int ToolbarHeight = 48;
        public const int ToolbarSideMargin = 12;
        public const int ToolbarInnerTop = 8;
        public const int ToolbarButtonStartX = 8;
        public const int ToolbarButtonSize = 32;
        public const int ToolbarButtonGap = 36;
        public const int ToolbarRightThemeOffset = 44;
        public const int ToolbarRightFavoriteOffset = 80;
        public const int AddressShellX = 160;
        public const int AddressShellY = 7;
        public const int AddressShellHeight = 34;
        public const int AddressBadgeOffsetX = 10;
        public const int AddressBadgeOffsetY = 10;
        public const int AddressBadgeSize = 26;
        public const int AddressEditOffsetX = 42;
        public const int AddressEditOffsetY = 10;
        public const int AddressBarMinWidth = 220;
        public const int AddressBarHeight = 28;

        public const int ContentMarginX = 32;
        public const int PageCardY = 164;
        public const int PageCardHeight = 132;
        public const int PageTitleInsetX = 24;
        public const int PageTitleInsetY = 20;
        public const int PageUrlInsetY = 62;
        public const int PageStateInsetY = 94;
        public const int InfoCardY = 316;
        public const int InfoCardHeight = 184;
        public const int InfoTitleInsetY = 20;
        public const int InfoLine1InsetY = 60;
        public const int InfoLineGap = 30;
        public const int FooterHeight = 34;
        public const int FooterInsetX = 16;
        public const int FooterInsetY = 7;

        public static int GetTabX(int visualIndex) => TabButtonX + (visualIndex * TabButtonPitch);

        public static int GetNewTabX(int visibleTabs) => TabButtonX + (visibleTabs * TabButtonPitch) + NewTabButtonOffset;

        public static int GetToolbarWidth(int windowWidth) => Math.Max(420, windowWidth - (ToolbarSideMargin * 2));

        public static int GetRightFavoriteX(int toolbarWidth) => Math.Max(0, toolbarWidth - ToolbarRightFavoriteOffset);

        public static int GetRightThemeX(int toolbarWidth) => Math.Max(0, toolbarWidth - ToolbarRightThemeOffset);

        public static int GetAddressShellWidth(int toolbarWidth)
        {
            return Math.Max(AddressBarMinWidth, GetRightFavoriteX(toolbarWidth) - AddressShellX - 12);
        }

        public static int GetAddressWidth(int windowWidth)
        {
            return Math.Max(140, GetAddressShellWidth(GetToolbarWidth(windowWidth)) - 52);
        }

        public static int GetContentWidth(int windowWidth) => Math.Max(640, windowWidth - (ContentMarginX * 2));

        public static int GetFooterWidth(int windowWidth) => Math.Max(200, windowWidth - (FooterInsetX * 2));
    }
}
