.版本 2

.DLL命令 创建Emoji窗口, 整数型, "emoji_window.dll", "create_window", , , , 公开
    .参数 标题, 文本型, , 窗口标题（UTF-8）
    .参数 宽度, 整数型, , 窗口宽度
    .参数 高度, 整数型, , 窗口高度

.DLL命令 创建Emoji窗口_字节集, 整数型, "emoji_window.dll", "create_window_bytes", , 创建窗口（UTF-8字节集版本，支持Emoji标题）, , 公开
    .参数 标题字节集指针, 整数型, , UTF-8标题字节集指针
    .参数 标题字节集长度, 整数型, , UTF-8标题字节集长度
    .参数 宽度, 整数型, , 窗口宽度
    .参数 高度, 整数型, , 窗口高度

.DLL命令 创建Emoji窗口_字节集_扩展, 整数型, "emoji_window.dll", "create_window_bytes_ex", , 创建窗口（UTF-8字节集扩展版本，支持Emoji标题和标题栏颜色）, , 公开
    .参数 标题字节集指针, 整数型, , UTF-8标题字节集指针
    .参数 标题字节集长度, 整数型, , UTF-8标题字节集长度
    .参数 宽度, 整数型, , 窗口宽度
    .参数 高度, 整数型, , 窗口高度
    .参数 标题栏颜色, 整数型, , 标题栏颜色（十进制RGB颜色，0=跟随主题）

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

.DLL命令 设置消息循环主窗口, , "emoji_window.dll", "set_message_loop_main_window", , , , 公开
    .参数 窗口句柄, 整数型

.DLL命令 运行消息循环, 整数型, "emoji_window.dll", "run_message_loop", , , , 公开

.DLL命令 销毁窗口, , "emoji_window.dll", "destroy_window", , , , 公开
    .参数 窗口句柄, 整数型, , 窗口句柄

.DLL命令 设置窗口图标, , "emoji_window.dll", "set_window_icon"
    .参数 窗口句柄, 整数型
    .参数 图标路径, 文本型

.DLL命令 设置窗口标题, , "emoji_window.dll", "set_window_title", , , 设置窗口标题（UTF-8）
    .参数 窗口句柄, 整数型
    .参数 标题字节集指针, 整数型, , UTF-8字节集指针
    .参数 标题长度, 整数型, , 字节集长度

.DLL命令 设置窗口标题栏颜色, , "emoji_window.dll", "set_window_titlebar_color", , , 设置窗口标题栏颜色（十进制RGB颜色，0=跟随主题）
    .参数 窗口句柄, 整数型
    .参数 标题栏颜色, 整数型

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
    .参数 文本垂直居中, 逻辑型, , 仅单行有效

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
