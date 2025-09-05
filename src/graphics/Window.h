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
    
    // Electric barrier shader-based line drawing
    static void DrawElectricBarrierLine(Line* LineLocation, const Color& LineColor, 
                                       float pulseSpeed = 15.0f, float thickness = 3.0f, float fadeTime = 0.5f);

    // Enhanced shader-based effects for Geometry Wars style
    static void DrawNeonGrid(float gridSize = 32.0f, float lineWidth = 0.02f, float glowIntensity = 1.0f, 
                            const Color& gridColor = {0, 100, 255, 80}, Vector2f* playerPos = nullptr, float warpIntensity = 0.0f);
    static void ResetGridDistortion(); // Reset grid distortion when player is inactive
    static void DrawParticleEffect(Vector2i* position, float size = 8.0f, float intensity = 1.0f, 
                                  const Color& particleColor = {255, 255, 255, 255});
    
    // Smoky vapor trail rendering with advanced shader effects
    static void DrawVaporTrailSegment(const Vector2f& start, const Vector2f& end, float width, 
                                     float trailPosition, const Color& trailColor, float alpha = 1.0f);
    static void DrawShieldGlow(Vector2i* center, float radius = 50.0f, float energy = 1.0f, 
                               const Color& shieldColor = {100, 200, 255, 180});
    static void ApplyPostProcessBloom(float threshold = 0.5f, float intensity = 1.5f, float radius = 0.01f);

    static Vector2i GetWindowSize();
    static int Random(int Min, int Max);

    static std::string dataPath();

    // Window management
    static bool ShouldClose();
    static void SetShouldClose(bool shouldClose);

    // Fullscreen support
    static bool CheckForFullscreenToggle();
    static void ToggleFullscreen();
    static bool IsFullscreen();

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
    static bgfx::ProgramHandle mGridProgram;
    static bgfx::ProgramHandle mParticleProgram;
    static bgfx::ProgramHandle mShieldProgram;
    static bgfx::ProgramHandle mPostProcessProgram;
    static bgfx::ProgramHandle mVaporTrailProgram;
    static bgfx::ProgramHandle mElectricBarrierProgram;
    static bgfx::FrameBufferHandle mBloomFrameBuffer;
    static bgfx::TextureHandle mBloomTexture;
    static bgfx::UniformHandle mBloomParams;
    static bgfx::UniformHandle mGridParams;
    static bgfx::UniformHandle mGridPlayerPos; // NEW: For grid distortion around player
    static bgfx::UniformHandle mParticleParams;
    static bgfx::UniformHandle mShieldParams;
    static bgfx::UniformHandle mVaporParams;
    static bgfx::UniformHandle mWarpParams;
    static bgfx::UniformHandle mElectricParams;

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
