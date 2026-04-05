using EmojiWindowDemo;

namespace EmojiWindowChromeStyleBrowserDemo.Styling
{
    internal static class ChromePalette
    {
        public static uint AccentBlue => EmojiWindowNative.ARGB(255, 66, 133, 244);

        public static uint TitleBarBackground(bool dark) => dark ? EmojiWindowNative.ARGB(255, 34, 37, 41) : EmojiWindowNative.ARGB(255, 240, 243, 247);

        public static uint WindowBackground(bool dark) => dark ? EmojiWindowNative.ARGB(255, 24, 27, 31) : EmojiWindowNative.ARGB(255, 244, 246, 249);

        public static uint TextPrimary(bool dark) => dark ? EmojiWindowNative.ARGB(255, 243, 246, 251) : EmojiWindowNative.ARGB(255, 32, 33, 36);

        public static uint TextSecondary(bool dark) => dark ? EmojiWindowNative.ARGB(255, 170, 177, 188) : EmojiWindowNative.ARGB(255, 95, 99, 104);

        public static uint Accent(bool dark) => dark ? EmojiWindowNative.ARGB(255, 138, 180, 248) : AccentBlue;

        public static uint TabStripBackground(bool dark) => dark ? EmojiWindowNative.ARGB(255, 34, 37, 41) : EmojiWindowNative.ARGB(255, 236, 240, 246);

        public static uint TabActiveBackground(bool dark) => dark ? EmojiWindowNative.ARGB(255, 56, 61, 68) : EmojiWindowNative.ARGB(255, 255, 255, 255);

        public static uint TabInactiveBackground(bool dark) => dark ? EmojiWindowNative.ARGB(255, 42, 46, 52) : EmojiWindowNative.ARGB(255, 229, 235, 243);

        public static uint TabHoverBackground(bool dark) => dark ? EmojiWindowNative.ARGB(255, 66, 72, 80) : EmojiWindowNative.ARGB(255, 218, 226, 237);

        public static uint ToolbarBackground(bool dark) => dark ? EmojiWindowNative.ARGB(255, 56, 61, 68) : EmojiWindowNative.ARGB(255, 252, 253, 255);

        public static uint ToolbarButtonBackground(bool dark) => dark ? EmojiWindowNative.ARGB(255, 56, 61, 68) : EmojiWindowNative.ARGB(255, 244, 247, 252);

        public static uint ToolbarButtonHover(bool dark) => dark ? EmojiWindowNative.ARGB(255, 66, 72, 80) : EmojiWindowNative.ARGB(255, 229, 235, 243);

        public static uint ToolbarButtonActive(bool dark) => dark ? EmojiWindowNative.ARGB(255, 83, 94, 109) : EmojiWindowNative.ARGB(255, 219, 233, 255);

        public static uint ToolbarButtonBorder(bool dark) => dark ? EmojiWindowNative.ARGB(255, 66, 72, 80) : EmojiWindowNative.ARGB(255, 232, 236, 242);

        public static uint AddressBackground(bool dark) => dark ? EmojiWindowNative.ARGB(255, 47, 52, 59) : EmojiWindowNative.ARGB(255, 255, 255, 255);

        public static uint AddressShellBackground(bool dark) => AddressBackground(dark);

        public static uint CardBackground(bool dark) => dark ? EmojiWindowNative.ARGB(255, 35, 39, 46) : EmojiWindowNative.ARGB(255, 255, 255, 255);

        public static uint CardAltBackground(bool dark) => dark ? EmojiWindowNative.ARGB(255, 30, 33, 39) : EmojiWindowNative.ARGB(255, 248, 250, 252);

        public static uint FooterBackground(bool dark) => dark ? EmojiWindowNative.ARGB(255, 30, 33, 39) : EmojiWindowNative.ARGB(255, 241, 243, 244);

        public static uint FooterText(bool dark) => TextSecondary(dark);

        public static uint AddressBadgeBackground(string url)
        {
            int seed = 0;
            if (!string.IsNullOrEmpty(url))
            {
                for (int i = 0; i < url.Length; i++)
                {
                    seed += url[i];
                }
            }

            uint[] colors =
            {
                EmojiWindowNative.ARGB(255, 66, 133, 244),
                EmojiWindowNative.ARGB(255, 52, 168, 83),
                EmojiWindowNative.ARGB(255, 251, 188, 5),
                EmojiWindowNative.ARGB(255, 234, 67, 53),
                EmojiWindowNative.ARGB(255, 120, 94, 240),
                EmojiWindowNative.ARGB(255, 0, 172, 193)
            };

            return colors[seed % colors.Length];
        }
    }
}
