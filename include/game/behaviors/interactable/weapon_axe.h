#pragma once

#include <engine/public/behavior.h>

class WeaponAxeInteractableBehavior : public Behavior {
public:
    WeaponAxeInteractableBehavior() = default;
    ~WeaponAxeInteractableBehavior() override = default;

    void on_start() override;
    void on_update(float dt) override {};
};
