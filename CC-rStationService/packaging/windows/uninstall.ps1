param(
  [string]$InstallRoot = "$env:ProgramFiles\CC-rStationService"
)

$ErrorActionPreference = "Stop"

$ServiceName = "CC-rStationService"
$AgentTaskName = "CC-rStationService-Agent"

if (Get-ScheduledTask -TaskName $AgentTaskName -ErrorAction SilentlyContinue) {
  Unregister-ScheduledTask -TaskName $AgentTaskName -Confirm:$false
}

$service = Get-Service -Name $ServiceName -ErrorAction SilentlyContinue
if ($null -ne $service) {
  if ($service.Status -ne "Stopped") {
    Stop-Service -Name $ServiceName -Force
  }
  sc.exe delete $ServiceName | Out-Null
}

if (Test-Path $InstallRoot) {
  Remove-Item $InstallRoot -Recurse -Force
}

Write-Host "Removed $ServiceName and $AgentTaskName"
