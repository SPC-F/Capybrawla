#include <game/behaviors/physics_gizmo_toggle_behavior.h>

#include <engine/core/engine.h>
#include <engine/input/input_manager.h>
#include <engine/input/input_system.h>

void PhysicsGizmoToggleBehavior::on_awake() {
    physics_service_ = Engine::instance().services->get_service<PhysicsService>();
}

void PhysicsGizmoToggleBehavior::on_update(float dt) {
    const IInputProvider &provider =
        Engine::instance().services->get_service<InputManager>().get().provider();

    if (provider.is_key_pressed(KeyCode::f2) && physics_service_.has_value()) {
        physics_service_->get().debug_mode(!physics_service_->get().debug_mode());
    }
}