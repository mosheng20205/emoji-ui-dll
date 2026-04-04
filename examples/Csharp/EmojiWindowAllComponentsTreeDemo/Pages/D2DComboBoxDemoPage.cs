using System;

namespace EmojiWindowDemo
{
    internal static class D2DComboBoxDemoPage
    {
        public static void Build(AllDemoShell shell, IntPtr page)
        {
            var app = shell.App;
            byte[] font = app.U("Microsoft YaHei UI");

            app.GroupBox(16, 16, 980, 520, "D2DComboBox 模式与事件", DemoTheme.Border, DemoTheme.Background, page);
            app.GroupBox(1020, 16, 444, 520, "文本 / 项目 / 颜色 / 状态", DemoTheme.Border, DemoTheme.Background, page);
            app.GroupBox(16, 558, 1448, 220, "D2DComboBox API 说明", DemoTheme.Border, DemoTheme.Background, page);

            app.Label(40, 56, 930, 24, "这一页只保留 D2DComboBox，同时放置只读和可编辑两种模式，把回调、文本、项目和状态集中展示。", DemoTheme.Muted, DemoTheme.Background, 12, PageCommon.AlignLeft, true, page);
            app.Label(56, 104, 220, 20, "只读 D2DComboBox", DemoTheme.Text, DemoTheme.Background, 14, PageCommon.AlignLeft, false, page);
            IntPtr readOnlyCombo = EmojiWindowNative.CreateD2DComboBox(page, 56, 128, 560, 40, 1, DemoTheme.Text, DemoTheme.Background, 32, font, font.Length, 13, 0, 0, 0);
            app.Label(56, 216, 220, 20, "可编辑 D2DComboBox", DemoTheme.Text, DemoTheme.Background, 14, PageCommon.AlignLeft, false, page);
            IntPtr editCombo = EmojiWindowNative.CreateD2DComboBox(page, 56, 240, 560, 40, 0, DemoTheme.Text, DemoTheme.Background, 32, font, font.Length, 13, 0, 0, 0);

            string[] readOnlyItems = { "🫧 D2D 默认", "🎨 D2D 主题", "✨ D2D 高亮", "🧩 D2D 扩展" };
            string[] editItems = { "✍️ 可编辑默认", "🌈 彩色文本", "🚨 紧急提示", "🔆 高亮条目" };
            foreach (string item in readOnlyItems)
            {
                byte[] text = app.U(item);
                EmojiWindowNative.AddD2DComboItem(readOnlyCombo, text, text.Length);
            }
            foreach (string item in editItems)
            {
                byte[] text = app.U(item);
                EmojiWindowNative.AddD2DComboItem(editCombo, text, text.Length);
            }

            EmojiWindowNative.SetD2DComboSelectedIndex(readOnlyCombo, 1);
            EmojiWindowNative.SetD2DComboSelectedIndex(editCombo, 1);
            EmojiWindowNative.SetD2DComboBoxColors(readOnlyCombo, DemoTheme.Text, DemoTheme.Background, DemoTheme.SurfacePrimary, DemoTheme.Surface, DemoTheme.BorderLight, DemoTheme.Surface);
            EmojiWindowNative.SetD2DComboBoxColors(editCombo, DemoTheme.Text, DemoTheme.Background, DemoTheme.SurfacePrimary, DemoTheme.Surface, DemoTheme.BorderLight, DemoTheme.Surface);

            app.Label(56, 300, 900, 38, "左侧两只控件都是真实 D2DComboBox。只读款验证 readonly 模式，右侧操作默认作用于下方可编辑框。", DemoTheme.Muted, DemoTheme.Background, 12, PageCommon.AlignLeft, true, page);
            IntPtr readout = app.Label(40, 350, 920, 104, "等待读取 D2DComboBox 状态。", DemoTheme.Text, DemoTheme.Background, 12, PageCommon.AlignLeft, true, page);
            IntPtr state = app.Label(40, 474, 920, 22, "D2DComboBox 页面状态会显示在这里。", DemoTheme.Primary, DemoTheme.Background, 12, PageCommon.AlignLeft, false, page);

            int editX = 56;
            int editY = 240;
            int editW = 560;
            int editH = 40;
            int nextItemId = 1;
            bool editEnabled = true;
            string editColorMode = "theme";

            string ReadItemText(IntPtr combo, int index)
            {
                if (index < 0)
                {
                    return "(无选中)";
                }

                return EmojiWindowNative.ReadUtf8(EmojiWindowNative.GetD2DComboItemText, combo, index);
            }

            void ApplyEditColors()
            {
                switch (editColorMode)
                {
                    case "cool":
                        EmojiWindowNative.SetD2DComboBoxColors(editCombo, DemoTheme.Primary, DemoTheme.SurfacePrimary, DemoTheme.Primary, DemoTheme.Surface, DemoTheme.BorderLight, DemoTheme.SurfacePrimary);
                        break;
                    case "warm":
                        EmojiWindowNative.SetD2DComboBoxColors(editCombo, DemoTheme.Warning, DemoTheme.SurfaceWarning, DemoTheme.Warning, DemoTheme.Surface, DemoTheme.Warning, DemoTheme.SurfaceWarning);
                        break;
                    default:
                        EmojiWindowNative.SetD2DComboBoxColors(editCombo, DemoTheme.Text, DemoTheme.Background, DemoTheme.SurfacePrimary, DemoTheme.Surface, DemoTheme.BorderLight, DemoTheme.Surface);
                        break;
                }
            }

            void Refresh(string note)
            {
                EmojiWindowNative.GetD2DComboBoxColors(readOnlyCombo, out uint roFg, out uint roBg, out uint roSel, out uint roHover, out uint roBorder, out uint roButton);
                EmojiWindowNative.GetD2DComboBoxColors(editCombo, out uint edFg, out uint edBg, out uint edSel, out uint edHover, out uint edBorder, out uint edButton);

                int roIndex = EmojiWindowNative.GetD2DComboSelectedIndex(readOnlyCombo);
                int edIndex = EmojiWindowNative.GetD2DComboSelectedIndex(editCombo);
                int roCount = EmojiWindowNative.GetD2DComboItemCount(readOnlyCombo);
                int edCount = EmojiWindowNative.GetD2DComboItemCount(editCombo);
                string editVisible = Win32Native.IsWindowVisible(editCombo) ? "显示" : "隐藏";
                string enabledText = editEnabled ? "启用" : "禁用";

                shell.SetLabelText(
                    readout,
                    $"只读: count={roCount} selected={roIndex} selected_text={EmojiWindowNative.ReadUtf8(EmojiWindowNative.GetD2DComboSelectedText, readOnlyCombo)} text={EmojiWindowNative.ReadUtf8(EmojiWindowNative.GetD2DComboText, readOnlyCombo)}\r\n" +
                    $"编辑: count={edCount} selected={edIndex} selected_text={EmojiWindowNative.ReadUtf8(EmojiWindowNative.GetD2DComboSelectedText, editCombo)} text={EmojiWindowNative.ReadUtf8(EmojiWindowNative.GetD2DComboText, editCombo)}\r\n" +
                    $"编辑 bounds=({editX}, {editY}, {editW}, {editH}) {editVisible}/{enabledText} fg/bg={PageCommon.FormatColor(edFg)}/{PageCommon.FormatColor(edBg)} border/button={PageCommon.FormatColor(edBorder)}/{PageCommon.FormatColor(edButton)}\r\n" +
                    $"只读 fg/bg={PageCommon.FormatColor(roFg)}/{PageCommon.FormatColor(roBg)} border/button={PageCommon.FormatColor(roBorder)}/{PageCommon.FormatColor(roButton)}");
                shell.SetLabelText(state, note);
                shell.SetStatus(note);
            }

            void RestoreItems()
            {
                EmojiWindowNative.ClearD2DComboBox(editCombo);
                foreach (string item in editItems)
                {
                    byte[] text = app.U(item);
                    EmojiWindowNative.AddD2DComboItem(editCombo, text, text.Length);
                }
                EmojiWindowNative.SetD2DComboSelectedIndex(editCombo, 1);
            }

            var callback = app.Pin(new EmojiWindowNative.ComboBoxCallback((handle, index) =>
            {
                string text = index >= 0 ? ReadItemText(handle, index) : "(无选中)";
                Refresh("D2DComboBox 回调: " + text);
            }));
            EmojiWindowNative.SetD2DComboBoxCallback(readOnlyCombo, callback);
            EmojiWindowNative.SetD2DComboBoxCallback(editCombo, callback);

            app.Label(1044, 56, 220, 22, "只读框导航", DemoTheme.Text, DemoTheme.Background, 14, PageCommon.AlignLeft, false, page);
            app.Button(1044, 94, 116, 34, "第 1 项", "1", DemoColors.Blue, () =>
            {
                EmojiWindowNative.SetD2DComboSelectedIndex(readOnlyCombo, 0);
                Refresh("只读 D2DComboBox 已选中第 1 项");
            }, page);
            app.Button(1172, 94, 116, 34, "第 4 项", "4", DemoColors.Green, () =>
            {
                EmojiWindowNative.SetD2DComboSelectedIndex(readOnlyCombo, 3);
                Refresh("只读 D2DComboBox 已选中第 4 项");
            }, page);
            app.Button(1300, 94, 124, 34, "读取选中", "i", DemoColors.Gray, () => Refresh("已读取只读 D2DComboBox 状态"), page);

            app.Label(1044, 148, 220, 22, "编辑框项目", DemoTheme.Text, DemoTheme.Background, 14, PageCommon.AlignLeft, false, page);
            app.Button(1044, 182, 116, 34, "新增项目", "+", DemoColors.Green, () =>
            {
                string value = "🆕 新增项目 " + nextItemId++;
                byte[] text = app.U(value);
                EmojiWindowNative.AddD2DComboItem(editCombo, text, text.Length);
                EmojiWindowNative.SetD2DComboSelectedIndex(editCombo, EmojiWindowNative.GetD2DComboItemCount(editCombo) - 1);
                Refresh("可编辑 D2DComboBox 已新增项目");
            }, page);
            app.Button(1172, 182, 116, 34, "删除末项", "-", DemoColors.Red, () =>
            {
                int count = EmojiWindowNative.GetD2DComboItemCount(editCombo);
                if (count > 0)
                {
                    EmojiWindowNative.RemoveD2DComboItem(editCombo, count - 1);
                    if (EmojiWindowNative.GetD2DComboItemCount(editCombo) > 0)
                    {
                        EmojiWindowNative.SetD2DComboSelectedIndex(editCombo, Math.Max(0, EmojiWindowNative.GetD2DComboItemCount(editCombo) - 1));
                    }
                    Refresh("可编辑 D2DComboBox 已删除最后一项");
                }
                else
                {
                    Refresh("可编辑 D2DComboBox 当前没有可删除的项目");
                }
            }, page);
            app.Button(1300, 182, 124, 34, "恢复列表", "R", DemoColors.Blue, () =>
            {
                RestoreItems();
                Refresh("可编辑 D2DComboBox 列表已恢复默认");
            }, page);

            app.Label(1044, 236, 220, 22, "文本与配色", DemoTheme.Text, DemoTheme.Background, 14, PageCommon.AlignLeft, false, page);
            app.Button(1044, 270, 116, 34, "写入主题", "T", DemoColors.Purple, () =>
            {
                byte[] text = app.U("🫧 D2D 手动输入 / 可编辑");
                EmojiWindowNative.SetD2DComboText(editCombo, text, text.Length);
                Refresh("可编辑 D2DComboBox 文本已写入");
            }, page);
            app.Button(1172, 270, 116, 34, "蓝色方案", "B", DemoColors.Blue, () =>
            {
                editColorMode = "cool";
                ApplyEditColors();
                Refresh("可编辑 D2DComboBox 已切到蓝色方案");
            }, page);
            app.Button(1300, 270, 124, 34, "暖色方案", "W", DemoColors.Orange, () =>
            {
                editColorMode = "warm";
                ApplyEditColors();
                Refresh("可编辑 D2DComboBox 已切到暖色方案");
            }, page);

            app.Label(1044, 324, 220, 22, "布局与状态", DemoTheme.Text, DemoTheme.Background, 14, PageCommon.AlignLeft, false, page);
            app.Button(1044, 358, 116, 34, "右移 80", ">", DemoColors.Green, () =>
            {
                editX = 136;
                EmojiWindowNative.SetD2DComboBoxBounds(editCombo, editX, editY, editW, editH);
                Refresh("可编辑 D2DComboBox 已整体右移");
            }, page);
            app.Button(1172, 358, 116, 34, "改宽 420", "W", DemoColors.Gray, () =>
            {
                editW = 420;
                EmojiWindowNative.SetD2DComboBoxBounds(editCombo, editX, editY, editW, editH);
                Refresh("可编辑 D2DComboBox 宽度已改为 420");
            }, page);
            app.Button(1300, 358, 124, 34, "禁用/启用", "E", DemoColors.Purple, () =>
            {
                editEnabled = !editEnabled;
                EmojiWindowNative.EnableD2DComboBox(editCombo, editEnabled ? 1 : 0);
                Refresh("可编辑 D2DComboBox 启用状态已切换");
            }, page);
            app.Button(1044, 402, 116, 34, "显示/隐藏", "V", DemoColors.Gray, () =>
            {
                EmojiWindowNative.ShowD2DComboBox(editCombo, Win32Native.IsWindowVisible(editCombo) ? 0 : 1);
                Refresh("可编辑 D2DComboBox 可见状态已切换");
            }, page);
            app.Button(1172, 402, 252, 34, "恢复布局与主题色", "T", DemoColors.Blue, () =>
            {
                editX = 56;
                editY = 240;
                editW = 560;
                editH = 40;
                editEnabled = true;
                editColorMode = "theme";
                EmojiWindowNative.SetD2DComboBoxBounds(editCombo, editX, editY, editW, editH);
                EmojiWindowNative.ShowD2DComboBox(editCombo, 1);
                EmojiWindowNative.EnableD2DComboBox(editCombo, 1);
                ApplyEditColors();
                Refresh("可编辑 D2DComboBox 布局与主题色已恢复");
            }, page);

            app.Label(40, 582, 1320, 24, "1. GetD2DComboItemCount / GetD2DComboSelectedIndex / GetD2DComboItemText：读取项目数、选中项和项目文本。", DemoTheme.Text, DemoTheme.Background, 12, PageCommon.AlignLeft, false, page);
            app.Label(40, 616, 1320, 24, "2. GetD2DComboText / GetD2DComboSelectedText / SetD2DComboText：读取或修改输入区文本与当前选中文本。", DemoTheme.Text, DemoTheme.Background, 12, PageCommon.AlignLeft, false, page);
            app.Label(40, 650, 1320, 24, "3. AddD2DComboItem / RemoveD2DComboItem / ClearD2DComboBox：直接操作 D2D 下拉项目列表。", DemoTheme.Text, DemoTheme.Background, 12, PageCommon.AlignLeft, false, page);
            app.Label(40, 684, 1320, 24, "4. SetD2DComboBoxColors / GetD2DComboBoxColors：切换配色并读取边框、按钮、选中和悬停颜色。", DemoTheme.Text, DemoTheme.Background, 12, PageCommon.AlignLeft, false, page);
            app.Label(40, 718, 1320, 24, "5. SetD2DComboBoxBounds / EnableD2DComboBox / ShowD2DComboBox：演示布局、启用态和可见态切换。", DemoTheme.Text, DemoTheme.Background, 12, PageCommon.AlignLeft, false, page);

            void ApplyTheme()
            {
                EmojiWindowNative.SetD2DComboBoxColors(readOnlyCombo, DemoTheme.Text, DemoTheme.Background, DemoTheme.SurfacePrimary, DemoTheme.Surface, DemoTheme.BorderLight, DemoTheme.Surface);
                if (editColorMode == "theme")
                {
                    ApplyEditColors();
                }
            }

            shell.RegisterPageThemeHandler(page, ApplyTheme);
            ApplyTheme();
            Refresh("D2DComboBox 页面已重排，可直接测试项目、文本、颜色和状态。");
        }
    }
}
