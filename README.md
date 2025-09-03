# Omega Race

A modern recreation of the classic 1981 arcade game Omega Race, built with C++ and BGFX for cross-platform performance.

![Game Version](https://img.shields.io/badge/version-1.0-blue.svg)
![Platform](https://img.shields.io/badge/platform-Cross--Platform-lightgrey.svg)
![Language](https://img.shields.io/badge/l### Background Grid
- **Grid Alpha** - Fine-tune background grid transparency to minimize UI interference
- **Grid Distortion** - Optimize grid distortion effects for performance
- **Rendering Order** - Ensure proper depth sorting between grid, game objects, and UI

## 🎮 TODO - Game Features

### Power-Up System
The following power-ups are planned for implementation:

#### Core Power-Ups
- **Extra Life** - Grants an additional ship/life to the player
  - Rare spawn rate to maintain game balance
  - Visual indicator when collected
  - Audio feedback for collection

- **Homing Missiles** - Secondary weapon system with target-seeking projectiles
  - Activate with secondary fire button (different from primary weapon)
  - Limited ammunition (3-5 missiles per power-up)
  - Smart targeting system that locks onto nearest enemy
  - Visual trail and guidance system for missiles
  - Enhanced damage compared to regular shots

- **Nuclear Weapon** - Screen-clearing super weapon
  - Destroys all enemies currently on screen
  - Massive explosion effect with screen shake
  - Very rare spawn rate for balance
  - Audio and visual warning before detonation
  - Bonus points for enemies destroyed

- **Ship Shield** - Protective barrier system
  - Absorbs damage from enemy collisions and projectiles
  - Dual degradation system:
    - Reduces strength when player takes hits
    - Slowly depletes over time (timer-based)
  - Visual shield effect around player ship
  - Audio feedback for shield activation/depletion
  - Multiple hit protection before destruction

#### Power-Up Mechanics
- **Spawn System** - Random appearance after destroying enemy waves
- **Collection** - Fly into power-up to collect
- **Visual Design** - Distinct icons and colors for each power-up type
- **Rarity Levels** - Balance power vs. availability
- **Duration Indicators** - Show remaining time for temporary effects
- **Stacking Rules** - Define which power-ups can be combined

## 📞 Supporte-C++-blue.svg)
![Graphics](https://img.shields.io/badge/graphics-BGFX-red.svg)
![Framework](https://img.shields.io/badge/framework-SDL2-green.svg)

## 🎮 About

Omega Race is a fast-paced space combat game where you pilot a fighter ship in an enclosed arena, battling against waves of enemy ships and avoiding deadly mines. Navigate the unique track-based arena while managing momentum and inertia in classic arcade style.

### Key Features

- **Classic Arcade Gameplay** - Faithful recreation of the original Omega Race mechanics
- **Vector-Style Graphics** - Clean line-based visuals with enhanced modern effects
- **Multiple Enemy Types** - Face various enemy ships with different behaviors:
  - **Lead Enemies** - Aggressive attackers that pursue the player
  - **Follow Enemies** - Ships that follow predetermined paths
  - **Fighter Ships** - Advanced enemies with shooting capabilities
  - **Mines** - Stationary and mobile explosive hazards
- **Progressive Difficulty** - Increasingly challenging waves with more enemies
- **Audio Effects** - Immersive sound design using FMOD audio engine
- **Game Controller Support** - Full DirectionalGamepad controller support
- **High-Resolution Display** - Optimized for modern Retina displays

## 🕹️ Gameplay

### Objective
Survive waves of enemies while scoring points by destroying ships and avoiding mines. Navigate the enclosed arena using realistic physics with momentum and inertia.

### Controls
- **Movement** - Thrust and rotate your ship to navigate
- **Shooting** - Fire projectiles to destroy enemies
- **Momentum** - Master the physics-based movement system

### Enemies
- **TriShip Enemies** - Triangular ships with different AI behaviors
- **Fighter Ships** - Advanced enemies that can shoot back
- **Mines** - Explosive hazards that must be avoided or carefully destroyed
- **UFOs** - Special bonus enemies (when present)

### Scoring
- Destroy enemies to earn points
- Survive longer for higher scores
- Bonus points for difficult targets

## 🛠️ Technical Details

### Built With
- **C++17** - Modern C++ with smart pointers and RAII
- **BGFX** - Cross-platform rendering library with Metal/DirectX/Vulkan/OpenGL backends
- **SDL2** - Cross-platform window management and input handling
- **FMOD** - Professional audio engine for sound effects
- **Vector Mathematics** - Custom math library for precise physics
- **Entity Component System** - Modular game object architecture

### Architecture
```
src/
├── main.cpp                   # Application entry point
├── core/                      # Core systems
│   ├── Game.cpp/h            # Main game loop and application management
│   ├── GameController.cpp/h  # Core game logic and state management
│   ├── Timer.cpp/h           # Timing utilities
│   ├── Common.cpp/h          # Shared definitions and utilities
│   ├── vmath.cpp/h           # Vector mathematics library
│   └── Logger.cpp/h          # Logging system
├── entities/                  # Game objects
│   ├── Entity.cpp/h          # Base entity class
│   ├── Player.cpp/h          # Player ship and input handling
│   ├── Enemy.cpp/h           # Base enemy functionality
│   ├── EnemyController.cpp/h # Enemy management system
│   ├── LeadEnemy.cpp/h       # Aggressive enemy AI
│   ├── FollowEnemy.cpp/h     # Path-following enemy AI
│   ├── Fighter.cpp/h         # Advanced enemy with shooting
│   ├── Mine.cpp/h            # Mine entities and behavior
│   ├── DoubleMine.cpp/h      # Double mine variants
│   ├── UFO.cpp/h             # UFO bonus enemies
│   ├── Rock.cpp/h            # Rock obstacles
│   ├── Shot.cpp/h            # Projectile system
│   └── PlayerShot.cpp/h      # Player projectiles
├── graphics/                  # Rendering and visual effects
│   ├── Window.cpp/h          # Display and rendering management
│   ├── Ship.cpp/h            # Base ship rendering and physics
│   ├── PlayerShip.cpp/h      # Enhanced player ship with effects
│   ├── TriShip.cpp/h         # Triangular ship rendering
│   ├── FighterShip.cpp/h     # Fighter ship visual effects
│   ├── VapourTrail.cpp/h     # Trail effect system
│   ├── Explosion.cpp/h       # Explosion particle effects
│   ├── ExplosionLine.cpp/h   # Explosion line effects
│   ├── PlayerExplosionLine.cpp/h # Player-specific explosions
│   ├── Borders.cpp/h         # Arena boundary system
│   ├── HUD.cpp/h             # Heads-up display
│   ├── StatusDisplay.cpp/h   # Status information display
│   ├── Letter.cpp/h          # Text rendering system
│   └── Number.cpp/h          # Number rendering system
├── audio/                     # Sound and music
│   └── AudioEngine.cpp/h     # FMOD audio management
└── resources/                 # Game assets
    ├── audio/                # Sound effects and music
    ├── shaders/              # Graphics shaders
    └── textures/             # Sprite and texture files
```

### Dependencies
- **CMake 3.15+** - Modern build system
- **BGFX** - Cross-platform rendering library (automatically fetched)
- **SDL2** - Window management and input handling
- **FMOD** - Audio engine for sound effects
- **C++17 Compiler** - GCC, Clang, or MSVC with C++17 support
- **Platform**: macOS 11.0+, Windows 10+, or Linux (Ubuntu 20.04+)

### Build System
The project uses **CMake** for cross-platform building:

```bash
# Quick build (Release mode)
./scripts/build.sh

# Debug build
./scripts/build.sh Debug

# Manual CMake build
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build . -j$(nproc)
```

### Prerequisites
- **CMake 3.15+** - Build system
- **SDL2** - Available via package manager (Homebrew on macOS, apt on Ubuntu, vcpkg on Windows)
- **FMOD SDK** - Included in third-party/ directory
- **Metal/DirectX/Vulkan/OpenGL** - Graphics API support (varies by platform)
- **C++17 compatible compiler** - GCC 7+, Clang 5+, MSVC 2017+

### Quick Build
```bash
# Clone the repository
git clone https://github.com/0x1337c0d3/OmegaRace.git
cd OmegaRace

# Build (Release mode by default)
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
# Install dependencies via Homebrew
brew install cmake sdl2

# Build and run
mkdir build && cd build
cmake ..
cmake --build .
open Debug/OmegaRace.app
```

#### Ubuntu/Linux
```bash
# Install dependencies
sudo apt update
sudo apt install cmake libsdl2-dev build-essential

# Build and run
mkdir build && cd build
cmake ..
make -j$(nproc)
./OmegaRace
```

#### Windows (with Visual Studio)
```powershell
# Install dependencies via vcpkg
vcpkg install sdl2:x64-windows

# Build
mkdir build && cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=C:/vcpkg/scripts/buildsystems/vcpkg.cmake
cmake --build . --config Release
```

### Manual Build
```bash
# Configure build system
cmake -B build -DCMAKE_BUILD_TYPE=Release

# Build the project
cmake --build build --config Release

# Alternative: Use build scripts (macOS)
./scripts/build.sh Release
```

### Build Scripts
The project includes automated build scripts for various platforms:

- **`scripts/build.sh`** - Cross-platform build script with configuration options
- **`build_release.sh`** - macOS-specific Release build with optional DMG creation  
- **`build_release_dmg.sh`** - Complete macOS build and distribution pipeline
- See `BUILD_README.md` for detailed macOS build documentation

## 📦 Distribution

The build system creates a professional DMG file for distribution:
- Self-contained app bundle with all dependencies
- Code-signed for security
- Compressed for smaller downloads
- Includes Applications folder for easy installation

## 🎵 Audio

The game features immersive audio effects:
- Engine thrust sounds
- Weapon firing effects
- Explosion and impact sounds
- Enemy-specific audio cues
- Background ambience

Audio files are managed through FMOD with support for:
- 3D spatial audio
- Dynamic mixing
- Professional sound banks
- Cross-platform audio formats

## 🎨 Graphics

Modern vector-style graphics powered by BGFX featuring:
- **Cross-platform rendering** - Metal (macOS), DirectX (Windows), Vulkan/OpenGL (Linux)
- **High-performance line rendering** - Hardware-accelerated vector graphics
- **Dynamic bloom effects** - Volumetric lighting and glow effects
- **Particle systems** - Advanced explosion and trail effects
- **Smooth anti-aliasing** - Clean line rendering at all resolutions
- **Retina/High-DPI support** - Crisp visuals on modern displays
- **60+ FPS gameplay** - Optimized rendering pipeline
- **Real-time effects** - Dynamic lighting and screen-space effects

## 🔧 Configuration

Game settings and configuration:
- High-resolution display support
- Game controller configuration
- Audio volume and mixing
- Display options and windowing
- Performance optimization settings

## 📋 System Requirements

### Minimum Requirements
- **OS**: macOS 11.0+, Windows 10+, or Ubuntu 20.04+
- **CPU**: 64-bit processor (Intel/AMD x64 or ARM64)
- **RAM**: 2GB available memory
- **Graphics**: DirectX 11, OpenGL 3.3, Metal, or Vulkan support
- **Storage**: 200MB available space

### Recommended
- **OS**: macOS 12.0+, Windows 11+, or Ubuntu 22.04+
- **CPU**: Multi-core processor (quad-core recommended)
- **RAM**: 4GB available memory
- **Graphics**: Dedicated GPU with modern driver support
- **Input**: Game controller for optimal experience
- **Display**: High-DPI/Retina display for enhanced visuals

## 🐛 Known Issues

- Performance may vary on older hardware without modern graphics API support
- Shader compilation requires compatible graphics drivers
- Some visual effects require DirectX 11/Metal/Vulkan support
- Game controller support optimized for DirectionalGamepad profile
- First launch may take longer due to shader compilation

## 🤝 Contributing

This project is a personal recreation of the classic Omega Race. While not actively seeking contributions, bug reports and suggestions are welcome.

## � Acknowledgments

This project was inspired by and builds upon the excellent work from:
- **[VectorOmegaRace](https://github.com/LanceJZ/VectorOmegaRace)** by LanceJZ - A fantastic C++ recreation that provided valuable insights into game mechanics and physics implementation.

## �📄 License

This project is a non-commercial recreation of the classic Omega Race arcade game. The original game was created by Midway Manufacturing in 1981.

**Copyright © 2025 One Red Dog Media. All rights reserved.**

## 🎯 Future Plans

Potential enhancements:
- Additional enemy types and behaviors
- Power-ups and weapon upgrades
- Multiple arena layouts
- Online leaderboards
- Additional visual effects

## � TODO - Graphics Refinements

The following graphics improvements are planned for future development:

### Alpha Blending Issues
- **Status Display** - Improve rendering order to prevent grid bleed-through on UI elements

### Visual Effects Enhancements
- **Particle Density** - Adjust particle count and fade rates for more realistic smoke effect

### Ship Appearance
- **Ship Color** - Refine player ship color scheme and materials
- **Ship Glow Effects** - Enhance ship outline and glow effects
- **Color Consistency** - Ensure ship colors work well with background and effects

### Background Grid
- **Grid Alpha** - Fine-tune background grid transparency to minimize UI interference
- **Grid Distortion** - Optimize grid distortion effects for performance
- **Rendering Order** - Ensure proper depth sorting between grid, game objects, and UI

## �📞 Support

For technical issues or questions:
- Check the build documentation in `BUILD_README.md`
- Review system requirements
- Ensure all dependencies are properly installed

---

*Relive the classic arcade experience with modern enhancements and smooth gameplay!*
