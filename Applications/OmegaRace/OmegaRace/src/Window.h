#pragma once

#include "Common.h"
#include "vmath.h"

// Raylib includes
#include <raylib.h>

#include <iostream>
#include <memory>
#include <random>
#include <sstream>
#include <stdexcept>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <time.h>

namespace omegarace {

struct Line {
    Vector2i start;
    Vector2i end;
};

struct Color {
    int red;
    int green;
    int blue;
    int alpha;
};

struct SDL_Rect {
    int x;
    int y;
    int w;
    int h;
};

struct SDL_Color {
    int r;
    int g;
    int b;
    int a;
};

class Window {
  public:
    static void Init(int width, int height, std::string title = "Raylib Window");
    static void Quit();
    static void Present();  // Kept for compatibility, but largely unused in Raylib
    static Rectangle Box();
    static void logRaylibError(std::ostream& os, const std::string& msg);
    
    // Basic drawing functions (converted from SDL)
    static void DrawLine(Line* LineLocation, const Color& LineColor);
    static void DrawPoint(Vector2i* PointLocation, const Color& PointColor);
    static void DrawRect(const Rectangle* RectangleLocation, const Color& RectColor);
    
    // NEW: Enhanced line drawing with volumetric effects
    static void DrawVolumetricLine(Line* LineLocation, const Color& LineColor, float thickness = 3.0f);
    static void DrawVolumetricLineWithBloom(Line* LineLocation, const Color& LineColor, 
                                           float thickness = 3.0f, float bloomIntensity = 0.5f);
    
    static Vector2i GetWindowSize();
    static int Random(int Min, int Max);

    static std::string dataPath();
    static int findController();
    static void updateControllerDetection();  // Periodic controller detection
    static int mControllerIndex;  // Changed from SDL_GameController*

    // NEW: Raylib-specific functions
    static void BeginDrawing();
    static void EndDrawing();
    static bool ShouldClose();
    
    // NEW: Shader support for advanced effects
    static void LoadBloomShader(const std::string& fragmentShaderPath);
    static void BeginBloomMode();
    static void EndBloomMode();
    
    // NEW: Enhanced controller support
    static bool IsControllerButtonPressed(int button);
    static bool IsControllerButtonDown(int button);
    static ::Vector2 GetControllerLeftStick();
    static ::Vector2 GetControllerRightStick();
    static float GetControllerLeftTrigger();
    static float GetControllerRightTrigger();
    static bool IsControllerConnected();
    static std::string GetControllerName();
    
    // NEW: FPS display
    static void DrawFPS(int x = 10, int y = 10);
    static void PrintFPS();  // Console output
    
    // NEW: Full screen warp effect for wave transitions
    static void DrawFullScreenWarp(float intensity = 1.0f, float time = 0.0f);
    static void BeginWarpTransition();
    static void EndWarpTransition();
    
    // NEW: Fullscreen support
    static bool CheckForFullscreenToggle();  // Returns true if screen size changed
    static void ToggleFullscreen();
    static bool IsFullscreen();

  private:
    static Rectangle mBox;  // Changed from SDL_Rect
    static bool mIsFullscreen;  // Track fullscreen state
    static int mWindowedWidth;  // Store windowed dimensions
    static int mWindowedHeight;
    
    // Scaling for aspect ratio preservation
    static float mRenderScale;
    static Vector2i mRenderOffset;
    static constexpr int GAME_WIDTH = 1024;
    static constexpr int GAME_HEIGHT = 768;
    // Note: No need for window/renderer pointers - Raylib manages these internally
};

} // namespace omegarace
