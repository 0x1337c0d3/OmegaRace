#include "UFO.h"
#include "Window.h"
#include <ctime>

namespace omegarace {

UFO::UFO()
    : radius(25.0f), destroyed(false), active(false), width(40.0f), directionTimer(0.0f), directionDuration(3.0f),
      fromLeft(true), explosionActive(false), explosionTimer(0.0f), explosionDuration(3.0f) {
    color.red = 0;
    color.green = 255;
    color.blue = 255;
    color.alpha = 255;
    buildUFO();

    // Initialize explosion particles
    for (int i = 0; i < 24; i++) {
        explosionParticles[i].active = false;
    }
}

void UFO::buildUFO() {
    // Build modern sleek UFO shape - 16 lines forming a sophisticated craft
    float w = width;

    // Top dome section (3 levels for depth)
    ufoLines[0][0] = omegarace::Vector2i(-(int)(w / 8), -(int)(w / 3));
    ufoLines[0][1] = omegarace::Vector2i((int)(w / 8), -(int)(w / 3));

    ufoLines[1][0] = omegarace::Vector2i(-(int)(w / 5), -(int)(w / 4));
    ufoLines[1][1] = omegarace::Vector2i((int)(w / 5), -(int)(w / 4));

    ufoLines[2][0] = omegarace::Vector2i(-(int)(w / 3), -(int)(w / 6));
    ufoLines[2][1] = omegarace::Vector2i((int)(w / 3), -(int)(w / 6));

    // Main hull section (widest part)
    ufoLines[3][0] = omegarace::Vector2i(-(int)(w / 2), 0);
    ufoLines[3][1] = omegarace::Vector2i((int)(w / 2), 0);

    // Lower hull sections (2 levels for smooth curve)
    ufoLines[4][0] = omegarace::Vector2i(-(int)(w / 2.5), (int)(w / 8));
    ufoLines[4][1] = omegarace::Vector2i((int)(w / 2.5), (int)(w / 8));

    ufoLines[5][0] = omegarace::Vector2i(-(int)(w / 4), (int)(w / 5));
    ufoLines[5][1] = omegarace::Vector2i((int)(w / 4), (int)(w / 5));

    // Dome connecting lines (left side)
    ufoLines[6][0] = ufoLines[0][0]; // Top small to medium
    ufoLines[6][1] = ufoLines[1][0];

    ufoLines[7][0] = ufoLines[1][0]; // Medium to large dome
    ufoLines[7][1] = ufoLines[2][0];

    ufoLines[8][0] = ufoLines[2][0]; // Large dome to main hull
    ufoLines[8][1] = ufoLines[3][0];

    // Dome connecting lines (right side)
    ufoLines[9][0] = ufoLines[0][1]; // Top small to medium
    ufoLines[9][1] = ufoLines[1][1];

    ufoLines[10][0] = ufoLines[1][1]; // Medium to large dome
    ufoLines[10][1] = ufoLines[2][1];

    ufoLines[11][0] = ufoLines[2][1]; // Large dome to main hull
    ufoLines[11][1] = ufoLines[3][1];

    // Hull tapering lines (left side)
    ufoLines[12][0] = ufoLines[3][0]; // Main hull to lower
    ufoLines[12][1] = ufoLines[4][0];

    ufoLines[13][0] = ufoLines[4][0]; // Lower to bottom
    ufoLines[13][1] = ufoLines[5][0];

    // Hull tapering lines (right side)
    ufoLines[14][0] = ufoLines[3][1]; // Main hull to lower
    ufoLines[14][1] = ufoLines[4][1];

    ufoLines[15][0] = ufoLines[4][1]; // Lower to bottom
    ufoLines[15][1] = ufoLines[5][1];
}

void UFO::activate(omegarace::Vector2f pos, bool startFromLeft) {
    position = pos;
    fromLeft = startFromLeft;
    destroyed = false;
    active = true;
    directionTimer = 0.0f;

    // Set initial velocity based on starting side
    if (fromLeft) {
        velocity.x = 60.0f + (rand() % 40);  // 60-100 pixels per second rightward
        velocity.y = -20.0f + (rand() % 40); // -20 to +20 vertical movement
    } else {
        velocity.x = -60.0f - (rand() % 40); // -60 to -100 pixels per second leftward
        velocity.y = -20.0f + (rand() % 40); // -20 to +20 vertical movement
    }

    buildUFO();
}

void UFO::update(double frame) {
    // Always update explosion if active, even if UFO is not active
    if (explosionActive) {
        updateExplosion(frame);
    }

    if (!active || destroyed)
        return;

    // Update position
    position.x += velocity.x * frame;
    position.y += velocity.y * frame;

    // Update direction change timer
    directionTimer += frame;
    if (directionTimer >= directionDuration) {
        changeDirection();
        directionTimer = 0.0f;
        directionDuration = 2.0f + ((rand() % 200) * 0.01f); // 2-4 second intervals
    }

    // Deactivate if UFO moves off screen horizontally
    if ((fromLeft && position.x > omegarace::Window::GetWindowSize().x + 50) || (!fromLeft && position.x < -50)) {
        active = false;
    }

    // Keep UFO within vertical bounds
    if (position.y < 50)
        position.y = 50;
    if (position.y > omegarace::Window::GetWindowSize().y - 50) {
        position.y = omegarace::Window::GetWindowSize().y - 50;
    }
}

void UFO::changeDirection() {
    if (!active)
        return;

    // Random direction change
    float angle;
    if (fromLeft) {
        angle = 0.1f + ((rand() % 200) * 0.01f); // Slight upward or downward angle
    } else {
        angle = 3.14f - 0.1f - ((rand() % 200) * 0.01f); // Slight upward or downward angle
    }

    float speed = 60.0f + (rand() % 40); // 60-100 speed
    velocity.x = cos(angle) * speed;
    velocity.y = sin(angle) * speed;
}

void UFO::draw() {
    // Draw UFO only if it's active and not destroyed
    if (active && !destroyed) {
        Line ufoLine;
        for (int line = 0; line < 16; line++) {
            ufoLine.start = Vector2i((int)(position.x + ufoLines[line][0].x), (int)(position.y + ufoLines[line][0].y));
            ufoLine.end = Vector2i((int)(position.x + ufoLines[line][1].x), (int)(position.y + ufoLines[line][1].y));
            Window::DrawLine(&ufoLine, color);
        }
    }

    // Always draw explosion if active (regardless of UFO state)
    if (explosionActive) {
        drawExplosion();
    }
}

void UFO::triggerExplosion() {
    if (explosionActive)
        return; // Already exploding

    // Immediately set UFO as destroyed and inactive for instant visual feedback
    destroyed = true;
    active = false;

    explosionActive = true;
    explosionTimer = 0.0f;
    explosionDuration = 3.0f; // Longer than enemy explosions

    // Create spectacular UFO explosion with 24 particles
    for (int i = 0; i < 24; i++) {
        UFOExplosionParticle& particle = explosionParticles[i];
        particle.active = true;
        particle.position = position;

        // Create expanding ring pattern with some randomness
        float angle = (i / 24.0f) * 6.28318f + ((rand() % 100) * 0.01f); // 2*PI with random offset
        float speed = 80.0f + (rand() % 120);                            // 80-200 pixels per second

        particle.velocity.x = cos(angle) * speed;
        particle.velocity.y = sin(angle) * speed;

        particle.maxLife = 2.5f + ((rand() % 100) * 0.01f); // 2.5-3.5 seconds
        particle.life = particle.maxLife;
        particle.size = 3.0f + ((rand() % 100) * 0.02f); // 3-5 pixel size

        // Start with bright cyan/white (UFO color theme)
        particle.color.red = 255;
        particle.color.green = 255;
        particle.color.blue = 255;
        particle.color.alpha = 255;
    }
}

void UFO::updateExplosion(double frame) {
    if (!explosionActive)
        return;

    explosionTimer += frame;

    bool anyParticleActive = false;
    for (int i = 0; i < 24; i++) {
        UFOExplosionParticle& particle = explosionParticles[i];
        if (!particle.active)
            continue;

        // Update particle position
        particle.position.x += particle.velocity.x * frame;
        particle.position.y += particle.velocity.y * frame;

        // Update particle life
        particle.life -= frame;
        if (particle.life <= 0) {
            particle.active = false;
            continue;
        }

        anyParticleActive = true;

        // Calculate life ratio for color transitions
        float lifeRatio = particle.life / particle.maxLife;

        // UFO-specific color transitions: White -> Cyan -> Blue -> Purple -> Dark
        if (lifeRatio > 0.8f) {
            // Bright white flash (20% of life)
            particle.color.red = 255;
            particle.color.green = 255;
            particle.color.blue = 255;
        } else if (lifeRatio > 0.6f) {
            // Transition to bright cyan (20% of life)
            float transition = (lifeRatio - 0.6f) / 0.2f;
            particle.color.red = (int)(255 * transition);
            particle.color.green = 255;
            particle.color.blue = 255;
        } else if (lifeRatio > 0.4f) {
            // Cyan to blue transition (20% of life)
            float transition = (lifeRatio - 0.4f) / 0.2f;
            particle.color.red = 0;
            particle.color.green = (int)(255 * transition);
            particle.color.blue = 255;
        } else if (lifeRatio > 0.2f) {
            // Blue to purple transition (20% of life)
            float transition = (lifeRatio - 0.2f) / 0.2f;
            particle.color.red = (int)(128 * (1.0f - transition));
            particle.color.green = 0;
            particle.color.blue = 255;
        } else {
            // Purple fade to dark (20% of life)
            float transition = lifeRatio / 0.2f;
            particle.color.red = (int)(128 * transition);
            particle.color.green = 0;
            particle.color.blue = (int)(255 * transition);
        }

        // Alpha fade based on life - use linear fade instead of quadratic for better visibility
        particle.color.alpha = (int)(255 * lifeRatio); // Linear fade instead of quadratic
    }

    // Deactivate explosion when no particles are active
    if (!anyParticleActive) {
        explosionActive = false;
    }
}

void UFO::drawExplosion() {
    if (!explosionActive)
        return;

    for (int i = 0; i < 24; i++) {
        UFOExplosionParticle& particle = explosionParticles[i];
        if (!particle.active)
            continue;

        // Draw particle as a small circle with bloom effect
        Vector2i particlePos((int)particle.position.x, (int)particle.position.y);

        // Draw core particle
        Line particleLine;
        particleLine.start = particlePos;
        particleLine.end = Vector2i(particlePos.x + (int)particle.size, particlePos.y);
        Window::DrawLine(&particleLine, particle.color);

        // Draw bloom effect with multiple lines at reduced alpha
        Color bloomColor = particle.color;
        bloomColor.alpha /= 3; // Reduce alpha for bloom

        // Horizontal bloom
        Line bloomLineH;
        bloomLineH.start = Vector2i(particlePos.x - 1, particlePos.y);
        bloomLineH.end = Vector2i(particlePos.x + (int)particle.size + 1, particlePos.y);
        Window::DrawLine(&bloomLineH, bloomColor);

        // Vertical bloom
        Line bloomLineV;
        bloomLineV.start = Vector2i(particlePos.x, particlePos.y - 1);
        bloomLineV.end = Vector2i(particlePos.x, particlePos.y + 1);
        Window::DrawLine(&bloomLineV, bloomColor);
    }
}

void UFO::forceResetExplosion() {
    // Immediately reset explosion state
    explosionActive = false;
    explosionTimer = 0.0f;

    // Deactivate all explosion particles
    for (int i = 0; i < 24; i++) {
        explosionParticles[i].active = false;
        explosionParticles[i].life = 0.0f;
    }
}

} // namespace omegarace
