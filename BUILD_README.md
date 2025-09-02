# OmegaRace Build System

This directory contains build scripts and documentation for building OmegaRace across multiple platforms.

## Build Technologies

- **CMake 3.15+** - Cross-platform build system
- **BGFX** - Cross-platform rendering library (auto-fetched)
- **SDL2** - Window management and input
- **FMOD** - Audio engine (included in third-party/)

## Quick Start

### Cross-Platform Build
```bash
# Configure and build (Release mode)
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build . -j$(nproc)

# Run the game
./Debug/OmegaRace.app/Contents/MacOS/OmegaRace  # macOS
./OmegaRace                                      # Linux/Windows
```

### Platform-Specific Setup

#### macOS
```bash
# Install dependencies
brew install cmake sdl2

# Build and package
./build_release_dmg.sh  # Creates distributable DMG
```

#### Ubuntu/Linux  
```bash
# Install dependencies
sudo apt install cmake libsdl2-dev build-essential

# Build
mkdir build && cd build
cmake .. && make -j$(nproc)
```

#### Windows
```powershell
# Using vcpkg for dependencies
vcpkg install sdl2:x64-windows

# Build with Visual Studio
mkdir build && cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=C:/vcpkg/scripts/buildsystems/vcpkg.cmake
cmake --build . --config Release
```

## Build Scripts (macOS)

### `build_release.sh`
macOS-specific script to build the Release version of OmegaRace using CMake.

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
Complete macOS distribution script that builds Release version and creates a distribution-ready DMG file.

**Usage:**
```bash
./build_release_dmg.sh
```

**Features:**
- Cleans previous builds
- Configures CMake for optimized Release build
- Builds with BGFX rendering backend
- Creates professional DMG with:
  - App bundle with proper signing
  - Applications folder symlink
  - Proper volume naming and icon
  - Compressed format for distribution
- Automatic version detection from CMake
- Date-stamped naming
- Opens Finder to distribution folder

### Cross-Platform Build Script
```bash
# Generic build script for all platforms
./scripts/build.sh [Debug|Release] [--clean]
```

## CMake Configuration Options

```bash
# Build types
-DCMAKE_BUILD_TYPE=Debug|Release|RelWithDebInfo|MinSizeRel

# Platform-specific options
-DBGFX_BUILD_EXAMPLES=OFF          # Disable BGFX examples (default)
-DBGFX_BUILD_TOOLS=ON              # Enable shader tools (default)
-DCMAKE_OSX_DEPLOYMENT_TARGET=11.0 # macOS minimum version

# Example configurations
cmake -DCMAKE_BUILD_TYPE=Debug ..                    # Debug build
cmake -DCMAKE_BUILD_TYPE=Release ..                  # Optimized build
cmake -DCMAKE_BUILD_TYPE=RelWithDebInfo ..           # Release with debug info
```

## Dependencies

### Automatically Fetched
- **BGFX** - Cross-platform rendering library (fetched via CMake)
- **BX** - Base library for BGFX (fetched as BGFX dependency)  
- **BIMG** - Image library for BGFX (fetched as BGFX dependency)

### Platform Dependencies
- **SDL2** - Window management and input
  - macOS: `brew install sdl2`
  - Ubuntu: `sudo apt install libsdl2-dev`
  - Windows: `vcpkg install sdl2:x64-windows`

### Included Dependencies  
- **FMOD** - Audio engine (included in `third-party/fmod/`)
- **Custom Math** - Vector mathematics library (in `src/core/vmath.cpp`)

## Output

### macOS Build Outputs
Build scripts create files in the `dist/` directory:
- `OmegaRace.app` - The application bundle (when using `build_release.sh`)
- `OmegaRace-YYYYMMDD.dmg` - Distribution-ready disk image

### Cross-Platform Build Outputs
CMake builds create platform-specific outputs in `build/`:
- **macOS**: `build/Debug/OmegaRace.app/` - App bundle
- **Linux**: `build/OmegaRace` - Executable binary  
- **Windows**: `build/Debug/OmegaRace.exe` - Executable

## Distribution

### macOS Distribution
The DMG file is ready for distribution. Users can:
1. Download and mount the DMG file
2. Drag OmegaRace.app to the Applications folder
3. Launch from Applications or Launchpad

### Cross-Platform Distribution
- **Linux**: Package the executable with required shared libraries
- **Windows**: Use installers or portable executable packages
- **All platforms**: Include FMOD libraries and audio assets

## Requirements

### Build Requirements
- **CMake 3.15+** - Modern build system
- **C++17 Compiler** - GCC 7+, Clang 5+, MSVC 2017+  
- **SDL2** - Window management and input library
- **Git** - For fetching BGFX dependencies

### Platform-Specific Requirements
- **macOS**: Xcode Command Line Tools, Metal support
- **Linux**: GCC/Clang, OpenGL/Vulkan drivers  
- **Windows**: Visual Studio 2017+, DirectX 11+ support

### Runtime Requirements
- **Graphics**: DirectX 11/Metal/Vulkan/OpenGL 3.3+ support
- **Audio**: System audio support for FMOD
- **Storage**: ~200MB for game assets and dependencies

## Notes

### Build Optimizations
- The Release build uses `-O3` optimization for maximum performance
- BGFX is configured for optimal runtime performance (examples disabled)
- Shader compilation occurs at build time for faster game startup
- Debug builds include full debugging symbols and runtime checks

### Graphics Backend
- **macOS**: Uses Metal backend for optimal performance
- **Windows**: Uses DirectX 11/12 backend  
- **Linux**: Uses OpenGL or Vulkan backend based on driver support
- **Cross-platform**: BGFX automatically selects the best available backend

### Distribution Notes
- macOS apps are code-signed automatically with developer certificate
- DMG files are compressed for smaller download size
- All dependencies are bundled within the app/executable
- FMOD audio libraries are included for all platforms

### Development
- First build downloads and compiles BGFX (~5-10 minutes)
- Subsequent builds are incremental and much faster
- Shader tools are built for custom shader development
- Debug mode enables BGFX validation and profiling
