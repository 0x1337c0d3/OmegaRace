#pragma once

#include "GameController.h"
#include "Timer.h"
#include "Window.h"
#include <thread>

namespace omegarace {

class Game {
  public:
    // Application state
    enum APP_STATE { APP_OK = 0, APP_FAILED = 1, APP_FAILEDTTF = 2, APP_FAILEDTEXTURE = 3 };

    Game();
    ~Game();

    int OnExecute();

  private:
    std::unique_ptr<Timer> pTimer;

    std::unique_ptr<GameController> pGameController;

    // Whether the application is running.
    bool running;

    // Ticks last cycle/frame
    int m_LastTickTime;

    // 60fps lock constants and variables
    static constexpr double TARGET_FPS = 60.0;
    static constexpr double TARGET_FRAME_TIME = 1.0 / TARGET_FPS; // ~0.01667 seconds per frame
    double m_AccumulatedTime;
    double m_LastUpdateTime;

    // Initialize application
    int onInit();

    // Clean up the application
    void onCleanup();

    // Called to handle input (replaces SDL event handling)
    void handleInput();

    // Called to update game logic
    void onUpdate();

    // Called to render the app.
    void onRender();
};

} // namespace omegarace
