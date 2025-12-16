#include "lib/timer.h"

#include <iostream>

lib::Timer::Timer(const int duration_seconds)
    : is_running_(false), duration_(duration_seconds), default_duration_(duration_seconds), elapsed_(0)
{
    elapsed_ = std::chrono::seconds(0);
}

void lib::Timer::start() {
    if (is_running_) {
        return;
    }
    start_time_ = std::chrono::steady_clock::now();
    is_running_ = true;
}

void lib::Timer::stop() {
    if (!is_running_) {
        return;
    }
    const auto now = std::chrono::steady_clock::now();
    elapsed_ = std::chrono::duration_cast<std::chrono::seconds>(now - start_time_);
    is_running_ = false;
}

int lib::Timer::remaining_time() const {
    if (is_running_) {
        const auto now = std::chrono::steady_clock::now();
        const auto elapsedTime = std::chrono::duration_cast<std::chrono::seconds>(now - start_time_);
        const int remainingTime = duration_ - elapsedTime.count();
        return std::max(remainingTime, 0);
    }

    const int remainingTime = duration_ - elapsed_.count();
    return std::max(remainingTime, 0);
}

std::string lib::Timer::formatted_remaining_time() const {
    const int seconds_left = remaining_time();

    const int minutes = seconds_left / 60;  // Get full minutes
    const int seconds = seconds_left % 60;  // Get the remaining seconds

    std::ostringstream oss;
    oss << std::setw(2) << std::setfill('0') << minutes << ":"
        << std::setw(2) << std::setfill('0') << seconds;

    return oss.str();
}

void lib::Timer::restart() {
    duration_ = default_duration_;
    start();
}
