using BaseTest;
using FBroSharp;
using FBroSharp.Const;
using FBroSharp.DataType;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace Emoji多标签浏览器
{
    static class Program
    {

        /// <summary>
        /// 应用程序的主入口点。
        /// </summary>
        [STAThread]
        static void Main()
        {

            FBroSharpInitSet set = new FBroSharpInitSet
            {
                //设置缓存目录
                cache_path = Directory.GetCurrentDirectory() + "\\Cache\\CachePath\\",

                //设置用户目录
                user_data_path = Directory.GetCurrentDirectory() + "\\Cache\\UserPath\\",

                //设置根缓存目录，必须是上面缓存目录的父目录，否则采用独立缓存可能会失效
                root_cache_path = Directory.GetCurrentDirectory() + "\\Cache\\",

                //启用事件消息循环
                multi_threaded_message_loop = true,

                //设置执行子进程，不设置他会默认以当前进程作为子进程
                browser_subprocess_path = Directory.GetCurrentDirectory() + "\\FBroSubprocess.exe",

                //本地化语言
                locale = "zh-CN",

                //日志模式，只有出现错误的时候才记录日志
                log_severity = FBroSharpLogSeverity.ERROR
            };

            //创建初始化事件
            InitEvent init_event = new InitEvent();

            //执行初始化
            if (!FBroSharpInitControl.InitPro(set, init_event))
                return;

            Application.EnableVisualStyles();
            Application.SetCompatibleTextRenderingDefault(false);
            Application.Run(new Form1());


            //关闭框架释放资源
            FBroSharpInitControl.Shutdown(false);

        }
    }
}
