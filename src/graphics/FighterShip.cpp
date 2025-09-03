#include "FighterShip.h"

namespace omegarace {

FighterShip::FighterShip() {
    m_Scale = 4.5;
    initilize();
}

FighterShip::~FighterShip() {
}

void FighterShip::initilize() {
    // Pure magenta for Fighter enemies
    m_ShipColor.red = 255;
    m_ShipColor.green = 0;
    m_ShipColor.blue = 255;
    m_ShipColor.alpha = 255;

    shipLines[0].start = Vector2i(0, -2);
    shipLines[1].start = Vector2i(-2, 0);

    shipLines[0].end = Vector2i(0, 2);
    shipLines[1].end = Vector2i(2, 0);
}

void FighterShip::update(const Vector2f& location, float rotation) {
    moveRotateLines(rotation, location);
}

void FighterShip::draw() {
    // Use volumetric bloom to showcase the bright magenta colors
    for (int line = 0; line < 2; line++) {
        Window::DrawVolumetricLineWithBloom(&newShipLines[line], m_ShipColor, 2.5f, 1.8f);
    }
}

void FighterShip::moveRotateLines(float rotation, const Vector2f& location) {
    float sinRot = sin(rotation);
    float cosRot = cos(rotation);

    for (int line = 0; line < 2; line++) {
        newShipLines[line].start.x =
            shipLines[line].start.x * m_Scale * cosRot - shipLines[line].start.y * m_Scale * sinRot;
        newShipLines[line].start.y =
            shipLines[line].start.x * m_Scale * sinRot + shipLines[line].start.y * m_Scale * cosRot;

        newShipLines[line].end.x = shipLines[line].end.x * m_Scale * cosRot - shipLines[line].end.y * m_Scale * sinRot;
        newShipLines[line].end.y = shipLines[line].end.x * m_Scale * sinRot + shipLines[line].end.y * m_Scale * cosRot;

        newShipLines[line].start += location;
        newShipLines[line].end += location;
    }
}

} // namespace omegarace
