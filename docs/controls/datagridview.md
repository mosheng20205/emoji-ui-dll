# DataGridView 表格控件

[← 返回主文档](../../README.md)

## 概述

高性能数据表格控件，支持多种列类型、虚拟模式、排序、冻结行列等功能。

## 创建表格

```c++
HWND __stdcall CreateDataGridView(
    HWND hParent,
    int x, int y, int width, int height,
    BOOL virtual_mode,      // 是否启用虚拟模式
    BOOL zebra_stripes,     // 是否启用隔行变色
    UINT32 bg_color
);
```

## 列管理

### 添加列

```c++
// 文本列
int __stdcall DataGrid_AddTextColumn(HWND hDataGrid, const unsigned char* header_bytes, int header_len, int width);

// 复选框列
int __stdcall DataGrid_AddCheckBoxColumn(HWND hDataGrid, const unsigned char* header_bytes, int header_len, int width);

// 按钮列
int __stdcall DataGrid_AddButtonColumn(HWND hDataGrid, const unsigned char* header_bytes, int header_len, int width);

// 链接列
int __stdcall DataGrid_AddLinkColumn(HWND hDataGrid, const unsigned char* header_bytes, int header_len, int width);

// 图片列
int __stdcall DataGrid_AddImageColumn(HWND hDataGrid, const unsigned char* header_bytes, int header_len, int width);
int __stdcall DataGrid_AddProgressColumn(HWND hDataGrid, const unsigned char* header_bytes, int header_len, int width);
```

### 列类型常量

| 常量 | 值 | 说明 |
|------|-----|------|
| `DGCOL_TEXT` | 0 | 文本列 |
| `DGCOL_CHECKBOX` | 1 | 复选框列 |
| `DGCOL_BUTTON` | 2 | 按钮列 |
| `DGCOL_LINK` | 3 | 链接列 |
| `DGCOL_IMAGE` | 4 | 图片列 |

### 其他列操作

```c++
void __stdcall DataGrid_RemoveColumn(HWND hDataGrid, int column_index);
void __stdcall DataGrid_ClearColumns(HWND hDataGrid);
int  __stdcall DataGrid_GetColumnCount(HWND hDataGrid);
void __stdcall DataGrid_SetColumnWidth(HWND hDataGrid, int column_index, int width);
```

## 行管理

```c++
int  __stdcall DataGrid_AddRow(HWND hDataGrid);
void __stdcall DataGrid_RemoveRow(HWND hDataGrid, int row_index);
void __stdcall DataGrid_ClearRows(HWND hDataGrid);
int  __stdcall DataGrid_GetRowCount(HWND hDataGrid);
```

## 单元格操作

```c++
// 文本
void __stdcall DataGrid_SetCellText(HWND hDataGrid, int row, int col, const unsigned char* text_bytes, int text_len);
int  __stdcall DataGrid_GetCellText(HWND hDataGrid, int row, int col, unsigned char* buffer, int buffer_size);

// 复选框
void __stdcall DataGrid_SetCellChecked(HWND hDataGrid, int row, int col, BOOL checked);
BOOL __stdcall DataGrid_GetCellChecked(HWND hDataGrid, int row, int col);
void __stdcall DataGrid_SetCellProgress(HWND hDataGrid, int row, int col, int progress);
int  __stdcall DataGrid_GetCellProgress(HWND hDataGrid, int row, int col);

// 样式
void __stdcall DataGrid_SetCellStyle(HWND hDataGrid, int row, int col, UINT32 fg_color, UINT32 bg_color);
```

## 选择和排序

```c++
int  __stdcall DataGrid_GetSelectedRow(HWND hDataGrid);
int  __stdcall DataGrid_GetSelectedCol(HWND hDataGrid);
void __stdcall DataGrid_SetSelectedCell(HWND hDataGrid, int row, int col);
void __stdcall DataGrid_SetSelectionMode(HWND hDataGrid, int mode);  // 0=单元格 1=整行
void __stdcall DataGrid_SortByColumn(HWND hDataGrid, int col, BOOL ascending);
```

## 冻结行列

```c++
void __stdcall DataGrid_SetFreezeHeader(HWND hDataGrid, BOOL freeze);
void __stdcall DataGrid_SetFreezeFirstColumn(HWND hDataGrid, BOOL freeze);  // 兼容接口
void __stdcall DataGrid_SetFreezeColumnCount(HWND hDataGrid, int count);
void __stdcall DataGrid_SetFreezeRowCount(HWND hDataGrid, int count);
```

- `DataGrid_SetFreezeHeader`：冻结列表头。启用后，纵向滚动时表头保持固定。
- `DataGrid_SetFreezeColumnCount`：冻结左侧前 `N` 列。横向滚动时，这些列保持固定。
- `DataGrid_SetFreezeRowCount`：冻结数据区顶部前 `N` 行。这里不包含列表头；如果需要表头固定，仍需同时调用 `DataGrid_SetFreezeHeader`。
- `DataGrid_SetFreezeFirstColumn`：旧接口，内部等价于 `DataGrid_SetFreezeColumnCount(hDataGrid, freeze ? 1 : 0)`，保留用于兼容旧代码。
- `count` 超出当前行数/列数时会自动截断；传 `0` 可取消对应冻结。

### 推荐用法

```c++
DataGrid_SetFreezeHeader(hGrid, TRUE);     // 固定列表头
DataGrid_SetFreezeColumnCount(hGrid, 2);   // 固定前 2 列
DataGrid_SetFreezeRowCount(hGrid, 2);      // 固定前 2 行数据
```

## 虚拟模式

虚拟模式适用于大数据量场景（10000+行），仅加载可见行数据：

```c++
void __stdcall DataGrid_SetVirtualRowCount(HWND hDataGrid, int count);

typedef void (__stdcall *VirtualDataNeededCallback)(HWND hDataGrid, int start_row, int row_count);
void __stdcall DataGrid_SetVirtualDataCallback(HWND hDataGrid, VirtualDataNeededCallback callback);
```

### 易语言虚拟模式示例

```
' 设置虚拟模式总行数
表格_设置虚拟行数 (表格句柄, 100000)

' 设置数据请求回调
表格_设置虚拟数据回调 (表格句柄, &虚拟数据回调)

.子程序 虚拟数据回调, , , stdcall
.参数 表格句柄, 整数型
.参数 起始行, 整数型
.参数 行数, 整数型

' 在此填充可见行的数据
.变量循环首 (起始行, 起始行 + 行数 - 1, 1, i)
    表格_设置单元格文本_辅助 (表格句柄, i, 0, "行" + 到文本(i))
.变量循环尾 ()
```

## 事件回调

```c++
typedef void (__stdcall *CellClickCallback)(HWND hDataGrid, int row, int col);
typedef void (__stdcall *CellDoubleClickCallback)(HWND hDataGrid, int row, int col);
typedef void (__stdcall *CellValueChangedCallback)(HWND hDataGrid, int row, int col);
typedef void (__stdcall *ColumnHeaderClickCallback)(HWND hDataGrid, int col);

void __stdcall DataGrid_SetCellClickCallback(HWND hDataGrid, CellClickCallback callback);
void __stdcall DataGrid_SetCellDoubleClickCallback(HWND hDataGrid, CellDoubleClickCallback callback);
void __stdcall DataGrid_SetCellValueChangedCallback(HWND hDataGrid, CellValueChangedCallback callback);
void __stdcall DataGrid_SetColumnHeaderClickCallback(HWND hDataGrid, ColumnHeaderClickCallback callback);
```

## 其他操作

```c++
void __stdcall DataGrid_SetShowGridLines(HWND hDataGrid, BOOL show);
void __stdcall DataGrid_SetDefaultRowHeight(HWND hDataGrid, int height);
void __stdcall DataGrid_SetHeaderHeight(HWND hDataGrid, int height);
void __stdcall DataGrid_Enable(HWND hDataGrid, BOOL enable);
void __stdcall DataGrid_Show(HWND hDataGrid, BOOL show);
void __stdcall DataGrid_SetBounds(HWND hDataGrid, int x, int y, int w, int h);
void __stdcall DataGrid_Refresh(HWND hDataGrid);

// CSV 导出
BOOL __stdcall DataGrid_ExportCSV(HWND hDataGrid, const unsigned char* file_path_bytes, int path_len);
```

## 表格特性

- ✅ 多种列类型：文本、复选框、按钮、链接、图片
- ✅ 虚拟模式：支持 100000+ 行数据，仅加载可见行
- ✅ 隔行变色：Zebra Stripes 提升可读性
- ✅ 冻结行列：支持冻结列头、前 N 列、前 N 行，方便浏览大表格
- ✅ 排序：点击列头排序，支持升序/降序
- ✅ 键盘导航：方向键、Tab、PageUp/Down、Home/End
- ✅ 单元格编辑：双击进入编辑模式
- ✅ CSV 导出：一键导出表格数据
- ✅ Element UI 风格：统一的视觉设计

## 相关文档

- [列表框](listbox.md)
- [性能优化](../performance.md)
