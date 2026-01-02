#pragma once

#include <engine/public/behavior.h>

class DyingCapybaraBehavior : public Behavior {
public:
    DyingCapybaraBehavior(float disappear_time);
    ~DyingCapybaraBehavior() override = default;    

    void on_awake() override;
    void on_update(float dt) override;

private: 
    float time_before_disappear_{2.0f};
    float accumulated_time_{0.0f};
};