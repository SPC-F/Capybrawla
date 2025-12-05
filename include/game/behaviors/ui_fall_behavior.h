#pragma once

#include <engine/public/behavior.h>
#include <engine/public/util/layers.h>

class UIFallBehavior : public Behavior {
public:
    UIFallBehavior(float rot_speed, float fall_speed, float lowest_point) : rotation_speed_(rot_speed), fall_speed_(fall_speed), lowest_point_(lowest_point) {}    

    void on_update(float dt) override {
        auto& transform = this->transform();
        
        Vector3 position = transform.position();
        position.y += fall_speed_ * dt;

        float new_rotation = transform.rotation() + rotation_speed_ * dt;
        transform.rotation(new_rotation);

        transform.position(position);

        if (position.y > lowest_point_ + 10.0f) {
            position.y = -100.0f;
            transform.position(position);
        }
    }
private: 
    float rotation_speed_{50.0f};
    float fall_speed_{100.0f};
    float lowest_point_{600.0f};
};