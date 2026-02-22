#!/bin/bash
set -e

OS="$(uname -s)"
ARCH="$(uname -m)"

case "$OS" in
Linux)
    if [ "$ARCH" = "x86_64" ]; then
        ASSET_NAME="lolcat-linux-amd64"
    else
        echo "Unsupported architecture: $ARCH"
        exit 1
    fi
    ;;
Darwin)
    if [ "$ARCH" = "arm64" ]; then
        ASSET_NAME="lolcat-macos-arm64"
    else
        echo "For macOS x86_64, please use Homebrew or build from source."
        exit 1
    fi
    ;;
*)
    echo "Unsupported OS: $OS"
    exit 1
    ;;
esac

if command -v lolcat >/dev/null 2>&1; then
    VERSION_OUTPUT=$(lolcat --version 2>&1 || true)
    if echo "$VERSION_OUTPUT" | grep -q "lolcat++"; then
        echo ">> Found existing lolcat++ installation. Upgrading..."
    else
        echo ">> Found another lolcat installation (likely the Ruby version)."
        echo ">> It will be replaced by lolcat++ in /usr/local/bin/lolcat."
        echo ">> This might cause weird stuff, if it's installed as a system package."
        read -p ">> Do you want to continue? [y/N] " -n 1 -r </dev/tty
        echo
        if [[ ! $REPLY =~ ^[Yy]$ ]]; then
            echo "Installation cancelled."
            exit 1
        fi
    fi
fi

echo ">> Downloading lolcat++ ($ASSET_NAME)..."
TEMP_FILE=$(mktemp)
curl --fail --show-error --location \
    "https://github.com/lolcatpp/lolcatpp/releases/latest/download/$ASSET_NAME" \
    -o "$TEMP_FILE"

if head -n 1 "$TEMP_FILE" | grep -qi "<!DOCTYPE html>"; then
    echo ">> Error: Download failed. Your network or firewall (probably) intercepted the download and returned an HTML page."
    rm "$TEMP_FILE"
    exit 1
fi

chmod +x "$TEMP_FILE"

echo ">> Installing to /usr/local/bin/lolcat..."
sudo mv -v "$TEMP_FILE" /usr/local/bin/lolcat

echo ">> Done! Try running 'lolcat --help'"
