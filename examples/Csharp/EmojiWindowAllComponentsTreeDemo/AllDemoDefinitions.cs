using System;
using System.Collections.Generic;

namespace EmojiWindowDemo
{
    internal sealed class DemoPageDefinition
    {
        public DemoPageDefinition(string key, string title, string description, Action<AllDemoShell, IntPtr> build)
        {
            Key = key;
            Title = title;
            Description = description;
            Build = build;
        }

        public string Key { get; }
        public string Title { get; }
        public string Description { get; }
        public Action<AllDemoShell, IntPtr> Build { get; }
    }

    internal sealed class DemoCategoryDefinition
    {
        public DemoCategoryDefinition(string title, string icon, DemoPageDefinition overviewPage, IReadOnlyList<DemoPageDefinition> pages)
        {
            Title = title;
            Icon = icon;
            OverviewPage = overviewPage;
            Pages = pages;
        }

        public string Title { get; }
        public string Icon { get; }
        public DemoPageDefinition OverviewPage { get; }
        public IReadOnlyList<DemoPageDefinition> Pages { get; }
    }
}
