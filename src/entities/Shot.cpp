#include "Shot.h"
#include "Window.h"

namespace omegarace {

Shot::Shot() {
    m_Active = false;
    m_Speed = 250;
    m_ShotTimer = 0;
    m_Angle = 0;
    m_ShotTimeLimit = 1.6;
    m_Radius = 2;
    m_Rectangle.w = 2;
    m_Rectangle.h = 2;

    pTimer = std::make_unique<Timer>();
    pTimer->start();

    // Bright cyan/blue laser core
    m_InsideColor.red = 100;
    m_InsideColor.green = 200;
    m_InsideColor.blue = 255;
    m_InsideColor.alpha = 255;

    // Slightly dimmer outer glow
    m_OutsideColor.red = 50;
    m_OutsideColor.green = 150;
    m_OutsideColor.blue = 255;
    m_OutsideColor.alpha = 180;
}

Shot::~Shot() {
}

void Shot::update(double Frame) {
    if (m_Active) {
        updateFrame(Frame);

        if (pTimer->seconds() > m_ShotTimer)
            m_Active = false;

        checkForEdge();
    } else {
        for (int line = 0; line < 4; line++)
            m_InsideLines[line] = false;

        for (int line = 0; line < 8; line++)
            m_OutsideLines[line] = false;
    }
}

void Shot::draw() {
    if (m_Active) {
        // Create a laser bolt with trail effect
        Vector2f currentPos = m_Location;
        Vector2f previousPos = m_Location - (m_Velocity * 0.1f); // Trail based on velocity

        // Create a line from previous position to current position for motion blur
        Line trailLine;
        trailLine.start = Vector2i((int)previousPos.x, (int)previousPos.y);
        trailLine.end = Vector2i((int)currentPos.x, (int)currentPos.y);

        // Draw the main laser bolt with volumetric effect
        Color laserCore = m_InsideColor;
        laserCore.alpha = 255;

        // Draw trail with decreasing alpha
        Color trailColor = m_InsideColor;
        trailColor.alpha = 128;

        // Use volumetric line for the trail
        Window::DrawVolumetricLineWithBloom(&trailLine, trailColor, 2.0f, 0.6f);

        // Draw the bright core
        Vector2i coreLocation = Vector2i((int)currentPos.x, (int)currentPos.y);
        Window::DrawPoint(&coreLocation, laserCore);

        // Add a small cross pattern for the core
        Vector2i crossPoint;
        crossPoint = coreLocation + Vector2i(1, 0);
        Window::DrawPoint(&crossPoint, laserCore);
        crossPoint = coreLocation + Vector2i(-1, 0);
        Window::DrawPoint(&crossPoint, laserCore);
        crossPoint = coreLocation + Vector2i(0, 1);
        Window::DrawPoint(&crossPoint, laserCore);
        crossPoint = coreLocation + Vector2i(0, -1);
        Window::DrawPoint(&crossPoint, laserCore);
    }
}

void Shot::activate(const Vector2f& location, float angle) {
    m_Active = true;
    m_Angle = angle;
    m_Location = location;

    float sinRot = sin(m_Angle);
    float cosRot = cos(m_Angle);

    m_Location.x += cosRot * 15;
    m_Location.y += sinRot * 15;

    m_Velocity.x = cosRot * m_Speed;
    m_Velocity.y = sinRot * m_Speed;

    for (int line = 0; line < 4; line++)
        m_InsideLines[line] = false;

    for (int line = 0; line < 8; line++)
        m_OutsideLines[line] = false;

    m_ShotTimer = m_ShotTimeLimit + pTimer->seconds();
}

void Shot::timerPause() {
    pTimer->pause();
}

void Shot::timerUnpause() {
    pTimer->unpause();
}

void Shot::setInsideBorder(const SDL_Rect& border) {
    m_InsideBorder = border;
}

bool Shot::getInsideLine(int line) {
    return m_InsideLines[line];
}

bool Shot::getOutsideLine(int line) {
    return m_OutsideLines[line];
}

void Shot::setInsideLine(int line) {
    m_InsideLines[line] = false;
}

void Shot::checkForEdge() {
    if (checkForXEdge()) {
        m_Active = false;

        if (m_Location.x > Window::GetWindowSize().x / 2) {
            m_Location.x -= 2;

            if (m_Location.y > Window::GetWindowSize().y / 2)
                m_OutsideLines[7] = true;
            else
                m_OutsideLines[6] = true;
        } else {
            m_Location.x += 2;

            if (m_Location.y > Window::GetWindowSize().y / 2)
                m_OutsideLines[5] = true;
            else
                m_OutsideLines[4] = true;
        }
    }

    if (checkForYEdge()) {
        m_Active = false;

        if (m_Location.y > Window::GetWindowSize().y / 2) {
            m_Location.y -= 2;

            if (m_Location.x > Window::GetWindowSize().x / 2)
                m_OutsideLines[3] = true;
            else
                m_OutsideLines[2] = true;
        } else {
            m_Location.y += 2;

            if (m_Location.x > Window::GetWindowSize().x / 2)
                m_OutsideLines[1] = true;
            else
                m_OutsideLines[0] = true;
        }
    }

    if (rectangleIntersect(m_InsideBorder)) {
        m_Active = false;

        if (valueInRange(m_Location.x, m_InsideBorder.x + 5, m_InsideBorder.x + m_InsideBorder.w - 5)) {
            if (m_Location.y > Window::GetWindowSize().y / 2) {
                m_InsideLines[2] = true;
            } else {
                m_InsideLines[0] = true;
            }
        }

        if (valueInRange(m_Location.y, m_InsideBorder.y + 5, m_InsideBorder.y + m_InsideBorder.h - 5)) {
            if (m_Location.x > Window::GetWindowSize().x / 2) {
                m_InsideLines[1] = true;
            } else {
                m_InsideLines[3] = true;
            }
        }
    }
}

} // namespace omegarace
