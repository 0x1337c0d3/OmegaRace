#include "TriShip.h"
#include <cmath>

namespace omegarace {

void TriShip::update(const Vector2f& location, float scale) {
    // Store current location and scale for vapour trail positioning
    m_CurrentLocation = location;
    m_CurrentScale = scale;
    
    moveScale(location, scale);
    
    // Update vapour trail from rear position
    m_VapourTrail.update(getRearPosition());
    
    // Update menacing animation effects
    m_AnimationTime += 0.1f;
    
    // Create aggressive pulsing effect
    if (m_AggressiveMode) {
        m_PulseIntensity = 0.7f + sin(m_AnimationTime * 3.0f) * 0.3f; // Fast, aggressive pulsing
    } else {
        m_PulseIntensity = 0.5f + sin(m_AnimationTime * 1.5f) * 0.2f; // Slower, ominous pulsing
    }
    
    updateMenacingColors();
}

void TriShip::draw() {
    // Draw vapour trail first (behind the ship)
    m_VapourTrail.draw();
    
    switch (m_Mode) {
        case TriShipMode::ENEMY: {
            // Draw multiple layers for menacing depth effect
            
            // Outer threat aura (largest, dimmest)
            Color auraColor = m_ThreatColor;
            auraColor.alpha = (int)(auraColor.alpha * 0.3f * m_PulseIntensity);
            for (int line = 0; line < 3; line++) {
                Window::DrawVolumetricLineWithBloom(&newTriangle[line], auraColor, 4.0f * m_ThreatLevel, 0.8f);
            }
            
            // Middle menacing glow
            Color glowColor = m_ThreatColor;
            glowColor.alpha = (int)(glowColor.alpha * 0.7f * m_PulseIntensity);
            for (int line = 0; line < 3; line++) {
                Window::DrawVolumetricLineWithBloom(&newTriangle[line], glowColor, 2.5f * m_ThreatLevel, 0.6f);
            }
            
            // Core ship structure (brightest)
            for (int line = 0; line < 3; line++) {
                Window::DrawVolumetricLineWithBloom(&newTriangle[line], m_CoreColor, 1.5f, 0.4f);
            }
            
            // Add menacing special effects
            if (m_ThreatLevel > 0.5f) {
                drawMenacingEffects();
            }
            break;
        }
            
        case TriShipMode::MINE: {
            drawMineEffects();
            break;
        }
            
        case TriShipMode::DOUBLE_MINE: {
            drawDoubleMineEffects();
            break;
        }
    }
}

void TriShip::setMode(TriShipMode mode) {
    m_Mode = mode;
    
    if (mode == TriShipMode::MINE || mode == TriShipMode::DOUBLE_MINE) {
        // Mines have constant high threat level and aggressive pulsing
        m_ThreatLevel = 1.0f;
        m_AggressiveMode = true;
        updateMineColors();
    }
}

void TriShip::setThreatLevel(float level) {
    m_ThreatLevel = fmax(0.0f, fmin(1.0f, level)); // Clamp to 0-1
}

void TriShip::setAggressiveMode(bool aggressive) {
    m_AggressiveMode = aggressive;
}

TriShip::TriShip() 
    : m_ThreatLevel(0.0f),
      m_PulseIntensity(0.0f),
      m_AnimationTime(0.0f),
      m_AggressiveMode(false),
      m_Mode(TriShipMode::ENEMY),
      m_MineWarningIntensity(0.0f),
      m_VapourTrail(25), // Standard trail length
      m_CurrentLocation(0.0f, 0.0f),
      m_CurrentScale(1.0f) {
    initilize();
    
    // Set orange trail color for enemy ships
    Color orangeTrail;
    orangeTrail.red = 255;
    orangeTrail.green = 153;
    orangeTrail.blue = 51;
    orangeTrail.alpha = 204; // 80% opacity
    m_VapourTrail.setTrailColor(orangeTrail);
}

TriShip::~TriShip() {
}

void TriShip::moveScale(const Vector2f& location, float scale) {
    for (int line = 0; line < 3; line++) {
        newTriangle[line].start = triangle[line].start * scale;
        newTriangle[line].end = triangle[line].end * scale;

        newTriangle[line].start += location;
        newTriangle[line].end += location;
    }
}

Vector2f TriShip::getRearPosition() const {
    // Calculate the rear point of the triangle for vapour trail positioning
    // The triangle points upward, so the rear is the bottom center
    Vector2f rearOffset(0.0f, -3.0f); // Offset behind the ship
    
    // Apply current scaling and location
    rearOffset = rearOffset * m_CurrentScale;
    return m_CurrentLocation + rearOffset;
}

void TriShip::initilize() {
    // Initialize menacing threat parameters
    m_ThreatLevel = 1.0f;        // Maximum menace by default
    m_PulseIntensity = 1.0f;
    m_AnimationTime = 0.0f;
    m_AggressiveMode = false;
    m_Mode = TriShipMode::ENEMY;  // Default to enemy mode
    m_MineWarningIntensity = 1.0f;
    
    // Set up threatening color scheme
    updateMenacingColors();
    updateMineColors();

    // Create angular, aggressive triangle shape
    triangle[0].start = Vector2i(-3, 2);   // Wider, more imposing
    triangle[1].start = Vector2i(0, -3);   // Sharper point
    triangle[2].start = Vector2i(3, 2);    // Wider, more imposing

    triangle[0].end = triangle[1].start;
    triangle[1].end = triangle[2].start;
    triangle[2].end = triangle[0].start;
}

void TriShip::updateMenacingColors() {
    // Base threat color - aggressive red with hints of orange
    m_ThreatColor.red = 255;
    m_ThreatColor.green = (int)(50 + m_ThreatLevel * 100); // 50-150 for orange tint
    m_ThreatColor.blue = 0;
    m_ThreatColor.alpha = (int)(200 * m_PulseIntensity);
    
    // Core color - bright orange-red for menacing feel
    m_CoreColor.red = 255;
    m_CoreColor.green = (int)(150 + m_ThreatLevel * 50); // Bright orange core
    m_CoreColor.blue = 50;
    m_CoreColor.alpha = (int)(255 * m_PulseIntensity);
    
    // In aggressive mode, add more red intensity
    if (m_AggressiveMode) {
        m_ThreatColor.green = (int)(m_ThreatColor.green * 0.5f); // More red, less orange
        m_CoreColor.red = 255;
        m_CoreColor.green = (int)(m_CoreColor.green * 0.7f);
    }
}

void TriShip::drawMenacingEffects() {
    // Add menacing energy spikes extending from triangle points
    for (int point = 0; point < 3; point++) {
        Vector2i spikeStart = newTriangle[point].start;
        
        // Calculate spike direction (outward from center)
        Vector2i center = Vector2i(
            (newTriangle[0].start.x + newTriangle[1].start.x + newTriangle[2].start.x) / 3,
            (newTriangle[0].start.y + newTriangle[1].start.y + newTriangle[2].start.y) / 3
        );
        
        Vector2i direction = spikeStart - center;
        float length = sqrt((float)(direction.x * direction.x + direction.y * direction.y));
        if (length > 0) {
            // Normalize and extend
            direction.x = (int)(direction.x / length * 8.0f * m_ThreatLevel * m_PulseIntensity);
            direction.y = (int)(direction.y / length * 8.0f * m_ThreatLevel * m_PulseIntensity);
            
            Line spike;
            spike.start = spikeStart;
            spike.end = spikeStart + direction;
            
            // Draw menacing energy spike
            Color spikeColor = m_ThreatColor;
            spikeColor.alpha = (int)(spikeColor.alpha * 0.8f);
            Window::DrawVolumetricLineWithBloom(&spike, spikeColor, 1.0f, 0.5f);
        }
    }
}

void TriShip::updateMineColors() {
    // Dangerous mine colors - bright yellow/orange warning colors
    m_WarningColor.red = 255;
    m_WarningColor.green = 200;
    m_WarningColor.blue = 0;     // Bright yellow-orange warning
    m_WarningColor.alpha = (int)(220 * m_PulseIntensity);
    
    // Extreme danger color - red alert
    m_DangerColor.red = 255;
    m_DangerColor.green = 50;
    m_DangerColor.blue = 0;      // Bright red danger
    m_DangerColor.alpha = (int)(255 * m_PulseIntensity);
}

void TriShip::drawMineEffects() {
    // Create aggressive warning pulse for mines
    m_MineWarningIntensity = 0.6f + sin(m_AnimationTime * 4.0f) * 0.4f; // Fast warning pulse
    
    // Outer danger aura - large and alarming
    Color outerWarning = m_WarningColor;
    outerWarning.alpha = (int)(outerWarning.alpha * 0.4f * m_MineWarningIntensity);
    for (int line = 0; line < 3; line++) {
        Window::DrawVolumetricLineWithBloom(&newTriangle[line], outerWarning, 6.0f, 0.8f);
    }
    
    // Middle warning layer
    Color middleWarning = m_WarningColor;
    middleWarning.alpha = (int)(middleWarning.alpha * 0.7f * m_MineWarningIntensity);
    for (int line = 0; line < 3; line++) {
        Window::DrawVolumetricLineWithBloom(&newTriangle[line], middleWarning, 3.5f, 0.6f);
    }
    
    // Core danger indicator - bright and pulsing
    for (int line = 0; line < 3; line++) {
        Window::DrawVolumetricLineWithBloom(&newTriangle[line], m_DangerColor, 2.0f, 0.4f);
    }
    
    // Add warning spikes during peak intensity
    if (m_MineWarningIntensity > 0.8f) {
        for (int point = 0; point < 3; point++) {
            Vector2i spikeStart = newTriangle[point].start;
            
            // Calculate spike direction (outward from center)
            Vector2i center = Vector2i(
                (newTriangle[0].start.x + newTriangle[1].start.x + newTriangle[2].start.x) / 3,
                (newTriangle[0].start.y + newTriangle[1].start.y + newTriangle[2].start.y) / 3
            );
            
            Vector2i direction = spikeStart - center;
            float length = sqrt((float)(direction.x * direction.x + direction.y * direction.y));
            if (length > 0) {
                // Normalize and extend warning spikes
                direction.x = (int)(direction.x / length * 12.0f * m_MineWarningIntensity);
                direction.y = (int)(direction.y / length * 12.0f * m_MineWarningIntensity);
                
                Line warningSpike;
                warningSpike.start = spikeStart;
                warningSpike.end = spikeStart + direction;
                
                // Draw dangerous warning spike
                Window::DrawVolumetricLineWithBloom(&warningSpike, m_DangerColor, 1.5f, 0.3f);
            }
        }
    }
}

void TriShip::drawDoubleMineEffects() {
    // Double mine is even more dangerous - more intense effects
    m_MineWarningIntensity = 0.7f + sin(m_AnimationTime * 5.5f) * 0.3f; // Even faster pulse
    
    // Massive outer danger aura
    Color extremeWarning = m_DangerColor;
    extremeWarning.alpha = (int)(extremeWarning.alpha * 0.5f * m_MineWarningIntensity);
    for (int line = 0; line < 3; line++) {
        Window::DrawVolumetricLineWithBloom(&newTriangle[line], extremeWarning, 8.0f, 1.0f);
    }
    
    // Bright warning layer
    Color brightWarning = m_WarningColor;
    brightWarning.alpha = (int)(brightWarning.alpha * 0.8f * m_MineWarningIntensity);
    for (int line = 0; line < 3; line++) {
        Window::DrawVolumetricLineWithBloom(&newTriangle[line], brightWarning, 5.0f, 0.7f);
    }
    
    // Extreme danger core
    Color extremeDanger = m_DangerColor;
    extremeDanger.alpha = 255; // Always maximum intensity
    for (int line = 0; line < 3; line++) {
        Window::DrawVolumetricLineWithBloom(&newTriangle[line], extremeDanger, 3.0f, 0.5f);
    }
    
    // Always show warning spikes for double mines - they're always dangerous
    for (int point = 0; point < 3; point++) {
        Vector2i spikeStart = newTriangle[point].start;
        
        // Calculate spike direction
        Vector2i center = Vector2i(
            (newTriangle[0].start.x + newTriangle[1].start.x + newTriangle[2].start.x) / 3,
            (newTriangle[0].start.y + newTriangle[1].start.y + newTriangle[2].start.y) / 3
        );
        
        Vector2i direction = spikeStart - center;
        float length = sqrt((float)(direction.x * direction.x + direction.y * direction.y));
        if (length > 0) {
            // Longer, more aggressive spikes for double mines
            direction.x = (int)(direction.x / length * 15.0f * m_MineWarningIntensity);
            direction.y = (int)(direction.y / length * 15.0f * m_MineWarningIntensity);
            
            Line extremeSpike;
            extremeSpike.start = spikeStart;
            extremeSpike.end = spikeStart + direction;
            
            // Draw extreme danger spike
            Window::DrawVolumetricLineWithBloom(&extremeSpike, extremeDanger, 2.0f, 0.4f);
        }
    }
}

void TriShip::clearVaporTrail() {
    m_VapourTrail.clearTrail();
}

} // namespace omegarace
