#!/bin/bash
# Modern CMake build script for OmegaRace

set -e

PROJECT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
BUILD_DIR="$PROJECT_DIR/build"
BUILD_TYPE="${1:-Release}"

echo "🚀 Building OmegaRace..."
echo "Project: $PROJECT_DIR"
echo "Build Type: $BUILD_TYPE"

# Create build directory
mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

# Configure CMake
echo "⚙️ Configuring CMake..."
cmake -DCMAKE_BUILD_TYPE="$BUILD_TYPE" \
      -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
      "$PROJECT_DIR"

# Build the project
echo "🔨 Building..."
cmake --build . --config "$BUILD_TYPE" -j$(nproc 2>/dev/null || sysctl -n hw.ncpu)

echo "✅ Build complete!"
echo "Executable: $BUILD_DIR/OmegaRace"

# On macOS, show app bundle location
if [[ "$OSTYPE" == "darwin"* ]]; then
    echo "App Bundle: $BUILD_DIR/OmegaRace.app"
fi
