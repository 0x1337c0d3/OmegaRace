#pragma once

#include "Entity.h"
#include "TriShip.h"

namespace omegarace {

class Mine : public Entity {
  public:
    Mine();
    ~Mine();

    void draw();
    void mineDropped();

  private:
    std::unique_ptr<TriShip> pMine;
};

} // namespace omegarace
