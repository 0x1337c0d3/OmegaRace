#pragma once

#include "AudioEngine.h"
#include "Entity.h"
#include "PlayerShip.h"
#include "PlayerShot.h"
#include "Timer.h"

namespace omegarace {

class Player : public Entity {
  public:
    Player();
    ~Player();

    void initialize();
    void draw();
    void update(double frame);
    void newGame();
    void restart();
    void hit();
    void spawn(bool rightSide = false);
    void fireButtonPressed();
    void setThrust(bool Thrust);
    void setTurnRight(bool TurnRight);
    void setTurnLeft(bool TurnLeft);
    void setShotActive(int Shot, bool Active);
    void setInsideBorder(const SDL_Rect& border);
    void timerPause();
    void timerUnpause();
    int getNumberOfShots();
    float getShotRadius();
    Vector2i getShotLocation(int Shot);
    bool getShotActive(int Shot);
    bool getShotCircle(const Vector2f& location, float radius, int shot);
    bool getHit();
    bool getExplosionOn();
    bool getInsideLineHit(int line);
    bool getOutsideLineHit(int line);
    void clearVaporTrail(); // NEW: Clear player vapor trail

    // Get offset position for firing shots from nose
    Vector2f getNosePosition();

  private:
    void updateShip();
    void updateShots(double frame);
    void updateRotationThrust();
    void updateEdge();
    void initializeShot();
    void drawShots();
    void drawThrust();
    void fireShot();
    void setExplosion();

    bool m_Thrust;
    bool m_Fire;
    bool m_TurnRight;
    bool m_TurnLeft;
    bool m_Hit;
    bool m_ExplosionOn;
    bool m_InsideLineHit[4];
    bool m_OutsideLineHit[8];
    int m_NumberOfShots;
    float m_MaxThrust;
    float m_ThrustMagnitude;
    float m_TurnRate;
    double m_ThrustDrawTimer;
    double m_ThrustDrawTimerAmount;
    double m_ExplosionTimer;
    double m_ExplosiontTimerAmount;
    SDL_Rect m_InsideBorder;

    int m_ThrustChannel;
    std::unique_ptr<Timer> pTimer;
    std::unique_ptr<PlayerShot> pShots[40];
    std::unique_ptr<PlayerShip> pShip;
    Color ShipColor;
};

} // namespace omegarace
