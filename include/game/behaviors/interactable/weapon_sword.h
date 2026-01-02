#pragma once

#include <engine/public/behavior.h>

class WeaponSwordInteractableBehavior : public Behavior {
public:
    WeaponSwordInteractableBehavior() = default;
    ~WeaponSwordInteractableBehavior() override = default;

    void on_start() override;
    void on_update(float dt) override {};
};
