#pragma once

#include "Enemy.h"
#include "Mine.h"
#include "Timer.h"
#include "TriShip.h"

namespace omegarace {

class FollowEnemy : public Enemy {
  public:
    FollowEnemy();
    ~FollowEnemy();

    void update(double Frame);
    void draw();
    void newGame();
    Vector2f getMineLocaiton(int mine);
    float getMineRadius();
    int getMineCount();
    bool getMineActive(int mine);
    void mineHit(int mine);
    void timerPause();
    void timerUnpause();
    void setPlayerPosition(const Vector2f& playerPos);
    void clearVaporTrail(); // NEW: Clear TriShip vapor trail

  private:
    std::unique_ptr<TriShip> pTriShip;
    std::vector<std::unique_ptr<Mine>> m_Mines;
    std::unique_ptr<Timer> pTimer = nullptr;

    float m_MineTimer;
    float m_MineTimerAmount;
    Vector2f m_PlayerPosition;
    float m_AggressionLevel;
    float m_DistanceToPlayer;
    bool m_InAttackMode;

    void spawnMine();
    void timeToLayMine();
    void resetTimer();
    void updateAggressionLevel();
    void updateThreatDisplay();
};

} // namespace omegarace
