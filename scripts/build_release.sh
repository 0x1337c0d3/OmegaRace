#!/bin/bash

# Simple OmegaRace Release Builder
# Usage: ./build_release.sh [--dmg] [--open]

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
DIST_DIR="$SCRIPT_DIR/dist"
CREATE_DMG=false
OPEN_DIST=false

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
        -h|--help)
            echo "Usage: $0 [--dmg] [--open]"
            echo "  --dmg   Create DMG file after building"
            echo "  --open  Open distribution directory when done"
            exit 0
            ;;
        *)
            echo "Unknown option: $1"
            exit 1
            ;;
    esac
done

cd "$PROJECT_DIR"

echo "🔨 Building OmegaRace Release..."
xcodebuild -project OmegaRace.xcodeproj -scheme OmegaRace -configuration Release clean build

if [ "$CREATE_DMG" = true ]; then
    echo "📦 Creating DMG..."
    "$SCRIPT_DIR/build_release_dmg.sh"
else
    BUILD_DIR="$PROJECT_DIR/DerivedData/Build/Products/Release"
    mkdir -p "$DIST_DIR"
    if [ -d "$BUILD_DIR/OmegaRace.app" ]; then
        echo "📁 Copying app to dist directory..."
        cp -R "$BUILD_DIR/OmegaRace.app" "$DIST_DIR/"
        echo "✅ OmegaRace.app copied to $DIST_DIR/"
    fi
fi

if [ "$OPEN_DIST" = true ] && command -v open >/dev/null 2>&1; then
    open "$DIST_DIR"
fi

echo "🎉 Done!"
