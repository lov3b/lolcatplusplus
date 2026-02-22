$ErrorActionPreference = 'Stop'

$existingLolcat = Get-Command lolcat -ErrorAction SilentlyContinue
if ($existingLolcat) {
    $versionOutput = & lolcat --version 2>&1 | Out-String
    if ($versionOutput -like "*lolcat++*") {
        Write-Host ">> Found existing lolcat++ installation. Upgrading..."
    } else {
        Write-Host ">> Found another lolcat installation."
        Write-Host ">> It will be replaced by lolcat++."
        $confirmation = Read-Host ">> Do you want to continue? [y/N]"
        if ($confirmation -notmatch '[Yy]') {
            Write-Host "Installation cancelled."
            exit
        }
    }
}

$assetName = "lolcat-windows-amd64.exe"
$url = "https://github.com/lolcatpp/lolcatpp/releases/latest/download/$assetName"
$tempFile = Join-Path $env:TEMP "lolcat.exe"

Write-Host ">> Downloading lolcat++ ($assetName)..."
Invoke-WebRequest -Uri $url -OutFile $tempFile

$firstLine = Get-Content -Path $tempFile -TotalCount 1 -ErrorAction SilentlyContinue
if ($firstLine -match "<!DOCTYPE html>") {
    Write-Host ">> Error: Download failed. Your network or firewall (probably) intercepted the download and returned an HTML page." -ForegroundColor Red
    Remove-Item -Path $tempFile -Force
    exit
}

$installDir = "C:\Program Files\lolcat++"
Write-Host ">> Installing to $installDir..."
if (-not (Test-Path $installDir)) {
    New-Item -ItemType Directory -Force -Path $installDir | Out-Null
}
Move-Item -Force $tempFile (Join-Path $installDir "lolcat.exe")

$currentPath = [System.Environment]::GetEnvironmentVariable("Path", "Machine")
if ($currentPath -notlike "*$installDir*") {
    Write-Host ">> Adding $installDir to Machine PATH..."
    [System.Environment]::SetEnvironmentVariable("Path", $currentPath + ";$installDir", "Machine")
    Write-Host "Installation complete! Please restart your terminal."
} else {
    Write-Host "Installation complete!"
}
