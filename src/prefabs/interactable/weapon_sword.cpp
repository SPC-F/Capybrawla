
#include <game/prefabs/interactables/weapon_sword.h>
#include <game/prefabs/config/weapon_sword_config.h>
#include <game/character/player_controller.h>
#include <game/character/player_weapon_controller.h>
#include <game/prefabs/weapons/weapon_sword_player_object.h>

#include <engine/core/engine.h>
#include <engine/physics/world/body/body_type_2d.h>
#include <engine/public/components/animator.h>
#include <engine/public/components/colliders/box_collider_2d.h>
#include <engine/public/components/network_identity.h>
#include <engine/public/components/rigidbody_2d.h>
#include <engine/public/components/sprite.h>
#include <engine/public/util/color.h>
#include <engine/public/util/layers.h>
#include <engine/public/prefab_service.h>

WeaponSwordPrefab::WeaponSwordPrefab(Scene& scene)
    : GameObject(scene)
{
    this->name("Weapon Sword Prefab");

    transform().scale({1, 1, 1});
    add_component<Sprite>("sword", Color(), 0, 0, 0, 0);
    add_component<Rigidbody2D>(BodyType2D::Static, 0.0f, false, 0.0f);

    auto& box_coll = add_component<BoxCollider2D>(0, 0, 32, 32, Point{0, 0}, true);
    box_coll.add_on_trigger_enter([this](Collider2D& self, Collider2D& other) {

        auto controller_opt = other.parent()->get().get_script<BehaviorScript, PlayerWeaponController>();
        if (!controller_opt.has_value()) return;
        
        auto& scene = self.parent()->get().scene();
        auto& weapon = scene.add_game_object<WeaponSwordPlayerObject>(scene, other.parent()->get());
        controller_opt->get().switch_weapon(weapon);

        mark_for_deletion();
    });

    layer(Layers::Foreground + 2);
}