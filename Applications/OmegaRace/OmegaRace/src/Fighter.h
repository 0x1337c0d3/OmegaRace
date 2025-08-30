#pragma once

#include "DoubleMine.h"
#include "Entity.h"
#include "Explosion.h"
#include "FighterShip.h"
#include "Shot.h"
#include "Timer.h"
#include "TriShip.h"

namespace omegarace {

class Fighter : public Entity {
  public:
    Fighter();
    ~Fighter();

    void update(double frame);
    void draw();
    void setPlayerLocation(const Vector2f& location);
    void setInsideBorder(const SDL_Rect& border);
    bool getInsideLineHit(int line);
    bool getOutsideLineHit(int line);
    Vector2f getMineLocaiton(int mine);
    float getMineRadius();
    int getMineCount();
    bool getMineActive(int mine);
    void mineHit(int mine);
    Vector2f getShotLocation();
    void shotHitTarget();
    float getShotRadius();
    bool getShotActive();
    void newGame();
    void start(const Vector2f& location, const Vector2f& velocity);
    void timerPause();
    void timerUnpause();
    void explode();
    bool getExplosionActive();

  private:
    void resetMineTimer();
    void resetShotTimer();
    void resetChangeVectorTimer();
    void timerCheck();
    void layMine();
    void shotPlayer();
    void changeVector();
    void checkBorders();

    std::unique_ptr<Timer> pTimer;
    std::unique_ptr<TriShip> pShip;
    std::unique_ptr<Shot> pShot;
    std::unique_ptr<FighterShip> pBlade;
    std::unique_ptr<Explosion> pExplosion;
    std::vector<std::unique_ptr<DoubleMine>> m_Mines;

    Vector2f m_PlayerLocation;
    SDL_Rect m_InsideBorder;
    bool m_InsideLineHit[4];
    bool m_OutsideLineHit[8];
    float m_MineTimer;
    float m_MineTimerAmount;
    float m_ShotTimer;
    float m_ShotTimerAmount;
    float m_ChangeVectorTimer;
    float m_ChangeVectorTimerAmount;
    float m_Speed;
    double m_Frame;
};

} // namespace omegarace
