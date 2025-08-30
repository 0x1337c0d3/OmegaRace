#include "Mine.h"

namespace omegarace {

Mine::Mine() {
    pMine = std::make_unique<TriShip>();
    pMine->setMode(TriShipMode::MINE);  // Set to dangerous mine appearance
    m_Radius = 6;  // Increased hit detection radius for easier shooting
    m_Scale = 4;
}

Mine::~Mine() {
}

void Mine::draw() {
    if (m_Active) {
        pMine->update(m_Location, m_Scale);
        pMine->draw();
    }
}

void Mine::mineDropped() {
    pMine->update(m_Location, m_Scale);
    m_Active = true;
}

} // namespace omegarace
