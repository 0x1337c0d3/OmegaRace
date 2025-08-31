# OmegaRace Workspace Configuration

This directory contains the refactored OmegaRace project with a modern, clean structure.

## Directory Structure

```
OmegaRace/                    # Root project directory
├── src/                      # Source code (organized by functionality)
│   ├── main.cpp             # Application entry point
│   ├── core/                # Core game systems
│   ├── entities/            # Game objects and logic
│   ├── graphics/            # Rendering and visual effects
│   └── audio/               # Sound and music systems
├── include/                  # Public header files
├── resources/                # Game assets
│   ├── audio/               # Sound effects and music
│   ├── shaders/             # Graphics shaders
│   └── textures/            # Images and sprites
├── third-party/             # External dependencies
├── build/                   # Build output (generated)
├── scripts/                 # Build and utility scripts
├── docs/                    # Documentation
└── CMakeLists.txt          # Modern CMake build configuration
```

## Quick Start

1. **Build**: `./scripts/build.sh`
2. **Run**: `./build/OmegaRace` (or `./build/OmegaRace.app` on macOS)
3. **Format**: `./scripts/format.sh`

## Migration from Old Structure

The old nested `Applications/OmegaRace/OmegaRace/src/` structure has been flattened and organized by functionality:

- **Core Systems** → `src/core/`
- **Game Entities** → `src/entities/`  
- **Graphics & Rendering** → `src/graphics/`
- **Audio Systems** → `src/audio/`
- **Game Assets** → `resources/`
- **Build Scripts** → `scripts/`
- **Dependencies** → `third-party/`

## Benefits

✅ **Cleaner Organization** - Logical separation by functionality
✅ **Modern Build System** - CMake instead of Xcode-only
✅ **Cross-Platform Ready** - Windows, macOS, and Linux support
✅ **Standard Structure** - Follows C++ project conventions
✅ **Easier Navigation** - No more deep nested directories
✅ **Better Maintainability** - Clear separation of concerns

## Legacy Support

The old `Applications/` directory structure is preserved for reference during the transition.
