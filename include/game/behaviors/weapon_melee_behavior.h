#pragma once

#include <engine/public/behavior.h>
#include <engine/public/components/sprite.h>
#include <engine/public/components/colliders/box_collider_2d.h>
#include <engine/public/util/layers.h>
#include <engine/public/util/point.h>

class WeaponMeleeBehavior : public Behavior {
public:
    WeaponMeleeBehavior(
        const std::string& attack_animation_name,
        const std::string& original_texture_name,
        int damage,
        int range,
        Point knockback_force,
        GameObject& hitbox_gameobject,
        GameObject& sprite_gameobject,
        Point hitbox_offset = Point{0, 0},
        Point sprite_offset_left = Point{0, 0},
        Point sprite_offset_right = Point{0, 0},
        Point animator_offset_left = Point{0, 0},
        Point animator_offset_right = Point{0, 0}
    );
    ~WeaponMeleeBehavior() override = default;    

    void on_awake() override;
    void on_update(float dt) override;
private: 
    std::string attack_animation_name_;
    std::string original_texture_name_;
    bool facing_right_{true};
    
    int damage_;
    int range_;
    int swingspeed_;
    Point knockback_force_;
    Point knockback_force_delta_;

    Point hitbox_offset_;
    Point sprite_offset_left_;
    Point sprite_offset_right_;
    Point animator_offset_left_;
    Point animator_offset_right_;

    std::reference_wrapper<GameObject> sprite_gameobject_;
    std::reference_wrapper<GameObject> hitbox_gameobject_;
    std::optional<std::reference_wrapper<Sprite>> sprite_component_;
    std::optional<std::reference_wrapper<BoxCollider2D>> hitbox_component_;
    std::optional<std::reference_wrapper<GameObject>> player_component_;

};