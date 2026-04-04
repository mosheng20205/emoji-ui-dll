using System;
using System.IO;
using System.Runtime.InteropServices;

namespace EmojiWindowDemo
{
    internal static class Win32Native
    {
        private static IntPtr _windowIconBig = IntPtr.Zero;
        private static IntPtr _windowIconSmall = IntPtr.Zero;

        public static readonly IntPtr HWND_TOP = IntPtr.Zero;
        public const int SW_HIDE = 0;
        public const int SW_SHOW = 5;
        public const uint WM_SETICON = 0x0080;
        public const uint WM_SETREDRAW = 0x000B;
        public const uint WM_CANCELMODE = 0x001F;
        public const uint WM_KEYDOWN = 0x0100;
        public const uint WM_KEYUP = 0x0101;
        public const int VK_ESCAPE = 0x1B;
        public const int ICON_SMALL = 0;
        public const int ICON_BIG = 1;
        public const uint IMAGE_ICON = 1;
        public const uint LR_LOADFROMFILE = 0x0010;
        public const uint LR_DEFAULTSIZE = 0x0040;
        public const uint SWP_NOSIZE = 0x0001;
        public const uint SWP_NOMOVE = 0x0002;
        public const uint SWP_NOACTIVATE = 0x0010;
        public const uint SWP_SHOWWINDOW = 0x0040;
        public const uint RDW_INVALIDATE = 0x0001;
        public const uint RDW_ERASE = 0x0004;
        public const uint RDW_ALLCHILDREN = 0x0080;
        public const uint RDW_UPDATENOW = 0x0100;
        public const uint RDW_FRAME = 0x0400;

        [DllImport("user32.dll")] public static extern int ShowWindow(IntPtr hWnd, int nCmdShow);
        [DllImport("user32.dll")] public static extern bool MoveWindow(IntPtr hWnd, int x, int y, int width, int height, bool repaint);
        [DllImport("user32.dll")] public static extern bool SetWindowPos(IntPtr hWnd, IntPtr hWndInsertAfter, int x, int y, int width, int height, uint flags);
        [DllImport("user32.dll")] public static extern IntPtr SetFocus(IntPtr hWnd);
        [DllImport("user32.dll")] public static extern IntPtr GetFocus();
        [DllImport("user32.dll")] public static extern bool IsWindowVisible(IntPtr hWnd);
        [DllImport("user32.dll")] public static extern bool IsWindowEnabled(IntPtr hWnd);
        [DllImport("user32.dll", CharSet = CharSet.Unicode, SetLastError = true)] public static extern IntPtr LoadImage(IntPtr hInst, string lpszName, uint uType, int cxDesired, int cyDesired, uint fuLoad);
        [DllImport("user32.dll")] public static extern IntPtr SendMessage(IntPtr hWnd, uint msg, IntPtr wParam, IntPtr lParam);
        [DllImport("user32.dll")] public static extern bool PostMessage(IntPtr hWnd, uint msg, IntPtr wParam, IntPtr lParam);
        [DllImport("user32.dll")] public static extern bool RedrawWindow(IntPtr hWnd, IntPtr lprcUpdate, IntPtr hrgnUpdate, uint flags);

        public static void SetRedraw(IntPtr hWnd, bool enabled)
        {
            if (hWnd != IntPtr.Zero)
            {
                SendMessage(hWnd, WM_SETREDRAW, enabled ? new IntPtr(1) : IntPtr.Zero, IntPtr.Zero);
            }
        }

        public static void RefreshWindowTree(IntPtr hWnd)
        {
            if (hWnd != IntPtr.Zero)
            {
                RedrawWindow(hWnd, IntPtr.Zero, IntPtr.Zero, RDW_INVALIDATE | RDW_ERASE | RDW_ALLCHILDREN | RDW_UPDATENOW | RDW_FRAME);
            }
        }

        public static bool TrySetWindowIcon(IntPtr hwnd, string iconPath)
        {
            if (hwnd == IntPtr.Zero || string.IsNullOrWhiteSpace(iconPath) || !File.Exists(iconPath))
            {
                return false;
            }

            _windowIconBig = LoadImage(IntPtr.Zero, iconPath, IMAGE_ICON, 32, 32, LR_LOADFROMFILE | LR_DEFAULTSIZE);
            _windowIconSmall = LoadImage(IntPtr.Zero, iconPath, IMAGE_ICON, 16, 16, LR_LOADFROMFILE | LR_DEFAULTSIZE);

            if (_windowIconBig != IntPtr.Zero)
            {
                SendMessage(hwnd, WM_SETICON, new IntPtr(ICON_BIG), _windowIconBig);
            }

            if (_windowIconSmall != IntPtr.Zero)
            {
                SendMessage(hwnd, WM_SETICON, new IntPtr(ICON_SMALL), _windowIconSmall);
            }

            return _windowIconBig != IntPtr.Zero || _windowIconSmall != IntPtr.Zero;
        }
    }
}
