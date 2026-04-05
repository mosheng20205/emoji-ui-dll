using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;

namespace EmojiWindowChromeStyleBrowserDemo.UI
{
    internal static class Win32Native
    {
        private const uint SwpNoSize = 0x0001;
        private const uint SwpNoMove = 0x0002;
        private const uint SwpNoActivate = 0x0010;
        private const uint WmSetRedraw = 0x000B;
        private const uint RdwInvalidate = 0x0001;
        private const uint RdwErase = 0x0004;
        private const uint RdwAllChildren = 0x0080;
        private const uint RdwFrame = 0x0400;
        private const uint RdwUpdateNow = 0x0100;

        public static readonly IntPtr HwndTop = IntPtr.Zero;

        private delegate bool EnumChildProc(IntPtr hwnd, IntPtr lParam);

        [DllImport("user32.dll", SetLastError = true)]
        private static extern bool MoveWindow(IntPtr hWnd, int x, int y, int nWidth, int nHeight, bool repaint);

        [DllImport("user32.dll", SetLastError = true)]
        private static extern bool ShowWindow(IntPtr hWnd, int nCmdShow);

        [DllImport("user32.dll", SetLastError = true)]
        private static extern bool SetWindowPos(IntPtr hWnd, IntPtr hWndInsertAfter, int x, int y, int cx, int cy, uint uFlags);

        [DllImport("user32.dll", SetLastError = true)]
        private static extern IntPtr SendMessage(IntPtr hWnd, uint msg, IntPtr wParam, IntPtr lParam);

        [DllImport("user32.dll", SetLastError = true)]
        private static extern bool RedrawWindow(IntPtr hWnd, IntPtr lprcUpdate, IntPtr hrgnUpdate, uint flags);

        [DllImport("user32.dll", SetLastError = true)]
        private static extern bool EnumChildWindows(IntPtr hWndParent, EnumChildProc lpEnumFunc, IntPtr lParam);

        public static void MoveChild(IntPtr hwnd, int x, int y, int width, int height)
        {
            if (hwnd == IntPtr.Zero)
            {
                return;
            }

            MoveWindow(hwnd, x, y, width, height, false);
        }

        public static void EnsureVisible(IntPtr hwnd)
        {
            if (hwnd == IntPtr.Zero)
            {
                return;
            }

            ShowWindow(hwnd, 5);
        }

        public static void RaiseChild(IntPtr hwnd)
        {
            if (hwnd == IntPtr.Zero)
            {
                return;
            }

            SetWindowPos(hwnd, HwndTop, 0, 0, 0, 0, SwpNoMove | SwpNoSize | SwpNoActivate);
        }

        public static List<IntPtr> CollectWindowTree(IntPtr hwnd)
        {
            var handles = new List<IntPtr>();
            if (hwnd == IntPtr.Zero)
            {
                return handles;
            }

            handles.Add(hwnd);
            EnumChildWindows(hwnd, (child, _) =>
            {
                if (!handles.Contains(child))
                {
                    handles.Add(child);
                }

                return true;
            }, IntPtr.Zero);
            return handles;
        }

        public static void SetRedraw(IntPtr hwnd, bool enabled)
        {
            if (hwnd == IntPtr.Zero)
            {
                return;
            }

            SendMessage(hwnd, WmSetRedraw, enabled ? new IntPtr(1) : IntPtr.Zero, IntPtr.Zero);
        }

        public static void RefreshWindowTree(IntPtr hwnd)
        {
            if (hwnd == IntPtr.Zero)
            {
                return;
            }

            RedrawWindow(hwnd, IntPtr.Zero, IntPtr.Zero, RdwInvalidate | RdwErase | RdwFrame | RdwAllChildren | RdwUpdateNow);
        }
    }
}
