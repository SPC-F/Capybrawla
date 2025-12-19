#pragma once

#include <engine/public/behavior.h>
#include <engine/public/gameObject.h>
#include <engine/input/key_code.h>

/**
 * @brief Behavior to toggle a GameObject's active state with a key press.
 * @NOTE: The game object can't be the same object this behavior is attached to,
 *       as that would disable the behavior itself on deactivation.
 */
class ObjectToggleBehavior : public Behavior {
public:
    ObjectToggleBehavior(GameObject& object, KeyCode toggle_key = KeyCode::f1);
    ~ObjectToggleBehavior() override = default;    

    void on_update(float dt) override; 
private:
    GameObject& object_;
    KeyCode toggle_key_;
};