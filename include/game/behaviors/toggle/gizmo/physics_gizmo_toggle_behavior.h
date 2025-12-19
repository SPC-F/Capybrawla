#pragma once

#include <engine/public/behavior.h>
#include <engine/physics/physics_service.h>

/**
 * @brief Behavior to toggle the physics debug gizmos with F2 key press.
 */
class PhysicsGizmoToggleBehavior : public Behavior {
public:
    PhysicsGizmoToggleBehavior() = default;
    ~PhysicsGizmoToggleBehavior() override = default;    

    void on_awake() override;
    void on_update(float dt) override; 
private:
    std::optional<std::reference_wrapper<PhysicsService>> physics_service_;
};