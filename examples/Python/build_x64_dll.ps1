# 在项目根目录生成 x64 Release emoji_window.dll（供 64 位 Python 加载）
$ErrorActionPreference = "Stop"
$root = Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
$sln = Join-Path $root "emoji_window.sln"
$vswhere = "${env:ProgramFiles(x86)}\Microsoft Visual Studio\Installer\vswhere.exe"
if (-not (Test-Path $vswhere)) {
    Write-Error "未找到 vswhere: $vswhere"
}
$msbuild = & $vswhere -latest -requires Microsoft.Component.MSBuild -find "MSBuild\**\Bin\MSBuild.exe" | Select-Object -First 1
if (-not $msbuild) { Write-Error "未找到 MSBuild" }
& $msbuild $sln /p:Configuration=Release /p:Platform=x64 /m /v:minimal
if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }
$dll = Join-Path $root "bin\x64\Release\emoji_window.dll"
if (-not (Test-Path $dll)) { Write-Error "未生成: $dll" }
Write-Host "OK: $dll"
