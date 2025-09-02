#include "Timer.h"

namespace omegarace {

Timer::Timer() : mStartTime(), mPausedDuration(0.0), mStarted(false), mPaused(false) {
}

void Timer::start() {
    mStarted = true;
    mPaused = false;
    mStartTime = std::chrono::high_resolution_clock::now();
    mPausedDuration = std::chrono::duration<double>(0.0);
}

void Timer::stop() {
    mStarted = false;
    mPaused = false;
}

void Timer::pause() {
    if (mStarted && !mPaused) {
        mPaused = true;
        auto currentTime = std::chrono::high_resolution_clock::now();
        mPausedDuration = std::chrono::duration_cast<std::chrono::duration<double>>(currentTime - mStartTime);
    }
}

void Timer::unpause() {
    if (mPaused) {
        mPaused = false;
        auto currentTime = std::chrono::high_resolution_clock::now();
        mStartTime = currentTime - std::chrono::duration_cast<std::chrono::high_resolution_clock::duration>(mPausedDuration);
        mPausedDuration = std::chrono::duration<double>(0.0);
    }
}

int Timer::restart() {
    int elapsedTicks = ticks();
    start();
    return elapsedTicks;
}

int Timer::ticks() const {
    if (mStarted) {
        if (mPaused) {
            return static_cast<int>(mPausedDuration.count() * 1000.0);  // Convert seconds to milliseconds
        } else {
            auto currentTime = std::chrono::high_resolution_clock::now();
            auto elapsed = std::chrono::duration_cast<std::chrono::duration<double>>(currentTime - mStartTime);
            return static_cast<int>(elapsed.count() * 1000.0);  // Convert seconds to milliseconds
        }
    }
    return 0;
}

double Timer::seconds() const {
    if (mStarted) {
        if (mPaused) {
            return mPausedDuration.count();
        } else {
            auto currentTime = std::chrono::high_resolution_clock::now();
            auto elapsed = std::chrono::duration_cast<std::chrono::duration<double>>(currentTime - mStartTime);
            return elapsed.count();
        }
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
