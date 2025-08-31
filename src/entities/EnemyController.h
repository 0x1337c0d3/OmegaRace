#pragma once

#include "Common.h"
#include "Enemy.h"
#include "Fighter.h"
#include "FollowEnemy.h"
#include "LeadEnemy.h"
#include "Timer.h"
#include <vector>

namespace omegarace {

class EnemyController : public Common {
  public:
    EnemyController();
    ~EnemyController();

    void update(double frame);
    void draw();

    Fighter* getFighterPointer();
    FollowEnemy* getFollowPointer();
    LeadEnemy* getLeadPointer();

    Vector2f getEnemyLocaiton(int ship);
    void spawnNewWave(bool rightSide, int numberOfShips);
    void enemyHit(int ship);
    void leadEnemyHit();
    void followEnemyHit();
    void resumeWave();
    void setInisdeBorder(const SDL_Rect& border);
    void timerPause();
    void timerUnpause();
    void setPlayerPosition(const Vector2f& playerPos);
    void clearAllVaporTrails(); // NEW: Clear all enemy vapor trails
    bool checkEndOfWave();
    bool getEnemyCircle(const Vector2f& location, float radius, int ship);
    bool getEnemyActive(int ship);
    bool checkExploding();
    int newGame();
    int newWave();
    int restartWave();
    int getEnemyCount();
    float getEnemyRadius();

  private:
    void spawnShip(int ship);
    void placeFollowEnemy(int ship);
    void checkFighterTimer();
    void resetFighterTimer();
    void launchFighter();
    bool enemyLeft();
    int findEnemy();

    int m_NumberOfShips;
    bool m_RightSide;
    bool m_FollowActive;
    float m_EnemySpeed;
    float m_FighterTimer;
    float m_FighterTimerAmount;
    double m_Frame;

    std::unique_ptr<Timer> pTimer;
    std::unique_ptr<LeadEnemy> pLeadEnemyShip;
    std::unique_ptr<FollowEnemy> pFollowEnemyShip;
    std::unique_ptr<Fighter> pFighter;
    std::vector<std::unique_ptr<Enemy>> m_EnemyShips;
};

} // namespace omegarace
