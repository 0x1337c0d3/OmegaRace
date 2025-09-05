# Workspace Documentation

This document describes the development workspace structure and tools for the OmegaRace project.

## 📁 Project Structure

```
OmegaRace/
├── README.md                     # Main project documentation
├── CMakeLists.txt               # Build configuration
├── format.sh                    # Code formatting script
├── .gitignore                   # Git ignore rules
├── 
├── docs/                        # Documentation
│   ├── BUILD_README.md         # Detailed build instructions
│   └── WORKSPACE.md            # This file
├── 
├── scripts/                     # Build and utility scripts
│   ├── build.sh                # Cross-platform development builds
│   └── build_release_xcode.sh  # Production macOS builds with distribution
├── 
├── src/                         # Source code
│   ├── main.cpp                # Application entry point
│   ├── core/                   # Core game systems
│   ├── entities/               # Game objects and logic
│   ├── graphics/               # Rendering and visual effects
│   └── audio/                  # Sound and music systems
├── 
├── Applications/                # Xcode workspace and projects
│   └── OmegaRace/              # Xcode-generated files
├── 
├── ThirdParty/                  # External dependencies
│   ├── fmod/                   # FMOD audio library
│   └── SDL2/                   # SDL2 frameworks (macOS)
├── 
├── build/                       # CMake build files (generated)
├── build_xcode/                 # Xcode build files (generated)
└── dist/                        # Distribution files (generated)
    └── *.dmg                   # macOS distribution packages
```

## 🛠️ Development Tools

### Build System
- **CMake 3.15+** - Primary build system
- **Xcode** - Native macOS development and distribution
- **Universal Binaries** - Apple Silicon + Intel support

### Dependencies
- **BGFX** - Cross-platform rendering (auto-fetched via CMake)
- **SDL2** - Window management and input
- **FMOD** - Audio engine (included in ThirdParty/)

### Code Quality
- **C++17** - Modern C++ standard
- **format.sh** - Code formatting automation
- **Consistent style** - Unified coding conventions

## 🚀 Development Workflow

### 1. Setup Development Environment

#### macOS
```bash
# Install dependencies
brew install cmake sdl2

# Install Xcode Command Line Tools
xcode-select --install

# Clone and build
git clone <repository-url>
cd OmegaRace
./scripts/build.sh
```

#### Linux (Ubuntu)
```bash
# Install dependencies
sudo apt install cmake libsdl2-dev build-essential

# Build
./scripts/build.sh
```

### 2. Development Builds

```bash
# Quick development build
./scripts/build.sh

# Debug build with symbols
./scripts/build.sh Debug

# Run after building
build/OmegaRace                                    # Linux
open build/Debug/OmegaRace.app                     # macOS
```

### 3. Code Formatting

```bash
# Format all source files
./format.sh
```

### 4. Production Distribution (macOS)

```bash
# Create signed app and distribution DMG
./scripts/build_release_xcode.sh

# Output: dist/OmegaRace-YYYYMMDD.dmg
```

## 📋 Development Guidelines

### Code Style
- Use modern C++17 features
- RAII and smart pointers preferred
- Consistent naming conventions
- Document public interfaces

### Build Practices
- Separate debug and release builds
- Test on both Apple Silicon and Intel Macs
- Verify code signing for distribution builds
- Clean builds for distribution

### Version Control
- Use meaningful commit messages
- Keep commits focused and atomic
- Test builds before pushing changes

## 🔧 IDE Configuration

### VS Code (Recommended)
Recommended extensions:
- C/C++ Extension Pack
- CMake Tools
- GitLens
- Clang-Format

Settings for VS Code:
```json
{
    "cmake.configureOnOpen": true,
    "cmake.buildDirectory": "${workspaceFolder}/build",
    "C_Cpp.default.configurationProvider": "ms-vscode.cmake-tools"
}
```

### Xcode
The project can generate Xcode projects:
```bash
cmake -G Xcode -B build_xcode
open build_xcode/OmegaRace.xcodeproj
```

### CLion
CLion can import CMake projects directly:
1. Open folder in CLion
2. CMake will auto-configure
3. Select appropriate build configuration

## 🐛 Debugging

### Debug Builds
```bash
# Build with debug symbols
./scripts/build.sh Debug

# Run with debugger
lldb build/OmegaRace              # macOS
gdb build/OmegaRace               # Linux
```

### Xcode Debugging
```bash
# Generate Xcode project with debug configuration
cmake -G Xcode -DCMAKE_BUILD_TYPE=Debug -B build_xcode
open build_xcode/OmegaRace.xcodeproj

# Set breakpoints and debug in Xcode UI
```

### Common Debug Tasks
- **Memory issues**: Use AddressSanitizer (`-DCMAKE_CXX_FLAGS=-fsanitize=address`)
- **Performance**: Use instruments on macOS or perf on Linux
- **Graphics**: Enable BGFX debug mode for rendering validation

## 📊 Performance Profiling

### macOS (Instruments)
```bash
# Build with debug info but optimized
./scripts/build.sh RelWithDebInfo

# Profile with Instruments
instruments -t "Time Profiler" build/OmegaRace.app
```

### Linux (perf)
```bash
# Install perf tools
sudo apt install linux-perf

# Profile application
perf record build/OmegaRace
perf report
```

## 🧪 Testing

### Build Verification
```bash
# Test release build
./scripts/build.sh Release
build/OmegaRace

# Test debug build
./scripts/build.sh Debug
build/OmegaRace

# Test distribution build (macOS)
./scripts/build_release_xcode.sh
open dist/*.dmg
```

### Cross-Platform Testing
- Test on different macOS versions
- Verify universal binary compatibility
- Test with different hardware configurations

## 📦 Distribution

### macOS Distribution Process
1. **Development**: Use `./scripts/build.sh` for testing
2. **Code Signing**: Ensure valid Developer ID certificate
3. **Distribution Build**: Run `./scripts/build_release_xcode.sh`
4. **Quality Assurance**: Test DMG on clean system
5. **Release**: Distribute `dist/OmegaRace-*.dmg`

### Distribution Checklist
- [ ] Code signing successful
- [ ] Universal binary (arm64 + x86_64)
- [ ] All frameworks included and signed
- [ ] DMG mounts correctly
- [ ] App runs from DMG installation
- [ ] No missing dependencies
- [ ] Proper volume and app icons

## 🔍 Troubleshooting

### Build Issues
- **CMake errors**: Check CMake version (3.15+ required)
- **Missing dependencies**: Verify SDL2 installation
- **Permission errors**: `chmod +x scripts/*.sh`

### Runtime Issues
- **Graphics errors**: Update graphics drivers
- **Audio problems**: Check FMOD library inclusion
- **Input issues**: Verify SDL2 functionality

### Code Signing Issues (macOS)
- **Certificate not found**: Install from Developer Portal
- **Keychain access**: Allow codesign in Keychain Access
- **Wrong certificate**: Update hash in build script

---

*This workspace is optimized for modern C++ development with cross-platform support and professional macOS distribution.*
