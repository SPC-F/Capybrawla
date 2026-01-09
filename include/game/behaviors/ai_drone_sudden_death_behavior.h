#pragma once

#include <engine/public/behavior.h>
#include <engine/public/components/ai/ai_controller.h>
#include <game/scripts/timer/RoundTimer.h>
#include <engine/audio/audio_service.h>

class AIDroneSuddenDeathBehavior : public Behavior {
public:
    AIDroneSuddenDeathBehavior() = default;
    ~AIDroneSuddenDeathBehavior() override = default;    

    void on_awake() override;
    void on_update(float dt) override;

private:
    std::optional<std::reference_wrapper<AIController>> ai_controller_;
    std::optional<std::reference_wrapper<RoundTimer>> round_timer_;
    std::optional<std::reference_wrapper<AudioService>> audio_service_;

    float attack_cooldown_ = 0.0f;
    const float attack_interval_ = 1.0f; // Attack every second
};