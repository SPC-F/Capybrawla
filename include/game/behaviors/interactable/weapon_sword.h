#pragma once

#include <engine/public/behavior.h>

class WeaponSwordInteractableBehavior : public Behavior {
public:
    WeaponSwordInteractableBehavior(bool is_dropped = false);
    ~WeaponSwordInteractableBehavior() override = default;

    void on_start() override;
    void on_update(float dt) override;
private:
    bool is_dropped_{false};
    float pickup_delay_{0.5f};
    float destroy_delay_{15.0f};
    float time_since_spawn_{0.0f};

    void pickup_weapon(class Collider2D& self, class Collider2D& other);
};
