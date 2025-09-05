#pragma once

#include "vmath.h"

// Vector2 struct to replace Raylib's Vector2
struct Vector2 {
    float x, y;
    Vector2() : x(0), y(0) {
    }
    Vector2(float x, float y) : x(x), y(y) {
    }
};

// Forward declarations
struct Vector3;

namespace omegarace {
// Line struct that was previously in Window.h
struct Line {
    Vector2i start;
    Vector2i end;
};

// Color struct from Window.h - used for drawing
struct Color {
    int red;
    int green;
    int blue;
    int alpha;
};

// Rectangle struct - equivalent to Raylib's Rectangle
struct Rectangle {
    float x, y;
    float width, height;
};

// For legacy SDL compatibility, we define SDL_Rect in our namespace
struct SDL_Rect {
    int x, y;
    int w, h;
};

struct SDL_Color {
    int r, g, b, a;
};

// Key constants - mapping from Raylib to SDL2 equivalents
// These will be used in our input wrapper functions
enum KeyCode {
    KEY_A = 97,
    KEY_D = 100,
    KEY_W = 119,
    KEY_S = 115,
    KEY_N = 110,
    KEY_P = 112,
    KEY_T = 116,
    KEY_LEFT = 1073741904,
    KEY_RIGHT = 1073741903,
    KEY_UP = 1073741906,
    KEY_DOWN = 1073741905,
    KEY_SPACE = 32,
    KEY_LEFT_CONTROL = 1073742048,
    KEY_ESCAPE = 27,
    KEY_F11 = 1073741882
};

// Gamepad button constants
enum GamepadButton {
    GAMEPAD_BUTTON_MIDDLE_LEFT = 6,      // Select/Back
    GAMEPAD_BUTTON_MIDDLE_RIGHT = 7,     // Start
    GAMEPAD_BUTTON_LEFT_FACE_LEFT = 13,  // D-pad Left
    GAMEPAD_BUTTON_LEFT_FACE_RIGHT = 14, // D-pad Right
    GAMEPAD_BUTTON_DPAD_UP = 11,         // D-pad Up
    GAMEPAD_BUTTON_DPAD_DOWN = 12,       // D-pad Down
    GAMEPAD_BUTTON_RIGHT_TRIGGER_1 = 9,  // R1/RB
    GAMEPAD_BUTTON_RIGHT_FACE_DOWN = 0,  // A/X button
    GAMEPAD_BUTTON_RIGHT_FACE_LEFT = 2,  // X/Square button
    GAMEPAD_BUTTON_RIGHT_FACE_RIGHT = 1, // B/Circle button
    GAMEPAD_BUTTON_RIGHT_FACE_UP = 3,    // Y/Triangle button
    GAMEPAD_BUTTON_A = 0,                // Alternative name for A/X button
    GAMEPAD_BUTTON_X = 2                 // Alternative name for X/Square button
};

// Gamepad axis constants
enum GamepadAxis {
    GAMEPAD_AXIS_LEFT_X = 0,
    GAMEPAD_AXIS_LEFT_Y = 1,
    GAMEPAD_AXIS_RIGHT_X = 2,
    GAMEPAD_AXIS_RIGHT_Y = 3,
    GAMEPAD_AXIS_LEFT_TRIGGER = 4,
    GAMEPAD_AXIS_RIGHT_TRIGGER = 5
};
} // namespace omegarace
