#ifndef UFO_H
#define UFO_H

#include "Common.h"

namespace omegarace {

class UFO : public Common {
  public:
    UFO();
    ~UFO() = default;

    // Core UFO methods
    void activate(omegarace::Vector2f pos, bool startFromLeft);
    void update(double frame);
    void draw();
    void changeDirection();

    // Explosion system
    void triggerExplosion();
    void updateExplosion(double frame);
    void drawExplosion();
    void forceResetExplosion(); // NEW: Force immediate explosion reset

    // State getters/setters
    bool isDestroyed() const {
        return destroyed;
    }
    void setDestroyed(bool d) {
        destroyed = d;
    }
    omegarace::Vector2f getLocation() const {
        return position;
    }
    float getRadius() const {
        return radius;
    }
    bool isActive() const {
        return active;
    }
    void setActive(bool a) {
        active = a;
    }
    bool isExplosionActive() const {
        return explosionActive;
    }

  private:
    void buildUFO();

    // UFO properties
    omegarace::Vector2f position;
    omegarace::Vector2f velocity;
    float radius;
    bool destroyed;
    bool active;
    omegarace::Vector2i ufoLines[16][2]; // Start and end points for 16 lines (modern design)
    omegarace::Color color;
    float width;
    float directionTimer;
    float directionDuration;
    bool fromLeft;

    // UFO Explosion system
    struct UFOExplosionParticle {
        omegarace::Vector2f position;
        omegarace::Vector2f velocity;
        omegarace::Color color;
        float life;
        float maxLife;
        float size;
        bool active;
    };
    UFOExplosionParticle explosionParticles[24]; // More particles than enemies
    bool explosionActive;
    float explosionTimer;
    float explosionDuration;
};

} // namespace omegarace

#endif // UFO_H
