#pragma once

#include "Common.h"

namespace omegarace {

struct Rotation {
    float amount;
    float velocity;
    float acceleration;
};

class Entity : public Common {
  public:
    Entity();
    ~Entity();

    bool circlesIntersect(const Vector2f& target, float targetRadius);
    bool rectangleIntersect(SDL_Rect& Target);
    bool valueInRange(int value, int min, int max);
    bool valueInRange(float value, float min, float max);
    bool valueInRange(float value, int min, int max);
    bool getActive();
    float getRadius();
    Rotation getRotation();
    Vector2f getLocation();
    Vector2f getVelocity();
    void setActive(bool active);
    void setLocation(const Vector2i& location);
    void setLocation(const Vector2f& location);
    void setVelocity(const Vector2f& velocity);
    void setAcceleration(const Vector2f& acceleration);
    void setRotation(const Rotation& rotation);
    void setScale(float scale);

  protected:
    void updateFrame(double frame);
    void bounceX();
    void bounceY();

    bool checkForXEdge();
    bool checkForYEdge();

    bool m_Active;

    Vector2f m_Location;
    Vector2f m_Velocity;
    Vector2f m_Acceleration;
    SDL_Rect m_Rectangle;
    Rotation m_Rotation;

    float m_Radius;
    float m_Scale;
};

} // namespace omegarace
