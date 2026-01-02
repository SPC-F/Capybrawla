#include <game/prefabs/interactables/weapon_sword.h>
#include <game/behaviors/interactable/weapon_sword.h>
#include <game/character/player_controller.h>

#include <engine/public/components/animator.h>
#include <engine/public/components/colliders/box_collider_2d.h>
#include <engine/public/components/network_identity.h>
#include <engine/public/components/rigidbody_2d.h>
#include <engine/public/components/sprite.h>
#include <engine/public/util/color.h>
#include <engine/public/util/layers.h>

WeaponSwordPrefab::WeaponSwordPrefab(Scene& scene)
    : GameObject(scene)
{
    this->name("Weapon_Sword_Interactable_Prefab");

    this->transform().scale({1, 1, 1});
    this->add_component<Sprite>("sword", Color(), 0, 0, 0, 0);
    this->add_component<Rigidbody2D>(BodyType2D::Static, 0.0f, false, 0.0f);
    this->add_component<BoxCollider2D>(0, 0, 32, 32, Point{0, 0}, true);
    this->add_component<BehaviorScript>(std::make_unique<WeaponSwordInteractableBehavior>());

    layer(Layers::Foreground + 2);
}