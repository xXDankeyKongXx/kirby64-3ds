# Kirby 64 3DS

A standalone 3DS app that boots straight into **Kirby 64: The Crystal Shards**, built on [DaedalusX64-3DS](https://github.com/masterfeizz/DaedalusX64-3DS) by MasterFeizz and contributors.

**No game data is included.** Supply your own ROM dump at `sdmc:/3ds/Kirby64/kirby64.z64`. See [docs/INSTALL.txt](docs/INSTALL.txt).

## Changes from upstream DaedalusX64-3DS
- Skips the ROM browser and boots the Kirby 64 ROM directly; shows a help screen if it's missing.
- Own data folder (`sdmc:/3ds/Kirby64/`); `roms.ini` and cheats are seeded from RomFS on first run.
- Own title (`Kirby 64`, unique ID `0xF4B64`), icon and banner, so it installs alongside other apps.
- Packaging moved to `tools/package.sh`; built by GitHub Actions.

## Building
Every push to `main` runs `.github/workflows/build.yml` in the `devkitpro/devkitarm` container and uploads `Kirby64.cia` + `Kirby64.3dsx` as a workflow artifact. Pushing a `v*` tag also publishes a release.

## License
GPLv2, same as DaedalusX64 (see `copying.txt`). Icon and banner are original art.
