#include <game/behaviors/interactable/weapon_sword.h>

#include <game/prefabs/weapons/weapon_sword_player_object.h>
#include <game/prefabs/config/weapon_sword_config.h>
#include <game/character/player_weapon_controller.h>

#include <engine/public/components/colliders/box_collider_2d.h>
#include <engine/public/components/colliders/collider_2d.h>
#include <engine/public/components/behaviorscript.h>
#include <engine/public/gameObject.h>

void WeaponSwordInteractableBehavior::on_start() {
    auto collider_opt = get_component<BoxCollider2D>();
    if (!collider_opt.has_value()) {
        throw std::runtime_error("WeaponSwordInteractableBehavior requires a BoxCollider2D component.");
    }

    collider_opt->get().add_on_trigger_enter([](Collider2D& self, Collider2D& other) {
        auto other_parent_opt = other.parent();
        if (!other_parent_opt.has_value()) return;

        auto& other_gameobject = other_parent_opt->get();
        if (other_gameobject.tag() != "Player") return;

        auto controller_opt = other_gameobject.get_script<BehaviorScript, PlayerWeaponController>();
        if (!controller_opt.has_value()) return;
        
        auto& scene = self.parent()->get().scene();
        auto& weapon = scene.add_game_object<WeaponSwordPlayerObject>(scene, other_gameobject);
        controller_opt->get().switch_weapon(weapon);

        self.parent()->get().mark_for_deletion();
    });
}