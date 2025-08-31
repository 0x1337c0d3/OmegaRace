#!/bin/bash

# OmegaRace Release Build and DMG Creation Script
# This script builds the Release version and creates a DMG file for distribution

set -e  # Exit on any error

# Configuration
PROJECT_NAME="OmegaRace"
PROJECT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
PROJECT_FILE="OmegaRace.xcodeproj"
SCHEME="OmegaRace"
BUILD_CONFIG="Release"
BUNDLE_NAME="OmegaRace.app"
DMG_NAME="OmegaRace-$(date +%Y%m%d)"
DIST_DIR="/Users/peter/Development/OmegaRace/dist"
TEMP_DMG_DIR="/tmp/OmegaRace_dmg_temp"

echo "========================================"
echo "OmegaRace Release Build & DMG Creation"
echo "========================================"
echo "Date: $(date)"
echo "Build Configuration: $BUILD_CONFIG"
echo "Project: $PROJECT_DIR/$PROJECT_FILE"
echo ""

# Change to project directory
cd "$PROJECT_DIR"

# Clean previous builds
echo "🧹 Cleaning previous builds..."
xcodebuild -project "$PROJECT_FILE" -scheme "$SCHEME" -configuration "$BUILD_CONFIG" clean

# Build Release version
echo "🔨 Building Release version..."
xcodebuild -project "$PROJECT_FILE" \
           -scheme "$SCHEME" \
           -configuration "$BUILD_CONFIG" \
           -derivedDataPath "./DerivedData" \
           build

# Check if build was successful
BUILD_PRODUCTS_DIR="./DerivedData/Build/Products/$BUILD_CONFIG"
if [ ! -d "$BUILD_PRODUCTS_DIR/$BUNDLE_NAME" ]; then
    echo "❌ Build failed! $BUNDLE_NAME not found in $BUILD_PRODUCTS_DIR"
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
cp -R "$BUILD_PRODUCTS_DIR/$BUNDLE_NAME" "$TEMP_DMG_DIR/"

# Create Applications symlink for easy installation
echo "🔗 Creating Applications symlink..."
ln -s /Applications "$TEMP_DMG_DIR/Applications"

# Optional: Add README or license files
if [ -f "/Users/peter/Development/OmegaRace/README.md" ]; then
    cp "/Users/peter/Development/OmegaRace/README.md" "$TEMP_DMG_DIR/"
fi

# Get app version if available
APP_VERSION=""
if [ -f "$BUILD_PRODUCTS_DIR/$BUNDLE_NAME/Contents/Info.plist" ]; then
    APP_VERSION=$(defaults read "$BUILD_PRODUCTS_DIR/$BUNDLE_NAME/Contents/Info.plist" CFBundleShortVersionString 2>/dev/null || echo "")
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
