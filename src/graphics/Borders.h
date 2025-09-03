#pragma once

#include "Common.h"
#include "Timer.h"
#include "Window.h"
#include <vector>

namespace omegarace {

struct DistortionSource {
    Vector2f position;
    float strength; // Multiplier for distortion strength
    float radius;   // Override radius, or use default if 0

    DistortionSource(const Vector2f& pos, float str = 1.0f, float rad = 0.0f)
        : position(pos), strength(str), radius(rad) {
    }
};

class Borders {
  public:
    Borders();
    ~Borders();

    void update();
    void draw();
    SDL_Rect getInsideBorder();
    void setInsideLineHit(int line);
    void setOutsideLineHit(int line);
    void setCentralBorderHit(); // New method for central border hits
    void initialize();
    void resetGridBackground();

  private:
    Line outsideLines[4];
    Line insideLines[4];
    Line newOutsideLines[4];
    Line newInsideLines[4];

    // Field boundary elements
    Vector2i insideDots[8];
    Line insideFieldLines[4];
    Line outsideFieldLines[8];
    Vector2i outsideDots[8];
    float insideLineTimers[4];
    float outsideLineTimers[8];
    bool insideLineOn[4];
    bool outsideLineOn[8];
    
    // Central border electric effect
    bool centralBorderElectricOn;
    float centralBorderTimer;

    // Geometry Wars grid background
    int m_GridSpacing;
    float m_GridAnimationTime;
    float m_DistortionRadius;
    float m_DistortionStrength;
    Color m_GridColor;

    std::unique_ptr<Timer> pTimer;
    SDL_Rect insideBorder;
    Color lineColor;
    Color borderColor;
};

} // namespace omegarace