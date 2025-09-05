#!/bin/bash

# OmegaRace CMake-based Release Build and DMG Creation Script
# This script builds the Release version using CMake and creates a DMG file for distribution

set -e  # Exit on any error

# Configuration
PROJECT_NAME="OmegaRace"
PROJECT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
BUILD_DIR="$PROJECT_DIR/build"
BUNDLE_NAME="OmegaRace.app"
DMG_NAME="OmegaRace-$(date +%Y%m%d)"
DIST_DIR="$PROJECT_DIR/dist"
TEMP_DMG_DIR="/tmp/OmegaRace_dmg_temp"

echo "========================================"
echo "OmegaRace CMake Release Build & DMG Creation"
echo "========================================"
echo "Date: $(date)"
echo "Build System: CMake"
echo "Project: $PROJECT_DIR"
echo ""

# Change to project directory
cd "$PROJECT_DIR"

# Create and enter build directory
mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

# Clean previous builds
echo "🧹 Cleaning previous builds..."
rm -rf *

# Configure CMake for Release
echo "⚙️ Configuring CMake (Release)..."
cmake -DCMAKE_BUILD_TYPE=Release \
      -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
      "$PROJECT_DIR"

# Build Release version
echo "🔨 Building Release version..."
cmake --build . --config Release -j$(nproc 2>/dev/null || sysctl -n hw.ncpu)

# Check if build was successful
if [ ! -d "$BUNDLE_NAME" ]; then
    echo "❌ Build failed! $BUNDLE_NAME not found in $BUILD_DIR"
    exit 1
fi

echo "✅ Build successful!"

# Create distribution directory
echo "📁 Creating distribution directory..."
mkdir -p "$DIST_DIR"

# Clean up any existing temp directory
rm -rf "$TEMP_DMG_DIR"
mkdir -p "$TEMP_DMG_DIR"

# Copy the app bundle to temp directory
echo "📦 Copying app bundle..."
cp -R "$BUILD_DIR/$BUNDLE_NAME" "$TEMP_DMG_DIR/"

# Create Applications symlink for easy installation
echo "🔗 Creating Applications symlink..."
ln -s /Applications "$TEMP_DMG_DIR/Applications"

# Optional: Add README or license files
if [ -f "$PROJECT_DIR/README.md" ]; then
    cp "$PROJECT_DIR/README.md" "$TEMP_DMG_DIR/"
fi

# Get app version if available
APP_VERSION=""
if [ -f "$BUILD_DIR/$BUNDLE_NAME/Contents/Info.plist" ]; then
    APP_VERSION=$(defaults read "$BUILD_DIR/$BUNDLE_NAME/Contents/Info.plist" CFBundleShortVersionString 2>/dev/null || echo "")
    if [ -n "$APP_VERSION" ]; then
        DMG_NAME="OmegaRace-v$APP_VERSION-$(date +%Y%m%d)"
    fi
fi

# Create the DMG
echo "💽 Creating DMG file: $DMG_NAME.dmg..."

# Remove existing DMG if it exists
rm -f "$DIST_DIR/$DMG_NAME.dmg"

# Create DMG using hdiutil
hdiutil create -volname "$PROJECT_NAME" \
               -srcfolder "$TEMP_DMG_DIR" \
               -ov \
               -format UDZO \
               -imagekey zlib-level=9 \
               "$DIST_DIR/$DMG_NAME.dmg"

# Clean up temp directory
rm -rf "$TEMP_DMG_DIR"

# Get file size
DMG_SIZE=$(du -h "$DIST_DIR/$DMG_NAME.dmg" | cut -f1)

echo ""
echo "🎉 SUCCESS!"
echo "========================================"
echo "✅ Release build completed successfully"
echo "✅ DMG created: $DIST_DIR/$DMG_NAME.dmg"
echo "📏 DMG size: $DMG_SIZE"
echo ""
echo "The DMG file is ready for distribution!"
echo "Users can mount the DMG and drag $BUNDLE_NAME to Applications."
echo ""

# Optional: Open the dist directory in Finder
if command -v open >/dev/null 2>&1; then
    echo "📂 Opening distribution directory..."
    open "$DIST_DIR"
fi

echo "Done! 🚀"
