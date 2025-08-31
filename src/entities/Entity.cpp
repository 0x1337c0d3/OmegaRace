#include "Entity.h"
#include "Window.h"

namespace omegarace {

Entity::Entity() : Common() {
    m_Active = false;

    m_Rotation.amount = 0;
    m_Rotation.acceleration = 0;
    m_Rotation.velocity = 0;
    m_Radius = 0;

    m_Location = Vector2f();
    m_Velocity = Vector2f();
    m_Acceleration = Vector2f();
}

Entity::~Entity() {
}

bool Entity::circlesIntersect(const Vector2f& target, float targetRadius) {
    float distance = sqrt((target.x - m_Location.x) * (target.x - m_Location.x) +
                          (target.y - m_Location.y) * (target.y - m_Location.y));

    if (distance < m_Radius + targetRadius) {
        return true;
    }

    return false;
}

bool Entity::rectangleIntersect(SDL_Rect& target) {
    int max = m_Rectangle.x + m_Rectangle.w;
    int targetmax = target.x + target.w;

    bool OverlapX = valueInRange(target.x, m_Rectangle.x, max) || valueInRange(m_Rectangle.x, target.x, targetmax);

    max = m_Rectangle.y + m_Rectangle.h;
    targetmax = target.y + target.h;

    bool OverlapY = valueInRange(target.y, m_Rectangle.y, max) || valueInRange(m_Rectangle.y, target.y, targetmax);

    return OverlapX && OverlapY;
}

bool Entity::getActive() {
    return m_Active;
}

Vector2f Entity::getLocation() {
    return m_Location;
}

Vector2f Entity::getVelocity() {
    return m_Velocity;
}

float Entity::getRadius() {
    return m_Radius;
}

Rotation Entity::getRotation() {
    return m_Rotation;
}

void Entity::setActive(bool active) {
    m_Active = active;
}

void Entity::setLocation(const Vector2i& location) {
    m_Location = location;
}

void Entity::setLocation(const Vector2f& location) {
    m_Location = location;
}

void Entity::setVelocity(const Vector2f& velocity) {
    m_Velocity = velocity;
}

void Entity::setAcceleration(const Vector2f& acceleration) {
    m_Acceleration = acceleration;
}

void Entity::setRotation(const Rotation& rotation) {
    m_Rotation = rotation;
}

void Entity::setScale(float scale) {
    m_Scale = scale;
}

bool Entity::valueInRange(int value, int min, int max) {
    return (value >= min) && (value <= max);
}

bool Entity::valueInRange(float value, float min, float max) {
    return (value >= min) && (value <= max);
}

bool Entity::valueInRange(float value, int min, int max) {
    return ((int)value >= min) && ((int)value <= max);
}

// Protected methods
void Entity::bounceX() {
    m_Velocity.x *= 0.5f;
    m_Velocity.x *= -1.0f;
    m_Acceleration = Vector2f(0, 0);
}

void Entity::bounceY() {
    m_Velocity.y *= 0.5f;
    m_Velocity.y *= -1.0f;
    m_Acceleration = Vector2f(0, 0);
}

bool Entity::checkForXEdge() {
    if (m_Location.x < m_Rectangle.w * 0.5f)
        return true;

    if (m_Location.x + m_Rectangle.w * 0.5f > Window::GetWindowSize().x)
        return true;

    return false;
}

bool Entity::checkForYEdge() {
    if (m_Location.y < m_Rectangle.h * 0.5f)
        return true;

    if (m_Location.y + m_Rectangle.h * 0.5f > Window::GetWindowSize().y)
        return true;

    return false;
}

void Entity::updateFrame(double Frame) {
    double frame = Frame;
    // Calculate movement this frame according to velocity and acceleration.
    m_Velocity += m_Acceleration;
    m_Location += (m_Velocity * frame);
    // Calculate rotation this frame according to velocity and acceleration.
    m_Rotation.velocity += m_Rotation.acceleration;
    m_Rotation.amount += (m_Rotation.velocity * frame);
    // Update rectangle. rectangle is always centered to the entity.
    m_Rectangle.x = m_Location.x - m_Rectangle.w * 0.5f;
    m_Rectangle.y = m_Location.y - m_Rectangle.h * 0.5f;
}

} // namespace omegarace
