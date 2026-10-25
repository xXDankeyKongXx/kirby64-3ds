#!/bin/bash
# Packages the built ELF into Kirby64.3dsx and Kirby64.cia.
# Usage: tools/package.sh <path-to-elf> <output-dir>
set -euo pipefail

ELF="$1"
OUT="$2"
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
RES="$ROOT/Source/SysCTR/Resources"

APP_TITLE="Kirby 64"
APP_DESC="The Crystal Shards - 3DS port"
APP_AUTHOR="xXDankeyKongXx"

mkdir -p "$OUT"

smdhtool --create "$APP_TITLE" "$APP_DESC" "$APP_AUTHOR" "$RES/icon.png" "$OUT/Kirby64.smdh"

3dsxtool "$ELF" "$OUT/Kirby64.3dsx" --romfs="$RES/romfs" --smdh="$OUT/Kirby64.smdh"

bannertool makebanner -i "$RES/banner.png" -a "$RES/audio_silent.wav" -o "$OUT/banner.bnr"

makerom -f cia -target t -exefslogo \
	-o "$OUT/Kirby64.cia" \
	-elf "$ELF" \
	-rsf "$RES/kirby64.rsf" \
	-banner "$OUT/banner.bnr" \
	-icon "$OUT/Kirby64.smdh" \
	-DAPP_ROMFS="$RES/romfs" \
	-major 1 -minor 0 -micro 0

rm -f "$OUT/banner.bnr" "$OUT/Kirby64.smdh"
ls -la "$OUT"
