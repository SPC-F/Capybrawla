#pragma once

#include <engine/public/behavior.h>
#include <engine/public/components/colliders/box_collider_2d.h>
#include <engine/public/components/sprite.h>
#include <engine/public/components/rigidbody_2d.h>
#include <engine/public/util/layers.h>

class CloudPlatformBehavior : public Behavior {
public:
    CloudPlatformBehavior() = default;
    ~CloudPlatformBehavior() override = default;    

    void on_start() override;
    void on_update(float dt) override;

private: 
    bool player_left_{false};
    float time_before_disappear_{2.0f};
    float accumulated_time_{0.0f};
    float float_timer_{0.0f};

    bool disappearing_{false};
    float disappear_timer_{0.0f};
    float disappear_duration_{0.5f};
    float disappear_speed_{600.0f};

    std::optional<std::reference_wrapper<BoxCollider2D>> box_collider_;
};