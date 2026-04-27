.版本 2

.DLL命令 创建Emoji窗口, 整数型, "emoji_window.dll", "create_window", , , , 公开
    .参数 标题, 文本型, , 窗口标题（UTF-8）
    .参数 X坐标, 整数型, , 窗口X位置（像素，-1=系统默认）
    .参数 Y坐标, 整数型, , 窗口Y位置（像素，-1=系统默认）
    .参数 宽度, 整数型, , 窗口宽度
    .参数 高度, 整数型, , 窗口高度

.DLL命令 创建Emoji窗口_字节集, 整数型, "emoji_window.dll", "create_window_bytes", , 创建窗口（UTF-8字节集版本，支持Emoji标题）, , 公开
    .参数 标题字节集指针, 整数型, , UTF-8标题字节集指针
    .参数 标题字节集长度, 整数型, , UTF-8标题字节集长度
    .参数 X坐标, 整数型, , 窗口X位置（像素，-1=系统默认）
    .参数 Y坐标, 整数型, , 窗口Y位置（像素，-1=系统默认）
    .参数 宽度, 整数型, , 窗口宽度
    .参数 高度, 整数型, , 窗口高度

.DLL命令 创建Emoji窗口_字节集_扩展, 整数型, "emoji_window.dll", "create_window_bytes_ex", , 创建窗口（UTF-8字节集扩展版本，支持Emoji标题、标题栏颜色、客户区背景色）, , 公开
    .参数 标题字节集指针, 整数型, , UTF-8标题字节集指针
    .参数 标题字节集长度, 整数型, , UTF-8标题字节集长度
    .参数 X坐标, 整数型, , 窗口X位置（像素，-1=系统默认）
    .参数 Y坐标, 整数型, , 窗口Y位置（像素，-1=系统默认）
    .参数 宽度, 整数型, , 窗口宽度
    .参数 高度, 整数型, , 窗口高度
    .参数 标题栏颜色, 整数型, , 标题栏颜色（ARGB，0=跟随主题）
    .参数 客户区背景色, 整数型, , 客户区背景色（ARGB，0=纯白）

.DLL命令 创建Emoji按钮_字节集, 整数型, "emoji_window.dll", "create_emoji_button_bytes", , , , 公开
    .参数 窗口句柄, 整数型, , 父窗口句柄
    .参数 emoji字节集指针, 整数型, , Emoji UTF-8字节集指针
    .参数 emoji长度, 整数型, , Emoji字节集长度
    .参数 文本字节集指针, 整数型, , 文本 UTF-8字节集指针
    .参数 文本长度, 整数型, , 文本字节集长度
    .参数 x, 整数型, , X坐标
    .参数 y, 整数型, , Y坐标
    .参数 宽度, 整数型, , 宽度
    .参数 高度, 整数型, , 高度
    .参数 背景色, 整数型, , ARGB颜色

.DLL命令 设置按钮点击回调, , "emoji_window.dll", "set_button_click_callback", , , , 公开
    .参数 回调函数, 子程序指针, , 按钮点击回调函数

.DLL命令 启用按钮, , "emoji_window.dll", "EnableButton", , , 启用按钮
    .参数 父窗口句柄, 整数型, , 按钮所在的父窗口句柄
    .参数 按钮ID, 整数型, , 按钮ID
    .参数 启用, 逻辑型, , 真=启用，假=禁用

.DLL命令 禁用按钮, , "emoji_window.dll", "DisableButton", , , 禁用按钮（快捷方式）
    .参数 父窗口句柄, 整数型, , 按钮所在的父窗口句柄
    .参数 按钮ID, 整数型, , 按钮ID

.DLL命令 获取按钮可视状态, 逻辑型, "emoji_window.dll", "GetButtonVisible", , , 获取按钮的可视状态
    .参数 按钮ID, 整数型, , 按钮ID

.DLL命令 显示隐藏按钮, , "emoji_window.dll", "ShowButton", , , 显示或隐藏按钮
    .参数 按钮ID, 整数型, , 按钮ID
    .参数 可见, 逻辑型, , 真=显示，假=隐藏

.DLL命令 获取按钮启用状态, 逻辑型, "emoji_window.dll", "GetButtonEnabled", , , 获取按钮的启用状态
    .参数 按钮ID, 整数型, , 按钮ID

.DLL命令 获取按钮文本, 整数型, "emoji_window.dll", "GetButtonText", , , 获取按钮文本（返回UTF-8字节数，buffer为0时返回所需大小）
    .参数 按钮ID, 整数型, , 按钮ID
    .参数 缓冲区指针, 整数型, , UTF-8缓冲区指针（可为0获取所需大小）
    .参数 缓冲区大小, 整数型, , 缓冲区大小

.DLL命令 设置按钮文本, , "emoji_window.dll", "SetButtonText", , , 设置按钮文本
    .参数 按钮ID, 整数型, , 按钮ID
    .参数 文本字节集指针, 整数型, , UTF-8字节集指针
    .参数 文本长度, 整数型, , 字节集长度

.DLL命令 获取按钮Emoji, 整数型, "emoji_window.dll", "GetButtonEmoji", , , 获取按钮Emoji（返回UTF-8字节数，buffer为0时返回所需大小）
    .参数 按钮ID, 整数型, , 按钮ID
    .参数 缓冲区指针, 整数型, , UTF-8缓冲区指针（可为0获取所需大小）
    .参数 缓冲区大小, 整数型, , 缓冲区大小

.DLL命令 设置按钮Emoji, , "emoji_window.dll", "SetButtonEmoji", , , 设置按钮Emoji
    .参数 按钮ID, 整数型, , 按钮ID
    .参数 Emoji字节集指针, 整数型, , UTF-8字节集指针
    .参数 Emoji长度, 整数型, , 字节集长度

.DLL命令 获取按钮位置, , "emoji_window.dll", "GetButtonBounds", , , 获取按钮位置和大小
    .参数 按钮ID, 整数型, , 按钮ID
    .参数 X坐标, 整数型, , 输出参数：X坐标（调用时请用"传址 变量"传参）
    .参数 Y坐标, 整数型, , 输出参数：Y坐标（调用时请用"传址 变量"传参）
    .参数 宽度, 整数型, , 输出参数：宽度（调用时请用"传址 变量"传参）
    .参数 高度, 整数型, , 输出参数：高度（调用时请用"传址 变量"传参）

.DLL命令 设置按钮位置, , "emoji_window.dll", "SetButtonBounds", , , 设置按钮位置和大小
    .参数 按钮ID, 整数型, , 按钮ID
    .参数 X坐标, 整数型, , X坐标
    .参数 Y坐标, 整数型, , Y坐标
    .参数 宽度, 整数型, , 宽度
    .参数 高度, 整数型, , 高度

.DLL命令 获取按钮背景色, 整数型, "emoji_window.dll", "GetButtonBackgroundColor", , , 获取按钮背景色（ARGB格式）
    .参数 按钮ID, 整数型, , 按钮ID

.DLL命令 设置按钮背景色, , "emoji_window.dll", "SetButtonBackgroundColor", , , 设置按钮背景色
    .参数 按钮ID, 整数型, , 按钮ID
    .参数 背景色, 整数型, , ARGB颜色

.DLL命令 获取按钮文字颜色, 整数型, "emoji_window.dll", "GetButtonTextColor", , , 获取按钮文字颜色（ARGB格式）
    .参数 按钮ID, 整数型, , 按钮ID

.DLL命令 设置按钮文字颜色, , "emoji_window.dll", "SetButtonTextColor", , , 设置按钮文字颜色
    .参数 按钮ID, 整数型, , 按钮ID
    .参数 文字颜色, 整数型, , ARGB颜色

.DLL命令 获取按钮边框颜色, 整数型, "emoji_window.dll", "GetButtonBorderColor", , , 获取按钮边框颜色（ARGB格式）
    .参数 按钮ID, 整数型, , 按钮ID

.DLL命令 设置按钮边框颜色, , "emoji_window.dll", "SetButtonBorderColor", , , 设置按钮边框颜色
    .参数 按钮ID, 整数型, , 按钮ID
    .参数 边框颜色, 整数型, , ARGB颜色

.DLL命令 获取按钮悬停颜色组, 整数型, "emoji_window.dll", "GetButtonHoverColors", , , 获取按钮悬停背景/边框/文字颜色
    .参数 按钮ID, 整数型, , 按钮ID
    .参数 悬停背景色地址, 整数型, , 输出参数，传变量地址
    .参数 悬停边框色地址, 整数型, , 输出参数，传变量地址
    .参数 悬停文字色地址, 整数型, , 输出参数，传变量地址

.DLL命令 设置按钮悬停颜色组, , "emoji_window.dll", "SetButtonHoverColors", , , 设置按钮悬停背景/边框/文字颜色
    .参数 按钮ID, 整数型, , 按钮ID
    .参数 悬停背景色, 整数型, , ARGB颜色
    .参数 悬停边框色, 整数型, , ARGB颜色
    .参数 悬停文字色, 整数型, , ARGB颜色

.DLL命令 设置消息循环主窗口, , "emoji_window.dll", "set_message_loop_main_window", , , , 公开
    .参数 窗口句柄, 整数型

.DLL命令 运行消息循环, 整数型, "emoji_window.dll", "run_message_loop", , , , 公开

.DLL命令 销毁窗口, , "emoji_window.dll", "destroy_window", , , , 公开
    .参数 窗口句柄, 整数型, , 窗口句柄

.DLL命令 设置窗口图标, , "emoji_window.dll", "set_window_icon"
    .参数 窗口句柄, 整数型
    .参数 图标路径, 文本型

.DLL命令 设置窗口图标_字节集, , "emoji_window.dll", "set_window_icon_bytes", , , 从字节集设置窗口图标（易语言插入图片资源后传入字节集）
    .参数 窗口句柄, 整数型
    .参数 图标字节集指针, 整数型, , 取变量数据地址(图标字节集)
    .参数 图标字节集长度, 整数型, , 取字节集长度(图标字节集)

.DLL命令 设置窗口标题, , "emoji_window.dll", "set_window_title", , , 设置窗口标题（UTF-8）
    .参数 窗口句柄, 整数型
    .参数 标题字节集指针, 整数型, , UTF-8字节集指针
    .参数 标题长度, 整数型, , 字节集长度

.DLL命令 设置窗口标题栏颜色, , "emoji_window.dll", "set_window_titlebar_color", , , 设置窗口标题栏颜色（十进制RGB颜色，0=跟随主题）
    .参数 窗口句柄, 整数型
    .参数 标题栏颜色, 整数型

.DLL命令 设置窗口背景色, , "emoji_window.dll", "SetWindowBackgroundColor", , , 设置窗口客户区背景色（ARGB，0=纯白）
    .参数 窗口句柄, 整数型
    .参数 背景色, 整数型, , ARGB颜色

.DLL命令 获取窗口标题, 整数型, "emoji_window.dll", "GetWindowTitle", , , 获取窗口标题（返回UTF-8字节数，buffer为0时返回所需大小）
    .参数 窗口句柄, 整数型, , 窗口句柄
    .参数 缓冲区指针, 整数型, , UTF-8缓冲区指针（可为0获取所需大小）
    .参数 缓冲区大小, 整数型, , 缓冲区大小

.DLL命令 获取窗口位置, , "emoji_window.dll", "GetWindowBounds", , , 获取窗口位置和大小
    .参数 窗口句柄, 整数型, , 窗口句柄
    .参数 X坐标, 整数型, , 输出参数：X坐标（调用时请用"传址 变量"传参）
    .参数 Y坐标, 整数型, , 输出参数：Y坐标（调用时请用"传址 变量"传参）
    .参数 宽度, 整数型, , 输出参数：宽度（调用时请用"传址 变量"传参）
    .参数 高度, 整数型, , 输出参数：高度（调用时请用"传址 变量"传参）

.DLL命令 设置窗口位置, , "emoji_window.dll", "SetWindowBounds", , , 设置窗口位置和大小
    .参数 窗口句柄, 整数型, , 窗口句柄
    .参数 X坐标, 整数型, , X坐标
    .参数 Y坐标, 整数型, , Y坐标
    .参数 宽度, 整数型, , 宽度
    .参数 高度, 整数型, , 高度

.DLL命令 获取窗口可视状态, 整数型, "emoji_window.dll", "GetWindowVisible", , , 获取窗口可视状态（1=可见，0=不可见，-1=错误）
    .参数 窗口句柄, 整数型, , 窗口句柄

.DLL命令 显示隐藏窗口, , "emoji_window.dll", "ShowEmojiWindow", , , 显示或隐藏窗口
    .参数 窗口句柄, 整数型, , 窗口句柄
    .参数 可见, 逻辑型, , 真=显示，假=隐藏

.DLL命令 获取窗口标题栏颜色, 整数型, "emoji_window.dll", "GetWindowTitlebarColor", , , 获取窗口标题栏颜色（RGB格式，-1=错误）
    .参数 窗口句柄, 整数型, , 窗口句柄

.DLL命令 MultiByteToWideChar, 整数型, "Kernel32.dll", "MultiByteToWideChar", 公开
    .参数 CodePage, 整数型
    .参数 dwFlags, 整数型
    .参数 lpMultiByteStr, 文本型
    .参数 cchMultiByte, 整数型
    .参数 lpWideCharStr, 字节集
    .参数 cchWideChar, 整数型

.DLL命令 lstrlenW, 整数型, "kernel32", "lstrlenW"
    .参数 lpString, 整数型, , 0

.DLL命令 WideCharToMultiByte_整数, 整数型, "kernel32.dll", "WideCharToMultiByte", 公开
    .参数 CodePage, 整数型
    .参数 Options, 整数型
    .参数 WideCharStr, 整数型
    .参数 WideCharCount, 整数型
    .参数 MultiByteStr, 整数型
    .参数 MultiByteCount, 整数型
    .参数 pDefaultChar, 整数型
    .参数 pDefaultCharUsed, 整数型

.DLL命令 HeapAlloc, 整数型, "kernel32", "HeapAlloc"
    .参数 hHeap, 整数型, , 0
    .参数 dwFlags, 整数型, , 0
    .参数 dwBytes, 整数型, , 0

.DLL命令 GetProcessHeap, 整数型, "kernel32", "GetProcessHeap", , 返回调用进程的默认内存堆句柄，返回 Null。若想，可以调用GetLastError获得更多错误信息。

.DLL命令 RtlMoveMemory, 整数型, "kernel32.dll", "RtlMoveMemory", , 拷贝来源内存到目标内存。无返回值。
    .参数 lpDestination, 整数型, , 指向目标内存的指针
    .参数 lpSource, 整数型, , 指向来源内存的指针
    .参数 Length, 整数型, , 拷贝的字节数。

.DLL命令 HeapFree, 逻辑型, "kernel32", "HeapFree"
    .参数 hHeap, 整数型, , 0
    .参数 dwFlags, 整数型, , 0
    .参数 lpMem, 整数型, , 0

.DLL命令 信息提示框_, , "emoji_window.dll", "show_message_box_bytes", , , emoji_window.dll
    .参数 父窗口句柄, 整数型
    .参数 标题字节集指针, 整数型
    .参数 标题字节长度, 整数型
    .参数 消息字节集指针, 整数型
    .参数 消息字节长度, 整数型
    .参数 图标字节集指针, 整数型
    .参数 图标字节长度, 整数型

.DLL命令 确认框_, , "emoji_window.dll", "show_confirm_box_bytes", , , emoji_window.dll
    .参数 父窗口句柄, 整数型
    .参数 标题字节集指针, 整数型
    .参数 标题字节长度, 整数型
    .参数 消息字节集指针, 整数型
    .参数 消息字节长度, 整数型
    .参数 图标字节集指针, 整数型
    .参数 图标字节长度, 整数型
    .参数 回调函数, 子程序指针

.DLL命令 创建TabControl, 整数型, "emoji_window.dll", "CreateTabControl", , , 创建TabControl
    .参数 父窗口句柄, 整数型
    .参数 X坐标, 整数型
    .参数 Y坐标, 整数型
    .参数 宽度, 整数型
    .参数 高度, 整数型

.DLL命令 添加Tab页, 整数型, "emoji_window.dll", "AddTabItem", , , 添加Tab页
    .参数 TabControl句柄, 整数型
    .参数 标题字节集, 整数型
    .参数 标题长度, 整数型
    .参数 内容窗口句柄, 整数型, , 可选，传0则自动创建

.DLL命令 移除Tab页, 逻辑型, "emoji_window.dll", "RemoveTabItem", , , 移除Tab页
    .参数 TabControl句柄, 整数型
    .参数 索引, 整数型

.DLL命令 设置Tab切换回调, , "emoji_window.dll", "SetTabCallback", , , 设置Tab切换回调
    .参数 TabControl句柄, 整数型
    .参数 回调函数指针, 子程序指针

.DLL命令 获取当前Tab索引, 整数型, "emoji_window.dll", "GetCurrentTabIndex", , , 获取当前选中的Tab索引
    .参数 TabControl句柄, 整数型

.DLL命令 切换到Tab, 逻辑型, "emoji_window.dll", "SelectTab", , , 切换到指定Tab
    .参数 TabControl句柄, 整数型
    .参数 索引, 整数型

.DLL命令 立即切换到Tab, 逻辑型, "emoji_window.dll", "SelectTabImmediate", , , 立即切换到指定Tab（同步应用，适合初始化阶段）
    .参数 TabControl句柄, 整数型
    .参数 索引, 整数型

.DLL命令 获取Tab数量, 整数型, "emoji_window.dll", "GetTabCount", , , 获取Tab数量
    .参数 TabControl句柄, 整数型

.DLL命令 获取Tab内容窗口, 整数型, "emoji_window.dll", "GetTabContentWindow", , , 获取指定Tab的内容窗口句柄
    .参数 TabControl句柄, 整数型
    .参数 索引, 整数型

.DLL命令 销毁TabControl, , "emoji_window.dll", "DestroyTabControl", , , 销毁TabControl
    .参数 TabControl句柄, 整数型

.DLL命令 更新TabControl布局, , "emoji_window.dll", "UpdateTabControlLayout", , , 手动更新TabControl布局
    .参数 TabControl句柄, 整数型

.DLL命令 创建菜单栏, 整数型, "emoji_window.dll", "CreateMenuBar", , , 创建窗口顶部菜单栏
    .参数 所属窗口句柄, 整数型

.DLL命令 销毁菜单栏, , "emoji_window.dll", "DestroyMenuBar", , , 销毁菜单栏
    .参数 菜单栏句柄, 整数型

.DLL命令 菜单栏添加项, 整数型, "emoji_window.dll", "MenuBarAddItem", , , 添加菜单栏一级菜单
    .参数 菜单栏句柄, 整数型
    .参数 文本字节集指针, 整数型, , 取变量数据地址(文本字节集)
    .参数 文本长度, 整数型, , 取字节集长度(文本字节集)
    .参数 菜单项ID, 整数型, , 自定义ID，回调时用于识别

.DLL命令 菜单栏添加子项, 整数型, "emoji_window.dll", "MenuBarAddSubItem", , , 添加菜单栏子菜单
    .参数 菜单栏句柄, 整数型
    .参数 父项ID, 整数型, , 父菜单项的ID
    .参数 文本字节集指针, 整数型, , 取变量数据地址(文本字节集)
    .参数 文本长度, 整数型, , 取字节集长度(文本字节集)
    .参数 菜单项ID, 整数型, , 自定义ID，回调时用于识别

.DLL命令 设置菜单栏位置, , "emoji_window.dll", "SetMenuBarPlacement", , , 设置菜单栏位置与大小
    .参数 菜单栏句柄, 整数型
    .参数 X坐标, 整数型
    .参数 Y坐标, 整数型
    .参数 宽度, 整数型
    .参数 高度, 整数型

.DLL命令 设置菜单栏回调, , "emoji_window.dll", "SetMenuBarCallback", , , 设置菜单项点击回调
    .参数 菜单栏句柄, 整数型
    .参数 回调函数, 子程序指针, , 回调签名：.子程序 回调, , , stdcall  .参数 menu_id, 整数型  .参数 item_id, 整数型

.DLL命令 更新菜单栏子项文字, 逻辑型, "emoji_window.dll", "MenuBarUpdateSubItemText", , , 动态更新菜单栏子项文本
    .参数 菜单栏句柄, 整数型
    .参数 父项ID, 整数型
    .参数 菜单项ID, 整数型
    .参数 文本字节集指针, 整数型, , 取变量数据地址(文本字节集)
    .参数 文本长度, 整数型, , 取字节集长度(文本字节集)

.DLL命令 创建弹出菜单, 整数型, "emoji_window.dll", "CreateEmojiPopupMenu", , , 创建右键弹出菜单（支持Emoji）
    .参数 所属窗口句柄, 整数型

.DLL命令 销毁弹出菜单, , "emoji_window.dll", "DestroyEmojiPopupMenu", , , 销毁弹出菜单
    .参数 弹出菜单句柄, 整数型

.DLL命令 弹出菜单添加项, 整数型, "emoji_window.dll", "PopupMenuAddItem", , , 添加弹出菜单顶级项
    .参数 弹出菜单句柄, 整数型
    .参数 文本字节集指针, 整数型, , 取变量数据地址(文本字节集)
    .参数 文本长度, 整数型, , 取字节集长度(文本字节集)
    .参数 菜单项ID, 整数型, , 自定义ID，回调时用于识别

.DLL命令 弹出菜单添加子项, 整数型, "emoji_window.dll", "PopupMenuAddSubItem", , , 添加弹出菜单子项
    .参数 弹出菜单句柄, 整数型
    .参数 父项ID, 整数型, , 父菜单项的ID
    .参数 文本字节集指针, 整数型, , 取变量数据地址(文本字节集)
    .参数 文本长度, 整数型, , 取字节集长度(文本字节集)
    .参数 菜单项ID, 整数型, , 自定义ID，回调时用于识别

.DLL命令 绑定控件菜单, , "emoji_window.dll", "BindControlMenu", , , 绑定控件与弹出菜单（控件右键时自动弹出）
    .参数 控件句柄, 整数型
    .参数 弹出菜单句柄, 整数型

.DLL命令 绑定按钮菜单, , "emoji_window.dll", "BindButtonMenu", , , 给按钮绑定弹出菜单（按钮点击后弹出）
    .参数 父窗口句柄, 整数型, , 按钮所属父窗口句柄
    .参数 按钮ID, 整数型, , 按钮ID
    .参数 弹出菜单句柄, 整数型, , 弹出菜单句柄

.DLL命令 显示上下文菜单, , "emoji_window.dll", "ShowContextMenu", , , 在指定坐标显示右键菜单
    .参数 弹出菜单句柄, 整数型
    .参数 X坐标, 整数型, , 屏幕X（-1=使用当前鼠标X）
    .参数 Y坐标, 整数型, , 屏幕Y（-1=使用当前鼠标Y）

.DLL命令 设置弹出菜单回调, , "emoji_window.dll", "SetPopupMenuCallback", , , 设置弹出菜单项点击回调
    .参数 弹出菜单句柄, 整数型
    .参数 回调函数, 子程序指针, , 回调签名：.子程序 回调, , , stdcall  .参数 menu_id, 整数型  .参数 item_id, 整数型

.DLL命令 设置窗口大小改变回调, , "emoji_window.dll", "SetWindowResizeCallback", , , 设置窗口大小改变回调
    .参数 回调函数指针, 子程序指针

.DLL命令 设置窗口关闭回调, , "emoji_window.dll", "SetWindowCloseCallback", , , 自绘窗口被关闭时触发回调（用户点X或代码销毁窗口均会触发）
    .参数 回调函数指针, 子程序指针

.DLL命令 MoveWindow, 逻辑型, "user32.dll", "MoveWindow", , , 移动和调整窗口大小
    .参数 窗口句柄, 整数型, , 要移动的窗口句柄
    .参数 X, 整数型, , 新的X坐标
    .参数 Y, 整数型, , 新的Y坐标
    .参数 宽度, 整数型, , 新的宽度
    .参数 高度, 整数型, , 新的高度
    .参数 重绘, 逻辑型, , 是否重绘窗口（真=重绘，假=不重绘）

.DLL命令 SendMessage, 整数型, "user32.dll", "SendMessageA", , , 发送窗口消息
    .参数 窗口句柄, 整数型, , 窗口句柄
    .参数 消息, 整数型, , 消息类型
    .参数 wParam, 整数型, , 消息参数1
    .参数 lParam, 整数型, , 消息参数2

.DLL命令 PostMessage, 逻辑型, "user32.dll", "PostMessageA", , , 投递窗口消息
    .参数 窗口句柄, 整数型, , 窗口句柄
    .参数 消息, 整数型, , 消息类型
    .参数 wParam, 整数型, , 消息参数1
    .参数 lParam, 整数型, , 消息参数2

.DLL命令 CreateWindowExA, 整数型, "user32.dll", "CreateWindowExA"
    .参数 dwExStyle, 整数型
    .参数 lpClassName, 文本型
    .参数 lpWindowName, 文本型
    .参数 dwStyle, 整数型
    .参数 x, 整数型
    .参数 y, 整数型
    .参数 nWidth, 整数型
    .参数 nHeight, 整数型
    .参数 hWndParent, 整数型
    .参数 hMenu, 整数型
    .参数 hInstance, 整数型
    .参数 lpParam, 整数型

.DLL命令 ShowWindow, 逻辑型, "user32.dll", "ShowWindow"
    .参数 hwnd, 整数型
    .参数 nCmdShow, 整数型

.DLL命令 UpdateWindow, 逻辑型, "user32.dll", "UpdateWindow"
    .参数 hwnd, 整数型

.DLL命令 GetModuleHandleA, 整数型, "kernel32.dll", "GetModuleHandleA"
    .参数 lpModuleName, 整数型


.DLL命令 创建编辑框, 整数型, "emoji_window.dll", "CreateEditBox", , , 创建编辑框
    .参数 父窗口句柄, 整数型
    .参数 X坐标, 整数型
    .参数 Y坐标, 整数型
    .参数 宽度, 整数型
    .参数 高度, 整数型
    .参数 文本字节集指针, 整数型
    .参数 文本长度, 整数型
    .参数 前景色, 整数型, , ARGB颜色
    .参数 背景色, 整数型, , ARGB颜色
    .参数 字体名字节集指针, 整数型
    .参数 字体名长度, 整数型
    .参数 字体大小, 整数型
    .参数 粗体, 逻辑型
    .参数 斜体, 逻辑型
    .参数 下划线, 逻辑型
    .参数 对齐方式, 整数型, , 0=左 1=中 2=右
    .参数 多行模式, 逻辑型
    .参数 只读模式, 逻辑型
    .参数 密码框, 逻辑型
    .参数 显示边框, 逻辑型
    .参数 文本垂直居中, 逻辑型, , 仅单行有效

.DLL命令 获取编辑框文本, 整数型, "emoji_window.dll", "GetEditBoxText", , , 获取编辑框文本
    .参数 编辑框句柄, 整数型
    .参数 缓冲区指针, 整数型, , 第一次传0仅获取 UTF-8 文本长度
    .参数 缓冲区大小, 整数型, , 第二次读取时建议传“返回长度 ＋ 1”，给结尾0预留空间

.DLL命令 设置编辑框文本, , "emoji_window.dll", "SetEditBoxText", , , 设置编辑框文本
    .参数 编辑框句柄, 整数型
    .参数 文本字节集指针, 整数型
    .参数 文本长度, 整数型

.DLL命令 设置编辑框字体, , "emoji_window.dll", "SetEditBoxFont", , , 设置编辑框字体
    .参数 编辑框句柄, 整数型
    .参数 字体名字节集指针, 整数型
    .参数 字体名长度, 整数型
    .参数 字体大小, 整数型
    .参数 粗体, 逻辑型
    .参数 斜体, 逻辑型
    .参数 下划线, 逻辑型

.DLL命令 设置编辑框颜色, , "emoji_window.dll", "SetEditBoxColor", , , 设置编辑框颜色
    .参数 编辑框句柄, 整数型
    .参数 前景色, 整数型, , ARGB颜色
    .参数 背景色, 整数型, , ARGB颜色

.DLL命令 设置编辑框位置, , "emoji_window.dll", "SetEditBoxBounds", , , 设置编辑框位置和大小
    .参数 编辑框句柄, 整数型
    .参数 X坐标, 整数型
    .参数 Y坐标, 整数型
    .参数 宽度, 整数型
    .参数 高度, 整数型

.DLL命令 启用编辑框, , "emoji_window.dll", "EnableEditBox", , , 启用或禁用编辑框
    .参数 编辑框句柄, 整数型
    .参数 启用, 逻辑型

.DLL命令 显示编辑框, , "emoji_window.dll", "ShowEditBox", , , 显示或隐藏编辑框
    .参数 编辑框句柄, 整数型
    .参数 显示, 逻辑型

.DLL命令 设置编辑框垂直居中, , "emoji_window.dll", "SetEditBoxVerticalCenter", , , 设置编辑框文本是否垂直居中（仅单行有效）
    .参数 编辑框句柄, 整数型
    .参数 垂直居中, 逻辑型

.DLL命令 设置编辑框按键回调, , "emoji_window.dll", "SetEditBoxKeyCallback", , , 设置编辑框按键回调（key_down: 1=按下 0=松开）
    .参数 编辑框句柄, 整数型
    .参数 回调子程序指针, 整数型, , 子程序需 stdcall，参数：hEdit, key_code, key_down, shift, ctrl, alt（均为整数型）

.DLL命令 创建彩色Emoji编辑框, 整数型, "emoji_window.dll", "CreateColorEmojiEditBox", , , 创建支持彩色Emoji的编辑框（使用RichEdit控件）
    .参数 父窗口句柄, 整数型
    .参数 X坐标, 整数型
    .参数 Y坐标, 整数型
    .参数 宽度, 整数型
    .参数 高度, 整数型
    .参数 文本字节集指针, 整数型
    .参数 文本长度, 整数型
    .参数 前景色, 整数型, , ARGB颜色
    .参数 背景色, 整数型, , ARGB颜色
    .参数 字体名字节集指针, 整数型
    .参数 字体名长度, 整数型
    .参数 字体大小, 整数型
    .参数 粗体, 逻辑型
    .参数 斜体, 逻辑型
    .参数 下划线, 逻辑型
    .参数 对齐方式, 整数型, , 0=左 1=中 2=右
    .参数 多行模式, 逻辑型
    .参数 只读模式, 逻辑型
    .参数 密码框, 逻辑型
    .参数 显示边框, 逻辑型
    .参数 文本垂直居中, 逻辑型, , 仅单行有效

.DLL命令 创建标签, 整数型, "emoji_window.dll", "CreateLabel", , , 创建标签
    .参数 父窗口句柄, 整数型
    .参数 X坐标, 整数型
    .参数 Y坐标, 整数型
    .参数 宽度, 整数型
    .参数 高度, 整数型
    .参数 文本字节集指针, 整数型
    .参数 文本长度, 整数型
    .参数 前景色, 整数型, , ARGB颜色
    .参数 背景色, 整数型, , ARGB颜色
    .参数 字体名字节集指针, 整数型
    .参数 字体名长度, 整数型
    .参数 字体大小, 整数型
    .参数 粗体, 逻辑型
    .参数 斜体, 逻辑型
    .参数 下划线, 逻辑型
    .参数 对齐方式, 整数型, , 0=左 1=中 2=右
    .参数 是否换行, 逻辑型, , 是否自动换行显示

.DLL命令 设置标签文本, , "emoji_window.dll", "SetLabelText", , , 设置标签文本
    .参数 标签句柄, 整数型
    .参数 文本字节集指针, 整数型
    .参数 文本长度, 整数型

.DLL命令 设置标签字体, , "emoji_window.dll", "SetLabelFont", , , 设置标签字体
    .参数 标签句柄, 整数型
    .参数 字体名字节集指针, 整数型
    .参数 字体名长度, 整数型
    .参数 字体大小, 整数型
    .参数 粗体, 逻辑型
    .参数 斜体, 逻辑型
    .参数 下划线, 逻辑型

.DLL命令 设置标签颜色, , "emoji_window.dll", "SetLabelColor", , , 设置标签颜色
    .参数 标签句柄, 整数型
    .参数 前景色, 整数型, , ARGB颜色
    .参数 背景色, 整数型, , ARGB颜色

.DLL命令 设置标签位置, , "emoji_window.dll", "SetLabelBounds", , , 设置标签位置和大小
    .参数 标签句柄, 整数型
    .参数 X坐标, 整数型
    .参数 Y坐标, 整数型
    .参数 宽度, 整数型
    .参数 高度, 整数型

.DLL命令 启用标签, , "emoji_window.dll", "EnableLabel", , , 启用或禁用标签
    .参数 标签句柄, 整数型
    .参数 启用, 逻辑型

.DLL命令 显示标签, , "emoji_window.dll", "ShowLabel", , , 显示或隐藏标签
    .参数 标签句柄, 整数型
    .参数 显示, 逻辑型

.DLL命令 获取标签文本, 整数型, "emoji_window.dll", "GetLabelText", , , 获取标签文本（返回UTF-8字节数，缓冲区指针为0时返回所需大小）
    .参数 标签句柄, 整数型, , 标签窗口句柄
    .参数 缓冲区指针, 整数型, , UTF-8缓冲区指针（可为0获取所需大小）
    .参数 缓冲区大小, 整数型, , 缓冲区大小

.DLL命令 获取标签字体, 整数型, "emoji_window.dll", "GetLabelFont", , , 获取标签字体信息（返回字体名UTF-8字节数，缓冲区指针为0时返回所需大小）
    .参数 标签句柄, 整数型, , 标签窗口句柄
    .参数 字体名缓冲区指针, 整数型, , UTF-8字体名缓冲区指针（可为0获取所需大小）
    .参数 字体名缓冲区大小, 整数型, , 缓冲区大小
    .参数 字体大小, 整数型, 传址, 输出参数：字体大小（调用时请用"传址 变量"传参）
    .参数 粗体, 整数型, 传址, 输出参数：是否粗体（1=是，0=否，调用时请用"传址 变量"传参）
    .参数 斜体, 整数型, 传址, 输出参数：是否斜体（1=是，0=否，调用时请用"传址 变量"传参）
    .参数 下划线, 整数型, 传址, 输出参数：是否下划线（1=是，0=否，调用时请用"传址 变量"传参）

.DLL命令 获取标签颜色, 整数型, "emoji_window.dll", "GetLabelColor", , , 获取标签颜色（成功返回0，失败返回-1）
    .参数 标签句柄, 整数型, , 标签窗口句柄
    .参数 前景色, 整数型, 传址, 输出参数：前景色ARGB（调用时请用"传址 变量"传参）
    .参数 背景色, 整数型, 传址, 输出参数：背景色ARGB（调用时请用"传址 变量"传参）

.DLL命令 获取标签位置, 整数型, "emoji_window.dll", "GetLabelBounds", , , 获取标签位置和大小（成功返回0，失败返回-1）
    .参数 标签句柄, 整数型, , 标签窗口句柄
    .参数 X坐标, 整数型, 传址, 输出参数：X坐标（调用时请用"传址 变量"传参）
    .参数 Y坐标, 整数型, 传址, 输出参数：Y坐标（调用时请用"传址 变量"传参）
    .参数 宽度, 整数型, 传址, 输出参数：宽度（调用时请用"传址 变量"传参）
    .参数 高度, 整数型, 传址, 输出参数：高度（调用时请用"传址 变量"传参）


.DLL命令 获取标签对齐方式, 整数型, "emoji_window.dll", "GetLabelAlignment", , , 获取标签对齐方式（0=左，1=中，2=右，-1=错误）
    .参数 标签句柄, 整数型, , 标签窗口句柄

.DLL命令 设置标签对齐方式, , "emoji_window.dll", "SetLabelAlignment", , , 设置标签对齐方式（0=左，1=中，2=右）
    .参数 标签句柄, 整数型, , 标签窗口句柄
    .参数 对齐方式, 整数型, , 0=左，1=中，2=右

.DLL命令 获取标签启用状态, 整数型, "emoji_window.dll", "GetLabelEnabled", , , 获取标签启用状态（1=启用，0=禁用，-1=错误）
    .参数 标签句柄, 整数型, , 标签窗口句柄

.DLL命令 获取标签可视状态, 整数型, "emoji_window.dll", "GetLabelVisible", , , 获取标签可视状态（1=可见，0=不可见，-1=错误）
    .参数 标签句柄, 整数型, , 标签窗口句柄


.DLL命令 创建复选框, 整数型, "emoji_window.dll", "CreateCheckBox", , , 创建复选框
    .参数 父窗口句柄, 整数型
    .参数 X坐标, 整数型
    .参数 Y坐标, 整数型
    .参数 宽度, 整数型
    .参数 高度, 整数型
    .参数 文本字节集指针, 整数型
    .参数 文本长度, 整数型
    .参数 选中状态, 逻辑型
    .参数 前景色, 整数型, , ARGB颜色
    .参数 背景色, 整数型, , ARGB颜色
    .参数 字体名字节集指针, 整数型, , UTF-8字体名字节集指针
    .参数 字体名长度, 整数型, , 字体名字节集长度
    .参数 字体大小, 整数型, , 字体大小（像素）
    .参数 粗体, 逻辑型
    .参数 斜体, 逻辑型
    .参数 下划线, 逻辑型

.DLL命令 获取复选框状态, 逻辑型, "emoji_window.dll", "GetCheckBoxState", , , 获取复选框选中状态
    .参数 复选框句柄, 整数型

.DLL命令 设置复选框状态, , "emoji_window.dll", "SetCheckBoxState", , , 设置复选框选中状态
    .参数 复选框句柄, 整数型
    .参数 选中状态, 逻辑型

.DLL命令 设置复选框回调, , "emoji_window.dll", "SetCheckBoxCallback", , , 设置复选框回调
    .参数 复选框句柄, 整数型
    .参数 回调子程序指针, 子程序指针, , 子程序需 stdcall，参数：hCheckBox(整数型), checked(逻辑型)

.DLL命令 启用复选框, , "emoji_window.dll", "EnableCheckBox", , , 启用或禁用复选框
    .参数 复选框句柄, 整数型
    .参数 启用, 逻辑型

.DLL命令 显示复选框, , "emoji_window.dll", "ShowCheckBox", , , 显示或隐藏复选框
    .参数 复选框句柄, 整数型
    .参数 显示, 逻辑型

.DLL命令 设置复选框文本, , "emoji_window.dll", "SetCheckBoxText", , , 设置复选框文本
    .参数 复选框句柄, 整数型
    .参数 文本字节集指针, 整数型
    .参数 文本长度, 整数型

.DLL命令 设置复选框位置, , "emoji_window.dll", "SetCheckBoxBounds", , , 设置复选框位置和大小
    .参数 复选框句柄, 整数型
    .参数 X坐标, 整数型
    .参数 Y坐标, 整数型
    .参数 宽度, 整数型
    .参数 高度, 整数型

' ========== 进度条功能 ==========

.DLL命令 创建进度条, 整数型, "emoji_window.dll", "CreateProgressBar", , , 创建进度条
    .参数 父窗口句柄, 整数型
    .参数 X坐标, 整数型
    .参数 Y坐标, 整数型
    .参数 宽度, 整数型
    .参数 高度, 整数型
    .参数 初始值, 整数型, , 0-100
    .参数 前景色, 整数型, , ARGB颜色
    .参数 背景色, 整数型, , ARGB颜色
    .参数 显示文本, 逻辑型, , 是否显示百分比文本
    .参数 文本颜色, 整数型, , ARGB颜色

.DLL命令 设置进度条值, , "emoji_window.dll", "SetProgressValue", , , 设置进度条值(0-100)
    .参数 进度条句柄, 整数型
    .参数 值, 整数型, , 0-100

.DLL命令 获取进度条值, 整数型, "emoji_window.dll", "GetProgressValue", , , 获取进度条值
    .参数 进度条句柄, 整数型

.DLL命令 设置进度条不确定模式, , "emoji_window.dll", "SetProgressIndeterminate", , , 设置进度条不确定模式
    .参数 进度条句柄, 整数型
    .参数 不确定模式, 逻辑型

.DLL命令 设置进度条颜色, , "emoji_window.dll", "SetProgressBarColor", , , 设置进度条颜色
    .参数 进度条句柄, 整数型
    .参数 前景色, 整数型, , ARGB颜色
    .参数 背景色, 整数型, , ARGB颜色

.DLL命令 设置进度条回调, , "emoji_window.dll", "SetProgressBarCallback", , , 设置进度条回调
    .参数 进度条句柄, 整数型
    .参数 回调子程序指针, 整数型, , 子程序需 stdcall，参数：hProgressBar(整数型), value(整数型)

.DLL命令 启用进度条, , "emoji_window.dll", "EnableProgressBar", , , 启用或禁用进度条
    .参数 进度条句柄, 整数型
    .参数 启用, 逻辑型

.DLL命令 显示进度条, , "emoji_window.dll", "ShowProgressBar", , , 显示或隐藏进度条
    .参数 进度条句柄, 整数型
    .参数 显示, 逻辑型

.DLL命令 设置进度条位置, , "emoji_window.dll", "SetProgressBarBounds", , , 设置进度条位置和大小
    .参数 进度条句柄, 整数型
    .参数 X坐标, 整数型
    .参数 Y坐标, 整数型
    .参数 宽度, 整数型
    .参数 高度, 整数型

.DLL命令 设置进度条显示文本, , "emoji_window.dll", "SetProgressBarShowText", , , 设置是否显示百分比文本
    .参数 进度条句柄, 整数型
    .参数 显示文本, 逻辑型


' ========== 树形框功能 ==========

.DLL命令 创建标准树形框, 整数型, "emoji_window.dll", "CreateSimpleTreeView", , , 创建标准Windows树形控件(SysTreeView32)，用于排查Tab内显示问题
    .参数 父窗口句柄, 整数型
    .参数 X坐标, 整数型
    .参数 Y坐标, 整数型
    .参数 宽度, 整数型
    .参数 高度, 整数型

.DLL命令 创建树形框, 整数型, "emoji_window.dll", "CreateTreeView", , , 创建树形框控件
    .参数 父窗口句柄, 整数型
    .参数 X坐标, 整数型
    .参数 Y坐标, 整数型
    .参数 宽度, 整数型
    .参数 高度, 整数型
    .参数 背景色, 整数型, , ARGB颜色
    .参数 文本色, 整数型, , ARGB颜色
    .参数 回调上下文, 整数型, , 可选，传 0

.DLL命令 设置树形框位置, 逻辑型, "emoji_window.dll", "SetTreeViewBounds", , , 设置树形框位置和大小；坐标与尺寸为96DPI逻辑单位
    .参数 树形框句柄, 整数型
    .参数 X坐标, 整数型
    .参数 Y坐标, 整数型
    .参数 宽度, 整数型
    .参数 高度, 整数型

.DLL命令 取树形框位置, 逻辑型, "emoji_window.dll", "GetTreeViewBounds", , , 读取树形框位置和大小；返回96DPI逻辑单位
    .参数 树形框句柄, 整数型
    .参数 X坐标指针, 整数型
    .参数 Y坐标指针, 整数型
    .参数 宽度指针, 整数型
    .参数 高度指针, 整数型

.DLL命令 添加根节点, 整数型, "emoji_window.dll", "AddRootNode", , , 添加根节点，返回节点ID
    .参数 树形框句柄, 整数型
    .参数 文本字节集指针, 整数型, , UTF-8字节集指针
    .参数 文本长度, 整数型, , 字节集长度
    .参数 图标字节集指针, 整数型, , UTF-8图标字节集指针，无图标传0
    .参数 图标长度, 整数型, , 图标字节集长度，无图标传0

.DLL命令 添加子节点, 整数型, "emoji_window.dll", "AddChildNode", , , 添加子节点，返回节点ID
    .参数 树形框句柄, 整数型
    .参数 父节点ID, 整数型
    .参数 文本字节集指针, 整数型, , UTF-8字节集指针
    .参数 文本长度, 整数型, , 字节集长度
    .参数 图标字节集指针, 整数型, , UTF-8图标字节集指针，无图标传0
    .参数 图标长度, 整数型, , 图标字节集长度，无图标传0

.DLL命令 清空树形框, 逻辑型, "emoji_window.dll", "ClearTree", , , 清空树形框所有节点
    .参数 树形框句柄, 整数型

.DLL命令 销毁树形框, 逻辑型, "emoji_window.dll", "DestroyTreeView", , , 销毁树形框
    .参数 树形框句柄, 整数型

.DLL命令 删除树节点, 逻辑型, "emoji_window.dll", "RemoveNode", , , 删除节点（含子树）
    .参数 树形框句柄, 整数型
    .参数 节点ID, 整数型

.DLL命令 展开树节点, 逻辑型, "emoji_window.dll", "ExpandNode", , , 展开节点
    .参数 树形框句柄, 整数型
    .参数 节点ID, 整数型

.DLL命令 折叠树节点, 逻辑型, "emoji_window.dll", "CollapseNode", , , 折叠节点
    .参数 树形框句柄, 整数型
    .参数 节点ID, 整数型

.DLL命令 展开全部树节点, 逻辑型, "emoji_window.dll", "ExpandAll", , , 展开全部
    .参数 树形框句柄, 整数型

.DLL命令 折叠全部树节点, 逻辑型, "emoji_window.dll", "CollapseAll", , , 折叠全部
    .参数 树形框句柄, 整数型

.DLL命令 取节点是否展开, 逻辑型, "emoji_window.dll", "IsNodeExpanded", , , 节点是否展开
    .参数 树形框句柄, 整数型
    .参数 节点ID, 整数型

.DLL命令 设置选中树节点, 逻辑型, "emoji_window.dll", "SetSelectedNode", , , 设置选中节点
    .参数 树形框句柄, 整数型
    .参数 节点ID, 整数型

.DLL命令 取选中树节点, 整数型, "emoji_window.dll", "GetSelectedNode", , , 获取选中节点ID，无选中返回-1
    .参数 树形框句柄, 整数型

.DLL命令 设置树节点文本, 逻辑型, "emoji_window.dll", "SetNodeText", , , 设置节点文本UTF-8
    .参数 树形框句柄, 整数型
    .参数 节点ID, 整数型
    .参数 文本字节集指针, 整数型
    .参数 文本长度, 整数型

.DLL命令 取树节点文本, 整数型, "emoji_window.dll", "GetNodeText", , , 获取节点文本，返回UTF-8字节数
    .参数 树形框句柄, 整数型
    .参数 节点ID, 整数型
    .参数 缓冲区指针, 整数型
    .参数 缓冲区大小, 整数型

.DLL命令 设置树节点图标, 逻辑型, "emoji_window.dll", "SetNodeIcon", , , 设置节点图标UTF-8（emoji）
    .参数 树形框句柄, 整数型
    .参数 节点ID, 整数型
    .参数 图标字节集指针, 整数型
    .参数 图标长度, 整数型

.DLL命令 取树节点图标, 整数型, "emoji_window.dll", "GetNodeIcon", , , 获取节点图标
    .参数 树形框句柄, 整数型
    .参数 节点ID, 整数型
    .参数 缓冲区指针, 整数型
    .参数 缓冲区大小, 整数型

.DLL命令 设置树节点前景色, 逻辑型, "emoji_window.dll", "SetNodeForeColor", , , 设置节点前景色ARGB
    .参数 树形框句柄, 整数型
    .参数 节点ID, 整数型
    .参数 颜色, 整数型

.DLL命令 设置树节点背景色, 逻辑型, "emoji_window.dll", "SetNodeBackColor", , , 设置节点背景色ARGB
    .参数 树形框句柄, 整数型
    .参数 节点ID, 整数型
    .参数 颜色, 整数型

.DLL命令 设置树节点启用, 逻辑型, "emoji_window.dll", "SetNodeEnabled", , , 启用/禁用节点
    .参数 树形框句柄, 整数型
    .参数 节点ID, 整数型
    .参数 启用, 逻辑型

.DLL命令 取树节点是否启用, 逻辑型, "emoji_window.dll", "IsNodeEnabled", , , 节点是否启用
    .参数 树形框句柄, 整数型
    .参数 节点ID, 整数型

.DLL命令 设置树节点复选框, 逻辑型, "emoji_window.dll", "SetNodeCheckBox", , , 是否显示复选框
    .参数 树形框句柄, 整数型
    .参数 节点ID, 整数型
    .参数 显示, 逻辑型

.DLL命令 设置树节点勾选, 逻辑型, "emoji_window.dll", "SetNodeChecked", , , 设置复选框状态
    .参数 树形框句柄, 整数型
    .参数 节点ID, 整数型
    .参数 选中, 逻辑型

.DLL命令 取树节点勾选状态, 整数型, "emoji_window.dll", "GetNodeChecked", , , 获取复选框状态
    .参数 树形框句柄, 整数型
    .参数 节点ID, 整数型

.DLL命令 按文本查找树节点, 整数型, "emoji_window.dll", "FindNodeByText", , , 按UTF-8文本查找节点ID
    .参数 树形框句柄, 整数型
    .参数 文本字节集指针, 整数型
    .参数 文本长度, 整数型

.DLL命令 取树节点父节点, 整数型, "emoji_window.dll", "GetNodeParent", , , 父节点ID，根返回-1
    .参数 树形框句柄, 整数型
    .参数 节点ID, 整数型

.DLL命令 取树子节点数量, 整数型, "emoji_window.dll", "GetNodeChildCount", , , 子节点数量
    .参数 树形框句柄, 整数型
    .参数 节点ID, 整数型

.DLL命令 取树子节点列表, 整数型, "emoji_window.dll", "GetNodeChildren", , , 子节点ID写入整数数组，返回数量
    .参数 树形框句柄, 整数型
    .参数 节点ID, 整数型
    .参数 缓冲区指针, 整数型, , 整数型数组首地址
    .参数 缓冲区元素数, 整数型

.DLL命令 取树节点层级, 整数型, "emoji_window.dll", "GetNodeLevel", , , 层级深度根为0
    .参数 树形框句柄, 整数型
    .参数 节点ID, 整数型

.DLL命令 滚动到树节点, 逻辑型, "emoji_window.dll", "ScrollToNode", , , 滚动到节点可见
    .参数 树形框句柄, 整数型
    .参数 节点ID, 整数型

.DLL命令 设置树形框滚动位置, 逻辑型, "emoji_window.dll", "SetTreeViewScrollPos", , , 设置滚动位置像素
    .参数 树形框句柄, 整数型
    .参数 位置, 整数型

.DLL命令 取树形框滚动位置, 整数型, "emoji_window.dll", "GetTreeViewScrollPos", , , 获取滚动位置像素
    .参数 树形框句柄, 整数型

.DLL命令 设置树形框回调, 逻辑型, "emoji_window.dll", "SetTreeViewCallback", , , 树形框回调，类型1选中2展开3折叠4双击5右键6文本改7勾选8移动9编辑完成
    .参数 树形框句柄, 整数型
    .参数 回调类型, 整数型
    .参数 回调函数, 子程序指针

.DLL命令 设置树形框编辑完成回调, 逻辑型, "emoji_window.dll", "SetTreeViewEditFinishedCallback", , , 设置节点编辑提交完成回调；回调参数为 节点ID, UTF-8文本指针, 文本长度, 是否改变, 上下文指针
    .参数 树形框句柄, 整数型
    .参数 回调函数, 子程序指针

.DLL命令 启用树形框拖放, 逻辑型, "emoji_window.dll", "EnableTreeViewDragDrop", , , 启用拖放移动节点
    .参数 树形框句柄, 整数型
    .参数 启用, 逻辑型

.DLL命令 设置树形框侧栏模式, 逻辑型, "emoji_window.dll", "SetTreeViewSidebarMode", , , 侧栏折叠菜单模式（右侧箭头交互见文档）
    .参数 树形框句柄, 整数型
    .参数 启用, 逻辑型

.DLL命令 取树形框侧栏模式, 逻辑型, "emoji_window.dll", "GetTreeViewSidebarMode", , , 是否侧栏模式
    .参数 树形框句柄, 整数型

.DLL命令 设置树形框行高, 逻辑型, "emoji_window.dll", "SetTreeViewRowHeight", , , 设置行高（逻辑单位）
    .参数 树形框句柄, 整数型
    .参数 行高, 小数型

.DLL命令 取树形框行高, 小数型, "emoji_window.dll", "GetTreeViewRowHeight", , , 获取行高
    .参数 树形框句柄, 整数型

.DLL命令 设置树形框行间距, 逻辑型, "emoji_window.dll", "SetTreeViewItemSpacing", , , 设置行间距
    .参数 树形框句柄, 整数型
    .参数 间距, 小数型

.DLL命令 取树形框行间距, 小数型, "emoji_window.dll", "GetTreeViewItemSpacing", , , 获取行间距
    .参数 树形框句柄, 整数型

.DLL命令 设置树形框文字色, 逻辑型, "emoji_window.dll", "SetTreeViewTextColor", , , 全局默认文字色ARGB
    .参数 树形框句柄, 整数型
    .参数 颜色, 整数型

.DLL命令 取树形框文字色, 整数型, "emoji_window.dll", "GetTreeViewTextColor", , , 获取全局默认文字色
    .参数 树形框句柄, 整数型

.DLL命令 设置树形框选中背景色, 逻辑型, "emoji_window.dll", "SetTreeViewSelectedBgColor", , , 选中背景ARGB
    .参数 树形框句柄, 整数型
    .参数 颜色, 整数型

.DLL命令 取树形框选中背景色, 整数型, "emoji_window.dll", "GetTreeViewSelectedBgColor", , , 获取选中背景
    .参数 树形框句柄, 整数型

.DLL命令 设置树形框选中前景色, 逻辑型, "emoji_window.dll", "SetTreeViewSelectedForeColor", , , 选中前景ARGB
    .参数 树形框句柄, 整数型
    .参数 颜色, 整数型

.DLL命令 取树形框选中前景色, 整数型, "emoji_window.dll", "GetTreeViewSelectedForeColor", , , 获取选中前景
    .参数 树形框句柄, 整数型

.DLL命令 设置树形框悬停背景色, 逻辑型, "emoji_window.dll", "SetTreeViewHoverBgColor", , , 悬停背景ARGB
    .参数 树形框句柄, 整数型
    .参数 颜色, 整数型

.DLL命令 取树形框悬停背景色, 整数型, "emoji_window.dll", "GetTreeViewHoverBgColor", , , 获取悬停背景
    .参数 树形框句柄, 整数型

.DLL命令 设置树形框字体, 逻辑型, "emoji_window.dll", "SetTreeViewFont", , , 设置树字体UTF-8族名、字号、字重、斜体
    .参数 树形框句柄, 整数型
    .参数 字体族字节集指针, 整数型
    .参数 字体族长度, 整数型
    .参数 字号, 小数型
    .参数 字重, 整数型, , 如400正常700粗体
    .参数 斜体, 逻辑型

.DLL命令 取树形框字体, 逻辑型, "emoji_window.dll", "GetTreeViewFont", , , 获取字体；族名写入缓冲区UTF-8
    .参数 树形框句柄, 整数型
    .参数 缓冲区指针, 整数型
    .参数 缓冲区大小, 整数型
    .参数 字号, 小数型, 传址
    .参数 字重, 整数型, 传址
    .参数 斜体, 逻辑型, 传址

.DLL命令 移动树形框节点, 逻辑型, "emoji_window.dll", "MoveTreeViewNode", , , 程序化移动节点；新父-1表示根
    .参数 树形框句柄, 整数型
    .参数 节点ID, 整数型
    .参数 新父节点ID, 整数型
    .参数 插入索引, 整数型


' ========== 图片框功能 ==========

.DLL命令 创建图片框, 整数型, "emoji_window.dll", "CreatePictureBox", , , 创建图片框
    .参数 父窗口句柄, 整数型
    .参数 X坐标, 整数型
    .参数 Y坐标, 整数型
    .参数 宽度, 整数型
    .参数 高度, 整数型
    .参数 缩放模式, 整数型, , 0=不缩放 1=拉伸 2=等比缩放 3=居中
    .参数 背景色, 整数型, , ARGB颜色

.DLL命令 从文件加载图片, 逻辑型, "emoji_window.dll", "LoadImageFromFile", , , 从文件加载图片(支持PNG、JPG、BMP、GIF)
    .参数 图片框句柄, 整数型
    .参数 文件路径字节集指针, 整数型
    .参数 路径长度, 整数型

.DLL命令 从内存加载图片, 逻辑型, "emoji_window.dll", "LoadImageFromMemory", , , 从内存加载图片
    .参数 图片框句柄, 整数型
    .参数 图片数据指针, 整数型
    .参数 数据长度, 整数型

.DLL命令 清除图片, , "emoji_window.dll", "ClearImage", , , 清除图片框中的图片
    .参数 图片框句柄, 整数型

.DLL命令 设置图片透明度, , "emoji_window.dll", "SetImageOpacity", , , 设置图片透明度(0.0-1.0)
    .参数 图片框句柄, 整数型
    .参数 透明度, 小数型, , 0.0-1.0

.DLL命令 设置图片框回调, , "emoji_window.dll", "SetPictureBoxCallback", , , 设置图片框回调
    .参数 图片框句柄, 整数型
    .参数 回调子程序指针, 整数型, , 子程序需 stdcall，参数：hPictureBox(整数型)

.DLL命令 启用图片框, , "emoji_window.dll", "EnablePictureBox", , , 启用或禁用图片框
    .参数 图片框句柄, 整数型
    .参数 启用, 逻辑型

.DLL命令 显示图片框, , "emoji_window.dll", "ShowPictureBox", , , 显示或隐藏图片框
    .参数 图片框句柄, 整数型
    .参数 显示, 逻辑型

.DLL命令 设置图片框位置, , "emoji_window.dll", "SetPictureBoxBounds", , , 设置图片框位置和大小
    .参数 图片框句柄, 整数型
    .参数 X坐标, 整数型
    .参数 Y坐标, 整数型
    .参数 宽度, 整数型
    .参数 高度, 整数型

.DLL命令 设置图片框缩放模式, , "emoji_window.dll", "SetPictureBoxScaleMode", , , 设置图片框缩放模式
    .参数 图片框句柄, 整数型
    .参数 缩放模式, 整数型, , 0=不缩放 1=拉伸 2=等比缩放 3=居中

.DLL命令 设置图片框背景色, , "emoji_window.dll", "SetPictureBoxBackgroundColor", , , 设置图片框背景色
    .参数 图片框句柄, 整数型
    .参数 背景色, 整数型, , ARGB颜色

.DLL命令 创建单选按钮, 整数型, "emoji_window.dll", "CreateRadioButton", , , 创建单选按钮
    .参数 父窗口句柄, 整数型
    .参数 X坐标, 整数型
    .参数 Y坐标, 整数型
    .参数 宽度, 整数型
    .参数 高度, 整数型
    .参数 文本字节集指针, 整数型, , UTF-8字节集指针
    .参数 文本长度, 整数型, , 字节集长度
    .参数 分组ID, 整数型, , 同组互斥
    .参数 选中状态, 逻辑型, , 是否选中
    .参数 前景色, 整数型, , ARGB颜色
    .参数 背景色, 整数型, , ARGB颜色
    .参数 字体名字节集指针, 整数型, , UTF-8字体名字节集指针
    .参数 字体名长度, 整数型, , 字体名字节集长度
    .参数 字体大小, 整数型, , 字体大小（像素）
    .参数 粗体, 逻辑型
    .参数 斜体, 逻辑型
    .参数 下划线, 逻辑型

.DLL命令 获取单选按钮状态, 逻辑型, "emoji_window.dll", "GetRadioButtonState", , , 获取单选按钮选中状态
    .参数 单选按钮句柄, 整数型

.DLL命令 设置单选按钮状态, , "emoji_window.dll", "SetRadioButtonState", , , 设置单选按钮选中状态
    .参数 单选按钮句柄, 整数型
    .参数 选中状态, 逻辑型

.DLL命令 设置单选按钮回调, , "emoji_window.dll", "SetRadioButtonCallback", , , 设置单选按钮回调
    .参数 单选按钮句柄, 整数型
    .参数 回调函数, 子程序指针

.DLL命令 启用单选按钮, , "emoji_window.dll", "EnableRadioButton", , , 启用/禁用单选按钮
    .参数 单选按钮句柄, 整数型
    .参数 启用, 逻辑型

.DLL命令 显示单选按钮, , "emoji_window.dll", "ShowRadioButton", , , 显示/隐藏单选按钮
    .参数 单选按钮句柄, 整数型
    .参数 显示, 逻辑型

.DLL命令 设置单选按钮文本, , "emoji_window.dll", "SetRadioButtonText", , , 设置单选按钮文本
    .参数 单选按钮句柄, 整数型
    .参数 文本字节集指针, 整数型, , UTF-8字节集指针
    .参数 文本长度, 整数型, , 字节集长度

.DLL命令 设置单选按钮位置, , "emoji_window.dll", "SetRadioButtonBounds", , , 设置单选按钮位置和大小
    .参数 单选按钮句柄, 整数型
    .参数 X坐标, 整数型
    .参数 Y坐标, 整数型
    .参数 宽度, 整数型
    .参数 高度, 整数型

.DLL命令 获取单选按钮位置, 整数型, "emoji_window.dll", "GetRadioButtonBounds", , , 获取单选按钮位置和大小（成功返回0，失败返回-1）
    .参数 单选按钮句柄, 整数型, , 单选按钮窗口句柄
    .参数 X坐标, 整数型, 传址, 输出参数：X坐标（调用时请用"传址 变量"传参）
    .参数 Y坐标, 整数型, 传址, 输出参数：Y坐标（调用时请用"传址 变量"传参）
    .参数 宽度, 整数型, 传址, 输出参数：宽度（调用时请用"传址 变量"传参）
    .参数 高度, 整数型, 传址, 输出参数：高度（调用时请用"传址 变量"传参）

.DLL命令 获取单选按钮可视状态, 整数型, "emoji_window.dll", "GetRadioButtonVisible", , , 获取单选按钮可视状态（1=可见，0=不可见，-1=错误）
    .参数 单选按钮句柄, 整数型, , 单选按钮窗口句柄

.DLL命令 获取单选按钮启用状态, 整数型, "emoji_window.dll", "GetRadioButtonEnabled", , , 获取单选按钮启用状态（1=启用，0=禁用，-1=错误）
    .参数 单选按钮句柄, 整数型, , 单选按钮窗口句柄

' ========== 列表框功能 ==========

.DLL命令 创建列表框, 整数型, "emoji_window.dll", "CreateListBox", , , 创建列表框
    .参数 父窗口句柄, 整数型
    .参数 X坐标, 整数型
    .参数 Y坐标, 整数型
    .参数 宽度, 整数型
    .参数 高度, 整数型
    .参数 多选模式, 逻辑型, , 是否支持多选
    .参数 前景色, 整数型, , ARGB颜色
    .参数 背景色, 整数型, , ARGB颜色

.DLL命令 添加列表项, 整数型, "emoji_window.dll", "AddListItem", , , 添加列表项，返回项目ID
    .参数 列表框句柄, 整数型
    .参数 文本字节集指针, 整数型, , UTF-8字节集指针
    .参数 文本长度, 整数型, , 字节集长度

.DLL命令 移除列表项, , "emoji_window.dll", "RemoveListItem", , , 移除指定索引的列表项
    .参数 列表框句柄, 整数型
    .参数 索引, 整数型, , 项目索引(从0开始)

.DLL命令 清空列表框, , "emoji_window.dll", "ClearListBox", , , 清空列表框所有项目
    .参数 列表框句柄, 整数型

.DLL命令 获取选中项索引, 整数型, "emoji_window.dll", "GetSelectedIndex", , , 获取当前选中项索引(-1表示无选中)
    .参数 列表框句柄, 整数型

.DLL命令 设置选中项索引, , "emoji_window.dll", "SetSelectedIndex", , , 设置选中项索引
    .参数 列表框句柄, 整数型
    .参数 索引, 整数型, , 项目索引(从0开始，-1表示取消选中)

.DLL命令 获取列表项数量, 整数型, "emoji_window.dll", "GetListItemCount", , , 获取列表项数量
    .参数 列表框句柄, 整数型

.DLL命令 获取列表项文本, 整数型, "emoji_window.dll", "GetListItemText", , , 获取列表项文本，返回实际复制的字节数
    .参数 列表框句柄, 整数型
    .参数 索引, 整数型, , 项目索引(从0开始)
    .参数 缓冲区指针, 整数型, , UTF-8缓冲区指针
    .参数 缓冲区大小, 整数型, , 缓冲区大小

.DLL命令 设置列表框回调, , "emoji_window.dll", "SetListBoxCallback", , , 设置列表框选中回调
    .参数 列表框句柄, 整数型
    .参数 回调函数, 子程序指针, , 子程序需 stdcall，参数：hListBox(整数型), index(整数型)

.DLL命令 启用列表框, , "emoji_window.dll", "EnableListBox", , , 启用/禁用列表框
    .参数 列表框句柄, 整数型
    .参数 启用, 逻辑型

.DLL命令 显示列表框, , "emoji_window.dll", "ShowListBox", , , 显示/隐藏列表框
    .参数 列表框句柄, 整数型
    .参数 显示, 逻辑型

.DLL命令 设置列表框位置, , "emoji_window.dll", "SetListBoxBounds", , , 设置列表框位置和大小
    .参数 列表框句柄, 整数型
    .参数 X坐标, 整数型
    .参数 Y坐标, 整数型
    .参数 宽度, 整数型
    .参数 高度, 整数型

.DLL命令 设置列表项文本, 逻辑型, "emoji_window.dll", "SetListItemText", , , 设置指定索引的列表项文本，成功返回真
    .参数 列表框句柄, 整数型
    .参数 索引, 整数型, , 项目索引(从0开始)
    .参数 文本字节集指针, 整数型, , UTF-8字节集指针
    .参数 文本长度, 整数型, , 字节集长度


' ========== 分组框功能 ==========

.DLL命令 创建分组框, 整数型, "emoji_window.dll", "CreateGroupBox", , , 创建分组框
    .参数 父窗口句柄, 整数型
    .参数 X坐标, 整数型
    .参数 Y坐标, 整数型
    .参数 宽度, 整数型
    .参数 高度, 整数型
    .参数 标题字节集指针, 整数型, , UTF-8字节集指针
    .参数 标题长度, 整数型, , 字节集长度
    .参数 边框颜色, 整数型, , ARGB颜色
    .参数 背景色, 整数型, , ARGB颜色
    .参数 字体名字节集指针, 整数型, , UTF-8字体名字节集指针，0则使用默认字体
    .参数 字体名长度, 整数型, , 字体名字节集长度
    .参数 字体大小, 整数型, , 0则使用默认大小14
    .参数 粗体, 逻辑型
    .参数 斜体, 逻辑型
    .参数 下划线, 逻辑型

.DLL命令 添加子控件到分组框, , "emoji_window.dll", "AddChildToGroup", , , 添加子控件到分组框
    .参数 分组框句柄, 整数型
    .参数 子控件句柄, 整数型

.DLL命令 从分组框移除子控件, , "emoji_window.dll", "RemoveChildFromGroup", , , 从分组框移除子控件
    .参数 分组框句柄, 整数型
    .参数 子控件句柄, 整数型

.DLL命令 设置分组框标题, , "emoji_window.dll", "SetGroupBoxTitle", , , 设置分组框标题
    .参数 分组框句柄, 整数型
    .参数 标题字节集指针, 整数型, , UTF-8字节集指针
    .参数 标题长度, 整数型, , 字节集长度

.DLL命令 启用分组框, , "emoji_window.dll", "EnableGroupBox", , , 启用/禁用分组框
    .参数 分组框句柄, 整数型
    .参数 启用, 逻辑型

.DLL命令 显示分组框, , "emoji_window.dll", "ShowGroupBox", , , 显示/隐藏分组框
    .参数 分组框句柄, 整数型
    .参数 显示, 逻辑型

.DLL命令 设置分组框位置, , "emoji_window.dll", "SetGroupBoxBounds", , , 设置分组框位置和大小
    .参数 分组框句柄, 整数型
    .参数 X坐标, 整数型
    .参数 Y坐标, 整数型
    .参数 宽度, 整数型
    .参数 高度, 整数型

.DLL命令 设置分组框回调, , "emoji_window.dll", "SetGroupBoxCallback", , , 设置分组框回调
    .参数 分组框句柄, 整数型
    .参数 回调函数, 子程序指针

.DLL命令 设置分组框标题颜色, , "emoji_window.dll", "SetGroupBoxTitleColor", , , 设置分组框标题颜色
    .参数 分组框句柄, 整数型
    .参数 标题颜色, 整数型, , ARGB颜色；也可传主题颜色索引

.DLL命令 获取分组框标题颜色, 整数型, "emoji_window.dll", "GetGroupBoxTitleColor", , , 获取分组框标题颜色
    .参数 分组框句柄, 整数型
    .参数 标题颜色地址, 整数型, , 输出参数，请传变量地址

.DLL命令 设置分组框样式, , "emoji_window.dll", "SetGroupBoxStyle", , , 设置分组框样式
    .参数 分组框句柄, 整数型
    .参数 样式, 整数型, , 0=Outline，1=Card，2=Plain，3=HeaderBar

.DLL命令 获取分组框样式, 整数型, "emoji_window.dll", "GetGroupBoxStyle", , , 获取分组框样式
    .参数 分组框句柄, 整数型


' ========== 组合框功能 ==========

.DLL命令 创建组合框, 整数型, "emoji_window.dll", "CreateComboBox", , , 创建组合框（增强版）
    .参数 父窗口句柄, 整数型
    .参数 X坐标, 整数型
    .参数 Y坐标, 整数型
    .参数 宽度, 整数型
    .参数 高度, 整数型
    .参数 只读模式, 逻辑型, , 是否只读（不可编辑）
    .参数 前景色, 整数型, , ARGB颜色
    .参数 背景色, 整数型, , ARGB颜色
    .参数 表项高度, 整数型, , 下拉列表项高度（默认35）
    .参数 字体名字节集指针, 整数型, , 字体名称UTF-8字节集指针
    .参数 字体名长度, 整数型, , 字体名长度
    .参数 字体大小, 整数型, , 字体大小（默认14）
    .参数 粗体, 逻辑型, , 是否粗体
    .参数 斜体, 逻辑型, , 是否斜体
    .参数 下划线, 逻辑型, , 是否下划线

.DLL命令 添加组合框项, 整数型, "emoji_window.dll", "AddComboItem", , , 添加组合框项，返回项目ID
    .参数 组合框句柄, 整数型
    .参数 文本字节集指针, 整数型, , UTF-8字节集指针
    .参数 文本长度, 整数型, , 字节集长度

.DLL命令 移除组合框项, , "emoji_window.dll", "RemoveComboItem", , , 移除指定索引的组合框项
    .参数 组合框句柄, 整数型
    .参数 索引, 整数型, , 项目索引(从0开始)

.DLL命令 清空组合框, , "emoji_window.dll", "ClearComboBox", , , 清空组合框所有项目
    .参数 组合框句柄, 整数型

.DLL命令 获取组合框选中索引, 整数型, "emoji_window.dll", "GetComboSelectedIndex", , , 获取当前选中项索引(-1表示无选中)
    .参数 组合框句柄, 整数型

.DLL命令 设置组合框选中索引, , "emoji_window.dll", "SetComboSelectedIndex", , , 设置选中项索引
    .参数 组合框句柄, 整数型
    .参数 索引, 整数型, , 项目索引(从0开始，-1表示取消选中)

.DLL命令 获取组合框项数量, 整数型, "emoji_window.dll", "GetComboItemCount", , , 获取组合框项数量
    .参数 组合框句柄, 整数型

.DLL命令 获取组合框项文本, 整数型, "emoji_window.dll", "GetComboItemText", , , 获取组合框项文本，返回实际复制的字节数
    .参数 组合框句柄, 整数型
    .参数 索引, 整数型, , 项目索引(从0开始)
    .参数 缓冲区指针, 整数型, , UTF-8缓冲区指针
    .参数 缓冲区大小, 整数型, , 缓冲区大小

.DLL命令 获取组合框文本, 整数型, "emoji_window.dll", "GetComboBoxText", , , 获取组合框编辑框文本，返回实际复制的字节数
    .参数 组合框句柄, 整数型
    .参数 缓冲区指针, 整数型, , UTF-8缓冲区指针
    .参数 缓冲区大小, 整数型, , 缓冲区大小

.DLL命令 设置组合框文本, , "emoji_window.dll", "SetComboBoxText", , , 设置组合框编辑框文本
    .参数 组合框句柄, 整数型
    .参数 文本字节集指针, 整数型, , UTF-8字节集指针
    .参数 文本长度, 整数型, , 字节集长度

.DLL命令 设置组合框回调, , "emoji_window.dll", "SetComboBoxCallback", , , 设置组合框选中回调
    .参数 组合框句柄, 整数型
    .参数 回调函数, 子程序指针, , 子程序需 stdcall，参数：hComboBox(整数型), index(整数型)

.DLL命令 启用组合框, , "emoji_window.dll", "EnableComboBox", , , 启用/禁用组合框
    .参数 组合框句柄, 整数型
    .参数 启用, 逻辑型

.DLL命令 显示组合框, , "emoji_window.dll", "ShowComboBox", , , 显示/隐藏组合框
    .参数 组合框句柄, 整数型
    .参数 显示, 逻辑型

.DLL命令 设置组合框位置, , "emoji_window.dll", "SetComboBoxBounds", , , 设置组合框位置和大小
    .参数 组合框句柄, 整数型
    .参数 X坐标, 整数型
    .参数 Y坐标, 整数型
    .参数 宽度, 整数型
    .参数 高度, 整数型


' ========== D2D组合框功能（支持彩色emoji） ==========

.DLL命令 创建D2D组合框, 整数型, "emoji_window.dll", "CreateD2DComboBox", , , 创建D2D组合框（完全自定义，支持彩色emoji）
    .参数 父窗口句柄, 整数型
    .参数 X坐标, 整数型
    .参数 Y坐标, 整数型
    .参数 宽度, 整数型
    .参数 高度, 整数型
    .参数 只读模式, 逻辑型, , 是否只读（不可编辑）
    .参数 前景色, 整数型, , ARGB颜色
    .参数 背景色, 整数型, , ARGB颜色
    .参数 表项高度, 整数型, , 下拉列表项高度（默认35）
    .参数 字体名字节集指针, 整数型, , 字体名称UTF-8字节集指针
    .参数 字体名长度, 整数型, , 字体名长度
    .参数 字体大小, 整数型, , 字体大小（默认14）
    .参数 粗体, 逻辑型, , 是否粗体
    .参数 斜体, 逻辑型, , 是否斜体
    .参数 下划线, 逻辑型, , 是否下划线

.DLL命令 添加D2D组合框项, 整数型, "emoji_window.dll", "AddD2DComboItem", , , 添加D2D组合框项，返回项目ID
    .参数 组合框句柄, 整数型
    .参数 文本字节集指针, 整数型, , UTF-8字节集指针
    .参数 文本长度, 整数型, , 字节集长度

.DLL命令 移除D2D组合框项, , "emoji_window.dll", "RemoveD2DComboItem", , , 移除指定索引的D2D组合框项
    .参数 组合框句柄, 整数型
    .参数 索引, 整数型, , 项目索引(从0开始)

.DLL命令 清空D2D组合框, , "emoji_window.dll", "ClearD2DComboBox", , , 清空D2D组合框所有项目
    .参数 组合框句柄, 整数型

.DLL命令 获取D2D组合框选中索引, 整数型, "emoji_window.dll", "GetD2DComboSelectedIndex", , , 获取当前选中项索引(-1表示无选中)
    .参数 组合框句柄, 整数型

.DLL命令 设置D2D组合框选中索引, , "emoji_window.dll", "SetD2DComboSelectedIndex", , , 设置选中项索引
    .参数 组合框句柄, 整数型
    .参数 索引, 整数型, , 项目索引(从0开始，-1表示取消选中)

.DLL命令 获取D2D组合框项数量, 整数型, "emoji_window.dll", "GetD2DComboItemCount", , , 获取D2D组合框项数量
    .参数 组合框句柄, 整数型

.DLL命令 获取D2D组合框项文本, 整数型, "emoji_window.dll", "GetD2DComboItemText", , , 获取D2D组合框项文本，返回实际复制的字节数
    .参数 组合框句柄, 整数型
    .参数 索引, 整数型, , 项目索引(从0开始)
    .参数 缓冲区指针, 整数型, , UTF-8缓冲区指针
    .参数 缓冲区大小, 整数型, , 缓冲区大小

.DLL命令 获取D2D组合框文本, 整数型, "emoji_window.dll", "GetD2DComboText", , , 获取D2D组合框编辑框文本，返回实际复制的字节数
    .参数 组合框句柄, 整数型
    .参数 缓冲区指针, 整数型, , UTF-8缓冲区指针
    .参数 缓冲区大小, 整数型, , 缓冲区大小

.DLL命令 设置D2D组合框文本, , "emoji_window.dll", "SetD2DComboText", , , 设置D2D组合框编辑框文本
    .参数 组合框句柄, 整数型
    .参数 文本字节集指针, 整数型, , UTF-8字节集指针
    .参数 文本长度, 整数型, , 字节集长度

.DLL命令 获取D2D组合框选中文本, 整数型, "emoji_window.dll", "GetD2DComboSelectedText", , , 获取D2D组合框选中项文本（返回UTF-8字节数，buffer为0时返回所需大小）
    .参数 组合框句柄, 整数型
    .参数 缓冲区指针, 整数型, , UTF-8缓冲区指针（可为0获取所需大小）
    .参数 缓冲区大小, 整数型, , 缓冲区大小

.DLL命令 设置D2D组合框回调, , "emoji_window.dll", "SetD2DComboBoxCallback", , , 设置D2D组合框选中回调
    .参数 组合框句柄, 整数型
    .参数 回调函数, 子程序指针, , 子程序需 stdcall，参数：hComboBox(整数型), index(整数型)

.DLL命令 启用D2D组合框, , "emoji_window.dll", "EnableD2DComboBox", , , 启用/禁用D2D组合框
    .参数 组合框句柄, 整数型
    .参数 启用, 逻辑型

.DLL命令 显示D2D组合框, , "emoji_window.dll", "ShowD2DComboBox", , , 显示/隐藏D2D组合框
    .参数 组合框句柄, 整数型
    .参数 显示, 逻辑型

.DLL命令 设置D2D组合框位置, , "emoji_window.dll", "SetD2DComboBoxBounds", , , 设置D2D组合框位置和大小
    .参数 组合框句柄, 整数型
    .参数 X坐标, 整数型
    .参数 Y坐标, 整数型
    .参数 宽度, 整数型
    .参数 高度, 整数型


' ========== D2D日期时间选择器（Element风格弹出层，支持彩色emoji显示文本） ==========

.DLL命令 创建D2D日期时间选择器, 整数型, "emoji_window.dll", "CreateD2DDateTimePicker", , , 创建D2D日期时间选择器
    .参数 父窗口句柄, 整数型
    .参数 X坐标, 整数型
    .参数 Y坐标, 整数型
    .参数 宽度, 整数型
    .参数 高度, 整数型
    .参数 初始精度, 整数型, , 0=仅年 1=年月日 2=年月日时 3=年月日时分 4=年月日时分秒
    .参数 前景色, 整数型, , ARGB颜色（易语言勿写 #FF303133，# 后为常量名；请用 到ARGB (255,R,G,B) 等）
    .参数 背景色, 整数型, , ARGB颜色
    .参数 边框色, 整数型, , ARGB颜色
    .参数 字体名字节集指针, 整数型, , 字体名称UTF-8字节集指针
    .参数 字体名长度, 整数型, , 字体名长度（字节）
    .参数 字体大小, 整数型
    .参数 粗体, 逻辑型
    .参数 斜体, 逻辑型
    .参数 下划线, 逻辑型

.DLL命令 获取D2D日期时间选择器精度, 整数型, "emoji_window.dll", "GetD2DDateTimePickerPrecision", , , 返回当前精度（0～4，含义同创建时的初始精度）
    .参数 选择器句柄, 整数型

.DLL命令 设置D2D日期时间选择器精度, , "emoji_window.dll", "SetD2DDateTimePickerPrecision", , , 设置显示精度并规范化内部日期时间
    .参数 选择器句柄, 整数型
    .参数 精度, 整数型, , 0=仅年 1=年月日 2=年月日时 3=年月日时分 4=年月日时分秒

.DLL命令 获取D2D日期时间选择器日期时间, , "emoji_window.dll", "GetD2DDateTimePickerDateTime", , , 读取年月日时分秒；不需要的项可传0（空指针）
    .参数 选择器句柄, 整数型
    .参数 年, 整数型, , 传址 输出；可为0表示不读取
    .参数 月, 整数型, , 传址 输出；可为0
    .参数 日, 整数型, , 传址 输出；可为0
    .参数 时, 整数型, , 传址 输出；可为0
    .参数 分, 整数型, , 传址 输出；可为0
    .参数 秒, 整数型, , 传址 输出；可为0

.DLL命令 设置D2D日期时间选择器日期时间, , "emoji_window.dll", "SetD2DDateTimePickerDateTime", , , 按当前精度写入；超出部分由DLL按规则忽略或规范化
    .参数 选择器句柄, 整数型
    .参数 年, 整数型
    .参数 月, 整数型
    .参数 日, 整数型
    .参数 时, 整数型
    .参数 分, 整数型
    .参数 秒, 整数型

.DLL命令 设置D2D日期时间选择器回调, , "emoji_window.dll", "SetD2DDateTimePickerCallback", , , 值改变时回调（选择日期/时间或程序调用设置后）
    .参数 选择器句柄, 整数型
    .参数 回调函数, 子程序指针, , stdcall，参数：控件句柄(整数型)

.DLL命令 启用D2D日期时间选择器, , "emoji_window.dll", "EnableD2DDateTimePicker", , , 启用或禁用
    .参数 选择器句柄, 整数型
    .参数 启用, 逻辑型

.DLL命令 显示D2D日期时间选择器, , "emoji_window.dll", "ShowD2DDateTimePicker", , , 显示或隐藏
    .参数 选择器句柄, 整数型
    .参数 显示, 逻辑型

.DLL命令 设置D2D日期时间选择器位置, , "emoji_window.dll", "SetD2DDateTimePickerBounds", , , 设置位置和大小
    .参数 选择器句柄, 整数型
    .参数 X坐标, 整数型
    .参数 Y坐标, 整数型
    .参数 宽度, 整数型
    .参数 高度, 整数型


' ========== 热键控件功能 ==========

.DLL命令 创建热键控件, 整数型, "emoji_window.dll", "CreateHotKeyControl", , , 创建热键控件
    .参数 父窗口句柄, 整数型
    .参数 X坐标, 整数型
    .参数 Y坐标, 整数型
    .参数 宽度, 整数型
    .参数 高度, 整数型
    .参数 前景色, 整数型, , ARGB颜色
    .参数 背景色, 整数型, , ARGB颜色

.DLL命令 获取热键值, , "emoji_window.dll", "GetHotKey", , , 获取热键值（输出虚拟键码与修饰键）
    .参数 热键控件句柄, 整数型
    .参数 虚拟键码, 整数型, , 输出参数：VK_*虚拟键码（调用时请用“传址 变量”传参）
    .参数 修饰键, 整数型, , 输出参数：Ctrl=1, Shift=2, Alt=4（按位组合，调用时请用“传址 变量”传参）

.DLL命令 设置热键值, , "emoji_window.dll", "SetHotKey", , , 设置热键值
    .参数 热键控件句柄, 整数型
    .参数 虚拟键码, 整数型, , VK_*常量
    .参数 修饰键, 整数型, , Ctrl=1, Shift=2, Alt=4（按位组合）

.DLL命令 清除热键, , "emoji_window.dll", "ClearHotKey", , , 清除热键值
    .参数 热键控件句柄, 整数型

.DLL命令 设置热键回调, , "emoji_window.dll", "SetHotKeyCallback", , , 设置热键改变回调
    .参数 热键控件句柄, 整数型
    .参数 回调函数, 子程序指针, , 子程序需 stdcall，参数：hHotKey(整数型), vk_code(整数型), modifiers(整数型)

.DLL命令 启用热键控件, , "emoji_window.dll", "EnableHotKeyControl", , , 启用/禁用热键控件
    .参数 热键控件句柄, 整数型
    .参数 启用, 逻辑型

.DLL命令 显示热键控件, , "emoji_window.dll", "ShowHotKeyControl", , , 显示/隐藏热键控件
    .参数 热键控件句柄, 整数型
    .参数 显示, 逻辑型

.DLL命令 设置热键控件位置, , "emoji_window.dll", "SetHotKeyControlBounds", , , 设置热键控件位置和大小
    .参数 热键控件句柄, 整数型
    .参数 X坐标, 整数型
    .参数 Y坐标, 整数型
    .参数 宽度, 整数型
    .参数 高度, 整数型


' ========== D2D彩色Emoji编辑框功能 ==========

.DLL命令 创建D2D彩色Emoji编辑框, 整数型, "emoji_window.dll", "CreateD2DColorEmojiEditBox", , , 创建支持彩色Emoji的D2D自定义编辑框（完美支持彩色emoji）
    .参数 父窗口句柄, 整数型
    .参数 X坐标, 整数型
    .参数 Y坐标, 整数型
    .参数 宽度, 整数型
    .参数 高度, 整数型
    .参数 文本字节集指针, 整数型, , UTF-8字节集指针
    .参数 文本长度, 整数型, , 字节集长度
    .参数 前景色, 整数型, , ARGB颜色
    .参数 背景色, 整数型, , ARGB颜色
    .参数 字体名字节集指针, 整数型, , UTF-8字节集指针
    .参数 字体名长度, 整数型, , 字节集长度
    .参数 字体大小, 整数型
    .参数 粗体, 逻辑型
    .参数 斜体, 逻辑型
    .参数 下划线, 逻辑型
    .参数 对齐方式, 整数型, , 0=左 1=中 2=右
    .参数 多行模式, 逻辑型
    .参数 只读模式, 逻辑型
    .参数 密码框, 逻辑型
    .参数 显示边框, 逻辑型
    .参数 文本垂直居中, 逻辑型, , 仅单行有效

.DLL命令 获取D2D编辑框文本, 整数型, "emoji_window.dll", "GetD2DEditBoxText", , , 获取D2D编辑框文本（返回UTF-8字节数）
    .参数 编辑框句柄, 整数型
    .参数 缓冲区指针, 整数型, , UTF-8字节集指针（可为0获取所需大小）
    .参数 缓冲区大小, 整数型, , 字节集大小

.DLL命令 设置D2D编辑框文本, , "emoji_window.dll", "SetD2DEditBoxText", , , 设置D2D编辑框文本
    .参数 编辑框句柄, 整数型
    .参数 文本字节集指针, 整数型, , UTF-8字节集指针
    .参数 文本长度, 整数型, , 字节集长度

.DLL命令 设置D2D编辑框按键回调, , "emoji_window.dll", "SetD2DEditBoxKeyCallback", , , 设置D2D编辑框按键回调
    .参数 编辑框句柄, 整数型
    .参数 回调子程序指针, 整数型, , 子程序需 stdcall，参数：hEdit, key_code, key_down, shift, ctrl, alt（均为整数型）

.DLL命令 启用D2D编辑框, , "emoji_window.dll", "EnableD2DEditBox", , , 启用/禁用D2D编辑框
    .参数 编辑框句柄, 整数型
    .参数 启用, 逻辑型

.DLL命令 显示D2D编辑框, , "emoji_window.dll", "ShowD2DEditBox", , , 显示/隐藏D2D编辑框
    .参数 编辑框句柄, 整数型
    .参数 显示, 逻辑型

.DLL命令 设置D2D编辑框位置, , "emoji_window.dll", "SetD2DEditBoxBounds", , , 设置D2D编辑框位置和大小
    .参数 编辑框句柄, 整数型
    .参数 X坐标, 整数型
    .参数 Y坐标, 整数型
    .参数 宽度, 整数型
    .参数 高度, 整数型

.DLL命令 设置D2D编辑框字体, , "emoji_window.dll", "SetD2DEditBoxFont", , , 设置D2D编辑框字体
    .参数 编辑框句柄, 整数型
    .参数 字体名字节集指针, 整数型, , UTF-8字节集指针
    .参数 字体名长度, 整数型, , 字节集长度
    .参数 字体大小, 整数型
    .参数 粗体, 逻辑型
    .参数 斜体, 逻辑型
    .参数 下划线, 逻辑型

.DLL命令 设置D2D编辑框颜色, , "emoji_window.dll", "SetD2DEditBoxColor", , , 设置D2D编辑框颜色
    .参数 编辑框句柄, 整数型
    .参数 前景色, 整数型, , ARGB颜色
    .参数 背景色, 整数型, , ARGB颜色


' ========== DataGridView表格控件功能 ==========

.DLL命令 创建DataGridView, 整数型, "emoji_window.dll", "CreateDataGridView", , , 创建DataGridView表格控件
    .参数 父窗口句柄, 整数型
    .参数 X坐标, 整数型
    .参数 Y坐标, 整数型
    .参数 宽度, 整数型
    .参数 高度, 整数型
    .参数 虚拟模式, 逻辑型, , 是否启用虚拟模式（大数据量）
    .参数 隔行变色, 逻辑型, , 是否启用隔行变色
    .参数 前景色, 整数型, , ARGB颜色
    .参数 背景色, 整数型, , ARGB颜色

' --- 列管理 ---

.DLL命令 表格_添加文本列, 整数型, "emoji_window.dll", "DataGrid_AddTextColumn", , , 添加文本列，返回列索引
    .参数 表格句柄, 整数型
    .参数 列头字节集指针, 整数型, , UTF-8字节集指针
    .参数 列头长度, 整数型, , 字节集长度
    .参数 列宽, 整数型, , 列宽（像素）

.DLL命令 表格_添加复选框列, 整数型, "emoji_window.dll", "DataGrid_AddCheckBoxColumn", , , 添加复选框列，返回列索引
    .参数 表格句柄, 整数型
    .参数 列头字节集指针, 整数型, , UTF-8字节集指针
    .参数 列头长度, 整数型, , 字节集长度
    .参数 列宽, 整数型, , 列宽（像素）

.DLL命令 表格_添加按钮列, 整数型, "emoji_window.dll", "DataGrid_AddButtonColumn", , , 添加按钮列，返回列索引
    .参数 表格句柄, 整数型
    .参数 列头字节集指针, 整数型, , UTF-8字节集指针
    .参数 列头长度, 整数型, , 字节集长度
    .参数 列宽, 整数型, , 列宽（像素）

.DLL命令 表格_添加链接列, 整数型, "emoji_window.dll", "DataGrid_AddLinkColumn", , , 添加链接列，返回列索引
    .参数 表格句柄, 整数型
    .参数 列头字节集指针, 整数型, , UTF-8字节集指针
    .参数 列头长度, 整数型, , 字节集长度
    .参数 列宽, 整数型, , 列宽（像素）

.DLL命令 表格_添加图片列, 整数型, "emoji_window.dll", "DataGrid_AddImageColumn", , , 添加图片列，返回列索引
    .参数 表格句柄, 整数型
    .参数 列头字节集指针, 整数型, , UTF-8字节集指针
    .参数 列头长度, 整数型, , 字节集长度
    .参数 列宽, 整数型, , 列宽（像素）

.DLL命令 表格_添加下拉框列, 整数型, "emoji_window.dll", "DataGrid_AddComboBoxColumn", , , 添加下拉框列，返回列索引
    .参数 表格句柄, 整数型
    .参数 列头字节集指针, 整数型, , UTF-8字节集指针
    .参数 列头长度, 整数型, , 字节集长度
    .参数 列宽, 整数型, , 列宽（像素）

.DLL命令 表格_添加标签列, 整数型, "emoji_window.dll", "DataGrid_AddTagColumn", , , 添加标签列，返回列索引
    .参数 表格句柄, 整数型
    .参数 列头字节集指针, 整数型, , UTF-8字节集指针
    .参数 列头长度, 整数型, , 字节集长度
    .参数 列宽, 整数型, , 列宽（像素）

.DLL命令 表格_添加进度列, 整数型, "emoji_window.dll", "DataGrid_AddProgressColumn", , , 添加进度条列，返回列索引
    .参数 表格句柄, 整数型
    .参数 列头字节集指针, 整数型, , UTF-8字节集指针
    .参数 列头长度, 整数型, , 字节集长度
    .参数 列宽, 整数型, , 列宽（像素）

.DLL命令 表格_添加Switch列, 整数型, "emoji_window.dll", "DataGrid_AddSwitchColumn", , , 添加 Switch 列，返回列索引
    .参数 表格句柄, 整数型
    .参数 列头字节集指针, 整数型, , UTF-8字节集指针
    .参数 列头长度, 整数型, , 字节集长度
    .参数 列宽, 整数型, , 列宽（像素）

.DLL命令 表格_移除列, , "emoji_window.dll", "DataGrid_RemoveColumn", , , 移除指定列
    .参数 表格句柄, 整数型
    .参数 列索引, 整数型

.DLL命令 表格_清空所有列, , "emoji_window.dll", "DataGrid_ClearColumns", , , 清空所有列
    .参数 表格句柄, 整数型

.DLL命令 表格_获取列数, 整数型, "emoji_window.dll", "DataGrid_GetColumnCount", , , 获取列数
    .参数 表格句柄, 整数型

.DLL命令 表格_设置列宽, , "emoji_window.dll", "DataGrid_SetColumnWidth", , , 设置列宽
    .参数 表格句柄, 整数型
    .参数 列索引, 整数型
    .参数 列宽, 整数型, , 列宽（像素）

.DLL命令 表格_设置列头文本, , "emoji_window.dll", "DataGrid_SetColumnHeaderText", , , 设置列头文本
    .参数 表格句柄, 整数型
    .参数 列索引, 整数型
    .参数 列头字节集指针, 整数型, , UTF-8字节集指针
    .参数 列头长度, 整数型, , 字节集长度

.DLL命令 表格_获取列头文本, 整数型, "emoji_window.dll", "DataGrid_GetColumnHeaderText", , , 获取列头文本，返回实际字节数
    .参数 表格句柄, 整数型
    .参数 列索引, 整数型
    .参数 缓冲区指针, 整数型, , UTF-8缓冲区指针
    .参数 缓冲区大小, 整数型, , 缓冲区大小

' --- 行管理 ---

.DLL命令 表格_添加行, 整数型, "emoji_window.dll", "DataGrid_AddRow", , , 添加行，返回行索引
    .参数 表格句柄, 整数型

.DLL命令 表格_移除行, , "emoji_window.dll", "DataGrid_RemoveRow", , , 移除指定行
    .参数 表格句柄, 整数型
    .参数 行索引, 整数型

.DLL命令 表格_清空所有行, , "emoji_window.dll", "DataGrid_ClearRows", , , 清空所有行
    .参数 表格句柄, 整数型

.DLL命令 表格_获取行数, 整数型, "emoji_window.dll", "DataGrid_GetRowCount", , , 获取行数
    .参数 表格句柄, 整数型

' --- 单元格操作 ---

.DLL命令 表格_设置单元格文本, , "emoji_window.dll", "DataGrid_SetCellText", , , 设置单元格文本
    .参数 表格句柄, 整数型
    .参数 行索引, 整数型
    .参数 列索引, 整数型
    .参数 文本字节集指针, 整数型, , UTF-8字节集指针
    .参数 文本长度, 整数型, , 字节集长度

.DLL命令 表格_获取单元格文本, 整数型, "emoji_window.dll", "DataGrid_GetCellText", , , 获取单元格文本，返回实际字节数
    .参数 表格句柄, 整数型
    .参数 行索引, 整数型
    .参数 列索引, 整数型
    .参数 缓冲区指针, 整数型, , UTF-8缓冲区指针
    .参数 缓冲区大小, 整数型, , 缓冲区大小

.DLL命令 表格_从文件设置单元格图片, 逻辑型, "emoji_window.dll", "DataGrid_SetCellImageFromFile", , , 从文件设置图片单元格
    .参数 表格句柄, 整数型
    .参数 行索引, 整数型
    .参数 列索引, 整数型
    .参数 文件路径字节集指针, 整数型, , UTF-8文件路径字节集指针
    .参数 路径长度, 整数型, , 字节集长度

.DLL命令 表格_从内存设置单元格图片, 逻辑型, "emoji_window.dll", "DataGrid_SetCellImageFromMemory", , , 从内存设置图片单元格
    .参数 表格句柄, 整数型
    .参数 行索引, 整数型
    .参数 列索引, 整数型
    .参数 图片数据指针, 整数型, , 图片字节集指针
    .参数 数据长度, 整数型, , 字节集长度

.DLL命令 表格_清除单元格图片, , "emoji_window.dll", "DataGrid_ClearCellImage", , , 清除图片单元格
    .参数 表格句柄, 整数型
    .参数 行索引, 整数型
    .参数 列索引, 整数型

.DLL命令 表格_设置单元格进度, , "emoji_window.dll", "DataGrid_SetCellProgress", , , 设置进度条单元格百分比
    .参数 表格句柄, 整数型
    .参数 行索引, 整数型
    .参数 列索引, 整数型
    .参数 进度值, 整数型, , 0 到 100 的百分比

.DLL命令 表格_获取单元格进度, 整数型, "emoji_window.dll", "DataGrid_GetCellProgress", , , 获取进度条单元格百分比
    .参数 表格句柄, 整数型
    .参数 行索引, 整数型
    .参数 列索引, 整数型

.DLL命令 表格_设置单元格复选状态, , "emoji_window.dll", "DataGrid_SetCellChecked", , , 设置单元格复选框状态
    .参数 表格句柄, 整数型
    .参数 行索引, 整数型
    .参数 列索引, 整数型
    .参数 选中状态, 逻辑型

.DLL命令 表格_获取单元格复选状态, 逻辑型, "emoji_window.dll", "DataGrid_GetCellChecked", , , 获取单元格复选框状态
    .参数 表格句柄, 整数型
    .参数 行索引, 整数型
    .参数 列索引, 整数型

.DLL命令 表格_设置单元格样式, , "emoji_window.dll", "DataGrid_SetCellStyle", , , 设置单元格样式
    .参数 表格句柄, 整数型
    .参数 行索引, 整数型
    .参数 列索引, 整数型
    .参数 前景色, 整数型, , ARGB颜色（0=默认）
    .参数 背景色, 整数型, , ARGB颜色（0=默认）
    .参数 粗体, 逻辑型
    .参数 斜体, 逻辑型

.DLL命令 表格_设置行样式, , "emoji_window.dll", "DataGrid_SetRowStyle", , , 设置整行默认样式
    .参数 表格句柄, 整数型
    .参数 行索引, 整数型
    .参数 前景色, 整数型, , ARGB颜色（0=默认）
    .参数 背景色, 整数型, , ARGB颜色（0=默认）
    .参数 粗体, 逻辑型
    .参数 斜体, 逻辑型

.DLL命令 表格_查找文本, 逻辑型, "emoji_window.dll", "DataGrid_FindText", , , 在表格中查找文本并回写命中的行列
    .参数 表格句柄, 整数型
    .参数 查找文本字节集指针, 整数型, , UTF-8字节集指针
    .参数 查找文本长度, 整数型, , 字节集长度
    .参数 起始行, 整数型, , 从该行开始查找
    .参数 起始列, 整数型, , 从该列开始查找
    .参数 区分大小写, 逻辑型
    .参数 整格匹配, 逻辑型
    .参数 环绕查找, 逻辑型, , 是否在结尾后回到开头继续查找
    .参数 找到行, 整数型, 传址, 输出参数：找到的行索引（调用时请用"传址 变量"传参）
    .参数 找到列, 整数型, 传址, 输出参数：找到的列索引（调用时请用"传址 变量"传参）

' --- 选择和导航 ---

.DLL命令 表格_获取选中行, 整数型, "emoji_window.dll", "DataGrid_GetSelectedRow", , , 获取选中行索引（-1=无）
    .参数 表格句柄, 整数型

.DLL命令 表格_获取选中列, 整数型, "emoji_window.dll", "DataGrid_GetSelectedCol", , , 获取选中列索引（-1=无）
    .参数 表格句柄, 整数型

.DLL命令 表格_设置选中单元格, , "emoji_window.dll", "DataGrid_SetSelectedCell", , , 设置选中单元格
    .参数 表格句柄, 整数型
    .参数 行索引, 整数型
    .参数 列索引, 整数型

.DLL命令 表格_设置选择模式, , "emoji_window.dll", "DataGrid_SetSelectionMode", , , 设置选择模式
    .参数 表格句柄, 整数型
    .参数 模式, 整数型, , 0=单元格选择 1=整行选择

' --- 排序 ---

.DLL命令 表格_按列排序, , "emoji_window.dll", "DataGrid_SortByColumn", , , 按列排序
    .参数 表格句柄, 整数型
    .参数 列索引, 整数型
    .参数 排序方向, 整数型, , 0=无 1=升序 2=降序

' --- 冻结 ---

.DLL命令 表格_设置冻结首行, , "emoji_window.dll", "DataGrid_SetFreezeHeader", , , 设置冻结首行（列头）
    .参数 表格句柄, 整数型
    .参数 冻结, 逻辑型

.DLL命令 表格_设置冻结首列, , "emoji_window.dll", "DataGrid_SetFreezeFirstColumn", , , 设置冻结首列
    .参数 表格句柄, 整数型
    .参数 冻结, 逻辑型

.DLL命令 表格_设置冻结列数, , "emoji_window.dll", "DataGrid_SetFreezeColumnCount", , , 设置左侧冻结列数
    .参数 表格句柄, 整数型
    .参数 列数, 整数型

.DLL命令 表格_设置冻结行数, , "emoji_window.dll", "DataGrid_SetFreezeRowCount", , , 设置顶部冻结行数（不含列头）
    .参数 表格句柄, 整数型
    .参数 行数, 整数型

' 兼容说明：表格_设置冻结首列 仍可继续使用，内部等价于“表格_设置冻结列数(表格句柄, 1/0)”
' 推荐用法：需要冻结多列/多行时，优先使用“表格_设置冻结列数 / 表格_设置冻结行数”

' --- 虚拟模式 ---

.DLL命令 表格_设置虚拟行数, , "emoji_window.dll", "DataGrid_SetVirtualRowCount", , , 设置虚拟模式总行数
    .参数 表格句柄, 整数型
    .参数 行数, 整数型

.DLL命令 表格_设置虚拟数据回调, , "emoji_window.dll", "DataGrid_SetVirtualDataCallback", , , 设置虚拟模式数据请求回调
    .参数 表格句柄, 整数型
    .参数 回调函数, 子程序指针, , stdcall: (hGrid, row, col, buffer, buffer_size) -> 返回写入字节数

' --- 外观 ---

.DLL命令 表格_设置显示网格线, , "emoji_window.dll", "DataGrid_SetShowGridLines", , , 设置是否显示网格线
    .参数 表格句柄, 整数型
    .参数 显示, 逻辑型

.DLL命令 表格_设置默认行高, , "emoji_window.dll", "DataGrid_SetDefaultRowHeight", , , 设置默认行高
    .参数 表格句柄, 整数型
    .参数 行高, 整数型

.DLL命令 表格_设置列头高度, , "emoji_window.dll", "DataGrid_SetHeaderHeight", , , 设置列头高度
    .参数 表格句柄, 整数型
    .参数 高度, 整数型

.DLL命令 表格_设置表头多行, , "emoji_window.dll", "DataGrid_SetHeaderMultiline", , , 设置表头是否允许多行显示
    .参数 表格句柄, 整数型
    .参数 启用, 逻辑型

.DLL命令 表格_设置表头样式, , "emoji_window.dll", "DataGrid_SetHeaderStyle", , , 设置表头样式（0=Plain 1=Dark 2=Bordered）
    .参数 表格句柄, 整数型
    .参数 样式, 整数型, , 0=Plain 1=Dark 2=Bordered

.DLL命令 表格_设置列下拉项, , "emoji_window.dll", "DataGrid_SetColumnComboItems", , , 设置下拉框列的候选项（多项请用换行符分隔）
    .参数 表格句柄, 整数型
    .参数 列索引, 整数型
    .参数 项目字节集指针, 整数型, , UTF-8字节集指针
    .参数 项目长度, 整数型, , 字节集长度

' --- 事件回调 ---

.DLL命令 表格_设置单元格点击回调, , "emoji_window.dll", "DataGrid_SetCellClickCallback", , , 设置单元格点击回调
    .参数 表格句柄, 整数型
    .参数 回调函数, 子程序指针, , stdcall: (hGrid, row, col)

.DLL命令 表格_设置单元格双击回调, , "emoji_window.dll", "DataGrid_SetCellDoubleClickCallback", , , 设置单元格双击回调
    .参数 表格句柄, 整数型
    .参数 回调函数, 子程序指针, , stdcall: (hGrid, row, col)

.DLL命令 表格_设置单元格值改变回调, , "emoji_window.dll", "DataGrid_SetCellValueChangedCallback", , , 设置单元格值改变回调
    .参数 表格句柄, 整数型
    .参数 回调函数, 子程序指针, , stdcall: (hGrid, row, col)

.DLL命令 表格_设置列头点击回调, , "emoji_window.dll", "DataGrid_SetColumnHeaderClickCallback", , , 设置列头点击回调
    .参数 表格句柄, 整数型
    .参数 回调函数, 子程序指针, , stdcall: (hGrid, col)

.DLL命令 表格_设置选择改变回调, , "emoji_window.dll", "DataGrid_SetSelectionChangedCallback", , , 设置选择改变回调
    .参数 表格句柄, 整数型
    .参数 回调函数, 子程序指针, , stdcall: (hGrid, row, col)

' --- 其他 ---

.DLL命令 表格_设置列头对齐方式, , "emoji_window.dll", "DataGrid_SetColumnHeaderAlignment", , , 设置列头对齐方式 (0=左对齐, 1=居中, 2=右对齐)
    .参数 表格句柄, 整数型
    .参数 列索引, 整数型
    .参数 对齐方式, 整数型, , 0=左对齐, 1=居中, 2=右对齐

.DLL命令 表格_设置列单元格对齐方式, , "emoji_window.dll", "DataGrid_SetColumnCellAlignment", , , 设置列单元格对齐方式 (0=左对齐, 1=居中, 2=右对齐)
    .参数 表格句柄, 整数型
    .参数 列索引, 整数型
    .参数 对齐方式, 整数型, , 0=左对齐, 1=居中, 2=右对齐

.DLL命令 表格_设置双击启用, , "emoji_window.dll", "DataGrid_SetDoubleClickEnabled", , , 设置是否启用单元格双击事件和双击编辑
    .参数 表格句柄, 整数型
    .参数 启用, 逻辑型

.DLL命令 表格_启用, , "emoji_window.dll", "DataGrid_Enable", , , 启用/禁用DataGridView
    .参数 表格句柄, 整数型
    .参数 启用, 逻辑型

.DLL命令 表格_显示, , "emoji_window.dll", "DataGrid_Show", , , 显示/隐藏DataGridView
    .参数 表格句柄, 整数型
    .参数 显示, 逻辑型

.DLL命令 表格_设置位置, , "emoji_window.dll", "DataGrid_SetBounds", , , 设置DataGridView位置和大小
    .参数 表格句柄, 整数型
    .参数 X坐标, 整数型
    .参数 Y坐标, 整数型
    .参数 宽度, 整数型
    .参数 高度, 整数型

.DLL命令 表格_刷新, , "emoji_window.dll", "DataGrid_Refresh", , , 刷新DataGridView
    .参数 表格句柄, 整数型

.DLL命令 表格_设置颜色, , "emoji_window.dll", "DataGrid_SetColors", , , 设置DataGridView主题颜色
    .参数 表格句柄, 整数型
    .参数 前景色, 整数型, , 文本颜色ARGB
    .参数 背景色, 整数型, , 背景颜色ARGB
    .参数 表头背景色, 整数型, , 表头背景ARGB
    .参数 表头前景色, 整数型, , 表头文字ARGB
    .参数 选中色, 整数型, , 选中高亮ARGB
    .参数 悬停色, 整数型, , 悬停高亮ARGB
    .参数 网格线颜色, 整数型, , 网格线ARGB

.DLL命令 表格_获取颜色, 整数型, "emoji_window.dll", "DataGrid_GetColors", , , 获取DataGridView主题颜色，返回0表示成功
    .参数 表格句柄, 整数型
    .参数 前景色, 整数型, 传址, 输出参数：文本颜色ARGB（调用时请用"传址 变量"传参）
    .参数 背景色, 整数型, 传址, 输出参数：背景颜色ARGB（调用时请用"传址 变量"传参）
    .参数 表头背景色, 整数型, 传址, 输出参数：表头背景ARGB（调用时请用"传址 变量"传参）
    .参数 表头前景色, 整数型, 传址, 输出参数：表头文字ARGB（调用时请用"传址 变量"传参）
    .参数 选中色, 整数型, 传址, 输出参数：选中高亮ARGB（调用时请用"传址 变量"传参）
    .参数 悬停色, 整数型, 传址, 输出参数：悬停高亮ARGB（调用时请用"传址 变量"传参）
    .参数 网格线颜色, 整数型, 传址, 输出参数：网格线ARGB（调用时请用"传址 变量"传参）

.DLL命令 表格_导出CSV, 逻辑型, "emoji_window.dll", "DataGrid_ExportCSV", , , 导出CSV文件
    .参数 表格句柄, 整数型
    .参数 文件路径字节集指针, 整数型, , UTF-8文件路径字节集指针
    .参数 路径长度, 整数型, , 字节集长度

' ========== 扩展事件回调系统 ==========

.DLL命令 设置鼠标进入回调, , "emoji_window.dll", "SetMouseEnterCallback", , , 设置鼠标进入控件时的回调函数
    .参数 控件句柄, 整数型, , 任意控件的窗口句柄
    .参数 回调函数, 子程序指针, , stdcall: (hwnd)

.DLL命令 设置鼠标离开回调, , "emoji_window.dll", "SetMouseLeaveCallback", , , 设置鼠标离开控件时的回调函数
    .参数 控件句柄, 整数型, , 任意控件的窗口句柄
    .参数 回调函数, 子程序指针, , stdcall: (hwnd)

.DLL命令 设置双击回调, , "emoji_window.dll", "SetDoubleClickCallback", , , 设置鼠标双击控件时的回调函数
    .参数 控件句柄, 整数型, , 任意控件的窗口句柄
    .参数 回调函数, 子程序指针, , stdcall: (hwnd, x, y)

.DLL命令 设置右键点击回调, , "emoji_window.dll", "SetRightClickCallback", , , 设置鼠标右键点击控件时的回调函数
    .参数 控件句柄, 整数型, , 任意控件的窗口句柄
    .参数 回调函数, 子程序指针, , stdcall: (hwnd, x, y)

.DLL命令 设置获得焦点回调, , "emoji_window.dll", "SetFocusCallback", , , 设置控件获得焦点时的回调函数
    .参数 控件句柄, 整数型, , 任意控件的窗口句柄
    .参数 回调函数, 子程序指针, , stdcall: (hwnd)

.DLL命令 设置失去焦点回调, , "emoji_window.dll", "SetBlurCallback", , , 设置控件失去焦点时的回调函数
    .参数 控件句柄, 整数型, , 任意控件的窗口句柄
    .参数 回调函数, 子程序指针, , stdcall: (hwnd)

.DLL命令 设置按键按下回调, , "emoji_window.dll", "SetKeyDownCallback", , , 设置键盘按键按下时的回调函数
    .参数 控件句柄, 整数型, , 任意控件的窗口句柄
    .参数 回调函数, 子程序指针, , stdcall: (hwnd, vk_code, shift, ctrl, alt)

.DLL命令 设置按键松开回调, , "emoji_window.dll", "SetKeyUpCallback", , , 设置键盘按键松开时的回调函数
    .参数 控件句柄, 整数型, , 任意控件的窗口句柄
    .参数 回调函数, 子程序指针, , stdcall: (hwnd, vk_code, shift, ctrl, alt)

.DLL命令 设置字符输入回调, , "emoji_window.dll", "SetCharCallback", , , 设置字符输入时的回调函数
    .参数 控件句柄, 整数型, , 任意控件的窗口句柄
    .参数 回调函数, 子程序指针, , stdcall: (hwnd, char_code)

.DLL命令 设置值改变回调, , "emoji_window.dll", "SetValueChangedCallback", , , 设置控件值改变时的回调函数
    .参数 控件句柄, 整数型, , 任意控件的窗口句柄
    .参数 回调函数, 子程序指针, , stdcall: (hwnd)

' ========== 布局管理器 ==========

.DLL命令 设置布局管理器, , "emoji_window.dll", "SetLayoutManager", , , 为窗口设置布局管理器类型（0=无 1=水平流式 2=垂直流式 3=网格 4=停靠）
    .参数 父窗口句柄, 整数型, , 要设置布局的窗口句柄
    .参数 布局类型, 整数型, , 0=无 1=水平流式 2=垂直流式 3=网格 4=停靠
    .参数 行数, 整数型, , 网格布局的行数（其他布局填0）
    .参数 列数, 整数型, , 网格布局的列数（其他布局填0）
    .参数 间距, 整数型, , 控件之间的间距（像素）

.DLL命令 设置布局内边距, , "emoji_window.dll", "SetLayoutPadding", , , 设置布局管理器的内边距
    .参数 父窗口句柄, 整数型, , 窗口句柄
    .参数 左边距, 整数型, , 左内边距（像素）
    .参数 上边距, 整数型, , 上内边距（像素）
    .参数 右边距, 整数型, , 右内边距（像素）
    .参数 下边距, 整数型, , 下内边距（像素）

.DLL命令 设置控件布局属性, , "emoji_window.dll", "SetControlLayoutProps", , , 设置控件在布局中的属性（边距、停靠、拉伸）
    .参数 控件句柄, 整数型, , 控件窗口句柄
    .参数 左边距, 整数型, , 控件左外边距
    .参数 上边距, 整数型, , 控件上外边距
    .参数 右边距, 整数型, , 控件右外边距
    .参数 下边距, 整数型, , 控件下外边距
    .参数 停靠位置, 整数型, , 0=无 1=上 2=下 3=左 4=右 5=填充
    .参数 水平拉伸, 逻辑型, , 是否水平拉伸
    .参数 垂直拉伸, 逻辑型, , 是否垂直拉伸

.DLL命令 添加控件到布局, , "emoji_window.dll", "AddControlToLayout", , , 将控件添加到布局管理器
    .参数 父窗口句柄, 整数型, , 窗口句柄
    .参数 控件句柄, 整数型, , 要添加的控件句柄

.DLL命令 从布局移除控件, , "emoji_window.dll", "RemoveControlFromLayout", , , 从布局管理器中移除控件
    .参数 父窗口句柄, 整数型, , 窗口句柄
    .参数 控件句柄, 整数型, , 要移除的控件句柄

.DLL命令 更新布局, , "emoji_window.dll", "UpdateLayout", , , 立即重新计算并应用布局
    .参数 父窗口句柄, 整数型, , 窗口句柄

.DLL命令 移除布局管理器, , "emoji_window.dll", "RemoveLayoutManager", , , 移除窗口的布局管理器
    .参数 父窗口句柄, 整数型, , 窗口句柄

' ========== 主题系统 API ==========

.DLL命令 从JSON加载主题, 逻辑型, "emoji_window.dll", "LoadThemeFromJSON", , , 从JSON字符串加载主题配置
    .参数 JSON字节集指针, 整数型, , UTF-8 JSON字节集指针
    .参数 JSON长度, 整数型, , JSON字节集长度

.DLL命令 从文件加载主题, 逻辑型, "emoji_window.dll", "LoadThemeFromFile", , , 从JSON文件加载主题配置
    .参数 文件路径字节集指针, 整数型, , UTF-8文件路径字节集指针
    .参数 路径长度, 整数型, , 路径字节集长度

.DLL命令 设置主题, , "emoji_window.dll", "SetTheme", , , 切换到指定主题（light或dark）
    .参数 主题名称字节集指针, 整数型, , UTF-8主题名称字节集指针
    .参数 名称长度, 整数型, , 名称字节集长度

.DLL命令 设置暗色模式, , "emoji_window.dll", "SetDarkMode", , , 切换亮色/暗色模式
    .参数 暗色模式, 逻辑型, , 真=暗色 假=亮色

.DLL命令 取主题颜色, 整数型, "emoji_window.dll", "EW_GetThemeColor", , , 获取主题颜色（ARGB格式）
    .参数 颜色名称字节集指针, 整数型, , UTF-8颜色名称（如primary、success等）
    .参数 名称长度, 整数型, , 名称字节集长度

.DLL命令 按索引取主题颜色, 整数型, "emoji_window.dll", "EW_GetThemeColorByIndex", , , 按索引获取主题颜色（ARGB格式）
    .参数 颜色索引, 整数型, , 0=主色 1=成功 2=警告 3=危险 4=信息 5-8=文本色 9-12=边框色 13-14=背景色

.DLL命令 取主题字体, 整数型, "emoji_window.dll", "EW_GetThemeFont", , , 获取主题字体名称（返回UTF-8字节数）
    .参数 字体类型, 整数型, , 0=标题 1=正文 2=等宽
    .参数 缓冲区指针, 整数型, , 接收字体名称的缓冲区
    .参数 缓冲区大小, 整数型, , 缓冲区大小

.DLL命令 取主题字号, 整数型, "emoji_window.dll", "GetThemeFontSize", , , 获取主题字号
    .参数 字体类型, 整数型, , 0=标题 1=正文 2=小号

.DLL命令 取主题尺寸, 整数型, "emoji_window.dll", "GetThemeSize", , , 获取主题尺寸值
    .参数 尺寸类型, 整数型, , 0=圆角半径 1=边框宽度 2=控件高度 3=小间距 4=中间距 5=大间距

.DLL命令 是否暗色模式, 逻辑型, "emoji_window.dll", "IsDarkMode", , , 获取当前是否暗色模式

.DLL命令 取当前主题名称, 整数型, "emoji_window.dll", "EW_GetCurrentThemeName", , , 获取当前主题名称（返回UTF-8字节数）
    .参数 缓冲区指针, 整数型, , 接收主题名称的缓冲区
    .参数 缓冲区大小, 整数型, , 缓冲区大小

.DLL命令 设置主题切换回调, , "emoji_window.dll", "SetThemeChangedCallback", , , 设置主题切换时的回调函数
    .参数 回调函数, 子程序指针, , 主题切换回调（参数：主题名称指针）

' ========== 复选框属性获取命令 ==========

.DLL命令 获取复选框文本, 整数型, "emoji_window.dll", "GetCheckBoxText", , , 获取复选框文本（返回UTF-8字节数，缓冲区指针为0时返回所需大小）
    .参数 复选框句柄, 整数型, , 复选框窗口句柄
    .参数 缓冲区指针, 整数型, , UTF-8缓冲区指针（可为0获取所需大小）
    .参数 缓冲区大小, 整数型, , 缓冲区大小

.DLL命令 设置复选框字体, , "emoji_window.dll", "SetCheckBoxFont", , , 设置复选框字体
    .参数 复选框句柄, 整数型, , 复选框窗口句柄
    .参数 字体名字节集指针, 整数型, , UTF-8字体名字节集指针
    .参数 字体名长度, 整数型, , 字节集长度
    .参数 字体大小, 整数型, , 字体大小（0=不修改）
    .参数 粗体, 整数型, , 1=粗体，0=非粗体
    .参数 斜体, 整数型, , 1=斜体，0=非斜体
    .参数 下划线, 整数型, , 1=下划线，0=无下划线

.DLL命令 获取复选框字体, 整数型, "emoji_window.dll", "GetCheckBoxFont", , , 获取复选框字体信息（返回字体名UTF-8字节数，缓冲区指针为0时返回所需大小）
    .参数 复选框句柄, 整数型, , 复选框窗口句柄
    .参数 字体名缓冲区指针, 整数型, , UTF-8字体名缓冲区指针（可为0获取所需大小）
    .参数 字体名缓冲区大小, 整数型, , 缓冲区大小
    .参数 字体大小, 整数型, 传址, 输出参数：字体大小（调用时请用"传址 变量"传参）
    .参数 粗体, 整数型, 传址, 输出参数：是否粗体（1=是，0=否，调用时请用"传址 变量"传参）
    .参数 斜体, 整数型, 传址, 输出参数：是否斜体（1=是，0=否，调用时请用"传址 变量"传参）
    .参数 下划线, 整数型, 传址, 输出参数：是否下划线（1=是，0=否，调用时请用"传址 变量"传参）

.DLL命令 设置复选框颜色, , "emoji_window.dll", "SetCheckBoxColor", , , 设置复选框颜色
    .参数 复选框句柄, 整数型, , 复选框窗口句柄
    .参数 前景色, 整数型, , ARGB颜色
    .参数 背景色, 整数型, , ARGB颜色

.DLL命令 获取复选框颜色, 整数型, "emoji_window.dll", "GetCheckBoxColor", , , 获取复选框颜色（成功返回0，失败返回-1）
    .参数 复选框句柄, 整数型, , 复选框窗口句柄
    .参数 前景色, 整数型, 传址, 输出参数：前景色ARGB（调用时请用"传址 变量"传参）
    .参数 背景色, 整数型, 传址, 输出参数：背景色ARGB（调用时请用"传址 变量"传参）

' ========== 单选按钮属性获取命令 ==========

.DLL命令 获取单选按钮文本, 整数型, "emoji_window.dll", "GetRadioButtonText", , , 获取单选按钮文本（返回UTF-8字节数，缓冲区指针为0时返回所需大小）
    .参数 单选按钮句柄, 整数型, , 单选按钮窗口句柄
    .参数 缓冲区指针, 整数型, , UTF-8缓冲区指针（可为0获取所需大小）
    .参数 缓冲区大小, 整数型, , 缓冲区大小

.DLL命令 设置单选按钮字体, , "emoji_window.dll", "SetRadioButtonFont", , , 设置单选按钮字体
    .参数 单选按钮句柄, 整数型, , 单选按钮窗口句柄
    .参数 字体名字节集指针, 整数型, , UTF-8字体名字节集指针
    .参数 字体名长度, 整数型, , 字节集长度
    .参数 字体大小, 整数型, , 字体大小（0=不修改）
    .参数 粗体, 整数型, , 1=粗体，0=非粗体
    .参数 斜体, 整数型, , 1=斜体，0=非斜体
    .参数 下划线, 整数型, , 1=下划线，0=无下划线

.DLL命令 获取单选按钮字体, 整数型, "emoji_window.dll", "GetRadioButtonFont", , , 获取单选按钮字体信息（返回字体名UTF-8字节数，缓冲区指针为0时返回所需大小）
    .参数 单选按钮句柄, 整数型, , 单选按钮窗口句柄
    .参数 字体名缓冲区指针, 整数型, , UTF-8字体名缓冲区指针（可为0获取所需大小）
    .参数 字体名缓冲区大小, 整数型, , 缓冲区大小
    .参数 字体大小, 整数型, 传址, 输出参数：字体大小（调用时请用"传址 变量"传参）
    .参数 粗体, 整数型, 传址, 输出参数：是否粗体（1=是，0=否，调用时请用"传址 变量"传参）
    .参数 斜体, 整数型, 传址, 输出参数：是否斜体（1=是，0=否，调用时请用"传址 变量"传参）
    .参数 下划线, 整数型, 传址, 输出参数：是否下划线（1=是，0=否，调用时请用"传址 变量"传参）

.DLL命令 设置单选按钮颜色, , "emoji_window.dll", "SetRadioButtonColor", , , 设置单选按钮颜色
    .参数 单选按钮句柄, 整数型, , 单选按钮窗口句柄
    .参数 前景色, 整数型, , ARGB颜色
    .参数 背景色, 整数型, , ARGB颜色

.DLL命令 获取单选按钮颜色, 整数型, "emoji_window.dll", "GetRadioButtonColor", , , 获取单选按钮颜色（成功返回0，失败返回-1）
    .参数 单选按钮句柄, 整数型, , 单选按钮窗口句柄
    .参数 前景色, 整数型, 传址, 输出参数：前景色ARGB（调用时请用"传址 变量"传参）
    .参数 背景色, 整数型, 传址, 输出参数：背景色ARGB（调用时请用"传址 变量"传参）

' ========== 分组框属性获取命令 ==========

.DLL命令 获取分组框标题, 整数型, "emoji_window.dll", "GetGroupBoxTitle", , , 获取分组框标题（返回UTF-8字节数，缓冲区指针为0时返回所需大小）
    .参数 分组框句柄, 整数型, , 分组框窗口句柄
    .参数 缓冲区指针, 整数型, , UTF-8缓冲区指针（可为0获取所需大小）
    .参数 缓冲区大小, 整数型, , 缓冲区大小

.DLL命令 获取分组框位置, 整数型, "emoji_window.dll", "GetGroupBoxBounds", , , 获取分组框位置和大小（成功返回0，失败返回-1）
    .参数 分组框句柄, 整数型, , 分组框窗口句柄
    .参数 X坐标, 整数型, 传址, 输出参数：X坐标（调用时请用"传址 变量"传参）
    .参数 Y坐标, 整数型, 传址, 输出参数：Y坐标（调用时请用"传址 变量"传参）
    .参数 宽度, 整数型, 传址, 输出参数：宽度（调用时请用"传址 变量"传参）
    .参数 高度, 整数型, 传址, 输出参数：高度（调用时请用"传址 变量"传参）

.DLL命令 获取分组框可视状态, 整数型, "emoji_window.dll", "GetGroupBoxVisible", , , 获取分组框可视状态（1=可见，0=不可见，-1=错误）
    .参数 分组框句柄, 整数型, , 分组框窗口句柄

.DLL命令 获取分组框启用状态, 整数型, "emoji_window.dll", "GetGroupBoxEnabled", , , 获取分组框启用状态（1=启用，0=禁用，-1=错误）
    .参数 分组框句柄, 整数型, , 分组框窗口句柄

.DLL命令 获取分组框颜色, 整数型, "emoji_window.dll", "GetGroupBoxColor", , , 获取分组框颜色（成功返回0，失败返回-1）
    .参数 分组框句柄, 整数型, , 分组框窗口句柄
    .参数 边框颜色, 整数型, 传址, 输出参数：边框颜色ARGB（调用时请用"传址 变量"传参）
    .参数 背景色, 整数型, 传址, 输出参数：背景色ARGB（调用时请用"传址 变量"传参）


' ========== TabControl属性命令 ==========

.DLL命令 获取Tab页标题, 整数型, "emoji_window.dll", "GetTabTitle", , , 获取指定Tab页的标题（返回UTF-8字节数，缓冲区指针为0时返回所需大小）
    .参数 TabControl句柄, 整数型, , TabControl窗口句柄
    .参数 索引, 整数型, , Tab页索引（从0开始）
    .参数 缓冲区指针, 整数型, , UTF-8缓冲区指针（可为0获取所需大小）
    .参数 缓冲区大小, 整数型, , 缓冲区大小

.DLL命令 设置Tab页标题, 整数型, "emoji_window.dll", "SetTabTitle", , , 设置指定Tab页的标题（成功返回0，失败返回-1）
    .参数 TabControl句柄, 整数型, , TabControl窗口句柄
    .参数 索引, 整数型, , Tab页索引（从0开始）
    .参数 标题字节集指针, 整数型, , UTF-8标题字节集指针
    .参数 标题长度, 整数型, , 字节集长度

.DLL命令 获取TabControl位置, 整数型, "emoji_window.dll", "GetTabControlBounds", , , 获取TabControl位置和大小（成功返回0，失败返回-1）
    .参数 TabControl句柄, 整数型, , TabControl窗口句柄
    .参数 X坐标, 整数型, 传址, 输出参数：X坐标（调用时请用"传址 变量"传参）
    .参数 Y坐标, 整数型, 传址, 输出参数：Y坐标（调用时请用"传址 变量"传参）
    .参数 宽度, 整数型, 传址, 输出参数：宽度（调用时请用"传址 变量"传参）
    .参数 高度, 整数型, 传址, 输出参数：高度（调用时请用"传址 变量"传参）

.DLL命令 设置TabControl位置, 整数型, "emoji_window.dll", "SetTabControlBounds", , , 设置TabControl位置和大小（成功返回0，失败返回-1）
    .参数 TabControl句柄, 整数型, , TabControl窗口句柄
    .参数 X坐标, 整数型, , X坐标
    .参数 Y坐标, 整数型, , Y坐标
    .参数 宽度, 整数型, , 宽度
    .参数 高度, 整数型, , 高度

.DLL命令 获取TabControl可视状态, 整数型, "emoji_window.dll", "GetTabControlVisible", , , 获取TabControl可视状态（1=可见，0=不可见，-1=错误）
    .参数 TabControl句柄, 整数型, , TabControl窗口句柄

.DLL命令 显示隐藏TabControl, 整数型, "emoji_window.dll", "ShowTabControl", , , 显示或隐藏TabControl（成功返回0，失败返回-1）
    .参数 TabControl句柄, 整数型, , TabControl窗口句柄
    .参数 可见, 整数型, , 1=显示，0=隐藏

.DLL命令 启用禁用TabControl, 整数型, "emoji_window.dll", "EnableTabControl", , , 启用或禁用TabControl（成功返回0，失败返回-1）
    .参数 TabControl句柄, 整数型, , TabControl窗口句柄
    .参数 启用, 整数型, , 1=启用，0=禁用

' ========== TabControl增强命令（v3.0新增）==========

' ----- 外观定制 -----
.DLL命令 设置标签页尺寸, 整数型, "emoji_window.dll", "SetTabItemSize", , , 设置标签页固定宽度和高度（成功返回0，失败返回-1）
    .参数 TabControl句柄, 整数型, , TabControl窗口句柄
    .参数 宽度, 整数型, , 标签页宽度（必须大于0）
    .参数 高度, 整数型, , 标签页高度（必须大于0）

.DLL命令 设置Tab标签字体, 整数型, "emoji_window.dll", "SetTabFont", , , 设置标签页字体名称和字号（成功返回0，失败返回-1）
    .参数 TabControl句柄, 整数型, , TabControl窗口句柄
    .参数 字体名字节集指针, 整数型, , UTF-8编码的字体名称字节集指针
    .参数 字体名长度, 整数型, , 字体名称字节集长度
    .参数 字号, 小数型, , 字号大小（如 14.0）

.DLL命令 设置Tab标签颜色, 整数型, "emoji_window.dll", "SetTabColors", , , 设置选中/未选中标签页的背景色和文字色（成功返回0，失败返回-1）
    .参数 TabControl句柄, 整数型, , TabControl窗口句柄
    .参数 选中背景色, 整数型, , ARGB格式颜色值
    .参数 未选中背景色, 整数型, , ARGB格式颜色值
    .参数 选中文字色, 整数型, , ARGB格式颜色值
    .参数 未选中文字色, 整数型, , ARGB格式颜色值

.DLL命令 设置指示条颜色, 整数型, "emoji_window.dll", "SetTabIndicatorColor", , , 设置选中标签页底部指示条颜色（成功返回0，失败返回-1）
    .参数 TabControl句柄, 整数型, , TabControl窗口句柄
    .参数 颜色, 整数型, , ARGB格式颜色值

.DLL命令 设置标签内边距, 整数型, "emoji_window.dll", "SetTabPadding", , , 设置标签页水平和垂直内边距（成功返回0，失败返回-1）
    .参数 TabControl句柄, 整数型, , TabControl窗口句柄
    .参数 水平内边距, 整数型, , 水平方向内边距（>=0）
    .参数 垂直内边距, 整数型, , 垂直方向内边距（>=0）

.DLL命令 设置Tab头样式, 整数型, "emoji_window.dll", "SetTabHeaderStyle", , , 设置Tab头部样式（成功返回0，失败返回-1）
    .参数 TabControl句柄, 整数型, , TabControl窗口句柄
    .参数 样式, 整数型, , 0=默认，1=扁平，2=卡片纯色等

' ----- 单个标签页控制 -----
.DLL命令 启用禁用标签页, 整数型, "emoji_window.dll", "EnableTabItem", , , 启用或禁用单个标签页（成功返回0，失败返回-1）
    .参数 TabControl句柄, 整数型, , TabControl窗口句柄
    .参数 索引, 整数型, , 标签页索引（从0开始）
    .参数 是否启用, 整数型, , 1=启用，0=禁用

.DLL命令 获取标签页启用状态, 整数型, "emoji_window.dll", "GetTabItemEnabled", , , 获取单个标签页的启用状态（1=启用，0=禁用，-1=错误）
    .参数 TabControl句柄, 整数型, , TabControl窗口句柄
    .参数 索引, 整数型, , 标签页索引（从0开始）

.DLL命令 显示隐藏标签页, 整数型, "emoji_window.dll", "ShowTabItem", , , 显示或隐藏单个标签页（成功返回0，失败返回-1）
    .参数 TabControl句柄, 整数型, , TabControl窗口句柄
    .参数 索引, 整数型, , 标签页索引（从0开始）
    .参数 是否显示, 整数型, , 1=显示，0=隐藏

.DLL命令 设置标签页图标, 整数型, "emoji_window.dll", "SetTabItemIcon", , , 设置标签页图标PNG数据（成功返回0，失败返回-1，传0清除图标）
    .参数 TabControl句柄, 整数型, , TabControl窗口句柄
    .参数 索引, 整数型, , 标签页索引（从0开始）
    .参数 图标字节集指针, 整数型, , PNG图片字节数据指针（传0清除图标）
    .参数 图标长度, 整数型, , 图片字节数据长度（传0清除图标）

' ----- 内容区域 -----
.DLL命令 设置标签页内容背景色, 整数型, "emoji_window.dll", "SetTabContentBgColor", , , 设置指定标签页的内容区域背景色（成功返回0，失败返回-1）
    .参数 TabControl句柄, 整数型, , TabControl窗口句柄
    .参数 索引, 整数型, , 标签页索引（从0开始）
    .参数 颜色, 整数型, , ARGB格式颜色值

.DLL命令 设置所有标签页内容背景色, 整数型, "emoji_window.dll", "SetTabContentBgColorAll", , , 设置所有标签页的内容区域背景色（成功返回0，失败返回-1）
    .参数 TabControl句柄, 整数型, , TabControl窗口句柄
    .参数 颜色, 整数型, , ARGB格式颜色值

' ----- 交互增强 -----
.DLL命令 设置标签页可关闭, 整数型, "emoji_window.dll", "SetTabClosable", , , 设置标签页是否显示关闭按钮（成功返回0，失败返回-1）
    .参数 TabControl句柄, 整数型, , TabControl窗口句柄
    .参数 是否可关闭, 整数型, , 1=显示关闭按钮，0=隐藏

.DLL命令 设置标签页关闭回调, 整数型, "emoji_window.dll", "SetTabCloseCallback", , , 设置标签页关闭按钮点击回调（成功返回0，失败返回-1）
    .参数 TabControl句柄, 整数型, , TabControl窗口句柄
    .参数 回调函数, 子程序指针, , 回调函数指针（参数：TabControl句柄, 索引）

.DLL命令 设置标签页右键回调, 整数型, "emoji_window.dll", "SetTabRightClickCallback", , , 设置标签页右键点击回调（成功返回0，失败返回-1）
    .参数 TabControl句柄, 整数型, , TabControl窗口句柄
    .参数 回调函数, 子程序指针, , 回调函数指针（参数：TabControl句柄, 索引, X, Y）

.DLL命令 设置标签页可拖拽, 整数型, "emoji_window.dll", "SetTabDraggable", , , 设置标签页是否可拖拽排序（成功返回0，失败返回-1）
    .参数 TabControl句柄, 整数型, , TabControl窗口句柄
    .参数 是否可拖拽, 整数型, , 1=可拖拽，0=不可拖拽

.DLL命令 设置标签页双击回调, 整数型, "emoji_window.dll", "SetTabDoubleClickCallback", , , 设置标签页双击回调（成功返回0，失败返回-1）
    .参数 TabControl句柄, 整数型, , TabControl窗口句柄
    .参数 回调函数, 子程序指针, , 回调函数指针（参数：TabControl句柄, 索引）

.DLL命令 设置新建标签按钮回调, 整数型, "emoji_window.dll", "SetTabNewButtonCallback", , , 设置Tab头右侧加号按钮回调（成功返回0，失败返回-1）
    .参数 TabControl句柄, 整数型, , TabControl窗口句柄
    .参数 回调函数, 子程序指针, , 回调函数指针（参数：TabControl句柄）

' ----- 布局与位置 -----
.DLL命令 设置标签栏位置, 整数型, "emoji_window.dll", "SetTabPosition", , , 设置标签栏位置（成功返回0，失败返回-1）
    .参数 TabControl句柄, 整数型, , TabControl窗口句柄
    .参数 位置, 整数型, , 0=上，1=下，2=左，3=右

.DLL命令 设置标签对齐方式, 整数型, "emoji_window.dll", "SetTabAlignment", , , 设置标签在标签栏中的对齐方式（成功返回0，失败返回-1）
    .参数 TabControl句柄, 整数型, , TabControl窗口句柄
    .参数 对齐方式, 整数型, , 0=左对齐，1=居中，2=右对齐

.DLL命令 设置标签栏可滚动, 整数型, "emoji_window.dll", "SetTabScrollable", , , 设置标签栏是否可滚动（成功返回0，失败返回-1）
    .参数 TabControl句柄, 整数型, , TabControl窗口句柄
    .参数 是否可滚动, 整数型, , 1=可滚动，0=不可滚动（多行模式）

' ----- 批量操作 -----
.DLL命令 清空所有标签页, 整数型, "emoji_window.dll", "RemoveAllTabs", , , 清空所有标签页并销毁内容窗口（成功返回0，失败返回-1）
    .参数 TabControl句柄, 整数型, , TabControl窗口句柄

.DLL命令 插入标签页, 整数型, "emoji_window.dll", "InsertTabItem", , , 在指定位置插入标签页（返回实际索引，失败返回-1）
    .参数 TabControl句柄, 整数型, , TabControl窗口句柄
    .参数 索引, 整数型, , 插入位置索引（超出范围追加到末尾）
    .参数 标题字节集指针, 整数型, , UTF-8编码的标题字节集指针
    .参数 标题长度, 整数型, , 标题字节集长度
    .参数 内容窗口句柄, 整数型, , 内容窗口句柄（传0自动创建）

.DLL命令 移动标签页, 整数型, "emoji_window.dll", "MoveTabItem", , , 移动标签页位置（成功返回0，失败返回-1）
    .参数 TabControl句柄, 整数型, , TabControl窗口句柄
    .参数 源索引, 整数型, , 源位置索引
    .参数 目标索引, 整数型, , 目标位置索引

.DLL命令 根据标题查找标签页, 整数型, "emoji_window.dll", "GetTabIndexByTitle", , , 根据标题查找标签页索引（未找到返回-1）
    .参数 TabControl句柄, 整数型, , TabControl窗口句柄
    .参数 标题字节集指针, 整数型, , UTF-8编码的标题字节集指针
    .参数 标题长度, 整数型, , 标题字节集长度

' ----- 状态查询 -----
.DLL命令 获取TabControl启用状态, 整数型, "emoji_window.dll", "GetTabEnabled", , , 获取整个TabControl的启用状态（1=启用，0=禁用，-1=错误）
    .参数 TabControl句柄, 整数型, , TabControl窗口句柄

.DLL命令 标签页是否选中, 整数型, "emoji_window.dll", "IsTabItemSelected", , , 判断指定标签页是否为当前选中（1=选中，0=未选中，-1=错误）
    .参数 TabControl句柄, 整数型, , TabControl窗口句柄
    .参数 索引, 整数型, , 标签页索引（从0开始）

' ========== 编辑框扩展属性获取命令 ==========

.DLL命令 获取编辑框字体, 整数型, "emoji_window.dll", "GetEditBoxFont", , , 获取编辑框字体信息（返回字体名UTF-8字节数，缓冲区指针为0时返回所需大小）
    .参数 编辑框句柄, 整数型, , 编辑框窗口句柄
    .参数 字体名缓冲区指针, 整数型, , UTF-8字体名缓冲区指针（可为0获取所需大小）
    .参数 字体名缓冲区大小, 整数型, , 缓冲区大小
    .参数 字体大小, 整数型, 传址, 输出参数：字体大小（调用时请用"传址 变量"传参）
    .参数 粗体, 整数型, 传址, 输出参数：是否粗体（1=是，0=否，调用时请用"传址 变量"传参）
    .参数 斜体, 整数型, 传址, 输出参数：是否斜体（1=是，0=否，调用时请用"传址 变量"传参）
    .参数 下划线, 整数型, 传址, 输出参数：是否下划线（1=是，0=否，调用时请用"传址 变量"传参）

.DLL命令 获取编辑框颜色, 整数型, "emoji_window.dll", "GetEditBoxColor", , , 获取编辑框颜色（成功返回0，失败返回-1）
    .参数 编辑框句柄, 整数型, , 编辑框窗口句柄
    .参数 前景色, 整数型, 传址, 输出参数：前景色ARGB（调用时请用"传址 变量"传参）
    .参数 背景色, 整数型, 传址, 输出参数：背景色ARGB（调用时请用"传址 变量"传参）

.DLL命令 获取编辑框位置, 整数型, "emoji_window.dll", "GetEditBoxBounds", , , 获取编辑框位置和大小（成功返回0，失败返回-1）
    .参数 编辑框句柄, 整数型, , 编辑框窗口句柄
    .参数 X坐标, 整数型, 传址, 输出参数：X坐标（调用时请用"传址 变量"传参）
    .参数 Y坐标, 整数型, 传址, 输出参数：Y坐标（调用时请用"传址 变量"传参）
    .参数 宽度, 整数型, 传址, 输出参数：宽度（调用时请用"传址 变量"传参）
    .参数 高度, 整数型, 传址, 输出参数：高度（调用时请用"传址 变量"传参）

.DLL命令 获取编辑框对齐方式, 整数型, "emoji_window.dll", "GetEditBoxAlignment", , , 获取编辑框对齐方式（0=左，1=中，2=右，-1=错误）
    .参数 编辑框句柄, 整数型, , 编辑框窗口句柄

.DLL命令 设置编辑框对齐方式, , "emoji_window.dll", "SetEditBoxAlignment", , , 设置编辑框对齐方式
    .参数 编辑框句柄, 整数型, , 编辑框窗口句柄
    .参数 对齐方式, 整数型, , 0=左对齐，1=居中，2=右对齐

.DLL命令 获取编辑框启用状态, 整数型, "emoji_window.dll", "GetEditBoxEnabled", , , 获取编辑框启用状态（1=启用，0=禁用，-1=错误）
    .参数 编辑框句柄, 整数型, , 编辑框窗口句柄

.DLL命令 获取编辑框可视状态, 整数型, "emoji_window.dll", "GetEditBoxVisible", , , 获取编辑框可视状态（1=可见，0=不可见，-1=错误）
    .参数 编辑框句柄, 整数型, , 编辑框窗口句柄

' ========== 进度条扩展属性获取命令 ==========

.DLL命令 获取进度条颜色, 整数型, "emoji_window.dll", "GetProgressBarColor", , , 获取进度条颜色（成功返回0，失败返回-1）
    .参数 进度条句柄, 整数型, , 进度条窗口句柄
    .参数 前景色, 整数型, 传址, 输出参数：前景色ARGB（调用时请用"传址 变量"传参）
    .参数 背景色, 整数型, 传址, 输出参数：背景色ARGB（调用时请用"传址 变量"传参）

.DLL命令 获取进度条位置, 整数型, "emoji_window.dll", "GetProgressBarBounds", , , 获取进度条位置和大小（成功返回0，失败返回-1）
    .参数 进度条句柄, 整数型, , 进度条窗口句柄
    .参数 X坐标, 整数型, 传址, 输出参数：X坐标（调用时请用"传址 变量"传参）
    .参数 Y坐标, 整数型, 传址, 输出参数：Y坐标（调用时请用"传址 变量"传参）
    .参数 宽度, 整数型, 传址, 输出参数：宽度（调用时请用"传址 变量"传参）
    .参数 高度, 整数型, 传址, 输出参数：高度（调用时请用"传址 变量"传参）

.DLL命令 获取进度条启用状态, 整数型, "emoji_window.dll", "GetProgressBarEnabled", , , 获取进度条启用状态（1=启用，0=禁用，-1=错误）
    .参数 进度条句柄, 整数型, , 进度条窗口句柄

.DLL命令 获取进度条可视状态, 整数型, "emoji_window.dll", "GetProgressBarVisible", , , 获取进度条可视状态（1=可见，0=不可见，-1=错误）
    .参数 进度条句柄, 整数型, , 进度条窗口句柄

.DLL命令 获取进度条显示文本状态, 整数型, "emoji_window.dll", "GetProgressBarShowText", , , 获取进度条是否显示百分比文本（1=显示，0=不显示，-1=错误）
    .参数 进度条句柄, 整数型, , 进度条窗口句柄


' ========== 标题栏样式自定义功能 ==========

.DLL命令 设置标题栏文字颜色, 整数型, "emoji_window.dll", "SetTitleBarTextColor", , , 设置标题栏文字颜色（ARGB，0=跟随主题），返回1=成功，0=失败
    .参数 窗口句柄, 整数型, , 窗口句柄
    .参数 颜色, 整数型, , ARGB颜色值，0表示恢复跟随主题

.DLL命令 获取标题栏文字颜色, 整数型, "emoji_window.dll", "GetTitleBarTextColor", , , 获取标题栏文字颜色（0=跟随主题），无效句柄返回0
    .参数 窗口句柄, 整数型, , 窗口句柄

.DLL命令 设置标题栏字体, 整数型, "emoji_window.dll", "SetTitleBarFont", , , 设置标题栏字体和字号，返回1=成功，0=失败
    .参数 窗口句柄, 整数型, , 窗口句柄
    .参数 字体名字节集指针, 整数型, , UTF-8字体名字节集指针（取变量数据地址）
    .参数 字体名长度, 整数型, , 字体名字节集长度
    .参数 字号, 小数型, , 字号（像素），必须大于0

.DLL命令 设置标题栏对齐方式, 整数型, "emoji_window.dll", "SetTitleBarAlignment", , , 设置标题栏文字对齐方式，返回1=成功，0=失败
    .参数 窗口句柄, 整数型, , 窗口句柄
    .参数 对齐方式, 整数型, , 0=左对齐，1=居中，2=右对齐
' ========== 维护规则（必须遵守） ==========
'
' 重要规则：
' 以后只要新增、删除、重命名、修改任何 DLL 导出方法，
' 都必须同步更新：
' examples\易语言代码\易语言代码文档\DLL命令.md
' 否则视为工作未完成。
'
' 本次新增命令登记：
'
.DLL命令 强制重绘TabControl, 逻辑型, "emoji_window.dll", "RedrawTabControl", , , 强制重绘TabControl及当前内容页，返回真=成功，假=失败
    .参数 TabControl句柄, 整数型, , TabControl句柄
'
.DLL命令 创建面板容器, 整数型, "emoji_window.dll", "CreatePanel", , , 创建可承载子控件的空白容器，适合布局器、树形框、TabControl等复杂内容宿主
    .参数 父窗口句柄, 整数型, , 父窗口句柄
    .参数 X坐标, 整数型, , X坐标
    .参数 Y坐标, 整数型, , Y坐标
    .参数 宽度, 整数型, , 宽度
    .参数 高度, 整数型, , 高度
    .参数 背景色, 整数型, , ARGB颜色

.DLL命令 创建宿主面板, 整数型, "emoji_window.dll", "CreateHostSurface", , , 创建 WinForms / 外部宿主专用的原生承载面板
    .参数 父窗口句柄, 整数型, , 父窗口句柄
    .参数 X坐标, 整数型, , X坐标
    .参数 Y坐标, 整数型, , Y坐标
    .参数 宽度, 整数型, , 宽度
    .参数 高度, 整数型, , 高度
    .参数 背景色, 整数型, , ARGB颜色

.DLL命令 销毁宿主面板, , "emoji_window.dll", "DestroyHostSurface", , , 销毁宿主面板及其原生子控件
    .参数 宿主面板句柄, 整数型

.DLL命令 设置宿主面板背景色, , "emoji_window.dll", "SetHostSurfaceBackgroundColor", , , 设置宿主面板背景色
    .参数 宿主面板句柄, 整数型
    .参数 背景色, 整数型, , ARGB颜色

.DLL命令 取宿主面板背景色, 整数型, "emoji_window.dll", "GetHostSurfaceBackgroundColor", , , 获取宿主面板背景色；返回0=成功
    .参数 宿主面板句柄, 整数型
    .参数 背景色指针, 整数型

.DLL命令 设置宿主面板位置, , "emoji_window.dll", "SetHostSurfaceBounds", , , 设置宿主面板位置和大小；坐标与尺寸为96DPI逻辑单位
    .参数 宿主面板句柄, 整数型
    .参数 X坐标, 整数型
    .参数 Y坐标, 整数型
    .参数 宽度, 整数型
    .参数 高度, 整数型

.DLL命令 取宿主面板位置, 整数型, "emoji_window.dll", "GetHostSurfaceBounds", , , 读取宿主面板位置和大小；返回96DPI逻辑单位，0=成功
    .参数 宿主面板句柄, 整数型
    .参数 X坐标指针, 整数型
    .参数 Y坐标指针, 整数型
    .参数 宽度指针, 整数型
    .参数 高度指针, 整数型

.DLL命令 设置面板位置, , "emoji_window.dll", "SetPanelBounds", , , 设置面板位置和大小；坐标与尺寸为96DPI逻辑单位
    .参数 面板句柄, 整数型
    .参数 X坐标, 整数型
    .参数 Y坐标, 整数型
    .参数 宽度, 整数型
    .参数 高度, 整数型

.DLL命令 取面板位置, 整数型, "emoji_window.dll", "GetPanelBounds", , , 读取面板位置和大小；返回96DPI逻辑单位，0=成功
    .参数 面板句柄, 整数型
    .参数 X坐标指针, 整数型
    .参数 Y坐标指针, 整数型
    .参数 宽度指针, 整数型
    .参数 高度指针, 整数型
'
' 本次新增命令登记（Element Plus 风格扩展）：
'
.DLL命令 设置单选按钮样式, , "emoji_window.dll", "SetRadioButtonStyle", , , 设置单选按钮样式：0=默认，1=带边框，2=按钮样式
    .参数 单选按钮句柄, 整数型, , 单选按钮句柄
    .参数 样式, 整数型, , 0=默认，1=带边框，2=按钮
'
.DLL命令 获取单选按钮样式, 整数型, "emoji_window.dll", "GetRadioButtonStyle", , , 获取单选按钮样式
    .参数 单选按钮句柄, 整数型, , 单选按钮句柄
'
.DLL命令 设置单选按钮圆点颜色, , "emoji_window.dll", "SetRadioButtonDotColor", , , 设置单选按钮选中圆点颜色
    .参数 单选按钮句柄, 整数型, , 单选按钮句柄
    .参数 颜色, 整数型, , ARGB颜色
'
.DLL命令 获取单选按钮圆点颜色, 整数型, "emoji_window.dll", "GetRadioButtonDotColor", , , 获取单选按钮选中圆点颜色
    .参数 单选按钮句柄, 整数型, , 单选按钮句柄
    .参数 颜色, 整数型, 参考, 输出ARGB颜色
'
.DLL命令 创建滑块, 整数型, "emoji_window.dll", "CreateSlider", , , 创建 Element Plus 风格滑块
    .参数 父窗口句柄, 整数型, , 父窗口句柄
    .参数 X坐标, 整数型, , X坐标
    .参数 Y坐标, 整数型, , Y坐标
    .参数 宽度, 整数型, , 宽度
    .参数 高度, 整数型, , 高度
    .参数 最小值, 整数型, , 最小值
    .参数 最大值, 整数型, , 最大值
    .参数 当前值, 整数型, , 当前值
    .参数 步长, 整数型, , 步长
    .参数 激活颜色, 整数型, , ARGB颜色
    .参数 背景颜色, 整数型, , ARGB颜色
'
.DLL命令 获取滑块值, 整数型, "emoji_window.dll", "GetSliderValue", , , 获取当前滑块值
    .参数 滑块句柄, 整数型, , 滑块句柄
'
.DLL命令 设置滑块值, , "emoji_window.dll", "SetSliderValue", , , 设置当前滑块值
    .参数 滑块句柄, 整数型, , 滑块句柄
    .参数 值, 整数型, , 当前值
'
.DLL命令 设置滑块范围, , "emoji_window.dll", "SetSliderRange", , , 设置滑块最小值和最大值
    .参数 滑块句柄, 整数型, , 滑块句柄
    .参数 最小值, 整数型, , 最小值
    .参数 最大值, 整数型, , 最大值
'
.DLL命令 设置滑块步长, , "emoji_window.dll", "SetSliderStep", , , 设置滑块步长
    .参数 滑块句柄, 整数型, , 滑块句柄
    .参数 步长, 整数型, , 步长
'
.DLL命令 设置滑块显示停点, , "emoji_window.dll", "SetSliderShowStops", , , 设置是否显示停点
    .参数 滑块句柄, 整数型, , 滑块句柄
    .参数 是否显示, 逻辑型, , 真=显示，假=隐藏
'
.DLL命令 设置滑块颜色, , "emoji_window.dll", "SetSliderColors", , , 设置滑块激活色、轨道色和按钮色
    .参数 滑块句柄, 整数型, , 滑块句柄
    .参数 激活颜色, 整数型, , ARGB颜色
    .参数 背景颜色, 整数型, , ARGB颜色
    .参数 按钮颜色, 整数型, , ARGB颜色
'
.DLL命令 获取滑块颜色, 逻辑型, "emoji_window.dll", "GetSliderColors", , , 获取滑块激活色、轨道色和按钮色
    .参数 滑块句柄, 整数型, , 滑块句柄
    .参数 激活颜色, 整数型, 参考, 输出ARGB颜色
    .参数 背景颜色, 整数型, 参考, 输出ARGB颜色
    .参数 按钮颜色, 整数型, 参考, 输出ARGB颜色
'
.DLL命令 设置滑块回调, , "emoji_window.dll", "SetSliderCallback", , , 设置滑块值变化回调
    .参数 滑块句柄, 整数型, , 滑块句柄
    .参数 回调函数, 整数型, , 回调地址
'
.DLL命令 启用滑块, , "emoji_window.dll", "EnableSlider", , , 启用或禁用滑块
    .参数 滑块句柄, 整数型, , 滑块句柄
    .参数 启用, 逻辑型, , 真=启用，假=禁用
'
.DLL命令 显示隐藏滑块, , "emoji_window.dll", "ShowSlider", , , 显示或隐藏滑块
    .参数 滑块句柄, 整数型, , 滑块句柄
    .参数 可见, 逻辑型, , 真=显示，假=隐藏
'
.DLL命令 设置滑块位置, , "emoji_window.dll", "SetSliderBounds", , , 设置滑块位置与大小
    .参数 滑块句柄, 整数型, , 滑块句柄
    .参数 X坐标, 整数型
    .参数 Y坐标, 整数型
    .参数 宽度, 整数型
    .参数 高度, 整数型
'
.DLL命令 创建开关, 整数型, "emoji_window.dll", "CreateSwitch", , , 创建 Element Plus 风格开关
    .参数 父窗口句柄, 整数型, , 父窗口句柄
    .参数 X坐标, 整数型, , X坐标
    .参数 Y坐标, 整数型, , Y坐标
    .参数 宽度, 整数型, , 宽度
    .参数 高度, 整数型, , 高度
    .参数 初始状态, 逻辑型, , 真=开启，假=关闭
    .参数 开启颜色, 整数型, , ARGB颜色
    .参数 关闭颜色, 整数型, , ARGB颜色
    .参数 开启文字, 字节集, , UTF-8文字
    .参数 开启文字长度, 整数型, , UTF-8长度
    .参数 关闭文字, 字节集, , UTF-8文字
    .参数 关闭文字长度, 整数型, , UTF-8长度
'
.DLL命令 获取开关状态, 逻辑型, "emoji_window.dll", "GetSwitchState", , , 获取当前开关状态
    .参数 开关句柄, 整数型, , 开关句柄
'
.DLL命令 设置开关状态, , "emoji_window.dll", "SetSwitchState", , , 设置当前开关状态
    .参数 开关句柄, 整数型, , 开关句柄
    .参数 状态, 逻辑型, , 真=开启，假=关闭
'
.DLL命令 设置开关文字, , "emoji_window.dll", "SetSwitchText", , , 设置开关开启/关闭文字
    .参数 开关句柄, 整数型, , 开关句柄
'
.DLL命令 设置开关颜色, , "emoji_window.dll", "SetSwitchColors", , , 设置开关开启/关闭颜色
    .参数 开关句柄, 整数型, , 开关句柄
    .参数 开启颜色, 整数型, , ARGB颜色
    .参数 关闭颜色, 整数型, , ARGB颜色
'
.DLL命令 设置开关文字颜色, , "emoji_window.dll", "SetSwitchTextColors", , , 设置开关开启/关闭文字颜色
    .参数 开关句柄, 整数型, , 开关句柄
    .参数 开启文字颜色, 整数型, , ARGB颜色
    .参数 关闭文字颜色, 整数型, , ARGB颜色
'
.DLL命令 获取开关颜色, 逻辑型, "emoji_window.dll", "GetSwitchColors", , , 获取开关轨道颜色和文字颜色
    .参数 开关句柄, 整数型, , 开关句柄
    .参数 开启颜色, 整数型, 参考, 输出ARGB颜色
    .参数 关闭颜色, 整数型, 参考, 输出ARGB颜色
    .参数 开启文字颜色, 整数型, 参考, 输出ARGB颜色
    .参数 关闭文字颜色, 整数型, 参考, 输出ARGB颜色
'
.DLL命令 设置开关回调, , "emoji_window.dll", "SetSwitchCallback", , , 设置开关状态变化回调
    .参数 开关句柄, 整数型, , 开关句柄
    .参数 回调函数, 整数型, , 回调地址
'
.DLL命令 启用开关, , "emoji_window.dll", "EnableSwitch", , , 启用或禁用开关
    .参数 开关句柄, 整数型, , 开关句柄
    .参数 启用, 逻辑型, , 真=启用，假=禁用
'
.DLL命令 显示隐藏开关, , "emoji_window.dll", "ShowSwitch", , , 显示或隐藏开关
    .参数 开关句柄, 整数型, , 开关句柄
    .参数 可见, 逻辑型, , 真=显示，假=隐藏
'
.DLL命令 设置开关位置, , "emoji_window.dll", "SetSwitchBounds", , , 设置开关位置与大小
    .参数 开关句柄, 整数型, , 开关句柄
    .参数 X坐标, 整数型
    .参数 Y坐标, 整数型
    .参数 宽度, 整数型
    .参数 高度, 整数型
'
.DLL命令 创建提示层, 整数型, "emoji_window.dll", "CreateTooltip", , , 创建 Tooltip 提示层
    .参数 所有者窗口句柄, 整数型, , 所有者窗口句柄
    .参数 文本, 字节集, , UTF-8提示文字
    .参数 文本长度, 整数型, , UTF-8长度
    .参数 位置, 整数型, , 参考 PopupPlacement 枚举
    .参数 背景颜色, 整数型, , ARGB颜色
    .参数 前景颜色, 整数型, , ARGB颜色
'
.DLL命令 设置提示层文字, , "emoji_window.dll", "SetTooltipText", , , 设置 Tooltip 文字
    .参数 提示层句柄, 整数型, , Tooltip句柄
'
.DLL命令 设置提示层位置, , "emoji_window.dll", "SetTooltipPlacement", , , 设置 Tooltip 出现方向
    .参数 提示层句柄, 整数型, , Tooltip句柄
    .参数 位置, 整数型, , 0=上方，1=下方，2=左侧，3=右侧
'
.DLL命令 设置提示层主题, , "emoji_window.dll", "SetTooltipTheme", , , 设置 Tooltip 主题模式
    .参数 提示层句柄, 整数型, , Tooltip句柄
    .参数 主题, 整数型, , 0=dark，1=light，2=custom
'
.DLL命令 设置提示层颜色, , "emoji_window.dll", "SetTooltipColors", , , 设置 Tooltip 背景/前景/边框颜色
    .参数 提示层句柄, 整数型, , Tooltip句柄
    .参数 背景颜色, 整数型, , ARGB颜色
    .参数 前景颜色, 整数型, , ARGB颜色
    .参数 边框颜色, 整数型, , ARGB颜色
'
.DLL命令 设置提示层字体, , "emoji_window.dll", "SetTooltipFont", , , 设置 Tooltip 字体和字号
    .参数 提示层句柄, 整数型, , Tooltip句柄
    .参数 字体字节集指针, 整数型, , 取变量数据地址(字体名字节集)
    .参数 字体长度, 整数型, , 取字节集长度(字体名字节集)
    .参数 字号, 小数型, , 字号大小（如 14.0）
'
.DLL命令 设置提示层触发方式, , "emoji_window.dll", "SetTooltipTrigger", , , 设置 Tooltip 触发方式
    .参数 提示层句柄, 整数型, , Tooltip句柄
    .参数 触发方式, 整数型, , 0=hover，1=click
'
.DLL命令 绑定提示层到控件, , "emoji_window.dll", "BindTooltipToControl", , , 绑定 Tooltip 与目标控件
    .参数 提示层句柄, 整数型, , Tooltip句柄
    .参数 目标控件句柄, 整数型, , 目标控件句柄
'
.DLL命令 对控件显示提示层, , "emoji_window.dll", "ShowTooltipForControl", , , 在指定控件附近显示 Tooltip
    .参数 提示层句柄, 整数型, , Tooltip句柄
    .参数 目标控件句柄, 整数型, , 目标控件句柄
'
.DLL命令 隐藏提示层, , "emoji_window.dll", "HideTooltip", , , 隐藏 Tooltip
    .参数 提示层句柄, 整数型, , Tooltip句柄
'
.DLL命令 销毁提示层, , "emoji_window.dll", "DestroyTooltip", , , 销毁 Tooltip
    .参数 提示层句柄, 整数型, , Tooltip句柄
'
.DLL命令 创建气泡确认框, 整数型, "emoji_window.dll", "CreatePopconfirm", , , 创建 Popconfirm 气泡确认框
    .参数 所有者窗口句柄, 整数型, , 所有者窗口句柄
'
.DLL命令 设置气泡确认框回调, , "emoji_window.dll", "SetPopconfirmCallback", , , 设置 Popconfirm 确认/取消回调
    .参数 气泡确认框句柄, 整数型, , Popconfirm句柄
    .参数 回调函数, 整数型, , 回调地址
'
.DLL命令 对控件显示气泡确认框, , "emoji_window.dll", "ShowPopconfirmForControl", , , 在指定控件附近显示 Popconfirm
    .参数 气泡确认框句柄, 整数型, , Popconfirm句柄
    .参数 目标控件句柄, 整数型, , 目标控件句柄
'
.DLL命令 隐藏气泡确认框, , "emoji_window.dll", "HidePopconfirm", , , 隐藏 Popconfirm
    .参数 气泡确认框句柄, 整数型, , Popconfirm句柄
'
.DLL命令 销毁气泡确认框, , "emoji_window.dll", "DestroyPopconfirm", , , 销毁 Popconfirm
    .参数 气泡确认框句柄, 整数型, , Popconfirm句柄
'
.DLL命令 显示通知, 整数型, "emoji_window.dll", "ShowNotification", , , 显示 Notification 通知窗口
    .参数 所有者窗口句柄, 整数型, , 所有者窗口句柄
    .参数 标题, 字节集, , UTF-8标题
    .参数 标题长度, 整数型, , UTF-8长度
    .参数 内容, 字节集, , UTF-8内容
    .参数 内容长度, 整数型, , UTF-8长度
    .参数 类型, 整数型, , 0=info，1=success，2=warning，3=error
    .参数 位置, 整数型, , 0=右上，1=左上，2=右下，3=左下
    .参数 持续毫秒, 整数型, , 自动关闭时长，0=不自动关闭
'
.DLL命令 设置通知回调, , "emoji_window.dll", "SetNotificationCallback", , , 设置 Notification 点击/关闭回调
    .参数 通知句柄, 整数型, , 通知句柄
    .参数 回调函数, 整数型, , 回调地址
'
.DLL命令 关闭通知, , "emoji_window.dll", "CloseNotification", , , 主动关闭 Notification
    .参数 通知句柄, 整数型, , 通知句柄
'
' ================================
' 补充别名命令（用于项目源码统一命名）
' ================================
'
.DLL命令 设置面板背景色, , "emoji_window.dll", "SetPanelBackgroundColor", , , 设置面板背景色
    .参数 面板句柄, 整数型
    .参数 背景色, 整数型, , ARGB颜色
'
.DLL命令 删除节点, 逻辑型, "emoji_window.dll", "RemoveNode", , , 删除节点（含子树）
    .参数 树形框句柄, 整数型
    .参数 节点ID, 整数型
'
.DLL命令 设置节点前景色, 逻辑型, "emoji_window.dll", "SetNodeForeColor", , , 设置节点前景色
    .参数 树形框句柄, 整数型
    .参数 节点ID, 整数型
    .参数 颜色, 整数型, , ARGB颜色
'
.DLL命令 设置树形框背景色, 逻辑型, "emoji_window.dll", "SetTreeViewBackgroundColor", , , 设置树形框背景色
    .参数 树形框句柄, 整数型
    .参数 背景色, 整数型, , ARGB颜色
'
.DLL命令 创建编辑框, 整数型, "emoji_window.dll", "CreateEditBox", , , 创建编辑框
    .参数 父窗口句柄, 整数型
    .参数 X坐标, 整数型
    .参数 Y坐标, 整数型
    .参数 宽度, 整数型
    .参数 高度, 整数型
    .参数 文本字节集指针, 整数型
    .参数 文本长度, 整数型
    .参数 前景色, 整数型, , ARGB颜色
    .参数 背景色, 整数型, , ARGB颜色
    .参数 字体名字节集指针, 整数型
    .参数 字体名长度, 整数型
    .参数 字号, 整数型
    .参数 粗体, 整数型
    .参数 斜体, 整数型
    .参数 下划线, 整数型
    .参数 对齐, 整数型
    .参数 多行, 整数型
    .参数 只读, 整数型
    .参数 密码, 整数型
    .参数 边框, 整数型
    .参数 垂直居中, 整数型
'
.DLL命令 获取编辑框文本, 整数型, "emoji_window.dll", "GetEditBoxText", , , 获取编辑框文本
    .参数 编辑框句柄, 整数型
    .参数 缓冲区指针, 整数型
    .参数 缓冲区大小, 整数型
'
.DLL命令 设置编辑框文本, , "emoji_window.dll", "SetEditBoxText", , , 设置编辑框文本
    .参数 编辑框句柄, 整数型
    .参数 文本字节集指针, 整数型
    .参数 文本长度, 整数型
'
.DLL命令 设置编辑框按键回调, , "emoji_window.dll", "SetEditBoxKeyCallback", , , 设置编辑框按键回调
    .参数 编辑框句柄, 整数型
    .参数 回调子程序指针, 整数型, , 子程序需 stdcall，参数：hEdit, key_code, key_down, shift, ctrl, alt
'
.DLL命令 设置编辑框字体, , "emoji_window.dll", "SetEditBoxFont", , , 设置编辑框字体
    .参数 编辑框句柄, 整数型
    .参数 字体名字节集指针, 整数型
    .参数 字体名长度, 整数型
    .参数 字号, 整数型
    .参数 粗体, 整数型
    .参数 斜体, 整数型
    .参数 下划线, 整数型
'
.DLL命令 设置编辑框颜色, , "emoji_window.dll", "SetEditBoxColor", , , 设置编辑框颜色
    .参数 编辑框句柄, 整数型
    .参数 前景色, 整数型
    .参数 背景色, 整数型
'
.DLL命令 设置编辑框位置, , "emoji_window.dll", "SetEditBoxBounds", , , 设置编辑框位置
    .参数 编辑框句柄, 整数型
    .参数 X坐标, 整数型
    .参数 Y坐标, 整数型
    .参数 宽度, 整数型
    .参数 高度, 整数型
'
.DLL命令 显示编辑框, , "emoji_window.dll", "ShowEditBox", , , 显示编辑框
    .参数 编辑框句柄, 整数型
    .参数 显示, 逻辑型

.DLL命令 设置按钮样式, , "emoji_window.dll", "SetButtonStyle", , 0 = BUTTON_STYLE_SOLID，1 = BUTTON_STYLE_PLAIN，2 = BUTTON_STYLE_TEXT，3 = BUTTON_STYLE_LINK
    .参数 按钮ID, 整数型
    .参数 样式, 整数型
'
.DLL命令 设置按钮尺寸, , "emoji_window.dll", "SetButtonSize", , 0 = BUTTON_SIZE_LARGE 表示大号按钮。内部会用更大的文字、emoji、loading 圈和更大的圆角。1 = BUTTON_SIZE_DEFAULT 表示默认尺寸。常规按钮大小，不传时默认就是这个。2 = BUTTON_SIZE_SMALL 表示小号按钮。文字、emoji、loading 圈、圆角都会更小。
    .参数 按钮ID, 整数型
    .参数 尺寸, 整数型
'
.DLL命令 设置按钮圆角, , "emoji_window.dll", "SetButtonRound", , , , , 公开
    .参数 按钮ID, 整数型
    .参数 圆角, 整数型
'
.DLL命令 设置按钮边框色, , "emoji_window.dll", "SetButtonBorderColor", , , , , 公开
    .参数 按钮ID, 整数型
    .参数 边框色, 整数型
'
.DLL命令 设置按钮文字色, , "emoji_window.dll", "SetButtonTextColor", , , , , 公开
    .参数 按钮ID, 整数型
    .参数 文字色, 整数型

'
.DLL命令 设置列表框颜色, , "emoji_window.dll", "SetListBoxColors", , , 设置列表框前景色、背景色、选中色和悬停色
    .参数 列表框句柄, 整数型
    .参数 前景色, 整数型, , ARGB颜色
    .参数 背景色, 整数型, , ARGB颜色
    .参数 选中色, 整数型, , ARGB颜色
    .参数 悬停色, 整数型, , ARGB颜色

'
.DLL命令 取面板位置, 整数型, "emoji_window.dll", "GetPanelBounds", , , 读取面板位置和大小；返回96DPI逻辑单位，0=成功
    .参数 面板句柄, 整数型
    .参数 X坐标指针, 整数型
    .参数 Y坐标指针, 整数型
    .参数 宽度指针, 整数型
    .参数 高度指针, 整数型

'
.DLL命令 取面板背景色, 整数型, "emoji_window.dll", "GetPanelBackgroundColor", , , 获取面板背景色；返回0=成功
    .参数 面板句柄, 整数型
    .参数 背景色指针, 整数型

'
.DLL命令 取面板像素位置, 整数型, "emoji_window.dll", "GetPanelBoundsPx", , , 读取面板位置和大小；返回物理像素单位，0=成功
    .参数 面板句柄, 整数型
    .参数 X坐标指针, 整数型
    .参数 Y坐标指针, 整数型
    .参数 宽度指针, 整数型
    .参数 高度指针, 整数型

.DLL命令 显示打开文件对话框, 整数型, "emoji_window.dll", "ShowOpenFileDialog", , 打开文件对话框；筛选器格式如“Excel文件|*.xlsx;*.xls|所有文件|*.*”；多选结果用换行分隔
    .参数 父窗口句柄, 整数型
    .参数 标题字节集指针, 整数型, , UTF-8，可为0
    .参数 标题长度, 整数型
    .参数 筛选器字节集指针, 整数型, , UTF-8，可为0
    .参数 筛选器长度, 整数型
    .参数 初始目录字节集指针, 整数型, , UTF-8，可为0
    .参数 初始目录长度, 整数型
    .参数 是否允许多选, 逻辑型
    .参数 输出缓冲区指针, 整数型, , UTF-8路径缓冲区；建议预留足够空间
    .参数 输出缓冲区大小, 整数型

.DLL命令 显示保存文件对话框, 整数型, "emoji_window.dll", "ShowSaveFileDialog", , 保存文件对话框；返回UTF-8路径字节数，取消返回0，缓冲区不足返回负的所需长度
    .参数 父窗口句柄, 整数型
    .参数 标题字节集指针, 整数型, , UTF-8，可为0
    .参数 标题长度, 整数型
    .参数 筛选器字节集指针, 整数型, , UTF-8，可为0
    .参数 筛选器长度, 整数型
    .参数 初始目录字节集指针, 整数型, , UTF-8，可为0
    .参数 初始目录长度, 整数型
    .参数 默认扩展名字节集指针, 整数型, , 如“xlsx”，UTF-8，可为0
    .参数 默认扩展名长度, 整数型
    .参数 输出缓冲区指针, 整数型
    .参数 输出缓冲区大小, 整数型

.DLL命令 显示选择文件夹对话框, 整数型, "emoji_window.dll", "ShowSelectFolderDialog", , 选择文件夹；返回UTF-8路径字节数，取消返回0
    .参数 父窗口句柄, 整数型
    .参数 标题字节集指针, 整数型, , UTF-8，可为0
    .参数 标题长度, 整数型
    .参数 初始目录字节集指针, 整数型, , UTF-8，可为0
    .参数 初始目录长度, 整数型
    .参数 输出缓冲区指针, 整数型
    .参数 输出缓冲区大小, 整数型

.DLL命令 显示颜色对话框, 逻辑型, "emoji_window.dll", "ShowColorDialog", , 选择颜色；颜色为ARGB整数
    .参数 父窗口句柄, 整数型
    .参数 初始颜色, 整数型
    .参数 选中颜色地址, 整数型, , 传址整数变量

.DLL命令 启用文件拖拽, 逻辑型, "emoji_window.dll", "EnableFileDrop", , 允许窗口或控件接收本地文件拖拽
    .参数 窗口或控件句柄, 整数型
    .参数 是否启用, 逻辑型

.DLL命令 设置文件拖拽回调, , "emoji_window.dll", "SetFileDropCallback", , 回调参数：句柄、路径字节集指针、路径长度、文件序号、文件总数、X、Y；路径指针仅回调期间有效
    .参数 窗口或控件句柄, 整数型
    .参数 回调函数, 子程序指针

.DLL命令 Excel打开工作簿, 整数型, "emoji_window.dll", "Excel_OpenWorkbook", , 打开CSV/TSV/TXT或通过本机Excel读取XLS/XLSX；成功返回句柄，失败返回负数
    .参数 文件路径字节集指针, 整数型, , UTF-8
    .参数 文件路径长度, 整数型
    .参数 工作表序号, 整数型, , 1=第一张；CSV类文件忽略

.DLL命令 Excel关闭工作簿, , "emoji_window.dll", "Excel_CloseWorkbook", , 释放读取缓存
    .参数 工作簿句柄, 整数型

.DLL命令 Excel关闭全部工作簿, , "emoji_window.dll", "Excel_CloseAllWorkbooks", , 释放全部读取缓存

.DLL命令 Excel取行数, 整数型, "emoji_window.dll", "Excel_GetRowCount", , 返回行数；失败返回-1
    .参数 工作簿句柄, 整数型

.DLL命令 Excel取列数, 整数型, "emoji_window.dll", "Excel_GetColumnCount", , 返回列数；失败返回-1
    .参数 工作簿句柄, 整数型

.DLL命令 Excel取单元格文本, 整数型, "emoji_window.dll", "Excel_GetCellText", , 行列均为1起始；返回UTF-8字节数，缓冲区不足返回负的所需长度
    .参数 工作簿句柄, 整数型
    .参数 行号, 整数型
    .参数 列号, 整数型
    .参数 输出缓冲区指针, 整数型
    .参数 输出缓冲区大小, 整数型

.DLL命令 Excel加载到表格, 逻辑型, "emoji_window.dll", "Excel_LoadWorkbookToDataGrid", , 把已打开工作簿加载到DataGridView
    .参数 工作簿句柄, 整数型
    .参数 表格句柄, 整数型
    .参数 首行作为表头, 逻辑型

.DLL命令 Excel文件加载到表格, 逻辑型, "emoji_window.dll", "Excel_ReadFileToDataGrid", , 直接读取文件并加载到DataGridView
    .参数 文件路径字节集指针, 整数型
    .参数 文件路径长度, 整数型
    .参数 表格句柄, 整数型
    .参数 工作表序号, 整数型
    .参数 首行作为表头, 逻辑型
