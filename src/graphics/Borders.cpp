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
    
    // Initialize central border electric effect
    centralBorderElectricOn = false;
    centralBorderTimer = 0.0f;

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
    
    // Update central border electric effect
    if (pTimer->seconds() > centralBorderTimer)
        centralBorderElectricOn = false;
}

void Borders::draw() {
    // Create four lines for the rectangle border
    Line borderLines[4];
    
    // Top line
    borderLines[0].start = Vector2i(insideBorder.x, insideBorder.y);
    borderLines[0].end = Vector2i(insideBorder.x + insideBorder.w, insideBorder.y);
    
    // Right line  
    borderLines[1].start = Vector2i(insideBorder.x + insideBorder.w, insideBorder.y);
    borderLines[1].end = Vector2i(insideBorder.x + insideBorder.w, insideBorder.y + insideBorder.h);
    
    // Bottom line
    borderLines[2].start = Vector2i(insideBorder.x + insideBorder.w, insideBorder.y + insideBorder.h);
    borderLines[2].end = Vector2i(insideBorder.x, insideBorder.y + insideBorder.h);
    
    // Left line
    borderLines[3].start = Vector2i(insideBorder.x, insideBorder.y + insideBorder.h);
    borderLines[3].end = Vector2i(insideBorder.x, insideBorder.y);
    
    // Draw central border - electric when hit, solid white otherwise
    if (centralBorderElectricOn) {
        // Electric effect when hit with quadruple thickness
        double currentTime = pTimer->seconds();
        double timeRemaining = centralBorderTimer - currentTime;
        Color borderElectricColor = {0, 200, 255, 255}; // Electric blue for central border
        
        for (int i = 0; i < 4; i++) {
            Window::DrawElectricBarrierLine(&borderLines[i], borderElectricColor, 
                                          10.0f, 12.0f, (float)timeRemaining); // Quadrupled thickness from 3.0f to 12.0f
        }
    } else {
        // Normal solid white lines
        Color whiteColor = {255, 255, 255, 255};
        for (int i = 0; i < 4; i++) {
            Window::DrawLine(&borderLines[i], whiteColor);
        }
    }

    for (int dot = 0; dot < 8; dot++) {
        Window::DrawPoint(&insideDots[dot], lineColor);
    }

    // Draw inside field lines with shader-based electric barrier effect
    for (int line = 0; line < 4; line++) {
        if (insideLineOn[line]) {
            // Calculate time remaining for fade effect
            double currentTime = pTimer->seconds();
            double timeRemaining = insideLineTimers[line] - currentTime;
            
            // Use new shader-based electric barrier line with even thicker lines when hit
            Color electricColor = {0, 255, 255, 255}; // Electric cyan
            Window::DrawElectricBarrierLine(&insideFieldLines[line], electricColor, 
                                          15.0f, 12.0f, (float)timeRemaining); // Increased thickness from 8.0f to 12.0f
        }
    }

    // Draw outside field lines with shader-based electric barrier effect
    for (int line = 0; line < 8; line++) {
        if (outsideLineOn[line]) {
            // Calculate time remaining for fade effect
            double currentTime = pTimer->seconds();
            double timeRemaining = outsideLineTimers[line] - currentTime;
            
            // Use new shader-based electric barrier line with even thicker lines when hit
            Color electricColor = {0, 255, 255, 255}; // Electric cyan
            Window::DrawElectricBarrierLine(&outsideFieldLines[line], electricColor, 
                                          15.0f, 12.0f, (float)timeRemaining); // Increased thickness from 8.0f to 12.0f
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

void Borders::setCentralBorderHit() {
    centralBorderElectricOn = true;
    centralBorderTimer = pTimer->seconds() + 0.5f; // Same duration as field lines
}

SDL_Rect Borders::getInsideBorder() {
    return insideBorder;
}

void Borders::resetGridBackground() {
    // Reset grid animation time to eliminate any accumulated distortion effects
    m_GridAnimationTime = 0.0f;

    // Reset grid properties to initial state to clear any residual distortion
    m_DistortionRadius = 120.0f;  // Reset to default radius
    m_DistortionStrength = 15.0f; // Reset to default strength
    
    // Reset the actual shader distortion effect
    Window::ResetGridDistortion();
}

} // namespace omegarace
