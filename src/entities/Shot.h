#pragma once

#include "Entity.h"
#include "Timer.h"

namespace omegarace {

class Shot : public Entity {
  public:
    Shot();
    ~Shot();

    void update(double frame);
    void draw();
    void activate(const Vector2f& location, float angle);
    void timerPause();
    void timerUnpause();
    void setInsideBorder(const SDL_Rect& border);
    void setInsideLine(int line);
    bool getInsideLine(int line);
    bool getOutsideLine(int line);

  private:
    void checkForEdge();

    bool m_InsideLines[4];
    bool m_OutsideLines[8];
    float m_ShotTimer;
    std::unique_ptr<Timer> pTimer;
    SDL_Rect m_InsideBorder;

  protected:
    Color m_InsideColor;
    Color m_OutsideColor;
    float m_Angle;
    float m_ShotTimeLimit;
    float m_Speed;
};

} // namespace omegarace
