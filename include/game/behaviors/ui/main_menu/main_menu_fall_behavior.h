#pragma once

#include <engine/public/behavior.h>
#include <engine/public/util/layers.h>

class MainMenuFallBehavior : public Behavior {
public:
    MainMenuFallBehavior(float rot_speed, float fall_speed, float lowest_point);
    
    void on_update(float dt) override;
private: 
    float rotation_speed_{50.0f};
    float fall_speed_{100.0f};
    float lowest_point_{600.0f};
};