#include "PlayerShot.h"
#include "Window.h"

namespace omegarace {

PlayerShot::PlayerShot() : Shot() {
    m_ShotTimeLimit = 2.1;
    m_Speed = 350;

    m_InsideColor.red = 255;
    m_InsideColor.green = 255;
    m_InsideColor.blue = 0;
    m_InsideColor.alpha = 255;

    m_OutsideColor.red = 255;
    m_OutsideColor.green = 0;
    m_OutsideColor.blue = 0;
    m_OutsideColor.alpha = 255;
}

PlayerShot::~PlayerShot() {
}

} // namespace omegarace
