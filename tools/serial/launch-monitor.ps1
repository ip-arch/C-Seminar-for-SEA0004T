# launch-monitor.ps1 -- シリアルモニタを別ウィンドウで開く (CMake の monitor ターゲットから呼ばれる)
param(
    [string]$Port = 'ch340',
    [int]$Baud = 115200,
    [string]$PidFile = ''
)

# すでに開いているモニタがあれば閉じてから、新しく開く (ポートの二重オープンを避ける)
& (Join-Path $PSScriptRoot 'stop-monitor.ps1') -PidFile $PidFile

$script = Join-Path $PSScriptRoot 'monitor.ps1'
$argList = @(
    '-NoProfile', '-ExecutionPolicy', 'Bypass',
    '-File', "`"$script`"",
    '-Port', $Port,
    '-Baud', $Baud
)
if ($PidFile) { $argList += @('-PidFile', "`"$PidFile`"") }

Start-Process -FilePath 'powershell' -ArgumentList $argList
