using System;

namespace EmojiWindowDemo
{
    internal static class ComboBoxDemoPage
    {
        public static void Build(AllDemoShell shell, IntPtr page)
        {
            var app = shell.App;
            byte[] font = app.U("Microsoft YaHei UI");

            IntPtr readOnlyCombo = EmojiWindowNative.CreateComboBox(page, 40, 110, 320, 36, 1, DemoColors.Black, DemoColors.White, 30, font, font.Length, 13, 0, 0, 0);
            foreach (string item in new[] { "首页 🏔️", "设置 ⚙️", "收藏 ⭐", "下载 ⬇️" })
            {
                byte[] text = app.U(item);
                EmojiWindowNative.AddComboItem(readOnlyCombo, text, text.Length);
            }
            EmojiWindowNative.SetComboSelectedIndex(readOnlyCombo, 0);

            IntPtr editCombo = EmojiWindowNative.CreateComboBox(page, 40, 174, 420, 36, 0, DemoColors.Black, DemoColors.White, 30, font, font.Length, 13, 0, 0, 0);
            foreach (string item in new[] { "https://www.example.com", "https://github.com", "https://openai.com" })
            {
                byte[] text = app.U(item);
                EmojiWindowNative.AddComboItem(editCombo, text, text.Length);
            }

            IntPtr readout = app.Label(520, 110, 620, 112, string.Empty, DemoColors.Black, DemoColors.WindowBg, 13, PageCommon.AlignLeft, true, page);
            IntPtr state = app.Label(40, 760, 1360, 22, "ComboBox 页状态将在这里更新。", DemoColors.Blue, DemoColors.WindowBg, 12, PageCommon.AlignLeft, false, page);

            void Refresh(string note)
            {
                int roIndex = EmojiWindowNative.GetComboSelectedIndex(readOnlyCombo);
                string roText = roIndex >= 0 ? EmojiWindowNative.ReadUtf8(EmojiWindowNative.GetComboItemText, readOnlyCombo, roIndex) : "(未选中)";
                string editText = EmojiWindowNative.ReadUtf8(EmojiWindowNative.GetComboBoxText, editCombo);
                shell.SetLabelText(readout, $"readOnlyIndex={roIndex}  readOnlyText={roText}\r\neditText={editText}\r\n{note}");
                shell.SetLabelText(state, note);
                shell.SetStatus(note);
            }

            var callback = app.Pin(new EmojiWindowNative.ComboBoxCallback((handle, index) =>
            {
                string text = index >= 0 ? EmojiWindowNative.ReadUtf8(EmojiWindowNative.GetComboItemText, handle, index) : "(未选中)";
                Refresh("ComboBox 回调: " + text);
            }));
            EmojiWindowNative.SetComboBoxCallback(readOnlyCombo, callback);
            EmojiWindowNative.SetComboBoxCallback(editCombo, callback);

            app.Button(40, 320, 156, 36, "读取只读框", "📄", DemoColors.Blue, () => Refresh("已读取只读 ComboBox"), page);
            app.Button(212, 320, 156, 36, "选第 3 项", "3", DemoColors.Green, () =>
            {
                EmojiWindowNative.SetComboSelectedIndex(readOnlyCombo, 2);
                Refresh("只读 ComboBox 已切到第 3 项");
            }, page);
            app.Button(384, 320, 156, 36, "写入树形页", "🌐", DemoColors.Orange, () =>
            {
                byte[] text = app.U("https://emoji-window.demo/tree");
                EmojiWindowNative.SetComboBoxText(editCombo, text, text.Length);
                Refresh("可编辑 ComboBox 已写入树形页地址");
            }, page);
            app.Button(556, 320, 156, 36, "读取文本", "✏️", DemoColors.Purple, () => Refresh("已读取可编辑 ComboBox 文本"), page);

            app.Label(40, 598, 1320, 22, "1. AddComboItem / GetComboSelectedIndex / GetComboItemText：读取只读 ComboBox 的选中项。", DemoColors.Black, DemoColors.WindowBg, 12, PageCommon.AlignLeft, false, page);
            app.Label(40, 632, 1320, 22, "2. GetComboBoxText / SetComboBoxText：读取和写入可编辑 ComboBox 文本。", DemoColors.Black, DemoColors.WindowBg, 12, PageCommon.AlignLeft, false, page);
            app.Label(40, 666, 1320, 22, "3. SetComboBoxCallback：选择变化会把当前项写回状态区。", DemoColors.Black, DemoColors.WindowBg, 12, PageCommon.AlignLeft, false, page);

            Refresh("ComboBox 页已加载，可直接测试只读框、可编辑框和文本回写");
        }
    }
}
