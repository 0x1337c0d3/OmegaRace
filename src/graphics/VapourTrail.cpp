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

    // Set default blue-white vapour color
    m_TrailColor.red = 180;
    m_TrailColor.green = 220;
    m_TrailColor.blue = 255;
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

    // Draw trail from oldest to newest points
    for (int i = 0; i < m_TrailLength - 1; i++) {
        int currentIndex = (m_TrailIndex + i) % m_TrailLength;
        int nextIndex = (m_TrailIndex + i + 1) % m_TrailLength;

        // Skip if either point is not active
        if (m_TrailAlpha[currentIndex] <= 0.0f || m_TrailAlpha[nextIndex] <= 0.0f) {
            continue;
        }

        // Create line between consecutive trail points
        Line trailLine;
        trailLine.start = Vector2i((int)m_TrailPoints[currentIndex].x, (int)m_TrailPoints[currentIndex].y);
        trailLine.end = Vector2i((int)m_TrailPoints[nextIndex].x, (int)m_TrailPoints[nextIndex].y);

        // Calculate distance between points to skip if too far apart
        float distance =
            sqrt(pow(trailLine.end.x - trailLine.start.x, 2) + pow(trailLine.end.y - trailLine.start.y, 2));

        if (distance > 15.0f)
            continue; // Skip large gaps

        // Calculate trail color and intensity
        float avgAlpha = (m_TrailAlpha[currentIndex] + m_TrailAlpha[nextIndex]) * 0.5f;

        // Apply alpha to trail color
        Color trailColor = m_TrailColor;
        trailColor.alpha = (int)(trailColor.alpha * avgAlpha * 0.6f); // 60% base opacity

        // Draw trail segment with varying thickness based on age
        float thickness = m_MinThickness + (m_MaxThickness - m_MinThickness) * avgAlpha;
        Window::DrawVolumetricLineWithBloom(&trailLine, trailColor, thickness * 2, 0.5f);
    }
}

void VapourTrail::setTrailColor(const Color& color) {
    m_TrailColor = color;
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
