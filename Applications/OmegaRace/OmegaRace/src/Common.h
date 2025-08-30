#pragma once

#include "Window.h"
#include "vmath.h"

namespace omegarace {

class Common {
  public:
    Common();
    ~Common();

  protected:
    Vector2i randomLocation();
    int getRandomX();
    int getRandomY();
    const float PiTimesTwo = 6.2831853f;
    const float Pi = 3.1415927f;
};

} // namespace omegarace
