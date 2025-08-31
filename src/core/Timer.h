#pragma once

#include <raylib.h>

namespace omegarace {

class Timer {
  public:
    Timer();
    void start();
    void stop();
    void pause();
    void unpause();
    int restart();
    int ticks() const;
    double seconds() const;
    bool started() const;
    bool paused() const;

  private:
    double mStartTime;      // Changed to double for GetTime() precision
    double mPausedTime;     // Changed to double for GetTime() precision
    bool mStarted;
    bool mPaused;
};

} // namespace omegarace
