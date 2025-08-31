#include "ExplosionLine.h"
#include "Window.h"

namespace omegarace {

ExplosionLine::ExplosionLine() {
    m_Active = false;
    m_TimerAmount = 120; // Longer duration for more dramatic effect
    pTimer = std::make_unique<Timer>();
    pTimer->start();

    // Start with bright white-hot core
    m_Color.red = 255;
    m_Color.green = 255;
    m_Color.blue = 255;
    m_Color.alpha = 255;
    
    m_InitialThickness = 0.0f;
    m_MaxThickness = 0.0f;
    m_CurrentThickness = 0.0f;
    m_ExplosionPhase = 0.0f;
}

ExplosionLine::~ExplosionLine() {
}

void ExplosionLine::draw() {
    if (m_Active) {
        // OPTIMIZED: Single layer explosion for performance
        Color dynamicColor = calculateExplosionColor();
        Window::DrawVolumetricLineWithBloom(&m_Line, dynamicColor, m_CurrentThickness * 1.5f, 0.8f);
        
        // Add sparks effect during early explosion phase
        if (m_ExplosionPhase < 0.4f) {
            drawExplosionSparks();
        }
    }
}

void ExplosionLine::update(double Frame) {
    if (m_Active) {
        if (m_Timer < pTimer->seconds())
            m_Active = false;

        updateFrame(Frame);
        m_Line.end = m_Location;

        // Calculate explosion phase (0.0 = start, 1.0 = end)
        double totalDuration = (double)m_TimerAmount * 0.01;
        double timeElapsed = totalDuration - (m_Timer - pTimer->seconds());
        m_ExplosionPhase = (float)(timeElapsed / totalDuration);
        
        // Dynamic thickness animation
        if (m_ExplosionPhase < 0.2f) {
            // Rapid expansion phase
            m_CurrentThickness = m_InitialThickness + (m_MaxThickness * (m_ExplosionPhase / 0.2f));
        } else if (m_ExplosionPhase < 0.6f) {
            // Peak intensity phase
            float peakPhase = (m_ExplosionPhase - 0.2f) / 0.4f;
            m_CurrentThickness = m_MaxThickness * (1.0f - peakPhase * 0.3f); // Slight reduction
        } else {
            // Fade out phase
            float fadePhase = (m_ExplosionPhase - 0.6f) / 0.4f;
            m_CurrentThickness = m_MaxThickness * 0.7f * (1.0f - fadePhase);
        }
        
        // Update line alpha with more dramatic fade curve
        float alphaMultiplier = 1.0f - (m_ExplosionPhase * m_ExplosionPhase); // Quadratic fade
        m_Color.alpha = (int)(255 * alphaMultiplier);
    }
}

void ExplosionLine::activate(const Vector2i& location, float angle, int size) {
    m_Active = true;
    m_Location = location + Vector2i(Window::Random(0, size), Window::Random(0, size));
    m_Line.start = location;
    m_Line.end = location;
    double frame = 0;
    updateFrame(frame);

    float sinRot = sin(angle);
    float cosRot = cos(angle);

    int maxV = 35; // Increased max velocity for more dramatic effect
    int minV = 20; // Increased min velocity
    m_Velocity = Vector2f(cosRot * (float)Window::Random(minV, maxV), sinRot * (float)Window::Random(minV, maxV));
    m_Timer = pTimer->seconds() + (float)Window::Random(m_TimerAmount / 2, m_TimerAmount) * 0.01;
    
    // Reset explosion properties
    m_ExplosionPhase = 0.0f;
    m_InitialThickness = 1.0f;
    m_MaxThickness = (float)Window::Random(4, 8); // Variable max thickness for variety
    m_CurrentThickness = m_InitialThickness;
    
    // Start with bright white-hot color
    m_Color.red = 255;
    m_Color.green = 255;
    m_Color.blue = 255;
    m_Color.alpha = 255;
}

Color ExplosionLine::calculateExplosionColor() {
    Color result;
    
    if (m_ExplosionPhase < 0.15f) {
        // White-hot flash (very brief)
        result.red = 255;
        result.green = 255;
        result.blue = 255;
    } else if (m_ExplosionPhase < 0.35f) {
        // Transition to bright yellow/orange
        float transitionPhase = (m_ExplosionPhase - 0.15f) / 0.2f;
        result.red = 255;
        result.green = (int)(255 - transitionPhase * 100); // 255 -> 155
        result.blue = (int)(255 - transitionPhase * 255);  // 255 -> 0
    } else if (m_ExplosionPhase < 0.65f) {
        // Orange to red transition
        float transitionPhase = (m_ExplosionPhase - 0.35f) / 0.3f;
        result.red = 255;
        result.green = (int)(155 - transitionPhase * 155); // 155 -> 0
        result.blue = 0;
    } else {
        // Deep red fade out
        float fadePhase = (m_ExplosionPhase - 0.65f) / 0.35f;
        result.red = (int)(255 - fadePhase * 100); // Stay reddish
        result.green = 0;
        result.blue = 0;
    }
    
    result.alpha = m_Color.alpha; // Use the calculated alpha from update
    return result;
}

void ExplosionLine::drawExplosionSparks() {
    // Create small spark lines perpendicular to the main explosion line
    Vector2i lineVec = m_Line.end - m_Line.start;
    float lineLength = sqrt((float)(lineVec.x * lineVec.x + lineVec.y * lineVec.y));
    
    if (lineLength > 5.0f) { // Only add sparks to lines that are long enough
        // Create 1-3 sparks along the line
        int numSparks = Window::Random(1, 3);
        
        for (int i = 0; i < numSparks; i++) {
            // Random position along the explosion line
            float t = Window::Random(20, 80) / 100.0f;
            Vector2i sparkStart;
            sparkStart.x = m_Line.start.x + (int)(lineVec.x * t);
            sparkStart.y = m_Line.start.y + (int)(lineVec.y * t);
            
            // Create perpendicular spark direction
            Vector2i perpVec;
            perpVec.x = -lineVec.y;
            perpVec.y = lineVec.x;
            
            // Normalize and scale
            float perpLength = sqrt((float)(perpVec.x * perpVec.x + perpVec.y * perpVec.y));
            if (perpLength > 0) {
                float sparkLength = Window::Random(3, 8) * (1.0f - m_ExplosionPhase); // Shorter as explosion progresses
                perpVec.x = (int)(perpVec.x / perpLength * sparkLength);
                perpVec.y = (int)(perpVec.y / perpLength * sparkLength);
                
                // Random direction
                if (Window::Random(0, 1)) {
                    perpVec.x = -perpVec.x;
                    perpVec.y = -perpVec.y;
                }
                
                Line sparkLine;
                sparkLine.start = sparkStart;
                sparkLine.end = sparkStart + perpVec;
                
                // Bright spark color
                Color sparkColor;
                sparkColor.red = 255;
                sparkColor.green = 200;
                sparkColor.blue = 100;
                sparkColor.alpha = (int)(180 * (1.0f - m_ExplosionPhase));
                
                Window::DrawVolumetricLineWithBloom(&sparkLine, sparkColor, 1.0f, 0.8f);
            }
        }
    }
}

void ExplosionLine::pauseTimer() {
    pTimer->pause();
}

void ExplosionLine::unpauseTimer() {
    pTimer->unpause();
}

} // namespace omegarace
