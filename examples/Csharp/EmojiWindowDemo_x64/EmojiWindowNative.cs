using System;
using System.Runtime.InteropServices;
using System.Text;

namespace EmojiWindowDemo
{
    /// <summary>
    /// emoji_window.dll 的 P/Invoke 声明（64位版本）
    /// </summary>
    public static class EmojiWindowNative
    {
        // DLL 名称（64位）
        private const string DLL_NAME = "emoji_window.dll";
        private const CallingConvention CALL_CONV = CallingConvention.StdCall;

        #region 回调委托定义

        [UnmanagedFunctionPointer(CALL_CONV)]
        public delegate void ButtonClickCallback(int buttonId, IntPtr parentHwnd);

        [UnmanagedFunctionPointer(CALL_CONV)]
        public delegate void MessageBoxCallback(int confirmed);

        [UnmanagedFunctionPointer(CALL_CONV)]
        public delegate void CheckBoxCallback(IntPtr hCheckBox, bool isChecked);

        #endregion

        #region 窗口管理

        /// <summary>
        /// 创建窗口（使用 UTF-8 字节）
        /// </summary>
        [DllImport(DLL_NAME, CallingConvention = CALL_CONV, CharSet = CharSet.Ansi)]
        public static extern IntPtr create_window_bytes(
            byte[] titleBytes,
            int titleLen,
            int width,
            int height
        );

        /// <summary>
        /// 设置消息循环主窗口
        /// </summary>
        [DllImport(DLL_NAME, CallingConvention = CALL_CONV)]
        public static extern void set_message_loop_main_window(IntPtr hwnd);

        /// <summary>
        /// 运行消息循环
        /// </summary>
        [DllImport(DLL_NAME, CallingConvention = CALL_CONV)]
        public static extern int run_message_loop();

        /// <summary>
        /// 销毁窗口
        /// </summary>
        [DllImport(DLL_NAME, CallingConvention = CALL_CONV)]
        public static extern void destroy_window(IntPtr hwnd);

        #endregion

        #region 按钮

        /// <summary>
        /// 创建 Emoji 按钮
        /// </summary>
        [DllImport(DLL_NAME, CallingConvention = CALL_CONV)]
        public static extern int create_emoji_button_bytes(
            IntPtr parent,
            byte[] emojiBytes,
            int emojiLen,
            byte[] textBytes,
            int textLen,
            int x, int y, int width, int height,
            uint bgColor
        );

        /// <summary>
        /// 设置按钮点击回调
        /// </summary>
        [DllImport(DLL_NAME, CallingConvention = CALL_CONV)]
        public static extern void set_button_click_callback(ButtonClickCallback callback);

        #endregion

        #region 标签

        /// <summary>
        /// 创建标签
        /// </summary>
        [DllImport(DLL_NAME, CallingConvention = CALL_CONV)]
        public static extern IntPtr CreateLabel(
            IntPtr hParent,
            int x, int y, int width, int height,
            byte[] textBytes,
            int textLen,
            uint fgColor,
            uint bgColor,
            byte[] fontNameBytes,
            int fontNameLen,
            int fontSize,
            bool bold,
            bool italic,
            bool underline,
            int alignment,
            bool wordWrap
        );

        /// <summary>
        /// 设置标签文本
        /// </summary>
        [DllImport(DLL_NAME, CallingConvention = CALL_CONV)]
        public static extern void SetLabelText(
            IntPtr hLabel,
            byte[] textBytes,
            int textLen
        );

        #endregion

        #region 分组框

        /// <summary>
        /// 创建分组框
        /// </summary>
        [DllImport(DLL_NAME, CallingConvention = CALL_CONV)]
        public static extern IntPtr CreateGroupBox(
            IntPtr hParent,
            int x, int y, int width, int height,
            byte[] titleBytes,
            int titleLen,
            uint borderColor,
            uint bgColor
        );

        #endregion

        #region 复选框

        /// <summary>
        /// 创建复选框
        /// </summary>
        [DllImport(DLL_NAME, CallingConvention = CALL_CONV)]
        public static extern IntPtr CreateCheckBox(
            IntPtr hParent,
            int x, int y, int width, int height,
            byte[] textBytes,
            int textLen,
            bool isChecked,
            uint fgColor,
            uint bgColor
        );

        /// <summary>
        /// 获取复选框状态
        /// </summary>
        [DllImport(DLL_NAME, CallingConvention = CALL_CONV)]
        public static extern bool GetCheckBoxState(IntPtr hCheckBox);

        /// <summary>
        /// 设置复选框回调
        /// </summary>
        [DllImport(DLL_NAME, CallingConvention = CALL_CONV)]
        public static extern void SetCheckBoxCallback(IntPtr hCheckBox, CheckBoxCallback callback);

        #endregion

        #region 信息框

        /// <summary>
        /// 显示信息框
        /// </summary>
        [DllImport(DLL_NAME, CallingConvention = CALL_CONV)]
        public static extern void show_message_box_bytes(
            IntPtr parent,
            byte[] titleBytes,
            int titleLen,
            byte[] messageBytes,
            int messageLen,
            byte[] iconBytes,
            int iconLen
        );

        /// <summary>
        /// 显示确认框
        /// </summary>
        [DllImport(DLL_NAME, CallingConvention = CALL_CONV)]
        public static extern void show_confirm_box_bytes(
            IntPtr parent,
            byte[] titleBytes,
            int titleLen,
            byte[] messageBytes,
            int messageLen,
            byte[] iconBytes,
            int iconLen,
            MessageBoxCallback callback
        );

        #endregion

        #region 辅助方法

        /// <summary>
        /// 将字符串转换为 UTF-8 字节数组
        /// </summary>
        public static byte[] StringToUtf8Bytes(string str)
        {
            if (string.IsNullOrEmpty(str))
                return new byte[0];
            return Encoding.UTF8.GetBytes(str);
        }

        /// <summary>
        /// 创建 ARGB 颜色值
        /// </summary>
        public static uint ARGB(byte a, byte r, byte g, byte b)
        {
            return (uint)((a << 24) | (r << 16) | (g << 8) | b);
        }

        #endregion
    }
}
