using System;
using System.Runtime.InteropServices;
using System.Text;

namespace BaseTest
{
    internal static class NativeTabControl
    {
        private const string DllName = "emoji_window.dll";
        private const CallingConvention CallConv = CallingConvention.StdCall;

        [UnmanagedFunctionPointer(CallConv)]
        internal delegate void TabCallback(IntPtr hTabControl, int selectedIndex);

        [UnmanagedFunctionPointer(CallConv)]
        internal delegate void TabCloseCallback(IntPtr hTabControl, int index);

        [StructLayout(LayoutKind.Sequential)]
        internal struct RECT
        {
            public int Left;
            public int Top;
            public int Right;
            public int Bottom;
        }

        internal static byte[] ToUtf8(string text)
        {
            return string.IsNullOrEmpty(text) ? Array.Empty<byte>() : Encoding.UTF8.GetBytes(text);
        }

        [DllImport(DllName, CallingConvention = CallConv)]
        internal static extern IntPtr CreateTabControl(IntPtr parent, int x, int y, int width, int height);

        [DllImport(DllName, CallingConvention = CallConv)]
        internal static extern int AddTabItem(IntPtr hTabControl, byte[] titleBytes, int titleLen, IntPtr hContentWindow);

        [DllImport(DllName, CallingConvention = CallConv)]
        [return: MarshalAs(UnmanagedType.Bool)]
        internal static extern bool RemoveTabItem(IntPtr hTabControl, int index);

        [DllImport(DllName, CallingConvention = CallConv)]
        [return: MarshalAs(UnmanagedType.Bool)]
        internal static extern bool SelectTab(IntPtr hTabControl, int index);

        [DllImport(DllName, CallingConvention = CallConv)]
        internal static extern int GetCurrentTabIndex(IntPtr hTabControl);

        [DllImport(DllName, CallingConvention = CallConv)]
        internal static extern int GetTabCount(IntPtr hTabControl);

        [DllImport(DllName, CallingConvention = CallConv)]
        internal static extern IntPtr GetTabContentWindow(IntPtr hTabControl, int index);

        [DllImport(DllName, CallingConvention = CallConv)]
        internal static extern int SetTabTitle(IntPtr hTabControl, int index, byte[] titleBytes, int titleLen);

        [DllImport(DllName, CallingConvention = CallConv)]
        internal static extern int SetTabControlBounds(IntPtr hTabControl, int x, int y, int width, int height);

        [DllImport(DllName, CallingConvention = CallConv)]
        internal static extern int SetTabItemSize(IntPtr hTabControl, int width, int height);

        [DllImport(DllName, CallingConvention = CallConv)]
        internal static extern int SetTabFont(IntPtr hTabControl, byte[] fontBytes, int fontLen, float fontSize);

        [DllImport(DllName, CallingConvention = CallConv)]
        internal static extern int SetTabColors(IntPtr hTabControl, uint selectedBg, uint unselectedBg, uint selectedText, uint unselectedText);

        [DllImport(DllName, CallingConvention = CallConv)]
        internal static extern int SetTabIndicatorColor(IntPtr hTabControl, uint color);

        [DllImport(DllName, CallingConvention = CallConv)]
        internal static extern int SetTabPadding(IntPtr hTabControl, int horizontal, int vertical);

        [DllImport(DllName, CallingConvention = CallConv)]
        internal static extern int SetTabClosable(IntPtr hTabControl, int closable);

        [DllImport(DllName, CallingConvention = CallConv)]
        internal static extern int SetTabDraggable(IntPtr hTabControl, int draggable);

        [DllImport(DllName, CallingConvention = CallConv)]
        internal static extern int SetTabScrollable(IntPtr hTabControl, int scrollable);

        [DllImport(DllName, CallingConvention = CallConv)]
        internal static extern int SetTabCloseCallback(IntPtr hTabControl, TabCloseCallback callback);

        [DllImport(DllName, CallingConvention = CallConv)]
        internal static extern void SetTabCallback(IntPtr hTabControl, TabCallback callback);

        [DllImport(DllName, CallingConvention = CallConv)]
        internal static extern void UpdateTabControlLayout(IntPtr hTabControl);

        [DllImport(DllName, CallingConvention = CallConv)]
        [return: MarshalAs(UnmanagedType.Bool)]
        internal static extern bool RedrawTabControl(IntPtr hTabControl);

        [DllImport("user32.dll", SetLastError = true)]
        [return: MarshalAs(UnmanagedType.Bool)]
        internal static extern bool GetClientRect(IntPtr hWnd, out RECT lpRect);

        [DllImport("user32.dll", CharSet = CharSet.Auto)]
        internal static extern IntPtr SendMessage(IntPtr hWnd, uint msg, IntPtr wParam, IntPtr lParam);
    }
}
