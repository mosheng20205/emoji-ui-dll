using FBroSharp;
using FBroSharp.Const;
using FBroSharp.DataType;
using FBroSharp.Event;
using FBroSharp.Lib;
using FBroSharp.Value;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Reflection;
using FBroSharp.Callback;
using System.IO;

namespace BaseTest
{
    public class BrowserList
    {
        public static List<FBroSharpBrowser> data = new List<FBroSharpBrowser>();

    }

    public class BrowserEvent : FBroSharpBrowserEvent
    {
        //即将创建
        public override void OnAfterCreated(IFBroSharpBrowser browser, IFBroSharpDictionaryValue extrainfo)
        {
            Console.WriteLine($"{MethodBase.GetCurrentMethod().Name} 浏览器标识：{browser.GetUserFlag()}");

            if (!browser.IsBackgroundHost())
            {
                BrowserList.data.Add((FBroSharpBrowser)browser);
                Console.WriteLine(MethodBase.GetCurrentMethod().Name + " 非后台浏览器创建");
                BrowserTabHost.AttachBrowser(browser);
            }
            else
                Console.WriteLine(MethodBase.GetCurrentMethod().Name + " 后台浏览器创建");

            //FBroSharpBrowser test = new FBroSharpBrowser(browser);
           // Console.WriteLine(test.GetIdentifier());
        }

        //即将关闭
        public override void OnBeforeClose(IFBroSharpBrowser browser)
        {
            Console.WriteLine(MethodBase.GetCurrentMethod().Name);

            for (int i = 0; i < BrowserList.data.Count; i++)
            {
                var temp_browwser = BrowserList.data[i];
                if (temp_browwser.IsSame(browser)){
;                    //BrowserList.data.Remove(temp_browwser);
                    BrowserList.data.RemoveAt(i);
                    break;
                }
            }

            BrowserTabHost.HandleBrowserClosed(browser);
        }
        public override bool DoClose(IFBroSharpBrowser browser)
        {
            return false;
        }

        public override void OnTitleChange(IFBroSharpBrowser browser, string title)
        {
            BrowserTabHost.UpdateTabTitle(browser.GetUserFlag(), title);
        }

        public override void OnAddressChange(IFBroSharpBrowser browser, IFBroSharpFrame frame, string url)
        {
            BrowserTabHost.UpdateTabUrl(browser.GetUserFlag(), url);
        }

        public override void OnLoadingStateChange(IFBroSharpBrowser browser, bool isLoading, bool canGoBack, bool canGoForward)
        {
            BrowserTabHost.UpdateNavigationState(browser.GetUserFlag(), canGoBack, canGoForward);
        }

        public override void OnFaviconURLChange(IFBroSharpBrowser browser, IFBroSharpListValue icon_urls)
        {
            BrowserTabHost.UpdateFaviconUrl(browser.GetUserFlag(), GetFirstFaviconUrl(icon_urls));
        }

        private static string GetFirstFaviconUrl(IFBroSharpListValue iconUrls)
        {
            if (iconUrls == null || !iconUrls.IsValid)
            {
                return string.Empty;
            }

            for (int i = 0; i < iconUrls.GetSize(); i++)
            {
                var iconUrl = iconUrls.GetString(i);
                if (!string.IsNullOrWhiteSpace(iconUrl))
                {
                    return iconUrl;
                }
            }

            return string.Empty;
        }

        //即将下载
        public override void OnBeforeDownload(IFBroSharpBrowser browser, IFBroSharpDownloadItem download_item, string suggested_name, IFBroSharpBeforeDownloadCallback callback)
        {
            Console.WriteLine(MethodBase.GetCurrentMethod().Name, suggested_name);
            callback.Continue(Directory.GetCurrentDirectory() + "\\" + suggested_name, false);
        }

        //下载中
        public override void OnDownloadUpdated(IFBroSharpBrowser browser, IFBroSharpDownloadItem download_item, IFBroSharpDownloadItemCallback callback)
        {
            Console.WriteLine(MethodBase.GetCurrentMethod().Name +
                " 开始时间：" + download_item.GetStartTime().ToString() +
                " 结束时间：" + download_item.GetEndTime().ToString() +
                " 进度：" + download_item.GetPercentComplete() +
                " 速度：" + download_item.GetCurrentSpeed());
        }


        //即将打开菜单
        public override void OnBeforeContextMenu(IFBroSharpBrowser browser, IFBroSharpFrame frame, IFBroSharpContextMenuParams menu_params, IFBroSharpMenuModel model)
        {
            //添加分隔栏
            model.AddSeparator();
            IFBroSharpMenuModel sub_model = model.AddSubMenu(10000, "自定义菜单");

            sub_model.AddItem(10001, "检查");
            sub_model.AddItem(10002, "开发者工具");

        }

        //即将执行菜单命令
        public override bool OnContextMenuCommand(IFBroSharpBrowser browser, IFBroSharpFrame frame, IFBroSharpContextMenuParams menu_params, int command_id, FBroSharpEventFlags event_flags)
        {
            switch (command_id)
            {
                case 10001:
                    FBroSharpElementAt element_at = new FBroSharpElementAt
                    {
                        x = menu_params.GetXCoord(),
                        y = menu_params.GetYCoord()
                    };
                    browser.ShowDevTools("开发者工具", IntPtr.Zero, 0, 0, 1024, 900, default, element_at, default, default);
                    break;
                case 10002:
                    browser.ShowDevTools("开发者工具", IntPtr.Zero, 0, 0, 1024, 900, default, default, default, default);
                    break;
            }
            return false;
        }


        //即将加载资源
        public override FBroSharpReturnValueType OnBeforeResourceLoad(IFBroSharpBrowser browser, IFBroSharpFrame frame, IFBroSharpRequest request, IFBroSharpCallback callback)
        {
            //Console.WriteLine(MethodBase.GetCurrentMethod().Name + " url:" + frame.GetURL() + " request_url:" + request.GetURL());

            //List <FBroSharpStringMap> header_map = request.GetHeaderMap();
            //for(int i = 0; i < header_map.Count; i++)
            //{
            //    Console.WriteLine("key:" + header_map[i].key + " value:" + header_map[i].value);
            //}        
            return FBroSharpReturnValueType.RV_CONTINUE;
        }



        public override bool OnBeforePopup(IFBroSharpBrowser browser, IFBroSharpFrame frame, string target_url, string target_frame_name,
            FBroSharpWindowOpenDisposition target_disposition, bool user_gesture, FBroSharpPopupfeatures popupFeatures, ref FBroSharpWindowsInfo windowInfo,
            ref FBroSharpBrowserSetting settings, ref bool no_javascript_access, IFBroSharpUseExtraData user_settings)
        {
            var popupTab = BrowserTabHost.CreatePopupTab(target_url);
            if (popupTab == null)
            {
                return true;
            }

            windowInfo.parent_window = popupTab.ContentWindow;
            windowInfo.x = 0;
            windowInfo.y = 0;
            if (NativeTabControl.GetClientRect(popupTab.ContentWindow, out var rect))
            {
                windowInfo.width = Math.Max(rect.Right - rect.Left, 1);
                windowInfo.height = Math.Max(rect.Bottom - rect.Top, 1);
            }
            else
            {
                windowInfo.width = 1;
                windowInfo.height = 1;
            }
            windowInfo.window_name = string.IsNullOrWhiteSpace(target_url) ? "新标签页" : target_url;

            //也可以完全新建一个
            //windowInfo = new FBroSharpWindowsInfo()
            //{
            //    parent_window = form.Handle,
            //    x = 0,
            //    y = 0,
            //    width = form.Width,
            //    height = form.Height
            //};

            BrowserEvent browser_event = new BrowserEvent();
            user_settings.SetEvent(browser_event, default);
            user_settings.SetUserFlagData(popupTab.BrowserFlag);

            return false;
        }

        //浏览器_请求证书错误
        public override bool OnCertificateError(IFBroSharpBrowser browser, int cert_error, string request_url, IFBroSharpSSLInfo ssl_info, IFBroSharpCallback callback)
        {
            //继续
            callback.Continue();
            //返回真跳转证书错误网站，返回假不跳转
            return true;
        }
    }



    public class URLEvent: FBroSharpURLEvent
    {
        public override void Start(long flag, IFBroSharpURLRequest request)
        {
            Console.WriteLine(MethodBase.GetCurrentMethod().Name, request.GetRequest().GetURL(), request.GetRequest().GetReferrerURL());
        }

        public override void End(long flag)
        {
            Console.WriteLine(MethodBase.GetCurrentMethod().Name);
        }

        public override bool GetAuthCredentials(long flag, bool isProxy, string host, int port, string realm, string scheme, IFBroSharpAuthCallback callback)
        {
            Console.WriteLine(MethodBase.GetCurrentMethod().Name); 
            return false;
        }
 
        public override void OnDownloadData(long flag, IFBroSharpURLRequest request, Stream data)
        {
            Console.WriteLine(MethodBase.GetCurrentMethod().Name);
            using (StreamReader reader = new StreamReader(data, Encoding.UTF8))
            {
                string result = reader.ReadToEnd();
                Console.WriteLine(result);
            }
        }

        public override void OnDownloadProgress(long flag, IFBroSharpURLRequest request, long current, long total)
        {
            Console.WriteLine(MethodBase.GetCurrentMethod().Name);
        }

        public override void OnRequestComplete(long flag, IFBroSharpURLRequest request)
        {
            Console.WriteLine(MethodBase.GetCurrentMethod().Name);
        }

        public override void OnUploadProgress(long flag, IFBroSharpURLRequest request, long current, long total)
        {
            Console.WriteLine(MethodBase.GetCurrentMethod().Name);
        }

        
    }
}
