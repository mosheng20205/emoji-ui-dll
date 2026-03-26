# 编译 x64 DLL 后仅打开 D2D 日期时间演示窗口（无 unittest）
$ErrorActionPreference = "Stop"
& "$PSScriptRoot\build_x64_dll.ps1"
if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }
Set-Location $PSScriptRoot
python demo_d2d_datetime_picker.py
exit $LASTEXITCODE
