namespace Emoji多标签浏览器
{
    partial class Form1
    {
        /// <summary>
        /// 必需的设计器变量。
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// 清理所有正在使用的资源。
        /// </summary>
        /// <param name="disposing">如果应释放托管资源，为 true；否则为 false。</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }

            base.Dispose(disposing);
        }

        #region Windows 窗体设计器生成的代码

        /// <summary>
        /// 设计器支持所需的方法 - 不要修改
        /// 使用代码编辑器修改此方法的内容。
        /// </summary>
        private void InitializeComponent()
        {
            this.components = new System.ComponentModel.Container();
            this.tableLayoutPanelMain = new System.Windows.Forms.TableLayoutPanel();
            this.tableLayoutPanelTop = new System.Windows.Forms.TableLayoutPanel();
            this.picBack = new System.Windows.Forms.PictureBox();
            this.picForward = new System.Windows.Forms.PictureBox();
            this.picRefresh = new System.Windows.Forms.PictureBox();
            this.txtUrl = new System.Windows.Forms.TextBox();
            this.picFavicon = new System.Windows.Forms.PictureBox();
            this.btnGo = new System.Windows.Forms.Button();
            this.btnExpand = new System.Windows.Forms.Button();
            this.tableLayoutPanelDown = new System.Windows.Forms.TableLayoutPanel();
            this.panelBrowser = new System.Windows.Forms.Panel();
            this.toolTip = new System.Windows.Forms.ToolTip(this.components);
            this.tableLayoutPanelMain.SuspendLayout();
            this.tableLayoutPanelTop.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.picBack)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.picForward)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.picRefresh)).BeginInit();
            this.txtUrl.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.picFavicon)).BeginInit();
            this.tableLayoutPanelDown.SuspendLayout();
            this.SuspendLayout();
            // 
            // tableLayoutPanelMain
            // 
            this.tableLayoutPanelMain.ColumnCount = 1;
            this.tableLayoutPanelMain.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 100F));
            this.tableLayoutPanelMain.Controls.Add(this.tableLayoutPanelTop, 0, 0);
            this.tableLayoutPanelMain.Controls.Add(this.tableLayoutPanelDown, 0, 1);
            this.tableLayoutPanelMain.Dock = System.Windows.Forms.DockStyle.Fill;
            this.tableLayoutPanelMain.Location = new System.Drawing.Point(0, 0);
            this.tableLayoutPanelMain.Margin = new System.Windows.Forms.Padding(0);
            this.tableLayoutPanelMain.Name = "tableLayoutPanelMain";
            this.tableLayoutPanelMain.RowCount = 2;
            this.tableLayoutPanelMain.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 60F));
            this.tableLayoutPanelMain.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 100F));
            this.tableLayoutPanelMain.Size = new System.Drawing.Size(1563, 932);
            this.tableLayoutPanelMain.TabIndex = 0;
            // 
            // tableLayoutPanelTop
            // 
            this.tableLayoutPanelTop.BackColor = System.Drawing.Color.Black;
            this.tableLayoutPanelTop.ColumnCount = 6;
            this.tableLayoutPanelTop.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 45F));
            this.tableLayoutPanelTop.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 45F));
            this.tableLayoutPanelTop.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 45F));
            this.tableLayoutPanelTop.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 100F));
            this.tableLayoutPanelTop.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 90F));
            this.tableLayoutPanelTop.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 130F));
            this.tableLayoutPanelTop.Controls.Add(this.picBack, 0, 0);
            this.tableLayoutPanelTop.Controls.Add(this.picForward, 1, 0);
            this.tableLayoutPanelTop.Controls.Add(this.picRefresh, 2, 0);
            this.tableLayoutPanelTop.Controls.Add(this.txtUrl, 3, 0);
            this.tableLayoutPanelTop.Controls.Add(this.btnGo, 4, 0);
            this.tableLayoutPanelTop.Controls.Add(this.btnExpand, 5, 0);
            this.tableLayoutPanelTop.Dock = System.Windows.Forms.DockStyle.Fill;
            this.tableLayoutPanelTop.Location = new System.Drawing.Point(0, 0);
            this.tableLayoutPanelTop.Margin = new System.Windows.Forms.Padding(0);
            this.tableLayoutPanelTop.Name = "tableLayoutPanelTop";
            this.tableLayoutPanelTop.RowCount = 1;
            this.tableLayoutPanelTop.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 100F));
            this.tableLayoutPanelTop.Size = new System.Drawing.Size(1563, 60);
            this.tableLayoutPanelTop.TabIndex = 0;
            // 
            // picBack
            // 
            this.picBack.BackColor = System.Drawing.Color.Transparent;
            this.picBack.BackgroundImageLayout = System.Windows.Forms.ImageLayout.Zoom;
            this.picBack.Cursor = System.Windows.Forms.Cursors.Hand;
            this.picBack.Dock = System.Windows.Forms.DockStyle.Fill;
            this.picBack.Location = new System.Drawing.Point(4, 4);
            this.picBack.Margin = new System.Windows.Forms.Padding(4);
            this.picBack.Name = "picBack";
            this.picBack.Size = new System.Drawing.Size(37, 52);
            this.picBack.SizeMode = System.Windows.Forms.PictureBoxSizeMode.Zoom;
            this.picBack.TabIndex = 0;
            this.picBack.TabStop = false;
            this.toolTip.SetToolTip(this.picBack, "后退");
            this.picBack.Click += new System.EventHandler(this.btnBack_Click);
            this.picBack.MouseEnter += new System.EventHandler(this.btnBack_MouseEnter);
            this.picBack.MouseLeave += new System.EventHandler(this.btnBack_MouseLeave);
            // 
            // picForward
            // 
            this.picForward.BackColor = System.Drawing.Color.Transparent;
            this.picForward.BackgroundImageLayout = System.Windows.Forms.ImageLayout.Zoom;
            this.picForward.Cursor = System.Windows.Forms.Cursors.Hand;
            this.picForward.Dock = System.Windows.Forms.DockStyle.Fill;
            this.picForward.Location = new System.Drawing.Point(49, 4);
            this.picForward.Margin = new System.Windows.Forms.Padding(4);
            this.picForward.Name = "picForward";
            this.picForward.Size = new System.Drawing.Size(37, 52);
            this.picForward.SizeMode = System.Windows.Forms.PictureBoxSizeMode.Zoom;
            this.picForward.TabIndex = 1;
            this.picForward.TabStop = false;
            this.toolTip.SetToolTip(this.picForward, "前进");
            this.picForward.Click += new System.EventHandler(this.btnForward_Click);
            this.picForward.MouseEnter += new System.EventHandler(this.btnForward_MouseEnter);
            this.picForward.MouseLeave += new System.EventHandler(this.btnForward_MouseLeave);
            // 
            // picRefresh
            // 
            this.picRefresh.BackColor = System.Drawing.Color.Transparent;
            this.picRefresh.BackgroundImageLayout = System.Windows.Forms.ImageLayout.Zoom;
            this.picRefresh.Cursor = System.Windows.Forms.Cursors.Hand;
            this.picRefresh.Dock = System.Windows.Forms.DockStyle.Fill;
            this.picRefresh.Location = new System.Drawing.Point(94, 4);
            this.picRefresh.Margin = new System.Windows.Forms.Padding(4);
            this.picRefresh.Name = "picRefresh";
            this.picRefresh.Size = new System.Drawing.Size(37, 52);
            this.picRefresh.SizeMode = System.Windows.Forms.PictureBoxSizeMode.Zoom;
            this.picRefresh.TabIndex = 2;
            this.picRefresh.TabStop = false;
            this.toolTip.SetToolTip(this.picRefresh, "刷新");
            this.picRefresh.Click += new System.EventHandler(this.btnRefresh_Click);
            this.picRefresh.MouseEnter += new System.EventHandler(this.btnRefresh_MouseEnter);
            this.picRefresh.MouseLeave += new System.EventHandler(this.btnRefresh_MouseLeave);
            // 
            // txtUrl
            // 
            this.txtUrl.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Left | System.Windows.Forms.AnchorStyles.Right)));
            this.txtUrl.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(78)))), ((int)(((byte)(81)))), ((int)(((byte)(79)))));
            this.txtUrl.Controls.Add(this.picFavicon);
            this.txtUrl.Font = new System.Drawing.Font("微软雅黑", 10.5F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.txtUrl.ForeColor = System.Drawing.Color.White;
            this.txtUrl.Location = new System.Drawing.Point(139, 12);
            this.txtUrl.Margin = new System.Windows.Forms.Padding(4);
            this.txtUrl.Name = "txtUrl";
            this.txtUrl.Size = new System.Drawing.Size(1200, 35);
            this.txtUrl.TabIndex = 3;
            this.txtUrl.KeyDown += new System.Windows.Forms.KeyEventHandler(this.txtUrl_KeyDown);
            // 
            // picFavicon
            // 
            this.picFavicon.BackColor = System.Drawing.Color.Transparent;
            this.picFavicon.Location = new System.Drawing.Point(4, 4);
            this.picFavicon.Margin = new System.Windows.Forms.Padding(4);
            this.picFavicon.Name = "picFavicon";
            this.picFavicon.Size = new System.Drawing.Size(20, 20);
            this.picFavicon.SizeMode = System.Windows.Forms.PictureBoxSizeMode.Zoom;
            this.picFavicon.TabIndex = 6;
            this.picFavicon.TabStop = false;
            this.toolTip.SetToolTip(this.picFavicon, "网站图标");
            // 
            // btnGo
            // 
            this.btnGo.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)));
            this.btnGo.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(22)))), ((int)(((byte)(136)))), ((int)(((byte)(241)))));
            this.btnGo.Cursor = System.Windows.Forms.Cursors.Hand;
            this.btnGo.FlatAppearance.BorderSize = 0;
            this.btnGo.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.btnGo.Font = new System.Drawing.Font("微软雅黑", 9F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.btnGo.ForeColor = System.Drawing.Color.White;
            this.btnGo.Location = new System.Drawing.Point(1346, 3);
            this.btnGo.Name = "btnGo";
            this.btnGo.Size = new System.Drawing.Size(84, 54);
            this.btnGo.TabIndex = 4;
            this.btnGo.Text = "前往";
            this.btnGo.UseVisualStyleBackColor = false;
            this.btnGo.Click += new System.EventHandler(this.btnGo_Click);
            // 
            // btnExpand
            // 
            this.btnExpand.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)));
            this.btnExpand.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(220)))), ((int)(((byte)(53)))), ((int)(((byte)(69)))));
            this.btnExpand.Cursor = System.Windows.Forms.Cursors.Hand;
            this.btnExpand.FlatAppearance.BorderSize = 0;
            this.btnExpand.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.btnExpand.Font = new System.Drawing.Font("微软雅黑", 9F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.btnExpand.ForeColor = System.Drawing.Color.White;
            this.btnExpand.Location = new System.Drawing.Point(1438, 3);
            this.btnExpand.Name = "btnExpand";
            this.btnExpand.Size = new System.Drawing.Size(122, 54);
            this.btnExpand.TabIndex = 5;
            this.btnExpand.Text = "🔧工具箱";
            this.btnExpand.UseVisualStyleBackColor = false;
            this.btnExpand.Click += new System.EventHandler(this.btnExpand_Click);
            // 
            // tableLayoutPanelDown
            // 
            this.tableLayoutPanelDown.ColumnCount = 1;
            this.tableLayoutPanelDown.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 100F));
            this.tableLayoutPanelDown.Controls.Add(this.panelBrowser, 0, 0);
            this.tableLayoutPanelDown.Dock = System.Windows.Forms.DockStyle.Fill;
            this.tableLayoutPanelDown.Location = new System.Drawing.Point(0, 60);
            this.tableLayoutPanelDown.Margin = new System.Windows.Forms.Padding(0);
            this.tableLayoutPanelDown.Name = "tableLayoutPanelDown";
            this.tableLayoutPanelDown.RowCount = 1;
            this.tableLayoutPanelDown.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 100F));
            this.tableLayoutPanelDown.Size = new System.Drawing.Size(1563, 872);
            this.tableLayoutPanelDown.TabIndex = 1;
            // 
            // panelBrowser
            // 
            this.panelBrowser.BackColor = System.Drawing.SystemColors.Window;
            this.panelBrowser.Dock = System.Windows.Forms.DockStyle.Fill;
            this.panelBrowser.Location = new System.Drawing.Point(0, 0);
            this.panelBrowser.Margin = new System.Windows.Forms.Padding(0);
            this.panelBrowser.Name = "panelBrowser";
            this.panelBrowser.Size = new System.Drawing.Size(1563, 872);
            this.panelBrowser.TabIndex = 0;
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(9F, 18F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(37)))), ((int)(((byte)(175)))), ((int)(((byte)(254)))));
            this.ClientSize = new System.Drawing.Size(1563, 932);
            this.Controls.Add(this.tableLayoutPanelMain);
            this.Margin = new System.Windows.Forms.Padding(4);
            this.MinimumSize = new System.Drawing.Size(1196, 791);
            this.Name = "Form1";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
            this.Text = "Emoji多标签浏览器";
            this.FormClosed += new System.Windows.Forms.FormClosedEventHandler(this.Form1_FormClosed);
            this.Load += new System.EventHandler(this.Form1_Load);
            this.Shown += new System.EventHandler(this.Form1_Shown);
            this.ResizeEnd += new System.EventHandler(this.Form1_ResizeEnd);
            this.Resize += new System.EventHandler(this.Form1_Resize);
            this.tableLayoutPanelMain.ResumeLayout(false);
            this.tableLayoutPanelTop.ResumeLayout(false);
            this.tableLayoutPanelTop.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.picBack)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.picForward)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.picRefresh)).EndInit();
            this.txtUrl.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.picFavicon)).EndInit();
            this.tableLayoutPanelDown.ResumeLayout(false);
            this.ResumeLayout(false);
        }

        #endregion

        private System.Windows.Forms.TableLayoutPanel tableLayoutPanelMain;
        private System.Windows.Forms.TableLayoutPanel tableLayoutPanelTop;
        private System.Windows.Forms.TableLayoutPanel tableLayoutPanelDown;
        private System.Windows.Forms.PictureBox picBack;
        private System.Windows.Forms.PictureBox picForward;
        private System.Windows.Forms.PictureBox picRefresh;
        private System.Windows.Forms.PictureBox picFavicon;
        private System.Windows.Forms.TextBox txtUrl;
        private System.Windows.Forms.Button btnGo;
        private System.Windows.Forms.Button btnExpand;
        private System.Windows.Forms.Panel panelBrowser;
        private System.Windows.Forms.ToolTip toolTip;
    }
}
