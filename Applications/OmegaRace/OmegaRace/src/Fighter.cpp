#include "Fighter.h"

namespace omegarace {

void Fighter::update(double Frame) {
    if (m_Active) {
        m_Frame = Frame;
        updateFrame(Frame);
        pShip->update(m_Location, m_Scale);
        pBlade->update(m_Location, m_Rotation.amount);
        timerCheck();
        checkBorders();
    }

    pShot->update(Frame);
    pExplosion->update(Frame);
}

void Fighter::draw() {
    if (m_Active) {
        pShip->draw();
        pBlade->draw();
    }

    pShot->draw();
    pExplosion->draw();

    for (int mine = 0; mine < m_Mines.size(); mine++)
        m_Mines[mine]->draw();
}

void Fighter::setPlayerLocation(const Vector2f& location) {
    m_PlayerLocation = location;
}

void Fighter::setInsideBorder(const SDL_Rect& border) {
    m_InsideBorder = border;
    pShot->setInsideBorder(border);
}

bool Fighter::getInsideLineHit(int line) {
    return m_InsideLineHit[line];
}

bool Fighter::getOutsideLineHit(int line) {
    return m_OutsideLineHit[line];
}

Vector2f Fighter::getMineLocaiton(int mine) {
    return m_Mines[mine]->getLocation();
}

float Fighter::getMineRadius() {
    return m_Mines[0]->getRadius();
}

int Fighter::getMineCount() {
    return (int)m_Mines.size();
}

bool Fighter::getMineActive(int mine) {
    return m_Mines[mine]->getActive();
}

void Fighter::mineHit(int mine) {
    m_Mines[mine]->setActive(false);
}

Vector2f Fighter::getShotLocation() {
    return pShot->getLocation();
}

float Fighter::getShotRadius() {
    return pShot->getRadius();
}

bool Fighter::getShotActive() {
    return pShot->getActive();
}

void Fighter::shotHitTarget() {
    pShot->setActive(false);
}

void Fighter::start(const Vector2f& location, const Vector2f& velocity) {
    m_Location = location;
    m_Velocity = velocity;
    m_Active = true;
    m_Rotation.velocity = 9.15;
    m_Speed = 105;

    for (int line = 0; line < 4; line++)
        m_InsideLineHit[line] = false;

    for (int line = 0; line < 8; line++)
        m_OutsideLineHit[line] = false;

    resetMineTimer();
    resetShotTimer();
    resetChangeVectorTimer();
    update(m_Frame);
}

void Fighter::newGame() {
    m_Mines.clear();
    m_Active = false;
}

void Fighter::timerPause() {
    pTimer->pause();
}

void Fighter::timerUnpause() {
    pTimer->unpause();
}

void Fighter::explode() {
    m_Active = false;
    pExplosion->activate(m_Location, 2);
}

bool Fighter::getExplosionActive() {
    return pExplosion->getActive();
}

Fighter::Fighter() {
    pShip = std::make_unique<TriShip>();
    pShot = std::make_unique<Shot>();
    pBlade = std::make_unique<FighterShip>();
    pExplosion = std::make_unique<Explosion>();
    pTimer = std::make_unique<Timer>();
    pTimer->start();
    m_ShotTimerAmount = 3.2;
    m_MineTimerAmount = 5.1;
    m_ChangeVectorTimerAmount = 2.4;
    m_Scale = 3.5;
    m_Radius = 5;
    m_Rectangle.h = 10;
    m_Rectangle.w = 10;
}

Fighter::~Fighter() {
}

void Fighter::resetMineTimer() {
    m_MineTimer = m_MineTimerAmount + Window::Random(0, m_MineTimerAmount) + pTimer->seconds();
}

void Fighter::resetShotTimer() {
    m_ShotTimer = m_ShotTimerAmount + Window::Random(0, m_ShotTimerAmount) + pTimer->seconds();
}

void Fighter::resetChangeVectorTimer() {
    m_ChangeVectorTimer = m_ChangeVectorTimerAmount + Window::Random(0, m_ChangeVectorTimerAmount) + pTimer->seconds();
}

void Fighter::timerCheck() {
    if (m_MineTimer < pTimer->seconds()) {
        layMine();
        resetMineTimer();
    }

    if (m_ShotTimer < pTimer->seconds()) {
        shotPlayer();
        resetShotTimer();
    }

    if (m_ChangeVectorTimer < pTimer->seconds()) {
        changeVector();
        resetChangeVectorTimer();
    }
}

void Fighter::layMine() {
    float scale = 3.5;
    m_Mines.push_back(std::make_unique<DoubleMine>());
    int mine = (int)m_Mines.size() - 1;
    m_Mines[mine]->initialize();
    m_Mines[mine]->setScale(scale);
    m_Mines[mine]->mineDropped(m_Location);
}

void Fighter::shotPlayer() {
    float angle = atan2(m_PlayerLocation.y - m_Location.y, m_PlayerLocation.x - m_Location.x);
    pShot->activate(m_Location, angle);
    resetShotTimer();
}

void Fighter::changeVector() {
    float angle;

    if (Window::Random(0, 2)) {
        angle = Window::Random(0, (Pi * 2) * 10) * 0.1;
    } else {
        angle = atan2(m_PlayerLocation.y - m_Location.y, m_PlayerLocation.x - m_Location.x);
    }

    m_Velocity.x = cos(angle) * m_Speed;
    m_Velocity.y = sin(angle) * m_Speed;

    m_Speed += 10;
}

void Fighter::checkBorders() {
    if (checkForXEdge()) {
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

} // namespace omegarace
