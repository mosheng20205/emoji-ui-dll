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

    internal sealed class DemoApp
    {
        private readonly Dictionary<int, Action> _buttonActions = new Dictionary<int, Action>();
        private readonly List<Delegate> _pinnedDelegates = new List<Delegate>();
        private readonly byte[] _fontBytes = EmojiWindowNative.ToUtf8("微软雅黑");

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
            Window = EmojiWindowNative.create_window_bytes_ex(titleBytes, titleBytes.Length, 120, 90, width, height, titlebarColor ?? DemoColors.Blue, backgroundColor ?? DemoColors.WindowBg);
            if (Window == IntPtr.Zero)
            {
                throw new InvalidOperationException("创建窗口失败。");
            }
        }

        public void CreateStatusBar(string text) => StatusLabel = Label(16, Height - 52, Width - 32, 28, text, DemoColors.Black, DemoColors.LightBlue);

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
            return EmojiWindowNative.CreateLabel(parent ?? Window, x, y, width, height, bytes, bytes.Length, fg ?? DemoColors.Black, bg ?? DemoColors.Transparent, _fontBytes, _fontBytes.Length, fontSize, 0, 0, 0, align, wrap ? 1 : 0);
        }

        public IntPtr EditBox(int x, int y, int width, int height, string text, bool multiline = false, bool readOnly = false, bool password = false, bool colorEmoji = false, IntPtr? parent = null)
        {
            byte[] bytes = U(text);
            if (colorEmoji)
            {
                return EmojiWindowNative.CreateColorEmojiEditBox(parent ?? Window, x, y, width, height, bytes, bytes.Length, DemoColors.Black, DemoColors.White, _fontBytes, _fontBytes.Length, 13, 0, 0, 0, 0, multiline ? 1 : 0, readOnly ? 1 : 0, password ? 1 : 0, 1, multiline ? 0 : 1);
            }

            return EmojiWindowNative.CreateEditBox(parent ?? Window, x, y, width, height, bytes, bytes.Length, DemoColors.Black, DemoColors.White, _fontBytes, _fontBytes.Length, 13, 0, 0, 0, 0, multiline ? 1 : 0, readOnly ? 1 : 0, password ? 1 : 0, 1, multiline ? 0 : 1);
        }

        public IntPtr GroupBox(int x, int y, int width, int height, string title, uint? borderColor = null, uint? bgColor = null, IntPtr? parent = null)
        {
            byte[] bytes = U(title);
            return EmojiWindowNative.CreateGroupBox(parent ?? Window, x, y, width, height, bytes, bytes.Length, borderColor ?? DemoColors.Border, bgColor ?? DemoColors.Surface, _fontBytes, _fontBytes.Length, 13, 1, 0, 0);
        }

        public IntPtr Panel(int x, int y, int width, int height, uint? bgColor = null, IntPtr? parent = null)
        {
            return EmojiWindowNative.CreatePanel(parent ?? Window, x, y, width, height, bgColor ?? DemoColors.Surface);
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
            int buttonId = EmojiWindowNative.create_emoji_button_bytes(parent ?? Window, emojiBytes, emojiBytes.Length, textBytes, textBytes.Length, x, y, width, height, color ?? DemoColors.Blue);
            if (onClick != null)
            {
                _buttonActions[buttonId] = onClick;
            }

            return buttonId;
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
