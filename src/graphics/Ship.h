#pragma once

#include "Window.h"
#include "VapourTrail.h"

namespace omegarace {

class Ship {
  public:
    Ship();
    ~Ship();

    void initialize();
    void update(float rotation, const Vector2f& location, float scale);
    void draw();
    
    // Vapour trail control
    void setVapourTrailActive(bool active);
    void setVapourTrailColor(const Color& color);

  protected:
    void moveRotateLines(float rotation, const Vector2f& location, float scale);
    Vector2f getRearPosition(float rotation, const Vector2f& centerLocation, float scale);

    Line ShipLines[8];
    Line NewShipLines[8];
    Color m_ShipColor;
    
    // Vapour trail effect
    std::unique_ptr<VapourTrail> m_VapourTrail;
};

} // namespace omegarace
