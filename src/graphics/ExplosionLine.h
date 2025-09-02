#pragma once

#include "Entity.h"
#include "Timer.h"

namespace omegarace {

class ExplosionLine : public Entity {
  public:
    ExplosionLine();
    ~ExplosionLine();

    void draw();
    void update(double Frame);
    void activate(const Vector2i& location, float angle, int size);
    // Needs timer pause and unpause.
    void pauseTimer();
    void unpauseTimer();

  private:
    float m_Timer;
    int m_TimerAmount;
    std::unique_ptr<Timer> pTimer;
    float m_Alpha;
    Color m_Color;
    Line m_Line;

    // Enhanced explosion properties
    float m_ExplosionPhase;   // 0.0 to 1.0 explosion progress
    float m_InitialThickness; // Starting thickness
    float m_MaxThickness;     // Peak thickness during explosion
    float m_CurrentThickness; // Current animated thickness

    // Helper methods for enhanced effects
    Color calculateExplosionColor();
    void drawExplosionSparks();
};

} // namespace omegarace
