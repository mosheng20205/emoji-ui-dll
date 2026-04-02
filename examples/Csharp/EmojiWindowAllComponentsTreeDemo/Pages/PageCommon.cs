using System;
using System.Collections.Generic;
using System.IO;

namespace EmojiWindowDemo
{
    internal static class PageCommon
    {
        public const int AlignLeft = 0;
        public const int AlignCenter = 1;
        public const int AlignRight = 2;
        public const int ButtonStyleSolid = 0;
        public const int ButtonStylePlain = 1;
        public const int ButtonStyleText = 2;
        public const int ButtonStyleLink = 3;
        public const int ButtonSizeLarge = 0;
        public const int ButtonSizeDefault = 1;
        public const int ButtonSizeSmall = 2;

        public const int DtpYmd = 1;
        public const int DtpYmdHm = 3;
        public const int DtpYmdHms = 4;

        public const int ScaleNone = 0;
        public const int ScaleStretch = 1;
        public const int ScaleFit = 2;
        public const int ScaleCenter = 3;

        public const int GroupBoxStyleOutline = 0;
        public const int GroupBoxStyleCard = 1;
        public const int GroupBoxStyleHeaderBar = 2;

        public const int CheckBoxStyleButton = 2;
        public const int CheckBoxStyleCard = 3;
        public const int RadioStyleBorder = 1;
        public const int RadioStyleButton = 2;

        public static readonly byte[] TinyPngBytes = Convert.FromBase64String(
            "iVBORw0KGgoAAAANSUhEUgAAAAEAAAABCAQAAAC1HAwCAAAAC0lEQVR42mP8/x8AAwMCAO+nXW0AAAAASUVORK5CYII=");

        public static string FormatColor(uint color) => $"0x{color:X8}";

        public static string AlignmentName(int alignment)
        {
            switch (alignment)
            {
                case AlignLeft:
                    return "left";
                case AlignCenter:
                    return "center";
                case AlignRight:
                    return "right";
                default:
                    return "unknown";
            }
        }

        public static string FormatDateTime(int year, int month, int day, int hour, int minute, int second)
            => $"{year:D4}-{month:D2}-{day:D2} {hour:D2}:{minute:D2}:{second:D2}";

        public static string FormatHotKey(int vkCode, int modifiers)
        {
            if (vkCode == 0)
            {
                return "(未设置)";
            }

            var parts = new List<string>();
            if ((modifiers & 2) != 0) parts.Add("Ctrl");
            if ((modifiers & 1) != 0) parts.Add("Shift");
            if ((modifiers & 4) != 0) parts.Add("Alt");

            string key;
            if (vkCode >= 0x70 && vkCode <= 0x7B)
            {
                key = "F" + (vkCode - 0x6F);
            }
            else if (vkCode >= 0x30 && vkCode <= 0x5A)
            {
                key = ((char)vkCode).ToString().ToUpperInvariant();
            }
            else
            {
                key = "VK_" + vkCode.ToString("X2");
            }

            parts.Add(key);
            return string.Join("+", parts);
        }

        public static string FindFirstExistingPath(DemoApp app, params string[] relativePaths)
        {
            foreach (string relativePath in relativePaths)
            {
                string path = app.FindFileUpwards(relativePath);
                if (!string.IsNullOrEmpty(path) && File.Exists(path))
                {
                    return path;
                }
            }

            return string.Empty;
        }

        public static void SetGridCellText(DemoApp app, IntPtr grid, int row, int col, string text)
        {
            byte[] bytes = app.U(text);
            EmojiWindowNative.DataGrid_SetCellText(grid, row, col, bytes, bytes.Length);
        }

        public static string BuildVirtualGridText(int row, int col)
        {
            string[] statuses = { "🕒 待处理", "🚧 进行中", "✅ 已完成", "⏸️ 已暂停" };
            string[] priorities = { "🔵 P1", "🟢 P2", "🟠 P3", "🟣 P4" };

            switch (col)
            {
                case 0: return $"🧾 虚拟任务 {row + 1:N0}";
                case 1: return statuses[row % statuses.Length];
                case 2: return priorities[(row / 3) % priorities.Length];
                case 3: return $"👤 节点 {row % 256:D3}";
                case 4: return $"🛠 路由 /node/{row % 1024:D4}";
                case 5: return "🖼️ 虚拟图片";
                default: return $"📝 第 {row + 1:N0} 行虚拟备注，用于测试 1,000,000 行滚动和绘制性能";
            }
        }
    }
}
