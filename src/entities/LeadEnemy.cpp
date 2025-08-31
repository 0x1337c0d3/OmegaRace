#include "LeadEnemy.h"

namespace omegarace {

LeadEnemy::LeadEnemy() {
    m_FireTimerAmount = 2.8;
    pTriShip = std::make_unique<TriShip>();
    pShot = std::make_unique<Shot>();
    pTimer = std::make_unique<Timer>();
    pTimer->start();
    resetShotTimer();
    Enemy::initialize();
}

LeadEnemy::~LeadEnemy() {
}

void LeadEnemy::update(double frame) {
    if (m_Active) {
        pTriShip->update(m_Location, m_Scale);
        checkToFire();
    }

    Enemy::update(frame);
    pShot->update(frame);
}

void LeadEnemy::draw() {
    if (m_Active) {
        pTriShip->draw();
    }

    Enemy::draw();
    pShot->draw();
}

void LeadEnemy::setPlayerLocation(const Vector2f& location) {
    m_PlayerLocation = location;
}

void LeadEnemy::setInsideBorderOnShot(const SDL_Rect& border) {
    pShot->setInsideBorder(border);
}

Vector2f LeadEnemy::getShotLocation() {
    return pShot->getLocation();
}

float LeadEnemy::getShotRadius() {
    return pShot->getRadius();
}

bool LeadEnemy::getShotActive() {
    return pShot->getActive();
}

void LeadEnemy::shotHitTarget() {
    pShot->setActive(false);
}

void LeadEnemy::timerPause() {
    pTimer->pause();
}

void LeadEnemy::timerUnpause() {
    pTimer->unpause();
}

void LeadEnemy::fireShot() {
    float angle = atan2(m_PlayerLocation.y - m_Location.y, m_PlayerLocation.x - m_Location.x);
    pShot->activate(m_Location, angle);
    resetShotTimer();
}

void LeadEnemy::checkToFire() {
    if (m_FireTimer < pTimer->seconds())
        fireShot();
}

void LeadEnemy::resetShotTimer() {
    m_FireTimer = m_FireTimerAmount + pTimer->seconds() + Window::Random(0, (int)(m_FireTimerAmount * 10) * 0.1);
}

void LeadEnemy::clearVaporTrail() {
    // Clear Enemy base class vapor trail
    Enemy::clearVaporTrail();
    
    // Clear TriShip vapor trail
    if (pTriShip) {
        pTriShip->clearVaporTrail();
    }
}

} // namespace omegarace
