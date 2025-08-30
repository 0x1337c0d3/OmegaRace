#pragma once

#include "Enemy.h"
#include "Shot.h"
#include "Timer.h"
#include "TriShip.h"

namespace omegarace {

class LeadEnemy : public Enemy {
  public:
    LeadEnemy();
    ~LeadEnemy();

    void update(double frame);
    void draw();
    void setPlayerLocation(const Vector2f& location);
    void setInsideBorderOnShot(const SDL_Rect& border);
    void shotHitTarget();
    Vector2f getShotLocation();
    float getShotRadius();
    bool getShotActive();
    void timerPause();
    void timerUnpause();

  private:
    void fireShot();
    void checkToFire();
    void resetShotTimer();

    float m_FireTimer;
    float m_FireTimerAmount;
    Vector2f m_PlayerLocation;

    std::unique_ptr<Timer> pTimer;
    std::unique_ptr<Shot> pShot;
    std::unique_ptr<TriShip> pTriShip;
};

} // namespace omegarace
