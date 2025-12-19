#include <game/behaviors/toggle/object_toggle_behavior.h>

#include <engine/core/engine.h>
#include <engine/input/input_manager.h>
#include <engine/input/input_system.h>

ObjectToggleBehavior::ObjectToggleBehavior(GameObject& object, KeyCode toggle_key)
    : object_(object), toggle_key_(toggle_key) {}

void ObjectToggleBehavior::on_update(float dt) {
    const IInputProvider &provider =
        Engine::instance().services->get_service<InputManager>().get().provider();

    if (provider.is_key_pressed(toggle_key_)) {
        bool active = object_.is_active();

        if (active) object_.set_inactive();
        else object_.set_active();
    }
}