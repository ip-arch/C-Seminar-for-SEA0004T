# monitor.ps1 -- SEA0004T のシリアル出力 (printf) を表示する
#   終了: Ctrl+C を押すか、このウィンドウを閉じる
param(
    [string]$Port = 'ch340',      # 'COM5' のような番号、またはデバイス名の一部 (ch340)
    [int]$Baud = 115200,
    [string]$PidFile = ''
)

# 'COM5' ならそのまま、デバイス名なら「USB-SERIAL CH340 (COM5)」のような名前から COM 番号を探す
function Resolve-ComPort([string]$name) {
    if ($name -match '^COM\d+$') { return $name }
    $found = Get-CimInstance Win32_PnPEntity |
        Where-Object { $_.Name -like "*$name*" -and $_.Name -match '\((COM\d+)\)' } |
        Select-Object -First 1
    if (-not $found) { return $null }
    [void]($found.Name -match '\((COM\d+)\)')
    return $Matches[1]
}

# stop-monitor.ps1 がこのウィンドウを見つけられるように、タイトルと PID を記録する
$Host.UI.RawUI.WindowTitle = 'SEA0004T Serial Monitor'
if ($PidFile) { Set-Content -Path $PidFile -Value $PID }

$sp = $null
try {
    while ($true) {
        $com = Resolve-ComPort $Port
        if (-not $com) {
            Write-Host "[waiting] port '$Port' not found. Connect the board."
            Start-Sleep -Seconds 1
            continue
        }
        try {
            $sp = New-Object System.IO.Ports.SerialPort $com, $Baud, 'None', 8, 'One'
            $sp.Open()
            Write-Host "[connected] $com $Baud bps   (Ctrl+C or close this window to quit)"
            Start-Sleep -Milliseconds 500      # ポートを開くとボードが再起動するので少し待つ
            $sp.DiscardInBuffer()              # 溜まっていた古いデータは捨てる
            while ($true) {
                if ($sp.BytesToRead -gt 0) { Write-Host -NoNewline $sp.ReadExisting() }
                else { Start-Sleep -Milliseconds 20 }
            }
        }
        catch {
            Write-Host "[disconnected] $($_.Exception.Message)"
            Start-Sleep -Seconds 1
        }
        finally {
            if ($sp) {
                if ($sp.IsOpen) { $sp.Close() }
                $sp.Dispose()
                $sp = $null
            }
        }
    }
}
finally {
    if ($PidFile) { Remove-Item -Path $PidFile -ErrorAction SilentlyContinue }
}
