#pragma once

#include "Window.h"

namespace omegarace {

class VapourTrail {
  public:
    VapourTrail(int trailLength = 25);
    ~VapourTrail();

    void setActive(bool active);
    bool isActive() const { return m_Active; }
    
    void update(const Vector2f& position);
    void draw();
    
    // Customization options
    void setTrailColor(const Color& color);
    void setFadeRate(float fadeRate);           // How fast trail fades (default 0.015f)
    void setUpdateFrequency(int frequency);     // How often to add points (default 2)
    void setMaxThickness(float thickness);      // Maximum trail thickness (default 2.0f)
    void setMinThickness(float thickness);      // Minimum trail thickness (default 0.5f)

  private:
    bool m_Active;
    int m_TrailLength;
    int m_TrailIndex;
    int m_UpdateCounter;
    int m_UpdateFrequency;
    float m_FadeRate;
    float m_MaxThickness;
    float m_MinThickness;
    
    Vector2f* m_TrailPoints;
    float* m_TrailAlpha;
    Color m_TrailColor;
    
    void clearTrail();
};

} // namespace omegarace
