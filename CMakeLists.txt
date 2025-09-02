cmake_minimum_required(VERSION 3.15)
project(OmegaRace 
    VERSION 1.0.0
    DESCRIPTION "Classic arcade space shooter remake"
    LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add compile definitions to avoid conflicts
add_compile_definitions(VMATH_NAMESPACE=omegarace)

# Platform detection
if(APPLE)
    set(PLATFORM_MACOS TRUE)
elseif(WIN32)
    set(PLATFORM_WINDOWS TRUE)
elseif(UNIX)
    set(PLATFORM_LINUX TRUE)
endif()

# Build configuration
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Compiler flags
if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -pedantic)
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        add_compile_options(-g -O0)
    else()
        add_compile_options(-O3)
    endif()
endif()

# Find packages
find_package(PkgConfig REQUIRED)

# SDL2 for window management and input
find_package(SDL2 REQUIRED)
if(NOT SDL2_FOUND)
    message(STATUS "SDL2 not found, trying pkg-config")
    pkg_check_modules(SDL2 REQUIRED sdl2)
endif()

# BGFX for advanced rendering
include(FetchContent)

# Configure BGFX to only build what we need - disable examples but enable tools for shader compilation
# These must be set BEFORE FetchContent_Declare to take effect properly
option(BGFX_BUILD_EXAMPLES "Build bgfx examples" OFF)
option(BGFX_BUILD_TOOLS "Build bgfx tools" ON)
option(BGFX_CUSTOM_TARGETS "Build bgfx custom targets" OFF)
option(BGFX_USE_OVR "Build with OVR support" OFF)
option(BGFX_AMALGAMATED "Build bgfx as amalgamated" OFF)
option(BGFX_CONFIG_DEBUG "Enable bgfx debug mode" OFF)

# Set cache variables to force the settings
set(BGFX_BUILD_EXAMPLES OFF CACHE BOOL "Build bgfx examples" FORCE)
set(BGFX_BUILD_TOOLS ON CACHE BOOL "Build bgfx tools" FORCE)
set(BGFX_CUSTOM_TARGETS OFF CACHE BOOL "Build bgfx custom targets" FORCE)
set(BGFX_USE_OVR OFF CACHE BOOL "Build with OVR support" FORCE)
set(BGFX_AMALGAMATED OFF CACHE BOOL "Build bgfx as amalgamated" FORCE)
set(BGFX_CONFIG_DEBUG OFF CACHE BOOL "Enable bgfx debug mode" FORCE)

FetchContent_Declare(
    bgfx.cmake
    GIT_REPOSITORY https://github.com/bkaradzic/bgfx.cmake.git
    GIT_TAG master
)
FetchContent_MakeAvailable(bgfx.cmake)

# SDL2_gfx for basic drawing (optional, as BGFX will handle rendering)
find_library(SDL2_GFX_LIBRARY 
    NAMES SDL2_gfx
    PATHS /opt/homebrew/lib /usr/local/lib
)
if(SDL2_GFX_LIBRARY)
    message(STATUS "Found SDL2_gfx: ${SDL2_GFX_LIBRARY}")
else()
    message(STATUS "SDL2_gfx not found - using BGFX for all rendering")
endif()

# Source files organized by category
set(CORE_SOURCES
    src/main.cpp
    src/core/Game.cpp
    src/core/GameController.cpp
    src/core/Timer.cpp
    src/core/Common.cpp
    src/core/vmath.cpp
    src/core/Logger.cpp
)

set(ENTITY_SOURCES
    src/entities/Entity.cpp
    src/entities/Player.cpp
    src/entities/Enemy.cpp
    src/entities/EnemyController.cpp
    src/entities/LeadEnemy.cpp
    src/entities/FollowEnemy.cpp
    src/entities/Fighter.cpp
    src/entities/Mine.cpp
    src/entities/DoubleMine.cpp
    src/entities/UFO.cpp
    src/entities/Rock.cpp
    src/entities/Shot.cpp
    src/entities/PlayerShot.cpp
)

set(GRAPHICS_SOURCES
    src/graphics/Window.cpp
    src/graphics/MetalLayerSetup.mm
    src/graphics/Ship.cpp
    src/graphics/PlayerShip.cpp
    src/graphics/TriShip.cpp
    src/graphics/FighterShip.cpp
    src/graphics/VapourTrail.cpp
    src/graphics/Explosion.cpp
    src/graphics/ExplosionLine.cpp
    src/graphics/PlayerExplosionLine.cpp
    src/graphics/Borders.cpp
    src/graphics/StatusDisplay.cpp
    src/graphics/Letter.cpp
    src/graphics/Number.cpp
)

set(AUDIO_SOURCES
    src/audio/AudioEngine.cpp
)

# Combine all sources
set(ALL_SOURCES
    ${CORE_SOURCES}
    ${ENTITY_SOURCES}
    ${GRAPHICS_SOURCES}
    ${AUDIO_SOURCES}
)

# Create executable
add_executable(${PROJECT_NAME} ${ALL_SOURCES})

# Include directories
target_include_directories(${PROJECT_NAME} PRIVATE
    src/core
    src/entities
    src/graphics
    src/audio
    include
    ${SDL2_INCLUDE_DIRS}
)

# Link libraries - SDL2 for windowing, BGFX for rendering
target_link_libraries(${PROJECT_NAME} PRIVATE 
    ${SDL2_LIBRARIES}
    bgfx
    bimg
    bx
)

# Add SDL2_gfx if available (though BGFX will handle most rendering)
if(SDL2_GFX_LIBRARY)
    target_link_libraries(${PROJECT_NAME} PRIVATE ${SDL2_GFX_LIBRARY})
endif()

# Platform-specific settings
if(PLATFORM_MACOS)
    # macOS-specific FMOD linking
    target_include_directories(${PROJECT_NAME} PRIVATE third-party/fmod/osx/inc)
    target_link_directories(${PROJECT_NAME} PRIVATE third-party/fmod/osx/lib)
    target_link_libraries(${PROJECT_NAME} PRIVATE fmod fmodstudio)
    
    # Set RPATH for app bundle
    set_target_properties(${PROJECT_NAME} PROPERTIES
        BUILD_RPATH "${CMAKE_CURRENT_SOURCE_DIR}/third-party/fmod/osx/lib"
        INSTALL_RPATH "@executable_path/../Frameworks"
    )
    
    # macOS frameworks for BGFX
    target_link_libraries(${PROJECT_NAME} PRIVATE 
        "-framework Cocoa"
        "-framework Metal"
        "-framework MetalKit" 
        "-framework QuartzCore"
        "-framework OpenGL"
        "-framework IOKit"
        "-framework CoreVideo"
    )
    
    # Configure Info.plist with proper values
    configure_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/resources/macos/Info.plist.in"
        "${CMAKE_CURRENT_BINARY_DIR}/Info.plist"
        @ONLY
    )
    
    # macOS app bundle configuration
    set_target_properties(${PROJECT_NAME} PROPERTIES
        MACOSX_BUNDLE TRUE
        MACOSX_BUNDLE_BUNDLE_NAME "OmegaRace"
        MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
        MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION}
        MACOSX_BUNDLE_IDENTIFIER "com.omegarace.game"
        MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_BINARY_DIR}/Info.plist"
        MACOSX_BUNDLE_EXECUTABLE_NAME "${PROJECT_NAME}"
        MACOSX_BUNDLE_GUI_IDENTIFIER "com.omegarace.game"
        MACOSX_BUNDLE_ICON_FILE "AppIcon"
        XCODE_ATTRIBUTE_CODE_SIGN_ENTITLEMENTS "${CMAKE_CURRENT_SOURCE_DIR}/resources/macos/OmegaRace.entitlements"
        XCODE_ATTRIBUTE_PRODUCT_BUNDLE_IDENTIFIER "com.omegarace.game"
        XCODE_ATTRIBUTE_DEVELOPMENT_LANGUAGE "en"
        XCODE_ATTRIBUTE_EXECUTABLE_NAME "${PROJECT_NAME}"
        XCODE_ATTRIBUTE_PRODUCT_NAME "OmegaRace"
        XCODE_ATTRIBUTE_MACOSX_DEPLOYMENT_TARGET "10.15"
    )

    # Copy FMOD dylibs to app bundle after build
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory 
            $<TARGET_BUNDLE_DIR:${PROJECT_NAME}>/Contents/Frameworks
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${CMAKE_CURRENT_SOURCE_DIR}/third-party/fmod/osx/lib/libfmod.dylib"
            $<TARGET_BUNDLE_DIR:${PROJECT_NAME}>/Contents/Frameworks/
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${CMAKE_CURRENT_SOURCE_DIR}/third-party/fmod/osx/lib/libfmodstudio.dylib"
            $<TARGET_BUNDLE_DIR:${PROJECT_NAME}>/Contents/Frameworks/
        COMMENT "Copying FMOD dylibs to app bundle"
    )
    
elseif(PLATFORM_WINDOWS)
    # Windows-specific FMOD linking
    target_include_directories(${PROJECT_NAME} PRIVATE third-party/fmod/windows/inc)
    target_link_directories(${PROJECT_NAME} PRIVATE third-party/fmod/windows/lib)
    target_link_libraries(${PROJECT_NAME} PRIVATE fmod_vc fmodstudio_vc)
    
elseif(PLATFORM_LINUX)
    # Linux-specific FMOD linking
    target_include_directories(${PROJECT_NAME} PRIVATE third-party/fmod/linux/inc)
    target_link_directories(${PROJECT_NAME} PRIVATE third-party/fmod/linux/lib)
    target_link_libraries(${PROJECT_NAME} PRIVATE fmod fmodstudio)
endif()

# Resource handling
if(PLATFORM_MACOS)
    # Copy resources to app bundle
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory 
            "$<TARGET_BUNDLE_DIR:${PROJECT_NAME}>/Contents/Resources"
        COMMAND ${CMAKE_COMMAND} -E copy_directory 
            "${CMAKE_CURRENT_SOURCE_DIR}/resources" 
            "$<TARGET_BUNDLE_DIR:${PROJECT_NAME}>/Contents/Resources"
    )
    
    # Copy app icon and convert to ICNS format
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory 
            "$<TARGET_BUNDLE_DIR:${PROJECT_NAME}>/Contents/Resources/AppIcon.iconset"
        COMMAND ${CMAKE_COMMAND} -E copy 
            "${CMAKE_CURRENT_SOURCE_DIR}/resources/macos/Assets.xcassets/AppIcon.appiconset/icon_1024.png"
            "$<TARGET_BUNDLE_DIR:${PROJECT_NAME}>/Contents/Resources/AppIcon.iconset/icon_1024x1024.png"
        COMMAND ${CMAKE_COMMAND} -E copy 
            "${CMAKE_CURRENT_SOURCE_DIR}/resources/macos/Assets.xcassets/AppIcon.appiconset/icon_1024.png"
            "$<TARGET_BUNDLE_DIR:${PROJECT_NAME}>/Contents/Resources/AppIcon.iconset/icon_512x512@2x.png"
        # Create ICNS file from iconset
        COMMAND iconutil -c icns 
            "$<TARGET_BUNDLE_DIR:${PROJECT_NAME}>/Contents/Resources/AppIcon.iconset" 
            -o "$<TARGET_BUNDLE_DIR:${PROJECT_NAME}>/Contents/Resources/AppIcon.icns"
        COMMAND ${CMAKE_COMMAND} -E remove_directory 
            "$<TARGET_BUNDLE_DIR:${PROJECT_NAME}>/Contents/Resources/AppIcon.iconset"
        COMMENT "Creating app icon ICNS file"
    )
else()
    # Copy resources to build directory
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory 
            "${CMAKE_CURRENT_SOURCE_DIR}/resources" 
            "${CMAKE_CURRENT_BINARY_DIR}/resources"
    )
endif()

# Installation
install(TARGETS ${PROJECT_NAME}
    BUNDLE DESTINATION .
    RUNTIME DESTINATION bin
)

if(PLATFORM_MACOS)
    # Set framework paths for runtime
    set_target_properties(${PROJECT_NAME} PROPERTIES
        INSTALL_RPATH "@executable_path/../Frameworks"
        BUILD_WITH_INSTALL_RPATH TRUE
        XCODE_ATTRIBUTE_LD_RUNPATH_SEARCH_PATHS "@executable_path/../Frameworks"
    )
    
    # Prepare list of frameworks to embed (FMOD only now)
    set(FRAMEWORKS_TO_EMBED
        "${CMAKE_CURRENT_SOURCE_DIR}/third-party/fmod/osx/lib/libfmod.dylib"
        "${CMAKE_CURRENT_SOURCE_DIR}/third-party/fmod/osx/lib/libfmodstudio.dylib"
    )
    
    # Add frameworks to be embedded and signed (equivalent to Xcode "Copy Bundle Frameworks")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        XCODE_EMBED_FRAMEWORKS "${FRAMEWORKS_TO_EMBED}"
        XCODE_EMBED_FRAMEWORKS_CODE_SIGN_ON_COPY ON
    )
    
    # For non-Xcode generators, install frameworks manually with signing
    if(NOT CMAKE_GENERATOR STREQUAL "Xcode")
        # Install frameworks manually
        install(FILES 
            "${CMAKE_CURRENT_SOURCE_DIR}/third-party/fmod/osx/lib/libfmod.dylib"
            "${CMAKE_CURRENT_SOURCE_DIR}/third-party/fmod/osx/lib/libfmodstudio.dylib"
            DESTINATION "${PROJECT_NAME}.app/Contents/Frameworks"
        )
        
        # Sign FMOD frameworks after installation  
        install(CODE "
            execute_process(COMMAND codesign --force --sign - 
                \${CMAKE_INSTALL_PREFIX}/${PROJECT_NAME}.app/Contents/Frameworks/libfmod.dylib
            )
            execute_process(COMMAND codesign --force --sign - 
                \${CMAKE_INSTALL_PREFIX}/${PROJECT_NAME}.app/Contents/Frameworks/libfmodstudio.dylib
            )
        ")
    endif()
endif()

if(NOT PLATFORM_MACOS)
    install(DIRECTORY resources/
        DESTINATION share/${PROJECT_NAME}/resources
    )
endif()

# Development targets
add_custom_target(format
    COMMAND find src -name "*.cpp" -o -name "*.h" | xargs clang-format -i
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "Formatting source code"
)

add_custom_target(clean-all
    COMMAND ${CMAKE_COMMAND} --build . --target clean
    COMMAND ${CMAKE_COMMAND} -E remove_directory build
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "Deep clean"
)
