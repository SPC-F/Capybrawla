#pragma once

#include <engine/public/behavior.h>

class WeaponBatInteractableBehavior : public Behavior {
public:
    WeaponBatInteractableBehavior() = default;
    ~WeaponBatInteractableBehavior() override = default;

    void on_start() override;
    void on_update(float dt) override {};
};
