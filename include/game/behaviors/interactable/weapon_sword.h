#pragma once

#include <engine/public/behavior.h>

class WeaponSwordInteractableBehavior : public Behavior {
public:
    WeaponSwordInteractableBehavior() = default;
    ~WeaponSwordInteractableBehavior() override = default;

    void on_start() override;
    void on_update(float dt) override;
private:
    float pickup_delay_{0.5f};
    float destroy_delay_{15.0f};
    float time_since_spawn_{0.0f};

    void pickup_weapon(class Collider2D& self, class Collider2D& other);
};
