#include "DoubleMine.h"

namespace omegarace {

DoubleMine::DoubleMine() { // Worth 500 pts
    pMine = std::make_unique<TriShip>();
    pMine->setMode(TriShipMode::DOUBLE_MINE);
}

DoubleMine::~DoubleMine() {
}

void DoubleMine::initialize() {
    // DoubleMine uses TriShip for enhanced visual effects
}

void DoubleMine::draw() {
    if (m_Active) {
        pMine->update(m_Location, m_Scale, m_Velocity);
        pMine->draw();
    }
}

void DoubleMine::mineDropped(const Vector2f& location) {
    m_Location = location;
    m_Active = true;
    m_Radius = m_Scale * 3; // Increased hit detection radius for easier shooting (was m_Scale * 2)
}

} // namespace omegarace
