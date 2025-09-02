#pragma once

#include <chrono>

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
    std::chrono::high_resolution_clock::time_point mStartTime;
    std::chrono::duration<double> mPausedDuration;
    bool mStarted;
    bool mPaused;
};

} // namespace omegarace
