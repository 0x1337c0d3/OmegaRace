# OmegaRace Build Scripts

This directory contains automated scripts for building and distributing OmegaRace.

## Scripts

### `build_release.sh`
Simple script to build the Release version of OmegaRace.

**Usage:**
```bash
./build_release.sh [--dmg] [--open]
```

**Options:**
- `--dmg` - Create a DMG file after building
- `--open` - Open the distribution directory when done

**Examples:**
```bash
# Just build Release version
./build_release.sh

# Build and create DMG
./build_release.sh --dmg

# Build, create DMG, and open dist folder
./build_release.sh --dmg --open
```

### `build_release_dmg.sh`
Complete script that builds Release version and creates a distribution-ready DMG file.

**Usage:**
```bash
./build_release_dmg.sh
```

**Features:**
- Cleans previous builds
- Builds optimized Release version
- Creates professional DMG with:
  - App bundle
  - Applications folder symlink
  - Proper volume naming
  - Compressed format
- Automatic version detection from Info.plist
- Date-stamped naming
- Opens Finder to distribution folder

## Output

Both scripts create files in the `dist/` directory:
- `OmegaRace.app` - The application bundle (when using `build_release.sh`)
- `OmegaRace-YYYYMMDD.dmg` - Distribution-ready disk image

## Distribution

The DMG file is ready for distribution. Users can:
1. Download and mount the DMG file
2. Drag OmegaRace.app to the Applications folder
3. Launch from Applications or Launchpad

## Requirements

- Xcode Command Line Tools
- macOS 11.0 or later (target)
- All project dependencies (raylib, FMOD, etc.)

## Notes

- The Release build uses `-Ofast` optimization for maximum performance
- All dependencies are bundled within the app
- Code signing is applied automatically with your developer certificate
- DMG files are compressed for smaller download size
