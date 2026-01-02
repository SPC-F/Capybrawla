
#include <game/prefabs/interactables/health_pack.h>

#include "engine/physics/world/body/body_type_2d.h"
#include "engine/public/components/animator.h"
#include "engine/public/components/rigidbody_2d.h"
#include "engine/public/components/sprite.h"
#include "engine/public/components/colliders/box_collider_2d.h"
#include "engine/public/util/color.h"
#include "engine/public/util/layers.h"
#include "game/character/player_controller.h"

HealthPackPrefab::HealthPackPrefab(Scene& scene)
    : GameObject(scene)
{
    this->name("Health Pack Prefab");

    transform().scale({3, 3, 3});
    add_component<Sprite>("health_pack", Color(), 0, 0, 0, 0);
    add_component<Animator>("health_pack_idle", 128);
    add_component<Rigidbody2D>(BodyType2D::Static, 0.0f, false, 0.0f);

    auto& box_coll = add_component<BoxCollider2D>(0, 0, 32, 32, Point{32, 32}, true);
    box_coll.add_on_trigger_enter([this](Collider2D&, Collider2D& other) {
        for (auto& behavior_script : other.parent()->get().get_components<BehaviorScript>())
        {
            if (auto* behavior = dynamic_cast<PlayerController*>(&behavior_script.get().behavior()))
            {
                behavior->heal(health_amount_);
                mark_for_deletion();
            }
        }
    });
    layer(Layers::Foreground + 2);
}