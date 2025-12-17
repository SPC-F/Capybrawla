#pragma once

#include <engine/public/behavior.h>
#include <engine/public/components/sprite.h>
#include <engine/public/components/colliders/box_collider_2d.h>
#include <engine/public/util/layers.h>

class WeaponMeleeBehavior : public Behavior {
public:
    WeaponMeleeBehavior(int damage, int range, int swingspeed, GameObject& hitbox_gameobject);
    ~WeaponMeleeBehavior() override = default;    

    void on_awake() override;
    void on_update(float dt) override;
private: 
    int damage_;
    int range_;
    int swingspeed_;
    
    bool looking_right{true};
    float hitbox_timer_{0.0f};
    const float hitbox_duration_ = 0.5f;

    std::reference_wrapper<GameObject> hitbox_gameobject_;
    std::optional<std::reference_wrapper<Sprite>> sprite_component_;
    std::optional<std::reference_wrapper<BoxCollider2D>> hitbox_component_;
    std::optional<std::reference_wrapper<GameObject>> player_component_;

};