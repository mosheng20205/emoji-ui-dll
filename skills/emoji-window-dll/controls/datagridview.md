# 表格控件 (DataGridView)

## 概述

DataGridView 是最复杂的控件，用于显示和编辑表格数据。支持多种列类型（文本、复选框、按钮、链接、图片）、虚拟模式（大数据量）、排序、冻结行列、隔行变色、CSV 导出等功能。

## C++ 导出函数列表

### 创建

| 函数名 | 参数 | 返回值 |
|--------|------|--------|
| `CreateDataGridView` | `HWND parent, int x, int y, int w, int h, BOOL virtualMode, BOOL alternateRowColor, UINT32 fgColor, UINT32 bgColor` | `int` 表格句柄 |

### 列管理

| 函数名 | 参数 | 返回值 |
|--------|------|--------|
| `DataGrid_AddTextColumn` | `int hGrid, const uint8_t* header_bytes, int header_len, int width` | `int` 列索引 |
| `DataGrid_AddCheckBoxColumn` | `int hGrid, const uint8_t* header_bytes, int header_len, int width` | `int` 列索引 |
| `DataGrid_AddButtonColumn` | `int hGrid, const uint8_t* header_bytes, int header_len, int width` | `int` 列索引 |
| `DataGrid_AddLinkColumn` | `int hGrid, const uint8_t* header_bytes, int header_len, int width` | `int` 列索引 |
| `DataGrid_AddImageColumn` | `int hGrid, const uint8_t* header_bytes, int header_len, int width` | `int` 列索引 |
| `DataGrid_RemoveColumn` | `int hGrid, int colIndex` | `void` |
| `DataGrid_ClearColumns` | `int hGrid` | `void` |
| `DataGrid_GetColumnCount` | `int hGrid` | `int` 列数 |
| `DataGrid_SetColumnWidth` | `int hGrid, int colIndex, int width` | `void` |

### 行管理

| 函数名 | 参数 | 返回值 |
|--------|------|--------|
| `DataGrid_AddRow` | `int hGrid` | `int` 行索引 |
| `DataGrid_RemoveRow` | `int hGrid, int rowIndex` | `void` |
| `DataGrid_ClearRows` | `int hGrid` | `void` |
| `DataGrid_GetRowCount` | `int hGrid` | `int` 行数 |

### 单元格操作

| 函数名 | 参数 | 返回值 |
|--------|------|--------|
| `DataGrid_SetCellText` | `int hGrid, int row, int col, const uint8_t* text_bytes, int text_len` | `void` |
| `DataGrid_GetCellText` | `int hGrid, int row, int col, uint8_t* buffer, int bufSize` | `int` 实际字节数 |
| `DataGrid_SetCellChecked` | `int hGrid, int row, int col, BOOL checked` | `void` |
| `DataGrid_GetCellChecked` | `int hGrid, int row, int col` | `BOOL` |
| `DataGrid_SetCellStyle` | `int hGrid, int row, int col, UINT32 fgColor, UINT32 bgColor, BOOL bold, BOOL italic` | `void` |

### 选择和导航

| 函数名 | 参数 | 返回值 |
|--------|------|--------|
| `DataGrid_GetSelectedRow` | `int hGrid` | `int` 选中行索引，-1=无 |
| `DataGrid_GetSelectedCol` | `int hGrid` | `int` 选中列索引，-1=无 |
| `DataGrid_SetSelectedCell` | `int hGrid, int row, int col` | `void` |
| `DataGrid_SetSelectionMode` | `int hGrid, int mode` | `void` |

选择模式：
| 值 | 说明 |
|----|------|
| 0 | 单元格选择 |
| 1 | 整行选择 |

### 排序

| 函数名 | 参数 | 返回值 |
|--------|------|--------|
| `DataGrid_SortByColumn` | `int hGrid, int colIndex, int direction` | `void` |

排序方向：
| 值 | 说明 |
|----|------|
| 0 | 无排序 |
| 1 | 升序 |
| 2 | 降序 |

### 冻结

| 函数名 | 参数 | 返回值 |
|--------|------|--------|
| `DataGrid_SetFreezeHeader` | `int hGrid, BOOL freeze` | `void` |
| `DataGrid_SetFreezeFirstColumn` | `int hGrid, BOOL freeze` | `void` |

### 虚拟模式

| 函数名 | 参数 | 返回值 |
|--------|------|--------|
| `DataGrid_SetVirtualRowCount` | `int hGrid, int rowCount` | `void` |
| `DataGrid_SetVirtualDataCallback` | `int hGrid, void* callback` | `void` |

### 外观

| 函数名 | 参数 | 返回值 |
|--------|------|--------|
| `DataGrid_SetShowGridLines` | `int hGrid, BOOL show` | `void` |
| `DataGrid_SetDefaultRowHeight` | `int hGrid, int height` | `void` |
| `DataGrid_SetHeaderHeight` | `int hGrid, int height` | `void` |

### 对齐

| 函数名 | 参数 | 返回值 |
|--------|------|--------|
| `DataGrid_SetColumnHeaderAlignment` | `int hGrid, int colIndex, int align` | `void` |
| `DataGrid_SetColumnCellAlignment` | `int hGrid, int colIndex, int align` | `void` |

对齐方式：0=左对齐, 1=居中, 2=右对齐

### 事件回调

| 函数名 | 参数 | 返回值 |
|--------|------|--------|
| `DataGrid_SetCellClickCallback` | `int hGrid, void* callback` | `void` |
| `DataGrid_SetCellDoubleClickCallback` | `int hGrid, void* callback` | `void` |
| `DataGrid_SetSelectionChangedCallback` | `int hGrid, void* callback` | `void` |
| `DataGrid_SetColumnHeaderClickCallback` | `int hGrid, void* callback` | `void` |
| `DataGrid_SetCellValueChangedCallback` | `int hGrid, void* callback` | `void` |

### 通用操作

| 函数名 | 参数 | 返回值 |
|--------|------|--------|
| `DataGrid_Enable` | `int hGrid, BOOL enable` | `void` |
| `DataGrid_SetDoubleClickEnabled` | `int hGrid, BOOL enabled` | `void` |
| `DataGrid_Show` | `int hGrid, BOOL show` | `void` |
| `DataGrid_SetBounds` | `int hGrid, int x, int y, int w, int h` | `void` |
| `DataGrid_Refresh` | `int hGrid` | `void` |
| `DataGrid_ExportCSV` | `int hGrid, const uint8_t* path_bytes, int path_len` | `BOOL` |

## 回调签名

### 单元格事件回调（CellClick / CellDoubleClick / SelectionChanged / CellValueChanged）

```c++
void __stdcall CellCallback(int hGrid, int row, int col);
```

### 列头点击回调

```c++
void __stdcall ColumnHeaderClickCallback(int hGrid, int col);
```

### 虚拟模式数据回调

```c++
int __stdcall VirtualDataCallback(int hGrid, int row, int col, uint8_t* buffer, int buffer_size);
```

返回写入 buffer 的字节数。表格需要显示某个单元格时会调用此回调请求数据。

## 注意事项

- 虚拟模式适用于大数据量场景（如上万行），不预先存储所有数据，按需通过回调获取
- 虚拟模式下必须调用 `DataGrid_SetVirtualRowCount` 设置总行数，并设置 `DataGrid_SetVirtualDataCallback` 提供数据
- 非虚拟模式下使用 `DataGrid_AddRow` + `DataGrid_SetCellText` 填充数据
- `DataGrid_SetCellStyle` 中颜色传 0 表示使用默认值
- `DataGrid_ExportCSV` 的文件路径为 UTF-8 编码
- 列头和单元格文本均为 UTF-8 编码
- 修改大量数据后调用 `DataGrid_Refresh` 刷新显示
- 冻结首行（列头）后，滚动时列头始终可见
- 冻结首列后，水平滚动时第一列始终可见
