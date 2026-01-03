
#include <game/prefabs/interactables/health_pack.h>
#include <game/behaviors/interactable/health_pack.h>
#include <game/character/player_controller.h>

#include <engine/public/components/animator.h>
#include <engine/public/components/colliders/box_collider_2d.h>
#include <engine/public/components/network_identity.h>
#include <engine/public/components/rigidbody_2d.h>
#include <engine/public/components/sprite.h>
#include <engine/public/util/color.h>
#include <engine/public/util/layers.h>

HealthPackPrefab::HealthPackPrefab(Scene& scene)
    : GameObject(scene)
{
    this->name("Health_Pack_Interactable_Prefab");
    this->transform().scale({3, 3, 3});

    this->add_component<Sprite>("health_pack", Color(), 0, 0, 0, 0);
    this->add_component<Animator>("health_pack_idle", 128).play(true);
    this->add_component<Rigidbody2D>(BodyType2D::Static, 0.0f, false, 0.0f);
    this->add_component<BoxCollider2D>(0, 0, 32, 32, Point{32, 32}, true);
    this->add_component<BehaviorScript>(std::make_unique<HealthPackInteractableBehavior>());

    this->layer(Layers::Foreground + 2);
}