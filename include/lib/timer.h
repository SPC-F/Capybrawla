#pragma once
#include <chrono>

namespace lib
{
class Timer {
public:
    explicit Timer(int duration_seconds);

    [[nodiscard]] int remaining_time() const;
    [[nodiscard]] std::string formatted_remaining_time() const;

    void start();
    void stop();
    void restart();

private:
    bool is_running_;
    int duration_;
    int default_duration_;

    std::chrono::steady_clock::time_point start_time_;
    std::chrono::seconds elapsed_;
};
}
