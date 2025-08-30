#pragma once

#include "ExplosionLine.h"

namespace omegarace {

class Explosion {
  public:
    Explosion();
    ~Explosion();

    void draw();
    void update(double frame);
    void activate(const Vector2i& location, int size);
    void pauseTimer();
    void unpauseTimer();
    bool getActive();

  private:
    std::unique_ptr<ExplosionLine> pLines[12];
    bool m_Active;
};

} // namespace omegarace
