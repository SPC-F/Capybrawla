
#include <game/prefabs/interactables/weapon_bat.h>
#include <game/behaviors/interactable/weapon_bat.h>
#include <game/character/player_controller.h>

#include <engine/public/components/animator.h>
#include <engine/public/components/colliders/box_collider_2d.h>
#include <engine/public/components/network_identity.h>
#include <engine/public/components/rigidbody_2d.h>
#include <engine/public/components/sprite.h>
#include <engine/public/util/color.h>
#include <engine/public/util/layers.h>

WeaponBatPrefab::WeaponBatPrefab(Scene& scene, bool is_dropped, Vector3 position)
    : GameObject(scene)
{
    this->name("Weapon_Bat_Interactable_Prefab");
    this->transform().scale({1.5, 1.5, 1});
    this->transform().position(position);

    this->add_component<Sprite>("bat", Color(), 0, 0, 0, 0);
    
    if (is_dropped) {
        this->add_component<Rigidbody2D>(BodyType2D::Dynamic, 20.0f, true);
        this->add_component<NetworkIdentity>();
    }
    else {
        this->add_component<Rigidbody2D>(BodyType2D::Static, 0.0f, false, 0.0f);
    }

    this->add_component<BoxCollider2D>(0, 0, 24, 48, Point{0, 0}, !is_dropped, !is_dropped);
    this->add_component<BehaviorScript>(std::make_unique<WeaponBatInteractableBehavior>(is_dropped));

    this->layer(Layers::Foreground + 2);
}