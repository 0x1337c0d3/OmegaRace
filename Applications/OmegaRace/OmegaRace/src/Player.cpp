#include "Player.h"

namespace omegarace {

Player::Player() {
    m_Thrust = false;
    m_Fire = false;
    m_TurnRight = false;
    m_TurnLeft = false;
    m_Active = false;
    m_Hit = false;
    m_ExplosionOn = false;
    m_Radius = 16;
    m_Scale = 2.5;
    m_Rectangle.h = 16;
    m_Rectangle.w = 16;

    m_MaxThrust = 325;
    m_ThrustMagnitude = 3.666;
    m_TurnRate = 4.5;

    m_NumberOfShots = 40;
    m_ThrustDrawTimerAmount = 0.100;
    m_ExplosiontTimerAmount = 2.500;

    pTimer = std::make_unique<Timer>();
    pShip = std::make_unique<PlayerShip>();

    m_ThrustChannel = -1;

    // Player Ship Color
    ShipColor.red = 255;
    ShipColor.green = 255;
    ShipColor.blue = 0;
    ShipColor.alpha = 255;

    initializeShot();
    pShip->initializeLines();
}

Player::~Player() {
}

void Player::initialize() {
    audio.LoadSound("PlayerShot");
    audio.LoadSound("Thrust");
    audio.LoadSound("PlayerHit");
    audio.LoadSound("BorderHit");
}

// Public Methods
void Player::draw() {
    if (m_Active && !m_Hit) {
        pShip->draw(ShipColor);

        if (m_Thrust)
            drawThrust();
    } else if (m_ExplosionOn)
        pShip->drawExplosion();

    drawShots();
}

void Player::update(double frame) {
    updateFrame(frame);
    updateShots(frame);

    if (m_Active && !m_Hit) {
        updateEdge();
        updateRotationThrust();
        updateShip();
        pShip->update(m_Rotation.amount, m_Location, m_Scale);
    } else if (m_ExplosionOn) {
        pShip->updateExplosion(frame);

        if (m_ExplosionTimer < pTimer->seconds())
            m_ExplosionOn = false;
    }
}

void Player::fireButtonPressed() {
    if (m_Active && !m_Hit) {
        fireShot();
    }
}

int Player::getNumberOfShots() {
    return m_NumberOfShots;
}

float Player::getShotRadius() {
    return pShots[0]->getRadius();
}

Vector2i Player::getShotLocation(int Shot) {
    return pShots[Shot]->getLocation();
}

bool Player::getShotActive(int Shot) {
    return pShots[Shot]->getActive();
}

bool Player::getShotCircle(const Vector2f& location, float radius, int shot) {
    return pShots[shot]->circlesIntersect(location, radius);
}

bool Player::getHit() {
    return m_Hit;
}

bool Player::getExplosionOn() {
    return m_ExplosionOn;
}

void Player::setShotActive(int Shot, bool Active) {
    pShots[Shot]->setActive(Active);
}

void Player::setThrust(bool Thrust) {
    m_Thrust = Thrust;
}

void Player::setTurnRight(bool TurnRight) {
    m_TurnRight = TurnRight;
}

void Player::setTurnLeft(bool TurnLeft) {
    m_TurnLeft = TurnLeft;
}

void Player::newGame() {
    pTimer->start();
    m_Active = true;
}

void Player::restart() {
    m_Active = true;
}

void Player::hit() {
    if (!m_Hit) {
        // Play Player explosion sound.
        audio.PlaySoundFile("PlayerHit");

        m_ExplosionOn = true;
        m_Hit = true;
        setExplosion();

        // Turn off thrust sound.
        if (m_ThrustChannel != -1) {
            audio.StopChannel(m_ThrustChannel);
            m_ThrustChannel = -1;
        }
    }
}

void Player::spawn(bool rightSide) {
    for (int shot = 0; shot < m_NumberOfShots; shot++) {
        pShots[shot]->setActive(false);
    }

    if (rightSide) {
        m_Rotation.amount = 0;
        m_Location.x = Window::GetWindowSize().x / 11.1;
    } else {
        m_Rotation.amount = Pi;
        m_Location.x = Window::GetWindowSize().x / 1.1;
    }

    m_Location.y = Window::GetWindowSize().y / 3;
    m_Hit = false;
    m_Acceleration = Vector2i();
    m_Velocity = Vector2i();
    pShip->setShieldStrength(0.25);
    pShip->setEngineIntensity(1);
    pShip->setVapourTrailActive(true);
}

void Player::setInsideBorder(const SDL_Rect& border) {
    m_InsideBorder = border;

    for (int shot = 0; shot < m_NumberOfShots; shot++) {
        pShots[shot]->setInsideBorder(m_InsideBorder);
    }
}

bool Player::getInsideLineHit(int line) {
    for (int shot = 0; shot < m_NumberOfShots; shot++) {
        if (pShots[shot]->getInsideLine(line))
            m_InsideLineHit[line] = true;

        pShots[shot]->setInsideLine(line);
    }

    return m_InsideLineHit[line];
}

bool Player::getOutsideLineHit(int line) {
    for (int shot = 0; shot < m_NumberOfShots; shot++) {
        if (pShots[shot]->getOutsideLine(line))
            m_OutsideLineHit[line] = true;
    }

    return m_OutsideLineHit[line];
}

Vector2f Player::getNosePosition() {
    // Get the nose position from the ship
    return pShip->getNosePosition(m_Rotation.amount, m_Location, m_Scale);
}

void Player::fireShot() {
    for (int shot = 0; shot < m_NumberOfShots; shot++) {
        if (!pShots[shot]->getActive()) {
            // If shot found that is not active, then activate that shot.
            audio.PlaySoundFile("PlayerShot");
            // Fire from nose position instead of center
            Vector2f nosePos = getNosePosition();
            pShots[shot]->activate(nosePos, m_Rotation.amount);
            break;
        }
    }
}

void Player::updateShots(double Frame) {
    for (int shot = 0; shot < m_NumberOfShots; shot++) {
        pShots[shot]->update(Frame);
    }
}

void Player::drawShots() {
    for (int shot = 0; shot < m_NumberOfShots; shot++)
        pShots[shot]->draw();
}

void Player::drawThrust() {
    if (pTimer->seconds() > m_ThrustDrawTimer) {
        m_ThrustDrawTimer = pTimer->seconds() + m_ThrustDrawTimerAmount;
    } else {
        pShip->drawThrust();
    }
}

void Player::initializeShot() {
    for (int shot = 0; shot < m_NumberOfShots; shot++) {
        pShots[shot] = std::make_unique<PlayerShot>();
        pShots[shot]->setInsideBorder(m_InsideBorder);
    }
}

void Player::updateEdge() {
    if (checkForXEdge()) {
        audio.PlaySoundFile("BorderHit");
        bounceX();

        if (m_Location.x > Window::GetWindowSize().x / 2) {
            m_Location.x -= 2;

            if (m_Location.y > Window::GetWindowSize().y / 2)
                m_OutsideLineHit[7] = true;
            else
                m_OutsideLineHit[6] = true;
        } else {
            m_Location.x += 2;

            if (m_Location.y > Window::GetWindowSize().y / 2)
                m_OutsideLineHit[5] = true;
            else
                m_OutsideLineHit[4] = true;
        }
    } else {
        for (int line = 4; line < 8; line++)
            m_OutsideLineHit[line] = false;
    }

    if (checkForYEdge()) {
        audio.PlaySoundFile("BorderHit");
        bounceY();

        if (m_Location.y > Window::GetWindowSize().y / 2) {
            m_Location.y -= 2;

            if (m_Location.x > Window::GetWindowSize().x / 2)
                m_OutsideLineHit[3] = true;
            else
                m_OutsideLineHit[2] = true;
        } else {
            m_Location.y += 2;

            if (m_Location.x > Window::GetWindowSize().x / 2)
                m_OutsideLineHit[1] = true;
            else
                m_OutsideLineHit[0] = true;
        }
    } else {
        for (int line = 0; line < 4; line++)
            m_OutsideLineHit[line] = false;
    }

    if (rectangleIntersect(m_InsideBorder)) {
        audio.PlaySoundFile("BorderHit");

        int maxborder = m_InsideBorder.x + m_InsideBorder.w - 1;
        if (valueInRange(m_Location.x, m_InsideBorder.x, maxborder)) {
            bounceY();

            if (m_Location.y > Window::GetWindowSize().y / 2) {
                m_Location.y += 1;
                m_InsideLineHit[2] = true;
            } else {
                m_Location.y -= 1;
                m_InsideLineHit[0] = true;
            }
        }

        maxborder = m_InsideBorder.y + m_InsideBorder.h - 1;
        if (valueInRange(m_Location.y, m_InsideBorder.y, maxborder)) {
            bounceX();

            if (m_Location.x > Window::GetWindowSize().x / 2) {
                m_Location.x += 1;
                m_InsideLineHit[1] = true;
            } else {
                m_Location.x -= 1;
                m_InsideLineHit[3] = true;
            }
        }
    } else {
        for (int line = 0; line < 4; line++) {
            m_InsideLineHit[line] = false;
        }
    }
}

void Player::updateShip() {
    // Get rotation keys
    if (m_TurnLeft || m_TurnRight) {
        if (m_TurnRight) {
            if (m_Rotation.amount < PiTimesTwo)
                m_Rotation.velocity = m_TurnRate;
            else
                m_Rotation.amount -= PiTimesTwo;
        }

        if (m_TurnLeft) {
            if (m_Rotation.amount > 0.0f)
                m_Rotation.velocity = -m_TurnRate;
            else
                m_Rotation.amount += PiTimesTwo;
        }
    } else {
        m_Rotation.velocity = 0;
    }
}

void Player::updateRotationThrust() {
    // Do thrust key
    if (m_Thrust) {
        // Play thrust sound.
        if (m_ThrustChannel == -1) {
            m_ThrustChannel = audio.PlaySoundFile("Thrust");
        }

        if (m_Velocity.x > m_MaxThrust)
            m_Velocity.x = m_MaxThrust;

        if (m_Velocity.x < -m_MaxThrust)
            m_Velocity.x = -m_MaxThrust;

        if (m_Velocity.y > m_MaxThrust)
            m_Velocity.y = m_MaxThrust;

        if (m_Velocity.y < -m_MaxThrust)
            m_Velocity.y = -m_MaxThrust;

        m_Acceleration.x = cos(m_Rotation.amount) * m_ThrustMagnitude;
        m_Acceleration.y = sin(m_Rotation.amount) * m_ThrustMagnitude;
    } else {
        // Turn off thrust sound.
        if (m_ThrustChannel != -1) {
            audio.StopChannel(m_ThrustChannel);
            m_ThrustChannel = -1;
        }

        m_Acceleration = Vector2f(0, 0);
    }
}

void Player::setExplosion() {
    Vector2i location = m_Location;
    pShip->setExplosion(location);
    m_ExplosionTimer = pTimer->seconds() + m_ExplosiontTimerAmount + Window::Random(0, (int)m_ExplosiontTimerAmount);
}

void Player::timerPause() {
    for (int shot = 0; shot < m_NumberOfShots; shot++) {
        pShots[shot]->timerPause();
    }
}

void Player::timerUnpause() {
    for (int shot = 0; shot < m_NumberOfShots; shot++) {
        pShots[shot]->timerUnpause();
    }
}

void Player::clearVaporTrail() {
    if (pShip) {
        pShip->clearVaporTrail();
    }
}

} // namespace omegarace
