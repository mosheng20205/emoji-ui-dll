# Emoji Window DLL（C++）

基于 **Direct2D / DirectWrite** 的 Windows 原生 UI 控件 DLL，支持彩色 **Emoji** 与 Unicode 文本、多套主题（亮/暗）、布局与表格等能力。

导出为 **`__stdcall` C 风格 API**：凡能在 Windows 上加载 DLL 并绑定原生函数的语言均可调用。常见环境包括 **C/C++**、**C#**（P/Invoke）、**VB.NET**、**Python**（ctypes）、**易语言**（`.DLL命令`）、**Delphi / Free Pascal**、**Rust**、**Go**、**Java**（JNA 等）、**Node.js**（ffi-napi 等）。

**当前仓库仅附带易语言、C#、Python 三种示例 Demo**；其他语言请对照 `docs` 与 `emoji_window.def` 自行声明。仓库另含源码与主题 JSON。

---

## 新增能力：通用对话框、文件拖拽、Excel/CSV读取

本 DLL 现在提供一组通用文件能力，适合在 易语言 / C# / Python 等宿主程序中直接调用：

- 通用对话框：`ShowOpenFileDialog`、`ShowSaveFileDialog`、`ShowSelectFolderDialog`、`ShowColorDialog`
- 本地文件拖拽：`EnableFileDrop`、`SetFileDropCallback`
- 表格文件读取：`Excel_OpenWorkbook`、`Excel_GetRowCount`、`Excel_GetColumnCount`、`Excel_GetCellText`
- 表格加载到 DataGridView：`Excel_LoadWorkbookToDataGrid`、`Excel_ReadFileToDataGrid`

路径、标题、筛选器等文本参数仍按项目惯例使用 UTF-8 字节指针 + 长度。打开文件对话框的筛选器格式示例：

```text
表格文件|*.xlsx;*.xls;*.csv;*.tsv;*.txt|所有文件|*.*
```

读取能力说明：

- `CSV / TSV / TXT` 由 DLL 直接解析，支持 UTF-8、UTF-8 BOM、UTF-16LE/BE 和系统 ANSI 回退。
- `XLS / XLSX` 通过本机 Excel COM 自动化读取，因此运行机器需要安装 Microsoft Excel。
- Excel 读取接口里的 `row`、`col` 均为 1 起始，方便易语言侧调用。
- `Excel_GetCellText` 和对话框返回值使用两段式缓冲区模式；缓冲区不足时返回负的所需字节数。

Python GUI 测试脚本：

```powershell
python examples\Python\demo_common_dialog_drag_excel.py
```

该脚本会创建窗口，演示打开/保存/选目录/选颜色对话框、拖拽本地表格文件、读取 CSV/XLS/XLSX 到 DataGridView，并提供一个自动生成的 CSV 样例用于快速测试。

发布脚本会同时构建 x86/x64：

```powershell
powershell -ExecutionPolicy Bypass -File .\build_release_dlls.ps1
```

输出位置：

- `DLL\emoji_window_x86.dll`：易语言默认建议使用的 32 位 DLL
- `DLL\emoji_window_x64.dll`：64 位宿主程序使用
- `emoji_window.dll`：发布脚本复制的 x64 根目录 DLL

---

## 目录说明

| 目录 | 说明 |
|------|------|
| [`designer/`](designer) | 可视化设计器的代码 |
| [`examples/`](examples) | 编程示例代码 |
| [`skills/`](skills) | 编程客户端的技能包 |

---

## 文档导航

详细说明见 [`docs/controls`](docs/controls) 目录（控件 API、行为说明与补丁记录）：

| 文档 | 说明 |
|------|------|
| [button.md](docs/controls/button.md) | 按钮 |
| [checkbox.md](docs/controls/checkbox.md) | 复选框 |
| [radiobutton.md](docs/controls/radiobutton.md) | 单选框 |
| [editbox.md](docs/controls/editbox.md) | 编辑框 |
| [combobox.md](docs/controls/combobox.md) | 组合框 |
| [listbox.md](docs/controls/listbox.md) | 列表框 |
| [picturebox.md](docs/controls/picturebox.md) | 图片框 |
| [progressbar.md](docs/controls/progressbar.md) | 进度条 |
| [datetimepicker.md](docs/controls/datetimepicker.md) | 日期时间选择 |
| [hotkey.md](docs/controls/hotkey.md) | 热键控件 |
| [groupbox.md](docs/controls/groupbox.md) | 分组框 |
| [tabcontrol.md](docs/controls/tabcontrol.md) | 标签页 |
| [treeview.md](docs/controls/treeview.md) | 树形控件 |
| [sidebar_menu_treeview.md](docs/controls/sidebar_menu_treeview.md) | 侧栏菜单树 |
| [datagridview.md](docs/controls/datagridview.md) | 表格（DataGridView） |
| [按钮回调说明.md](docs/controls/按钮回调说明.md) | 按钮回调说明 |
| [按钮启用禁用.md](docs/controls/按钮启用禁用.md) | 按钮启用/禁用 |
| [分组框使用对比.md](docs/controls/分组框使用对比.md) | 分组框使用对比 |
| [分组框快速参考.md](docs/controls/分组框快速参考.md) | 分组框快速参考 |
| [分组框改进说明.md](docs/controls/分组框改进说明.md) | 分组框改进说明 |
| [分组框按钮显示修复.md](docs/controls/分组框按钮显示修复.md) | 分组框按钮显示修复 |
| [tabcontrol首屏显示与切换闪烁修复.md](docs/controls/tabcontrol首屏显示与切换闪烁修复.md) | 标签页首屏/闪烁修复 |

---

## 界面截图

<details>
<summary>点击展开查看界面效果示例</summary>

4.0 起整体向 Element 风格与皮肤系统靠拢，以下为界面效果示例（图源：`WindowsEmojiDll/4.0/截图.md`）。

![9d48a9b1cde44b44b53704196a18e971](https://img.msblog.cc/img/9d48a9b1cde44b44b53704196a18e971.png)

![c6876ac0507dddd574b9b0cd1e1ae110](https://img.msblog.cc/img/c6876ac0507dddd574b9b0cd1e1ae110.png)

![167d5b5ef03e68e90c56ec5dd73f0c39](https://img.msblog.cc/img/167d5b5ef03e68e90c56ec5dd73f0c39.png)

![7cf0ca7c3015d07f0e00432d7225c24f](https://img.msblog.cc/img/7cf0ca7c3015d07f0e00432d7225c24f.png)

![5adba63aa0b1e73d6792ec6b10469eaa](https://img.msblog.cc/img/5adba63aa0b1e73d6792ec6b10469eaa.png)

![0cb9071e681f82f37716b14e61cef335](https://img.msblog.cc/img/0cb9071e681f82f37716b14e61cef335.png)

![2fc5373e9547348d67f1fa44d26966be](https://img.msblog.cc/img/2fc5373e9547348d67f1fa44d26966be.png)

![40cc75a4cbe644b5146dfb0002df2070](https://img.msblog.cc/img/40cc75a4cbe644b5146dfb0002df2070.png)

</details>

---

## 交流群

- **QQ 交流群：** 464558635
- 有 bug 和建议请提交 **Issues**，不要在群内反馈（消息太多，很混乱）。

## 打赏与联系

如果这个项目对你有帮助，欢迎随缘打赏，支持后续维护与示例补充。

- QQ：`1098901025`
- 微信：`zhx_ms`
- 添加请注明来意

<details>
<summary>如需打赏，可展开查看收款码</summary>

<p>
  <img src="https://img.msblog.cc/image-20250523012804344.png" alt="支付宝收款码" width="180">
  <img src="https://img.msblog.cc/image-20250523012814243.png" alt="微信收款码" width="180">
</p>

</details>

