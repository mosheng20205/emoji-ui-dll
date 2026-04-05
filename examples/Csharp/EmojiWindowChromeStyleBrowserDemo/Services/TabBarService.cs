using System;
using EmojiWindowChromeStyleBrowserDemo.Core;
using EmojiWindowChromeStyleBrowserDemo.Models;
using EmojiWindowChromeStyleBrowserDemo.Styling;
using EmojiWindowChromeStyleBrowserDemo.UI;
using EmojiWindowDemo;

namespace EmojiWindowChromeStyleBrowserDemo.Services
{
    internal static class TabBarService
    {
        public static void CreateSharedControls(BrowserState state)
        {
            bool dark = state.DarkThemeEnabled;
            state.TabStripBackground = ChromeControlFactory.CreateLabel(
                state,
                0,
                ChromeLayoutMetrics.TabStripY,
                state.Width,
                ChromeLayoutMetrics.TabStripHeight,
                string.Empty,
                ChromePalette.TabStripBackground(dark),
                ChromePalette.TabStripBackground(dark));

            state.NewTabButtonId = ChromeControlFactory.CreateTextButton(
                state,
                "+",
                ChromeLayoutMetrics.TabButtonX,
                ChromeLayoutMetrics.TabButtonY,
                ChromeLayoutMetrics.ToolbarButtonSize,
                ChromeLayoutMetrics.ToolbarButtonSize,
                ChromePalette.TabInactiveBackground(dark));

            ApplySmallButtonStyle(state, state.NewTabButtonId);
        }

        public static BrowserTab AddTab(BrowserState state, string url, bool activate)
        {
            var tab = new BrowserTab
            {
                Title = BrowserText.TitleFromUrl(url),
                HomeUrl = "https://www.google.com/"
            };

            tab.ButtonId = ChromeControlFactory.CreateTextButton(
                state,
                BrowserText.ChromeTabCaption(url),
                ChromeLayoutMetrics.GetTabX(state.Tabs.Count),
                ChromeLayoutMetrics.TabButtonY,
                ChromeLayoutMetrics.TabButtonWidth,
                ChromeLayoutMetrics.TabButtonHeight,
                ChromePalette.TabInactiveBackground(state.DarkThemeEnabled));

            state.Tabs.Add(tab);
            if (activate)
            {
                state.ActiveTabIndex = state.Tabs.Count - 1;
            }

            Layout(state);
            ApplyStyles(state);
            return tab;
        }

        public static bool TryActivateByButton(BrowserState state, int buttonId)
        {
            for (int i = 0; i < state.Tabs.Count; i++)
            {
                if (state.Tabs[i].ButtonId == buttonId)
                {
                    state.ActiveTabIndex = i;
                    ApplyStyles(state);
                    return true;
                }
            }

            return false;
        }

        public static void Layout(BrowserState state)
        {
            if (state.TabStripBackground != IntPtr.Zero)
            {
                EmojiWindowNative.SetLabelBounds(state.TabStripBackground, 0, ChromeLayoutMetrics.TabStripY, state.Width, ChromeLayoutMetrics.TabStripHeight);
                EmojiWindowNative.ShowLabel(state.TabStripBackground, 1);
            }

            for (int i = 0; i < state.Tabs.Count; i++)
            {
                EmojiWindowNative.SetButtonBounds(
                    state.Tabs[i].ButtonId,
                    ChromeLayoutMetrics.GetTabX(i),
                    ChromeLayoutMetrics.TabButtonY,
                    ChromeLayoutMetrics.TabButtonWidth,
                    ChromeLayoutMetrics.TabButtonHeight);
                EmojiWindowNative.ShowButton(state.Tabs[i].ButtonId, 1);
            }

            if (state.NewTabButtonId != 0)
            {
                EmojiWindowNative.SetButtonBounds(
                    state.NewTabButtonId,
                    ChromeLayoutMetrics.GetNewTabX(state.Tabs.Count),
                    ChromeLayoutMetrics.TabButtonY,
                    ChromeLayoutMetrics.ToolbarButtonSize,
                    ChromeLayoutMetrics.ToolbarButtonSize);
                EmojiWindowNative.ShowButton(state.NewTabButtonId, 1);
            }
        }

        public static void ApplyStyles(BrowserState state)
        {
            bool dark = state.DarkThemeEnabled;
            for (int i = 0; i < state.Tabs.Count; i++)
            {
                BrowserTab tab = state.Tabs[i];
                bool active = i == state.ActiveTabIndex;
                uint background = active ? ChromePalette.TabActiveBackground(dark) : ChromePalette.TabInactiveBackground(dark);
                uint text = active ? ChromePalette.TextPrimary(dark) : ChromePalette.Accent(dark);
                uint hover = active ? ChromePalette.TabActiveBackground(dark) : ChromePalette.TabHoverBackground(dark);

                EmojiWindowNative.SetButtonBackgroundColor(tab.ButtonId, background);
                EmojiWindowNative.SetButtonTextColor(tab.ButtonId, text);
                EmojiWindowNative.SetButtonBorderColor(tab.ButtonId, background);
                EmojiWindowNative.SetButtonHoverColors(tab.ButtonId, hover, hover, text);
                ChromeControlFactory.SetButtonText(tab.ButtonId, BrowserText.ChromeTabCaption(tab.CurrentUrl));
            }

            ApplySmallButtonStyle(state, state.NewTabButtonId);
        }

        public static void ApplyTheme(BrowserState state)
        {
            bool dark = state.DarkThemeEnabled;
            if (state.TabStripBackground != IntPtr.Zero)
            {
                EmojiWindowNative.SetLabelColor(state.TabStripBackground, ChromePalette.TabStripBackground(dark), ChromePalette.TabStripBackground(dark));
            }

            ApplyStyles(state);
        }

        private static void ApplySmallButtonStyle(BrowserState state, int buttonId)
        {
            if (buttonId == 0)
            {
                return;
            }

            bool dark = state.DarkThemeEnabled;
            EmojiWindowNative.SetButtonBackgroundColor(buttonId, ChromePalette.TabInactiveBackground(dark));
            EmojiWindowNative.SetButtonTextColor(buttonId, ChromePalette.TextPrimary(dark));
            EmojiWindowNative.SetButtonBorderColor(buttonId, ChromePalette.TabInactiveBackground(dark));
            EmojiWindowNative.SetButtonHoverColors(buttonId, ChromePalette.TabHoverBackground(dark), ChromePalette.TabHoverBackground(dark), ChromePalette.TextPrimary(dark));
        }
    }
}
