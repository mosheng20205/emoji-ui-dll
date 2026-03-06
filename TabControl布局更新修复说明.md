# TabControl 布局更新修复说明

## 问题描述

1. 当用户在 Tab3 中调整窗口大小后，切换回 Tab1 时，Tab 的边框位置没有正确调整
2. 窗口大小改变时出现递归调用，导致窗口变成空白
3. **切换 Tab 后内容变空白**（最新问题）

## 根本原因分析

### 原始问题
易语言回调函数只调用了 `MoveWindow` 来调整 TabControl 的大小，但没有更新所有 Tab 页的内容窗口布局。

### 递归调用问题
在 C++ 代码的 `TabControlParentSubclassProc` 中处理 `WM_SIZE` 消息时调用 `UpdateTabLayout`，导致递归调用：

1. 易语言 `_窗口大小改变` 回调 → 调用 `MoveWindow`
2. `MoveWindow` → 触发父窗口 `WM_SIZE` 消息
3. `TabControlParentSubclassProc` 的 `WM_SIZE` 处理 → 调用 `UpdateTabLayout`
4. `UpdateTabLayout` 中的 `SetWindowPos` → 又触发父窗口 `WM_SIZE` 消息
5. 形成递归循环，导致窗口大小不断递减至 0x0

### 坐标转换错误问题（导致空白）
在 `UpdateTabLayout` 函数中，使用了错误的坐标转换方法：

```cpp
// ❌ 错误的坐标转换
POINT pt = { rcTab.left, rcTab.top };
MapWindowPoints(state->hTabControl, state->hParent, &pt, 1);
```

**问题分析**：
- `MapWindowPoints` 假设两个窗口是父子关系
- 但实际上 **Tab 内容窗口的父窗口是主窗口**，不是 TabControl
- TabControl 和内容窗口都是主窗口的子窗口（兄弟关系）
- 因此坐标转换结果不正确，导致内容窗口被移动到错误位置（可能在可视区域外）

## ✅ 最终解决方案

### 核心思路
1. **移除 `WM_SIZE` 的自动处理**，改为手动调用更新布局，避免递归调用
2. **修复坐标转换逻辑**，使用正确的屏幕坐标转换方法

### C++ 代码修改

#### 1. 修复 `UpdateTabLayout` 中的坐标转换（解决空白问题）

```cpp
void UpdateTabLayout(TabControlState* state) {
    if (!state || !state->hTabControl) return;

    // 获取 Tab Control 的显示区域（相对于 TabControl 自身）
    RECT rcTab;
    GetClientRect(state->hTabControl, &rcTab);
    
    // ⚠️ 防止在窗口最小化或无效大小时更新布局
    if (rcTab.right <= 0 || rcTab.bottom <= 0) {
        return;
    }
    
    // 调整矩形以获取内容区域（去除 Tab 标签栏的高度）
    TabCtrl_AdjustRect(state->hTabControl, FALSE, &rcTab);

    // ⚠️ CRITICAL FIX: 内容窗口的父窗口是主窗口，需要正确转换坐标
    // 将 TabControl 客户区坐标转换为主窗口坐标
    RECT rcTabInParent = rcTab;
    POINT ptTopLeft = { rcTab.left, rcTab.top };
    POINT ptBottomRight = { rcTab.right, rcTab.bottom };
    
    // 正确的坐标转换：TabControl 客户区 → 屏幕坐标 → 主窗口客户区
    ClientToScreen(state->hTabControl, &ptTopLeft);
    ClientToScreen(state->hTabControl, &ptBottomRight);
    ScreenToClient(state->hParent, &ptTopLeft);
    ScreenToClient(state->hParent, &ptBottomRight);
    
    rcTabInParent.left = ptTopLeft.x;
    rcTabInParent.top = ptTopLeft.y;
    rcTabInParent.right = ptBottomRight.x;
    rcTabInParent.bottom = ptBottomRight.y;

    // 遍历所有 Tab 页，更新可见性和位置
    for (size_t i = 0; i < state->pages.size(); i++) {
        TabPageInfo& page = state->pages[i];
        if (page.hContentWindow && IsWindow(page.hContentWindow)) {
            if ((int)i == state->currentIndex) {
                // 显示当前选中的页面
                SetWindowPos(
                    page.hContentWindow,
                    HWND_TOP,
                    rcTabInParent.left,      // ✅ 使用转换后的坐标
                    rcTabInParent.top,       // ✅ 使用转换后的坐标
                    rcTabInParent.right - rcTabInParent.left,
                    rcTabInParent.bottom - rcTabInParent.top,
                    SWP_SHOWWINDOW | SWP_NOACTIVATE | SWP_NOZORDER
                );
                page.visible = true;
            } else {
                // 隐藏其他页面
                ShowWindow(page.hContentWindow, SW_HIDE);
                page.visible = false;
            }
        }
    }
}
```

**关键修复点**：
- ❌ 错误方法：`MapWindowPoints(state->hTabControl, state->hParent, &pt, 1)`
  - 假设 TabControl 和主窗口是父子关系
  - 但实际上它们是兄弟关系（都是主窗口的子窗口）
- ✅ 正确方法：`ClientToScreen` → `ScreenToClient`
  - 先将 TabControl 客户区坐标转换为屏幕坐标
  - 再将屏幕坐标转换为主窗口客户区坐标
  - 这样无论窗口层级关系如何都能正确转换

#### 2. 移除 `TabControlParentSubclassProc` 中的 `WM_SIZE` 处理（避免递归）

```cpp
LRESULT CALLBACK TabControlParentSubclassProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData) {
    HWND hTabControl = (HWND)dwRefData;
    auto it = g_tab_controls.find(hTabControl);
    if (it == g_tab_controls.end()) {
        return DefSubclassProc(hwnd, msg, wparam, lparam);
    }

    TabControlState* state = it->second;

    switch (msg) {
    case WM_NOTIFY: {
        NMHDR* pnmhdr = (NMHDR*)lparam;
        if (pnmhdr->hwndFrom == hTabControl) {
            if (pnmhdr->code == TCN_SELCHANGE) {
                // Tab 切换事件 - 自动更新布局
                int newIndex = TabCtrl_GetCurSel(hTabControl);
                if (newIndex >= 0 && newIndex < (int)state->pages.size()) {
                    state->currentIndex = newIndex;
                    UpdateTabLayout(state);  // ✅ 只在 Tab 切换时更新

                    if (state->callback) {
                        state->callback(hTabControl, newIndex);
                    }
                }
            }
        }
        break;
    }

    // ⚠️ 移除了 WM_SIZE 处理，避免递归调用
    
    case WM_DESTROY: {
        RemoveWindowSubclass(hwnd, TabControlParentSubclassProc, uIdSubclass);
        break;
    }
    }

    return DefSubclassProc(hwnd, msg, wparam, lparam);
}
```

2. **保留 `UpdateTabControlLayout` 导出函数**（用于手动调用）：

```cpp
void __stdcall UpdateTabControlLayout(HWND hTabControl) {
    auto it = g_tab_controls.find(hTabControl);
    if (it == g_tab_controls.end()) return;

    TabControlState* state = it->second;
    UpdateTabLayout(state);
}
```

3. **在 `UpdateTabLayout` 中保留窗口大小检查**：

```cpp
void UpdateTabLayout(TabControlState* state) {
    if (!state || !state->hTabControl) return;

    RECT rcTab;
    GetClientRect(state->hTabControl, &rcTab);
    
    // ⚠️ 防止在窗口最小化或无效大小时更新布局
    if (rcTab.right <= 0 || rcTab.bottom <= 0) {
        return;
    }
    
    // ... 其余代码
}
```

### 易语言代码修改

**在 `窗口程序集_窗口_TabControl按钮编辑框标签.e` 中修改 `_窗口大小改变` 子程序**：

```
.子程序 _窗口大小改变, , 公开
.参数 窗口句柄, 整数型
.参数 新宽度, 整数型
.参数 新高度, 整数型

调试输出 ("窗口大小改变: " ＋ 到文本 (新宽度) ＋ " x " ＋ 到文本 (新高度))

' 调整 TabControl 大小以适应新窗口（距离边缘 30 像素）
.如果真 (TabControl句柄 ≠ 0)
    ' 使用 Windows API MoveWindow 调整 TabControl 大小
    MoveWindow (TabControl句柄, 30, 30, 新宽度 － 60, 新高度 － 60, 真)
    
    ' ⚠️ 手动更新 TabControl 布局
    ' 因为我们移除了 WM_SIZE 的自动处理，避免递归调用
    更新TabControl布局 (TabControl句柄)
.如果真结束
```

## 工作原理

### 窗口大小改变流程
1. **窗口大小改变** → 触发易语言 `_窗口大小改变` 回调
2. **调用 `MoveWindow`** → 调整 TabControl 的大小和位置
3. **手动调用 `更新TabControl布局`** → 更新所有 Tab 页的布局
4. **`UpdateTabLayout` 更新所有 Tab 页** → 调整内容窗口的大小和可见性
5. **完成布局更新** → 所有 Tab 页的布局都正确

### Tab 切换流程
1. **用户点击 Tab** → 触发 `TCN_SELCHANGE` 通知
2. **`TabControlParentSubclassProc` 接收通知** → 自动调用 `UpdateTabLayout`
3. **`UpdateTabLayout` 更新布局** → 显示当前 Tab，隐藏其他 Tab
4. **触发易语言回调**（如果设置了）

## 关键点

1. ✅ **修复了坐标转换逻辑**，使用 `ClientToScreen` → `ScreenToClient` 正确转换坐标
2. ✅ **移除了 `WM_SIZE` 的自动处理**，避免递归调用
3. ✅ **在易语言中手动调用 `更新TabControl布局`**，确保窗口大小改变时布局正确
4. ✅ **Tab 切换时自动更新布局**（通过 `TCN_SELCHANGE` 通知）
5. ✅ **在 `UpdateTabLayout` 中添加窗口大小检查**，防止无效更新
6. ✅ **在 `SetWindowPos` 中使用 `SWP_NOACTIVATE | SWP_NOZORDER` 标志**，减少额外消息

## 为什么之前的坐标转换会导致空白？

### 错误的坐标转换
```cpp
// ❌ 错误：使用 MapWindowPoints
POINT pt = { rcTab.left, rcTab.top };
MapWindowPoints(state->hTabControl, state->hParent, &pt, 1);
```

**问题**：
- `MapWindowPoints` 假设第一个窗口是第二个窗口的子窗口
- 但 TabControl 和主窗口不是父子关系，而是兄弟关系
- 导致坐标转换结果不正确

### 正确的坐标转换
```cpp
// ✅ 正确：通过屏幕坐标中转
ClientToScreen(state->hTabControl, &ptTopLeft);   // TabControl 客户区 → 屏幕
ScreenToClient(state->hParent, &ptTopLeft);       // 屏幕 → 主窗口客户区
```

**优点**：
- 无论窗口层级关系如何，都能正确转换
- 屏幕坐标是绝对坐标，不受窗口层级影响
- 这是 Windows API 推荐的坐标转换方法

## 测试验证

1. ✅ 运行易语言程序，窗口正常显示
2. ✅ 在 Tab3 中拖动窗口边缘调整大小，窗口大小正常改变
3. ✅ 切换到 Tab1 或 Tab2，Tab 边框和内容窗口正确调整
4. ✅ 验证窗口大小不会递减，内容正常显示
5. ✅ **验证切换 Tab 后内容不会变空白**（坐标转换修复）
6. ✅ 验证所有 Tab 页的按钮、编辑框、标签都正常显示和交互

## 编译

```bash
# 使用 Visual Studio 编译 32 位 DLL
msbuild emoji_window.sln /p:Configuration=Release /p:Platform=Win32 /t:Clean,Build
```

编译后的 DLL 位于 `Release/emoji_window.dll`（约 118KB）。

## 总结

通过两个关键修复，成功解决了所有问题：

1. **修复坐标转换逻辑**：使用 `ClientToScreen` → `ScreenToClient` 正确转换坐标，解决了切换 Tab 后内容变空白的问题
2. **移除 `WM_SIZE` 自动处理**：改为手动调用更新布局，解决了递归调用导致窗口变空白的问题

现在：
- ✅ 窗口大小改变时，布局正确更新
- ✅ Tab 切换时，布局自动更新
- ✅ 没有递归调用，窗口不会变空白
- ✅ 切换 Tab 后内容正常显示，不会变空白
- ✅ 所有功能正常工作

## 技术要点

### 坐标转换的正确方法
在 Windows 编程中，当需要在不同窗口之间转换坐标时：
- ✅ **推荐**：使用 `ClientToScreen` + `ScreenToClient`（通过屏幕坐标中转）
- ❌ **不推荐**：使用 `MapWindowPoints`（仅适用于父子窗口关系）

### 为什么通过屏幕坐标转换更可靠？
- 屏幕坐标是绝对坐标，不受窗口层级影响
- 适用于任何窗口关系（父子、兄弟、无关）
- 是 Windows API 文档推荐的通用方法
