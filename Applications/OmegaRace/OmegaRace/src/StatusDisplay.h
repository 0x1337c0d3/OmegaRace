#pragma once

#include "Common.h"
#include "Letter.h"
#include "Number.h"
#include "PlayerShip.h"
#include <fstream>
#include <cstdlib>

namespace omegarace {

class StatusDisplay : Common {
  public:
      // Application state
    enum GAME_STATE { APP_START = 0, APP_INSTRUCTIONS = 1, APP_GAMEOVER = 2, APP_PLAYING = 3 };

    StatusDisplay();
    ~StatusDisplay();

    void initialize();
    void draw();
    void update();
    void setScore(int score);
    void setShip(int ship);
    void setState(GAME_STATE s);
    void setInstructions(bool instruction);
    void newGame();

  private:
    void drawTitle();
    void drawGameOver();
    void drawInstructions();
    void drawShips();
    void loadHighScore();
    void saveHighScore();

    GAME_STATE m_State;
    int m_Score;
    int m_HighScore;
    int m_Ships;
    float m_ShipRotation;
    std::string m_GameText[8];
    std::string m_ScoreText[2];
    std::string m_HighScoreFilePath;
    Color m_ShipColor;
    Vector2i m_GameOverLocation;
    Vector2i m_InstructionsLocation;
    Vector2i m_GameTextLocation;
    Vector2f m_ShipLocation;
    Vector2i m_ScoreLocation;
    Vector2i m_HiScoreLocation;
    Vector2i m_ScoreTextLocation;
    Vector2i m_HiScoreTextLocation;

    std::unique_ptr<Number> pNumber;
    std::unique_ptr<Letter> pLetter;
    std::unique_ptr<PlayerShip> pShip;
};

} // namespace omegarace
