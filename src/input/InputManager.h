#pragma once

#include "Types.h"
#include <SDL2/SDL.h>
#include <chrono>
#include <string>

namespace omegarace {

class InputManager {
public:
    static void Init();
    static void Shutdown();
    static void Update(); // Called during frame processing to update input state
    static void ProcessEvent(const SDL_Event& event); // Called from main event loop
    
    // Keyboard input
    static bool IsKeyPressed(int key);  // True only on the frame the key was pressed
    static bool IsKeyDown(int key);     // True while key is held down
    
    // Gamepad input
    static bool IsGamepadButtonPressed(int gamepad, int button);  // True only on frame pressed
    static bool IsGamepadButtonDown(int gamepad, int button);     // True while held down
    static float GetGamepadAxisMovement(int gamepad, int axis);
    
    // Controller management
    static bool IsControllerConnected();
    static std::string GetControllerName();
    static Vector2f GetControllerLeftStick();
    static Vector2f GetControllerRightStick();
    static float GetControllerLeftTrigger();
    static float GetControllerRightTrigger();
    static void updateControllerDetection();
    
private:
    // Controller detection and management
    static int findController();
    
    // Input state tracking
    static bool mKeysPressed[512];           // True only on frame key was pressed
    static bool mGamepadButtonsPressed[32];  // Previous frame button states
    static bool mGamepadButtonsCurrent[32];  // Current frame button states
    
    // Controller state
    static int mControllerIndex;
    static std::chrono::high_resolution_clock::time_point mLastControllerCheck;
    
    // Internal helper functions
    static SDL_Scancode mapKeyToScancode(int key);
    static SDL_GameControllerButton mapGamepadButton(int button);
    static SDL_GameControllerAxis mapGamepadAxis(int axis);
};

} // namespace omegarace
