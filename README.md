<div align="center">
    <img src="assets/cat.svg" alt="icon" height=300 />
</div>

# Lolcat++

[![Blazingly fast](https://www.blazingly.fast/api/badge.svg?repo=lolcatpp%2Flolcatpp)]()
[![GitHub License](https://img.shields.io/github/license/lolcatpp/lolcatpp?logo=opensourceinitiative&logoColor=white)](/LICENSE)
[![GitHub top language](https://img.shields.io/github/languages/top/lolcatpp/lolcatpp?logo=cplusplus)]()
[![Build Status](https://img.shields.io/github/actions/workflow/status/lolcatpp/lolcatpp/release.yml?logo=CMake)](https://github.com/lolcatpp/lolcatpp/actions)
[![GitHub Release](https://img.shields.io/github/v/release/lolcatpp/lolcatpp?logo=github&logoColor=white)](https://github.com/lolcatpp/lolcatpp/releases/latest)
[![AUR Version](https://img.shields.io/aur/version/lolcat%2B%2B?logo=archlinux&logoColor=white)](https://aur.archlinux.org/packages/lolcat++)

A rewrite of the popular utility [_LOLCAT_](https://github.com/busyloop/lolcat) in C++.
The rationale is that lolcat is often used in init scripts and such.
It can at times be rather time-consuming to run it, since it's written in Ruby.
Now it's -- naturally -- _BLAZINGLY FAST_ in C++.
It's also cross-platform.

## Example usage

**Help:**

<div align="center">
  <picture>
    <source media="(prefers-color-scheme: dark)" srcset="assets/help-dark.jpg">
    <source media="(prefers-color-scheme: light)" srcset="assets/help-light.jpg">
    <img alt="Example usage" src="assets/help-light.jpg" width="900" style="max-width: 100%;" >
  </picture>
</div>

**Example usage:** `fortune` + `cowsay` + `lolcat`

<div align="center">
  <picture>
    <source media="(prefers-color-scheme: dark)" srcset="assets/example-dark.jpg">
    <source media="(prefers-color-scheme: light)" srcset="assets/example-light.jpg">
    <img alt="Example usage" src="assets/example-light.jpg" width="900" style="max-width: 100%;">
  </picture>
</div>

## Getting it

Just head over to the [releases](https://github.com/lolcatpp/lolcatpp/releases) and download it.
Here's some copy-paste scripts for your platform:

### Arch Linux: AUR

Use your prefered AUR helper and choose whether to compile from source, or to just grab the
binary that the CI produces. This aur-package is updated automatically in the CI/CD.

**From source:**

```bash
paru -S lolcat++
```

**Binary release:**

```bash
paru -S lolcat++-bin
```

### Linux: from releases

Grab the latest release from the releases with the following command.

```bash
curl -L \
    https://github.com/lolcatpp/lolcatpp/releases/latest/download/lolcat-linux-amd64 \
    -o /tmp/lolcat
chmod +x /tmp/lolcat
sudo mv -v /tmp/lolcat /usr/local/bin/lolcat
```

### macOS: Homebrew (recommended)

There's a homebrew tap setup under [lolcatpp/homebrew-tap](https://github.com/lolcatpp/homebrew-tap).
See the repository for more detailed instructions.
It's updated automatically in the CI/CD, which means that you're getting the latest updates.
It downloads the prebuilt binary found under the releases if you're on _arm64_ or compiles from source
if your're on _x86_64_.

```bash
brew tap lolcatpp/tap
brew install lolcatpp
```

### macOS: from releases, only Apple Silicon

Grab the latest release from the releases with the following command.
Unfortunately it's a pain in the ass to compile to _x86_64_ for macOS on Github-Actions.
We recommend that you use either the _homebrew-tap_ or compile manually from source.

```bash
curl -L \
    https://github.com/lolcatpp/lolcatpp/releases/latest/download/lolcat-macos-arm64 \
    -o /tmp/lolcat
chmod +x /tmp/lolcat
sudo mv -v /tmp/lolcat /usr/local/bin/lolcat
```

### Windows: from releases (Administrator PowerShell)

Grab the latest release from the releases, and update the `$PATH` variable, with the following command.

```powershell
Invoke-WebRequest `
    -Uri "https://github.com/lolcatpp/lolcatpp/releases/latest/download/lolcat-windows-amd64.exe" `
    -OutFile "$env:TEMP\lolcat.exe"

# Move the file to the install directory
$installDir = "C:\Program Files\lolcat++"
New-Item -ItemType Directory -Force -Path $installDir | Out-Null
Move-Item -Force "$env:TEMP\lolcat.exe" "$installDir\lolcat.exe"

# Update the $PATH variable, so that it can be used as "lolcat" in the shell
$currentPath = [System.Environment]::GetEnvironmentVariable("Path", "Machine")
if ($currentPath -notlike "*$installDir*") {
    [System.Environment]::SetEnvironmentVariable("Path", $currentPath + ";$installDir", "Machine")
    Write-Host "Installation complete! Please restart your terminal."
}
```

### Building

You'll need _cmake_, boost and a C++ 20 compatible compiler.
If your compiler supports C++ 20, but doesn't support `<format>`, then you'll also need
_libfmt_ (it's used as a polyfill).
Thereafter, just run

```bash
cmake -S . -B build && cmake --build build --parallel
```

You'll now have the executable in `build/lolcat`

If you'd like to install the program, just run

```bash
cmake --install build
```

## Acknowledgement

Both the project and the codebase are heavily inspired by the [_LOLCAT_](https://github.com/busyloop/lolcat) project.
