$projectRoot = (Get-Location).path
$scripts = ".\build\Release\YourProjectName.exe"
# Start-Process powershell -ArgumentList "-NoExit", "-Command", $scripts
Invoke-Expression -Command $scripts
Write-Host
Set-Location $projectRoot