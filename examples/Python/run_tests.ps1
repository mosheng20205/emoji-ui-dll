# 编译 x64 Release DLL 后：可选图形界面或命令行跑全部自动化测试
$ErrorActionPreference = "Stop"
& "$PSScriptRoot\build_x64_dll.ps1"
if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }
Set-Location $PSScriptRoot
# 图形界面（窗口保持打开，便于查看日志）
python run_all_tests_gui.py
exit $LASTEXITCODE
