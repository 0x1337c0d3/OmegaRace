#include "Number.h"

namespace omegarace {

Number::Number() {
}

Number::~Number() {
}

void Number::initializeNumberLine() {
    // Color of letters and numbers.
    m_Color.red = 255;
    m_Color.green = 255;
    m_Color.blue = 255;
    m_Color.alpha = 255;
    // LED Grid location of line start and end points. 0, 0 is the top left corner.
    m_NumberLineStart[0] = Vector2i(0, 0);
    m_NumberLineStart[1] = Vector2i(1, 0);
    m_NumberLineStart[2] = Vector2i(1, 1);
    m_NumberLineStart[3] = Vector2i(0, 2);
    m_NumberLineStart[4] = Vector2i(0, 1);
    m_NumberLineStart[5] = Vector2i(0, 0);
    m_NumberLineStart[6] = Vector2i(0, 1);

    m_NumberLineEnd[0] = Vector2i(1, 0);
    m_NumberLineEnd[1] = Vector2i(1, 1);
    m_NumberLineEnd[2] = Vector2i(1, 2);
    m_NumberLineEnd[3] = Vector2i(1, 2);
    m_NumberLineEnd[4] = Vector2i(0, 2);
    m_NumberLineEnd[5] = Vector2i(0, 1);
    m_NumberLineEnd[6] = Vector2i(1, 1);

    // LED Grid, what lines are on for each number.
    // Line 0 is the top line.
    // Line 1 is upper right side line.
    // Line 2 is lower right side line.
    // Line 3 is bottom line.
    // Line 4 is lower left side line.
    // Line 5 is upper left side line.
    // Line 6 is the middle line.

    numbers[0].lines[0] = true;
    numbers[0].lines[1] = true;
    numbers[0].lines[2] = true;
    numbers[0].lines[3] = true;
    numbers[0].lines[4] = true;
    numbers[0].lines[5] = true;
    numbers[0].lines[6] = false;

    numbers[1].lines[0] = false;
    numbers[1].lines[1] = true;
    numbers[1].lines[2] = true;
    numbers[1].lines[3] = false;
    numbers[1].lines[4] = false;
    numbers[1].lines[5] = false;
    numbers[1].lines[6] = false;

    numbers[2].lines[0] = true;
    numbers[2].lines[1] = true;
    numbers[2].lines[2] = false;
    numbers[2].lines[3] = true;
    numbers[2].lines[4] = true;
    numbers[2].lines[5] = false;
    numbers[2].lines[6] = true;

    numbers[3].lines[0] = true;
    numbers[3].lines[1] = true;
    numbers[3].lines[2] = true;
    numbers[3].lines[3] = true;
    numbers[3].lines[4] = false;
    numbers[3].lines[5] = false;
    numbers[3].lines[6] = true;

    numbers[4].lines[0] = false;
    numbers[4].lines[1] = true;
    numbers[4].lines[2] = true;
    numbers[4].lines[3] = false;
    numbers[4].lines[4] = false;
    numbers[4].lines[5] = true;
    numbers[4].lines[6] = true;

    numbers[5].lines[0] = true;
    numbers[5].lines[1] = false;
    numbers[5].lines[2] = true;
    numbers[5].lines[3] = true;
    numbers[5].lines[4] = false;
    numbers[5].lines[5] = true;
    numbers[5].lines[6] = true;

    numbers[6].lines[0] = true;
    numbers[6].lines[1] = false;
    numbers[6].lines[2] = true;
    numbers[6].lines[3] = true;
    numbers[6].lines[4] = true;
    numbers[6].lines[5] = true;
    numbers[6].lines[6] = true;

    numbers[7].lines[0] = true;
    numbers[7].lines[1] = true;
    numbers[7].lines[2] = true;
    numbers[7].lines[3] = false;
    numbers[7].lines[4] = false;
    numbers[7].lines[5] = false;
    numbers[7].lines[6] = false;

    numbers[8].lines[0] = true;
    numbers[8].lines[1] = true;
    numbers[8].lines[2] = true;
    numbers[8].lines[3] = true;
    numbers[8].lines[4] = true;
    numbers[8].lines[5] = true;
    numbers[8].lines[6] = true;

    numbers[9].lines[0] = true;
    numbers[9].lines[1] = true;
    numbers[9].lines[2] = true;
    numbers[9].lines[3] = false;
    numbers[9].lines[4] = false;
    numbers[9].lines[5] = true;
    numbers[9].lines[6] = true;
}

void Number::processNumber(int number, const Vector2i& locationStart, int size) {
    int space = 0;
    int sizeIn = size;
    int numberIn = number;
    Vector2i locationIn = locationStart;

    do {
        // Make digit the modulus of 10 from number.
        int digit = numberIn % 10;
        // This sends a digit to the draw function with the location and size.
        drawDigit(Vector2i(locationIn.x - space, locationIn.y), digit, size);
        // Dividing the int by 10, we discard the digit that was derived from the modulus operation.
        numberIn /= 10;
        // Move the location for the next digit location to the left. We start on the right hand side with the lowest
        // digit.
        space += sizeIn * 2;
    } while (numberIn > 0);
}

void Number::drawDigit(const Vector2i& location, int digit, int size) {
    Line NumberLine;
    int sizeIn = size;

    if (digit > -1 && digit < 10) {
        for (int line = 0; line < 7; line++) {
            if (numbers[digit].lines[line]) {
                NumberLine.start = m_NumberLineStart[line] * sizeIn + location;
                NumberLine.end = m_NumberLineEnd[line] * sizeIn + location;

                Window::DrawLine(&NumberLine, m_Color);
            }
        }
    }
}

} // namespace omegarace
