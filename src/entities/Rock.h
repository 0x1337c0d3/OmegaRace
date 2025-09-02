#ifndef ROCK_H
#    define ROCK_H
#    include "Entity.h"
#    include "Explosion.h"
#    include "Window.h"

namespace omegarace {

class Rock : public Entity {
  private:
    Color m_Color;
    Vector2i m_RockPoints[12];

    Explosion* pExplosion;
    // Note: Using FMOD audio system instead of Mix_Chunk

    void buildRock();

    // SimpleRock dust explosion system
    struct RockDustParticle {
        Vector2f position;
        Vector2f velocity;
        Color color;
        float life;
        float maxLife;
        float size;
        bool active;
    };
    RockDustParticle m_DustParticles[12]; // Fewer particles for dust effect
    bool m_DustActive;
    float m_DustTimer;
    float m_DustDuration;

  protected:
    bool m_Distroyed;
    int m_MaxVelocity;
    int m_RockLow;
    int m_RockMed;
    int m_RockHigh;
    // how much the rock varies.
    int m_RockVarienceHigh;
    int m_RockVarienceMed;
    int m_RockVarienceLow;

  public:
    Rock(std::mt19937& random);
    ~Rock();

    void initialize(std::mt19937& random);
    void pauseTimer();
    void unpauseTimer();

    // SimpleRock compatibility methods (actively used by GameController)
    void activate(Vector2f pos, Vector2f vel);
    void update(double frame);
    void draw();
    void triggerDustExplosion();
    void updateDustExplosion(double frame);
    void drawDustExplosion();
    void forceResetDustExplosion(); // NEW: Force immediate dust explosion reset
    void checkForEdge();
    bool isDestroyed() const {
        return m_Distroyed;
    }
    void setDestroyed(bool d);
    Vector2f getLocation() const {
        return m_Location;
    }
    float getRadius() const {
        return m_Radius;
    }
    bool isDustActive() const {
        return m_DustActive;
    }
    Vector2f position; // Public access for compatibility
    Vector2f velocity; // Public access for compatibility
    bool destroyed;    // Public access for compatibility
    bool active;       // Public access for compatibility
};

#endif

} // namespace omegarace
