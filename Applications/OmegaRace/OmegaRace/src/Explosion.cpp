#include "Explosion.h"

namespace omegarace {

Explosion::Explosion() {
    m_Active = false;

    for (int line = 0; line < 12; line++) {
        pLines[line] = std::make_unique<ExplosionLine>();
    }
}

Explosion::~Explosion() {
}

void Explosion::draw() {
    if (m_Active) {
        for (int line = 0; line < 12; line++) {
            pLines[line]->draw();
        }
    }
}

void Explosion::update(double frame) {
    if (m_Active) {
        for (int line = 0; line < 12; line++) {
            pLines[line]->update(frame);
        }
    }
}

void Explosion::activate(const Vector2i& location, int size) {
    m_Active = true;

    float angle = 0;

    for (int line = 0; line < 12; line++) {
        angle += (Window::Random(0, 630)) * 0.01;
        pLines[line]->activate(location, angle, size);
    }
}

void Explosion::pauseTimer() {
    for (int line = 0; line < 12; line++) {
        pLines[line]->pauseTimer();
    }
}

void Explosion::unpauseTimer() {
    for (int line = 0; line < 12; line++) {
        pLines[line]->unpauseTimer();
    }
}

bool Explosion::getActive() {
    bool active = false;

    for (int line = 0; line < 12; line++) {
        if (pLines[line]->getActive())
            active = true;
    }

    return active;
}

} // namespace omegarace
