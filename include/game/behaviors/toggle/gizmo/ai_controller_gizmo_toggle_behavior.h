#pragma once

#include <engine/public/behavior.h>
#include <engine/public/components/sprite.h>
#include <engine/public/components/rigidbody_2d.h>
#include <engine/public/util/layers.h>

/**
 * @brief Behavior to toggle Navigation Grid Gizmo (navigation graph) with F3 key presses.
 */
class AIControllerGizmoToggleBehavior : public Behavior {
public:
    AIControllerGizmoToggleBehavior(std::reference_wrapper<GameObject> ai_agent_object);
    ~AIControllerGizmoToggleBehavior() override = default;    

    void on_update(float dt) override;
private:
    std::reference_wrapper<GameObject> ai_agent_object_;
};