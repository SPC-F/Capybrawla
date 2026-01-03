#pragma once

#include <engine/public/behavior.h>

class HealthPackInteractableBehavior : public Behavior {
public:
    HealthPackInteractableBehavior() = default;
    ~HealthPackInteractableBehavior() override = default;

    void on_start() override;
    void on_update(float dt) override {};
};
