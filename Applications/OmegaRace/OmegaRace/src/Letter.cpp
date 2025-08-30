#include "Letter.h"

namespace omegarace {

Letter::Letter() {
}

Letter::~Letter() {
}

void Letter::initializeLetterLine() {
    m_LetterLineStart[0] = Vector2i(0, 0);  // 1
    m_LetterLineStart[1] = Vector2i(1, 0);  // 2
    m_LetterLineStart[2] = Vector2i(2, 0);  // 3
    m_LetterLineStart[3] = Vector2i(2, 2);  // 4
    m_LetterLineStart[4] = Vector2i(1, 4);  // 5
    m_LetterLineStart[5] = Vector2i(0, 4);  // 6
    m_LetterLineStart[6] = Vector2i(0, 2);  // 7
    m_LetterLineStart[7] = Vector2i(0, 0);  // 8
    m_LetterLineStart[8] = Vector2i(0, 0);  // 9
    m_LetterLineStart[9] = Vector2i(2, 0);  // 10
    m_LetterLineStart[10] = Vector2i(1, 2); // 11
    m_LetterLineStart[11] = Vector2i(1, 2); // 12
    m_LetterLineStart[12] = Vector2i(1, 2); // 13
    m_LetterLineStart[13] = Vector2i(0, 2); // 14
    m_LetterLineStart[14] = Vector2i(1, 0); // 15
    m_LetterLineStart[15] = Vector2i(1, 2); // 16

    m_LetterLineEnd[0] = Vector2i(1, 0);  // 1
    m_LetterLineEnd[1] = Vector2i(2, 0);  // 2
    m_LetterLineEnd[2] = Vector2i(2, 2);  // 3
    m_LetterLineEnd[3] = Vector2i(2, 4);  // 4
    m_LetterLineEnd[4] = Vector2i(2, 4);  // 5
    m_LetterLineEnd[5] = Vector2i(1, 4);  // 6
    m_LetterLineEnd[6] = Vector2i(0, 4);  // 7
    m_LetterLineEnd[7] = Vector2i(0, 2);  // 8
    m_LetterLineEnd[8] = Vector2i(1, 2);  // 9
    m_LetterLineEnd[9] = Vector2i(1, 2);  // 10
    m_LetterLineEnd[10] = Vector2i(2, 2); // 11
    m_LetterLineEnd[11] = Vector2i(2, 4); // 12
    m_LetterLineEnd[12] = Vector2i(0, 4); // 13
    m_LetterLineEnd[13] = Vector2i(1, 2); // 14
    m_LetterLineEnd[14] = Vector2i(1, 2); // 15
    m_LetterLineEnd[15] = Vector2i(1, 4); // 16

    // A
    letters[0].klines[0] = true;
    letters[0].klines[1] = true;
    letters[0].klines[2] = true;
    letters[0].klines[3] = true;
    letters[0].klines[4] = false;
    letters[0].klines[5] = false;
    letters[0].klines[6] = true;
    letters[0].klines[7] = true;
    letters[0].klines[8] = false;
    letters[0].klines[9] = false;
    letters[0].klines[10] = true;
    letters[0].klines[11] = false;
    letters[0].klines[12] = false;
    letters[0].klines[13] = true;
    letters[0].klines[14] = false;
    letters[0].klines[15] = false;
    // B
    letters[1].klines[0] = true;
    letters[1].klines[1] = true;
    letters[1].klines[2] = true;
    letters[1].klines[3] = true;
    letters[1].klines[4] = true;
    letters[1].klines[5] = true;
    letters[1].klines[6] = false;
    letters[1].klines[7] = false;
    letters[1].klines[8] = false;
    letters[1].klines[9] = false;
    letters[1].klines[10] = true;
    letters[1].klines[11] = false;
    letters[1].klines[12] = false;
    letters[1].klines[13] = false;
    letters[1].klines[14] = true;
    letters[1].klines[15] = true;
    // C
    letters[2].klines[0] = true;
    letters[2].klines[1] = true;
    letters[2].klines[2] = false;
    letters[2].klines[3] = false;
    letters[2].klines[4] = true;
    letters[2].klines[5] = true;
    letters[2].klines[6] = true;
    letters[2].klines[7] = true;
    letters[2].klines[8] = false;
    letters[2].klines[9] = false;
    letters[2].klines[10] = false;
    letters[2].klines[11] = false;
    letters[2].klines[12] = false;
    letters[2].klines[13] = false;
    letters[2].klines[14] = false;
    letters[2].klines[15] = false;
    // D
    letters[3].klines[0] = true;
    letters[3].klines[1] = true;
    letters[3].klines[2] = true;
    letters[3].klines[3] = true;
    letters[3].klines[4] = true;
    letters[3].klines[5] = true;
    letters[3].klines[6] = false;
    letters[3].klines[7] = false;
    letters[3].klines[8] = false;
    letters[3].klines[9] = false;
    letters[3].klines[10] = false;
    letters[3].klines[11] = false;
    letters[3].klines[12] = false;
    letters[3].klines[13] = false;
    letters[3].klines[14] = true;
    letters[3].klines[15] = true;
    // E
    letters[4].klines[0] = true;
    letters[4].klines[1] = true;
    letters[4].klines[2] = false;
    letters[4].klines[3] = false;
    letters[4].klines[4] = true;
    letters[4].klines[5] = true;
    letters[4].klines[6] = true;
    letters[4].klines[7] = true;
    letters[4].klines[8] = false;
    letters[4].klines[9] = false;
    letters[4].klines[10] = false;
    letters[4].klines[11] = false;
    letters[4].klines[12] = false;
    letters[4].klines[13] = true;
    letters[4].klines[14] = false;
    letters[4].klines[15] = false;
    // F
    letters[5].klines[0] = true;
    letters[5].klines[1] = true;
    letters[5].klines[2] = false;
    letters[5].klines[3] = false;
    letters[5].klines[4] = false;
    letters[5].klines[5] = false;
    letters[5].klines[6] = true;
    letters[5].klines[7] = true;
    letters[5].klines[8] = false;
    letters[5].klines[9] = false;
    letters[5].klines[10] = false;
    letters[5].klines[11] = false;
    letters[5].klines[12] = false;
    letters[5].klines[13] = true;
    letters[5].klines[14] = false;
    letters[5].klines[15] = false;
    // G
    letters[6].klines[0] = true;
    letters[6].klines[1] = true;
    letters[6].klines[2] = false;
    letters[6].klines[3] = true;
    letters[6].klines[4] = true;
    letters[6].klines[5] = true;
    letters[6].klines[6] = true;
    letters[6].klines[7] = true;
    letters[6].klines[8] = false;
    letters[6].klines[9] = false;
    letters[6].klines[10] = true;
    letters[6].klines[11] = false;
    letters[6].klines[12] = false;
    letters[6].klines[13] = false;
    letters[6].klines[14] = false;
    letters[6].klines[15] = false;
    // H
    letters[7].klines[0] = false;
    letters[7].klines[1] = false;
    letters[7].klines[2] = true;
    letters[7].klines[3] = true;
    letters[7].klines[4] = false;
    letters[7].klines[5] = false;
    letters[7].klines[6] = true;
    letters[7].klines[7] = true;
    letters[7].klines[8] = false;
    letters[7].klines[9] = false;
    letters[7].klines[10] = true;
    letters[7].klines[11] = false;
    letters[7].klines[12] = false;
    letters[7].klines[13] = true;
    letters[7].klines[14] = false;
    letters[7].klines[15] = false;
    // I
    letters[8].klines[0] = true;
    letters[8].klines[1] = true;
    letters[8].klines[2] = false;
    letters[8].klines[3] = false;
    letters[8].klines[4] = true;
    letters[8].klines[5] = true;
    letters[8].klines[6] = false;
    letters[8].klines[7] = false;
    letters[8].klines[8] = false;
    letters[8].klines[9] = false;
    letters[8].klines[10] = false;
    letters[8].klines[11] = false;
    letters[8].klines[12] = false;
    letters[8].klines[13] = false;
    letters[8].klines[14] = true;
    letters[8].klines[15] = true;
    // J
    letters[9].klines[0] = false;
    letters[9].klines[1] = true;
    letters[9].klines[2] = true;
    letters[9].klines[3] = true;
    letters[9].klines[4] = true;
    letters[9].klines[5] = true;
    letters[9].klines[6] = true;
    letters[9].klines[7] = false;
    letters[9].klines[8] = false;
    letters[9].klines[9] = false;
    letters[9].klines[10] = false;
    letters[9].klines[11] = false;
    letters[9].klines[12] = false;
    letters[9].klines[13] = false;
    letters[9].klines[14] = false;
    letters[9].klines[15] = false;
    // K
    letters[10].klines[0] = false;
    letters[10].klines[1] = false;
    letters[10].klines[2] = false;
    letters[10].klines[3] = false;
    letters[10].klines[4] = false;
    letters[10].klines[5] = false;
    letters[10].klines[6] = true;
    letters[10].klines[7] = true;
    letters[10].klines[8] = false;
    letters[10].klines[9] = true;
    letters[10].klines[10] = false;
    letters[10].klines[11] = true;
    letters[10].klines[12] = false;
    letters[10].klines[13] = true;
    letters[10].klines[14] = false;
    letters[10].klines[15] = false;
    // L
    letters[11].klines[0] = false;
    letters[11].klines[1] = false;
    letters[11].klines[2] = false;
    letters[11].klines[3] = false;
    letters[11].klines[4] = true;
    letters[11].klines[5] = true;
    letters[11].klines[6] = true;
    letters[11].klines[7] = true;
    letters[11].klines[8] = false;
    letters[11].klines[9] = false;
    letters[11].klines[10] = false;
    letters[11].klines[11] = false;
    letters[11].klines[12] = false;
    letters[11].klines[13] = false;
    letters[11].klines[14] = false;
    letters[11].klines[15] = false;
    // M
    letters[12].klines[0] = false;
    letters[12].klines[1] = false;
    letters[12].klines[2] = true;
    letters[12].klines[3] = true;
    letters[12].klines[4] = false;
    letters[12].klines[5] = false;
    letters[12].klines[6] = true;
    letters[12].klines[7] = true;
    letters[12].klines[8] = true;
    letters[12].klines[9] = true;
    letters[12].klines[10] = false;
    letters[12].klines[11] = false;
    letters[12].klines[12] = false;
    letters[12].klines[13] = false;
    letters[12].klines[14] = false;
    letters[12].klines[15] = false;
    // N
    letters[13].klines[0] = false;
    letters[13].klines[1] = false;
    letters[13].klines[2] = true;
    letters[13].klines[3] = true;
    letters[13].klines[4] = false;
    letters[13].klines[5] = false;
    letters[13].klines[6] = true;
    letters[13].klines[7] = true;
    letters[13].klines[8] = true;
    letters[13].klines[9] = false;
    letters[13].klines[10] = false;
    letters[13].klines[11] = true;
    letters[13].klines[12] = false;
    letters[13].klines[13] = false;
    letters[13].klines[14] = false;
    letters[13].klines[15] = false;
    // O
    letters[14].klines[0] = true;
    letters[14].klines[1] = true;
    letters[14].klines[2] = true;
    letters[14].klines[3] = true;
    letters[14].klines[4] = true;
    letters[14].klines[5] = true;
    letters[14].klines[6] = true;
    letters[14].klines[7] = true;
    letters[14].klines[8] = false;
    letters[14].klines[9] = false;
    letters[14].klines[10] = false;
    letters[14].klines[11] = false;
    letters[14].klines[12] = false;
    letters[14].klines[13] = false;
    letters[14].klines[14] = false;
    letters[14].klines[15] = false;
    // P
    letters[15].klines[0] = true;
    letters[15].klines[1] = true;
    letters[15].klines[2] = true;
    letters[15].klines[3] = false;
    letters[15].klines[4] = false;
    letters[15].klines[5] = false;
    letters[15].klines[6] = true;
    letters[15].klines[7] = true;
    letters[15].klines[8] = false;
    letters[15].klines[9] = false;
    letters[15].klines[10] = true;
    letters[15].klines[11] = false;
    letters[15].klines[12] = false;
    letters[15].klines[13] = true;
    letters[15].klines[14] = false;
    letters[15].klines[15] = false;
    // Q
    letters[16].klines[0] = true;
    letters[16].klines[1] = true;
    letters[16].klines[2] = true;
    letters[16].klines[3] = true;
    letters[16].klines[4] = true;
    letters[16].klines[5] = true;
    letters[16].klines[6] = true;
    letters[16].klines[7] = true;
    letters[16].klines[8] = false;
    letters[16].klines[9] = false;
    letters[16].klines[10] = false;
    letters[16].klines[11] = true;
    letters[16].klines[12] = false;
    letters[16].klines[13] = false;
    letters[16].klines[14] = false;
    letters[16].klines[15] = false;
    // R
    letters[17].klines[0] = true;
    letters[17].klines[1] = true;
    letters[17].klines[2] = true;
    letters[17].klines[3] = false;
    letters[17].klines[4] = false;
    letters[17].klines[5] = false;
    letters[17].klines[6] = true;
    letters[17].klines[7] = true;
    letters[17].klines[8] = false;
    letters[17].klines[9] = false;
    letters[17].klines[10] = true;
    letters[17].klines[11] = true;
    letters[17].klines[12] = false;
    letters[17].klines[13] = true;
    letters[17].klines[14] = false;
    letters[17].klines[15] = false;
    // S
    letters[18].klines[0] = true;
    letters[18].klines[1] = true;
    letters[18].klines[2] = false;
    letters[18].klines[3] = true;
    letters[18].klines[4] = true;
    letters[18].klines[5] = true;
    letters[18].klines[6] = false;
    letters[18].klines[7] = true;
    letters[18].klines[8] = false;
    letters[18].klines[9] = false;
    letters[18].klines[10] = true;
    letters[18].klines[11] = false;
    letters[18].klines[12] = false;
    letters[18].klines[13] = true;
    letters[18].klines[14] = false;
    letters[18].klines[15] = false;
    // T
    letters[19].klines[0] = true;
    letters[19].klines[1] = true;
    letters[19].klines[2] = false;
    letters[19].klines[3] = false;
    letters[19].klines[4] = false;
    letters[19].klines[5] = false;
    letters[19].klines[6] = false;
    letters[19].klines[7] = false;
    letters[19].klines[8] = false;
    letters[19].klines[9] = false;
    letters[19].klines[10] = false;
    letters[19].klines[11] = false;
    letters[19].klines[12] = false;
    letters[19].klines[13] = false;
    letters[19].klines[14] = true;
    letters[19].klines[15] = true;
    // U
    letters[20].klines[0] = false;
    letters[20].klines[1] = false;
    letters[20].klines[2] = true;
    letters[20].klines[3] = true;
    letters[20].klines[4] = true;
    letters[20].klines[5] = true;
    letters[20].klines[6] = true;
    letters[20].klines[7] = true;
    letters[20].klines[8] = false;
    letters[20].klines[9] = false;
    letters[20].klines[10] = false;
    letters[20].klines[11] = false;
    letters[20].klines[12] = false;
    letters[20].klines[13] = false;
    letters[20].klines[14] = false;
    letters[20].klines[15] = false;
    // V
    letters[21].klines[0] = false;
    letters[21].klines[1] = false;
    letters[21].klines[2] = false;
    letters[21].klines[3] = false;
    letters[21].klines[4] = false;
    letters[21].klines[5] = false;
    letters[21].klines[6] = true;
    letters[21].klines[7] = true;
    letters[21].klines[8] = false;
    letters[21].klines[9] = true;
    letters[21].klines[10] = false;
    letters[21].klines[11] = false;
    letters[21].klines[12] = true;
    letters[21].klines[13] = false;
    letters[21].klines[14] = false;
    letters[21].klines[15] = false;
    // W
    letters[22].klines[0] = false;
    letters[22].klines[1] = false;
    letters[22].klines[2] = true;
    letters[22].klines[3] = true;
    letters[22].klines[4] = false;
    letters[22].klines[5] = false;
    letters[22].klines[6] = true;
    letters[22].klines[7] = true;
    letters[22].klines[8] = false;
    letters[22].klines[9] = false;
    letters[22].klines[10] = false;
    letters[22].klines[11] = true;
    letters[22].klines[12] = true;
    letters[22].klines[13] = false;
    letters[22].klines[14] = false;
    letters[22].klines[15] = false;
    // X
    letters[23].klines[0] = false;
    letters[23].klines[1] = false;
    letters[23].klines[2] = false;
    letters[23].klines[3] = false;
    letters[23].klines[4] = false;
    letters[23].klines[5] = false;
    letters[23].klines[6] = false;
    letters[23].klines[7] = false;
    letters[23].klines[8] = true;
    letters[23].klines[9] = true;
    letters[23].klines[10] = false;
    letters[23].klines[11] = true;
    letters[23].klines[12] = true;
    letters[23].klines[13] = false;
    letters[23].klines[14] = false;
    letters[23].klines[15] = false;
    // Y
    letters[24].klines[0] = false;
    letters[24].klines[1] = false;
    letters[24].klines[2] = false;
    letters[24].klines[3] = false;
    letters[24].klines[4] = false;
    letters[24].klines[5] = false;
    letters[24].klines[6] = false;
    letters[24].klines[7] = false;
    letters[24].klines[8] = true;
    letters[24].klines[9] = true;
    letters[24].klines[10] = false;
    letters[24].klines[11] = false;
    letters[24].klines[12] = false;
    letters[24].klines[13] = false;
    letters[24].klines[14] = false;
    letters[24].klines[15] = true;
    // Z
    letters[25].klines[0] = true;
    letters[25].klines[1] = true;
    letters[25].klines[2] = false;
    letters[25].klines[3] = false;
    letters[25].klines[4] = true;
    letters[25].klines[5] = true;
    letters[25].klines[6] = false;
    letters[25].klines[7] = false;
    letters[25].klines[8] = false;
    letters[25].klines[9] = true;
    letters[25].klines[10] = false;
    letters[25].klines[11] = false;
    letters[25].klines[12] = true;
    letters[25].klines[13] = false;
    letters[25].klines[14] = false;
    letters[25].klines[15] = false;
}

void Letter::drawLetter(const Vector2i& location, int letter, int size) {
    Line LetterLine;
    int sizeIn = size * 0.5;

    if (letter > -1 && letter < 26) {
        for (int line = 0; line < 16; line++) {
            if (letters[letter].klines[line]) {
                LetterLine.start = m_LetterLineStart[line] * sizeIn + location;
                LetterLine.end = m_LetterLineEnd[line] * sizeIn + location;

                Window::DrawLine(&LetterLine, m_Color);
            }
        }
    }
}

void Letter::processString(const std::string& string, const Vector2i& locationStart, int size) {
    std::string stringIn = string;
    Vector2i locationIn = locationStart;
    int stringlength = (int)stringIn.size(); // length of the string.
    int space = 0;

    for (int letter = 0; letter < stringlength; letter++) {
        int letterIn = (int)stringIn[letter];

        if (letterIn > 64 && letterIn < 91) {
            letterIn -= 65;
            drawLetter(Vector2i(locationIn.x + space, locationIn.y), letterIn, size);
        }

        space += float(size * 1.666);
    }
}

void Letter::setColor(const Color& color) {
    m_Color = color;
}

} // namespace omegarace
