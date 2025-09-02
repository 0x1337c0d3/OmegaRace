#include "Ship.h"

namespace omegarace {

Ship::Ship() {
    // Initialize vapour trail with 20 points for ships (slightly shorter than player)
    m_VapourTrail = std::make_unique<VapourTrail>(20);

    // Set a brighter, more visible color for enemy ships
    Color enemyTrailColor;
    enemyTrailColor.red = 250;
    enemyTrailColor.green = 180;
    enemyTrailColor.blue = 100; // Orange-ish trail for enemies
    enemyTrailColor.alpha = 255;
    m_VapourTrail->setActive(true);
    m_VapourTrail->setTrailColor(enemyTrailColor);
    m_VapourTrail->setMaxThickness(5.0f);
}

Ship::~Ship() {
}

void Ship::initialize() {
    m_ShipColor.red = 255;
    m_ShipColor.green = 255;
    m_ShipColor.blue = 255;
    m_ShipColor.alpha = 255;

    ShipLines[0].start = Vector2i(-3, -3);
    ShipLines[1].start = Vector2i(3, -3);
    ShipLines[2].start = Vector2i(3, 3);
    ShipLines[3].start = Vector2i(-3, 3);
    ShipLines[4].start = Vector2i(-4, 0);
    ShipLines[5].start = Vector2i(0, -4);
    ShipLines[6].start = Vector2i(4, 0);
    ShipLines[7].start = Vector2i(0, 4);

    ShipLines[0].end = ShipLines[1].start;
    ShipLines[1].end = ShipLines[2].start;
    ShipLines[2].end = ShipLines[3].start;
    ShipLines[3].end = ShipLines[0].start;
    ShipLines[4].end = ShipLines[5].start;
    ShipLines[5].end = ShipLines[6].start;
    ShipLines[6].end = ShipLines[7].start;
    ShipLines[7].end = ShipLines[4].start;
}

void Ship::update(float rotation, const Vector2f& location, float scale) {
    moveRotateLines(rotation, location, scale);

    // Calculate rear position for vapour trail (behind the ship)
    Vector2f rearPosition = getRearPosition(rotation, location, scale);
    m_VapourTrail->update(rearPosition);
}

void Ship::draw() {
    // Draw vapour trail first (behind ship)
    m_VapourTrail->draw();

    // Draw ship hull
    for (int line = 0; line < 8; line++) {
        Window::DrawVolumetricLineWithBloom(&NewShipLines[line], m_ShipColor, 1.0f, 1.0f);
    }
}

void Ship::moveRotateLines(float rotation, const Vector2f& location, float scale) {
    float sinRot = sin(rotation);
    float cosRot = cos(rotation);

    for (int line = 0; line < 8; line++) {
        NewShipLines[line].start.x =
            ShipLines[line].start.x * scale * cosRot - ShipLines[line].start.y * scale * sinRot;
        NewShipLines[line].start.y =
            ShipLines[line].start.x * scale * sinRot + ShipLines[line].start.y * scale * cosRot;

        NewShipLines[line].end.x = ShipLines[line].end.x * scale * cosRot - ShipLines[line].end.y * scale * sinRot;
        NewShipLines[line].end.y = ShipLines[line].end.x * scale * sinRot + ShipLines[line].end.y * scale * cosRot;

        NewShipLines[line].start += location;
        NewShipLines[line].end += location;
    }
}

void Ship::setVapourTrailActive(bool active) {
    m_VapourTrail->setActive(active);
}

void Ship::setVapourTrailColor(const Color& color) {
    m_VapourTrail->setTrailColor(color);
}

Vector2f Ship::getRearPosition(float rotation, const Vector2f& centerLocation, float scale) {
    // For enemy ships, the rear is approximately at (-4, 0) in ship coordinates
    // This is based on the ship design where line 4 goes from (-4, 0) to (0, -4)
    float rearOffsetX = -4.0f * scale;
    float rearOffsetY = 0.0f * scale;

    // Apply rotation
    float sinRot = sin(rotation);
    float cosRot = cos(rotation);

    Vector2f rearPosition;
    rearPosition.x = centerLocation.x + (rearOffsetX * cosRot - rearOffsetY * sinRot);
    rearPosition.y = centerLocation.y + (rearOffsetX * sinRot + rearOffsetY * cosRot);

    return rearPosition;
}

} // namespace omegarace
