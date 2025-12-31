#pragma once
#include <engine/public/behavior.h>

#include <string>

class ItemDropper : public Behavior {
private:
    std::vector<std::string> drops_;
    std::string latest_obj_;
    constexpr static double default_drop_duration_secs = 15.0;
    double remaining_time_before_drop = default_drop_duration_secs;

public:
    [[nodiscard]] GameObject& random_drop();
    void on_start() override;
    void on_update(float dt) override;
};