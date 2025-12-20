#pragma once

#include <engine/public/behavior.h>
#include <engine/public/components/sprite.h>
#include <engine/public/components/rigidbody_2d.h>
#include <engine/public/util/layers.h>

/**
 * @brief Behavior to toggle AI gizmos (navigation graph and pathfinding) with F3 and F4 key presses.
 */
class AIGizmoToggleBehavior : public Behavior {
public:
    AIGizmoToggleBehavior(std::reference_wrapper<GameObject> tilemap_parent);
    ~AIGizmoToggleBehavior() override = default;    

    void on_update(float dt) override;
private: 
   std::reference_wrapper<GameObject> tilemap_parent_;
};