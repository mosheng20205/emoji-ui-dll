namespace EmojiWindowDemo
{
    internal static class OverviewPages
    {
        public static void BuildBasic(AllDemoShell shell, System.IntPtr page) =>
            BuildOverview(shell, page, "基础组件", "这一组覆盖窗口、基础输入、状态切换和容器。", new[]
            {
                "Window / Button / Label / EditBox / ColorEmojiEditBox",
                "CheckBox / RadioButton / ProgressBar / Slider / Switch",
                "GroupBox / Panel",
                "GroupBox 页面按文档要求使用安全挂载方式。"
            });

        public static void BuildSelect(AllDemoShell shell, System.IntPtr page) =>
            BuildOverview(shell, page, "选择组件", "这一组覆盖列表、组合、日期时间和热键输入。", new[]
            {
                "ListBox / ComboBox / D2DComboBox",
                "D2DDateTimePicker / HotKeyControl",
                "页面内包含读写、回调、显隐、启用和样式切换。"
            });

        public static void BuildDisplay(AllDemoShell shell, System.IntPtr page) =>
            BuildOverview(shell, page, "显示组件", "这一组覆盖图片、提示和通知。", new[]
            {
                "PictureBox 支持图片加载、缩放、背景、透明度、显隐和启用。",
                "Tooltip 支持绑定控件、主题、颜色和主动显示。",
                "Notification 支持信息、成功、警告、错误通知。"
            });

        public static void BuildPopup(AllDemoShell shell, System.IntPtr page) =>
            BuildOverview(shell, page, "弹窗组件", "这一组单独验证消息框和确认框。", new[]
            {
                "MessageBox：信息、警告、错误。",
                "ConfirmBox：确认回调和状态回写。"
            });

        public static void BuildNavigation(AllDemoShell shell, System.IntPtr page) =>
            BuildOverview(shell, page, "导航组件", "这一组覆盖标签、树、菜单栏和右键菜单。", new[]
            {
                "TabControl：新增、关闭、切换。",
                "TreeView：节点创建、展开折叠、选中、重命名。",
                "MenuBar / PopupMenu：菜单回调、文案修改和绑定。"
            });

        public static void BuildData(AllDemoShell shell, System.IntPtr page) =>
            BuildOverview(shell, page, "数据组件", "当前数据页主角是 DataGridView。", new[]
            {
                "普通模式表格。",
                "单元格 / 列头回调。",
                "排序、增删行。",
                "冻结列表头、前 N 列、前 N 行。"
            });

        private static void BuildOverview(AllDemoShell shell, System.IntPtr page, string title, string desc, string[] lines)
        {
            var app = shell.App;
            app.Label(24, 24, 980, 32, title, DemoColors.Black, DemoColors.WindowBg, 18, 0, false, page);
            app.Label(24, 60, 1280, 28, desc, DemoColors.Gray, DemoColors.WindowBg, 12, 0, true, page);

            int y = 118;
            foreach (string line in lines)
            {
                app.Label(40, y, 1280, 24, "• " + line, DemoColors.Black, DemoColors.WindowBg, 13, 0, true, page);
                y += 34;
            }

            app.Label(24, y + 16, 1360, 56, "左侧树形导航会切换到对应页面。每个 control 页面都拆成单独 cs 文件，方便继续扩展，并且 GroupBox 页面明确避开了 C# 里直接把 HWND 控件挂到 GroupBox 句柄上的空白坑。", DemoColors.Gray, DemoColors.WindowBg, 12, 0, true, page);
        }
    }
}
