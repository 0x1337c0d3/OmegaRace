#pragma once

#include "Types.h"

// SDL2 for window management and input
#include <SDL2/SDL.h>

// BGFX for advanced cross-platform rendering
#include <bgfx/bgfx.h>
#include <bgfx/platform.h>

#include <atomic>
#include <chrono>
#include <iostream>
#include <memory>
#include <mutex>
#include <random>
#include <sstream>
#include <stdexcept>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <thread>
#include <time.h>

namespace omegarace {

// Forward declarations
struct DistortionSource;

class Window {
  public:
    static void Init(int width, int height, std::string title = "OmegaRace");
    static void Quit();
    static void BeginFrame();
    static void EndFrame();
    static Rectangle Box();
    static void logError(std::ostream& os, const std::string& msg);

    // Basic drawing functions (placeholder implementations for now)
    static void DrawLine(Line* LineLocation, const Color& LineColor);
    static void DrawPoint(Vector2i* PointLocation, const Color& PointColor);
    static void DrawRect(const Rectangle* RectangleLocation, const Color& RectColor);

    // Enhanced line drawing with volumetric effects (placeholder for now)
    static void DrawVolumetricLine(Line* LineLocation, const Color& LineColor, float thickness = 3.0f);
    static void DrawVolumetricLineWithBloom(Line* LineLocation, const Color& LineColor, float thickness = 3.0f,
                                            float bloomIntensity = 0.5f);

    static Vector2i GetWindowSize();
    static int Random(int Min, int Max);

    static std::string dataPath();

    // Controller support
    static int findController();
    static void updateControllerDetection();
    static int mControllerIndex;

    // Window management
    static bool ShouldClose();
    static void SetShouldClose(bool shouldClose);

    // Shader support for advanced effects (placeholder)
    static void BeginBloomMode();
    static void EndBloomMode();

    // Controller input
    static bool IsControllerButtonPressed(int button);
    static bool IsControllerButtonDown(int button);
    static Vector2f GetControllerLeftStick();
    static Vector2f GetControllerRightStick();
    static float GetControllerLeftTrigger();
    static float GetControllerRightTrigger();
    static bool IsControllerConnected();
    static std::string GetControllerName();

    // Full screen warp effect for wave transitions (placeholder)
    static void DrawFullScreenWarp(float intensity = 1.0f, float time = 0.0f);
    static void BeginWarpTransition();
    static void EndWarpTransition();

    // Fullscreen support
    static bool CheckForFullscreenToggle();
    static void ToggleFullscreen();
    static bool IsFullscreen();

    // Input functions - Raylib-compatible API using SDL2
    static bool IsKeyPressed(int key);
    static bool IsKeyDown(int key);
    static bool IsGamepadButtonPressed(int gamepad, int button);
    static bool IsGamepadButtonDown(int gamepad, int button);
    static float GetGamepadAxisMovement(int gamepad, int axis);

  private:
    // SDL2 state
    static SDL_Window* mWindow;
    static SDL_Renderer* mRenderer;
    static Rectangle mBox;
    static bool mIsFullscreen;
    static int mWindowedWidth;
    static int mWindowedHeight;

    // BGFX rendering state
    static bgfx::ViewId mMainView;
    static bgfx::ViewId mBackgroundView;
    static bgfx::ViewId mBloomView;
    static bgfx::ProgramHandle mBloomProgram;
    static bgfx::ProgramHandle mLineProgram;
    static bgfx::FrameBufferHandle mBloomFrameBuffer;
    static bgfx::TextureHandle mBloomTexture;
    static bgfx::UniformHandle mBloomParams;

    // Scaling for aspect ratio preservation
    static float mRenderScale;
    static Vector2i mRenderOffset;
    static constexpr int GAME_WIDTH = 1024;
    static constexpr int GAME_HEIGHT = 768;

    // Frame timing
    static std::chrono::high_resolution_clock::time_point mLastFrameTime;
    static double mDeltaTime;

    // Input state
    static bool mShouldClose;
    static bool mKeysPressed[512]; // SDL_NUM_SCANCODES is typically 512
    static bool mGamepadButtonsPressed[32]; // Track previous frame gamepad button states
    static bool mGamepadButtonsCurrent[32]; // Track current frame gamepad button states

    static bool mShouldQuit;

    // BGFX initialization and management
    static bool InitializeBGFX();
    static void SetupRenderStates();
    static void CreateBloomResources();
    static void ShutdownBGFX();

    // Shader loading functions
    static bgfx::ProgramHandle loadProgram(const char* vsName, const char* fsName);
    static bgfx::ShaderHandle loadShader(const char* name);
};

} // namespace omegarace
