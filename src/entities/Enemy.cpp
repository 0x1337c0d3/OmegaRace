#include "Enemy.h"

namespace omegarace {

Enemy::Enemy() {
    m_Radius = 16;
    m_Scale = 3;
    m_Rectangle.h = 16;
    m_Rectangle.w = 16;
}

Enemy::~Enemy() {
}

void Enemy::initialize() {
    pShip = std::make_unique<Ship>();
    pShip->initialize();
    pExplosion = std::make_unique<Explosion>();
}

void Enemy::update(double Frame) {
    updateFrame(Frame);

    if (m_Active) {
        checkForTurn();
        // Pass velocity information to ship for dynamic vapor trails
        pShip->update(m_Rotation.amount, m_Location, m_Scale, m_Velocity);
    }

    pExplosion->update(Frame);
}

void Enemy::draw() {
    if (m_Active)
        pShip->draw();

    pExplosion->draw();
}

void Enemy::setRightSide(bool rightSide) {
    m_RightSide = rightSide;
}

WhenToTurn Enemy::getTurns() {
    return m_WhenToTurn;
}

void Enemy::setTurns(WhenToTurn turns) {
    m_WhenToTurn = turns;
}

void Enemy::setTurns() {
    m_WhenToTurn.left = Window::Random(25, int(Window::GetWindowSize().x * 0.15625f) - 8);
    m_WhenToTurn.right = Window::Random(int(Window::GetWindowSize().x * 0.84375f) + 8, Window::GetWindowSize().x - 25);
    m_WhenToTurn.top = Window::Random(25, int(Window::GetWindowSize().y * 0.3335f));
    m_WhenToTurn.bottom = m_Location.y;
}

void Enemy::startMoving() {
    m_Velocity.y = 0;

    if (m_RightSide) {
        m_Rotation.velocity = -Pi * 0.5f;
        m_Velocity.x = m_MoveSpeed;
    } else {
        m_Rotation.velocity = Pi * 0.5f;
        m_Velocity.x = -m_MoveSpeed;
    }
}

void Enemy::explode() {
    pExplosion->activate(m_Location, 4);
}

bool Enemy::getExplosionActive() {
    return pExplosion->getActive();
}

void Enemy::checkForTurn() {
    if (m_RightSide) {
        if (m_Location.x <= m_WhenToTurn.left) {
            if (m_Location.y >= m_WhenToTurn.bottom) {
                m_Velocity.y = 0;
                m_Velocity.x = m_MoveSpeed;
            }

            if (m_Location.y <= m_WhenToTurn.top) {
                m_Velocity.y = m_MoveSpeed;
                m_Velocity.x = 0;
            }
        }

        if (m_Location.x >= m_WhenToTurn.right) {
            if (m_Location.y >= m_WhenToTurn.bottom) {
                m_Velocity.y = -m_MoveSpeed;
                m_Velocity.x = 0;
            }

            if (m_Location.y <= m_WhenToTurn.top) {
                m_Velocity.y = 0;
                m_Velocity.x = -m_MoveSpeed;
            }
        }
    } else {
        if (m_Location.x <= m_WhenToTurn.left) {
            if (m_Location.y >= m_WhenToTurn.bottom) {
                m_Velocity.y = -m_MoveSpeed;
                m_Velocity.x = 0;
            }

            if (m_Location.y <= m_WhenToTurn.top) {
                m_Velocity.y = 0;
                m_Velocity.x = m_MoveSpeed;
            }
        }

        if (m_Location.x >= m_WhenToTurn.right) {
            if (m_Location.y >= m_WhenToTurn.bottom) {
                m_Velocity.y = 0;
                m_Velocity.x = -m_MoveSpeed;
            }

            if (m_Location.y <= m_WhenToTurn.top) {
                m_Velocity.y = m_MoveSpeed;
                m_Velocity.x = 0;
            }
        }
    }
}

void Enemy::setSpeed(float speed) {
    m_MoveSpeed = speed;
}

void Enemy::clearVaporTrail() {
    if (pShip) {
        pShip->setVapourTrailActive(false); // This will clear the trail
        pShip->setVapourTrailActive(true);  // Reactivate for future use
    }
}

} // namespace omegarace
