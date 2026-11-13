# stop-monitor.ps1 -- 開いているシリアルモニタを閉じて、COM ポートを解放する
#   upload_lesson_N の書き込み前に自動で呼ばれる
param([string]$PidFile = '')

if ($PidFile -and (Test-Path $PidFile)) {
    $text = Get-Content -Path $PidFile -ErrorAction SilentlyContinue | Select-Object -First 1
    $id = 0
    if ([int]::TryParse("$text", [ref]$id) -and $id -gt 0) {
        $proc = Get-Process -Id $id -ErrorAction SilentlyContinue
        # ウィンドウのタイトルで、本当にモニタかどうかを確かめてから閉じる
        if ($proc -and $proc.MainWindowTitle -eq 'SEA0004T Serial Monitor') {
            Stop-Process -Id $id -Force
            Start-Sleep -Milliseconds 700      # ポートが解放されるのを待つ
        }
    }
    Remove-Item -Path $PidFile -ErrorAction SilentlyContinue
}
exit 0
