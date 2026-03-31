using FBroSharp;
using FBroSharp.Const;
using FBroSharp.Event;
using FBroSharp.Lib;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Reflection;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Forms;

namespace BaseTest
{
    public class InitEvent : FBroInitEvent
    {
        //即将处理命令行
        public override void OnBeforeCommandLineProcessing(string process_type, IFBroSharpCommandLine cmd)
        {
            //if (Debugger.IsAttached)
            //{
            //    cmd.AppendSwitch("--single-process");
            //}

            ////关闭GPU加速
            //cmd.AppendSwitchWithValue("--disable-gpu", "1");
            //cmd.AppendSwitchWithValue("--disable-gpu-compositing", "1");
            cmd.DisableGpuBlockList();
            cmd.DisableGpuCache();

            Console.WriteLine(MethodBase.GetCurrentMethod().Name + ":" + process_type + cmd.IsReadOnly() + cmd.GetProgram());
        }

        //初始化完毕
        public override void OnContextInitialized()
        {
            Console.WriteLine(MethodBase.GetCurrentMethod().Name);



        }

        public override void OnBeforeChildProcessLaunch(IFBroSharpCommandLine command_line)
        {
            Console.WriteLine(MethodBase.GetCurrentMethod().Name + ":" + command_line.IsReadOnly() + command_line.GetProgram());
        }

        public override bool OnProcessMessageReceived(IFBroSharpBrowser browser, IFBroSharpFrame frame, FBroSharpProcessId source_process, IFBroSharpProcessMessage message)
        {
            var list = message.GetArgumentList();

            //因为这里跨进程了，所有用信息框显示，而且要用当前exe作为执行进程才会有用
            MessageBox.Show(message.GetName() + ":" + list.GetString(0), MethodBase.GetCurrentMethod().Name);
            //返回真才会释放当前数据，代表数据处理完了
            return true;
        }
    }
}