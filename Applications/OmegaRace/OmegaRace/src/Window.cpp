#include "Window.h"
#include <rlgl.h>  // For matrix operations
#include <algorithm>
#include <cctype>
#include <filesystem>
#ifdef __APPLE__
#include <mach-o/dyld.h>
#endif

namespace omegarace {

// Random Number Generator
std::mt19937 m_Random;

// Static members - much simpler with Raylib
Rectangle Window::mBox;
int Window::mControllerIndex = -1;
bool Window::mIsFullscreen = false;
int Window::mWindowedWidth = 1024;
int Window::mWindowedHeight = 768;

// Scaling for aspect ratio preservation
float Window::mRenderScale = 1.0f;
Vector2i Window::mRenderOffset = {0, 0};

void Window::updateControllerDetection() {
    // Check if current controller is still connected
    if (mControllerIndex >= 0 && !IsGamepadAvailable(mControllerIndex)) {
        mControllerIndex = -1;
    }
    
    // Always check for controller connections (not just when no controller is detected)
    // This handles cases where a controller was connected but not detected, or
    // when switching between multiple controllers
    static int lastCheckFrame = 0;
    static int currentFrame = 0;
    currentFrame++;
    
    // Check every 30 frames (approximately twice per second at 60 FPS) - more frequent
    if (currentFrame - lastCheckFrame >= 30) {
        lastCheckFrame = currentFrame;
        int newController = findController();
        if (newController != -1 && newController != mControllerIndex) {
            mControllerIndex = newController;
        }
    }
}

std::string Window::dataPath() {
#ifdef __APPLE__
    // Get the path to the executable on macOS
    char path[1024];
    uint32_t size = sizeof(path);
    if (_NSGetExecutablePath(path, &size) == 0) {
        std::filesystem::path execPath(path);
        std::filesystem::path resourcePath = execPath.parent_path() / ".." / "Resources";
        return resourcePath.string();
    }
#endif
    
    // Fallback to relative path if unable to get executable path
    return "../Resources";
}

// Shader support for advanced effects
static Shader bloomShader = {0};  // Static shader storage
static bool shaderLoaded = false;

void Window::Init(int width, int height, std::string title) {
    m_Random.seed((unsigned int)time(0));

    // Store initial windowed dimensions
    mWindowedWidth = width;
    mWindowedHeight = height;
    mIsFullscreen = false;

    // Initialize Raylib window with resizable flag
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(width, height, title.c_str());
    SetTargetFPS(60);  // Enable VSync equivalent
    
    // Initialize audio system
    InitAudioDevice();
    
    // Set up controller
    mControllerIndex = findController();
    
    // Set up window box
    mBox.x = 0;
    mBox.y = 0;
    mBox.width = (float)width;
    mBox.height = (float)height;
        
    // Enable alpha blending for volumetric effects
    // Raylib handles this automatically
    
    // Load bloom shader for enhanced volumetric lines
    std::string shaderPath = dataPath() + "/shaders/bloom.fs";
    LoadBloomShader(shaderPath);
}

void Window::Quit() {
    CloseAudioDevice();
    CloseWindow();
}

void Window::logRaylibError(std::ostream& os, const std::string& msg) {
    os << msg << " error: " << std::endl;
}

void Window::Present() {
    // Raylib handles this automatically in EndDrawing()
    // This function is now essentially a no-op, but kept for compatibility
}

Rectangle Window::Box() {
    mBox.width = (float)GAME_WIDTH;  // Always return logical game dimensions
    mBox.height = (float)GAME_HEIGHT;
    return mBox;
}

void Window::DrawLine(Line* LineLocation, const Color& LineColor) {
    // Convert custom Color to Raylib Color
    ::Color rlColor = {
        (unsigned char)LineColor.red,
        (unsigned char)LineColor.green,
        (unsigned char)LineColor.blue,
        (unsigned char)LineColor.alpha
    };
    
    // Convert Vector2i to Raylib Vector2
    ::Vector2 start = {(float)LineLocation->start.x, (float)LineLocation->start.y};
    ::Vector2 end = {(float)LineLocation->end.x, (float)LineLocation->end.y};
    
    DrawLineV(start, end, rlColor);
}

void Window::DrawVolumetricLine(Line* LineLocation, const Color& LineColor, float thickness) {
    // Use the bloom version with moderate bloom intensity for enhanced visuals
    DrawVolumetricLineWithBloom(LineLocation, LineColor, thickness, 0.3f);
}

void Window::DrawVolumetricLineWithBloom(Line* LineLocation, const Color& LineColor, 
                                        float thickness, float bloomIntensity) {
    // Convert custom Color to Raylib Color
    ::Color rlColor = {
        (unsigned char)LineColor.red,
        (unsigned char)LineColor.green,
        (unsigned char)LineColor.blue,
        (unsigned char)LineColor.alpha
    };
    
    ::Vector2 start = {(float)LineLocation->start.x, (float)LineLocation->start.y};
    ::Vector2 end = {(float)LineLocation->end.x, (float)LineLocation->end.y};
    
    // Performance optimization: Use simpler rendering
    if (bloomIntensity <= 0.5f) {
        // Low bloom - just draw core line with slight glow
        DrawLineEx(start, end, thickness, rlColor);
        if (bloomIntensity > 0.2f) {
            unsigned char glowAlpha = (unsigned char)(rlColor.a * bloomIntensity * 0.3f);
            ::Color glowColor = {rlColor.r, rlColor.g, rlColor.b, glowAlpha};
            DrawLineEx(start, end, thickness + 1.0f, glowColor);
        }
    } else {
        // Higher bloom - use 2 passes maximum instead of 3+
        DrawLineEx(start, end, thickness, rlColor);
        
        // Single bloom pass
        float bloomThickness = thickness + 2.0f;
        unsigned char bloomAlpha = (unsigned char)(rlColor.a * fmin(bloomIntensity, 1.0f) * 0.4f);
        ::Color bloomColor = {rlColor.r, rlColor.g, rlColor.b, bloomAlpha};
        DrawLineEx(start, end, bloomThickness, bloomColor);
    }
}

void Window::DrawPoint(Vector2i* Location, const Color& PointColor) {
    ::Color rlColor = {
        (unsigned char)PointColor.red,
        (unsigned char)PointColor.green,
        (unsigned char)PointColor.blue,
        (unsigned char)PointColor.alpha
    };
    
    DrawPixel(Location->x, Location->y, rlColor);
}

void Window::DrawRect(const Rectangle* RectangleLocation, const Color& RectangleColor) {
    ::Color rlColor = {
        (unsigned char)RectangleColor.red,
        (unsigned char)RectangleColor.green,
        (unsigned char)RectangleColor.blue,
        (unsigned char)RectangleColor.alpha
    };
    
    DrawRectangleLinesEx(*RectangleLocation, 1.0f, rlColor);  // Draw outline only
}

Vector2i Window::GetWindowSize() {
    return { GAME_WIDTH, GAME_HEIGHT }; // Always return logical game dimensions
}

int Window::Random(int Min, int Max) {
    std::uniform_int_distribution<uint32_t> roll(Min, Max);
    return roll(m_Random);
}

int Window::findController() {
    // Enhanced gamepad detection with PS4 controller support
    
    for (int i = 0; i < 16; i++) {  // Check up to 16 gamepads (increased from 4)
        if (IsGamepadAvailable(i)) {
            const char* gamepadName = GetGamepadName(i);            
            if (gamepadName) {
                std::string name = gamepadName;
                // Convert to lowercase for case-insensitive matching
                std::transform(name.begin(), name.end(), name.begin(), ::tolower);
                
                bool isPlayStationController = false;
                std::string controllerType = "Generic";
                
                // Check for PlayStation controller patterns
                if (name.find("ps4") != std::string::npos ||
                    name.find("dualshock") != std::string::npos ||
                    name.find("dualshock 4") != std::string::npos) {
                    isPlayStationController = true;
                    controllerType = "PS4 DualShock 4";
                } else if (name.find("dualsense") != std::string::npos) {
                    isPlayStationController = true;
                    controllerType = "PS5 DualSense";
                } else if (name.find("sony") != std::string::npos) {
                    isPlayStationController = true;
                    controllerType = "Sony Controller";
                } else if (name.find("wireless controller") != std::string::npos) {
                    // This could be either PS4 or PS5, but we'll accept it
                    isPlayStationController = true;
                    controllerType = "PlayStation Wireless Controller";
                } else if (name.find("controller") != std::string::npos && 
                          (name.find("(wireless") != std::string::npos || 
                           name.find("(dualshock") != std::string::npos)) {
                    // Handle cases like "Controller (Wireless Controller)"
                    isPlayStationController = true;
                    controllerType = "PlayStation Controller";
                }
            }
            
            return i;  // Return first available gamepad (prioritizes PlayStation controllers due to order)
        }
    }
    return -1;
}

void Window::BeginDrawing() {
    ::BeginDrawing();

    int screenWidth = GetScreenWidth();
    int screenHeight = GetScreenHeight();
    
    // Calculate uniform scale to preserve aspect ratio
    float scaleX = (float)screenWidth / GAME_WIDTH;
    float scaleY = (float)screenHeight / GAME_HEIGHT;
    mRenderScale = fmin(scaleX, scaleY);  // Use smaller scale to fit entirely
    
    // Calculate centering offset
    float scaledWidth = GAME_WIDTH * mRenderScale;
    float scaledHeight = GAME_HEIGHT * mRenderScale;
    mRenderOffset.x = (int)((screenWidth - scaledWidth) / 2.0f);
    mRenderOffset.y = (int)((screenHeight - scaledHeight) / 2.0f);

    // Use a subtle dark gray for letterboxing instead of pure black
    ClearBackground((::Color){20, 20, 24, 255});
    
    rlPushMatrix();
    rlTranslatef((float)mRenderOffset.x, (float)mRenderOffset.y, 0.0f);
    rlScalef(mRenderScale, mRenderScale, 1.0f);
}
 
void Window::EndDrawing() {
    // Restore the transformation matrix
    rlPopMatrix();
    ::EndDrawing();
}

bool Window::ShouldClose() {
    return WindowShouldClose();
}

void Window::LoadBloomShader(const std::string& fragmentShaderPath) {
    if (!shaderLoaded) {
        // Load shader (vertex shader can be 0 for default)
        bloomShader = LoadShader(0, fragmentShaderPath.c_str());
        shaderLoaded = (bloomShader.id > 0);
        
        if (shaderLoaded) {
            // Set up shader uniforms for bloom effect
            int bloomIntensityLoc = GetShaderLocation(bloomShader, "bloomIntensity");
            int bloomRadiusLoc = GetShaderLocation(bloomShader, "bloomRadius");
            int bloomThresholdLoc = GetShaderLocation(bloomShader, "bloomThreshold");
            
            // Set default values
            float defaultIntensity = 1.0f;
            float defaultRadius = 2.0f;
            float defaultThreshold = 0.8f;
            
            SetShaderValue(bloomShader, bloomIntensityLoc, &defaultIntensity, SHADER_UNIFORM_FLOAT);
            SetShaderValue(bloomShader, bloomRadiusLoc, &defaultRadius, SHADER_UNIFORM_FLOAT);
            SetShaderValue(bloomShader, bloomThresholdLoc, &defaultThreshold, SHADER_UNIFORM_FLOAT);
        }
    }
}

void Window::BeginBloomMode() {
    if (shaderLoaded && bloomShader.id > 0) {
        BeginShaderMode(bloomShader);
    }
}

void Window::EndBloomMode() {
    if (shaderLoaded && bloomShader.id > 0) {
        EndShaderMode();
    }
}

// Enhanced input support
bool Window::IsControllerButtonPressed(int button) {
    if (mControllerIndex >= 0) {
        return IsGamepadButtonPressed(mControllerIndex, button);
    }
    return false;
}

::Vector2 Window::GetControllerLeftStick() {
    if (mControllerIndex >= 0) {
        return {
            GetGamepadAxisMovement(mControllerIndex, GAMEPAD_AXIS_LEFT_X),
            GetGamepadAxisMovement(mControllerIndex, GAMEPAD_AXIS_LEFT_Y)
        };
    }
    return {0, 0};
}

// NEW: Enhanced PS4 controller support functions
bool Window::IsControllerButtonDown(int button) {
    if (mControllerIndex >= 0) {
        return IsGamepadButtonDown(mControllerIndex, button);
    }
    return false;
}

::Vector2 Window::GetControllerRightStick() {
    if (mControllerIndex >= 0) {
        return {
            GetGamepadAxisMovement(mControllerIndex, GAMEPAD_AXIS_RIGHT_X),
            GetGamepadAxisMovement(mControllerIndex, GAMEPAD_AXIS_RIGHT_Y)
        };
    }
    return {0, 0};
}

float Window::GetControllerLeftTrigger() {
    if (mControllerIndex >= 0) {
        return GetGamepadAxisMovement(mControllerIndex, GAMEPAD_AXIS_LEFT_TRIGGER);
    }
    return 0.0f;
}

float Window::GetControllerRightTrigger() {
    if (mControllerIndex >= 0) {
        return GetGamepadAxisMovement(mControllerIndex, GAMEPAD_AXIS_RIGHT_TRIGGER);
    }
    return 0.0f;
}

bool Window::IsControllerConnected() {
    return mControllerIndex >= 0 && IsGamepadAvailable(mControllerIndex);
}

std::string Window::GetControllerName() {
    if (mControllerIndex >= 0) {
        const char* name = GetGamepadName(mControllerIndex);
        return name ? std::string(name) : "Unknown Controller";
    }
    return "No Controller";
}

// FPS display functions
void Window::DrawFPS(int x, int y) {
    // Raylib's built-in FPS display function
    // ::DrawFPS(x, y);
}

void Window::PrintFPS() {
    // Print FPS to console
    static int frameCounter = 0;
    static float timeAccumulator = 0.0f;
    
    frameCounter++;
    timeAccumulator += GetFrameTime();
    
    // Print FPS every second
    if (timeAccumulator >= 1.0f) {
        frameCounter = 0;
        timeAccumulator = 0.0f;
    }
}

// Full screen warp effect for dramatic wave transitions
void Window::DrawFullScreenWarp(float intensity, float time) {
    // Use logical game dimensions instead of physical screen dimensions
    ::Vector2 center = { (float)GAME_WIDTH / 2.0f, (float)GAME_HEIGHT / 2.0f };
    
    // Create multiple concentric rings expanding outward
    for (int ring = 0; ring < 8; ring++) {
        float ringPhase = time * 3.0f + ring * 0.5f;
        float radius = (50.0f + ring * 40.0f) * intensity;
        
        // Pulsing warp rings with distortion
        float distortion = sinf(ringPhase) * 0.3f + 1.0f;
        float actualRadius = radius * distortion;
        
        // Color shifts from blue to white to cyan
        unsigned char alpha = (unsigned char)(128 * intensity * (1.0f - ring / 8.0f));
        ::Color ringColor;
        if (ring % 2 == 0) {
            ringColor = { 0, 150, 255, alpha };  // Bright blue
        } else {
            ringColor = { 100, 255, 255, alpha }; // Cyan
        }
        
        // Draw thick ring with bloom effect
        DrawRingLines(center, actualRadius - 8.0f, actualRadius + 8.0f, 0, 360, 32, ringColor);
    }
    
    // Radial lines emanating from center
    for (int i = 0; i < 16; i++) {
        float angle = (i * 22.5f) + time * 45.0f; // Rotating radial lines
        float lineLength = 400.0f * intensity;
        
        // Convert to radians manually (DEG2RAD equivalent)
        float angleRad = angle * PI / 180.0f;
        
        ::Vector2 endPos = {
            center.x + cosf(angleRad) * lineLength,
            center.y + sinf(angleRad) * lineLength
        };
        
        // Alternating line colors
        ::Color lineColor = (i % 2 == 0) ? 
            (::Color){ 255, 255, 255, (unsigned char)(100 * intensity) } :  // White
            (::Color){ 0, 255, 255, (unsigned char)(80 * intensity) };      // Cyan
            
        DrawLineEx(center, endPos, 3.0f, lineColor);
    }
    
    // Central burst effect
    float burstRadius = 30.0f * intensity * (1.0f + sinf(time * 8.0f) * 0.3f);
    ::Color burstColor = { 255, 255, 255, (unsigned char)(200 * intensity) };
    DrawCircleLines((int)center.x, (int)center.y, burstRadius, burstColor);
}

void Window::BeginWarpTransition() {
    // Set additive blending for intense warp effect
    BeginBlendMode(BLEND_ADDITIVE);
}

void Window::EndWarpTransition() {
    // Restore normal blending
    EndBlendMode();
}

bool Window::CheckForFullscreenToggle() {
    // Check both fullscreen state and window size changes
    bool currentFullscreenState = IsWindowFullscreen();
    int currentWidth = GetScreenWidth();
    int currentHeight = GetScreenHeight();
    
    // Check if either fullscreen state changed OR window size changed
    bool stateChanged = (currentFullscreenState != mIsFullscreen);
    bool sizeChanged = (currentWidth != (int)mBox.width || currentHeight != (int)mBox.height);
    
    if (stateChanged || sizeChanged) {
        mIsFullscreen = currentFullscreenState;
        
        // Update mBox dimensions to match new screen size
        mBox.width = (float)currentWidth;
        mBox.height = (float)currentHeight;
                
        return true;  // Screen size or state changed
    }
    
    return false;  // No change
}

void Window::ToggleFullscreen() {
    ::ToggleFullscreen();
    mIsFullscreen = IsWindowFullscreen();
    
    // Update mBox dimensions
    mBox.width = (float)GetScreenWidth();
    mBox.height = (float)GetScreenHeight();
}

bool Window::IsFullscreen() {
    return mIsFullscreen;
}

} // namespace omegarace
