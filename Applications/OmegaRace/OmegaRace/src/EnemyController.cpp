#include "EnemyController.h"

namespace omegarace {

EnemyController::EnemyController() {
    pTimer = std::make_unique<Timer>();
    pTimer->start();
    pLeadEnemyShip = std::make_unique<LeadEnemy>();
    pFollowEnemyShip = std::make_unique<FollowEnemy>();
    pFighter = std::make_unique<Fighter>();
    pFollowEnemyShip->setActive(false);
}

EnemyController::~EnemyController() {
    m_EnemyShips.clear();
}

void EnemyController::update(double frame) {
    m_Frame = frame;

    for (int ship = 0; ship < m_EnemyShips.size(); ship++) {
        m_EnemyShips[ship]->update(frame);
    }

    pLeadEnemyShip->update(frame);
    pFollowEnemyShip->update(frame);
    pFighter->update(frame);

    checkFighterTimer();
}

void EnemyController::draw() {
    for (int ship = 0; ship < m_EnemyShips.size(); ship++) {
        m_EnemyShips[ship]->draw();
    }

    pLeadEnemyShip->draw();
    pFollowEnemyShip->draw();
    pFighter->draw();
}

int EnemyController::newGame() {
    m_FollowActive = false;
    m_EnemyShips.clear();
    m_NumberOfShips = 5;
    m_EnemySpeed = 10;
    pFollowEnemyShip->newGame();
    pFighter->newGame();
    m_FighterTimerAmount = 20.5;
    resetFighterTimer();

    return m_NumberOfShips;
}

int EnemyController::newWave() {
    m_FollowActive = true;
    m_NumberOfShips += 1;
    m_EnemySpeed += 8.5;
    pFollowEnemyShip->setActive(true);
    pLeadEnemyShip->setActive(true);
    pLeadEnemyShip->shotHitTarget();
    pFighter->shotHitTarget();

    if (m_FighterTimerAmount > 6) {
        m_FighterTimerAmount -= 3.5;
    }

    resetFighterTimer();

    return m_NumberOfShips;
}

bool EnemyController::checkEndOfWave() {
    for (int ship = 0; ship < m_EnemyShips.size(); ship++) {
        if (m_EnemyShips[ship]->getActive())
            return false;
    }

    if (pFollowEnemyShip->getActive())
        return false;

    if (pFighter->getActive())
        return false;

    if (pLeadEnemyShip->getActive())
        return false;

    return true;
}

int EnemyController::restartWave() {
    int activeCount = 0;

    for (int ship = 0; ship < m_EnemyShips.size(); ship++) {
        if (m_EnemyShips[ship]->getActive()) {
            activeCount++;
            m_EnemyShips[ship]->setActive(false);
        }
    }

    if (pFighter->getActive()) {
        if (pLeadEnemyShip->getActive())
            activeCount++;
    }

    if (pFollowEnemyShip->getActive()) {
        activeCount++;
    }

    if (m_FollowActive && activeCount > 0) {
        pFollowEnemyShip->setActive(true);
    }

    pFighter->setActive(false);
    pLeadEnemyShip->shotHitTarget();
    pFighter->shotHitTarget();

    return activeCount;
}

void EnemyController::resumeWave() {
}

void EnemyController::spawnNewWave(bool rightSide, int numberOfShips) {
    m_RightSide = rightSide;

    for (int ship = 0; ship < numberOfShips; ship++) {
        bool spawnNewShip = true;

        for (int shipCheck = 0; shipCheck < m_EnemyShips.size(); shipCheck++) {
            if (!m_EnemyShips[shipCheck]->getActive()) {
                spawnNewShip = false;
                spawnShip(shipCheck);
                break;
            }
        }

        if (spawnNewShip) {
            m_EnemyShips.push_back(std::make_unique<Enemy>());
            m_EnemyShips[m_EnemyShips.size() - 1]->initialize();
            spawnShip((int)m_EnemyShips.size() - 1);
        }
    }

    Vector2i spawnPoint;

    spawnPoint.y = Window::Random(Window::GetWindowSize().y * 0.5f + int(Window::GetWindowSize().y * 0.125f) + 25,
                                  Window::GetWindowSize().y - 25);

    if (m_RightSide)
        spawnPoint.x = Window::GetWindowSize().x - 55;
    else
        spawnPoint.x = 55;

    pLeadEnemyShip->setLocation(spawnPoint);
    pLeadEnemyShip->update(m_Frame);
    float speed = m_EnemySpeed * 1.5;
    pLeadEnemyShip->setSpeed(speed);
    pLeadEnemyShip->setRightSide(m_RightSide);
    pLeadEnemyShip->startMoving();
    pLeadEnemyShip->setTurns();
    pLeadEnemyShip->setActive(true);

    if (pFollowEnemyShip->getActive() && numberOfShips > 0) {
        placeFollowEnemy(0);
    }

    update(m_Frame);
}

void EnemyController::spawnShip(int ship) {
    Vector2i spawnPoint;

    spawnPoint.y = Window::Random(Window::GetWindowSize().y * 0.5f + int(Window::GetWindowSize().y * 0.125f) + 25,
                                  Window::GetWindowSize().y - 25);

    if (m_RightSide) {
        spawnPoint.x = Window::Random(Window::GetWindowSize().x * 0.5f, Window::GetWindowSize().x - 105);
    } else {
        spawnPoint.x = Window::Random(105, Window::GetWindowSize().x / 2);
    }

    m_EnemyShips[ship]->setLocation(spawnPoint);
    m_EnemyShips[ship]->update(m_Frame);
    m_EnemyShips[ship]->setSpeed(m_EnemySpeed);
    m_EnemyShips[ship]->setRightSide(m_RightSide);
    m_EnemyShips[ship]->startMoving();
    m_EnemyShips[ship]->setTurns();
    m_EnemyShips[ship]->setActive(true);
}

float EnemyController::getEnemyRadius() {
    return m_EnemyShips[0]->getRadius();
}

int EnemyController::getEnemyCount() {
    return (int)m_EnemyShips.size();
}

Vector2f EnemyController::getEnemyLocaiton(int ship) {
    return m_EnemyShips[ship]->getLocation();
}

bool EnemyController::getEnemyCircle(const Vector2f& location, float radius, int ship) {
    return m_EnemyShips[ship]->circlesIntersect(location, radius);
}

void EnemyController::enemyHit(int ship) {
    m_EnemyShips[ship]->explode();
    m_EnemyShips[ship]->setActive(false);
}

bool EnemyController::getEnemyActive(int ship) {
    return m_EnemyShips[ship]->getActive();
}

void EnemyController::setInisdeBorder(const SDL_Rect& border) {
    pLeadEnemyShip->setInsideBorderOnShot(border);
    pFighter->setInsideBorder(border);
}

void EnemyController::leadEnemyHit() {
    pLeadEnemyShip->explode();

    if (enemyLeft()) {
        int ship = findEnemy();
        pLeadEnemyShip->setLocation(m_EnemyShips[ship]->getLocation());
        Vector2f newVelocity = m_EnemyShips[ship]->getVelocity() * 1.5;
        pLeadEnemyShip->setVelocity(newVelocity);
        pLeadEnemyShip->setTurns(m_EnemyShips[ship]->getTurns());
        m_EnemyShips[ship]->setActive(false);
    } else if (pFollowEnemyShip->getActive()) {
        pLeadEnemyShip->setLocation(pFollowEnemyShip->getLocation());
        Vector2f newVelocity = pFollowEnemyShip->getVelocity() * 1.5;
        pLeadEnemyShip->setVelocity(newVelocity);
        pLeadEnemyShip->setTurns(pFollowEnemyShip->getTurns());
        pFollowEnemyShip->setActive(false);
    } else {
        pLeadEnemyShip->setActive(false);
    }
}

void EnemyController::followEnemyHit() {
    pFollowEnemyShip->explode();

    if (enemyLeft()) {
        placeFollowEnemy(findEnemy());
    } else {
        pFollowEnemyShip->setActive(false);
    }
}

void EnemyController::placeFollowEnemy(int ship) {
    pFollowEnemyShip->setLocation(m_EnemyShips[ship]->getLocation());
    pFollowEnemyShip->setVelocity(m_EnemyShips[ship]->getVelocity());
    pFollowEnemyShip->setRotation(m_EnemyShips[ship]->getRotation());
    pFollowEnemyShip->setSpeed(m_EnemySpeed);
    pFollowEnemyShip->setRightSide(m_RightSide);
    pFollowEnemyShip->setTurns(m_EnemyShips[ship]->getTurns());
    m_EnemyShips[ship]->setActive(false);
}

void EnemyController::checkFighterTimer() {
    if (m_FighterTimer < pTimer->seconds()) {
        if (!pFighter->getActive()) {
            launchFighter();
            resetFighterTimer();
        }
    }
}

void EnemyController::resetFighterTimer() {
    m_FighterTimer = m_FighterTimerAmount + Window::Random(0, m_FighterTimerAmount) + pTimer->seconds();
}

void EnemyController::launchFighter() {
    if (enemyLeft()) {
        int ship = findEnemy();
        pFighter->start(m_EnemyShips[ship]->getLocation(), m_EnemyShips[ship]->getVelocity());
        m_EnemyShips[ship]->setActive(false);
    } else if (pFollowEnemyShip->getActive()) {
        pFighter->start(pFollowEnemyShip->getLocation(), pFollowEnemyShip->getVelocity());
        pFollowEnemyShip->setActive(false);
    } else if (pLeadEnemyShip->getActive()) {
        pFighter->start(pLeadEnemyShip->getLocation(), pLeadEnemyShip->getVelocity());
        pLeadEnemyShip->setActive(false);
    }
}

bool EnemyController::enemyLeft() {
    for (int ship = 0; ship < m_EnemyShips.size(); ship++) {
        if (m_EnemyShips[ship]->getActive()) {
            return true;
        }
    }

    return false;
}

int EnemyController::findEnemy() {
    bool shipActive = false;
    int ship = 0;

    while (!shipActive) {
        ship = Window::Random(0, (int)m_EnemyShips.size() - 1);
        shipActive = m_EnemyShips[ship]->getActive();
    }

    return ship;
}

Fighter* EnemyController::getFighterPointer() {
    return pFighter.get();
}

FollowEnemy* EnemyController::getFollowPointer() {
    return pFollowEnemyShip.get();
}

LeadEnemy* EnemyController::getLeadPointer() {
    return pLeadEnemyShip.get();
}

void EnemyController::timerPause() {
    pTimer->pause();
    pFighter->timerPause();
    pFollowEnemyShip->timerPause();
    pLeadEnemyShip->timerPause();
}

void EnemyController::timerUnpause() {
    pTimer->unpause();
    pFighter->timerUnpause();
    pFollowEnemyShip->timerUnpause();
    pLeadEnemyShip->timerUnpause();
}

bool EnemyController::checkExploding() {
    bool explodeing = false;

    if (pLeadEnemyShip->getExplosionActive())
        explodeing = true;

    if (pFighter->getExplosionActive())
        explodeing = true;

    return explodeing;
}

void EnemyController::setPlayerPosition(const Vector2f& playerPos) {
    // Update FollowEnemy with player position for menacing effects
    if (pFollowEnemyShip) {
        pFollowEnemyShip->setPlayerPosition(playerPos);
    }
}

} // namespace omegarace
