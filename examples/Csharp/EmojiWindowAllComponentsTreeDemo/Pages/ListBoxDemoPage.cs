using System;

namespace EmojiWindowDemo
{
    internal static class ListBoxDemoPage
    {
        public static void Build(AllDemoShell shell, IntPtr page)
        {
            var app = shell.App;
            IntPtr list = EmojiWindowNative.CreateListBox(page, 40, 110, 360, 300, 0, DemoColors.Black, DemoColors.White);
            foreach (string item in new[] { "首页 🏔️", "下载管理 ⬇️", "书签 ⭐", "设置 ⚙️", "关于 ℹ️" })
            {
                byte[] text = app.U(item);
                EmojiWindowNative.AddListItem(list, text, text.Length);
            }
            EmojiWindowNative.SetSelectedIndex(list, 0);

            IntPtr readout = app.Label(440, 110, 520, 90, string.Empty, DemoColors.Black, DemoColors.WindowBg, 13, PageCommon.AlignLeft, true, page);
            IntPtr state = app.Label(40, 760, 1360, 22, "ListBox 页状态将在这里更新。", DemoColors.Blue, DemoColors.WindowBg, 12, PageCommon.AlignLeft, false, page);

            void Refresh(string note)
            {
                int index = EmojiWindowNative.GetSelectedIndex(list);
                int count = EmojiWindowNative.GetListItemCount(list);
                string text = index >= 0 ? EmojiWindowNative.ReadUtf8(EmojiWindowNative.GetListItemText, list, index) : "(未选中)";
                shell.SetLabelText(readout, $"count={count}  selectedIndex={index}\r\nselectedText={text}\r\n{note}");
                shell.SetLabelText(state, note);
                shell.SetStatus(note);
            }

            var callback = app.Pin(new EmojiWindowNative.ListBoxCallback((handle, index) =>
            {
                string text = index >= 0 ? EmojiWindowNative.ReadUtf8(EmojiWindowNative.GetListItemText, handle, index) : "(未选中)";
                Refresh("ListBox 回调: " + text);
            }));
            EmojiWindowNative.SetListBoxCallback(list, callback);

            app.Button(40, 440, 140, 36, "添加一项", "➕", DemoColors.Green, () =>
            {
                int count = EmojiWindowNative.GetListItemCount(list);
                byte[] text = app.U("新增项 " + (count + 1));
                EmojiWindowNative.AddListItem(list, text, text.Length);
                Refresh("已向 ListBox 新增一项");
            }, page);
            app.Button(196, 440, 140, 36, "读取选中项", "📄", DemoColors.Blue, () => Refresh("已读取当前选中项"), page);
            app.Button(352, 440, 140, 36, "删除选中项", "🗑️", DemoColors.Red, () =>
            {
                int index = EmojiWindowNative.GetSelectedIndex(list);
                if (index >= 0)
                {
                    EmojiWindowNative.RemoveListItem(list, index);
                    Refresh("已删除当前选中项");
                }
                else
                {
                    Refresh("请先选中一项");
                }
            }, page);
            app.Button(508, 440, 140, 36, "选第 3 项", "3", DemoColors.Orange, () =>
            {
                EmojiWindowNative.SetSelectedIndex(list, 2);
                Refresh("程序已选中第 3 项");
            }, page);

            app.Label(40, 598, 1320, 22, "1. AddListItem / RemoveListItem / GetListItemCount：操作列表项。", DemoColors.Black, DemoColors.WindowBg, 12, PageCommon.AlignLeft, false, page);
            app.Label(40, 632, 1320, 22, "2. GetSelectedIndex / SetSelectedIndex / GetListItemText：读取和设置当前选中项。", DemoColors.Black, DemoColors.WindowBg, 12, PageCommon.AlignLeft, false, page);
            app.Label(40, 666, 1320, 22, "3. SetListBoxCallback：选中变化会把当前项文本写回状态区。", DemoColors.Black, DemoColors.WindowBg, 12, PageCommon.AlignLeft, false, page);

            Refresh("ListBox 页已加载，可直接测试增删项、选中读取和回调");
        }
    }
}
