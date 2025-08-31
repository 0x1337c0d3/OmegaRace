#include "PlayerShip.h"

namespace omegarace {

PlayerShip::PlayerShip() {
    m_ThrustColor.red = 255;
    m_ThrustColor.green = 127;
    m_ThrustColor.blue = 0;
    m_ThrustColor.alpha = 255;
    
    // Enhanced ship visual properties
    m_ShipIntensity = 1.0f;
    m_EngineGlowIntensity = 0.0f;
    m_ShieldStrength = 0.0f;
    m_DamageLevel = 0.0f;
    
    // Initialize vapour trail with 25 points for player ship
    m_VapourTrail = std::make_unique<VapourTrail>(25);
    
    // Set player ship trail color (blue-white)
    Color playerTrailColor;
    playerTrailColor.red = 150;
    playerTrailColor.green = 200;
    playerTrailColor.blue = 255;
    playerTrailColor.alpha = 255;
    m_VapourTrail->setTrailColor(playerTrailColor);
    
    initialize();
}

PlayerShip::~PlayerShip() {
}

void PlayerShip::initialize() {
    // IMPROVED SHIP DESIGN - More distinctive front-facing appearance
    
    // Main hull outline (triangular body with extended nose)
    playerLines[0].start = Vector2i(-5, 3);     // Left rear hull
    playerLines[0].end = Vector2i(1, 2);        // to front left
    
    playerLines[1].start = Vector2i(-5, -3);    // Right rear hull  
    playerLines[1].end = Vector2i(1, -2);       // to front right
    
    playerLines[2].start = Vector2i(-5, 3);     // Left rear
    playerLines[2].end = Vector2i(-5, -3);      // to right rear (back edge)
    
    // Extended nose cone (makes direction very clear)
    playerLines[3].start = Vector2i(1, 2);      // Front left
    playerLines[3].end = Vector2i(5, 0);        // to nose tip
    
    playerLines[4].start = Vector2i(1, -2);     // Front right
    playerLines[4].end = Vector2i(5, 0);        // to nose tip
    
    // Cockpit canopy (distinctive front section)
    playerLines[5].start = Vector2i(-2, 1);     // Cockpit left
    playerLines[5].end = Vector2i(2, 0);        // to cockpit front
    
    playerLines[6].start = Vector2i(-2, -1);    // Cockpit right
    playerLines[6].end = Vector2i(2, 0);        // to cockpit front
    
    // Wing details (for better ship recognition)
    playerLines[7].start = Vector2i(-3, 3);     // Left wing tip
    playerLines[7].end = Vector2i(-1, 4);       // to wing extension
    
    playerLines[8].start = Vector2i(-3, -3);    // Right wing tip
    playerLines[8].end = Vector2i(-1, -4);      // to wing extension
    
    // Wing connections
    playerLines[9].start = Vector2i(-1, 4);     // Left wing extension
    playerLines[9].end = Vector2i(0, 3);        // to front wing
    
    playerLines[10].start = Vector2i(-1, -4);   // Right wing extension
    playerLines[10].end = Vector2i(0, -3);      // to front wing
    
    // Forward directional indicator (arrow-like lines on nose)
    playerLines[11].start = Vector2i(3, 0);     // Nose direction indicator
    playerLines[11].end = Vector2i(4, 0);       // extends forward

    // Enhanced Thrust Lines - positioned at rear engines
    thrustLines[0].start = Vector2i(-7, 1);     // Left engine exhaust
    thrustLines[0].end = Vector2i(-5, 2);       // to hull

    thrustLines[1].start = Vector2i(-7, -1);    // Right engine exhaust
    thrustLines[1].end = Vector2i(-5, -2);      // to hull
}

void PlayerShip::update(float rotation, const Vector2f& location, float scale) {
    moveRotateLines(rotation, location, scale);
    
    // Update visual effects based on ship state
    updateVisualEffects();
    
    // Update vapour trail from engine position (rear of ship)
    Vector2f enginePos = getEnginePosition(rotation, location, scale);
    m_VapourTrail->update(enginePos);
}

void PlayerShip::updateVisualEffects() {
    // Create subtle pulsing effect for ship intensity
    static float pulseTimer = 0.0f;
    pulseTimer += 0.05f; // Adjust speed as needed
    
    // Gentle pulsing between 0.8 and 1.2 intensity
    m_ShipIntensity = 1.0f + sin(pulseTimer) * 0.1f;
    
    // Engine glow effect (can be controlled by thrust state)
    m_EngineGlowIntensity = 0.7f + sin(pulseTimer * 2.0f) * 0.3f;
}

void PlayerShip::draw(const Color& color) {
    // Draw vapour trail first (behind ship)
    m_VapourTrail->draw();
    
    // Calculate enhanced ship colors
    Color enhancedColor = calculateEnhancedShipColor(color);
    Color coreColor = enhancedColor;
    Color glowColor = enhancedColor;
    glowColor.alpha = glowColor.alpha / 2;
    
    // Draw main hull (lines 0-2) - body structure
    for (int line = 0; line < 3; line++) {
        Window::DrawVolumetricLineWithBloom(&newPlayerLines[line], enhancedColor, 2.0f * m_ShipIntensity, 0.8f);
    }
    
    // Draw nose cone (lines 3-4) - BRIGHT to show direction clearly
    Color noseColor;
    noseColor.red = fmin(255, coreColor.red + 80);
    noseColor.green = fmin(255, coreColor.green + 80);
    noseColor.blue = fmin(255, coreColor.blue + 80);
    noseColor.alpha = coreColor.alpha;
    
    for (int line = 3; line < 5; line++) {
        Window::DrawVolumetricLineWithBloom(&newPlayerLines[line], noseColor, 2.5f, 1.2f);
    }
    
    // Draw cockpit canopy (lines 5-6) - Distinctive bright cockpit
    Color cockpitColor;
    cockpitColor.red = fmin(255, coreColor.red + 100);
    cockpitColor.green = fmin(255, coreColor.green + 150);
    cockpitColor.blue = fmin(255, coreColor.blue + 100);
    cockpitColor.alpha = coreColor.alpha;
    
    for (int line = 5; line < 7; line++) {
        Window::DrawVolumetricLineWithBloom(&newPlayerLines[line], cockpitColor, 1.8f, 1.0f);
    }
    
    // Draw wings (lines 7-10) - Wing structure
    for (int line = 7; line < 11; line++) {
        Window::DrawVolumetricLineWithBloom(&newPlayerLines[line], enhancedColor, 1.5f * m_ShipIntensity, 0.6f);
    }
    
    // Draw directional indicator (line 11) - VERY BRIGHT forward arrow
    Color directionColor;
    directionColor.red = 255;
    directionColor.green = 255;
    directionColor.blue = 255;
    directionColor.alpha = 255;
    
    Window::DrawVolumetricLineWithBloom(&newPlayerLines[11], directionColor, 3.0f, 1.5f);
    
    // Engine glow effects on rear sections (hull rear)
    if (m_EngineGlowIntensity > 0.3f) {
        Color engineGlow;
        engineGlow.red = 100;
        engineGlow.green = 150; 
        engineGlow.blue = 255;
        engineGlow.alpha = (int)(150 * m_EngineGlowIntensity);
        
        // Highlight the rear hull line (line 2) with engine glow
        Window::DrawVolumetricLineWithBloom(&newPlayerLines[2], engineGlow, 3.0f, 0.8f);
    }
    
    // Add shield effect if shields are active
    if (m_ShieldStrength > 0.1f) {
        drawShieldEffect();
    }
}

void PlayerShip::drawThrust() {
    // Enhanced thrust with multiple flame layers
    Color coreFlame = m_ThrustColor;
    coreFlame.alpha = (int)(255 * m_EngineGlowIntensity);
    
    Color outerFlame;
    outerFlame.red = 255;
    outerFlame.green = 50;
    outerFlame.blue = 0;
    outerFlame.alpha = (int)(180 * m_EngineGlowIntensity);
    
    Color hotCore;
    hotCore.red = 255;
    hotCore.green = 255;
    hotCore.blue = 200;
    hotCore.alpha = (int)(200 * m_EngineGlowIntensity);
    
    // Draw thrust with OPTIMIZED single layer per line for performance
    for (int line = 0; line < 2; line++) {
        // Single flame effect with moderate bloom
        Window::DrawVolumetricLineWithBloom(&newThrustLines[line], coreFlame, 6.0f * m_EngineGlowIntensity, 1.5f);
    }
    
    // Add engine exhaust particles
    if (m_EngineGlowIntensity > 0.5f) {
        drawEngineParticles();
    }
}

void PlayerShip::updateExplosion(double frame) {
    for (int line = 0; line < 12; line++) {
        pExplosionLines[line]->update(frame);
    }
}

void PlayerShip::drawExplosion() {
    for (int line = 0; line < 12; line++) {
        pExplosionLines[line]->draw();
    }
}

void PlayerShip::setExplosion(const Vector2i& location) {
    for (int line = 0; line < 12; line++) {
        pExplosionLines[line]->initialize(newPlayerLines[line], location);
    }
}

void PlayerShip::initializeLines() {
    for (int line = 0; line < 12; line++) {
        pExplosionLines[line] = std::make_unique<PlayerExplosionLine>();
    }
}

void PlayerShip::moveRotateLines(float rotation, const Vector2f& location, float scale) {
    float sinRot = sin(rotation);
    float cosRot = cos(rotation);

    for (int line = 0; line < 12; line++) {
        newPlayerLines[line].start.x =
            playerLines[line].start.x * scale * cosRot - playerLines[line].start.y * scale * sinRot;
        newPlayerLines[line].start.y =
            playerLines[line].start.x * scale * sinRot + playerLines[line].start.y * scale * cosRot;

        newPlayerLines[line].end.x =
            playerLines[line].end.x * scale * cosRot - playerLines[line].end.y * scale * sinRot;
        newPlayerLines[line].end.y =
            playerLines[line].end.x * scale * sinRot + playerLines[line].end.y * scale * cosRot;

        newPlayerLines[line].start += location;
        newPlayerLines[line].end += location;
    }

    for (int line = 0; line < 2; line++) {
        newThrustLines[line].start.x =
            thrustLines[line].start.x * scale * cosRot - thrustLines[line].start.y * scale * sinRot;
        newThrustLines[line].start.y =
            thrustLines[line].start.x * scale * sinRot + thrustLines[line].start.y * scale * cosRot;

        newThrustLines[line].end.x =
            thrustLines[line].end.x * scale * cosRot - thrustLines[line].end.y * scale * sinRot;
        newThrustLines[line].end.y =
            thrustLines[line].end.x * scale * sinRot + thrustLines[line].end.y * scale * cosRot;

        newThrustLines[line].start += location;
        newThrustLines[line].end += location;
    }
}

Color PlayerShip::calculateEnhancedShipColor(const Color& baseColor) {
    Color enhanced = baseColor;
    
    // Apply intensity multiplier
    enhanced.red = fmin(255, (int)(enhanced.red * m_ShipIntensity));
    enhanced.green = fmin(255, (int)(enhanced.green * m_ShipIntensity));
    enhanced.blue = fmin(255, (int)(enhanced.blue * m_ShipIntensity));
    
    // Apply damage effects (redder as damage increases)
    if (m_DamageLevel > 0.1f) {
        enhanced.red = fmin(255, enhanced.red + (int)(m_DamageLevel * 100));
        enhanced.green = fmax(50, enhanced.green - (int)(m_DamageLevel * 50));
        enhanced.blue = fmax(50, enhanced.blue - (int)(m_DamageLevel * 50));
    }
    
    return enhanced;
}

void PlayerShip::drawShieldEffect() {
    // Create a subtle shield bubble around the ship
    Color shieldColor;
    shieldColor.red = 100;
    shieldColor.green = 200;
    shieldColor.blue = 255;
    shieldColor.alpha = (int)(80 * m_ShieldStrength);
    
    // Draw shield as expanded version of ship outline
    for (int line = 0; line < 12; line++) {
        // Create expanded shield lines
        Vector2i center;
        center.x = (newPlayerLines[line].start.x + newPlayerLines[line].end.x) / 2;
        center.y = (newPlayerLines[line].start.y + newPlayerLines[line].end.y) / 2;
        
        Line shieldLine;
        Vector2i startOffset = newPlayerLines[line].start - center;
        Vector2i endOffset = newPlayerLines[line].end - center;
        
        // Expand by 20%
        shieldLine.start = center + Vector2i((int)(startOffset.x * 1.2f), (int)(startOffset.y * 1.2f));
        shieldLine.end = center + Vector2i((int)(endOffset.x * 1.2f), (int)(endOffset.y * 1.2f));
        
        Window::DrawVolumetricLineWithBloom(&shieldLine, shieldColor, 1.5f, 0.8f);
    }
}

void PlayerShip::drawEngineParticles() {
    // Create small particle effects streaming from the engines
    Vector2i engineCenter;
    engineCenter.x = (newThrustLines[0].start.x + newThrustLines[1].start.x) / 2;
    engineCenter.y = (newThrustLines[0].start.y + newThrustLines[1].start.y) / 2;
    
    // Create 3-5 small particle trails
    for (int i = 0; i < 4; i++) {
        // Random particle direction (mostly backward)
        float particleAngle = Window::Random(-30, 30) * 0.01f; // ±0.3 radians
        float particleLength = Window::Random(3, 8);
        
        Vector2i particleEnd;
        particleEnd.x = engineCenter.x + (int)(cos(particleAngle) * particleLength);
        particleEnd.y = engineCenter.y + (int)(sin(particleAngle) * particleLength);
        
        Line particleLine;
        particleLine.start = engineCenter;
        particleLine.end = particleEnd;
        
        Color particleColor;
        particleColor.red = 255;
        particleColor.green = 100;
        particleColor.blue = 50;
        particleColor.alpha = (int)(120 * m_EngineGlowIntensity);
        
        Window::DrawVolumetricLineWithBloom(&particleLine, particleColor, 0.8f, 0.5f);
    }
}

void PlayerShip::setShieldStrength(float strength) {
    m_ShieldStrength = fmax(0.0f, fmin(1.0f, strength));
}

void PlayerShip::setDamageLevel(float damage) {
    m_DamageLevel = fmax(0.0f, fmin(1.0f, damage));
}

void PlayerShip::setEngineIntensity(float intensity) {
    m_EngineGlowIntensity = fmax(0.0f, fmin(1.0f, intensity));
}

void PlayerShip::setVapourTrailActive(bool active) {
    if (m_VapourTrail) {
        m_VapourTrail->setActive(active);
    }
}

void PlayerShip::clearVaporTrail() {
    if (m_VapourTrail) {
        m_VapourTrail->clearTrail();
    }
}

Vector2f PlayerShip::getNosePosition(float rotation, const Vector2f& centerLocation, float scale) {
    // Nose tip is at (5, 0) in ship coordinates
    float noseOffsetX = 5.0f * scale;
    float noseOffsetY = 0.0f * scale;
    
    // Apply rotation
    float sinRot = sin(rotation);
    float cosRot = cos(rotation);
    
    Vector2f nosePosition;
    nosePosition.x = centerLocation.x + (noseOffsetX * cosRot - noseOffsetY * sinRot);
    nosePosition.y = centerLocation.y + (noseOffsetX * sinRot + noseOffsetY * cosRot);
    
    return nosePosition;
}

Vector2f PlayerShip::getEnginePosition(float rotation, const Vector2f& centerLocation, float scale) {
    // Engine center is at the rear (-6, 0) in ship coordinates
    float engineOffsetX = -6.0f * scale;
    float engineOffsetY = 0.0f * scale;
    
    // Apply rotation
    float sinRot = sin(rotation);
    float cosRot = cos(rotation);
    
    Vector2f enginePosition;
    enginePosition.x = centerLocation.x + (engineOffsetX * cosRot - engineOffsetY * sinRot);
    enginePosition.y = centerLocation.y + (engineOffsetX * sinRot + engineOffsetY * cosRot);
    
    return enginePosition;
}

} // namespace omegarace
