#pragma once

#include "vmath.h"

// Forward declarations to avoid including raylib in every header
struct Vector2;
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

    // For legacy SDL compatibility, we define SDL_Rect in our namespace
    struct SDL_Rect {
        int x, y;
        int w, h;
    };

    struct SDL_Color {
        int r, g, b, a;
    };
}
