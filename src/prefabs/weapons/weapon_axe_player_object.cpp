#include <game/prefabs/weapons/weapon_axe_player_object.h>

#include <game/behaviors/weapon_melee_behavior.h>

#include <engine/public/components/animator.h>
#include <engine/public/components/behaviorscript.h>
#include <engine/public/components/colliders/box_collider_2d.h>
#include <engine/public/components/rigidbody_2d.h>
#include <engine/public/components/sprite.h>
#include <engine/public/util/layers.h>

WeaponAxePlayerObject::WeaponAxePlayerObject(Scene& scene, GameObject& parent)
    : GameObject(scene) 
{
    this->name("Weapon_Axe_Player_Object");
    this->parent(parent);
    
    const std::string attack_idle_name = "axe";
    const std::string attack_animation_name = "axe_swing_anim";
    
    /// Seperate game object for hitbox
    auto& melee_weapon_hitbox = scene.add_game_object("Player_Melee_Weapon_Hitbox");
    melee_weapon_hitbox.parent(dynamic_cast<GameObject&>(*this));
    melee_weapon_hitbox.add_component<Rigidbody2D>(BodyType2D::Kinematic);
    melee_weapon_hitbox.add_component<BoxCollider2D>(0.5f, 0.1f, 32.0f, 60.0f, Point{0.0f, 0.0f}, true, false);

    /// Seperate game object for sprite
    auto& melee_weapon_sprite = scene.add_game_object("Player_Melee_Weapon_Sprite");
    melee_weapon_sprite.transform().scale({1.5f, 1.5f, 0.0f});
    melee_weapon_sprite.parent(dynamic_cast<GameObject&>(*this));
    melee_weapon_sprite.add_component<Sprite>(attack_idle_name, Color{255, 255, 255, 255}, 0, 0, 0, 0);
    melee_weapon_sprite.add_component<Animator>(attack_animation_name, 60);
    melee_weapon_sprite.layer(Layers::Foreground + 1);

    const int damage = 40;
    const int range = 20;
    const Point knockback_force = Point{24.0f, -1.5f};


    const Point hitbox_offset{60, 0};
    const Point sprite_offset_left{-5.0f, 0};
    const Point sprite_offset_right{25.0f, 0};
    const Point animator_offset_left{-25.0f, 0};
    const Point animator_offset_right{15.0f, 0};

    this->add_component<BehaviorScript>(std::make_unique<WeaponMeleeBehavior>(
        attack_animation_name,
        attack_idle_name,
        damage,
        range,
        knockback_force,
        melee_weapon_hitbox,
        melee_weapon_sprite,
        hitbox_offset,
        sprite_offset_left,
        sprite_offset_right,
        animator_offset_left,
        animator_offset_right
    ));
}