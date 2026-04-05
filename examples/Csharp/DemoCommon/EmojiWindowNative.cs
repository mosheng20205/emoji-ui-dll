using System;
using System.Runtime.InteropServices;
using System.Text;

namespace EmojiWindowDemo
{
    internal static class EmojiWindowNative
    {
        private const string Dll = "emoji_window.dll";
        private const CallingConvention Cc = CallingConvention.StdCall;

        public static byte[] ToUtf8(string text) => string.IsNullOrEmpty(text) ? Array.Empty<byte>() : Encoding.UTF8.GetBytes(text);

        public static string FromUtf8(byte[] bytes)
        {
            if (bytes == null || bytes.Length == 0)
            {
                return string.Empty;
            }

            return Encoding.UTF8.GetString(bytes).TrimEnd('\0');
        }

        public static uint ARGB(int a, int r, int g, int b) => (uint)((a << 24) | (r << 16) | (g << 8) | b);

        public delegate int TextReaderHandle(IntPtr handle, IntPtr buffer, int bufferSize);
        public delegate int TextReaderId(int id, IntPtr buffer, int bufferSize);
        public delegate int TextReaderHandleIndex(IntPtr handle, int index, IntPtr buffer, int bufferSize);
        public delegate int GridTextReader(IntPtr handle, int row, int col, IntPtr buffer, int bufferSize);

        public static string ReadUtf8(TextReaderHandle reader, IntPtr handle)
        {
            int size = reader(handle, IntPtr.Zero, 0);
            if (size <= 0)
            {
                return string.Empty;
            }

            IntPtr buffer = Marshal.AllocHGlobal(size);
            try
            {
                reader(handle, buffer, size);
                byte[] bytes = new byte[size];
                Marshal.Copy(buffer, bytes, 0, size);
                return FromUtf8(bytes);
            }
            finally
            {
                Marshal.FreeHGlobal(buffer);
            }
        }

        public static string ReadUtf8(TextReaderHandleIndex reader, IntPtr handle, int index)
        {
            int size = reader(handle, index, IntPtr.Zero, 0);
            if (size <= 0)
            {
                return string.Empty;
            }

            IntPtr buffer = Marshal.AllocHGlobal(size);
            try
            {
                reader(handle, index, buffer, size);
                byte[] bytes = new byte[size];
                Marshal.Copy(buffer, bytes, 0, size);
                return FromUtf8(bytes);
            }
            finally
            {
                Marshal.FreeHGlobal(buffer);
            }
        }

        public static string ReadUtf8(TextReaderId reader, int id)
        {
            int size = reader(id, IntPtr.Zero, 0);
            if (size <= 0)
            {
                return string.Empty;
            }

            IntPtr buffer = Marshal.AllocHGlobal(size);
            try
            {
                reader(id, buffer, size);
                byte[] bytes = new byte[size];
                Marshal.Copy(buffer, bytes, 0, size);
                return FromUtf8(bytes);
            }
            finally
            {
                Marshal.FreeHGlobal(buffer);
            }
        }

        public static string ReadUtf8(GridTextReader reader, IntPtr handle, int row, int col)
        {
            int size = reader(handle, row, col, IntPtr.Zero, 0);
            if (size <= 0)
            {
                return string.Empty;
            }

            IntPtr buffer = Marshal.AllocHGlobal(size);
            try
            {
                reader(handle, row, col, buffer, size);
                byte[] bytes = new byte[size];
                Marshal.Copy(buffer, bytes, 0, size);
                return FromUtf8(bytes);
            }
            finally
            {
                Marshal.FreeHGlobal(buffer);
            }
        }

        [UnmanagedFunctionPointer(Cc)] public delegate void ButtonClickCallback(int buttonId, IntPtr parentHwnd);
        [UnmanagedFunctionPointer(Cc)] public delegate void MessageBoxCallback(int confirmed);
        [UnmanagedFunctionPointer(Cc)] public delegate void TabCallback(IntPtr hTabControl, int selectedIndex);
        [UnmanagedFunctionPointer(Cc)] public delegate void TabCloseCallback(IntPtr hTabControl, int index);
        [UnmanagedFunctionPointer(Cc)] public delegate void WindowResizeCallback(IntPtr hwnd, int width, int height);
        [UnmanagedFunctionPointer(Cc)] public delegate void WindowCloseCallback(IntPtr hwnd);
        [UnmanagedFunctionPointer(Cc)] public delegate void MenuItemClickCallback(int menuId, int itemId);
        [UnmanagedFunctionPointer(Cc)] public delegate void EditBoxKeyCallback(IntPtr hEdit, int keyCode, int keyDown, int shift, int ctrl, int alt);
        [UnmanagedFunctionPointer(Cc)] public delegate void CheckBoxCallback(IntPtr hCheckBox, int checkedState);
        [UnmanagedFunctionPointer(Cc)] public delegate void ProgressBarCallback(IntPtr hProgressBar, int value);
        [UnmanagedFunctionPointer(Cc)] public delegate void PictureBoxCallback(IntPtr hPictureBox);
        [UnmanagedFunctionPointer(Cc)] public delegate void RadioButtonCallback(IntPtr hRadioButton, int groupId, int checkedState);
        [UnmanagedFunctionPointer(Cc)] public delegate void SliderCallback(IntPtr hSlider, int value);
        [UnmanagedFunctionPointer(Cc)] public delegate void SwitchCallback(IntPtr hSwitch, int checkedState);
        [UnmanagedFunctionPointer(Cc)] public delegate void NotificationCallback(IntPtr hNotification, int eventType);
        [UnmanagedFunctionPointer(Cc)] public delegate void ListBoxCallback(IntPtr hListBox, int index);
        [UnmanagedFunctionPointer(Cc)] public delegate void ComboBoxCallback(IntPtr hComboBox, int index);
        [UnmanagedFunctionPointer(Cc)] public delegate void HotKeyCallback(IntPtr hHotKey, int vkCode, int modifiers);
        [UnmanagedFunctionPointer(Cc)] public delegate void ValueChangedCallback(IntPtr hwnd);
        [UnmanagedFunctionPointer(Cc)] public delegate void DataGridCellCallback(IntPtr hGrid, int row, int col);
        [UnmanagedFunctionPointer(Cc)] public delegate void DataGridColumnHeaderCallback(IntPtr hGrid, int col);
        [UnmanagedFunctionPointer(Cc)] public delegate int DataGridVirtualDataCallback(IntPtr hGrid, int row, int col, IntPtr buffer, int bufferSize);
        [UnmanagedFunctionPointer(Cc)] public delegate void ControlPointCallback(IntPtr hwnd, int x, int y);
        [UnmanagedFunctionPointer(Cc)] public delegate void ControlKeyCallback(IntPtr hwnd, int vkCode, int shift, int ctrl, int alt);
        [UnmanagedFunctionPointer(Cc)] public delegate void ControlCharCallback(IntPtr hwnd, int charCode);
        [UnmanagedFunctionPointer(Cc)] public delegate void TreeNodeCallback(int nodeId, IntPtr context);
        [UnmanagedFunctionPointer(Cc)] public delegate void ThemeChangedCallback([MarshalAs(UnmanagedType.LPStr)] string themeName);

        [DllImport(Dll, CallingConvention = Cc)] public static extern IntPtr create_window_bytes_ex(byte[] title, int titleLen, int x, int y, int width, int height, uint titlebarColor, uint clientBgColor);
        [DllImport(Dll, CallingConvention = Cc)] public static extern void set_message_loop_main_window(IntPtr hwnd);
        [DllImport(Dll, CallingConvention = Cc)] public static extern int run_message_loop();
        [DllImport(Dll, CallingConvention = Cc)] public static extern int GetWindowTitle(IntPtr hwnd, IntPtr buffer, int bufferSize);
        [DllImport(Dll, CallingConvention = Cc)] public static extern int GetWindowBounds(IntPtr hwnd, out int x, out int y, out int width, out int height);
        [DllImport(Dll, CallingConvention = Cc)] public static extern void SetWindowBounds(IntPtr hwnd, int x, int y, int width, int height);
        [DllImport(Dll, CallingConvention = Cc)] public static extern void set_window_icon_bytes(IntPtr hwnd, byte[] iconData, int dataLen);
        [DllImport(Dll, CallingConvention = Cc)] public static extern void set_window_title(IntPtr hwnd, byte[] titleUtf8, int titleLen);
        [DllImport(Dll, CallingConvention = Cc)] public static extern void set_window_titlebar_color(IntPtr hwnd, uint color);
        [DllImport(Dll, CallingConvention = Cc)] public static extern int GetWindowVisible(IntPtr hwnd);
        [DllImport(Dll, CallingConvention = Cc)] public static extern void ShowEmojiWindow(IntPtr hwnd, int visible);
        [DllImport(Dll, CallingConvention = Cc)] public static extern uint GetWindowTitlebarColor(IntPtr hwnd);
        [DllImport(Dll, CallingConvention = Cc)] public static extern int SetTitleBarTextColor(IntPtr hwnd, uint color);
        [DllImport(Dll, CallingConvention = Cc)] public static extern uint GetTitleBarTextColor(IntPtr hwnd);
        [DllImport(Dll, CallingConvention = Cc)] public static extern void SetWindowBackgroundColor(IntPtr hwnd, uint color);
        [DllImport(Dll, CallingConvention = Cc)] public static extern void SetDarkMode(int darkMode);
        [DllImport(Dll, CallingConvention = Cc)] public static extern int IsDarkMode();
        [DllImport(Dll, CallingConvention = Cc)] public static extern uint EW_GetThemeColor(byte[] colorNameUtf8, int nameLen);
        [DllImport(Dll, CallingConvention = Cc)] public static extern void SetThemeChangedCallback(ThemeChangedCallback callback);
        [DllImport(Dll, CallingConvention = Cc)] public static extern void SetWindowResizeCallback(WindowResizeCallback callback);
        [DllImport(Dll, CallingConvention = Cc)] public static extern void SetWindowCloseCallback(WindowCloseCallback callback);

        [DllImport(Dll, CallingConvention = Cc)] public static extern int create_emoji_button_bytes(IntPtr parent, byte[] emoji, int emojiLen, byte[] text, int textLen, int x, int y, int width, int height, uint bgColor);
        [DllImport(Dll, CallingConvention = Cc)] public static extern void set_button_click_callback(ButtonClickCallback callback);
        [DllImport(Dll, CallingConvention = Cc)] public static extern int GetButtonText(int buttonId, IntPtr buffer, int bufferSize);
        [DllImport(Dll, CallingConvention = Cc)] public static extern int GetButtonEmoji(int buttonId, IntPtr buffer, int bufferSize);
        [DllImport(Dll, CallingConvention = Cc)] public static extern void SetButtonText(int buttonId, byte[] text, int textLen);
        [DllImport(Dll, CallingConvention = Cc)] public static extern void SetButtonEmoji(int buttonId, byte[] emoji, int emojiLen);
        [DllImport(Dll, CallingConvention = Cc)] public static extern int GetButtonBounds(int buttonId, out int x, out int y, out int width, out int height);
        [DllImport(Dll, CallingConvention = Cc)] public static extern void SetButtonBounds(int buttonId, int x, int y, int width, int height);
        [DllImport(Dll, CallingConvention = Cc)] public static extern uint GetButtonBackgroundColor(int buttonId);
        [DllImport(Dll, CallingConvention = Cc)] public static extern void SetButtonBackgroundColor(int buttonId, uint color);
        [DllImport(Dll, CallingConvention = Cc)] public static extern uint GetButtonTextColor(int buttonId);
        [DllImport(Dll, CallingConvention = Cc)] public static extern void SetButtonTextColor(int buttonId, uint color);
        [DllImport(Dll, CallingConvention = Cc)] public static extern uint GetButtonBorderColor(int buttonId);
        [DllImport(Dll, CallingConvention = Cc)] public static extern void SetButtonBorderColor(int buttonId, uint color);
        [DllImport(Dll, CallingConvention = Cc)] public static extern void SetButtonHoverColors(int buttonId, uint bgColor, uint borderColor, uint textColor);
        [DllImport(Dll, CallingConvention = Cc)] public static extern void ResetButtonColorOverrides(int buttonId);
        [DllImport(Dll, CallingConvention = Cc)] public static extern void SetButtonType(int buttonId, int type);
        [DllImport(Dll, CallingConvention = Cc)] public static extern int GetButtonType(int buttonId);
        [DllImport(Dll, CallingConvention = Cc)] public static extern void SetButtonStyle(int buttonId, int style);
        [DllImport(Dll, CallingConvention = Cc)] public static extern int GetButtonStyle(int buttonId);
        [DllImport(Dll, CallingConvention = Cc)] public static extern void SetButtonSize(int buttonId, int size);
        [DllImport(Dll, CallingConvention = Cc)] public static extern int GetButtonSize(int buttonId);
        [DllImport(Dll, CallingConvention = Cc)] public static extern void SetButtonRound(int buttonId, int round);
        [DllImport(Dll, CallingConvention = Cc)] public static extern int GetButtonRound(int buttonId);
        [DllImport(Dll, CallingConvention = Cc)] public static extern void SetButtonCircle(int buttonId, int circle);
        [DllImport(Dll, CallingConvention = Cc)] public static extern int GetButtonCircle(int buttonId);
        [DllImport(Dll, CallingConvention = Cc)] public static extern void SetButtonLoading(int buttonId, int loading);
        [DllImport(Dll, CallingConvention = Cc)] public static extern int GetButtonLoading(int buttonId);
        [DllImport(Dll, CallingConvention = Cc)] public static extern int GetButtonVisible(int buttonId);
        [DllImport(Dll, CallingConvention = Cc)] public static extern void ShowButton(int buttonId, int visible);
        [DllImport(Dll, CallingConvention = Cc)] public static extern int GetButtonEnabled(int buttonId);
        [DllImport(Dll, CallingConvention = Cc)] public static extern void EnableButton(IntPtr parentHwnd, int buttonId, int enable);

        [DllImport(Dll, CallingConvention = Cc)] public static extern IntPtr CreateLabel(IntPtr parent, int x, int y, int width, int height, byte[] text, int textLen, uint fgColor, uint bgColor, byte[] fontName, int fontNameLen, int fontSize, int bold, int italic, int underline, int alignment, int wordWrap);
        [DllImport(Dll, CallingConvention = Cc)] public static extern int GetLabelText(IntPtr hLabel, IntPtr buffer, int bufferSize);
        [DllImport(Dll, CallingConvention = Cc)] public static extern void SetLabelText(IntPtr hLabel, byte[] text, int textLen);
        [DllImport(Dll, CallingConvention = Cc)] public static extern void SetLabelColor(IntPtr hLabel, uint fgColor, uint bgColor);
        [DllImport(Dll, CallingConvention = Cc)] public static extern void SetLabelFont(IntPtr hLabel, byte[] fontName, int fontNameLen, int fontSize, int bold, int italic, int underline);
        [DllImport(Dll, CallingConvention = Cc)] public static extern void SetLabelBounds(IntPtr hLabel, int x, int y, int width, int height);
        [DllImport(Dll, CallingConvention = Cc)] public static extern void EnableLabel(IntPtr hLabel, int enabled);
        [DllImport(Dll, CallingConvention = Cc)] public static extern void ShowLabel(IntPtr hLabel, int visible);
        [DllImport(Dll, CallingConvention = Cc)] public static extern int GetLabelFont(IntPtr hLabel, IntPtr buffer, int bufferSize, out int fontSize, out int bold, out int italic, out int underline);
        [DllImport(Dll, CallingConvention = Cc)] public static extern int GetLabelColor(IntPtr hLabel, out uint fgColor, out uint bgColor);
        [DllImport(Dll, CallingConvention = Cc)] public static extern int GetLabelBounds(IntPtr hLabel, out int x, out int y, out int width, out int height);
        [DllImport(Dll, CallingConvention = Cc)] public static extern int GetLabelAlignment(IntPtr hLabel);
        [DllImport(Dll, CallingConvention = Cc)] public static extern int GetLabelEnabled(IntPtr hLabel);
        [DllImport(Dll, CallingConvention = Cc)] public static extern int GetLabelVisible(IntPtr hLabel);

        [DllImport(Dll, CallingConvention = Cc)] public static extern IntPtr CreateEditBox(IntPtr parent, int x, int y, int width, int height, byte[] text, int textLen, uint fgColor, uint bgColor, byte[] fontName, int fontNameLen, int fontSize, int bold, int italic, int underline, int alignment, int multiline, int readOnly, int password, int hasBorder, int verticalCenter);
        [DllImport(Dll, CallingConvention = Cc)] public static extern IntPtr CreateColorEmojiEditBox(IntPtr parent, int x, int y, int width, int height, byte[] text, int textLen, uint fgColor, uint bgColor, byte[] fontName, int fontNameLen, int fontSize, int bold, int italic, int underline, int alignment, int multiline, int readOnly, int password, int hasBorder, int verticalCenter);
        [DllImport(Dll, CallingConvention = Cc)] public static extern IntPtr CreateD2DColorEmojiEditBox(IntPtr parent, int x, int y, int width, int height, byte[] text, int textLen, uint fgColor, uint bgColor, byte[] fontName, int fontNameLen, int fontSize, int bold, int italic, int underline, int alignment, int multiline, int readOnly, int password, int hasBorder, int verticalCenter);
        [DllImport(Dll, CallingConvention = Cc)] public static extern int GetEditBoxText(IntPtr hEdit, IntPtr buffer, int bufferSize);
        [DllImport(Dll, CallingConvention = Cc)] public static extern void SetEditBoxText(IntPtr hEdit, byte[] text, int textLen);
        [DllImport(Dll, CallingConvention = Cc)] public static extern void SetEditBoxKeyCallback(IntPtr hEdit, EditBoxKeyCallback callback);
        [DllImport(Dll, CallingConvention = Cc)] public static extern void SetEditBoxFont(IntPtr hEdit, byte[] fontName, int fontNameLen, int fontSize, int bold, int italic, int underline);
        [DllImport(Dll, CallingConvention = Cc)] public static extern void SetEditBoxColor(IntPtr hEdit, uint fgColor, uint bgColor);
        [DllImport(Dll, CallingConvention = Cc)] public static extern void SetEditBoxBounds(IntPtr hEdit, int x, int y, int width, int height);
        [DllImport(Dll, CallingConvention = Cc)] public static extern void EnableEditBox(IntPtr hEdit, int enable);
        [DllImport(Dll, CallingConvention = Cc)] public static extern void ShowEditBox(IntPtr hEdit, int show);
        [DllImport(Dll, CallingConvention = Cc)] public static extern int GetEditBoxFont(IntPtr hEdit, IntPtr buffer, int bufferSize, out int fontSize, out int bold, out int italic, out int underline);
        [DllImport(Dll, CallingConvention = Cc)] public static extern int GetEditBoxColor(IntPtr hEdit, out uint fgColor, out uint bgColor);
        [DllImport(Dll, CallingConvention = Cc)] public static extern int GetEditBoxBounds(IntPtr hEdit, out int x, out int y, out int width, out int height);
        [DllImport(Dll, CallingConvention = Cc)] public static extern int GetEditBoxAlignment(IntPtr hEdit);
        [DllImport(Dll, CallingConvention = Cc)] public static extern int GetEditBoxEnabled(IntPtr hEdit);
        [DllImport(Dll, CallingConvention = Cc)] public static extern int GetEditBoxVisible(IntPtr hEdit);

        [DllImport(Dll, CallingConvention = Cc)] public static extern IntPtr CreateCheckBox(IntPtr parent, int x, int y, int width, int height, byte[] text, int textLen, int checkedState, uint fgColor, uint bgColor, byte[] fontName, int fontNameLen, int fontSize, int bold, int italic, int underline);
        [DllImport(Dll, CallingConvention = Cc)] public static extern int GetCheckBoxState(IntPtr hCheckBox);
        [DllImport(Dll, CallingConvention = Cc)] public static extern void SetCheckBoxState(IntPtr hCheckBox, int checkedState);
        [DllImport(Dll, CallingConvention = Cc)] public static extern void SetCheckBoxCallback(IntPtr hCheckBox, CheckBoxCallback callback);
        [DllImport(Dll, CallingConvention = Cc)] public static extern int GetCheckBoxText(IntPtr hCheckBox, IntPtr buffer, int bufferSize);
        [DllImport(Dll, CallingConvention = Cc)] public static extern void EnableCheckBox(IntPtr hCheckBox, int enable);
        [DllImport(Dll, CallingConvention = Cc)] public static extern void ShowCheckBox(IntPtr hCheckBox, int show);
        [DllImport(Dll, CallingConvention = Cc)] public static extern void SetCheckBoxText(IntPtr hCheckBox, byte[] text, int textLen);
        [DllImport(Dll, CallingConvention = Cc)] public static extern void SetCheckBoxBounds(IntPtr hCheckBox, int x, int y, int width, int height);
        [DllImport(Dll, CallingConvention = Cc)] public static extern void SetCheckBoxColor(IntPtr hCheckBox, uint fgColor, uint bgColor);
        [DllImport(Dll, CallingConvention = Cc)] public static extern void SetCheckBoxCheckColor(IntPtr hCheckBox, uint color);
        [DllImport(Dll, CallingConvention = Cc)] public static extern void SetCheckBoxStyle(IntPtr hCheckBox, int style);
        [DllImport(Dll, CallingConvention = Cc)] public static extern int GetCheckBoxStyle(IntPtr hCheckBox);
        [DllImport(Dll, CallingConvention = Cc)] public static extern int GetCheckBoxColor(IntPtr hCheckBox, out uint fgColor, out uint bgColor);
        [DllImport(Dll, CallingConvention = Cc)] public static extern int GetCheckBoxCheckColor(IntPtr hCheckBox, out uint checkColor);

        [DllImport(Dll, CallingConvention = Cc)] public static extern IntPtr CreateRadioButton(IntPtr parent, int x, int y, int width, int height, byte[] text, int textLen, int groupId, int checkedState, uint fgColor, uint bgColor, byte[] fontName, int fontNameLen, int fontSize, int bold, int italic, int underline);
        [DllImport(Dll, CallingConvention = Cc)] public static extern int GetRadioButtonState(IntPtr hRadioButton);
        [DllImport(Dll, CallingConvention = Cc)] public static extern void SetRadioButtonState(IntPtr hRadioButton, int checkedState);
        [DllImport(Dll, CallingConvention = Cc)] public static extern void SetRadioButtonCallback(IntPtr hRadioButton, RadioButtonCallback callback);
        [DllImport(Dll, CallingConvention = Cc)] public static extern int GetRadioButtonText(IntPtr hRadioButton, IntPtr buffer, int bufferSize);
        [DllImport(Dll, CallingConvention = Cc)] public static extern void EnableRadioButton(IntPtr hRadioButton, int enable);
        [DllImport(Dll, CallingConvention = Cc)] public static extern void ShowRadioButton(IntPtr hRadioButton, int show);
        [DllImport(Dll, CallingConvention = Cc)] public static extern void SetRadioButtonText(IntPtr hRadioButton, byte[] text, int textLen);
        [DllImport(Dll, CallingConvention = Cc)] public static extern void SetRadioButtonBounds(IntPtr hRadioButton, int x, int y, int width, int height);
        [DllImport(Dll, CallingConvention = Cc)] public static extern void SetRadioButtonColor(IntPtr hRadioButton, uint fgColor, uint bgColor);
        [DllImport(Dll, CallingConvention = Cc)] public static extern void SetRadioButtonDotColor(IntPtr hRadioButton, uint color);
        [DllImport(Dll, CallingConvention = Cc)] public static extern void SetRadioButtonStyle(IntPtr hRadioButton, int style);
        [DllImport(Dll, CallingConvention = Cc)] public static extern int GetRadioButtonColor(IntPtr hRadioButton, out uint fgColor, out uint bgColor);
        [DllImport(Dll, CallingConvention = Cc)] public static extern int GetRadioButtonDotColor(IntPtr hRadioButton, out uint dotColor);
        [DllImport(Dll, CallingConvention = Cc)] public static extern int GetRadioButtonStyle(IntPtr hRadioButton);

        [DllImport(Dll, CallingConvention = Cc)] public static extern IntPtr CreateProgressBar(IntPtr parent, int x, int y, int width, int height, int initialValue, uint fgColor, uint bgColor, int showText, uint textColor);
        [DllImport(Dll, CallingConvention = Cc)] public static extern void SetProgressValue(IntPtr hProgressBar, int value);
        [DllImport(Dll, CallingConvention = Cc)] public static extern int GetProgressValue(IntPtr hProgressBar);
        [DllImport(Dll, CallingConvention = Cc)] public static extern void SetProgressIndeterminate(IntPtr hProgressBar, int indeterminate);
        [DllImport(Dll, CallingConvention = Cc)] public static extern void SetProgressBarCallback(IntPtr hProgressBar, ProgressBarCallback callback);
        [DllImport(Dll, CallingConvention = Cc)] public static extern void SetProgressBarTextColor(IntPtr hProgressBar, uint color);
        [DllImport(Dll, CallingConvention = Cc)] public static extern void SetProgressBarShowText(IntPtr hProgressBar, int showText);

        [DllImport(Dll, CallingConvention = Cc)] public static extern IntPtr CreateListBox(IntPtr parent, int x, int y, int width, int height, int multiSelect, uint fgColor, uint bgColor);
        [DllImport(Dll, CallingConvention = Cc)] public static extern int AddListItem(IntPtr hListBox, byte[] text, int textLen);
        [DllImport(Dll, CallingConvention = Cc)] public static extern void RemoveListItem(IntPtr hListBox, int index);
        [DllImport(Dll, CallingConvention = Cc)] public static extern int GetSelectedIndex(IntPtr hListBox);
        [DllImport(Dll, CallingConvention = Cc)] public static extern void SetSelectedIndex(IntPtr hListBox, int index);
        [DllImport(Dll, CallingConvention = Cc)] public static extern int GetListItemCount(IntPtr hListBox);
        [DllImport(Dll, CallingConvention = Cc)] public static extern int GetListItemText(IntPtr hListBox, int index, IntPtr buffer, int bufferSize);
        [DllImport(Dll, CallingConvention = Cc)] public static extern void SetListBoxCallback(IntPtr hListBox, ListBoxCallback callback);
        [DllImport(Dll, CallingConvention = Cc)] public static extern void EnableListBox(IntPtr hListBox, int enable);
        [DllImport(Dll, CallingConvention = Cc)] public static extern void ShowListBox(IntPtr hListBox, int show);
        [DllImport(Dll, CallingConvention = Cc)] public static extern void SetListBoxBounds(IntPtr hListBox, int x, int y, int width, int height);
        [DllImport(Dll, CallingConvention = Cc)] public static extern void SetListBoxColors(IntPtr hListBox, uint fgColor, uint bgColor, uint selectColor, uint hoverColor);
        [DllImport(Dll, CallingConvention = Cc)] public static extern int GetListBoxColors(IntPtr hListBox, out uint fgColor, out uint bgColor, out uint selectColor, out uint hoverColor);

        [DllImport(Dll, CallingConvention = Cc)] public static extern IntPtr CreateComboBox(IntPtr parent, int x, int y, int width, int height, int readOnly, uint fgColor, uint bgColor, int itemHeight, byte[] fontName, int fontNameLen, int fontSize, int bold, int italic, int underline);
        [DllImport(Dll, CallingConvention = Cc)] public static extern int AddComboItem(IntPtr hComboBox, byte[] text, int textLen);
        [DllImport(Dll, CallingConvention = Cc)] public static extern void RemoveComboItem(IntPtr hComboBox, int index);
        [DllImport(Dll, CallingConvention = Cc)] public static extern void ClearComboBox(IntPtr hComboBox);
        [DllImport(Dll, CallingConvention = Cc)] public static extern int GetComboItemCount(IntPtr hComboBox);
        [DllImport(Dll, CallingConvention = Cc)] public static extern int GetComboSelectedIndex(IntPtr hComboBox);
        [DllImport(Dll, CallingConvention = Cc)] public static extern void SetComboSelectedIndex(IntPtr hComboBox, int index);
        [DllImport(Dll, CallingConvention = Cc)] public static extern int GetComboItemText(IntPtr hComboBox, int index, IntPtr buffer, int bufferSize);
        [DllImport(Dll, CallingConvention = Cc)] public static extern int GetComboBoxText(IntPtr hComboBox, IntPtr buffer, int bufferSize);
        [DllImport(Dll, CallingConvention = Cc)] public static extern void SetComboBoxText(IntPtr hComboBox, byte[] text, int textLen);
        [DllImport(Dll, CallingConvention = Cc)] public static extern void SetComboBoxCallback(IntPtr hComboBox, ComboBoxCallback callback);
        [DllImport(Dll, CallingConvention = Cc)] public static extern void EnableComboBox(IntPtr hComboBox, int enable);
        [DllImport(Dll, CallingConvention = Cc)] public static extern void ShowComboBox(IntPtr hComboBox, int show);
        [DllImport(Dll, CallingConvention = Cc)] public static extern void SetComboBoxBounds(IntPtr hComboBox, int x, int y, int width, int height);
        [DllImport(Dll, CallingConvention = Cc)] public static extern void SetComboBoxColors(IntPtr hComboBox, uint fgColor, uint bgColor, uint selectColor, uint hoverColor);
        [DllImport(Dll, CallingConvention = Cc)] public static extern int GetComboBoxColors(IntPtr hComboBox, out uint fgColor, out uint bgColor, out uint selectColor, out uint hoverColor);

        [DllImport(Dll, CallingConvention = Cc)] public static extern IntPtr CreateMenuBar(IntPtr hWindow);
        [DllImport(Dll, CallingConvention = Cc)] public static extern void DestroyMenuBar(IntPtr hMenuBar);
        [DllImport(Dll, CallingConvention = Cc)] public static extern int MenuBarAddItem(IntPtr hMenuBar, byte[] text, int textLen, int itemId);
        [DllImport(Dll, CallingConvention = Cc)] public static extern int MenuBarAddSubItem(IntPtr hMenuBar, int parentItemId, byte[] text, int textLen, int itemId);
        [DllImport(Dll, CallingConvention = Cc)] public static extern void SetMenuBarPlacement(IntPtr hMenuBar, int x, int y, int width, int height);
        [DllImport(Dll, CallingConvention = Cc)] public static extern void SetMenuBarCallback(IntPtr hMenuBar, MenuItemClickCallback callback);
        [DllImport(Dll, CallingConvention = Cc)] public static extern int MenuBarUpdateSubItemText(IntPtr hMenuBar, int parentItemId, int itemId, byte[] text, int textLen);

        [DllImport(Dll, CallingConvention = Cc)] public static extern IntPtr CreateEmojiPopupMenu(IntPtr hOwner);
        [DllImport(Dll, CallingConvention = Cc)] public static extern int PopupMenuAddItem(IntPtr hPopupMenu, byte[] text, int textLen, int itemId);
        [DllImport(Dll, CallingConvention = Cc)] public static extern int PopupMenuAddSubItem(IntPtr hPopupMenu, int parentItemId, byte[] text, int textLen, int itemId);
        [DllImport(Dll, CallingConvention = Cc)] public static extern void BindControlMenu(IntPtr hControl, IntPtr hPopupMenu);
        [DllImport(Dll, CallingConvention = Cc)] public static extern void BindButtonMenu(IntPtr hParent, int buttonId, IntPtr hPopupMenu);
        [DllImport(Dll, CallingConvention = Cc)] public static extern void ShowContextMenu(IntPtr hPopupMenu, int x, int y);
        [DllImport(Dll, CallingConvention = Cc)] public static extern void SetPopupMenuCallback(IntPtr hPopupMenu, MenuItemClickCallback callback);

        [DllImport(Dll, CallingConvention = Cc)] public static extern IntPtr CreateGroupBox(IntPtr parent, int x, int y, int width, int height, byte[] title, int titleLen, uint borderColor, uint bgColor, byte[] fontName, int fontNameLen, int fontSize, int bold, int italic, int underline);
        [DllImport(Dll, CallingConvention = Cc)] public static extern void AddChildToGroup(IntPtr hGroupBox, IntPtr hChild);
        [DllImport(Dll, CallingConvention = Cc)] public static extern void SetGroupBoxTitle(IntPtr hGroupBox, byte[] title, int titleLen);
        [DllImport(Dll, CallingConvention = Cc)] public static extern void SetGroupBoxTitleColor(IntPtr hGroupBox, uint titleColor);
        [DllImport(Dll, CallingConvention = Cc)] public static extern void SetGroupBoxStyle(IntPtr hGroupBox, int style);

        [DllImport(Dll, CallingConvention = Cc)] public static extern IntPtr CreateTabControl(IntPtr parent, int x, int y, int width, int height);
        [DllImport(Dll, CallingConvention = Cc)] public static extern int AddTabItem(IntPtr hTabControl, byte[] title, int titleLen, IntPtr hContentWindow);
        [DllImport(Dll, CallingConvention = Cc)] public static extern int RemoveTabItem(IntPtr hTabControl, int index);
        [DllImport(Dll, CallingConvention = Cc)] public static extern void SetTabCallback(IntPtr hTabControl, TabCallback callback);
        [DllImport(Dll, CallingConvention = Cc)] public static extern int GetCurrentTabIndex(IntPtr hTabControl);
        [DllImport(Dll, CallingConvention = Cc)] public static extern int SelectTab(IntPtr hTabControl, int index);
        [DllImport(Dll, CallingConvention = Cc)] public static extern int GetTabCount(IntPtr hTabControl);
        [DllImport(Dll, CallingConvention = Cc)] public static extern IntPtr GetTabContentWindow(IntPtr hTabControl, int index);
        [DllImport(Dll, CallingConvention = Cc)] public static extern int SetTabItemSize(IntPtr hTabControl, int width, int height);
        [DllImport(Dll, CallingConvention = Cc)] public static extern int SetTabColors(IntPtr hTabControl, uint selectedBg, uint unselectedBg, uint selectedText, uint unselectedText);
        [DllImport(Dll, CallingConvention = Cc)] public static extern int SetTabIndicatorColor(IntPtr hTabControl, uint color);
        [DllImport(Dll, CallingConvention = Cc)] public static extern int SetTabPadding(IntPtr hTabControl, int horizontal, int vertical);
        [DllImport(Dll, CallingConvention = Cc)] public static extern int SetTabClosable(IntPtr hTabControl, int closable);
        [DllImport(Dll, CallingConvention = Cc)] public static extern int SetTabScrollable(IntPtr hTabControl, int scrollable);
        [DllImport(Dll, CallingConvention = Cc)] public static extern int SetTabAlignment(IntPtr hTabControl, int alignment);
        [DllImport(Dll, CallingConvention = Cc)] public static extern int SetTabHeaderStyle(IntPtr hTabControl, int style);
        [DllImport(Dll, CallingConvention = Cc)] public static extern int SetTabCloseCallback(IntPtr hTabControl, TabCloseCallback callback);

        [DllImport(Dll, CallingConvention = Cc)] public static extern IntPtr CreatePictureBox(IntPtr parent, int x, int y, int width, int height, int scaleMode, uint bgColor);
        [DllImport(Dll, CallingConvention = Cc)] public static extern int LoadImageFromFile(IntPtr hPictureBox, byte[] filePath, int pathLen);
        [DllImport(Dll, CallingConvention = Cc)] public static extern int LoadImageFromMemory(IntPtr hPictureBox, byte[] imageData, int dataLen);
        [DllImport(Dll, CallingConvention = Cc)] public static extern void ClearImage(IntPtr hPictureBox);
        [DllImport(Dll, CallingConvention = Cc)] public static extern void EnablePictureBox(IntPtr hPictureBox, int enable);
        [DllImport(Dll, CallingConvention = Cc)] public static extern void ShowPictureBox(IntPtr hPictureBox, int show);
        [DllImport(Dll, CallingConvention = Cc)] public static extern void SetPictureBoxBounds(IntPtr hPictureBox, int x, int y, int width, int height);
        [DllImport(Dll, CallingConvention = Cc)] public static extern void SetPictureBoxBackgroundColor(IntPtr hPictureBox, uint bgColor);
        [DllImport(Dll, CallingConvention = Cc)] public static extern void SetImageOpacity(IntPtr hPictureBox, float opacity);
        [DllImport(Dll, CallingConvention = Cc)] public static extern void SetPictureBoxCallback(IntPtr hPictureBox, PictureBoxCallback callback);
        [DllImport(Dll, CallingConvention = Cc)] public static extern void SetPictureBoxScaleMode(IntPtr hPictureBox, int scaleMode);

        [DllImport(Dll, CallingConvention = Cc)] public static extern IntPtr CreateSlider(IntPtr parent, int x, int y, int width, int height, int minValue, int maxValue, int value, int step, uint activeColor, uint bgColor);
        [DllImport(Dll, CallingConvention = Cc)] public static extern int GetSliderValue(IntPtr hSlider);
        [DllImport(Dll, CallingConvention = Cc)] public static extern void SetSliderValue(IntPtr hSlider, int value);
        [DllImport(Dll, CallingConvention = Cc)] public static extern void SetSliderShowStops(IntPtr hSlider, int showStops);
        [DllImport(Dll, CallingConvention = Cc)] public static extern void SetSliderColors(IntPtr hSlider, uint activeColor, uint bgColor, uint buttonColor);
        [DllImport(Dll, CallingConvention = Cc)] public static extern void SetSliderCallback(IntPtr hSlider, SliderCallback callback);

        [DllImport(Dll, CallingConvention = Cc)] public static extern IntPtr CreateSwitch(IntPtr parent, int x, int y, int width, int height, int checkedState, uint activeColor, uint inactiveColor, byte[] activeText, int activeTextLen, byte[] inactiveText, int inactiveTextLen);
        [DllImport(Dll, CallingConvention = Cc)] public static extern int GetSwitchState(IntPtr hSwitch);
        [DllImport(Dll, CallingConvention = Cc)] public static extern void SetSwitchState(IntPtr hSwitch, int checkedState);
        [DllImport(Dll, CallingConvention = Cc)] public static extern void SetSwitchText(IntPtr hSwitch, byte[] activeText, int activeTextLen, byte[] inactiveText, int inactiveTextLen);
        [DllImport(Dll, CallingConvention = Cc)] public static extern void SetSwitchColors(IntPtr hSwitch, uint activeColor, uint inactiveColor);
        [DllImport(Dll, CallingConvention = Cc)] public static extern void SetSwitchTextColors(IntPtr hSwitch, uint activeTextColor, uint inactiveTextColor);
        [DllImport(Dll, CallingConvention = Cc)] public static extern int GetSwitchColors(IntPtr hSwitch, out uint activeColor, out uint inactiveColor, out uint activeTextColor, out uint inactiveTextColor);
        [DllImport(Dll, CallingConvention = Cc)] public static extern void SetSwitchBounds(IntPtr hSwitch, int x, int y, int width, int height);
        [DllImport(Dll, CallingConvention = Cc)] public static extern void SetSwitchCallback(IntPtr hSwitch, SwitchCallback callback);

        [DllImport(Dll, CallingConvention = Cc)] public static extern IntPtr CreateTooltip(IntPtr owner, byte[] text, int textLen, int placement, uint bgColor, uint fgColor);
        [DllImport(Dll, CallingConvention = Cc)] public static extern void SetTooltipTheme(IntPtr hTooltip, int themeMode);
        [DllImport(Dll, CallingConvention = Cc)] public static extern void SetTooltipPlacement(IntPtr hTooltip, int placement);
        [DllImport(Dll, CallingConvention = Cc)] public static extern void SetTooltipTrigger(IntPtr hTooltip, int triggerMode);
        [DllImport(Dll, CallingConvention = Cc)] public static extern void SetTooltipColors(IntPtr hTooltip, uint bgColor, uint fgColor, uint borderColor);
        [DllImport(Dll, CallingConvention = Cc)] public static extern void SetTooltipFont(IntPtr hTooltip, byte[] fontName, int fontNameLen, float fontSize);
        [DllImport(Dll, CallingConvention = Cc)] public static extern void BindTooltipToControl(IntPtr hTooltip, IntPtr hTarget);
        [DllImport(Dll, CallingConvention = Cc)] public static extern void ShowTooltipForControl(IntPtr hTooltip, IntPtr hTarget);
        [DllImport(Dll, CallingConvention = Cc)] public static extern void HideTooltip(IntPtr hTooltip);

        [DllImport(Dll, CallingConvention = Cc)] public static extern IntPtr ShowNotification(IntPtr owner, byte[] title, int titleLen, byte[] message, int messageLen, int type, int position, int durationMs);
        [DllImport(Dll, CallingConvention = Cc)] public static extern void SetNotificationCallback(IntPtr hNotification, NotificationCallback callback);

        [DllImport(Dll, CallingConvention = Cc)] public static extern IntPtr CreateD2DComboBox(IntPtr parent, int x, int y, int width, int height, int readOnly, uint fgColor, uint bgColor, int itemHeight, byte[] fontName, int fontNameLen, int fontSize, int bold, int italic, int underline);
        [DllImport(Dll, CallingConvention = Cc)] public static extern int AddD2DComboItem(IntPtr hComboBox, byte[] text, int textLen);
        [DllImport(Dll, CallingConvention = Cc)] public static extern void RemoveD2DComboItem(IntPtr hComboBox, int index);
        [DllImport(Dll, CallingConvention = Cc)] public static extern void ClearD2DComboBox(IntPtr hComboBox);
        [DllImport(Dll, CallingConvention = Cc)] public static extern int GetD2DComboSelectedIndex(IntPtr hComboBox);
        [DllImport(Dll, CallingConvention = Cc)] public static extern int GetD2DComboItemCount(IntPtr hComboBox);
        [DllImport(Dll, CallingConvention = Cc)] public static extern int GetD2DComboItemText(IntPtr hComboBox, int index, IntPtr buffer, int bufferSize);
        [DllImport(Dll, CallingConvention = Cc)] public static extern int GetD2DComboText(IntPtr hComboBox, IntPtr buffer, int bufferSize);
        [DllImport(Dll, CallingConvention = Cc)] public static extern int GetD2DComboSelectedText(IntPtr hComboBox, IntPtr buffer, int bufferSize);
        [DllImport(Dll, CallingConvention = Cc)] public static extern void SetD2DComboText(IntPtr hComboBox, byte[] text, int textLen);
        [DllImport(Dll, CallingConvention = Cc)] public static extern void SetD2DComboSelectedIndex(IntPtr hComboBox, int index);
        [DllImport(Dll, CallingConvention = Cc)] public static extern void SetD2DComboBoxCallback(IntPtr hComboBox, ComboBoxCallback callback);
        [DllImport(Dll, CallingConvention = Cc)] public static extern void EnableD2DComboBox(IntPtr hComboBox, int enable);
        [DllImport(Dll, CallingConvention = Cc)] public static extern void ShowD2DComboBox(IntPtr hComboBox, int show);
        [DllImport(Dll, CallingConvention = Cc)] public static extern void SetD2DComboBoxBounds(IntPtr hComboBox, int x, int y, int width, int height);
        [DllImport(Dll, CallingConvention = Cc)] public static extern void SetD2DComboBoxColors(IntPtr hComboBox, uint fgColor, uint bgColor, uint selectColor, uint hoverColor, uint borderColor, uint buttonColor);
        [DllImport(Dll, CallingConvention = Cc)] public static extern int GetD2DComboBoxColors(IntPtr hComboBox, out uint fgColor, out uint bgColor, out uint selectColor, out uint hoverColor, out uint borderColor, out uint buttonColor);

        [DllImport(Dll, CallingConvention = Cc)] public static extern IntPtr CreateD2DDateTimePicker(IntPtr parent, int x, int y, int width, int height, int initialPrecision, uint fgColor, uint bgColor, uint borderColor, byte[] fontName, int fontNameLen, int fontSize, int bold, int italic, int underline);
        [DllImport(Dll, CallingConvention = Cc)] public static extern void GetD2DDateTimePickerDateTime(IntPtr hPicker, out int year, out int month, out int day, out int hour, out int minute, out int second);
        [DllImport(Dll, CallingConvention = Cc)] public static extern void SetD2DDateTimePickerDateTime(IntPtr hPicker, int year, int month, int day, int hour, int minute, int second);
        [DllImport(Dll, CallingConvention = Cc)] public static extern void SetD2DDateTimePickerCallback(IntPtr hPicker, ValueChangedCallback callback);
        [DllImport(Dll, CallingConvention = Cc)] public static extern void SetD2DDateTimePickerPrecision(IntPtr hPicker, int precision);
        [DllImport(Dll, CallingConvention = Cc)] public static extern int GetD2DDateTimePickerPrecision(IntPtr hPicker);
        [DllImport(Dll, CallingConvention = Cc)] public static extern int GetD2DDateTimePickerColors(IntPtr hPicker, out uint fgColor, out uint bgColor, out uint borderColor);
        [DllImport(Dll, CallingConvention = Cc)] public static extern void EnableD2DDateTimePicker(IntPtr hPicker, int enable);
        [DllImport(Dll, CallingConvention = Cc)] public static extern void ShowD2DDateTimePicker(IntPtr hPicker, int show);
        [DllImport(Dll, CallingConvention = Cc)] public static extern void SetD2DDateTimePickerBounds(IntPtr hPicker, int x, int y, int width, int height);
        [DllImport(Dll, CallingConvention = Cc)] public static extern void SetD2DDateTimePickerColors(IntPtr hPicker, uint fgColor, uint bgColor, uint borderColor);

        [DllImport(Dll, CallingConvention = Cc)] public static extern IntPtr CreateHotKeyControl(IntPtr parent, int x, int y, int width, int height, uint fgColor, uint bgColor);
        [DllImport(Dll, CallingConvention = Cc)] public static extern void GetHotKey(IntPtr hHotKey, out int vkCode, out int modifiers);
        [DllImport(Dll, CallingConvention = Cc)] public static extern void SetHotKey(IntPtr hHotKey, int vkCode, int modifiers);
        [DllImport(Dll, CallingConvention = Cc)] public static extern void ClearHotKey(IntPtr hHotKey);
        [DllImport(Dll, CallingConvention = Cc)] public static extern void SetHotKeyCallback(IntPtr hHotKey, HotKeyCallback callback);
        [DllImport(Dll, CallingConvention = Cc)] public static extern void EnableHotKeyControl(IntPtr hHotKey, int enable);
        [DllImport(Dll, CallingConvention = Cc)] public static extern void ShowHotKeyControl(IntPtr hHotKey, int show);
        [DllImport(Dll, CallingConvention = Cc)] public static extern void SetHotKeyControlBounds(IntPtr hHotKey, int x, int y, int width, int height);
        [DllImport(Dll, CallingConvention = Cc)] public static extern void SetHotKeyColors(IntPtr hHotKey, uint fgColor, uint bgColor, uint borderColor);
        [DllImport(Dll, CallingConvention = Cc)] public static extern int GetHotKeyColors(IntPtr hHotKey, out uint fgColor, out uint bgColor, out uint borderColor);

        [DllImport(Dll, CallingConvention = Cc)] public static extern IntPtr CreatePanel(IntPtr parent, int x, int y, int width, int height, uint bgColor);
        [DllImport(Dll, CallingConvention = Cc)] public static extern void SetPanelBackgroundColor(IntPtr hPanel, uint bgColor);
        [DllImport(Dll, CallingConvention = Cc)] public static extern int GetPanelBackgroundColor(IntPtr hPanel, out uint bgColor);

        [DllImport(Dll, CallingConvention = Cc)] public static extern IntPtr CreateTreeView(IntPtr parent, int x, int y, int width, int height, uint bgColor, uint textColor, IntPtr callbackContext);
        [DllImport(Dll, CallingConvention = Cc)] public static extern int AddRootNode(IntPtr hTreeView, byte[] text, int textLen, byte[] icon, int iconLen);
        [DllImport(Dll, CallingConvention = Cc)] public static extern int AddChildNode(IntPtr hTreeView, int parentId, byte[] text, int textLen, byte[] icon, int iconLen);
        [DllImport(Dll, CallingConvention = Cc)] public static extern int ExpandAll(IntPtr hTreeView);
        [DllImport(Dll, CallingConvention = Cc)] public static extern int CollapseAll(IntPtr hTreeView);
        [DllImport(Dll, CallingConvention = Cc)] public static extern int SetSelectedNode(IntPtr hTreeView, int nodeId);
        [DllImport(Dll, CallingConvention = Cc)] public static extern int GetSelectedNode(IntPtr hTreeView);
        [DllImport(Dll, CallingConvention = Cc)] public static extern int SetNodeText(IntPtr hTreeView, int nodeId, byte[] text, int textLen);
        [DllImport(Dll, CallingConvention = Cc)] public static extern int GetNodeText(IntPtr hTreeView, int nodeId, IntPtr buffer, int bufferSize);
        [DllImport(Dll, CallingConvention = Cc)] public static extern int SetNodeChecked(IntPtr hTreeView, int nodeId, int checkedState);
        [DllImport(Dll, CallingConvention = Cc)] public static extern int SetTreeViewSidebarMode(IntPtr hTreeView, int enable);
        [DllImport(Dll, CallingConvention = Cc)] public static extern int SetTreeViewRowHeight(IntPtr hTreeView, float height);
        [DllImport(Dll, CallingConvention = Cc)] public static extern int SetTreeViewItemSpacing(IntPtr hTreeView, float spacing);
        [DllImport(Dll, CallingConvention = Cc)] public static extern int SetTreeViewBackgroundColor(IntPtr hTreeView, uint argb);
        [DllImport(Dll, CallingConvention = Cc)] public static extern int SetTreeViewTextColor(IntPtr hTreeView, uint argb);
        [DllImport(Dll, CallingConvention = Cc)] public static extern int SetTreeViewSelectedBgColor(IntPtr hTreeView, uint argb);
        [DllImport(Dll, CallingConvention = Cc)] public static extern int SetTreeViewSelectedForeColor(IntPtr hTreeView, uint argb);
        [DllImport(Dll, CallingConvention = Cc)] public static extern int SetTreeViewHoverBgColor(IntPtr hTreeView, uint argb);
        [DllImport(Dll, CallingConvention = Cc)] public static extern int ScrollToNode(IntPtr hTreeView, int nodeId);
        [DllImport(Dll, CallingConvention = Cc)] public static extern int SetTreeViewFont(IntPtr hTreeView, byte[] fontName, int fontNameLen, float fontSize, int fontWeight, int italic);
        [DllImport(Dll, CallingConvention = Cc)] public static extern int EnableTreeViewDragDrop(IntPtr hTreeView, int enable);
        [DllImport(Dll, CallingConvention = Cc)] public static extern int SetTreeViewCallback(IntPtr hTreeView, int callbackType, TreeNodeCallback callback);

        [DllImport(Dll, CallingConvention = Cc)] public static extern IntPtr CreateDataGridView(IntPtr parent, int x, int y, int width, int height, int virtualMode, int alternateRowColor, uint fgColor, uint bgColor);
        [DllImport(Dll, CallingConvention = Cc)] public static extern int DataGrid_AddTextColumn(IntPtr hGrid, byte[] header, int headerLen, int width);
        [DllImport(Dll, CallingConvention = Cc)] public static extern int DataGrid_AddCheckBoxColumn(IntPtr hGrid, byte[] header, int headerLen, int width);
        [DllImport(Dll, CallingConvention = Cc)] public static extern int DataGrid_AddButtonColumn(IntPtr hGrid, byte[] header, int headerLen, int width);
        [DllImport(Dll, CallingConvention = Cc)] public static extern int DataGrid_AddLinkColumn(IntPtr hGrid, byte[] header, int headerLen, int width);
        [DllImport(Dll, CallingConvention = Cc)] public static extern int DataGrid_AddImageColumn(IntPtr hGrid, byte[] header, int headerLen, int width);
        [DllImport(Dll, CallingConvention = Cc)] public static extern int DataGrid_AddComboBoxColumn(IntPtr hGrid, byte[] header, int headerLen, int width);
        [DllImport(Dll, CallingConvention = Cc)] public static extern int DataGrid_AddTagColumn(IntPtr hGrid, byte[] header, int headerLen, int width);
        [DllImport(Dll, CallingConvention = Cc)] public static extern int DataGrid_AddRow(IntPtr hGrid);
        [DllImport(Dll, CallingConvention = Cc)] public static extern void DataGrid_RemoveRow(IntPtr hGrid, int rowIndex);
        [DllImport(Dll, CallingConvention = Cc)] public static extern void DataGrid_ClearRows(IntPtr hGrid);
        [DllImport(Dll, CallingConvention = Cc)] public static extern int DataGrid_GetRowCount(IntPtr hGrid);
        [DllImport(Dll, CallingConvention = Cc)] public static extern int DataGrid_GetColumnCount(IntPtr hGrid);
        [DllImport(Dll, CallingConvention = Cc)] public static extern void DataGrid_SetCellText(IntPtr hGrid, int row, int col, byte[] text, int textLen);
        [DllImport(Dll, CallingConvention = Cc)] public static extern int DataGrid_GetCellText(IntPtr hGrid, int row, int col, IntPtr buffer, int bufferSize);
        [DllImport(Dll, CallingConvention = Cc)] public static extern void DataGrid_SetCellChecked(IntPtr hGrid, int row, int col, int checkedState);
        [DllImport(Dll, CallingConvention = Cc)] public static extern int DataGrid_GetCellChecked(IntPtr hGrid, int row, int col);
        [DllImport(Dll, CallingConvention = Cc)] public static extern int DataGrid_SetCellImageFromFile(IntPtr hGrid, int row, int col, byte[] filePath, int pathLen);
        [DllImport(Dll, CallingConvention = Cc)] public static extern void DataGrid_SetCellStyle(IntPtr hGrid, int row, int col, uint fgColor, uint bgColor, int bold, int italic);
        [DllImport(Dll, CallingConvention = Cc)] public static extern int DataGrid_GetSelectedRow(IntPtr hGrid);
        [DllImport(Dll, CallingConvention = Cc)] public static extern int DataGrid_GetSelectedCol(IntPtr hGrid);
        [DllImport(Dll, CallingConvention = Cc)] public static extern void DataGrid_SetSelectedCell(IntPtr hGrid, int row, int col);
        [DllImport(Dll, CallingConvention = Cc)] public static extern void DataGrid_SetSelectionMode(IntPtr hGrid, int mode);
        [DllImport(Dll, CallingConvention = Cc)] public static extern void DataGrid_SortByColumn(IntPtr hGrid, int col, int direction);
        [DllImport(Dll, CallingConvention = Cc)] public static extern void DataGrid_SetColumnHeaderText(IntPtr hGrid, int col, byte[] text, int textLen);
        [DllImport(Dll, CallingConvention = Cc)] public static extern int DataGrid_GetColumnHeaderText(IntPtr hGrid, int col, IntPtr buffer, int bufferSize);
        [DllImport(Dll, CallingConvention = Cc)] public static extern void DataGrid_SetFreezeHeader(IntPtr hGrid, int freeze);
        [DllImport(Dll, CallingConvention = Cc)] public static extern void DataGrid_SetFreezeFirstColumn(IntPtr hGrid, int freeze);
        [DllImport(Dll, CallingConvention = Cc)] public static extern void DataGrid_SetFreezeColumnCount(IntPtr hGrid, int count);
        [DllImport(Dll, CallingConvention = Cc)] public static extern void DataGrid_SetFreezeRowCount(IntPtr hGrid, int count);
        [DllImport(Dll, CallingConvention = Cc)] public static extern void DataGrid_SetVirtualRowCount(IntPtr hGrid, int count);
        [DllImport(Dll, CallingConvention = Cc)] public static extern void DataGrid_SetVirtualDataCallback(IntPtr hGrid, DataGridVirtualDataCallback callback);
        [DllImport(Dll, CallingConvention = Cc)] public static extern void DataGrid_SetShowGridLines(IntPtr hGrid, int show);
        [DllImport(Dll, CallingConvention = Cc)] public static extern void DataGrid_SetDefaultRowHeight(IntPtr hGrid, int height);
        [DllImport(Dll, CallingConvention = Cc)] public static extern void DataGrid_SetHeaderHeight(IntPtr hGrid, int height);
        [DllImport(Dll, CallingConvention = Cc)] public static extern void DataGrid_SetDoubleClickEnabled(IntPtr hGrid, int enabled);
        [DllImport(Dll, CallingConvention = Cc)] public static extern void DataGrid_SetHeaderMultiline(IntPtr hGrid, int enabled);
        [DllImport(Dll, CallingConvention = Cc)] public static extern void DataGrid_SetHeaderStyle(IntPtr hGrid, int style);
        [DllImport(Dll, CallingConvention = Cc)] public static extern void DataGrid_SetColumnComboItems(IntPtr hGrid, int col, byte[] items, int itemsLen);
        [DllImport(Dll, CallingConvention = Cc)] public static extern void DataGrid_SetCellClickCallback(IntPtr hGrid, DataGridCellCallback callback);
        [DllImport(Dll, CallingConvention = Cc)] public static extern void DataGrid_SetCellDoubleClickCallback(IntPtr hGrid, DataGridCellCallback callback);
        [DllImport(Dll, CallingConvention = Cc)] public static extern void DataGrid_SetCellValueChangedCallback(IntPtr hGrid, DataGridCellCallback callback);
        [DllImport(Dll, CallingConvention = Cc)] public static extern void DataGrid_SetSelectionChangedCallback(IntPtr hGrid, DataGridCellCallback callback);
        [DllImport(Dll, CallingConvention = Cc)] public static extern void DataGrid_SetColumnHeaderClickCallback(IntPtr hGrid, DataGridColumnHeaderCallback callback);
        [DllImport(Dll, CallingConvention = Cc)] public static extern void DataGrid_Show(IntPtr hGrid, int show);
        [DllImport(Dll, CallingConvention = Cc)] public static extern void DataGrid_SetBounds(IntPtr hGrid, int x, int y, int width, int height);
        [DllImport(Dll, CallingConvention = Cc)] public static extern void DataGrid_Refresh(IntPtr hGrid);
        [DllImport(Dll, CallingConvention = Cc)] public static extern void DataGrid_SetColors(IntPtr hGrid, uint fgColor, uint bgColor, uint headerBgColor, uint headerFgColor, uint selectColor, uint hoverColor, uint gridLineColor);
        [DllImport(Dll, CallingConvention = Cc)] public static extern void DataGrid_SetColumnHeaderAlignment(IntPtr hGrid, int col, int alignment);
        [DllImport(Dll, CallingConvention = Cc)] public static extern void DataGrid_SetColumnCellAlignment(IntPtr hGrid, int col, int alignment);
        [DllImport(Dll, CallingConvention = Cc)] public static extern int DataGrid_ExportCSV(IntPtr hGrid, byte[] filePath, int pathLen);

        [DllImport(Dll, CallingConvention = Cc)] public static extern void SetMouseEnterCallback(IntPtr hControl, ValueChangedCallback callback);
        [DllImport(Dll, CallingConvention = Cc)] public static extern void SetMouseLeaveCallback(IntPtr hControl, ValueChangedCallback callback);
        [DllImport(Dll, CallingConvention = Cc)] public static extern void SetDoubleClickCallback(IntPtr hControl, ControlPointCallback callback);
        [DllImport(Dll, CallingConvention = Cc)] public static extern void SetRightClickCallback(IntPtr hControl, ControlPointCallback callback);
        [DllImport(Dll, CallingConvention = Cc)] public static extern void SetFocusCallback(IntPtr hControl, ValueChangedCallback callback);
        [DllImport(Dll, CallingConvention = Cc)] public static extern void SetBlurCallback(IntPtr hControl, ValueChangedCallback callback);
        [DllImport(Dll, CallingConvention = Cc)] public static extern void SetKeyDownCallback(IntPtr hControl, ControlKeyCallback callback);
        [DllImport(Dll, CallingConvention = Cc)] public static extern void SetKeyUpCallback(IntPtr hControl, ControlKeyCallback callback);
        [DllImport(Dll, CallingConvention = Cc)] public static extern void SetCharCallback(IntPtr hControl, ControlCharCallback callback);
        [DllImport(Dll, CallingConvention = Cc)] public static extern void SetValueChangedCallback(IntPtr hControl, ValueChangedCallback callback);

        [DllImport(Dll, CallingConvention = Cc)] public static extern void show_message_box_bytes(IntPtr parent, byte[] title, int titleLen, byte[] message, int messageLen, byte[] icon, int iconLen);
        [DllImport(Dll, CallingConvention = Cc)] public static extern void show_confirm_box_bytes(IntPtr parent, byte[] title, int titleLen, byte[] message, int messageLen, byte[] icon, int iconLen, MessageBoxCallback callback);
    }
}
