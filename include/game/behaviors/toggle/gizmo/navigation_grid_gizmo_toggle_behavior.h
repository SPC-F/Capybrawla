#pragma once

#include <engine/public/behavior.h>
#include <engine/public/components/sprite.h>
#include <engine/public/components/rigidbody_2d.h>
#include <engine/public/util/layers.h>

/**
 * @brief Behavior to toggle Navigation Grid Gizmo (navigation graph) with F3 key presses.
 */
class NavigationGridGizmoToggleBehavior : public Behavior {
public:
    NavigationGridGizmoToggleBehavior() = default;
    ~NavigationGridGizmoToggleBehavior() override = default;    

    void on_update(float dt) override;
private:
};