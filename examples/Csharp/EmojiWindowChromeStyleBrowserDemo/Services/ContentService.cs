using EmojiWindowChromeStyleBrowserDemo.Core;
using EmojiWindowChromeStyleBrowserDemo.Styling;
using EmojiWindowChromeStyleBrowserDemo.UI;
using EmojiWindowDemo;

namespace EmojiWindowChromeStyleBrowserDemo.Services
{
    internal static class ContentService
    {
        public static void CreateSharedControls(BrowserState state)
        {
            bool dark = state.DarkThemeEnabled;
            int contentWidth = ChromeLayoutMetrics.GetContentWidth(state.Width);
            uint windowBg = ChromePalette.WindowBackground(dark);

            state.PageCardLabel = ChromeControlFactory.CreateLabel(state, ChromeLayoutMetrics.ContentMarginX, ChromeLayoutMetrics.PageCardY, contentWidth, ChromeLayoutMetrics.PageCardHeight, string.Empty, ChromePalette.CardBackground(dark), ChromePalette.CardBackground(dark));
            state.PageTitleLabel = ChromeControlFactory.CreateLabel(state, ChromeLayoutMetrics.ContentMarginX + ChromeLayoutMetrics.PageTitleInsetX, ChromeLayoutMetrics.PageCardY + ChromeLayoutMetrics.PageTitleInsetY, contentWidth - 48, 34, string.Empty, ChromePalette.TextPrimary(dark), ChromePalette.CardBackground(dark), 22);
            state.PageUrlLabel = ChromeControlFactory.CreateLabel(state, ChromeLayoutMetrics.ContentMarginX + ChromeLayoutMetrics.PageTitleInsetX, ChromeLayoutMetrics.PageCardY + ChromeLayoutMetrics.PageUrlInsetY, contentWidth - 48, 24, string.Empty, ChromePalette.TextSecondary(dark), ChromePalette.CardBackground(dark));
            state.PageStateLabel = ChromeControlFactory.CreateLabel(state, ChromeLayoutMetrics.ContentMarginX + ChromeLayoutMetrics.PageTitleInsetX, ChromeLayoutMetrics.PageCardY + ChromeLayoutMetrics.PageStateInsetY, contentWidth - 48, 24, string.Empty, ChromePalette.TextSecondary(dark), ChromePalette.CardBackground(dark));

            state.InfoCardLabel = ChromeControlFactory.CreateLabel(state, ChromeLayoutMetrics.ContentMarginX, ChromeLayoutMetrics.InfoCardY, contentWidth, ChromeLayoutMetrics.InfoCardHeight, string.Empty, ChromePalette.CardAltBackground(dark), ChromePalette.CardAltBackground(dark));
            state.InfoTitleLabel = ChromeControlFactory.CreateLabel(state, ChromeLayoutMetrics.ContentMarginX + ChromeLayoutMetrics.PageTitleInsetX, ChromeLayoutMetrics.InfoCardY + ChromeLayoutMetrics.InfoTitleInsetY, contentWidth - 48, 28, "Tab Session", ChromePalette.TextPrimary(dark), ChromePalette.CardAltBackground(dark), 16);
            state.InfoLine1Label = ChromeControlFactory.CreateLabel(state, ChromeLayoutMetrics.ContentMarginX + ChromeLayoutMetrics.PageTitleInsetX, ChromeLayoutMetrics.InfoCardY + ChromeLayoutMetrics.InfoLine1InsetY, contentWidth - 48, 22, string.Empty, ChromePalette.TextSecondary(dark), ChromePalette.CardAltBackground(dark));
            state.InfoLine2Label = ChromeControlFactory.CreateLabel(state, ChromeLayoutMetrics.ContentMarginX + ChromeLayoutMetrics.PageTitleInsetX, ChromeLayoutMetrics.InfoCardY + ChromeLayoutMetrics.InfoLine1InsetY + ChromeLayoutMetrics.InfoLineGap, contentWidth - 48, 22, string.Empty, ChromePalette.TextSecondary(dark), ChromePalette.CardAltBackground(dark));
            state.InfoLine3Label = ChromeControlFactory.CreateLabel(state, ChromeLayoutMetrics.ContentMarginX + ChromeLayoutMetrics.PageTitleInsetX, ChromeLayoutMetrics.InfoCardY + ChromeLayoutMetrics.InfoLine1InsetY + (ChromeLayoutMetrics.InfoLineGap * 2), contentWidth - 48, 22, string.Empty, ChromePalette.TextSecondary(dark), ChromePalette.CardAltBackground(dark));
            state.InfoLine4Label = ChromeControlFactory.CreateLabel(state, ChromeLayoutMetrics.ContentMarginX + ChromeLayoutMetrics.PageTitleInsetX, ChromeLayoutMetrics.InfoCardY + ChromeLayoutMetrics.InfoLine1InsetY + (ChromeLayoutMetrics.InfoLineGap * 3), contentWidth - 48, 22, string.Empty, ChromePalette.TextSecondary(dark), ChromePalette.CardAltBackground(dark));

            state.FooterBackgroundLabel = ChromeControlFactory.CreateLabel(state, 0, state.Height - ChromeLayoutMetrics.FooterHeight, state.Width, ChromeLayoutMetrics.FooterHeight, string.Empty, ChromePalette.FooterBackground(dark), ChromePalette.FooterBackground(dark));
            state.FooterStatusLabel = ChromeControlFactory.CreateLabel(state, ChromeLayoutMetrics.FooterInsetX, state.Height - ChromeLayoutMetrics.FooterHeight + ChromeLayoutMetrics.FooterInsetY, ChromeLayoutMetrics.GetFooterWidth(state.Width), 20, string.Empty, ChromePalette.FooterText(dark), ChromePalette.FooterBackground(dark));

            EmojiWindowNative.SetLabelColor(state.PageCardLabel, ChromePalette.CardBackground(dark), ChromePalette.CardBackground(dark));
            EmojiWindowNative.SetLabelColor(state.InfoCardLabel, ChromePalette.CardAltBackground(dark), ChromePalette.CardAltBackground(dark));
        }

        public static void Layout(BrowserState state)
        {
            int contentWidth = ChromeLayoutMetrics.GetContentWidth(state.Width);
            EmojiWindowNative.SetLabelBounds(state.PageCardLabel, ChromeLayoutMetrics.ContentMarginX, ChromeLayoutMetrics.PageCardY, contentWidth, ChromeLayoutMetrics.PageCardHeight);
            EmojiWindowNative.SetLabelBounds(state.PageTitleLabel, ChromeLayoutMetrics.ContentMarginX + ChromeLayoutMetrics.PageTitleInsetX, ChromeLayoutMetrics.PageCardY + ChromeLayoutMetrics.PageTitleInsetY, contentWidth - 48, 34);
            EmojiWindowNative.SetLabelBounds(state.PageUrlLabel, ChromeLayoutMetrics.ContentMarginX + ChromeLayoutMetrics.PageTitleInsetX, ChromeLayoutMetrics.PageCardY + ChromeLayoutMetrics.PageUrlInsetY, contentWidth - 48, 24);
            EmojiWindowNative.SetLabelBounds(state.PageStateLabel, ChromeLayoutMetrics.ContentMarginX + ChromeLayoutMetrics.PageTitleInsetX, ChromeLayoutMetrics.PageCardY + ChromeLayoutMetrics.PageStateInsetY, contentWidth - 48, 24);

            EmojiWindowNative.SetLabelBounds(state.InfoCardLabel, ChromeLayoutMetrics.ContentMarginX, ChromeLayoutMetrics.InfoCardY, contentWidth, ChromeLayoutMetrics.InfoCardHeight);
            EmojiWindowNative.SetLabelBounds(state.InfoTitleLabel, ChromeLayoutMetrics.ContentMarginX + ChromeLayoutMetrics.PageTitleInsetX, ChromeLayoutMetrics.InfoCardY + ChromeLayoutMetrics.InfoTitleInsetY, contentWidth - 48, 28);
            EmojiWindowNative.SetLabelBounds(state.InfoLine1Label, ChromeLayoutMetrics.ContentMarginX + ChromeLayoutMetrics.PageTitleInsetX, ChromeLayoutMetrics.InfoCardY + ChromeLayoutMetrics.InfoLine1InsetY, contentWidth - 48, 22);
            EmojiWindowNative.SetLabelBounds(state.InfoLine2Label, ChromeLayoutMetrics.ContentMarginX + ChromeLayoutMetrics.PageTitleInsetX, ChromeLayoutMetrics.InfoCardY + ChromeLayoutMetrics.InfoLine1InsetY + ChromeLayoutMetrics.InfoLineGap, contentWidth - 48, 22);
            EmojiWindowNative.SetLabelBounds(state.InfoLine3Label, ChromeLayoutMetrics.ContentMarginX + ChromeLayoutMetrics.PageTitleInsetX, ChromeLayoutMetrics.InfoCardY + ChromeLayoutMetrics.InfoLine1InsetY + (ChromeLayoutMetrics.InfoLineGap * 2), contentWidth - 48, 22);
            EmojiWindowNative.SetLabelBounds(state.InfoLine4Label, ChromeLayoutMetrics.ContentMarginX + ChromeLayoutMetrics.PageTitleInsetX, ChromeLayoutMetrics.InfoCardY + ChromeLayoutMetrics.InfoLine1InsetY + (ChromeLayoutMetrics.InfoLineGap * 3), contentWidth - 48, 22);

            EmojiWindowNative.SetLabelBounds(state.FooterBackgroundLabel, 0, state.Height - ChromeLayoutMetrics.FooterHeight, state.Width, ChromeLayoutMetrics.FooterHeight);
            EmojiWindowNative.SetLabelBounds(state.FooterStatusLabel, ChromeLayoutMetrics.FooterInsetX, state.Height - ChromeLayoutMetrics.FooterHeight + ChromeLayoutMetrics.FooterInsetY, ChromeLayoutMetrics.GetFooterWidth(state.Width), 20);
        }

        public static void SyncFromActiveTab(BrowserState state)
        {
            if (state.ActiveTab == null)
            {
                return;
            }

            string url = state.ActiveTab.CurrentUrl;
            ChromeControlFactory.SetLabelText(state.PageTitleLabel, state.ActiveTab.Title);
            ChromeControlFactory.SetLabelText(state.PageUrlLabel, BrowserText.ShortUrl(url, 88));
            ChromeControlFactory.SetLabelText(state.PageStateLabel, (NavigationService.IsCurrentFavorite(state) ? "Bookmarked" : "Not bookmarked") + "  |  Theme " + (state.DarkThemeEnabled ? "Dark" : "Light"));
            ChromeControlFactory.SetLabelText(state.InfoLine1Label, "Current URL: " + url);
            ChromeControlFactory.SetLabelText(state.InfoLine2Label, "History: " + (state.ActiveTab.HistoryIndex + 1) + "/" + state.ActiveTab.History.Count);
            ChromeControlFactory.SetLabelText(state.InfoLine3Label, "Home: " + state.ActiveTab.HomeUrl);
            ChromeControlFactory.SetLabelText(state.InfoLine4Label, "Favorites: " + state.Favorites.Count + " saved");
        }

        public static void RefreshThemeSummary(BrowserState state)
        {
            if (state.ActiveTab == null)
            {
                return;
            }

            ChromeControlFactory.SetLabelText(
                state.PageStateLabel,
                (NavigationService.IsCurrentFavorite(state) ? "Bookmarked" : "Not bookmarked") + "  |  Theme " + (state.DarkThemeEnabled ? "Dark" : "Light"));
        }

        public static void SetStatus(BrowserState state, string text)
        {
            ChromeControlFactory.SetLabelText(state.FooterStatusLabel, text);
        }

        public static void ApplyTheme(BrowserState state)
        {
            bool dark = state.DarkThemeEnabled;
            uint windowBg = ChromePalette.WindowBackground(dark);
            uint cardBg = ChromePalette.CardBackground(dark);
            uint altBg = ChromePalette.CardAltBackground(dark);
            uint textPrimary = ChromePalette.TextPrimary(dark);
            uint textSecondary = ChromePalette.TextSecondary(dark);
            uint footerBg = ChromePalette.FooterBackground(dark);
            uint footerText = ChromePalette.FooterText(dark);

            EmojiWindowNative.SetLabelColor(state.PageCardLabel, cardBg, cardBg);
            EmojiWindowNative.SetLabelColor(state.PageTitleLabel, textPrimary, cardBg);
            EmojiWindowNative.SetLabelColor(state.PageUrlLabel, textSecondary, cardBg);
            EmojiWindowNative.SetLabelColor(state.PageStateLabel, textSecondary, cardBg);
            EmojiWindowNative.SetLabelColor(state.InfoCardLabel, altBg, altBg);
            EmojiWindowNative.SetLabelColor(state.InfoTitleLabel, textPrimary, altBg);
            EmojiWindowNative.SetLabelColor(state.InfoLine1Label, textSecondary, altBg);
            EmojiWindowNative.SetLabelColor(state.InfoLine2Label, textSecondary, altBg);
            EmojiWindowNative.SetLabelColor(state.InfoLine3Label, textSecondary, altBg);
            EmojiWindowNative.SetLabelColor(state.InfoLine4Label, textSecondary, altBg);
            EmojiWindowNative.SetLabelColor(state.FooterBackgroundLabel, footerBg, footerBg);
            EmojiWindowNative.SetLabelColor(state.FooterStatusLabel, footerText, footerBg);
        }
    }
}
