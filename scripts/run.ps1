$OutputEncoding = [System.Text.Encoding]::UTF8
$projectRoot = (Get-Location).path
$scripts = ".\build\Debug\chttpd.exe"
# Start-Process powershell -ArgumentList "-NoExit", "-Command", $scripts
Invoke-Expression -Command $scripts
Write-Host
Set-Location $projectRoot