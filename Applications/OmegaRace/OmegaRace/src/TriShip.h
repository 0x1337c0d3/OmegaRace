#pragma once

#include "Window.h"
#include "VapourTrail.h"

namespace omegarace {

enum class TriShipMode {
    ENEMY,      // Standard enemy appearance
    MINE,       // Dangerous mine appearance
    DOUBLE_MINE // Extra dangerous double mine
};

class TriShip {
  public:
    TriShip();
    ~TriShip();

    void update(const Vector2f& location, float scale);
    void draw();
    void setThreatLevel(float level); // 0.0 to 1.0 - affects menacing appearance
    void setAggressiveMode(bool aggressive);
    void setMode(TriShipMode mode);   // Set visual mode (enemy, mine, double mine)

  private:
    void initilize();
    void moveScale(const Vector2f& location, float scale);
    Vector2f getRearPosition() const; // Get rear position for vapour trail
    void drawMenacingEffects();
    void drawMineEffects();
    void drawDoubleMineEffects();
    void updateMenacingColors();
    void updateMineColors();

    Line triangle[3];
    Line newTriangle[3];
    Color m_ShipColor;
    VapourTrail m_VapourTrail;
    Vector2f m_CurrentLocation;
    float m_CurrentScale;
    
    // Menacing visual enhancements
    float m_ThreatLevel;
    float m_PulseIntensity;
    float m_AnimationTime;
    bool m_AggressiveMode;
    Color m_ThreatColor;
    Color m_CoreColor;
    TriShipMode m_Mode;
    
    // Mine-specific properties
    float m_MineWarningIntensity;
    Color m_WarningColor;
    Color m_DangerColor;
};

} // namespace omegarace
