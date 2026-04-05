using EmojiWindowChromeStyleBrowserDemo.Core;
using EmojiWindowChromeStyleBrowserDemo.Styling;
using EmojiWindowDemo;

namespace EmojiWindowChromeStyleBrowserDemo.Services
{
    internal static class ThemeService
    {
        public static void Apply(BrowserState state)
        {
            bool dark = state.DarkThemeEnabled;
            EmojiWindowNative.SetDarkMode(dark ? 1 : 0);
            EmojiWindowNative.set_window_titlebar_color(state.Window, ChromePalette.TitleBarBackground(dark));
            EmojiWindowNative.SetWindowBackgroundColor(state.Window, ChromePalette.WindowBackground(dark));
            EmojiWindowNative.SetTitleBarTextColor(state.Window, ChromePalette.TextPrimary(dark));

            TabBarService.ApplyTheme(state);
            ToolbarService.ApplyTheme(state);
            ContentService.ApplyTheme(state);
        }

        public static void Toggle(BrowserState state)
        {
            state.DarkThemeEnabled = !state.DarkThemeEnabled;
            Apply(state);
        }
    }
}
