using EmojiWindowChromeStyleBrowserDemo.Core;
using EmojiWindowChromeStyleBrowserDemo.Models;

namespace EmojiWindowChromeStyleBrowserDemo.Services
{
    internal static class NavigationService
    {
        public static BrowserTab AddTab(BrowserState state, string url, bool activate)
        {
            string normalizedUrl = BrowserText.NormalizeUrl(url);
            var tab = TabBarService.AddTab(state, normalizedUrl, activate);
            tab.HomeUrl = "https://www.google.com/";
            tab.History.Clear();
            tab.History.Add(normalizedUrl);
            tab.HistoryIndex = 0;
            tab.Title = BrowserText.TitleFromUrl(normalizedUrl);
            return tab;
        }

        public static void NavigateActiveTab(BrowserState state, string url)
        {
            NavigateTo(state, state.ActiveTab, url, true);
        }

        public static void NavigateTo(BrowserState state, BrowserTab tab, string url, bool addHistory)
        {
            if (tab == null)
            {
                return;
            }

            string normalizedUrl = BrowserText.NormalizeUrl(url);
            if (addHistory)
            {
                if (tab.HistoryIndex >= 0 && tab.HistoryIndex < tab.History.Count - 1)
                {
                    tab.History.RemoveRange(tab.HistoryIndex + 1, tab.History.Count - tab.HistoryIndex - 1);
                }

                tab.History.Add(normalizedUrl);
                tab.HistoryIndex = tab.History.Count - 1;
            }
            else if (tab.HistoryIndex >= 0 && tab.HistoryIndex < tab.History.Count)
            {
                tab.History[tab.HistoryIndex] = normalizedUrl;
            }
            else
            {
                tab.History.Add(normalizedUrl);
                tab.HistoryIndex = tab.History.Count - 1;
            }

            tab.Title = BrowserText.TitleFromUrl(normalizedUrl);
        }

        public static bool GoBack(BrowserState state)
        {
            BrowserTab tab = state.ActiveTab;
            if (tab == null || !tab.CanGoBack)
            {
                return false;
            }

            tab.HistoryIndex--;
            tab.Title = BrowserText.TitleFromUrl(tab.CurrentUrl);
            return true;
        }

        public static bool GoForward(BrowserState state)
        {
            BrowserTab tab = state.ActiveTab;
            if (tab == null || !tab.CanGoForward)
            {
                return false;
            }

            tab.HistoryIndex++;
            tab.Title = BrowserText.TitleFromUrl(tab.CurrentUrl);
            return true;
        }

        public static bool GoHome(BrowserState state)
        {
            BrowserTab tab = state.ActiveTab;
            if (tab == null)
            {
                return false;
            }

            NavigateTo(state, tab, tab.HomeUrl ?? "https://www.google.com/", true);
            return true;
        }

        public static bool Reload(BrowserState state)
        {
            BrowserTab tab = state.ActiveTab;
            if (tab == null)
            {
                return false;
            }

            tab.Title = BrowserText.TitleFromUrl(tab.CurrentUrl);
            return true;
        }

        public static bool ToggleFavorite(BrowserState state)
        {
            BrowserTab tab = state.ActiveTab;
            if (tab == null)
            {
                return false;
            }

            string url = tab.CurrentUrl;
            int index = state.Favorites.IndexOf(url);
            if (index >= 0)
            {
                state.Favorites.RemoveAt(index);
                return false;
            }

            state.Favorites.Add(url);
            return true;
        }

        public static bool IsCurrentFavorite(BrowserState state)
        {
            return state.ActiveTab != null && state.Favorites.Contains(state.ActiveTab.CurrentUrl);
        }
    }
}
