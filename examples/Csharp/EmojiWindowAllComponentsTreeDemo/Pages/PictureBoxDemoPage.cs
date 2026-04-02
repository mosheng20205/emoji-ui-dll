using System;
using System.Collections.Generic;
using System.IO;

namespace EmojiWindowDemo
{
    internal static class PictureBoxDemoPage
    {
        public static void Build(AllDemoShell shell, IntPtr page)
        {
            var app = shell.App;
            IntPtr picture = EmojiWindowNative.CreatePictureBox(page, 40, 176, 912, 308, PageCommon.ScaleFit, DemoColors.White);
            IntPtr stateLabel = app.Label(1020, 96, 404, 92, "等待 PictureBox 状态。", DemoColors.Black, DemoColors.Surface, 12, PageCommon.AlignLeft, true, page);
            IntPtr eventLabel = app.Label(1020, 214, 404, 284, "等待 PictureBox 事件。", DemoColors.Gray, DemoColors.Surface, 12, PageCommon.AlignLeft, true, page);
            IntPtr tip = app.Label(40, 508, 900, 24, "点击图片框后可以继续按字母、数字、方向键或 Tab，右侧会记录键盘和焦点事件。", DemoColors.Gray, DemoColors.WindowBg, 12, PageCommon.AlignLeft, true, page);

            string pngPath = PageCommon.FindFirstExistingPath(app, @"imgs\图片框示例.png", @"imgs\1.png");
            string altPath = PageCommon.FindFirstExistingPath(app, @"imgs\2.png", @"imgs\3.png");
            var events = new List<string>();

            int x = 40;
            int y = 176;
            int width = 912;
            int height = 308;
            bool visible = true;
            bool enabled = true;
            float opacity = 1.0f;
            int scaleMode = PageCommon.ScaleFit;
            string scaleText = "FIT";
            uint background = DemoColors.White;
            string imageName = "未加载";
            bool loaded = false;

            void PushEvent(string message)
            {
                events.Insert(0, message);
                if (events.Count > 8)
                {
                    events.RemoveAt(events.Count - 1);
                }

                shell.SetLabelText(eventLabel, string.Join("\r\n", events));
            }

            void Refresh(string note)
            {
                bool hasFocus = Win32Native.GetFocus() == picture;
                shell.SetLabelText(
                    stateLabel,
                    $"图片={imageName}  已加载={(loaded ? "是" : "否")}  可见={(visible ? "是" : "否")}  启用={(enabled ? "是" : "否")}\r\n" +
                    $"模式={scaleText}  透明度={(int)(opacity * 100)}%  焦点={(hasFocus ? "是" : "否")}\r\n" +
                    $"背景={PageCommon.FormatColor(background)}  位置=({x}, {y})  尺寸={width} x {height}\r\n" +
                    $"{note}");
                shell.SetStatus(note);
            }

            void LoadFromPath(string path, string label)
            {
                bool ok = false;
                if (!string.IsNullOrEmpty(path) && File.Exists(path))
                {
                    byte[] bytes = app.U(path);
                    ok = EmojiWindowNative.LoadImageFromFile(picture, bytes, bytes.Length) != 0;
                }

                if (!ok)
                {
                    ok = EmojiWindowNative.LoadImageFromMemory(picture, PageCommon.TinyPngBytes, PageCommon.TinyPngBytes.Length) != 0;
                    label = ok ? "内存占位图" : "未加载";
                }

                loaded = ok;
                imageName = label;
                Refresh(ok ? $"PictureBox 已加载 {label}" : "PictureBox 加载失败");
            }

            void SetScale(int mode, string label, string note)
            {
                scaleMode = mode;
                scaleText = label;
                EmojiWindowNative.SetPictureBoxScaleMode(picture, mode);
                Refresh(note);
            }

            void SetOpacity(float value, string note)
            {
                opacity = value;
                EmojiWindowNative.SetImageOpacity(picture, value);
                Refresh(note);
            }

            var clickCallback = app.Pin(new EmojiWindowNative.PictureBoxCallback(_ =>
            {
                PushEvent("Click -> PictureBox 被单击");
                Refresh("PictureBox 点击事件已触发");
            }));
            var mouseEnterCallback = app.Pin(new EmojiWindowNative.ValueChangedCallback(_ => PushEvent("MouseEnter -> 鼠标进入 PictureBox")));
            var mouseLeaveCallback = app.Pin(new EmojiWindowNative.ValueChangedCallback(_ => PushEvent("MouseLeave -> 鼠标离开 PictureBox")));
            var doubleClickCallback = app.Pin(new EmojiWindowNative.ControlPointCallback((_, px, py) =>
            {
                PushEvent($"DoubleClick -> ({px}, {py})");
                Refresh("PictureBox 双击事件已触发");
            }));
            var rightClickCallback = app.Pin(new EmojiWindowNative.ControlPointCallback((_, px, py) =>
            {
                PushEvent($"RightClick -> ({px}, {py})");
                Refresh("PictureBox 右键事件已触发");
            }));
            var focusCallback = app.Pin(new EmojiWindowNative.ValueChangedCallback(_ =>
            {
                PushEvent("Focus -> PictureBox 获得焦点");
                Refresh("PictureBox 已获得焦点");
            }));
            var blurCallback = app.Pin(new EmojiWindowNative.ValueChangedCallback(_ =>
            {
                PushEvent("Blur -> PictureBox 失去焦点");
                Refresh("PictureBox 已失去焦点");
            }));
            var keyDownCallback = app.Pin(new EmojiWindowNative.ControlKeyCallback((_, vk, shift, ctrl, alt) =>
                PushEvent($"KeyDown -> {PageCommon.FormatHotKey(vk, (shift != 0 ? 1 : 0) | (ctrl != 0 ? 2 : 0) | (alt != 0 ? 4 : 0))}")));
            var keyUpCallback = app.Pin(new EmojiWindowNative.ControlKeyCallback((_, vk, shift, ctrl, alt) =>
                PushEvent($"KeyUp -> {PageCommon.FormatHotKey(vk, (shift != 0 ? 1 : 0) | (ctrl != 0 ? 2 : 0) | (alt != 0 ? 4 : 0))}")));
            var charCallback = app.Pin(new EmojiWindowNative.ControlCharCallback((_, charCode) =>
                PushEvent($"Char -> {(charCode >= 32 && charCode <= 126 ? ((char)charCode).ToString() : "U+" + charCode.ToString("X4"))}")));
            var valueChangedCallback = app.Pin(new EmojiWindowNative.ValueChangedCallback(_ => PushEvent("ValueChanged -> 图片内容或属性已变化")));

            EmojiWindowNative.SetPictureBoxCallback(picture, clickCallback);
            EmojiWindowNative.SetMouseEnterCallback(picture, mouseEnterCallback);
            EmojiWindowNative.SetMouseLeaveCallback(picture, mouseLeaveCallback);
            EmojiWindowNative.SetDoubleClickCallback(picture, doubleClickCallback);
            EmojiWindowNative.SetRightClickCallback(picture, rightClickCallback);
            EmojiWindowNative.SetFocusCallback(picture, focusCallback);
            EmojiWindowNative.SetBlurCallback(picture, blurCallback);
            EmojiWindowNative.SetKeyDownCallback(picture, keyDownCallback);
            EmojiWindowNative.SetKeyUpCallback(picture, keyUpCallback);
            EmojiWindowNative.SetCharCallback(picture, charCallback);
            EmojiWindowNative.SetValueChangedCallback(picture, valueChangedCallback);

            app.Button(40, 96, 120, 34, "PNG 示例", "🖼️", DemoColors.Blue, () => LoadFromPath(pngPath, Path.GetFileName(pngPath)), page);
            app.Button(172, 96, 120, 34, "第二张图", "📷", DemoColors.Green, () => LoadFromPath(altPath, Path.GetFileName(altPath)), page);
            app.Button(304, 96, 120, 34, "清空图片", "🧹", DemoColors.Orange, () =>
            {
                EmojiWindowNative.ClearImage(picture);
                loaded = false;
                imageName = "已清空";
                Refresh("PictureBox 图片已清空");
            }, page);
            app.Button(436, 96, 120, 34, "原始大小", "1:1", DemoColors.Red, () => SetScale(PageCommon.ScaleNone, "NONE", "PictureBox 缩放模式已切到 NONE"), page);
            app.Button(568, 96, 120, 34, "拉伸铺满", "↔", DemoColors.Gray, () => SetScale(PageCommon.ScaleStretch, "STRETCH", "PictureBox 缩放模式已切到 STRETCH"), page);

            app.Button(40, 138, 120, 34, "等比适应", "FIT", DemoColors.Purple, () => SetScale(PageCommon.ScaleFit, "FIT", "PictureBox 缩放模式已切到 FIT"), page);
            app.Button(172, 138, 120, 34, "居中显示", "◎", DemoColors.Blue, () => SetScale(PageCommon.ScaleCenter, "CENTER", "PictureBox 缩放模式已切到 CENTER"), page);
            app.Button(304, 138, 120, 34, "100%", "100%", DemoColors.Green, () => SetOpacity(1.0f, "PictureBox 透明度已设为 100%"), page);
            app.Button(436, 138, 120, 34, "60%", "60%", DemoColors.Orange, () => SetOpacity(0.6f, "PictureBox 透明度已设为 60%"), page);
            app.Button(568, 138, 120, 34, "25%", "25%", DemoColors.Red, () => SetOpacity(0.25f, "PictureBox 透明度已设为 25%"), page);

            app.Button(1020, 520, 94, 34, "聚焦图片", "🎯", DemoColors.Blue, () =>
            {
                Win32Native.SetFocus(picture);
                Refresh("焦点已切到 PictureBox");
            }, page);
            app.Button(1124, 520, 94, 34, "移开焦点", "↩", DemoColors.Gray, () =>
            {
                Win32Native.SetFocus(page);
                Refresh("焦点已从 PictureBox 移开");
            }, page);
            app.Button(1228, 520, 94, 34, "显示切换", "👁", DemoColors.Green, () =>
            {
                visible = !visible;
                EmojiWindowNative.ShowPictureBox(picture, visible ? 1 : 0);
                Refresh(visible ? "PictureBox 已显示" : "PictureBox 已隐藏");
            }, page);
            app.Button(1332, 520, 94, 34, "启用切换", "🚫", DemoColors.Red, () =>
            {
                enabled = !enabled;
                EmojiWindowNative.EnablePictureBox(picture, enabled ? 1 : 0);
                Refresh(enabled ? "PictureBox 已启用" : "PictureBox 已禁用");
            }, page);

            app.Button(1020, 562, 94, 34, "放大预览", "⬆", DemoColors.Purple, () =>
            {
                x = 40;
                y = 176;
                width = 980;
                height = 332;
                EmojiWindowNative.SetPictureBoxBounds(picture, x, y, width, height);
                Refresh("PictureBox 预览区域已放大");
            }, page);
            app.Button(1124, 562, 94, 34, "恢复默认", "↺", DemoColors.Blue, () =>
            {
                x = 40;
                y = 176;
                width = 912;
                height = 308;
                visible = true;
                enabled = true;
                opacity = 1.0f;
                background = DemoColors.White;
                EmojiWindowNative.SetPictureBoxBounds(picture, x, y, width, height);
                EmojiWindowNative.ShowPictureBox(picture, 1);
                EmojiWindowNative.EnablePictureBox(picture, 1);
                EmojiWindowNative.SetPictureBoxBackgroundColor(picture, background);
                SetScale(PageCommon.ScaleFit, "FIT", "PictureBox 已恢复默认状态");
                LoadFromPath(pngPath, Path.GetFileName(pngPath));
            }, page);
            app.Button(1228, 562, 94, 34, "冷色底", "💙", DemoColors.Green, () =>
            {
                background = DemoColors.LightBlue;
                EmojiWindowNative.SetPictureBoxBackgroundColor(picture, background);
                Refresh("PictureBox 已切到冷色背景");
            }, page);
            app.Button(1332, 562, 94, 34, "暖色底", "🧡", DemoColors.Orange, () =>
            {
                background = DemoColors.Yellow;
                EmojiWindowNative.SetPictureBoxBackgroundColor(picture, background);
                Refresh("PictureBox 已切到暖色背景");
            }, page);

            app.Label(40, 598, 1320, 22, "1. CreatePictureBox / LoadImageFromFile / LoadImageFromMemory / ClearImage：创建图片框、加载本地图片、回退到内存占位图以及清空内容。", DemoColors.Black, DemoColors.WindowBg, 12, PageCommon.AlignLeft, false, page);
            app.Label(40, 632, 1320, 22, "2. SetPictureBoxScaleMode：直接切换 NONE / STRETCH / FIT / CENTER 四种缩放模式。", DemoColors.Black, DemoColors.WindowBg, 12, PageCommon.AlignLeft, false, page);
            app.Label(40, 666, 1320, 22, "3. SetImageOpacity / SetPictureBoxBackgroundColor：验证透明度和图片框底色可独立调整。", DemoColors.Black, DemoColors.WindowBg, 12, PageCommon.AlignLeft, false, page);
            app.Label(40, 700, 1320, 22, "4. SetPictureBoxBounds / ShowPictureBox / EnablePictureBox：演示布局调整、可见性和启用状态切换。", DemoColors.Black, DemoColors.WindowBg, 12, PageCommon.AlignLeft, false, page);
            app.Label(40, 734, 1320, 22, "5. SetPictureBoxCallback + 通用事件回调：页面已接入 Click、DoubleClick、RightClick、MouseEnter、MouseLeave、Focus、Blur、KeyDown、KeyUp、Char、ValueChanged。", DemoColors.Black, DemoColors.WindowBg, 12, PageCommon.AlignLeft, false, page);

            LoadFromPath(pngPath, Path.GetFileName(pngPath));
            Refresh("图片框页已加载，右侧集中展示状态与事件回写。");
        }
    }
}
