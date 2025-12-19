#pragma once

#include <engine/public/behavior.h>
#include <engine/physics/physics_service.h>

class PhysicsGizmoToggleBehavior : public Behavior {
public:
    PhysicsGizmoToggleBehavior() = default;
    ~PhysicsGizmoToggleBehavior() override = default;    

    void on_awake() override;
    void on_update(float dt) override; 
private:
    std::optional<std::reference_wrapper<PhysicsService>> physics_service_;
};