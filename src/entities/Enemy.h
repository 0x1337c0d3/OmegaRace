#pragma once

#include "Entity.h"
#include "Explosion.h"
#include "Ship.h"

namespace omegarace {

struct WhenToTurn {
    int left;
    int right;
    int top;
    int bottom;
};

class Enemy : public Entity {
  public:
    Enemy();
    ~Enemy();

    void initialize();
    void update(double Frame);
    void draw();
    void setRightSide(bool rightSide);
    void setTurns();
    void startMoving();
    void setSpeed(float speed);
    void setTurns(WhenToTurn turns);
    void explode();
    bool getExplosionActive();
    WhenToTurn getTurns();
    void clearVaporTrail(); // NEW: Clear vapor trail

  private:
    std::unique_ptr<Ship> pShip;
    std::unique_ptr<Explosion> pExplosion;
    WhenToTurn m_WhenToTurn;
    bool m_RightSide;

  protected:
    float m_MoveSpeed;

    void checkForTurn();
};

} // namespace omegarace
