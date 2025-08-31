#pragma once

#include "PlayerExplosionLine.h"
#include "Window.h"
#include "VapourTrail.h"

namespace omegarace {

class PlayerShip {
  public:
    PlayerShip();
    ~PlayerShip();

    void update(float rotation, const Vector2f& location, float scale);
    void draw(const Color& color);
    void drawThrust();
    void updateExplosion(double frame);
    void drawExplosion();
    void drawShieldEffect();
    void setExplosion(const Vector2i& location);
    void initializeLines();
    
    // Enhanced visual control methods
    void setShieldStrength(float strength);     // 0.0 to 1.0
    void setDamageLevel(float damage);          // 0.0 to 1.0
    void setEngineIntensity(float intensity);   // 0.0 to 1.0
    
    // Vapour trail control
    void setVapourTrailActive(bool active);
    void clearVaporTrail(); // NEW: Clear vapor trail
    
    // Position offset methods for shots and vapour trail
    Vector2f getNosePosition(float rotation, const Vector2f& centerLocation, float scale);
    Vector2f getEnginePosition(float rotation, const Vector2f& centerLocation, float scale);

  private:
    void initialize();
    void moveRotateLines(float rotation, const Vector2f& location, float scale);
    
    // Enhanced visual effect methods
    void updateVisualEffects();
    Color calculateEnhancedShipColor(const Color& baseColor);
    void drawShield();
    void addEngineParticle();
    void updateEngineParticles();
    void drawEngineParticles();

    std::unique_ptr<PlayerExplosionLine> pExplosionLines[12];

    Line playerLines[12];
    Line newPlayerLines[12];

    Line thrustLines[2];
    Line newThrustLines[2];

    Color m_ThrustColor;
    
    // Enhanced visual properties
    float m_ShipIntensity;       // Overall ship brightness/intensity
    float m_EngineGlowIntensity; // Engine glow effect intensity
    float m_ShieldStrength;      // Shield effect strength (0.0 to 1.0)
    float m_DamageLevel;         // Damage level affects ship appearance (0.0 to 1.0)
    
    // Vapour trail effect
    std::unique_ptr<VapourTrail> m_VapourTrail;
};

} // namespace omegarace
