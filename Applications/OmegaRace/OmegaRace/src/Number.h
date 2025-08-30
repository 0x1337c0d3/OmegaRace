#pragma once

#include "Common.h"

namespace omegarace {

struct NumberData {
    bool lines[7];
};

class Number {
  public:
    Number();
    ~Number();

    void initializeNumberLine();
    void processNumber(int number, const Vector2i& locationStart, int size);
    void drawDigit(const Vector2i& location, int digit, int size);

  private:
    Color m_Color;
    // Number lines
    NumberData numbers[10];
    Vector2i m_NumberLineStart[7];
    Vector2i m_NumberLineEnd[7];
};

} // namespace omegarace
