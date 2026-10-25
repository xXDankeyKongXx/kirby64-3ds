#!/bin/bash
set -euo pipefail
mkdir -p drop
gh api repos/azahar-emu/azahar/releases/latest --jq '.tag_name, (.assets[].name)' | tee drop/azahar-assets.txt
name=$(grep -iE 'appimage' drop/azahar-assets.txt | grep -viE 'room|aarch64|arm64|zsync' | head -1 || true)
[ -z "$name" ] && name=$(grep -iE 'linux' drop/azahar-assets.txt | grep -viE 'room|aarch64|arm64|zsync' | head -1)
echo "Using: $name"
gh release download -R azahar-emu/azahar -p "$name" -D drop
gh release download -R "$GITHUB_REPOSITORY" -p 'Kirby64.*' -D drop
ls -la drop
echo "::notice title=Drop::$(ls drop | tr '\n' ' ')"
