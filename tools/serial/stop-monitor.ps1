
# stop-monitor.ps1 -- 開いているシリアルモニタを閉じて、COM ポートを解放する
param([string]$PidFile = '')

if (-not $PidFile -or -not (Test-Path $PidFile)) {
    exit 0
}

$text = Get-Content -Path $PidFile -ErrorAction SilentlyContinue |
        Select-Object -First 1

$id = 0

if ([int]::TryParse("$text", [ref]$id) -and $id -gt 0) {
    $proc = Get-Process -Id $id -ErrorAction SilentlyContinue

    if ($proc) {
        # PID ファイルに記録されたモニタを終了
        Write-Host "[stop-monitor] stopping PID=$id ($($proc.ProcessName))"

        Stop-Process -Id $id -Force -ErrorAction Stop

        # プロセス終了を最大5秒待つ
        $timeout = 5000
        $interval = 100
        $elapsed = 0

        while (Get-Process -Id $id -ErrorAction SilentlyContinue) {
            if ($elapsed -ge $timeout) {
                Write-Error "[stop-monitor] process $id did not terminate."
                exit 1
            }

            Start-Sleep -Milliseconds $interval
            $elapsed += $interval
        }

        Write-Host "[stop-monitor] process terminated."
    }
}

Remove-Item -Path $PidFile -ErrorAction SilentlyContinue
exit 0
