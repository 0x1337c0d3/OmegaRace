#pragma once

#include "Entity.h"
#include "Window.h"

namespace omegarace {

class PlayerExplosionLine : public Entity {
  public:
    PlayerExplosionLine();
    ~PlayerExplosionLine();

    void initialize(const Line& Line, const Vector2i& Location);
    void update(double frame);
    void draw();

    // Check if explosion is still active
    bool isActive() const {
        return m_ExplosionPhase < 1.0f;
    }

  private:
    Line shipLine;
    Line newLine;
    Color m_Color;

    // Catastrophic explosion properties
    float m_ExplosionPhase;    // 0.0 to 1.0 explosion timeline
    float m_MaxLifetime;       // Total explosion duration
    float m_CurrentLifetime;   // Current elapsed time
    float m_InitialThickness;  // Starting fragment thickness
    float m_MaxThickness;      // Peak explosion thickness
    float m_CurrentThickness;  // Current animated thickness
    float m_RotationSpeed;     // Fragment spinning speed
    float m_CurrentRotation;   // Current rotation angle
    float m_FragmentIntensity; // Individual fragment brightness multiplier

    // Catastrophic effect methods
    Color calculateCatastrophicColor();
    Line rotateLineAroundCenter(const Line& line, float angle);
    Line scaleLineFromCenter(const Line& line, float scale);
    float calculateFragmentScale();
    void updateThicknessAnimation();
    void drawCatastrophicArcs(const Color& baseColor);
    void drawDebrisSparks(const Color& baseColor);
};

} // namespace omegarace
