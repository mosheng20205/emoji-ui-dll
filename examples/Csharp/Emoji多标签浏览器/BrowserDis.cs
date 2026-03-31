using FBroSharp;
using FBroSharp.DataType;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace BaseTest
{
    public class BrowserDis
    {
        //创建浏览器
        public static void CreatBrowser(string title, string url, IntPtr parent_window, int x, int y, int width, int height, string user_flag)
        {
            //开始创建浏览器
            FBroSharpWindowsInfo windows_info = new FBroSharpWindowsInfo
            {
                parent_window = parent_window,//parent_window,
                x = x,
                y = y,
                width = width,
                height = height,
                window_name = title
            };

            //创建浏览器事件回调类
            BrowserEvent browser_event = new BrowserEvent();

            //设置运行环境
            //FBroSharpRequestContextSet contextSet = new FBroSharpRequestContextSet();
            //contextSet.cache_path = Directory.GetCurrentDirectory() + "\\Cache\\other cache\\"; ;

            //FBroSharpRequestContext request_context = (FBroSharpRequestContext)FBroSharpRequestContext.CreateContext(contextSet);
            //Console.WriteLine("缓存目录:" + request_context.GetCachePath());

            //设置不需要的事件
            //FBroSharpEventDisableControl event_contrl = new FBroSharpEventDisableControl();
            //event_contrl.disableGetAudioHandler = true;


            //设置传递参数
            //FBroSharpDictionaryValue extra_info = new FBroSharpDictionaryValue();//.Creat();
            //extra_info.SetString("传参测试", "这是创建浏览器的时候传递过来的参数");

            if (FBroSharpControl.CreatBrowser(url, windows_info, default, default /*request_context*/, default/*extra_info*/, browser_event, default/*event_contrl*/,user_flag))
                Console.WriteLine("true");
        }
    }
}
