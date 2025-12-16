#pragma once

#include <engine/public/behavior.h>
#include <engine/public/components/sprite.h>
#include <engine/public/components/rigidbody_2d.h>
#include <engine/public/util/layers.h>

class AIDroneMovementBehavior : public Behavior {
public:
    AIDroneMovementBehavior() = default;
    ~AIDroneMovementBehavior() override = default;    

    void on_awake() override;
    void on_update(float dt) override;
private: 
    std::optional<std::reference_wrapper<Sprite>> sprite_component_;
    std::optional<std::reference_wrapper<Rigidbody2D>> rigidbody_component_;
};