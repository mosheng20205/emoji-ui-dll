namespace EmojiWindowDemo
{
    internal static class AllDemoCatalog
    {
        public static DemoCategoryDefinition[] Create()
        {
            return new[]
            {
                new DemoCategoryDefinition(
                    "基础组件",
                    "📦",
                    new DemoPageDefinition("overview_basic", "基础组件概览", "窗口、按钮、文本输入、状态类控件和容器类控件。", OverviewPages.BuildBasic),
                    new[]
                    {
                        new DemoPageDefinition("page_window", "窗口 EmojiWindow", "窗口标题、位置、尺寸、标题栏颜色和主题切换。", WindowDemoPage.Build),
                        new DemoPageDefinition("page_button", "按钮 EmojiButton", "按钮文本、颜色、加载状态和点击动作。", ButtonDemoPage.Build),
                        new DemoPageDefinition("page_label", "标签 Label", "文本、对齐、颜色和多行标签。", LabelDemoPage.Build),
                        new DemoPageDefinition("page_editbox", "编辑框 EditBox", "单行、多行、回车读取与程序写入。", EditBoxDemoPage.Build),
                        new DemoPageDefinition("page_color_emoji_edit", "彩色 Emoji 编辑框", "彩色 emoji 文本、多行内容和回写。", ColorEmojiEditDemoPage.Build),
                        new DemoPageDefinition("page_checkbox", "复选框 CheckBox", "多复选项状态读取与切换。", CheckBoxDemoPage.Build),
                        new DemoPageDefinition("page_radiobutton", "单选框 RadioButton", "分组单选和当前模式读取。", RadioButtonDemoPage.Build),
                        new DemoPageDefinition("page_progressbar", "进度条 ProgressBar", "进度值、不确定模式和回调。", ProgressBarDemoPage.Build),
                        new DemoPageDefinition("page_slider", "滑块 Slider", "数值、颜色、停靠点和回调。", SliderDemoPage.Build),
                        new DemoPageDefinition("page_switch", "开关 Switch", "状态切换、文本切换和回调。", SwitchDemoPage.Build),
                        new DemoPageDefinition("page_state", "组合状态 State", "CheckBox / RadioButton / ProgressBar / Slider / Switch 的组合状态读取。", StateDemoPage.Build),
                        new DemoPageDefinition("page_groupbox", "分组框 GroupBox", "分组样式与 C# 下安全挂载子控件。", GroupBoxDemoPage.Build),
                        new DemoPageDefinition("page_panel", "面板 Panel", "分区容器和背景色切换。", PanelDemoPage.Build),
                    }),
                new DemoCategoryDefinition(
                    "选择组件",
                    "🎛️",
                    new DemoPageDefinition("overview_select", "选择组件概览", "列表框、组合框、日期时间和热键控件。", OverviewPages.BuildSelect),
                    new[]
                    {
                        new DemoPageDefinition("page_listbox", "列表框 ListBox", "列表项增删、选中读取和回调。", ListBoxDemoPage.Build),
                        new DemoPageDefinition("page_combobox", "组合框 ComboBox", "只读 / 可编辑组合框和文本读取。", ComboBoxDemoPage.Build),
                        new DemoPageDefinition("page_d2d_combobox", "D2D 组合框", "D2DComboBox 文本、选中和颜色。", D2DComboBoxDemoPage.Build),
                        new DemoPageDefinition("page_datetime", "日期时间选择器", "日期时间读写、精度、颜色和显隐。", DateTimePickerDemoPage.Build),
                        new DemoPageDefinition("page_hotkey", "热键框 HotKeyControl", "录入、预设、清空、启用和颜色。", HotKeyDemoPage.Build),
                    }),
                new DemoCategoryDefinition(
                    "显示组件",
                    "🖼️",
                    new DemoPageDefinition("overview_display", "显示组件概览", "图片框、Tooltip 和 Notification。", OverviewPages.BuildDisplay),
                    new[]
                    {
                        new DemoPageDefinition("page_picturebox", "图片框 PictureBox", "图片加载、缩放、透明度、背景、事件和显隐。", PictureBoxDemoPage.Build),
                        new DemoPageDefinition("page_tooltip", "提示 Tooltip", "控件绑定、主题、颜色和主动显示。", TooltipDemoPage.Build),
                        new DemoPageDefinition("page_notification", "通知 Notification", "多种通知类型与回调。", NotificationDemoPage.Build),
                    }),
                new DemoCategoryDefinition(
                    "弹窗组件",
                    "🔔",
                    new DemoPageDefinition("overview_popup", "弹窗组件概览", "消息框与确认框独立验证。", OverviewPages.BuildPopup),
                    new[]
                    {
                        new DemoPageDefinition("page_messagebox", "消息框 MessageBox", "信息、警告、错误消息框。", MessageBoxDemoPage.Build),
                        new DemoPageDefinition("page_confirmbox", "确认框 ConfirmBox", "确认框回调与状态写回。", ConfirmBoxDemoPage.Build),
                    }),
                new DemoCategoryDefinition(
                    "导航组件",
                    "🧭",
                    new DemoPageDefinition("overview_nav", "导航组件概览", "TabControl、TreeView、MenuBar 和 PopupMenu。", OverviewPages.BuildNavigation),
                    new[]
                    {
                        new DemoPageDefinition("page_tabcontrol", "标签控件 TabControl", "新增、切换、关闭标签页。", TabControlDemoPage.Build),
                        new DemoPageDefinition("page_treeview", "树形框 TreeView", "节点创建、展开折叠、选中与重命名。", TreeViewDemoPage.Build),
                        new DemoPageDefinition("page_menubar", "菜单栏 MenuBar", "一级 / 二级菜单、回调和文案更新。", MenuBarDemoPage.Build),
                        new DemoPageDefinition("page_popupmenu", "弹出菜单 PopupMenu", "右键菜单、按钮菜单和回调。", PopupMenuDemoPage.Build),
                    }),
                new DemoCategoryDefinition(
                    "数据组件",
                    "📊",
                    new DemoPageDefinition("overview_data", "数据组件概览", "DataGridView 读写、排序和冻结行列。", OverviewPages.BuildData),
                    new[]
                    {
                        new DemoPageDefinition("page_datagrid", "表格 DataGridView", "高级列、排序、回调、导出 CSV、普通 / 虚拟模式和冻结前 N 列 / 前 N 行。", DataGridDemoPage.Build),
                    }),
            };
        }
    }
}
