using System;

namespace EmojiWindowChromeStyleBrowserDemo.Core
{
    internal static class BrowserText
    {
        public static string NormalizeUrl(string input)
        {
            string value = (input ?? string.Empty).Trim();
            if (string.IsNullOrEmpty(value))
            {
                return "https://www.google.com/";
            }

            if (!value.Contains("://"))
            {
                value = "https://" + value;
            }

            return value;
        }

        public static string TitleFromUrl(string url)
        {
            if (Uri.TryCreate(url, UriKind.Absolute, out Uri uri))
            {
                if (!string.IsNullOrEmpty(uri.Host))
                {
                    return uri.Host.ToLowerInvariant() + "/";
                }

                if (!string.IsNullOrEmpty(uri.AbsolutePath) && uri.AbsolutePath != "/")
                {
                    return uri.AbsolutePath.Trim('/') + "/";
                }
            }

            string fallback = (url ?? string.Empty).Trim();
            return string.IsNullOrEmpty(fallback) ? "new-tab/" : fallback;
        }

        public static string ShortUrl(string url, int limit)
        {
            if (string.IsNullOrEmpty(url) || url.Length <= limit)
            {
                return url ?? string.Empty;
            }

            return url.Substring(0, Math.Max(0, limit - 3)) + "...";
        }

        public static string ChromeTabCaption(string url)
        {
            if (Uri.TryCreate(url, UriKind.Absolute, out Uri uri))
            {
                string host = uri.Host;
                if (host.StartsWith("www.", StringComparison.OrdinalIgnoreCase))
                {
                    host = host.Substring(4);
                }

                if (!string.IsNullOrEmpty(host))
                {
                    return host.Length <= 20 ? host : host.Substring(0, 17) + "...";
                }
            }

            string fallback = string.IsNullOrWhiteSpace(url) ? "New Tab" : url.Trim();
            return fallback.Length <= 20 ? fallback : fallback.Substring(0, 17) + "...";
        }

        public static string BadgeFromUrl(string url)
        {
            if (Uri.TryCreate(url, UriKind.Absolute, out Uri uri))
            {
                string host = uri.Host;
                if (!string.IsNullOrEmpty(host))
                {
                    return host.Substring(0, 1).ToUpperInvariant();
                }
            }

            return "G";
        }
    }
}
