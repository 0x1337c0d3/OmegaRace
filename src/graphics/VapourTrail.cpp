#include "VapourTrail.h"
#include <cmath>

namespace omegarace {

VapourTrail::VapourTrail(int trailLength) {
    m_TrailLength = trailLength;
    m_Active = true;
    m_TrailIndex = 0;
    m_UpdateFrequency = 2; // Add new point every 2 updates
    m_FadeRate = 0.015f;   // Fade speed
    m_MaxThickness = 2.0f;
    m_MinThickness = 0.5f;

    // Allocate arrays
    m_TrailPoints = new Vector2f[m_TrailLength];
    m_TrailAlpha = new float[m_TrailLength];

    // Set bright orange default vapour color for fiery trail effect
    m_TrailColor.red = 255;    // Full red
    m_TrailColor.green = 140;  // Medium orange
    m_TrailColor.blue = 0;     // No blue for pure orange
    m_TrailColor.alpha = 255;

    clearTrail();
}

VapourTrail::~VapourTrail() {
    delete[] m_TrailPoints;
    delete[] m_TrailAlpha;
}

void VapourTrail::setActive(bool active) {
    m_Active = active;

    // Clear trail when deactivating
    if (!active) {
        clearTrail();
    }
}

void VapourTrail::update(const Vector2f& position) {
    if (!m_Active)
        return;

    // Add new trail point every few updates for smooth trail
    m_UpdateCounter++;

    if (m_UpdateCounter >= m_UpdateFrequency) {
        m_UpdateCounter = 0;

        // Add current position to trail
        m_TrailPoints[m_TrailIndex] = position;
        m_TrailAlpha[m_TrailIndex] = 1.0f; // Full alpha for new point

        // Move to next index (circular buffer)
        m_TrailIndex = (m_TrailIndex + 1) % m_TrailLength;
    }

    // Fade out all trail points
    for (int i = 0; i < m_TrailLength; i++) {
        if (m_TrailAlpha[i] > 0.0f) {
            m_TrailAlpha[i] -= m_FadeRate;
            if (m_TrailAlpha[i] < 0.0f) {
                m_TrailAlpha[i] = 0.0f;
            }
        }
    }
}

void VapourTrail::draw() {
    if (!m_Active)
        return;

    // Draw trail from oldest to newest points using new shader-based approach
    for (int i = 0; i < m_TrailLength - 1; i++) {
        int currentIndex = (m_TrailIndex + i) % m_TrailLength;
        int nextIndex = (m_TrailIndex + i + 1) % m_TrailLength;

        // Skip if either point is not active
        if (m_TrailAlpha[currentIndex] <= 0.0f || m_TrailAlpha[nextIndex] <= 0.0f) {
            continue;
        }

        // Calculate distance between points to skip if too far apart
        float distance = sqrt(pow(m_TrailPoints[nextIndex].x - m_TrailPoints[currentIndex].x, 2) + 
                            pow(m_TrailPoints[nextIndex].y - m_TrailPoints[currentIndex].y, 2));

        if (distance > 15.0f)
            continue; // Skip large gaps

        // Calculate trail position (0.0 = oldest, 1.0 = newest)
        float trailPosition = (float)i / (float)(m_TrailLength - 1);
        
        // Calculate trail color and intensity
        float avgAlpha = (m_TrailAlpha[currentIndex] + m_TrailAlpha[nextIndex]) * 0.5f;

        // Apply alpha to trail color with stronger base opacity for smoky effect
        Color trailColor = m_TrailColor;
        trailColor.alpha = (int)(trailColor.alpha * avgAlpha * 1.0f); // 100% base opacity for strong visibility

        // Calculate trail width based on age (wider at newer segments) - much thicker
        float width = m_MinThickness + (m_MaxThickness - m_MinThickness) * avgAlpha;
        
        // Use new shader-based vapor trail rendering for smoky effect - much thicker trails
        Window::DrawVaporTrailSegment(m_TrailPoints[currentIndex], m_TrailPoints[nextIndex], 
                                    width * 6.0f, trailPosition, trailColor, avgAlpha * 2.0f);
    }
}

void VapourTrail::setTrailColor(const Color& color) {
    m_TrailColor = color;
}

void VapourTrail::setTrailLength(int length) {
    // Only resize if the new length is different and reasonable
    if (length != m_TrailLength && length > 5 && length < 100) {
        // Save current trail data
        Vector2f* oldPoints = m_TrailPoints;
        float* oldAlpha = m_TrailAlpha;
        int oldLength = m_TrailLength;
        int oldIndex = m_TrailIndex;
        
        // Update length and allocate new arrays
        m_TrailLength = length;
        m_TrailPoints = new Vector2f[m_TrailLength];
        m_TrailAlpha = new float[m_TrailLength];
        
        // Initialize new arrays
        for (int i = 0; i < m_TrailLength; i++) {
            m_TrailPoints[i] = Vector2f(0, 0);
            m_TrailAlpha[i] = 0.0f;
        }
        
        // Copy over existing trail data if we had any
        if (oldPoints && oldAlpha) {
            int copyLength = fmin(oldLength, m_TrailLength);
            for (int i = 0; i < copyLength; i++) {
                int sourceIndex = (oldIndex + i) % oldLength;
                m_TrailPoints[i] = oldPoints[sourceIndex];
                m_TrailAlpha[i] = oldAlpha[sourceIndex];
            }
            m_TrailIndex = copyLength % m_TrailLength;
            
            // Clean up old arrays
            delete[] oldPoints;
            delete[] oldAlpha;
        } else {
            m_TrailIndex = 0;
        }
        
        m_UpdateCounter = 0;
    }
}

void VapourTrail::setFadeRate(float fadeRate) {
    m_FadeRate = fadeRate;
}

void VapourTrail::setUpdateFrequency(int frequency) {
    m_UpdateFrequency = frequency;
}

void VapourTrail::setMaxThickness(float thickness) {
    m_MaxThickness = thickness;
}

void VapourTrail::setMinThickness(float thickness) {
    m_MinThickness = thickness;
}

void VapourTrail::clearTrail() {
    for (int i = 0; i < m_TrailLength; i++) {
        m_TrailPoints[i] = Vector2f(0, 0);
        m_TrailAlpha[i] = 0.0f;
    }
    m_TrailIndex = 0;
    m_UpdateCounter = 0;
}

} // namespace omegarace
