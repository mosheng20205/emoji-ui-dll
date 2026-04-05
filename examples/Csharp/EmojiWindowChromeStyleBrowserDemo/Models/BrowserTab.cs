using System.Collections.Generic;

namespace EmojiWindowChromeStyleBrowserDemo.Models
{
    internal sealed class BrowserTab
    {
        public int ButtonId { get; set; }
        public string Title { get; set; }
        public string HomeUrl { get; set; }
        public List<string> History { get; } = new List<string>();
        public int HistoryIndex { get; set; } = -1;

        public string CurrentUrl
        {
            get
            {
                if (HistoryIndex >= 0 && HistoryIndex < History.Count)
                {
                    return History[HistoryIndex];
                }

                return HomeUrl ?? "https://www.google.com/";
            }
        }

        public bool CanGoBack => HistoryIndex > 0;

        public bool CanGoForward => HistoryIndex >= 0 && HistoryIndex < History.Count - 1;
    }
}
