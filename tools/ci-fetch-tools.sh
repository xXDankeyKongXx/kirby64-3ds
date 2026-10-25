#!/bin/bash
# Downloads makerom and bannertool release binaries into /tmp/bin.
set -euo pipefail
mkdir -p /tmp/bin /tmp/dl
api() { curl -fsSL -H "Authorization: Bearer $GH_TOKEN" "$1"; }
pick() { grep -o '"browser_download_url": *"[^"]*"' | sed 's/.*"\(https[^"]*\)"/\1/'; }

echo "makerom assets:"; api "https://api.github.com/repos/3DSGuy/Project_CTR/releases/tags/$MAKEROM_TAG" | pick | tee /tmp/dl/mr.txt
url=$(grep -iE 'makerom' /tmp/dl/mr.txt | grep -iE 'ubuntu|linux' | grep -iE 'x86_64|x64|amd64' | head -1)
echo "using: $url"
curl -fsSL -o /tmp/dl/makerom.zip "$url"
unzip -o -q /tmp/dl/makerom.zip -d /tmp/dl/makerom

echo "bannertool assets:"; api "https://api.github.com/repos/Steveice10/bannertool/releases/latest" | pick | tee /tmp/dl/bt.txt
url=$(grep -i '\.zip$' /tmp/dl/bt.txt | head -1)
echo "using: $url"
curl -fsSL -o /tmp/dl/bannertool.zip "$url"
unzip -o -q /tmp/dl/bannertool.zip -d /tmp/dl/bannertool
find /tmp/dl/makerom /tmp/dl/bannertool -type f

cp "$(find /tmp/dl/makerom -type f -name 'makerom*' ! -name '*.zip' | head -1)" /tmp/bin/makerom
cp "$(find /tmp/dl/bannertool -type f -ipath '*linux*' -name 'bannertool*' | grep -iE 'x86_64|x64|amd64' | head -1)" /tmp/bin/bannertool
chmod +x /tmp/bin/*
file /tmp/bin/*
/tmp/bin/makerom 2>&1 | head -2 || true
