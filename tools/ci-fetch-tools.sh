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

echo "bannertool assets:"; api "https://api.github.com/repos/diasurgical/bannertool/releases/latest" | pick | tee /tmp/dl/bt.txt
url=$(grep -i '\.zip$' /tmp/dl/bt.txt | head -1)
echo "using: $url"
curl -fsSL -o /tmp/dl/bannertool.zip "$url"
unzip -o -q /tmp/dl/bannertool.zip -d /tmp/dl/bannertool
find /tmp/dl/makerom /tmp/dl/bannertool -type f

cp "$(find /tmp/dl/makerom -type f -name 'makerom*' ! -name '*.zip' | head -1)" /tmp/bin/makerom
bt=$(find /tmp/dl/bannertool -type f -name 'bannertool*' -exec sh -c 'file "$1" | grep -q "ELF 64-bit.*x86-64" && echo "$1"' _ {} \; | head -1)
cp "$bt" /tmp/bin/bannertool
chmod +x /tmp/bin/*
file /tmp/bin/*

# Prebuilt makerom may need a newer glibc than the container; build it from source if so.
mr_out=$(/tmp/bin/makerom 2>&1 || true)
if grep -q GLIBC <<<"$mr_out"; then
	echo "Prebuilt makerom incompatible; building from source ($MAKEROM_TAG)"
	git clone -q --depth 1 -b "$MAKEROM_TAG" https://github.com/3DSGuy/Project_CTR /tmp/Project_CTR
	make -C /tmp/Project_CTR/makerom deps -j"$(nproc)"
	make -C /tmp/Project_CTR/makerom -j"$(nproc)"
	cp "$(find /tmp/Project_CTR/makerom -type f -name makerom -perm -u+x | head -1)" /tmp/bin/makerom
fi
/tmp/bin/makerom 2>&1 | head -3 || true
