#include "StatusDisplay.h"

namespace omegarace {

StatusDisplay::StatusDisplay() {
    m_Score = 0;
    m_Ships = 0;
    m_HighScore = 0;
    m_ShipRotation = Pi * 1.5f;
    m_ShipLocation = Vector2f(Window::GetWindowSize().x * 0.20, Window::GetWindowSize().y * 0.40);

    m_GameText[0] = "OMEGA RACE";
    m_GameText[1] = "GAME OVER";
    m_GameText[2] = "N OR START BUTTON TO START OR RESTART GAME";
    m_GameText[3] = "ARROW KEYS TO CONTROL SHIP UP FOR THRUST";
    m_GameText[4] = "LEFT CTRL KEY OR SPACE TO FIRE";
    m_GameText[5] = "HIGH SCORE HEROES";
    m_GameText[6] = "NEW HIGH SCORE";
    m_GameText[7] = "ARROW KEYS TO SELECT LETTER FIRE KEY WHEN DONE";

    m_ScoreText[0] = "HIGH SCORE";
    m_ScoreText[1] = "PLAYER SCORE";

    m_State = APP_START;
    m_GameOverLocation.x = Window::GetWindowSize().x * 0.38;
    m_GameOverLocation.y = Window::GetWindowSize().y / 2 - 30;
    m_InstructionsLocation.x = Window::GetWindowSize().x * 0.25;
    m_InstructionsLocation.y = Window::GetWindowSize().y / 2 - 30;

    m_GameTextLocation.x = Window::GetWindowSize().x / 2;
    m_GameTextLocation.y = Window::GetWindowSize().y / 2;

    m_HiScoreLocation = Vector2i(Window::GetWindowSize().x * 0.8f, Window::GetWindowSize().y * 0.5f + 40);
    m_ScoreLocation = m_HiScoreLocation - Vector2i(0, 60);
    m_ScoreTextLocation = m_ScoreLocation - Vector2i(145, 30);
    m_HiScoreTextLocation = m_HiScoreLocation - Vector2i(120, 30);

    pNumber = std::make_unique<Number>();
    pLetter = std::make_unique<Letter>();
    pShip = std::make_unique<PlayerShip>();

    // Player Ship Color
    m_ShipColor.red = 255;
    m_ShipColor.green = 255;
    m_ShipColor.blue = 255;
    m_ShipColor.alpha = 255;

    pLetter->setColor(m_ShipColor);

    // Create Application Support directory path for high scores
    std::string homeDir = getenv("HOME");
    std::string appSupportDir = homeDir + "/Library/Application Support/Omega Race";

    // Create directory if it doesn't exist
    std::string createDirCommand = "mkdir -p \"" + appSupportDir + "\"";
    system(createDirCommand.c_str());

    // Set the high score file path
    m_HighScoreFilePath = appSupportDir + "/highscore";

    // Load high score from file
    loadHighScore();
}

StatusDisplay::~StatusDisplay() {
}

void StatusDisplay::initialize() {
    Vector2i windowSize = Window::GetWindowSize();

    // Recalculate all screen-dependent positions based on current window size
    m_ShipLocation = Vector2f(windowSize.x * 0.20, windowSize.y * 0.40);

    m_GameOverLocation.x = windowSize.x * 0.38;
    m_GameOverLocation.y = windowSize.y / 2 - 30;
    m_InstructionsLocation.x = windowSize.x * 0.25;
    m_InstructionsLocation.y = windowSize.y / 2 - 30;

    m_GameTextLocation.x = windowSize.x / 2;
    m_GameTextLocation.y = windowSize.y / 2;

    m_HiScoreLocation = Vector2i(windowSize.x * 0.8f, windowSize.y * 0.5f + 40);
    m_ScoreLocation = m_HiScoreLocation - Vector2i(0, 60);
    m_ScoreTextLocation = m_ScoreLocation - Vector2i(145, 30);
    m_HiScoreTextLocation = m_HiScoreLocation - Vector2i(120, 30);

    pNumber->initializeNumberLine();
    pLetter->initializeLetterLine();
}

void StatusDisplay::draw() {
    int size = 8;

    switch (m_State) {
        case APP_START:
            drawTitle();
            break;
        case APP_INSTRUCTIONS:
            drawInstructions();
            break;
        case APP_GAMEOVER:
            drawGameOver();
            break;
        case APP_PLAYING:
            pLetter->processString(m_ScoreText[0], m_HiScoreTextLocation, size);
            pNumber->processNumber(m_HighScore, m_HiScoreLocation, size);
            pLetter->processString(m_ScoreText[1], m_ScoreTextLocation, size);
            pNumber->processNumber(m_Score, m_ScoreLocation, size);
            drawShips();
            break;
    }
}

void StatusDisplay::update() {
}

void StatusDisplay::setScore(int score) {
    m_Score = score;
}

void StatusDisplay::setShip(int ship) {
    m_Ships = ship;
}

void StatusDisplay::setState(GAME_STATE s) {
    m_State = s;
}

StatusDisplay::GAME_STATE StatusDisplay::getState() const {
    return m_State;
}

void StatusDisplay::newGame() {
    if (m_Score > m_HighScore) {
        m_HighScore = m_Score;
        saveHighScore();
    }

    m_Score = 0;
    m_Ships = 0;
    m_State = APP_PLAYING;
}

void StatusDisplay::drawTitle() {
    int size = 15;
    pLetter->processString(m_GameText[0], m_GameOverLocation, size);
}

void StatusDisplay::drawGameOver() {
    int size = 15;
    pLetter->processString(m_GameText[1], m_GameOverLocation, size);
}

void StatusDisplay::drawInstructions() {
    int size = 8;
    Vector2i location = m_InstructionsLocation;
    for (int line = 2; line < 5; line++) {
        pLetter->processString(m_GameText[line], location, size);
        location.y += 30;
    }
}

void StatusDisplay::drawShips() {
    for (int ship = 0; ship < m_Ships; ship++) {
        float scale = 1.8;
        Vector2f location = m_ShipLocation + Vector2f(ship * 20, 0);
        pShip->update(m_ShipRotation, location, scale);
        pShip->draw(m_ShipColor);
    }
}

void StatusDisplay::loadHighScore() {
    std::ifstream file(m_HighScoreFilePath, std::ios::binary);
    if (file.is_open()) {
        file.read(reinterpret_cast<char*>(&m_HighScore), sizeof(m_HighScore));
        file.close();
    } else {
        // Default high score if file doesn't exist
        m_HighScore = 0;
    }
}

void StatusDisplay::saveHighScore() {
    std::ofstream file(m_HighScoreFilePath, std::ios::binary);
    if (file.is_open()) {
        file.write(reinterpret_cast<const char*>(&m_HighScore), sizeof(m_HighScore));
        file.close();
    }
}

} // namespace omegarace
