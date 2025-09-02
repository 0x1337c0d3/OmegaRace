#include "PlayerExplosionLine.h"

namespace omegarace {

PlayerExplosionLine::PlayerExplosionLine() : Entity() {
    // Start with brilliant white-hot explosion
    m_Color.red = 255;
    m_Color.green = 255;
    m_Color.blue = 255;
    m_Color.alpha = 255;

    // Initialize catastrophic explosion properties
    m_ExplosionPhase = 0.0f;
    m_MaxLifetime = 0.0f;
    m_CurrentLifetime = 0.0f;
    m_InitialThickness = 0.0f;
    m_MaxThickness = 0.0f;
    m_CurrentThickness = 0.0f;
    m_RotationSpeed = 0.0f;
    m_CurrentRotation = 0.0f;
    m_FragmentIntensity = 1.0f;
}

PlayerExplosionLine::~PlayerExplosionLine() {
}

void PlayerExplosionLine::initialize(const Line& expLine, const Vector2i& location) {
    shipLine = expLine;
    shipLine.start -= location;
    shipLine.end -= location;
    m_Location = location;

    // Much more violent explosion forces for catastrophic player death
    int maxForce = 400; // Doubled the force
    m_Velocity = Vector2f((Window::Random(-maxForce, maxForce) * 0.1f) - (Window::Random(-maxForce, maxForce) * 0.1f),
                          (Window::Random(-maxForce, maxForce) * 0.1f) - (Window::Random(-maxForce, maxForce) * 0.1f));

    // Initialize catastrophic properties
    m_MaxLifetime = Window::Random(180, 300) * 0.01f; // 1.8-3.0 seconds duration
    m_CurrentLifetime = 0.0f;
    m_InitialThickness = 1.0f;
    m_MaxThickness = (float)Window::Random(6, 12); // Much thicker fragments
    m_CurrentThickness = m_InitialThickness;

    // Add violent spinning to fragments
    m_RotationSpeed = (Window::Random(-300, 300) * 0.01f); // -3 to +3 radians per second
    m_CurrentRotation = 0.0f;

    // Variable fragment intensity for variety
    m_FragmentIntensity = Window::Random(80, 120) * 0.01f; // 0.8 to 1.2
}

void PlayerExplosionLine::update(double frame) {
    updateFrame(frame);

    // Update explosion timeline
    m_CurrentLifetime += frame;
    m_ExplosionPhase = m_CurrentLifetime / m_MaxLifetime;

    // Clamp explosion phase
    if (m_ExplosionPhase > 1.0f)
        m_ExplosionPhase = 1.0f;

    // Update rotation for spinning fragments
    m_CurrentRotation += m_RotationSpeed * frame;

    // Calculate rotated ship line for spinning effect
    Line rotatedShipLine = rotateLineAroundCenter(shipLine, m_CurrentRotation);

    // Apply fragment scaling (fragments can grow slightly then shrink)
    float fragmentScale = calculateFragmentScale();
    Line scaledLine = scaleLineFromCenter(rotatedShipLine, fragmentScale);

    // Update final line position
    newLine.start = m_Location + scaledLine.start;
    newLine.end = m_Location + scaledLine.end;

    // Update thickness animation
    updateThicknessAnimation();
}

void PlayerExplosionLine::draw() {
    if (m_ExplosionPhase < 1.0f) { // Only draw if still active
        std::cout << "PlayerExplosionLine::draw() called - m_ExplosionPhase=" << m_ExplosionPhase << std::endl;
        // Calculate catastrophic explosion colors
        Color fragmentColor = calculateCatastrophicColor();

        // Draw multiple layers for maximum visual impact
        // Massive outer explosion aura (for catastrophic feel)
        Color outerAura = fragmentColor;
        outerAura.alpha = outerAura.alpha / 6;
        Window::DrawVolumetricLineWithBloom(&newLine, outerAura, m_CurrentThickness * 4.0f, 2.5f);

        // Large middle explosion layer
        Color middleExplosion = fragmentColor;
        middleExplosion.alpha = middleExplosion.alpha / 3;
        Window::DrawVolumetricLineWithBloom(&newLine, middleExplosion, m_CurrentThickness * 2.5f, 2.0f);

        // Bright inner fragment core
        Color innerCore = fragmentColor;
        innerCore.alpha = (int)(innerCore.alpha * 1.2f); // Extra bright
        if (innerCore.alpha > 255)
            innerCore.alpha = 255;
        Window::DrawVolumetricLineWithBloom(&newLine, innerCore, m_CurrentThickness, 1.5f);

        // Add violent electrical arcs during early explosion phase
        if (m_ExplosionPhase < 0.5f) {
            drawCatastrophicArcs(fragmentColor);
        }

        // Add debris sparks throughout the explosion
        if (m_ExplosionPhase < 0.8f) {
            drawDebrisSparks(fragmentColor);
        }
    }
}

Color PlayerExplosionLine::calculateCatastrophicColor() {
    Color result;

    if (m_ExplosionPhase < 0.1f) {
        // Brilliant white-hot nuclear flash
        result.red = 255;
        result.green = 255;
        result.blue = 255;
    } else if (m_ExplosionPhase < 0.25f) {
        // Intense blue-white plasma
        float phase = (m_ExplosionPhase - 0.1f) / 0.15f;
        result.red = (int)(255 - phase * 55);   // 255 -> 200
        result.green = (int)(255 - phase * 55); // 255 -> 200
        result.blue = 255;
    } else if (m_ExplosionPhase < 0.45f) {
        // Electric blue to cyan
        float phase = (m_ExplosionPhase - 0.25f) / 0.2f;
        result.red = (int)(200 - phase * 100);  // 200 -> 100
        result.green = (int)(200 + phase * 55); // 200 -> 255
        result.blue = 255;
    } else if (m_ExplosionPhase < 0.65f) {
        // Cyan to orange (like hot metal)
        float phase = (m_ExplosionPhase - 0.45f) / 0.2f;
        result.red = (int)(100 + phase * 155);   // 100 -> 255
        result.green = (int)(255 - phase * 100); // 255 -> 155
        result.blue = (int)(255 - phase * 255);  // 255 -> 0
    } else {
        // Orange to deep red fade
        float phase = (m_ExplosionPhase - 0.65f) / 0.35f;
        result.red = (int)(255 - phase * 100);   // 255 -> 155
        result.green = (int)(155 - phase * 155); // 155 -> 0
        result.blue = 0;
    }

    // Apply fragment intensity and fade
    float fadeMultiplier = (1.0f - m_ExplosionPhase) * m_FragmentIntensity;
    result.alpha = (int)(255 * fadeMultiplier);

    return result;
}

Line PlayerExplosionLine::rotateLineAroundCenter(const Line& line, float angle) {
    // Calculate line center
    Vector2i center;
    center.x = (line.start.x + line.end.x) / 2;
    center.y = (line.start.y + line.end.y) / 2;

    // Translate to origin, rotate, translate back
    Line rotated;
    Vector2i startRel = line.start - center;
    Vector2i endRel = line.end - center;

    float cosA = cos(angle);
    float sinA = sin(angle);

    rotated.start.x = (int)(startRel.x * cosA - startRel.y * sinA) + center.x;
    rotated.start.y = (int)(startRel.x * sinA + startRel.y * cosA) + center.y;

    rotated.end.x = (int)(endRel.x * cosA - endRel.y * sinA) + center.x;
    rotated.end.y = (int)(endRel.x * sinA + endRel.y * cosA) + center.y;

    return rotated;
}

Line PlayerExplosionLine::scaleLineFromCenter(const Line& line, float scale) {
    Vector2i center;
    center.x = (line.start.x + line.end.x) / 2;
    center.y = (line.start.y + line.end.y) / 2;

    Line scaled;
    Vector2i startRel = line.start - center;
    Vector2i endRel = line.end - center;

    scaled.start.x = (int)(startRel.x * scale) + center.x;
    scaled.start.y = (int)(startRel.y * scale) + center.y;
    scaled.end.x = (int)(endRel.x * scale) + center.x;
    scaled.end.y = (int)(endRel.y * scale) + center.y;

    return scaled;
}

float PlayerExplosionLine::calculateFragmentScale() {
    if (m_ExplosionPhase < 0.2f) {
        // Initial expansion
        return 1.0f + (m_ExplosionPhase / 0.2f) * 0.3f; // 1.0 -> 1.3
    } else if (m_ExplosionPhase < 0.7f) {
        // Maintain size
        return 1.3f;
    } else {
        // Shrink as fragments burn up
        float shrinkPhase = (m_ExplosionPhase - 0.7f) / 0.3f;
        return 1.3f - shrinkPhase * 0.5f; // 1.3 -> 0.8
    }
}

void PlayerExplosionLine::updateThicknessAnimation() {
    if (m_ExplosionPhase < 0.15f) {
        // Violent expansion
        m_CurrentThickness = m_InitialThickness + (m_MaxThickness * (m_ExplosionPhase / 0.15f));
    } else if (m_ExplosionPhase < 0.5f) {
        // Peak catastrophic phase
        float peakPhase = (m_ExplosionPhase - 0.15f) / 0.35f;
        m_CurrentThickness = m_MaxThickness * (1.0f + sin(peakPhase * 6.28f) * 0.2f); // Oscillate ±20%
    } else {
        // Gradual fade and shrink
        float fadePhase = (m_ExplosionPhase - 0.5f) / 0.5f;
        m_CurrentThickness = m_MaxThickness * (1.0f - fadePhase * 0.7f); // Shrink to 30%
    }
}

void PlayerExplosionLine::drawCatastrophicArcs(const Color& baseColor) {
    // Create violent electrical discharges between fragments
    Vector2i lineVec = newLine.end - newLine.start;
    float lineLength = sqrt((float)(lineVec.x * lineVec.x + lineVec.y * lineVec.y));

    if (lineLength > 8.0f) {
        // Create 3-6 violent arcs
        int numArcs = Window::Random(3, 6);

        for (int i = 0; i < numArcs; i++) {
            float t = Window::Random(10, 90) / 100.0f;
            Vector2i arcStart;
            arcStart.x = newLine.start.x + (int)(lineVec.x * t);
            arcStart.y = newLine.start.y + (int)(lineVec.y * t);

            // Create multiple arc directions for chaotic effect
            for (int dir = 0; dir < 2; dir++) {
                Vector2i perpVec;
                perpVec.x = -lineVec.y;
                perpVec.y = lineVec.x;

                float perpLength = sqrt((float)(perpVec.x * perpVec.x + perpVec.y * perpVec.y));
                if (perpLength > 0) {
                    float arcLength = Window::Random(8, 20) * (1.0f - m_ExplosionPhase * 0.5f);
                    perpVec.x = (int)(perpVec.x / perpLength * arcLength);
                    perpVec.y = (int)(perpVec.y / perpLength * arcLength);

                    if (dir == 1) {
                        perpVec.x = -perpVec.x;
                        perpVec.y = -perpVec.y;
                    }

                    Line arcLine;
                    arcLine.start = arcStart;
                    arcLine.end = arcStart + perpVec;

                    Color arcColor;
                    arcColor.red = 150;
                    arcColor.green = 200;
                    arcColor.blue = 255;
                    arcColor.alpha = (int)(200 * (1.0f - m_ExplosionPhase * 0.8f));

                    Window::DrawVolumetricLineWithBloom(&arcLine, arcColor, 1.5f, 1.0f);
                }
            }
        }
    }
}

void PlayerExplosionLine::drawDebrisSparks(const Color& baseColor) {
    // Create small debris particles flying off the main fragment
    Vector2i lineCenter;
    lineCenter.x = (newLine.start.x + newLine.end.x) / 2;
    lineCenter.y = (newLine.start.y + newLine.end.y) / 2;

    // Create 2-4 debris sparks
    int numSparks = Window::Random(2, 4);

    for (int i = 0; i < numSparks; i++) {
        // Random direction from fragment center
        float sparkAngle = Window::Random(0, 628) * 0.01f; // 0 to 2π
        float sparkLength = Window::Random(3, 12) * (1.0f - m_ExplosionPhase * 0.6f);

        Vector2i sparkEnd;
        sparkEnd.x = lineCenter.x + (int)(cos(sparkAngle) * sparkLength);
        sparkEnd.y = lineCenter.y + (int)(sin(sparkAngle) * sparkLength);

        Line sparkLine;
        sparkLine.start = lineCenter;
        sparkLine.end = sparkEnd;

        Color sparkColor;
        sparkColor.red = 255;
        sparkColor.green = 180;
        sparkColor.blue = 50;
        sparkColor.alpha = (int)(150 * (1.0f - m_ExplosionPhase));

        Window::DrawVolumetricLineWithBloom(&sparkLine, sparkColor, 0.8f, 0.6f);
    }
}

} // namespace omegarace
