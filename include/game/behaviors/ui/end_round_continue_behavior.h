#pragma once

#include <engine/public/behavior.h>

class EndRoundContinueBehavior : public Behavior {
public:
    EndRoundContinueBehavior(float disappear_time = 2.0f);
    ~EndRoundContinueBehavior() override = default;    

    void on_update(float dt) override;

private: 
    float time_before_disappear_{2.0f};
    float accumulated_time_{0.0f};
};