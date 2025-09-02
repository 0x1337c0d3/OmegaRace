#!/bin/bash

# Setup script for BGFX integration
echo "Setting up BGFX for OmegaRace..."

# Create third-party directory if it doesn't exist
mkdir -p third-party

# Add BGFX as a git submodule
if [ ! -d "third-party/bgfx" ]; then
    echo "Adding BGFX submodule..."
    git submodule add https://github.com/bkaradzic/bgfx.git third-party/bgfx
else
    echo "BGFX submodule already exists"
fi

# Initialize and update all submodules recursively
echo "Updating submodules..."
git submodule update --init --recursive

# Check if we're in the BGFX directory
if [ ! -d "third-party/bgfx" ]; then
    echo "Error: BGFX directory not found!"
    exit 1
fi

cd third-party/bgfx

# Build BGFX tools - use the correct build system
echo "Building BGFX..."

# Check if we have bx and bimg submodules
if [ ! -d "bx" ] || [ ! -d "bimg" ]; then
    echo "Initializing BGFX dependencies..."
    git submodule update --init --recursive
fi

# Use make to build BGFX (this will build the required tools first)
echo "Building BGFX for macOS..."
make osx-release64

# Alternative: try building directly with the scripts
if [ ! -f ".build/osx64_clang/bin/Release/libבgfx-shared-libRelease.dylib" ]; then
    echo "Trying alternative build method..."
    cd scripts
    ./build.sh --help || echo "Build script not executable, trying direct make..."
    cd ..
fi

cd ../..

echo "BGFX setup complete!"
echo "You can now build the project with: mkdir build && cd build && cmake .. && make"
