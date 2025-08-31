#pragma once

#include "Entity.h"
#include "TriShip.h"

namespace omegarace {

class DoubleMine : public Entity {
  public:
    DoubleMine();
    ~DoubleMine();

    void initialize();
    void draw();
    void mineDropped(const Vector2f& location);

  private:
    std::unique_ptr<TriShip> pMine;
};

} // namespace omegarace
