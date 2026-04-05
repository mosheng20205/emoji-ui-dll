using System;
using System.Collections.Generic;
using System.IO;

namespace EmojiWindowDemo
{
    internal enum DemoKind
    {
        Button,
        CheckBox,
        ColorEmojiEditBox,
        ComboBox,
        ConfirmBox,
        D2DComboBox,
        DataGridView,
        DateTimePicker,
        EditBox,
        GroupBox,
        HotKey,
        Label,
        ListBox,
        MenuBar,
        MessageBox,
        Notification,
        Panel,
        PictureBox,
        PopupMenu,
        ProgressBar,
        RadioButton,
        Slider,
        Switch,
        TabControl,
        Tooltip,
        TreeView,
        Window,
    }

    internal static class DemoColors
    {
        public static readonly uint White = EmojiWindowNative.ARGB(255, 255, 255, 255);
        public static readonly uint Black = EmojiWindowNative.ARGB(255, 48, 49, 51);
        public static readonly uint Transparent = EmojiWindowNative.ARGB(0, 0, 0, 0);
        public static readonly uint WindowBg = EmojiWindowNative.ARGB(255, 245, 247, 250);
        public static readonly uint Surface = EmojiWindowNative.ARGB(255, 255, 255, 255);
        public static readonly uint Border = EmojiWindowNative.ARGB(255, 220, 223, 230);
        public static readonly uint Blue = EmojiWindowNative.ARGB(255, 64, 158, 255);
        public static readonly uint Green = EmojiWindowNative.ARGB(255, 103, 194, 58);
        public static readonly uint Orange = EmojiWindowNative.ARGB(255, 230, 162, 60);
        public static readonly uint Red = EmojiWindowNative.ARGB(255, 245, 108, 108);
        public static readonly uint Gray = EmojiWindowNative.ARGB(255, 144, 147, 153);
        public static readonly uint Purple = EmojiWindowNative.ARGB(255, 144, 122, 255);
        public static readonly uint Cyan = EmojiWindowNative.ARGB(255, 54, 190, 215);
        public static readonly uint Yellow = EmojiWindowNative.ARGB(255, 250, 236, 216);
        public static readonly uint LightBlue = EmojiWindowNative.ARGB(255, 236, 245, 255);
        public static readonly uint LightGreen = EmojiWindowNative.ARGB(255, 240, 249, 235);
        public static readonly uint LightRed = EmojiWindowNative.ARGB(255, 254, 240, 240);
    }

    internal static class DemoTheme
    {
        public const uint Primary = 0;
        public const uint Success = 1;
        public const uint Warning = 2;
        public const uint Danger = 3;
        public const uint Info = 4;
        public const uint Text = 5;
        public const uint Muted = 6;
        public const uint Subtle = 7;
        public const uint Border = 9;
        public const uint BorderLight = 10;
        public const uint BorderSoft = 11;
        public const uint Background = 13;
        public const uint Surface = 14;
        public const uint SurfacePrimary = 15;
        public const uint SurfaceSuccess = 16;
        public const uint SurfaceWarning = 17;
        public const uint SurfaceDanger = 18;
        public const uint SurfaceInfo = 19;
    }

    internal sealed class DemoThemePalette
    {
        public DemoThemePalette(
            bool dark,
            uint pageBackground,
            uint cardBackground,
            uint text,
            uint muted,
            uint accent,
            uint sidebarBackground,
            uint sidebarText,
            uint sidebarMuted,
            uint treeBackground,
            uint treeHoverBackground)
        {
            Dark = dark;
            PageBackground = pageBackground;
            CardBackground = cardBackground;
            Text = text;
            Muted = muted;
            Accent = accent;
            SidebarBackground = sidebarBackground;
            SidebarText = sidebarText;
            SidebarMuted = sidebarMuted;
            TreeBackground = treeBackground;
            TreeHoverBackground = treeHoverBackground;
        }

        public bool Dark { get; }
        public uint PageBackground { get; }
        public uint CardBackground { get; }
        public uint Text { get; }
        public uint Muted { get; }
        public uint Accent { get; }
        public uint SidebarBackground { get; }
        public uint SidebarText { get; }
        public uint SidebarMuted { get; }
        public uint TreeBackground { get; }
        public uint TreeHoverBackground { get; }
    }

    internal sealed class DemoApp
    {
        private const int GroupBoxStyleCard = 1;

        private struct GroupBoxRegistration
        {
            public IntPtr Handle;
            public IntPtr Parent;
            public int X;
            public int Y;
            public int Width;
            public int Height;
        }

        private readonly Dictionary<int, Action> _buttonActions = new Dictionary<int, Action>();
        private readonly List<Delegate> _pinnedDelegates = new List<Delegate>();
        private readonly List<GroupBoxRegistration> _groupBoxes = new List<GroupBoxRegistration>();
        private readonly byte[] _fontBytes = EmojiWindowNative.ToUtf8("Microsoft YaHei UI");

        public IntPtr Window { get; private set; }
        public IntPtr StatusLabel { get; private set; }
        public int Width { get; private set; }
        public int Height { get; private set; }

        public byte[] U(string text) => EmojiWindowNative.ToUtf8(text);

        public T Pin<T>(T callback) where T : Delegate
        {
            _pinnedDelegates.Add(callback);
            return callback;
        }

        public void CreateWindow(string title, int width = 900, int height = 620, uint? titlebarColor = null, uint? backgroundColor = null)
        {
            Width = width;
            Height = height;
            byte[] titleBytes = U(title);
            Window = EmojiWindowNative.create_window_bytes_ex(
                titleBytes,
                titleBytes.Length,
                120,
                90,
                width,
                height,
                NormalizeThemeColor(titlebarColor ?? DemoColors.Blue),
                NormalizeThemeColor(backgroundColor ?? DemoColors.WindowBg));
            if (Window == IntPtr.Zero)
            {
                throw new InvalidOperationException("Failed to create window.");
            }
        }

        public void CreateStatusBar(string text) => StatusLabel = Label(16, Height - 72, Width - 32, 32, text, DemoTheme.Muted, DemoTheme.Background);

        public void UpdateWindowSize(int width, int height)
        {
            Width = width;
            Height = height;
        }

        public void SetStatus(string text)
        {
            if (StatusLabel == IntPtr.Zero)
            {
                return;
            }

            byte[] bytes = U(text);
            EmojiWindowNative.SetLabelText(StatusLabel, bytes, bytes.Length);
        }

        public IntPtr Label(int x, int y, int width, int height, string text, uint? fg = null, uint? bg = null, int fontSize = 12, int align = 0, bool wrap = false, IntPtr? parent = null)
        {
            byte[] bytes = U(text);
            IntPtr resolvedParent = parent ?? Window;
            uint normalizedForeground = NormalizeThemeColor(fg ?? DemoColors.Black);
            uint normalizedBackground = ResolveLabelBackground(resolvedParent, x, y, width, height, NormalizeThemeColor(bg ?? DemoColors.Transparent));
            return EmojiWindowNative.CreateLabel(
                resolvedParent,
                x,
                y,
                width,
                height,
                bytes,
                bytes.Length,
                normalizedForeground,
                normalizedBackground,
                _fontBytes,
                _fontBytes.Length,
                fontSize,
                0,
                0,
                0,
                align,
                wrap ? 1 : 0);
        }

        public IntPtr EditBox(int x, int y, int width, int height, string text, bool multiline = false, bool readOnly = false, bool password = false, bool colorEmoji = false, IntPtr? parent = null)
        {
            byte[] bytes = U(text);
            if (colorEmoji)
            {
                return EmojiWindowNative.CreateColorEmojiEditBox(
                    parent ?? Window,
                    x,
                    y,
                    width,
                    height,
                    bytes,
                    bytes.Length,
                    NormalizeThemeColor(DemoColors.Black),
                    NormalizeThemeColor(DemoColors.White),
                    _fontBytes,
                    _fontBytes.Length,
                    13,
                    0,
                    0,
                    0,
                    0,
                    multiline ? 1 : 0,
                    readOnly ? 1 : 0,
                    password ? 1 : 0,
                    1,
                    multiline ? 0 : 1);
            }

            return EmojiWindowNative.CreateEditBox(
                parent ?? Window,
                x,
                y,
                width,
                height,
                bytes,
                bytes.Length,
                NormalizeThemeColor(DemoColors.Black),
                NormalizeThemeColor(DemoColors.White),
                _fontBytes,
                _fontBytes.Length,
                13,
                0,
                0,
                0,
                0,
                multiline ? 1 : 0,
                readOnly ? 1 : 0,
                password ? 1 : 0,
                1,
                multiline ? 0 : 1);
        }

        public IntPtr GroupBox(int x, int y, int width, int height, string title, uint? borderColor = null, uint? bgColor = null, IntPtr? parent = null)
        {
            byte[] bytes = U(title);
            uint resolvedBorder = borderColor ?? DemoTheme.BorderLight;
            uint resolvedBackground = bgColor ?? DemoTheme.Surface;

            // Match the Python demo's default card-style group box look.
            if (resolvedBorder == DemoTheme.Border)
            {
                resolvedBorder = DemoTheme.BorderLight;
            }

            if (resolvedBackground == DemoTheme.Background)
            {
                resolvedBackground = DemoTheme.Surface;
            }

            IntPtr groupBox = EmojiWindowNative.CreateGroupBox(
                parent ?? Window,
                x,
                y,
                width,
                height,
                bytes,
                bytes.Length,
                NormalizeThemeColor(resolvedBorder),
                NormalizeThemeColor(resolvedBackground),
                _fontBytes,
                _fontBytes.Length,
                14,
                1,
                0,
                0);
            EmojiWindowNative.SetGroupBoxStyle(groupBox, GroupBoxStyleCard);
            EmojiWindowNative.SetGroupBoxTitleColor(groupBox, DemoTheme.Text);
            _groupBoxes.Add(new GroupBoxRegistration
            {
                Handle = groupBox,
                Parent = parent ?? Window,
                X = x,
                Y = y,
                Width = width,
                Height = height
            });
            return groupBox;
        }

        public IntPtr Panel(int x, int y, int width, int height, uint? bgColor = null, IntPtr? parent = null)
        {
            return EmojiWindowNative.CreatePanel(parent ?? Window, x, y, width, height, NormalizeThemeColor(bgColor ?? DemoColors.Surface));
        }

        public void AttachToGroup(IntPtr groupBox, params IntPtr[] children)
        {
            foreach (IntPtr child in children)
            {
                if (child != IntPtr.Zero)
                {
                    EmojiWindowNative.AddChildToGroup(groupBox, child);
                }
            }
        }

        public int Button(int x, int y, int width, int height, string text, string emoji = "", uint? color = null, Action onClick = null, IntPtr? parent = null)
        {
            byte[] textBytes = U(text);
            byte[] emojiBytes = U(emoji);
            int buttonId = EmojiWindowNative.create_emoji_button_bytes(
                parent ?? Window,
                emojiBytes,
                emojiBytes.Length,
                textBytes,
                textBytes.Length,
                x,
                y,
                width,
                height,
                NormalizeThemeColor(color ?? DemoColors.Blue));
            if (onClick != null)
            {
                _buttonActions[buttonId] = onClick;
            }

            return buttonId;
        }

        public bool IsDarkTheme() => EmojiWindowNative.IsDarkMode() != 0;

        public uint ThemeColor(string name)
        {
            byte[] bytes = U(name);
            return EmojiWindowNative.EW_GetThemeColor(bytes, bytes.Length);
        }

        public DemoThemePalette GetThemePalette()
        {
            bool dark = IsDarkTheme();
            uint pageBackground = dark ? EmojiWindowNative.ARGB(255, 29, 30, 31) : EmojiWindowNative.ARGB(255, 245, 247, 250);
            uint cardBackground = ThemeColor("background_light");
            uint text = ThemeColor("text_primary");
            uint muted = ThemeColor("text_regular");
            uint accent = ThemeColor("primary");
            uint sidebarBackground = dark ? EmojiWindowNative.ARGB(255, 22, 23, 24) : DemoColors.White;
            uint sidebarText = dark ? EmojiWindowNative.ARGB(255, 243, 245, 247) : DemoColors.Black;
            uint sidebarMuted = dark ? EmojiWindowNative.ARGB(255, 156, 163, 175) : DemoColors.Gray;
            uint treeBackground = dark ? EmojiWindowNative.ARGB(255, 29, 30, 32) : DemoColors.White;
            uint treeHoverBackground = dark ? EmojiWindowNative.ARGB(255, 43, 47, 54) : DemoColors.LightBlue;
            return new DemoThemePalette(
                dark,
                pageBackground,
                cardBackground,
                text,
                muted,
                accent,
                sidebarBackground,
                sidebarText,
                sidebarMuted,
                treeBackground,
                treeHoverBackground);
        }

        public static uint NormalizeThemeColor(uint color)
        {
            if (color <= DemoTheme.SurfaceInfo)
            {
                return color;
            }

            if (color == DemoColors.Blue) return DemoTheme.Primary;
            if (color == DemoColors.Green) return DemoTheme.Success;
            if (color == DemoColors.Orange) return DemoTheme.Warning;
            if (color == DemoColors.Red) return DemoTheme.Danger;
            if (color == DemoColors.Cyan) return DemoTheme.Info;
            if (color == DemoColors.Black) return DemoTheme.Text;
            if (color == DemoColors.Gray) return DemoTheme.Muted;
            if (color == DemoColors.Border) return DemoTheme.BorderLight;
            if (color == DemoColors.WindowBg) return DemoTheme.Background;
            if (color == DemoColors.Surface || color == DemoColors.White) return DemoTheme.Surface;
            if (color == DemoColors.LightBlue) return DemoTheme.SurfacePrimary;
            if (color == DemoColors.LightGreen) return DemoTheme.SurfaceSuccess;
            if (color == DemoColors.Yellow) return DemoTheme.SurfaceWarning;
            if (color == DemoColors.LightRed) return DemoTheme.SurfaceDanger;
            return color;
        }

        private uint ResolveLabelBackground(IntPtr parent, int x, int y, int width, int height, uint background)
        {
            if (background != DemoTheme.Background)
            {
                return background;
            }

            foreach (GroupBoxRegistration groupBox in _groupBoxes)
            {
                if (groupBox.Handle == parent)
                {
                    return DemoTheme.Surface;
                }

                if (groupBox.Parent != parent)
                {
                    continue;
                }

                bool insideHorizontal = x >= groupBox.X + 8 && x + width <= groupBox.X + groupBox.Width - 8;
                bool insideVertical = y >= groupBox.Y + 8 && y + height <= groupBox.Y + groupBox.Height - 8;
                if (insideHorizontal && insideVertical)
                {
                    return DemoTheme.Surface;
                }
            }

            return background;
        }

        public void DispatchButton(int buttonId)
        {
            if (_buttonActions.TryGetValue(buttonId, out Action action))
            {
                action();
            }
        }

        public string FindFileUpwards(string fileName)
        {
            string current = AppDomain.CurrentDomain.BaseDirectory;
            while (!string.IsNullOrEmpty(current))
            {
                string candidate = Path.Combine(current, fileName);
                if (File.Exists(candidate))
                {
                    return candidate;
                }

                DirectoryInfo parent = Directory.GetParent(current);
                if (parent == null)
                {
                    break;
                }

                current = parent.FullName;
            }

            return string.Empty;
        }
    }

    internal static class DemoRunner
    {
        private static DemoApp _current;
        private static readonly EmojiWindowNative.ButtonClickCallback ButtonDispatcher = OnButtonClick;

        public static void Run(DemoKind kind)
        {
            _current = new DemoApp();
            EmojiWindowNative.set_button_click_callback(ButtonDispatcher);
            DemoScenarios.Build(kind, _current);
            EmojiWindowNative.set_message_loop_main_window(_current.Window);
            EmojiWindowNative.run_message_loop();
            _current = null;
        }

        private static void OnButtonClick(int buttonId, IntPtr parentHwnd) => _current?.DispatchButton(buttonId);
    }
}
