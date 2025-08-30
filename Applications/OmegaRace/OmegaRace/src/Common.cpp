#include "Common.h"

namespace omegarace {

Common::Common() {
}

Common::~Common() {
}

Vector2i Common::randomLocation() {
    return Vector2i(Window::Random(0, Window::GetWindowSize().x), Window::Random(0, Window::GetWindowSize().y));
}

int Common::getRandomX() {
    return (Window::Random(0, Window::GetWindowSize().x));
}

int Common::getRandomY() {
    return (Window::Random(0, Window::GetWindowSize().y));
}

} // namespace omegarace
