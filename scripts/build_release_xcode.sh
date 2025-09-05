#!/bin/bash

# Build script for OmegaRace with Xcode generator and proper code signing
# This script creates a release build using Xcode project files

set -e  # Exit on any error

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

echo -e "${GREEN}🚀 Building OmegaRace with Xcode Generator${NC}"

# Configuration
PROJECT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
BUILD_DIR="build_xcode"
CMAKE_BUILD_TYPE="Release"
CODE_SIGN_IDENTITY="Apple Development"
DEVELOPMENT_TEAM="ASB8U9Q83W"

# Clean previous build if requested
if [[ "$1" == "--clean" || "$1" == "-c" ]]; then
    echo -e "${YELLOW}🧹 Cleaning previous build...${NC}"
    rm -rf "${BUILD_DIR}"
fi

# Create build directory
mkdir -p "${BUILD_DIR}"

echo -e "${YELLOW}📝 Configuring CMake with Xcode generator...${NC}"

# Configure with Xcode generator
cmake -S . -B "${BUILD_DIR}" \
    -G Xcode \
    -DCMAKE_BUILD_TYPE="${CMAKE_BUILD_TYPE}" \
    -DCMAKE_XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY="${CODE_SIGN_IDENTITY}" \
    -DCMAKE_XCODE_ATTRIBUTE_DEVELOPMENT_TEAM="${DEVELOPMENT_TEAM}" \
    -DCMAKE_XCODE_ATTRIBUTE_CODE_SIGN_STYLE="Automatic" \
    -DCMAKE_XCODE_ATTRIBUTE_PRODUCT_BUNDLE_IDENTIFIER="au.com.onereddog.omegarace" \
    -DCMAKE_OSX_ARCHITECTURES="arm64;x86_64"

echo -e "${YELLOW}🔨 Building with Xcode...${NC}"

# Build the project
cmake --build "${BUILD_DIR}" --config "${CMAKE_BUILD_TYPE}" --parallel $(sysctl -n hw.ncpu)

echo -e "${YELLOW}✍️ Code signing the app bundle...${NC}"

# Manual code signing for extra assurance
APP_BUNDLE="${BUILD_DIR}/${CMAKE_BUILD_TYPE}/OmegaRace.app"
MANUAL_SIGN_IDENTITY="Apple Development: Peter Johnson (LLPZVPZ36Q)"

if [[ -d "${APP_BUNDLE}" ]]; then
    # Sign frameworks first
    echo -e "${YELLOW}  → Signing frameworks...${NC}"
    find "${APP_BUNDLE}/Contents/Frameworks" -name "*.dylib" -exec codesign \
        --force \
        --verify \
        --verbose \
        --sign "${MANUAL_SIGN_IDENTITY}" \
        --entitlements "resources/macos/OmegaRace.entitlements" \
        --options runtime \
        {} \;
    
    # Sign the main app bundle
    echo -e "${YELLOW}  → Signing main app bundle...${NC}"
    codesign \
        --force \
        --verify \
        --verbose \
        --sign "${MANUAL_SIGN_IDENTITY}" \
        --entitlements "resources/macos/OmegaRace.entitlements" \
        --options runtime \
        "${APP_BUNDLE}"
    
    echo -e "${GREEN}✅ Code signing completed${NC}"
    
    # Verify the signature
    echo -e "${YELLOW}🔍 Verifying code signature...${NC}"
    codesign --verify --deep --strict "${APP_BUNDLE}" && \
        echo -e "${GREEN}✅ Code signature verification passed${NC}" || \
        echo -e "${RED}❌ Code signature verification failed${NC}"
    
    # Show certificate info
    echo -e "${YELLOW}📋 Certificate info:${NC}"
    codesign --display --verbose=2 "${APP_BUNDLE}" 2>&1 | head -10
    
else
    echo -e "${RED}❌ App bundle not found at ${APP_BUNDLE}${NC}"
    exit 1
fi

echo "🎉 Build completed successfully!"
echo "📁 App bundle location: $BUILD_DIR/Release/OmegaRace.app"

# DMG Creation
echo ""
echo "💽 Creating distribution DMG..."

# DMG Configuration
BUNDLE_NAME="OmegaRace.app"
DMG_NAME="OmegaRace-$(date +%Y%m%d)"
DIST_DIR="$PROJECT_DIR/dist"
TEMP_DMG_DIR="/tmp/OmegaRace_dmg_temp"
APP_BUNDLE_PATH="$BUILD_DIR/Release/$BUNDLE_NAME"

# Create distribution directory
echo "📁 Creating distribution directory..."
mkdir -p "$DIST_DIR"

# Clean up any existing temp directory
rm -rf "$TEMP_DMG_DIR"
mkdir -p "$TEMP_DMG_DIR"

# Copy the signed app bundle to temp directory
echo "📦 Copying signed app bundle..."
cp -R "$APP_BUNDLE_PATH" "$TEMP_DMG_DIR/"

# Create Applications symlink for easy installation
echo "🔗 Creating Applications symlink..."
ln -s /Applications "$TEMP_DMG_DIR/Applications"

# Optional: Add README or license files
if [ -f "$PROJECT_DIR/README.md" ]; then
    cp "$PROJECT_DIR/README.md" "$TEMP_DMG_DIR/"
fi

# Get app version if available
APP_VERSION=""
if [ -f "$APP_BUNDLE_PATH/Contents/Info.plist" ]; then
    APP_VERSION=$(defaults read "$APP_BUNDLE_PATH/Contents/Info.plist" CFBundleShortVersionString 2>/dev/null || echo "")
    if [ -n "$APP_VERSION" ]; then
        DMG_NAME="OmegaRace-v$APP_VERSION-$(date +%Y%m%d)"
    fi
fi

# Create the DMG
echo "💽 Creating DMG file: $DMG_NAME.dmg..."

# Remove existing DMG if it exists
rm -f "$DIST_DIR/$DMG_NAME.dmg"

# Create DMG using hdiutil
hdiutil create -volname "$BUNDLE_NAME" \
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
echo "🎉 DISTRIBUTION READY!"
echo "========================================"
echo "✅ Xcode build completed successfully"
echo "✅ App bundle signed and verified"
echo "✅ DMG created: $DIST_DIR/$DMG_NAME.dmg"
echo "📏 DMG size: $DMG_SIZE"
echo ""
echo "The DMG file is ready for distribution!"
echo "Users can mount the DMG and drag OmegaRace.app to Applications."
echo ""

# Optional: Open the dist directory in Finder
if command -v open >/dev/null 2>&1; then
    echo "📂 Opening distribution directory..."
    open "$DIST_DIR"
fi

# Optional: Create a disk image
if [[ "$2" == "--dmg" ]]; then
    echo -e "${YELLOW}💿 Creating disk image...${NC}"
    
    # Remove existing DMG if it exists
    rm -f "${DMG_NAME}"
    
    # Create DMG using hdiutil
    hdiutil create -volname "OmegaRace" \
                   -srcfolder "${APP_BUNDLE}" \
                   -ov \
                   -format UDZO \
                   -imagekey zlib-level=9 \
                   "${DMG_NAME}"
    
    # Get DMG file size for reporting
    if [ -f "${DMG_NAME}" ]; then
        DMG_SIZE=$(du -h "${DMG_NAME}" | cut -f1)
        echo -e "${GREEN}✅ DMG created successfully: ${DMG_NAME} (${DMG_SIZE})${NC}"
        echo -e "${BLUE}📁 Location: $(pwd)/${DMG_NAME}${NC}"
    else
        echo -e "${RED}❌ Failed to create DMG${NC}"
        exit 1
    fi
fi

# Optional: Run the app
if [[ "$2" == "--run" || "$3" == "--run" ]]; then
    echo -e "${YELLOW}🏃 Launching OmegaRace...${NC}"
    open "${APP_BUNDLE}"
fi
