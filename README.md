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
├── Game.cpp/h              # Main game loop and application management
├── GameController.cpp/h    # Core game logic and state management
├── Player.cpp/h            # Player ship and input handling
├── Entity.cpp/h            # Base entity class for game objects
├── Ship.cpp/h              # Base ship rendering and physics
├── PlayerShip.cpp/h        # Enhanced player ship with effects
├── Enemy.cpp/h             # Base enemy functionality
├── LeadEnemy.cpp/h         # Aggressive enemy AI
├── FollowEnemy.cpp/h       # Path-following enemy AI
├── Fighter.cpp/h           # Advanced enemy with shooting
├── Mine.cpp/h              # Mine entities and behavior
├── TriShip.cpp/h           # Triangular ship rendering
├── AudioEngine.cpp/h       # Sound effect management
├── Borders.cpp/h           # Arena boundary system
├── Window.cpp/h            # Display and rendering management
└── vmath.h                 # Vector mathematics utilities
```

### Dependencies
- **Raylib** - Graphics and input handling
- **FMOD** - Audio engine for sound effects
- **macOS 11.0+** - Target platform
- **Xcode** - Development environment

## 🚀 Building the Game

### Prerequisites
- Xcode 12.0 or later
- macOS 11.0 or later
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
- macOS 11.0 (Big Sur) or later
- 2GB RAM
- Metal-compatible graphics
- 100MB available storage

### Recommended
- macOS 12.0 (Monterey) or later
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
