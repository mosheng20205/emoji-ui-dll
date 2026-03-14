using System;
using System.Runtime.InteropServices;

namespace EmojiWindowDemo
{
    class Program
    {
        // 保持回调引用，防止被垃圾回收
        private static EmojiWindowNative.ButtonClickCallback buttonCallback;
        private static EmojiWindowNative.CheckBoxCallback checkBoxCallback;
        private static EmojiWindowNative.MessageBoxCallback confirmCallback;

        // 窗口和控件句柄
        private static IntPtr mainWindow;
        private static IntPtr checkBox1;
        private static IntPtr label1;

        [STAThread]
        static void Main(string[] args)
        {
            Console.WriteLine("=== Emoji Window C# 示例 (32位) ===");
            Console.WriteLine("正在创建窗口...");

            try
            {
                // 1. 创建主窗口（带 Unicode 表情）
                string windowTitle = "🎨 抖店安全参数生成器 - C# Demo";
                byte[] titleBytes = EmojiWindowNative.StringToUtf8Bytes(windowTitle);
                mainWindow = EmojiWindowNative.create_window_bytes(titleBytes, titleBytes.Length, 800, 600);

                if (mainWindow == IntPtr.Zero)
                {
                    Console.WriteLine("❌ 创建窗口失败！");
                    return;
                }

                Console.WriteLine("✅ 窗口创建成功！");

                // 2. 创建分组框 - 控制面板
                CreateGroupBox1();

                // 3. 创建分组框 - 生成结果
                CreateGroupBox2();

                // 4. 创建分组框 - 验证测试
                CreateGroupBox3();

                // 5. 设置按钮点击回调
                buttonCallback = OnButtonClick;
                EmojiWindowNative.set_button_click_callback(buttonCallback);

                // 6. 设置复选框回调
                checkBoxCallback = OnCheckBoxChanged;
                EmojiWindowNative.SetCheckBoxCallback(checkBox1, checkBoxCallback);

                // 7. 设置消息循环主窗口
                EmojiWindowNative.set_message_loop_main_window(mainWindow);

                Console.WriteLine("✅ 控件创建完成，进入消息循环...");

                // 8. 运行消息循环
                EmojiWindowNative.run_message_loop();

                Console.WriteLine("程序退出。");
            }
            catch (Exception ex)
            {
                Console.WriteLine($"❌ 发生错误: {ex.Message}");
                Console.WriteLine($"堆栈跟踪: {ex.StackTrace}");
            }
        }

        /// <summary>
        /// 创建分组框1 - 控制面板
        /// </summary>
        static void CreateGroupBox1()
        {
            // 分组框
            byte[] groupTitle = EmojiWindowNative.StringToUtf8Bytes("📋 控制面板");
            IntPtr groupBox = EmojiWindowNative.CreateGroupBox(
                mainWindow,
                20, 50, 360, 200,
                groupTitle, groupTitle.Length,
                EmojiWindowNative.ARGB(255, 220, 220, 220), // 边框颜色
                EmojiWindowNative.ARGB(255, 250, 250, 250)  // 背景色
            );

            // 标签1 - URL
            byte[] label1Text = EmojiWindowNative.StringToUtf8Bytes("🌐 URL:");
            byte[] fontName = EmojiWindowNative.StringToUtf8Bytes("Microsoft YaHei UI");
            IntPtr label1 = EmojiWindowNative.CreateLabel(
                mainWindow,
                40, 80, 80, 30,
                label1Text, label1Text.Length,
                EmojiWindowNative.ARGB(255, 50, 50, 50),    // 前景色
                EmojiWindowNative.ARGB(0, 0, 0, 0),         // 透明背景
                fontName, fontName.Length,
                14, false, false, false,
                0, // 左对齐
                false
            );

            // 标签2 - source
            byte[] label2Text = EmojiWindowNative.StringToUtf8Bytes("📦 source:");
            IntPtr label2 = EmojiWindowNative.CreateLabel(
                mainWindow,
                40, 120, 80, 30,
                label2Text, label2Text.Length,
                EmojiWindowNative.ARGB(255, 50, 50, 50),
                EmojiWindowNative.ARGB(0, 0, 0, 0),
                fontName, fontName.Length,
                14, false, false, false,
                0, false
            );

            // 标签3 - appid
            byte[] label3Text = EmojiWindowNative.StringToUtf8Bytes("🔑 appid:");
            IntPtr label3 = EmojiWindowNative.CreateLabel(
                mainWindow,
                40, 160, 80, 30,
                label3Text, label3Text.Length,
                EmojiWindowNative.ARGB(255, 50, 50, 50),
                EmojiWindowNative.ARGB(0, 0, 0, 0),
                fontName, fontName.Length,
                14, false, false, false,
                0, false
            );

            // 按钮 - 批量生成
            byte[] btnEmoji = EmojiWindowNative.StringToUtf8Bytes("🚀");
            byte[] btnText = EmojiWindowNative.StringToUtf8Bytes("批量生成");
            int btnId = EmojiWindowNative.create_emoji_button_bytes(
                mainWindow,
                btnEmoji, btnEmoji.Length,
                btnText, btnText.Length,
                40, 200, 120, 35,
                EmojiWindowNative.ARGB(255, 64, 158, 255) // 主题蓝色
            );

            // 复选框
            byte[] checkText = EmojiWindowNative.StringToUtf8Bytes("✅ 启用自动刷新");
            checkBox1 = EmojiWindowNative.CreateCheckBox(
                mainWindow,
                180, 205, 150, 30,
                checkText, checkText.Length,
                false,
                EmojiWindowNative.ARGB(255, 50, 50, 50),
                EmojiWindowNative.ARGB(0, 0, 0, 0)
            );
        }

        /// <summary>
        /// 创建分组框2 - 生成结果
        /// </summary>
        static void CreateGroupBox2()
        {
            byte[] groupTitle = EmojiWindowNative.StringToUtf8Bytes("📊 生成结果");
            IntPtr groupBox = EmojiWindowNative.CreateGroupBox(
                mainWindow,
                400, 50, 380, 200,
                groupTitle, groupTitle.Length,
                EmojiWindowNative.ARGB(255, 220, 220, 220),
                EmojiWindowNative.ARGB(255, 250, 250, 250)
            );

            // 标签 - msToken
            byte[] fontName = EmojiWindowNative.StringToUtf8Bytes("Microsoft YaHei UI");
            byte[] labelText = EmojiWindowNative.StringToUtf8Bytes("🔐 msToken: ✓");
            label1 = EmojiWindowNative.CreateLabel(
                mainWindow,
                420, 90, 340, 30,
                labelText, labelText.Length,
                EmojiWindowNative.ARGB(255, 103, 194, 58),  // 成功绿色
                EmojiWindowNative.ARGB(0, 0, 0, 0),
                fontName, fontName.Length,
                14, true, false, false, // 粗体
                0, false
            );

            // 标签 - a_bogus
            byte[] label2Text = EmojiWindowNative.StringToUtf8Bytes("🔒 a_bogus: ✗");
            IntPtr label2 = EmojiWindowNative.CreateLabel(
                mainWindow,
                420, 130, 340, 30,
                label2Text, label2Text.Length,
                EmojiWindowNative.ARGB(255, 245, 108, 108), // 错误红色
                EmojiWindowNative.ARGB(0, 0, 0, 0),
                fontName, fontName.Length,
                14, true, false, false,
                0, false
            );

            // 标签 - verifyFp
            byte[] label3Text = EmojiWindowNative.StringToUtf8Bytes("🛡️ verifyFp: ✓");
            IntPtr label3 = EmojiWindowNative.CreateLabel(
                mainWindow,
                420, 170, 340, 30,
                label3Text, label3Text.Length,
                EmojiWindowNative.ARGB(255, 103, 194, 58),
                EmojiWindowNative.ARGB(0, 0, 0, 0),
                fontName, fontName.Length,
                14, true, false, false,
                0, false
            );

            // 按钮 - 复制
            byte[] btnEmoji = EmojiWindowNative.StringToUtf8Bytes("📋");
            byte[] btnText = EmojiWindowNative.StringToUtf8Bytes("复制");
            int btnId = EmojiWindowNative.create_emoji_button_bytes(
                mainWindow,
                btnEmoji, btnEmoji.Length,
                btnText, btnText.Length,
                420, 210, 100, 30,
                EmojiWindowNative.ARGB(255, 103, 194, 58)
            );
        }

        /// <summary>
        /// 创建分组框3 - 验证测试
        /// </summary>
        static void CreateGroupBox3()
        {
            byte[] groupTitle = EmojiWindowNative.StringToUtf8Bytes("🧪 验证测试");
            IntPtr groupBox = EmojiWindowNative.CreateGroupBox(
                mainWindow,
                20, 270, 760, 280,
                groupTitle, groupTitle.Length,
                EmojiWindowNative.ARGB(255, 220, 220, 220),
                EmojiWindowNative.ARGB(255, 250, 250, 250)
            );

            // 标签 - 说明
            byte[] fontName = EmojiWindowNative.StringToUtf8Bytes("Microsoft YaHei UI");
            byte[] infoText = EmojiWindowNative.StringToUtf8Bytes(
                "💡 提示：某些参数可能无法生成，请检查页面是否完全加载。\n" +
                "⚠️ 注意：参数生成可能需要一定时间，请耐心等待。"
            );
            IntPtr infoLabel = EmojiWindowNative.CreateLabel(
                mainWindow,
                40, 310, 720, 60,
                infoText, infoText.Length,
                EmojiWindowNative.ARGB(255, 144, 147, 153), // 次要文本色
                EmojiWindowNative.ARGB(0, 0, 0, 0),
                fontName, fontName.Length,
                13, false, false, false,
                0, true // 自动换行
            );

            // 按钮组
            byte[] btn1Emoji = EmojiWindowNative.StringToUtf8Bytes("📝");
            byte[] btn1Text = EmojiWindowNative.StringToUtf8Bytes("验证参数");
            int btn1Id = EmojiWindowNative.create_emoji_button_bytes(
                mainWindow,
                btn1Emoji, btn1Emoji.Length,
                btn1Text, btn1Text.Length,
                40, 390, 120, 35,
                EmojiWindowNative.ARGB(255, 64, 158, 255)
            );

            byte[] btn2Emoji = EmojiWindowNative.StringToUtf8Bytes("🔄");
            byte[] btn2Text = EmojiWindowNative.StringToUtf8Bytes("测试请求");
            int btn2Id = EmojiWindowNative.create_emoji_button_bytes(
                mainWindow,
                btn2Emoji, btn2Emoji.Length,
                btn2Text, btn2Text.Length,
                180, 390, 120, 35,
                EmojiWindowNative.ARGB(255, 230, 162, 60) // 警告橙色
            );

            byte[] btn3Emoji = EmojiWindowNative.StringToUtf8Bytes("🗑️");
            byte[] btn3Text = EmojiWindowNative.StringToUtf8Bytes("批量生成");
            int btn3Id = EmojiWindowNative.create_emoji_button_bytes(
                mainWindow,
                btn3Emoji, btn3Emoji.Length,
                btn3Text, btn3Text.Length,
                320, 390, 120, 35,
                EmojiWindowNative.ARGB(255, 245, 108, 108) // 危险红色
            );

            // 状态标签
            byte[] statusText = EmojiWindowNative.StringToUtf8Bytes(
                "⏱️ 18:15:46 - 生成成功 - msToken: ✓ | a_bogus: ✗ | verifyFp: ✓"
            );
            IntPtr statusLabel = EmojiWindowNative.CreateLabel(
                mainWindow,
                40, 450, 720, 80,
                statusText, statusText.Length,
                EmojiWindowNative.ARGB(255, 48, 49, 51),
                EmojiWindowNative.ARGB(255, 245, 247, 250),
                fontName, fontName.Length,
                12, false, false, false,
                0, true
            );
        }

        /// <summary>
        /// 按钮点击回调
        /// </summary>
        static void OnButtonClick(int buttonId, IntPtr parentHwnd)
        {
            Console.WriteLine($"按钮被点击: ID = {buttonId}");

            // 显示信息框
            byte[] title = EmojiWindowNative.StringToUtf8Bytes("⚠️ 警告");
            byte[] message = EmojiWindowNative.StringToUtf8Bytes(
                "参数生成不完整！\n\n" +
                "msToken: ✓ | a_bogus: ✗ | verifyFp: ✓\n\n" +
                "某些参数可能无法生成，请检查页面是否完全加载。"
            );
            byte[] icon = EmojiWindowNative.StringToUtf8Bytes("⚠️");

            EmojiWindowNative.show_message_box_bytes(
                mainWindow,
                title, title.Length,
                message, message.Length,
                icon, icon.Length
            );
        }

        /// <summary>
        /// 复选框状态改变回调
        /// </summary>
        static void OnCheckBoxChanged(IntPtr hCheckBox, bool isChecked)
        {
            Console.WriteLine($"复选框状态改变: {(isChecked ? "选中" : "未选中")}");

            if (isChecked)
            {
                // 更新标签文本
                byte[] newText = EmojiWindowNative.StringToUtf8Bytes("🔐 msToken: ✓ (自动刷新中...)");
                EmojiWindowNative.SetLabelText(label1, newText, newText.Length);
            }
        }
    }
}
