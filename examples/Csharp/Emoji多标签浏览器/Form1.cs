using BaseTest;
using System;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Drawing.Imaging;
using System.IO;
using System.Net;
using System.Runtime.InteropServices;
using System.Threading;
using System.Windows.Forms;

namespace Emoji多标签浏览器
{
    public partial class Form1 : Form
    {
        [DllImport("dwmapi.dll", PreserveSig = true)]
        private static extern int DwmSetWindowAttribute(IntPtr hwnd, int attr, ref int attrValue, int attrSize);

        [DllImport("user32.dll")]
        private static extern IntPtr SendMessage(IntPtr hWnd, int msg, IntPtr wp, IntPtr lp);

        private const int DWMWA_CAPTION_COLOR = 35;
        private const int DWMWA_USE_IMMERSIVE_DARK_MODE = 20;
        private const int EM_SETMARGINS = 0xD3;
        private const int EC_LEFTMARGIN = 0x1;

        private int _faviconRequestId;
        private string _currentFaviconUrl = string.Empty;

        public Form1()
        {
            InitializeComponent();
            BrowserTabHost.CurrentTabStateChanged += BrowserTabHost_CurrentTabStateChanged;
        }

        private void Form1_Load(object sender, EventArgs e)
        {
            SetTitleBarColor(Color.FromArgb(37, 175, 254));
            LoadNavigationButtonImages();
            SetDefaultFavicon();
            AdjustFaviconPosition();
            SetTextBoxLeftMargin(this.txtUrl, this.picFavicon.Width + 10);
            ApplyNavigationState(false, false, false);

            BrowserTabHost.Initialize(this.panelBrowser, this.panelBrowser.ClientSize.Width, this.panelBrowser.ClientSize.Height);
            BrowserTabHost.CreateBrowserTab("百度", "https://www.baidu.com/");
        }

        private void Form1_Shown(object sender, EventArgs e)
        {
            AdjustFaviconPosition();
            SetTextBoxLeftMargin(this.txtUrl, this.picFavicon.Width + 10);
        }

        private void Form1_FormClosed(object sender, FormClosedEventArgs e)
        {
            BrowserTabHost.CurrentTabStateChanged -= BrowserTabHost_CurrentTabStateChanged;
            DisposePictureBoxResources(this.picBack);
            DisposePictureBoxResources(this.picForward);
            DisposePictureBoxResources(this.picRefresh);
            DisposeImage(this.picFavicon.Image);
            this.picFavicon.Image = null;
        }

        private void Form1_Resize(object sender, EventArgs e)
        {
            BrowserTabHost.Layout(this.panelBrowser.ClientSize.Width, this.panelBrowser.ClientSize.Height);
            AdjustFaviconPosition();
        }

        private void Form1_ResizeEnd(object sender, EventArgs e)
        {
            BrowserTabHost.Layout(this.panelBrowser.ClientSize.Width, this.panelBrowser.ClientSize.Height);
            AdjustFaviconPosition();
        }

        private void BrowserTabHost_CurrentTabStateChanged(BrowserTabState state)
        {
            if (state == null)
            {
                return;
            }

            this.txtUrl.Text = state.CurrentUrl ?? string.Empty;
            this.btnGo.Enabled = state.HasBrowser;
            ApplyNavigationState(state.CanGoBack, state.CanGoForward, state.HasBrowser);
            UpdateCurrentFavicon(state.CurrentFaviconUrl);
        }

        private void btnBack_Click(object sender, EventArgs e)
        {
            BrowserTabHost.GoBackCurrent();
        }

        private void btnForward_Click(object sender, EventArgs e)
        {
            BrowserTabHost.GoForwardCurrent();
        }

        private void btnRefresh_Click(object sender, EventArgs e)
        {
            BrowserTabHost.ReloadCurrent();
        }

        private void btnGo_Click(object sender, EventArgs e)
        {
            NavigateFromAddressBar();
        }

        private void btnExpand_Click(object sender, EventArgs e)
        {
            MessageBox.Show(this, "这里是工具箱按钮的占位动作。", "工具箱", MessageBoxButtons.OK, MessageBoxIcon.Information);
        }

        private void txtUrl_KeyDown(object sender, KeyEventArgs e)
        {
            if (e.KeyCode != Keys.Enter)
            {
                return;
            }

            e.Handled = true;
            e.SuppressKeyPress = true;
            NavigateFromAddressBar();
        }

        private void btnBack_MouseEnter(object sender, EventArgs e)
        {
            ApplyButtonHoverEffect(this.picBack, true);
        }

        private void btnBack_MouseLeave(object sender, EventArgs e)
        {
            ApplyButtonHoverEffect(this.picBack, false);
        }

        private void btnForward_MouseEnter(object sender, EventArgs e)
        {
            ApplyButtonHoverEffect(this.picForward, true);
        }

        private void btnForward_MouseLeave(object sender, EventArgs e)
        {
            ApplyButtonHoverEffect(this.picForward, false);
        }

        private void btnRefresh_MouseEnter(object sender, EventArgs e)
        {
            ApplyButtonHoverEffect(this.picRefresh, true);
        }

        private void btnRefresh_MouseLeave(object sender, EventArgs e)
        {
            ApplyButtonHoverEffect(this.picRefresh, false);
        }

        private void NavigateFromAddressBar()
        {
            BrowserTabHost.NavigateCurrent(this.txtUrl.Text);
        }

        private void LoadNavigationButtonImages()
        {
            LoadPictureBoxImage(this.picBack, "后退.png");
            LoadPictureBoxImage(this.picForward, "前进.png");
            LoadPictureBoxImage(this.picRefresh, "刷新.png");
        }

        private void LoadPictureBoxImage(PictureBox pictureBox, string fileName)
        {
            string imagePath = GetAssetPath(fileName);
            if (string.IsNullOrWhiteSpace(imagePath) || !File.Exists(imagePath))
            {
                return;
            }

            Image loadedImage = null;
            try
            {
                loadedImage = Image.FromFile(imagePath);
                ReplacePictureBoxBackgroundImage(pictureBox, (Image)loadedImage.Clone());
                DisposeImage(pictureBox.Tag as Image);
                pictureBox.Tag = (Image)loadedImage.Clone();
            }
            finally
            {
                DisposeImage(loadedImage);
            }
        }

        private string GetAssetPath(string fileName)
        {
            string[] candidatePaths =
            {
                Path.Combine(Application.StartupPath, fileName),
                Path.Combine(AppDomain.CurrentDomain.BaseDirectory, fileName),
                Path.Combine(AppDomain.CurrentDomain.BaseDirectory, "Resources", fileName),
                Path.Combine(Directory.GetCurrentDirectory(), fileName),
                Path.Combine(AppDomain.CurrentDomain.BaseDirectory, "..", "..", fileName),
                Path.Combine(AppDomain.CurrentDomain.BaseDirectory, "..", "..", "..", fileName)
            };

            foreach (var candidatePath in candidatePaths)
            {
                try
                {
                    var fullPath = Path.GetFullPath(candidatePath);
                    if (File.Exists(fullPath))
                    {
                        return fullPath;
                    }
                }
                catch
                {
                }
            }

            return string.Empty;
        }

        private void ApplyNavigationState(bool canGoBack, bool canGoForward, bool hasBrowser)
        {
            SetPictureBoxEnabledState(this.picBack, canGoBack);
            SetPictureBoxEnabledState(this.picForward, canGoForward);
            SetPictureBoxEnabledState(this.picRefresh, hasBrowser);
        }

        private void SetPictureBoxEnabledState(PictureBox pictureBox, bool enabled)
        {
            pictureBox.Enabled = enabled;
            pictureBox.Cursor = enabled ? Cursors.Hand : Cursors.Default;

            var originalImage = pictureBox.Tag as Image;
            if (originalImage == null)
            {
                return;
            }

            ReplacePictureBoxBackgroundImage(
                pictureBox,
                enabled ? (Image)originalImage.Clone() : AdjustImageOpacity(originalImage, 0.35f));
        }

        private void ApplyButtonHoverEffect(PictureBox pictureBox, bool isHover)
        {
            if (!pictureBox.Enabled)
            {
                return;
            }

            var originalImage = pictureBox.Tag as Image;
            if (originalImage == null)
            {
                return;
            }

            ReplacePictureBoxBackgroundImage(
                pictureBox,
                isHover ? AdjustImageOpacity(originalImage, 0.5f) : (Image)originalImage.Clone());
        }

        private void ReplacePictureBoxBackgroundImage(PictureBox pictureBox, Image image)
        {
            var oldImage = pictureBox.BackgroundImage;
            pictureBox.BackgroundImage = image;
            if (oldImage != null && !ReferenceEquals(oldImage, pictureBox.Tag))
            {
                oldImage.Dispose();
            }
        }

        private Image AdjustImageOpacity(Image image, float opacity)
        {
            if (image == null)
            {
                return null;
            }

            var bitmap = new Bitmap(image.Width, image.Height);
            using (var graphics = Graphics.FromImage(bitmap))
            using (var attributes = new ImageAttributes())
            {
                graphics.SmoothingMode = SmoothingMode.HighQuality;
                graphics.InterpolationMode = InterpolationMode.HighQualityBicubic;
                graphics.PixelOffsetMode = PixelOffsetMode.HighQuality;

                var matrix = new ColorMatrix();
                matrix.Matrix33 = opacity;
                attributes.SetColorMatrix(matrix, ColorMatrixFlag.Default, ColorAdjustType.Bitmap);
                graphics.DrawImage(
                    image,
                    new Rectangle(0, 0, bitmap.Width, bitmap.Height),
                    0,
                    0,
                    image.Width,
                    image.Height,
                    GraphicsUnit.Pixel,
                    attributes);
            }

            return bitmap;
        }

        private void SetTitleBarColor(Color color)
        {
            try
            {
                int darkMode = 1;
                DwmSetWindowAttribute(this.Handle, DWMWA_USE_IMMERSIVE_DARK_MODE, ref darkMode, sizeof(int));

                int colorValue = color.R | (color.G << 8) | (color.B << 16);
                DwmSetWindowAttribute(this.Handle, DWMWA_CAPTION_COLOR, ref colorValue, sizeof(int));
            }
            catch
            {
            }
        }

        private void UpdateCurrentFavicon(string faviconUrl)
        {
            faviconUrl = faviconUrl ?? string.Empty;
            if (string.Equals(_currentFaviconUrl, faviconUrl, StringComparison.Ordinal))
            {
                return;
            }

            _currentFaviconUrl = faviconUrl;
            int requestId = Interlocked.Increment(ref _faviconRequestId);
            if (string.IsNullOrWhiteSpace(faviconUrl))
            {
                SetDefaultFavicon();
                return;
            }

            ThreadPool.QueueUserWorkItem(_ => LoadFavicon(requestId, faviconUrl));
        }

        private void LoadFavicon(int requestId, string faviconUrl)
        {
            Image faviconImage = null;

            try
            {
                using (var webClient = new WebClient())
                {
                    byte[] bytes = webClient.DownloadData(faviconUrl);
                    using (var memoryStream = new MemoryStream(bytes))
                    using (var sourceImage = Image.FromStream(memoryStream, false, false))
                    {
                        faviconImage = new Bitmap(sourceImage);
                    }
                }
            }
            catch
            {
            }

            if (this.IsDisposed || !this.IsHandleCreated)
            {
                DisposeImage(faviconImage);
                return;
            }

            BeginInvoke(new Action(() =>
            {
                if (requestId != _faviconRequestId)
                {
                    DisposeImage(faviconImage);
                    return;
                }

                if (faviconImage == null)
                {
                    SetDefaultFavicon();
                    return;
                }

                SetFaviconImage(faviconImage);
            }));
        }

        private void SetDefaultFavicon()
        {
            var size = Math.Max(Math.Min(this.picFavicon.Width, this.picFavicon.Height), 16);
            var favicon = new Bitmap(size, size);

            using (var graphics = Graphics.FromImage(favicon))
            using (var backgroundBrush = new SolidBrush(Color.FromArgb(100, 100, 100)))
            using (var pen = new Pen(Color.White, Math.Max(1.2f, size / 14f)))
            {
                graphics.SmoothingMode = SmoothingMode.AntiAlias;
                graphics.InterpolationMode = InterpolationMode.HighQualityBicubic;

                int margin = Math.Max(1, size / 10);
                int radius = size - (margin * 2);
                int center = size / 2;

                graphics.FillEllipse(backgroundBrush, margin, margin, radius, radius);
                graphics.DrawEllipse(pen, margin + 1, margin + 1, Math.Max(radius - 2, 1), Math.Max(radius - 2, 1));
                graphics.DrawLine(pen, center, margin + 2, center, size - margin - 2);
                graphics.DrawLine(pen, margin + 2, center, size - margin - 2, center);
                graphics.DrawEllipse(pen, Math.Max(margin + 2, 2), margin + 2, Math.Max(size / 3, 1), Math.Max(radius - 4, 1));
            }

            SetFaviconImage(favicon);
        }

        private void SetFaviconImage(Image image)
        {
            int width = Math.Max(this.picFavicon.Width, 16);
            int height = Math.Max(this.picFavicon.Height, 16);
            var scaledImage = new Bitmap(width, height);

            using (var graphics = Graphics.FromImage(scaledImage))
            {
                graphics.SmoothingMode = SmoothingMode.HighQuality;
                graphics.InterpolationMode = InterpolationMode.HighQualityBicubic;
                graphics.PixelOffsetMode = PixelOffsetMode.HighQuality;
                graphics.Clear(Color.Transparent);
                graphics.DrawImage(image, new Rectangle(0, 0, width, height));
            }

            DisposeImage(image);

            var oldImage = this.picFavicon.Image;
            this.picFavicon.Image = scaledImage;
            if (oldImage != null)
            {
                oldImage.Dispose();
            }
        }

        private void AdjustFaviconPosition()
        {
            try
            {
                int centerY = (this.txtUrl.ClientSize.Height - this.picFavicon.Height) / 2;
                this.picFavicon.Location = new Point(3, Math.Max(centerY, 0));
            }
            catch
            {
            }
        }

        private void SetTextBoxLeftMargin(TextBox textBox, int leftMargin)
        {
            try
            {
                SendMessage(textBox.Handle, EM_SETMARGINS, (IntPtr)EC_LEFTMARGIN, (IntPtr)leftMargin);
            }
            catch
            {
            }
        }

        private void DisposePictureBoxResources(PictureBox pictureBox)
        {
            if (pictureBox == null)
            {
                return;
            }

            DisposeImage(pictureBox.BackgroundImage);
            pictureBox.BackgroundImage = null;
            DisposeImage(pictureBox.Tag as Image);
            pictureBox.Tag = null;
        }

        private void DisposeImage(Image image)
        {
            if (image == null)
            {
                return;
            }

            image.Dispose();
        }
    }
}
