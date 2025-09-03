#include "Borders.h"
#include <cmath>

namespace omegarace {

Borders::Borders() {
    pTimer = std::make_unique<Timer>();

    // Initialize Geometry Wars grid parameters
    m_GridSpacing = 40; // Distance between grid lines
    m_GridAnimationTime = 0.0f;
    m_DistortionRadius = 120.0f;  // Radius of player influence
    m_DistortionStrength = 15.0f; // How much the grid bends

    // More subtle dark blue-cyan grid color (much more muted)
    m_GridColor.red = 0;
    m_GridColor.green = 30; // Reduced from 100 to 30
    m_GridColor.blue = 150;  // Reduced from 150 to 50
    m_GridColor.alpha = 40; // Reduced from 40 to 15 for much more subtlety to avoid status display interference
}

Borders::~Borders() {
}

void Borders::initialize() {
    pTimer->start();

    lineColor.red = 0;
    lineColor.green = 255;
    lineColor.blue = 255;
    lineColor.alpha = 255;

    borderColor.red = 255;
    borderColor.green = 255;
    borderColor.blue = 255;
    borderColor.alpha = 255;

    int InsideWidth = Window::GetWindowSize().x * 0.666;
    int InsideHight = Window::GetWindowSize().y * 0.24;  // Reduced from 0.25 to 0.24 to avoid bottom clipping
    int InsideLeft = Window::GetWindowSize().x / 2 - InsideWidth / 2;
    int InsideRight = Window::GetWindowSize().x / 2 + InsideWidth / 2;
    int InsideTop = Window::GetWindowSize().y / 2 - InsideHight / 2;
    int InsideBottom = Window::GetWindowSize().y / 2 + InsideHight / 2;

    insideBorder.x = InsideLeft;
    insideBorder.y = InsideTop;
    insideBorder.h = InsideHight;
    insideBorder.w = InsideWidth;

    insideDots[0] = Vector2i(InsideLeft + (InsideLeft / 10), InsideTop);
    insideDots[1] = Vector2i(InsideRight - (InsideLeft / 10), InsideTop);
    insideDots[2] = Vector2i(InsideRight, InsideTop + (InsideLeft / 10));
    insideDots[3] = Vector2i(InsideRight, InsideBottom - (InsideLeft / 10));
    insideDots[4] = Vector2i(InsideRight - (InsideLeft / 10), InsideBottom);
    insideDots[5] = Vector2i(InsideLeft + (InsideLeft / 10), InsideBottom);
    insideDots[6] = Vector2i(InsideLeft, InsideBottom - (InsideLeft / 10));
    insideDots[7] = Vector2i(InsideLeft, InsideTop + (InsideLeft / 10));

    insideFieldLines[0].start = Vector2i(InsideLeft, InsideTop);
    insideFieldLines[1].start = Vector2i(InsideRight, InsideTop);
    insideFieldLines[2].start = Vector2i(InsideRight, InsideBottom);
    insideFieldLines[3].start = Vector2i(InsideLeft, InsideBottom);

    insideFieldLines[0].end = Vector2i(InsideRight, InsideTop);
    insideFieldLines[1].end = Vector2i(InsideRight, InsideBottom);
    insideFieldLines[2].end = Vector2i(InsideLeft, InsideBottom);
    insideFieldLines[3].end = Vector2i(InsideLeft, InsideTop);

    outsideFieldLines[0].start = Vector2i(5, 5);
    outsideFieldLines[1].start = Vector2i(Window::GetWindowSize().x / 2, 5);
    outsideFieldLines[2].start = Vector2i(5, Window::GetWindowSize().y - 5);
    outsideFieldLines[3].start = Vector2i(Window::GetWindowSize().x / 2, Window::GetWindowSize().y - 5);
    outsideFieldLines[4].start = Vector2i(outsideFieldLines[0].start);
    outsideFieldLines[5].start = Vector2i(5, Window::GetWindowSize().y / 2);
    outsideFieldLines[6].start = Vector2i(Window::GetWindowSize().x - 5, 5);
    outsideFieldLines[7].start = Vector2i(Window::GetWindowSize().x - 5, Window::GetWindowSize().y / 2);

    outsideFieldLines[0].end = Vector2i(outsideFieldLines[1].start);
    outsideFieldLines[1].end = Vector2i(outsideFieldLines[6].start);
    outsideFieldLines[2].end = Vector2i(outsideFieldLines[3].start);
    outsideFieldLines[3].end = Vector2i(Window::GetWindowSize().x - 5, Window::GetWindowSize().y - 5);
    outsideFieldLines[4].end = Vector2i(outsideFieldLines[5].start);
    outsideFieldLines[5].end = Vector2i(outsideFieldLines[2].start);
    outsideFieldLines[6].end = Vector2i(outsideFieldLines[7].start);
    outsideFieldLines[7].end = Vector2i(Window::GetWindowSize().x - 5, Window::GetWindowSize().y - 5);

    outsideDots[0] = outsideFieldLines[0].start;
    outsideDots[1] = outsideFieldLines[1].start;
    outsideDots[2] = outsideFieldLines[2].start;
    outsideDots[3] = outsideFieldLines[7].start;
    outsideDots[4] = outsideFieldLines[7].end;
    outsideDots[5] = outsideFieldLines[3].start;
    outsideDots[6] = outsideFieldLines[5].start;
    outsideDots[7] = outsideFieldLines[6].start;
}

void Borders::update() {
    // Update grid animation (slower and more subtle)
    m_GridAnimationTime += 0.008f; // Reduced from 0.02f to 0.008f for more subtle animation

    for (int line = 0; line < 4; line++) {
        if (pTimer->seconds() > insideLineTimers[line])
            insideLineOn[line] = false;
    }

    for (int line = 0; line < 8; line++) {
        if (pTimer->seconds() > outsideLineTimers[line])
            outsideLineOn[line] = false;
    }
}

void Borders::draw() {
    Rectangle r;
    r.x = insideBorder.x;
    r.y = insideBorder.y;
    r.width = insideBorder.w;
    r.height = insideBorder.h;
    Window::DrawRect(&r, borderColor);

    for (int dot = 0; dot < 8; dot++) {
        Window::DrawPoint(&insideDots[dot], lineColor);
    }

    // Draw inside field lines with electric shimmer effect
    for (int line = 0; line < 4; line++) {
        if (insideLineOn[line]) {
            drawElectricShimmerLine(&insideFieldLines[line], insideLineTimers[line]);
        }
    }

    // Draw outside field lines with electric shimmer effect
    for (int line = 0; line < 8; line++) {
        if (outsideLineOn[line]) {
            drawElectricShimmerLine(&outsideFieldLines[line], outsideLineTimers[line]);
        }

        Window::DrawPoint(&outsideDots[line], lineColor);
    }
}

void Borders::setInsideLineHit(int line) {
    insideLineOn[line] = true;
    insideLineTimers[line] = pTimer->seconds() + 0.5f; // Longer duration for shimmer effect
}

void Borders::setOutsideLineHit(int line) {
    outsideLineOn[line] = true;
    outsideLineTimers[line] = pTimer->seconds() + 0.5f; // Longer duration for shimmer effect
}

void Borders::drawElectricShimmerLine(Line* line, double endTime) {
    double currentTime = pTimer->seconds();
    double timeRemaining = endTime - currentTime;
    double totalDuration = 0.5; // Total effect duration

    // Create pulsing intensity based on sine wave
    double pulseSpeed = 15.0;                                 // How fast the pulse oscillates
    double pulse = sin(currentTime * pulseSpeed) * 0.5 + 0.5; // 0 to 1

    // Create electric blue/cyan colors with pulsing intensity
    Color electricCore;
    electricCore.red = (int)(50 + pulse * 150);                      // 50-200
    electricCore.green = (int)(150 + pulse * 105);                   // 150-255
    electricCore.blue = 255;                                         // Always full blue
    electricCore.alpha = (int)(200 * timeRemaining / totalDuration); // Fade out over time

    Color electricGlow;
    electricGlow.red = (int)(100 + pulse * 100);  // 100-200
    electricGlow.green = (int)(200 + pulse * 55); // 200-255
    electricGlow.blue = 255;
    electricGlow.alpha = (int)(150 * timeRemaining / totalDuration); // Fade out over time

    // OPTIMIZED: Single layer electric effect for performance
    float thickness = 3.0f + (float)(pulse * 2.0f);      // 3-5 pixels
    float bloomIntensity = 0.4f + (float)(pulse * 0.2f); // 0.4-0.6 (reduced)

    // Draw single electric layer
    Window::DrawVolumetricLineWithBloom(line, electricCore, thickness, bloomIntensity);

    // Add random electric arcs along the line (sparks)
    if (pulse > 0.7) { // Only show sparks during peak intensity
        drawElectricArcs(line, electricCore);
    }
}

void Borders::drawElectricArcs(Line* mainLine, const Color& arcColor) {
    // Calculate line properties
    Vector2i lineVec = mainLine->end - mainLine->start;

    // Create 2-4 random arcs along the line
    int numArcs = Window::Random(2, 4);

    for (int i = 0; i < numArcs; i++) {
        // Random position along the main line (0.0 to 1.0)
        float t = Window::Random(20, 80) / 100.0f; // 0.2 to 0.8 to avoid endpoints

        // Calculate point on main line
        Vector2i arcStart;
        arcStart.x = mainLine->start.x + (int)(lineVec.x * t);
        arcStart.y = mainLine->start.y + (int)(lineVec.y * t);

        // Create perpendicular vector for arc direction
        Vector2i perpVec;
        perpVec.x = -lineVec.y; // Perpendicular
        perpVec.y = lineVec.x;

        // Normalize and scale the perpendicular vector
        float perpLength = sqrt((float)(perpVec.x * perpVec.x + perpVec.y * perpVec.y));
        if (perpLength > 0) {
            float arcLength = Window::Random(5, 15); // Random arc length
            perpVec.x = (int)(perpVec.x / perpLength * arcLength);
            perpVec.y = (int)(perpVec.y / perpLength * arcLength);

            // Random direction (up or down from line)
            if (Window::Random(0, 1)) {
                perpVec.x = -perpVec.x;
                perpVec.y = -perpVec.y;
            }

            // Create the arc line
            Line arcLine;
            arcLine.start = arcStart;
            arcLine.end = arcStart + perpVec;

            // Draw the electric arc
            Color sparkColor = arcColor;
            sparkColor.alpha = sparkColor.alpha / 2; // Dimmer than main line
            Window::DrawVolumetricLineWithBloom(&arcLine, sparkColor, 1.0f, 0.5f);
        }
    }
}

SDL_Rect Borders::getInsideBorder() {
    return insideBorder;
}

void Borders::drawBackground(const Vector2f& playerPosition) {
    drawGeometryWarsGrid(playerPosition);
}

void Borders::drawBackground(const std::vector<DistortionSource>& distortionSources) {
    drawGeometryWarsGrid(distortionSources);
}

void Borders::drawGeometryWarsGrid(const Vector2f& playerPosition) {
    Vector2i windowSize = Window::GetWindowSize();

    // Draw vertical grid lines
    for (int x = 0; x < windowSize.x + m_GridSpacing; x += m_GridSpacing) {
        for (int y = 0; y < windowSize.y; y += m_GridSpacing / 4) { // More segments for smoother curves
            if (y + m_GridSpacing / 4 >= windowSize.y)
                continue;

            // Calculate distorted positions
            float distortion1 = calculateGridDistortion((float)x, (float)y, playerPosition);
            float distortion2 = calculateGridDistortion((float)x, (float)(y + m_GridSpacing / 4), playerPosition);

            // Add very subtle wave animation
            float wave1 = sin(m_GridAnimationTime + x * 0.01f + y * 0.01f) * 0.1f; // Reduced from 0.3f to 0.1f
            float wave2 = sin(m_GridAnimationTime + x * 0.01f + (y + m_GridSpacing / 4) * 0.01f) * 0.1f;

            Line gridLine;
            gridLine.start = Vector2i(x + (int)(distortion1 + wave1), y);
            gridLine.end = Vector2i(x + (int)(distortion2 + wave2), y + m_GridSpacing / 4);

            // Fade lines at edges
            float edgeFade = 1.0f;
            if (x < m_GridSpacing * 2 || x > windowSize.x - m_GridSpacing * 2) {
                edgeFade = 0.3f;
            }

            Color fadeColor = m_GridColor;
            fadeColor.alpha = (int)(fadeColor.alpha * edgeFade);

            Window::DrawVolumetricLine(&gridLine, fadeColor);
        }
    }

    // Draw horizontal grid lines
    for (int y = 0; y < windowSize.y + m_GridSpacing; y += m_GridSpacing) {
        for (int x = 0; x < windowSize.x; x += m_GridSpacing / 4) { // More segments for smoother curves
            if (x + m_GridSpacing / 4 >= windowSize.x)
                continue;

            // Calculate distorted positions
            float distortion1 = calculateGridDistortion((float)x, (float)y, playerPosition);
            float distortion2 = calculateGridDistortion((float)(x + m_GridSpacing / 4), (float)y, playerPosition);

            // Add very subtle wave animation
            float wave1 = sin(m_GridAnimationTime + x * 0.01f + y * 0.01f) * 0.1f; // Reduced from 0.3f to 0.1f
            float wave2 = sin(m_GridAnimationTime + (x + m_GridSpacing / 4) * 0.01f + y * 0.01f) * 0.1f;

            Line gridLine;
            gridLine.start = Vector2i(x, y + (int)(distortion1 + wave1));
            gridLine.end = Vector2i(x + m_GridSpacing / 4, y + (int)(distortion2 + wave2));

            // Fade lines at edges
            float edgeFade = 1.0f;
            if (y < m_GridSpacing * 2 || y > windowSize.y - m_GridSpacing * 2) {
                edgeFade = 0.3f;
            }

            Color fadeColor = m_GridColor;
            fadeColor.alpha = (int)(fadeColor.alpha * edgeFade);

            Window::DrawVolumetricLine(&gridLine, fadeColor);
        }
    }
}

void Borders::drawGeometryWarsGrid(const std::vector<DistortionSource>& distortionSources) {
    Vector2i windowSize = Window::GetWindowSize();

    // Draw vertical grid lines
    for (int x = 0; x < windowSize.x + m_GridSpacing; x += m_GridSpacing) {
        for (int y = 0; y < windowSize.y; y += m_GridSpacing / 4) { // More segments for smoother curves
            if (y + m_GridSpacing / 4 >= windowSize.y)
                continue;

            // Calculate distorted positions from multiple sources
            float distortion1 = calculateGridDistortion((float)x, (float)y, distortionSources);
            float distortion2 = calculateGridDistortion((float)x, (float)(y + m_GridSpacing / 4), distortionSources);

            // Add very subtle wave animation
            float wave1 = sin(m_GridAnimationTime + x * 0.01f + y * 0.01f) * 0.1f; // Reduced from 0.3f to 0.1f
            float wave2 = sin(m_GridAnimationTime + x * 0.01f + (y + m_GridSpacing / 4) * 0.01f) * 0.1f;

            Line gridLine;
            gridLine.start = Vector2i(x + (int)(distortion1 + wave1), y);
            gridLine.end = Vector2i(x + (int)(distortion2 + wave2), y + m_GridSpacing / 4);

            // Fade lines at edges
            float edgeFade = 1.0f;
            if (x < m_GridSpacing * 2 || x > windowSize.x - m_GridSpacing * 2) {
                edgeFade = 0.3f;
            }

            Color fadeColor = m_GridColor;
            fadeColor.alpha = (int)(fadeColor.alpha * edgeFade);

            Window::DrawVolumetricLine(&gridLine, fadeColor);
        }
    }

    // Draw horizontal grid lines
    for (int y = 0; y < windowSize.y + m_GridSpacing; y += m_GridSpacing) {
        for (int x = 0; x < windowSize.x; x += m_GridSpacing / 4) { // More segments for smoother curves
            if (x + m_GridSpacing / 4 >= windowSize.x)
                continue;

            // Calculate distorted positions from multiple sources
            float distortion1 = calculateGridDistortion((float)x, (float)y, distortionSources);
            float distortion2 = calculateGridDistortion((float)(x + m_GridSpacing / 4), (float)y, distortionSources);

            // Add very subtle wave animation
            float wave1 = sin(m_GridAnimationTime + x * 0.01f + y * 0.01f) * 0.1f; // Reduced from 0.3f to 0.1f
            float wave2 = sin(m_GridAnimationTime + (x + m_GridSpacing / 4) * 0.01f + y * 0.01f) * 0.1f;

            Line gridLine;
            gridLine.start = Vector2i(x, y + (int)(distortion1 + wave1));
            gridLine.end = Vector2i(x + m_GridSpacing / 4, y + (int)(distortion2 + wave2));

            // Fade lines at edges
            float edgeFade = 1.0f;
            if (y < m_GridSpacing * 2 || y > windowSize.y - m_GridSpacing * 2) {
                edgeFade = 0.3f;
            }

            Color fadeColor = m_GridColor;
            fadeColor.alpha = (int)(fadeColor.alpha * edgeFade);

            Window::DrawVolumetricLine(&gridLine, fadeColor);
        }
    }
}

float Borders::calculateGridDistortion(float x, float y, const Vector2f& playerPos) {
    // Calculate distance from this grid point to player
    float dx = x - playerPos.x;
    float dy = y - playerPos.y;
    float distance = sqrt(dx * dx + dy * dy);

    if (distance > m_DistortionRadius) {
        return 0.0f; // No distortion beyond radius
    }

    // Create distortion that falls off with distance
    float distortionFactor = 1.0f - (distance / m_DistortionRadius);
    distortionFactor = distortionFactor * distortionFactor; // Smooth falloff

    // Calculate distortion direction (away from player)
    if (distance < 1.0f)
        distance = 1.0f; // Avoid division by zero
    float dirX = dx / distance;

    // Apply distortion
    return dirX * m_DistortionStrength * distortionFactor;
}

float Borders::calculateGridDistortion(float x, float y, const std::vector<DistortionSource>& sources) {
    float totalDistortion = 0.0f;

    for (const auto& source : sources) {
        // Calculate distance from this grid point to distortion source
        float dx = x - source.position.x;
        float dy = y - source.position.y;
        float distance = sqrt(dx * dx + dy * dy);

        // Use source-specific radius if provided, otherwise use default
        float radius = (source.radius > 0.0f) ? source.radius : m_DistortionRadius;

        if (distance > radius) {
            continue; // No distortion beyond radius for this source
        }

        // Create distortion that falls off with distance
        float distortionFactor = 1.0f - (distance / radius);
        distortionFactor = distortionFactor * distortionFactor; // Smooth falloff

        // Calculate distortion direction (away from source)
        if (distance < 1.0f)
            distance = 1.0f; // Avoid division by zero
        float dirX = dx / distance;

        // Apply distortion with source-specific strength
        float sourceStrength = m_DistortionStrength * source.strength;
        totalDistortion += dirX * sourceStrength * distortionFactor;
    }

    return totalDistortion;
}

void Borders::resetGridBackground() {
    // Reset grid animation time to eliminate any accumulated distortion effects
    m_GridAnimationTime = 0.0f;

    // Reset grid properties to initial state to clear any residual distortion
    m_DistortionRadius = 120.0f;  // Reset to default radius
    m_DistortionStrength = 15.0f; // Reset to default strength
}

} // namespace omegarace
