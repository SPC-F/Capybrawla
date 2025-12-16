#pragma once

#include <engine/public/behavior.h>
#include <engine/public/components/sprite.h>
#include <engine/public/components/rigidbody_2d.h>
#include <engine/public/util/layers.h>

class AIRenderingBehavior : public Behavior {
public:
    AIRenderingBehavior(std::reference_wrapper<GameObject> tilemap_parent);
    ~AIRenderingBehavior() override = default;    

    void on_update(float dt) override;
private: 
   std::reference_wrapper<GameObject> tilemap_parent_;
};