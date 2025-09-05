#!/bin/bash

# Modern CMake-based OmegaRace Release Builder
# Usage: ./build_release.sh [--dmg] [--open] [--sign]

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
DIST_DIR="$PROJECT_DIR/dist"
BUILD_DIR="$PROJECT_DIR/build"
CREATE_DMG=false
OPEN_DIST=false
SIGN_APP=false

# Parse command line arguments
while [[ $# -gt 0 ]]; do
    case $1 in
        --dmg)
            CREATE_DMG=true
            shift
            ;;
        --open)
            OPEN_DIST=true
            shift
            ;;
        --sign)
            SIGN_APP=true
            shift
            ;;
        -h|--help)
            echo "Usage: $0 [--dmg] [--open] [--sign]"
            echo "  --dmg   Create DMG file after building"
            echo "  --open  Open distribution directory when done"
            echo "  --sign  Sign the application (requires valid developer certificate)"
            exit 0
            ;;
        *)
            echo "Unknown option: $1"
            exit 1
            ;;
    esac
done

cd "$PROJECT_DIR"

echo "🔨 Building OmegaRace Release with CMake..."

# Create build directory
mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

# Configure CMake for Release
echo "⚙️ Configuring CMake (Release)..."
cmake -DCMAKE_BUILD_TYPE=Release \
      -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
      "$PROJECT_DIR"

# Build the project
echo "🔨 Building Release version..."
cmake --build . --config Release -j$(nproc 2>/dev/null || sysctl -n hw.ncpu)

# Verify build was successful
if [ ! -d "OmegaRace.app" ]; then
    echo "❌ Build failed! OmegaRace.app not found in $BUILD_DIR"
    exit 1
fi

echo "✅ Build successful!"

# Sign the application if requested
if [ "$SIGN_APP" = true ]; then
    echo "🔐 Signing application..."
    # Note: This requires a valid developer certificate in your keychain
    # You may need to specify your developer identity with -s "Developer ID Application: Your Name"
    codesign --force --deep --sign - "OmegaRace.app"
    echo "✅ Application signed"
fi

if [ "$CREATE_DMG" = true ]; then
    echo "📦 Creating DMG..."
    "$SCRIPT_DIR/build_release_dmg.sh"
else
    # Copy to dist directory
    mkdir -p "$DIST_DIR"
    echo "📁 Copying app to dist directory..."
    cp -R "OmegaRace.app" "$DIST_DIR/"
    echo "✅ OmegaRace.app copied to $DIST_DIR/"
fi

if [ "$OPEN_DIST" = true ] && command -v open >/dev/null 2>&1; then
    open "$DIST_DIR"
fi

echo "🎉 Done!"
