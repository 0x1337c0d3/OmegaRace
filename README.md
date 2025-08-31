# Omega Race

A modern recreation of the classic 1981 arcade game Omega Race, built with C++ and Raylib for macOS.

![Game Version](https://img.shields.io/badge/version-1.0-blue.svg)
![Platform](https://img.shields.io/badge/platform-macOS-lightgrey.svg)
![Language](https://img.shields.io/badge/language-C++-blue.svg)
![Framework](https://img.shields.io/badge/framework-Raylib-green.svg)

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
- **Raylib** - Cross-platform game development library
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
- **Raylib** - Graphics and input handling
- **FMOD** - Audio engine for sound effects
- **C++17 Compiler** - GCC, Clang, or MSVC
- **Platform**: macOS 14.6+, Windows 10+, or Linux

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
- Xcode 16.0 or later
- macOS 14.6 or later
- Raylib (installed via Homebrew)
- FMOD SDK (included in ThirdParty/)

### Quick Build
```bash
# Build Release version and create DMG
./build_release_dmg.sh

# Or build just the Release version
./build_release.sh
```

### Manual Build
```bash
# Open in Xcode
open Applications/OmegaRace/OmegaRace.xcodeproj

# Or build from command line
cd Applications/OmegaRace
xcodebuild -project OmegaRace.xcodeproj -configuration Release
```

### Build Scripts
The project includes automated build scripts:

- **`build_release.sh`** - Simple Release build with optional DMG creation
- **`build_release_dmg.sh`** - Complete build and distribution pipeline
- See `BUILD_README.md` for detailed build documentation

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

Modern vector-style graphics featuring:
- Smooth line rendering with anti-aliasing
- Dynamic color effects and intensity
- Particle effects for explosions
- Enhanced visual feedback
- Retina display optimization
- Smooth 60fps gameplay

## 🔧 Configuration

Game settings and configuration:
- High-resolution display support
- Game controller configuration
- Audio volume and mixing
- Display options and windowing
- Performance optimization settings

## 📋 System Requirements

### Minimum Requirements
- macOS 14.6 or later
- 2GB RAM
- Metal-compatible graphics
- 100MB available storage

### Recommended
- macOS 14.6 or later
- 4GB RAM
- Dedicated graphics card
- Game controller for optimal experience

## 🐛 Known Issues

- Performance may vary on older hardware
- Some visual effects require Metal support
- Game controller support limited to DirectionalGamepad profile

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
- High score persistence
- Online leaderboards
- Additional visual effects

## 📞 Support

For technical issues or questions:
- Check the build documentation in `BUILD_README.md`
- Review system requirements
- Ensure all dependencies are properly installed

---

*Relive the classic arcade experience with modern enhancements and smooth gameplay!*
