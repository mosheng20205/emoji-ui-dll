.版本 2

' ============================================================
' 侧栏折叠菜单（TreeView 侧栏模式）示例
' 覆盖侧栏 API：SetTreeViewSidebarMode、行高/间距、默认字色/选中底/选中前景/悬停底、字体读写、MoveTreeViewNode
' 及树形框常用 API：ClearTree、AddRootNode、AddChildNode、ExpandNode、SetSelectedNode、SetTreeViewCallback、DestroyTreeView 等
' 文档：docs/controls/sidebar_menu_treeview.md
' 请将 emoji_window.dll 与程序同目录（位数与 DLL 一致，本仓库默认生成 x64）
' ============================================================

.程序集 窗口程序集_启动窗口

.程序集变量 主窗口句柄, 整数型
.程序集变量 树形框句柄, 整数型
.程序集变量 节点_订单管理, 整数型
.程序集变量 节点_API, 整数型
.程序集变量 节点_子项, 整数型

.子程序 __启动窗口_创建完毕

主窗口句柄 ＝ 创建Emoji窗口 (“侧栏菜单示例”, －1, －1, 900, 600)
.如果真 (主窗口句柄 ＝ 0)
    信息框 (“创建主窗口失败”, 0, )
    返回 ()
.如果真结束

设置消息循环主窗口 (主窗口句柄)

树形框句柄 ＝ 创建树形框 (主窗口句柄, 10, 40, 320, 520, #FFFFFFFF, #FF303133, 0)
.如果真 (树形框句柄 ＝ 0)
    信息框 (“创建树形框失败”, 0, )
    返回 ()
.如果真结束

ClearTree (树形框句柄)

' ---------- 演示侧栏全部读写 API（须在 SetTreeViewSidebarMode(真) 之前或之后均可） ----------
调用子程序 _演示侧栏全部读写API ()

SetTreeViewSidebarMode (树形框句柄, 真)

SetTreeViewCallback (树形框句柄, 1, &回调_树选中)
SetTreeViewCallback (树形框句柄, 2, &回调_树展开)
SetTreeViewCallback (树形框句柄, 3, &回调_树折叠)
SetTreeViewCallback (树形框句柄, 8, &回调_树移动)

.局部变量 文本, 字节集
.局部变量 图标, 字节集

文本 ＝ 到UTF8 (“订单管理”)
图标 ＝ 到UTF8 (“📦”)
节点_订单管理 ＝ AddRootNode (树形框句柄, 取变量数据地址 (文本), 取字节集长度 (文本), 取变量数据地址 (图标), 取字节集长度 (图标))

文本 ＝ 到UTF8 (“API”)
节点_API ＝ AddChildNode (树形框句柄, 节点_订单管理, 取变量数据地址 (文本), 取字节集长度 (文本), 0, 0)

文本 ＝ 到UTF8 (“子项演示”)
节点_子项 ＝ AddChildNode (树形框句柄, 节点_API, 取变量数据地址 (文本), 取字节集长度 (文本), 0, 0)

ExpandNode (树形框句柄, 节点_订单管理)
SetSelectedNode (树形框句柄, 节点_API)

运行消息循环 ()


.子程序 _演示侧栏全部读写API

SetTreeViewRowHeight (树形框句柄, 38)
SetTreeViewItemSpacing (树形框句柄, 2)
调试输出 (“GetTreeViewRowHeight=” ＋ 到文本 (GetTreeViewRowHeight (树形框句柄)))
调试输出 (“GetTreeViewItemSpacing=” ＋ 到文本 (GetTreeViewItemSpacing (树形框句柄)))

SetTreeViewTextColor (树形框句柄, #FF303133)
SetTreeViewSelectedBgColor (树形框句柄, #334096E6)
SetTreeViewSelectedForeColor (树形框句柄, #FF4096E6)
SetTreeViewHoverBgColor (树形框句柄, #FFF5F7FA)
调试输出 (“GetTreeViewTextColor=” ＋ 到文本 (GetTreeViewTextColor (树形框句柄)))
调试输出 (“GetSelBg=” ＋ 到文本 (GetTreeViewSelectedBgColor (树形框句柄)))
调试输出 (“GetSelFg=” ＋ 到文本 (GetTreeViewSelectedForeColor (树形框句柄)))
调试输出 (“GetHoverBg=” ＋ 到文本 (GetTreeViewHoverBgColor (树形框句柄)))

.局部变量 字体族, 字节集
.局部变量 字号, 小数型
.局部变量 字重, 整数型
.局部变量 斜体, 逻辑型

字体族 ＝ 到UTF8 (“Segoe UI Emoji”)
.如果真 (SetTreeViewFont (树形框句柄, 取变量数据地址 (字体族), 取字节集长度 (字体族), 14, 400, 假))
    字号 ＝ 0
    字重 ＝ 0
    斜体 ＝ 假
    字体族 ＝ 取空白字节集 (256)
    GetTreeViewFont (树形框句柄, 取变量数据地址 (字体族), 256, 传址 字号, 传址 字重, 传址 斜体)
    调试输出 (“GetTreeViewFont size=” ＋ 到文本 (字号) ＋ “ w=” ＋ 到文本 (字重))
.如果真结束

调试输出 (“GetTreeViewSidebarMode=” ＋ 到文本 (GetTreeViewSidebarMode (树形框句柄)))


.子程序 回调_树选中, , , stdcall
.参数 节点ID, 整数型
.参数 上下文, 整数型

调试输出 (“[选中] id=” ＋ 到文本 (节点ID))


.子程序 回调_树展开, , , stdcall
.参数 节点ID, 整数型
.参数 上下文, 整数型

调试输出 (“[展开] id=” ＋ 到文本 (节点ID))


.子程序 回调_树折叠, , , stdcall
.参数 节点ID, 整数型
.参数 上下文, 整数型

调试输出 (“[折叠] id=” ＋ 到文本 (节点ID))


.子程序 回调_树移动, , , stdcall
.参数 节点ID, 整数型
.参数 新父ID, 整数型
.参数 插入索引, 整数型
.参数 上下文, 整数型

调试输出 (“[移动] id=” ＋ 到文本 (节点ID) ＋ “ parent=” ＋ 到文本 (新父ID) ＋ “ idx=” ＋ 到文本 (插入索引))


' 设计器可加按钮绑定：演示 MoveTreeViewNode、GetNodeText 等
.子程序 _按钮_移动子项到根_被单击

.如果真 (MoveTreeViewNode (树形框句柄, 节点_子项, －1, 0))
    信息框 (“MoveTreeViewNode 成功”, 0, )
.否则
    信息框 (“MoveTreeViewNode 失败”, 0, )
.如果真结束


.子程序 _按钮_读取文本_被单击

.局部变量 buf, 字节集
.局部变量 n, 整数型

buf ＝ 取空白字节集 (512)
n ＝ GetNodeText (树形框句柄, 节点_API, 取变量数据地址 (buf), 取字节集长度 (buf))
信息框 (“GetNodeText 长度=” ＋ 到文本 (n), 0, )


.子程序 __启动窗口_可否被关闭, 逻辑型

.如果真 (树形框句柄 ≠ 0)
    DestroyTreeView (树形框句柄)
.如果真结束
.如果真 (主窗口句柄 ≠ 0)
    销毁窗口 (主窗口句柄)
.如果真结束
返回 (真)
