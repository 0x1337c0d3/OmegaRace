# Build Documentation

This document provides detailed build instructions and configuration options for the OmegaRace project.

## 🚀 Quick Start

### Production Builds (macOS)

For creating distribution-ready builds with code signing and DMG creation:

```bash
# Complete build and distribution pipeline
./scripts/build_release_xcode.sh
```

This creates:
- Universal binary (Apple Silicon + Intel)
- Code-signed app bundle and frameworks
- Professional DMG in `dist/` directory
- Ready for distribution to users

### Development Builds (All Platforms)

For development and testing:

```bash
# Release build
./scripts/build.sh

# Debug build with debugging symbols
./scripts/build.sh Debug
```

## 🛠️ Build Scripts

### `scripts/build_release_xcode.sh`

**Purpose**: Complete production build and distribution pipeline for macOS

**Features**:
- Uses Xcode generator for native macOS development
- Creates universal binaries (arm64 + x86_64)
- Automatic code signing with developer certificate
- Professional DMG creation with proper structure
- Comprehensive error checking and validation

**Usage**:
```bash
# Standard build with DMG creation
./scripts/build_release_xcode.sh

# Build and launch app after completion
./scripts/build_release_xcode.sh --run
```

**Output**:
- Built app: `build_xcode/Release/OmegaRace.app`
- Distribution DMG: `dist/OmegaRace-YYYYMMDD.dmg`

**Requirements**:
- macOS 11.0+
- Xcode Command Line Tools
- Valid Apple Developer certificate for code signing
- CMake 3.15+

### `scripts/build.sh`

**Purpose**: Cross-platform development builds

**Usage**:
```bash
# Release build (optimized)
./scripts/build.sh

# Debug build (with debugging symbols)
./scripts/build.sh Debug

# Clean build (removes build directory first)
./scripts/build.sh Release clean
```

**Output**:
- Built executable in `build/` directory
- Platform-specific binary format

## 📋 System Requirements

### Build Dependencies

#### macOS
```bash
# Install via Homebrew
brew install cmake sdl2

# Xcode Command Line Tools
xcode-select --install
```

#### Ubuntu/Linux
```bash
sudo apt update
sudo apt install \
    cmake \
    libsdl2-dev \
    build-essential \
    libgl1-mesa-dev \
    libx11-dev
```

#### Windows
```powershell
# Using vcpkg
vcpkg install sdl2:x64-windows

# Or using Chocolatey
choco install cmake
```

### Runtime Requirements

#### Minimum
- **OS**: macOS 11.0+, Windows 10+, Ubuntu 20.04+
- **CPU**: 64-bit processor
- **RAM**: 2GB available
- **Graphics**: DirectX 11, Metal, Vulkan, or OpenGL 3.3+

#### Recommended
- **CPU**: Multi-core processor
- **RAM**: 4GB available
- **Graphics**: Dedicated GPU with modern drivers
- **Display**: High-DPI support for enhanced visuals

## 🔧 Manual Build Process

For advanced users or when scripts don't work:

### Step 1: Configure
```bash
# Create build directory
mkdir build && cd build

# Configure with CMake
cmake -DCMAKE_BUILD_TYPE=Release ..

# Alternative: Use Xcode generator (macOS only)
cmake -G Xcode -DCMAKE_BUILD_TYPE=Release ..
```

### Step 2: Build
```bash
# Build with CMake
cmake --build . --config Release

# Alternative: Use native build system
make -j$(nproc)                    # Linux/macOS with Makefiles
xcodebuild -configuration Release  # macOS with Xcode
```

### Step 3: Run
```bash
# macOS
./Release/OmegaRace.app/Contents/MacOS/OmegaRace

# Linux
./OmegaRace

# Windows
./Release/OmegaRace.exe
```

## 📦 Code Signing (macOS)

The build system automatically handles code signing for distribution builds.

### Requirements
- Apple Developer Program membership
- Valid Developer ID certificate installed in Keychain
- Certificate configured in build script

### Manual Signing
If automatic signing fails:

```bash
# Sign frameworks
codesign --sign "Developer ID Application: Your Name (TEAMID)" \
         --options=runtime \
         OmegaRace.app/Contents/Frameworks/*.dylib

# Sign main app bundle
codesign --sign "Developer ID Application: Your Name (TEAMID)" \
         --options=runtime \
         --entitlements entitlements.plist \
         OmegaRace.app

# Verify signature
codesign --verify --deep --strict OmegaRace.app
```

## 🗂️ Build Configuration

### CMake Options

Common configuration options:

```bash
# Build type
-DCMAKE_BUILD_TYPE=Release|Debug|RelWithDebInfo

# Generator selection
-G "Unix Makefiles"     # Default on Linux/macOS
-G "Xcode"              # Xcode project files
-G "Visual Studio 16"   # Visual Studio solution

# Custom installation prefix
-DCMAKE_INSTALL_PREFIX=/usr/local

# Force specific compiler
-DCMAKE_C_COMPILER=clang
-DCMAKE_CXX_COMPILER=clang++
```

### Environment Variables

Useful environment variables:

```bash
# Parallel build jobs
export CMAKE_BUILD_PARALLEL_LEVEL=8

# Compiler selection
export CC=clang
export CXX=clang++

# SDK paths (macOS)
export SDKROOT=$(xcrun --show-sdk-path)
```

## 🐛 Troubleshooting

### Common Issues

#### "CMake not found"
```bash
# macOS
brew install cmake

# Ubuntu
sudo apt install cmake

# Windows
winget install cmake
```

#### "SDL2 not found"
```bash
# macOS
brew install sdl2

# Ubuntu
sudo apt install libsdl2-dev

# Windows
vcpkg install sdl2:x64-windows
```

#### "Code signing failed" (macOS)
1. Check if Developer ID certificate is installed:
   ```bash
   security find-identity -v -p codesigning
   ```
2. Update certificate hash in build script
3. Ensure Keychain Access allows codesign to access certificate

#### "Permission denied" on scripts
```bash
# Make scripts executable
chmod +x scripts/*.sh
```

### Build Directories

The project uses separate build directories for different configurations:

- `build/` - Default build directory for development
- `build_xcode/` - Xcode-specific build files
- `dist/` - Distribution files (DMGs, packages)

### Cleaning Builds

To start fresh:

```bash
# Remove all build directories
rm -rf build build_xcode dist

# Or use build script clean option
./scripts/build.sh Release clean
```

## 📊 Performance Notes

### Build Performance
- Use parallel builds: `cmake --build . -j$(nproc)`
- Enable ccache for faster rebuilds (if available)
- Consider using Ninja generator for faster builds

### Runtime Performance
- Release builds are significantly faster than Debug
- Use RelWithDebInfo for profiling with optimization
- Modern GPUs provide better rendering performance

## 📝 Development Notes

### Debug Builds
Debug builds include:
- Debugging symbols for gdb/lldb
- Assertions and debug logging
- Unoptimized code for accurate debugging
- Additional runtime checks

### Release Builds
Release builds include:
- Full compiler optimizations
- Stripped debugging symbols
- Minimal logging output
- Maximum performance

---

*For questions about building or build issues, check the main README.md or create an issue with build environment details.*
