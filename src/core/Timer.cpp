#include "Timer.h"

namespace omegarace {

Timer::Timer() : mStartTime(0.0), mPausedTime(0.0), mStarted(false), mPaused(false) {
}

void Timer::start() {
    mStarted = true;
    mPaused = false;
    mStartTime = GetTime();  // Raylib's GetTime() returns seconds as double
}

void Timer::stop() {
    mStarted = false;
    mPaused = false;
}

void Timer::pause() {
    if (mStarted && !mPaused) {
        mPaused = true;
        mPausedTime = GetTime() - mStartTime;
    }
}

void Timer::unpause() {
    if (mPaused) {
        mPaused = false;
        mStartTime = GetTime() - mPausedTime;
        mPausedTime = 0.0;
    }
}

int Timer::restart() {
    int elapsedTicks = ticks();
    start();
    return elapsedTicks;
}

int Timer::ticks() const {
    if (mStarted) {
        if (mPaused)
            return (int)(mPausedTime * 1000.0);  // Convert seconds to milliseconds
        else
            return (int)((GetTime() - mStartTime) * 1000.0);  // Convert seconds to milliseconds
    }

    return 0;
}

double Timer::seconds() const {
    if (mStarted) {
        if (mPaused)
            return mPausedTime;
        else
            return GetTime() - mStartTime;
    }

    return 0.0;
}

bool Timer::started() const {
    return mStarted;
}

bool Timer::paused() const {
    return mPaused;
}

} // namespace omegarace
