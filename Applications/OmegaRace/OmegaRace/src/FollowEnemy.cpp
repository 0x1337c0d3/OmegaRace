#include "FollowEnemy.h"
#include <cmath>

namespace omegarace {

FollowEnemy::FollowEnemy() {
    Enemy::initialize();
    m_MineTimerAmount = 5.5;
    pTriShip = std::make_unique<TriShip>();
    pTimer = std::make_unique<Timer>();
    pTimer->start();
    resetTimer();
    
    // Initialize menacing properties
    m_AggressionLevel = 0.5f;
    m_DistanceToPlayer = 1000.0f;
    m_InAttackMode = false;
    m_PlayerPosition = Vector2f(0, 0);
}

FollowEnemy::~FollowEnemy() {
}

void FollowEnemy::update(double Frame) {
    if (m_Active) {
        Enemy::update(Frame);
        
        // Update aggression based on proximity to player
        updateAggressionLevel();
        updateThreatDisplay();
        
        pTriShip->update(m_Location, m_Scale);
        timeToLayMine();
    }
}

void FollowEnemy::draw() {
    if (m_Active) {
        Enemy::draw();
        pTriShip->draw();
    }

    for (int mine = 0; mine < m_Mines.size(); mine++) {
        m_Mines[mine]->draw();
    }
}

void FollowEnemy::setPlayerPosition(const Vector2f& playerPos) {
    m_PlayerPosition = playerPos;
}

void FollowEnemy::updateAggressionLevel() {
    // Calculate distance to player
    Vector2f diff = m_Location - m_PlayerPosition;
    m_DistanceToPlayer = sqrt(diff.x * diff.x + diff.y * diff.y);
    
    // Closer = more aggressive (threat level increases as distance decreases)
    float maxThreatDistance = 200.0f;
    if (m_DistanceToPlayer < maxThreatDistance) {
        m_AggressionLevel = 1.0f - (m_DistanceToPlayer / maxThreatDistance);
        m_InAttackMode = m_DistanceToPlayer < 100.0f; // Very close = attack mode
    } else {
        m_AggressionLevel = 0.3f; // Base menace level
        m_InAttackMode = false;
    }
    
    // Increase aggression when low on mines (desperation)
    if (m_Mines.size() < 3) {
        m_AggressionLevel = fmin(1.0f, m_AggressionLevel + 0.3f);
    }
}

void FollowEnemy::updateThreatDisplay() {
    // Apply aggression to visual threat level
    pTriShip->setThreatLevel(m_AggressionLevel);
    pTriShip->setAggressiveMode(m_InAttackMode);
}

void FollowEnemy::newGame() {
    m_Mines.clear();
    m_Active = false;
}

Vector2f FollowEnemy::getMineLocaiton(int mine) {
    return m_Mines[mine]->getLocation();
}

float FollowEnemy::getMineRadius() {
    return m_Mines[0]->getRadius();
}

int FollowEnemy::getMineCount() {
    return (int)m_Mines.size();
}

bool FollowEnemy::getMineActive(int mine) {
    return m_Mines[mine]->getActive();
}

void FollowEnemy::mineHit(int mine) {
    m_Mines[mine]->setActive(false);
}

void FollowEnemy::timerPause() {
    pTimer->pause();
}

void FollowEnemy::timerUnpause() {
    pTimer->unpause();
}

void FollowEnemy::clearVaporTrail() {
    // Clear Enemy base class vapor trail
    Enemy::clearVaporTrail();
    
    // Clear TriShip vapor trail
    if (pTriShip) {
        pTriShip->clearVaporTrail();
    }
}

void FollowEnemy::spawnMine() {
    float scale = 3.5;
    m_Mines.push_back(std::make_unique<Mine>());
    int mine = (int)m_Mines.size() - 1;
    m_Mines[mine]->setLocation(m_Location);
    m_Mines[mine]->setScale(scale);
    m_Mines[mine]->mineDropped();
}

void FollowEnemy::timeToLayMine() {
    if (m_MineTimer < pTimer->seconds()) {
        resetTimer();
        spawnMine();
    }
}

void FollowEnemy::resetTimer() {
    m_MineTimer = m_MineTimerAmount + Window::Random(0, m_MineTimerAmount) + pTimer->seconds();
}

} // namespace omegarace
