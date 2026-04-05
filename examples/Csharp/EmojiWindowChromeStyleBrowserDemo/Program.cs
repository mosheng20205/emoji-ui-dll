using System;

namespace EmojiWindowChromeStyleBrowserDemo
{
    internal static class Program
    {
        [STAThread]
        private static void Main()
        {
            var app = new ChromeBrowserApp();
            app.Run();
        }
    }
}
