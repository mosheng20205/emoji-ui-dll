using System;
using EmojiWindowChromeStyleBrowserDemo.Core;
using EmojiWindowChromeStyleBrowserDemo.UI;
using EmojiWindowDemo;

namespace EmojiWindowChromeStyleBrowserDemo.Services
{
    internal static class FavoriteMenuService
    {
        private const int MenuFavoriteToggle = 2001;
        private const int MenuFavoriteHome = 2002;
        private const int MenuFavoriteEmpty = 2003;
        private const int MenuFavoriteBase = 2100;

        public static void Rebuild(BrowserState state)
        {
            if (state.Window == IntPtr.Zero || state.FavoriteButtonId == 0)
            {
                return;
            }

            state.FavoriteMenu = EmojiWindowNative.CreateEmojiPopupMenu(state.Window);
            if (state.FavoriteMenu == IntPtr.Zero)
            {
                return;
            }

            EmojiWindowNative.SetPopupMenuCallback(state.FavoriteMenu, state.MenuItemClickCallback);
            ChromeControlFactory.AddMenuItem(state.FavoriteMenu, "Toggle Current Favorite", MenuFavoriteToggle);
            ChromeControlFactory.AddMenuItem(state.FavoriteMenu, "Open Home", MenuFavoriteHome);

            if (state.Favorites.Count == 0)
            {
                ChromeControlFactory.AddMenuItem(state.FavoriteMenu, "No Favorites Yet", MenuFavoriteEmpty);
            }
            else
            {
                for (int i = 0; i < state.Favorites.Count && i < 30; i++)
                {
                    ChromeControlFactory.AddMenuItem(state.FavoriteMenu, BrowserText.ShortUrl(state.Favorites[i], 46), MenuFavoriteBase + i);
                }
            }

            EmojiWindowNative.BindButtonMenu(state.Window, state.FavoriteButtonId, state.FavoriteMenu);
        }

        public static bool HandleMenuClick(BrowserState state, int itemId)
        {
            if (itemId == MenuFavoriteToggle)
            {
                NavigationService.ToggleFavorite(state);
                Rebuild(state);
                return true;
            }

            if (itemId == MenuFavoriteHome)
            {
                NavigationService.GoHome(state);
                return true;
            }

            if (itemId == MenuFavoriteEmpty)
            {
                return true;
            }

            if (itemId >= MenuFavoriteBase)
            {
                int index = itemId - MenuFavoriteBase;
                if (index >= 0 && index < state.Favorites.Count)
                {
                    NavigationService.NavigateActiveTab(state, state.Favorites[index]);
                    return true;
                }
            }

            return false;
        }
    }
}
