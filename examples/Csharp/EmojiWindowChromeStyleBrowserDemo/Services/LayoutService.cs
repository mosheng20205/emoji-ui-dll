using EmojiWindowChromeStyleBrowserDemo.Core;
using EmojiWindowChromeStyleBrowserDemo.UI;

namespace EmojiWindowChromeStyleBrowserDemo.Services
{
    internal static class LayoutService
    {
        public static void ApplyAll(BrowserState state)
        {
            TabBarService.Layout(state);
            ToolbarService.Layout(state);
            ContentService.Layout(state);

            Win32Native.RaiseChild(state.FooterBackgroundLabel);
            Win32Native.RaiseChild(state.FooterStatusLabel);
        }
    }
}
