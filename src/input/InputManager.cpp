#include "InputManager.h"
#include <algorithm>
#include <cctype>

namespace omegarace {

// Static member definitions
bool InputManager::mKeysPressed[512] = {false};
bool InputManager::mGamepadButtonsPressed[32] = {false};
bool InputManager::mGamepadButtonsCurrent[32] = {false};
int InputManager::mControllerIndex = -1;
std::chrono::high_resolution_clock::time_point InputManager::mLastControllerCheck;

void InputManager::Init() {
    // Initialize input arrays
    std::fill(std::begin(mKeysPressed), std::end(mKeysPressed), false);
    std::fill(std::begin(mGamepadButtonsPressed), std::end(mGamepadButtonsPressed), false);
    std::fill(std::begin(mGamepadButtonsCurrent), std::end(mGamepadButtonsCurrent), false);
    
    // Find initial controller
    mControllerIndex = findController();
    mLastControllerCheck = std::chrono::high_resolution_clock::now();
}

void InputManager::Shutdown() {
    // Reset controller index
    mControllerIndex = -1;
}

void InputManager::Update() {
    // Clear key pressed state for next frame
    std::fill(std::begin(mKeysPressed), std::end(mKeysPressed), false);
    
    // Update gamepad button states for next frame
    std::copy(std::begin(mGamepadButtonsCurrent), std::end(mGamepadButtonsCurrent), 
              std::begin(mGamepadButtonsPressed));
    
    // Update current gamepad state with actual controller state
    if (mControllerIndex >= 0) {
        SDL_GameController* controller = SDL_GameControllerFromInstanceID(mControllerIndex);
        if (controller) {
            // Update all face button states
            mGamepadButtonsCurrent[GAMEPAD_BUTTON_RIGHT_FACE_DOWN] = 
                SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_A) != 0;
            mGamepadButtonsCurrent[GAMEPAD_BUTTON_RIGHT_FACE_LEFT] = 
                SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_X) != 0;
            mGamepadButtonsCurrent[GAMEPAD_BUTTON_RIGHT_FACE_RIGHT] = 
                SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_B) != 0;
            mGamepadButtonsCurrent[GAMEPAD_BUTTON_RIGHT_FACE_UP] = 
                SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_Y) != 0;
            mGamepadButtonsCurrent[GAMEPAD_BUTTON_MIDDLE_LEFT] = 
                SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_BACK) != 0;
            mGamepadButtonsCurrent[GAMEPAD_BUTTON_MIDDLE_RIGHT] = 
                SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_START) != 0;
            mGamepadButtonsCurrent[GAMEPAD_BUTTON_LEFT_FACE_LEFT] = 
                SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_DPAD_LEFT) != 0;
            mGamepadButtonsCurrent[GAMEPAD_BUTTON_LEFT_FACE_RIGHT] = 
                SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_DPAD_RIGHT) != 0;
            mGamepadButtonsCurrent[GAMEPAD_BUTTON_DPAD_UP] = 
                SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_DPAD_UP) != 0;
            mGamepadButtonsCurrent[GAMEPAD_BUTTON_DPAD_DOWN] = 
                SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_DPAD_DOWN) != 0;
            mGamepadButtonsCurrent[GAMEPAD_BUTTON_RIGHT_TRIGGER_1] = 
                SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_RIGHTSHOULDER) != 0;
            
            // Debug: Check for alternative button mappings that might work for PS controllers
            static int debugCounter = 0;
            debugCounter++;
            if (debugCounter % 180 == 0) { // Every 3 seconds
                std::cout << "=== SDL Button State Debug ===" << std::endl;
                std::cout << "START button: " << (SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_START) ? "PRESSED" : "released") << std::endl;
                std::cout << "BACK button: " << (SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_BACK) ? "PRESSED" : "released") << std::endl;
                std::cout << "GUIDE button: " << (SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_GUIDE) ? "PRESSED" : "released") << std::endl;
                std::cout << "MISC1 button: " << (SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_MISC1) ? "PRESSED" : "released") << std::endl;
            }
        }
    }
    
    // Periodically check for controller changes
    updateControllerDetection();
}

void InputManager::ProcessEvent(const SDL_Event& event) {
    if (event.type == SDL_KEYDOWN) {
        // Store key press events for IsKeyPressed detection
        SDL_Scancode scancode = event.key.keysym.scancode;
        if (scancode < 512) { // SDL_NUM_SCANCODES
            mKeysPressed[scancode] = true;
        }
    }
}

bool InputManager::IsKeyPressed(int key) {
    SDL_Scancode scancode = mapKeyToScancode(key);
    if (scancode != SDL_SCANCODE_UNKNOWN) {
        return mKeysPressed[scancode];
    }
    return false;
}

bool InputManager::IsKeyDown(int key) {
    SDL_Scancode scancode = mapKeyToScancode(key);
    if (scancode != SDL_SCANCODE_UNKNOWN) {
        const Uint8* keyState = SDL_GetKeyboardState(NULL);
        return keyState[scancode];
    }
    return false;
}

bool InputManager::IsGamepadButtonPressed(int gamepad, int button) {
    // Return true only if button is currently pressed but was not pressed last frame
    if (button >= 0 && button < 32) {
        return mGamepadButtonsCurrent[button] && !mGamepadButtonsPressed[button];
    }
    return false;
}

bool InputManager::IsGamepadButtonDown(int gamepad, int button) {
    if (mControllerIndex < 0)
        return false;

    SDL_GameController* controller = SDL_GameControllerFromInstanceID(mControllerIndex);
    if (!controller)
        return false;

    SDL_GameControllerButton sdlButton = mapGamepadButton(button);
    if (sdlButton != SDL_CONTROLLER_BUTTON_INVALID) {
        return SDL_GameControllerGetButton(controller, sdlButton) != 0;
    }
    
    return false;
}

float InputManager::GetGamepadAxisMovement(int gamepad, int axis) {
    if (mControllerIndex < 0)
        return 0.0f;

    SDL_GameController* controller = SDL_GameControllerFromInstanceID(mControllerIndex);
    if (!controller)
        return 0.0f;

    SDL_GameControllerAxis sdlAxis = mapGamepadAxis(axis);
    if (sdlAxis != SDL_CONTROLLER_AXIS_INVALID) {
        Sint16 value = SDL_GameControllerGetAxis(controller, sdlAxis);
        return value / 32767.0f; // Normalize to -1.0 to 1.0
    }
    
    return 0.0f;
}

bool InputManager::IsControllerConnected() {
    return mControllerIndex >= 0;
}

std::string InputManager::GetControllerName() {
    if (mControllerIndex >= 0) {
        SDL_GameController* controller = SDL_GameControllerFromInstanceID(mControllerIndex);
        if (controller) {
            const char* name = SDL_GameControllerName(controller);
            return name ? std::string(name) : "Unknown Controller";
        }
    }
    return "No Controller";
}

Vector2f InputManager::GetControllerLeftStick() {
    if (mControllerIndex >= 0) {
        SDL_GameController* controller = SDL_GameControllerFromInstanceID(mControllerIndex);
        if (controller) {
            float x = SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_LEFTX) / 32767.0f;
            float y = SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_LEFTY) / 32767.0f;
            return {x, y};
        }
    }
    return {0.0f, 0.0f};
}

Vector2f InputManager::GetControllerRightStick() {
    if (mControllerIndex >= 0) {
        SDL_GameController* controller = SDL_GameControllerFromInstanceID(mControllerIndex);
        if (controller) {
            float x = SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_RIGHTX) / 32767.0f;
            float y = SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_RIGHTY) / 32767.0f;
            return {x, y};
        }
    }
    return {0.0f, 0.0f};
}

float InputManager::GetControllerLeftTrigger() {
    if (mControllerIndex >= 0) {
        SDL_GameController* controller = SDL_GameControllerFromInstanceID(mControllerIndex);
        if (controller) {
            return SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_TRIGGERLEFT) / 32767.0f;
        }
    }
    return 0.0f;
}

float InputManager::GetControllerRightTrigger() {
    if (mControllerIndex >= 0) {
        SDL_GameController* controller = SDL_GameControllerFromInstanceID(mControllerIndex);
        if (controller) {
            return SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_TRIGGERRIGHT) / 32767.0f;
        }
    }
    return 0.0f;
}

int InputManager::findController() {
    // Look for connected game controllers
    for (int i = 0; i < SDL_NumJoysticks(); i++) {
        if (SDL_IsGameController(i)) {
            SDL_GameController* controller = SDL_GameControllerOpen(i);
            if (controller) {
                const char* name = SDL_GameControllerName(controller);
                if (name) {
                    std::string nameStr = name;
                    std::transform(nameStr.begin(), nameStr.end(), nameStr.begin(), ::tolower);

                    // Check for PlayStation controller patterns
                    if (nameStr.find("ps4") != std::string::npos || nameStr.find("dualshock") != std::string::npos ||
                        nameStr.find("dualsense") != std::string::npos || nameStr.find("sony") != std::string::npos ||
                        nameStr.find("wireless controller") != std::string::npos) {
                        // Preferred controller found
                        return i;
                    }
                }
                // Keep first available controller as fallback
                return i;
            }
        }
    }
    return -1;
}

void InputManager::updateControllerDetection() {
    // Check if current controller is still connected
    if (mControllerIndex >= 0 && !SDL_GameControllerGetAttached(SDL_GameControllerFromInstanceID(mControllerIndex))) {
        mControllerIndex = -1;
    }

    // Check for new controllers periodically
    auto now = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - mLastControllerCheck);

    if (duration.count() > 500) { // Check every 500ms
        mLastControllerCheck = now;
        int newController = findController();
        if (newController != -1 && newController != mControllerIndex) {
            mControllerIndex = newController;
        }
    }
}

SDL_Scancode InputManager::mapKeyToScancode(int key) {
    switch (key) {
        case KEY_A: return SDL_SCANCODE_A;
        case KEY_D: return SDL_SCANCODE_D;
        case KEY_W: return SDL_SCANCODE_W;
        case KEY_S: return SDL_SCANCODE_S;
        case KEY_N: return SDL_SCANCODE_N;
        case KEY_P: return SDL_SCANCODE_P;
        case KEY_LEFT: return SDL_SCANCODE_LEFT;
        case KEY_RIGHT: return SDL_SCANCODE_RIGHT;
        case KEY_UP: return SDL_SCANCODE_UP;
        case KEY_DOWN: return SDL_SCANCODE_DOWN;
        case KEY_SPACE: return SDL_SCANCODE_SPACE;
        case KEY_LEFT_CONTROL: return SDL_SCANCODE_LCTRL;
        case KEY_ESCAPE: return SDL_SCANCODE_ESCAPE;
        case KEY_F11: return SDL_SCANCODE_F11;
        default: return SDL_SCANCODE_UNKNOWN;
    }
}

SDL_GameControllerButton InputManager::mapGamepadButton(int button) {
    switch (button) {
        case GAMEPAD_BUTTON_MIDDLE_LEFT: return SDL_CONTROLLER_BUTTON_BACK;
        case GAMEPAD_BUTTON_MIDDLE_RIGHT: return SDL_CONTROLLER_BUTTON_START;
        case GAMEPAD_BUTTON_LEFT_FACE_LEFT: return SDL_CONTROLLER_BUTTON_DPAD_LEFT;
        case GAMEPAD_BUTTON_LEFT_FACE_RIGHT: return SDL_CONTROLLER_BUTTON_DPAD_RIGHT;
        case GAMEPAD_BUTTON_DPAD_UP: return SDL_CONTROLLER_BUTTON_DPAD_UP;
        case GAMEPAD_BUTTON_DPAD_DOWN: return SDL_CONTROLLER_BUTTON_DPAD_DOWN;
        case GAMEPAD_BUTTON_RIGHT_TRIGGER_1: return SDL_CONTROLLER_BUTTON_RIGHTSHOULDER;
        case GAMEPAD_BUTTON_RIGHT_FACE_DOWN: return SDL_CONTROLLER_BUTTON_A;  // X button on PS4/5
        case GAMEPAD_BUTTON_RIGHT_FACE_LEFT: return SDL_CONTROLLER_BUTTON_X;  // Square button on PS4/5
        case GAMEPAD_BUTTON_RIGHT_FACE_RIGHT: return SDL_CONTROLLER_BUTTON_B; // Circle button on PS4/5
        case GAMEPAD_BUTTON_RIGHT_FACE_UP: return SDL_CONTROLLER_BUTTON_Y;    // Triangle button on PS4/5
        default: return SDL_CONTROLLER_BUTTON_INVALID;
    }
}

SDL_GameControllerAxis InputManager::mapGamepadAxis(int axis) {
    switch (axis) {
        case GAMEPAD_AXIS_LEFT_X: return SDL_CONTROLLER_AXIS_LEFTX;
        case GAMEPAD_AXIS_RIGHT_Y: return SDL_CONTROLLER_AXIS_RIGHTY;
        default: return SDL_CONTROLLER_AXIS_INVALID;
    }
}

} // namespace omegarace
