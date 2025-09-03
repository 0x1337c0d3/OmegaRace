#include "Ship.h"

namespace omegarace {

Ship::Ship() {
    // Initialize vapour trail with 20 points for ships (slightly shorter than player)
    m_VapourTrail = std::make_unique<VapourTrail>(20);

    // Set pure green trail color for basic enemies
    Color enemyTrailColor;
    enemyTrailColor.red = 0;
    enemyTrailColor.green = 255;
    enemyTrailColor.blue = 0; // Pure green
    enemyTrailColor.alpha = 255;
    m_VapourTrail->setActive(true);
    m_VapourTrail->setTrailColor(enemyTrailColor);
    m_VapourTrail->setMaxThickness(5.0f);
}

Ship::~Ship() {
}

void Ship::initialize() {
    // Pure bright green for basic enemies
    m_ShipColor.red = 0;      // No red at all
    m_ShipColor.green = 255;  // Maximum green
    m_ShipColor.blue = 0;     // No blue - pure green
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

void Ship::update(float rotation, const Vector2f& location, float scale, const Vector2f& velocity) {
    moveRotateLines(rotation, location, scale);

    // **1. Velocity-Based Trail Activation** - Only show trail when moving at speed
    float velocityMagnitude = sqrt(velocity.x * velocity.x + velocity.y * velocity.y);
    float speedThreshold = 8.0f; // Minimum speed to show trail
    bool shouldShowTrail = velocityMagnitude > speedThreshold;
    
    // **3. Longer, More Dynamic Trails** - Trail length based on speed
    int baseTrailLength = 25; // Increased from 20
    int dynamicTrailLength = baseTrailLength + (int)(velocityMagnitude * 1.5f); // Up to ~50 points
    dynamicTrailLength = fmin(dynamicTrailLength, 60); // Cap at 60 points
    
    if (shouldShowTrail) {
        // **2. Movement Direction Trail** - Trail position based on movement direction
        Vector2f trailDirection = Vector2f(-velocity.x, -velocity.y); // Opposite to movement
        float trailDirectionMagnitude = sqrt(trailDirection.x * trailDirection.x + trailDirection.y * trailDirection.y);
        
        Vector2f trailStartPosition;
        if (trailDirectionMagnitude > 0.1f) {
            // Normalize trail direction and offset from ship center
            trailDirection.x /= trailDirectionMagnitude;
            trailDirection.y /= trailDirectionMagnitude;
            
            // Position trail start behind movement direction (not ship rotation)
            float trailOffset = 8.0f * scale; // Distance behind ship
            trailStartPosition.x = location.x + (trailDirection.x * trailOffset);
            trailStartPosition.y = location.y + (trailDirection.y * trailOffset);
        } else {
            // Fallback to rear position when velocity is very low
            trailStartPosition = getRearPosition(rotation, location, scale);
        }
        
        // **4. Trail Intensity Based on Speed** - More visible when moving faster
        float maxSpeed = 50.0f; // Adjust based on your game's max enemy speed
        float speedIntensity = fmin(velocityMagnitude / maxSpeed, 1.0f);
        
        // Adjust trail color alpha based on speed
        Color adjustedTrailColor = m_VapourTrail->getTrailColor();
        adjustedTrailColor.alpha = (int)(255 * speedIntensity * 0.8f); // Max 80% opacity
        m_VapourTrail->setTrailColor(adjustedTrailColor);
        
        // Update trail length dynamically
        m_VapourTrail->setTrailLength(dynamicTrailLength);
        
        // Activate trail and update position
        m_VapourTrail->setActive(true);
        m_VapourTrail->update(trailStartPosition);
    } else {
        // Deactivate trail when not moving fast enough
        m_VapourTrail->setActive(false);
    }
}

void Ship::draw() {
    // Draw vapour trail first (behind ship)
    m_VapourTrail->draw();

    // Draw ship hull with moderate bloom to preserve colors
    for (int line = 0; line < 8; line++) {
        Window::DrawVolumetricLineWithBloom(&NewShipLines[line], m_ShipColor, 2.0f, 0.8f);
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
