param(
  [string]$InstallRoot = "$env:ProgramFiles\CC-rStationService",
  [string]$ConfigPath = "$env:ProgramFiles\CC-rStationService\CC-rStationService.toml",
  [string]$AgentUser = $env:USERNAME
)

$ErrorActionPreference = "Stop"

$Root = Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
$ServiceBinary = Join-Path $Root "target\release\cc-rstationservice.exe"
$AgentBinary = Join-Path $Root "target\release\cc-rstationservice-agent.exe"
$ConfigSource = Join-Path $Root "CC-rStationService.toml"
$ServiceName = "CC-rStationService"
$AgentTaskName = "CC-rStationService-Agent"
$AuthToken = [Convert]::ToHexString([System.Security.Cryptography.RandomNumberGenerator]::GetBytes(32)).ToLowerInvariant()

if (-not (Test-Path $ServiceBinary) -or -not (Test-Path $AgentBinary)) {
  throw "release binaries missing, run cargo build --release first"
}

New-Item -ItemType Directory -Force -Path $InstallRoot | Out-Null
Copy-Item $ServiceBinary (Join-Path $InstallRoot "cc-rstationservice.exe") -Force
Copy-Item $AgentBinary (Join-Path $InstallRoot "cc-rstationservice-agent.exe") -Force

Copy-Item $ConfigSource $ConfigPath -Force
$ConfigContent = Get-Content $ConfigPath -Raw
$ConfigContent = [System.Text.RegularExpressions.Regex]::Replace($ConfigContent, '^auth_token = .*$', "auth_token = `"$AuthToken`"", 'Multiline')
Set-Content -Path $ConfigPath -Value $ConfigContent -NoNewline

$ServiceExe = Join-Path $InstallRoot "cc-rstationservice.exe"
$ServiceCommand = "`"$ServiceExe`" --config `"$ConfigPath`" foreground"

$serviceExists = Get-Service -Name $ServiceName -ErrorAction SilentlyContinue
if ($null -eq $serviceExists) {
  sc.exe create $ServiceName binPath= $ServiceCommand start= auto | Out-Null
} else {
  sc.exe config $ServiceName binPath= $ServiceCommand start= auto | Out-Null
}

Start-Service -Name $ServiceName

$AgentExe = Join-Path $InstallRoot "cc-rstationservice-agent.exe"
$AgentAction = New-ScheduledTaskAction -Execute $AgentExe -Argument "--config `"$ConfigPath`""
$AgentTrigger = New-ScheduledTaskTrigger -AtLogOn -User $AgentUser
$AgentPrincipal = New-ScheduledTaskPrincipal -UserId $AgentUser -LogonType Interactive -RunLevel Highest
Register-ScheduledTask -TaskName $AgentTaskName -Action $AgentAction -Trigger $AgentTrigger -Principal $AgentPrincipal -Force | Out-Null

Write-Host "Installed $ServiceName and scheduled desktop agent task $AgentTaskName"
