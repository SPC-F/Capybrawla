#pragma once
#include <engine/public/behavior.h>

class ItemDropper : public Behavior {
private:
    std::vector<std::unique_ptr<GameObject>> drops_;
    constexpr static double default_drop_duration_secs = 15.0;
    double remaining_time_before_drop = default_drop_duration_secs;

public:
    [[nodiscard]] std::unique_ptr<GameObject> random_drop() const;
    void on_start() override;
    void on_update(float dt) override;
};