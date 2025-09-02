#!/bin/bash

# Shader compilation script for OmegaRace
# This script compiles .sc shader files to .bin format for different rendering backends

SHADERC="build/_deps/bgfx.cmake-build/cmake/bgfx/shaderc"
INCLUDE_DIR="build/_deps/bgfx.cmake-src/bgfx/src"
VARYING_DEF="src/shaders/varying.def.sc"
VARYING_VOLUMETRIC_DEF="src/shaders/varying_volumetric.def.sc"
SHADER_DIR="resources/shaders"

# Create shader directories for different backends
mkdir -p "$SHADER_DIR/metal"
mkdir -p "$SHADER_DIR/glsl"
mkdir -p "$SHADER_DIR/dx11"
mkdir -p "$SHADER_DIR/spirv"

echo "Compiling shaders..."

# Compile basic line shaders
echo "Compiling vs_line.sc..."
$SHADERC -f src/shaders/vs_line.sc -o $SHADER_DIR/metal/vs_line.bin --type vertex --platform osx --profile metal --varyingdef $VARYING_DEF -i $INCLUDE_DIR
$SHADERC -f src/shaders/vs_line.sc -o $SHADER_DIR/glsl/vs_line.bin --type vertex --platform linux --profile 120 --varyingdef $VARYING_DEF -i $INCLUDE_DIR
$SHADERC -f src/shaders/vs_line.sc -o $SHADER_DIR/dx11/vs_line.bin --type vertex --platform windows --profile vs_4_0 --varyingdef $VARYING_DEF -i $INCLUDE_DIR
$SHADERC -f src/shaders/vs_line.sc -o $SHADER_DIR/spirv/vs_line.bin --type vertex --platform linux --profile spirv --varyingdef $VARYING_DEF -i $INCLUDE_DIR

echo "Compiling fs_line.sc..."
$SHADERC -f src/shaders/fs_line.sc -o $SHADER_DIR/metal/fs_line.bin --type fragment --platform osx --profile metal --varyingdef $VARYING_DEF -i $INCLUDE_DIR
$SHADERC -f src/shaders/fs_line.sc -o $SHADER_DIR/glsl/fs_line.bin --type fragment --platform linux --profile 120 --varyingdef $VARYING_DEF -i $INCLUDE_DIR
$SHADERC -f src/shaders/fs_line.sc -o $SHADER_DIR/dx11/fs_line.bin --type fragment --platform windows --profile ps_4_0 --varyingdef $VARYING_DEF -i $INCLUDE_DIR
$SHADERC -f src/shaders/fs_line.sc -o $SHADER_DIR/spirv/fs_line.bin --type fragment --platform linux --profile spirv --varyingdef $VARYING_DEF -i $INCLUDE_DIR

# Compile volumetric line shaders with bloom
echo "Compiling vs_volumetric_line.sc..."
$SHADERC -f src/shaders/vs_volumetric_line.sc -o $SHADER_DIR/metal/vs_volumetric_line.bin --type vertex --platform osx --profile metal --varyingdef $VARYING_VOLUMETRIC_DEF -i $INCLUDE_DIR
$SHADERC -f src/shaders/vs_volumetric_line.sc -o $SHADER_DIR/glsl/vs_volumetric_line.bin --type vertex --platform linux --profile 120 --varyingdef $VARYING_VOLUMETRIC_DEF -i $INCLUDE_DIR
$SHADERC -f src/shaders/vs_volumetric_line.sc -o $SHADER_DIR/dx11/vs_volumetric_line.bin --type vertex --platform windows --profile vs_4_0 --varyingdef $VARYING_VOLUMETRIC_DEF -i $INCLUDE_DIR
$SHADERC -f src/shaders/vs_volumetric_line.sc -o $SHADER_DIR/spirv/vs_volumetric_line.bin --type vertex --platform linux --profile spirv --varyingdef $VARYING_VOLUMETRIC_DEF -i $INCLUDE_DIR

echo "Compiling fs_volumetric_line.sc..."
$SHADERC -f src/shaders/fs_volumetric_line.sc -o $SHADER_DIR/metal/fs_volumetric_line.bin --type fragment --platform osx --profile metal --varyingdef $VARYING_VOLUMETRIC_DEF -i $INCLUDE_DIR
$SHADERC -f src/shaders/fs_volumetric_line.sc -o $SHADER_DIR/glsl/fs_volumetric_line.bin --type fragment --platform linux --profile 120 --varyingdef $VARYING_VOLUMETRIC_DEF -i $INCLUDE_DIR
$SHADERC -f src/shaders/fs_volumetric_line.sc -o $SHADER_DIR/dx11/fs_volumetric_line.bin --type fragment --platform windows --profile ps_4_0 --varyingdef $VARYING_VOLUMETRIC_DEF -i $INCLUDE_DIR
$SHADERC -f src/shaders/fs_volumetric_line.sc -o $SHADER_DIR/spirv/fs_volumetric_line.bin --type fragment --platform linux --profile spirv --varyingdef $VARYING_VOLUMETRIC_DEF -i $INCLUDE_DIR

echo "Shader compilation complete!"
echo "Metal shaders: $SHADER_DIR/metal/"
echo "OpenGL shaders: $SHADER_DIR/glsl/"
echo "DirectX shaders: $SHADER_DIR/dx11/"
echo "Vulkan shaders: $SHADER_DIR/spirv/"
