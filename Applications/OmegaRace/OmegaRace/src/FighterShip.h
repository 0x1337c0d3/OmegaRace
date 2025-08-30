#pragma once

#include "Window.h"

namespace omegarace {

class FighterShip {
  public:
    FighterShip();
    ~FighterShip();

    void update(const Vector2f& location, float rotation);
    void draw();

  private:
    void initilize();
    void moveRotateLines(float rotation, const Vector2f& location);

    Line shipLines[2];
    Line newShipLines[2];
    Color m_ShipColor;
    float m_Scale;
};

} // namespace omegarace
