.版本 2

.DLL命令 创建Emoji窗口, 整数型, "emoji_window.dll", "create_window", , , , 公开
    .参数 标题, 文本型, , 窗口标题（UTF-8）
    .参数 宽度, 整数型, , 窗口宽度
    .参数 高度, 整数型, , 窗口高度

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

.DLL命令 运行消息循环, 整数型, "emoji_window.dll", "run_message_loop", , , , 公开

.DLL命令 销毁窗口, , "emoji_window.dll", "destroy_window", , , , 公开
    .参数 窗口句柄, 整数型, , 窗口句柄

.DLL命令 设置窗口图标, , "emoji_window.dll", "set_window_icon"
    .参数 窗口句柄, 整数型
    .参数 图标路径, 文本型

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

.DLL命令 获取Tab数量, 整数型, "emoji_window.dll", "GetTabCount", , , 获取Tab数量
    .参数 TabControl句柄, 整数型

.DLL命令 获取Tab内容窗口, 整数型, "emoji_window.dll", "GetTabContentWindow", , , 获取指定Tab的内容窗口句柄
    .参数 TabControl句柄, 整数型
    .参数 索引, 整数型

.DLL命令 销毁TabControl, , "emoji_window.dll", "DestroyTabControl", , , 销毁TabControl
    .参数 TabControl句柄, 整数型

.DLL命令 更新TabControl布局, , "emoji_window.dll", "UpdateTabControlLayout", , , 手动更新TabControl布局
    .参数 TabControl句柄, 整数型

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

.DLL命令 获取编辑框文本, 整数型, "emoji_window.dll", "GetEditBoxText", , , 获取编辑框文本
    .参数 编辑框句柄, 整数型
    .参数 缓冲区指针, 整数型
    .参数 缓冲区大小, 整数型

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
