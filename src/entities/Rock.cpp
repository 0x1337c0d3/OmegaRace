#include "Rock.h"

namespace omegarace {

Rock::Rock(std::mt19937& random) : Entity() {
    // Note: Using FMOD audio system instead of Mix_Chunk

    // Rock Color.
    m_Color.red = 255;
    m_Color.green = 255;
    m_Color.blue = 255;
    m_Color.alpha = 255;

    m_RockLow = 8;
    m_RockMed = 12;
    m_RockHigh = 16;
    m_RockVarienceLow = 4;
    m_RockVarienceMed = 6;
    m_RockVarienceHigh = 8;
    m_Radius = 20.0f;

    Initialize(random);
}

void Rock::Initialize(std::mt19937& random) {
    m_Active = false;
    m_Distroyed = false;
    m_MaxVelocity = 50; // Set a reasonable velocity for rocks
    // Don't reset rock size parameters - they're set in constructor
    // m_RockLow, m_RockMed, m_RockHigh should keep their constructor values
    // m_RockVarienceHigh, m_RockVarienceMed, m_RockVarienceLow should keep their constructor values
    // Instance explosion.
    pExplosion = new Explosion();
    
    destroyed = false;
    active = false;
    m_DustActive = false;
    m_DustTimer = 0.0f;
    m_DustDuration = 1.5f;
    
    // Initialize dust particles
    for (int i = 0; i < 12; i++) {
        m_DustParticles[i].active = false;
    }
}

void Rock::PauseTimer() {
    pExplosion->pauseTimer();
}

void Rock::UnpauseTimer() {
    pExplosion->unpauseTimer();
}

Rock::~Rock() {
    delete pExplosion;
}

// Private methods ----------------------------------------------------------------
void Rock::BuildRock() {
    m_RockPoints[0].x = m_RockLow + Window::Random(0, m_RockVarienceLow);
    m_RockPoints[0].y = -m_RockMed - Window::Random(0, m_RockVarienceMed);

    m_RockPoints[1].x = m_RockMed + Window::Random(0, m_RockVarienceMed);
    m_RockPoints[1].y = -m_RockHigh - Window::Random(0, m_RockVarienceHigh);

    m_RockPoints[2].x = m_RockHigh + Window::Random(0, m_RockVarienceHigh);
    m_RockPoints[2].y = -m_RockMed - Window::Random(0, m_RockVarienceMed);

    m_RockPoints[3].x = m_RockMed + Window::Random(0, m_RockVarienceMed);
    m_RockPoints[3].y = m_RockLow + Window::Random(0, m_RockVarienceLow);

    m_RockPoints[4].x = m_RockHigh + Window::Random(0, m_RockVarienceHigh);
    m_RockPoints[4].y = m_RockMed + Window::Random(0, m_RockVarienceMed);

    m_RockPoints[5].x = m_RockMed + Window::Random(0, m_RockVarienceMed);
    m_RockPoints[5].y = m_RockHigh + Window::Random(0, m_RockVarienceHigh);

    m_RockPoints[6].x = m_RockLow + Window::Random(0, m_RockVarienceLow);
    m_RockPoints[6].y = m_RockMed + Window::Random(0, m_RockVarienceMed);

    m_RockPoints[7].x = -m_RockMed - Window::Random(0, m_RockVarienceMed);
    m_RockPoints[7].y = m_RockHigh + Window::Random(0, m_RockVarienceHigh);

    m_RockPoints[8].x = -m_RockHigh - Window::Random(0, m_RockVarienceHigh);
    m_RockPoints[8].y = m_RockMed + Window::Random(0, m_RockVarienceHigh);

    m_RockPoints[9].x = -m_RockMed - Window::Random(0, m_RockVarienceMed);
    m_RockPoints[9].y = m_RockLow + Window::Random(0, m_RockVarienceLow);

    m_RockPoints[10].x = -m_RockHigh - Window::Random(0, m_RockVarienceHigh);
    m_RockPoints[10].y = -m_RockMed - Window::Random(0, m_RockVarienceMed);

    m_RockPoints[11].x = -m_RockMed - Window::Random(0, m_RockVarienceMed);
    m_RockPoints[11].y = -m_RockHigh - Window::Random(0, m_RockVarienceHigh);
}

void Rock::activate(Vector2f pos, Vector2f vel) {
    position = pos;
    velocity = vel;
    m_Location = pos;
    m_Velocity = vel;
    active = true;
    m_Active = true;
    destroyed = false;
    m_Distroyed = false;
    BuildRock();
}

void Rock::update(double frame) {
    pExplosion->update(frame);

    if (m_Active) {
        updateFrame(frame);
    }
    
    // Always update dust explosion if active (even if rock is destroyed)
    updateDustExplosion(frame);
    
    if (m_Active && !m_Distroyed) {
        position.x += velocity.x * (float)frame;
        position.y += velocity.y * (float)frame;
        m_Location = position;
        
        // Check for edge collisions and bounce
        checkForEdge();
        
        m_Location = position;
    }
}

void Rock::draw() {
    // Draw rock only if it's active and not destroyed
    if (m_Active && !m_Distroyed) {
        Line rockLine;
        for (int point = 0; point < 11; point++) {
            rockLine.start = Vector2i((int)(position.x + m_RockPoints[point].x), 
                                     (int)(position.y + m_RockPoints[point].y));
            rockLine.end = Vector2i((int)(position.x + m_RockPoints[point + 1].x), 
                                   (int)(position.y + m_RockPoints[point + 1].y));
            Window::DrawLine(&rockLine, m_Color);
        }

        rockLine.start = Vector2i((int)(position.x + m_RockPoints[11].x), 
                                 (int)(position.y + m_RockPoints[11].y));
        rockLine.end = Vector2i((int)(position.x + m_RockPoints[0].x), 
                               (int)(position.y + m_RockPoints[0].y));
        Window::DrawLine(&rockLine, m_Color);
    }
    
    // Always draw dust explosion if active (regardless of rock state)
    if (m_DustActive) {
        drawDustExplosion();
    }
}

void Rock::setDestroyed(bool d) {
    destroyed = d;
    m_Distroyed = d;
}

void Rock::triggerDustExplosion() {
    if (m_DustActive) return; // Already exploding
    
    m_DustActive = true;
    m_DustTimer = 0.0f;
    m_DustDuration = 1.5f; // Shorter than UFO explosions
    
    // Create dust cloud with 12 particles
    for (int i = 0; i < 12; i++) {
        RockDustParticle& particle = m_DustParticles[i];
        particle.active = true;
        particle.position = position;
        
        // Create scattered pattern with lower velocities for dust effect
        float angle = (i / 12.0f) * 6.28318f + ((rand() % 100) * 0.02f); // 2*PI with random offset
        float speed = 20.0f + (rand() % 40); // 20-60 pixels per second (slower than UFO)
        
        particle.velocity.x = cos(angle) * speed;
        particle.velocity.y = sin(angle) * speed;
        
        particle.maxLife = 1.0f + ((rand() % 100) * 0.01f); // 1-2 seconds
        particle.life = particle.maxLife;
        particle.size = 1.0f + ((rand() % 100) * 0.01f); // 1-2 pixel size (smaller than UFO)
        
        // Start with gray/white dust colors
        particle.color.red = 180 + (rand() % 75);   // 180-255
        particle.color.green = 180 + (rand() % 75); // 180-255  
        particle.color.blue = 180 + (rand() % 75);  // 180-255
        particle.color.alpha = 255;
    }
}

void Rock::updateDustExplosion(double frame) {
    if (!m_DustActive) return;
    
    m_DustTimer += frame;
    if (m_DustTimer > m_DustDuration) {
        active = false;
    }
    
    bool anyParticleActive = false;
    for (int i = 0; i < 12; i++) {
        RockDustParticle& particle = m_DustParticles[i];
        if (!particle.active) continue;
        
        // Update particle position with slight gravity effect
        particle.position.x += particle.velocity.x * frame;
        particle.position.y += particle.velocity.y * frame;
        particle.velocity.y += 30.0f * frame; // Slight downward drift for dust settling
        
        // Update particle life
        particle.life -= frame;
        if (particle.life <= 0) {
            particle.active = false;
            continue;
        }
        
        anyParticleActive = true;
        
        // Calculate life ratio for fading
        float lifeRatio = particle.life / particle.maxLife;
        
        // Simple dust color transition: Stay grayish but fade to darker
        if (lifeRatio > 0.5f) {
            // Stay bright dust color
            particle.color.red = 180 + (int)(75 * lifeRatio);
            particle.color.green = 180 + (int)(75 * lifeRatio);
            particle.color.blue = 180 + (int)(75 * lifeRatio);
        } else {
            // Fade to darker gray
            float fadeRatio = lifeRatio / 0.5f;
            particle.color.red = (int)(180 * fadeRatio);
            particle.color.green = (int)(180 * fadeRatio);
            particle.color.blue = (int)(180 * fadeRatio);
        }
        
        // Alpha fade based on life
        particle.color.alpha = (int)(255 * lifeRatio);
    }
    
    // Deactivate explosion when no particles are active
    if (!anyParticleActive) {
        m_DustActive = false;
    }
}

void Rock::drawDustExplosion() {
    if (!m_DustActive) return;
    
    for (int i = 0; i < 12; i++) {
        RockDustParticle& particle = m_DustParticles[i];
        if (!particle.active) continue;
        
        // Draw particle as a small dot/line
        Vector2i particlePos((int)particle.position.x, (int)particle.position.y);
        
        // Draw core dust particle as a small line
        Line dustLine;
        dustLine.start = particlePos;
        dustLine.end = Vector2i(particlePos.x + (int)particle.size, particlePos.y);
        Window::DrawLine(&dustLine, particle.color);
        
        // Add a small cross pattern for more dust-like appearance
        if (particle.color.alpha > 50) {
            Color fadedColor = particle.color;
            fadedColor.alpha = particle.color.alpha / 2;
            
            // Vertical line
            dustLine.start = Vector2i(particlePos.x, particlePos.y - 1);
            dustLine.end = Vector2i(particlePos.x, particlePos.y + 1);
            Window::DrawLine(&dustLine, fadedColor);
            
            // Horizontal line
            dustLine.start = Vector2i(particlePos.x - 1, particlePos.y);
            dustLine.end = Vector2i(particlePos.x + 1, particlePos.y);
            Window::DrawLine(&dustLine, fadedColor);
        }
    }
}

void Rock::checkForEdge() {
    // Use radius for edge checking and respect both outside border boundaries AND inside StatusDisplay border
    float rockRadius = m_Radius;
    int borderOffset = 5; // Outside border offset
    
    // Calculate StatusDisplay (inside) border dimensions - same as in Borders.cpp
    int InsideWidth = Window::GetWindowSize().x * 0.666;
    int InsideHeight = Window::GetWindowSize().y * 0.25;
    int InsideLeft = Window::GetWindowSize().x / 2 - InsideWidth / 2;
    int InsideRight = Window::GetWindowSize().x / 2 + InsideWidth / 2;
    int InsideTop = Window::GetWindowSize().y / 2 - InsideHeight / 2;
    int InsideBottom = Window::GetWindowSize().y / 2 + InsideHeight / 2;
    
    // Check X edges - both outside borders AND inside StatusDisplay border
    if (m_Location.x - rockRadius < borderOffset || m_Location.x + rockRadius > Window::GetWindowSize().x - borderOffset ||
        (m_Location.x + rockRadius > InsideLeft && m_Location.x - rockRadius < InsideRight && 
         m_Location.y + rockRadius > InsideTop && m_Location.y - rockRadius < InsideBottom)) {
        
        // Only bounce if hitting outside borders or StatusDisplay border
        bool hitOutsideBorder = (m_Location.x - rockRadius < borderOffset || m_Location.x + rockRadius > Window::GetWindowSize().x - borderOffset);
        bool hitStatusDisplay = (m_Location.x + rockRadius > InsideLeft && m_Location.x - rockRadius < InsideRight && 
                               m_Location.y + rockRadius > InsideTop && m_Location.y - rockRadius < InsideBottom);
        
        if (hitOutsideBorder || hitStatusDisplay) {
            bounceX();
            
            // Adjust position to keep rock in bounds
            if (m_Location.x - rockRadius < borderOffset) {
                m_Location.x = borderOffset + rockRadius + 2;
            }
            if (m_Location.x + rockRadius > Window::GetWindowSize().x - borderOffset) {
                m_Location.x = Window::GetWindowSize().x - borderOffset - rockRadius - 2;
            }
            
            // Handle StatusDisplay collisions
            if (hitStatusDisplay) {
                if (m_Location.x < (InsideLeft + InsideRight) / 2) {
                    // Hit left side of StatusDisplay
                    m_Location.x = InsideLeft - rockRadius - 2;
                } else {
                    // Hit right side of StatusDisplay
                    m_Location.x = InsideRight + rockRadius + 2;
                }
            }
            
            position.x = m_Location.x;
            velocity.x = m_Velocity.x;
        }
    }
    
    // Check Y edges - both outside borders AND inside StatusDisplay border
    if (m_Location.y - rockRadius < borderOffset || m_Location.y + rockRadius > Window::GetWindowSize().y - borderOffset ||
        (m_Location.x + rockRadius > InsideLeft && m_Location.x - rockRadius < InsideRight && 
         m_Location.y + rockRadius > InsideTop && m_Location.y - rockRadius < InsideBottom)) {
        
        // Only bounce if hitting outside borders or StatusDisplay border
        bool hitOutsideBorder = (m_Location.y - rockRadius < borderOffset || m_Location.y + rockRadius > Window::GetWindowSize().y - borderOffset);
        bool hitStatusDisplay = (m_Location.x + rockRadius > InsideLeft && m_Location.x - rockRadius < InsideRight && 
                               m_Location.y + rockRadius > InsideTop && m_Location.y - rockRadius < InsideBottom);
        
        if (hitOutsideBorder || hitStatusDisplay) {
            bounceY();
            
            // Adjust position to keep rock in bounds
            if (m_Location.y - rockRadius < borderOffset) {
                m_Location.y = borderOffset + rockRadius + 2;
            }
            if (m_Location.y + rockRadius > Window::GetWindowSize().y - borderOffset) {
                m_Location.y = Window::GetWindowSize().y - borderOffset - rockRadius - 2;
            }
            
            // Handle StatusDisplay collisions
            if (hitStatusDisplay) {
                if (m_Location.y < (InsideTop + InsideBottom) / 2) {
                    // Hit top side of StatusDisplay
                    m_Location.y = InsideTop - rockRadius - 2;
                } else {
                    // Hit bottom side of StatusDisplay
                    m_Location.y = InsideBottom + rockRadius + 2;
                }
            }
            
            position.y = m_Location.y;
            velocity.y = m_Velocity.y;
        }
    }
}

void Rock::forceResetDustExplosion() {
    // Immediately reset dust explosion state
    m_DustActive = false;
    m_DustTimer = 0.0f;
    
    // Deactivate all dust particles
    for (int i = 0; i < 12; i++) {
        m_DustParticles[i].active = false;
        m_DustParticles[i].life = 0.0f;
    }
}

} // namespace omegarace
