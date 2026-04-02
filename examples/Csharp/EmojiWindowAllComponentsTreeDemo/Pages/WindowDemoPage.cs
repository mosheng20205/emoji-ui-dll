using System;

namespace EmojiWindowDemo
{
    internal static class WindowDemoPage
    {
        public static void Build(AllDemoShell shell, IntPtr page)
        {
            var app = shell.App;
            IntPtr readout = app.Label(24, 24, 1080, 110, string.Empty, DemoColors.Black, DemoColors.WindowBg, 13, PageCommon.AlignLeft, true, page);
            IntPtr state = app.Label(24, 760, 1360, 22, "窗口页状态将在这里更新。", DemoColors.Blue, DemoColors.WindowBg, 12, PageCommon.AlignLeft, false, page);

            uint clientBg = DemoColors.WindowBg;
            string clientBgName = "默认灰蓝";

            void Refresh(string note)
            {
                EmojiWindowNative.GetWindowBounds(app.Window, out int x, out int y, out int width, out int height);
                string title = EmojiWindowNative.ReadUtf8(EmojiWindowNative.GetWindowTitle, app.Window);
                string visible = EmojiWindowNative.GetWindowVisible(app.Window) == 1 ? "显示" : "隐藏";
                uint titlebar = EmojiWindowNative.GetWindowTitlebarColor(app.Window);
                uint titlebarText = EmojiWindowNative.GetTitleBarTextColor(app.Window);

                shell.SetLabelText(
                    readout,
                    $"标题: {title}\r\n" +
                    $"句柄: 0x{app.Window.ToInt64():X}  可见性: {visible}\r\n" +
                    $"位置尺寸: x={x}, y={y}, w={width}, h={height}\r\n" +
                    $"标题栏背景: {PageCommon.FormatColor(titlebar)}  标题栏文字: {PageCommon.FormatColor(titlebarText)}\r\n" +
                    $"最近一次客户区背景: {clientBgName} / {PageCommon.FormatColor(clientBg)}");
                shell.SetLabelText(state, note);
                shell.SetStatus(note);
            }

            void SetWindowBounds(int x, int y, int width, int height, string note)
            {
                EmojiWindowNative.SetWindowBounds(app.Window, x, y, width, height);
                shell.ResizeShell(width, height);
                Refresh(note);
            }

            void SetClientBackground(uint color, string name, string note)
            {
                clientBg = color;
                clientBgName = name;
                EmojiWindowNative.SetWindowBackgroundColor(app.Window, color);
                Refresh(note);
            }

            app.Button(24, 156, 166, 36, "产品标题", "🪟", DemoColors.Blue, () =>
            {
                byte[] title = app.U("🪟 Tree AllDemo Enhanced / 产品演示窗口");
                EmojiWindowNative.set_window_title(app.Window, title, title.Length);
                Refresh("窗口标题已切到产品演示风格");
            }, page);
            app.Button(206, 156, 166, 36, "调试标题", "🛠️", DemoColors.Green, () =>
            {
                byte[] title = app.U("🛠️ EmojiWindow Debug Surface / 属性回归中");
                EmojiWindowNative.set_window_title(app.Window, title, title.Length);
                Refresh("窗口标题已切到调试风格");
            }, page);
            app.Button(388, 156, 166, 36, "彩色标题", "✨", DemoColors.Purple, () =>
            {
                byte[] title = app.U("✨ EmojiWindow 属性页 / 🌈 Unicode 彩色标题");
                EmojiWindowNative.set_window_title(app.Window, title, title.Length);
                Refresh("窗口标题已切到彩色 Unicode 方案");
            }, page);

            app.Button(24, 208, 176, 36, "紧凑 1600x900", "📦", DemoColors.Orange, () =>
            {
                EmojiWindowNative.GetWindowBounds(app.Window, out int x, out int y, out _, out _);
                SetWindowBounds(x, y, 1600, 900, "窗口尺寸已切到紧凑演示 1600x900");
            }, page);
            app.Button(216, 208, 176, 36, "标准 1820x980", "🖥️", DemoColors.Blue, () =>
            {
                EmojiWindowNative.GetWindowBounds(app.Window, out int x, out int y, out _, out _);
                SetWindowBounds(x, y, 1820, 980, "窗口尺寸已恢复到标准 1820x980");
            }, page);
            app.Button(408, 208, 176, 36, "加宽 1920x1040", "🧱", DemoColors.Green, () =>
            {
                EmojiWindowNative.GetWindowBounds(app.Window, out int x, out int y, out _, out _);
                SetWindowBounds(x, y, 1920, 1040, "窗口尺寸已切到加宽展示 1920x1040");
            }, page);

            app.Button(624, 156, 160, 36, "恢复到 40,40", "↖️", DemoColors.Gray, () =>
            {
                EmojiWindowNative.GetWindowBounds(app.Window, out _, out _, out int width, out int height);
                SetWindowBounds(40, 40, width, height, "窗口位置已恢复到 (40, 40)");
            }, page);
            app.Button(800, 156, 160, 36, "向右移动 80", "➡️", DemoColors.Blue, () =>
            {
                EmojiWindowNative.GetWindowBounds(app.Window, out int x, out int y, out int width, out int height);
                SetWindowBounds(x + 80, y, width, height, "窗口已向右移动 80 像素");
            }, page);
            app.Button(624, 208, 160, 36, "向下移动 60", "⬇️", DemoColors.Green, () =>
            {
                EmojiWindowNative.GetWindowBounds(app.Window, out int x, out int y, out int width, out int height);
                SetWindowBounds(x, y + 60, width, height, "窗口已向下移动 60 像素");
            }, page);
            app.Button(800, 208, 160, 36, "立即读取", "📡", DemoColors.Purple, () => Refresh("已重新读取窗口当前属性"), page);

            app.Button(1010, 156, 150, 36, "浅色主题", "☀️", DemoColors.Blue, () =>
            {
                EmojiWindowNative.SetDarkMode(0);
                Refresh("主窗口主题已切到浅色");
            }, page);
            app.Button(1176, 156, 150, 36, "深色主题", "🌙", DemoColors.Black, () =>
            {
                EmojiWindowNative.SetDarkMode(1);
                Refresh("主窗口主题已切到深色");
            }, page);

            app.Button(1010, 208, 100, 36, "科技蓝", "💙", DemoColors.Blue, () =>
            {
                EmojiWindowNative.set_window_titlebar_color(app.Window, DemoColors.Blue);
                Refresh("标题栏背景已切到科技蓝");
            }, page);
            app.Button(1124, 208, 100, 36, "深空黑", "🖤", DemoColors.Black, () =>
            {
                EmojiWindowNative.set_window_titlebar_color(app.Window, EmojiWindowNative.ARGB(255, 43, 47, 54));
                Refresh("标题栏背景已切到深空黑");
            }, page);
            app.Button(1238, 208, 100, 36, "青绿色", "💚", DemoColors.Green, () =>
            {
                EmojiWindowNative.set_window_titlebar_color(app.Window, EmojiWindowNative.ARGB(255, 39, 174, 96));
                Refresh("标题栏背景已切到青绿色");
            }, page);

            app.Button(1010, 260, 84, 36, "亮字", "🤍", DemoColors.Gray, () =>
            {
                EmojiWindowNative.SetTitleBarTextColor(app.Window, DemoColors.White);
                Refresh("标题栏文字已切到亮色");
            }, page);
            app.Button(1108, 260, 84, 36, "暗字", "🖤", DemoColors.Black, () =>
            {
                EmojiWindowNative.SetTitleBarTextColor(app.Window, EmojiWindowNative.ARGB(255, 29, 30, 31));
                Refresh("标题栏文字已切到暗色");
            }, page);
            app.Button(1206, 260, 84, 36, "蓝字", "💙", DemoColors.Blue, () =>
            {
                EmojiWindowNative.SetTitleBarTextColor(app.Window, EmojiWindowNative.ARGB(255, 140, 197, 255));
                Refresh("标题栏文字已切到蓝色");
            }, page);
            app.Button(1304, 260, 84, 36, "跟随", "↺", DemoColors.Green, () =>
            {
                EmojiWindowNative.SetTitleBarTextColor(app.Window, 0);
                Refresh("标题栏文字已恢复为跟随主题");
            }, page);

            app.Button(1010, 312, 110, 36, "纯白背景", "🤍", DemoColors.Green, () => SetClientBackground(DemoColors.White, "纯白", "客户区背景已切到纯白"), page);
            app.Button(1134, 312, 110, 36, "灰蓝背景", "🩶", DemoColors.Gray, () => SetClientBackground(DemoColors.WindowBg, "灰蓝", "客户区背景已切到默认灰蓝"), page);
            app.Button(1258, 312, 110, 36, "深色背景", "🌘", DemoColors.Black, () => SetClientBackground(EmojiWindowNative.ARGB(255, 31, 35, 41), "深色", "客户区背景已切到深色演示底色"), page);

            app.Label(24, 356, 1320, 22, "1. GetWindowTitle / set_window_title: 读取和修改主窗口标题。", DemoColors.Black, DemoColors.WindowBg, 12, PageCommon.AlignLeft, false, page);
            app.Label(24, 390, 1320, 22, "2. GetWindowBounds / SetWindowBounds: 读取和修改窗口位置与尺寸，并同步重排树形 demo 壳层。", DemoColors.Black, DemoColors.WindowBg, 12, PageCommon.AlignLeft, false, page);
            app.Label(24, 424, 1320, 22, "3. GetWindowVisible / GetWindowTitlebarColor / GetTitleBarTextColor: 实时读取可见态和标题栏颜色。", DemoColors.Black, DemoColors.WindowBg, 12, PageCommon.AlignLeft, false, page);
            app.Label(24, 458, 1320, 22, "4. SetWindowBackgroundColor / SetDarkMode: 演示客户区背景和主题切换。", DemoColors.Black, DemoColors.WindowBg, 12, PageCommon.AlignLeft, false, page);

            Refresh("窗口页已加载，可直接测试窗口属性读取与设置");
        }
    }
}
