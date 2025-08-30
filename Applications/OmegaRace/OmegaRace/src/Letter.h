#pragma once

#include "Common.h"

namespace omegarace {

struct LetterData {
    bool klines[16];
};

class Letter {
  public:
    Letter();
    ~Letter();

    void initializeLetterLine();
    void processString(const std::string& string, const Vector2i& locationStart, int size);
    void drawLetter(const Vector2i& location, int letter, int size);
    void setColor(const Color& color);

  private:
    Color m_Color;
    // Letter Lines
    LetterData letters[26];
    Vector2i m_LetterLineStart[16];
    Vector2i m_LetterLineEnd[16];
};

} // namespace omegarace
