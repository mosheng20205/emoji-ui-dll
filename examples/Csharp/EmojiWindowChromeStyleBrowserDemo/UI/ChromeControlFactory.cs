using System;
using EmojiWindowChromeStyleBrowserDemo.Core;
using EmojiWindowChromeStyleBrowserDemo.Styling;
using EmojiWindowDemo;

namespace EmojiWindowChromeStyleBrowserDemo.UI
{
    internal static class ChromeControlFactory
    {
        public static int CreateTextButton(BrowserState state, string text, int x, int y, int width, int height, uint background)
        {
            byte[] textBytes = EmojiWindowNative.ToUtf8(text);
            return EmojiWindowNative.create_emoji_button_bytes(
                state.Window,
                state.EmptyBytes,
                0,
                textBytes,
                textBytes.Length,
                x,
                y,
                width,
                height,
                background);
        }

        public static int CreateEmojiButton(BrowserState state, string emoji, int x, int y, int width, int height, uint background)
        {
            byte[] emojiBytes = EmojiWindowNative.ToUtf8(emoji);
            return EmojiWindowNative.create_emoji_button_bytes(
                state.Window,
                emojiBytes,
                emojiBytes.Length,
                state.EmptyBytes,
                0,
                x,
                y,
                width,
                height,
                background);
        }

        public static IntPtr CreateLabel(BrowserState state, int x, int y, int width, int height, string text, uint foreground, uint background, int fontSize = 12, int alignment = 0, IntPtr? parent = null)
        {
            byte[] textBytes = EmojiWindowNative.ToUtf8(text);
            return EmojiWindowNative.CreateLabel(
                parent ?? state.Window,
                x,
                y,
                width,
                height,
                textBytes,
                textBytes.Length,
                foreground,
                background,
                state.FontBytes,
                state.FontBytes.Length,
                fontSize,
                0,
                0,
                0,
                alignment,
                0);
        }

        public static IntPtr CreateAddressEditBox(BrowserState state, string text, IntPtr? parent = null)
        {
            bool dark = state.DarkThemeEnabled;
            byte[] textBytes = EmojiWindowNative.ToUtf8(text);
            return EmojiWindowNative.CreateD2DColorEmojiEditBox(
                parent ?? state.Window,
                ChromeLayoutMetrics.AddressShellX + ChromeLayoutMetrics.AddressEditOffsetX,
                ChromeLayoutMetrics.AddressEditOffsetY,
                ChromeLayoutMetrics.GetAddressWidth(state.Width),
                ChromeLayoutMetrics.AddressBarHeight,
                textBytes,
                textBytes.Length,
                ChromePalette.TextPrimary(dark),
                ChromePalette.AddressBackground(dark),
                state.FontBytes,
                state.FontBytes.Length,
                13,
                0,
                0,
                0,
                0,
                0,
                0,
                0,
                1,
                1);
        }

        public static IntPtr CreateToolbarPanel(BrowserState state)
        {
            return EmojiWindowNative.CreatePanel(
                state.Window,
                ChromeLayoutMetrics.ToolbarX,
                ChromeLayoutMetrics.ToolbarY,
                ChromeLayoutMetrics.GetToolbarWidth(state.Width),
                ChromeLayoutMetrics.ToolbarHeight,
                ChromePalette.ToolbarBackground(state.DarkThemeEnabled));
        }

        public static void SetLabelText(IntPtr handle, string text)
        {
            if (handle == IntPtr.Zero)
            {
                return;
            }

            byte[] bytes = EmojiWindowNative.ToUtf8(text);
            EmojiWindowNative.SetLabelText(handle, bytes, bytes.Length);
        }

        public static void SetButtonText(int buttonId, string text)
        {
            if (buttonId == 0)
            {
                return;
            }

            byte[] bytes = EmojiWindowNative.ToUtf8(text);
            EmojiWindowNative.SetButtonText(buttonId, bytes, bytes.Length);
        }

        public static void SetButtonEmoji(int buttonId, string emoji)
        {
            if (buttonId == 0)
            {
                return;
            }

            byte[] bytes = EmojiWindowNative.ToUtf8(emoji);
            EmojiWindowNative.SetButtonEmoji(buttonId, bytes, bytes.Length);
        }

        public static void SetEditText(IntPtr handle, string text)
        {
            if (handle == IntPtr.Zero)
            {
                return;
            }

            byte[] bytes = EmojiWindowNative.ToUtf8(text);
            EmojiWindowNative.SetEditBoxText(handle, bytes, bytes.Length);
        }

        public static void AddMenuItem(IntPtr menu, string text, int itemId)
        {
            if (menu == IntPtr.Zero)
            {
                return;
            }

            byte[] bytes = EmojiWindowNative.ToUtf8(text);
            EmojiWindowNative.PopupMenuAddItem(menu, bytes, bytes.Length, itemId);
        }
    }
}
